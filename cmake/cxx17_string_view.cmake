################################################################################
# Copyright (c) 2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
#
# Checks std::string_view
################################################################################
include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state()

check_include_file_cxx("string_view" __have_std_string_view_header)

add_library(std::string_view INTERFACE IMPORTED)

if (__have_std_string_view_header)
    string(CONFIGURE [[
        #include <string_view>

        int main () {
            char array[3] = {'a', 'b', 'c'};
            std::string_view array_v(array, std::size(array));
            return 0;
        }
    ]] __check_cxx_code @ONLY)

    # Try to compile a simple program without any linker flags
    check_cxx_source_compiles("${_check_cxx_code}" __std_string_view_compiled)

    if (__std_string_view_compiled)
        set(__have_std_string_view TRUE)
    endif()
endif()

if (NOT __have_std_string_view)
    target_compile_definitions(std::string_view INTERFACE PFS_NO_STD_STRING_VIEW)
endif()

cmake_pop_check_state()
