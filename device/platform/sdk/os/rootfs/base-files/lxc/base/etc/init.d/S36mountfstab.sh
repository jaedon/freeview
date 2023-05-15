#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems in fstab..."
fi

(
	mount -a

) 2>&1 | grep -v '(already|nothing was) mounted'

