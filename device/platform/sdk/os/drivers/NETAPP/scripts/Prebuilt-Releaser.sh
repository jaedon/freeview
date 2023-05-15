#!/bin/bash
####################################################################################################
# 					INTERNAL NETAPP RELEASE GENERATION SCRIPT
# This script only should be run by Broadcom NetApp development team
####################################################################################################
KERNEL_VERSION=$1
TOOLCHAIN=$2
PLATFORM=$3
LINUX=/opt/brcm/${KERNEL_VERSION}/${PLATFORM}
NEW_PATH=/opt/toolchains/${TOOLCHAIN}/bin:${PATH}
export PATH=${NEW_PATH}
export LINUX=${LINUX}
echo "PATH=${PATH}"
echo "LINUX=${LINUX}"

echo "Cleaning..."
make -C build distclean APPLIBS_TOP= LINUX=${LINUX}

echo "Building for ${KERNEL_VERSION}..."
make -C build prebuilt APPLIBS_TOP= LINUX=${LINUX}

