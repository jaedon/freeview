#!/bin/sh
#
# thttpd.sh - startup script for thttpd on FreeBSD
#
# This goes in /usr/local/etc/rc.d and gets run at boot-time.

case "$1" in

    start)    
    /usr/bin/thttpd -D -C /usr/bin/thttpd_conf &
	echo "Start cThrough" >&2
    ;;

    stop)
    kill -USR1 `cat /var/thttpd/thttpd.pid`
    ;;

    *)
    echo "usage: $0 { start | stop }" >&2
    exit 1
    ;;

esac
