HUMAX_APP_NAME := tima
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/tima/main/
obj-y += $(OCTOSVC_SRC_DIR)/tima/secure/api/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/tima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/tima/secure/include
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hcrlib/include
endif
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
#ccflags-y += -DHxMEMDBG_ENABLE
ccflags-$(CONFIG_SUPPORT_TIMA_ASAN) += -fsanitize=address -fno-omit-frame-pointer
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

