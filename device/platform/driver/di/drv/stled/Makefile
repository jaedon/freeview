#
ifeq ($(CONFIG_STLED_316S),y)
obj-y := drv_stled_316s.o
else
ifeq ($(CONFIG_STLED_MC2003),y)
obj-y := drv_stled_mc2003.o
else
ifeq ($(CONFIG_STLED_MC2302),y)
obj-y := drv_stled_mc2302.o
else
#error need device type
endif
endif
endif
