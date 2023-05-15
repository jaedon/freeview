#
ifneq ($(CONFIG_DI_MODULE_USB_SHARED_LIB),y)

ifeq ($(CONFIG_DI_SHARED_LIB),y)
so-y := di_usb.o

ifeq ($(CONFIG_USB_HID),y)
so-y += di_usb_hid.o
endif

ifneq ($(CONFIG_USB_WIRELESS),)
so-y += di_usb_wireless.o
endif
else
ifeq ($(CONFIG_OS_UCOS),y)
obj-y := di_usb_ucos.o
else
obj-y := di_usb.o
endif
ifeq ($(CONFIG_USB_HID),y)
obj-y += di_usb_hid.o
endif

ifneq ($(CONFIG_USB_WIRELESS),)
ifneq ($(CONFIG_DINETWORK), y)
obj-y += di_usb_wireless.o
endif
endif
endif

endif
