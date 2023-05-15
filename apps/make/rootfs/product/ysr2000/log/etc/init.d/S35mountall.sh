#!/bin/sh

if [ -f /sys/block/mmcblk0/mmcblk0boot1/force_ro ]; then
	echo 0 > /sys/block/mmcblk0/mmcblk0boot1/force_ro
fi

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1  /dev/mmcblk0p1 /var/lib/humaxtv
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p2 /var/lib/humaxtv_backup
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p3 /var/lib/humaxtv_user

	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1  /dev/mmcblk0gp1p3 /usr/browser/webapps
	mount /dev/mtdblock0 /usr/lib/vmx
) 2>&1 | grep -v '(already|nothing was) mounted'
