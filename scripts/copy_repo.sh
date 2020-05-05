#!/usr/bin/env bash
if [ -z ${1+x} ];
then
 echo "> Missing hostname parameter";
 echo "> Usage copy_repo <hostname/ip-address>";
 exit
 fi
TARGET=$1

TAR_ARG="./caffe ./config ./Core ./scripts ./tools ./python ./.gitignore Exp_const_arrivals.cpp README.md RunPipeline.cpp ScheduledPipeline.cpp CMakeLists.txt"

#Compress all
echo '> Compressing repository'
tar -czf edgecaffe.tar.gz ${TAR_ARG}
echo "> Transfering repository to ${TARGET}"
scp ./edgecaffe.tar.gz ubuntu@${TARGET}:.
rm edgecaffe.tar.gz
echo '> Unpacking repository at remote'
ssh ubuntu@${TARGET} 'mkdir -p ~/edgecaffe'
ssh ubuntu@${TARGET} 'tar -xzf edgecaffe.tar.gz -C ~/edgecaffe'
ssh ubuntu@${TARGET} 'mkdir -p ~/edgecaffe/networks'
ssh ubuntu@${TARGET} 'mkdir -p ~/edgecaffe/resources'
ssh ubuntu@${TARGET} 'rm ~/edgecaffe.tar.gz'
echo "> Logging in at ${TARGET}"

ssh ubuntu@${TARGET}