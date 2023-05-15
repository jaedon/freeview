#!/bin/sh

if [ ! -f /var/lib/humaxtv/pgtool ]; then
    rmmod r8101
    modprobe pgdrv
    ./rtnicpg-arm-cortex-a9 /w /efuse
    if [ $? -gt 0 ]; then
        echo -e " pgtool error "
    else
        echo -e " pgtool ok "
        touch /var/lib/humaxtv/pgtool
    fi
    rmmod pgdrv
    modprobe r8101
fi
