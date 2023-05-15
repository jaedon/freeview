#####################################################
# Module dependent link and library
#####################################################
ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE),y)
NEXUS_LIB=nexus
else
NEXUS_LIB=nexus_client
endif

APP-LINK-y += -l$(NEXUS_LIB)
APP-LINK-y += -lmemory-null
#APP-LINK-y += -lmemory-debug
APP-LINK-y += -lhlib -lhapi -lpapi -loapi -ldlib -lrlib -lsitextlib -lummalib

# library for HLIB
APP-LINK-y += -lstdc++
APP-LINK-$(CONFIG_PACKAGE_CAP) += -lcap -lattr
APP-LINK-$(CONFIG_PACKAGE_LIBEXT2FS) += -lblkid -lcom_err -le2p -lss -luuid -lext2fs
APP-LINK-y += -pthread -lrt -lssp -lm
APP-LINK-y += -lthapi -loapi -lrlib
APP-LINK-y += -lssp
APP-LINK-y += -lpthread
APP-LINK-y += -lvkernel
APP-LINK-y += -ldrv_hotplug

APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_USB) += -ldi_usb
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += -ldi_mmc
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += -ldi_hdd -l$(NEXUS_LIB)
APP-LINK-$(CONFIG_PROD_DEVICE_FILE_SYSTEM) += -ldi_fs
