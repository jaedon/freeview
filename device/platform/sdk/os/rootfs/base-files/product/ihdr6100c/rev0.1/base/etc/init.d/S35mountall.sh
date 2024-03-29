#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 7 
	ubiattach -m 8
	mount -t ubifs -o sync ubi1:dbdata /var/lib/humaxtv
	mount -t ubifs -o sync ubi1:dbbackup /var/lib/humaxtv_backup
	mount -t ubifs -o sync ubi1:dbuser /var/lib/humaxtv_user

) 2>&1 | grep -v '(already|nothing was) mounted'

# Set up MoCA link, if present
[ -e /dev/bmoca0 ] && mocacfg boot
