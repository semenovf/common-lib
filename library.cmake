################################################################################
# Copyright (c) 2019-2024 Vladislav Trifochkin
#
# This file is part of `common-lib`
#
# Changelog:
#       2019.12.16 Initial commit.
#       2024.10.26 Removed `portable_target` dependency.
#       2024.12.10 Min CMake version is 3.19 (CMakePresets).
#       2025.03.19 Removed PFS__LOG_LEVEL.
################################################################################
cmake_minimum_required (VERSION 3.19)
project(common
    VERSION 1.0.0
    DESCRIPTION "Common C++ library"
    LANGUAGES CXX C)

if (PFS__ENABLE_NLS)
    if (MSVC)
        set(_use_imported_gettext_lib ON)
    else()
        set(_use_imported_gettext_lib OFF)
    endif()
else()
    set(_use_imported_gettext_lib OFF)
endif()

option(PFS__USE_IMPORTED_GETTEXT_LIB "Enable external gettext library" ${_use_imported_gettext_lib})

find_package(Threads REQUIRED)

add_library(common INTERFACE)
add_library(pfs::common ALIAS common)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(common INTERFACE Threads::Threads)

# On Windows, Clang is not self-sufficient, and is supposed to be used in combination 
# with an other compiler: either MinGW (GCC) or MSVC
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"
        OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC"))
    target_compile_definitions(common INTERFACE _UNICODE UNICODE)
    target_compile_definitions(common INTERFACE NOMINMAX)
    target_compile_options(common INTERFACE "/utf-8" "/Zc:__cplusplus")
endif()

if (PFS__ENABLE_ICU)
    # https://cmake.org/cmake/help/v3.20/module/FindICU.html
    # Available components: data i18n io le lx test tu uc
    find_package(ICU QUIET COMPONENTS uc data)

    if (ICU_UC_FOUND AND ICU_DATA_FOUND)
        message(STATUS "ICU library version: ${ICU_VERSION}")
        target_link_libraries(common INTERFACE ICU::uc ICU::data)
        target_compile_definitions(common INTERFACE "PFS__ICU_ENABLED=1")
    else()
        include(${CMAKE_CURRENT_LIST_DIR}/icu.cmake)

        if (TARGET ICU::uc AND TARGET ICU::data)
            target_link_libraries(common INTERFACE ICU::uc ICU::data)
            target_compile_definitions(common INTERFACE "PFS__ICU_ENABLED=1")
        endif()
    endif()
endif(PFS__ENABLE_ICU)

if (UNIX)
    target_link_libraries(common INTERFACE dl)
endif()

if (PFS__UNIVERSAL_ID_IMPL_UUIDV7)
    target_compile_definitions(common INTERFACE "PFS__UNIVERSAL_ID_IMPL_UUIDV7=1")
endif()

if (PFS__FORCE_ULID_STRUCT)
    target_compile_definitions(common INTERFACE "PFS__FORCE_ULID_STRUCT=1")
endif()

if (PFS__ENABLE_NLS)
    # [DEPRECATED]
    target_compile_definitions(common INTERFACE "PFS__ENABLE_NLS=1")

    target_compile_definitions(common INTERFACE "PFS__NLS_ENABLED=1")
endif()

if (ANDROID)
    target_link_libraries(common INTERFACE log)
endif()

if (PFS__ENABLE_NLS)
    if (MSVC OR ANDROID)
        if (NOT TARGET libintl)
            message(FATAL_ERROR "Expected `libintl` TARGET to support NLS" )
        endif()

        if (NOT TARGET libiconv)
            message(FATAL_ERROR "Expected `libiconv` TARGET to support NLS")
        endif()

        target_link_libraries(common INTERFACE libintl libiconv)
    endif()
endif()

# Some targets need to direct access to `fmt` library.
if (NOT TARGET fmt::fmt)
    add_library(fmt INTERFACE)
    add_library(fmt::fmt ALIAS fmt)
    target_include_directories(fmt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/pfs/3rdparty")
endif()

if (NOT TARGET fmt::fmt-header-only)
    add_library(fmt-header-only INTERFACE)
    add_library(fmt::fmt-header-only ALIAS fmt-header-only)
    target_include_directories(fmt-header-only INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/pfs/3rdparty")
    target_compile_definitions(fmt-header-only INTERFACE "FMT_HEADER_ONLY=1")
endif()
