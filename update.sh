#!/bin/bash

CWD=`pwd`

if [ -d .git ] ; then

    git pull \
        && git submodule update --init \
        && cd 3rdparty/portable-target && git pull && git checkout master \
        && cd $CWD

fi

