obj-y += umma_fs.o
obj-y += umma_fs_device.o
obj-y += umma_fs_config.o

obj-y += umma_fs_hdd.o
obj-y += umma_fs_usb.o
obj-y += umma_fs_mmc.o
obj-y += umma_fs_flash.o

obj-$(CONFIG_MW_MM_PVR) += umma_fs_pvr.o

obj-y += pal/

# Additional include path
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main/module/fs/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main/module/fs/pal/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include
