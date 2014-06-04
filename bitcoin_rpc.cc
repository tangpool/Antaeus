//
//  bitcoin_rpc.cc
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#include "bitcoin_rpc.h"

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <openssl/md5.h>
#include <json/json.h>

#include "antaeus.h"
#include "util.h"


int
btcd_rpc_call(Json::Value &json_result, const char *method,
                 const Json::Value &json_params) {
  const size_t buf_size = 4*1024*1024;
  char *buf = (char *)malloc(buf_size);
  memset(buf, 0, buf_size);
  
  int res = -1;
  int return_len  = -1;
  int timeout_sec = BITCOIND_RPC_TIMEOUT_SEC;
  
  Json::Value  j_request;
  Json::Reader j_reader;
  Json::FastWriter j_writer;
  Json::Value json_result_raw;
  std::string res_str;
  
  srand((int)time(0));
  j_request["method"] = Json::Value(method);
  j_request["params"] = json_params;
  j_request["id"]     = rand();
  
  std::string json_str = j_writer.write(j_request);
  g_state.log_->debug("bitcoind rpc call start, method: %s, post: %s", method,
                      trim(json_str).c_str());
  
  // do http request
  return_len = curl_get_contents(g_state.bitcoind_host_url_, g_state.bitcoind_userpwd_,
                                 j_writer.write(j_request).c_str(), buf, buf_size,
                                 timeout_sec);
  if (return_len < 0) {
    g_state.log_->error("curl_get_contents failure, return length: %d", return_len);
    // network error, sleep & exit
    g_state.log_->fatal("network error");
    sleep(1);
    exit(EXIT_FAILURE);
  }
  
  res_str = std::string(buf);
  if (!j_reader.parse(trim(res_str), json_result_raw)) {
    g_state.log_->error("parse json failure, result: %s, length: %d",
                        trim(res_str).c_str(), return_len);
    res = -2;
    goto finish;
  }
  
  if (json_result_raw.isMember("error") && !json_result_raw["error"].empty()) {
    g_state.log_->error("btcd_rpc_call error, code: %d, message: %s, request params: %s",
                        json_result_raw["error"]["code"].asInt(),
                        json_result_raw["error"]["message"].asCString(),
                        j_writer.write(j_request).c_str());
    res = -3;
    goto finish;
  }
  
  res = 0;
  json_result = json_result_raw["result"];
  json_str = j_writer.write(json_result_raw);
  g_state.log_->debug("bitcoind rpc call end, return: %s", trim(json_str).c_str());
  
finish:
  free(buf);
  return res;
}

