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

# Copy test files to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unicode/utf8/data DESTINATION unicode/utf8)

set(TEST_NAMES
    compare_op
    dynamic_library
    filesystem
    iterator_bidirectional
    iterator_forward
    iterator_input
    iterator_output
    iterator_random_access
    pointer_proxy_iterator
    rational
    safeformat
    variant
    utf8_decode
    utf8_encode)

set(utf8_resource_SOURCES
    unicode/utf8/cyrillic.c
    unicode/utf8/gothic.c
    unicode/utf8/greek.c
    unicode/utf8/mideng.c
    unicode/utf8/midger.c
    unicode/utf8/multilang.c
    unicode/utf8/rune.c
    unicode/utf8/vietnamese.c)

set(utf8_decode_SOURCES ${utf8_resource_SOURCES})
set(utf8_encode_SOURCES ${utf8_resource_SOURCES})

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${name}.cpp ${${name}_SOURCES})
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    target_link_libraries(${name} PRIVATE pfs::common)
    add_test(NAME ${name} COMMAND ${name})
endforeach()

if (UNIX)
    target_link_libraries(dynamic_library PRIVATE dl)
endif()

# Helper library for test `dynamic_library`
add_library(shared_object SHARED shared_object.c)
