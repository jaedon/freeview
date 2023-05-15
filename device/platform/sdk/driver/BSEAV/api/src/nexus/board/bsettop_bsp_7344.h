/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7344.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/6/12 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7344.h $
 * 
 * 1   9/6/12 11:28p mphillip
 * SW7425-3739: Merge symlink removal to main
 * 
 * SW7425-3739/1   9/6/12 5:28p mphillip
 * SW7425-3739: Remove symlinks
 * 
 * 5   4/5/11 4:11p randyjew
 * SW7344-24:remove ANR support for 7344/7346
 * 
 * 4   3/24/11 6:08p randyjew
 * SW7344-9:Turn on SECAM support for 7346 and 7344
 * 
 * 3   1/4/11 4:44p jrubio
 * SW7344-20: add hd pip
 * 
 * 1   12/6/10 3:14p katrep
 * SW7231-4:inital support
 * 
 * 3   12/15/08 4:49p katrep
 * PR50222: Add independent delay support for audio dacs. dac_output_delay
 * 
 * 2   7/18/08 4:32p shyam
 * PR43426 : Add 7043 656 Encode support
 * 
 * 1   7/17/08 4:19p erickson
 * PR44919: make Settop API #defines depend on nexus_platform_features.h
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
#define B_HAS_SECAM 1


#define B_MAX_DECODES            NEXUS_NUM_VIDEO_DECODERS
#define B_MAX_STILL_DECODES      NEXUS_NUM_STILL_DECODES
#define B_MAX_AUDIO_ES_DECODES   1
#define B_MAX_MOSAIC_DECODES     NEXUS_NUM_MOSAIC_DECODES
#define B_MAX_REMUXES            NEXUS_NUM_REMUX


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

