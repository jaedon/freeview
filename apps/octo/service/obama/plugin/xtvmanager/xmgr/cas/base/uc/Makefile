obj-y += xmgr_cas_ir.o
obj-y += xmgr_cas_ir_adapt.o
obj-y += xmgr_cas_ir_pincode.o
obj-y += xmgr_cas_ir_sc.o
obj-y += xmgr_cas_ir_util.o

ifeq ($(CONFIG_MW_CAS_IRDETO_PPV), y)
obj-y += xmgr_cas_ir_ppv.o
endif

ifeq ($(CONFIG_MW_CAS_IRDETO_PPV), y)
obj-y += xmgr_cas_ir_ippv.o
endif

obj-y += ui/

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path

