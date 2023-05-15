/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7445.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/15/12 12:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7445.h $
 * 
 * 1   10/15/12 12:53p katrep
 * SW7445-1:add support for 7445
 * 
 *
 ***************************************************************************/

/* This file shall be included only from the bsettop_board.h therefore no protection from mutliple inclusion */

#define B_N_I2C_CHANNELS    5
#define B_I2C_CHANNEL_HDMI   0
#define B_HAS_FPGA  1

#define B_HAS_SDS 1

#define B_HAS_SVC_MVC 1
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
#define B_N_DNR 2
#define B_HAS_DCR 1
#define B_HAS_ANR 1

/* B_HAS_HSM must come from Makefile */
/* no daughter card, just an on board 3382*/
#define B_N_EXT_PHYSICAL_SLOTS 0
#define B_N_EXT_VIRTUAL_SLOTS 0
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 0
#define B_DYNANIC_EXT_SLOTS 0

#if 0
#ifndef B_XVD_NO_HD_SUPPORT
/* You can support 1080p 24/25/30 by default. 1080p 60 is not supported by this HW. 
We don't default this on here, you can export max_hd_display_format=1080p_30hz at run time */
#define B_HAS_1080P 1
#endif
#endif
#define B_HAS_1080P_60 1
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
#define B_MAX_MOSAIC_DECODES 12

/* Moved from base_video.h */
#define B_N_DISPLAYS 3
#define B_N_GRAPHICS B_N_DISPLAYS
#define B_N_WINDOWS 2

#define B_N_COMPOSITE_OUTPUTS 1

#define B_N_RF_OUTPUTS 0
#define B_N_COMPONENT_OUTPUTS 1
#define B_N_SVIDEO_OUTPUTS 0

#define B_N_DVI_OUTPUTS         NEXUS_NUM_HDMI_OUTPUTS


/* Moved from base_xpt.h */
#define B_MAX_PCRS  10
#define B_MAX_PARSERS 16
#define B_MAX_INPUTS 9 

#define B_HAS_I2S_OUTPUT 1
#define B_HAS_MULTICHANNEL 1

#define BSETTOP_P_DEFAULT_TO_PCM 1

/* PCM Capture API is supported */
#define B_CAPTURE_PCM_DATA 1

/* AVS is not supported on 7400 */
#define B_XVD_NO_AVS_SUPPORT   1

