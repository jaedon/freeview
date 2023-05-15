#!/bin/bash

while true; do

echo "SDCARD R/W Test Loop..."
dd if=/dev/urandom of=/dev/mmcblk1 bs=1M count=10
dd if=/dev/mmcblk1 of=/dev/zero bs=1M count=10

sleep 1

done
