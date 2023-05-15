HUMAX_APP_NAME := prism
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .globalconfig

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/prism/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ifeq ($(CONFIG_PRISM_ZAPPING),y)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/clib/include
endif
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/ondk/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/prism
ccflags-y += -I$(APPLTOP_SRC_DIR)/prism/teletext/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/prism/subtitle/include

ccflags-$(CONFIG_HUMAX_CRASHREPORT) += -I$(OCTOLIB_SRC_DIR)/hcrlib/include
ccflags-$(CONFIG_SUPPORT_PRISM_APP_ASAN) += -fsanitize=address -fno-omit-frame-pointer
# application private configuration
ifneq (,$(findstring sotal,$(CONFIG_APWEB_UI_NAME)))
# only for sotal model
ccflags-y += -DCONTROL_VOLUME_BY_WEB_APP
endif
ifneq (,$(findstring yousee,$(CONFIG_APWEB_UI_NAME)))
# only for yousee model
ccflags-y += -DCONTROL_VOLUME_BY_WEB_APP
endif
# end private configuration

ifeq ($(CONFIG_SUPPORT_PRISM_APP),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif

