find_path(LIBCURL_INCLUDE_DIR curl/curl.h
  PATHS "/usr/include"
      "/usr/local/include" )
 
find_library(LIBCURL_LIBRARY NAMES libcurl curl
  PATHS "/usr/lib"
      "/usr/local/lib" )

mark_as_advanced(
    LIBCURL_LIBRARY
    LIBCURL_INCLUDE_DIR
)
