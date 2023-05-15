
HUMAX_APP_NAME := hama
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_APP_FOLDER := /home
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/hama/main/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
ccflags-y += -I$(OCTOSVC_SRC_DIR)/secma/include
endif
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hcrlib/include
endif

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
