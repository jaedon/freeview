#
obj-y := di_ci.o
ifeq ($(CONFIG_USE_EXTERNAL_CI_CONTROLLER),y)
obj-y += pdriver_cimax.o
else
obj-y += pdriver.o
endif
