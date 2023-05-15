#!/bin/bash

while true; do

echo "SDCARD Read Test Loop..."
dd if=/dev/mmcblk1 of=/dev/zero bs=1M count=10

sleep 1

done
