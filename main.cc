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
#include <ev.h>
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

ev_timer  g_gbt_timeout_watcher;
struct ev_loop *g_loop = EV_DEFAULT;

DEFINE_string(conf_file, ANTAEUS_DEFAULT_CONF, "configure file path");

// static functions
static void parse_config();
static void init_log4cpp();
static void init_bitcoind();

static void create_thread(void *(*func)(void *), void *arg);
static void gbt_timeout_cb(EV_P_ ev_timer *w, int revents);



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
  
  //---------------- set gbt timer ----------------
  int gbt_intval_sec;
  g_state.cfg_->lookupValue("gbt_intval_sec", gbt_intval_sec);
  if (gbt_intval_sec < 1 || gbt_intval_sec > 30) {
    g_state.log_->fatal("gbt_intval_sec is out of range");
    exit(EXIT_FAILURE);
  }
  
  ev_init(&g_gbt_timeout_watcher, gbt_timeout_cb);
  g_gbt_timeout_watcher.repeat = gbt_intval_sec;
  ev_timer_again(g_loop, &g_gbt_timeout_watcher);

//  // while (1)
//  while (1) {
//    g_state.log_->info("%d", time(NULL));
//    sleep(1);
//  }
  
  ev_run(g_loop, 0);
  
  return 0;
}


static void gbt_timeout_cb(EV_P_ ev_timer *w, int revents) {
  g_state.log_->debug("gbt_timeout_cb, %d", time(NULL));
  
  // reset the timeout to start ticking again
  ev_timer_again(g_loop, &g_gbt_timeout_watcher);
  
  Json::Value rpc_res;
  Json::Value rpc_params, json_tmp;
  
  // {"id": 0, "method": "getblocktemplate",
  //  "params": [{"capabilities": ["coinbasetxn", "workid", "coinbase/append"]}]}
  json_tmp["mode"] = "template";
  json_tmp["capabilities"].append("workid");
  json_tmp["capabilities"].append("coinbasevalue");
  json_tmp["capabilities"].append("coinbasetxn");
  json_tmp["capabilities"].append("coinbase/append");
  rpc_params.append(json_tmp);
  
  if (btcd_rpc_call(rpc_res, "getblocktemplate", rpc_params) != 0) {
    g_state.log_->fatal("bitcoind getblocktemplate failure");
    exit(EXIT_FAILURE);
  }
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
//  Json::FastWriter j_writer;
  
  if (btcd_rpc_call(rpc_res, "getinfo") != 0) {
    g_state.log_->fatal("bitcoind getinfo failure");
    exit(EXIT_FAILURE);
  }

  if (rpc_res.isMember("errors") && rpc_res["errors"].asString().length() > 0) {
    g_state.log_->fatal("bitcoind has errors: %s", rpc_res["errors"].asCString());
    exit(EXIT_FAILURE);
  }
  
  // check block height
  int blk_min_height, is_testnet;
  g_state.cfg_->lookupValue("bitcoind.is_testnet", is_testnet);
  if (!is_testnet) {
    g_state.cfg_->lookupValue("bitcoind.block_min_height", blk_min_height);
    
    // @2014-06-04, block height 304173
    if (blk_min_height < 304173 || rpc_res["blocks"].asInt() <= blk_min_height) {
      g_state.log_->fatal("bitcoind's block height(%d) should more than %d",
                          rpc_res["blocks"].asInt(), blk_min_height);
      exit(EXIT_FAILURE);
    }
  }
  
  g_state.log_->info("bitcoind version: %d", rpc_res["version"].asInt());
  g_state.log_->info("bitcoind protocolversion: %d", rpc_res["protocolversion"].asInt());
  g_state.log_->info("bitcoind block height: %d", rpc_res["blocks"].asInt());
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


