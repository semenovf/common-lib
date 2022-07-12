################################################################################
# Copyright (c) 2019-2021 Vladislav Trifochkin
#
# This file is part of `common-lib`
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(PFS__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)
option(PFS__ENABLE_EXCEPTIONS "Enable exceptions for library" OFF)
option(PFS__ENABLE_NLS "Enable Native Language Support " ON)

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

if (PFS__ENABLE_NLS)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "PFS__ENABLE_NLS=1")
endif()

if (ANDROID)
    portable_target(LINK ${PROJECT_NAME} INTERFACE log)
endif()

if (PFS__ENABLE_NLS)
    if (PFS__USE_IMPORTED_GETTEXT_LIB)
        if (MSVC)
            add_library(libintl SHARED IMPORTED GLOBAL)

            set_target_properties(libintl PROPERTIES 
                IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/3rdparty/gettext-0.21/libintl.dll"
                IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/3rdparty/gettext-0.21/libintl.lib"
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/3rdparty/gettext-0.21"
                # Custom target for libintl dependency
                ICONV_LIB "${CMAKE_CURRENT_LIST_DIR}/3rdparty/gettext-0.21/libiconv-2.dll")
                
            portable_target(LINK ${PROJECT_NAME} INTERFACE libintl)

            # FIXME
            # Target "pfs-common" is an INTERFACE library that may not have PRE_BUILD,
            # PRE_LINK, or POST_BUILD commands.
            #
            # add_custom_command(TARGET ${PROJECT_NAME}
            #     POST_BUILD
            #     COMMAND ${CMAKE_COMMAND} -E copy 
            #         "$<TARGET_PROPERTY:libintl,IMPORTED_LOCATION>"
            #         "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            #     COMMAND ${CMAKE_COMMAND} -E copy 
            #         "$<TARGET_PROPERTY:libintl,ICONV_LIB>"
            #         "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            #     VERBATIM)
        endif (MSVC)
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
