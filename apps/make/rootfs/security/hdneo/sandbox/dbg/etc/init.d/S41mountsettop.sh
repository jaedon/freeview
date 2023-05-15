#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for settop..."
fi

(
	mount -o rbind,ro /bin /home1/settop/root/bin
	mount -o rbind,rshared /dev /home1/settop/root/dev
	mount -o rbind,ro /etc /home1/settop/root/etc
	mount -o rbind,rshared,ro /lib /home1/settop/root/lib
	mount -o rbind,rshared,rw /home1/sechdlr/root/media /home1/settop/root/media
	mount -o rbind,rshared,rw /home1/sechdlr/root/mnt /home1/settop/root/mnt
	#mount -o rbind,ro /opt /home1/settop/root/opt
	mount -o rbind,rshared /proc /home1/settop/root/proc
	#mount -o rbind,ro /root /home1/settop/root/root
	mount -o rbind,rshared,so /sbin /home1/settop/root/sbin
	#mount -o rbind,ro /share /home1/settop/root/share
	mount -o rbind,rshared,rw /shared /home1/settop/root/shared
	mount -o rbind,rshared,rw /sys /home1/settop/root/sys
	mount -o rbind,rshared,rw /tmp /home1/settop/root/tmp

	mount -o rbind,ro /usr/bin /home1/settop/root/usr/bin
	mount -o rbind,rshared,ro /usr/etc /home1/settop/root/usr/etc
	mount -o rbind,rshared,ro /usr/lib /home1/settop/root/usr/lib
	mount -o rbind,ro /usr/sbin /home1/settop/root/usr/sbin
	mount -o rbind,ro /usr/fonts /home1/settop/root/usr/fonts
	mount -o rbind,ro /usr/image /home1/settop/root/usr/image
	mount -o rbind,ro /usr/string /home1/settop/root/usr/string
	mount -o rbind,ro /usr/data /home1/settop/root/usr/data
	mount -o rbind,ro /usr/samba /home1/settop/root/usr/samba

	mount -o rbind,rshared,rw /var /home1/settop/root/var
	
) 2>&1 | grep -v '(already|nothing was) mounted'

