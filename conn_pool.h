//
//  conn_pool.h
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#ifndef ANTAEUS_CONN_POOL_H
#define ANTAEUS_CONN_POOL_H

#include <pthread.h>
#include <vector>

#include <mysql/mysql.h>

class GblState;


struct MysqlConn {
  int idx;
  MYSQL *db;
};


void tp_mysql_connect(MYSQL *db);
void tp_mysql_query(MYSQL *db, const char *sql);


class MysqlConnsPool {
  pthread_mutex_t lock_;
  pthread_cond_t  cond_;
  
  int conn_num_;
  int off_set_;
  int idle_conn_num_;
  
  std::vector<struct MysqlConn> conns_;
  std::vector<int> usage_;
  
public:
  MysqlConnsPool();
  ~MysqlConnsPool();
  void init_conns(int conn_num);
  
  void borrow_conn(struct MysqlConn *conn);
  void return_conn(struct MysqlConn *conn);
};


#endif