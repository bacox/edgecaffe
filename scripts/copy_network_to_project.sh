#!/usr/bin/env bash

cp -v /opt/edgecaffe/networks/AgeNet/age_net.caffemodel ./networks/AgeNet/age_net.caffemodel
cp -rfv /opt/edgecaffe/networks/AgeNet/age_net_partials ./networks/AgeNet/age_net_partials

cp -v /opt/edgecaffe/networks/FaceNet/face_full_conv.caffemodel ./networks/FaceNet/face_full_conv.caffemodel
cp -rfv /opt/edgecaffe/networks/FaceNet/face_full_conv_partials ./networks/FaceNet/face_full_conv_partials
