
obj-y += om_main.o
obj-y += om_platform.o
#obj-y += mgr_oapi_platform.o

obj-y += builder/
obj-y += media/
obj-y += meta/
obj-y += output/
obj-y += resource/
obj-y += sysinfo/
obj-y += scenario/
obj-y += cas/
obj-y += exe/
obj-y += swupdate/
obj-y += dsmcc/
obj-y += drmagent/
obj-y += extdev/
obj-y += download/

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.oapimgr
# Additional include path
ccflags-y += -I$(OCTO_DIR)/oapi/lib
ccflags-y += -I$(OCTO_DIR)/oapi/server
ccflags-y += -I$(OCTO_DIR)/oapi
ccflags-y += -I$(OCTO_DIR)/oapi/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver


