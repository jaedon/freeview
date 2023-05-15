#!/bin/bash
####################################################################################################
# 					INTERNAL NETAPP RELEASE GENERATION SCRIPT
# This script is used to pull the various opensource libraries together with NetApp library to generate
# a release. This script only should be run by Broadcom NetApp development team
####################################################################################################
RELEASE_NAME=$1
TARBALL_ONLY=$2
LIBS="udev usb flac sqlite json-c iperf avahi"

echo "Cleaning..."
make -C build distclean
for lib in $LIBS
do
    rm -rf libs/$lib
    make -C ../../opensource/$lib clean
    make -C ../../opensource/$lib distclean
done


echo "Copying libraries..."
for lib in $LIBS
do
    cp -Rpf ../../opensource/$lib libs
done

echo "Building Release Tarball..."
tar --exclude=.git -zcf ../${RELEASE_NAME}.tgz *

if [ "${TARBALL_ONLY}" == "" ]; then
    echo "Tagging..."
    git tag -f $RELEASE_NAME
    git push --tags
    echo "Uploading..."
    scp ../${RELEASE_NAME}.tgz steven@stb-irva-01.broadcom.com:/projects/applibs/netapp/${RELEASE_NAME}.tgz
fi

