#################################################################
####Author: Someone
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
TOOL_INC_PATH += -I$(OCTOTOP_SRC_DIR)/testkit/atkit/c_apps/tc/include
TOOL_INC_PATH += -I$(OCTOTOP_SRC_DIR)/testkit/atkit/c_apps/tc/lib
TOOL_INC_PATH += -I$(OCTOSVC_SRC_DIR)/sama/include

HUMAX_EXTRA_CFLAG := -include $(HUMAX_MAKE_DIR)/autoconf.h
HUMAX_EXTRA_CFLAG += -DOS_LINUX
HUMAX_EXTRA_CFLAG += $(TOOL_INC_PATH)

include $(HUMAX_GLOBALCONF_DIR)/HumaxGlobalLinkLibrary.mak
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak
include ./TcLinkLibrary.mak

HUMAX_MAKE_LINK := $(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxPlatform.mak

OBJ_TC_DIR = $(shell pwd)
OBJ_DIR_files = $(wildcard $(OBJ_TC_DIR)/*.o)
SRC_TC_DIR = $(OBJ_TC_DIR)/../src
RESULT_TC_DIR = $(OBJ_TC_DIR)/../resultTC
LIB_TC_DIR = $(OBJ_TC_DIR)/../lib

SRC_TC_files = $(wildcard $(SRC_TC_DIR)/*/*.c)
OBJ_TC_files = $(patsubst %.c, %.o, $(SRC_TC_files))
LIB_TC_files = $(wildcard $(LIB_TC_DIR)/src/*.c)
LIB_OBJ_files = $(patsubst %.c, %.o, $(LIB_TC_files))

STATIC_LIBS := libmanifest.a

TARGET = maintc
#Adding target as sub-folder name
TARGET += tc_appkit_recording
TARGET += tc_hlib_socket
TARGET += tc_appkit_power
TARGET += tc_appkit_db
TARGET += tc_appkit_mediaplay
TARGET += tc_appkit_output
TARGET += tc_appkit_meta_recording
TARGET += tc_appkit_filemanager
TARGET += tc_appkit_network
TARGET += tc_appkit_dlna
TARGET += tc_appkit_sysinfo
TARGET += auto-manifest

define build_target
	$(eval $(1)_C_files := $(wildcard $(SRC_TC_DIR)/$(1)/*.c))
	$(eval $(1)_OBJ_files := $(patsubst %.c, %.o, $($(1)_C_files)))
	$(HUMAX_CC) $($(1)_OBJ_files) -o $(1) $(HUMAX_CFLAG) $(HUMAX_EXTRA_CFLAG) $(APP-LINK-y)
endef

all: lib $(OBJ_TC_files) FORCE
	$(foreach subtarget, $(TARGET), $(call build_target,$(subtarget)))
	sudo $(HUMAX_CP) $(TARGET) $(HUMAX_NFS_INSTALL_DIR)/home/atkit/tc
	sudo $(HUMAX_CP) $(TARGET) $(HUMAX_APP_STAG_ROOT_DIR)/home/atkit/tc
	sudo $(HUMAX_CP) ../manifest/*.manifest $(HUMAX_NFS_INSTALL_DIR)/home/atkit/manifest/
	sudo $(HUMAX_CP) ../manifest/*.manifest $(HUMAX_APP_STAG_ROOT_DIR)/home/atkit/manifest/
	@if [ ! -d $(RESULT_TC_DIR) ]; then \
		mkdir -p $(RESULT_TC_DIR); \
	fi;
	$(HUMAX_MV) $(TARGET) $(RESULT_TC_DIR)

%o:%c
	$(HUMAX_CC) $(HUMAX_CFLAG) $(HUMAX_EXTRA_CFLAG) $(APP-LINK-y) -c $< -o $@

lib:
	$(HUMAX_CC) $(HUMAX_CFLAG) $(HUMAX_EXTRA_CFLAG) $(APP-LINK-y) -c $(LIB_TC_files) -o $(LIB_OBJ_files)
	$(HUMAX_AR) rcs $(LIB_TC_DIR)/$(STATIC_LIBS) $(LIB_OBJ_files)

clean: FORCE
	-rm -rf $(OBJ_TC_files)
	-rm -rf $(RESULT_TC_DIR)
	-rm -rf ../$(TARGET)
	-rm -rf $(OBJ_DIR_files)
	-rm -rf $(LIB_OBJ_files)
FORCE:
