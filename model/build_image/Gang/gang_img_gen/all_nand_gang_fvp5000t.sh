#!/bin/bash

# set nandflash params ###
NAND_TOTAL_SIZE=512
IO_SIZE=2048
PEB_SIZE=128KiB
LEB_SIZE=124KiB
##########################

DO_UBINIZE=./.bin/ubinize
MK_UBIFS=./.bin/mkfs.ubifs
INI_DIR=./config
ECC_TOOL_DIR=./.eccgen_tool

NAND_MAP_CFG=$1
MASTER_IMAGE_NAME=$2

ECC_TOOL=add_bch4_ecc

echo "Compile 4bit ECC tool"
gcc -m32 $ECC_TOOL_DIR/$ECC_TOOL.c -o $ECC_TOOL

if [ -f ./src_images/factory.bin ]; then
	cp ./src_images/factory.bin .
else
	echo "factory.bin not found."
	exit 1
fi
if [ -f ./src_images/fvp5000t_gang.bin ]; then
	cp ./src_images/fvp5000t_gang.bin .
else
	echo "fvp5000t_gang.bin not found."
	exit 1
fi

cat $NAND_MAP_CFG | while read IMG_NAME INI_FILE MOUNT_OFFSET CLEANUP_FLAG; do
  case "${IMG_NAME:0:1}" in
		'#')
			#echo "== REMARK ${IMG_NAME}"
			continue;
	esac
  case $INI_FILE in
		"none")
			echo "skip $IMG_NAME (no need to be ubinized)"
			continue;;
	esac	
	echo "--------- make $IMG_NAME from $INI_FILE ------" 
	$DO_UBINIZE -o $IMG_NAME -p $PEB_SIZE -m $IO_SIZE $INI_DIR/$INI_FILE

done

	echo "--------- make NAND Master Image ------" 
	./$ECC_TOOL $NAND_MAP_CFG $NAND_TOTAL_SIZE $MASTER_IMAGE_NAME.bin $MASTER_IMAGE_NAME.csv

	tar -zcf $MASTER_IMAGE_NAME.tgz $MASTER_IMAGE_NAME.bin $MASTER_IMAGE_NAME.csv
	rm $ECC_TOOL
