cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(pfs_common_BUILD_TESTS "Build tests" OFF)

include(${CMAKE_SOURCE_DIR}/cmake/cxx_filesystem.cmake)

add_library(pfs-common INTERFACE)

if (TARGET std::filesystem)
    target_link_libraries(pfs-common INTERFACE std::filesystem)
endif()

if (pfs_common_BUILD_TESTS)
    enable_testing()
    include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
    add_subdirectory(tests)
endif()
