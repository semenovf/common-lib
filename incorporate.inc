#!/bin/sh

#HEADERS="safeformat.hpp"
#SOURCE_ROOT_DIR="../pfs-common"
#TARGET_ROOT_DIR=`pwd`

if [ ! -d $SOURCE_ROOT_DIR ]; then
    echo "E: Source directory not found: $SOURCE_ROOT_DIR" 1>&2
    exit 1
fi

if [ ! -d $TARGET_ROOT_DIR ]; then
    echo "E: Target directory not found: $TARGET_ROOT_DIR" 1>&2
    exit 1
fi

cd $SOURCE_ROOT_DIR

for h in $HEADERS ; do
    echo "I: Process header: $h"
    source=$h

    if [ ! -f $source ]; then
        echo "E: Header not found: $SOURCE_ROOT_DIR/$source" 1>&2
        exit 1
    fi

    target=$TARGET_ROOT_DIR/$h
    copy_options="--parents"

    if [ -f $target ]; then
        cmp -s $source $target

        if [ $? -eq 0 ]; then
            echo "I: Process header is not necessary (source and target are identical): $h"
            continue
        else
            echo "I: Source: `ls -l $source`"
            echo "I: Target: `ls -l $target`"

            read -p "W: Files are different, overwrite [yN]: " answer

            case "$answer" in
                y|Y)
                    copy_options="$copy_options -f"
                    ;;
                *)
                    continue
                    ;;
            esac
        fi
    fi

    cp $copy_options $source $TARGET_ROOT_DIR

    if [ $? -eq 0 ]; then
        echo "I: Process header successful: $h"
    else
        echo "I: Process header failed: $h" 1>&2
        exit 1
    fi
done
