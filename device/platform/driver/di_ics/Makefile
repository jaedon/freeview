#
#add by junsy (for media and iptuner)
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/driver/di/iptuner
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/bfile
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/utils
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/media
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/avi
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/asf
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready
inccflag-y += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/lib/playready/inc
HUMAX_EXTRA_CFLAG += -DDRM_BUILD_PROFILE=10 -DTARGET_LITTLE_ENDIAN=1

obj-y := di_mheg_ics_iptuner.o
obj-y += di_mheg_ics_iptuner_underflow.o
obj-y += di_mheg_ics_media.o

