HUMAX_SO_NAME := nxlink
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(APPLTOP_SRC_DIR)/netflix/nxlink/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(APPLTOP_SRC_DIR)/netflix/nxlink/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/memory
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
#ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include

ccflags-y += -I$(HUMAX_MAKE_DIR)/include

## ccflags-$(CONFIG_HUMAX_CRASHREPORT) += -I$(OCTOLIB_SRC_DIR)/hcrlib/include

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

