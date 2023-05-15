#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

(
	ubiattach -m 3
	ubiattach -m 4
	mount -t ubifs -o sync ubi1:dbdata /var/lib/humaxtv
	mount -t ubifs -o sync ubi1:dbbackup /var/lib/humaxtv_backup
	mount -t ubifs -o sync ubi1:dbuser /var/lib/humaxtv_user

	if [ -f /var/lib/humaxtv/appsmnt ]
	then
		APPSMNT=`cat /var/lib/humaxtv/appsmnt`
		if [ $APPSMNT = "ext2" ]
		then
			EXTMNT=upgrade
			EXT2MNT=apps
		else
			EXTMNT=apps
			EXT2MNT=upgrade
		fi
	else
		# default mount point
		EXTMNT=apps
		EXT2MNT=upgrade
	fi

	mount -t ubifs -o sync ubi1:dbuserext /usr/browser/webapps/$EXTMNT
	mount -t ubifs -o sync ubi1:dbuserext2 /usr/browser/webapps/$EXT2MNT
	echo $EXTMNT > /var/lib/humaxtv/curext
) 2>&1 | grep -v '(already|nothing was) mounted'

