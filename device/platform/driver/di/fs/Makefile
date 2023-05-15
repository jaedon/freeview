#
ifneq ($(CONFIG_DI_MODULE_FS_SHARED_LIB),y)
ifeq ($(CONFIG_DI_SHARED_LIB),y)
so-y := di_fs.o
so-y += di_fs_partition.o
else
ifeq ($(CONFIG_OS_UCOS),y)
obj-y := di_fs_ucos.o
else
obj-y := di_fs.o
obj-y += di_fs_partition.o
endif
endif
endif
