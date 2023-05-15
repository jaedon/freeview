obj-y									+= base/
obj-$(CONFIG_MW_EPG_GATHERING_BARKER)	+= bgbarker/


obj-y	+= xmgr_epggathering.o
obj-y	+= pmgr_epggathering.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_DIR)/mgr/epggathering/local_include