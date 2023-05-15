############################################################
#     Copyright (c) 2003-2013, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdsp_raaga_img.inc $
# $brcm_Revision: Hydra_Software_Devel/29 $
# $brcm_Date: 1/30/13 10:34a $
#
# Module Description:
#
# Revision History:
#
# Created: 12/15/2010 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/dsp/build/bdsp_raaga_img.inc $
# 
# Hydra_Software_Devel/29   1/30/13 10:34a sriniva
# FWRAA-559: Renaming braaga files of dts lbr to dts_express
# 
# Hydra_Software_Devel/28   1/10/13 10:25a skalapur
# FWRAA-474:iLBC code merging to main latest
# 
# Hydra_Software_Devel/MAGNUM_ILBC_ISAC_integration/1   12/20/12 11:06a skalapur
# FWRAA-474:iLBC and iSAC integration
# 
# Hydra_Software_Devel/27   11/26/12 1:12p ananthan
# SW7425-4242: Add SCM3 interface and merge SCM to mainline.
# 
# Hydra_Software_Devel/26   11/7/12 12:51p ananthan
# SW7425-3443: Remove non-STB audio interfaces and firmware 1.Removing
# ADV,ABX, Custom ACF, Custom AVLP, Custom DBE, SRS CS, SRS EQ/HL, WMA
# Encode, AACLC encoder. 2.Removing reference to AC3ENC and having DD
# Transcoder getting added based on MS10 flag
# 
# Hydra_Software_Devel/25   8/30/12 1:00p shivak
# FWRAA-424 : Disabling VP8
# 
# Hydra_Software_Devel/24   8/23/12 12:15a rshijith
# FWRAA-424: Code modification to support VP8 encoder in raaga.
# 
# Hydra_Software_Devel/23   7/16/12 3:37p dlimaye
# SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
# and AMRWB decoder/encoder
# 
# Hydra_Software_Devel/22   6/19/12 3:15a gprasad
# SW7425-2952: DSP directory structure flattening
# 
# Hydra_Software_Devel/21   6/6/12 1:01p ananthan
# FWRAA-443: Add MAT Encoder support in Raaga
# 
# Hydra_Software_Devel/20   5/11/12 3:32p dlimaye
# FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
# sys_top_raaga changes
# 
# Hydra_Software_Devel/19   3/30/12 12:33a gprasad
# SW7425-2786: BDSP changes for adding MLP Passthru support
# 
# Hydra_Software_Devel/18   3/22/12 9:02p gprasad
# FWRAA-399: DTS LBR Support
# 
# Hydra_Software_Devel/17   3/22/12 6:50p gprasad
# FWRAA-399: DTS LBR SUPPORT
# 
# Hydra_Software_Devel/16   3/16/12 5:09p ramanatm
# SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
# 
# Hydra_Software_Devel/15   2/7/12 8:30p rshijith
# FWRAA-377 : Enabling raaga video encoder in mainline
# 
# Hydra_Software_Devel/14   2/3/12 12:25p ramanatm
# SW7425-1752:[7425] Add support for FLAC media file format
# 
# Hydra_Software_Devel/13   1/13/12 9:52p gprasad
# FWRAA-342: Adding support for SPEEX AEC
# 
# Hydra_Software_Devel/12   1/12/12 6:16p gprasad
# FWRAA-370: Fixing build errors observed when BDSP_MS10_SUPPORT=n and
# BDSP_AC3ENC_SUPPORT=y
# 
# Hydra_Software_Devel/11   12/29/11 5:55p gprasad
# FWRAA-370: G723.1 codec development on Raaga platform
# 
# Hydra_Software_Devel/10   12/5/11 1:57p ashoky
# FWRAA-330: Adding WMA encoder support.
# 
# Hydra_Software_Devel/9   11/8/11 1:40p ramanatm
# FWRAA-348:[7425] Vorbis bringup
# 
# Hydra_Software_Devel/8   10/20/11 5:46p gprasad
# FWRAA-359: Add support for ring buffer capture for cert testing
# 
# Hydra_Software_Devel/7   8/16/11 9:11a gprasad
# FWRAA-327: Adding g.7XX fw binaries for B0
# 
# Hydra_Software_Devel/6   7/13/11 3:00p gprasad
# SW7425-858: Runtime errors with BDSP_3RDPARTY_ALGO_SUPPORT=n - code
# cleanup
# 
# Hydra_Software_Devel/5   6/16/11 7:51p gprasad
# SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
# AudioEncode in BDSP
# 
# Hydra_Software_Devel/4   5/19/11 4:18p ashoky
# SW7425-477: [7425]Including mp3 encoder source for build.
# 
# Hydra_Software_Devel/3   5/19/11 12:02p gprasad
# SW7425-572: [7425] Adding VP6 support to basemodules
# 
# Hydra_Software_Devel/2   4/15/11 5:08p gprasad
# SW7425-342: DSP: Build fails if MS10 support is disabled
# 
# Hydra_Software_Devel/1   4/14/11 11:26a gprasad
# FWRAA-280: Adding support for fp-2011
# 
# Hydra_Software_Devel/2   4/6/11 11:45a gprasad
# SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
# folder
# 
# Hydra_Software_Devel/1   4/5/11 4:08p gprasad
# SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
# folder
# 
# Hydra_Software_Devel/16   3/24/11 5:25p jgarrett
# SW7422-146: Fixes for kernel mode and big endian
# 
# Hydra_Software_Devel/15   3/15/11 9:24p srajapur
# SW7422-356 : [7425] Added support for GenCbdItb.
# 
# Hydra_Software_Devel/14   3/8/11 12:04a srajapur
# SW7422-229:[7422]Added support for MS10 algorithm
# 
# Hydra_Software_Devel/13   3/5/11 5:05a srajapur
# SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
# Codebase
# 
# Hydra_Software_Devel/12   2/26/11 12:43a srajapur
# SW7422-293:[7422] Adding support for encode algorithms
# 
# Hydra_Software_Devel/11   2/16/11 5:40a srajapur
# SW7422-247 : [7422] Adding all audio post processing algorithms.
# 
# Hydra_Software_Devel/10   2/10/11 5:06p jgarrett
# SW7422-229: [7422] Fixed issue with overlapping DTS-HD and DTS-
# Broadcast FW images
# 
# Hydra_Software_Devel/9   2/10/11 7:04a srajapur
# SW7422-229:[7422]Added support for following audio codec algorithms
# ========== WMA - tested WMAPRO - tested DRA - tested DTSHD - tested
# DTSBROADCAST - tested LPCMDVD - tested PCMWAV PCM REALAUDIOLBR ADPCM
# ==========
# 
# Hydra_Software_Devel/8   2/3/11 3:16a srajapur
# SW7422-229 : [7422]Adding support for DDP decode.
# 
# Hydra_Software_Devel/7   1/29/11 6:50a srajapur
# SW7422-229 : [7422] Adding support for MPEG algorithm
# 
# Hydra_Software_Devel/6   1/29/11 5:03a srajapur
# SW7422-229 :[7422]Adding support for AAC, AACLOAS, AACSBR and
# AACSBRADTS algorithm.
# 
# Hydra_Software_Devel/4   1/19/11 2:56p jgarrett
# SW7422-191: Renaming PLATFORM_DEFINES to BDSP_DEFINES
# 
# Hydra_Software_Devel/3   1/13/11 7:34a gautamk
# SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
# includes - DSP Booting - Open time Download :  - Start Time Audio
# Decode algorithm download  - Message buffer interaction with DSP:  -
# Synchronized Interrupt:  - Async interrupts:  - Config Commands for
# Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
# Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
# level interrupt
# 
# Hydra_Software_Devel/1   12/15/10 6:47p jgarrett
# SW7422-146: Initial compileable prototype
# 
############################################################

#
# Conditional Options for this Makefile
#
# BDSP_AUDIO_SUPPORT (default=y) - Include audio support
# BDSP_VIDEO_SUPPORT (default=n) - Include video support
# BDSP_IMG_ONLY (default=n) - Include firmware only
# BDSP_IMG_EXTERNAL (default=n) - Include non-firmware files only
#

# Option to disable all/3rd-party algo support
BDSP_DEFAULT_ALGO_SUPPORT ?= y
BDSP_3RDPARTY_ALGO_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)

# Include standard files
BDSP_INCLUDES += $(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)

#
# All Firmware sources must be in BDSP_IMG_SOURCES, not BDSP_SOURCES.  Based on BDSP_IMG_EXTERNAL, choose to include the firmware files or not at the end.
#
BDSP_IMG_SOURCES += $(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/bdsp_raaga_img.c

# Vom table files should not be part of BDSP_IMG_SOURCES
ifneq ($(BDSP_IMG_ONLY),y)
BDSP_SOURCES += \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vom_algo_start_addr.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vom_table.c
endif

# Determine defines based on presence of files.  E.g. BDSP_AC3_SUPPORT, BDSP_DDP_SUPPORT, BDSP_MS11_SUPPORT, etc. 

# Based on algo defines, include proper firmware files.  
# Add firmware files here
BDSP_IMG_SOURCES += \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_rdbvars_${BCHP_CHIP}.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_system_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_system_data.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_syslib_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_algolib_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_idscommon_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vididscommon_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vididscommon_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm_task_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm1_digest.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm2_digest.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_cdb_passthru_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_cdb_passthru_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_cdb_passthru_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_version.c

#################################################################
#		ADD AUDIO ALGORITHM SUPPORT			#
#################################################################

#
#DDP Passthrough 
#
ifneq ($(BDSP_DDP_PASSTHRU_SUPPORT),n)
BDSP_DDP_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddp_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddp_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_DDP_PASSTHRU_SOURCES)),)
BDSP_DDP_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DDP_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_DDP_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DDP_PASSTHRU_SOURCES)
endif

#
#AC3 Passthrough 
#
ifneq ($(BDSP_AC3_PASSTHRU_SUPPORT),n)
BDSP_AC3_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ac3_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ac3_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_AC3_PASSTHRU_SOURCES)),)
BDSP_AC3_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AC3_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_AC3_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AC3_PASSTHRU_SOURCES)
endif


#
#AACSBR Passthrough 
#
ifneq ($(BDSP_AACSBR_PASSTHRU_SUPPORT),n)
BDSP_AACSBR_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aacheadts_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aacheadts_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aacheloas_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aacheloas_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_AACSBR_PASSTHRU_SOURCES)),)
BDSP_AACSBR_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AACSBR_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_AACSBR_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AACSBR_PASSTHRU_SOURCES)
endif


#
#MS10 - Support 
#
ifneq ($(BDSP_MS10_SUPPORT),n)
BDSP_MS10_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_ms_ddp_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_ms_ddp_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_ms_ddp_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_pulse_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_pulse_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dolby_pulse_decode.c

ifneq ($(wildcard $(BDSP_MS10_SOURCES)),)
ifneq ($(wildcard $(BDSP_DDP_PASSTHRU_SOURCES)),)
ifneq ($(wildcard $(BDSP_AC3_PASSTHRU_SOURCES)),)
ifneq ($(wildcard $(BDSP_AACSBR_PASSTHRU_SOURCES)),)
BDSP_MS10_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif
endif
endif

ifeq ($(BDSP_MS10_SUPPORT),y)
BDSP_DEFINES += BDSP_MS10_SUPPORT
BDSP_DEFINES += BDSP_AACSBR_SUPPORT
BDSP_DEFINES += BDSP_DDP_SUPPORT
BDSP_DEFINES += BDSP_AC3_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MS10_SOURCES)
endif


#
#DDP (non-MS10) 
#
ifneq ($(BDSP_MS10_SUPPORT),y)
ifneq ($(BDSP_DDP_SUPPORT),n)
BDSP_DDP_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddp_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddp_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddp_decode_tables.c

ifneq ($(wildcard $(BDSP_DDP_SOURCES)),)
ifneq ($(wildcard $(BDSP_DDP_PASSTHRU_SOURCES)),)
BDSP_DDP_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
BDSP_AC3_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif
endif

ifeq ($(BDSP_DDP_SUPPORT),y)
BDSP_DEFINES += BDSP_DDP_SUPPORT
BDSP_DEFINES += BDSP_AC3_SUPPORT
BDSP_DEFINES += BDSP_DDP_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DDP_SOURCES)
endif

#
#AC3 (non-MS10) 
#
ifneq ($(BDSP_MS10_SUPPORT),y)
ifneq ($(BDSP_DDP_SUPPORT),y)
ifneq ($(BDSP_AC3_SUPPORT),n)
BDSP_AC3_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ac3_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ac3_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ac3_decode_tables.c

ifneq ($(wildcard $(BDSP_AC3_SOURCES)),)
ifneq ($(wildcard $(BDSP_AC3_PASSTHRU_SOURCES)),)
BDSP_AC3_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif
endif

ifeq ($(BDSP_AC3_SUPPORT),y)
BDSP_DEFINES += BDSP_AC3_SUPPORT
BDSP_DEFINES += BDSP_AC3_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AC3_SOURCES)
endif
endif

#
#AACSBR (non-MS10) 
#
ifneq ($(BDSP_MS10_SUPPORT),y)
ifneq ($(BDSP_AACSBR_SUPPORT),n)
BDSP_AACSBR_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_decode_tables.c

ifneq ($(wildcard $(BDSP_AACSBR_SOURCES)),)
ifneq ($(wildcard $(BDSP_AACSBR_PASSTHRU_SOURCES)),)
BDSP_AACSBR_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif
endif

ifeq ($(BDSP_AACSBR_SUPPORT),y)
BDSP_DEFINES += BDSP_AACSBR_SUPPORT
BDSP_DEFINES += BDSP_AACSBR_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AACSBR_SOURCES)
endif


#
#MPEG Passthrough 
#
ifneq ($(BDSP_MPEG_PASSTHRU_SUPPORT),n)
BDSP_MPEG_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mpeg1_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mpeg1_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_MPEG_PASSTHRU_SOURCES)),)
BDSP_MPEG_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_MPEG_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_MPEG_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MPEG_PASSTHRU_SOURCES)
endif



#
#MPEG DECODE 
#
ifneq ($(BDSP_MPEG_SUPPORT),n)
BDSP_MPEG_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mpeg1_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mpeg1_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mpeg1_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_MPEG_SOURCES)),)
ifneq ($(wildcard $(BDSP_MPEG_PASSTHRU_SOURCES)),)
BDSP_MPEG_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif
endif

ifeq ($(BDSP_MPEG_SUPPORT),y)
BDSP_DEFINES += BDSP_MPEG_SUPPORT
BDSP_DEFINES += BDSP_MPEG_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MPEG_SOURCES)
endif


#
#WMA DECODE
#
ifneq ($(BDSP_WMA_SUPPORT),n)
BDSP_WMA_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_WMA_SOURCES)),)
BDSP_WMA_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_WMA_SUPPORT),y)
BDSP_DEFINES += BDSP_WMA_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_WMA_SOURCES)
endif

#
#WMAPRO DECODE
#
ifneq ($(BDSP_WMAPRO_SUPPORT),n)
BDSP_WMAPRO_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_pro_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_pro_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_pro_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_pro_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_wma_pro_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_WMAPRO_SOURCES)),)
BDSP_WMAPRO_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_WMAPRO_SUPPORT),y)
BDSP_DEFINES += BDSP_WMAPRO_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_WMAPRO_SOURCES)
endif

#
#LPCMDVD DECODE
#
ifneq ($(BDSP_LPCMDVD_SUPPORT),n)
BDSP_LPCMDVD_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dvdlpcm_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dvdlpcm_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_lpcm_decode_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_lpcm_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_lpcm_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_LPCMDVD_SOURCES)),)
BDSP_LPCMDVD_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_LPCMDVD_SUPPORT),y)
BDSP_DEFINES += BDSP_LPCMDVD_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_LPCMDVD_SOURCES)
endif


#
#DTSHD Passthrough 
#
ifneq ($(BDSP_DTSHD_PASSTHRU_SUPPORT),n)
BDSP_DTSHD_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_DTSHD_PASSTHRU_SOURCES)),)
BDSP_DTSHD_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DTSHD_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSHD_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DTSHD_PASSTHRU_SOURCES)
endif

#
#DTSHD DECODE 
#
ifneq ($(BDSP_DTSHD_SUPPORT),n)
BDSP_DTSHD_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_DTSHD_SOURCES)),)
ifneq ($(wildcard $(BDSP_DTSHD_PASSTHRU_SOURCES)),)
BDSP_DTSHD_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif

ifeq ($(BDSP_DTSHD_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSHD_SUPPORT
BDSP_DEFINES += BDSP_DTSHD_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DTSHD_SOURCES)
endif


#
#DTSLBR DECODE  (Change it to DTS MX latter even macro to BDSP_DTSMX_SUPPORT )
#
ifneq ($(BDSP_DTSLBR_SUPPORT),n) 
BDSP_DTSLBR_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_express_ids_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_express_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_express_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_express_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_express_decode_inter_frame.c
endif

ifneq ($(wildcard $(BDSP_DTSLBR_SOURCES)),)
BDSP_DTSLBR_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif

ifeq ($(BDSP_DTSLBR_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSLBR_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DTSLBR_SOURCES)
endif

#
#DTSBROADCAST Passthrough 
#
ifneq ($(BDSP_DTSBROADCAST_PASSTHRU_SUPPORT),n)
BDSP_DTSBROADCAST_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_DTSBROADCAST_PASSTHRU_SOURCES)),)
BDSP_DTSBROADCAST_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DTSBROADCAST_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSBROADCAST_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DTSBROADCAST_PASSTHRU_SOURCES)
endif

#
#DTSBROADCAST DECODE 
#
ifneq ($(BDSP_DTSBROADCAST_SUPPORT),n)
BDSP_DTSBROADCAST_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dtshd_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_DTSBROADCAST_SOURCES)),)
ifneq ($(wildcard $(BDSP_DTSBROADCAST_PASSTHRU_SOURCES)),)
BDSP_DTSBROADCAST_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif

ifeq ($(BDSP_DTSBROADCAST_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSBROADCAST_SUPPORT
BDSP_DEFINES += BDSP_DTSBROADCAST_PASSTHRU_SUPPORT
ifneq ($(BDSP_DTSHD_SUPPORT),y)
BDSP_IMG_SOURCES += $(BDSP_DTSBROADCAST_SOURCES)
endif
endif
#
#PCMWAV DECODE
#
ifneq ($(BDSP_PCMWAV_SUPPORT),n)
BDSP_PCMWAV_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcmwav_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcmwav_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcmwav_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcmwav_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcmwav_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_PCMWAV_SOURCES)),)
BDSP_PCMWAV_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_PCMWAV_SUPPORT),y)
BDSP_DEFINES += BDSP_PCMWAV_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_PCMWAV_SOURCES)
endif

#
#DRA Passthrough 
#
ifneq ($(BDSP_DRA_PASSTHRU_SUPPORT),n)
BDSP_DRA_PASSTHRU_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dra_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dra_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_DRA_PASSTHRU_SOURCES)),)
BDSP_DRA_PASSTHRU_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DRA_PASSTHRU_SUPPORT),y)
BDSP_DEFINES += BDSP_DRA_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DRA_PASSTHRU_SOURCES)
endif



#
#DRA DECODE 
#
ifneq ($(BDSP_DRA_SUPPORT),n)
BDSP_DRA_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dra_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dra_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dra_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_DRA_SOURCES)),)
ifneq ($(wildcard $(BDSP_DRA_PASSTHRU_SOURCES)),)
BDSP_DRA_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif
endif

ifeq ($(BDSP_DRA_SUPPORT),y)
BDSP_DEFINES += BDSP_DRA_SUPPORT
BDSP_DEFINES += BDSP_DRA_PASSTHRU_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DRA_SOURCES)
endif

#
#PCM PASSTHRU DECODER
#
ifneq ($(BDSP_PCM_SUPPORT),n)
BDSP_PCM_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcm_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_pcm_ids_inter_frame.c
ifneq ($(wildcard $(BDSP_PCM_SOURCES)),)
BDSP_PCM_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_PCM_SUPPORT),y)
BDSP_DEFINES += BDSP_PCM_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_PCM_SOURCES)
endif

#
#REALAUDIOLBR DECODE 
#
ifneq ($(BDSP_REALAUDIOLBR_SUPPORT),n)
BDSP_REALAUDIOLBR_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ralbr_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ralbr_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ralbr_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ralbr_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ralbr_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_REALAUDIOLBR_SOURCES)),)
BDSP_REALAUDIOLBR_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_REALAUDIOLBR_SUPPORT),y)
BDSP_DEFINES += BDSP_REALAUDIOLBR_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_REALAUDIOLBR_SOURCES)
endif

#
#ADPCM DECODE
#
ifneq ($(BDSP_ADPCM_SUPPORT),n)
BDSP_ADPCM_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_adpcm_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_adpcm_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_adpcm_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_adpcm_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_adpcm_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_ADPCM_SOURCES)),)
BDSP_ADPCM_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_ADPCM_SUPPORT),y)
BDSP_DEFINES += BDSP_ADPCM_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_ADPCM_SOURCES)
endif


#
#G711G726 DECODE
#
ifneq ($(BDSP_G711G726_SUPPORT),n)
BDSP_G711G726_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_G711G726_SOURCES)),)
BDSP_G711G726_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G711G726_SUPPORT),y)
BDSP_DEFINES += BDSP_G711G726_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G711G726_SOURCES)
endif


#
#G729 DECODE
#
ifneq ($(BDSP_G729_SUPPORT),n)
BDSP_G729_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_G729_SOURCES)),)
BDSP_G729_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G729_SUPPORT),y)
BDSP_DEFINES += BDSP_G729_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G729_SOURCES)
endif

#
#G723.1 DECODE
#
ifneq ($(BDSP_G723_1_SUPPORT),n)
BDSP_G723_1_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_G723_1_SOURCES)),)
BDSP_G723_1_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G723_1_SUPPORT),y)
BDSP_DEFINES += BDSP_G723_1_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G723_1_SOURCES)
endif

#
#SPEEX AEC
#
ifneq ($(BDSP_SPEEXAEC_SUPPORT),n)
BDSP_SPEEXAEC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_speexaec_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_speexaec_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_speexaec_inter_frame.c

ifneq ($(wildcard $(BDSP_SPEEXAEC_SOURCES)),)
BDSP_SPEEXAEC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SPEEXAEC_SUPPORT),y)
BDSP_DEFINES += BDSP_SPEEXAEC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SPEEXAEC_SOURCES)
endif

#
#VORBIS DECODE
#
ifneq ($(BDSP_VORBIS_SUPPORT),n)
BDSP_VORBIS_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vorbis_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vorbis_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vorbis_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vorbis_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vorbis_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_VORBIS_SOURCES)),)
BDSP_VORBIS_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_VORBIS_SUPPORT),y)
BDSP_DEFINES += BDSP_VORBIS_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_VORBIS_SOURCES)
endif

#
#MLP DECODE
#
ifneq ($(BDSP_MLP_SUPPORT),n)
BDSP_MLP_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_truehd_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_truehd_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_truehd_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_MLP_SOURCES)),)
BDSP_MLP_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_MLP_SUPPORT),y)
BDSP_DEFINES += BDSP_MLP_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MLP_SOURCES)
endif


#
#MLP PASSTHROUGH DECODE
#
ifneq ($(BDSP_MLP_PASSTHROUGH_SUPPORT),n)
BDSP_MLP_PASSTHROUGH_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mlp_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mlp_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mlp_passthrough_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mlp_passthrough_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mlp_passthrough_inter_frame.c

ifneq ($(wildcard $(BDSP_MLP_PASSTHROUGH_SOURCES)),)
BDSP_MLP_PASSTHROUGH_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_MLP_PASSTHROUGH_SUPPORT),y)
BDSP_DEFINES += BDSP_MLP_PASSTHROUGH_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MLP_PASSTHROUGH_SOURCES)
endif

#
#FLAC DECODE
#
ifneq ($(BDSP_FLAC_SUPPORT),n)
BDSP_FLAC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_flac_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_flac_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_flac_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_flac_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_flac_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_FLAC_SOURCES)),)
BDSP_FLAC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_FLAC_SUPPORT),y)
BDSP_DEFINES += BDSP_FLAC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_FLAC_SOURCES)
endif

#
#MAC DECODE
#
ifneq ($(BDSP_MAC_SUPPORT),n)
BDSP_MAC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mac_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mac_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mac_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mac_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mac_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_MAC_SOURCES)),)
BDSP_MAC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_MAC_SUPPORT),y)
BDSP_DEFINES += BDSP_MAC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MAC_SOURCES)
endif


#
#AMR DECODE / Passthrough
#
ifneq ($(BDSP_AMR_SUPPORT),n)
BDSP_AMR_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_AMR_SOURCES)),)
BDSP_AMR_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AMR_SUPPORT),y)
BDSP_DEFINES += BDSP_AMR_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AMR_SOURCES)
endif

#
#AMRWB DECODE / Passthrough
#
ifneq ($(BDSP_AMRWB_SUPPORT),n)
BDSP_AMRWB_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_AMRWB_SOURCES)),)
BDSP_AMRWB_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AMRWB_SUPPORT),y)
BDSP_DEFINES += BDSP_AMRWB_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AMRWB_SOURCES)
endif

#
#ILBC DECODE / Passthrough
#
ifneq ($(BDSP_ILBC_SUPPORT),n)
BDSP_ILBC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_ILBC_SOURCES)),)
BDSP_ILBC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_ILBC_SUPPORT),y)
BDSP_DEFINES += BDSP_ILBC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_ILBC_SOURCES)
endif

#
#ISAC DECODE 
#
ifneq ($(BDSP_ISAC_SUPPORT),n)
BDSP_ISAC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_ids_inter_frame.c

ifneq ($(wildcard $(BDSP_ISAC_SOURCES)),)
BDSP_ISAC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_ISAC_SUPPORT),y)
BDSP_DEFINES += BDSP_ISAC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_ISAC_SOURCES)
endif


#################################################################
#		ADD VIDEO ALGORITHM SUPPORT			#
#################################################################

#
#VP6 DECODE 
#
ifneq ($(BDSP_VP6_SUPPORT),n)
BDSP_VP6_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp6_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp6_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp6_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp6_decode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp6_decode_inter_frame.c

ifneq ($(wildcard $(BDSP_VP6_SOURCES)),)
BDSP_VP6_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_VP6_SUPPORT),y)
BDSP_DEFINES += BDSP_VP6_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_VP6_SOURCES)
endif

#################################################################
#		ADD VIDEO ENCODE ALGORITHM SUPPORT			#
#################################################################

#
#H264 ENCODE 
#
ifneq ($(BDSP_H264_ENCODE_SUPPORT),n)
BDSP_H264_ENCODE_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_h264_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_h264_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_h264_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_H264_ENCODE_SOURCES)),)
BDSP_H264_ENCODE_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_H264_ENCODE_SUPPORT),y)
BDSP_DEFINES += BDSP_H264_ENCODE_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_H264_ENCODE_SOURCES)
endif
#
#VP8 ENCODE 
#
ifneq ($(BDSP_VP8_ENCODE_SUPPORT),n)
BDSP_VP8_ENCODE_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp8_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp8_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_vp8_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_VP8_ENCODE_SOURCES)),)
BDSP_VP8_ENCODE_SUPPORT ?= n
endif
endif

ifeq ($(BDSP_VP8_ENCODE_SUPPORT),y)
BDSP_DEFINES += BDSP_VP8_ENCODE_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_VP8_ENCODE_SOURCES)
endif
#################################################################
#		ADD POST PROCESSING SUPPORT			#
#################################################################

#
#SRC Post Processing
#
ifneq ($(BDSP_SRC_SUPPORT),n)
BDSP_SRC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_src_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_src_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_src_tables.c

ifneq ($(wildcard $(BDSP_SRC_SOURCES)),)
BDSP_SRC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SRC_SUPPORT),y)
BDSP_DEFINES += BDSP_SRC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SRC_SOURCES)
endif

#
#FWMIXER Post Processing
#
ifneq ($(BDSP_FWMIXER_SUPPORT),n)
BDSP_FWMIXER_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mixer_ids.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_fw_mixer_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mixer_ids_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_fw_mixer_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_fw_mixer_tables.c

ifneq ($(wildcard $(BDSP_FWMIXER_SOURCES)),)
BDSP_FWMIXER_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_FWMIXER_SUPPORT),y)
BDSP_DEFINES += BDSP_FWMIXER_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_FWMIXER_SOURCES)
endif

#
#DSOLA Post Processing
#
ifneq ($(BDSP_DSOLA_SUPPORT),n)
BDSP_DSOLA_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dsola_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dsola_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dsola_tables.c

ifneq ($(wildcard $(BDSP_DSOLA_SOURCES)),)
BDSP_DSOLA_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DSOLA_SUPPORT),y)
BDSP_DEFINES += BDSP_DSOLA_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DSOLA_SOURCES)
endif

#
#CUSTOMVOICE Post processing
#
ifneq ($(BDSP_CUSTOMVOICE_SUPPORT),n)
BDSP_CUSTOMVOICE_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_customvoice_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_customvoice_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_customvoice_tables.c

ifneq ($(wildcard $(BDSP_CUSTOMVOICE_SOURCES)),)
BDSP_CUSTOMVOICE_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_CUSTOMVOICE_SUPPORT),y)
BDSP_DEFINES += BDSP_CUSTOMVOICE_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_CUSTOMVOICE_SOURCES)
endif


#
#SRSHD - SRS HD
#
ifneq ($(BDSP_SRSHD_SUPPORT),n)
BDSP_SRSHD_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_trusurroundhd_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_trusurroundhd_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_trusurroundhd_tables.c

ifneq ($(wildcard $(BDSP_SRSHD_SOURCES)),)
BDSP_SRSHD_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SRSHD_SUPPORT),y)
BDSP_DEFINES += BDSP_SRSHD_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SRSHD_SOURCES)
endif

#
#SRSTRUVOL - SRS Tru Volume
#
ifneq ($(BDSP_SRSTRUVOL_SUPPORT),n)
BDSP_SRSTRUVOL_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_tvol_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_tvol_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_srs_tvol_tables.c

ifneq ($(wildcard $(BDSP_SRSTRUVOL_SOURCES)),)
BDSP_SRSTRUVOL_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SRSTRUVOL_SUPPORT),y)
BDSP_DEFINES += BDSP_SRSTRUVOL_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SRSTRUVOL_SOURCES)
endif


#
#AVL - Broadcom Auto Volume Leveller
#
ifneq ($(BDSP_AVL_SUPPORT),n)
BDSP_AVL_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_avl_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_avl_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_avl_tables.c

ifneq ($(wildcard $(BDSP_AVL_SOURCES)),)
BDSP_AVL_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AVL_SUPPORT),y)
BDSP_DEFINES += BDSP_AVL_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AVL_SOURCES)
endif

#
#BRCM3DSURROUND - Broadcom 3D SURROUND
#
ifneq ($(BDSP_BRCM3DSURROUND_SUPPORT),n)
BDSP_BRCM3DSURROUND_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_3dsurround_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_3dsurround_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_brcm_3dsurround_tables.c

ifneq ($(wildcard $(BDSP_BRCM3DSURROUND_SOURCES)),)
BDSP_BRCM3DSURROUND_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_BRCM3DSURROUND_SUPPORT),y)
BDSP_DEFINES += BDSP_BRCM3DSURROUND_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_BRCM3DSURROUND_SOURCES)
endif

#
#GENCDBITB - Generate CDB ITB
#
ifneq ($(BDSP_GENCDBITB_SUPPORT),n)
BDSP_GENCDBITB_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_gen_cdbitb_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_gen_cdbitb_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_gen_cdbitb_tables.c

ifneq ($(wildcard $(BDSP_GENCDBITB_SOURCES)),)
BDSP_GENCDBITB_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_GENCDBITB_SUPPORT),y)
BDSP_DEFINES += BDSP_GENCDBITB_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_GENCDBITB_SOURCES)
endif

#
#BTSC ENCODER - BTSC Encoder
#
ifneq ($(BDSP_BTSCENC_SUPPORT),n)
BDSP_BTSCENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_btscenc_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_btscenc_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_btscenc_tables.c

ifneq ($(wildcard $(BDSP_BTSCENC_SOURCES)),)
BDSP_BTSCENC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_BTSCENC_SUPPORT),y)
BDSP_DEFINES += BDSP_BTSCENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_BTSCENC_SOURCES)
endif


#################################################################
#		ADD ENCODE SUPPORT				#
#################################################################


#
#DTS Encoder
#
ifneq ($(BDSP_DTSENC_SUPPORT),n)
BDSP_DTSENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dts_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_DTSENC_SOURCES)),)
BDSP_DTSENC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DTSENC_SUPPORT),y)
BDSP_DEFINES += BDSP_DTSENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DTSENC_SOURCES)
endif

#
#AC3 Encoder
#
ifeq ($(BDSP_MS10_SUPPORT),y)
BDSP_AC3ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dd_transcode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dd_transcode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dd_transcode_inter_frame.c
endif

ifneq ($(wildcard $(BDSP_AC3ENC_SOURCES)),)
BDSP_AC3ENC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif

ifeq ($(BDSP_MS10_SUPPORT),y)
BDSP_DEFINES += BDSP_AC3ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AC3ENC_SOURCES)
endif

#
#AACHE Encoder
#
ifneq ($(BDSP_AACHEENC_SUPPORT),n)
BDSP_AACHEENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_aache_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_AACHEENC_SOURCES)),)
BDSP_AACHEENC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AACHEENC_SUPPORT),y)
BDSP_DEFINES += BDSP_AACHEENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AACHEENC_SOURCES)
endif

#
#MP3 Encoder
#
ifneq ($(BDSP_MP3ENC_SUPPORT),n)
BDSP_MP3ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mp3_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mp3_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_mp3_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_MP3ENC_SOURCES)),)
BDSP_MP3ENC_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_MP3ENC_SUPPORT),y)
BDSP_DEFINES += BDSP_MP3ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_MP3ENC_SOURCES)
endif



#
#G711G726 Encoder
#
ifneq ($(BDSP_G711G726ENC_SUPPORT),n)
BDSP_G711G726ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g711_g726_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_G711G726ENC_SOURCES)),)
BDSP_G711G726ENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G711G726ENC_SUPPORT),y)
BDSP_DEFINES += BDSP_G711G726ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G711G726ENC_SOURCES)
endif


#
#G729 Encoder
#
ifneq ($(BDSP_G729ENC_SUPPORT),n)
BDSP_G729ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g729_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_G729ENC_SOURCES)),)
BDSP_G729ENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G729ENC_SUPPORT),y)
BDSP_DEFINES += BDSP_G729ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G729ENC_SOURCES)
endif

#
#G723.1 Encoder
#
ifneq ($(BDSP_G723_1ENC_SUPPORT),n)
BDSP_G723_1ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g723_1_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_G723_1ENC_SOURCES)),)
BDSP_G723_1ENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G723_1ENC_SUPPORT),y)
BDSP_DEFINES += BDSP_G723_1ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G723_1ENC_SOURCES)
endif


#
#G722 Encoder
#
ifneq ($(BDSP_G722ENC_SUPPORT),n)
BDSP_G722ENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g722_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g722_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_g722_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_G722ENC_SOURCES)),)
BDSP_G722ENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_G722ENC_SUPPORT),y)
BDSP_DEFINES += BDSP_G722ENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_G722ENC_SOURCES)
endif

#
#AMR Encoder
#
ifneq ($(BDSP_AMRENC_SUPPORT),n)
BDSP_AMRENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amr_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_AMRENC_SOURCES)),)
BDSP_AMRENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AMRENC_SUPPORT),y)
BDSP_DEFINES += BDSP_AMRENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AMRENC_SOURCES)
endif

#
#AMRWB Encoder
#
ifneq ($(BDSP_AMRWBENC_SUPPORT),n)
BDSP_AMRWBENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_amrwb_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_AMRWBENC_SOURCES)),)
BDSP_AMRWBENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_AMRWBENC_SUPPORT),y)
BDSP_DEFINES += BDSP_AMRWBENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_AMRWBENC_SOURCES)
endif

#
#ILBC Encoder
#
ifneq ($(BDSP_ILBCENC_SUPPORT),n)
BDSP_ILBCENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ilbc_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_ILBCENC_SOURCES)),)
BDSP_ILBCENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_ILBCENC_SUPPORT),y)
BDSP_DEFINES += BDSP_ILBCENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_ILBCENC_SOURCES)
endif


#
#ISAC Encoder
#
ifneq ($(BDSP_ISACENC_SUPPORT),n)
BDSP_ISACENC_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_encode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_encode_tables.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_isac_encode_inter_frame.c

ifneq ($(wildcard $(BDSP_ISACENC_SOURCES)),)
BDSP_ISACENC_SUPPORT ?= $(BDSP_DEFAULT_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_ISACENC_SUPPORT),y)
BDSP_DEFINES += BDSP_ISACENC_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_ISACENC_SOURCES)
endif

#
#DV258 - DOLBY VOLUME
#
ifneq ($(BDSP_DV258_SUPPORT),n)
BDSP_DV258_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dv258_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dv258_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_dv258_tables.c

ifneq ($(wildcard $(BDSP_DV258_SOURCES)),)
BDSP_DV258_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DV258_SUPPORT),y)
BDSP_DEFINES += BDSP_DV258_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DV258_SOURCES)
endif

#
#DDRE - DOLBY DIGITAL RE-ENCODE
#
ifneq ($(BDSP_DDRE_SUPPORT),n)
BDSP_DDRE_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddre_code.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddre_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_ddre_tables.c

ifneq ($(wildcard $(BDSP_DDRE_SOURCES)),)
BDSP_DDRE_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_DDRE_SUPPORT),y)
BDSP_DEFINES += BDSP_DDRE_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_DDRE_SOURCES)
endif

ifeq ($(RAP_RBUF_CAP_SUPPORT),y)
    BDSP_DEFINES += BDSP_FW_RBUF_CAPTURE
endif

#
# SCM 1 ALGORITHM
#
ifneq ($(BDSP_SCM1_SUPPORT),n)
BDSP_SCM1_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm1_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm1_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm1_decode_tables.c

ifneq ($(wildcard $(BDSP_SCM1_SOURCES)),)
BDSP_SCM1_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SCM1_SUPPORT),y)
BDSP_DEFINES += BDSP_SCM1_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SCM1_SOURCES)
endif

ifeq ($(RAP_RBUF_CAP_SUPPORT),y)
    BDSP_DEFINES += BDSP_FW_RBUF_CAPTURE
endif


#
# SCM 2 ALGORITHM
#
ifneq ($(BDSP_SCM2_SUPPORT),n)
BDSP_SCM2_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm2_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm2_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm2_decode_tables.c

ifneq ($(wildcard $(BDSP_SCM2_SOURCES)),)
BDSP_SCM2_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SCM2_SUPPORT),y)
BDSP_DEFINES += BDSP_SCM2_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SCM2_SOURCES)
endif

#
# SCM 3 ALGORITHM
#
ifneq ($(BDSP_SCM3_SUPPORT),n)
BDSP_SCM3_SOURCES := \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm3_decode.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm3_decode_inter_frame.c \
	$(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)/braaga_scm3_decode_tables.c

ifneq ($(wildcard $(BDSP_SCM3_SOURCES)),)
BDSP_SCM3_SUPPORT ?= $(BDSP_3RDPARTY_ALGO_SUPPORT)
endif
endif

ifeq ($(BDSP_SCM3_SUPPORT),y)
BDSP_DEFINES += BDSP_SCM3_SUPPORT
BDSP_IMG_SOURCES += $(BDSP_SCM3_SOURCES)
endif


ifeq ($(RAP_RBUF_CAP_SUPPORT),y)
    BDSP_DEFINES += BDSP_FW_RBUF_CAPTURE
endif
# This must be last
#
ifneq ($(BDSP_IMG_EXTERNAL),y)
BDSP_DEFINES += BDSP_IMG_SUPPORT=1
BDSP_SOURCES += $(BDSP_IMG_SOURCES)
endif
