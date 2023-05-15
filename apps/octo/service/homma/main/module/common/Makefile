obj-y += pl_common.o
obj-$(CONFIG_HOMMA_CDI) += pl_crypt.o
obj-y += pl_fs.o
obj-y += pl_op.o
obj-y += pl_pvr.o
obj-y += pl_res_table.o
obj-y += pl_userdata.o
obj-y += pl_ch_list.o

ifeq ($(strip $(CONFIG_HOMMA_DMS)),y)
obj-y += streaming/
else
obj-$(CONFIG_HOMMA_SATIP) += streaming/
endif

# Basic include path
include $(OCTOSVC_SRC_DIR)/homma/main/module/include/Makefile.include.mxlib
