/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_priv.h $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 8/15/12 6:50p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_priv.h $
* 
* Hydra_Software_Devel/7   8/15/12 6:50p bandrews
* SW7425-717: disable dynamic/timer-based RMD.  Not useful anymore.
* 
* Hydra_Software_Devel/6   8/15/12 6:25p bandrews
* SW7435-208: default rate rematch detection off
* 
* Hydra_Software_Devel/5   3/22/10 5:40p bandrews
* sw7408-83: add unconditional video unmute; clean up unused defines
* 
* Hydra_Software_Devel/4   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/3   8/4/09 4:56p bandrews
* PR52812: added improved rmd for dmv2
* 
* Hydra_Software_Devel/2   2/25/09 7:52p bandrews
* PR52514: needed another const
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/5   2/25/08 9:33p bandrews
* PR37951: Fixed various bugs
* 
* Hydra_Software_Devel/4   2/22/08 8:28p bandrews
* PR37951: Fixed bug in units conversion
* 
* Hydra_Software_Devel/3   2/20/08 10:03p bandrews
* PR37951: Updated based on feedback from usage
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bsyslib_list.h"
#include "bsynclib.h"

#ifndef BSYNCLIB_PRIV_H__
#define BSYNCLIB_PRIV_H__

#if 0
#define BSYNCLIB_XVD_PTS_STC_OFFSET_SUPPORT 1
#define BSYNCLIB_P_MAX_VEC_RATE 60;                /* in vsyncs per second */
#define BSYNCLIB_RAP_DELAY_PI_FN_SUPPORT 1

#define BSYNCLIB_DSS_STC_RATE 27000000                 /* ticks / s */
#define BSYNCLIB_MPEG_STC_RATE 90000                       /* ticks / s */
#define BSYNCLIB_MPEG_TO_DSS(X) ((X) * 300)
#define BSYNCLIB_DSS_TO_MPEG(X) ((X) / 300)

/* PR27128 20070212 bandrews - for limiting the cap bufs sync requests to
sync simul windows */
#define BSYNCLIB_P_SD_BUFFERS_PER_HD_BUFFER 4
#define BSYNCLIB_P_MAX_CUMULATIVE_HD_VDC_DELAY 1
#define BSYNCLIB_P_MAX_CUMULATIVE_SD_VDC_DELAY (BSYNCLIB_P_MAX_CUMULATIVE_HD_VDC_DELAY * BSYNCLIB_P_SD_BUFFERS_PER_HD_BUFFER)
/* only first window/display is HD so far */
#define BSYNCLIB_P_HD_WINDOW_MASK 0x1 

/* default PTS jitter tolerance in BSYNCLIB ticks */
#define BSYNCLIB_VIDEO_PTS_STC_DIFF_THRESHOLD 10       /* 1 ms */

/* default XVD PTS jitter tolerance in XVD STC ticks */
#define BSYNCLIB_P_DEFAULT_PTS_STC_DIFF_THRESHOLD 45 /* 1 ms */

/* 20070614 bandrews - video priming unnecessary now that we mute video until all
adjustments are made */
/* default video priming delay */
#define BSYNCLIB_VIDEO_INITIAL_DELAY (0)       /* 0 ms */
#endif

/* PR20199 20060725 bandrews - Change default to 200 ms */
#define BSYNCLIB_VIDEO_TSM_LOCK_TIMER_DEFAULT_TIMEOUT 100 /* ms */

/* 20070406 bandrews - default to 5000 ms */
#define BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_TIMER_DEFAULT_TIMEOUT 0 /* ms */
#define BSYNCLIB_VIDEO_RATE_REMATCH_DETECTION_TIMER_DEFAULT_TIMEOUT 0 /* ms */
#if 0
#define BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_CALLBACK_COUNT 6
#define BSYNCLIB_VIDEO_RATE_REMATCH_DETECTION_DEFAULT_ACCEPTABLE_CALLBACK_COUNT 0
#endif
#define BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_MTBC_LOWER 300
#define BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_MTBC_UPPER 1500
#define BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_TTLC 1500
#define BSYNCLIB_VIDEO_RATE_REMATCH_DETECTION_DEFAULT_ACCEPTABLE_TTLC 9000

#if 0
/* PR22992 20060803 bandrews - Added */
#define BSYNCLIB_PASS_WINDOW_CORRECTION_SUPPORT 1

/* PR22004 20060901 bandrews - Added */
#define BSYNCLIB_FORCED_CAPTURE_SUPPORT 1
#endif

/* PR20199 20060829 bandrews - moved this to the top */
#define BSYNCLIB_AUDIO_UNMUTE_DEFAULT_TIMEOUT 200 /* ms */

/* 20070502 bandrews - added video muting */
#define BSYNCLIB_VIDEO_UNMUTE_DEFAULT_TIMEOUT 133 /* ms */

/* 20070406 bandrews - compensate for external audio receiver on SPDIF output */
#define BSYNCLIB_AUDIO_RECEIVER_DELAY_COMPENSATION_DEFAULT 0 /* ms */

/* SW7408-83 20100319 bandrews - added for unconditional video unmute */
#define BSYNCLIB_UNCONDITIONAL_VIDEO_UNMUTE_SUPPORT 1
#define BSYNCLIB_VIDEO_UNCONDITIONAL_UNMUTE_DEFAULT_TIMEOUT 5000

/* PR23752 20060829 bandrews - added for unconditional audio unmute */
#define BSYNCLIB_UNCONDITIONAL_AUDIO_UNMUTE_SUPPORT 1
#define BSYNCLIB_AUDIO_UNCONDITIONAL_UNMUTE_DEFAULT_TIMEOUT 5000

#if 0
#define BSYNCLIB_P_VDC_INCLUDES_1_FIELD_VEC_DELAY 1
#define BSYNCLIB_MAX_VEC_DELAY_INTERLACED 2 /* vsyncs */
#define BSYNCLIB_MAX_VEC_DELAY_PROGRESSIVE 1 /* vsync */

#define BSYNCLIB_AUD_PCM_BUFFER_ALIGNMENT 16
#define BSYNCLIB_AUD_PCM_BUFFER_ALIGNMENT_MASK 0xf

extern unsigned int upow10(unsigned int p);

#define BSYNCLIB_PRECISION 4                                           /* calcs are done in 100's of us */
#define BSYNCLIB_SEC_TO_TICK(x) ((x) * upow10(BSYNCLIB_PRECISION))        /* converts from seconds to BSYNCLIB precision level */
#define BSYNCLIB_MS_TO_TICK(x) ((x) * upow10(BSYNCLIB_PRECISION) / 1000)        /* converts from milliseconds to BSYNCLIB precision level */
#define BSYNCLIB_TICK_TO_SEC(x) ((x) / upow10(BSYNCLIB_PRECISION))        /* converts from BSYNCLIB precision level to seconds */
#if defined(BDBG_DEBUG_BUILD)
#define BSYNCLIB_TICK_TO_MS(x) ((float)(x) / upow10(BSYNCLIB_PRECISION - 3)) /* converts to milliseconds (float) */
#endif

#define BSYNCLIB_P_IS_HD_WINDOW(W) (((1 << (W)) & BSYNCLIB_P_HD_WINDOW_MASK) != 0)
#endif

/*
Summary:
*/
struct BSYNClib_Impl
{
	BSYNClib_Settings sSettings;
	BSYSlib_List_Handle hChannels;
};

bool BSYNClib_P_Enabled(BSYNClib_Handle hSync);

unsigned int BSYNClib_P_Convert(unsigned int uiValue, BSYNClib_Units eFromUnits, BSYNClib_Units eToUnits);
unsigned int BSYNClib_P_Convert_isr(unsigned int uiValue, BSYNClib_Units eFromUnits, BSYNClib_Units eToUnits);
int BSYNClib_P_ConvertSigned(int iValue, BSYNClib_Units eFromUnits, BSYNClib_Units eToUnits);
int BSYNClib_P_ConvertSigned_isr(int iValue, BSYNClib_Units eFromUnits, BSYNClib_Units eToUnits);

#if BDBG_DEBUG_BUILD
extern const char * const BSYNClib_P_UnitsStrings[];
#endif

#if 0
#define BSYNCLIB_P_PRECISION_FACTOR 100
#define BSYNCLIB_P_MS_TO_US(X) ((X) * 1000)
#define BSYNCLIB_P_US_TO_MS(X) ((X) / 1000)
#endif

#endif /* BSYNCLIB_PRIV_H__ */

