USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
HUMAX_CURMAKE_DIR := $(CURDIR)
#HUMAX_BUILD_DIR := $(CURDIR)
HUMAX_OUTPUT_DIR := $(CURDIR)
#HUMAX_NFS_INSTALL_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(CONFIG_PRODUCT_NAME)/root))

HUMAX_EXTRA_CFLAG := $(ccflags-y)

ifeq ($(HUMAX_CONFIG_MODE),.config)
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/include/autoconf.h
else
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/include/globalconf.h
endif
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/include/secure_header.h
endif

HUMAX_EXTRA_CFLAG += -DOS_LINUX

HUMAX_LIB_LIST_FILE := $(CURDIR)/lib/lib.lst

export HUMAX_EXTRA_CFLAG
export HUMAX_OUTPUT_DIR
export HUMAX_CURMAKE_DIR CONFIG_TOOLCHAIN_PATH
export HUMAX_LIB_LIST_FILE
#export HUMAX_BUILD_DIR
export PACKAGE_INC_DIR-y
export APP-LINK-y

include $(HUMAX_GLOBALCONF_DIR)/HumaxGlobalLinkLibrary.mak
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak

HUMAX_MAKE_LINK := $(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxPlatform.mak

$(HUMAX_SO_NAME):
#	$(HUMAX_ECHO) ================================
#	$(HUMAX_ECHO) HUMAX_OBJ = $(obj-y)
#	$(HUMAX_ECHO) ================================
#	echo "HUMAX_EXTRA_CFLAG = $(HUMAX_EXTRA_CFLAG)"
ifeq ($(PARALLEL_MAKE_APPS), yes)
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' objlist
endif
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' library
	@ln -sf lib$(HUMAX_SO_NAME).so.0.0.1 lib$(HUMAX_SO_NAME).so.0
	@ln -sf lib$(HUMAX_SO_NAME).so.0.0.1 lib$(HUMAX_SO_NAME).so
#ifneq ($(CONFIG_DEBUG), y)
#	$(HUMAX_STRIP) --strip-debug --strip-unneeded lib$(HUMAX_SO_NAME).so
#endif
	@sudo $(HUMAX_CP) -rdf $(HUMAX_CURMAKE_DIR)/lib$(HUMAX_SO_NAME).so* $(HUMAX_NFS_INSTALL_DIR)/usr/lib/
	@sudo $(HUMAX_CP) -rdf $(HUMAX_CURMAKE_DIR)/lib$(HUMAX_SO_NAME).so* $(HUMAX_APP_STAG_DIR)/root/usr/lib/

clean:
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' clean-act
	@$(HUMAX_RM) -f $(HUMAX_CURMAKE_DIR)/lib$(HUMAX_SO_NAME).so*
	@sudo $(HUMAX_RM) -f $(HUMAX_NFS_INSTALL_DIR)/usr/lib/lib$(HUMAX_SO_NAME).so*
	@sudo $(HUMAX_RM) -f $(HUMAX_APP_STAG_DIR)/root/usr/lib/lib$(HUMAX_SO_NAME).so*

