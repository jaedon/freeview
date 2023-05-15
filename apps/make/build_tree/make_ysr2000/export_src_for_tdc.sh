#!/bin/bash

USER=$(echo $(whoami) | sed "s/[ ]//g")
MAKE_DIR=$(pwd)/../..
APPS_DIR=$MAKE_DIR/..
TOP_DIR=$MAKE_DIR/../..

cd $MAKE_DIR

echo "============= remove unused files and directories ================"
# 필요없는 파일 삭제
if [[ -e Makefile.security_hd9000iru ]] ; then rm Makefile.security_hd9000iru; fi
if [[ -e Makefile.security_icordpro ]] ; then rm Makefile.security_icordpro; fi
if [[ -e HowToMakeCoreInReleaseMode.txt ]] ; then rm HowToMakeCoreInReleaseMode.txt; fi
if [[ -e test_current_world.sh ]] ; then rm test_current_world.sh; fi
if [[ -e test_next_world.sh  ]] ; then rm test_next_world.sh; fi
if [[ -e Makefile.app_web_for_nodejs  ]] ; then rm Makefile.app_web_for_nodejs; fi
if [[ -e Makefile.octopus  ]] ; then rm Makefile.octopus; fi
if [[ -e Makefile.app_node  ]] ; then rm Makefile.app_node; fi
if [[ -e appl_make/bml  ]] ; then rm -rf appl_make/bml; fi
if [[ -e $APPS_DIR/appl/bml  ]] ; then rm -rf $APPS_DIR/appl/bml; fi
if [[ -e appl_make/aribcc  ]] ; then rm -rf appl_make/aribcc; fi
if [[ -e $APPS_DIR/appl/aribcc  ]] ; then rm -rf $APPS_DIR/appl/aribcc; fi
if [[ -e appl_make/android  ]] ; then rm -rf appl_make/android; fi
if [[ -e $APPS_DIR/appl/android  ]] ; then rm -rf $APPS_DIR/appl/android; fi
if [[ -e appl_make/bsautomsg  ]] ; then rm -rf appl_make/bsautomsg; fi
if [[ -e $APPS_DIR/appl/bsautomsg  ]] ; then rm -rf $APPS_DIR/appl/bsautomsg; fi
if [[ -e appl_make/ipepg  ]] ; then rm -rf appl_make/ipepg; fi
if [[ -e $APPS_DIR/appl/ipepg  ]] ; then rm -rf $APPS_DIR/appl/ipepg; fi
if [[ -e appl_make/irldvt  ]] ; then rm -rf appl_make/irldvt; fi
if [[ -e $APPS_DIR/appl/irldvt  ]] ; then rm -rf $APPS_DIR/appl/irldvt; fi
if [[ -e appl_make/mheg  ]] ; then rm -rf appl_make/mheg; fi
if [[ -e $APPS_DIR/appl/mheg  ]] ; then rm -rf $APPS_DIR/appl/mheg; fi
if [[ -e appl_make/mhegib  ]] ; then rm -rf appl_make/mhegib; fi
if [[ -e $APPS_DIR/appl/mhegib  ]] ; then rm -rf $APPS_DIR/appl/mhegib; fi
if [[ -e appl_make/nodebinder  ]] ; then rm -rf appl_make/nodebinder; fi
if [[ -e $APPS_DIR/appl/nodebinder  ]] ; then rm -rf $APPS_DIR/appl/nodebinder; fi
if [[ -e appl_make/octopus  ]] ; then rm -rf appl_make/octopus; fi
if [[ -e $APPS_DIR/appl/octopus  ]] ; then rm -rf $APPS_DIR/appl/octopus; fi
if [[ -e appl_make/remoteapp  ]] ; then rm -rf appl_make/remoteapp; fi
if [[ -e $APPS_DIR/appl/remoteapp  ]] ; then rm -rf $APPS_DIR/appl/remoteapp; fi
if [[ -e appl_make/cloudsync  ]] ; then rm -rf appl_make/cloudsync; fi
if [[ -e $APPS_DIR/appl/cloudsync ]] ; then rm -rf $APPS_DIR/appl/cloudsync; fi
if [[ -e appl_make/restnode  ]] ; then rm -rf appl_make/restnode; fi
if [[ -e $APPS_DIR/appl/restnode  ]] ; then rm -rf $APPS_DIR/appl/restnode; fi
if [[ -e appl_make/rfvod  ]] ; then rm -rf appl_make/rfvod; fi
if [[ -e $APPS_DIR/appl/rfvod  ]] ; then rm -rf $APPS_DIR/appl/rfvod; fi
if [[ -e appl_make/sotalupgrade  ]] ; then rm -rf appl_make/sotalupgrade; fi
if [[ -e $APPS_DIR/appl/sotalupgrade  ]] ; then rm -rf $APPS_DIR/appl/sotalupgrade; fi
if [[ -e appl_make/wind3  ]] ; then rm -rf appl_make/wind3; fi
if [[ -e $APPS_DIR/appl/wind3  ]] ; then rm -rf $APPS_DIR/appl/wind3; fi
if [[ -e $APPS_DIR/appl/node  ]] ; then rm -rf $APPS_DIR/appl/node; fi
if [[ -e $APPS_DIR/appl/webappl/base_prism  ]] ; then rm -rf  $APPS_DIR/appl/webappl/base_prism; fi
if [[ -e $APPS_DIR/appl/webappl/doc  ]] ; then rm -rf  $APPS_DIR/appl/webappl/doc; fi
if [[ -e $APPS_DIR/octo/tools/fsat  ]] ; then rm -rf  $APPS_DIR/octo/tools/fsat; fi
if [[ -e $TOP_DIR/3rd_party/cas ]] ; then rm -rf  $TOP_DIR/3rd_party/cas; fi
if [[ -e $TOP_DIR/device/apps/factory ]] ; then rm -rf  $TOP_DIR/device/apps/factory; fi
if [[ -e $TOP_DIR/device/apps/hwtest ]] ; then rm -rf  $TOP_DIR/device/apps/hwtest; fi
if [[ -e $TOP_DIR/device/apps/hwtest_multi ]] ; then rm -rf  $TOP_DIR/device/apps/hwtest_multi; fi
for d in unpack_device*; do
	rm -f $d
done
for d in $APPS_DIR/appl/prism/image/startlogo_*; do
    if [[ "$d" == *"yousee.gif" ]] ; then continue; fi
    rm -rf $d
done
for d in $APPS_DIR/octo/service/umma/main/module/fs/profile/*; do
    if [[ "$d" == *"ysr2000_storage_profile" ]] ; then continue; fi
    rm -rf $d
done
if [[ -e $TOP_DIR/model/down ]] ; then rm -rf  $TOP_DIR/model/down; fi
if [[ -e $TOP_DIR/model/tool/si_parser ]] ; then rm -rf  $TOP_DIR/model/tool/si_parser; fi

# 다른 project의 rootfs구성은 삭제한다.
echo "============= remove files of other projects ================"
for d in ./rootfs/product/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in ./rootfs/security/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done

# 다른 project의 configuration은 삭제한다.
for d in ./product_configs/*; do
    if [[ "$d" == *"ysr2000"* ]] ; then continue; fi
    rm -rf $d
done
for d in $APPS_DIR/octo/library/dlib/include/config/*; do
    if [[ "$d" == *"base.h" ]] ; then continue; fi
    if [[ "$d" == *"ysr2000.h" ]] ; then continue; fi
    rm -rf $d
done
for d in ./build_tree/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/apps/factory/config/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/platform/sdk/os/rootfs/base-files/product/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/platform/sdk/os/rootfs/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/platform/sdk/os/kernel/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/platform/sdk/driver/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/platform/driver/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/flashmap/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/defconfigs/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/rammap/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/package/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    if [[ "$d" == *"kconfigs" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/product_config/defconfigs/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    if [[ "$d" == *"kconfigs" ]] ; then continue; fi
    rm -rf $d
done
for d in $TOP_DIR/device/images/*; do
    if [[ "$d" == *"ysr2000" ]] ; then continue; fi
    rm -rf $d
done
echo "============= Make World  ================"
# obama 관련 library 및 excute file을 뽑아내기 위해 build 수행
#cp ./build_tree/make_ysr2000/Makefile.prepare ./Makefile
#make ysr2000_yousee_defconfig
#make world

echo "================ Remove OCTO and library source code ========================"
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile $MAKE_DIR/Makefile
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.cas $MAKE_DIR/Makefile.cas
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.extract $MAKE_DIR/Makefile.extract
cp $MAKE_DIR/build_tree/make_ysr2000/distclean_gen_rootfs.sh $MAKE_DIR/distclean_gen_rootfs.sh
cp -Rf $APPS_DIR/octo/service/obama/plugin/xtvmanager/xmgr/cas/base/resource/cas_string.bin $MAKE_DIR/
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.driver $TOP_DIR/device/make/Makefile.driver

rm -rf `ls | find ../octo/library/clib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/cutest/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/dlib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/hcrlib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/hlib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/memory/* -name memory_debug.h -prune -o -print`
rm -rf `ls | find ../octo/library/ondk/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/rlib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/library/silib/* -name include -prune -o -print`
rm -rf `ls | find ../octo/service/obama/* -name include -prune -o -print`

echo "================ Remove Device Driver Source Code ========================"
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.driver $TOP_DIR/device/make/Makefile.driver
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.image $TOP_DIR/device/make/Makefile.image
cp $MAKE_DIR/build_tree/make_ysr2000/Makefile.main $TOP_DIR/device/make/Makefile.main
sudo rm -rf $TOP_DIR/device/platform/driver/di_exec
sudo rm -rf $TOP_DIR/device/platform/driver/di_ext
sudo rm -rf $TOP_DIR/device/platform/driver/di_ics
rm -rf `ls | find $TOP_DIR/device/platform/driver/di/* -name drv -prune -o -print`
rm -rf `ls | find $TOP_DIR/device/platform/driver/di/drv/* -name include -prune -o -print`
for d in $TOP_DIR/device/platform/driver/di/drv/include/*; do
    if [[ "$d" == *"drv_err.h" ]] ; then continue; fi
    if [[ "$d" == *"drv_hotplug.h" ]] ; then continue; fi
    if [[ "$d" == *"drv_pppoe.h" ]] ; then continue; fi
    if [[ "$d" == *"drv_wifi.h" ]] ; then continue; fi
    if [[ "$d" == *"taskdef.h" ]] ; then continue; fi
    sudo rm -rf $d
done

echo "============= remove BRCM code  ================"
rm -rf $TOP_DIR/device/platform/sdk/driver
rm -rf $TOP_DIR/device/platform/sdk/AppLibs/opensource/directfb/src/DirectFB-Broadcom

#echo "================ Compress NFS Environment ========================"
#sudo tar -jcvpf $TOP_DIR/ysr2000_NFS_$(date '+%Y-%m-%d').tar.bz2 /nfsroot/$USER/ysr2000/*

#echo "================ DISTCLEAN ========================"
#make distclean

#echo "================ COMPRESS SOURCE and BUILD ENVIRONMENT ========================"
#cd $TOP_DIR
#sudo tar --exclude-vcs --exclude=make_ysr2000 -cpf ysr2000_build_package.$(date '+%Y-%m-%d').tar 3rd_party apps device include packages

#rm -rf $MAKE_DIR/build_tree
