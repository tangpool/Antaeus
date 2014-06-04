//
//  util.h
//  Antaeus
//
//  @author PanZhibiao
//  @since 2014-06-04
//
//  Copyright (c) 2014 Tangpool Inc. All rights reserved.
//
#ifndef ANTAEUS_HASH_H
#define ANTAEUS_HASH_H

#include <ext/hash_map>
#include <ext/hash_set>

namespace __gnu_cxx
{
  template <> struct hash<std::string>
  {
    size_t operator()(const std::string& s) const
    {
      unsigned long __h = 0;
      for (unsigned i = 0; i < s.size(); ++i) {
        __h ^= (( __h << 5) + (__h >> 2) + s[i]);
      }
      return size_t(__h);
    }
  };
 
  template<> struct hash<long long> {
    size_t operator()(long long __x) const { return __x; }
  };
  template<> struct hash<const long long> {
    size_t operator()(const long long __x) const { return __x; }
  };
  template<> struct hash<unsigned long long> {
    size_t operator()(unsigned long long __x) const { return __x; }
  };
  template<> struct hash<const unsigned long long> {
    size_t operator()(const unsigned long long __x) const { return __x; }
  };
}
 
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;

#endif