#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	/usr/local/bin/mount --bind /var /var
	/usr/local/bin/mount --make-shared /var
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p1 /var/lib/humaxtv
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p2 /var/lib/humaxtv_backup
	mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk0p3 /var/lib/humaxtv_user
) 2>&1 | grep -v '(already|nothing was) mounted'

