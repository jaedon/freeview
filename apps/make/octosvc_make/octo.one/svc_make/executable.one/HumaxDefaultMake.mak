USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
HUMAX_CURMAKE_DIR := $(CURDIR)
#HUMAX_BUILD_DIR := $(CURDIR)
HUMAX_OUTPUT_DIR := $(CURDIR)
#HUMAX_NFS_INSTALL_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(CONFIG_PRODUCT_NAME)/root))

HUMAX_EXTRA_CFLAG := $(ccflags-y)
HUMAX_EXTRA_CFLAG += -DOS_LINUX

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO :=
endif

ifeq ($(HUMAX_CONFIG_MODE),.config)
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/include/autoconf.h
else
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/include/globalconfig.h
endif

HUMAX_LIB_LIST_FILE := $(CURDIR)/lib.lst
HUMAX_APP_STAG_ROOT_DIR = $(HUMAX_APP_STAG_DIR)/root

export PACKAGES_DIR
export HUMAX_EXTRA_CFLAG
export HUMAX_OUTPUT_DIR
export HUMAX_CURMAKE_DIR CONFIG_TOOLCHAIN_PATH
export HUMAX_LIB_LIST_FILE
#export HUMAX_NFS_INSTALL_DIR
export HUMAX_APP_STAG_ROOT_DIR
export CONFIG_DEBUG
export AP_STATIC_LIB_DIR

include $(HUMAX_GLOBALCONF_DIR)/HumaxGlobalLinkLibrary.mak
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak

export APP-LINK-y
export PACKAGE_INC_DIR-y
HUMAX_MAKE_LINK := $(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxPlatform.mak

ifeq ($(HUMAX_APP_FOLDER),)
HUMAX_APP_FOLDER := /home/$(HUMAX_APP_NAME)
endif

PRIVATE_LINK=$(wildcard $(HUMAX_CURMAKE_DIR)/HumaxLinkOption.mak)
ifneq ($(PRIVATE_LINK),)
include $(HUMAX_CURMAKE_DIR)/HumaxLinkOption.mak
else

$(HUMAX_APP_NAME):
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' library
	$(HUMAX_MAKE_LINK) link
	$(HUMAX_NM) $@ | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > $@.map
	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/$(HUMAX_APP_FOLDER)
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_DIR)/root/$(HUMAX_APP_FOLDER)
	@$(HUMAX_SUDO) cp $@ $(HUMAX_NFS_INSTALL_DIR)/$(HUMAX_APP_FOLDER)
	@cp $@ $(HUMAX_APP_STAG_DIR)/root/$(HUMAX_APP_FOLDER)
	@$(HUMAX_RM) $@

clean:
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean-act

endif

