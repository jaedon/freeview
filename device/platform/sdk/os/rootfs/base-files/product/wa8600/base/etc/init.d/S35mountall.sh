#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p1 /var/lib/humaxtv
mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p2 /var/lib/humaxtv_backup
mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p3 /var/lib/humaxtv_user
