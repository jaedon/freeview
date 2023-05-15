#!/bin/sh

echo "ifconfig eth0 up"
ifconfig eth0 up
wait 1

echo "ifconfig lo up"
ifconfig lo up
wait 1

echo "udhcpc -i eth0 &"
udhcpc -i eth0 &
wait 10

ifconfig -a

exit 0