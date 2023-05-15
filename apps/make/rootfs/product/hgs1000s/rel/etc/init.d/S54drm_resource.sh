#!/bin/sh

echo "copy drm resouce files";

if [ ! -d /var/lib/humaxtv/drm ]; then
	mkdir /var/lib/humaxtv/drm
	mkdir /var/lib/humaxtv/drm/playready25/
	cp -af /usr/drm/playready25.bin /var/lib/humaxtv/drm/playready25/
fi
