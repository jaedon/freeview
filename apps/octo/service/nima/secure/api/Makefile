obj-$(CONFIG_SUPPORT_NIMA) += ncapi.o
obj-$(CONFIG_SUPPORT_NIMA) += ncapi_codec.o

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
obj-$(CONFIG_SUPPORT_NIMA) += ncapi_client.o
else
obj-$(CONFIG_SUPPORT_NIMA) += ncapi_client_secoff.o
endif


obj-$(CONFIG_SUPPORT_NIMA) += modules/ 
