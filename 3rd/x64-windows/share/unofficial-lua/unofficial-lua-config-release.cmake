#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lua" for configuration "Release"
set_property(TARGET lua APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lua PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/lua.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/lua.dll"
  )

list(APPEND _cmake_import_check_targets lua )
list(APPEND _cmake_import_check_files_for_lua "${_IMPORT_PREFIX}/lib/lua.lib" "${_IMPORT_PREFIX}/bin/lua.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
