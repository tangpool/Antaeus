find_path(LOG4CPP_INCLUDE_DIR log4cpp/Category.hh
    HINTS
    PATH_SUFFIXES include
    PATHS
    /usr/local
    /usr
)
 
find_library(LOG4CPP_LIBRARY NAMES log4cpp
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/local
  /usr
)
 
mark_as_advanced(LOG4CPP_LIBRARY LOG4CPP_INCLUDE_DIR)

