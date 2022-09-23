#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lz4::lz4" for configuration "Release"
set_property(TARGET lz4::lz4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lz4::lz4 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/lz4.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/lz4.dll"
  )

list(APPEND _cmake_import_check_targets lz4::lz4 )
list(APPEND _cmake_import_check_files_for_lz4::lz4 "${_IMPORT_PREFIX}/lib/lz4.lib" "${_IMPORT_PREFIX}/bin/lz4.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
