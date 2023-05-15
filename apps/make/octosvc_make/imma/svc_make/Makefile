HUMAX_APP_NAME := imma
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR=$(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/imma/main/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(CURDIR)/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ccflags-$(CONFIG_SUPPORT_IMMA_ASAN) += -fsanitize=address -fno-omit-frame-pointer

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
