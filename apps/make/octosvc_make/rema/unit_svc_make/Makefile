HUMAX_APP_NAME := unit_rema
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

REMA_MAIN_DIR := $(OCTOSVC_SRC_DIR)/rema/main
#REMA_MODULE_DIR := -I$(OCTOSVC_SRC_DIR)/rema/main/module
#REMA_MODULE_INCLUDE_DIR := -I$(OCTOSVC_SRC_DIR)/rema/main/module/include
#REMA_SERVICE_DIR := -I$(OCTOSVC_SRC_DIR)/rema/main/service
#REMA_SERVICE_INCLUDE_DIR := -I$(OCTOSVC_SRC_DIR)/rema/main/service/include
DUNIT_TEST_REMA := y

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export REMA_MAIN_DIR
export DUNIT_TEST_REMA
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

obj-y += $(OCTOSVC_SRC_DIR)/rema/unitary_test/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/clib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/rema/main/datamodel
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/service/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/include
ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include
#ccflags-y += -DHxMEMDBG_ENABLE
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak

