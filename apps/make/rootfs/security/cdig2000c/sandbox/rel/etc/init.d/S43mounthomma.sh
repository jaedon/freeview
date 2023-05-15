#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for homma..."
fi

(
	mount -o rbind,ro /bin /home/homma/root/bin
	mount -o rbind,rshared /dev /home/homma/root/dev
	mount -o rbind,ro /etc /home/homma/root/etc
	mount -o rbind,ro /lib /home/homma/root/lib
	mount -o rbind,rshared,rw /home/sechdlr/root/media /home/homma/root/media
	mount -o rbind,rshared,rw /home/sechdlr/root/mnt	/home/homma/root/mnt
	#mount -o rbind,ro /opt /home/homma/root/opt
	mount -o rbind,rshared /proc /home/homma/root/proc
	#mount -o rbind,ro /root /home/homma/root/root

	#mount -o rbind,rshared,rw /shared	/home/homma/root/shared

	mount -o rbind,ro /sbin /home/homma/root/sbin
	mount -o rbind /tmp /home/homma/root/tmp

	mount -o rbind,ro /usr/bin /home/homma/root/usr/bin
	mount -o rbind,ro /usr/etc /home/homma/root/usr/etc
	mount -o rbind,ro /usr/lib /home/homma/root/usr/lib
	mount -o rbind,ro /usr/sbin /home/homma/root/usr/sbin

	mount -o rbind,rshared,rw /var /home/homma/root/var

) 2>&1 | grep -v '(already|nothing was) mounted'

