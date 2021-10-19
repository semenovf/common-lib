################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(common-lib-ALL CXX C)

option(PFS_COMMON_LIB__BUILD_STRICT "Build with strict policies: C++ standard required, C++ extension is OFF etc" ON)
option(PFS_COMMON_LIB__BUILD_TESTS "Build tests" OFF)
option(PFS_COMMON_LIB__ENABLE_COVERAGE "Build tests with coverage support" OFF)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

if (PFS_COMMON_LIB__BUILD_STRICT)
    set(CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX
endif()

include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)

if (PFS_COMMON_LIB__BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()
