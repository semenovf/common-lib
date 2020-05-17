################################################################################
# Check <filesystem>
#
# Inspired from https://github.com/vector-of-bool/CMakeCM/blob/master/modules/FindFilesystem.cmake
################################################################################
include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state()

# set(CMAKE_CXX_STANDARD 17)
check_include_file_cxx("filesystem" _HAVE_STD_FILESYSTEM_HEADER)

if (_HAVE_STD_FILESYSTEM_HEADER)
    add_library(std::filesystem INTERFACE IMPORTED)
    target_compile_definitions(std::filesystem INTERFACE HAVE_STD_FILESYSTEM)

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
                target_link_libraries(std::filesystem INTERFACE -lc++fs)
            endif() # NOT _STD_FILESYSTEM_REQUIRED
        else()
            target_link_libraries(std::filesystem INTERFACE -lstdc++fs)
        endif() # NOT _STD_FILESYSTEM_REQUIRED
    endif() # NOT _NO_STD_FILESYSTEM_LINK_REQUIRED
endif()

cmake_pop_check_state()
