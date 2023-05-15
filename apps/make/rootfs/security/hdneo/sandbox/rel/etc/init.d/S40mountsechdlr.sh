#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for secure handlres..."
fi

(
	mount -o rbind,rshared /bin /home1/sechdlr/root/bin
	mount -o rbind,rshared,rw /dev /home1/sechdlr/root/dev
	mount -o rbind,rshared /etc /home1/sechdlr/root/etc
	mount -o rbind,rshared /lib /home1/sechdlr/root/lib
	mount -o rbind,rshared,rw /media /home1/sechdlr/root/media
	/usr/bin/newmount --bind /home1/sechdlr/root/media /home1/sechdlr/root/media
	/usr/bin/newmount --make-shared /home1/sechdlr/root/media
	mount -o rbind,rshared,rw /mnt /home1/sechdlr/root/mnt
	/usr/bin/newmount --bind /home1/sechdlr/root/mnt /home1/sechdlr/root/mnt
	/usr/bin/newmount --make-shared /home1/sechdlr/root/mnt
	mount -o rbind,rshared /opt /home1/sechdlr/root/opt
	mount -o rbind,rshared /proc /home1/sechdlr/root/proc
	mount -o rbind,rshared /root /home1/sechdlr/root/root
	mount -o rbind,rshared /sbin /home1/sechdlr/root/sbin
	mount -o rbind,rshared /share /home1/sechdlr/root/share
	mount -o rbind,rshared,rw /shared /home1/sechdlr/root/shared
	mount -o rbind,rshared,rw /sys /home1/sechdlr/root/sys
	mount -o rbind.rshared,rw /tmp /home1/sechdlr/root/tmp
	mount -o rbind,rshared /usr /home1/sechdlr/root/usr
	mount -o rbind,rshared,rw /var /home1/sechdlr/root/var

) 2>&1 | grep -v '(already|nothing was) mounted'

