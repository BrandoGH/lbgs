#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lz4::lz4" for configuration "Debug"
set_property(TARGET lz4::lz4 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lz4::lz4 PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/lz4d.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/lz4d.dll"
  )

list(APPEND _cmake_import_check_targets lz4::lz4 )
list(APPEND _cmake_import_check_files_for_lz4::lz4 "${_IMPORT_PREFIX}/debug/lib/lz4d.lib" "${_IMPORT_PREFIX}/debug/bin/lz4d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
