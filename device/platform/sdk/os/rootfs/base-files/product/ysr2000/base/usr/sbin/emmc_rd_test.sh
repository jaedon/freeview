#!/bin/bash

while true; do

echo "eMMC Read Test Loop..."
dd if=/dev/mmcblk0 of=/dev/zero bs=1M count=10

sleep 1

done
