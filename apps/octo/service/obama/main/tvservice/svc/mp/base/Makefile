obj-y += mw_mp_si_base.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/pvr/include
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/common/include

ifeq ($(CONFIG_MW_MM_DLNA),y)
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/media/include
endif
