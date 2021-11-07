#!/bin/sh

BASE_URL=https://raw.githubusercontent.com/semenovf/scripts/master
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
    FILENAME=$1
    SOURCE=$2
    TARGET=$3

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
    curl_copy $f $BASE_URL/scripts/${f} $f
    [ $LAST_STATUS -ne 0 ] && exit 1
done

for f in $CONFIGS ; do
    curl_copy $f $BASE_URL/configs/${f} $f
    [ $LAST_STATUS -ne 0 ] && exit 1
done

