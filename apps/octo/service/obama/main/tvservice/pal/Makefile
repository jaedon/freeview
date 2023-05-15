obj-y += sef/
obj-$(CONFIG_MW_EPG_DATACAROUSEL) += dsmcc/
obj-$(CONFIG_3RD_CRYPT) += crypt/
obj-$(CONFIG_MW_MULTIMEDIA) += recorder/
obj-$(CONFIG_MW_MULTIMEDIA) += playback/
obj-$(CONFIG_MW_MM_MEDIA) += media/
obj-y += video/
obj-y += audio/
obj-y += channel/
obj-y += panel/
obj-y += key/
obj-$(CONFIG_MW_CI) += ci/
obj-y += pes/
obj-y += pipe/
obj-y += power/
obj-y += sys/
obj-$(CONFIG_PROD_DEVICE_MIC) += mic/

ifeq ($(strip $(CONFIG_MW_CAS)),y)
ifneq "$(CONFIG_MW_CAS_NUM_SC_SLOT)" "0"
obj-y += sc/
endif
endif
ifeq ($(strip $(CONFIG_MW_CAS)),y)
obj-y += dsc/
else
ifeq ($(strip $(CONFIG_MW_CI_PLUS)),y)
obj-y += dsc/
endif
endif

obj-y += output/
obj-y += scaler/
obj-y += scart/
obj-$(CONFIG_MW_OUTPUT_HDMI) += hdmi/
obj-$(CONFIG_MW_CEC) += cec/
obj-y += demux/
obj-y += thumbnail/
obj-y += ringbuffer/
obj-y += siraw/
obj-$(CONFIG_MW_MM_TRANSCODING) += transcoder/
obj-y += param/
obj-y += fuse/

obj-y += datastream/

