#!/bin/sh

[ -x /usr/bin/opcontrol ] || exit 0

opcontrol --vmlinux=/vmlinux --session-dir=/var/lib/oprofile --callgraph=5
opcontrol --reset
opcontrol --start
opcontrol --dump
