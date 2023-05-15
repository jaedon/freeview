HUMAX_APP_NAME := se_umma
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_APP_FOLDER := /home/sechdlr
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/umma/secure/main/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include
ccflags-y += -I$(HUMAX_MAKE_DIR)/../../device/platform/driver/di/drv/include

#ccflags-y += -DHxMEMDBG_ENABLE

ifeq ($(CONFIG_SUPPORT_UMMA),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

else
$(HUMAX_APP_NAME):

clean:

endif
