obj-y						+= base/
obj-$(CONFIG_OP_AUS_DTT)	+= ausdtt/
obj-$(CONFIG_OP_UK_DTT)		+= ukdtt/
obj-$(CONFIG_OP_ZIGGO)		+= ziggo/
obj-$(CONFIG_OP_NORDIG)		+= nordig/

obj-y	+= xmgr_dynamicupdate.o
obj-y	+= pmgr_dynamicupdate.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_DIR)/mgr/dynamicupdate/local_include