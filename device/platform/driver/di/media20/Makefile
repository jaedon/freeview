#
#add by junsy (for media and iptuner)
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/bfile
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/utils
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/media
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/avi
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/asf
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/drm

ifeq ($(CONFIG_MEDIA),y)
obj-$(CONFIG_MEDIA20) += di_media.o
obj-$(CONFIG_MEDIA20) += iptuner/
endif
