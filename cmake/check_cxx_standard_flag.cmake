################################################################################
# Copyright (c) 2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
#
################################################################################
include(CheckCXXCompilerFlag)

# Useful resources:
# [/std (Specify Language Standard Version)](https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=vs-2019)

function (check_cxx_standard_flag FLAG)
if (CMAKE_CXX_STANDARD)
    set(__cxx_standard_flag "-std=c++${CMAKE_CXX_STANDARD}")
    check_cxx_compiler_flag(${__cxx_standard_flag} __have_cxx_standard_flag)

    if (NOT __have_cxx_standard_flag)
        set(__cxx_standard_flag "/std:c++${CMAKE_CXX_STANDARD}")
        check_cxx_compiler_flag(${__cxx_standard_flag} __have_cxx_standard_flag)
    endif()

    if (__have_cxx_standard_flag)
        set(${FLAG} "${__cxx_standard_flag}" PARENT_SCOPE)
    else()
        message(WARNING "Unable to determine C++ standard specific flag")
    endif()
endif()
endfunction()
