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
 * $brcm_Workfile: nexus_ir_blaster.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 8/24/12 4:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/ir_blaster/src/nexus_ir_blaster.c $
 * 
 * 16   8/24/12 4:51p jtna
 * SW7425-3731: Coverity Defect ID: 20797 RESOURCE_LEAK
 * 
 * 15   7/24/12 12:09p jtna
 * SW7425-3542: Coverity Defect ID:20777 STACK_USE
 * 
 * 14   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 13   4/20/11 6:13p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 12   7/30/10 10:56a erickson
 * SW7400-2845: add NEXUS_IrBlaster_SetSettings and
 *  NEXUS_IrBlaster_GetSettings
 *
 * 11   5/11/10 5:05p jtna
 * SW7125-307: Coverity - check for null pointer arguments before
 *  dereferencing them
 *
 * 10   1/26/09 12:05p erickson
 * PR51468: global variable naming convention
 *
 * 9   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 8   9/22/08 10:48a erickson
 * PR47064: add XMP support
 *
 * 7   8/18/08 10:28a katrep
 * PR45674: Compiler warnings in DEBUG=n builds
 *
 * 6   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 5   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 4   3/8/08 7:25a erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 3   2/28/08 12:00p erickson
 * PR39981: can't copy custom settings. must map.
 *
 * 2   2/27/08 12:14a erickson
 * PR39981: initial impl
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   11/30/07 11:49a jgarrett
 * PR 37423: Removing warnings
 *
 * Nexus_Devel/2   11/21/07 11:12a erickson
 * PR37423: update
 *
 * Nexus_Devel/1   11/20/07 5:55p erickson
 * PR37423: added module for ir_blaster, keypad and uhf_input
 *
 **************************************************************************/
#include "nexus_ir_blaster_module.h"
#include "priv/nexus_core.h"
#include "birb.h"

BDBG_MODULE(nexus_ir_blaster);

NEXUS_ModuleHandle g_NEXUS_irBlasterModule;
struct {
    NEXUS_IrBlasterModuleSettings settings;
} g_NEXUS_irBlaster;

/****************************************
* Module functions
***************/

void NEXUS_IrBlasterModule_GetDefaultSettings(NEXUS_IrBlasterModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_IrBlasterModule_Init(const NEXUS_IrBlasterModuleSettings *pSettings)
{
    BDBG_ASSERT(!g_NEXUS_irBlasterModule);
    g_NEXUS_irBlasterModule = NEXUS_Module_Create("ir_blaster", NULL);
    if (pSettings) {
        g_NEXUS_irBlaster.settings = *pSettings;
    }
    else {
        NEXUS_IrBlasterModule_GetDefaultSettings(&g_NEXUS_irBlaster.settings);
    }
    return g_NEXUS_irBlasterModule;
}

void NEXUS_IrBlasterModule_Uninit()
{
    NEXUS_Module_Destroy(g_NEXUS_irBlasterModule);
    g_NEXUS_irBlasterModule = NULL;
}

/****************************************
* API functions
***************/

struct NEXUS_IrBlaster {
    NEXUS_OBJECT(NEXUS_IrBlaster);
    BIRB_Handle irbHandle;
    NEXUS_IrBlasterSettings settings;
    NEXUS_EventCallbackHandle eventHandler;
    NEXUS_TaskCallbackHandle transmitCompleteCallback;
};

void NEXUS_IrBlaster_GetDefaultSettings(NEXUS_IrBlasterSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

static void NEXUS_IrBlaster_P_Event(void *context)
{
    NEXUS_IrBlasterHandle irBlaster = context;
    NEXUS_TaskCallback_Fire(irBlaster->transmitCompleteCallback);
}

NEXUS_IrBlasterHandle NEXUS_IrBlaster_Open(unsigned index, const NEXUS_IrBlasterSettings *pSettings)
{
    NEXUS_IrBlasterHandle irBlaster;
    BIRB_Settings irbSettings;
    BERR_Code rc;
    NEXUS_IrBlasterSettings defaultSettings;
    BKNI_EventHandle event;

    if (index > 0) {
        rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    if (!pSettings) {
        NEXUS_IrBlaster_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    irBlaster = BKNI_Malloc(sizeof(*irBlaster));
    if (!irBlaster) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_IrBlaster, irBlaster);
    irBlaster->transmitCompleteCallback = NEXUS_TaskCallback_Create(irBlaster, NULL);

    BIRB_GetDefaultSettings(&irbSettings, g_pCoreHandles->chp);

    rc = BIRB_Open(&irBlaster->irbHandle, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &irbSettings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    rc = BIRB_GetEventHandle(irBlaster->irbHandle, &event);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    irBlaster->eventHandler = NEXUS_RegisterEvent(event, NEXUS_IrBlaster_P_Event, irBlaster);

    NEXUS_IrBlaster_SetSettings(irBlaster, pSettings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    return irBlaster;
error:
    NEXUS_IrBlaster_Close(irBlaster);
    return NULL;
}

static void NEXUS_IrBlaster_P_Finalizer(NEXUS_IrBlasterHandle irBlaster)
{
    NEXUS_OBJECT_ASSERT(NEXUS_IrBlaster, irBlaster);
    if (irBlaster->eventHandler) {
        NEXUS_UnregisterEvent(irBlaster->eventHandler);
    }
    if (irBlaster->irbHandle) {
        BIRB_Close(irBlaster->irbHandle);
    }

    NEXUS_TaskCallback_Destroy(irBlaster->transmitCompleteCallback);
    NEXUS_OBJECT_DESTROY(NEXUS_IrBlaster, irBlaster);
    BKNI_Free(irBlaster);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_IrBlaster, NEXUS_IrBlaster_Close);

void NEXUS_IrBlaster_GetSettings(NEXUS_IrBlasterHandle  irBlaster, NEXUS_IrBlasterSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    *pSettings = irBlaster->settings;
}

NEXUS_Error NEXUS_IrBlaster_SetSettings(NEXUS_IrBlasterHandle  irBlaster, const NEXUS_IrBlasterSettings *pSettings)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);

    NEXUS_TaskCallback_Set(irBlaster->transmitCompleteCallback, &pSettings->transmitComplete);

    BDBG_CASSERT(NEXUS_IrBlasterMode_eMax == (NEXUS_IrBlasterMode)BIRB_Device_eTest);
    rc = BIRB_Config(irBlaster->irbHandle, (BIRB_Device)pSettings->mode);
    if (rc) return BERR_TRACE(rc);

    irBlaster->settings = *pSettings;

    return 0;
}

NEXUS_Error NEXUS_IrBlaster_GetStatus(NEXUS_IrBlasterHandle irBlaster, NEXUS_IrBlasterStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    pStatus->busy = !BIRB_IsIrbFinished(irBlaster->irbHandle);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_Send(NEXUS_IrBlasterHandle irBlaster, const uint32_t *pData, uint8_t numBits, bool headerPulse)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    if (!pData) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    rc = BIRB_SendWithHeaderOption(irBlaster->irbHandle, (uint32_t*)pData, numBits, headerPulse);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_SendABBB(NEXUS_IrBlasterHandle irBlaster, const uint32_t *pDataA, uint8_t numBitsA, const uint32_t *pDataB,
    uint8_t numBitsB, bool headerPulseA, bool headerPulseB, bool fixedFrameLength)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    if (!pDataA) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    if (numBitsB && !pDataB) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    rc = BIRB_SendABBB(irBlaster->irbHandle, (uint32_t*)pDataA, numBitsA, (uint32_t*)pDataB, numBitsB,
        headerPulseA, headerPulseB, fixedFrameLength);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_SendAAAA(NEXUS_IrBlasterHandle irBlaster, const uint32_t *pDataA, uint8_t  numBitsA, bool headerPulse, bool fixedFrameLength)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    if (!pDataA) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    rc = BIRB_SendAAAA(irBlaster->irbHandle, (uint32_t*)pDataA, numBitsA, headerPulse, fixedFrameLength);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_Abort(NEXUS_IrBlasterHandle irBlaster)
{
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    BIRB_Reset(irBlaster->irbHandle);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_Resend(NEXUS_IrBlasterHandle irBlaster)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);
    rc = BIRB_Blast(irBlaster->irbHandle);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void NEXUS_IrBlaster_GetDefaultCustomSettings(NEXUS_IrBlasterCustomSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_IrBlaster_SetCustomSettings(NEXUS_IrBlasterHandle irBlaster, const NEXUS_IrBlasterCustomSettings *pSettings)
{
    SIrbConfiguration *config = BKNI_Malloc(sizeof(SIrbConfiguration));
    if (!config) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }

    BDBG_OBJECT_ASSERT(irBlaster, NEXUS_IrBlaster);

    BKNI_Memset(config, 0, sizeof(*config));

    config->name = pSettings->reserved;
    config->masterClockDivisor = pSettings->masterClockDivisor;
    config->carrierClockDivisor = pSettings->carrierClockDivisor;
    config->indexClockDivisor = pSettings->indexClockDivisor;
    config->noCarrier = pSettings->noCarrier;
    config->carrierHighCount = pSettings->carrierHighCount;
    config->carrierLowCount = pSettings->carrierLowCount;
    config->numberSequences = pSettings->numberSequences;
    BKNI_Memcpy(&config->logic0IndexPair, &pSettings->logic0IndexPair, sizeof(pSettings->logic0IndexPair));
    BKNI_Memcpy(&config->logic1IndexPair, &pSettings->logic1IndexPair, sizeof(pSettings->logic1IndexPair));
    BKNI_Memcpy(config->sequenceIndex, pSettings->sequenceIndex, sizeof(pSettings->sequenceIndex));
    BKNI_Memcpy(config->duration, pSettings->duration, sizeof(pSettings->duration));
    config->framePeriod = pSettings->framePeriod;
    config->framePeriodB = pSettings->framePeriodB;
    config->lastSequenceOffIndex = pSettings->lastSequenceOffIndex;
    config->repeatCount = pSettings->repeatCount;
    config->repeatStartIndex = pSettings->repeatStartIndex;
    /* Latest IRB PI has altModCnt, but not all chips have latest IRB. */

    BIRB_ConfigCustom(irBlaster->irbHandle, config);
    BKNI_Free(config);

    return 0;
}

NEXUS_Error NEXUS_IrBlaster_SendXmp2Ack( NEXUS_IrBlasterHandle irBlaster )
{
    BERR_Code rc;
    rc = BIRB_SendXmp2Ack(irBlaster->irbHandle);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_SendXmp2Nack( NEXUS_IrBlasterHandle irBlaster )
{
    BERR_Code rc;
    rc = BIRB_SendXmp2Nack(irBlaster->irbHandle);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_IrBlaster_SendXmp2Bytes( NEXUS_IrBlasterHandle irBlaster, const uint8_t *pData, unsigned numBytes )
{
    BERR_Code rc;
    rc = BIRB_SendXmp2Bytes(irBlaster->irbHandle, (uint8_t *)pData, numBytes);
    if (rc) return BERR_TRACE(rc);
    return 0;
}
