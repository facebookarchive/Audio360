## Copyright (c) 2018-present, Facebook, Inc.
## 
## This source code is licensed under the MIT license found in the
## LICENSE file in the root directory of this source tree.

##############################################################################
## Project Info
##############################################################################

cmake_minimum_required (VERSION 3.0)

set(MODULE_NAME FBAudioRenderer)
set(MODULE_TEST ${MODULE_NAME}-tests)
set(ROOT_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(RENDERER_SRC_DIR ${ROOT_SRC_DIR}/renderer/src)

project(${MODULE_NAME})
include (${ROOT_SRC_DIR}/cmake/utils.cmake)

##############################################################################
## Options
##############################################################################

setup_default_options()

##############################################################################
## Settings - Pre Dependencies
##############################################################################

set(MACOSX_RPATH ON)
config_default_compiler_settings()

##############################################################################
## Dependencies
##############################################################################

include_dsp(${ROOT_SRC_DIR})

##############################################################################
## Sources
##############################################################################

set(RENDERER_SRC
  ${RENDERER_SRC_DIR}/AmbiBinauralCoefficients.hh
  ${RENDERER_SRC_DIR}/AmbiBinauralCoefficients.cpp
  ${RENDERER_SRC_DIR}/AmbiDefinitions.hh
  ${RENDERER_SRC_DIR}/AmbiSphericalConvolution.hh
  ${RENDERER_SRC_DIR}/AmbiSphericalConvolution.cpp
  )

set(RENDERER_TESTS_SRC
  ${RENDERER_SRC_DIR}/tests/test_AmbiSphericalConvolution.cpp
)

set(INCLUDE_DIR 
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}
  ${ROOT_SRC_DIR}
  )

##############################################################################
## Targets
##############################################################################

add_library(${MODULE_NAME}-object OBJECT ${RENDERER_SRC})
target_include_directories(${MODULE_NAME}-object PRIVATE ${INCLUDE_DIR})

add_library(${MODULE_NAME} ${RENDERER_SRC})
target_include_directories(${MODULE_NAME} PRIVATE ${INCLUDE_DIR})


set(SRC_FILES ${RENDERER_TESTS_SRC})
set(DEFS)
set(LIBS dsp ${MODULE_NAME})

if (GTEST_ENABLED)
  add_gtest_app(${MODULE_TEST} "${SRC_FILES}" "${DEFS}" "${LIBS}" "${ROOT_SRC_DIR}/cmake/")
  target_include_directories(${MODULE_TEST} PRIVATE ${INCLUDE_DIR})
endif()

##############################################################################
