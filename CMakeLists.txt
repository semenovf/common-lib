################################################################################
# Copyright (c) 2019-2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#       2019.12.16 Initial version.
#       2024.12.10 Min CMake version is 3.19 (CMakePresets).
#       2025.11.18 Merged with library.cmake.
################################################################################
cmake_minimum_required (VERSION 3.19)
project(common
    VERSION 1.0.0
    DESCRIPTION "Common C++ library"
    LANGUAGES CXX C)

option(PFS__BUILD_STRICT "Build with strict policies: C++ standard required, C++ extension is OFF etc" ON)
option(PFS__BUILD_TESTS "Build tests" OFF)
option(PFS__BUILD_DEMO "Build examples/demo" OFF)

option(PFS__UNIVERSAL_ID_IMPL_UUIDV7 "Enable UUIDv7 implementation for UUID (default is ULID)" OFF)
option(PFS__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)
option(PFS__ENABLE_NLS "Enable Native Language Support" OFF)
option(PFS__ENABLE_ICU "Enable `icu` library" OFF)

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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (DEBBY__BUILD_STRICT)
    if (NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 14)
    endif()

    set(CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX
endif()

####################################################################################################

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
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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

####################################################################################################

if (PFS__BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

if (PFS__BUILD_DEMO AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/demo)
    add_subdirectory(demo)
endif()

include(GNUInstallDirs)

install(TARGETS common
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
