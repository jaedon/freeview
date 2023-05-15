/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7335.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/6/12 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7335.h $
 * 
 * 1   9/6/12 11:28p mphillip
 * SW7425-3739: Merge symlink removal to main
 * 
 * SW7425-3739/1   9/6/12 5:28p mphillip
 * SW7425-3739: Remove symlinks
 * 
 * 12   6/12/09 5:38p jrubio
 * PR55324: map nexus->settop vars
 * 
 * 11   5/28/08 8:02p jrubio
 * PR42353: fix #defines
 * 
 * 10   3/11/08 3:02p katrep
 * PR37217: Make base_rap compiles with the current raptor label.
 * 
 * 9   2/26/08 11:06a agin
 * PR39968: Support multiple tuners for 97325/97335.
 * 
 * 8   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 * 
 * 7   1/21/08 10:36a katrep
 * PR37217: Reverting back previous change. 97335 uses SPI front panel.
 * 
 * 6   1/18/08 11:12a farshidf
 * PR38815: enable Front Panel enable
 * 
 * 5   12/27/07 8:53p agin
 * PR38430: Support BCM97315FP keypad for BCM97325 and BCM97335.
 * 
 * 4   12/19/07 5:32p katrep
 * PR37217: Fixed the number of windows. Fixed the HDSD_SIMUL mode in
 * brutus.
 * 
 * 3   12/17/07 3:50p katrep
 * PR37217: 7335 uses I2C0 for HDMI port.
 * 
 * 2   12/16/07 9:07p katrep
 * PR37217: Disabled userio,allocated vdc buffres for 7335 bringup
 * 
 * 1   11/27/07 3:26p katrep
 * PR37217: Copied from 7405, and added 7335 support
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

