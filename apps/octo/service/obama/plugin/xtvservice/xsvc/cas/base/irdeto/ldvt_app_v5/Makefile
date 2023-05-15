LDVT_V5_PATH=$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ldvt_app_v5
FLASH_SETTING_PATH=$(HUMAX_TOP_DIR)/device/platform/driver/di/drv/include

obj-y += dvtapp.o
obj-y += environment.o
obj-y += irldvt_ir_api.o
obj-y += irldvt_output.o
obj-y += ldvtmain.o
obj-y += tir_api.o
obj-y += tir_nvr.o

# Additional include path
ccflags-y += -I$(LDVT_V5_PATH)/include
ccflags-y += -I$(LDVT_V5_PATH)/include/product
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include/ExportAPI
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/include/ImportSPI
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/irdeto/ld_v5/LoaderCore/Src/Shared

ccflags-y += -I$(HUMAX_TOP_DIR)/apps/octo/library/ondk/include/
ccflags-y += -I$(FLASH_SETTING_PATH)

#ccflags-y += -I./glib
#ccflags-y += -I./glib/include
#ccflags-y += -I./pal
#ccflags-y += -I./pal/include
#ccflags-y += -I./output/include
#ccflags-y += -I./ondk/include


# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas


