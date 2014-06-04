//
//  conn_pool.cc
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#include "antaeus.h"
#include "conn_pool.h"


void tp_mysql_connect(MYSQL *db) {
  int port;
  const char *host = NULL, *username = NULL, *password = NULL;
  const char *database = NULL, *init_sql = NULL;
  
  g_state.cfg_->lookupValue("mysql.port", port);
  g_state.cfg_->lookupValue("mysql.host", host);
  g_state.cfg_->lookupValue("mysql.username", username);
  g_state.cfg_->lookupValue("mysql.password", password);
  g_state.cfg_->lookupValue("mysql.database", database);
  
  g_state.log_->debug("host: '%s', username:'%s', passwd: '%s', database: '%s', port: %d",
                      host, username, password, database, port);

  if (!mysql_real_connect(db, host, username, password, database,
                         (unsigned int)port, NULL, 0)) {
    g_state.log_->fatal("mysql_real_connect failure: %s", mysql_error(db));
    exit(EXIT_FAILURE);
  }

  // per query sql
  if (g_state.cfg_->lookupValue("mysql.init_sql", init_sql)) {
    tp_mysql_query(db, init_sql);
  }
}



void tp_mysql_query(MYSQL *db, const char *sql) {
  int res, sec;
  int try_count = 0;

query:
  g_state.log_->debug("dw_mysql_query sql: %s, len: %d", sql, strlen(sql));
  res = mysql_query(db, sql);
  
//  res = mysql_real_query(db, sql, strlen(sql));
  g_state.log_->debug("mysql_query res: %d", res);
  
  if (res == 0) { return; }  // success
  
  /* ---- 这里处理一下连接丢失的情况。通常由于数据库重启、网络中断等造成 ---- */
  // 错误：2013 (CR_SERVER_LOST)
  // 消息：查询过程中丢失了与MySQL服务器的连接。
  if (res == 2013) {
    // 重试30次
    while (try_count++ <= 30) {
      g_state.log_->fatal("mysql lost connection, try reconnect...");
      
      // mysql_ping: 检查与服务器的连接是否工作。如果连接丢失，将自动尝试再连接。
      if (mysql_ping(db) == 0) {  // reconnect ok
        g_state.log_->fatal("mysql reconnect success");
        goto query;
      }
      sec = 3;  // 每3秒自动尝试连接一次
      sleep(sec);
    }
  }
  
  // error
  g_state.log_->error("dw_mysql_query failure, error: %s, sql: %s",
                     mysql_error(db), sql);
  exit(EXIT_FAILURE);
}



MysqlConnsPool::MysqlConnsPool() {
  off_set_ = 0;
  pthread_mutex_init(&lock_, NULL);
  pthread_cond_init(&cond_, NULL);
}

MysqlConnsPool::~MysqlConnsPool() {
  for (int i = 0; i < conn_num_; ++i) {
    mysql_close(conns_[i].db);
  }
}

void MysqlConnsPool::init_conns(int conn_num) {
  idle_conn_num_ = conn_num_ = conn_num;
  usage_.resize(conn_num_);
  conns_.resize(conn_num_);
  
  for (int i = 0; i < conn_num_; ++i) {
    usage_[i] = 0;
    conns_[i].idx = i;
    conns_[i].db  = mysql_init(NULL);
    tp_mysql_connect(conns_[i].db);
  }
}


void MysqlConnsPool::borrow_conn(struct MysqlConn *conn) {
  memset(conn, 0, sizeof(struct MysqlConn));
  
  pthread_mutex_lock(&lock_);
  while (idle_conn_num_ <= 0) {
    pthread_cond_wait(&cond_, &lock_);
  }
  
  while (1) {
    off_set_++;
    if (off_set_ >= conn_num_) {
      off_set_ = 0;
    }
    if (usage_[off_set_] == 0) {  // find idle one
      usage_[off_set_] = 1;
      *conn = conns_[off_set_];
      idle_conn_num_--;
      assert(idle_conn_num_ >= 0);
      break;
    }
  }
  
  pthread_mutex_unlock(&lock_);
  g_state.log_->debug("borrow_conn index: %d", conn->idx);
}


void MysqlConnsPool::return_conn(struct MysqlConn *conn) {
  if (conn == NULL) { return; }
  
  pthread_mutex_lock(&lock_);
  assert(usage_[conn->idx] == 1);
  
  usage_[conn->idx] = 0;  // mark as idle
  idle_conn_num_++;
  
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&lock_);
  
  g_state.log_->debug("return_conn index: %d", conn->idx);
}
