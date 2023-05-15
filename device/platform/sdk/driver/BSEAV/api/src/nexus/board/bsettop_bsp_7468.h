/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7468.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 3/8/10 3:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7468.h $
 * 
 * 3   3/8/10 3:57p randyjew
 * SW7468-85: DOLBY VOL and SRS TRU VOL support.
 * 
 * 2   12/20/09 4:53p randyjew
 * SW7468-6:Add 7208 support
 * 
 * 1   9/28/09 6:14p lwhite
 * SW7468-6: Add 7468 support
 * 
 * 
 ***************************************************************************/

/* VDC options */
#define B_HAS_MNR 1
#define B_HAS_BNR 1
#define B_HAS_MAD 1
#define B_HAS_BOX_DETECT 1
#define B_HAS_SHARPNESS 1
#define B_HAS_LAB 1
#define B_HAS_CAB 1
#define B_N_DNR 1
#define B_HAS_DCR 1

/* frontend. this varies easily by board. */
#if SDS_CHIP
#define B_HAS_SDS 1
#endif
#ifdef  AST_CHIP
#define B_HAS_AST 1
#endif

#if BCM_BOARD == 97208
#define B_HAS_1080P_60 1
#endif

#ifdef NEXUS_DOLBYVOL
#define B_HAS_DOLBY_VOLUME 1
#endif

#ifdef NEXUS_SRSTRUVOL
#define B_HAS_SRS_TRUE_VOLUME 1
#endif

#define B_MAX_DECODES            NEXUS_NUM_VIDEO_DECODERS
#define B_MAX_STILL_DECODES      NEXUS_NUM_STILL_DECODES
#define B_MAX_AUDIO_ES_DECODES   1
#define B_MAX_MOSAIC_DECODES     NEXUS_NUM_MOSAIC_DECODES
#define B_MAX_REMUXES            NEXUS_NUM_REMUX

#if B_HAS_TRANSCODE
#define B_MAX_TRANSCODES     6   /* Upto 4 SD realtime trasncodes+2 non realtime trasncodes */
#define B_MAX_ENCODES        2   /* Max 2 SD realtime encodes */
#endif

#define B_N_DISPLAYS NEXUS_NUM_DISPLAYS
#define B_N_GRAPHICS NEXUS_NUM_DISPLAYS
#define B_N_WINDOWS  NEXUS_NUM_VIDEO_WINDOWS

#define B_N_COMPOSITE_OUTPUTS   NEXUS_NUM_COMPOSITE_OUTPUTS
#define B_N_RF_OUTPUTS          NEXUS_NUM_RFM_OUTPUTS
#define B_N_COMPONENT_OUTPUTS   NEXUS_NUM_COMPONENT_OUTPUTS
#define B_N_SVIDEO_OUTPUTS      NEXUS_NUM_SVIDEO_OUTPUTS
#define B_N_DVI_OUTPUTS         NEXUS_NUM_HDMI_OUTPUTS

#define B_N_AUDIO_DACS NEXUS_NUM_AUDIO_DACS

#ifdef B_HAS_IP
#define B_N_I2S_OUTPUTS 1
#endif

#define B_MAX_PCRS    NEXUS_NUM_TIMEBASES
#define B_MAX_PARSERS NEXUS_NUM_PARSER_BANDS
#define B_MAX_INPUTS  NEXUS_NUM_INPUT_BANDS

