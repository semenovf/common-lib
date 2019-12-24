project(pfs_common_TESTS)
option(ENABLE_COVERAGE "Build tests with coverage support" OFF)

if (ENABLE_COVERAGE)
    message(STATUS "Coverage feature enabled")
    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(FATAL_ERROR "Coverage feature requires GCC")
    endif()

    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_C_FLAGS "-g -O0 --coverage")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/cxx_filesystem.cmake)

set(TEST_NAMES
    dynamic_library
    filesystem
    iterator_bidirectional
    iterator_forward
    iterator_input
    iterator_output
    iterator_random_access
    pointer_proxy_iterator
    safeformat
    variant)

foreach (name ${TEST_NAMES})
    add_executable(${name} ${name}.cpp)
    add_test(NAME ${name} COMMAND ${name})
endforeach()

if (TARGET std::filesystem)
    target_link_libraries(dynamic_library std::filesystem)
    target_link_libraries(filesystem std::filesystem)
endif()

if (UNIX)
    target_link_libraries(dynamic_library dl)
endif()

# Helper library for test `dynamic_library`
add_library(shared_object SHARED shared_object.c)

