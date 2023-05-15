obj-y += dmx/
obj-y += icc/
obj-y += osy/
obj-y += sec/

ifneq ($(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE),y) 
obj-y += bdc/
obj-y += dsc/
obj-$(CONFIG_MW_CAS_NAGRA_OP_SES) += scr/
else
obj-y += bdc/
obj-$(CONFIG_MW_CAS_NAGRA_DALTEST) += dsc/
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_NOCS_3_X),y) 
obj-y += cert/
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME),y)
obj-y += vfs/
endif

#obj-$(CONFIG_MW_CAS_NAGRA_FEATURE_RPH) += rph/
obj-y += rph/

obj-$(CONFIG_MW_CAS_NAGRA_NOCS_3_X) += cert/

# MW include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
