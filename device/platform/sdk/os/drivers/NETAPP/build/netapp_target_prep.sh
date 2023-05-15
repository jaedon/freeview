#!/bin/bash
###############################################################################
# NetApp Target Preparation Script
#
# Run this script to setup your platform to use NetApp examples
###############################################################################
HOME=/usr/local/bin
PGM=netapp_target_prep.sh
SQUASH_OUT= > /dev/null 2>&1
############################################################
# FUNCTION: check shell
####vvvv####vvvv####vvvv####vvvv####vvvv####vvvv####vvvv####
function check_shell {
    case $0 in
    *bash)
        # you're sourcing it. you're ok
        ;;
    
    \-sh)
        # you're sourcing it. you're ok
        ;;
    *)
        echo "$PGM: ERROR:"; echo
        echo "    This script must be sourced, not executed."
        echo "    Please type the following:"; echo
        echo "          source $PGM"; echo
        exit 1
    esac
}


# rmmod previous driver
TEST_BSASERVER=`ps | grep bsa_server >/dev/null && echo 1`
if [ ! -z $TEST_BSASERVER ]; then
    killall bsa_server ${SQUASH_OUT}
fi

TEST_DHD=`lsmod | grep dhd >/dev/null && echo 1`
if [ ! -z $TEST_DHD ]; then
    rmmod dhd
fi

TEST_WL=`lsmod | grep wl >/dev/null && echo 1`
if [ ! -z $TEST_WL ]; then
    rmmod wl
fi

TEST_USBSHIM=`lsmod | grep bcm_usbshim >/dev/null && echo 1`
if [ ! -z $TEST_USBSHIM ]; then
    rmmod bcm_usbshim
fi

TEST_BTHID=`lsmod | grep bthid >/dev/null && echo 1`
if [ ! -z $TEST_BTHID ]; then
    rmmod bthid
fi

TEST_BTUSB=`lsmod | grep btusb >/dev/null && echo 1`
if [ ! -z $TEST_BTUSB ]; then
    rmmod btusb
fi

if [ ! -e /lib/firmware/brcm ] ; then
    mkdir -p /lib/firmware
    ln -s /usr/local/firmware/brcm /lib/firmware/brcm
fi
# install driver

MODULE_PATH=./../lib/modules
if [ -e $MODULE_PATH/nexus.ko ]; then \
	insmod $MODULE_PATH/nexus.ko config="${config}"; \
elif [ -e $MODULE_PATH/bcmdriver.ko ]; then \
	insmod $MODULE_PATH/bcmdriver.ko; \
	mknod /dev/brcm0 c 30 0; \
else \
	echo "Nexus Drivers not found at ${MODULE_PATH}"; \
fi

export LD_LIBRARY_PATH=/usr/local/lib:.:${LD_LIBRARY_PATH}

echo "Prep. Complete! "

