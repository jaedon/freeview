obj-$(CONFIG_MW_CI)				+= ci/
obj-$(CONFIG_MW_CAS_NAGRA)		+= nagra/
obj-$(CONFIG_MW_CAS_CONAX)		+= conax/
obj-$(CONFIG_MW_CAS_VIACCESS)	+= viaccess/
obj-$(CONFIG_MW_CAS_IRDETO_UCAS) += uc/
obj-$(CONFIG_MW_CAS_IRDETO_SCELL) += irdeto/	
obj-$(CONFIG_MW_CAS_VERIMATRIX) += verimatrix/

ifeq ($(strip $(CONFIG_MW_CAS)),y)
obj-y += resource/
obj-y += util/
else
ifeq ($(strip $(CONFIG_MW_CI)),y)
obj-y += resource/
obj-y += util/
endif
endif

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
export ccflags-y

