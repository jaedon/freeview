#
ifneq ($(CONFIG_DI_MODULE_CRYPT_SHARED_LIB),y)
obj-y := di_crypt.o
endif

ifeq ($(CONFIG_KEY_PROVISIONING),y)
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/sagelib/include
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/sage/srai/include
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/sage/manufacturing/

ifeq ($(shell expr $(NEXUS_VERSION) ">=" 1501),1)
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/sage/manufacturing/include
else
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/sage/manufacturing/provisioning
endif
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/common_drm/include/tl
endif
