
#

HUMAX_PLATFORM_BASE=y
ifneq ($(CONFIG_BRCM_MOD_MSDRM_PD_SUPPORT),)
MSDRM_PD_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_MEDIA_ASF_SUPPORT),)
MEDIA_ASF_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_MEDIA_AVI_SUPPORT),)
MEDIA_AVI_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_OTPMSP_SUPPORT),)
OTPMSP_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_BVDC_MACROVISION),)
BVDC_MACROVISION=y
endif

ifneq ($(CONFIG_BRCM_MOD_KEYLADDER_SUPPORT),)
KEYLADDER_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_HDCP_SUPPORT),)
NEXUS_HDCP_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_BCEC_SUPPORT),)
BCEC_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_TEMP_MONITOR_SUPPORT),)
NEXUS_TEMP_MONITOR_SUPPORT=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_FRONTEND_4506),)
NEXUS_FRONTEND_4506=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_FRONTEND_4538),)
NEXUS_USE_FRONTEND_4538=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_FRONTEND_45216),)
NEXUS_FRONTEND_45216=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_FRONTEND_3461),)
NEXUS_FRONTEND_3461=y
endif

ifneq ($(CONFIG_BRCM_MOD_NEXUS_FRONTEND_3128),)
NEXUS_FRONTEND_3128=y
endif

ifneq ($(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP),)
TRANSCODING_32BIT_TIMESTAMP=y
endif

ifneq ($(BRCM_MOD_USE_SPI_FRONTEND),)
USE_SPI_FRONTEND=y
endif

ifneq ($(CONFIG_PVR),y)
NEXUS_FILE_SUPPORT=n
NEXUS_PLAYBACK_SUPPORT=n
NEXUS_RECORD_SUPPORT=n
endif

ifneq ($(CONFIG_HDMI_IN),y)
NEXUS_HDMI_INPUT_SUPPORT=n
endif

ifneq ($(CONFIG_TRANSCODER),y)
NEXUS_VIDEO_ENCODER_SUPPORT=n
NEXUS_STREAM_MUX_SUPPORT=n
NEXUS_FILE_MUX_SUPPORT=n
else
VIDEO_ENCODER_SUPPORT=y
endif

ifneq ($(CONFIG_HDMI_CEC),y)
NEXUS_CEC_SUPPORT=n
endif

ifneq ($(CONFIG_SMARTCARD),y)
NEXUS_SMARTCARD_SUPPORT=n
endif

ifneq ($(CONFIG_AUDIO_DECODER),y)
NEXUS_AUDIO_SUPPORT=n
NEXUS_SYNC_CHANNEL_SUPPORT=n
NEXUS_ASTM_SUPPORT=n
NEXUS_SIMPLE_DECODER_SUPPORT=n
endif

ifneq ($(CONFIG_VIDEO_DECODER),y)
ifeq ($(CONFIG_OS_UCOS),y)
NEXUS_VIDEO_DECODER_SUPPORT=n
else
#NEXUS_VIDEO_DECODER_SUPPORT=n
endif
NEXUS_PICTURE_DECODER_SUPPORT=n
endif

ifneq ($(CONFIG_VIDEO_DISPLAY),y)
NEXUS_HDMI_OUTPUT_SUPPORT=n
NEXUS_HDMI_DVO_SUPPORT=n
NEXUS_SURFACE_SUPPORT=n
NEXUS_DISPLAY_SUPPORT=n
NEXUS_GRAPHICS2D_SUPPORT=n
NEXUS_SURFACE_COMPOSITOR_SUPPORT=n
endif

ifneq ($(CONFIG_CRYPTO),y)
ifneq ($(CONFIG_HDCP),y)
ifneq ($(CONFIG_DSC),y)
NEXUS_SECURITY_SUPPORT=n
endif
endif
endif

ifneq ($(CONFIG_HDCP_22),)
SAGE_SUPPORT=y
endif

ifeq ($(CONFIG_OS_UCOS),y)
NEXUS_SERVER_SUPPORT=n
endif

#ifneq ($(CONFIG_XXX),y)
#NEXUS_TRANSPORT_SUPPORT=n
#NEXUS_DMA_SUPPORT=n
#NEXUS_INPUT_ROUTER_SUPPORT=n
#NEXUS_UART_SUPPORT=n
#endif

#ifeq ($(CONFIG_ABCD),y)
#V3D_SUPPORT=y
#NSK2_SUPPORT=y
#NSK2_EMM_SUPPORT=y
#NSK2_ECM_SUPPORT=y
#NSK2_SMARTCARD_SUPPORT=y
#NEXUS_DVB_CI_SUPPORT=y
#NEXUS_TOUCHPAD_SUPPORT=y
#NEXUS_TEMP_MONITOR_SUPPORT=y
#MSDRM_PRDY_SUPPORT=y
#GL_SUPPORT=y
#SAGE_SUPPORT=y
#NEXUS_MPOD_SUPPORT=y
#endif



include $(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/platforms/$(PLATFORM)/build/platform_app.inc