#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	if grep -q 'root=/dev/mtdblock2' /proc/cmdline ; then
		mount /dev/mtdblock3 /usr/bin
	fi
	
	if grep -q 'root=/dev/mtdblock5' /proc/cmdline ; then
		mount /dev/mtdblock6 /usr/bin
	fi

) 2>&1 | grep -v '(already|nothing was) mounted'

