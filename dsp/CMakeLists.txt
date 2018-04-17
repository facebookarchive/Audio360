## Copyright (c) 2018-present, Facebook, Inc.
## 
## This source code is licensed under the MIT license found in the
## LICENSE file in the root directory of this source tree.

##############################################################################
## Project Info
##############################################################################

cmake_minimum_required (VERSION 3.0)

set(MODULE_NAME dsp)
set(MODULE_TEST ${MODULE_NAME}-tests)
set(ROOT_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(DSP_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# Set the project name
project(${MODULE_NAME})
include (${ROOT_SRC_DIR}/cmake/utils.cmake)

##############################################################################
## Options
##############################################################################

setup_default_options()

##############################################################################
## Compiler Settings - Pre Dependencies
##############################################################################

set(MACOSX_RPATH ON)
config_default_compiler_settings()

##############################################################################
## Dependencies
##############################################################################


##############################################################################
## Include folders
##############################################################################

set(INCLUDE_FOLDERS 
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}
  ${ROOT_SRC_DIR}
  )

include_directories(${INCLUDE_FOLDERS})

##############################################################################
## Sources
##############################################################################

set(DSP_SRC
  ${DSP_SRC_DIR}/DSP.hh
  ${DSP_SRC_DIR}/AudioBufferList.hh
  ${DSP_SRC_DIR}/DSP_Neon.cpp
  ${DSP_SRC_DIR}/DSP_SSE.cpp
  ${DSP_SRC_DIR}/DSP_AVX.cpp
  ${DSP_SRC_DIR}/DSP_Common.cpp
  ${DSP_SRC_DIR}/CpuFeatures.hh
  ${DSP_SRC_DIR}/Internal.hh
  )

set(DSP_TESTS_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/tests/test_dsp.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/tests/test_AudioBufferList.cpp
)

# The AVX flags need to be set for all x86/x86_64 targets including:
# macOS, linux and iOS simulator
if((APPLE AND NOT (IOS AND NOT IOS_SIMULATOR)) OR (${ANDROID_ABI} MATCHES "x86") OR (${ANDROID_ABI} MATCHES "x86_64"))
  set_source_files_properties(${DSP_SRC_DIR}/DSP_AVX.cpp PROPERTIES COMPILE_FLAGS "-mavx")
elseif(MSVC)
  set_source_files_properties(${DSP_SRC_DIR}/DSP_AVX.cpp PROPERTIES COMPILE_FLAGS "/arch:AVX")
endif()

# GNU has a bug that requires an additional flag and we also must not set the flag if this is Android build
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
  # If this is Linux or Android build for x86 or x86_64
  if(NOT ANDROID OR (${ANDROID_ABI} MATCHES "x86") OR (${ANDROID_ABI} MATCHES "x86_64"))
    set_source_files_properties(${DSP_SRC_DIR}/DSP_AVX.cpp PROPERTIES COMPILE_FLAGS "-mavx -fabi-version=6")
  endif()
endif()

add_library(${MODULE_NAME}-object OBJECT ${DSP_SRC})
add_library(${MODULE_NAME} ${DSP_SRC})

if (GTEST_ENABLED)
  set(SRC_FILES ${DSP_TESTS_SRC})
  set(DEFS)
  set(LIBS ${MODULE_NAME})
  add_gtest_app(${MODULE_TEST} "${SRC_FILES}" "${DEFS}" "${LIBS}" "${ROOT_SRC_DIR}/cmake/")
endif()

##############################################################################
