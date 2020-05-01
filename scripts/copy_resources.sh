#!/usr/bin/env bash

if [ -z ${1+x} ];
then
 echo "Missing hostname parameter";
 echo "Usage copy_resources <hostname/ip-address>";
 exit
 fi
TARGET=$1
echo "Copying resources to ${TARGET}"
ssh ubuntu@${TARGET} 'mkdir -p /opt/edgecaffe/networks'
scp -r ./resources/ ubuntu@${TARGET}:/opt/edgecaffe/resources
