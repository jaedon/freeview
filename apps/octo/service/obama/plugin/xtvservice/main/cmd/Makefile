obj-$(CONFIG_OP_FREESAT) += xsvc_cmd_fsat.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/main/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/local_include

