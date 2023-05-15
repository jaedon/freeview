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
 * $brcm_Workfile: nexus_smartcard.c $
 * $brcm_Revision: 52 $
 * $brcm_Date: 8/20/12 7:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/smartcard/src/nexus_smartcard.c $
 * 
 * 52   8/20/12 7:00p vishk
 * SW7425-3772: Coverity fix
 * 
 * 51   8/7/12 4:07p vishk
 * SW7425-3507: Bug fix
 * 
 * 50   8/7/12 4:05p vishk
 * SW7425-3507: Wrong state after a smartcard reset
 * 
 * 49   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 48   6/4/12 4:11p vishk
 * SW7425-2978: Convert conditional compiler settings to runtime
 *  configurable setting in SCD PI
 * 
 * SW7425-3109/1   6/1/12 5:38p vishk
 * SW7425-2978, SW7425-3109: Add support for Nordig and for on board
 *  direct power supply.
 * 
 * 47   5/24/12 12:04p vishk
 * SW7358-217, SW7425-3060: Remove insertCard and removeCard and add
 *  cardCallback.
 * 
 * 46   1/24/12 2:35p erickson
 * SW7405-5594: remove NEXUS_SmartcardStatus.err which was unused
 * 
 * 45   12/28/11 12:50p vishk
 * SW7346-620: NEXUS Smartcard module not powering doen correctly.
 * 
 * 44   12/1/11 4:43p jtna
 * SW7425-1708: add comment about app having to call
 *  NEXUS_Smartcard_SetSettings() after resume
 * 
 * 43   11/7/11 12:25p vishk
 * SW7405-5152: Nexus smartcard module error when ATR bytes actually read
 *  is less than what is expected
 * 
 * 42   8/3/11 11:09a gmohile
 * SW7125-1014 : Add module to active priority scheduler
 * 
 * 41   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * SW7125-1014/1   7/20/11 4:37p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 40   5/26/11 3:53p vishk
 * SW7346-228: Add support for the new attribute BSCD_ResetWaitCycles in
 *  BSCD_ChannelSettings to NEXUS
 * 
 * 39   5/26/11 3:52p vishk
 * SW7346-228: Add support for the new attribute BSCD_ResetWaitCycles in
 *  BSCD_ChannelSettings to NEXUS
 * 
 * 38   4/28/11 8:15a vishk
 * SW7405-5152: Nexus smartcard module error when ATR bytes actually read
 *  is less than what is expected Bug fix pertaining to the logic when
 *  bytes expected is equall to bytes read.
 * 
 * 37   4/21/11 5:01p VISHK
 * SW7405-5152: Nexus smartcard module error when ATR bytes actually read
 *  is less than what is expected.
 * 
 * 36   4/21/11 3:50p VISHK
 * SW7405-5152: Nexus smartcard module error when ATR bytes actually read
 *  is less than what is expected.
 * 
 * 35   4/20/11 6:15p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 34   4/13/11 11:55a VISHK
 * SW7346-138: NEXUS Smartcard settings need to be able to set the
 *  BSCD_ChannelSettings.ATRRecvTimeInteger parameter Fix compile errors.
 * 
 * 33   4/13/11 10:21a VISHK
 * SW7346-138: NEXUS Smartcard settings need to be able to set the
 *  BSCD_ChannelSettings.ATRRecvTimeInteger parameter Set the default
 *  parameters.
 * 
 * 32   4/11/11 4:54p vishk
 * SW7346-138: NEXUS Smartcard settings need to be able to set the
 *  BSCD_ChannelSettings.ATRRecvTimeInteger parameter
 * 
 * 31   4/8/11 12:39p erickson
 * SW7405-5210: add NEXUS_SmartcardStatus.iccClockFrequency
 *
 * 30   3/3/11 12:38p VISHK
 * SW7408-210 : Add standby support
 *
 * 29   3/1/11 5:38p gmohile
 * SW7408-210 : Add standby support
 *
 * 28   2/25/11 2:39p vishk
 * SW7405-5152: Nexus smartcard module error when ATR bytes actually read
 *  is less than what is expected.
 *
 * 27   2/7/11 4:28p VISHK
 * SW7346-25: Allow CLK Freq configuration in Nexus.  Initialize and set
 *  source clock frequency per smartcard channel. The frequency can be
 *  changed dynamically hence its also applied in
 *  NEXUS_Smartcard_SetSettings().
 *
 * 26   2/7/11 9:52a erickson
 * SW7400-2841: fix vcc default, fix bounds check in NEXUS_Smartcard_Open,
 *  add memset to GetStatus to future-proof
 *
 * 25   12/8/10 7:40a VISHK
 * SW7342-321: Allow the function NEXUS_Smartcard_ResetCard to be called
 *  without any data having to be read.
 *
 * 24   8/24/10 3:41p VISHK
 * SW7125-598:  Need to add the IFSI param interface in
 *  NEXUS_Smartcard_SetSettings
 *
 * 23   8/16/10 4:41p VISHK
 * SW7405-4023: SMARTCARD: Support card deactivation
 *
 * 22   8/10/10 6:22p VISHK
 * SW7335-811: NEXUS_Smartcard_PowerIcc does not account for TDA8024
 *  presence.  To Toggle power up and down as the coupling chip TDA8024 is
 *  active LOW.
 *
 * 21   8/3/10 10:51a VISHK
 * SW7125-478: NEXUS_Smartcard_SetParamsFromAtr() is not completed.
 *  Currently the ATR(answer to reset) is passed through to the
 *  application. Its the resposibility of the application to decode the
 *  atr.  But, once decoded, the application can use
 *  NEXUS_Smartcard_SetSettings() to set the desired parameters. So,
 *  deleting the api NEXUS_Smartcard_SetParamsFromAtr() as it is
 *  redundant. Please refer to the smartcard.c and smartcard_t1.c in
 *  nexus/examples/io for further details as to how to decode the ATR.
 *
 * 20   5/29/10 11:07a VISHK
 * SW7325-753: Smartcard: Irdeto Smartcard settings is not set correctly
 *
 * 19   2/16/10 10:33a VISHK
 * SW7400-2689: NEXUS smartcard module needs to use the
 *  NEXUS_SmartcardSettings to open the smartcard channel.
 *
 * 18   2/16/10 10:32a VISHK
 * SW7400-2689: NEXUS smartcard module needs to use the
 *  NEXUS_SmartcardSettings to open the smartcard channel.
 *
 * 17   2/11/10 12:35p VISHK
 * SW7400-2689: NEXUS smartcard module needs to use the
 *  NEXUS_SmartcardSettings to open the smartcard channel.
 *
 * 16   2/8/10 4:32p VISHK
 * SW7400-2610: The NEXU Smartcard API needs to add support for two new
 *  smartcard parameters setPinmuxCb and setVccCb;
 *
 * 15   1/26/10 1:59p VISHK
 * SW7400-2551: The NEXUS Smartcard module needs to add support for power
 *  up/down of the smartcard inteface.
 *
 * 14   1/26/10 11:17a VISHK
 * SW7400-2610: The NEXU Smartcard API needs to add support for two new
 *  smartcard parameters setPinmuxCb and setVccCb;
 *
 * 13   10/28/09 4:09p vishk
 * SW7400-2551: The NEXUS Smartcard module needs to add support for power
 *  up/down of the smartcard inteface.
 *
 * 12   9/30/09 3:47p vishk
 * SW7400-2551: The NEXUS Smartcard module needs to add support for power
 *  up/down of the smartcard inteface.
 *
 * 11   7/20/09 5:55p vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 10   5/22/09 5:08p vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 9   5/1/09 5:33p vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 9   5/1/09 5:27p vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 8   4/21/09 11:42a vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 7   4/20/09 3:18p vishk
 * PR 54381: Nexus smartcard module setting not configurable
 *
 * 6   1/26/09 12:06p erickson
 * PR51468: global variable naming convention
 *
 * 5   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 4   9/3/08 10:06a vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 *  configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 3   8/19/08 3:18p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 *  configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 2   8/19/08 1:59p vishk
 * PR 45615: ISO smartcard channel settings not correct and not
 *  configurable.
 * PR 29851: Smartcard functions need to allow more parameter changes
 *
 * 1   5/19/08 12:05p vishk
 * PR 40034: Develop Nexus Smartcard Interface/Module.
 *
 **************************************************************************/
#include "nexus_smartcard_module.h"
#include "bscd.h"
#include "bscd_datatypes.h"
#include "priv/nexus_core.h"
#include "nexus_platform_features.h"

BDBG_MODULE(nexus_smartcard);

NEXUS_ModuleHandle g_NEXUS_smartcardModule;

struct {
    NEXUS_SmartcardModuleSettings settings;
    BSCD_Handle scd;
} g_NEXUS_smartcard;

/****************************************
* Module functions
***************/

void NEXUS_SmartcardModule_GetDefaultSettings(NEXUS_SmartcardModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_SmartcardModule_Init(const NEXUS_SmartcardModuleSettings *pSettings)
{
    BSCD_Settings scdSettings;
    NEXUS_Error errCode;
    NEXUS_ModuleSettings moduleSettings;

    BDBG_ASSERT(!g_NEXUS_smartcardModule);
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLowActiveStandby;
    g_NEXUS_smartcardModule = NEXUS_Module_Create("smartcard", &moduleSettings);
    if(!g_NEXUS_smartcardModule) {
    return NULL;
    }

    if (pSettings)
    {
        g_NEXUS_smartcard.settings = *pSettings;
    }
    else
    {
        NEXUS_SmartcardModule_GetDefaultSettings(&g_NEXUS_smartcard.settings);
    }

    BSCD_GetDefaultSettings(&scdSettings, g_pCoreHandles->chp);
    scdSettings.moduleClkFreq.FreqSrc = (BSCD_ClockFreqSrc) g_NEXUS_smartcard.settings.clockSource;
    scdSettings.moduleClkFreq.ulClkFreq = g_NEXUS_smartcard.settings.clockFrequency;
    scdSettings.moduleClkFreq.bIsUsingOsc = g_NEXUS_smartcard.settings.externalOscillatior;
    scdSettings.moduleClkFreq.bIsRoutedInternal = g_NEXUS_smartcard.settings.routedInternal;
    errCode = BSCD_Open(&g_NEXUS_smartcard.scd, g_pCoreHandles->reg, g_pCoreHandles->chp, g_pCoreHandles->bint, &scdSettings);
    if (errCode)
    {
        NEXUS_Module_Destroy(g_NEXUS_smartcardModule);
        g_NEXUS_smartcardModule = NULL;

        BERR_TRACE(NEXUS_UNKNOWN);
        return NULL;
    }

    return g_NEXUS_smartcardModule;
}

void NEXUS_SmartcardModule_Uninit()
{
    BSCD_Close(g_NEXUS_smartcard.scd);
    NEXUS_Module_Destroy(g_NEXUS_smartcardModule);
    g_NEXUS_smartcardModule = NULL;
}

/****************************************
* API functions
***************/

typedef struct NEXUS_Smartcard {
    NEXUS_OBJECT(NEXUS_Smartcard);
    bool opened;
    BSCD_ChannelHandle channelHandle; /* scd channel */
    BSCD_ChannelSettings channelSettings;
    NEXUS_SmartcardSettings settings;
    NEXUS_SmartcardState state;
    NEXUS_IsrCallbackHandle cardAppCallback;
} NEXUS_Smartcard;

static NEXUS_Smartcard g_NEXUS_smartcards[BSCD_MAX_SUPPOTED_CHANNELS];

void NEXUS_Smartcard_GetDefaultSettings(NEXUS_SmartcardSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->standard = NEXUS_SmartcardStandard_eNds;
    pSettings->protocol = NEXUS_SmartcardProtocol_eT0;
    pSettings->fFactor = 1;
    pSettings->dFactor = 1;
    pSettings->extClockDivisor = 1;
    pSettings->txRetries = 4;
    pSettings->rxRetries = 4;
    pSettings->workWaitTime.value = 9600;
    pSettings->blockWaitTime.value = 971;
    pSettings->extraGuardTime.value= 2;
    pSettings->blockGuardTime.value = 22;
    pSettings->timeOut.value= 5000;
    pSettings->timeOut.unit= NEXUS_TimerUnit_eMilliSec;
    pSettings->scPresDbInfo.dbWidth = 7;
    pSettings->scPresDbInfo.scPresMode = NEXUS_ScPresMode_eMask;
    pSettings->isPresHigh = true;
    pSettings->setPinmux = true;
    pSettings->setVcc = true;
    pSettings->vcc = NEXUS_SmartcardVcc_e5V;
    pSettings->sourceClockFreq = 27000000;
    pSettings->atrReceiveTime.value = 40000;
    pSettings->atrReceiveTime.unit = NEXUS_TimerUnit_eClk;
    pSettings->resetCycles = 42000;
}

void NEXUS_Smartcard_GetSettings(NEXUS_SmartcardHandle smartcard, NEXUS_SmartcardSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);
    *pSettings = smartcard->settings;
}

NEXUS_Error NEXUS_Smartcard_SetSettings(NEXUS_SmartcardHandle smartcard, const NEXUS_SmartcardSettings *pSettings)
{
    NEXUS_Error errCode;
    BSCD_ChannelSettings *pChSettings;

    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    if ( pSettings == NULL )
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    pChSettings = &smartcard->channelSettings;

    errCode = BSCD_Channel_GetParameters(smartcard->channelHandle, pChSettings);
    if ( errCode ) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if (pSettings->standard == NEXUS_SmartcardStandard_eNds || pSettings->standard == NEXUS_SmartcardStandard_eIso || 
		pSettings->standard == NEXUS_SmartcardStandard_eIrdeto  || pSettings->standard == NEXUS_SmartcardStandard_eNordig) {
        BDBG_CASSERT(NEXUS_SmartcardStandard_eNordig == (NEXUS_SmartcardStandard)BSCD_Standard_eNordig);
        pChSettings->scStandard = pSettings->standard;
    } else {
        BDBG_ERR(("The requested standard is not supported at this time"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* This is hard-coding due to PI requirements.
            This way the PI picks up the right values based on the F and D factors. */

    pChSettings->ucEtuClkDiv = 0;
    pChSettings->ucScClkDiv = 0;
    pChSettings->unPrescale = 0;
    pChSettings->ucBaudDiv = 0;


    BDBG_CASSERT(NEXUS_SmartcardProtocol_eT14 == (NEXUS_SmartcardProtocol)BSCD_AsyncProtocolType_e14_IRDETO);
    pChSettings->eProtocolType = pSettings->protocol;

    pChSettings->ucFFactor = pSettings->fFactor;
    pChSettings->ucDFactor = pSettings->dFactor;
    pChSettings->ucExternalClockDivisor = pSettings->extClockDivisor;
    pChSettings->ucTxRetries = pSettings->txRetries;
    pChSettings->ucRxRetries = pSettings->rxRetries;
    pChSettings->ucBaudDiv = pSettings->baudDiv;
    pChSettings->unCurrentIFSD = pSettings->currentIFSD;

    BDBG_CASSERT(NEXUS_TimerUnit_eMilliSec == (NEXUS_TimerUnit)BSCD_TimerUnit_eMilliSec);
    pChSettings->workWaitTime.ulValue = pSettings->workWaitTime.value;
    pChSettings->workWaitTime.unit = pSettings->workWaitTime.unit;
    pChSettings->blockWaitTime.ulValue = pSettings->blockWaitTime.value;
    pChSettings->blockWaitTime.unit = pSettings->blockWaitTime.unit;
    pChSettings->extraGuardTime.ulValue = pSettings->extraGuardTime.value;
    pChSettings->extraGuardTime.unit = pSettings->extraGuardTime.unit;
    pChSettings->blockGuardTime.ulValue = pSettings->blockGuardTime.value;
    pChSettings->blockGuardTime.unit = pSettings->blockGuardTime.unit;
    pChSettings->ulCharacterWaitTimeInteger = pSettings->characterWaitTime;

    BDBG_CASSERT(NEXUS_EdcEncode_eCrc == (NEXUS_EdcEncode)BSCD_EDCEncode_eCRC);
    pChSettings->edcSetting.edcEncode = pSettings->edcSetting.edcEncode;
    pChSettings->edcSetting.bIsEnabled = pSettings->edcSetting.isEnabled;
    pChSettings->timeOut.ulValue = pSettings->timeOut.value;
    pChSettings->timeOut.unit = pSettings->timeOut.unit;
    pChSettings->bAutoDeactiveReq = pSettings->autoDeactiveReq;
    pChSettings->bNullFilter = pSettings->nullFilter;

    BDBG_CASSERT(NEXUS_ScPresMode_eMask == (NEXUS_ScPresMode)BSCD_ScPresMode_eMask);
    pChSettings->scPresDbInfo.scPresMode = pSettings->scPresDbInfo.scPresMode;
    pChSettings->scPresDbInfo.bIsEnabled = pSettings->scPresDbInfo.isEnabled;
    pChSettings->scPresDbInfo.ucDbWidth = pSettings->scPresDbInfo.dbWidth;

    BDBG_CASSERT(NEXUS_ResetCardAction_eReceiveAndDecode == (NEXUS_ResetCardAction)BSCD_ResetCardAction_eReceiveAndDecode);
    pChSettings->resetCardAction = pSettings->resetCardAction;
    pChSettings->blockWaitTimeExt.ulValue = pSettings->blockWaitTimeExt.value;
    pChSettings->blockWaitTimeExt.unit = pSettings->blockWaitTimeExt.unit;
    pChSettings->bIsPresHigh = pSettings->isPresHigh;
    pChSettings->setPinmux = pSettings->setPinmux;
    pChSettings->setVcc = pSettings->setVcc;
    pChSettings->bConnectDirectly = pSettings->connectDirectly;
    pChSettings->srcClkFreqInHz = pSettings->sourceClockFreq;
    pChSettings->ATRRecvTimeInteger.ulValue = pSettings->atrReceiveTime.value;
    pChSettings->ATRRecvTimeInteger.unit = pSettings->atrReceiveTime.unit;
    pChSettings->bDirectVccInverted = pSettings->directPowerSupply.vccInverted;
    pChSettings->bDirectRstInverted = pSettings->directPowerSupply.resetInverted;    
    switch (pSettings->resetCycles)
    {
        case 42000:
            pChSettings->eResetCycles = BSCD_MAX_RESET_IN_CLK_CYCLES;
            break;
        case 60000:
            pChSettings->eResetCycles = BSCD_TDA803X_MAX_RESET_IN_CLK_CYCLES;
            break;
        default:
            pChSettings->eResetCycles = BSCD_MAX_RESET_IN_CLK_CYCLES;
    }

    NEXUS_IsrCallback_Set(smartcard->cardAppCallback, &pSettings->cardCallback);

    errCode = BSCD_Channel_SetParameters(smartcard->channelHandle, pChSettings);
    if ( errCode ) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    BSCD_Channel_EnableInterrupts(smartcard->channelHandle);

    smartcard->settings = *pSettings;

    return NEXUS_SUCCESS;
}

static void NEXUS_Smartcard_P_CardInsertedRemoved_isr(BSCD_ChannelHandle channelHandle, void * pData )
{
    int index;

    /* There is no way of setting pData to the smartcard handle, so we must look it up. */
    BSTD_UNUSED(pData);
    for (index=0; index < BSCD_MAX_SUPPOTED_CHANNELS; index++) {
        if (g_NEXUS_smartcards[index].channelHandle == channelHandle) {
            if ( g_NEXUS_smartcards[index].settings.cardCallback.callback) {
                NEXUS_Module_IsrCallback_Fire_isr(g_NEXUS_smartcardModule, g_NEXUS_smartcards[index].cardAppCallback);
            }
        }
    }
}

NEXUS_Error NEXUS_Smartcard_P_Open(NEXUS_SmartcardHandle smartcard, unsigned index, const NEXUS_SmartcardSettings *pSettings)
{
    NEXUS_Error errCode;

    errCode = BSCD_GetChannelDefaultSettings(g_NEXUS_smartcard.scd, index, &smartcard->channelSettings);
    if ( errCode )
    {
    errCode = BERR_TRACE(errCode);
    goto error;
    }

    smartcard->channelSettings.ucEtuClkDiv = 0;
    smartcard->channelSettings.ucScClkDiv = 0;
    smartcard->channelSettings.unPrescale = 0;
    smartcard->channelSettings.ucBaudDiv = 0;
    smartcard->channelSettings.setVcc = pSettings->setVcc;
    smartcard->channelSettings.vcc = pSettings->vcc;
    smartcard->channelSettings.setPinmux = pSettings->setPinmux;
    smartcard->channelSettings.eProtocolType = pSettings->protocol;
    smartcard->channelSettings.scStandard = pSettings->standard;
    smartcard->channelSettings.ucFFactor = pSettings->fFactor;
    smartcard->channelSettings.ucDFactor = pSettings->dFactor;
    smartcard->channelSettings.ucExternalClockDivisor = pSettings->extClockDivisor;
    smartcard->channelSettings.ucTxRetries = pSettings->txRetries;
    smartcard->channelSettings.ucRxRetries = pSettings->rxRetries;
    smartcard->channelSettings.ucBaudDiv = pSettings->baudDiv;
    smartcard->channelSettings.resetCardAction = pSettings->resetCardAction;
    smartcard->channelSettings.unCurrentIFSD = pSettings->currentIFSD;

    smartcard->channelSettings.ulCharacterWaitTimeInteger = pSettings->characterWaitTime;
    smartcard->channelSettings.bAutoDeactiveReq = pSettings->autoDeactiveReq;
    smartcard->channelSettings.bNullFilter = pSettings->nullFilter;
    smartcard->channelSettings.bIsPresHigh = pSettings->isPresHigh;
    smartcard->channelSettings.bConnectDirectly = pSettings->connectDirectly;

    NEXUS_IsrCallback_Set(smartcard->cardAppCallback, &pSettings->cardCallback);

    smartcard->channelSettings.workWaitTime.ulValue = pSettings->workWaitTime.value;
    smartcard->channelSettings.workWaitTime.unit = pSettings->workWaitTime.unit;
    smartcard->channelSettings.blockWaitTime.ulValue = pSettings->blockWaitTime.value;
    smartcard->channelSettings.blockWaitTime.unit = pSettings->blockWaitTime.unit;
    smartcard->channelSettings.extraGuardTime.ulValue = pSettings->extraGuardTime.value;
    smartcard->channelSettings.extraGuardTime.unit = pSettings->extraGuardTime.unit;
    smartcard->channelSettings.blockGuardTime.ulValue = pSettings->blockGuardTime.value;
    smartcard->channelSettings.blockGuardTime.unit = pSettings->blockGuardTime.unit;
    smartcard->channelSettings.edcSetting.edcEncode = pSettings->edcSetting.edcEncode;
    smartcard->channelSettings.edcSetting.bIsEnabled = pSettings->edcSetting.isEnabled;
    smartcard->channelSettings.timeOut.ulValue = pSettings->timeOut.value;
    smartcard->channelSettings.timeOut.unit = pSettings->timeOut.unit;
    smartcard->channelSettings.scPresDbInfo.scPresMode = pSettings->scPresDbInfo.scPresMode;
    smartcard->channelSettings.scPresDbInfo.bIsEnabled = pSettings->scPresDbInfo.isEnabled;
    smartcard->channelSettings.scPresDbInfo.ucDbWidth = pSettings->scPresDbInfo.dbWidth;
    smartcard->channelSettings.blockWaitTimeExt.ulValue = pSettings->blockWaitTimeExt.value;
    smartcard->channelSettings.blockWaitTimeExt.unit = pSettings->blockWaitTimeExt.unit;
    smartcard->channelSettings.srcClkFreqInHz = pSettings->sourceClockFreq;
    smartcard->channelSettings.ATRRecvTimeInteger.ulValue = pSettings->atrReceiveTime.value;
    smartcard->channelSettings.ATRRecvTimeInteger.unit = pSettings->atrReceiveTime.unit;
    smartcard->channelSettings.bDirectVccInverted = pSettings->directPowerSupply.vccInverted;
    smartcard->channelSettings.bDirectRstInverted = pSettings->directPowerSupply.resetInverted;
    switch (pSettings->resetCycles)
    {
        case 42000:
            smartcard->channelSettings.eResetCycles = BSCD_MAX_RESET_IN_CLK_CYCLES;
            break;
        case 60000:
            smartcard->channelSettings.eResetCycles = BSCD_TDA803X_MAX_RESET_IN_CLK_CYCLES;
            break;
        default:
            smartcard->channelSettings.eResetCycles = BSCD_MAX_RESET_IN_CLK_CYCLES;
    }
        

    smartcard->settings = *pSettings;

    errCode = BSCD_Channel_Open(g_NEXUS_smartcard.scd, &smartcard->channelHandle, index, &smartcard->channelSettings);
    if ( errCode ) {
    BERR_TRACE(NEXUS_UNKNOWN);
    goto error;
    }

    BKNI_EnterCriticalSection();
    errCode = BSCD_Channel_EnableIntrCallback_isr(smartcard->channelHandle,BSCD_IntType_eCardInsertInt,NEXUS_Smartcard_P_CardInsertedRemoved_isr);
    BKNI_LeaveCriticalSection();
    if ( errCode ) {
    BERR_TRACE(NEXUS_UNKNOWN);
    goto error;
    }
    BKNI_EnterCriticalSection();
    errCode = BSCD_Channel_EnableIntrCallback_isr(smartcard->channelHandle,BSCD_IntType_eCardRemoveInt,NEXUS_Smartcard_P_CardInsertedRemoved_isr);
    BKNI_LeaveCriticalSection();
    if ( errCode ) {
    BERR_TRACE(NEXUS_UNKNOWN);
    goto error;
    }

error:
    return errCode;

}

NEXUS_SmartcardHandle NEXUS_Smartcard_Open(unsigned index, const NEXUS_SmartcardSettings *pSettings)
{
    NEXUS_Error errCode;
    unsigned char totalChannels;
    NEXUS_SmartcardHandle smartcard;

    if ( pSettings == NULL )
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    if (index >= BSCD_MAX_SUPPOTED_CHANNELS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    smartcard = &g_NEXUS_smartcards[index];

    if ( smartcard->opened == true )
    {
        BDBG_ERR(("Smartcard %d is already open", index));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    BSCD_GetTotalChannels(g_NEXUS_smartcard.scd, &totalChannels);
    if ( index >= totalChannels)
    {
        BDBG_ERR(("unable to open Smartcard[%d]", index));
        errCode = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    if (pSettings->standard == NEXUS_SmartcardStandard_eNds || pSettings->standard == NEXUS_SmartcardStandard_eIso || 
		pSettings->standard == NEXUS_SmartcardStandard_eIrdeto || pSettings->standard == NEXUS_SmartcardStandard_eNordig) {

        NEXUS_OBJECT_INIT(NEXUS_Smartcard, smartcard);
        smartcard->opened = true;
        smartcard->cardAppCallback = NEXUS_IsrCallback_Create(smartcard, NULL);

        smartcard->state = NEXUS_SmartcardState_eUnknown;

    errCode = NEXUS_Smartcard_P_Open(smartcard, index, pSettings);
    if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto error;
        }
    } else {
        BDBG_ERR(("The requested standard is not supported at this time"));
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return smartcard;

error:
    NEXUS_Smartcard_Close(smartcard);
    return NULL;
}

void NEXUS_Smartcard_P_Close(NEXUS_SmartcardHandle smartcard)
{
    if (smartcard->channelHandle)
    {
        BKNI_EnterCriticalSection();
        BSCD_Channel_DisableIntrCallback_isr(smartcard->channelHandle,BSCD_IntType_eCardInsertInt);
        BKNI_LeaveCriticalSection();

        BKNI_EnterCriticalSection();
        BSCD_Channel_DisableIntrCallback_isr(smartcard->channelHandle,BSCD_IntType_eCardRemoveInt);
        BKNI_LeaveCriticalSection();
        if(BSCD_Channel_Close(smartcard->channelHandle))
        {
            BERR_TRACE(NEXUS_UNKNOWN);
        }
    }
}

static void NEXUS_Smartcard_P_Finalizer(NEXUS_SmartcardHandle smartcard)
{
    NEXUS_OBJECT_ASSERT(NEXUS_Smartcard, smartcard);

    NEXUS_Smartcard_P_Close(smartcard);

    smartcard->state = NEXUS_SmartcardState_eUnknown;
    NEXUS_IsrCallback_Destroy(smartcard->cardAppCallback);

    NEXUS_OBJECT_UNSET(NEXUS_Smartcard, smartcard);
    BKNI_Memset(smartcard, 0, sizeof(*smartcard));
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Smartcard, NEXUS_Smartcard_Close);

NEXUS_Error NEXUS_Smartcard_Reset(NEXUS_SmartcardHandle smartcard, bool warmReset)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    smartcard->state = warmReset ? NEXUS_SmartcardState_eWarmResetting : NEXUS_SmartcardState_eColdResetting;
    if (BSCD_Channel_ResetIFD(smartcard->channelHandle, warmReset ? BSCD_ResetType_eWarm : BSCD_ResetType_eCold)) {
        smartcard->state = NEXUS_SmartcardState_eResetDone;
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    smartcard->state = NEXUS_SmartcardState_eResetDone;

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_Smartcard_Read(NEXUS_SmartcardHandle smartcard, void *pData,  size_t numBytes, size_t *pBytesRead)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    smartcard->state = NEXUS_SmartcardState_eReceiving;
    if (BSCD_Channel_Receive(smartcard->channelHandle, pData, (unsigned long *) pBytesRead, numBytes)) {
        smartcard->state = NEXUS_SmartcardState_eReady;
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    smartcard->state = NEXUS_SmartcardState_eReady;
    if (*pBytesRead <= 0) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Smartcard_Write(NEXUS_SmartcardHandle smartcard, const void *pData, size_t numBytes, size_t *pBytesWritten)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    smartcard->state = NEXUS_SmartcardState_eTransmitting;

    if (BSCD_Channel_Transmit(smartcard->channelHandle, (uint8_t *) pData, numBytes)) {
        *pBytesWritten = 0;
        smartcard->state = NEXUS_SmartcardState_eTransmitted;
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    else {
        *pBytesWritten = numBytes;
        smartcard->state = NEXUS_SmartcardState_eTransmitted;
        return NEXUS_SUCCESS;
    }
}

NEXUS_Error NEXUS_Smartcard_GetStatus(NEXUS_SmartcardHandle smartcard, NEXUS_SmartcardStatus *pStatus)
{
    BSCD_Status  internal_status;
    BSCD_ChannelSettings internal_settings;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);
    BDBG_ASSERT(NULL != pStatus);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    rc = BSCD_Channel_GetStatus(smartcard->channelHandle, &internal_status);
    if (rc) return BERR_TRACE(rc);

    pStatus->cardPresent = internal_status.bCardPresent;
    pStatus->state = smartcard->state;

    /* some SCD settings are actually read-only status */
    rc = BSCD_Channel_GetParameters(smartcard->channelHandle, &internal_settings);
    if (rc) return BERR_TRACE(rc);

    pStatus->iccClockFrequency = internal_settings.currentICCClkFreq;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Smartcard_DetectCard(NEXUS_SmartcardHandle smartcard)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    if (BSCD_Channel_DetectCard(smartcard->channelHandle, BSCD_CardPresent_eInserted)) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return NEXUS_SUCCESS;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
NEXUS_Error NEXUS_Smartcard_ResetCard(NEXUS_SmartcardHandle smartcard, void *pData, size_t numBytes, size_t *pBytesRead)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    smartcard->state = NEXUS_SmartcardState_eUnknown;

    if (BSCD_Channel_ResetCard(smartcard->channelHandle, BSCD_ResetCardAction_eNoAction)) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if (pData) {
        if (BSCD_Channel_Receive(smartcard->channelHandle, pData, (unsigned long *) pBytesRead, numBytes))
            return BERR_TRACE(NEXUS_UNKNOWN);

        if (*pBytesRead > numBytes) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        if (*pBytesRead <= 0) {
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        else {
            smartcard->state = NEXUS_SmartcardState_eResetDone;
            /* interpret ATR data and set the settings */
            smartcard->channelSettings.extraGuardTime.ulValue = ((unsigned long *) pData )[4];
            return NEXUS_SUCCESS;
        }
    }
    return BERR_TRACE(NEXUS_UNKNOWN);
}
#else
NEXUS_Error NEXUS_Smartcard_ResetCard(NEXUS_SmartcardHandle smartcard, void *pData, size_t numBytes, size_t *pBytesRead)
{
    NEXUS_Error errCode;
    bool inverseConvention;
    bool tckPresesnt = false;
    uint8_t size = 0, n=0;
    unsigned char k=0;
    char y=0;
    char * pAtr;
    char * pAtrTemp;
    unsigned long readCount=0, errCount=0;

    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);
    smartcard->state = NEXUS_SmartcardState_eUnknown;

    if (BSCD_Channel_ResetCard(smartcard->channelHandle, BSCD_ResetCardAction_eNoAction)) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if(numBytes == 0){
        return NEXUS_SUCCESS;
    }

     if (pData) {
        BSCD_Channel_Receive(smartcard->channelHandle, pData, &readCount, numBytes);

        *pBytesRead = readCount;
        pAtr = (char *)pData;
        pAtrTemp = (char *)pData;

        if(readCount < 2){
            BDBG_ERR(("Minimum ATR size is two bytes. Data read (%d bytes) is less than minimum ATR size of two bytes.", readCount));       
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        
        if(*pAtr == 0x3f) inverseConvention=true;
        else inverseConvention=false;
        size ++;
        pAtr ++;

        k = (*pAtr) & 0x0f;
        size ++;

        if(readCount == 2){
            BDBG_WRN(("The requested ATR size = %d bytes, received ATR size = %d bytes and decoded ATR size = %d bytes.", numBytes, *pBytesRead, size));
            return NEXUS_SUCCESS;
        }


        while ((y = ((*pAtr) & 0xf0))) {
            if((*pAtr) & 0x10) n++;
            if((*pAtr) & 0x20) n++;
            if((*pAtr) & 0x40) n++;
            if((*pAtr) & 0x80) n++;
            if(((*pAtr) & 0x0f) && (size != 2)) tckPresesnt = true; /* TCK*/

            size += n;

            if((size > *pBytesRead) && (numBytes > *pBytesRead)) { /*because TS is also accounted for in pBytesRead*/
                readCount = 0;
                errCode = BSCD_Channel_Receive(smartcard->channelHandle, (void *)(pAtrTemp + *pBytesRead), &readCount, (numBytes - *pBytesRead));
                if(errCode != BSCD_STATUS_READ_SUCCESS) break;
                *pBytesRead += readCount;
                if(!readCount) {
                    errCount++;
                    /* Trying forty times(Max ATR size is 32 bytes) to get the complete ATR. If not, error out. */
                    if (errCount > 40){
                        BDBG_WRN(("The requested ATR of size = %d bytes could not be received, received ATR size = %d bytes and decoded ATR size = %d bytes.", numBytes, *pBytesRead, size));
                        return BERR_TRACE(NEXUS_UNKNOWN);
                    }
                    size -= n;                  
                    continue;
                }
            }
            else if(numBytes <= *pBytesRead){
                pAtr += n;
                break;
            }
            pAtr += n;
            n=0;
        }

        if(((*pAtr) & 0x0f) || tckPresesnt) size++;

        size += k;

        if ((*pBytesRead == numBytes) && (*pBytesRead == size)) {
            smartcard->state = NEXUS_SmartcardState_eResetDone;        
            return NEXUS_SUCCESS;
        }

        if (*pBytesRead <= 0) {
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        else {
            BDBG_WRN(("The requested ATR size = %d bytes, received ATR size = %d bytes and decoded ATR size = %d bytes.", numBytes, *pBytesRead, size));
            smartcard->state = NEXUS_SmartcardState_eResetDone;
            return NEXUS_SUCCESS;
        }
    } else {
        BDBG_ERR(("Null Pointer passed for pData."));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return NEXUS_SUCCESS;
}
#endif

NEXUS_Error NEXUS_Smartcard_PowerIcc (NEXUS_SmartcardHandle smartcard, NEXUS_SmartcardPowerIcc iccAction)
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    if (iccAction) {
        errCode = BSCD_Channel_PowerICC(smartcard->channelHandle, BSCD_PowerICC_ePowerUp);

    }
    else {
        errCode = BSCD_Channel_PowerICC(smartcard->channelHandle, BSCD_PowerICC_ePowerDown);
    }

    smartcard->state = NEXUS_SmartcardState_eUnknown;
    return errCode;
}

NEXUS_Error NEXUS_Smartcard_Deactivate (NEXUS_SmartcardHandle smartcard)
{
    BDBG_OBJECT_ASSERT(smartcard, NEXUS_Smartcard);

    if (BSCD_Channel_Deactivate(smartcard->channelHandle)){
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SmartcardModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_Error errCode = NEXUS_SUCCESS;
    unsigned i;
    BSTD_UNUSED(pSettings);

    for (i=0; i<sizeof(g_NEXUS_smartcards)/sizeof(g_NEXUS_smartcards[0]); i++) {
        NEXUS_SmartcardHandle smartcard = &g_NEXUS_smartcards[i];
        if (!smartcard) { continue; }
        if (!smartcard->opened) { continue; }

        if (enabled) {
            NEXUS_Smartcard_P_Close(smartcard);
        } 
        else {
            errCode = NEXUS_Smartcard_P_Open(smartcard, i, &smartcard->settings);
            if (errCode) { BERR_TRACE(NEXUS_UNKNOWN); return errCode; }
            /* after resume, the app must call NEXUS_Smartcard_SetSettings(), just like 
               it must do after NEXUS_Smartcard_Open(). the smartcard API is weird like that */
        }
    }

    return errCode;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif
}
