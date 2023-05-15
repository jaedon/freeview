/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_bsp_7403.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/6/12 11:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7403.h $
 * 
 * 1   9/6/12 11:28p mphillip
 * SW7425-3739: Merge symlink removal to main
 * 
 * SW7425-3739/1   9/6/12 5:28p mphillip
 * SW7425-3739: Remove symlinks
 * 
 * 52   3/24/09 1:10p gmohile
 * PR 51326 : Add SRS support for 7401
 * 
 * 51   2/27/09 7:17p srajapur
 * PR42285 : Add audio descriptor support
 * ---> Added RAP_AD_SUPPORT compile time option.
 * 
 * 50   12/9/08 11:38a gmohile
 * PR 43467 : Add DRA support
 * 
 * 49   12/2/08 3:31p gmohile
 * PR 42285 : Add audio descriptor support
 * 
 * 48   12/2/08 2:56p gmohile
 * PR 25109 : Default pcm capture to ON
 * 
 * 47   2/15/08 3:35p gmohile
 * PR 39627 : Added 1080p pass-thru support
 * 
 * 46   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 * 
 * 45   10/15/07 2:15p erickson
 * PR36068: moved #defines from base_video.h
 * 
 * 44   10/15/07 9:47a jrubio
 * PR35007: add 4506 support
 * 
 * 43   8/22/07 3:30p katrep
 * PR34204: Added support for mosaic mode API. Enabled as default only for
 * 7405 And 7400.
 * 
 * 42   6/21/07 3:49p erickson
 * PR32422: default 1080p 24/25/30 on for 7401
 * 
 * 41   4/3/07 4:56p mward
 * PR29446: Create B_NO_3255=y build option to test boards with 3255
 * without using it
 * 
 * 40   4/3/07 3:48p gmohile
 * Pr 25109: B_HAS_LEGACY_VDC only for 7401
 * 
 * 39   4/2/07 5:29p jgarrett
 * PR 29424: Refactoring B_HAS_LEGACY_VDC
 * 
 * 38   3/28/07 3:48p vsilyaev
 * PR 29125: Marked 7403 to have native support for MPEG1 stream format
 * 
 * 37   1/31/07 4:41p erickson
 * PR27344: added B_XVD_NO_SW_RAVE_SUPPORT, default off
 * 
 * 36   1/17/07 5:43p vsilyaev
 * PR 26400: Added dedicated decoder for  audio ES (aka TSM) decodes
 * 
 * 35   1/17/07 11:20a anilmm
 * PR27129:add 97458A0 platform support
 *
 * 34   1/4/07 8:43p vle
 * PR 20727: Add 4501 dual-demod support to Settop API
 *
 * 33   12/21/06 2:19p erickson
 * PR25109: remove 7401 A0 support, which fixes 7403 support
 *
 * 32   12/1/06 2:14p erickson
 * PR24264: changed sample 1080P define. added 7401 SECAM.
 *
 * 31   11/30/06 7:26p jgarrett
 * PR 24698: Adding new defines for DNR, MAD
 *
 * 30   10/31/06 9:18a erickson
 * PR25109: add 7403 support
 *
 * 29   9/21/06 3:38p haisongw
 * PR24338: add 97455B0 Board support
 *
 * 28   9/18/06 12:47p erickson
 * PR24264: add 1080p and other HD formats to Settop API
 *
 * 27   9/12/06 3:01p erickson
 * PR22954: added #if's for reduced memory configurations. defaulted off.
 *
 * 26   9/6/06 11:09a erickson
 * PR21941: added BINT_Stats support, but default off
 *
 * 25   8/8/06 3:36p erickson
 * PR23129: default B_HAS_EXTERNAL_ANALOG off
 *
 * 24   8/8/06 3:31p erickson
 * PR23129: added 740x I2S input support using B_HAS_EXTERNAL_ANALOG, and
 * convert 656 input to use the same #define
 *
 * 23   8/3/06 11:41a erickson
 * PR23129: added 656/I2S in/out #defines, default off
 *
 * 22   7/21/06 10:27a erickson
 * PR22837: added B_HAS_656_INPUT as example, but still default off
 *
 * 21   6/20/06 8:03a erickson
 * PR21941: re-add VEC swap now that SEC VEC is working
 *
 * 20   6/6/06 5:12p jgarrett
 * PR 21978: Moving B_NO_VEC_SWAP to BSP headers
 *
 * 19   4/20/06 3:20p erickson
 * PR19738: 97435 merge
 *
 * 18   4/20/06 10:15a erickson
 * PR20039: settop api will use dual channel raptor instead of simul mode
 * for 7401
 *
 * 17   4/11/06 5:24p dlwin
 * PR 19978: Merge to main branch for 3255 support.
 *
 * 16   3/24/06 10:11a erickson
 * PR20407: B_HAS_HSM must come from Makefile
 *
 * 15   2/24/06 5:25p erickson
 * PR19790: added 4501 AST
 *
 * 14   2/17/06 10:07a erickson
 * PR19738: allow for no SDS 97401 builds
 *
 * 13   1/23/06 8:07p rgreen
 * PR17748: Modify HDMI HDCP to use HSM for chips that support AEGIS
 *
 * 12   1/19/06 3:19p erickson
 * PR17108: removed 7400
 *
 * 11   12/15/05 9:53a erickson
 * PR17108: moved some defines from bsettop_impl to bsp .h
 *
 * 10   11/23/05 5:11p erickson
 * PR18256: added 97455 support
 *
 * 9   11/18/05 3:07p erickson
 * PR17108: enabling VBI for 7401
 *
 * 8   10/20/05 5:26p erickson
 * PR17108: added 7401 hdmi
 *
 * 7   10/20/05 4:02p vsilyaev
 * PR 17710: Added 3517 support
 *
 * 6   10/19/05 7:01p vsilyaev
 * PR 17710: Improved daughtercard support
 *
 * 5   10/19/05 5:49p vsilyaev
 * PR 17710: Updated dauhthecard software to hanlde 97401 platform
 *
 * 4   10/13/05 12:20p erickson
 * PR17108: 97401 user_io bringup
 *
 * 3   9/22/05 12:33p erickson
 * PR17108: 7401 bringup
 *
 * 2   9/7/05 5:00p vsilyaev
 * PR 15377: Merged 7401 changes
 *
 * 1   7/18/05 6:17p vsilyaev
 * PR 15377: Refactoring of the board support package
 *
 * Refsw_97401_Bringup/1   7/12/05 1:51a vsilyaev
 * Split board initialization files
 *
 *
 ***************************************************************************/

/* This file shall be included only from the bsettop_board.h therefore no protection from mutliple inclusion */

#define B_N_I2C_CHANNELS    4
#define B_I2C_CHANNEL_HDMI   1
#if BCM_BOARD ==97435 || ((BCM_BOARD == 97455 || BCM_BOARD == 97458) && !defined(BCM97455MBV00))
#define B_HAS_FPGA  0
#else
#define B_HAS_FPGA  1
#endif

/* Reduced memory support
The minimum system is: MPEG2 SD video, MPEG and AC3 audio, SD output.
To enable, uncomment any one of the following:
*/
#if 0
/* Negative logic - these default on */
#define B_XVD_NO_AVC_SUPPORT 1
#define B_XVD_NO_VC1_SUPPORT 1
#define B_XVD_NO_DIVX_SUPPORT 1
#define B_XVD_NO_HD_SUPPORT 1
#define B_XVD_NO_STILL_SUPPORT 1
#define B_RAP_NO_AAC_SUPPORT 1
#define B_RAP_NO_AACPLUS_SUPPORT 1
#define B_RAP_NO_AC3PLUS_SUPPORT 1
#define B_RAP_NO_WMA_SUPPORT 1
/* No HD display also assumes one CMP/VEC, no MAD. */
#define B_DISPLAY_NO_HD_SUPPORT 1
/* SW_RAVE is required for VC1 simple/main and DivX 3.11. It adds
another tiny-CDB and normal-sized ITB. Remove if not needed. */
#define B_XVD_NO_SW_RAVE_SUPPORT 1
#endif

#if BCHP_CHIP == 7403
#define B_RAP_DRA_SUPPORT 1
#endif

/* 1080p 24/25/30 passh- thru is by  default off. 1080p 60 is not supported by this HW. */
#if 0
#define B_HAS_1080P 1
#endif


#define B_HAS_XVD 1
#define B_HAS_RAP 1
#define B_HAS_RAVE 1
#define B_HAS_USER_IO 1
#define B_HAS_FRONTPANEL 1
#define B_HAS_VBI 1
/* B_HAS_HSM must come from Makefile */

/* frontend. this varies easily by board. */
#if SDS_CHIP
#define B_HAS_SDS 1
#endif
#ifdef AST_CHIP
#define B_HAS_AST 1
#endif
#if !(BCM_BOARD == 97455) && !(BCM_BOARD == 97458)
#define B_HAS_VSB 1
#endif

#if BCM_BOARD == 97455 || BCM_BOARD == 97458
/* no daughter card, just an on board 3255 */
#define B_N_EXT_PHYSICAL_SLOTS 0
#define B_N_EXT_VIRTUAL_SLOTS 0
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 0
#define B_DYNANIC_EXT_SLOTS 0

#elif BCM_BOARD == 97435
#define B_N_EXT_PHYSICAL_SLOTS 5
#define B_N_EXT_VIRTUAL_SLOTS 2
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 3
#define B_DYNANIC_EXT_SLOTS 1
#else
/* on board 4500 & 3517 handled as a daughter cards in dedicated slots */
#define B_N_EXT_PHYSICAL_SLOTS (2+3)
#define B_N_EXT_VIRTUAL_SLOTS 2
#define B_N_PHYSICAL_TO_VIRTUAL_OFFSET 3    /* Offset between virtual slots and dual channel physical slots */
#define B_DYNANIC_EXT_SLOTS 1
#endif

#define B_N_EXT_SLOTS (B_N_EXT_PHYSICAL_SLOTS + B_N_EXT_VIRTUAL_SLOTS)

/* B_N_XVD_CHANNELS is defined in bsettop_base_xvd.h */

#define B_N_RAP_CHANNELS 2
#define B_N_AVC_DECODERS 1
#define B_MAX_DECODES 1
#define B_MAX_AUD_DECODES B_MAX_DECODES
#define B_MAX_VID_DECODES B_MAX_DECODES
#define B_MAX_WINDOWS_PER_DECODE 1
#define B_MAX_AUDIO_ES_DECODES  1
#ifdef B_XVD_NO_STILL_SUPPORT
#define B_MAX_STILL_DECODES 0
#else
#define B_MAX_STILL_DECODES 1
#endif

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

#ifdef RAP_SRSTRUVOL_SUPPORT
#define B_HAS_SRS_VOLUME 1
#endif

/* PCM Capture API is supported */
#define B_CAPTURE_PCM_DATA 1

#if RAP_AD_SUPPORT
#define B_HAS_AD_SUPPORT 1
#endif

/* Moved from base_video.h */
# define B_N_DISPLAYS 2
# define B_N_GRAPHICS B_N_DISPLAYS
# define B_N_WINDOWS 1
# define B_N_COMPOSITE_OUTPUTS 1
# define B_N_RF_OUTPUTS 1
# define B_N_COMPONENT_OUTPUTS 1
# define B_N_SVIDEO_OUTPUTS 1
# define B_N_DVI_OUTPUTS 1

/*  define transport and parser bands    */
#define B_MAX_PCRS  2
/* 740x has 5 parser bands for live, 2 for playback. we account for the playback parser bands separately */
#define B_MAX_PARSERS 5
#define B_MAX_INPUTS 5

#if BCHP_CHIP != 7401 && BCHP_CHIP != 7403
#error Unsupported BCHP_CHIP for this file
#endif

#ifdef BCHP_7411_VER
#error BCHP_7411_VER cannot be defined for 740x.
#endif

#if BCHP_CHIP == 7403
#define B_HAS_NATIVE_MPEG1  1
#endif

/* VDC options */
#define B_HAS_MAD 1
#define B_HAS_SECAM 1
#define B_HAS_BOX_DETECT 1
#define B_N_DNR 1
#define B_HAS_BNR 1
#define B_HAS_MNR 1
#define B_HAS_DCR 1
#if BCHP_CHIP == 7401
#define B_HAS_LEGACY_VDC 1
#endif

/* Add this to enable BINT_Stats. This is helpful for catching
ISR flaws.
#define B_HAS_INT_STATS 1
*/
/* AVS is not supported on 7401 */
#define B_XVD_NO_AVS_SUPPORT   1
