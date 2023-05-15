#!/bin/bash

while true; do

echo "SDCARD Write Test Loop..."
dd if=/dev/urandom of=/dev/mmcblk1 bs=1M count=10

sleep 1

done
