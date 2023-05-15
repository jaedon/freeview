HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/include/ai

#ifeq ($(CONFIG_NETAPP), y)
#include ${NETAPP_OUTPUT_DIR}/include/netapp.inc
#endif

ifneq ($(CONFIG_DI_MODULE_NETWORK_SHARED_LIB),y)
ifeq ($(CONFIG_ETHERNET), y)
obj-$(CONFIG_ETHERNET) += di_network.o
endif
endif

ifeq ($(CONFIG_ETHERNET), y)
obj-$(CONFIG_ETHERNET) += di_network_ext.o
endif

#ifneq ($(CONFIG_DI_MODULE_WIFI_SHARED_LIB),y)
#ifeq ($(CONFIG_NETAPP), y)
#obj-$(CONFIG_WIFI) += netapp/di_netapp.o
#endif
#endif

ifneq ($(CONFIG_DI_MODULE_WIFI_SHARED_LIB),y)
ifeq ($(CONFIG_WIFI), y)
#ifeq ($(CONFIG_NETAPP), y)
#obj-$(CONFIG_WIFI) += netapp/di_netapp_wifi.o
#else
obj-$(CONFIG_WIFI) += di_wifi.o
#endif
ifneq ($(CONFIG_ETHERNET), y)
obj-$(CONFIG_ETHERNET) += di_network.o
obj-$(CONFIG_ETHERNET) += di_network_ext.o
endif
endif
endif

ifneq ($(CONFIG_DI_MODULE_BT_SHARED_LIB),y)
ifeq ($(CONFIG_BLUETOOTH), y)
#ifeq ($(CONFIG_NETAPP), y)
#obj-$(CONFIG_BLUETOOTH) += netapp/di_netapp_bt.o
#else
obj-$(CONFIG_BLUETOOTH) += di_bt.o
obj-$(CONFIG_BLUETOOTH) += di_bt_audio.o
#endif
endif
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_FACTORY),y) #TEMP
obj-$(CONFIG_BLUETOOTH) += di_bt.o
endif

ifneq ($(CONFIG_DI_MODULE_PPPOE_SHARED_LIB),y)
ifeq ($(CONFIG_PPPOE), y)
obj-$(CONFIG_PPPOE) += di_pppoe.o
ifneq ($(CONFIG_ETHERNET), y)
obj-$(CONFIG_ETHERNET) += di_network.o
obj-$(CONFIG_ETHERNET) += di_network_ext.o
endif
endif
endif

# Additional include path
