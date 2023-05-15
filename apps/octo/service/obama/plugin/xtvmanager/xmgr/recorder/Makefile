obj-y						+= base/
obj-$(CONFIG_OP_FREESAT)	+= freesat/
obj-$(CONFIG_OP_UK_DTT)		+= ukdtt/
obj-$(CONFIG_OP_SES)		+= ses/
obj-$(CONFIG_OP_ERTELECOM)	+= ertelecom/
obj-$(CONFIG_OP_TDC)		+= tdc/

obj-y += pmgr_recorder.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/recorder/local_include
