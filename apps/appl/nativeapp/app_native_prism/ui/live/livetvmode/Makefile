obj-y += nx_prism_audio_ui.o
ifeq ($(strip $(CONFIG_PROD_M1)),y)
else
obj-y += nx_prism_channellist.o
endif
obj-y += nx_prism_channel_opt.o
obj-y += nx_prism_channel_addfav.o
obj-y += nx_prism_chinfo.o
obj-y += nx_prism_dcn.o
obj-y += nx_prism_dolby.o
obj-y += nx_prism_infohub.o
obj-y += nx_prism_mute.o
obj-y += nx_prism_no_service_mgr.o
obj-y += nx_prism_option.o
obj-y += nx_prism_subtitle_ui.o
obj-y += nx_prism_wide.o
obj-y += nx_prism_sys_volume.o
obj-y += nx_prism_radio.o

# Basic include path
ccflags-y += -I$(NATIVEAPP_TOP_DIR)/include

# Additional include path
cclfags-y += -IS(NATIVEAPP_UI_INCLUDE_DIR)/include
ccflags-y += -I$(NATIVEAPP_PORT_INCLUDE_DIR)/include