# - Find libconfig
# Find the native libconfig includes and library
#
#  LIBCONFIG_INCLUDE_DIRS - where to find libconfig.h
#  LIBCONFIG_LIBRARIES    - List of libraries when using libconfig.
#  LIBCONFIG_FOUND        - True if libconfig found.


find_path( LIBCONFIG_INCLUDE_DIR libconfig.h++
	PATHS "/usr/include"
		  "/usr/local/include" )
		
find_library( LIBCONFIG_LIBRARY NAMES libconfig++ config++
	PATHS "/usr/lib"
		  "/usr/local/lib" )

mark_as_advanced( LIBCONFIG_LIBRARY LIBCONFIG_INCLUDE_DIR )
