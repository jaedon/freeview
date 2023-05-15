obj-y += base/
obj-$(CONFIG_OP_SES) += ses/

obj-y += pmgr_satip.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/satip/local_include
