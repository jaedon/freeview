HUMAX_APP_NAME := cthrough
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOTOP_SRC_DIR)/testkit/cthrough/src/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/lib
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include

#ccflags-y += -DHxMEMDBG_ENABLE


HUMAX_APP_FOLDER = /usr/local/cthourgh

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak	
