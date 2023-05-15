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
 * $brcm_Workfile: nexus_video_window_tune.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/18/10 3:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_video_window_tune.c $
 * 
 * 4   6/18/10 3:28p bandrews
 * SW35230-271: remove warning
 * 
 * 3   6/17/10 1:36p mbatchel
 * SW35230-271: Integrate 35230 tuning changes.
 * 
 * 2   6/2/10 8:30p bandrews
 * SW3548-2323: share nexus_video_window_tune.h; create others for 35230
 * 
 * 1   6/1/10 9:39p bandrews
 * SW3548-2323: unlinked work on 7420 with 35230, input and display only
 *  tuned on latter
 * 
 ***************************************************************************/
#include "nexus_display_module.h"
#include "bvdc_tune.h"

BDBG_MODULE(nexus_video_window_tune);

#define pVideo (&g_NEXUS_DisplayModule_State)

/* SW3548-2323 */
NEXUS_Error NEXUS_VideoWindow_SetTuneParams( 
    NEXUS_VideoWindowHandle window,
    const uint32_t * aFields,
    const uint32_t * aValues,
    int num
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if (window->vdcState.window)
    {
        mrc = BVDC_Window_SetTuneParam(window->vdcState.window, aFields, aValues, num);
        if (mrc) {rc = BERR_TRACE(mrc); goto done;}

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) {rc = BERR_TRACE(rc); goto done;}
    }
    else
    {
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

done:
    return rc;
}

NEXUS_Error NEXUS_VideoWindow_GetTuneParams(
    NEXUS_VideoWindowHandle window,
    const uint32_t * aFields,
    uint32_t * aValues,
    int num
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if (window->vdcState.window)
    {
        mrc = BVDC_Window_GetTuneParam(window->vdcState.window, aFields, aValues, num);
        if (mrc) {rc = BERR_TRACE(mrc); goto done;}
    }
    else
    {
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

done:
    return rc;
}

NEXUS_Error NEXUS_VideoWindow_ResetTuneParams(
    NEXUS_VideoWindowHandle window,
    const uint32_t * aFields, /* parameter ids */
    int num /* count of params */
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if (window->vdcState.window)
    {
#if NEXUS_DTV_PLATFORM > 2
#undef BVDC_Window_ResetTuneParam
        mrc = BVDC_Window_ResetTuneParam(window->vdcState.window, aFields, num);
        if (mrc) {rc = BERR_TRACE(mrc); goto done;}

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) {rc = BERR_TRACE(rc); goto done;}
#else
        BSTD_UNUSED(mrc);
        BSTD_UNUSED(aFields);
        BSTD_UNUSED(num);
#endif
    }
    else
    {
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

#if NEXUS_DTV_PLATFORM > 2
done:
#endif
    return rc;
}

NEXUS_Error NEXUS_VideoWindow_ResetAllTuneParams(
    NEXUS_VideoWindowHandle window
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if (window->vdcState.window)
    {
#if NEXUS_DTV_PLATFORM > 2
        mrc = BVDC_Window_ResetTune(window->vdcState.window);
#else
        mrc = BVDC_Window_ResetTuneParam(window->vdcState.window);
#endif
        if (mrc) {rc = BERR_TRACE(mrc); goto done;}

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) {rc = BERR_TRACE(rc); goto done;}
    }
    else
    {
        rc = BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

done:
    return rc;
}

