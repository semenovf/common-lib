################################################################################
# Copyright (c) 2019-2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#       2019.12.16 Initial version.
#       2024.12.10 Min CMake version is 3.19 (CMakePresets).
################################################################################
cmake_minimum_required (VERSION 3.19)
project(common-ALL CXX C)

option(PFS__BUILD_STRICT "Build with strict policies: C++ standard required, C++ extension is OFF etc" ON)
option(PFS__BUILD_TESTS "Build tests" OFF)
option(PFS__BUILD_DEMO "Build examples/demo" OFF)

option(PFS__FORCE_ULID_STRUCT "Enable ULID struct representation (UUID backend)" OFF)
option(PFS__ENABLE_NLS "Enable Native Language Support" OFF)
option(PFS__ENABLE_ICU "Enable `icu` library" ON)

if (DEBBY__BUILD_STRICT)
    if (NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 14)
    endif()

    set(CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX
endif()

get_property(_runtime_output_directory GLOBAL PROPERTY RUNTIME_OUTPUT_DIRECTORY)
get_property(_library_output_directory GLOBAL PROPERTY LIBRARY_OUTPUT_DIRECTORY)
get_property(_archive_output_directory GLOBAL PROPERTY ARCHIVE_OUTPUT_DIRECTORY)

if (NOT _runtime_output_directory)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
endif()

if (NOT _library_output_directory)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
endif()

if (NOT _archive_output_directory)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)

if (PFS__BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

if (PFS__BUILD_DEMO AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/demo)
    add_subdirectory(demo)
endif()

include(GNUInstallDirs)

install(TARGETS common
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
