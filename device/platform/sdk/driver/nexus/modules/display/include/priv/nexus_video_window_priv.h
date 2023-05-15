/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_window_priv.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/3/12 7:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/priv/nexus_video_window_priv.h $
 * 
 * 5   8/3/12 7:03p bandrews
 * SW7420-2314: merge to main
 * 
 * SW7420-2314/1   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * 4   7/3/12 5:10p bandrews
 * SW7425-717: merge to main
 * 
 * SW7425-717/2   6/4/12 6:51p bandrews
 * SW7425-717: Added refresh rate for later; add prototype for
 *  UpdatePhaseDelay_isr
 * 
 * SW7425-717/1   5/15/12 7:24p bandrews
 * SW7425-717: initial impl
 * 
 * 3   10/20/11 5:12p erickson
 * SW7420-1992: remove unnecessary priv function
 * 
 * 2   10/20/11 1:43p erickson
 * SW7420-1992: add private api's for atomically setting multiple video
 *  window settings
 * 
 * 1   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 * 
 **************************************************************************/
#ifndef NEXUS_VIDEO_WINDOW_PRIV_H__
#define NEXUS_VIDEO_WINDOW_PRIV_H__

#include "nexus_video_window.h"
#include "bavc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_VideoWindowSyncSettings
{
    unsigned delay;
    
    NEXUS_Callback stateChangeCallback_isr; /* fire if window state changes, except for format and delay */
    NEXUS_Callback delayCallback_isr; /* fire if BVN delay changes */
    NEXUS_Callback formatCallback_isr; /* fire if display format changes */
    void *callbackContext; /* user context passed callback_isr */
} NEXUS_VideoWindowSyncSettings;

void NEXUS_VideoWindow_GetSyncSettings_priv(
    NEXUS_VideoWindowHandle handle,
    NEXUS_VideoWindowSyncSettings *pSyncSettings
    );
    
NEXUS_Error NEXUS_VideoWindow_SetSyncSettings_priv(
    NEXUS_VideoWindowHandle handle,
    const NEXUS_VideoWindowSyncSettings *pSyncSettings
    );
    
typedef struct NEXUS_VideoWindowSyncStatus 
{
    /* window */
    bool forcedCaptureEnabled; /* is forced capture enabled on this window */
    bool masterFrameRateEnabled; /* is master frame rate enabled on the main window for this display */
    bool fullScreen; /* does window rect match display rect? */
    bool visible; /* is this window visible? */
    bool syncLocked; /* is this window sync-locked? */
    bool delayValid;
    int delay; /* current delay in VSYNCs at the display rate */
    int phaseDelay; /* current sub-VSYNC delay in ms */

    /* display (format is always valid) */   
    unsigned int height; /* height of display format, required to predict VDC MAD state changes */
    bool interlaced; /* whether the display format is interlaced */
    BAVC_FrameRateCode frameRate; /* the frame rate of the display format */
    bool aligned; /* this display is aligned to another display */
    unsigned refreshRate; /* the refresh rate of the display */
} NEXUS_VideoWindowSyncStatus;
        
NEXUS_Error NEXUS_VideoWindow_GetSyncStatus_isr(
    NEXUS_VideoWindowHandle handle,
    NEXUS_VideoWindowSyncStatus *pSyncStatus
    );
    
NEXUS_Error NEXUS_VideoWindow_SetSettings_priv(
    NEXUS_VideoWindowHandle handle,
    const NEXUS_VideoWindowSettings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_WINDOW_PRIV_H__ */

