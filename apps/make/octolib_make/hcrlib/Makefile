HUMAX_SO_NAME := hcrlib
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(OCTOLIB_SRC_DIR)/hcrlib/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hcrlib/include

#the following declaration must be added for using HCR.
ccflags-y += -D_GNU_SOURCE 


include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

