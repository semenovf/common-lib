#!/bin/bash

CWD=`pwd`

if [ -e .git ] ; then

    git checkout master && git pull origin master \
        && git submodule update --init --recursive \
        && git submodule update --init --remote -- 3rdparty/portable-target

fi

