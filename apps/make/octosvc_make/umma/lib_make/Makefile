HUMAX_SO_NAME := ummalib
HUMAX_GLOBALCONF_DIR = $(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR  = $(HUMAX_GLOBALENV_SODIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/umma/lib/


ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
#ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
#ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
#ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/lib
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ccflags-y += -I$(HUMAX_MAKE_DIR)/../../device/platform/driver/di/drv/include
#ccflags-y += -DHxMEMDBG_ENABLE

include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

