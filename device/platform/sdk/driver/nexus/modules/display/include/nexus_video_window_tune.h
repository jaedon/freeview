/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_window_tune.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/3/10 2:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_video_window_tune.h $
 * 
 * 2   9/3/10 2:40p jhaberf
 * SW35230-677: Added thunk attributes for nexus tune APIs
 * 
 * 1   6/2/10 8:30p bandrews
 * SW3548-2323: share nexus_video_window_tune.h; create others for 35230
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_WINDOW_TUNE_H__
#define NEXUS_VIDEO_WINDOW_TUNE_H__

#include "nexus_types.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary: Tuning passthrough functions.  These functions expose the VDC tuning 
APIs through nexus.
Usage: These functions are intended to be installed by a nexus application as
LibTune's interface handlers.
**/

/**
Summary:
Sets the tuning parameters to a window
**/
NEXUS_Error NEXUS_VideoWindow_SetTuneParams(
	NEXUS_VideoWindowHandle display,
	const uint32_t * aFields, /* attr{nelem=num} parameter ids */
	const uint32_t * aValues, /* attr{nelem=num} values */
	int num /* count of params */
);

/**
Summary:
Gets the tuning parameters from a window
**/
NEXUS_Error NEXUS_VideoWindow_GetTuneParams(
	NEXUS_VideoWindowHandle display,
	const uint32_t * aFields, /* attr{nelem=num} parameter ids */
	uint32_t * aValues, /* attr{nelem=num} [out] values */
	int num /* count of params */
);

/**
Summary:
Resets the specified tuning parameters for a window
**/
NEXUS_Error NEXUS_VideoWindow_ResetTuneParams(
	NEXUS_VideoWindowHandle window,
	const uint32_t * aFields, /* attr{nelem=num} parameter ids */
	int num /* count of params */
);

/**
Summary:
Resets all tuning parameters for a window.  This will return all parameters
to software control (instead of LibTune control).
**/
NEXUS_Error NEXUS_VideoWindow_ResetAllTuneParams(
	NEXUS_VideoWindowHandle window
);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_WINDOW_TUNE_H__ */

