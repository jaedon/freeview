#
obj-y :=

ifeq ($(CONFIG_SCART_6417),y)
obj-y += stv6417.o
endif

ifeq ($(CONFIG_SCART_GPIO),y)
obj-y += scart_gpio.o
endif

ifeq ($(CONFIG_SCART_NULL),y)
obj-y += scart_null.o
endif
