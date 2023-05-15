/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core_utils.c $
* $brcm_Revision: 15 $
* $brcm_Date: 10/19/12 5:31p $
*
* API Description:
*   API name: Base
*   It also initializes the magnum base modules. And provides system level
*   interrupt routing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/src/nexus_core_utils.c $
* 
* 15   10/19/12 5:31p erickson
* SW7425-3782: allow NEXUS_FrontendConnector to be acquired with Frontend
* 
* 14   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
*
* 13   9/4/12 1:11p vsilyaev
* SW7420-1004: Removed NEXUS_GetLastError
*
* 12   7/23/12 11:35a vsilyaev
* SW7420-2328: Use per-thread storage to keep information about the
*  current client, remove per-module client information
*
* 11   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
*
* 10   1/23/12 4:34p erickson
* SW7435-27: rearrange so nexus_core_convert.c has no public functions
*
* 9   11/1/11 1:03p vsilyaev
* SW7420-2114: Added code to handle full resolution 3D display formats
*
* 8   3/4/11 5:45p jtna
* SW7422-221: add NEXUS_VideoFormatInfo.isFullRes3d
*
* 7   9/15/10 3:44p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
*
* 6   1/26/09 11:07a erickson
* PR51468: global variable naming convention
*
* 5   9/22/08 9:46a erickson
* PR46063: added NEXUS_P_VideoFormat_SetInfo for custom panel formats
*
* 4   8/11/08 11:40a erickson
* PR45219: add DBG
*
* 3   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 2   2/28/08 9:42p vsilyaev
* PR 40103: Added NEXUS_Stop/StartCallback functions
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/6   12/28/07 5:05p erickson
* PR38470: add NEXUS_VideoFormat_GetInfo to public API in
* nexus_core_utils.h
*
* Nexus_Devel/5   12/5/07 10:52a vsilyaev
* PR 37574: Improved reporting of not supported files
*
* Nexus_Devel/4   11/1/07 9:06a erickson
* PR36633: update base enums
*
* Nexus_Devel/3   10/8/07 5:40p vsilyaev
* PR 35824: Renamed MPEG1 PES to MPEG1 PS (Program Stream)
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Fixing linker error
*
* Nexus_Devel/1   9/14/07 4:59p erickson
* PR34925: update
*
***************************************************************************/
#include "nexus_core_module.h"
#include "bkni.h"
#include "bfmt.h"

BDBG_MODULE(nexus_core_utils);

NEXUS_VideoFormat g_NEXUS_customVideoFormat;
NEXUS_VideoFormatInfo g_NEXUS_customVideoFormatInfo;

void NEXUS_P_VideoFormat_SetInfo( NEXUS_VideoFormat videoFormat, const NEXUS_VideoFormatInfo *pInfo )
{
    g_NEXUS_customVideoFormat = videoFormat;
    g_NEXUS_customVideoFormatInfo = *pInfo;
}

void NEXUS_VideoFormat_GetInfo(NEXUS_VideoFormat videoFormat, NEXUS_VideoFormatInfo *pInfo)
{
    BFMT_VideoInfo fmtInfo;
    BERR_Code rc = BERR_SUCCESS;
    BFMT_VideoFmt fmt;

    BKNI_Memset(pInfo, 0, sizeof(*pInfo));

    /* allow for one custom video format at a time. TODO: consider extending this if needed. */
    if (videoFormat == g_NEXUS_customVideoFormat) {
        *pInfo = g_NEXUS_customVideoFormatInfo;
        return;
    }

    rc = NEXUS_P_VideoFormat_ToMagnum(videoFormat, &fmt);
    if (rc) {BERR_TRACE(rc); return;}

    rc = BFMT_GetVideoFormatInfo(fmt, &fmtInfo);
    if (rc) {BERR_TRACE(rc); return;}

    BDBG_MSG(("NEXUS_VideoFormat_GetInfo %d->%d: %dx%d", videoFormat, fmt, fmtInfo.ulWidth, fmtInfo.ulHeight));

    pInfo->width = fmtInfo.ulWidth;
    pInfo->height = fmtInfo.ulHeight;
    pInfo->digitalWidth = fmtInfo.ulDigitalWidth;
    pInfo->digitalHeight = fmtInfo.ulDigitalHeight;
    pInfo->scanWidth = fmtInfo.ulScanWidth;
    pInfo->scanHeight = fmtInfo.ulScanHeight;
    pInfo->topActive = fmtInfo.ulTopActive;
    pInfo->topMaxVbiPassThru = fmtInfo.ulTopMaxVbiPassThru;
    pInfo->bottomMaxVbiPassThru = fmtInfo.ulBotMaxVbiPassThru;
    pInfo->verticalFreq = fmtInfo.ulVertFreq;
    pInfo->interlaced = fmtInfo.bInterlaced;
    pInfo->aspectRatio = fmtInfo.eAspectRatio;
    pInfo->pixelFreq = fmtInfo.ulPxlFreq;

    pInfo->isFullRes3d = BFMT_IS_3D_MODE(fmt);
    return;
}

void
NEXUS_LookupFrameRate(unsigned frameRateInteger, NEXUS_VideoFrameRate *pNexusFrameRate)
{
    NEXUS_P_LookupFrameRate(frameRateInteger, pNexusFrameRate);
    return;
}

struct NEXUS_FrontendConnector
{
    NEXUS_OBJECT(NEXUS_FrontendConnector);
};

NEXUS_FrontendConnectorHandle NEXUS_FrontendConnector_Create(void)
{
    NEXUS_FrontendConnectorHandle handle;
    handle = BKNI_Malloc(sizeof(*handle));
    if (!handle) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_FrontendConnector, handle);
    return handle;
}

NEXUS_FrontendConnectorHandle NEXUS_FrontendConnector_Acquire(unsigned index)
{
    BSTD_UNUSED(index);
    return NULL;
}

void NEXUS_FrontendConnector_Release(NEXUS_FrontendConnectorHandle handle)
{
    BSTD_UNUSED(handle);
}  

static void NEXUS_FrontendConnector_P_Finalizer( NEXUS_FrontendConnectorHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_FrontendConnector, handle);
    NEXUS_OBJECT_DESTROY(NEXUS_FrontendConnector, handle);
    BKNI_Free(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_FrontendConnector, NEXUS_FrontendConnector_Destroy);
