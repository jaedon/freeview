

obj-y += om_drmagent.o


# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.oapimgr
# Additional include path
ccflags-y += -I$(OCTO_DIR)/oapi/lib
ccflags-y += -I$(OCTO_DIR)/oapi/server
ccflags-y += -I$(OCTO_DIR)/oapi
ccflags-y += -I$(OCTO_DIR)/oapi/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver

ifeq ($(CONFIG_SUPPORT_IMMA), y)
ccflags-y += -I$(PACKAGES_DIR)/playready/include
obj-y += om_drmagent_util_http.o
endif
