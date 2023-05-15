HUMAX_APP_NAME := nativeapp
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(APPLTOP_SRC_DIR)/nativeapp/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/clib/include
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
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/trauma/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/ap_core/include
ccflags-$(CONFIG_SUPPORT_NIMA) += -I$(APPLTOP_SRC_DIR)/nativeapp/ap_core/contents/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/ap_port/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/menu/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/player
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/util/include
ccflags-y += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/installwizard/include
ccflags-$(CONFIG_MW_CAS_NAGRA) += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/product/cas_na/include
ccflags-$(CONFIG_MW_CAS_IRDETO) += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/product/cas_ir/include
ccflags-$(CONFIG_OP_SHAHID_APPS) += -I$(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui/product/op_shahid/include

ccflags-$(CONFIG_HUMAX_CRASHREPORT) += -I$(OCTOLIB_SRC_DIR)/hcrlib/include

NATIVEAPP_TOP_DIR				:= $(APPLTOP_SRC_DIR)/nativeapp
NATIVEAPP_CORE_INCLUDE_DIR		:= $(APPLTOP_SRC_DIR)/nativeapp/ap_core
NATIVEAPP_PORT_INCLUDE_DIR		:= $(APPLTOP_SRC_DIR)/nativeapp/ap_port
NATIVEAPP_APP_INCLUDE_DIR		:= $(APPLTOP_SRC_DIR)/nativeapp/app_native_prism
NATIVEAPP_UI_INCLUDE_DIR		:= $(APPLTOP_SRC_DIR)/nativeapp/app_native_prism/ui

export NATIVEAPP_CORE_INCLUDE_DIR
export NATIVEAPP_PORT_INCLUDE_DIR
export NATIVEAPP_APP_INCLUDE_DIR
export NATIVEAPP_UI_INCLUDE_DIR


ifeq ($(CONFIG_SUPPORT_NATIVE_APP),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
else
$(HUMAX_APP_NAME):

clean:

endif

