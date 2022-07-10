#!/bin/bash

CWD=`pwd`

if [ -e .git ] ; then

    git checkout master && git pull origin master \
        && git submodule update --init \
        && cd 3rdparty/portable-target && git checkout master && git pull origin master \
        && cd $CWD

fi

