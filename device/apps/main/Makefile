#

obj-y := main.o

inccflag-y := -I$(HUMAX_MAKE_DIR)/../include/di
inccflag-y += -I$(HUMAX_MAKE_DIR)/../include/hos
inccflag-y += -I$(HUMAX_MAKE_DIR)/../include/cmd


export HUMAX_EXTRA_CFLAG := $(inccflag-y)
export HUMAX_MODULE=MAIN

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak
-include $(HUMAX_MAKE_DIR)/.crc_config
-include $(HUMAX_MAKE_DIR)/.mem_config

ifeq ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
ODIR = DEBUG
else
ODIR = RELEASE
endif

ifeq ($(CONFIG_CRC_AUTOTEST),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_CRC_AUTOTEST
ifeq ($(CONFIG_USB_MODULE),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_USB_MODULE
endif
endif
ifeq ($(CONFIG_MEM_AUTOTEST),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_MEM_AUTOTEST
HUMAX_EXTRA_CFLAG += -DCONFIG_MEM_CHIP=\"$(CONFIG_CHIP)$(CONFIG_CHIP_REV)\"
ifeq ($(CONFIG_USB_MODULE),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_USB_MODULE
endif
endif

$(ODIR):
	$(Q) test -d $(ODIR) || install -d -m 755 $(ODIR)

main: $(ODIR)
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" LIBNAME=$@ static-library
	$(HUMAX_CP) $(obj-y) $(ODIR)

clean_main:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" HUMAX_OBJ='$(obj-y)' clean
	$(HUMAX_RM) -rf $(ODIR)
	$(HUMAX_RM) -rf *.a
