#!/bin/bash

cd ../..

tar cvf device_package.tar device/include/* device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97425B2/usr/include/directfb/* device/platform/sdk/driver/AppLibs/opensource/directfb/build/1.4.17/97425_linuxuser/include/* device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97425B2/usr/include/sawman/* device/images/jcngs/*

DATETIME=$(date +%Y%m%d_%H%M%S)
mv device_package.tar apps/make/device_package_${DATETIME}.tar

cd apps/make
