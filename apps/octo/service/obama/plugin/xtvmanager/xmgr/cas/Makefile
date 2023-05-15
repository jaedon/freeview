obj-y += base/

obj-y += pmgr_cas.o
obj-y += xmgr_cas.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/cas/base/include

export ccflags-y

