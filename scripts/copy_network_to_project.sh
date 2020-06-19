#!/usr/bin/env bash


cp /opt/networks/AgeNet/age_net.caffemodel ./networks/AgeNet/age_net.caffemodel
cp -rf /opt/networks/AgeNet/age_net_partials ./networks/AgeNet/age_net_partials

cp /opt/networks/FaceNet/face_full_conv.caffemodel ./networks/FaceNet/face_full_conv.caffemodel
cp -rf /opt/networks/FaceNet/face_full_conv_partials ./networks/FaceNet/face_full_conv_partials