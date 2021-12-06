#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libffi" for configuration "Debug"
set_property(TARGET libffi APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libffi PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libffi.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libffi.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libffi )
list(APPEND _IMPORT_CHECK_FILES_FOR_libffi "${_IMPORT_PREFIX}/debug/lib/libffi.lib" "${_IMPORT_PREFIX}/debug/bin/libffi.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
