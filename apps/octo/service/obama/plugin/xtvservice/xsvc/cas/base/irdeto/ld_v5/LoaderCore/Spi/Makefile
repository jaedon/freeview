obj-y += LoaderCoreSPI_Module.o
obj-y += LoaderCoreSPI_Device.o
obj-y += LoaderCoreSPI_Stdlib.o


# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
#
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include/ExportAPI
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include/ImportSPI
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/Src/Shared

FLASH_SETTING_PATH=$(HUMAX_TOP_DIR)/device/platform/driver/di/drv/include
ccflags-y += -I$(FLASH_SETTING_PATH)
