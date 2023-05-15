#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 7
	ubiattach -m 8
	ubiattach -m 9
	ubiattach -m 10
	ubiattach -m 11
	ubiattach -m 12
	mount -t ubifs -o sync ubi4:altidata /mnt/flash
	mount -t ubifs -o sync ubi4:altidb /var/lib/humaxtv
	mount -t ubifs -o sync ubi4:altidbbackup /var/lib/humaxtv_backup
) 2>&1 | grep -v '(already|nothing was) mounted'

