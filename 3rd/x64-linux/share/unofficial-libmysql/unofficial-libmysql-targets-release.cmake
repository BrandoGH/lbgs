#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mysqlclient" for configuration "Release"
set_property(TARGET mysqlclient APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mysqlclient PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libmysqlclient.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS mysqlclient )
list(APPEND _IMPORT_CHECK_FILES_FOR_mysqlclient "${_IMPORT_PREFIX}/lib/libmysqlclient.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
