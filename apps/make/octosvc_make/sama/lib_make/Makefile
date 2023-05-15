HUMAX_SO_NAME := samalib
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y := $(OCTOSVC_SRC_DIR)/sama/lib/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/lib

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

