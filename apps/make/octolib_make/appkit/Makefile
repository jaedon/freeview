HUMAX_SO_NAME := appkit
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

#obj-y := $(OCTOTOP_SRC_DIR)/appkit/
obj-y += $(OCTOTOP_SRC_DIR)/appkit/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/lib
ccflags-y += -I$(OCTOSVC_SRC_DIR)/tima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ifeq ($(CONFIG_NOVA_SATIP_SERVER), y)
ccflags-y += -I$(OCTOTOP_SRC_DIR)/../appl/nova/include
endif

ccflags-y += -I$(HUMAX_MAKE_DIR)/include
#ccflags-y += -DHxMEMDBG_ENABLE
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

