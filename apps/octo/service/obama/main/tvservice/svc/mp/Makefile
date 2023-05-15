ifeq ($(CONFIG_MW_MM_MEDIA),y)

ifeq ($(CONFIG_SUPPORT_IMMA), y)
obj-y += svc_cso.o
obj-y += svc_cso_sub.o
else
obj-y += svc_mp.o
obj-y += svc_mp_manager.o
obj-y += svc_mp_utils.o

#ifeq ($(CONFIG_MW_MM_DLNA_DMP_SI),y)
#obj-y += svc_mp_si.o
#obj-y += base/
#endif

endif

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/pvr/include
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/common/include
endif
