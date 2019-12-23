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

set(TEST_NAMES filesystem
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

