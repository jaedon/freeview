#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for imma..."
fi

(
	mount -o rbind,ro /bin /home/imma/root/bin
	mount -o rbind,rshared /dev /home/imma/root/dev
	mount -o rbind,ro /etc /home/imma/root/etc
	mount -o rbind,ro /lib /home/imma/root/lib
	mount -o rbind,rshared,rw /home/sechdlr/root/media /home/imma/root/media
	mount -o rbind,rshared,rw /home/sechdlr/root/mnt	/home/imma/root/mnt
	#mount -o rbind,ro /opt /home/imma/root/opt
	mount -o rbind,rshared /proc /home/imma/root/proc
	#mount -o rbind,ro /root /home/imma/root/root

	#mount -o rbind,rshared,rw /shared	/home/imma/root/shared

	mount -o rbind,ro /sbin /home/imma/root/sbin
	mount -o rbind /tmp /home/imma/root/tmp

	mount -o rbind,ro /usr/bin /home/imma/root/usr/bin
	mount -o rbind,ro /usr/etc /home/imma/root/usr/etc
	mount -o rbind,ro /usr/lib /home/imma/root/usr/lib
	mount -o rbind,ro /usr/sbin /home/imma/root/usr/sbin

	mount -o rbind,rshared,rw /var /home/imma/root/var

) 2>&1 | grep -v '(already|nothing was) mounted'

