obj-$(CONFIG_MW_MULTIMEDIA) += nx_prism_play_media.o
obj-$(CONFIG_MW_MM_MEDIA) += nx_prism_play_subtitle_option.o
obj-$(CONFIG_MW_MM_MEDIA) += nx_prism_play_subtitle_sync.o
obj-$(CONFIG_MW_MM_PVR) += nx_prism_play_rec.o
obj-$(CONFIG_MW_MM_PVR) += nx_prism_play_tsr.o

ifeq ($(filter $(CONFIG_MW_MM_PVR) $(CONFIG_MW_MM_MEDIA),y),y)
obj-y += nx_prism_playmgr.o
obj-y += nx_prism_playbar.o
endif

# Additional include path
ccflags-y += -I../../../../../../../make/include

