#!/bin/sh
umount /var/lib/humaxtv
echo 'partitionaing eMMC flash'
#GP0
echo 'partitioning GP0...'
parted -s /dev/mmcblk0gp0 mklabel gpt u s mkpart primary 34 100% name 1 upgrader
#GP1
echo 'partitioning GP1...'
parted -s /dev/mmcblk0gp1 mklabel gpt u s mkpart primary 34 12321 name 1 kernel mkpart primary 12322 524321 name 2 rootfs mkpart primary 524322 100% name 3 apps
#GP2
echo 'partitioning GP2...'
parted -s /dev/mmcblk0gp2 mklabel gpt u s mkpart primary 34 147489 name 1 otaup mkpart primary 147490 720929 name 2 otamain mkpart primary 720930 100% name 3 otaapps
#GP4
echo 'partitioning GP3...'
parted -s /dev/mmcblk0gp3 mklabel gpt u s mkpart primary 34 100% name 1 securefs
#MainFS
echo 'partitioning User Data Area...'
parted -s /dev/mmcblk0 mklabel gpt u s mkpart primary 34 100% name 1 fs
echo 'creating EXT4 f/s in User Data Area partition...'
mkfs.ext4 /dev/mmcblk0p1
echo 'completed.'

