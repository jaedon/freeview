
#################### Start of Change env by models #################
#Select one WIFI type.
#export WIFI_TYPE=BCM43236B_USB
#export WIFI_TYPE=BCM43228_PCIe
export WIFI_TYPE=BCM43236B_USB_AARDVARK

# Driver name.
if [  $WIFI_TYPE = "BCM43236B_USB" ]; then
export WIFI_DRV_NAME=dhd
export NVRAM=hdr1010s
fi

if [  $WIFI_TYPE = "BCM43228_PCIe" ]; then
export WIFI_DRV_NAME=wl
fi

if [  $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
export WIFI_DBUS_NAME=bcm_dbus
export WIFI_DRV_NAME=wl
export NVRAM=hdr1000sph2
fi

#################### End of Change env by models #################


#################### Start of common env #################
# Set MACROs
export WIFI_DIR=/tmp/bcm_wifi_test
export WL_DUT=${WIFI_DIR}/bin/wl
export IPERF=${WIFI_DIR}/bin/iperf
export INSTALL=${WIFI_DIR}/install.sh

export WIFI_CUR_DRIVER_TXT=${WIFI_DIR}/WIFI_CUR_DRIVER.txt

export WIFI_MFG_DRV=${WIFI_DIR}/driver/${WIFI_DRV_NAME}_mfg.ko
export WIFI_PRD_DRV=${WIFI_DIR}/driver/${WIFI_DRV_NAME}_prd.ko
if [  $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
export WIFI_DBUS_MFG_DRV=${WIFI_DIR}/driver/${WIFI_DBUS_NAME}_mfg.ko
export WIFI_DBUS_PRD_DRV=${WIFI_DIR}/driver/${WIFI_DBUS_NAME}_prd.ko
fi

if [  $WIFI_TYPE = "BCM43236B_USB" ]; then
export WIFI_DOWNLOAD_TOOL=${WIFI_DIR}/bin/bcmdl
export WIFI_MFG_FIRMWARE_FILENAME=${WIFI_DIR}/firmware/mfg/rtecdc.bin.trx
export WIFI_PRD_FIRMWARE_FILENAME=${WIFI_DIR}/firmware/prd/rtecdc.bin.trx
export WIFI_MFG_NVRAM_FILENAME=${WIFI_DIR}/nvrams/${NVRAM}.nvm
export WIFI_PRD_NVRAM_FILENAME=${WIFI_DIR}/nvrams/${NVRAM}.nvm
fi

if [ $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
export WIFI_DOWNLOAD_TOOL=${WIFI_DIR}/bin/bcmdl
export WIFI_MFG_FIRMWARE_FILENAME=${WIFI_DIR}/firmware/mfg/rtecdc.bin.trx
export WIFI_PRD_FIRMWARE_FILENAME=${WIFI_DIR}/firmware/prd/rtecdc.bin.trx
export WIFI_MFG_NVRAM_FILENAME=${WIFI_DIR}/nvrams/${NVRAM}.nvm
export WIFI_PRD_NVRAM_FILENAME=${WIFI_DIR}/nvrams/${NVRAM}.nvm
fi 

export IFNAME=wlan0
export USB_FS="usbfs"
export SCAN_TIMEOUT=8
export JOIN_TIMEOUT=8

export WIFI_COUNTRY_CODE=GB
export WIFI_COUNTRY_CODE_ALL=ALL

export WIFI_RET_OK=0
export WIFI_RET_ERR_FIRMWARE=1
export WIFI_RET_ERR_INSMOD=2

#################### End of common env #################

