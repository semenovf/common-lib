################################################################################
# Copyright (c) 2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
#
# Checks:
#       * std::any
#       * std::optional
#       * std::variant
################################################################################
include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state()

check_include_file_cxx("any" __have_std_any_header)
check_include_file_cxx("optional" __have_std_optional_header)
check_include_file_cxx("variant" __have_std_variant_header)

add_library(std::utility_classes INTERFACE IMPORTED)

################################################################################
# Check std::any
################################################################################
if (__have_std_any_header)
    string(CONFIGURE [[
        #include <any>

        int main () {
            std::any a = 1;
            return 0;
        }
    ]] __check_cxx_code @ONLY)

    # Try to compile a simple program without any linker flags
    check_cxx_source_compiles("${_check_cxx_code}" __std_any_compiled)

    if (__std_any_compiled)
        set(__have_std_any TRUE)
    endif()
endif()

################################################################################
# Check std::optional
################################################################################
if (__have_std_optional_header)
    string(CONFIGURE [[
        #include <optional>

        int main () {
            std::optional<int> = {};
            return 0;
        }
    ]] __check_cxx_code @ONLY)

    # Try to compile a simple program without any linker flags
    check_cxx_source_compiles("${_check_cxx_code}" __std_optional_compiled)

    if (__std_optional_compiled)
        set(__have_std_optional TRUE)
    endif()
endif()

################################################################################
# Check std::variant
################################################################################
if (__have_std_variant_header)
    string(CONFIGURE [[
        #include <variant>

        int main () {
            std::variant<int, float> v;
            return 0;
        }
    ]] __check_cxx_code @ONLY)

    # Try to compile a simple program without any linker flags
    check_cxx_source_compiles("${_check_cxx_code}" __std_variant_compiled)

    if (NOT __std_variant_compiled)
        set(__have_std_variant TRUE)
    endif()
endif()

################################################################################
if (NOT __have_std_any)
    message(STATUS "C++ any: use internal (third party)")
    target_compile_definitions(std::utility_classes INTERFACE PFS_NO_STD_ANY)
else()
    message(STATUS "C++ any: use built in library")
endif()

if (NOT __have_std_optional)
    message(STATUS "C++ optional: use internal (third party)")
    target_compile_definitions(std::utility_classes INTERFACE PFS_NO_STD_OPTIONAL)
else()
    message(STATUS "C++ optional: use built in library")
endif()

if (NOT __have_std_variant)
    message(STATUS "C++ variant: use internal (third party)")
    target_compile_definitions(std::utility_classes INTERFACE PFS_NO_STD_VARIANT)
else()
    message(STATUS "C++ variant: use built in library")
endif()

cmake_pop_check_state()
