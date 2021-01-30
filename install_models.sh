#!/bin/bash
LOG_PREFIX=">>>"
VERSION="v1.1"
MODEL_FILE_NAME=edgecaffe-models-$VERSION
MODEL_FILE=$MODEL_FILE_NAME.tar.gz
URL=https://gitlab.com/bacox/edgecaffe-models/-/archive/$VERSION/$MODEL_FILE

if [[ ! -f "$MODEL_FILE" ]]; then
    echo "$LOG_PREFIX Model archive missing."
    echo "$LOG_PREFIX Fetching model archive from repo"
    wget $URL
else
	echo "$LOG_PREFIX Using cached model archive."
fi

echo "$LOG_PREFIX Extracting models"
tar -zxf ./$MODEL_FILE
echo "$LOG_PREFIX Moving models"
cp -r $MODEL_FILE_NAME/* ./networks
echo "$LOG_PREFIX Removing temporary folder"
rm -rf $MODEL_FILE_NAME
echo "$LOG_PREFIX Done"