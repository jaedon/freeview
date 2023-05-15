#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 1
	ubiattach -m 2
	ubiattach -m 3
	mount -t ubifs -o sync ubi2:dbdata /var/lib/humaxtv
	mount -t ubifs -o sync ubi2:dbbackup /var/lib/humaxtv_backup
	mount -t ubifs -o sync ubi2:dbuser /var/lib/humaxtv_user

) 2>&1 | grep -v '(already|nothing was) mounted'

