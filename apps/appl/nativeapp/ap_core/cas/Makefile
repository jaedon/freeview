obj-y += nx_core_cas.o
obj-$(CONFIG_MW_CAS_IRDETO) += nx_core_cas_ir.o
obj-$(CONFIG_MW_CAS_NAGRA) += nx_core_cas_na.o

# Basic include path
ccflags-y += -I$(NATIVEAPP_TOP_DIR)/include
ccflags-y += -I$(NATIVEAPP_CORE_INCLUDE_DIR)/include

# Additional include path
ccflags-y += -I$(NATIVEAPP_PORT_INCLUDE_DIR)/include