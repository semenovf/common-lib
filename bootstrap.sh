#!/bin/sh

BASE_URL=https://raw.githubusercontent.com/semenovf/scripts/master
PORTABLE_TARGET_BASE_URL=https://raw.githubusercontent.com/semenovf/portable-target/master/cmake/v2
BAK_SUFFIX=`date +%Y_%m_%d_%H_%M_%S`

SCRIPTS="build.sh \
    build-windows.cmd"

CONFIGS=".gitignore \
    .travis.yml \
    appveyor.yml \
    codecov.yml"

LAST_STATUS=0
LAST_COMAMND=

curl_copy()
{
    SOURCE=$1
    TARGET=$2
    FILENAME=`basename $TARGET`

    if [ -e $TARGET ] ; then
        echo "Target already exists: $TARGET, new one will be renamed into: $TARGET.$BAK_SUFFIX"
        TARGET="$TARGET.$BAK_SUFFIX"
    fi

    LAST_COMMAND="curl -s $SOURCE -o $TARGET"
    echo -n "Coping $FILENAME ... "

    $LAST_COMMAND > /dev/null
    LAST_STATUS=$?

    if [ $LAST_STATUS -eq 0 ] ; then
        echo "OK"
    else
        echo "Failure on command"
        echo "\t$LAST_COMMAND"
    fi
}

for f in $SCRIPTS ; do
    curl_copy $BASE_URL/scripts/${f} $f
    [ $LAST_STATUS -ne 0 ] && exit 1
done

for f in $CONFIGS ; do
    curl_copy $BASE_URL/configs/${f} $f
    [ $LAST_STATUS -ne 0 ] && exit 1
done

######################################################
# portable-target
######################################################
#mkdir -p cmake/portable-target/actions
#
#PORTABLE_TARGET="ColouredMessage.cmake \
#    Functions.cmake \
#    PortableTarget.cmake"
#
#PORTABLE_TARGET_ACTIONS="add_executable.cmake \
#    add_library.cmake \
#    compile_options.cmake \
#    definitions.cmake \
#    include_directories.cmake \
#    link_libraries.cmake \
#    link_qt5_components.cmake \
#    properties.cmake \
#    sources.cmake"
#
#for f in $PORTABLE_TARGET ; do
#    curl_copy $PORTABLE_TARGET_BASE_URL/${f} cmake/portable-target/$f
#    [ $LAST_STATUS -ne 0 ] && exit 1
#done
#
#for f in $PORTABLE_TARGET_ACTIONS ; do
#    curl_copy $PORTABLE_TARGET_BASE_URL/actions/${f} cmake/portable-target/actions/$f
#    [ $LAST_STATUS -ne 0 ] && exit 1
#done
