obj-y += om_download.o
obj-$(CONFIG_OTA_IP) += om_download_ipmedia.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.oapimgr
# Additional include path
ccflags-y += -I$(OCTO_DIR)/oapi/lib
ccflags-y += -I$(OCTO_DIR)/oapi/server
ccflags-y += -I$(OCTO_DIR)/oapi
ccflags-y += -I$(OCTO_DIR)/oapi/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver

ifeq ($(CONFIG_DEBUG),y)
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/debug/include
else
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/release/include
endif

