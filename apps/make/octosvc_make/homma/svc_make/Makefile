HUMAX_APP_NAME := homma
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config

HOMMA_MODULE_DIR := -I$(OCTOSVC_SRC_DIR)/homma/main/module
HOMMA_MODULE_INCLUDE_DIR := -I$(OCTOSVC_SRC_DIR)/homma/main/module/include
HOMMA_SERVICE_DIR := -I$(OCTOSVC_SRC_DIR)/homma/main/service
HOMMA_SERVICE_INCLUDE_DIR := -I$(OCTOSVC_SRC_DIR)/homma/main/service/include
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HOMMA_MODULE_INCLUDE_DIR
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/homma/main/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/clib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/module/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/service/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/trauma/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-$(CONFIG_HUMAX_CRASHREPORT) += -I$(OCTOLIB_SRC_DIR)/hcrlib/include

#ccflags-y += -DHxMEMDBG_ENABLE
ccflags-$(CONFIG_SUPPORT_HOMMA_ASAN) += -fsanitize=address -fno-omit-frame-pointer
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

