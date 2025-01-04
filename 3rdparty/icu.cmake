################################################################################
# Copyright (c) 2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2024.09.01 Initial version.
################################################################################
#project(icu-ep)
set(PROJ_NAME icu-ep)

if (MSVC)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/icu-prebuilt/include/unicode/utf8.h")
        set(_build_variant_1 TRUE)
        set(_icu_root_dir "${CMAKE_CURRENT_LIST_DIR}/icu-prebuilt")
    elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/icu/icu4c/source/allinone/allinone.sln")
        set(_build_variant_2 TRUE)
        set(_prefix "${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}")
        set(_icu_root_dir "${_prefix}/icu/icu4c")
    else ()
        message(WARNING
            "ICU support is preferred for search facilities but ICU library or "
            "one of it's component not found. See README.md for instructions.")
        return()
    endif()

    if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(_icu_lib_subdir "bin64")
        set(_icu_implib_subdir "lib64")
    elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(_icu_lib_subdir "bin")
        set(_icu_implib_subdir "lib")
    else()
        message(FATAL_ERROR "Unknown or unsupported target paltform: ${CMAKE_GENERATOR_PLATFORM}")
    endif()

    set(_icu_uc_lib_path "${_icu_root_dir}/${_icu_lib_subdir}/icuuc71.dll")
    set(_icu_uc_implib_path "${_icu_root_dir}/${_icu_implib_subdir}/icuuc.lib")
    set(_icu_data_lib_path "${_icu_root_dir}/${_icu_lib_subdir}/icudt71.dll")
    set(_icu_data_implib_path "${_icu_root_dir}/${_icu_implib_subdir}/icudt.lib")

    set(_icu_inc_dir "${_icu_root_dir}/include")

    if (_build_variant_1)
        if (NOT EXISTS ${_icu_inc_dir})
            message(FATAL_ERROR "ICU include directory not found: ${_icu_inc_dir}")
        endif()

        if (NOT EXISTS ${_icu_uc_lib_path})
            message(FATAL_ERROR "ICU UC library not found: ${_icu_uc_lib_path}")
        endif()
    endif()

    if (_build_variant_2)
        string(FIND ${_icu_root_dir} " " _space_pos)

        # NOTE: There is a problem to build ICU when path to library sources contains spaces.
        if (_space_pos GREATER_EQUAL 0)
            message(FATAL_ERROR "ICU source directory name contains spaces.")
        endif()

        include(ExternalProject)

        configure_file(${CMAKE_CURRENT_LIST_DIR}/icu.cmd.in ${_icu_root_dir}/build.cmd @ONLY)

        ExternalProject_Add(${PROJ_NAME} 
            PREFIX ${_prefix}
            DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E echo "Copy ICU sources"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/icu/icu4c/source" "${_icu_root_dir}/source"
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Build ICU"
               COMMAND ${CMAKE_COMMAND} -E chdir "${_icu_root_dir}" build.cmd
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS
                ${_icu_uc_lib_path}
                ${_icu_uc_implib_path}
                ${_icu_data_lib_path}
                ${_icu_data_implib_path})
    endif()

    add_library(ICU::uc SHARED IMPORTED GLOBAL)
    set_target_properties(ICU::uc PROPERTIES
            IMPORTED_LOCATION "${_icu_uc_lib_path}"
            INTERFACE_INCLUDE_DIRECTORIES "${_icu_inc_dir}"
            IMPORTED_IMPLIB "${_icu_uc_implib_path}")

    add_library(ICU::data SHARED IMPORTED GLOBAL)
    set_target_properties(ICU::data PROPERTIES
            IMPORTED_LOCATION "${_icu_data_lib_path}"
            IMPORTED_IMPLIB "${_icu_data_implib_path}")
else (MSVC)
    set(_prefix "${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}")
    get_property(_icu_lib_dir GLOBAL PROPERTY LIBRARY_OUTPUT_DIRECTORY)

    if (NOT _icu_lib_dir)
        set(_icu_lib_dir ${CMAKE_BINARY_DIR}/output)
    endif()

    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(_icu_platform "Linux/gcc")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(_icu_platform "Linux/Clang")
        else()
            set(_icu_platform "Linux")
        endif()
    else()
        message(FATAL_ERROR "Add instructions to support this platform: ${CMAKE_SYSTEM_NAME}/${CMAKE_CXX_COMPILER_ID}")
    endif()

    set(_icu_uc_lib_path "${_icu_lib_dir}/libicuuc.so")
    set(_icu_data_lib_path "${_icu_lib_dir}/libicudata.so")
    set(_icu_inc_dir "${_icu_lib_dir}/include")

    # --with-data-packaging     specify how to package ICU data. Possible values:
    #     files    raw files (.res, etc)
    #     archive  build a single icudtXX.dat file
    #     library  shared library (.dll/.so/etc.)
    #     static   static library (.a/.lib/etc.)
    #     auto     build shared if possible (default)
    #        See https://unicode-org.github.io/icu/userguide/icudata for more info.
    include(ExternalProject)
    ExternalProject_Add(${PROJ_NAME}
        PREFIX ${_prefix}
        GIT_REPOSITORY "https://github.com/unicode-org/icu"
        GIT_TAG "release-71-1"
        GIT_SHALLOW ON
        GIT_PROGRESS ON
        PATCH_COMMAND ""
        CONFIGURE_COMMAND COMMAND ${CMAKE_COMMAND} -E chdir "../${PROJ_NAME}/icu4c/source"
            ./runConfigureICU ${_icu_platform}
                "--prefix=${_prefix}"
                "--libdir=${_icu_lib_dir}"
                "--includedir=${_icu_lib_dir}/include"
                --with-data-packaging=library
                --enable-extras=no
                --enable-tools=yes
                --enable-fuzzer=no
                --enable-tests=no
                --enable-samples=no
                --enable-draft=no
                --enable-shared=yes
                --enable-static=no
                --enable-release=yes
                --enable-debug=no
        BUILD_COMMAND COMMAND ${CMAKE_COMMAND} -E chdir "../${PROJ_NAME}/icu4c/source" make
        INSTALL_COMMAND COMMAND ${CMAKE_COMMAND} -E chdir "../${PROJ_NAME}/icu4c/source" make install
        BUILD_BYPRODUCTS
            ${_icu_uc_lib_path}
            ${_icu_data_lib_path})
    add_dependencies(common ${PROJ_NAME})

    add_library(ICU::uc SHARED IMPORTED GLOBAL)
    set_target_properties(ICU::uc PROPERTIES
        IMPORTED_LOCATION "${_icu_uc_lib_path}"
        INTERFACE_INCLUDE_DIRECTORIES "${_icu_inc_dir}")

    add_library(ICU::data SHARED IMPORTED GLOBAL)
    set_target_properties(ICU::data PROPERTIES
        IMPORTED_LOCATION "${_icu_data_lib_path}")
endif(MSVC)
