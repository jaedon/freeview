HUMAX_APP_NAME := octo.one
HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR=$(CURDIR)/executable.one
HUMAX_CONFIG_MODE := .config
HUMAX_GCSECTION := -Wl,--gc-sections

export HUMAX_APP_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE
export HUMAX_GCSECTION

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)
obj-y := 

obj-y += $(OCTOSVC_SRC_DIR)/obama/plugin/
obj-y += $(OCTOSVC_SRC_DIR)/obama/main/

obj-y += $(HUMAX_TOP_DIR)/3rd_party/

obj-y += $(OCTOSVC_SRC_DIR)/comma/cmddaemon/
obj-y += $(OCTOSVC_SRC_DIR)/dama/main/
obj-y += $(OCTOSVC_SRC_DIR)/hama/main/
obj-y += $(OCTOSVC_SRC_DIR)/hamalauncher/main/
obj-y += $(OCTOSVC_SRC_DIR)/homma/main/
obj-y += $(OCTOSVC_SRC_DIR)/imma/main/
obj-y += $(OCTOSVC_SRC_DIR)/nima/main/
obj-y += $(OCTOSVC_SRC_DIR)/nima/secure/main/
obj-y += $(OCTOSVC_SRC_DIR)/nima/secure/api/
obj-y += $(OCTOSVC_SRC_DIR)/pama/main/
obj-y += $(OCTOSVC_SRC_DIR)/pama/secure/api/
obj-y += $(OCTOSVC_SRC_DIR)/sama/main/
obj-y += $(OCTOSVC_SRC_DIR)/thma/main/
obj-y += $(OCTOSVC_SRC_DIR)/tima/main/
obj-y += $(OCTOSVC_SRC_DIR)/tima/secure/api/
obj-y += $(OCTOSVC_SRC_DIR)/umma/main/
obj-y += $(OCTOSVC_SRC_DIR)/umma/secure/api/
obj-y += $(OCTOSVC_SRC_DIR)/thma/main/

ccflags-y := $(HUMAX_PLATFORM_INCLUDE_FLAG)

ccflags-y += -I$(OCTOLIB_SRC_DIR)/cutest/include

ccflags-y += -I$(CURDIR)/include
ccflags-y += -I$(CURDIR)/local_include

ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/include

ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/thma/include

ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/main

ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/include/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/module/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/service/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/module
ccflags-y += -I$(OCTOSVC_SRC_DIR)/homma/main/service

ccflags-y += -I$(OCTOTOP_SRC_DIR)/appkit/include

ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/sama/lib

ccflags-y += -I$(OCTOSVC_SRC_DIR)/tima/secure/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/umma/secure/main

ccflags-y += -I$(OCTOSVC_SRC_DIR)/comma/include

ccflags-y += -I$(OCTOSVC_SRC_DIR)/tima/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/dlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/rlib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/silib/include
ccflags-y += -I$(OCTOLIB_SRC_DIR)/clib/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/hama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/dama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/pama/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/modules/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/include/modules
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/api/modules
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/main
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/nima/secure/include/modules
ccflags-y += -I$(HUMAX_MAKE_DIR)/../../device/platform/driver/di/drv/include
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
#ccflags-y += -I$(OCTOSVC_SRC_DIR)/secma/include
endif
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
ccflags-y += -I$(OCTOLIB_SRC_DIR)/hcrlib/include
endif

ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/toolkit
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/utils/utils/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/db/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/tvservice/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/tvservice/svc/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/tvservice/svc/common/include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/tvservice/svc/epg/local_include
ccflags-y += -I$(OCTOSVC_SRC_DIR)/obama/main/mediators
ifeq ($(CONFIG_MW_CAS_IRDETO_LOADER_DVT),y)
#it shall be ignored below. 20160212
## ccflags-y += -I$(APPLTOP_SRC_DIR)/irldvt
endif
#ccflags-y += -DHxMEMDBG_ENABLE

USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
HUMAX_CURMAKE_DIR := $(CURDIR)
HUMAX_BUILD_DIR := $(CURDIR)
HUMAX_OUTPUT_DIR := $(CURDIR)
#HUMAX_NFS_INSTALL_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(CONFIG_PRODUCT_NAME)/root))
HUMAX_NFS_IMAGE_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(HUMAX_INSTALL_NAME)/image))

HUMAX_EXTRA_CFLAG := $(ccflags-y) -include $(HUMAX_MAKE_DIR)/autoconf.h -include $(HUMAX_MAKE_DIR)/../octo/service/version.h
# HUMAX_EXTRA_CFLAG += -DCONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME)
ifeq ($(CONFIG_DEBUG), y)
HUMAX_EXTRA_CFLAG += -DCONFIG_DEBUG=1
endif
HUMAX_EXTRA_CFLAG += -DOS_LINUX

HUMAX_LIB_LIST_FILE := $(CURDIR)/lib/lib.lst

HUMAX_APP_STAG_ROOT_DIR = $(HUMAX_APP_STAG_DIR)/root

OCTO_MAIN_INCLUDE_DIR			:= $(OCTO_DIR)/main/include
OCTO_DB_DIR						:= $(OCTO_DIR)/main/db
OCTO_TVSERVICE_DIR				:= $(OCTO_DIR)/main/tvservice
OCTO_TVMANAGER_DIR				:= $(OCTO_DIR)/main/tvmanager
OCTO_OAPIMANAGER_DIR			:= $(OCTO_TVMANAGER_DIR)/om

OCTO_DB_PLUGIN_DIR				:= $(OCTO_DIR)/plugin/xdb
OCTO_TVSERVICE_PLUGIN_DIR		:= $(OCTO_DIR)/plugin/xtvservice
OCTO_TVMANAGER_PLUGIN_DIR		:= $(OCTO_DIR)/plugin/xtvmanager
OCTO_OAPIMANAGER_PLUGIN_DIR		:= $(OCTO_TVMANAGER_PLUGIN_DIR)/xom

OCTO_PLUGIN_DIR				:= $(OCTO_DIR)/plugin
OCTO_PLUGIN_INCLUDE_DIR		:= $(OCTO_DIR)/plugin/include
OCTO_THIRDPARTY_PORT_DIR	:= $(OCTO_DIR)/plugin/3rdparty_port
OCTO_PROFILE_DIR  			:= $(OCTO_DIR)/main/profile
OCTO_UTILS_DIR				:= $(OCTO_DIR)/main/utils
OCTO_PAL_INCLUDE_DIR		:= $(OCTO_TVSERVICE_DIR)/pal/include

OCTO_XSVC_CAS_PLUGIN_DIR	:= $(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas
OCTO_XMGR_CAS_PLUGIN_DIR	:= $(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/cas

HUMAX_FIND := find
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO :=
endif

VERSION_HEADER = $(HUMAX_MAKE_DIR)/../octo/service/version.h
BIN_HEADER = $(shell ./binheader.sh $(HUMAX_MAKE_DIR)/.config $(VERSION_HEADER))
VERSION_FILE = APP_VERSION.bin

export PACKAGES_DIR
##export PACKAGE_INC_DIR := $(PACKAGES_DIR)/build/staging_dir/usr/include
export HUMAX_EXTRA_CFLAG
export HUMAX_MODULE=APPS
export HUMAX_CURMAKE_DIR
export HUMAX_OUTPUT_DIR
export CONFIG_TOOLCHAIN_PATH
export HUMAX_LIB_LIST_FILE
#export HUMAX_NFS_INSTALL_DIR
export AP_STATIC_LIB_DIR
export HUMAX_APP_STAG_ROOT_DIR
export OCTO_MAIN_INCLUDE_DIR
export OCTO_DB_DIR
export OCTO_TVSERVICE_DIR
export OCTO_PAL_INCLUDE_DIR
export OCTO_TVMANAGER_DIR
export OCTO_OAPIMANAGER_DIR
export OCTO_PLUGIN_DIR
export OCTO_PLUGIN_INCLUDE_DIR
export OCTO_THIRDPARTY_PORT_DIR
export OCTO_DB_PLUGIN_DIR
export OCTO_TVMANAGER_PLUGIN_DIR
export OCTO_TVSERVICE_PLUGIN_DIR
export OCTO_OAPIMANAGER_PLUGIN_DIR
export OCTO_UTILS_DIR
export OCTO_XSVC_CAS_PLUGIN_DIR
export OCTO_XMGR_CAS_PLUGIN_DIR

include $(HUMAX_GLOBALCONF_DIR)/HumaxGlobalLinkLibrary.mak
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak

export PACKAGE_INC_DIR-y
export APP-LINK-y

HUMAX_MAKE_LINK := $(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxPlatform.mak

$(HUMAX_APP_NAME): make_version_bin
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' library
	$(HUMAX_MAKE_LINK) link
	$(HUMAX_NM) $(HUMAX_APP_NAME) | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > $(HUMAX_APP_NAME)_system.map
ifneq ($(CONFIG_DEBUG), y)
	$(HUMAX_STRIP) --strip-all $(HUMAX_CURMAKE_DIR)/$(HUMAX_APP_NAME)
endif
	@$(HUMAX_SUDO) $(HUMAX_CP) $(OCTO_PROFILE_DIR)/$(CONFIG_PRODUCT_NAME)_profile.hxm $(HUMAX_NFS_INSTALL_DIR)/usr/etc/transcoder_profile.hxm
	@$(HUMAX_CP) $(OCTO_PROFILE_DIR)/$(CONFIG_PRODUCT_NAME)_profile.hxm $(HUMAX_APP_STAG_ROOT_DIR)/usr/etc/transcoder_profile.hxm
	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_ROOT_DIR)/home/$@
	@$(HUMAX_SUDO) $(HUMAX_CP) $@ $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_CP) $@ $(HUMAX_APP_STAG_ROOT_DIR)/home/$@
	@$(HUMAX_MKDIR) $(HUMAX_CURMAKE_DIR)/obj
	@$(HUMAX_FIND) $(OCTO_DIR) -name "*.o" -exec $(HUMAX_CP) {} $(HUMAX_CURMAKE_DIR)/obj \;

clean: clean_version_bin
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean-act
	@$(HUMAX_RM) -f $(HUMAX_CURMAKE_DIR)/obj
	@find $(HUMAX_TOP_DIR)/apps/octo/service/obama/ -name "*.d" -or -name "*.o" | xargs rm -f

install:
	$(HUMAX_MAKE_LINK) link
	$(HUMAX_SUDO) $(HUMAX_CP) $@ $(HUMAX_NFS_INSTALL_DIR)/usr/bin/

distclean: clean
	@$(HUMAX_RM)	-f $(HUMAX_CURMAKE_DIR)/$(HUMAX_APP_NAME)

make_version_bin : clean_version_bin
	$(HUMAX_ECHO) =============================================
	$(HUMAX_ECHO) "[*** Make Version Binary ***]"
	$(HUMAX_ECHO) $(BIN_HEADER)
	@$(HUMAX_CP) $(VERSION_FILE) $(HUMAX_NFS_IMAGE_DIR)/
	$(HUMAX_ECHO) =============================================

clean_version_bin :
	@$(HUMAX_RM) -f $(VERSION_FILE)
	@$(HUMAX_RM) -f $(HUMAX_NFS_IMAGE_DIR)/$(VERSION_FILE)

