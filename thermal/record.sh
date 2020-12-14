#!/usr/bin/env bash

echo $(date) $(cat /sys/class/thermal/thermal_zone*/temp) $(vcgencmd measure_temp) >> /opt/analysis/thermal/cpu_temp.log