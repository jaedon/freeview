#!/bin/bash

## tool version display
echo "[$0]Tool to extract version information from header file. v1.0"

## error check
if [ $# != 2 ] ; then
	echo "[ERROR]parameter error. # ./binheader CONFIG_PATH/.config OCTO_PATH/version.h"
	exit 1
fi

if [ "${1#*.config}" = "$1" ] ; then
	echo "[ERROR]$1 is not .config file"
	exit 1
fi
if [ "${2#*version.h}" = "$2" ] ; then
	echo "[ERROR]$2 is not version.h"
	exit 1
fi

## find model name (CONFIG_PROD_XXX) using .config file
model_name=`grep "CONFIG_PROD_" $1 | grep "=y" | head -n 1 | awk -F '=' '{print $1}'`
if [ -z "$model_name" ] ; then
	echo "[ERROR]cannot find CONFIG_PROD_XXX in .config"
	exit 1
fi
echo $model_name

## calculate start/end line number using version.h
start_linenum=`grep -n defined\($model_name\) $2 | awk -F ':' '{print $1}'`
start_linenum=`expr $start_linenum + 1`
linenum=`sed -n $start_linenum,$$p $2 | grep -n "elif\|else\|endif" | head -n 1 | awk -F ':' '{print $1}'`
end_linenum=`expr $start_linenum + $linenum`
if [ $start_linenum -gt $end_linenum ] ; then
	echo "[ERROR]something is wrong(start line:$start_linenum end line:$end_linenum!!!"
	exit 1
fi
##echo $start_linenum
##echo $end_linenum

## SYSTEM_APP_VER_HEAD
## print lines at version.h from start line to end line using pal_version.h and find value
ver_head=`sed -n ${start_linenum},${end_linenum}p $2 | grep "SYSTEM_APP_VER_HEAD" | awk '{print $3}'`
ver_head=${ver_head#*\"}
ver_head=${ver_head%%\"}
if [ ${#ver_head} -lt 6 ] ; then
	echo "[ERROR]SYSTEM_APP_VER_HEAD is not 6 bytes. please check it out"
	exit 1
fi
echo $ver_head

## write SYSTEM_APP_VER_HEAD to temporary header file
echo $ver_head > temp_head.bin
## remove \n in temporary header file
cat temp_head.bin | tr -d "\n" > temp_head_1.bin

## SYSTEM_APP_VERSION
## print lines at version.h from start line to end line using pal_version.h and find value
ver_num=`sed -n ${start_linenum},${end_linenum}p $2 | grep "SYSTEM_APP_VERSION" | awk '{print $3}'`
ver_num=${ver_num#(*x}
ver_num=${ver_num%%(*[0-9]}
if [ ${#ver_num} -lt 6 ] ; then
	echo "[ERROR]SYSTEM_APP_VERSION is not 3 bytes.[ex:0x010101] Please check it out."
	exit 1
fi
echo $ver_num

## make binary format for converting
binary_string="00000000: $ver_num"
if [ ${#binary_string} -gt 42 ] ; then
	echo "[ERROR]string is too long. this version do support 16bytes only"
	exit 1
fi
echo $binary_string > temp_string.bin

## convert text to binary
xxd -r temp_string.bin binary_string.bin

## attach binary to temporary header file
cat binary_string.bin >> temp_head_1.bin

## make final binary using redirection
cat temp_head_1.bin > APP_VERSION.bin

## delete temporary files
rm -f temp_head.bin temp_head_1.bin temp_string.bin binary_string.bin

exit 0

