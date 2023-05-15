/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_3548.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 2/20/09 1:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_3548.h $
 * 
 * 7   2/20/09 1:44p erickson
 * PR52355: add ANR support
 * 
 * 6   2/9/09 2:03p erickson
 * PR51925: support 2 displays on 3548/3556
 * 
 * 5   11/13/08 1:47p jgarrett
 * PR 48965: Allowing composite output on 3549b0
 * 
 * 4   10/20/08 3:50p erickson
 * PR47994: need B_N_COMPONENT_OUTPUTS 1 for DTV (for the panel)
 *
 * 3   10/16/08 12:15p jgarrett
 * PR 47994: Fixing composite output defines
 *
 * 2   6/24/08 10:05a erickson
 * PR43222: set B_MAX_STILL_DECODES
 *
 * 1   5/29/08 8:48a erickson
 * PR39453: 3548/3556 updates
 *
 * 3   3/25/08 10:20a erickson
 * PR36068: added B_HAS_ANALOG
 *
 * 2   3/19/08 1:25p erickson
 * PR36068: add B_MAX_STILL_DECODES
 *
 * 1   11/27/07 9:19a erickson
 * PR37590: add 93563
 *
 ***************************************************************************/

/* This file shall be included only from the bsettop_board.h therefore no protection from mutliple inclusion */

#define B_N_I2C_CHANNELS    4
#define B_I2C_CHANNEL_HDMI   1
#define B_HAS_FPGA  0

/* Reduced memory support
The minimum system is: MPEG2 SD video, MPEG and AC3 audio, SD output.
To enable, uncomment any one of the following:
*/
#if 0
/* Negative logic - these default on */
#define B_XVD_NO_HD_SUPPORT 1
#define B_XVD_NO_STILL_SUPPORT 1
#define B_RAP_NO_AAC_SUPPORT 1
#define B_RAP_NO_AACPLUS_SUPPORT 1
#define B_RAP_NO_AC3PLUS_SUPPORT 1
#define B_RAP_NO_WMA_SUPPORT 1
/* No HD display also assumes one CMP/VEC, no MAD. */
#define B_DISPLAY_NO_HD_SUPPORT 1
#endif

#ifndef B_XVD_NO_HD_SUPPORT
/* we support 1080p 24/25/30 by default. 1080p 60 is not supported by this HW. */
#define B_HAS_1080P 1
#endif

#define B_HAS_RAP 1
#define B_HAS_RAVE 1
#define B_HAS_USER_IO 1
#define B_HAS_FRONTPANEL 1
#define B_HAS_VBI 1
#define B_HAS_ANALOG 1
/* B_HAS_HSM must come from Makefile */

#define B_HAS_VSB 1

#define B_N_RAP_CHANNELS 2
#define B_MAX_DECODES 1
#define B_MAX_AUD_DECODES B_MAX_DECODES
#define B_MAX_VID_DECODES B_MAX_DECODES
#define B_MAX_WINDOWS_PER_DECODE 1
#define B_MAX_AUDIO_ES_DECODES  1
#define B_MAX_STILL_DECODES 1

/* Additional input/output support.
1. 656 and I2S input support is available on 7401 but defaulted off because
of additional VDC memory requirements.
2. 656 output support not available yet.
3. I2S output support is available on 7401 but default off because of
required board change (resistor required at R1101) and pinmux changes.

To enable code, uncomment any one of the following:
*/
#if 0
#define B_HAS_656_OUTPUT 1
#define B_HAS_I2S_OUTPUT 1
#define B_HAS_EXTERNAL_ANALOG 1
#endif

# define B_N_DISPLAYS 2
# define B_N_GRAPHICS B_N_DISPLAYS
# define B_N_WINDOWS 2
# define B_N_RF_OUTPUTS 0
# define B_N_COMPOSITE_OUTPUTS 0
/* because the panel is accessed via the component api, B_N_COMPONENT_OUTPUTS must be 1 */
# define B_N_COMPONENT_OUTPUTS 1
# define B_N_SVIDEO_OUTPUTS 0
# define B_N_DVI_OUTPUTS 0

/*  define transport and parser bands    */
#define B_MAX_PCRS  2
/* 740x has 5 parser bands for live, 2 for playback. we account for the playback parser bands separately */
#define B_MAX_PARSERS 5
#define B_MAX_INPUTS 5

#if BCHP_CHIP != 3548
#error Unsupported BCHP_CHIP for this file
#endif

/* VDC options */
#define B_HAS_MAD 1
#define B_HAS_BOX_DETECT 1
#define B_N_DNR 1
#define B_HAS_BNR 1
#define B_HAS_MNR 1
#define B_HAS_DCR 1
#define B_HAS_ANR 1

/* Add this to enable BINT_Stats. This is helpful for catching
ISR flaws.
#define B_HAS_INT_STATS 1
*/
