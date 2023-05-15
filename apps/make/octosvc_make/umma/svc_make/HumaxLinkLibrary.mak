#####################################################
# Module dependent link and library
#####################################################
ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE),y)
NEXUS_LIB=nexus
else
NEXUS_LIB=nexus_client
endif

APP-LINK-$(CONFIG_SUPPORT_UMMA_ASAN) += -lasan
APP-LINK-$(CONFIG_SUPPORT_UMMA_SAMBA_CLIENT) += -l$(NEXUS_LIB) -lnapi
APP-LINK-$(CONFIG_SUPPORT_UMMA_SAMBA_SERVER) += -l$(NEXUS_LIB) -lnapi
APP-LINK-$(CONFIG_SUPPORT_UMMA_FTP_SERVER) += -l$(NEXUS_LIB) -lnapi -lcrypt
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY) += -lsqlite3
APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib
APP-LINK-$(CONFIG_FUNC_USE_DECRYPTCOPY) += -l$(NEXUS_LIB) -ldi_crypt
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi

APP-LINK-y += -lmemory-null
#APP-LINK-y 	+= -lmemory-debug
APP-LINK-y += -lhlib -lhapi -lpapi -ldapi -ldlib -lsitextlib
APP-LINK-y += -pthread -lssp
APP-LINK-y += -lthapi -loapi -lrlib
APP-LINK-y += -lssp
APP-LINK-y += -lpthread
APP-LINK-y += -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)             += -lcap -lattr
ifndef $(CONFIG_ENHANCED_SECURITY)

APP-LINK-$(CONFIG_PACKAGE_LIBEXT2FS) += -lblkid -lcom_err -le2p -lss -luuid -lext2fs
APP-LINK-y += -ldrv_hotplug
APP-LINK-y += -lummalib
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_USB) += -ldi_usb
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += -ldi_mmc
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += -ldi_hdd -l$(NEXUS_LIB)
APP-LINK-$(CONFIG_PROD_DEVICE_FILE_SYSTEM) += -ldi_fs

endif

