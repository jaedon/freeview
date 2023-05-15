#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting filesystems for secure handlres..."
fi

(
	mount -o rbind,rshared /bin /home/sechdlr/root/bin
	mount -o rbind,rshared,rw /dev /home/sechdlr/root/dev
	mount -o rbind,rshared /etc /home/sechdlr/root/etc
	mount -o rbind,rshared /lib /home/sechdlr/root/lib
	/usr/local/bin/mount --bind /home/sechdlr/root/media /home/sechdlr/root/media
	/usr/local/bin/mount --make-shared /home/sechdlr/root/media
	mount -o rbind,rshared,rw /home/sechdlr/root/media /media
	/usr/local/bin/mount --bind /home/sechdlr/root/mnt /home/sechdlr/root/mnt
	/usr/local/bin/mount --make-shared /home/sechdlr/root/mnt
	mount -o rbind,rshared,rw /home/sechdlr/root/mnt /mnt
	mount -o rbind,rshared /opt /home/sechdlr/root/opt
	mount -o rbind,rshared /proc /home/sechdlr/root/proc
	mount -o rbind,rshared /root /home/sechdlr/root/root
	mount -o rbind,rshared /sbin /home/sechdlr/root/sbin
	mount -o rbind,rshared /share /home/sechdlr/root/share
	mount -o rbind,rshared,rw /shared /home/sechdlr/root/shared
	mount -o rbind,rsharedmrw /sys /home/sechdlr/root/sys
	mount -o rbind.rshared,rw /tmp /home/sechdlr/root/tmp
	mount -o rbind,rshared /usr /home/sechdlr/root/usr
	mount -o rbind,rshared,rw /var /home/sechdlr/root/var

) 2>&1 | grep -v '(already|nothing was) mounted'

