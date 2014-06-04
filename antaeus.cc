//
//  deliverer.cc
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#include <stdlib.h>
#include <pthread.h>
#include <algorithm>
#include <queue>

#include "antaeus.h"
#include "conn_pool.h"

GblState::GblState() {
  log_ = NULL;
  cfg_ = new libconfig::Config();
  
  conn_pool_  = new MysqlConnsPool();
  
  memset(bitcoind_host_url_, 0, sizeof(bitcoind_host_url_));
  memset(bitcoind_userpwd_,  0, sizeof(bitcoind_userpwd_));
}


GblState::~GblState() {
//  log4cpp::Category::shutdown();
  delete cfg_;
  delete conn_pool_;
}
