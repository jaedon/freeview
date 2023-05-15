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
 * $brcm_Workfile: nexus_rfm.c $
 * $brcm_Revision: 15 $
 * $brcm_Date: 6/19/12 1:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/rfm/7400/src/nexus_rfm.c $
 * 
 * 15   6/19/12 1:27p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 14   11/22/11 5:48p jtna
 * SW7425-1708: add s3 power management for rfm
 * 
 * 13   9/17/10 5:52p jtna
 * SW7420-972: move RFM PM from nexus to magnum
 * 
 * 12   8/26/10 2:59p jtna
 * SW7420-972: added RFM power management
 * 
 * 11   11/3/09 2:33p jgarrett
 * SW7325-646: Fixing scaling of initial volume
 * 
 * 10   10/30/09 2:31p jgarrett
 * SW7325-646: Adding volume setting
 * 
 * 9   9/9/09 1:16p erickson
 * SW7405-2977: convert to NEXUS_AudioOutputPort
 *
 * 8   8/11/09 6:52p jtna
 * PR56435: fix rfm mute condition
 *
 * 7   7/1/09 4:37p jtna
 * PR56435: RFM dynamic power management support
 *
 * 6   1/26/09 1:55p erickson
 * PR51468: global variable naming convention
 *
 * 5   4/18/08 4:20p jgarrett
 * PR 41623: Setting output port to avoid confusion with SPDIF
 *
 * 4   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 3   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 2   3/4/08 4:31p jgarrett
 * PR 39405: Saving connection settings
 *
 * 1   2/20/08 1:42p erickson
 * PR39405: added RFM
 *
 **************************************************************************/
#include "nexus_rfm_module.h"
#include "priv/nexus_rfm_priv.h"
#include "priv/nexus_core.h"
#include "priv/nexus_core_video.h"
#include "priv/nexus_core_audio.h"

BDBG_MODULE(nexus_rfm);

NEXUS_ModuleHandle g_NEXUS_rfmModule;
static NEXUS_RfmHandle g_NEXUS_rfmHandle[NEXUS_NUM_RFM_OUTPUTS];

void NEXUS_RfmModule_GetDefaultSettings( NEXUS_RfmModuleSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_RfmModule_Init( const NEXUS_RfmModuleSettings *pSettings )
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_Error errCode;
    unsigned i;

    BDBG_ASSERT(NULL == g_NEXUS_rfmModule);
    BSTD_UNUSED(pSettings);

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    g_NEXUS_rfmModule = NEXUS_Module_Create("rfm", &moduleSettings);
    if ( NULL == g_NEXUS_rfmModule )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        return NULL;
    }

    for (i=0; i<NEXUS_NUM_RFM_OUTPUTS; i++) {
        g_NEXUS_rfmHandle[i] = NULL;
    }

    /* Success */
    return g_NEXUS_rfmModule;
}

void NEXUS_RfmModule_Uninit(void)
{
    BDBG_ASSERT(NULL != g_NEXUS_rfmModule);
    NEXUS_Module_Destroy(g_NEXUS_rfmModule);
    g_NEXUS_rfmModule = NULL;
}

/*********************************************/

struct NEXUS_Rfm {
    NEXUS_OBJECT(NEXUS_Rfm);
    NEXUS_RfmSettings settings;
    NEXUS_RfmConnectionSettings connectionSettings;
    NEXUS_VideoOutputObject videoOutput;
    NEXUS_AudioOutputObject audioOutput;
    unsigned index;
    bool volumeChanged;

    BRFM_Handle mRfm;
};

void NEXUS_Rfm_GetDefaultSettings( NEXUS_RfmSettings *pSettings )
{
    BRFM_Settings rfmSettings;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BRFM_GetDefaultSettings(&rfmSettings, g_pCoreHandles->chp);
    pSettings->volume = rfmSettings.volume*100;
    pSettings->channel = 3;
}

NEXUS_RfmHandle NEXUS_Rfm_Open( unsigned index, const NEXUS_RfmSettings *pSettings )
{
    NEXUS_RfmHandle rfm;
    NEXUS_RfmSettings defaultSettings;
    BRFM_Settings rfmSettings;
    BERR_Code rc;

    if (index >= NEXUS_NUM_RFM_OUTPUTS) {
        BDBG_ERR(("Invalid RFM index %d", index));
        return NULL;
    }

    if (!pSettings) {
        NEXUS_Rfm_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    rfm = BKNI_Malloc(sizeof(*rfm));
    if (!rfm) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Rfm, rfm);
    rfm->index = index;

    NEXUS_VIDEO_OUTPUT_INIT(&rfm->videoOutput, NEXUS_VideoOutputType_eRfm, rfm);
    NEXUS_AUDIO_OUTPUT_INIT(&rfm->audioOutput, NEXUS_AudioOutputType_eRfm, rfm);
    rfm->audioOutput.port = NEXUS_AudioOutputPort_eRfMod;

    rc = BRFM_GetDefaultSettings(&rfmSettings, g_pCoreHandles->chp);
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    rfmSettings.volume = pSettings->volume/100;

    rc = BRFM_Open(&rfm->mRfm, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &rfmSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    rc = NEXUS_Rfm_SetSettings(rfm, pSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    g_NEXUS_rfmHandle[index] = rfm;
    return rfm;

error:
    NEXUS_Rfm_Close(rfm);
    return NULL;
}

static void NEXUS_Rfm_P_Finalizer( NEXUS_RfmHandle rfm )
{
    NEXUS_OBJECT_ASSERT(NEXUS_Rfm, rfm);

    if (rfm->mRfm) {
        BRFM_Close(rfm->mRfm);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_Rfm, rfm);
    BKNI_Free(rfm);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Rfm, NEXUS_Rfm_Close);

void NEXUS_Rfm_GetSettings( NEXUS_RfmHandle rfm, NEXUS_RfmSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    *pSettings = rfm->settings;
}

NEXUS_Error NEXUS_Rfm_SetSettings( NEXUS_RfmHandle rfm, const NEXUS_RfmSettings *pSettings )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);

    /* save the settings first, then call the internal set function */
    if ( rfm->settings.volume != pSettings->volume )
    {
        rfm->volumeChanged = true;
    }
    rfm->settings = *pSettings;

    rc = NEXUS_Rfm_SetConnectionSettings_priv(rfm, &rfm->connectionSettings);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

NEXUS_VideoOutput NEXUS_Rfm_GetVideoConnector( NEXUS_RfmHandle rfm )
{
    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    return &rfm->videoOutput;
}

NEXUS_AudioOutput NEXUS_Rfm_GetAudioConnector( NEXUS_RfmHandle rfm )
{
    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    return &rfm->audioOutput;
}

void NEXUS_Rfm_GetIndex_priv( NEXUS_RfmHandle rfm, unsigned *pIndex )
{
    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    NEXUS_ASSERT_MODULE();
    *pIndex = rfm->index;
}

void NEXUS_Rfm_GetConnectionSettings_priv( NEXUS_RfmHandle rfm, NEXUS_RfmConnectionSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    NEXUS_ASSERT_MODULE();
    *pSettings = rfm->connectionSettings;
}

/* This function combines the user Settings and the internal ConnectionSettings and
applies them to RFM PI */
NEXUS_Error NEXUS_Rfm_SetConnectionSettings_priv( NEXUS_RfmHandle rfm, const NEXUS_RfmConnectionSettings *pSettings )
{
    BRFM_ModulationType modType;
    BRFM_OutputChannel chNbr;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rfm, NEXUS_Rfm);
    NEXUS_ASSERT_MODULE();

    /* We do not call BRFM_SetAudioVolume for 74xx.
    74xx chips change vol via the mixer thereby affecting dac volume and RFM volume downstream.
    7038 based systems, do not use mixer to change vol, hence it would need RFM vol control here. */

    if (pSettings->audioEnabled) {
        rc = BRFM_SetAudioMute(rfm->mRfm, rfm->settings.muted || !pSettings->audioEnabled);
        if (rc) return BERR_TRACE(rc);
        if ( rfm->volumeChanged )
        {
            rc = BRFM_SetAudioVolume(rfm->mRfm, rfm->settings.volume/100);
            if (rc) return BERR_TRACE(rc);
            rfm->volumeChanged = false;
        }
    }

    switch (rfm->settings.channel) {
    case 3: chNbr = BRFM_OutputChannel_eCh3; break;
    case 4: chNbr = BRFM_OutputChannel_eCh4; break;
    default: return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch (pSettings->videoFormat) {
    case NEXUS_VideoFormat_eSecam:
        modType = BRFM_ModulationType_eSecamBG;
        break;
    case NEXUS_VideoFormat_ePalG:
    case NEXUS_VideoFormat_ePalB:
    case NEXUS_VideoFormat_ePalB1:
        modType = BRFM_ModulationType_ePalBG;
        break;
    case NEXUS_VideoFormat_ePalH:
    case NEXUS_VideoFormat_ePalK: /* TODO: is this right? */
        modType = BRFM_ModulationType_ePalH;
        break;
    case NEXUS_VideoFormat_ePalI:
        modType = BRFM_ModulationType_ePalI;
        break;
    case NEXUS_VideoFormat_ePalM:
        modType = BRFM_ModulationType_ePalM;
        break;
    case NEXUS_VideoFormat_ePalN:
    case NEXUS_VideoFormat_ePalNc:
        modType = BRFM_ModulationType_ePalN;
        break;
    case NEXUS_VideoFormat_ePalD:
        modType = BRFM_ModulationType_ePalDChina;
        break;
    case NEXUS_VideoFormat_ePalD1:
        modType = BRFM_ModulationType_ePalD;
        break;
    default:
        modType = BRFM_ModulationType_eNtscOpenCable;
        break;
#if 0
/* unused RFM enums */
        modType = BRFM_ModulationType_eNtscCustom;
        modType = BRFM_ModulationType_ePalCustom;
        modType = BRFM_ModulationType_eSecamDK;
        modType = BRFM_ModulationType_eSecamK1;
#endif
    }

    if (pSettings->videoEnabled || pSettings->audioEnabled) {
        rc = BRFM_SetModulationType(rfm->mRfm, modType, chNbr);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        rc = BRFM_DisableRfOutput(rfm->mRfm);
        if (rc) return BERR_TRACE(rc);
        rc = BRFM_EnablePowerSaver(rfm->mRfm);
        if (rc) return BERR_TRACE(rc);
    }

    rfm->connectionSettings = *pSettings;

    return 0;
}

NEXUS_Error NEXUS_RfmModule_Standby_priv( bool enabled, const NEXUS_StandbySettings *pSettings )
{
#if NEXUS_POWER_MANAGEMENT
    BERR_Code rc;
    unsigned i;
    NEXUS_RfmHandle rfm = NULL;
    BSTD_UNUSED(pSettings);
    
    for (i=0; i<NEXUS_NUM_RFM_OUTPUTS; i++) {
        if ( (rfm = g_NEXUS_rfmHandle[i]) ) {
            if (enabled) { /* means that we have to power down in one way or another */
                if (pSettings->mode != NEXUS_StandbyMode_eDeepSleep) { /* not S3 */
                    rc = BRFM_Standby(rfm->mRfm);
                    if (rc) { return BERR_TRACE(NEXUS_UNKNOWN); }
                }
                else {
                    BRFM_Close(rfm->mRfm);
                    rfm->mRfm = NULL;
                }
            }
            else { /* resume, using the opposite call */
                if (rfm->mRfm) {
                    BRFM_Resume(rfm->mRfm);
                }
                else {
                    BRFM_Settings rfmSettings;
                    rc = BRFM_GetDefaultSettings(&rfmSettings, g_pCoreHandles->chp);
                    if (rc) { return BERR_TRACE(NEXUS_UNKNOWN); }

                    rc = BRFM_Open(&rfm->mRfm, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &rfmSettings);
                    if (rc) { return BERR_TRACE(NEXUS_UNKNOWN); }

                    rc = NEXUS_Rfm_SetSettings(rfm, &rfm->settings);
                    if (rc) { return BERR_TRACE(NEXUS_UNKNOWN); }
                }
            }            
        }
    }
    
    return NEXUS_SUCCESS;    
#else
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(enabled);
    return NEXUS_SUCCESS;
#endif
}

