# Helpfull resources:
# 1. https://github.com/TheLartians/ModernCppStarter
# 2. https://github.com/bsamseth/cpp-project
# 3. https://github.com/eddyxu/cpp-coveralls
# 4. https://github.com/bilke/cmake-modules
# 5. https://github.com/JoakimSoderberg/coveralls-cmake
# 6. https://github.com/okkez/coveralls-lcov

message(STATUS "Coverage feature enabled")

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
        message(FATAL_ERROR "Coverage feature requires GCC or Clang")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Code coverage results with an optimised (non-Debug) build may be misleading")
endif()

# find_program(GCOV_BIN gcov)       # mandatory program
find_program(LCOV_BIN lcov)       # mandatory program
find_program(GENHTML_BIN genhtml) # optional program
# find_program(GCOVR_BIN gcovr)     # optional program

# if (GCOV_BIN)
#     message(STATUS "`gcov` program found at: ${GCOV_BIN}")
# else()
#     message(FATAL_ERROR "`gcov` program not found (is mandatory)")
# endif()

if (LCOV_BIN)
    message(STATUS "`lcov` program found at: ${LCOV_BIN}")
else()
    message(FATAL_ERROR "`lcov` program not found (is mandatory)")
endif()

if (GENHTML_BIN)
    message(STATUS "`genhtml` program found at: ${GENHTML_BIN}")
else()
    message(STATUS "`genhtml` program not found (is optional)")
endif()

# if (GCOVR_BIN)
#     message(STATUS "`gcovr` program found at: ${GCOVR_BIN}")
# else()
#     message(STATUS "`gcovr` program not found (is optional)")
# endif()

################################################################################
# Inspired from https://github.com/bsamseth/cpp-project/blob/master/cmake/CodeCoverage.cmake
function (coverage_target EXCLUDE_PATTERNS)
    set(COVERAGE_TARGET Coverage)
    set(COVERAGE_FILE ${COVERAGE_TARGET}.info)
    set(COVERAGE_DIR ${COVERAGE_TARGET}.dir)

    set(LCOV_SILENT '-q')

    if (NOT GENHTML_BIN)
        set(GENHTML_BIN "true")
    endif()

    # Setup target
    add_custom_target(${COVERAGE_TARGET}
        # Cleanup lcov
        ${LCOV_BIN} --directory . --zerocounters

        # Run tests
        COMMAND ctest

        # Capturing lcov counters and generating report
        COMMAND ${LCOV_BIN} --directory . --capture --output-file ${COVERAGE_FILE} ${LCOV_SILENT}
        COMMAND ${LCOV_BIN} --remove ${COVERAGE_FILE} ${EXCLUDE_PATTERNS} --output-file ${COVERAGE_FILE} ${LCOV_SILENT}
        COMMAND ${GENHTML_BIN} -o ${COVERAGE_DIR} ${COVERAGE_FILE}
        COMMAND ${LCOV_BIN} --list ${COVERAGE_FILE}

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Processing code coverage counters and generating report.")

    # Show info where to find the report
    add_custom_command(TARGET ${COVERAGE_TARGET} POST_BUILD
        COMMAND echo "-- Generated coverage report at: ${CMAKE_BINARY_DIR}/${COVERAGE_DIR}/index.html")
endfunction()
