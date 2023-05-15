#!/bin/sh
dd if=/dev/urandom of=testfile_$1 bs=1 count=$1
