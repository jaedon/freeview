obj-y += om_media_play.o

obj-y += om_media_play_live.o
obj-$(CONFIG_MW_MM_PVR) += om_media_play_tsrpb.o
obj-$(CONFIG_MW_MM_PVR) += om_media_play_pvrpb.o
obj-$(CONFIG_MW_MM_MEDIA) += om_media_play_mediapb.o
obj-$(CONFIG_3RD_MHEG_SI_MONITORING) += om_media_play_mheg.o

obj-$(CONFIG_MW_MM_PVR) += om_media_record.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.oapimgr
# Additional include path
ccflags-y += -I$(OCTO_DIR)/oapi/lib
ccflags-y += -I$(OCTO_DIR)/oapi/server
ccflags-y += -I$(OCTO_DIR)/oapi
ccflags-y += -I$(OCTO_DIR)/oapi/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver

ifeq ($(CONFIG_DEBUG),y)
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/debug/include
else
ccflags-$(CONFIG_SUPPORT_IMMA) += -I$(PACKAGES_DIR)/calypso/release/include
endif

