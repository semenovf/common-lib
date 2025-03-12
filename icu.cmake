################################################################################
# Copyright (c) 2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2024.09.01 Initial version.
#      2025.01.04 Added build for Linux.
#      2025.01.05 Fixed build for Windows.
################################################################################
set(PROJ_NAME icu-ep)

#message(WARNING
#    "ICU support is preferred for search facilities but ICU library or "
#    "one of it's component not found. See README.md for instructions.")
#return()

set(_prefix "${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}")
get_property(_icu_lib_dir GLOBAL PROPERTY LIBRARY_OUTPUT_DIRECTORY)

if (NOT _icu_lib_dir)
    set(_icu_lib_dir ${CMAKE_BINARY_DIR}/output)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(_icu_uc_lib_path "${_icu_lib_dir}/icuuc71.dll")
        set(_icu_uc_implib_path "${_icu_lib_dir}/icuuc.lib")
        set(_icu_data_lib_path "${_icu_lib_dir}/icudt71.dll")
        set(_icu_data_implib_path "${_icu_lib_dir}/icudt.lib")
        set(_icu_inc_dir "${_icu_lib_dir}/include")

        # https://learn.microsoft.com/en-us/cpp/build/cmakesettings-reference?view=msvc-170#environments
        # Used non-"Visual Studio Generators" or arch is not set by -A option
        if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR $ENV{VSCMD_ARG_TGT_ARCH} STREQUAL "x64")
            set(PFS__ICU_ARCH x64)
            set(_icu_lib_subdir "bin64")
            set(_icu_implib_subdir "lib64")
        elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" OR $ENV{VSCMD_ARG_TGT_ARCH} STREQUAL "x86")
            set(PFS__ICU_ARCH Win32)
            set(_icu_lib_subdir "bin")
            set(_icu_implib_subdir "lib")
        else()
            message(FATAL_ERROR "Target platform not set: use -A option (-A x64 or -A Win32)")
        endif()

        configure_file(${CMAKE_CURRENT_LIST_DIR}/build-icu.cmd.in ${CMAKE_CURRENT_BINARY_DIR}/build-icu.cmd @ONLY)

        include(ExternalProject)
        ExternalProject_Add(${PROJ_NAME}
            PREFIX ${_prefix}
            GIT_REPOSITORY "https://github.com/unicode-org/icu"
            GIT_TAG "release-71-1"
            GIT_SHALLOW ON
            GIT_PROGRESS ON
            PATCH_COMMAND ""
            CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/build-icu.cmd "../${PROJ_NAME}/icu4c"
            BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Build ICU"
                COMMAND ${CMAKE_COMMAND} -E chdir "../${PROJ_NAME}/icu4c" build-icu.cmd
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_if_different "../${PROJ_NAME}/icu4c/${_icu_lib_subdir}/icuuc71.dll" ${_icu_lib_dir}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "../${PROJ_NAME}/icu4c/${_icu_lib_subdir}/icudt71.dll" ${_icu_lib_dir}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "../${PROJ_NAME}/icu4c/${_icu_implib_subdir}/icuuc.lib" ${_icu_lib_dir}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "../${PROJ_NAME}/icu4c/${_icu_implib_subdir}/icudt.lib" ${_icu_lib_dir}
                COMMAND ${CMAKE_COMMAND} -E copy_directory "../${PROJ_NAME}/icu4c/include" "${_icu_lib_dir}/include"
            BUILD_BYPRODUCTS
                ${_icu_uc_lib_path}
                ${_icu_data_lib_path}
                ${_icu_uc_implib_path}
                ${_icu_data_implib_path})    else()
        message(FATAL_ERROR "Add instructions to support this platform: ${CMAKE_SYSTEM_NAME}/${CMAKE_CXX_COMPILER_ID}")
    endif()
else ()
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(_icu_platform "Linux/gcc")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(_icu_platform "Linux/Clang")
        else()
            set(_icu_platform "Linux")
        endif()

        set(_icu_uc_lib_path "${_icu_lib_dir}/libicuuc.so")
        set(_icu_data_lib_path "${_icu_lib_dir}/libicudata.so")
        set(_icu_inc_dir "${_icu_lib_dir}/include")
    else()
        message(FATAL_ERROR "Add instructions to support this platform: ${CMAKE_SYSTEM_NAME}/${CMAKE_CXX_COMPILER_ID}")
    endif()

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
endif(MSVC)

add_dependencies(common ${PROJ_NAME})

add_library(ICU::uc SHARED IMPORTED GLOBAL)
set_target_properties(ICU::uc PROPERTIES
    IMPORTED_LOCATION "${_icu_uc_lib_path}"
    INTERFACE_INCLUDE_DIRECTORIES "${_icu_inc_dir}")

add_library(ICU::data SHARED IMPORTED GLOBAL)
set_target_properties(ICU::data PROPERTIES
    IMPORTED_LOCATION "${_icu_data_lib_path}")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(ICU::uc PROPERTIES IMPORTED_IMPLIB "${_icu_uc_implib_path}")
    set_target_properties(ICU::data PROPERTIES IMPORTED_IMPLIB "${_icu_data_implib_path}")
endif()
