#!/usr/bin/env bash



echo 'Closing RPI 1 GB'
ssh ubuntu@rpi_1 'sudo init 0' &

echo 'Closing RPI 2 GB'
ssh ubuntu@rpi_2 'sudo init 0' &

echo 'Closing RPI 4 GB'
ssh ubuntu@rpi_4 'sudo init 0' &