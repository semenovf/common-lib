################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [common-lib](https://github.com/semenovf/common-lib) library.
################################################################################
project(common-TESTS CXX C)

# Copy test files to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unicode/utf8/data DESTINATION unicode/utf8)

list(APPEND TEST_NAMES
    any
    argvapi
    binary_stream
    byteswap
    calendar
    charconv
    compare_op
    crc16_32_64
    dynamic_library
    emitter
    endian
    expected
    filesystem
    find
    function_queue
    getenv
    home_directory_path
    i18n
    integer
    iterator_bidirectional
    iterator_forward
    iterator_input
    iterator_output
    iterator_random_access
    levenshtein_distance
    locale_utils
    numeric_cast
    optional
    pointer_proxy_iterator
    rational
    real
    resources
    ring_buffer
    ring_buffer_mt
    sha256
    source_location
    split
    string_view
    synchronized
    time_point
    timer_pool
    type_traits
    variant
    unordered_erase
    utf8_iterator
    utf8_decode
    utf8_encode
    utf16le_decode
##     utf16le_encode
    utf16be_decode
##     utf16be_encode
    uuid
    uuidv7)

set(utf8_resource_SOURCES
    unicode/utf8/cyrillic.c
    unicode/utf8/cyrillic1.c
    unicode/utf8/cyrillic2.c
    unicode/utf8/gothic.c
    unicode/utf8/greek.c
    unicode/utf8/mideng.c
    unicode/utf8/midger.c
    unicode/utf8/multilang.c
    unicode/utf8/rune.c
    unicode/utf8/vietnamese.c)

set(utf16le_resource_SOURCES
    unicode/utf16le/cyrillic.c
    unicode/utf16le/cyrillic1.c
    unicode/utf16le/cyrillic2.c
    unicode/utf16le/gothic.c
    unicode/utf16le/greek.c
    unicode/utf16le/mideng.c
    unicode/utf16le/midger.c
    unicode/utf16le/multilang.c
    unicode/utf16le/rune.c
    unicode/utf16le/vietnamese.c)

set(utf16be_resource_SOURCES
    unicode/utf16be/cyrillic.c
    unicode/utf16be/cyrillic1.c
    unicode/utf16be/cyrillic2.c
    unicode/utf16be/gothic.c
    unicode/utf16be/greek.c
    unicode/utf16be/mideng.c
    unicode/utf16be/midger.c
    unicode/utf16be/multilang.c
    unicode/utf16be/rune.c
    unicode/utf16be/vietnamese.c)

set(utf8_decode_SOURCES ${utf8_resource_SOURCES})
set(utf8_encode_SOURCES ${utf8_resource_SOURCES})

set(utf16le_decode_SOURCES ${utf16le_resource_SOURCES})
set(utf16be_decode_SOURCES ${utf16be_resource_SOURCES})

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${${name}_SOURCES} ${name}.cpp)
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options
    #
    # --coverage
    #       This option is used to compile and link code instrumented for
    #       coverage analysis. The option is a synonym for -fprofile-arcs
    #       -ftest-coverage (when compiling) and -lgcov (when linking).
    if (PFS__ENABLE_COVERAGE)
        target_compile_options(${name} PRIVATE -g -O0 --coverage)
        target_link_libraries(${name} PRIVATE -g -O0 --coverage)
    endif()

    target_link_libraries(${name} PRIVATE pfs::common)
    add_test(NAME ${name} COMMAND ${name})
endforeach()

# Helper library for test `dynamic_library`
add_library(shared_object SHARED shared_object.c)

if (TARGET endian)
    if (MSVC)
        target_link_libraries(endian PRIVATE Ws2_32)
    endif()
endif()

if (TARGET i18n)
    # Target `libintl` available if `PFS__ENABLE_NLS` is ON
    # and `PFS__USE_IMPORTED_GETTEXT_LIB` is ON (on Windows platform)
    if (TARGET libintl)
        target_link_libraries(i18n PRIVATE libintl)

        add_custom_command(TARGET i18n
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_PROPERTY:libintl,IMPORTED_LOCATION>"
                "$<TARGET_FILE_DIR:i18n>"
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_PROPERTY:libintl,ICONV_LIB>"
                "$<TARGET_FILE_DIR:i18n>"
            VERBATIM)
    endif()
endif()

if (TARGET utf8_decode)
    # Copy test data files
    add_custom_command(TARGET utf8_decode
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf8_decode>/unicode"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf8_decode>/unicode/utf8"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf8_decode>/unicode/utf8/data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/unicode/utf8/data"
            "$<TARGET_FILE_DIR:utf8_decode>/unicode/utf8/data"
        VERBATIM)
endif()

if (TARGET utf16le_decode)
    add_custom_command(TARGET utf16le_decode
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16le_decode>/unicode"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16le_decode>/unicode/utf16le"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16le_decode>/unicode/utf16le/data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/unicode/utf16le/data"
            "$<TARGET_FILE_DIR:utf16le_decode>/unicode/utf16le/data"
        VERBATIM)
endif()

if (TARGET utf16be_decode)
    add_custom_command(TARGET utf16be_decode
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16be_decode>/unicode"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16be_decode>/unicode/utf16be"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:utf16be_decode>/unicode/utf16be/data"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/unicode/utf16be/data"
            "$<TARGET_FILE_DIR:utf16be_decode>/unicode/utf16be/data"
        VERBATIM)
endif()

if (TARGET uuidv7)
    find_package(Qt5 COMPONENTS Core)

    if (Qt5Core_FOUND)
        target_link_libraries(uuidv7 PRIVATE Qt5::Core)
        target_compile_definitions(uuidv7 PRIVATE "QT5_CORE_ENABLED=1")
    endif()
endif()

if (TARGET resources)
    add_library(rc1 STATIC resources/rc1.cpp)
    add_library(rc2 SHARED resources/rc2.cpp)
    target_link_libraries(rc1 PRIVATE pfs::common)
    target_link_libraries(rc2 PRIVATE pfs::common)
    target_link_libraries(resources PRIVATE rc1 rc2)
endif()

if (TARGET ICU::uc AND TARGET ICU::data)
    add_custom_command(TARGET utf8_iterator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_PROPERTY:ICU::uc,IMPORTED_LOCATION>"
                "$<TARGET_FILE_DIR:utf8_iterator>"
            COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_PROPERTY:ICU::data,IMPORTED_LOCATION>"
                "$<TARGET_FILE_DIR:utf8_iterator>")
endif()

if (MSVC)
    add_executable(msvc_cplusplus msvc_cplusplus.cpp)
    add_executable(msvc_cplusplus_set msvc_cplusplus_set.cpp)
    add_executable(msvc_cplusplus_std msvc_cplusplus_std.cpp)

    if (TARGET msvc_cplusplus_set)
        target_compile_options(msvc_cplusplus_set PRIVATE "/Zc:__cplusplus")
    endif()

    if (TARGET msvc_cplusplus_std)
        # NOTE: MSVC does not support /std:c++11 option.
        # So, set_property(TARGET msvc_cplusplus_std PROPERTY CXX_STANDARD 11) sets __cplusplus to 199711L.
        target_compile_options(msvc_cplusplus_std PRIVATE "/Zc:__cplusplus" "/std:c++14")
    endif()
endif()
