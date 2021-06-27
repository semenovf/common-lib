#!/bin/bash
################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# Unified build script for Linux distributions
#
# Changelog:
#      2021.05.20 Initial version
################################################################################

CMAKE_OPTIONS=

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
        [Oo][Nn])
            BUILD_STRICT=ON
            ;;
        *)
            unset BUILD_STRICT
            ;;
    esac
fi

if [ -n "$BUILD_STRICT" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_STRICT=$BUILD_STRICT"
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

if [ -n $BUILD_TESTS ] ; then
    case $BUILD_TESTS in
        [Oo][Nn])
            BUILD_TESTS=ON
            ;;
        *)
            unset BUILD_TESTS
            ;;
    esac
fi

if [ -n "$BUILD_TESTS" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_TESTS=$BUILD_TESTS"
fi

if [ -n $BUILD_DEMO ] ; then
    case $BUILD_DEMO in
        [Oo][Nn])
            BUILD_DEMO=ON
            ;;
        *)
            unset BUILD_DEMO
            ;;
    esac
fi

if [ -n "$BUILD_DEMO" ] ; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_DEMO=$BUILD_DEMO"
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
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_COVERAGE=$ENABLE_COVERAGE"
fi

BUILD_DIR=builds/${CXX_COMPILER:-default}.cxx${CXX_STANDARD:-}${ENABLE_COVERAGE:+.coverage}

# We are inside source directory
if [ -d .git ] ; then
    if [ -z "$ENABLE_COVERAGE" ] ; then
        SOURCE_DIR=`pwd`
    fi
    cd ..
fi

if [ -z "$SOURCE_DIR" ] ; then
    if [ -d src/.git ] ; then
        SOURCE_DIR=`pwd`/src
    else
        echo "ERROR: SOURCE_DIR must be specified" >&2
        exit 1
    fi
fi

mkdir -p ${BUILD_DIR} \
    && cd ${BUILD_DIR} \
    && cmake -G ${BUILD_GENERATOR} $CMAKE_OPTIONS $SOURCE_DIR \
    && cmake --build . \
    && [ -n "$BUILD_TESTS" ] && ctest \
    && [ -n "$ENABLE_COVERAGE" ] && cmake --build . --target Coverage
