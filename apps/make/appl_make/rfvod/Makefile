HUMAX_APP_NAME := rfvod
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .globalconfig

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/rfvod/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/ondk/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/rfvod

ifeq ($(CONFIG_SUPPORT_RFVOD_APP),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif

