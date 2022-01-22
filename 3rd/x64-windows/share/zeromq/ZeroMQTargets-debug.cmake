#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libzmq" for configuration "Debug"
set_property(TARGET libzmq APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libzmq PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libzmq-mt-gd-4_3_4.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "ws2_32;advapi32;rpcrt4;iphlpapi"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libzmq-mt-gd-4_3_4.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libzmq )
list(APPEND _IMPORT_CHECK_FILES_FOR_libzmq "${_IMPORT_PREFIX}/debug/lib/libzmq-mt-gd-4_3_4.lib" "${_IMPORT_PREFIX}/debug/bin/libzmq-mt-gd-4_3_4.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
