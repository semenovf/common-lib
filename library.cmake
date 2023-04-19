################################################################################
# Copyright (c) 2019-2021 Vladislav Trifochkin
#
# This file is part of `common-lib`
################################################################################
cmake_minimum_required (VERSION 3.5)
project(common CXX C)

option(PFS__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)
option(PFS__ENABLE_NLS "Enable Native Language Support" OFF)
option(PFS__ENABLE_ICU "Enable `icu` library" ON)

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

portable_target(ADD_INTERFACE ${PROJECT_NAME} ALIAS pfs::common)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE Threads::Threads)

if (PFS__ENABLE_ICU)
    # https://cmake.org/cmake/help/v3.20/module/FindICU.html
    # Available components: data i18n io le lx test tu uc
    find_package(ICU COMPONENTS uc data)

    if (ICU_UC_FOUND AND ICU_DATA_FOUND)
        _portable_target_status(${PROJECT_NAME} "ICU library version: ${ICU_VERSION}")
        portable_target(LINK ${PROJECT_NAME} INTERFACE ICU::uc ICU::data)
        portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__ICU_ENABLED=1")
    else()
        _portable_target_warn(${PROJECT_NAME}
            "ICU support disabled: ICU library or one of it's component not found")
    endif()
endif(PFS__ENABLE_ICU)

if (UNIX)
    portable_target(LINK ${PROJECT_NAME} INTERFACE dl)
endif()

if (MSVC)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE -D_UNICODE -DUNICODE)
endif()

if (PFS__FORCE_ULID_STRUCT)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__FORCE_ULID_STRUCT=1")
endif()

if (PFS__LOG_LEVEL)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__LOG_LEVEL=${PFS__LOG_LEVEL}")
endif()

if (PFS__ENABLE_NLS)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__ENABLE_NLS=1")
endif()

if (ANDROID)
    portable_target(LINK ${PROJECT_NAME} INTERFACE log)
endif()

if (PFS__ENABLE_NLS)
    if (MSVC OR ANDROID)
        if (NOT TARGET libintl)
            portable_target_error(${PROJECT_NAME} "Expected `libintl` TARGET to support NLS" )
        endif()

        if (NOT TARGET libiconv)
            portable_target_error(${PROJECT_NAME} "Expected `libiconv` TARGET to support NLS")
        endif()

        portable_target(LINK ${PROJECT_NAME} INTERFACE libintl libiconv)
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
