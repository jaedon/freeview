/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7325.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/6/12 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7325.h $
 * 
 * 1   9/6/12 11:28p mphillip
 * SW7425-3739: Merge symlink removal to main
 * 
 * SW7425-3739/1   9/6/12 5:28p mphillip
 * SW7425-3739: Remove symlinks
 * 
 * 19   6/25/10 8:30a jrubio
 * SW7325-726: Add SRS TSHD to 7325 bsp
 * 
 * 18   6/12/09 3:33p jrubio
 * PR55324: map nexus->settop vars
 * 
 * 17   5/1/08 7:39a jrubio
 * PR40169: up the pcm channel to 2
 * 
 * 16   2/26/08 10:46a agin
 * PR39968: Support multiple tuners for 97325/97335.
 * 
 * 15   2/14/08 3:44p jrubio
 * PR39363: add RF outputs
 * 
 * 14   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 * 
 * 13   1/7/08 4:44p jrubio
 * PR38575: enable VBI and 2 displays for HD SD simul mode^
 * 
 * 12   1/3/08 2:21p jrubio
 * PR38507: change I2c for HDMI
 * 
 * 11   12/21/07 12:13p agin
 * PR38430: Support BCM97315FP keypad for BCM97325 and BCM97335.
 * 
 * 10   12/4/07 5:35p jrubio
 * PR35015: corrected settings
 * 
 * 9   11/27/07 3:34p jrubio
 * PR35015: new interrupt
 * 
 * 8   11/13/07 11:53a jrubio
 * PR35015: setting rap cfhannel to 2
 * 
 * 7   11/12/07 4:32p jrubio
 * PR35015: changin inputs
 * 
 * 6   11/12/07 1:06p jrubio
 * PR35015: setting channels to 2 for rap , 1 fails?
 * 
 * 5   11/12/07 7:26a jrubio
 * PR35015: changing options
 * 
 * 4   11/8/07 5:50p jrubio
 * PR35015: modify files
 * 
 * 3   10/26/07 5:31p jrubio
 * PR35015: update for 7325
 * 
 * 2   10/25/07 10:54p jrubio
 * PR35015: adding 7325
 * 
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


#ifdef NEXUS_SRSTSHD
#define B_HAS_SRS_TSHD_VOLUME 1
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

