/*     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_picture_ctrl.c $
 * $brcm_Revision: 55 $
 * $brcm_Date: 3/14/12 12:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_picture_ctrl.c $
 * 
 * 55   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 54   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 53   7/29/10 6:43p mward
 * SW7125-541:  Set bInterpolateTables = false, since only one DC table is
 *  being provided.
 * 
 * 52   5/13/10 10:55a erickson
 * SW3548-2921: add pq_disabled=y support
 *
 * 51   3/26/10 5:25p erickson
 * SW3548-2858: fix typo
 *
 * 50   3/26/10 12:26p erickson
 * SW3548-2858: functions that malloc/free custom data for VDC need to
 *  free only after BVDC_ApplyChanges has released the memory from VDC's
 *  state
 *
 * 49   3/3/10 3:07p nickh
 * SW7420-565: Refactor dynamic contrast settings and set default dynamic
 *  contrast table settings
 *
 * 48   2/9/10 4:52p rjain
 * SW7550-198: For Dual display in Brutus, we need to add 7550 to this
 *  chip list
 *
 * 47   12/29/09 12:47p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 46   12/20/09 5:01p randyjew
 * SW7468-6:Add 7468 support
 *
 * 45   11/11/09 6:38p mward
 * SW7400-2606: Sharpness should default to minimum.
 *
 * 44   11/9/09 5:50p mward
 * SW7400-2574:  Don't set sharpness config on a window that doesnt'
 *  support it.
 *
 * 43   10/23/09 5:49p mward
 * SW7125-45: Enable AdvColorSettings on 7125.
 *
 * 42   9/29/09 10:28a mward
 * SW7125-27:  7125 also has sharpness control only on first window of
 *  primary display.
 *
 * 41   9/1/09 5:40p erickson
 * SW7420-303: set dynamic contrast gain shift. comment out unused dynamic
 *  contrast settings.
 *
 * 40   8/12/09 10:10p jrubio
 * PR55232: add 7340/7342 support
 *
 * 39   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 38   7/31/09 11:17a gmohile
 * PR 56512 : Add 7403 support
 *
 * 37   6/24/09 3:43p erickson
 * PR56233: extend PEP restriction for CMP0_V0 to 7325 and 7335
 *
 * 36   6/22/09 4:00p erickson
 * PR56233: remove DTV code. added #error if this code is built for DTV in
 *  the future.
 *
 * 35   5/29/09 2:34p jrubio
 * PR55232: fix typo
 *
 * 34   5/29/09 2:26p jrubio
 * PR55232: add 7342/7340 support
 *
 * 33   4/17/09 7:14a erickson
 * PR54147: move large BVDC_ContrastStretch structure off of the stack
 *
 * 32   3/12/09 11:55a nickh
 * PR52746: Use the default dynamic contrast shift value from VDC to
 *  adjust the gain to a valid range
 *
 * 31   2/6/09 11:48a jrubio
 * PR51629: add 7336 support
 *
 * 30   12/12/08 4:14p katrep
 * PR48426: fix tnt error for 7405
 *
 * 29   11/28/08 1:01p nickh
 * PR48963: Add 7420 support
 *
 * 28   11/25/08 8:25p jrubio
 * PR48426: fix TNT error, make sure that 7325/7335 only use main
 *  display/main window to set settings
 *
 * 27   11/19/08 6:46p mward
 * PR47739: For 7400 make sure PEP, TNT are used only for first window of
 *  HD display.
 *
 * 26   11/14/08 5:44p jgarrett
 * PR 48957: Updating to match new VDC API style
 *
 * 25   9/22/08 12:10p erickson
 * PR47190: added NEXUS_PictureCtrl_SetColorClipSettings
 *
 * 24   9/22/08 10:34a erickson
 * PR46948: merge
 *
 * PR46948/1   9/22/08 2:53p chengs
 * PR46948: Backlight Dimming in Dynamic Contrast code.
 *
 * 23   9/5/08 1:16p erickson
 * PR45897: add dynamic backlight options for panels
 *
 * 22   9/2/08 10:21a erickson
 * PR45181: update hungarian notation change
 *
 * PR45479/1   8/12/08 12:25p chengs
 * PR45479: Updated Dynamic Contrast code.
 *
 * 20   6/26/08 1:21p erickson
 * PR43948: hardcode constrast stretch on for DTV
 *
 * 19   6/26/08 10:20a erickson
 * PR44071: prevent overlap of NEXUS_PictureCtrlCommonSettings.colorTemp
 *  and NEXUS_PictureCtrlAdvColorSettings attenuation settings. added
 *  enable booleans.
 *
 * 18   6/23/08 4:22p erickson
 * PR43699: Remove VDC Get's from NEXUS GetSettings calls to avoid race
 *  conditions
 *
 * 17   6/16/08 4:30p erickson
 * PR43474: roll back merge. feature does not work.
 *
 * 15   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 14   5/9/08 3:47p erickson
 * PR42552: readd Display luma stats functions
 *
 * 13   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 12   4/10/08 11:07a erickson
 * PR41595: call BVDC_Window_GetLumaStatsConfiguration to init structure
 *
 * 11   4/8/08 3:18p erickson
 * PR39453: add 3548 & 3556
 *
 * 10   4/3/08 10:47a erickson
 * PR41122: fix B_HAS_LEGACY_VDC
 *
 * 9   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 8   4/2/08 2:47p erickson
 * PR40950: move PictureCtrl custom data into function params
 *
 * 7   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 6   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 5   2/22/08 5:09p erickson
 * PR39738: fix mem leak
 *
 * 4   2/21/08 5:48p erickson
 * PR39003: PQ api change
 *
 * PR38710/1   2/1/08 5:18p chengs
 * PR38710: Rename GreenStretch and BlueStretch to GreenBoost and
 *  BlueBoost,  support new BlueStretch.
 *
 * 2   1/25/08 9:42a erickson
 * PR39003: adapt to latest VDC
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/13   1/11/08 11:04a erickson
 * PR36159: refactored LumaStatistics api
 *
 * Nexus_Devel/12   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/11   12/18/07 1:35p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/10   12/12/07 2:55p gezhang
 * PR36159: Correct "offsetG" and "offsetB" in
 * NEXUS_PictureCtrl_GetAdvColorSettings()
 *
 * Nexus_Devel/9   11/30/07 10:52a erickson
 * PR35457: api update
 *
 * Nexus_Devel/8   11/29/07 2:23p vsilyaev
 * PR 36159: All XXX_GetYYYSettings functions shall return void
 *
 * Nexus_Devel/7   11/29/07 10:57a vsilyaev
 * PR 36159: Use flat structure to control color conversion
 *
 * Nexus_Devel/6   11/29/07 10:33a vsilyaev
 * PR 36159: More renaming of structure members
 *
 * Nexus_Devel/5   11/29/07 10:17a vsilyaev
 * PR 36159: Flattened structure to load custom tables
 *
 * Nexus_Devel/4   11/28/07 5:47p vsilyaev
 * PR 36159: Use fixed point numbers for attenuation
 *
 * Nexus_Devel/3   11/28/07 1:44p vsilyaev
 * PR 36159: Added global UpdateMode, removed per-function ApplyChanges
 *
 * Nexus_Devel/2   11/28/07 9:34a vsilyaev
 * PR36159: Renamed structure members in new API's
 *
 * Nexus_Devel/1   11/26/07 12:47p vsilyaev
 * PR36159: PR36159: Bring up Nexus on 93563 platform
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

BDBG_MODULE(NEXUS_picture_ctrl);

/*=======================data structures===================================*/
#define NEXUS_CCB_NUM_GAMMA_VALUE 3
#define NEXUS_CCB_NUM_COLOR_TEMP  5

/*============================private methods====================================*/

/***************************************************************************
Summary:
    Apply the settings set before calling window connected.
Description:
    VDC Window is created in NEXUS_VideoWindow_P_Setup, "setxxxSettings"
    functions will only take effect after video window connect.
    If you called "setxxxSettings" functions before window connected,
    we will save the settings in the VWIN context and set flags to dirty.
    Then in window connecting, we will call this function to apply the
    dirty settings.
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_P_ApplySetSettings( NEXUS_VideoWindowHandle window )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);

    /* Apply Color settings */
    if (pContext->bCommonSet)
    {
        rc = NEXUS_PictureCtrl_SetCommonSettings(window,
            &pContext->stCommonSettings
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply Enhanced Color settings */
    if (pContext->bAdvColorSet)
    {
        rc = NEXUS_PictureCtrl_SetAdvColorSettings(window,
            &pContext->stAdvColorSettings
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply customized contrast stretch settings */
    if (pContext->bCustomContrastSet)
    {
        rc = NEXUS_PictureCtrl_SetCustomContrastStretch(window,
            &pContext->stCustomContrast,
            pContext->customContrastStretchData,
            0
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply customized sharpness settings */
    if (pContext->bCustomSharpnessSet)
    {
        rc = NEXUS_PictureCtrl_SetSharpnessValue(window,
            &pContext->stCustomSharpness
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply color management settings */
    if (pContext->bCmsSet)
    {
        rc = NEXUS_PictureCtrl_SetCmsSettings(window,
            &pContext->stCmsSettings
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    /* Apply dither settings */
    if (pContext->bDitherSet)
    {
        rc = NEXUS_PictureCtrl_SetDitherSettings(window,
            &pContext->stDitherSettings
            );
        if (rc != NEXUS_SUCCESS) return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
}


/*============================public methods====================================*/

/***************************************************************************
Summary:
    Get contrast stretch parameters.
****************************************************************************/
void
NEXUS_PictureCtrl_GetContrastStretch(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlContrastStretch *pContrast)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pContrast = window->picContext.stCustomContrast;
}

/***************************************************************************
Summary:
    Set contrast stretch parameters.
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetContrastStretch( NEXUS_VideoWindowHandle window, const NEXUS_PictureCtrlContrastStretch *pContrast)
{
    return NEXUS_PictureCtrl_SetCustomContrastStretch(window, pContrast, NULL, 0);
}

NEXUS_Error NEXUS_PictureCtrl_SetCustomContrastStretch( NEXUS_VideoWindowHandle window, const NEXUS_PictureCtrlContrastStretch *pContrast, const int16_t *pTable, unsigned numTableEntries )
{
#if NEXUS_NUM_LAB
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_PictureCtrlContext *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_ContrastStretch *pVdcContrastStretchSettings;
    void *freeAfter = NULL;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pContrast);

    pContext->bCustomContrastSet = true;
    pContext->stCustomContrast = *pContrast;

    if ( !((window->display->index == 0) && (window->windowId == BVDC_WindowId_eVideo0))) {
        /* TODO: if HW doesn't exist, should we really avoid the error? */
        return 0;
    }

    /* NEXUS_PictureCtrl_P_ApplySetSettings might call this function using the already-saved data */
    if (pContext->customContrastStretchData != pTable || numTableEntries != 0) {
        if (pContext->customContrastStretchData) {
            /* we cannot free pContext->customContrastStretchData because VDC still has the memory. free after BVDC_ApplyChanges.
            if freeAfter is set, we must execute the "done:" code to avoid a KNI leak. */
            freeAfter = pContext->customContrastStretchData;
            pContext->customContrastStretchData = NULL;
        }
        if (pTable) {
            /* VDC does not copy the data, so we have to. */
            pContext->customContrastStretchData = BKNI_Malloc(numTableEntries * sizeof(pTable[0]));
            if (!pContext->customContrastStretchData) {
                rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                goto done;
            }
            BKNI_Memcpy(pContext->customContrastStretchData, pTable, numTableEntries * sizeof(pTable[0]));
        }
    }

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled) {
        rc = NEXUS_SUCCESS;
        goto done;
    }

    pVdcContrastStretchSettings = BKNI_Malloc(sizeof(BVDC_ContrastStretch));
    if (!pVdcContrastStretchSettings) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }

    rc = BVDC_Window_GetContrastStretch(windowVDC, pVdcContrastStretchSettings);
    if (rc) {
        BKNI_Free(pVdcContrastStretchSettings);
        rc = BERR_TRACE(rc);
        goto done;
    }

    pVdcContrastStretchSettings->iGain = pContrast->gain;
    pVdcContrastStretchSettings->ulShift = pContrast->gainShift;

    BDBG_CASSERT(sizeof(pVdcContrastStretchSettings->aulDcTable1) == sizeof(window->picContext.stCustomContrast.dcTable1));
    BKNI_Memcpy(&pVdcContrastStretchSettings->aulDcTable1[0], &window->picContext.stCustomContrast.dcTable1[0], sizeof(pVdcContrastStretchSettings->aulDcTable1));
    pVdcContrastStretchSettings->bInterpolateTables = false;
    BDBG_CASSERT(sizeof(pVdcContrastStretchSettings->alIreTable) == sizeof(window->picContext.stCustomContrast.ireTable));
    BKNI_Memcpy(&pVdcContrastStretchSettings->alIreTable[0], &window->picContext.stCustomContrast.ireTable[0], sizeof(pVdcContrastStretchSettings->alIreTable));

#if 0
/* unused */
    pVdcContrastStretchSettings->iFilterNum = pContrast->filterNum;
    pVdcContrastStretchSettings->iFilterDenom = pContrast->filterDenom;
    pVdcContrastStretchSettings->iDynContrastBlackGain = pContrast->dynamicContrastBlackGain;
    pVdcContrastStretchSettings->iDynContrastWhiteGain = pContrast->dynamicContrastWhiteGain;
    pVdcContrastStretchSettings->uiDynContrastBlackLimit = pContrast->dynamicContrastBlackLimit;
    pVdcContrastStretchSettings->uiDynContrastWhiteLimit = pContrast->dynamicContrastWhiteLimit;
    pVdcContrastStretchSettings->uiDynContrastEstCurMaxPt = pContrast->dynamicContrastEstCurMaxPt;
    pVdcContrastStretchSettings->uiDynContrastEstCurMinPt = pContrast->dynamicContrastEstCurMinPt;
#endif
    pVdcContrastStretchSettings->pvCustomParams = pContext->customContrastStretchData;
    pVdcContrastStretchSettings->pfCallback = NULL;

    rc = BVDC_Window_SetContrastStretch(windowVDC, pVdcContrastStretchSettings);
    if (rc) {
        BKNI_Free(pVdcContrastStretchSettings);
        rc = BERR_TRACE(rc);
        goto done;
    }
    BKNI_Free(pVdcContrastStretchSettings);

    /* TODO: use pContrast->enabled instead? */
    if(pContrast->gain==0&&pContrast->dynamicContrastBlackGain==0&&pContrast->dynamicContrastWhiteGain==0)
        rc = BVDC_Window_EnableContrastStretch(windowVDC, false);
    else
        rc = BVDC_Window_EnableContrastStretch(windowVDC, true);
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
#else
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BSTD_UNUSED(pContrast);
    BSTD_UNUSED(pTable);
    BSTD_UNUSED(numTableEntries);
    return BERR_SUCCESS;
#endif
}

/***************************************************************************
Summary:
    Get customer sharpness setting registers.
****************************************************************************/
void NEXUS_PictureCtrl_GetSharpnessValue( NEXUS_VideoWindowHandle window, NEXUS_PictureCtrlSharpnessValue *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->picContext.stCustomSharpness;
}

/***************************************************************************
Summary:
    Load customer sharpness setting registers.
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetSharpnessValue( NEXUS_VideoWindowHandle window, const NEXUS_PictureCtrlSharpnessValue *pData )
{
#if NEXUS_HAS_TNT
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BERR_Code rc = BERR_SUCCESS;
    BVDC_SharpnessSettings sharpnessCfg;
    bool enable = false;
    int16_t sharpness;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pData);

    if ( (window->display->index == 0) && (window->windowId == BVDC_WindowId_eVideo0))
    {

        pContext->bCustomSharpnessSet = true;
        pContext->stCustomSharpness = *pData;

        if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
        {
            return NEXUS_SUCCESS;
        }

        rc = BVDC_Window_GetSharpnessConfig(windowVDC,
            &sharpnessCfg);
        if (rc) return BERR_TRACE(rc);

        /* update customer settings */
        sharpnessCfg.ulLumaCtrlCore = pData->lumaControlCore;
        sharpnessCfg.ulLumaCtrlGain = pData->lumaControlGain;
        sharpnessCfg.ulLumaCtrlBlur = pData->lumaControlBlur;
        sharpnessCfg.bLumaCtrlSoften = pData->lumaCtrlSoften;
        sharpnessCfg.bLumaCtrlHOnly = pData->lumaCtrlHOnly;
        sharpnessCfg.ulLumaPeakingHAvoid = pData->lumaPeakingHAvoid;
        sharpnessCfg.ulLumaPeakingVAvoid = pData->lumaPeakingVAvoid;
        sharpnessCfg.ulLumaPeakingPeakLimit = pData->lumaPeakingPeakLimit;
        sharpnessCfg.ulLumaPeakingPeakValue = pData->lumaPeakingPeakValue;
        sharpnessCfg.ulChromaCtrlCore = pData->chromaControlCore;
        sharpnessCfg.bChromaCtrlWideChroma = pData->chromaCtrlWideChroma;
        sharpnessCfg.ulChromaCtrlFalseColor = pData->chromaControlFalseColor;
        sharpnessCfg.ulChromaCtrlGain = pData->chromaControlGain;
        sharpnessCfg.bChromaCtrlHOnly = pData->chromaCtrlHOnly;
        sharpnessCfg.ulWideLumaCtrlCore = pData->wideLumaControlCore;
        sharpnessCfg.ulWideLumaCtrlMode = pData->wideLumaControlMode;
        if (pData->enable == true)
        {
            rc = BVDC_Window_SetSharpnessConfig(windowVDC,
                &sharpnessCfg);
        }
        else
        {
            /* use "NULL" to disable customized sharpness */
            rc = BVDC_Window_SetSharpnessConfig(windowVDC,
                NULL);
        }
        if (rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            return NEXUS_UNKNOWN;
        }

        /* BVDC_Window_SetSharpness() must be called for customized
         * sharpness settings to take effect. */
        rc = BVDC_Window_GetSharpness(windowVDC,
            &enable,
            &sharpness);
        if (rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            return NEXUS_UNKNOWN;
        }

        rc = BVDC_Window_SetSharpness(windowVDC,
            pData->enable,
            sharpness);
        if (rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            return NEXUS_UNKNOWN;
        }

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(window);
    BSTD_UNUSED(pData);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}


/***************************************************************************
Summary:
    Get color configurations.
****************************************************************************/
void
NEXUS_PictureCtrl_GetCommonSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlCommonSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->picContext.stCommonSettings;
}

/***************************************************************************
Summary:
    Set color configurations.
****************************************************************************/
NEXUS_Error
NEXUS_PictureCtrl_SetCommonSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlCommonSettings *pSettings
    )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pSettings);

    pContext->bCommonSet = true;
    pContext->stCommonSettings = *pSettings;

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
    {
        return NEXUS_SUCCESS;
    }

    rc = BVDC_Window_SetContrast(windowVDC, pSettings->contrast);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_Window_SetSaturation(windowVDC, pSettings->saturation);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_Window_SetHue(windowVDC, pSettings->hue);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_Window_SetBrightness(windowVDC, pSettings->brightness);
    if (rc) return BERR_TRACE(rc);

#if NEXUS_HAS_TNT
    /* Pending PR49461, Check for 1st window of primary display */
    if ( (window->display->index == 0) && (window->windowId == BVDC_WindowId_eVideo0))
    {
        rc = BVDC_Window_SetSharpness(windowVDC,true, pSettings->sharpness); /* once to set the value */
        if (pSettings->sharpness == -32768)
        {
            rc = BVDC_Window_SetSharpness(windowVDC,false, pSettings->sharpness); /* again to disable, if value is min. */
        }
        if (rc) return BERR_TRACE(rc);
    }
#endif

    if (pSettings->colorTempEnabled) {
        rc = BVDC_Window_SetColorTemp(windowVDC, pSettings->colorTemp);
        if (rc) return BERR_TRACE(rc);
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Get enhanced color configurations.
****************************************************************************/
void
NEXUS_PictureCtrl_GetAdvColorSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlAdvColorSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->picContext.stAdvColorSettings;
}

#define NEXUS_P_ATTENUATION_LIMIT(a) (((a)<NEXUS_PICTURE_ATTENUATION_BASE) ? (a) : NEXUS_PICTURE_ATTENUATION_BASE)

/***************************************************************************
Summary:
    Set enhanced color configurations.
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetAdvColorSettings( NEXUS_VideoWindowHandle window, const NEXUS_PictureCtrlAdvColorSettings *pSettings )
{
#if NEXUS_NUM_CAB
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_BlueStretch stBlueStretchSettings;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pSettings);

    /* Pending PR49461, Check for 1st window of primary display */
    if ( (window->display->index == 0) && (window->windowId == BVDC_WindowId_eVideo0))
    {
        pContext->bAdvColorSet = true;
        pContext->stAdvColorSettings = *pSettings;

        if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
        {
            return NEXUS_SUCCESS;
        }

        if (pSettings->attenuationRbgEnabled) {
            int32_t lAttenuationR, lAttenuationG, lAttenuationB;
            lAttenuationR = NEXUS_P_ATTENUATION_LIMIT(pSettings->attenuationR);
            lAttenuationG = NEXUS_P_ATTENUATION_LIMIT(pSettings->attenuationG);
            lAttenuationB = NEXUS_P_ATTENUATION_LIMIT(pSettings->attenuationB);

            rc = BVDC_Window_SetAttenuationRGB(windowVDC,
                lAttenuationR,
                lAttenuationG,
                lAttenuationB,
                pSettings->offsetR,
                pSettings->offsetG,
                pSettings->offsetB);
            if (rc) return BERR_TRACE(rc);
        }

        /* load CAB table */
        rc = BVDC_Window_SetAutoFlesh(windowVDC,
            pSettings->fleshTone);
        if (rc) return BERR_TRACE(rc);

        rc = BVDC_Window_SetGreenBoost(windowVDC,
            pSettings->greenBoost);
        if (rc) return BERR_TRACE(rc);

        rc = BVDC_Window_SetBlueBoost(windowVDC,
            pSettings->blueBoost);
        if (rc) return BERR_TRACE(rc);

        stBlueStretchSettings.ulBlueStretchOffset = pSettings->blueStretchSettings.blueStretchOffset;
        stBlueStretchSettings.ulBlueStretchSlope = pSettings->blueStretchSettings.blueStretchSlope;
        rc = BVDC_Window_SetBlueStretch(windowVDC,
                &stBlueStretchSettings);
        if (rc) return BERR_TRACE(rc);


        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);

    }
#else
    BSTD_UNUSED(window);
    BSTD_UNUSED(pSettings);
#endif

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Get CMS control Settings.
****************************************************************************/
void
NEXUS_PictureCtrl_GetCmsSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlCmsSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->picContext.stCmsSettings;
}

/***************************************************************************
Summary:
    Set CMS control Settings.
****************************************************************************/
NEXUS_Error
NEXUS_PictureCtrl_SetCmsSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlCmsSettings *pSettings
    )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_ColorBar hueGain, saturationGain;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pSettings);

    pContext->bCmsSet = true;
    pContext->stCmsSettings = *pSettings;

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
    {
        return NEXUS_SUCCESS;
    }

    hueGain.lRed = pSettings->hueGain.red;
    hueGain.lGreen = pSettings->hueGain.green;
    hueGain.lBlue = pSettings->hueGain.blue;
    hueGain.lCyan = pSettings->hueGain.cyan;
    hueGain.lMagenta = pSettings->hueGain.magenta;
    hueGain.lYellow = pSettings->hueGain.yellow;
    saturationGain.lRed = pSettings->saturationGain.red;
    saturationGain.lGreen = pSettings->saturationGain.green;
    saturationGain.lBlue = pSettings->saturationGain.blue;
    saturationGain.lCyan = pSettings->saturationGain.cyan;
    saturationGain.lMagenta = pSettings->saturationGain.magenta;
    saturationGain.lYellow = pSettings->saturationGain.yellow;

    rc = BVDC_Window_SetCmsControl(windowVDC,
        &saturationGain,
        &hueGain);
    if (rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        return NEXUS_UNKNOWN;
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return rc;
}

/***************************************************************************
Summary:
    Get current window dither configurations.
****************************************************************************/
void
NEXUS_PictureCtrl_GetDitherSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlDitherSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->picContext.stDitherSettings;
}

/***************************************************************************
Summary:
    Set Dither configurations.
****************************************************************************/
NEXUS_Error
NEXUS_PictureCtrl_SetDitherSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlDitherSettings *pSettings
    )
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_PictureCtrlContext  *pContext;
    BVDC_Window_Handle  windowVDC;
    BVDC_DitherSettings ditherSettings;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    pContext = &(window->picContext);
    BDBG_ASSERT(pSettings);

    pContext->bDitherSet = true;
    pContext->stDitherSettings = *pSettings;

    if (NULL == windowVDC || g_NEXUS_DisplayModule_State.pqDisabled)
    {
        return NEXUS_SUCCESS;
    }

    ditherSettings.bReduceSmooth = pSettings->reduceSmooth;
    ditherSettings.bSmoothEnable = pSettings->smoothEnable;
    ditherSettings.ulSmoothLimit = pSettings->smoothLimit;
    rc = BVDC_Window_SetDitherConfiguration(windowVDC,
        &ditherSettings);
    if (rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        return NEXUS_UNKNOWN;
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    This function loads the CAB table.
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_LoadCabTable( NEXUS_VideoWindowHandle window, const uint32_t *pTable, unsigned numTableEntries, unsigned offset )
{
#if NEXUS_HAS_PEP
    BVDC_Window_Handle  windowVDC;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;

    if (NULL == windowVDC)
    {
        BDBG_ERR(("VideoWindow must have VideoInput connected to load CAB table"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    if (pTable && numTableEntries > 0)
    {
        rc = BVDC_Window_LoadCabTable(windowVDC, pTable, offset, numTableEntries);
        if (rc) return BERR_TRACE(rc);
    }
    else
    {
        rc = BVDC_Window_LoadCabTable(windowVDC, NULL, 0, 0);
        if (rc) return BERR_TRACE(rc);
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(window);
    BSTD_UNUSED(pTable);
    BSTD_UNUSED(numTableEntries);
    BSTD_UNUSED(offset);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

/***************************************************************************
Summary:
    This function loads the LAB table.
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_LoadLabTable( NEXUS_VideoWindowHandle window, const uint32_t *pTable, unsigned numTableEntries, unsigned offset )
{
#if NEXUS_HAS_PEP
    BVDC_Window_Handle  windowVDC;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;

    if (NULL == windowVDC)
    {
        BDBG_ERR(("VideoWindow must have VideoInput connected to load LAB table"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    if (pTable && numTableEntries > 0)
    {
        rc = BVDC_Window_LoadLabTable(windowVDC, pTable, offset, numTableEntries);
        if (rc) return BERR_TRACE(rc);
    }
    else
    {
        rc = BVDC_Window_LoadLabTable(windowVDC, NULL, 0, 0);
        if (rc) return BERR_TRACE(rc);
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(window);
    BSTD_UNUSED(pTable);
    BSTD_UNUSED(numTableEntries);
    BSTD_UNUSED(offset);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error
NEXUS_PictureCtrl_ConfigDisplayLumaStatistics(
    NEXUS_DisplayHandle display,
    const NEXUS_ClipRect *pRect)
{
    BVDC_LumaSettings lumaSettings;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    BVDC_Compositor_GetLumaStatsConfiguration(display->compositor, &lumaSettings);

    lumaSettings.stRegion.ulLeft = pRect->left;
    lumaSettings.stRegion.ulTop = pRect->top;
    lumaSettings.stRegion.ulRight = pRect->right;
    lumaSettings.stRegion.ulBottom = pRect->bottom;

    rc = BVDC_Compositor_SetLumaStatsConfiguration(display->compositor, &lumaSettings);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureCtrl_GetDisplayLumaStatistics(
    NEXUS_DisplayHandle display,
    NEXUS_LumaStatistics *pLumaStat)
{
    BVDC_LumaStatus lumaStatus;
    BERR_Code rc = BERR_SUCCESS;

    BKNI_Memset(pLumaStat, 0, sizeof(*pLumaStat));
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    /* Get luma sum */
    rc = BVDC_Compositor_GetLumaStatus(display->compositor, &lumaStatus);
    if (rc) return BERR_TRACE(rc);

    /* Calculate APL from luma sum and region */
    pLumaStat->average = lumaStatus.ulSum / lumaStatus.ulPixelCnt;
    pLumaStat->min = lumaStatus.ulMin;
    pLumaStat->max = lumaStatus.ulMax;
    BKNI_Memcpy(pLumaStat->histogram, lumaStatus.aulHistogram, NEXUS_LUMA_HISTOGRAM_COUNT * 4);
    BKNI_Memcpy(pLumaStat->levelStats, lumaStatus.aulLevelStats, NEXUS_LUMA_HISTOGRAM_LEVELS * sizeof(uint32_t));
    return NEXUS_SUCCESS;
}


/***************************************************************************
Summary:
    Set luma average calculation region.
**************************************************************************/
NEXUS_Error
NEXUS_PictureCtrl_ConfigWindowLumaStatistics(
    NEXUS_VideoWindowHandle window,
    const NEXUS_ClipRect *pRect)
{
    BVDC_Window_Handle  windowVDC;
    BVDC_LumaSettings lumaSettings;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    windowVDC = window->vdcState.window;
    if (NULL == windowVDC)
    {
        BDBG_ERR(("Window not connnected\n"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    rc = BVDC_Window_GetLumaStatsConfiguration(windowVDC, &lumaSettings);
    if (rc) return BERR_TRACE(rc);

    lumaSettings.stRegion.ulLeft = pRect->left;
    lumaSettings.stRegion.ulTop = pRect->top;
    lumaSettings.stRegion.ulRight = pRect->right;
    lumaSettings.stRegion.ulBottom = pRect->bottom;

    rc = BVDC_Window_SetLumaStatsConfiguration(windowVDC, &lumaSettings);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Get luma average value of main video surface.
**************************************************************************/
NEXUS_Error
NEXUS_PictureCtrl_GetWindowLumaStatistics(
    NEXUS_VideoWindowHandle window,
    NEXUS_LumaStatistics *pLumaStat)
{
    BVDC_Window_Handle  windowVDC;
    BVDC_LumaStatus lumaStatus;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    BKNI_Memset(pLumaStat, 0, sizeof(*pLumaStat));
    windowVDC = window->vdcState.window;
    if (NULL == windowVDC)
    {
        BDBG_ERR(("VideoWindow must have VideoInput connected to get luma statistics\n"));
        return NEXUS_UNKNOWN;
    }
    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    /* Get luma sum */
    rc = BVDC_Window_GetLumaStatus(windowVDC, &lumaStatus);
    if (rc) return BERR_TRACE(rc);

    /* Calculate APL from luma sum and region */
    pLumaStat->average = lumaStatus.ulSum / lumaStatus.ulPixelCnt;
    pLumaStat->min = lumaStatus.ulMin;
    pLumaStat->max = lumaStatus.ulMax;
    BKNI_Memcpy(pLumaStat->histogram, lumaStatus.aulHistogram, NEXUS_LUMA_HISTOGRAM_COUNT * sizeof(uint32_t));
    BKNI_Memcpy(pLumaStat->levelStats, lumaStatus.aulLevelStats, NEXUS_LUMA_HISTOGRAM_LEVELS * sizeof(uint32_t));

    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureCtrl_SetColorCorrectionTable( NEXUS_DisplayHandle display, const NEXUS_PictureControlColorCorrectionSettings *pSettings )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    if (pSettings==NULL || !pSettings->enabled)
    {
        rc = BVDC_Display_EnableColorCorrection(display->displayVdc, false);
        if (rc) return BERR_TRACE(rc);
    }
    else
    {
        rc = BVDC_Display_EnableColorCorrection(display->displayVdc, true);
        if (rc) return BERR_TRACE(rc);

        if((pSettings->gammaId < NEXUS_CCB_NUM_GAMMA_VALUE) && (pSettings->colorId < NEXUS_CCB_NUM_COLOR_TEMP))
        {
            rc = BVDC_Display_SetColorCorrectionTable(display->displayVdc, pSettings->gammaId, pSettings->colorId);
            if (rc) return BERR_TRACE(rc);
        }
        else
        {
            BDBG_ERR(("Invalid CCB table IDs"));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_PictureCtrl_SetCustomColorCorrectionTable( NEXUS_DisplayHandle display, const uint32_t *pTable, unsigned numTableEntries )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(pTable);
    BSTD_UNUSED(numTableEntries);

    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    rc = BVDC_Display_EnableColorCorrection(display->displayVdc, true);
    if (rc) return BERR_TRACE(rc);

    rc = BVDC_Display_LoadColorCorrectionTable(display->displayVdc, pTable);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

void NEXUS_PictureCtrl_GetColorClipSettings( NEXUS_DisplayHandle display, NEXUS_PictureCtrlColorClipSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pSettings = display->colorClipSettings;
}

NEXUS_Error NEXUS_PictureCtrl_SetColorClipSettings( NEXUS_DisplayHandle display, const NEXUS_PictureCtrlColorClipSettings *pSettings )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_CASSERT(sizeof(BVDC_ColorClipSettings) == sizeof(NEXUS_PictureCtrlColorClipSettings));
    BDBG_CASSERT(BVDC_ColorClipMode_Both == NEXUS_ColorClipMode_eMax-1);

    if (g_NEXUS_DisplayModule_State.pqDisabled) {
        return 0;
    }

    rc = BVDC_Compositor_SetColorClip(display->compositor, (BVDC_ColorClipSettings*)pSettings);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    display->colorClipSettings = *pSettings;

    return NEXUS_SUCCESS;
}
