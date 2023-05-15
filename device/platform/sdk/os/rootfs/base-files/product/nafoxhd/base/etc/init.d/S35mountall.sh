#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	mount -t yaffs2 /dev/mtdblock7 /var/lib/humaxtv
	mount -t yaffs2 /dev/mtdblock8 /var/lib/humaxtv_backup

) 2>&1 | grep -v '(already|nothing was) mounted'

