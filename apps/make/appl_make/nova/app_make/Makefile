HUMAX_APP_NAME := nova
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .globalconfig

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/nova/main/
obj-y += $(APPLTOP_SRC_DIR)/nova/main/server/src/
obj-y += $(APPLTOP_SRC_DIR)/nova/port/
obj-y += $(APPLTOP_SRC_DIR)/nova/test/
obj-y += $(APPLTOP_SRC_DIR)/nova/util/


ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include

ccflags-$(CONFIG_HUMAX_CRASHREPORT) += -I$(OCTOLIB_SRC_DIR)/hcrlib/include

ccflags-y += -I$(APPLTOP_SRC_DIR)/nova/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nova/main/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nova/main/server/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nova/port/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nova/util

ccflags-y += -I$(THIRDPARTY_DIR)/nova/include

ifeq ($(CONFIG_SUPPORT_NOVA_APP),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif


