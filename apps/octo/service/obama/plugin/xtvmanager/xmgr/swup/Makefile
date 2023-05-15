obj-$(CONFIG_OTA_DVBSSU)		+= dvbssu/
obj-$(CONFIG_OTA_ASTRA)			+= astra/
obj-$(CONFIG_OTA_HUMAX)			+= humax/
obj-$(CONFIG_OTA_IRDETO)		+= irdeto/
obj-$(CONFIG_OTA_SES)			+= ses/
obj-$(CONFIG_OTA_DTT_EU)		+= dtt/
obj-$(CONFIG_OTA_DTT_UK)		+= dtt/
obj-$(CONFIG_OTA_DTT_UK_SSU)	+= dtt/

obj-y	+= pmgr_swup.o
obj-y	+= xmgr_swup.o
#obj-y 	+= xmgr_swup_param.o
#obj-y	+= xmgr_swup_wakeup.o
obj-y 	+= xmgr_swup_usb.o
obj-y 	+= xmgr_swup_download_dvbssu.o
obj-y	+= xmgr_swup_debug.o
obj-y	+= xmgr_swup_scenario.o


# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/swup/local_include
