/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7125.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/20/09 6:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7125.h $
 * 
 * 1   8/20/09 6:52p mward
 * PR55545: Adding 7125 platforms.
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
