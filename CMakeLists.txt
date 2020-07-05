################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)

message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX

include(${CMAKE_CURRENT_LIST_DIR}/cmake/cxx17_filesystem.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/cxx17_string_view.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/cxx17_utility_classes.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::common ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

set(__all_dependences_found ON)

if (TARGET std::filesystem)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::filesystem)
else()
    message(SEND_ERROR "std::filesystem target not found")
    set(__all_dependences_found OFF)
endif()

if (TARGET std::string_view)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::string_view)
else()
    message(SEND_ERROR "std::string_view target not found")
    set(__all_dependences_found OFF)
endif()

if (TARGET std::utility_classes)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::utility_classes)
else()
    message(SEND_ERROR "std::utility_classes target not found")
    set(__all_dependences_found OFF)
endif()

if (NOT __all_dependences_found)
    message(FATAL_ERROR "Some dependences not found")
endif()

if (${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
