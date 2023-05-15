#!/bin/bash
cd ../..

# 인자가 있다면 인자로 file name 을 만들고 인자가 없다면 최신 file 선택
if [ "$1" = "" ]; then
    DATETIME=$(date +%Y%m%d_%H%M%S)
else
    DATETIME="$1"
fi

BASE_DIR=${PWD}
DEVICE_PACKAGE_DIR=${BASE_DIR}/device_package
APP_MAKE_DIR=${BASE_DIR}/apps/make
DOWN_DIR=${BASE_DIR}/model/down

if [ -d device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_release_build.97250B0/ ]; then
	tar cvf device_package.tar device/include/* \
	device/platform/driver/di/drv/include/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_release_build.97250B0/usr/include/directfb/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/build/1.7.7/97250_linuxuser/include/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_release_build.97250B0/usr/include/sawman/* \
	device/images/cdig2000c/* --exclude=".svn"
	MODE="RELEASE"
else
	tar cvf device_package.tar device/include/* \
	device/platform/driver/di/drv/include/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.97250B0/usr/include/directfb/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/build/1.7.7/97250_linuxuser/include/* \
	device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.97250B0/usr/include/sawman/* \
	device/images/cdig2000c/* --exclude=".svn"
fi 



if [ ! -d device_package/ ]; then
    mkdir device_package/
fi
mv device_package.tar device_package/device_package_${DATETIME}.tar

cd ${DOWN_DIR}
./down.sh full_head local_info > ${DATETIME}
mv ${DATETIME} ${DEVICE_PACKAGE_DIR}


cd ${APP_MAKE_DIR}

