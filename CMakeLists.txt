cmake_minimum_required(VERSION 3.0)

# Check for C++11
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)
include(GNUInstallDirs)
include(FindPkgConfig)

project(pekwm-doc-tool)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH True)

option(PEDANTIC "turn on strict compile-time warnings" OFF)
option(TESTS "include tests" OFF)

pkg_check_modules(MD4C md4c REQUIRED)

if (PEDANTIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")
endif (PEDANTIC)

configure_file(${CMAKE_SOURCE_DIR}/CMake/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src)

if (TESTS)
   enable_testing()
   add_subdirectory(test)
endif (TESTS)
