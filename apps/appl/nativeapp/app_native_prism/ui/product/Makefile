obj-$(CONFIG_MW_CAS_CONAX) +=	cas_cx/
obj-$(CONFIG_MW_CAS_IREDETO) +=	cas_ir/
obj-$(CONFIG_MW_CAS_NAGRA) +=	cas_na/
obj-$(CONFIG_OP_SHAHID_APPS) += op_shahid/
obj-$(CONFIG_OP_MBC) += op_mbc/
obj-$(CONFIG_OP_AMX) += op_amx/

# Basic include path
ccflags-y += -I$(NATIVEAPP_TOP_DIR)/include

# Additional include path
cclfags-y += -IS(NATIVEAPP_UI_INCLUDE_DIR)/include
ccflags-y += -I$(NATIVEAPP_PORT_INCLUDE_DIR)/include