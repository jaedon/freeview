obj-y += base/
obj-y += pmgr_schedule.o
obj-$(CONFIG_OP_SES)							+= ses/
obj-$(CONFIG_OP_PVR_BASED_ON_TVANYTIME)			+= tva/

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/schedule/local_include

