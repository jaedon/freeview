#####################################################
# Module dependent link and library
#####################################################

APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)

APP-LINK-y += -ldriver
APP-LINK-y += -londk
ifeq ($(CONFIG_SUPPORT_NIMA), y)
APP-LINK-y += -ldi_network
APP-LINK-y += -ldi_wifi
APP-LINK-y += -ldrv_hotplug
APP-LINK-$(CONFIG_MW_INET_PPPOE) += -ldi_pppoe
APP-LINK-$(CONFIG_PROD_DEVICE_BLUETOOTH) += -lbsa -ldi_bt
endif

APP-LINK-y += -ldrv_hotplug
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_USB) += -ldi_usb
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += -ldi_mmc
APP-LINK-$(CONFIG_PROD_DEVICE_FILE_SYSTEM) += -ldi_fs
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += -ldi_hdd

#ifneq ($(NEXUS_CLIENT),)
#APP-LINK-y			+= -lnexus_client
#endif

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y                      += -lnexus
else
APP-LINK-y                      += -lnexus_client
endif

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y                                      += -linit
else
APP-LINK-y                                      += -linit_client
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y                      += -pthread -lrt -lssp -lm
APP-LINK-y                                                              += -lhlib -lhapi -ldlib -lpapi -lsilib -ldapi -lrlib -lnapi -lsamalib -lsapi -luapi -lthapi -ltapi

APP-LINK-y += -ldirectfb -lfusion -ldirect -ldl -lsawman -lz
APP-LINK-y += -lm
APP-LINK-y += -lstdc++

APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)             += -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz

# library for HLIB
APP-LINK-y                                      += -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)             += -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)          += -lcap -lattr

APP-LINK-$(CONFIG_PACKAGE_LIBEXT2FS)    += -lblkid -lcom_err -le2p -lss -luuid -lext2fs

#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/di
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/ai
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/directfb
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/fusion
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/sawman
#PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/pcd/lib/include
#PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/sqlite/include
