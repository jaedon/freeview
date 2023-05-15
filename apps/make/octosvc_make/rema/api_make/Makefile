HUMAX_SO_NAME := remapi
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(OCTOSVC_SRC_DIR)/rema/api/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/main/include

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

