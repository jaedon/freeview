obj-y += ir_msg_scell.o
obj-y += ir_svc.o
obj-y += ir_status.o

ifeq ($(strip $(CONFIG_MW_CAS_IRDETO_PVR)),y)
obj-y += ir_pvr.o
endif

obj-y += ir_ppv.o
ifeq ($(strip $(CONFIG_MW_CAS_IRDETO_SOL)),y)
obj-y += ir_sol.o
endif

ifeq ($(strip $(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)),y)
obj-y += ir_mr.o
endif

ifeq ($(strip $(CONFIG_DEBUG)),y)
obj-y += ir_debug.o
endif

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
