HUMAX_APP_NAME := umma
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/umma/main/
obj-y += $(OCTOSVC_SRC_DIR)/umma/secure/api/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/lib

ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hcrlib/include
endif
#ccflags-y += -DHxMEMDBG_ENABLE

ccflags-$(CONFIG_SUPPORT_UMMA_ASAN) += -fsanitize=address -fno-omit-frame-pointer

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO := sudo
endif

ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(HUMAX_MAKE_DIR)/../../device/platform/driver/di/drv/include

OCTO_UMMA_FS_PROPILE_DIR	:= $(OCTOSVC_SRC_DIR)/umma/main/module/fs/profile

ifeq ($(CONFIG_SUPPORT_UMMA),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif



