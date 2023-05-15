#

-include .config

obj-y := dfbmaster.o

inccflag-y := -I$(HUMAX_MAKE_DIR)/../include/di
inccflag-y += -I$(HUMAX_MAKE_DIR)/../include/hos
inccflag-y += -I$(HUMAX_MAKE_DIR)/../include/cmd
inccflag-y += -I$(HUMAX_MAKE_DIR)/../include/directfb
inccflag-y += -I$(HUMAX_MAKE_DIR)/../platform/sdk/AppLibs/opensource/directfb/src/DirectFB-1.4.17/lib
inccflag-y += -I$(HUMAX_MAKE_DIR)/../platform/sdk/AppLibs/opensource/directfb/src/DirectFB-1.4.17/include
inccflag-y += -I$(HUMAX_MAKE_DIR)/../platform/sdk/AppLibs/opensource/directfb/src/DirectFB-1.4.17/include
inccflag-y += -I$(HUMAX_MAKE_DIR)/../platform/sdk/AppLibs/opensource/directfb/src/DirectFB-Broadcom

export HUMAX_EXTRA_CFLAG := $(inccflag-y) -include $(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)/autoconf.h
export HUMAX_MODULE=MAIN

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak

ifeq ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
ODIR = DEBUG
else
ODIR = RELEASE
endif

$(ODIR):
	$(Q) test -d $(ODIR) || install -d -m 755 $(ODIR)

dfbmaster: $(ODIR)
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" LIBNAME=$@ static-library
	$(HUMAX_CP) $(obj-y) $(ODIR)

clean_dfbmaster:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" HUMAX_OBJ='$(obj-y)' clean
	$(HUMAX_RM) -rf $(ODIR)
	$(HUMAX_RM) -rf *.a

