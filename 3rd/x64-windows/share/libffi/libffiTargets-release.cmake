#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libffi" for configuration "Release"
set_property(TARGET libffi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libffi PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libffi.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libffi.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libffi )
list(APPEND _IMPORT_CHECK_FILES_FOR_libffi "${_IMPORT_PREFIX}/lib/libffi.lib" "${_IMPORT_PREFIX}/bin/libffi.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
