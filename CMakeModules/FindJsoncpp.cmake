
find_path( JSONCPP_INCLUDE_DIR json/json.h
  PATHS "/usr/include"
      "/usr/local/include" )
 
find_library( JSONCPP_LIBRARY NAMES libjson json
  PATHS "/usr/lib"
      "/usr/local/lib" )
 
mark_as_advanced( JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR )

