
HUMAX_SO_NAME := oapi
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(OCTOSVC_SRC_DIR)/obama/api/
#obj-y += $(OCTOSVC_SRC_DIR)/umma/api/
#obj-y += $(OCTOSVC_SRC_DIR)/thma/api/
obj-y += $(OCTOSVC_SRC_DIR)/homma/api/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/oxserver
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
#ccflags-y += -DHxMEMDBG_ENABLE
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak


