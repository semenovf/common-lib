#!/bin/bash

LOG_FILE='download.log'
GIT_DOWNLOADER="git clone"

# SSH (for developers)
SSH_SOURCES="git@github.com:semenovf/portable-target.git -b master portable-target
git@github.com:semenovf/scripts.git -b master scripts"

# HTTPS
HTTPS_SOURCES="https://github.com/semenovf/portable-target.git --depth=1 -b master portable-target
https://github.com/semenovf/scripts.git --depth=1 -b master scripts"

if [ "$1" = "https" ] ; then
    DEFAULT_SOURCES=${HTTPS_SOURCES}
else 
    DEFAULT_SOURCES=${SSH_SOURCES}
fi

DEFAULT_DOWNLOADER=${GIT_DOWNLOADER}

IFS=$'\n'

echo `date` > ${LOG_FILE}

for src in ${DEFAULT_SOURCES} ; do
    eval "${DEFAULT_DOWNLOADER} $src" >> ${LOG_FILE} 2>&1

    if [ $? -eq 0 ] ; then
        echo "Cloning $src: SUCCESS"
    else
        echo "Cloning $src: FAILURE"
    fi
done
