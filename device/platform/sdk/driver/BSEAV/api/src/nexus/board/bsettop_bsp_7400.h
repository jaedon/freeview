/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7400.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/6/12 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7400.h $
 * 
 * 1   9/6/12 11:28p mphillip
 * SW7425-3739: Merge symlink removal to main
 * 
 * SW7425-3739/1   9/6/12 5:28p mphillip
 * SW7425-3739: Remove symlinks
 * 
 * 50   3/16/10 4:39p mward
 * SW7400-2517:  Maintain REFSW_XVD_7400_SINGLEDECODE support.  Set
 * B_MAX_MOSAIC_DECODES=0 at compile time if B_XVD_SINGLEDECODE=y.
 * REFSW_PREREL_97400_SINGLE is deprecated, use REFSW_PREREL_97400.
 * 
 * 49   3/13/09 5:41p mward
 * PR50613: Add support for SRS Volume IQ.
 * PR49827: Added compilation option for dolby volume.
 * 
 * PR49827/4   2/25/09 12:18p mward
 * PR52490: Change name of SRS VIQ to SRS TrueVolume
 * 
 * PR49827/3   1/13/09 5:12p mward
 * PR50613: Add support for SRS Volume IQ.
 * 
 * PR49827/2   1/13/09 9:37p srajapur
 * PR49827: Add a Dolby Volume
 * --> Added compilation option for dolby volume.
 * 
 * PR49827/1   12/1/08 4:37p mward
 * PR49827: Add a Dolby Volume checkbox in Info:Audio.
 * 
 * 48   8/20/08 4:59p mward
 * PR32047: Mosaic mode is now default for 7400.
 * 
 * SanDiego_Brutus_Mosaic/6   8/20/08 4:46p mward
 * PR32047: PCM capture support now defaults on for 7400.
 * 
 * SanDiego_Brutus_Mosaic/5   8/11/08 5:40p mward
 * PR39988: For 7400, select DDP pass-thru support at run-time, switching
 * use of RAP channels if necessary.  Merge to main.
 * 
 * SanDiego_Brutus_Mosaic/4   5/22/08 6:44p mward
 * PR39988: Create build option B_PASSTHRU_DDP_SUPPORT=y which allows DDP
 * transcode to SPDIF + Passthru to HDMI.  Dual decode is disabled with
 * this option.
 * 
 * SanDiego_Brutus_Mosaic/3   2/8/08 2:39p mward
 * PR38429: Merge error.
 * 
 * 45   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 * 
 * 44   10/15/07 9:49a jrubio
 * PR35007: adding 4506
 * 
 * 43   9/28/07 12:23p mward
 * PR35437: Fix compiler warning and default 1080p off until it is
 * verified working.
 * 
 * 42   9/21/07 2:45p mward
 * PR34204: In order to not use mosaic API, don't define XVD heaps.
 * 
 * 41   9/19/07 4:01p mward
 * PR34204: Improved XVD memory settings.
 * 
 * 40   9/12/07 1:00p mward
 * PR34204: Fix build with B_MAX_MOSAIC_DECODES=0.  Still need to review
 * XVD memory settings.
 * 
 * 39   8/29/07 6:50p mward
 * PR32865: Enable WMA and WMAPRO in Settop API.
 * 
 * 38   8/22/07 3:30p katrep
 * PR34204: Added support for mosaic mode API. Enabled as default only for
 * 7405 And 7400.
 * 
 * PR32044/2   6/27/07 5:26p jgarrett
 * PR 32044: Updating allocation sizes for xvd
 * 
 * PR32044/1   6/26/07 5:56p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 * 
 * 37   5/4/07 4:06p jgarrett
 * PR 25900: Enabling RFM for 7400B0
 * 
 * 36   4/3/07 4:56p mward
 * PR29446: Create B_NO_3255=y build option to test boards with 3255
 * without using it
 * 
 * 35   3/8/07 2:40p jgarrett
 * PR 25900: Re-enabling HDMI
 * 
 * 34   3/6/07 3:10p jgarrett
 * PR 25900: Re-enabling VBI for the 7400 b0
 * 
 * 33   3/6/07 2:40p jgarrett
 * PR 28298: Enabling 2nd DNR/DCR in 7400 B0
 * 
 * 32   3/1/07 12:56p jgarrett
 * PR 25900: Enabling MAD, DNR, DCR for 7400 B0
 * 
 * 31   2/27/07 5:32p jgarrett
 * PR 25900: Disabling MAD/DNR for 7400 B0
 * 
 * 30   2/23/07 3:42p jgarrett
 * PR 28036: Adding new VDC options for LAB/CAB/Sharpness
 * 
 * 29   2/22/07 4:23p jgarrett
 * PR 25900: Disabling some features for 7400 B0
 * 
 * 28   2/15/07 4:58p jgarrett
 * PR 27703: Re-enabling DNR and box-detect for the 7400A
 * 
 * 27   2/7/07 8:15p jgarrett
 * PR 27703: Disabling box-detect for 7400 A0.
 * 
 * 26   2/7/07 8:01p jgarrett
 * PR 27703: Disabling box-detect for 97400
 * 
 * 25   1/17/07 5:43p vsilyaev
 * PR 26400: Added dedicated decoder for  audio ES (aka TSM) decodes
 * 
 * 24   1/16/07 3:04p haisongw
 * PR 20984: 97456 has no daugter tuner board
 *
 * 23   1/9/07 1:51p jgarrett
 * PR 26914: Relocating 7400 xpt defines to bsp header
 *
 * 22   12/5/06 6:49p jgarrett
 * PR 24698: Re-enabling DNR
 *
 * 21   12/1/06 11:33a jgarrett
 * PR 24698: disabling DNR on 7400 until VDC issue is resolved
 *
 * 20   11/30/06 7:27p jgarrett
 * PR 24698: Adding DNR/MAD defines
 *
 * 19   9/20/06 11:25a jgarrett
 * PR 24351: Enabling advanced audio codecs.
 *
 * 18   9/12/06 3:01p erickson
 * PR22954: added #if's for reduced memory configurations. defaulted off.
 *
 * 17   8/21/06 5:08p jgarrett
 * PR 23738: Adding 4501/AST support
 *
 * 16   8/9/06 3:46p jgarrett
 * PR 22675: Adding B_HAS_MULTICHANNEL
 *
 * 15   8/3/06 11:41a erickson
 * PR23129: added B_HAS_I2S_OUTPUT
 *
 * 14   6/6/06 5:12p jgarrett
 * PR 21978: Moving declaration of B_NO_VEC_SWAP to BSP headers
 *
 * 13   5/26/06 3:06p jgarrett
 * PR 21457: Enabling VBI
 *
 * 12   5/16/06 1:46p jgarrett
 * PR 21417: Adding 3rd RAP channel for passthrough
 *
 * 11   5/5/06 10:57a jgarrett
 * PR 20139: Enabling RFM
 *
 * 10   5/4/06 12:06p jgarrett
 * PR 21266: Merging 3-display code to main.
 *
 * REFSW_97400_3DISPLAY/2   5/3/06 5:55p jgarrett
 * PR 21266: Adding 2nd raptor channel
 *
 * REFSW_97400_3DISPLAY/1   5/2/06 6:30p jgarrett
 * PR 21266: Adding 3rd display
 *
 * 9   4/28/06 1:45p haisongw
 * PR 20984: add 97456 platform support
 *
 * 8   4/27/06 3:06p jgarrett
 * PR 21266: Moved base_video defines here for simplicity
 *
 * 7   3/28/06 11:13a jgarrett
 * PR 20236: Enabling still decodes
 *
 * 6   2/23/06 2:13p jgarrett
 * PR 19674: Revisiting dual-audio support for 7400
 *
 * 5   2/22/06 10:10a jgarrett
 * PR 19674: Enabling dual-decode support
 *
 * 4   2/13/06 5:50p jgarrett
 * PR 19250: Removing Still Pictures for BCM7400
 *
 * 3   1/25/06 11:07a gmullen
 * PR19105: Added support for 5th I2C master.
 *
 * 2   1/20/06 4:29p jgarrett
 * PR 19105: Create BSP for 7400
 *
 * 1   1/17/06 2:15p jgarrett
 * PR 19105: Updating BSP for 97400
 *
 * Refsw_Devel_7400_A0/1   1/12/06 3:05p jgarrett
 * PR 18997: Creating directories for BCM7400
 * Added file element "bsettop_bsp_7400.c".
 * Added file element "bsettop_bsp_7400.h".
 * PR 18997: Creating directories for 7400
 * PR 18997: Creating directories for 7400
 *
 ***************************************************************************/

/* This file shall be included only from the bsettop_board.h therefore no protection from mutliple inclusion */

#define B_N_I2C_CHANNELS    5
#define B_I2C_CHANNEL_HDMI   0
#if BCM_BOARD != 97456
#define B_HAS_FPGA  1
#endif

#define B_HAS_XVD 1
#define B_HAS_RAP 1
#define B_HAS_RAVE 1
#define B_HAS_USER_IO 1
#define B_HAS_FRONTPANEL 1
#define B_HAS_VBI 1

/* VDC options */
#define B_HAS_MNR 1
#define B_HAS_BNR 1
#define B_HAS_MAD 1
#define B_HAS_BOX_DETECT 1
#define B_HAS_SHARPNESS 1
#define B_HAS_LAB 1
#define B_HAS_CAB 1
/* Chip version-specific  VDC options */
#if BCHP_VER >= BCHP_VER_B0
#define B_N_DNR 2
#define B_HAS_DCR 1
#else
#define B_N_DNR 1
#define B_HAS_DCR 0
#endif

/* B_HAS_HSM must come from Makefile */

/* frontend. this varies easily by board. */
#if SDS_CHIP
#define B_HAS_SDS 1
#endif
#ifdef AST_CHIP
#define B_HAS_AST 1
#endif
#if BCM_BOARD != 97456
#define B_HAS_VSB 1
#endif

#if BCM_BOARD == 97456
/* no daughter card, just an on board 3255 */
#define B_N_EXT_PHYSICAL_SLOTS 0
#define B_N_EXT_VIRTUAL_SLOTS 0
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 0
#define B_DYNANIC_EXT_SLOTS 0

#else
/* on board 4500 & 3517 handled as a daughter cards in dedicated slots */
#define B_N_EXT_PHYSICAL_SLOTS (2+3)
#define B_N_EXT_VIRTUAL_SLOTS 2
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 3    /* Offset between virtual slots and dual channel physical slots */
#define B_DYNANIC_EXT_SLOTS 1
#endif

#if 0
#ifndef B_XVD_NO_HD_SUPPORT
/* You can support 1080p 24/25/30 by default. 1080p 60 is not supported by this HW. 
We don't default this on here, you can export max_hd_display_format=1080p_30hz at run time */
#define B_HAS_1080P 1
#endif
#endif
#define B_N_EXT_SLOTS (B_N_EXT_PHYSICAL_SLOTS + B_N_EXT_VIRTUAL_SLOTS)

/* B_N_XVD_CHANNELS is defined in bsettop_base_xvd.h */

#define B_N_AVC_DECODERS 2
#define B_N_RAP_CHANNELS 3

#define B_MAX_DECODES 2
#define B_MAX_AUD_DECODES 2
#define B_MAX_AUDIO_ES_DECODES  2
#define B_MAX_STILL_DECODES 2
#define B_MAX_VID_DECODES B_MAX_DECODES
#define B_MAX_WINDOWS_PER_DECODE 2

#if B_MAX_MOSAIC_DECODES
/* Please refer to the XVD memory spreadsheet for these values */
#define B_XVD_SECURE_HEAP_SIZE (5210112+307200) /* CABAC for AVC HD, AVC HD still*/
#define B_XVD_GENERAL_HEAP_SIZE (1024*1024 + (B_MAX_MOSAIC_DECODES*1179648) + 518656) /* FW + AVC CIF mosaics + AVC HD Still */
#define B_XVD_PICTURE_HEAP_SIZE (3194880*7+3194880) /* AVC HD + AVC HD still*/
#endif
#define B_XVD_VIDEO_CDB_SIZE (3*1024*1024/2)
#define B_XVD_VIDEO_ITB_SIZE (512*1024)

#define B_XVD_MOSAIC_CDB_SIZE (150*1024)
#define B_XVD_MOSAIC_ITB_SIZE (32*1024) /* Guess... */
#define B_XVD_STILL_CDB_SIZE (1024*1024/2)
#define B_XVD_STILL_ITB_SIZE (125*1024)

/* Moved from base_video.h */
# define B_N_DISPLAYS 3
# define B_N_GRAPHICS B_N_DISPLAYS
# define B_N_WINDOWS 2
# define B_N_COMPOSITE_OUTPUTS 2
# define B_N_RF_OUTPUTS 1
# define B_N_COMPONENT_OUTPUTS 1
# define B_N_SVIDEO_OUTPUTS 1
# define B_N_DVI_OUTPUTS 1

/* Moved from base_xpt.h */
#define B_MAX_PCRS  4
#define B_MAX_PARSERS 5
#define B_MAX_INPUTS 5

#define B_HAS_I2S_OUTPUT 1
#define B_HAS_MULTICHANNEL 1

#ifdef RAP_DOLBYVOLUME_SUPPORT
#if !(defined(B_RAP_NO_AC3_SUPPORT) && defined(B_RAP_NO_AC3PLUS_SUPPORT))
#define B_HAS_DOLBY_VOLUME 1
#endif
#endif

#ifdef RAP_SRSTRUVOL_SUPPORT
#define B_HAS_SRS_VOLUME 1
#endif

#define BSETTOP_P_DEFAULT_TO_PCM 1

/* PCM Capture API is supported */
#define B_CAPTURE_PCM_DATA 1

/**
B_NO_VEC_SWAP is an option for all platforms. However, 7400 A0 requires it because of a HW bug.
The default is to swap the VEC's.
If not swapped, the primary VEC will be used for display0. This has a limited number of DAC's,
therefore in single display mode, only component will be available.
**/
#if BCHP_CHIP == 7400 && BCHP_VER == BCHP_VER_A0
#define B_NO_VEC_SWAP 1
#endif

#ifdef BCHP_7411_VER
#error BCHP_7411_VER cannot be defined for 740x.
#endif


/* AVS is not supported on 7400 */
#define B_XVD_NO_AVS_SUPPORT   1
