EXECUTABLE := get_all_schedule get_all_recording atkit_ua_monitor

all : $(EXECUTABLE) install

#get_all_schedule get_all_recording atkit_ua_monitor

.PHONY: all

.PHONY: install

##################################################################
##################################################################

HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR =$(HUMAX_GLOBALENV_BINDIR)
HUMAX_CONFIG_MODE := .config

export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)

HUMAX_CURMAKE_DIR :=
USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
HUMAX_NFS_INSTALL_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(HUMAX_INSTALL_NAME)/root))
HUMAX_APP_STAG_ROOT_DIR = $(HUMAX_APP_STAG_DIR)/root

TOOL_INC_PATH :=
TOOL_INC_PATH += $(HUMAX_PLATFORM_INCLUDE_FLAG)
TOOL_INC_PATH += -I$(OCTOLIB_SRC_DIR)/hlib/include
TOOL_INC_PATH += -I$(OCTOLIB_SRC_DIR)/dlib/include
TOOL_INC_PATH += -I$(OCTOLIB_SRC_DIR)/clib/include
TOOL_INC_PATH += -I$(OCTOLIB_SRC_DIR)/rlib/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/hama/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/dama/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/umma/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/thma/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/nima/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/homma/include
TOOL_INC_PATH += -I$(OCTOTOP_SRC_DIR)/appkit/include
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/obama/include

HUMAX_EXTRA_CFLAG := -include $(HUMAX_MAKE_DIR)/autoconf.h
HUMAX_EXTRA_CFLAG += -DOS_LINUX
HUMAX_EXTRA_CFLAG += $(TOOL_INC_PATH)

APP-LINK-y += -lappkit

include $(HUMAX_GLOBALCONF_DIR)/HumaxGlobalLinkLibrary.mak
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak
include ./UserAppLinkLibrary.mak

HUMAX_MAKE_LINK := $(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxPlatform.mak

%: %.c
	$(HUMAX_CC) $^ -o $@ $(HUMAX_CFLAG) $(APP-LINK-y)

install:
	sudo cp $(EXECUTABLE) $(HUMAX_NFS_INSTALL_DIR)/home/atkit/userapp
	sudo cp $(EXECUTABLE) $(HUMAX_APP_STAG_DIR)/root/home/atkit/userapp

clean:
	$(RM) $(EXECUTABLE)
