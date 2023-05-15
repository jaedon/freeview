#!/bin/sh

echo "Change a permission and ownership of RW files..."

if [ -d /usr/browser/webapps/apps ]; then
	chmod 777 /usr/browser/webapps/apps
fi
if [ -d /usr/browser/webapps/upgrade ]; then
	chmod 777 /usr/browser/webapps/upgrade
fi
if [ -f /var/lib/humaxtv/TZ ]; then
	chown obama:settop /var/lib/humaxtv/TZ
fi
