NEXUS_PLATFORM ?= $(PLATFORM)
NEXUS_TOP = $(DRM_ROOT_PATH)/../../../nexus

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
include $(NEXUS_TOP)/../BSEAV/lib/media/playready.inc

#NEXUS_CFLAGS := $(NEXUS_CFLAGS) $(addprefix -D,$(NEXUS_APP_DEFINES)) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS))
NEXUS_CFLAGS := -DBDBG_DEBUG_BUILD=1 -DNEXUS_SERVER_SUPPORT=1 -D_GNU_SOURCE=1 -DLINUX -pipe -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -DUCLIBC_VERSION="libuClibc*" -DNEXUS_PLATFORM_LEGACY_PARSER_BAND -DNEXUS_PLATFORM_LEGACY_TIMEBASE -DNEXUS_HAS_SPI_FRONTPANEL=1
NEXUS_CFLAGS += $(addprefix -D,$(NEXUS_APP_DEFINES)) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS))
NEXUS_CFLAGS += $(addprefix -D,$(BMEDIA_DEFINES))
NEXUS_CFLAGS += $(addprefix -I,$(BMEDIA_INCLUDES))
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/security/common_crypto/include
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/std
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/std/config
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/dbg
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/err
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/mem
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/chp
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/commonutils/lst
NEXUS_CFLAGS += -I$(NEXUS_TOP)/../magnum/basemodules/reg


ifeq ($(STANDALONE_COMPILE), y)
include $(DRM_SOURCE_PATH)/tools.mak

DRM_CFLAGS += -DDRM_BUILD_PROFILE=10
DRM_CFLAGS += -DDRM_SUPPORT_LOCKING=0
DRM_CFLAGS += -DDRM_SUPPORT_MULTI_THREADING=0
DRM_CFLAGS += -DDRM_SUPPORT_DATASTORE_PREALLOC=1
DRM_CFLAGS += -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1
DRM_CFLAGS += -DDRM_SUPPORT_FORCE_ALIGN=0
DRM_CFLAGS += -DDRM_SUPPORT_ASSEMBLY=1
DRM_CFLAGS += -DDRM_SUPPORT_PRECOMPUTE_GTABLE=0
DRM_CFLAGS += -DDRM_SUPPORT_TRACING=0
DRM_CFLAGS += -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0
DRM_CFLAGS += -DDRM_SUPPORT_EMBEDDED_CERTS=0
DRM_CFLAGS += -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0
DRM_CFLAGS += -DDRM_SUPPORT_NONVAULTSIGNING=1
DRM_CFLAGS += -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0
DRM_CFLAGS += -D_DATASTORE_WRITE_THRU=1
DRM_CFLAGS += -D_ADDLICENSE_WRITE_THRU=0
DRM_CFLAGS += -DDRM_HDS_COPY_BUFFER_SIZE=32768
DRM_CFLAGS += -DDRM_TEST_SUPPORT_NET_IO=0
DRM_CFLAGS += -DUSE_PK_NAMESPACES=0
DRM_CFLAGS += -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=0
DRM_CFLAGS += -DDRM_KEYFILE_VERSION=3
DRM_CFLAGS += -DDBG=1
DRM_CFLAGS += -D_CRT_SECURE_NO_DEPRECATE
DRM_CFLAGS += -D_CRT_OBSOLETE_NO_DEPRECATE
DRM_CFLAGS += -D_CRT_SECURE_FORCE_DEPRECATE
DRM_CFLAGS += -D_X86_
#DRM_CFLAGS += -D_M_IX86
DRM_CFLAGS += -DTARGET_LITTLE_ENDIAN=1
DRM_CFLAGS += $(DRM_MODULE_CFLAGS)
DRM_CFLAGS += -DPRDY_ROBUSTNESS=1

DRM_INC += -I$(DRM_SOURCE_PATH)/inc -I$(DRM_SOURCE_PATH)/results -I$(DRM_SOURCE_PATH)/certs -I$(DRM_SOURCE_PATH)/xmr
DRM_INC += -I$(DRM_SOURCE_PATH)/ndtinc -I$(DRM_SOURCE_PATH)/keyfile -I$(DRM_SOURCE_PATH)/keyfileformat 
DRM_INC += -I$(DRM_SOURCE_PATH)/oem/common/inc -I$(DRM_SOURCE_PATH)/oem/ansi/inc -I$(DRM_SOURCE_PATH)/oem/ansi/trace
DRM_INC += -I$(DRM_SOURCE_PATH)/tools/common

NEXUS_DIR = $(DRM_SOURCE_PATH)/../../../../nexus
#DRM_INC += -I$(NEXUS_DIR)/modules/core/7346/include
#DRM_INC += -I$(NEXUS_DIR)/modules/dma/7346/include
#DRM_INC += -I$(NEXUS_DIR)/modules/security/7346/include
DRM_INC += -I$(NEXUS_DIR)/extensions/security/keyladder/7346/include
DRM_INC += -I$(NEXUS_DIR)/extensions/security/usercmd/7346/include
#DRM_INC += -I$(NEXUS_DIR)/../BSEAV/api/include/linux

#DRM_INC += -DBDBG_DEBUG_BUILD=1 -DNEXUS_SERVER_SUPPORT=1 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -DNEXUS_PLATFORM_LEGACY_PARSER_BAND -DNEXUS_PLATFORM_LEGACY_TIMEBASE -DNEXUS_HAS_SPI_FRONTPANEL=1 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/base/include/linuxuser/config -DBCHP_CHIP=7346 -DBCHP_VER=BCHP_VER_B0 -DBINT_REENTRANT_CONFIG=BINT_REENTRANT -DBMEM_REENTRANT_CONFIG=BMEM_REENTRANT -DB_HAS_DRM=1 -DB_HAS_MSDRM_PD=1 -DB_HAS_MSDRM_PRDY=1 -DB_HAS_MSDRM_PRDY_20=1 -DDBG=1 -DDRM_BUILD_PROFILE=10 -DDRM_HDS_COPY_BUFFER_SIZE=32768 -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=0 -DDRM_KEYFILE_VERSION=3 -DDRM_SUPPORT_ASSEMBLY=1 -DDRM_SUPPORT_DATASTORE_PREALLOC=1 -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0 -DDRM_SUPPORT_EMBEDDED_CERTS=0 -DDRM_SUPPORT_FORCE_ALIGN=0 -DDRM_SUPPORT_LOCKING=0 -DDRM_SUPPORT_MULTI_THREADING=0 -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1 -DDRM_SUPPORT_NONVAULTSIGNING=1 -DDRM_SUPPORT_PRECOMPUTE_GTABLE=0 -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0 -DDRM_SUPPORT_TRACING=0 -DDRM_TEST_SUPPORT_NET_IO=0 -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0 -DMACROVISION_SUPPORT=1 -DNEXUS_BASE_OS_linuxuser=1 -DNEXUS_DRM_BUILD=1 -DNEXUS_FRONTEND_7346 -DNEXUS_FRONTEND_AST -DNEXUS_HAS_KEYLADDER_SUPPORT=1 -DNEXUS_KEYLADDER=1 -DNEXUS_MODE_client -DNEXUS_OTPMSP=1 -DNEXUS_PLATFORM=97346 -DNEXUS_PROFILE_OS_linuxuser -DNEXUS_SECURITY_CHIP_SIZE=40 -DNEXUS_SECURITY_EXT_KEY_IV -DNEXUS_SECURITY_HAS_ASKM=1 -DNEXUS_SECURITY_SC_VALUE -DNEXUS_SYNC_CHANNEL_SARNOFF_LIPSYNC_OFFSET_SUPPORT=1 -DNEXUS_USERCMD=1 -DPLATFORM=97346 -DPRDY_ROBUSTNESS=1 -DTARGET_LITTLE_ENDIAN=1 -DUSE_PK_NAMESPACES=0 -D_ADDLICENSE_WRITE_THRU=0 -D_CRT_OBSOLETE_NO_DEPRECATE -D_CRT_SECURE_FORCE_DEPRECATE -D_CRT_SECURE_NO_DEPRECATE -D_DATASTORE_WRITE_THRU=1 -D_X86_ -DNEXUS_HAS_PLATFORM -DNEXUS_HAS_CORE -DNEXUS_HAS_PWM -DNEXUS_HAS_I2C -DNEXUS_HAS_GPIO -DNEXUS_HAS_LED -DNEXUS_HAS_IR_INPUT -DNEXUS_HAS_IR_BLASTER -DNEXUS_HAS_INPUT_CAPTURE -DNEXUS_HAS_UHF_INPUT -DNEXUS_HAS_KEYPAD -DNEXUS_HAS_SPI -DNEXUS_HAS_SECURITY -DNEXUS_HAS_CEC -DNEXUS_HAS_DMA -DNEXUS_HAS_TRANSPORT -DNEXUS_HAS_VIDEO_DECODER -DNEXUS_HAS_SIMPLE_DECODER -DNEXUS_HAS_AUDIO -DNEXUS_HAS_SURFACE -DNEXUS_HAS_SURFACE_COMPOSITOR -DNEXUS_HAS_INPUT_ROUTER -DNEXUS_HAS_GRAPHICS2D -DNEXUS_HAS_DISPLAY -DNEXUS_HAS_ASTM -DNEXUS_HAS_SYNC_CHANNEL -DNEXUS_HAS_HDMI_OUTPUT -DNEXUS_HAS_SMARTCARD -DNEXUS_HAS_BASE -DNEXUS_HAS_FILE -DNEXUS_HAS_PLAYBACK -DNEXUS_HAS_RECORD -DNEXUS_HAS_FRONTEND  -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/platforms/97346/include -include /home/juyou/project/hdr1000s/device/make/hdr1000s/autoconf.h -DHUMAX_PLATFORM_BASE -g -Os -DPIC -fpic -DBAPE_RAAGA=1 -DBCEC_SUPPORT -DBDSP_AACHEENC_SUPPORT -DBDSP_AACSBR_PASSTHRU_SUPPORT -DBDSP_AACSBR_SUPPORT -DBDSP_ABX_SUPPORT -DBDSP_AC3ENC_SUPPORT -DBDSP_AC3_PASSTHRU_SUPPORT -DBDSP_AC3_SUPPORT -DBDSP_ADPCM_SUPPORT -DBDSP_ADV_SUPPORT -DBDSP_AMR_SUPPORT -DBDSP_AUDIO_SUPPORT=1 -DBDSP_AVL_SUPPORT -DBDSP_BRCM3DSURROUND_SUPPORT -DBDSP_BTSCENC_SUPPORT -DBDSP_CUSTOMACF_SUPPORT -DBDSP_CUSTOMAVLP_SUPPORT -DBDSP_CUSTOMDBE_SUPPORT -DBDSP_CUSTOMVOICE_SUPPORT -DBDSP_DDP_PASSTHRU_SUPPORT -DBDSP_DDP_SUPPORT -DBDSP_DDRE_SUPPORT -DBDSP_DRA_PASSTHRU_SUPPORT -DBDSP_DRA_SUPPORT -DBDSP_DSOLA_SUPPORT -DBDSP_DTSBROADCAST_PASSTHRU_SUPPORT -DBDSP_DTSBROADCAST_SUPPORT -DBDSP_DTSENC_SUPPORT -DBDSP_DTSHD_PASSTHRU_SUPPORT -DBDSP_DTSHD_SUPPORT -DBDSP_DTSLBR_SUPPORT -DBDSP_DV258_SUPPORT -DBDSP_FLAC_SUPPORT -DBDSP_FWMIXER_SUPPORT -DBDSP_G711G726ENC_SUPPORT -DBDSP_G711G726_SUPPORT -DBDSP_G722ENC_SUPPORT -DBDSP_G723_1ENC_SUPPORT -DBDSP_G723_1_SUPPORT -DBDSP_G729ENC_SUPPORT -DBDSP_G729_SUPPORT -DBDSP_GENCDBITB_SUPPORT -DBDSP_H264_ENCODE_SUPPORT -DBDSP_IMG_SUPPORT=1 -DBDSP_LPCMDVD_SUPPORT -DBDSP_MAC_SUPPORT -DBDSP_MLP_PASSTHROUGH_SUPPORT -DBDSP_MLP_SUPPORT -DBDSP_MP3ENC_SUPPORT -DBDSP_MPEG_PASSTHRU_SUPPORT -DBDSP_MPEG_SUPPORT -DBDSP_MS10_SUPPORT -DBDSP_PCMWAV_SUPPORT -DBDSP_PCM_SUPPORT -DBDSP_RAAGA_SUPPORT=1 -DBDSP_REALAUDIOLBR_SUPPORT -DBDSP_SPEEXAEC_SUPPORT -DBDSP_SRC_SUPPORT -DBDSP_SRSCSTD_SUPPORT -DBDSP_SRSEQHL_SUPPORT -DBDSP_SRSHD_SUPPORT -DBDSP_SRSTRUVOL_SUPPORT -DBDSP_VIDEO_ENCODE_SUPPORT=1 -DBDSP_VIDEO_SUPPORT=1 -DBDSP_VORBIS_SUPPORT -DBDSP_WMAENC_SUPPORT -DBDSP_WMAPRO_SUPPORT -DBDSP_WMA_SUPPORT -DBHDM_3D_SUPPORT -DBHSM_AEGIS_SUPPORT -DBHSM_PWRMGMT_SUPPORT=0 -DBHSM_SC_VALUE -DBHSM_SECURE_PI_SUPPORT_KEYLADDER=1 -DBHSM_SECURE_PI_SUPPORT_MISC=0 -DBHSM_SECURE_PI_SUPPORT_OTPMSP=1 -DBSP_GENERIC -DBXVD_P_AVD_ARC600 -DB_HAS_ASF=1 -DB_HAS_AVI=1 -DB_HAS_NAV_PLAYER=1 -DGRCPACKET_SUPPORT=1 -DUNIFIED_ITB_SUPPORT -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/asf -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/avi -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/bcmplayer/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/bfile -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/media -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/mpeg2_ts_parse -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/otfpvr -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/security/common_crypto/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/thumbnail -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/tspsi -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/../BSEAV/lib/utils -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/chp -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/chp/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/chp/7346/rdb/b0 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/dbg -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/dsp/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/dsp/include/raaga -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/dsp/src/raaga/firmware/fp2011 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/err -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/int -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/int/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/kni/linuxuser -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/mem -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/reg -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/std -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/std/types/linuxuser -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/basemodules/tmr/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/afl/ -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/avc/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/fmt -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/img -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/lst -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/mrc/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/mth -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/pxl -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/rdc/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/sur/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/udp -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/vlc -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/commonutils/xdm/ -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ape/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ape/include/raaga -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ape/src/raaga -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ast -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ast/g3 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/ast/g3/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/cec/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/grc/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hdm/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/common -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/common/aegis -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/cust_generic3A -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/cust_generic3A/aegis -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/keyladder -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/otpmsp -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/otpmsp/aegis -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/hsm/7346/b0/user_misc_cmd -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/i2c/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/icp/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/irb/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/kir/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/kpd/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/led/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/mmd/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/pwm/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/scd/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/spi/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/uhf/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/vbi/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/vdc/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/xpt/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/xpt/7346/uif_image/b0 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/portinginterface/xvd/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/astmlib/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/framework/7346/ -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/grclib/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/hdcplib/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/pcrlib/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/pvrlib -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/synclib/7346 -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/../magnum/syslib/vbilib -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/extensions/security/keyladder/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/core/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/dma/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/security/7346/include  -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/frontend/common/src -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/platforms/97346/bin/syncthunk -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/frontend/common/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/frontend/7346/include/  -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/base/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/base/include/linuxuser -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/core/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/gpio/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/i2c/7346/include -I/home/juyou/project/hdr1000s/device/platform/sdk/driver/nexus/modules/spi/7346/include

ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
CFLAGS += -DPRDY_ROBUSTNESS=1
endif
endif

BINDIR = $(DRM_ROOT_PATH)/output/bin/playready
ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
LIBDIR = $(DRM_ROOT_PATH)/output/lib/playready_robust
else
LIBDIR = $(DRM_ROOT_PATH)/output/lib/playready
endif

DRM_LDFLAGS +=

DRM_TOOLS_DIR      = Tools
DRM_TEST_DIR       = test

DRM_CFLAGS += -g -fshort-wchar -O3 -fpic  -I. -I..

# GGM: added to allow -shared option. There are references to libsettop.so
LNKFLAGS		   = -static

TOOLS_INCLUDE	   = -I$(DRM_ROOT_PATH)/tools/common
CLIB             = $(CC)

AR_FLAGS = rcv

CFLAGS += $(DRM_CFLAGS) $(DRM_INC) $(NEXUS_CFLAGS)
LDFLAGS += $(DRM_LDFLAGS)

SYSLIBS		   = -lc
OBJDIR		   = .
OBJOUT		   = -o

OBJEXT		 = o
LIBEXT		 = a

OBJLIST = $(SOURCES:.c=.o)

ifneq ($(TARGET),)
TARGETNAME = libdrm$(TARGET).$(LIBEXT)
endif

BUILDDIRS = $(DIRS:%=build-%)
CLEANDIRS = $(DIRS:%=clean-%)

SYSLIBS = $(wildcard $(LIBDIR)/*)

all: $(BUILDDIRS) $(TARGETNAME) $(TARGETEXE)

check:
	@echo $(CFLAGS)
#	@echo $(MAGNUM_PRECOMPILED_OBJECTS)

clean: $(CLEANDIRS)
ifneq ($(TARGETEXE),)
	$(RM) $(OBJLIST) $(OBJLIST:%.o=%.d)
	$(RM) $(TARGETEXE)
endif
ifneq ($(TARGETNAME),)
	$(RM) $(OBJLIST) $(OBJLIST:%.o=%.d)
	$(RM) $(TARGETNAME)
endif

distclean : clean
	$(RM) $(BINDIR)/*
	$(RM) $(LIBDIR)/*

$(TARGETEXE): $(OBJLIST)
	@$(CC) $(LDFLAGS) $^ -Wl,--start-group $(SYSLIBS) -Wl,--end-group -o $(BINDIR)/$@
	
%.o: %.c
	@$(CC) $(CFLAGS) -MT $@ -MD -MF $(@:%.o=%.d) -c -o $@ $<

%.a: $(OBJLIST)
	$(AR) $(AR_FLAGS) $@ $(OBJLIST)
	$(CP) $@ $(LIBDIR)
	$(AR) -x $@ 
	$(CP) *.o $(LIBDIR)

%.so: $(OBJLIST)
	@$(CC) $(CFLAGS) -shared -o $@ $(OBJLIST)
	$(CP) $@ $(LIBDIR)

$(BUILDDIRS):
	@echo "Building directory [$(@:build-%=%)]"
	@$(MAKE) -C $(@:build-%=%)

$(CLEANDIRS):
	@echo "Cleaning directory [$(@:clean-%=%)]"
	@$(MAKE) -C $(@:clean-%=%) clean
	
-include $(OBJLIST:.o=.d)

.PHONY: subdirs $(DIRS)
.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
.PHONY: all clean

