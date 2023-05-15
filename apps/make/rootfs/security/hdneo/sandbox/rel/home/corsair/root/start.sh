#!/bin/sh

route add default gw 10.255.128.200

telnetd &

mount -t ubifs -o sync ubi1:dbdata /var/lib/humaxtv
mount -t ubifs -o sync ubi1:dbbackup /var/lib/humaxtv_backup
mount -t ubifs -o sync ubi1:dbuser /var/lib/humaxtv_user

chmod 777 /var/lib/humaxtv
chmod 777 /var/lib/humaxtv_backup
chmod 777 /var/lib/humaxtv_user

/home/hamalauncher


