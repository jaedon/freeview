#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 5
	ubiattach -m 6
	/usr/bin/newmount --bind /var /var
	/usr/bin/newmount --make-shared /var
) 2>&1 | grep -v '(already|nothing was) mounted'

