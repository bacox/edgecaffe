#!/usr/bin/env bash

if [ -z ${1+x} ];
then
 echo "> Missing hostname parameter";
 echo "> Usage copy_repo <hostname/ip-address>";
 exit
 fi
TARGET=$1

echo "> Creating archive of analysis files at ${TARGET}"
ssh ubuntu@${TARGET} "cd /opt/edgecaffe && tar -czf ~/analysis-${TARGET}.tar.gz ./analysis/*"
echo "> Transfering archive of analysis files from ${TARGET}"
scp ubuntu@${TARGET}:~/analysis-${TARGET}.tar.gz ./analysis-${TARGET}.tar.gz
echo "> Deleting archive of analysis files at ${TARGET}"
ssh ubuntu@${TARGET} "rm ~/analysis-${TARGET}.tar.gz"
