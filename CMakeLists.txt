################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(pfs_common_BUILD_TESTS "Build tests" OFF)

include(${CMAKE_SOURCE_DIR}/cmake/cxx17_filesystem.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cxx17_string_view.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cxx17_utility_classes.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::common ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if (TARGET std::filesystem)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::filesystem)
endif()

if (TARGET std::string_view)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::string_view)
endif()

if (TARGET std::utility_classes)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::utility_classes)
endif()

if (pfs_common_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
