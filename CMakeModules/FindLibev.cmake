find_path(LIBEV_INCLUDE_DIR ev.h
  PATHS "/usr/include"
      "/usr/local/include" )
 
find_library(LIBEV_LIBRARY NAMES libev ev
  PATHS "/usr/lib"
      "/usr/local/lib" )
 
mark_as_advanced(
    LIBEV_LIBRARY
    LIBEV_INCLUDE_DIR
)
