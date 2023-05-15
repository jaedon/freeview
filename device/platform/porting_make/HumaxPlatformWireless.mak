######################################################################
# Add wireless modules to build targets
######################################################################
ifeq ($(CONFIG_WIFI),y)
ifeq ($(CONFIG_NETAPP_WIFI),y)
else
WIRELESS_TARGETS += wifi
endif
endif

ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
WIRELESS_TARGETS += bt
endif
endif

######################################################################
# Wireless MACROs
######################################################################

KERNEL_WIRELESS_RELEASE=$(shell cat $(HUMAX_WORK_DIR)/platform/sdk/os/kernel/include/config/kernel.release 2> /dev/null)

ifeq ($(CONFIG_WIFI),y)
ifeq ($(CONFIG_NETAPP_WIFI),y)
else
	HUMAX_WIFI_COMMON_BUILD_PARAMS = CC=$(HUMAX_CROSS)gcc \
								LD=$(HUMAX_CROSS)ld \
								CROSS_COMPILE=$(HUMAX_CROSS) \
								LINUX_SRC=$(KERNEL_DIR) \
								TARGET_DIR=$(HUMAX_PREPARE_ROOT_DIR) \
								HUMAX_PLATFORM_BASE=y \
								HUMAX_PREPARE_ROOT_DIR=$(HUMAX_PREPARE_ROOT_DIR) \
								KERNEL_DIR=$(KERNEL_DIR) \
								KERNEL_WIRELESS_RELEASE=$(KERNEL_WIRELESS_RELEASE) \
								CONFIG_WPA_SUPPLICANT=$(CONFIG_WPA_SUPPLICANT) \
								CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
								CONFIG_SUPPORT_WPS=$(CONFIG_SUPPORT_WPS) \
								CONFIG_SUPPORT_PBC_PHYSICAL=$(CONFIG_SUPPORT_PBC_PHYSICAL) \
								CONFIG_SUPPORT_WOWL=$(CONFIG_SUPPORT_WOWL) \
								CONFIG_SUPPORT_SECDMA=$(CONFIG_SUPPORT_SECDMA) \
								CONFIG_SUPPORT_ENTERPRISE=$(CONFIG_SUPPORT_ENTERPRISE) \
								DEBUG=$(CONFIG_PRODUCT_DEVICE_DEBUG_LOG)				

	# WPA_SUPPLICANT (WIFI common)
ifeq ($(CONFIG_WPA_SUPPLICANT),y)
	WPA_SUPPLICANT_OUTPUT = $(HUMAX_WORK_DIR)/../packages/wpa_supplicant/bin/*
	WPA_SUPPLICANT_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
endif

ifeq ($(CONFIG_WIRELESS_TOOLS),y)
	WIRELESS_TOOLS_PATH = $(HUMAX_WORK_DIR)/../packages/wireless_tools
endif
	
	ifeq ($(CONFIG_RT3070),y)
		WIFI_RT3070_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/RT3070
	endif
	
	ifeq ($(CONFIG_RT3572),y)
		WIFI_RT3572_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/RT3572
	endif

	ifeq ($(CONFIG_RT5572),y)
		WIFI_RT5572_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/RT5572
		ifeq ($(CONFIG_RT5572_SUPPORT_WLAN_LED),y)
			RT5572_BUILD_PARAMS += SUPPORT_WLAN_LED=y
		endif
    ifeq ($(CONFIG_RT5572_SCAN_BY_RSSI),y)
      RT5572_BUILD_PARAMS += SUPPORT_SCAN_BY_RSSI=y
    endif
	endif

	ifeq ($(CONFIG_MT7601),y)
		WIFI_MT7601_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/mt7601u
	endif

	ifeq ($(CONFIG_RTL8188),y)
		WIFI_RTL8188_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/rtl8188eu
	endif

	ifeq ($(CONFIG_SWB_B70_WIFI),y)
		WIFI_SWB_B70_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/SWB-B70/production
	endif
	
	ifeq ($(CONFIG_SWB_B70_WIFI_MFG),y)
		WIFI_SWB_B70_MFG_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/SWB-B70/mfg
	endif
	
	ifeq ($(CONFIG_BCM43236),y)
		WIFI_BCM43236_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/bcm43236/production
		ifeq ($(CONFIG_WIFI_AARDVARK), y)
			BCM43236_BUILD_PARAMS += CONFIG_BCM43236=y
			ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
				BCM43236_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y                                                                                                                                         
				BCM43236_BUILD_PARAMS += FIRMWARE=ag-p2p-mchan-media/rtecdc.bin.trx                                                                                                                        
				BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=mipsel-mips-media-high-p2p                                                                                                                         
			else                                                                                                                                                                                               
				BCM43236_BUILD_PARAMS += FIRMWARE=ag-nodis-p2p-mchan-media/rtecdc.bin.trx                                                                                                                  
				BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=mipsel-mips-media-reqfw-high-p2p                                                                                                                   
			endif 
		else
			ifeq ($(CONFIG_BCM43236_SUPPORT_ADAPTIVITY),y)
				ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
					BCM43236_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
					BCM43236_BUILD_PARAMS += FIRMWARE=ag/rtecdc_adaptivity.bin.trx
					BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-gpl
				else
					BCM43236_BUILD_PARAMS += FIRMWARE=ag/rtecdc_adaptivity.bin.trx
					BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-reqfw-gpl
				endif
			else
				ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
					BCM43236_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
					BCM43236_BUILD_PARAMS += FIRMWARE=ag/rtecdc.bin.trx
					BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-gpl
				else
					BCM43236_BUILD_PARAMS += FIRMWARE=ag-nodis/rtecdc.bin.trx
					BCM43236_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-reqfw-gpl
				endif
			endif
		endif
	endif
	
	ifeq ($(CONFIG_BCM43236_MFG),y)
		WIFI_BCM43236_MFG_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/bcm43236/mfg
		ifeq ($(CONFIG_WIFI_AARDVARK), y)
			BCM43236_MFG_BUILD_PARAMS += CONFIG_BCM43236_MFG=y
			ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
				BCM43236_MFG_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
				BCM43236_MFG_BUILD_PARAMS += FIRMWARE=ag-assert-mfgtest/rtecdc.bin.trx
				BCM43236_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=mipsel-mips-media-high-p2p
			else
				BCM43236_MFG_BUILD_PARAMS += FIRMWARE=ag-nodis-mfgtest/rtecdc.bin.trx
				BCM43236_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=mipsel-mips-media-reqfw-high-p2p
			endif
		else
			ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
				BCM43236_MFG_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
				BCM43236_MFG_BUILD_PARAMS += FIRMWARE=ag-mfgtest-seqcmds/rtecdc.bin.trx
				BCM43236_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-gpl
			else
				BCM43236_MFG_BUILD_PARAMS += FIRMWARE=ag-nodis-mfgtest-seqcmds/rtecdc.bin.trx
				BCM43236_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-cdc-usb-reqfw-gpl
			endif
		endif
	endif

        ifeq ($(CONFIG_BCM43567),y)
		WIFI_BCM43567_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/bcm43567/production
		BCM43567_BUILD_PARAMS += CONFIG_BCM43567=y
		ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
			BCM43567_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
		endif
		ifeq ($(CONFIG_SUPPORT_SECDMA),y)
			ifeq ($(CONFIG_PRODUCT_DEVICE_DEBUG_LOG),y)
				BCM43567_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-secdma-armv7l-debug
			else
				BCM43567_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-secdma-armv7l
			endif
		else
			ifeq ($(CONFIG_PRODUCT_DEVICE_DEBUG_LOG),y)
				BCM43567_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-armv7l-debug
			else
				BCM43567_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-armv7l
			endif
		endif
		BCM43567_BUILD_PARAMS += FIRMWARE=pcie-ag-pktctx-splitrx-amsdutx-txbf-p2p-mchan-idauth-idsup-tdls-mfp-sr-proptxstatus-pktfilter-wowlpf-ampduhostreorder-keepalive-wfds.bin
	endif
        
        ifeq ($(CONFIG_BCM43567_MFG),y)
		WIFI_BCM43567_MFG_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/wlan/bcm43567/mfg
		BCM43567_MFG_BUILD_PARAMS += CONFIG_BCM43567_MFG=y
		ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
			BCM43567_MFG_BUILD_PARAMS += CONFIG_WIFI_SCRIPT_TEST=y
		endif
		ifeq ($(CONFIG_SUPPORT_SECDMA),y)
			BCM43567_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-secdma-armv7l
		else
			BCM43567_MFG_BUILD_PARAMS += HUMAX_BUILDCFG=dhd-msgbuf-pciefd-mfp-armv7l
		endif
		BCM43567_MFG_BUILD_PARAMS += FIRMWARE=pcie-ag-splitrx-txbf-p2p-mfgtest-seqcmds-phydbg.bin
	endif
endif # END of ifeq ($(CONFIG_NETAPP_WIFI),y)
endif # END of ifeq ($(CONFIG_WIFI),y)

ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
ifeq ($(CONFIG_SWB_B70_BT),y)
	HUMAX_BUILD_BT_PARAMS = CPU=mips \
							CC=$(HUMAX_CROSS)gcc \
							LD=$(HUMAX_CROSS)ld \
							CROSS_COMPILE=$(HUMAX_CROSS) \
							MIPSGCC=$(HUMAX_CROSS)gcc \
							MIPSCCFLAGS=-DHUMAX_PLATFORM_BASE \
							HUMAX_PLATFORM_BASE=y \
							HUMAX_PREPARE_ROOT_DIR=$(HUMAX_PREPARE_ROOT_DIR) \
							KERNEL_DIR=$(KERNEL_DIR) \
							KERNEL_WIRELESS_RELEASE=$(KERNEL_WIRELESS_RELEASE) \
							HUMAX_LIB_DIR=$(HUMAX_OUTPUT_DIR) \
							CONFIG_PRODUCT_APP_NAME=$(CONFIG_PRODUCT_APP_NAME) \
							MIPSGCCLIBPATH=$(HUMAX_UCLIBC_OLD_DIR)/lib \
							ENABLE_BTHID=TRUE \
							BSASHAREDLIB=TRUE \
							KDIR=$(KERNEL_DIR) \
	
		BT_DRIVER_USB_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/brcm/linux/btusb
		BT_DRIVER_HID_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/brcm/linux/bthid
		BT_LIB_BSA_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/libbsa/build
		BT_SERVER_BSA_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/server/build
		BT_INST_MODULES_DIR = ${HUMAX_PREPARE_ROOT_DIR}/lib/modules/${KERNEL_WIRELESS_RELEASE}/kernel/drivers/bluetooth/swb-b70

		ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
			BT_CLIENT_MANAGER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/app_manager/build
			BT_CLIENT_AV_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/app_av/build
			BT_CLIENT_HID_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/app_hh/build
			BT_CLIENT_DG_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/app_dg/build
			BT_CLIENT_TM_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/app_tm/build
		endif
else
	HUMAX_BUILD_BT_PARAMS = CC=$(HUMAX_CROSS)gcc \
							LD=$(HUMAX_CROSS)ld \
							CROSS_COMPILE=$(HUMAX_CROSS) \
							HUMAX_PLATFORM_BASE=y \
							HUMAX_PREPARE_ROOT_DIR=$(HUMAX_PREPARE_ROOT_DIR) \
							KERNEL_DIR=$(KERNEL_DIR) \
							KERNEL_WIRELESS_RELEASE=$(KERNEL_WIRELESS_RELEASE) \
							HUMAX_LIB_DIR=$(HUMAX_OUTPUT_DIR) \
							CONFIG_PRODUCT_APP_NAME=$(CONFIG_PRODUCT_APP_NAME) \
							ENABLE_BTHID=TRUE \
							BSASHAREDLIB=TRUE \
							KDIR=$(KERNEL_DIR)
	ifeq ($(CONFIG_ARM), y)
		HUMAX_BUILD_BT_PARAMS += CPU=arm
		HUMAX_BUILD_BT_PARAMS += ARMGCC=$(HUMAX_CROSS)gcc
		HUMAX_BUILD_BT_PARAMS += ARMCCFLAGS=-DHUMAX_PLATFORM_BASE
		HUMAX_BUILD_BT_PARAMS += ARMGCCLIBPATH=$(HUMAX_UCLIBC_OLD_DIR)/lib
		BT_CPU = arm
	else
		HUMAX_BUILD_BT_PARAMS += CPU=mips
		HUMAX_BUILD_BT_PARAMS += MIPSGCC=$(HUMAX_CROSS)gcc
		HUMAX_BUILD_BT_PARAMS += MIPSCCFLAGS=-DHUMAX_PLATFORM_BASE
		HUMAX_BUILD_BT_PARAMS += MIPSGCCLIBPATH=$(HUMAX_UCLIBC_OLD_DIR)/lib
		BT_CPU = mips
	endif

		BT_DRIVER_USB_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/brcm/linux/btusb
		BT_DRIVER_HID_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/brcm/linux/bthid
		BT_BTDL_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/brcm/linux/btdl/build
		BT_LIB_BSA_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build
		BT_SERVER_BSA_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/server/build
		BT_INST_MODULES_DIR = ${HUMAX_PREPARE_ROOT_DIR}/lib/modules/${KERNEL_WIRELESS_RELEASE}/kernel/drivers/bluetooth

		ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
			BT_CLIENT_MANAGER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_manager/build
			BT_CLIENT_AV_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_av/build
			BT_CLIENT_AVK_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_avk/build
			BT_CLIENT_BLE_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_ble/build
			BT_CLIENT_HID_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_hh/build
			BT_CLIENT_TM_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/app_tm/build
			BT_CLIENT_SIMPLEAPP_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/simple_app/build
		endif
	HUMAX_HOST_NPROCS := $(shell grep '^processor' /proc/cpuinfo | wc -l)$
	HUMAX_HOST_LOAD   = $(shell uptime | sed -e 's/^.*load average: //' -e 's/[.].*//')$
	HUMAX_MAKE_J_ARG =  $(shell if [ ${HUMAX_HOST_NPROCS} -gt ${HUMAX_HOST_LOAD} ] ; then let "X = ${HUMAX_HOST_NPROCS} - ${HUMAX_HOST_LOAD}" && echo -n "-j $$X"  ; fi)
endif # END of ifeq ($(CONFIG_SWB_B70_BT),y)
endif # END of ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
endif # END of ifeq ($(CONFIG_BLUETOOTH),y)


######################################################################
# Install Targets
######################################################################
ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
WIRELESS_INSTALL += bt-install
else
WIRELESS_INSTALL += install_wifi bt-install
endif

install_wifi :
ifeq ($(CONFIG_WIFI),y)
ifeq ($(CONFIG_NETAPP_WIFI),y)
else
ifeq ($(CONFIG_WPA_SUPPLICANT),y)
	@install -m 777 $(WPA_SUPPLICANT_OUTPUT) $(WPA_SUPPLICANT_OUTPUT_INSTALL)
endif
ifeq ($(CONFIG_WIRELESS_TOOLS),y)
	@install -m 777 $(WIRELESS_TOOLS_PATH)/bin/* $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
	@install -m 777 $(WIRELESS_TOOLS_PATH)/lib/* $(HUMAX_PREPARE_ROOT_DIR)/lib
endif
	@echo $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install
ifeq ($(CONFIG_RT3070),y)	
	@$(MAKE) -C $(WIFI_RT3070_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_RT3572),y)	
	@$(MAKE) -C $(WIFI_RT3572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_RT5572),y)	
	@$(MAKE) -C $(WIFI_RT5572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(RT5572_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_MT7601),y)
	@$(MAKE) -C $(WIFI_MT7601_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install
endif
ifeq ($(CONFIG_RTL8188),y)
	@$(MAKE) -C $(WIFI_RTL8188_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install
endif
ifeq ($(CONFIG_SWB_B70_WIFI),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_SWB_B70_WIFI_MFG),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_BCM43236),y)	
	@$(MAKE) -C $(WIFI_BCM43236_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_BCM43236_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43236_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_MFG_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_BCM43567),y)	
	@$(MAKE) -C $(WIFI_BCM43567_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_BUILD_PARAMS) install	
endif
ifeq ($(CONFIG_BCM43567_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43567_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_MFG_BUILD_PARAMS) install	
endif
endif
endif


bt-install :
ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
ifeq ($(CONFIG_SWB_B70_BT),y)
	$(HUMAX_ECHO) $(HUMAX_BUILD_BT_PARAMS) install
	@if ! [ -d ${BT_INST_MODULES_DIR} ] ; then \
		$(HUMAX_MKDIR) ${BT_INST_MODULES_DIR}; \
	fi
	$(HUMAX_CP) -v ${BT_DRIVER_USB_PATH}/btusb.ko ${BT_INST_MODULES_DIR}
	$(HUMAX_CP) -v ${BT_DRIVER_HID_PATH}/bthid.ko ${BT_INST_MODULES_DIR}
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/usr/bin ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin; \
	fi
	$(HUMAX_CP) -dpR ${BT_SERVER_BSA_PATH}/mips/bsa_server ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/usr/lib ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/usr/lib; \
	fi
	$(HUMAX_CP) -dpR ${BT_LIB_BSA_PATH}/mips/sharedlib/libbsa.so ${HUMAX_PREPARE_ROOT_DIR}/usr/lib/
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm; \
	fi
	$(HUMAX_CP) -dpR ${BT_SERVER_BSA_PATH}/BCM20702A1_001.002.014.0472.0485_detuned_cl1.hcd ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm/BCM20702.hcd

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_CP) -dpR ${BT_CLIENT_MANAGER_PATH}/mips/app_manager ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_AV_PATH}/mips/app_av ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_HID_PATH}/mips/app_hh ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_DG_PATH}/mips/app_dg ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_TM_PATH}/mips/app_tm ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
endif
else
	$(HUMAX_ECHO) $(HUMAX_BUILD_BT_PARAMS) install
	@if ! [ -d ${BT_INST_MODULES_DIR} ] ; then \
		$(HUMAX_MKDIR) ${BT_INST_MODULES_DIR}; \
	fi
	$(HUMAX_CP) -v ${BT_DRIVER_USB_PATH}/btusb.ko ${BT_INST_MODULES_DIR}
	$(HUMAX_CP) -v ${BT_DRIVER_HID_PATH}/bthid.ko ${BT_INST_MODULES_DIR}
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/usr/bin ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin; \
	fi
	$(HUMAX_CP) -dpR ${BT_SERVER_BSA_PATH}/${BT_CPU}/bsa_server ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/usr/lib ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/usr/lib; \
	fi
	$(HUMAX_CP) -dpR ${BT_LIB_BSA_PATH}/${BT_CPU}/sharedlib/libbsa.so ${HUMAX_PREPARE_ROOT_DIR}/usr/lib/
	@if ! [ -d ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm ] ; then \
		$(HUMAX_MKDIR) ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm; \
	fi
ifeq ($(CONFIG_BLUETOOTH_CHIP),"BCM43570")
	$(HUMAX_CP) -dpR ${BT_SERVER_BSA_PATH}/BCM43570A2_001.003.004.0056.0107.hcd ${HUMAX_PREPARE_ROOT_DIR}/lib/firmware/brcm/BCM43570.hcd
endif
	$(HUMAX_CP) -dpR ${BT_BTDL_PATH}/${BT_CPU}/btdl ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_CP) -dpR ${BT_CLIENT_MANAGER_PATH}/${BT_CPU}/app_manager ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_AV_PATH}/${BT_CPU}/app_av ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_AVK_PATH}/${BT_CPU}/app_avk ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_BLE_PATH}/${BT_CPU}/app_ble ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_HID_PATH}/${BT_CPU}/app_hh ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_TM_PATH}/${BT_CPU}/app_tm ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
	$(HUMAX_CP) -dpR ${BT_CLIENT_SIMPLEAPP_PATH}/${BT_CPU}/simple_app ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/
endif
endif
endif
endif


######################################################################
# Build Targets
######################################################################
ifeq ($(CONFIG_WIFI),y)
ifeq ($(CONFIG_NETAPP_WIFI),y)
else
wifi : 
	@echo $(HUMAX_WIFI_COMMON_BUILD_PARAMS)
ifeq ($(CONFIG_RT3070),y)	
	@$(MAKE) -C $(WIFI_RT3070_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_RT3572),y)	
	@$(MAKE) -C $(WIFI_RT3572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_RT5572),y)	
	@$(MAKE) -C $(WIFI_RT5572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(RT5572_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_MT7601),y)
	@$(MAKE) -C $(WIFI_MT7601_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all
endif
ifeq ($(CONFIG_RTL8188),y)
	@$(MAKE) -C $(WIFI_RTL8188_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all
endif
ifeq ($(CONFIG_SWB_B70_WIFI),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_SWB_B70_WIFI_MFG),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_BCM43236),y)	
	@$(MAKE) -C $(WIFI_BCM43236_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_BCM43236_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43236_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_MFG_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_BCM43567),y)	
	@$(MAKE) -C $(WIFI_BCM43567_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_BUILD_PARAMS) all	
endif
ifeq ($(CONFIG_BCM43567_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43567_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_MFG_BUILD_PARAMS) all
endif
	
wifi-clean :
ifeq ($(CONFIG_RT3070),y)	
	@$(MAKE) -C $(WIFI_RT3070_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_RT3572),y)	
	@$(MAKE) -C $(WIFI_RT3572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_RT5572),y)	
	@$(MAKE) -C $(WIFI_RT5572_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(RT5572_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_MT7601),y)
	@$(MAKE) -C $(WIFI_MT7601_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean
endif
ifeq ($(CONFIG_RTL8188),y)
	@$(MAKE) -C $(WIFI_RTL8188_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean
endif
ifeq ($(CONFIG_SWB_B70_WIFI),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean	
endif	
ifeq ($(CONFIG_SWB_B70_WIFI_MFG),y)	
	@$(MAKE) -C $(WIFI_SWB_B70_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_BCM43236),y)	
	@$(MAKE) -C $(WIFI_BCM43236_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_BCM43236_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43236_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43236_MFG_BUILD_PARAMS) clean	
endif
ifeq ($(CONFIG_BCM43567),y)	
	@$(MAKE) -C $(WIFI_BCM43567_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_BUILD_PARAMS) clean
endif
ifeq ($(CONFIG_BCM43567_MFG),y)	
	@$(MAKE) -C $(WIFI_BCM43567_MFG_DRIVER_PATH) $(HUMAX_WIFI_COMMON_BUILD_PARAMS) $(BCM43567_MFG_BUILD_PARAMS) clean	
endif

wifi-distclean : wifi-clean
endif
endif

bt : 
ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
	$(HUMAX_ECHO) $(HUMAX_BUILD_BT_PARAMS)
ifeq ($(CONFIG_SWB_B70_BT),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_ECHO) "**** Build all(.ko, bsa_server, libbsa, apps) ****"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) -f $(BT_SERVER_BSA_PATH)/Makefile.all $(HUMAX_BUILD_BT_PARAMS) btusb bthid all
else
	$(HUMAX_ECHO) "************* Build btusb.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_MAKE) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS)
	$(HUMAX_ECHO) "************* Build bthid.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_MAKE) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS)
	$(HUMAX_ECHO) "************* Build bsa_server ***************"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) 
	$(HUMAX_ECHO) "************* Build BSA Library  ***************"
	$(HUMAX_MAKE) -C $(BT_LIB_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS)
endif
else
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_ECHO) "**** Build all(.ko, bsa_server, libbsa, apps) ****"
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_SERVER_BSA_PATH) -f $(BT_SERVER_BSA_PATH)/Makefile.all $(HUMAX_BUILD_BT_PARAMS) btusb bthid btdl all
else
	$(HUMAX_ECHO) "************* Build btusb.ko ***************"
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS)
	$(HUMAX_ECHO) "************* Build bthid.ko ***************"
	$(HUMAX_MAKE) $(HUMAX_MAKEFLAGS) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS)
	$(HUMAX_ECHO) "************* Build bsa_server ***************"
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_SERVER_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) 
	$(HUMAX_ECHO) "************* Build BSA Library  ***************"
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_LIB_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS)
	$(HUMAX_ECHO) "************* Build btdl ***************"
	$(HUMAX_MAKE) $(HUMAX_MAKE_J_ARG) -C $(BT_BTDL_PATH) $(HUMAX_BUILD_BT_PARAMS)
endif
endif

bt-clean :
ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
ifeq ($(CONFIG_SWB_B70_BT),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_ECHO) "*** Clean all(ko, bsa_server, libbsa, apps) ***"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) -f $(BT_SERVER_BSA_PATH)/Makefile.all $(HUMAX_BUILD_BT_PARAMS) clean
else
	$(HUMAX_ECHO) "************* Clean btusb.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* Clean bthid.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* clean bsa_server ***************"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* Clean BSA Library  ***************"
	$(HUMAX_MAKE) -C $(BT_LIB_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
endif
else
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_ECHO) "*** Clean all(ko, bsa_server, libbsa, apps) ***"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) -f $(BT_SERVER_BSA_PATH)/Makefile.all $(HUMAX_BUILD_BT_PARAMS) clean
else
	$(HUMAX_ECHO) "************* Clean btusb.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_USB_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* Clean bthid.ko ***************"
	$(HUMAX_MAKE) -C $(BT_DRIVER_HID_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* clean bsa_server ***************"
	$(HUMAX_MAKE) -C $(BT_SERVER_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* Clean BSA Library  ***************"
	$(HUMAX_MAKE) -C $(BT_LIB_BSA_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
	$(HUMAX_ECHO) "************* Clean btdl  ***************"
	$(HUMAX_MAKE) -C $(BT_BTDL_PATH) $(HUMAX_BUILD_BT_PARAMS) clean
endif
endif
endif
endif

bt-distclean : bt-clean
ifeq ($(CONFIG_BLUETOOTH),y)
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
else
ifeq ($(CONFIG_SWB_B70_BT),y)
	$(HUMAX_ECHO) "***** Remove all outputs in prepare_root_dir *****"
	$(HUMAX_RM) ${BT_INST_MODULES_DIR}
#	$(HUMAX_RM) ${BT_INST_MODULES_DIR}/btusb.ko
#	$(HUMAX_RM) ${BT_INST_MODULES_DIR}/bthid.ko 
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/bsa_server
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/lib/libbsa.so
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/lib/bluetooth
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_manager
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_av
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_hh
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_dg
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_tm
endif
else
	$(HUMAX_ECHO) "***** Remove all outputs in prepare_root_dir *****"
	$(HUMAX_RM) ${BT_INST_MODULES_DIR}
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/bsa_server
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/lib/libbsa.so
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/lib/bluetooth
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/btdl
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_manager
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_av
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_avk
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_hh
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/app_tm
	$(HUMAX_RM) ${HUMAX_PREPARE_ROOT_DIR}/usr/bin/simple_app
endif
endif
endif
endif
endif
endif

