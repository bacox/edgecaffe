#!/usr/bin/env bash

echo '> Retrieving arm deb file from RPI_4'
mkdir -p ./tmp
rm ./tmp/edgecaffe-arm.deb || true
scp ubuntu@rpi_4:~/projects/edgecaffe/cmake-build-debug/EdgeCaffe-0.1.1-Linux-EdgeCaffeApp.deb ./tmp/edgecaffe-arm.deb

echo '> Copy deb file to RPI_1'
ssh ubuntu@rpi_1 'sudo apt-get purge -y edgecaffe' || true
ssh ubuntu@rpi_1 'rm edgecaffe-arm.deb' || true
scp ./tmp/edgecaffe-arm.deb ubuntu@rpi_1:.  && echo "Done transfering to RPI_1"  &

echo '> Copy deb file to RPI_2'
ssh ubuntu@rpi_2 'sudo apt-get purge -y edgecaffe' || true
ssh ubuntu@rpi_2 'rm edgecaffe-arm.deb' || true
scp ./tmp/edgecaffe-arm.deb ubuntu@rpi_2:.  && echo "Done transfering to RPI_2" &

echo '> Copy deb file to RPI_4'
ssh ubuntu@rpi_4 'sudo apt-get purge -y edgecaffe' || true
ssh ubuntu@rpi_4 'rm edgecaffe-arm.deb' || true
scp ./tmp/edgecaffe-arm.deb ubuntu@rpi_4:. && echo "Done transfering to RPI_4" &

read  -n 1 -p "Continue with installation" mainmenuinput
ssh ubuntu@rpi_1 'sudo apt-get install -y ./edgecaffe-arm.deb' &
ssh ubuntu@rpi_2 'sudo apt-get install -y ./edgecaffe-arm.deb' &
ssh ubuntu@rpi_4 'sudo apt-get install -y ./edgecaffe-arm.deb' &



