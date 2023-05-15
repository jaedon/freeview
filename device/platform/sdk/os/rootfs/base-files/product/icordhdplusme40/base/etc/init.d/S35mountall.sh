#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 4
	ubiattach -m 5
	mount -t ubifs ubi1:dbdata /var/lib/humaxtv
	mount -t ubifs ubi1:dbbackup /var/lib/humaxtv_backup
	mount -t ubifs ubi1:dbuser /var/lib/humaxtv_user

) 2>&1 | grep -v '(already|nothing was) mounted'
