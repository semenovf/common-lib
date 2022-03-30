################################################################################
# Copyright (c) 2019-2021 Vladislav Trifochkin
#
# This file is part of `common-lib`
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(PFS__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)
option(PFS__ENABLE_EXCEPTIONS "Enable exceptions for library" OFF)

find_package(Threads REQUIRED)

portable_target(LIBRARY ${PROJECT_NAME} INTERFACE ALIAS pfs::common)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE Threads::Threads)

if (UNIX)
    portable_target(LINK ${PROJECT_NAME} INTERFACE dl)
endif()

if (MSVC)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE -D_UNICODE -DUNICODE)
endif()

if (PFS__ENABLE_EXCEPTIONS)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__EXCEPTIONS_ENABLED=1")
endif()

if (PFS__FORCE_ULID_STRUCT)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__FORCE_ULID_STRUCT=1")
endif()

if (PFS__LOG_LEVEL)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__LOG_LEVEL=${PFS__LOG_LEVEL}")
endif()

if (ANDROID)
    portable_target(LINK ${PROJECT_NAME} INTERFACE log)
endif()

