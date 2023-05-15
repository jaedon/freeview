#
obj-y :=

ifeq (${CONFIG_MICOM_JP}, y)
obj-y += drv_micom_jp.o
endif
ifeq (${CONFIG_MICOM}, y)
obj-y += drv_micom.o
ifeq (${CONFIG_MICOM_UPGRADE}, y)
obj-y += drv_micom_upgrade.o
endif
endif
ifeq (${CONFIG_MICOM_TINY24}, y)
obj-y += drv_micom_tiny24.o
endif
ifeq (${CONFIG_MICOM_NONE}, y)
obj-y += drv_micom_emu.o
endif

