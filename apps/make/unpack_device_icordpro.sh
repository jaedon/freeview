#!/bin/bash

PACKAGE_PATH=../../device_package

# ���ڰ� �ִٸ� ���ڷ� file name �� ����� ���ڰ� ���ٸ� �ֽ� file ����
if [ "$1" = "" ]; then
    for file in $(ls -r ${PACKAGE_PATH}/device_package*.tar); do
		PACKAGE_FILE=${file}
        break;
    done
else
    PACKAGE_FILE=${PACKAGE_PATH}/device_package_$1.tar
fi

# �����ϴ� file ���� check
if [ ! -f ${PACKAGE_FILE} ]; then
    echo ${PACKAGE_FILE} is not exist.
	echo Please check date_time argument
	exit
fi

cp ${PACKAGE_FILE} ../../device_package.tar

cd ../..

BASE_DIR=$(pwd)

if [ -d device/make ]; then
	mv device device__
else
	rm -rf device
fi

tar xvf device_package.tar

rm -f device_package.tar 

cd device/include

rm -rf directfb fusion sawman

cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97425B2/usr/include/directfb
DIRECTFB_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${DIRECTFB_DIR} directfb

cd ../platform/sdk/driver/AppLibs/opensource/directfb/build/1.4.17/97425_linuxuser/usr/include
FUSION_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${FUSION_DIR} fusion

cd ../platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97425B2/usr/include/sawman
SAWMAN_DIR=$(pwd)
cd ${BASE_DIR}/device/include
ln -s ${SAWMAN_DIR} sawman

cd ../../apps/make

echo
echo
echo finish to unpack ${PACKAGE_FILE}

 
