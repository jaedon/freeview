#obj-y += oxserver.o
obj-y += obama_rpcprotocol.o
obj-$(CONFIG_PACKAGE_WEBSOCKET) += oapi_websocket.o
# ---- include path----
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mgr
ccflags-y += -I$(HLIB_DIR)/main/include
ccflags-y += -I$(OCTO_DIR)/include
ccflags-y += -I$(OCTO_DIR)/api/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver
ccflags-y += -I$(OCTO_TVMANAGER_DIR)/include
