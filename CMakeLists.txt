cmake_minimum_required (VERSION 3.5)
project(pfs-common CXX C)

option(pfs_common_BUILD_TESTS "Build tests" OFF)

include(${CMAKE_SOURCE_DIR}/cmake/cxx_filesystem.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::common ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if (TARGET std::filesystem)
    target_link_libraries(${PROJECT_NAME} INTERFACE std::filesystem)
endif()

if (pfs_common_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
