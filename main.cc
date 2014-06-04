//
//  main.cc
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//


//
// libconfig
//    http://www.hyperrealm.com/libconfig/
// gflags
//
// log4cpp, jsoncpp, libcurl
//

#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

#include <curl/curl.h>
#include <google/gflags.h>
#include <json/json.h>

#include "antaeus.h"
#include "bitcoin_rpc.h"
#include "conn_pool.h"
#include "log.h"
#include "util.h"

// global vars
GblState g_state;
log4cpp::Category *g_log;
DEFINE_string(conf_file, ANTAEUS_DEFAULT_CONF, "configure file path");


// static functions
static void parse_config();
static void init_log4cpp();
static void init_bitcoind();

static void create_thread(void *(*func)(void *), void *arg);

int main(int argc, char **argv)
{
  int mysql_pool_max_connections;
  
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  curl_global_init(CURL_GLOBAL_DEFAULT);
  
  parse_config();
  init_log4cpp();
  init_bitcoind();
  
  // init mysql connection pool
  g_state.log_->info("init mysql connection pool...");
  g_state.conn_pool_->init_conns(mysql_pool_max_connections);
  g_state.log_->info("init mysql connection pool done");
  
  // while (1)
  while (1) {
    g_state.log_->info("%d", time(NULL));
    sleep(1);
  }
  
  exit(EXIT_FAILURE);
}



static void init_log4cpp() {
  try{
    std::string log4cpp_conf;
    g_state.cfg_->lookupValue("log4cpp.conf_file", log4cpp_conf);
    
    log4cpp::PropertyConfigurator::configure(log4cpp_conf);
    g_state.log_ = &log4cpp::Category::getRoot();
    
  }catch(log4cpp::ConfigureFailure& f){
    std::cerr << f.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  
//  log4cpp::Category& root = log4cpp::Category::getRoot();
//  root.setPriority(log4cpp::Priority::INFO);
  
//  log4cpp::Category& sub1 = root.getInstance("sub1");
//  g_state.log_ = &root.getInstance("sub1");
//  g_state.log_ = &log4cpp::Category::getInstance("rootAppender");
//  g_state.log_->setPriority(log4cpp::Priority::INFO);
}


static void init_bitcoind() {
  const char *host_url, *userpwd;
  g_state.cfg_->lookupValue("bitcoind.host_url", host_url);
  g_state.cfg_->lookupValue("bitcoind.userpwd", userpwd);
  
  snprintf(g_state.bitcoind_host_url_, sizeof(g_state.bitcoind_host_url_),
           "%s", host_url);
  snprintf(g_state.bitcoind_userpwd_,  sizeof(g_state.bitcoind_userpwd_),
           "%s", userpwd);
  
  Json::Value rpc_res;
  Json::FastWriter j_writer;
  
  if (btcd_rpc_call(rpc_res, "getinfo") != 0) {
    g_state.log_->fatal("bitcoind getinfo failure");
    exit(EXIT_FAILURE);
  }

  if (rpc_res.isMember("errors") && !rpc_res["errors"].empty()) {
    g_state.log_->warn("bitcoind has errors: %s", rpc_res["errors"].asCString());
  }
}


static void parse_config() {
  // Read the file. If there is an error, report it and exit.
  try {
    g_state.cfg_->readFile(fLS::FLAGS_conf_file.c_str());
  }
  catch(const libconfig::FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file: " << fLS::FLAGS_conf_file.c_str() << std::endl;
    exit(EXIT_FAILURE);
  }
  catch(const libconfig::ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
    << " - " << pex.getError() << std::endl;
    exit(EXIT_FAILURE);
  }
}


