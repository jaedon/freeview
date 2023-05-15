#!/bin/sh
#MainFS
echo 'partitioning User Data Area...'
parted -s /dev/mmcblk0 mklabel gpt u s mkpart primary 34 100% name 1 fs
echo 'creating EXT4 f/s in User Data Area partition...'
mkfs.ext4 /dev/mmcblk0p1
echo 'completed.'

