obj-y += Sha1.o
obj-y += LedDisplay.o
obj-y += Crc.o
obj-y += Rsa.o
obj-y += Sha256.o
obj-y += Pkcs.o
obj-y += ErrorReport.o
obj-y += ModuleManager.o
obj-y += Bbcb.o
obj-y += KeyUpdate.o
obj-y += Bit.o
obj-y += LoaderPartition.o
obj-y += VbCipher.o
##obj-y += UartInterface.o
##obj-y += USBInterface.o


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