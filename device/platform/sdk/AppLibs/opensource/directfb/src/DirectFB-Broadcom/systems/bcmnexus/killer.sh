#!/bin/sh
#
# Fancy little script to kill processes that our connected to our driver
# so we can rmmod the driver.
# Written by Troy Trammel
# Copyright 2008 Broadcom Corp.
# Date: 28-Jan-2008
# 
#
# 1. Look at driver and verify someone is still attached to it
#
REFS=`lsmod | grep bcmdriver | cut -d ' ' -f 3`
if [ "$REFS" \> "0" ]; then
    echo "Need to kill processes"
   #
   # 2. Go though all processes looking for open file descriptors connected
   # to our driver
   cd /proc
   for process in [1-9]*; do
      pushd $process > /dev/null
      if [ -d fd ]; then
         cd fd
         if ls -l | grep brcm0 > /dev/null ; then 
            echo "Killing " $process
            kill -9 $process
         fi
      fi
      popd > /dev/null
   done
else
    echo "No processes to be killed, exiting..."
fi
