#####################################################
# Private application link option
#####################################################

APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

APP-LINK-y += -lcwmp_cpe 
APP-LINK-y += -lcwmp_engine

#APP-LINK-y += -lmxdlna_core -lmxkernel
#APP-LINK-y += -lmxdtcp -lmxfile -lmxmedia -lmxres

ifeq ("1","1") #
APP-LINK-y += -ldriver
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

ifeq ($(CONFIG_SUPPORT_IMMA), y)
APP-LINK-y += -lmediapb_ipc
endif

ifeq ($(CONFIG_HOMMA_CDI), y)
APP-LINK-y += -ldi_crypt
endif


ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y			+= -lnexus
else
APP-LINK-y			+= -lnexus_client
endif

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y					+= -linit
else
APP-LINK-y					+= -linit_client
endif

APP-LINK-$(CONFIG_PACKAGE_FFMPEG) += -lavformat -lavcodec -lswscale -lavutil

APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz
endif 
##("0","1")


APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi -lpapi -loapi -ldapi -lnapi -lsamalib -lsapi -ldlib -lrlib -lsilib -lappkit -luapi -lthapi -lclib

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_JANSSON)		+= -ljansson
APP-LINK-$(CONFIG_PACKAGE_OPENSSL)		+= -lssl -lcrypto
APP-LINK-$(CONFIG_PACKAGE_LIBXML)		+= -lxml
APP-LINK-$(CONFIG_PACKAGE_LIBXML2)		+= -lxml2
APP-LINK-$(CONFIG_PACKAGE_CURL)			+= -lcurl
APP-LINK-$(CONFIG_PACKAGE_ZLIB)			+= -lz
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
###########################################################################

APP-LINK-y += -pthread -lrt -lssp -lm -lsqlite3 -laxis2_axiom -laxis2_parser -laxutil -lcurl -lcrypto -lz

APP-LINK-y += -lcunit

