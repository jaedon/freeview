#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	mount /dev/mtdblock3 /usr/lib
	mount /dev/mtdblock2 /usr/bin
	mount /dev/mtdblock1 /opt/share

	mount -t jffs2 /dev/mtdblock8 /var/lib/humaxtv
	mount -t jffs2 /dev/mtdblock5 /var/lib/humaxtv_backup

	chown -R 1001:1001 /var/lib/humaxtv
	chown -R 1001:1001 /var/lib/humaxtv_backup
) 2>&1 | grep -v '(already|nothing was) mounted'

