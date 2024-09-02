################################################################################
# Copyright (c) 2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2024.09.01 Initial version.
################################################################################
project(icu-ep)

if (MSVC)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/icu-prebuilt/include/unicode/utf8.h")
        set(_build_variant_1 TRUE)
        set(_icu_root_dir "${CMAKE_CURRENT_LIST_DIR}/icu-prebuilt")
    elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/icu/icu4c/source/allinone/allinone.sln")
        set(_build_variant_2 TRUE)
        set(_prefix "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}")
        set(_icu_root_dir "${_prefix}/icu/icu4c")
    else ()
        _portable_target_fatal(${PROJECT_NAME}
            "ICU support required but ICU library or one of it's component not found. See README.md for instructions")
    endif()

    if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(_icu_lib_subdir "bin64")
        set(_icu_implib_subdir "lib64")
    elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(_icu_lib_subdir "bin")
        set(_icu_implib_subdir "lib")
    else()
        _portable_target_fatal(${PROJECT_NAME} "Unknown or unsupported target paltform: ${CMAKE_GENERATOR_PLATFORM}")
    endif()

    set(_icu_uc_lib_path "${_icu_root_dir}/${_icu_lib_subdir}/icuuc71.dll")
    set(_icu_uc_implib_path "${_icu_root_dir}/${_icu_implib_subdir}/icuuc.lib")
    set(_icu_data_lib_path "${_icu_root_dir}/${_icu_lib_subdir}/icudt71.dll")
    set(_icu_data_implib_path "${_icu_root_dir}/${_icu_implib_subdir}/icudt.lib")

    set(_icu_inc_dir "${_icu_root_dir}/include")

    if (_build_variant_1)
        if (NOT EXISTS ${_icu_inc_dir})
            _portable_target_fatal(${PROJECT_NAME} "ICU include directory not found: ${_icu_inc_dir}")
        endif()

        if (NOT EXISTS ${_icu_uc_lib_path})
            _portable_target_fatal(${PROJECT_NAME} "ICU UC library not found: ${_icu_uc_lib_path}")
        endif()
    endif()

    if (_build_variant_2)
        string(FIND ${_icu_root_dir} " " _space_pos)

        # NOTE: There is a problem to build ICU when path to library sources contains spaces.
        if (_space_pos GREATER_OR_EQUAL 0)
            _portable_target_fatal(${PROJECT_NAME} "ICU source directory name contains spaces.")
        endif()

        include(ExternalProject)

        configure_file(${CMAKE_CURRENT_LIST_DIR}/icu.cmd.in ${_icu_root_dir}/build.cmd @ONLY)

        ExternalProject_Add(${PROJECT_NAME} 
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
endif(MSVC)