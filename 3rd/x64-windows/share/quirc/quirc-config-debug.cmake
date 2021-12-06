#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "quirc::quirc" for configuration "Debug"
set_property(TARGET quirc::quirc APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(quirc::quirc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/quirc.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS quirc::quirc )
list(APPEND _IMPORT_CHECK_FILES_FOR_quirc::quirc "${_IMPORT_PREFIX}/debug/lib/quirc.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
