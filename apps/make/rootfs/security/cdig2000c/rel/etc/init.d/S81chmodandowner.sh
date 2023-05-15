#!/bin/sh

echo "Change a permission and ownership of RW files..."

if [ -f /var/lib/humaxtv/TZ ]; then
	chown obama:settop /var/lib/humaxtv/TZ
else
	touch /var/lib/humaxtv/TZ
	chown obama:settop /var/lib/humaxtv/TZ
fi
