#!/usr/bin/env bash

function copy_model() {
local TARGET=$1
echo "> Transfer archive to ${TARGET}"
scp -r ./networks.tar.gz ubuntu@${TARGET}:~/networks.tar.gz

ssh ubuntu@${TARGET} 'mv ~/networks.tar.gz /opt/edgecaffe/networks/networks.tar.gz'
ssh ubuntu@${TARGET} 'cd /opt/edgecaffe/networks && tar -xvzf ./networks.tar.gz'
ssh ubuntu@${TARGET} 'rm /opt/edgecaffe/networks/networks.tar.gz'

}
echo '> Creating target directories'
ssh ubuntu@rpi_1 'sudo mkdir -p /opt/edgecaffe/networks' &
ssh ubuntu@rpi_2 'sudo mkdir -p /opt/edgecaffe/networks' &
ssh ubuntu@rpi_4 'sudo mkdir -p /opt/edgecaffe/networks'

echo '> Creating analysis directories'
ssh ubuntu@rpi_1 'sudo mkdir -p /opt/analysis' &
ssh ubuntu@rpi_2 'sudo mkdir -p /opt/analysis' &
ssh ubuntu@rpi_4 'sudo mkdir -p /opt/analysis'

echo '> Setting permissions'
ssh ubuntu@rpi_1 'sudo chown -R $USER:$USER /opt/edgecaffe'&
ssh ubuntu@rpi_2 'sudo chown -R $USER:$USER /opt/edgecaffe' &
ssh ubuntu@rpi_4 'sudo chown -R $USER:$USER /opt/edgecaffe'

echo '> compressing models'
tar -cvzf networks.tar.gz --exclude='*Generated*' ./networks

copy_model rpi_4 &
copy_model rpi_2 &
copy_model rpi_1
