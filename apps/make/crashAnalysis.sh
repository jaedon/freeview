#!/bin/bash

MODEL=`cat .config | grep CONFIG_PRODUCT_NAME | sed -e 's/CONFIG_PRODUCT_NAME//' -e 's/[=\"]//g'`
TOOLCHAIN_PATH=`cat .config | grep CONFIG_TOOLCHAIN_PATH | sed -e 's/CONFIG_TOOLCHAIN_PATH//' -e 's/[=\"]//g'`
CPU_BRCM_ARM=`cat .config | grep CONFIG_PROD_CPU_BCM_ARM | sed -e 's/CONFIG_PROD_CPU_BCM_ARM//' -e 's/[=\"]//g'`
STB_PLATFORM='mips'
GNU_TYPE='uclibc-'
USER_NAME=${USER}

if [ -z $USER ]; then
  $USER_NAME=${USERNAME}
fi

if [ ${CPU_BRCM_ARM} = 'y' ]; then
	STB_PLATFORM='arm'
	GNU_TYPE=''
fi

echo "MODEL=${MODEL}"
NFS_ROOT=/nfsroot/${USER_NAME}/${MODEL}/root/
sudo chmod 777 ${NFS_ROOT}core*

CoreLists=$(echo ${NFS_ROOT}core*)
echo $CoreLists
for i in $CoreLists
do
	echo "==========================================="
	echo $i
	echo "==========================================="
	FILE=`echo $i | sed s@$NFS_ROOT@@`
	PROCESS=`echo ${FILE} | sed -e 's/^core\.//' -e 's/\..*$//'`
	echo $PROCESS
	${TOOLCHAIN_PATH}/bin/${STB_PLATFORM}-linux-${GNU_TYPE}gdb /usr/bin/${STB_PLATFORM}-gdb ${NFS_ROOT}home/${PROCESS}/${PROCESS} ${NFS_ROOT}${FILE} --ex "set solib-absolute-prefix ${NFS_ROOT}" -ex "set solib-search-path ${NFS_ROOT}usr/lib" --ex "bt" --ex "thread apply all bt" --ex "quit"
done
