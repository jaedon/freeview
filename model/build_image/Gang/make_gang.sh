#!/bin/sh
#
# Copyright (C) 2014 HUMAX Co., Ltd. All rights reserved.
#

echo "Copyright (C) 2014 HUMAX Co., Ltd. All rights reserved."
echo "Command : make_gang.sh"

if [ $# -eq 0 ];
then
	echo "Error! make_gang Usage : make_gang.sh [target_project]"
fi

if [ $# -eq 1 ];
then
	echo "[Target Project : $1]"
	PRODUCT_NAME=`echo $1`
	USER=$(/usr/bin/whoami)
	BUILD_IMAGE_PATH=../
	GANG_IMAGE_PATH=$(pwd)
	GANG_NAND_IMAGE_PATH=$GANG_IMAGE_PATH/gang_img_gen
	TOOL_PATH=$BUILD_IMAGE_PATH/Util/linux
	HUMAX_MAKEHDF_DIR=$TOOL_PATH/makehdf
	HUMAX_MAKEHDF=$HUMAX_MAKEHDF_DIR/makehdf
	HUMAX_MG_DIR=$TOOL_PATH/mg
        HUMAX_MG=$HUMAX_MG_DIR/mg
	HUMAX_MAKENVRAM_DIR=$TOOL_PATH/makenvram
	HUMAX_MAKENVRAM=$HUMAX_MAKENVRAM_DIR/makenvram
	HUMAX_CRC_DIR=$TOOL_PATH/crc
	HUMAX_CRC32=$HUMAX_CRC_DIR/crc32
	INPUT_IMAGE_PATH=$BUILD_IMAGE_PATH/Image
	NFS_IMAGE_PATH=/nfsroot/$USER/fvp4000t/image
	OUTPUT_IMAGE_PATH=$GANG_IMAGE_PATH/output
	OUTPUT_FILE_NAME="$PRODUCT_NAME"_gang.hdf
	OUTPUT_GANG_BIN_NANE="$PRODUCT_NAME"_gang.bin
	OUTPUT_GANG_LOADER_BIN_NAME="$PRODUCT_NAME"_gang_factory_loader.bin
	GANG_CFG=gang_"$PRODUCT_NAME".cfg
	SIGN_HDF_CFG=enc_sign_kernel_N_rootfs.cfg
	SING_HDF_NAME="$PRODUCT_NAME"_enc_N_sign_tmp.hdf

	echo Start Gang Images

	echo Clean Binary
	$GANG_IMAGE_PATH/clean.sh
	rm -rf $OUTPUT_IMAGE_PATH
	
	echo Make Output Directory
	mkdir $OUTPUT_IMAGE_PATH

	echo STEP 0. Copy Configuration Files from binary directory
	cp $NFS_IMAGE_PATH/* $INPUT_IMAGE_PATH
	cp $INPUT_IMAGE_PATH/nvram.bin.zip $INPUT_IMAGE_PATH/db.bin.zip $INPUT_IMAGE_PATH/dbbackup.bin.zip $GANG_IMAGE_PATH
	cp $INPUT_IMAGE_PATH/dbuser.bin.zip $INPUT_IMAGE_PATH/crcprefx.bin $INPUT_IMAGE_PATH/ff.bin $GANG_IMAGE_PATH
	cp $INPUT_IMAGE_PATH/aes_key.bin $INPUT_IMAGE_PATH/rsa_private_key_2048.bin $INPUT_IMAGE_PATH/rsa_public_key_2048.bin $GANG_IMAGE_PATH
	cp $INPUT_IMAGE_PATH/loader.bin $INPUT_IMAGE_PATH/loader_d.bin $INPUT_IMAGE_PATH/loader_f_$PRODUCT_NAME.bin $GANG_IMAGE_PATH
        cp $INPUT_IMAGE_PATH/user_bg.jpg $INPUT_IMAGE_PATH/LOADER_VERSION.bin $GANG_IMAGE_PATH
	cp $INPUT_IMAGE_PATH/root.squa $INPUT_IMAGE_PATH/vmlinuz $INPUT_IMAGE_PATH/APP_VERSION.bin $GANG_IMAGE_PATH

	echo STEP 1. Unzip DB and NVRAM files
	unzip -o db.bin.zip
	unzip -o dbbackup.bin.zip
	unzip -o dbuser.bin.zip

	echo STEP 2. Create Singed/Encrypted Kernel/Rootfs Binary.
	$HUMAX_MAKEHDF $SIGN_HDF_CFG $SING_HDF_NAME

	echo STEP 3. Make UBI Binary
	$HUMAX_MG ubi.lst UBI_CODE ubi_code.bin
	$HUMAX_MG ubi.lst UBI_RAWFS ubi_rawfs.bin

	echo STEP 4. Make CRC of UBI Code Area 
	$HUMAX_CRC32 ubi_code.bin ubi_code.crc

	echo STEP 5. Make NVRAM Binary
	$HUMAX_MAKENVRAM ronvram_"$PRODUCT_NAME".cfg ronvram.bin
	$HUMAX_MAKENVRAM rwnvram.cfg rwnvram.bin
	$HUMAX_MG ubi.lst MG_NVRAM ubi_nvram_mg.bin

	echo STEP 6. Merge UBI Area
	$HUMAX_MG ubi.lst RF_K_NVR $OUTPUT_IMAGE_PATH/$OUTPUT_GANG_BIN_NANE

	echo STEP 7. Make GANG HDF
	$HUMAX_MAKEHDF $GANG_CFG $OUTPUT_IMAGE_PATH/$OUTPUT_FILE_NAME
	cp loader_f_$PRODUCT_NAME.bin $OUTPUT_IMAGE_PATH/$OUTPUT_GANG_LOADER_BIN_NAME

	echo [$GANG_CFG $OUTPUT_IMAGE_PATH/$OUTPUT_FILE_NAME] Created!!

	echo Clean Binary
	$GANG_IMAGE_PATH/clean.sh

	echo STEP 8. Make ubi/ubifs image
	cd $GANG_NAND_IMAGE_PATH
	sh run.sh "$PRODUCT_NAME"
	cd ../
	
	echo STEP 9. Copy ubi/ubifs Image
	cp $GANG_NAND_IMAGE_PATH/*.img $OUTPUT_IMAGE_PATH

	echo STEP 10. Copy csv/tgz Image
	cp $GANG_NAND_IMAGE_PATH/*.csv $GANG_NAND_IMAGE_PATH/*.tgz $OUTPUT_IMAGE_PATH

	echo Finish Gang Images
fi

