#
obj-y :=

ifneq ($(CONFIG_RECORD),)
obj-y += di_rec.o
endif

ifneq ($(CONFIG_PLAYBACK),)
ifneq ($(CONFIG_DI20),)
obj-y += di_pb.o
endif
ifneq ($(CONFIG_DI10),)
obj-y += di_pb_10.o
endif
endif

ifneq ($(CONFIG_PVR_EDIT),)
obj-y += di_pvr_edit.o
endif

ifeq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT),y)
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/sagelib/include
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/sage/srai/include
endif