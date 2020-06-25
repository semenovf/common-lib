################################################################################
# Copyright (c) 2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
#
# Inspired from https://github.com/vector-of-bool/CMakeCM/blob/master/modules/FindFilesystem.cmake
#
# Checks std::filesystem
################################################################################
include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(${CMAKE_CURRENT_LIST_DIR}/check_cxx_standard_flag.cmake)

cmake_push_check_state()

check_include_file_cxx("filesystem" __have_std_filesystem_header)

add_library(std::filesystem INTERFACE IMPORTED)

if (__have_std_filesystem_header)
    message(STATUS "C++ filesystem: use standard header")

    string(CONFIGURE [[
        #include <filesystem>

        int main() {
            auto cwd = std::filesystem::current_path();
            return static_cast<int>(cwd.string().size());
        }
    ]] __check_cxx_code @ONLY)

    check_cxx_standard_flag(__cxx_standard_flag)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${__cxx_standard_flag}")

    # Try to compile a simple filesystem program without any linker flags
    check_cxx_source_compiles("${__check_cxx_code}" __no_std_filesystem_link_required)

    if (NOT __no_std_filesystem_link_required)
        set(CMAKE_REQUIRED_LIBRARIES -lstdc++fs)
        check_cxx_source_compiles("${code}" __std_filesystem_required)

        if (NOT __std_filesystem_required)
            set(CMAKE_REQUIRED_LIBRARIES -lc++fs)
            check_cxx_source_compiles("${code}" __std_filesystem_required)

            if (NOT __std_filesystem_required)
                message(FATAL_ERROR "Unable to select std::filesystem's library")
            else()
                message(STATUS "C++ filesystem: use CLang library")
                target_link_libraries(std::filesystem INTERFACE -lc++fs)
            endif() # NOT __std_filesystem_required
        else()
            message(STATUS "C++ filesystem: use GNU library")
            target_link_libraries(std::filesystem INTERFACE -lstdc++fs)
        endif() # NOT __std_filesystem_required
    else()
        message(STATUS "C++ filesystem: use built in library")
    endif() # NOT __no_std_filesystem_link_required
else()
    target_compile_definitions(std::filesystem INTERFACE PFS_NO_STD_FILESYSTEM)
    message(STATUS "C++ filesystem: use internal (third party)")
endif()

cmake_pop_check_state()
