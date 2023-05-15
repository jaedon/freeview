############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: brap_fmm130.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 9/11/12 4:38p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/rap/build/brap_fmm130.inc $
# 
# Hydra_Software_Devel/1   9/11/12 4:38p srajapur
# SW7420-2341:Remove symbolic links and flatten the RAP directory
# 
# Hydra_Software_Devel/NEW_DIR_SW7420_2341/1   9/7/12 11:36a srajapur
# SW7420-2341:Remove symbolic links and flatten the RAP directory
# 
# Hydra_Software_Devel/77   4/5/11 3:14p speter
# SWDTV-5975: [35233] Add Support for 35233 Raaga PI
# 
# Hydra_Software_Devel/76   3/22/11 4:43p gautamk
# SW7400-3019: Fix build with GL_SUPPORT=n NEXUS_MODE=proxy. Prevent
# build of RAP PI files in linuxuser.proxy.
# 
# Hydra_Software_Devel/75   1/31/11 1:25p bhanus
# SWBLURAY-24449:[7640]Initial Code check-in
# 
# Hydra_Software_Devel/74   12/7/10 6:04p srajapur
# SW7231-5:[7231]Adding  raptor PI for 7231
# 
# Hydra_Software_Devel/73   12/3/10 4:46p srajapur
# SW7552-10: [7552] Adding RAP PI support for 7552
# 
# Hydra_Software_Devel/72   12/3/10 2:35p srajapur
# SW7344-6 : [7344] Adding RAP PI Support for 7344/7346
# 
# Hydra_Software_Devel/71   11/15/10 12:47p speter
# SW35125-15: [35125] Adding support for 35125
# 
# Hydra_Software_Devel/70   9/6/10 10:49a srajapur
# SW7425-17 : [7425] Adding RAP PI support for 7425
# 
# Hydra_Software_Devel/69   8/15/10 10:52p srajapur
# SW7358-10:[7358] Adding 7358 chip support.
# 
# Hydra_Software_Devel/68   7/20/10 11:22a srajapur
# SW7422-9:[7422] Adding support for 7422 raptor
# 
# Hydra_Software_Devel/67   5/10/10 11:51a srajapur
# SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
# 
# Hydra_Software_Devel/66   2/10/10 11:42a speter
# SWRAA-1:[35230] Merging emulation code to main line of development
# 
# Hydra_Software_Devel/65   12/16/09 5:27a srajapur
# SW7400-2602 : [7400] Support DSOLA PB80 / PB120 on BCM7400
# 
# Hydra_Software_Devel/64   11/12/09 11:09a bhanus
# SW7468-26:[7468]Adding 7468 support
# 
# Hydra_Software_Devel/63   9/16/09 3:03p srajapur
# SW7400-2505: [7400] Add TSM log support on Nexus as it existed for
# Native settop.  Build with BRAP_SUPPORT_TSM_LOG=y.
# 
# Hydra_Software_Devel/62   9/15/09 11:32a gdata
# SW7630-23:[7630] Porting code for Grain.
# 
# Hydra_Software_Devel/61   7/21/09 6:55p srajapur
# PR53240 : [7403] DRA decoder development on 7403 chip
# 
# Hydra_Software_Devel/60   6/12/09 6:16p srajapur
# PR53489: [7401,7400]  Added support for SRS True Volume certification
# and added PCM decode algorithm
# 
# Hydra_Software_Devel/59   6/10/09 3:39p srajapur
# PR55890 : [7550] Modifying 7405 code for 7550( as per RDB and 7550
# spec)
# 
# Hydra_Software_Devel/58   5/27/09 9:25p bhanus
# PR 55213:[7125,7340,7342]Adding support for 7125/7340/7342
# 
# Hydra_Software_Devel/57   4/6/09 4:42p srajapur
# PR 53700 : [7400,7401,7403,7118] Added Downloadable scheduler code for
# code memory optimization
# 
# Hydra_Software_Devel/56   3/18/09 8:39p srajapur
# PR53363 : [7400] Adding DTS passthrough support
# 
# Hydra_Software_Devel/55   3/18/09 7:55p srajapur
# PR43833 : [7400]  BCM7400 support codec for AAC HE V2
# 
# Hydra_Software_Devel/54   3/11/09 1:47p bhanus
# PR 13647[DVD]: [7635]Adding support for 7635
# 
# Hydra_Software_Devel/53   2/27/09 6:48p srajapur
# PR51362: [7401 7403] Adding SRS VIQ
# 
# Hydra_Software_Devel/52   2/25/09 6:21p srajapur
# PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
# 
# Hydra_Software_Devel/51   1/23/09 6:15p gautamk
# PR48850: [7335] Adding 7336 Folder
# 
# Hydra_Software_Devel/50   1/12/09 10:58p srajapur
# PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
# 
# Hydra_Software_Devel/49   12/18/08 11:46a gautamk
# PR50474: [7413]  adding support for RAP 7413 chip.
# 
# Hydra_Software_Devel/48   11/12/08 7:39p srajapur
# PR48893 :[7400] 74XX merged release for 7400,7401,7403 and 7118 [For
# more info see Description]
# PR45669: [7403] Adding support for DTS Certification.
# PR43467 : [7403] Implementation of DRA on 7403
# PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
# Motorola Requirements
# PR42285 : [7401] Added Audio descriptor support
# PR44899 :[7401] Independent delay on spdif causes left and right
# channel to be out of phase
# PR44614 : [7400] [7401] Few seconds of noise on SPDIF/HDMI during PCM
# to Compressed & vice-a-versa transition
# PR42342: [7401] Added Downmix Method parameter in the AACHE user config
# structure for Sumitomo
# 
# Hydra_Software_Devel/47   8/18/08 4:13p srajapur
# PR 41308 : [7400] Added MPEG Layer 3 LSF(SRC) support needed for DivX
# 
# Hydra_Software_Devel/46   8/4/08 12:20p srajapur
# PR 41308 : [7400] Removed MPEG Layer 3 LSF support needed for DivX
# 
# Hydra_Software_Devel/45   7/31/08 7:52p srajapur
# PR42131 : [7118] Added support for AACHEV2
# 
# Hydra_Software_Devel/44   7/28/08 1:29p bhanus
# PR 45109 : [7420]Adding support for 7420
# 
# Hydra_Software_Devel/43   7/17/08 9:28a gdata
# PR43971: [7601] Changing the name to 7601 from 7443
# 
# Hydra_Software_Devel/42   6/12/08 9:29p srajapur
# PR 41308 : [7400] MPEG Layer 3 LSF support needed for DivX
# 
# Hydra_Software_Devel/41   6/10/08 11:19a bhanus
# PR 43438 : [7443] Fixing Compilation break for other chips.
# 
# Hydra_Software_Devel/40   6/9/08 12:27p bhanus
# PR 43438 : [7443] Adding support for 7443.
# 
# Hydra_Software_Devel/39   5/19/08 4:44p srajapur
# PR42245 : [7400] Added 7.1 Channel Pass-Thru Support for DDP on HDMI
# output
# 
# Hydra_Software_Devel/38   5/6/08 7:54p srajapur
# PR42131: [7403] Added support for AACHEV2
# 
# Hydra_Software_Devel/37   4/28/08 3:38p speter
# PR 38950: [3548] Fixing compilation for 3548/3556
# 
# Hydra_Software_Devel/36   4/28/08 2:26p srajapur
# PR42131 : [7401] Added support for AACHEV2
# 
# Hydra_Software_Devel/35   4/11/08 6:36p srajapur
# PR41579:[7401]WMA certification - Added API to program the config
# structure for WMA.
# 
# Hydra_Software_Devel/34   2/28/08 8:40p srajapur
# PR37419 : [7400] Added I2S Pass-through of two compressed ouput
# 
# Hydra_Software_Devel/33   1/21/08 5:17p srajapur
# PR 38674 : [7401,7403,7118] Implemented AC3 pass-thru without requiring
# AC3 license for 7401/7403/7118 - Added check for 7400 compilation
# before including  passthrough files.
# 
# Hydra_Software_Devel/32   1/17/08 8:45p srajapur
# PR 38674 : [7401,7403,7118] Implemented AC3 pass-thru without requiring
# AC3 license for 7401/7403/7118
# 
# Hydra_Software_Devel/31   11/14/07 3:50p sushmit
# PR 36877: [7335] Adding 7335 RAP PI Folders & Files.
# 
# Hydra_Software_Devel/30   11/2/07 5:20p srajapur
# PR 36680 : [7401,7118,7403,7400] Removed ifdef to support  LPCM
# algorithm
# 
# Hydra_Software_Devel/29   9/20/07 5:02p sushmit
# PR 35140: [7325] Adding 7325 file
# 
# Hydra_Software_Devel/27   8/8/07 2:49p bselva
# PR 33582:[7400,7401,7118,7403] AC3 codec is compiled based upon the
# export variable RAP_AC3_SUPPORT
# 
# Hydra_Software_Devel/26   8/8/07 2:19p bselva
# PR 33582:[7400,7401,7118,7403] AC3 codec is compiled based upon the
# export variable RAP_AC3_SUPPORT
# 
# Hydra_Software_Devel/24   8/1/07 11:59a bselva
# PR33588:[7400] Corrected the kernel mode build breaking when DDP
# support is enabled for 7400
# 
# Hydra_Software_Devel/23   7/19/07 6:41p bselva
# PR 33176:[7400]Checking in the PI changes for phase 4.0 release
# 
# Hydra_Software_Devel/22   7/16/07 5:04p bselva
# PR 28274:[7403]Adding the support for DVD LPCM
# 
# Hydra_Software_Devel/21   7/12/07 10:37a gautamk
# PR32256: [7401] Adding passthru support for wma pro
# 
# Hydra_Software_Devel/20   7/6/07 12:08p gautamk
# PR32256: [7401] Adding new algo WMA PRO for 7401
# 
# Hydra_Software_Devel/19   7/5/07 4:09p gautamk
# PR32256: [7401] Adding new algo WMA PRO for 7401
# 
# Hydra_Software_Devel/RAP_7401_WMA_PRO/1   6/18/07 12:26p gautamk
# PR32256: [7401] Adding new algo WMA PRO for 7401
# 
# Hydra_Software_Devel/18   6/12/07 2:59p ahulse
# PR23100: B_CONFIG_IMAGE support for 7401
# 
# Hydra_Software_Devel/17   6/1/07 3:47p bselva
# PR 26501:[7401 Family]Added the new executable names to be compiled for
# aache support.
# 
# Hydra_Software_Devel/16   5/31/07 6:07p bselva
# PR 26501:[7401 Family]Added the support for AAC-HE multichannel and
# AAC-HE pass through
# 
# Hydra_Software_Devel/15   5/22/07 12:28p rjain
# PR 28422:[7401 family] including DSOLA files
# 
# Hydra_Software_Devel/14   5/9/07 1:56p sushmit
# PR 27646: Adding brap_7405.inc
# 
# Hydra_Software_Devel/13   3/8/07 2:47p bselva
# PR 27078: Corrected the include path for DDP convert files.
# 
# Hydra_Software_Devel/12   2/28/07 9:11a rjain
# PR 24247: Porting LSF (with SRC) for 7118 and 7403
# 
# Hydra_Software_Devel/11   2/13/07 11:05a nitinb
# PR 25346: Modified for 3563
# 
# Hydra_Software_Devel/10   2/9/07 1:46p rjain
# PR 24247: changing name of SRC post processing files
# 
# Hydra_Software_Devel/9   2/9/07 11:01a kagrawal
# PR 25607: [7440] Renamed 7440/brap.inc to 7440/brap_7440.inc
# 
# Hydra_Software_Devel/8   2/9/07 9:51a kagrawal
# PR 25607: Removed brap_src for non-7440 chips
# 
# Hydra_Software_Devel/6   2/8/07 4:26p rjain
# PR 24247 : adding support for SRC as post processing module
# 
# Hydra_Software_Devel/5   2/8/07 2:59p bhanus
# PR 25607 : Modified to include rap.inc from 7440 directory.
# 
# Hydra_Software_Devel/4   1/24/07 10:55a rjain
# PR 27078: fixing compilation for 7401
# 
# Hydra_Software_Devel/3   1/23/07 10:50a bselva
# PR 27078: Checking in the changes for Ac3 separation from DDP
# executables
# 
# Hydra_Software_Devel/2   1/19/07 2:27p rjain
# PR27004: reverting back to the macros RAP_DDP_SUPPORT and
# RAP_WMA_SUPPORT
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################
# Define this module
MAGNUM_MODULES += BRAP

ifeq ($(BRAP_SUPPORT_TSM_LOG),y)
BRAP_DEFINES += BRAP_SUPPORT_TSM_LOG=1
endif

# Module Includes
BRAP_INCLUDES += $(MAGNUM)/portinginterface/rap/src/fmm130
BRAP_INCLUDES += $(MAGNUM)/portinginterface/rap/include/fmm130
BRAP_INCLUDES += $(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500

BRAP_SOURCES += \
    $(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_img.c

ifeq ($(BRAP_IMG_ONLY), )
# Module Sources
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_dec.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_priv.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_int_priv.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_dsp.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_dspchn.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_fmm.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_rbuf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_srcch.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_dstch.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_mixer.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_spdiffm.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_op.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_rm.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_hifidac.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_spdiffm_fw.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_pb.c\
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_cap.c\
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_pvr.c\
	$(MAGNUM)/portinginterface/rap/src/fmm130/brap_enc.c
endif

BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_l1_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_l2_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mpeg_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_inter_frame.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_inter_stage.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_scratch.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_stg0.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_mp3e_stg1.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_iboot.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_scheduler_code.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_scheduler_data.c  \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aac_downmix.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aacplus_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aacplus_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_encoder_iboot.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_encoder_scheduler_code.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_encoder_scheduler_data.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_scheduler_dwd_code.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_src_postprocess_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_src_postprocess.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_src_interframe_buf.c 

ifeq ($(RAP_SRSTRUVOL_SUPPORT),y)
BRAP_DEFINES += RAP_SRSTRUVOL_SUPPORT
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_srs_TruVolume_post_process.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_srs_TruVolume_table.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_srs_TruVolume_interframe_buf.c
endif

ifeq ($(RAP_SRSTRUVOL_CERTIFICATION),y)
BRAP_DEFINES += RAP_SRSTRUVOL_CERTIFICATION
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_pcm_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_pcm_framesync.c
endif
	
ifeq ($(RAP_DOLBYVOLUME_SUPPORT),y)
BRAP_DEFINES += RAP_DOLBYVOLUME_SUPPORT
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dolby_volume.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dolby_volume_table.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dolby_volume_interframe_buf.c 
endif

BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dsola_postprocess_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dsola_postprocess.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dsola_interframe_buf.c 

BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_passthru.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_stg0.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_stg1.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_stg2.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_stg3.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_stg4.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_table_stg0.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_table_stg1.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_table_stg2.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_table_stg3.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_decode_table_stg4.c	\
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_aache_interframe_buf.c

BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dts_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dts_framesync.c

BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dvdlpcm_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dvdlpcm_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_dvdlpcm_interframe_buf.c

ifeq ($(RAP_DDP_SUPPORT),y)
BRAP_DEFINES += BRAP_DDP_SUPPORT
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_be_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_fe_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_passthru_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_passthru_table.c \
    $(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_convert.c \
    $(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddpdep_framesync.c
else
 BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ddp_stub.c
endif

ifeq ($(RAP_I2S_COMPRESS_SUPPORT),y)
BRAP_DEFINES += RAP_I2S_COMPRESS_SUPPORT
endif

ifeq ($(WMA_CERTIFICATION),y)
BRAP_DEFINES += WMA_CERTIFICATION
endif

ifeq ($(RAP_AC3_SUPPORT),y)
BRAP_DEFINES += RAP_AC3_SUPPORT
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_passthru_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_convert.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_be_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_fe_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_passthru_interframe_buf.c
else
 BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_ac3_stub.c 
endif 

ifeq ($(RAP_WMA_SUPPORT),y)
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_std_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_std_interframe_buf.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_std_stg1_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_std_stg1_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_decode_stg0.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_decode_stg1.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_decode_table.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_passthru.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_interframe_buf.c 
else
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_stub.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wmapro_stub.c
endif

ifeq ($(RAP_PCMWAV_SUPPORT),y)
BRAP_DEFINES += RAP_PCMWAV_SUPPORT
BRAP_SOURCES += \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_pcmwav_framesync.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_pcmwav_decode.c \
	$(MAGNUM)/portinginterface/rap/src/fmm130/firmware/zsp500/brap_pcmwav_interframe_buf.c
endif

