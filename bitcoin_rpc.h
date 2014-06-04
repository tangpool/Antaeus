//
//  bitcoin_rpc.h
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#ifndef DW_DELIVERER_BITCOIN_RPC_H
#define DW_DELIVERER_BITCOIN_RPC_H

#include <vector>
#include <json/json.h>

#include "antaeus.h"

class GblState;

int
btcd_rpc_call(Json::Value &json_result, const char *method,
              const Json::Value &json_params=Json::Value());
#endif