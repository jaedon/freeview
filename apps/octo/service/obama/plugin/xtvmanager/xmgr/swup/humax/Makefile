obj-y += xmgr_swup_humax.o
obj-y += xmgr_swup_usb_humax.o
obj-y += xmgr_swup_action_humax.o
#obj-y += xmgr_swup_download_action_humax.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/swup/local_include
ccflags-y += -I$(OCTO_TVMANAGER_DIR)/mgr/swup/local_include
