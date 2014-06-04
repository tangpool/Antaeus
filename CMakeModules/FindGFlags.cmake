# - Find GFLAGS (gflags.h, libgflags.a, libgflags.so, and libgflags.so.0)
# This module defines
#  GFLAGS_INCLUDE_DIR, directory containing headers
#  GFLAGS_LIBS, directory containing gflag libraries
#  GFLAGS_STATIC_LIB, path to libgflags.a
#  GFLAGS_FOUND, whether gflags has been found


find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h
    HINTS
    PATH_SUFFIXES include
    PATHS
    /usr/local
    /usr
)
 
find_library(GFLAGS_LIBRARY NAMES libgflags gflags
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/local
  /usr
)
 
mark_as_advanced(GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR)

