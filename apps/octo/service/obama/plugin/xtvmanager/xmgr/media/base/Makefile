obj-y += xmgr_mplayer_base.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/media/local_include

ifeq ($(CONFIG_DEBUG),y)
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/debug/include
else
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/release/include
endif

