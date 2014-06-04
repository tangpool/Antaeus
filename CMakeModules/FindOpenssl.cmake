
find_path( OPENSSL_INCLUDE_DIR openssl/md5.h
  PATHS "/usr/include"
      "/usr/local/include" )
 
find_library( OPENSSL_LIBRARY NAMES ssl
  PATHS "/usr/lib"
      "/usr/local/lib" )
 
find_library( CRYPTO_LIBRARY NAMES crypto 
  PATHS "/usr/lib"
      "/usr/local/lib" )
 
mark_as_advanced( OPENSSL_LIBRARY CRYPTO_LIBRARY OPENSSL_INCLUDE_DIR )


