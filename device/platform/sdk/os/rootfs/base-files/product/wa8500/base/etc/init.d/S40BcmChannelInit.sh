#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "bcm pcieeth module init & get ip from bcm3383..."
fi

#udhcpc -i bcm0 -p /tmp/udhcpc.bcm0.pid
#killall udhcpc
ifconfig bcm0 192.168.100.10 netmask 255.255.255.0 up
ifconfig bcm0:0 192.168.17.10 netmask 255.255.255.0 up
