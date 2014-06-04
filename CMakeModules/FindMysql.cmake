FIND_PATH(MYSQL_INCLUDE_DIR mysql.h                                                                                                             
    HINTS
    PATH_SUFFIXES mysql/include include include/mysql
    PATHS
    /usr/local
    /usr
)
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES mysqlclient_r mysqlclient
  HINTS
  PATH_SUFFIXES mysql/lib lib64 lib lib/mysql
  PATHS
  /usr/local
  /usr
)
 
MARK_AS_ADVANCED(MYSQL_INCLUDE_DIR MYSQL_LIBRARIE)
