################################################################################
# Copyright (c) 2019-2021 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(PFS_COMMON__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::common ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE Threads::Threads)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} INTERFACE dl)
endif()

if (MSVC)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -D_UNICODE -DUNICODE)
endif()

if (PFS_COMMON__FORCE_ULID_STRUCT)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DPFS_COMMON__FORCE_ULID_STRUCT=1)
endif(PFS_COMMON__FORCE_ULID_STRUCT)
