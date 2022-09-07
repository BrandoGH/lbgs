#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lua" for configuration "Debug"
set_property(TARGET lua APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lua PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/lua.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/lua.dll"
  )

list(APPEND _cmake_import_check_targets lua )
list(APPEND _cmake_import_check_files_for_lua "${_IMPORT_PREFIX}/debug/lib/lua.lib" "${_IMPORT_PREFIX}/debug/bin/lua.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
