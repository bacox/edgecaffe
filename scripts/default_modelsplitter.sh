#!/usr/bin/env bash
if [ -z ${1+x} ];
then
 PREFIX=.
 else
 PREFIX=$1
 fi


ARGS="./networks/AgeNet/age_net.caffemodel
./networks/GenderNet/gender_net.caffemodel
./networks/FaceNet/face_full_conv.caffemodel
./networks/SoS/AlexNet_SalObjSub.caffemodel
./networks/SoS_GoogleNet/GoogleNet_SOS.caffemodel"

${PREFIX}/ModelSplitter ${ARGS}