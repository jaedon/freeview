#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for corsair..."
fi

(
	mount -o rbind,ro /bin /home/corsair/root/bin
	mount -o rbind,rshared /dev /home/corsair/root/dev
	mount -o rbind,ro /etc /home/corsair/root/etc
	mount -o rbind,ro /lib /home/corsair/root/lib
	mount -o rbind,rshared,rw /home/sechdlr/root/media /home/corsair/root/media
	mount -o rbind,rshared,rw /home/sechdlr/root/mnt	/home/corsair/root/mnt
	#mount -o rbind,ro /opt /home/corsair/root/opt
	mount -o rbind,rshared /proc /home/corsair/root/proc
	#mount -o rbind,ro /root /home/corsair/root/root

	mount -o rbind,rshared,rw /shared	/home/corsair/root/shared

	mount -o rbind,ro /sbin /home/corsair/root/sbin
	mount -o rbind /tmp /home/corsair/root/tmp

	mount -o rbind,ro /usr/bin /home/corsair/root/usr/bin
	mount -o rbind,ro /usr/etc /home/corsair/root/usr/etc
	mount -o rbind,ro /usr/lib /home/corsair/root/usr/lib
	mount -o rbind,ro /usr/sbin /home/corsair/root/usr/sbin

	mount -o rbind,ro /usr/fonts /home/corsair/root/usr/fonts
	mount -o rbind,ro /usr/string /home/corsair/root/usr/string

	mount -o rbind,rshared,rw /var /home/corsair/root/var

	mount -o rbind,rshared,rw /usr/browser/webapps/apps /home/corsair/root/usr/browser/webapps/apps
	mount -o rbind,rshared,rw /usr/browser/webapps/upgrade /home/corsair/root/usr/browser/webapps/upgrade

) 2>&1 | grep -v '(already|nothing was) mounted'

