#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "date::date-tz" for configuration "Debug"
set_property(TARGET date::date-tz APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(date::date-tz PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/date-tz.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/date-tz.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS date::date-tz )
list(APPEND _IMPORT_CHECK_FILES_FOR_date::date-tz "${_IMPORT_PREFIX}/debug/lib/date-tz.lib" "${_IMPORT_PREFIX}/debug/bin/date-tz.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
