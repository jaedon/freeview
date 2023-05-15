obj-y									+= xmgr_cas_na.o
obj-y									+= xmgr_cas_na_util.o

# 임시로 막음.
ifeq ($(strip $(CONFIG_MW_CAS_NAGRA)),y)
obj-y									+= ui/
obj-y									+= xmgr_cas_na_adapt.o
obj-y									+= xmgr_cas_na_cmd.o
obj-y									+= xmgr_cas_na_pincode.o
obj-$(CONFIG_MW_CAS_NAGRA_OP_SES)			+= xmgr_cas_na_ses.o
obj-$(CONFIG_MW_CAS_NAGRA_MEDIASET)		+= xmgr_cas_na_mediaset.o
obj-$(CONFIG_OP_SPAIN_TDT) 				+= xmgr_cas_na_tdt.o
obj-$(CONFIG_MW_CAS_NAGRA_RCS)			+= xmgr_cas_na_rcs.o
obj-$(CONFIG_MW_CAS_NAGRA_UPC)			+= xmgr_cas_na_upc.o
obj-$(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)	+= xmgr_cas_na_emmupdate.o
obj-$(CONFIG_MW_CAS_NAGRA_PRM)		+= xmgr_cas_na_prm.o
obj-$(CONFIG_SUPPORT_NOVA_APP)								+= xmgr_cas_na_prm_novaspread.o
obj-$(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)					+= xmgr_cas_na_prm_pvr.o
obj-$(CONFIG_DEBUG)						+= xmgr_cas_na_prm_dvl_integration_test.o
obj-$(CONFIG_DEBUG)						+= xmgr_cas_na_prm_dvl_test.o
obj-$(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)	+= xmgr_cas_na_parental_control.o

ifeq ($(strip $(CONFIG_MW_MM_PVR)),y)
obj-$(CONFIG_MW_CAS_NAGRA_DALTEST)		+= xmgr_cas_na_dal.o
endif
endif

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path

# Sub-CAS include path
