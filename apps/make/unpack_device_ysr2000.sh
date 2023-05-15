#!/bin/bash

PACKAGE_PATH=../../device_package

# 인자가 있다면 인자로 file name 을 만들고 인자가 없다면 최신 file 선택
if [ "$1" = "" ]; then
    for file in $(ls -r ${PACKAGE_PATH}/device_package*.tar); do
		PACKAGE_FILE=${file}
        break;
    done
else
    PACKAGE_FILE=${PACKAGE_PATH}/device_package_$1.tar
fi

# 존재하는 file 인지 check
if [ ! -f ${PACKAGE_FILE} ]; then
    echo ${PACKAGE_FILE} is not exist.
	echo Please check date_time argument
	exit
fi

cp ${PACKAGE_FILE} ../../device_package.tar

cd ../..

BASE_DIR=$(pwd)
MODEL_NAME=ysr2000
CHIP_VER=97241
CHIP_REV=B0


if [ -d device/make ]; then
	mv device device__
else
	rm -rf device
fi

tar xvf device_package.tar

rm -f device_package.tar 

cd device/include

rm -rf directfb fusion sawman

if [ -d ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.${CHIP_VER}${CHIP_REV}/usr/include/directfb ]; then
	cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.${CHIP_VER}${CHIP_REV}/usr/include/directfb
else
	cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_release_build.${CHIP_VER}${CHIP_REV}/usr/include/directfb
fi

DIRECTFB_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${DIRECTFB_DIR} directfb

cd ../platform/sdk/driver/AppLibs/opensource/directfb/build/1.7.7/${CHIP_VER}_linuxuser/include
FUSION_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${FUSION_DIR} fusion

if [ -d ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.${CHIP_VER}${CHIP_REV}/usr/include/sawman ]; then
	cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_debug_build.${CHIP_VER}${CHIP_REV}/usr/include/sawman
else
	cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.7.7_multi_release_build.${CHIP_VER}${CHIP_REV}/usr/include/sawman
fi
SAWMAN_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${SAWMAN_DIR} sawman

cd ../../apps/make

echo
echo
echo finish to unpack ${PACKAGE_FILE}

 
