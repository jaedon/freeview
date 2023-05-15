#
so-y := mi_init.o
so-y += pumper_soc_brcm.o
so-y += decryptor_soc_brcm.o
so-y += audioplayback/
ifneq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT),y)
so-y += bcm_oemimpl.o
so-y += bcm_oem_init.o
so-y += bcm_oem_m2m_dma.o
else
ifneq ($(CONFIG_MEDIA20),y)
so-y += bcm_oemimpl.o
so-y += bcm_oem_init.o
so-y += bcm_oem_m2m_dma.o
endif
endif

HUMAX_EXTRA_CFLAG += -I.
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/security/common_crypto/include

ifeq    ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/calypso/debug/include
else
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/calypso/release/include
endif

#ifeq ($(PRDY_ROBUSTNESS_ENABLE),y)
HUMAX_EXTRA_CFLAG += -DPRDY_ROBUSTNESS=1
#endif

