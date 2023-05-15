#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-y += library/
obj-y += ota/
obj-$(CONFIG_MW_SI_AIT) += hbbtv/

## base satellite
obj-$(CONFIG_MW_CH_SATELLITE) += eu_sat/

## base terrestrial
obj-$(CONFIG_MW_CH_TERRESTRIAL) += eu_dtt/

## base cable
obj-$(CONFIG_MW_CH_CABLE) += eu_cab/

## base cable

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
