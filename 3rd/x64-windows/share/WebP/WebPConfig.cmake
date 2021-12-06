set(WebP_VERSION 1.1.0)
set(WEBP_VERSION ${WebP_VERSION})


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was WebPConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include ("${CMAKE_CURRENT_LIST_DIR}/WebPTargets.cmake")
include(${CMAKE_ROOT}/Modules/SelectLibraryConfigurations.cmake)

find_path(WEBP_INCLUDE_DIR NAMES webp/types.h)
set(WebP_INCLUDE_DIR ${WEBP_INCLUDE_DIR})
set(WebP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
set(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})

if(NOT WEBP_BASE_LIBRARY AND NOT WebP_BASE_LIBRARY)
  find_library(WEBP_BASE_LIBRARY_RELEASE NAMES webp)
  find_library(WEBP_BASE_LIBRARY_DEBUG NAMES webpd)
  select_library_configurations(WEBP_BASE)
  set(WebP_BASE_LIBRARY ${WEBP_BASE_LIBRARY})
  if(WEBP_BASE_LIBRARY)
    list(APPEND WEBP_LIBRARIES "${WEBP_BASE_LIBRARY}")
    list(APPEND WebP_LIBRARIES "${WebP_BASE_LIBRARY}")
  endif()
endif()

if(NOT WEBP_DECODER_LIBRARY AND NOT WebP_DECODER_LIBRARY)
  find_library(WEBP_DECODER_LIBRARY_RELEASE NAMES webpdecoder)
  find_library(WEBP_DECODER_LIBRARY_DEBUG NAMES webpdecoderd)
  select_library_configurations(WEBP_DECODER)
  set(WebP_DECODER_LIBRARY ${WEBP_DECODER_LIBRARY})
  if(WEBP_DECODER_LIBRARY)
    list(APPEND WEBP_LIBRARIES "${WEBP_DECODER_LIBRARY}")
    list(APPEND WebP_LIBRARIES "${WebP_DECODER_LIBRARY}")
  endif()
endif()

if(NOT WEBP_DEMUX_LIBRARY AND NOT WebP_DEMUX_LIBRARY)
  find_library(WEBP_DEMUX_LIBRARY_RELEASE NAMES webpdemux)
  find_library(WEBP_DEMUX_LIBRARY_DEBUG NAMES webpdemuxd)
  select_library_configurations(WEBP_DEMUX)
  set(WebP_DEMUX_LIBRARY ${WEBP_DEMUX_LIBRARY})
  if(WEBP_DEMUX_LIBRARY)
    list(APPEND WEBP_LIBRARIES "${WEBP_DEMUX_LIBRARY}")
    list(APPEND WebP_LIBRARIES "${WebP_DEMUX_LIBRARY}")
  endif()
endif()

if(NOT WEBP_MUX_LIBRARY AND NOT WebP_MUX_LIBRARY)
  find_library(WEBP_MUX_LIBRARY_RELEASE NAMES libwebpmux)
  find_library(WEBP_MUX_LIBRARY_DEBUG NAMES libwebpmuxd)
  select_library_configurations(WEBP_MUX)
  set(WebP_MUX_LIBRARY ${WEBP_MUX_LIBRARY})
  if(WEBP_MUX_LIBRARY)
    list(APPEND WEBP_LIBRARIES "${WEBP_MUX_LIBRARY}")
    list(APPEND WebP_LIBRARIES "${WebP_MUX_LIBRARY}")
  endif()
endif()

set(WEBP_LIBRARY ${WEBP_BASE_LIBRARY})
set(WebP_LIBRARY ${WebP_BASE_LIBRARY})
if(WEBP_BASE_LIBRARY AND WEBP_INCLUDE_DIR)
  set(WEBP_FOUND 1)
  set(WebP_FOUND 1)
endif()
