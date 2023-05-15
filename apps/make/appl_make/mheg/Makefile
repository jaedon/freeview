HUMAX_APP_NAME := mheg
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .globalconfig

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/mheg/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/mheg/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/mheg
ccflags-y += -I$(APPLTOP_SRC_DIR)/mheg/itk/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/mheg/itk/opensrc/include

ccflags-y += -I$(THIRDPARTY_DIR)/mheg/itk/include

ifeq ($(CONFIG_3RD_MHEG_USE_SNT_ENGINE),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif
