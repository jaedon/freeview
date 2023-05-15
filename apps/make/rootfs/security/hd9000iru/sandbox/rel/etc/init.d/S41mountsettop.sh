#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for settop..."
fi

(
	mount -o rbind,ro /bin /home/settop/root/bin
	mount -o rbind,rshared /dev /home/settop/root/dev
	mount -o rbind,ro /etc /home/settop/root/etc
	mount -o rbind,rshared,ro /lib /home/settop/root/lib
	mount -o rbind,rshared,rw /home/sechdlr/root/media /home/settop/root/media
	mount -o rbind,rshared,rw /home/sechdlr/root/mnt /home/settop/root/mnt
	#mount -o rbind,ro /opt /home/settop/root/opt
	mount -o rbind,rshared /proc /home/settop/root/proc
	#mount -o rbind,ro /root /home/settop/root/root
	mount -o rbind,rshared,so /sbin /home/settop/root/sbin
	#mount -o rbind,ro /share /home/settop/root/share
	mount -o rbind,rshared,rw /shared /home/settop/root/shared
	mount -o rbind,rshared,rw /sys /home/settop/root/sys
	mount -o rbind,rshared,rw /tmp /home/settop/root/tmp

	mount -o rbind,ro /usr/bin /home/settop/root/usr/bin
	mount -o rbind,rshared,ro /usr/etc /home/settop/root/usr/etc
	mount -o rbind,rshared,ro /usr/lib /home/settop/root/usr/lib
	mount -o rbind,ro /usr/sbin /home/settop/root/usr/sbin
	mount -o rbind,ro /usr/fonts /home/settop/root/usr/fonts
	mount -o rbind,ro /usr/image /home/settop/root/usr/image
	mount -o rbind,ro /usr/string /home/settop/root/usr/string

	mount -o rbind,rshared,rw /var /home/settop/root/var

	mount -o rbind,rshared,rw /usr/browser/webapps/apps /home/settop/root/usr/browser/webapps/apps
	mount -o rbind,rshared,rw /usr/browser/webapps/upgrade /home/settop/root/usr/browser/webapps/upgrade

) 2>&1 | grep -v '(already|nothing was) mounted'

