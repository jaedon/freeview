HUMAX_APP_NAME := irldvt
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .globalconfig


export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/irldvt/
obj-y += $(APPLTOP_SRC_DIR)/irldvt/irdeto

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/ondk/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/irdeto/include
#ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/glib
#ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/glib/include
#ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/pal
#ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/pal/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/utils
ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt/utils/include



ifeq ($(CONFIG_PRODUCT_NAME),"ir4000hd")
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
#include $(OCTOSVC_SRC_DIR)/obama/main/include/Makefile.include.pal
#include $(OCTO_DIR)/main/include/Makefile.include.mw
#include $(OCTO_DIR)/main/include/Makefile.include.xmgr
else
$(HUMAX_APP_NAME):



clean:
endif

