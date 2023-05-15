obj-$(CONFIG_DEBUG) += xsvc_main_cmd.o

obj-$(CONFIG_DEBUG) += cmd/

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/main/local_include

