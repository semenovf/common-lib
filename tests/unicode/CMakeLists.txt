project(pfs_unicode_TESTS)

set(TEST_NAMES utf8_decode) # utf8_encode utf8_iterator)

set(utf8_decode_SOURCES
    utf8/cyrillic.c
    utf8/gothic.c
    utf8/greek.c
    utf8/mideng.c
    utf8/midger.c
    utf8/multilang.c
    utf8/rune.c
    utf8/vietnamese.c)

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${name}.cpp ${${name}_SOURCES})
    else()
        add_executable(${name} ${name}.cpp)
    endif()

    add_test(NAME ${name} COMMAND ${name})
endforeach()

