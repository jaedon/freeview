obj-y += remoteapp_udp.o
obj-y += remoteapp_parser.o
obj-y += remoteapp_main.o
#obj-y += test_client_remoteapp.o

# Additional include path
ccflags-y += -I../../../../make/include
ifeq ($(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE),y)
ccflags-y += -I$(HUMAX_TOP_DIR)/3rd_party/dlna/mxlib
endif
