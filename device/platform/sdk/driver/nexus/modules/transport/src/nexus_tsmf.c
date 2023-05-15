/***************************************************************************
 *     (c)2003-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_tsmf.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 11/14/12 12:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_tsmf.c $
 * 
 * 12   11/14/12 12:05p jtna
 * SW7425-3782: default to NEXUS_TsmfSourceType_eMtsif
 * 
 * 11   10/8/12 4:29p jtna
 * SW7425-3782: use NEXUS_FrontendConnector for TSMF
 * 
 * 10   10/8/12 11:38a jtna
 * SW7425-4044: Coverity Defect ID:21130 REVERSE_INULL
 * 
 * 9   10/3/12 5:02p jtna
 * SW7425-3782: refactor nexus tsmf impl
 * 
 * 8   9/5/12 6:10p jtna
 * SW7425-3782: rework host and demod TSMF impl. both cases now go through
 *  the host API
 * 
 * 7   6/19/12 1:27p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 5   11/2/11 11:57a jtna
 * SW7231-391: fix compiler warning
 * 
 * 4   10/31/11 7:49p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 3   10/14/11 2:53p jtna
 * SW7425-1189: added NEXUS_TsmfSettings.frontendTsmfIndex. allow change
 *  between auto and semi-auto mode without enable/disable.
 * 
 * 2   10/5/11 1:32p jtna
 * SW7425-1189: fix non-7425 platforms
 * 
 * 1   10/5/11 12:27p jtna
 * SW7425-1189: add transport and frontend TSMF support
 * 
 ***************************************************************************/
#include "nexus_transport_module.h"
#include "nexus_playpump_impl.h"
#include "priv/nexus_tsmf_priv.h"
#if NEXUS_HAS_TSMF
#include "bxpt_tsmf.h"
#endif

BDBG_MODULE(nexus_tsmf);

#if NEXUS_HAS_TSMF

struct NEXUS_Tsmf
{
    NEXUS_OBJECT(NEXUS_Tsmf);
    BLST_S_ENTRY(NEXUS_Tsmf) link;
    unsigned index;
    NEXUS_TsmfOpenSettings openSettings;
    NEXUS_TsmfSettings settings;
    bool pendingSetSettings; /* set on NEXUS_Tsmf_SetSettings(). cleared on NEXUS_TuneXyz() */
};

NEXUS_Tsmf_P_State g_NEXUS_Tsmf_P_State;

/* establishes TSMF -> PB mapping on host (i.e. set output of TSMF). 
   uses the currently-saved PB settings and TSMF settings */
NEXUS_Error NEXUS_Tsmf_SetOutput_priv(void *parserBandHandle)
{
    NEXUS_ParserBandHandle parserBand = parserBandHandle;
    NEXUS_Error rc;
    NEXUS_ASSERT_MODULE();

    BDBG_ASSERT(parserBand->settings.sourceType == NEXUS_ParserBandSourceType_eTsmf);

    if (parserBand->settings.sourceTypeSettings.tsmf==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    NEXUS_OBJECT_ASSERT(NEXUS_Tsmf, parserBand->settings.sourceTypeSettings.tsmf);

    if (parserBand->settings.sourceTypeSettings.tsmf->settings.sourceType == NEXUS_TsmfSourceType_eMtsif) {
        return NEXUS_SUCCESS; /* demod TSMF output is handled in frontend code */
    }
    else {
        unsigned parserIndex = parserBand->hwIndex;
        unsigned tsmfIndex = parserBand->settings.sourceTypeSettings.tsmf->index;
        unsigned enabled = parserBand->settings.sourceTypeSettings.tsmf->settings.enabled;

        BDBG_MSG(("TSMF%u -> PB%u (enable %u)", tsmfIndex, parserIndex, enabled));
        rc = BXPT_TSMF_SetParserConfig(pTransport->xpt, parserIndex, tsmfIndex, enabled);
        if (rc) return BERR_TRACE(rc);
    }
    return NEXUS_SUCCESS;
}

bool NEXUS_Tsmf_GetPending_priv(NEXUS_TsmfHandle tsmf)
{
    return tsmf->pendingSetSettings;
}

void NEXUS_Tsmf_SetPending_priv(NEXUS_TsmfHandle tsmf, bool pending)
{
    tsmf->pendingSetSettings = pending;
}

unsigned NEXUS_Tsmf_GetNumHandles_priv(void)
{
    NEXUS_TsmfHandle tsmf;
    unsigned count = 0;
    NEXUS_ASSERT_MODULE();

    for (tsmf=BLST_S_FIRST(&g_NEXUS_Tsmf_P_State.handles); tsmf; tsmf=BLST_S_NEXT(tsmf, link)) {
        count++;
    }
    return count;
}

NEXUS_TsmfHandle NEXUS_Tsmf_GetHandle_priv(unsigned num, unsigned *hwIndex)
{
    NEXUS_TsmfHandle tsmf;
    unsigned count = 0;
    NEXUS_ASSERT_MODULE();

    for (tsmf=BLST_S_FIRST(&g_NEXUS_Tsmf_P_State.handles); tsmf; tsmf=BLST_S_NEXT(tsmf, link)) {
        if (count==num) {
            *hwIndex = tsmf->index;
            return tsmf;
        }
        count++;
    }
    return NULL;
}

bool NEXUS_Tsmf_TestFrontendInput_priv(NEXUS_TsmfHandle tsmf, NEXUS_FrontendConnectorHandle frontend) 
{
    NEXUS_ASSERT_MODULE();

    if (tsmf->settings.sourceType==NEXUS_TsmfSourceType_eMtsif && tsmf->settings.sourceTypeSettings.mtsif==frontend) {
        return true;
    }
    return false;
}

void NEXUS_Tsmf_GetDefaultOpenSettings(NEXUS_TsmfOpenSettings *pOpenSettings)
{
    BKNI_Memset(pOpenSettings, 0, sizeof(*pOpenSettings));
}

NEXUS_TsmfHandle NEXUS_Tsmf_Open(unsigned index, const NEXUS_TsmfOpenSettings* pOpenSettings)
{
    NEXUS_Error rc;
    NEXUS_TsmfHandle handle, prev, curr;
    NEXUS_TsmfOpenSettings openSettings;
    if (pOpenSettings==NULL) {
        NEXUS_Tsmf_GetDefaultOpenSettings(&openSettings);
    }

    for (handle=BLST_S_FIRST(&g_NEXUS_Tsmf_P_State.handles); handle; handle=BLST_S_NEXT(handle, link)) {
        if (handle->index==index) {
            BDBG_ERR(("TSMF index %u already opened", index));
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            return NULL;
        }
    }
  
    handle = BKNI_Malloc(sizeof(*handle));
    if (!handle) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    BKNI_Memset(handle, 0, sizeof(*handle));
    NEXUS_OBJECT_INIT(NEXUS_Tsmf, handle);

    handle->index = index;
    handle->openSettings = pOpenSettings ? *pOpenSettings : openSettings;
    handle->settings.sourceType = NEXUS_TsmfSourceType_eMtsif;

#if 0
    BLST_S_INSERT_HEAD(&g_NEXUS_Tsmf_P_State.handles, handle, link);
#else
    for (prev=NULL, curr=BLST_S_FIRST(&g_NEXUS_Tsmf_P_State.handles); curr; curr=BLST_S_NEXT(curr, link)) {
        prev = curr;
    }
    if (prev) {
        BLST_S_INSERT_AFTER(&g_NEXUS_Tsmf_P_State.handles, prev, handle, link);
    }
    else {
        BLST_S_INSERT_HEAD(&g_NEXUS_Tsmf_P_State.handles, handle, link);
    }
#endif
    
    return handle;
}

static void NEXUS_Tsmf_P_Finalizer(NEXUS_TsmfHandle handle)
{
    BLST_S_REMOVE(&g_NEXUS_Tsmf_P_State.handles, handle, NEXUS_Tsmf, link);
    NEXUS_OBJECT_DESTROY(NEXUS_Tsmf, handle);
    BKNI_Free(handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Tsmf, NEXUS_Tsmf_Close);

void NEXUS_Tsmf_GetSettings(NEXUS_TsmfHandle handle, NEXUS_TsmfSettings *pSettings)
{
    NEXUS_Error rc;
    unsigned tsmfIndex = handle->index;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_Tsmf);
    BDBG_ASSERT(NULL != pSettings);

    if (handle->settings.sourceType==NEXUS_TsmfSourceType_eMtsif) {
        goto done;
    }
    rc = BXPT_TSMF_GetFldVerifyConfig(pTransport->xpt, tsmfIndex, (BXPT_TSMFFldVerifyConfig *)(&(handle->settings.fieldVerifyConfig)));
    if (rc) {rc = BERR_TRACE(rc);} /* keep going */

done:
    *pSettings = handle->settings;
}

NEXUS_Error NEXUS_Tsmf_SetSettings(NEXUS_TsmfHandle handle, const NEXUS_TsmfSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned tsmfIndex = handle->index;
    bool changed = false;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Tsmf);
    BDBG_ASSERT(pSettings);

    BDBG_CASSERT(sizeof(NEXUS_TsmfFieldVerifyConfig) == sizeof(BXPT_TSMFFldVerifyConfig));
    BDBG_CASSERT(NEXUS_TsmfVersionChangeMode_eFrameChangeVer == (NEXUS_TsmfVersionChangeMode)BXPT_TSMFVersionChgMode_eFrameChgVer);

    if (pSettings->sourceType==NEXUS_TsmfSourceType_eMtsif) {
        /* defer to frontend */
        handle->pendingSetSettings = true;
        goto done;
    }

    rc = BXPT_TSMF_SetFldVerifyConfig(pTransport->xpt, tsmfIndex, (const BXPT_TSMFFldVerifyConfig *)&(pSettings->fieldVerifyConfig));
    if (rc) {rc = BERR_TRACE(rc);} /* keep going */

    if ((pSettings->enabled!=handle->settings.enabled) || (pSettings->semiAutomaticMode!=handle->settings.semiAutomaticMode)) {
        changed = true;
       
        if (pSettings->enabled) {
            BXPT_TSMF_InputSel inputSelect = BXPT_TSMF_InputSel_eIB0;

            /* convert to XPT enum */
            if (pSettings->sourceType==NEXUS_TsmfSourceType_eInputBand) {
                inputSelect = BXPT_TSMF_InputSel_eIB0 + (pSettings->sourceTypeSettings.inputBand - NEXUS_InputBand_e0);
            }
            else if (pSettings->sourceType==NEXUS_TsmfSourceType_eRemux) {
                if (pSettings->sourceTypeSettings.remux->index > 0) {
                    return BERR_TRACE(NEXUS_NOT_SUPPORTED); /* if HW supports, must extend enum */
                }
                inputSelect = BXPT_TSMF_InputSel_eRMX;
            }

            /* specify the TSMF input (and other params). the TSMF output is set via NEXUS_ParserBand_SetSettings or NEXUS_Frontend_TuneXyz */
            BDBG_MSG(("IB%u -> TSMF%u", inputSelect, tsmfIndex));
            if (pSettings->semiAutomaticMode) {
                rc = BXPT_TSMF_EnableSemiAutoMode(pTransport->xpt, inputSelect, tsmfIndex, pSettings->slotMapLo, pSettings->slotMapHi, pSettings->relativeTsNum);
                if (rc) return BERR_TRACE(rc);
            }
            else {
                rc = BXPT_TSMF_EnableAutoMode(pTransport->xpt, inputSelect, tsmfIndex, pSettings->relativeTsNum);
                if (rc) return BERR_TRACE(rc);
            }
        }
        else {
            BDBG_MSG(("TSMF%u disabled", tsmfIndex));
            rc = BXPT_TSMF_DisableTsmf(pTransport->xpt, tsmfIndex);
            if (rc) return BERR_TRACE(rc);
        }
    }

done:
    handle->settings = *pSettings;

    /* if output already connected (TSMF -> PB), then reapply */
    if (changed && pSettings->sourceType!=NEXUS_TsmfSourceType_eMtsif) {
        unsigned i;
        for (i=0; i<NEXUS_NUM_PARSER_BANDS; i++) {
            NEXUS_ParserBandSettings *pbSettings = &(pTransport->parserBand[i].settings);
            if (pbSettings->sourceType == NEXUS_ParserBandSourceType_eTsmf && pbSettings->sourceTypeSettings.tsmf != NULL) 
            {
                NEXUS_TsmfHandle pbTsmf = pbSettings->sourceTypeSettings.tsmf;
                NEXUS_OBJECT_ASSERT(NEXUS_Tsmf, pbTsmf);
                if (pbTsmf == handle) {
                    NEXUS_Tsmf_SetOutput_priv(&pTransport->parserBand[i]);
                }
            }
        }
    }

    return rc;
}

#else /* NEXUS_HAS_TSMF */

void NEXUS_Tsmf_GetDefaultOpenSettings(NEXUS_TsmfOpenSettings *pOpenSettings)
{
    BSTD_UNUSED(pOpenSettings);
}

struct NEXUS_Tsmf
{
    NEXUS_OBJECT(NEXUS_Tsmf);
};

NEXUS_TsmfHandle NEXUS_Tsmf_Open(unsigned index, const NEXUS_TsmfOpenSettings* pOpenSettings)
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pOpenSettings);
    return NULL;
}

static void NEXUS_Tsmf_P_Finalizer(NEXUS_TsmfHandle handle)
{
    BSTD_UNUSED(handle);
}

void NEXUS_Tsmf_GetSettings(NEXUS_TsmfHandle handle, NEXUS_TsmfSettings *pSettings)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_Tsmf_SetSettings(NEXUS_TsmfHandle handle, const NEXUS_TsmfSettings *pSettings)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return 0;
}
NEXUS_OBJECT_CLASS_MAKE(NEXUS_Tsmf, NEXUS_Tsmf_Close);
#endif
