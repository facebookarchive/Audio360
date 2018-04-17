## Copyright (c) 2018-present, Facebook, Inc.
## 
## This source code is licensed under the MIT license found in the
## LICENSE file in the root directory of this source tree.

##############################################################################

## Apply default compiler settings for all supported platforms
macro(config_default_compiler_settings)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  if (MSVC AND MSVC_STATIC_CRT AND NOT (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore"))
    # https://cmake.org/Wiki/CMake_FAQ#Dynamic_Replace
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach()
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3")
  if(MSVC)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
  elseif(APPLE)
    ## none for now
  elseif(ANDROID)
    ## none for now
  else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endmacro()

##############################################################################

## Adds a gtest executable target
## appName Name of the app/target
## sourceFiles List of source files 
## compileDefs List of compiler/preprocessor defs
## libsToLink List of libraries to link
## cmakeUtilsDir Path to the cmake utils directory
function(add_gtest_app appName sourceFiles compileDefs libsToLink cmakeUtilsDir)
  if(IOS)
    # programmatically build an iOS app. You'd still need to use Xcode for
    # provisioning/signing
    add_executable(${appName} MACOSX_BUNDLE
      ${sourceFiles}
      ${cmakeUtilsDir}/iOS/main.mm
    )
    target_include_directories(${appName} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    target_link_libraries(${appName}
      ${libsToLink}
      gtest
      "-framework CoreGraphics"
      "-framework Foundation"
      "-framework UIKit"
      "-framework AVFoundation"
      "-framework AudioToolbox"
    )

    set_target_properties(${appName} PROPERTIES
      XCODE_ATTRIBUTE_ARCHS $(ARCHS_STANDARD)
      MACOSX_BUNDLE_GUI_IDENTIFIER "com.test.${appName}"
      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_ADDONS_DIR}/iOS/Info.plist
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.test.${appName}"
      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
      XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES
      XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
      COMPILE_DEFINITIONS "${compileDefs}"
    )
  else()
    add_executable(${appName} ${sourceFiles})
    set_target_properties(${appName} PROPERTIES COMPILE_DEFINITIONS "${compileDefs}")
    target_link_libraries(${appName} ${libsToLink} ${GTEST_BOTH_LIBRARIES})
    target_include_directories(${appName} PRIVATE ${GTEST_INCLUDE_DIRS})
  endif()
endfunction(add_gtest_app)

##############################################################################

## Include the dsp library in a cmake project. 
macro(include_dsp repoRootPath)
  if(NOT TARGET dsp)
    add_subdirectory(${repoRootPath}/dsp/ ${CMAKE_CURRENT_BINARY_DIR}/dsp/)
  endif()
endmacro()

##############################################################################

## Exposes GTEST_SRC_DIR as a CMake option. GTEST_SRC_DIR should point to the gtest source directory
## that includes the gtest cmake file
macro(setup_gtest_option)
  option(GTEST_SRC_DIR "Path to the gtests src directory. Tests are skipped if not provided or if gtest isn't installed" 
      OFF)
  if (GTEST_SRC_DIR)
    message(STATUS "[audio360] GTEST_SRC_DIR is set to " ${GTEST_SRC_DIR})
    if(NOT TARGET gtest)
      add_subdirectory(${GTEST_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/gtest EXCLUDE_FROM_ALL)
    endif() 
    set(GTEST_BOTH_LIBRARIES gtest gtest_main)
    set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    set(GTEST_ENABLED TRUE)
  else()
    find_package(GTest)
    if (NOT GTEST_FOUND)
      message(STATUS "[audio360] Did not find Gtest and GTEST_SRC_DIR is not set, skipping tests!")  
    else()
      set(GTEST_ENABLED TRUE)
    endif()
  endif()
endmacro()

## Expose MT/MD option for MSVC
macro(setup_msvc_static_crt_option)
  option(MSVC_STATIC_CRT "Enable MT builds on Windows" ON)
endmacro()

## Setup default options
macro(setup_default_options)
  setup_msvc_static_crt_option()
  setup_gtest_option()
endmacro()
