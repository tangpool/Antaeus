//
//  util.cc
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#include "util.h"

#include <stdlib.h>
#include <time.h>

#include <curl/curl.h>

#include "antaeus.h"


static size_t
write_curl_data_chunk(void *contents, size_t size, size_t nmemb, void *userp);


//
// timestamp to string
// string to timestamp
//
std::string time2str(const char *format, time_t timestamp) {
  char buf[64] = {0};
  struct tm tm;
  localtime_r(&timestamp, &tm);
  strftime(buf, sizeof(buf), format, &tm);
  return std::string(buf);
}
time_t str2time(const char *format, const char *timestr) {
  struct tm tm;
  strptime(timestr, format, &tm);
  tm.tm_isdst = -1;
  return mktime(&tm);
}
std::string time2str() {
  return time2str("%Y-%m-%d %H:%M:%S", time(NULL));
}
std::string time2str(time_t timestamp) {
  return time2str("%Y-%m-%d %H:%M:%S", timestamp);
}
time_t str2time(const char *timestr) {
  return str2time("%Y-%m-%d %H:%M:%S", timestr);
}


int
curl_get_contents(const char *url, const char *userpwd,
                  const char *post_data, char *buf, size_t buf_size,
                  const int timeout_sec) {
  int res = -1;
  size_t res_len = 0;
  CURL *curl = NULL;
  struct curl_slist *headers = NULL;
  CURLcode curl_res;
  struct CurlDataChunk chunk;
  
  /* will be grown as needed by the realloc above */
  chunk.memory = (char *)malloc(8);
  chunk.size = 0;

  curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_sec);
  headers = curl_slist_append(headers, "Connection: close");
  headers = curl_slist_append(headers, "User-Agent: dw_deliverer/dearcoin.com");

  curl = curl_easy_init();
  if (!curl) {
    g_state.log_->error("curl_easy_init failure");
    exit(EXIT_FAILURE);
  }
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_curl_data_chunk);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  if (userpwd != NULL && strlen(userpwd)) {
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD,  userpwd);
  }
  if (post_data != NULL && strlen(post_data)) {
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
  }
  
  // perform
  curl_res = curl_easy_perform(curl);
  if(curl_res != CURLE_OK) {
    g_state.log_->error("curl_easy_perform() failed: %s", curl_easy_strerror(curl_res));
    goto finish;
  }
  // copy to buf
  if (chunk.size > 0) {
    assert(buf_size >= 1);
    res_len = chunk.size >= buf_size ? buf_size - 1 : chunk.size;
    if (res_len)
      memcpy(buf, chunk.memory, res_len);
    buf[res_len] = '\0';
  }
  res = (int)chunk.size;
  
finish:
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(chunk.memory);
  return res;
}


static size_t
write_curl_data_chunk(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct CurlDataChunk *mem = (struct CurlDataChunk *)userp;
  
  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {  // out of memory
    g_state.log_->fatal("not enough memory (realloc returned NULL)");
    exit(EXIT_FAILURE);
  }
  
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  
  return realsize;
}




