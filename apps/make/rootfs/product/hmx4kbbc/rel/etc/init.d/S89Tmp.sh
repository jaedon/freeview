#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS

ifconfig gphy up
udhcpc -i gphy &
