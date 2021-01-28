#!/usr/bin/env bash
if [ -z ${1+x} ];
then
 PREFIX=.
 else
 PREFIX=$1
 fi


ARGS="
./networks/AgeNet/age_net.caffemodel
./networks/EmotionNet/EmotiW_VGG_S.caffemodel
./networks/FaceNet/face_full_conv.caffemodel
./networks/GenderNet/gender_net.caffemodel
./networks/MemNet/memnet.caffemodel
./networks/ObjectNet/VGG_ILSVRC_19_layers.caffemodel
./networks/SceneNet/places205CNN_iter_300000_upgraded.caffemodel
./networks/SoS/AlexNet_SalObjSub.caffemodel
./networks/SoS_GoogleNet/GoogleNet_SOS.caffemodel
./networks/TinyYolo/tiny_yolo.caffemodel"

${PREFIX}/ModelSplitter ${ARGS}