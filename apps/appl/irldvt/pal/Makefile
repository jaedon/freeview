obj-y += sef/
obj-y += channel/
obj-y += key/
obj-y += power/
obj-y += sys/

ifeq ($(strip $(CONFIG_MW_CAS)),y)
ifneq "$(CONFIG_MW_CAS_NUM_SC_SLOT)" "0"
#obj-y += sc/
endif
endif
ifeq ($(strip $(CONFIG_MW_CAS)),y)
#obj-y += dsc/
else
ifeq ($(strip $(CONFIG_MW_CI_PLUS)),y)
#obj-y += dsc/
endif
endif

obj-y += output/
obj-y += scaler/
obj-$(CONFIG_MW_OUTPUT_HDMI) += hdmi/
obj-y += hdmi/
obj-y += demux/
obj-y += param/
obj-y += fuse/



