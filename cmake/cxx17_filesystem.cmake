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

cmake_push_check_state()

check_include_file_cxx("filesystem" _HAVE_STD_FILESYSTEM_HEADER)

add_library(std::filesystem INTERFACE IMPORTED)

if (_HAVE_STD_FILESYSTEM_HEADER)
    message(STATUS "C++ filesystem: use standard header")

    string(CONFIGURE [[
        #include <filesystem>

        int main() {
            auto cwd = filesystem::current_path();
            return static_cast<int>(cwd.string().size());
        }
    ]] _check_cxx_code @ONLY)

    # Try to compile a simple filesystem program without any linker flags
    check_cxx_source_compiles("${_check_cxx_code}" _NO_STD_FILESYSTEM_LINK_REQUIRED)

    if (NOT _NO_STD_FILESYSTEM_LINK_REQUIRED)
        set(CMAKE_REQUIRED_LIBRARIES -lstdc++fs)
        check_cxx_source_compiles("${code}" _STD_FILESYSTEM_REQUIRED)

        if (NOT _STD_FILESYSTEM_REQUIRED)
            set(CMAKE_REQUIRED_LIBRARIES -lc++fs)
            check_cxx_source_compiles("${code}" _STD_FILESYSTEM_REQUIRED)

            if (NOT _STD_FILESYSTEM_REQUIRED)
                message(FATAL_ERROR "Unable to select std::filesystem's library")
            else()
                message(STATUS "C++ filesystem: use CLang library")
                target_link_libraries(std::filesystem INTERFACE -lc++fs)
            endif() # NOT _STD_FILESYSTEM_REQUIRED
        else()
            message(STATUS "C++ filesystem: use GNU library")
            target_link_libraries(std::filesystem INTERFACE -lstdc++fs)
        endif() # NOT _STD_FILESYSTEM_REQUIRED
    else()
        message(STATUS "C++ filesystem: use built in library")
    endif() # NOT _NO_STD_FILESYSTEM_LINK_REQUIRED
else()
    target_compile_definitions(std::filesystem INTERFACE PFS_NO_STD_FILESYSTEM)
    message(STATUS "C++ filesystem: use internal (third party)")
endif()

cmake_pop_check_state()
