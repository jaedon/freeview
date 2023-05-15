#!/bin/bash

while true; do

echo "eMMC Write Test Loop..."
dd if=/dev/urandom of=/dev/mmcblk0 bs=1M count=10

sleep 1

done
