#
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/autoconf.h

ifeq ($(CONFIG_OS_UCOS),y)
obj-y := di_flash_loader.o
else # CONFIG_OS_UCOS
obj-$(CONFIG_NOR_FLASH) := di_flash.o
obj-$(CONFIG_EMMC_FLASH) += di_flash_emmc.o
ifneq ($(CONFIG_DI10),)
obj-y += di_flash_emu.o
endif
ifeq ($(CONFIG_NAND_FLASH),y)
obj-y += di_flash_nand.o
else
obj-y += di_flash_nand_null.o
endif
endif # CONFIG_OS_XXX
