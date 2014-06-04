//
//  util.h
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#ifndef DW_DELIVERER_UTIL_H
#define DW_DELIVERER_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>


class GblState;

struct CurlDataChunk {
  char  *memory;
  size_t size;
};


// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(),
          std::find_if(s.begin(), s.end(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
          s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}


int
curl_get_contents(const char *url, const char *userpwd,
                  const char *post_data, char *buf, size_t buf_size,
                  const int timeout_sec);

std::string time2str();
std::string time2str(time_t timestamp);
std::string time2str(const char *format, time_t timestamp);

time_t str2time(const char *timestr);
time_t str2time(const char *format, const char *timestr);


#endif