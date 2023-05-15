#!/bin/sh
#version :  v1.0.3 2012-09-13
#
# Installs the WiFi drivers
# - If the driver is already installed, it returns OK
# - If the manufacturer firmware is available, it uses it. Otherwise it uses the distribution firmware
#
# Inputs:  none
# Outputs: 
# - 0: success
# - 1: failed to download firmware
# - 2: failed to install driver
#


#Make sure USB filesystem is mounted because it's required to download the firmware

if ! mount | grep $USB_FS &> /dev/null; then
	mount -tusbfs usbfs /proc/bus/usb &> /dev/null
	echo "Mount usbfs"   
fi

if ! lsmod | grep $WIFI_DRV_NAME &> /dev/null; then
	if [  $WIFI_TYPE = "BCM43236B_USB" ]; then
		if [ $1 = "prd" ]; then
			WIFI_USE_FIRMWARE=$WIFI_PRD_FIRMWARE_FILENAME                                                                                                
			WIFI_USE_DRV=$WIFI_PRD_DRV                                                             
			WIFI_USE_NVRAM=$WIFI_PRD_NVRAM_FILENAME                                                          
    	fi

		if [ $1 = "mfg" ]; then
			WIFI_USE_FIRMWARE=$WIFI_MFG_FIRMWARE_FILENAME                                                                                                
			WIFI_USE_DRV=$WIFI_MFG_DRV                                                             
			WIFI_USE_NVRAM=$WIFI_MFG_NVRAM_FILENAME                                                          
    	fi
    	
    	echo "Loading firmware"
		echo $WIFI_USE_FIRMWARE  
		echo $WIFI_USE_NVRAM                                                                      
		# Download firmware to WiFi dongle
		$WIFI_DOWNLOAD_TOOL -n $WIFI_USE_NVRAM $WIFI_USE_FIRMWARE
		if [ $? -ne 0 ]; then
			echo "Failed downloading firmware"
			exit $WIFI_RET_ERR_FIRMWARE
		fi
	fi
	
	if [ $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
		if [ $1 = "prd" ]; then
			WIFI_USE_FIRMWARE=$WIFI_PRD_FIRMWARE_FILENAME                                                                                                
			WIFI_USE_DRV=$WIFI_PRD_DRV                                                             
			WIFI_USE_DBUS_DRV=$WIFI_DBUS_PRD_DRV
			WIFI_USE_NVRAM=$WIFI_PRD_NVRAM_FILENAME                                                          
   	fi

		if [ $1 = "mfg" ]; then
			WIFI_USE_FIRMWARE=$WIFI_MFG_FIRMWARE_FILENAME                                                                                                
			WIFI_USE_DRV=$WIFI_MFG_DRV                                                             
			WIFI_USE_DBUS_DRV=$WIFI_DBUS_MFG_DRV                                                   
			WIFI_USE_NVRAM=$WIFI_MFG_NVRAM_FILENAME                                                          
   	fi
    	
   	echo "Loading firmware"
		echo $WIFI_USE_FIRMWARE  
		echo $WIFI_USE_NVRAM                                                                      
		# Download firmware to WiFi dongle
		$WIFI_DOWNLOAD_TOOL -n $WIFI_USE_NVRAM $WIFI_USE_FIRMWARE
		if [ $? -ne 0 ]; then
			echo "Failed downloading firmware"
			exit $WIFI_RET_ERR_FIRMWARE
		fi
	fi
	
	if [  $WIFI_TYPE = "BCM43228_PCIe" ]; then
		if [ $1 = "prd" ]; then                                                                                              
			WIFI_USE_DRV=$WIFI_PRD_DRV                                                             
    	fi

		if [ $1 = "mfg" ]; then                                                      
			WIFI_USE_DRV=$WIFI_MFG_DRV                                                             
    	fi
	fi

	# Load the driver modules
	if [ $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
		echo "Loading DBUS driver"  
		insmod $WIFI_USE_DBUS_DRV
		sleep 1
	fi

	echo "Loading WiFi drivers"  
	insmod $WIFI_USE_DRV
	sleep 2
	if ! lsmod | grep $WIFI_DRV_NAME &> /dev/null; then
	  echo "Failed to install driver"
	  #exit $WIFI_RET_ERR_INSMOD
	fi
	$WL_DUT country
	echo "WIFI NVRAM:    $WIFI_USE_NVRAM"
	echo "WIFI FIRMWARE: $WIFI_USE_FIRMWARE"
	if [ $WIFI_TYPE = "BCM43236B_USB_AARDVARK" ]; then
		echo "WIFI DBUS DRIVER: $WIFI_USE_DBUS_DRV"
	fi
	echo "WIFI DRIVER:   $WIFI_USE_DRV"
	echo "WiFi driver installed successfully"
else
	echo "WiFi drivers already installed"
fi

exit $WIFI_RET_OK
