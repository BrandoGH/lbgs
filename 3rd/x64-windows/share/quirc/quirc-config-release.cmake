#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "quirc::quirc" for configuration "Release"
set_property(TARGET quirc::quirc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(quirc::quirc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/quirc.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS quirc::quirc )
list(APPEND _IMPORT_CHECK_FILES_FOR_quirc::quirc "${_IMPORT_PREFIX}/lib/quirc.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
