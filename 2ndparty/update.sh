#!/bin/bash

CWD="${PWD}"
LOG_FILE="${CWD}/update.log"
GIT_UPDATER="git pull"

echo `date` > ${LOG_FILE}

for repo in $(ls -d */); do
    cd ${CWD}/${repo%%/} && git pull origin master >> ${LOG_FILE} 2>&1

    if [ $? -eq 0 ] ; then
        echo "Updating ${repo%%/}: SUCCESS"
    else
        echo "Updating ${repo%%/}: FAILURE"
        break
    fi
done
