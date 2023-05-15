#!/bin/bash

while true; do

echo "eMMC R/W Test Loop..."
dd if=/dev/urandom of=/dev/mmcblk0 bs=1M count=10
dd if=/dev/mmcblk0 of=/dev/zero bs=1M count=10

sleep 1

done
