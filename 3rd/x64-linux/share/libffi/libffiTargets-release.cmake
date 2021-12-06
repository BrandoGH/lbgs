#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libffi" for configuration "Release"
set_property(TARGET libffi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libffi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "ASM;C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libffi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libffi )
list(APPEND _IMPORT_CHECK_FILES_FOR_libffi "${_IMPORT_PREFIX}/lib/libffi.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
