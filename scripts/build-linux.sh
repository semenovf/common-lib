#!/bin/bash
################################################################################
# Copyright (c) 2021-2023 Vladislav Trifochkin
#
# Unified build script for Linux distributions
#
# Changelog:
#      2021.05.20 Initial version.
#      2021.11.07 Added PROJECT_OPT_PREFIX variable.
#      2021.11.08 SOURCE_DIR recognition modified.
#      2021.11.19 Added support for profiling.
#      2022.07.06 Added CTEST_OPTIONS.
#      2022.07.26 Fixed conditions for run tests and coverage tests.
#      2023.04.11 Added PROJECT_NAME variable.
#      2023.09.20 Changed default values:
#                 * BUILD_TESTS by default is ON
#                 * BUILD_DEMO by default is ON
#                 * PROJECT_NAME by default is the name of the project folder.
#                 * PROJECT_OPT_PREFIX default value is in the PREFIX file.
#                 The script is now unified again (has no any explicit project
#                 dependecies inside).
#      2023.11.16 Extended source directory (SOURCE_DIR) recognition.
################################################################################

CMAKE_OPTIONS="${CMAKE_OPTIONS}"
CTEST_OPTIONS="${CTEST_OPTIONS}"

SCRIPT_ABS_PATH="$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"
PROJECT_FOLDER="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_FOLDER_NAME="$(basename "$PROJECT_FOLDER")"
echo "** Script absolute path: $SCRIPT_ABS_PATH"
echo "** Project folder: $PROJECT_FOLDER"
echo "** Project folder name: $PROJECT_FOLDER_NAME"

if [ ! -d "$PROJECT_FOLDER" ] ; then
    echo "ERROR: Project folder not found: $PROJECT_FOLDER." >&2
    exit 1
fi

if [ -z "$PROJECT_NAME" ] ; then
    PROJECT_NAME=$PROJECT_FOLDER_NAME
fi

if [ -z "$PROJECT_OPT_PREFIX" ] ; then
    if [ ! -f "$PROJECT_FOLDER/PREFIX" ] ; then
        echo "ERROR: PREFIX file must be in the project folder." >&2
        exit 1
    fi

    PROJECT_OPT_PREFIX=$(<"$PROJECT_FOLDER/PREFIX")
    echo "** Option prefix name: $PROJECT_OPT_PREFIX"
fi

if [ -z "$BUILD_GENERATOR" ] ; then
    if command -v ninja > /dev/null ; then
        BUILD_GENERATOR=Ninja
    else
        echo "WARN: Preferable build system 'ninja' not found, use default." >&2
        BUILD_GENERATOR="Unix Makefiles"
    fi
fi

if [ -n $BUILD_STRICT ] ; then
    case $BUILD_STRICT in
        [Oo][Ff][Ff])
            BUILD_STRICT=OFF
            ;;
        *)
            BUILD_STRICT=ON
            ;;
    esac
fi

if [ -n "$BUILD_STRICT" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -D${PROJECT_OPT_PREFIX}BUILD_STRICT=$BUILD_STRICT"
fi

if [ -n "$CXX_STANDARD" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_CXX_STANDARD=$CXX_STANDARD"
fi

if [ -n "$C_COMPILER" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=$C_COMPILER"
fi

if [ -n "$CXX_COMPILER" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_CXX_COMPILER=$CXX_COMPILER"
fi

if [ -z "$BUILD_TYPE" ] ; then
    BUILD_TYPE=Debug
fi

CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ -z "$BUILD_TESTS" ] ; then
    BUILD_TESTS=ON
fi

if [ -n $BUILD_TESTS ] ; then
    case $BUILD_TESTS in
        [Oo][Ff][Ff])
            BUILD_TESTS=OFF
            ;;
        *)
            BUILD_TESTS=ON
            ;;
    esac
fi

if [ -n "$BUILD_TESTS" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -D${PROJECT_OPT_PREFIX}BUILD_TESTS=$BUILD_TESTS"
fi

if [ -n $CTEST_VERBOSE ] ; then
    case $CTEST_VERBOSE in
        [Oo][Nn])
            CTEST_OPTIONS="--verbose ${CTEST_OPTIONS}"
            ;;
        *)
            ;;
    esac
fi

if [ -z "$BUILD_DEMO" ] ; then
    BUILD_DEMO=ON
fi

if [ -n $BUILD_DEMO ] ; then
    case $BUILD_DEMO in
        [Oo][Ff][Ff])
            BUILD_DEMO=OFF
            ;;
        *)
            BUILD_DEMO=ON
            ;;
    esac
fi

if [ -n "$BUILD_DEMO" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -D${PROJECT_OPT_PREFIX}BUILD_DEMO=$BUILD_DEMO"
fi

if [ -n $ENABLE_COVERAGE ] ; then
    case $ENABLE_COVERAGE in
        [Oo][Nn])
            ENABLE_COVERAGE=ON
            ;;
        *)
            unset ENABLE_COVERAGE
            ;;
    esac
fi

if [ -n "$ENABLE_COVERAGE" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -D${PROJECT_OPT_PREFIX}ENABLE_COVERAGE=$ENABLE_COVERAGE"
fi

if [ -n $ENABLE_PROFILER ] ; then
    case $ENABLE_PROFILER in
        [Oo][Nn])
            ENABLE_PROFILER=ON
            ;;
        *)
            unset ENABLE_PROFILER
            ;;
    esac
fi

if [ -n "$ENABLE_PROFILER" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -D${PROJECT_OPT_PREFIX}ENABLE_PROFILER=$ENABLE_PROFILER"
fi

if [ -z "$BUILD_DIR" ] ; then
    BUILD_DIR=builds/${PROJECT_NAME}-${CXX_COMPILER:-default}.cxx${CXX_STANDARD:-}${ENABLE_COVERAGE:+.coverage}${ENABLE_PROFILER:+.profiler}${BUILD_DIR_SUFFIX:+-}${BUILD_DIR_SUFFIX:-}
fi

# We are inside source directory
if [ -d .git -o -f LICENSE -o -f PREFIX -o -f CMakeLists.txt ] ; then
    if [ -z "$SOURCE_DIR" ] ; then
        SOURCE_DIR=`pwd`
    fi
    BUILD_DIR="../$BUILD_DIR"
fi

if [ -z "$SOURCE_DIR" ] ; then
    # We are inside subdirectory (usually from scripts directory)
    if [ -d ../.git -o -f ../LICENSE -o -f ../PREFIX -o -f ../CMakeLists.txt ] ; then
        SOURCE_DIR=`pwd`/..
        BUILD_DIR="../../$BUILD_DIR"
    else
        echo "ERROR: SOURCE_DIR must be specified" >&2
        exit 1
    fi
fi

mkdir -p ${BUILD_DIR} \
    && cd ${BUILD_DIR} \
    && cmake -G "${BUILD_GENERATOR}" $CMAKE_OPTIONS $SOURCE_DIR \
    && cmake --build .

if [ $? -eq 0 ] ; then
    if [ -n "$BUILD_TESTS" ] ; then
        ctest $CTEST_OPTIONS -C $BUILD_TYPE
    fi
fi

if [ $? -eq 0 ] ; then
    if [ -n "$ENABLE_COVERAGE" ] ; then
        cmake --build . --target Coverage
    fi
fi
