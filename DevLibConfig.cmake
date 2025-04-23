# - Config file for the DevLib package
# It defines the following variables
#  DevLib_INCLUDE_DIR - include directories for DevLib
#  DevLib_LIBRARIES    - libraries to link against


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was DevLibConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../usr/local" ABSOLUTE)

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

# GCC 주요 버전 추출
execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GCC_VERSION MATCHES "^[0-9]+")
  string(REGEX MATCH "^[0-9]+" GCC_MAJOR_VERSION ${GCC_VERSION})
else()
  message(FATAL_ERROR "Invalid GCC version format: ${GCC_VERSION}")
endif()


# Compute paths
set_and_check(DevLib_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

# Our library dependencies 
set_and_check(DevLib_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)

set(DEVLIB )
IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") # for desktop
  set(DEVLIB ${DevLib_LIB_DIR}/libDevLib_gcc${GCC_VERSION}.a)
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64") # for aarch64
  set(DEVLIB ${DevLib_LIB_DIR}/libDevLib_aarch64_gcc${GCC_VERSION}.a)
ENDIF()

set(DevLib_LIBRARIES 
  ${DEVLIB}
  
  # ffmpeg
  avcodec
  avformat 
  avutil 
  swscale

  # system dependencies
  X11
  GL
  GLU
  pthread
  rt

  # crypto
  dl
)

