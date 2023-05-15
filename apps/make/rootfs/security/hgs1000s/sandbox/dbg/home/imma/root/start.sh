#!/bin/sh

export ipc_cso=10.255.128.211
export ipc_mediapb=10.255.128.200

route add default gw 10.255.128.200

telnetd &
/home/hamalauncher


