obj-y += ir_api.o
obj-y += ir_nvr.o
obj-y += ir_evt.o
obj-y += ir_msg.o
ifneq ($(CONFIG_MW_CAS_IRDETO_LOADER_V5),y)
obj-y += ir_sup/
endif
obj-y += ir_uc/
obj-y += ir_os/

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path