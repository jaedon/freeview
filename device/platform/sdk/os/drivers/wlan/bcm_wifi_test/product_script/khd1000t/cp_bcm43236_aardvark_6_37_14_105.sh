#!/bin/sh
export PRODUCT_ID=bcm43236
export C_DIR=$PWD
export WLAN_DRIVER_DIR=$PWD/../../../$PRODUCT_ID
export WIFI_TEST_DIR=$PWD/../../

if [ -e $WLAN_DRIVER_DIR/mfg/3.3.8/wl.ko ]; then
	export WLAN_DRIVER_DIR=$PWD/../../../$PRODUCT_ID
	export WIFI_TEST_DIR=$PWD/../../
else
	export WLAN_DRIVER_DIR=$PWD/../$PRODUCT_ID
	export WIFI_TEST_DIR=$PWD
fi

echo "Copy WiFi DBUS Driver..."
cp -f $WLAN_DRIVER_DIR/mfg/3.3.8/wl.ko $WIFI_TEST_DIR/driver/wl_mfg.ko
cp -f $WLAN_DRIVER_DIR/production/3.3.8/wl.ko $WIFI_TEST_DIR/driver/wl_prd.ko

echo "Copy WiFi Driver..."
cp -f $WLAN_DRIVER_DIR/mfg/3.3.8/bcm_dbus.ko $WIFI_TEST_DIR/driver/bcm_dbus_mfg.ko
cp -f $WLAN_DRIVER_DIR/production/3.3.8/bcm_dbus.ko $WIFI_TEST_DIR/driver/bcm_dbus_prd.ko

echo "Copy WiFi Firmware..."
cp -f $WLAN_DRIVER_DIR/mfg/src/dongle/rte/wl/builds/43236b-bmac/ag-assert-mfgtest/rtecdc.bin.trx $WIFI_TEST_DIR/firmware/mfg/rtecdc.bin.trx
cp -f $WLAN_DRIVER_DIR/production/src/dongle/rte/wl/builds/43236b-bmac/ag-p2p-mchan-media/rtecdc.bin.trx $WIFI_TEST_DIR/firmware/prd/rtecdc.bin.trx

echo "Copy WiFi NVRAM..."
cp -f $WLAN_DRIVER_DIR/production/nvrams/khd1000t/khd1000t.nvm $WIFI_TEST_DIR/nvrams/

echo "Copy wl binary..."
cp -f $WLAN_DRIVER_DIR/mfg/3.3.8/wl $WIFI_TEST_DIR/bin/

