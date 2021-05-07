################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common-ALL CXX C)

option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_COVERAGE "Build tests with coverage support" OFF)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX

include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
