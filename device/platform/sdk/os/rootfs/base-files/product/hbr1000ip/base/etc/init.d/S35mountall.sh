#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 3
	ubiattach -m 4
	mount -t ubifs -o sync ubi1:dbbackup /var/lib/humaxtv_backup

) 2>&1 | grep -v '(already|nothing was) mounted'

