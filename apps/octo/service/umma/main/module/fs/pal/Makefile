obj-y += umma_fs_pal_fs.o
obj-y += umma_fs_pal_chunk.o
obj-y += umma_fs_pal_partition.o
obj-$(CONFIG_PROD_DEVICE_STORAGE_USB) += umma_fs_pal_usb.o
obj-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += umma_fs_pal_hdd.o
obj-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += umma_fs_pal_mmc.o
obj-y += umma_fs_pal_flash.o
# Basic include path

# Additional include path
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main/module/fs/pal/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include