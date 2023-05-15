#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-$(CONFIG_OTA_HUMAX) += xsvc_si_ota_humax.o
ifeq ($(CONFIG_OTA_IRDETO),y)
ifeq ($(CONFIG_OTA_IRDETO_LOADER_V5),y)
obj-y += xsvc_si_ota_irdeto_v5.o
else
obj-y += xsvc_si_ota_irdeto.o
endif
endif
obj-$(CONFIG_OTA_DVBSSU) += xsvc_si_ota_dvbssu.o
obj-$(CONFIG_OTA_ASTRA) += xsvc_si_ota_astra.o
obj-$(CONFIG_OTA_SES) += xsvc_si_ota_ses.o
obj-$(CONFIG_OTA_DTT_EU) += xsvc_si_ota_eudtt.o

obj-$(CONFIG_OTA_DVBSSU) += xsvc_si_otalive_dvbssu.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include

ccflags-$(CONFIG_OTA_IRDETO) += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/irdeto/include

