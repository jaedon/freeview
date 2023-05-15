#!/bin/sh
#GP2
echo 'partitioning GP2...'
parted -s /dev/mmcblk0gp2 mklabel gpt u s mkpart primary 34 100% name 1 ota
echo 'creating EXT4 f/s in User Data Area partition...'
mkfs.ext4 /dev/mmcblk0gp2p1
echo 'completed.'

