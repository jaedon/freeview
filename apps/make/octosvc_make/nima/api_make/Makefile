HUMAX_SO_NAME := napi
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(OCTOSVC_SRC_DIR)/nima/api/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/api/modules
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include/modules
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
else
ccflags-y += -I$(HUMAX_MAKE_DIR)/../../device/platform/driver/di/drv/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/include/modules
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
endif

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

