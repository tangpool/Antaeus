//
//  antaeus.h
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#ifndef ANTAEUS_ANTAEUS_H
#define ANTAEUS_ANTAEUS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include <algorithm>
#include <queue>
#include <vector>

#include <libconfig.h++>
#include <mysql/mysql.h>

#include "hash.h"
#include "log.h"

#define BITCOIN_ADDRESS_MAX_SIZE 36
#define BITCOIND_RPC_TIMEOUT_SEC 3

#define ANTAEUS_DEFAULT_CONF "antaeus.conf"

class GblState;
class MysqlConnsPool;
struct RequestTask;


class GblState {
public:
  GblState();
  ~GblState();
  
  // bitcoind rpc
  char bitcoind_host_url_[1024];
  char bitcoind_userpwd_[512];
  
  // config
  libconfig::Config *cfg_;
  
  // log4cpp
  log4cpp::Category *log_;
  
//  __gnu_cxx::hash_map<std::string, std::vector<int64_t>> ds_address_;
  
  // db connection pool
  MysqlConnsPool *conn_pool_;
};

// global vars
extern GblState g_state;

#endif