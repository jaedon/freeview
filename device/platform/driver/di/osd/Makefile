#
ifneq ($(CONFIG_PRODUCT_IMAGE_FACTORY),)
obj-y := di_osd.o
else
ifeq ($(CONFIG_DIRECTFB),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
obj-y := di_osd_dfb.o
else
#obj-y := di_osd_dfb_octo.o
obj-y := di_osd_dfb_octo.o
endif
else
obj-y := di_osd.o
endif
endif
