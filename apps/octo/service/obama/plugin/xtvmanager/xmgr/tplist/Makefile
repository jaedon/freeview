obj-y								+= models/
obj-y								+= base/
obj-$(CONFIG_MW_CH_SATELLITE)		+= base_sat/
obj-$(CONFIG_MW_CH_CABLE)			+= base_cab/
obj-$(CONFIG_MW_CH_TERRESTRIAL)		+= base_ter/

obj-$(CONFIG_OP_FREESAT)			+= freesat/
obj-$(CONFIG_OP_ERTELECOM)			+= ertelecom/

obj-y		+= xmgr_tplist.o
obj-y		+= pmgr_tplist.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path

