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
 * $brcm_Workfile: nexus_video_adj.c $
 * $brcm_Revision: 39 $
 * $brcm_Date: 6/15/12 1:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_video_adj.c $
 * 
 * 39   6/15/12 1:19p erickson
 * SW7425-3202: fix enable logic, add DBG
 * 
 * 38   6/15/12 12:45p erickson
 * SW7425-3202: add NEXUS_VideoWindow_SetCompressionSettings
 * 
 * 37   4/12/12 12:58p erickson
 * SW7420-2286: add NEXUS_VideoWindowMadSettings.pqEnhancement, default
 *  eOff
 * 
 * 36   4/10/12 5:15p erickson
 * SW7420-2286: set madSettings.ePqEnhancement = BVDC_Mode_eOff to prevent
 *  audio glitch because of lipsync adjustment
 * 
 * 35   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 34   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 33   4/20/11 6:12p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 32   9/17/10 12:07p mward
 * SW7125-424:  Add NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL to
 *  nexus_platform_features.h to use deinterlacer compression on platforms
 *  that need it, such as 97125.
 * 
 * 31   5/13/10 10:55a erickson
 * SW3548-2921: add pq_disabled=y support
 *
 * 30   3/26/10 12:26p erickson
 * SW3548-2858: functions that malloc/free custom data for VDC need to
 *
 * 29   2/11/10 1:38p erickson
 * SW7405-3829: change impl to new vdc bShrinkWidth api
 *
 * 28   2/10/10 2:35p erickson
 * SW7405-3829: refactor internal functions to create/destroy VDC window.
 *  NEXUS_VideoWindowMadSettings.shrinkWidth requires window to be
 *  recreated.
 *
 * 27   8/10/09 8:39a gmohile
 * PR 56400 : Fix legacy vdc mad settings
 *
 * 26   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 25   7/31/09 11:17a gmohile
 * PR 56512 : Add 7403 support
 *
 * 24   3/19/09 10:57a erickson
 * PR52461: get rid of BVDC_Source_Get/SetAnrConfiguration
 *
 * 23   3/17/09 2:00p erickson
 * PR53331: check return code on NEXUS_VideoInput_P_Get
 *
 * 22   2/24/09 6:08p nickh
 * PR52461: Merge 7420 ANR Nexus changes to main branch
 *
 * 7420_mcvp/2   2/24/09 5:58p nickh
 * PR52461: Restrict ANR changes to 7420
 *
 * 7420_mcvp/1   2/5/09 9:51a syang
 * PR 45785, PR 45789:  get rid of BVDC_Source_Get/SetAnrConfiguration
 *
 * 21   12/1/08 2:24p erickson
 * PR49499: consolidate CUSTOM_VIDEO_ADJ code into
 *  NEXUS_VideoAdj_P_SetCustomValues function
 *
 * 20   10/29/08 11:38a erickson
 * PR48028: added deringing and dejagging options to
 *  NEXUS_VideoWindowScalerSettings
 *
 * 19   9/25/08 9:45a erickson
 * PR47113: replace NEXUS_VideoWindow_SetScalerCoefficientIndex with
 *  NEXUS_VideoWindow_SetCoefficientIndexSettings
 *
 * 18   9/16/08 5:17p erickson
 * PR46967: fix naming of NEXUS_VideoWindowScalerSettings
 *
 * 17   7/17/08 9:34a erickson
 * PR44764: update for BVDC_Window_SetBandwidthEquationParams api change
 *
 * 16   7/11/08 5:05p erickson
 * PR44764: VDC API changing. commenting out for now.
 *
 * 15   6/18/08 10:04a erickson
 * PR43617: extended NEXUS_VideoWindowScalerSettings with bandwidth equation
 *  params
 *
 * 14   6/16/08 12:17p erickson
 * PR43699: Remove VDC Get's from NEXUS GetSettings calls to avoid race
 *  conditions
 *
 * 13   6/12/08 12:49p erickson
 * PR43488: merge
 *
 * PR43488/1   6/10/08 4:36p dyzhang
 * PR43488: anr need to be set in both source and window
 *
 * 12   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 11   5/21/08 9:39a erickson
 * PR40567: remove unneeded BCHP_CHIP != 7405 wrappers
 *
 * 10   4/7/08 10:37a vsilyaev
 * PR 40567: Partially revert back pixelformat settings, removed in main/6
 *
 * 9   4/3/08 10:47a erickson
 * PR41122: fix B_HAS_LEGACY_VDC
 *
 * 8   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 7   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 6   3/27/08 4:51p vsilyaev
 * PR 40818: Improved internal API to allow source private heaps
 *
 * 5   3/26/08 2:37p erickson
 * PR40950: set custom ANR/DNR data in custom function
 *
 * 4   3/26/08 12:54p erickson
 * PR40567: add MAD and ANR pixel format
 *
 * PR40567/2   3/26/08 6:22p dyzhang
 * PR40567: use pixel format transfer function
 *
 * PR40567/1   3/26/08 4:20p xhuang
 * PR40567:add pixel format set in Nexus
 *
 * 3   3/24/08 10:39a erickson
 * PR39202: merge
 *
 * PR39202/1   3/21/08 1:37p xhuang
 * PR39202: temp to add default setting for MAD pixel format to remove
 *  runtime error
 *
 * 2   1/25/08 9:42a erickson
 * PR39003: adapt to latest VDC
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/9   1/8/08 3:52p erickson
 * PR36159: rename DNR/ANR/MAD functions to be VideoWindow
 *
 * Nexus_Devel/8   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/7   11/29/07 2:23p vsilyaev
 * PR 36159: All XXX_GetYYYSettings functions shall return void
 *
 * Nexus_Devel/6   11/29/07 1:10p vsilyaev
 * PR 36159: Reworked capture function to return the Nexus Surface
 *
 * Nexus_Devel/5   11/29/07 11:41a vsilyaev
 * PR 36159: Another renaming of structre members
 *
 * Nexus_Devel/4   11/29/07 11:34a vsilyaev
 * PR 36159: Flatten structure to control noise removal
 *
 * Nexus_Devel/3   11/28/07 1:44p vsilyaev
 * PR 36159: Added global UpdateMode, removed per-function ApplyChanges
 *
 * Nexus_Devel/2   11/28/07 9:34a vsilyaev
 * PR36159: Renamed structure members in new API's
 *
 * Nexus_Devel/1   11/26/07 12:46p vsilyaev
 * PR36159: Merged 3563 code
 *
 * Nexus_Devel/PR36159/1   11/26/07 10:06a gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/1   10/18/07 9:59p gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"

BDBG_MODULE(nexus_video_adj);

/*============================private methods====================================*/

/***************************************************************************
Apply stored settings after the VDC window is created.

In Nexus, the window handle exists w/ or w/o a source.
In VDC, the window exists only if there's a connected source.
Therefore, Nexus must store the settings. After the VDC window is created, these settings can be applied.
****************************************************************************/
NEXUS_Error
NEXUS_VideoAdj_P_ApplySetSettings( NEXUS_VideoWindowHandle window )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoAdjContext  *pContext;
    BVDC_Window_Handle  windowVDC;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);

    /* Apply DNR settings */
    if (pContext->bDnrSet)
    {
        rc = NEXUS_VideoWindow_SetCustomDnrSettings(window,
            &pContext->stDnrSettings, pContext->customDnrData, 0 );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply ANR settings */
    if (pContext->bAnrSet)
    {
        rc = NEXUS_VideoWindow_SetCustomAnrSettings(window,
            &pContext->stAnrSettings, pContext->customAnrData, 0 );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply MAD settings */
    if (pContext->bMadSet )
    {
        rc = NEXUS_VideoWindow_SetMadSettings(window, &pContext->stMadSettings );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply Scaler settings */
    if (pContext->bSclSet)
    {
        rc = NEXUS_VideoWindow_SetScalerSettings(window,
            &pContext->stSclSettings
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    if (pContext->bGameModeDelaySet)
    {
        rc = NEXUS_VideoWindow_SetGameModeDelay(window, &pContext->stGameModeDelaySettings );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    if (pContext->bCoefficientIndexSettingsSet)
    {
        rc = NEXUS_VideoWindow_SetCoefficientIndexSettings(window, &pContext->coefficientIndexSettings);
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }
    
    rc = NEXUS_VideoWindow_SetCompressionSettings(window, &pContext->compressionSettings);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}


/*============================public methods====================================*/

/***************************************************************************
*
* DNR
*
****************************************************************************/
void
NEXUS_VideoWindow_GetDnrSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowDnrSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.stDnrSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetDnrSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowDnrSettings *pSettings )
{
    return NEXUS_VideoWindow_SetCustomDnrSettings(window, pSettings, NULL, 0);
}

NEXUS_Error NEXUS_VideoWindow_SetCustomDnrSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowDnrSettings *pSettings,
    const uint8_t *pData, unsigned numEntries )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoAdjContext *pContext;
    BVDC_Dnr_Settings  DnrSettings;
    BVDC_Window_Handle  windowVDC;
    NEXUS_VideoInput_P_Link *pLink;
    void *freeAfter = NULL;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);
    BDBG_ASSERT(pSettings);

    pContext->bDnrSet = true;
    pContext->stDnrSettings = *pSettings;

    /* NEXUS_VideoAdj_P_ApplySetSettings might call this function using the already-saved data */
    if (pContext->customDnrData != pData || numEntries != 0) {
        if (pContext->customDnrData) {
            /* we cannot free pContext->customDnrData because VDC still has the memory. free after BVDC_ApplyChanges.
            if freeAfter is set, we must execute the "done:" code to avoid a KNI leak. */
            freeAfter = pContext->customDnrData;
            pContext->customDnrData = NULL;
        }
        if (pData) {
            /* VDC does not copy the data, so we have to. */
            pContext->customDnrData = BKNI_Malloc(numEntries * sizeof(pData[0]));
            if (!pContext->customDnrData) {
                rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                goto done;
            }
            BKNI_Memcpy(pContext->customDnrData, pData, numEntries * sizeof(pData[0]));
        }
    }

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) /* Window not connected */
    {
        rc = NEXUS_SUCCESS;
        goto done;
    }

    pLink = NEXUS_VideoInput_P_Get(window->input);
    if (!pLink) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto done;
    }

    BVDC_Source_GetDnrConfiguration(pLink->sourceVdc, &DnrSettings);
    DnrSettings.eBnrMode = pSettings->bnr.mode;
    DnrSettings.eDcrMode = pSettings->dcr.mode;
    DnrSettings.eMnrMode = pSettings->mnr.mode;
    DnrSettings.iBnrLevel = pSettings->bnr.level;
    DnrSettings.iMnrLevel = pSettings->mnr.level;
    DnrSettings.iDcrLevel = pSettings->dcr.level;
    DnrSettings.pvUserInfo = pContext->customDnrData;
    DnrSettings.ulQp = pSettings->qp;
    rc = BVDC_Source_SetDnrConfiguration(pLink->sourceVdc, &DnrSettings);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto done;
    }

    if (freeAfter) {
        /* This NEXUS_Display_P_ApplyChanges must take effect if freeAfter is set. */
        if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);} /* fall through */

        rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
        if (rc) {
            rc = BERR_TRACE(rc);
            goto done;
        }
    }
    else {
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) {
            rc = BERR_TRACE(rc);
            goto done;
        }
    }

done:
    if (freeAfter) {
        BKNI_Free(freeAfter);
    }
    return rc;
}   /* NEXUS_VideoWindow_SetDnrSettings() */

/***************************************************************************
Summary:
    Get current MAD configurations.
****************************************************************************/
void
NEXUS_VideoWindow_GetMadSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowMadSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.stMadSettings;
}

/***************************************************************************
Summary:
    Set MAD configurations.
****************************************************************************/
NEXUS_Error
NEXUS_VideoWindow_SetMadSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowMadSettings *pSettings
    )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoAdjContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_Deinterlace_Settings madSettings;
    BVDC_Deinterlace_Reverse22Settings Reverse22Setting;
    BVDC_Deinterlace_Reverse32Settings Reverse32Setting;
    BVDC_Deinterlace_ChromaSettings ChromaSettings;
    BVDC_Deinterlace_MotionSettings MotionSettings;
    BVDC_422To444UpSampler upSampler;
    BVDC_444To422DnSampler downSampler;
    BVDC_Deinterlace_LowAngleSettings lowAngles;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);
    BDBG_ASSERT(pSettings);

    pContext->bMadSet = true;
    pContext->stMadSettings = *pSettings;

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
    {
        return NEXUS_SUCCESS;
    }

    /* must hook up substructures before getting a default from VDC.
    there is a risk here that a new substructure is added and not defaults, therefore we memset. */
    BKNI_Memset(&madSettings, 0, sizeof(madSettings));
    madSettings.pReverse22Settings = &Reverse22Setting;
    madSettings.pReverse32Settings = &Reverse32Setting;
    madSettings.pChromaSettings = &ChromaSettings;
    madSettings.pMotionSettings = &MotionSettings;
    madSettings.pUpSampler = &upSampler;
    madSettings.pDnSampler = &downSampler;
    madSettings.pLowAngles = &lowAngles;
    BVDC_Window_GetDeinterlaceDefaultConfiguration(windowVDC, &madSettings);

    madSettings.bReverse32Pulldown = pSettings->enable32Pulldown;
    madSettings.bReverse22Pulldown = pSettings->enable22Pulldown;
    BDBG_CASSERT(NEXUS_VideoWindowGameMode_eMax == (NEXUS_VideoWindowGameMode)BVDC_MadGameMode_eMaxCount);
    madSettings.eGameMode = pSettings->gameMode;
    madSettings.bShrinkWidth = pSettings->shrinkWidth;
    madSettings.ePqEnhancement = pSettings->pqEnhancement;

    if (pSettings->pixelFormat != NEXUS_PixelFormat_eUnknown) {
        rc = NEXUS_P_PixelFormat_ToMagnum(pSettings->pixelFormat, &madSettings.ePxlFormat);
        if (rc) return BERR_TRACE(rc);
    }

    madSettings.stVideoTestFeature1.bEnable = pContext->compressionSettings.mad.enabled;
    madSettings.stVideoTestFeature1.ulBitsPerPixel = pContext->compressionSettings.mad.compressionAmount;
    BDBG_MSG(("MAD enabled %d, compression? %d", 
        pSettings->deinterlace, madSettings.stVideoTestFeature1.bEnable?madSettings.stVideoTestFeature1.ulBitsPerPixel:0));
    
    rc = BVDC_Window_SetDeinterlaceConfiguration(windowVDC, pSettings->deinterlace, &madSettings);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}   /* NEXUS_VideoWindow_SetMadSettings() */

/***************************************************************************
Summary:
    Get scaler configurations.
****************************************************************************/
void
NEXUS_VideoWindow_GetScalerSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowScalerSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.stSclSettings;
}

/***************************************************************************
Summary:
    Set scaler configurations.
****************************************************************************/
NEXUS_Error
NEXUS_VideoWindow_SetScalerSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowScalerSettings *pSettings
    )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoAdjContext  *pContext;
    BVDC_Window_Handle  windowVDC;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);
    BDBG_ASSERT(pSettings);

    pContext->bSclSet = true;
    pContext->stSclSettings = *pSettings;
    if (pSettings->nonLinearScaling == false) {
        /* clear the setting to be zero after turning off non-linear. */
        pContext->stSclSettings.nonLinearSourceWidth = 0;
        pContext->stSclSettings.nonLinearScalerOutWidth = 0;
    }

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) /* window not connected */
    {
        return NEXUS_SUCCESS;
    }

    /* set scaler mode */
    if (pSettings->nonLinearScaling == true)
    {
        /* TODO - do we need to add in a restriction here to force the
        * nonLinearSourceWidth and nonLinearScalerOutWidth to be larger than one
        * quarter of the source/display width? */
        BDBG_MSG(("Turn on non-linear, nonLinearSourceWidth=%d, nonLinearScalerOutWidth=%d",
            pSettings->nonLinearSourceWidth,
            pSettings->nonLinearScalerOutWidth));

        rc = BVDC_Window_SetNonLinearScl(windowVDC,
            pSettings->nonLinearSourceWidth,
            pSettings->nonLinearScalerOutWidth);
        if (rc) return BERR_TRACE(rc);
    }
    else
    {
        /* set both source/scl output width to turn off non-linear. If non-linear
        * is turned off, the source/scl output width val in SCL settings will be
        * ignored. */
        BDBG_MSG(("Turn off non-linear"));

        rc = BVDC_Window_SetNonLinearScl(windowVDC, 0, 0);
        if (rc) return BERR_TRACE(rc);
    }

    BDBG_CASSERT(BVDC_SclCapBias_eSclAfterCap == (BVDC_SclCapBias)NEXUS_ScalerCaptureBias_eScalerAfterCapture);
    rc = BVDC_Window_SetBandwidthEquationParams(windowVDC, pSettings->bandwidthEquationParams.delta, pSettings->bandwidthEquationParams.bias);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

/***************************************************************************
*
* ANR
*
****************************************************************************/
void
NEXUS_VideoWindow_GetAnrSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowAnrSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.stAnrSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetAnrSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowAnrSettings *pSettings )
{
    return NEXUS_VideoWindow_SetCustomAnrSettings(window, pSettings, NULL, 0);
}

NEXUS_Error NEXUS_VideoWindow_SetCustomAnrSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowAnrSettings *pSettings,
    const uint8_t *pData, unsigned numEntries )
{
    NEXUS_VideoAdjContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    NEXUS_VideoInput_P_Link *pLink;
    BERR_Code rc = NEXUS_SUCCESS;
    BVDC_Anr_Settings anrCfg;
    void *freeAfter = NULL;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);
    BDBG_ASSERT(pSettings);

    pContext->bAnrSet = true;
    pContext->stAnrSettings = *pSettings;

    /* NEXUS_VideoAdj_P_ApplySetSettings might call this function using the already-saved data */
    if (pContext->customAnrData != pData || numEntries != 0) {
        if (pContext->customAnrData) {
            /* we cannot free pContext->customAnrData because VDC still has the memory. free after BVDC_ApplyChanges.
            if freeAfter is set, we must execute the "done:" code to avoid a KNI leak. */
            freeAfter = pContext->customAnrData;
            pContext->customAnrData = NULL;
        }
        if (pData) {
            /* VDC does not copy the data, so we have to. */
            pContext->customAnrData = BKNI_Malloc(numEntries * sizeof(pData[0]));
            if (!pContext->customAnrData) {
                rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                goto done;
            }
            BKNI_Memcpy(pContext->customAnrData, pData, numEntries * sizeof(pData[0]));
        }
    }

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) /* window not connected */
    {
        rc = NEXUS_SUCCESS;
        goto done;
    }

    pLink = NEXUS_VideoInput_P_Get(window->input);
    if (!pLink) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto done;
    }

    BVDC_Window_GetAnrConfiguration(windowVDC, &anrCfg);
    anrCfg.eMode = pSettings->anr.mode;
    anrCfg.iSnDbAdjust = pSettings->anr.level;
    anrCfg.pvUserInfo = pContext->customAnrData;

    if (pSettings->pixelFormat != NEXUS_PixelFormat_eUnknown) {
        rc = NEXUS_P_PixelFormat_ToMagnum(pSettings->pixelFormat, &anrCfg.ePxlFormat);
        if (rc) {
            rc = BERR_TRACE(rc);
            goto done;
        }
    }
    
    anrCfg.stVideoTestFeature1.bEnable = pContext->compressionSettings.anr.enabled;
    anrCfg.stVideoTestFeature1.ulBitsPerPixel = pContext->compressionSettings.anr.compressionAmount;
    BDBG_MSG(("ANR mode %d, compression? %d", 
        pSettings->anr.mode, anrCfg.stVideoTestFeature1.bEnable?anrCfg.stVideoTestFeature1.ulBitsPerPixel:0));

    rc = BVDC_Window_SetAnrConfiguration(windowVDC, &anrCfg);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto done;
    }

    if (freeAfter) {
        /* This NEXUS_Display_P_ApplyChanges must take effect if freeAfter is set. */
        if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);} /* fall through */

        rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
        if (rc) {
            rc = BERR_TRACE(rc);
            goto done;
        }
    }
    else {
        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) {
            rc = BERR_TRACE(rc);
            goto done;
        }
    }

done:
    if (freeAfter) {
        BKNI_Free(freeAfter);
    }
    return rc;
}

void NEXUS_VideoWindow_GetCoefficientIndexSettings( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowCoefficientIndexSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.coefficientIndexSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetCoefficientIndexSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowCoefficientIndexSettings *pSettings )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    window->adjContext.coefficientIndexSettings = *pSettings;
    window->adjContext.bCoefficientIndexSettingsSet = true;

    if (NULL == window->vdcState.window || g_NEXUS_DisplayModule_State.pqDisabled) {
        /* they will get applied when the window is created */
        return BERR_SUCCESS;
    }

    BDBG_CASSERT(sizeof(NEXUS_VideoWindowCoefficientIndexSettings) == sizeof(BVDC_CoefficientIndex));

    rc = BVDC_Window_SetCoefficientIndex(window->vdcState.window, (const BVDC_CoefficientIndex *)pSettings);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return rc;
}

NEXUS_Error NEXUS_VideoWindow_SetGameModeDelay( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowGameModeDelay *pSettings )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoAdjContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_Window_GameModeSettings GameModeDelaySettings;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->adjContext);
    BDBG_ASSERT(pSettings);

    pContext->bGameModeDelaySet = true;
    pContext->stGameModeDelaySettings = *pSettings;

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) {
        /* they will get applied when the window is created */
        return BERR_SUCCESS;
    }

    /* get a default to prevent uninitialized memory when new params are added */
    (void)BVDC_Window_GetGameModeDelay(windowVDC, &GameModeDelaySettings);

    GameModeDelaySettings.bEnable = pSettings->enable;
    GameModeDelaySettings.bForceCoarseTrack = pSettings->forceCoarseTrack;
    GameModeDelaySettings.ulBufferDelayTarget = pSettings->bufferDelayTarget;
    GameModeDelaySettings.ulBufferDelayTolerance = pSettings->bufferDelayTolerance;

    rc = BVDC_Window_SetGameModeDelay(windowVDC, &GameModeDelaySettings);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return rc;
}

void NEXUS_VideoWindow_GetGameModeDelay( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowGameModeDelay *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.stGameModeDelaySettings;
}

void NEXUS_VideoWindow_GetCompressionSettings( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowCompressionSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->adjContext.compressionSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetCompressionSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowCompressionSettings *pSettings )
{
    BVDC_Window_Handle windowVDC;
    BERR_Code rc;
    NEXUS_DisplayUpdateMode saveUpdateMode;
    NEXUS_VideoAdjContext  *pContext;
    
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    window->adjContext.compressionSettings = *pSettings;
    pContext = &(window->adjContext);
    
    if (!windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }
    
    /* switch to manual to avoid multiple BVDC_ApplyChanges */
    saveUpdateMode = g_NEXUS_DisplayModule_State.updateMode;
    g_NEXUS_DisplayModule_State.updateMode = NEXUS_DisplayUpdateMode_eManual;
    
#if 0
/* CAP/VFD not supported on settop */
    {
        BVDC_Video_TestFeature1_Settings windowSettings;
        (void)BVDC_Window_GetTestFeature1(windowVDC, &windowSettings);
        windowSettings.bEnable = pSettings->cap.enabled;
        windowSettings.ulBitsPerPixel = pSettings->cap.compressionAmount;
        rc = BVDC_Window_SetTestFeature1(windowVDC, &windowSettings);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }
#endif
    
    /* use nexus functions to avoid unsymmetric VDC get/set */
    /* ANR is not enabled by default, so user must call NEXUS_VideoWindow_SetAnrSettings beforehand to apply compression */
    if (pContext->bAnrSet) {
        rc = NEXUS_VideoWindow_SetCustomAnrSettings(window, &pContext->stAnrSettings, pContext->customAnrData, 0 );
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }

    /* either MAD was enabled by default or user must call NEXUS_VideoWindow_SetMadSettings beforehand to apply compression */
    if (pContext->bMadSet) {
        rc = NEXUS_VideoWindow_SetMadSettings(window, &pContext->stMadSettings );
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }
    
    g_NEXUS_DisplayModule_State.updateMode = saveUpdateMode;
    
    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    
    return 0;
    
error:
    g_NEXUS_DisplayModule_State.updateMode = saveUpdateMode;
    return rc;
}
