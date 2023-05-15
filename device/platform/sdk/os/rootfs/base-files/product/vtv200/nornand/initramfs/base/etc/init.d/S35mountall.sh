#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 4
	ubiattach -m 6
	ubiattach -m 7
	ubiattach -m 5
) 2>&1

