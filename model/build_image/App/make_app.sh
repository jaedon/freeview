#!/bin/sh
#
# Copyright (C) 2014 HUMAX Co., Ltd. All rights reserved.
#

echo "Copyright (C) 2014 HUMAX Co., Ltd. All rights reserved."
echo "Command : make_app.sh"

if [ $# -eq 0 ];
then
	echo "Error! make_app Usage : make_app.sh [target_project]"
fi

if [ $# -eq 1 ];
then
	echo "[Target Project : $1]"
	PRODUCT_NAME=`echo $1`
	USER=$(/usr/bin/whoami)
	BUILD_IMAGE_PATH=../
	APP_IMAGE_PATH=$(pwd)
        TOOL_PATH=$BUILD_IMAGE_PATH/Util/linux
        HUMAX_MAKEHDF_DIR=$TOOL_PATH/makehdf
        HUMAX_MAKEHDF=$HUMAX_MAKEHDF_DIR/makehdf
	INPUT_IMAGE_PATH=$BUILD_IMAGE_PATH/Image
	NFS_IMAGE_PATH=/nfsroot/$USER/$PRODUCT_NAME/image
	OUTPUT_IMAGE_PATH=$APP_IMAGE_PATH/output
	OUTPUT_FILE_NAME="$PRODUCT_NAME"_upgrade.hdf
	APP_CFG=app.cfg

	echo Start Application HDF Image

	echo Clean Binary
	$APP_IMAGE_PATH/clean.sh
	rm -rf $OUTPUT_IMAGE_PATH
	
	echo Make Output Directory
	mkdir $OUTPUT_IMAGE_PATH

	echo Step 0. Copy Application Images
	cp $INPUT_IMAGE_PATH/aes_key.bin $INPUT_IMAGE_PATH/rsa_private_key_2048.bin $INPUT_IMAGE_PATH/rsa_public_key_2048.bin $APP_IMAGE_PATH
	cp $NFS_IMAGE_PATH/root.squa $NFS_IMAGE_PATH/vmlinuz $NFS_IMAGE_PATH/APP_VERSION.bin $APP_IMAGE_PATH

	echo Step 1. Make Application HDF file
	$HUMAX_MAKEHDF $APP_CFG $OUTPUT_IMAGE_PATH/$OUTPUT_FILE_NAME

	echo [$OUTPUT_IMAGE_DIR/$OUTPUT_FILE_NAME] Created!!

	echo Clean Binary
	$APP_IMAGE_PATH/clean.sh

	echo Finish Application HDF Image
fi

