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
* $brcm_Workfile: nexus_frontend_satellite.c $
* $brcm_Revision: 13 $
* $brcm_Date: 6/21/12 5:26p $
*
* API Description:
*   API name: Frontend QAM
*    Generic APIs for QAM tuning.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend_satellite.c $
*
* 13   6/21/12 5:26p mphillip
* SW7358-304: Merge tone search to main
*
* SW7358-304/2   6/21/12 5:13p mphillip
* SW7358-304: Place tone search min ratio numerator/denominator in a
*  minRatio struct in tone search settings
*
* SW7358-304/1   6/20/12 6:32p mphillip
* SW7358-304: Implement tone search
*
* 12   5/15/12 3:35p mphillip
* SW3461-210: Switch Read/WriteSatelliteRegister to the new, general
*  interface
*
* 11   2/9/12 9:59a erickson
* SW7425-1786: verify chip id before typecasting to NEXUS_AstDevice
*
* 10   8/3/09 9:24a erickson
* PR57239: fix non-AST platforms
*
* 9   7/13/09 4:48p erickson
* PR56773: added NEXUS_FrontendDiseqcStatus
*
* PR54416/2   5/21/09 6:03p jtna
* PR54416: improve naming
*
* PR54416/1   5/15/09 7:47p jtna
* PR54416: nexus support for satellite scanning
*
* 7   5/11/09 4:29p erickson
* PR53078: get time of tune or resetStatus so that elapsed time can be
*  returned with status
*
* 6   4/2/09 2:29p erickson
* PR53078: satellite api additions
*
* 5   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
*
* 4   12/22/08 9:49p agin
* PR49982: Add reset diseqc.
*
* 3   7/11/08 5:07p jtna
* PR42924: Add channel change measurements functionality
*
* 2   3/13/08 7:41p jgarrett
* PR 39985: Setting defaults
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   10/5/07 5:52p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/

#include "nexus_frontend_module.h"

BDBG_MODULE(nexus_frontend_satellite);

BTRC_MODULE_DECLARE(ChnChange_Tune);

void NEXUS_Frontend_GetDefaultSatelliteSettings( NEXUS_FrontendSatelliteSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->mode = NEXUS_FrontendSatelliteMode_eDvb;
    pSettings->symbolRate = 20000000;
    pSettings->searchRange = 5000000;
    /* Set the equivalent of BAST_ACQSETTINGS_DEFAULT - 0x00036004 */
    pSettings->spectralInversion = NEXUS_FrontendSatelliteInversion_eScan;
    pSettings->ldpcPilot = true;
    pSettings->ldpcPilotPll = true;
    pSettings->nyquist20 = true;
}

void NEXUS_Frontend_GetDefaultSatelliteSettingsForMode( NEXUS_FrontendSatelliteMode mode, NEXUS_FrontendSatelliteSettings *pSettings )
{
    NEXUS_Frontend_GetDefaultSatelliteSettings(pSettings);
    pSettings->mode = mode;
    switch (mode) {
    default:
    case NEXUS_FrontendSatelliteMode_eDss:
    case NEXUS_FrontendSatelliteMode_eDvb:
        /* acq_ctl 0x0600C */
        pSettings->bertEnable = true;
        pSettings->nyquist20 = true;
        break;

    case NEXUS_FrontendSatelliteMode_eDcii:
    case NEXUS_FrontendSatelliteMode_eQpskTurbo:
    case NEXUS_FrontendSatelliteMode_e8pskTurbo:
    case NEXUS_FrontendSatelliteMode_eTurbo:
        break;

    case NEXUS_FrontendSatelliteMode_eQpskLdpc:
    case NEXUS_FrontendSatelliteMode_e8pskLdpc:
        /* acq_ctl 0x3600C */
        pSettings->bertEnable = true;
        pSettings->nyquist20 = true;
        pSettings->ldpcPilot = true;
        pSettings->ldpcPilotPll = true;
        break;

    case NEXUS_FrontendSatelliteMode_eLdpc:
        break;
    }
}

NEXUS_Error NEXUS_Frontend_TuneSatellite( NEXUS_FrontendHandle handle, const NEXUS_FrontendSatelliteSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    NEXUS_Time_Get(&handle->resetStatusTime);

    BDBG_ASSERT(NULL != pSettings);
    BTRC_TRACE(ChnChange_Tune, START);

    if ( NULL == handle->tuneSatellite )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_TuneSatellite(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->tuneSatellite(handle->pDeviceHandle, pSettings);
    }
}

/***************************************************************************
Summary:
    Get the status of a satellite tuner
See Also:
    NEXUS_Frontend_TuneSatellite
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteStatus *pStatus
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pStatus);

    if ( NULL == handle->getSatelliteStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatelliteStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSatelliteStatus(handle->pDeviceHandle, pStatus);
    }
}

/***************************************************************************
Summary:
    Get the current DiSEqC settings for a satellite tuner
See Also:
    NEXUS_Frontend_SetDiseqcSettings
 ***************************************************************************/
void NEXUS_Frontend_GetDiseqcSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pSettings);

    if ( NULL == handle->getDiseqcSettings )
    {
        if ( handle->pParentFrontend )
        {
            NEXUS_Frontend_GetDiseqcSettings(handle->pParentFrontend, pSettings);
        }
        else
        {
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        handle->getDiseqcSettings(handle->pDeviceHandle, pSettings);
    }
}

/***************************************************************************
Summary:
    Set the current DiSEqC settings for a satellite tuner
See Also:
    NEXUS_Frontend_GetDiseqcSettings
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_SetDiseqcSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendDiseqcSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pSettings);

    if ( NULL == handle->setDiseqcSettings )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SetDiseqcSettings(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->setDiseqcSettings(handle->pDeviceHandle, pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_GetDiseqcStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcStatus *pStatus
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->getDiseqcStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDiseqcStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDiseqcStatus(handle->pDeviceHandle, pStatus);
    }
}

/***************************************************************************
Summary:
    Send a DiSEqC message
Description:
    This routine will send the number of bytes specified in a DiSEqC message.
    The callback provided will be called when the transfer is complete, at
    which point the reply/status can be read.
See Also:
    NEXUS_Frontend_GetDiseqcReply
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_SendDiseqcMessage(
    NEXUS_FrontendHandle handle,
    const uint8_t *pSendData,
    size_t sendDataSize,                    /* In Bytes */
    const NEXUS_CallbackDesc *pSendComplete /* Callback will be received when message status is ready */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pSendData);

    if ( NULL == handle->sendDiseqcMessage )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SendDiseqcMessage(handle->pParentFrontend, pSendData, sendDataSize, pSendComplete);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->sendDiseqcMessage(handle->pDeviceHandle, pSendData, sendDataSize, pSendComplete);
    }
}

/***************************************************************************
Summary:
    Reset DiSEqC
Description:
    This routine will reset the diseqc.
See Also:
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_ResetDiseqc(
    NEXUS_FrontendHandle handle,
    uint8_t options
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->resetDiseqc )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_ResetDiseqc(handle->pParentFrontend, options);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->resetDiseqc(handle->pDeviceHandle, options);
    }
}

/***************************************************************************
Summary:
    Read a DiSEqC message reply
Description:
    This routine will read up to the number of bytes specified from a DiSEqC
    message reply.  Any remaining bytes will be discarded.
See Also:
    NEXUS_Frontend_SendDiseqcMessage
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDiseqcReply(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcMessageStatus *pStatus,
    uint8_t *pReplyBuffer,              /* [out] */
    size_t replyBufferSize,            /* in bytes */
    size_t *pReplyLength                /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pStatus);
    BDBG_ASSERT(NULL != pReplyBuffer);
    BDBG_ASSERT(NULL != pReplyLength);

    if ( NULL == handle->getDiseqcReply )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDiseqcReply(handle->pParentFrontend, pStatus, pReplyBuffer, replyBufferSize, pReplyLength);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDiseqcReply(handle->pDeviceHandle, pStatus, pReplyBuffer, replyBufferSize, pReplyLength);
    }
}

/***************************************************************************
Summary:
    Send a DiSEqC Auto Control Word
See Also:
    NEXUS_Frontend_SendDiseqcMessage
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_SendDiseqcAcw(
    NEXUS_FrontendHandle handle,
    uint8_t codeWord
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->sendDiseqcAcw )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SendDiseqcAcw(handle->pParentFrontend, codeWord);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->sendDiseqcAcw(handle->pDeviceHandle, codeWord);
    }
}

NEXUS_Error NEXUS_Frontend_ReadSatelliteConfig( NEXUS_FrontendHandle handle, unsigned id, void *buffer, unsigned bufferSize )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->readSatelliteConfig )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_ReadSatelliteConfig(handle->pParentFrontend, id, buffer, bufferSize);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->readSatelliteConfig(handle->pDeviceHandle, id, buffer, bufferSize);
    }
}

NEXUS_Error NEXUS_Frontend_WriteSatelliteConfig( NEXUS_FrontendHandle handle, unsigned id, const void *buffer, unsigned bufferSize )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->writeSatelliteConfig )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_WriteSatelliteConfig(handle->pParentFrontend, id, buffer, bufferSize);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->writeSatelliteConfig(handle->pDeviceHandle, id, buffer, bufferSize);
    }
}

void NEXUS_Frontend_GetDefaultSatellitePeakscanSettings(NEXUS_FrontendSatellitePeakscanSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->frequency= 0;
    pSettings->minSymbolRate = 15 * 1000000;
    pSettings->maxSymbolRate = 30 * 1000000;
    pSettings->frequencyRange = 5 * 1000000;
    pSettings->frequencyStep = 1 * 1000000;
    /* TODO: hardcoded values are ok for now, but consider querying AST for these */
}

void NEXUS_Frontend_GetDefaultSatelliteToneSearchSettings(NEXUS_FrontendSatelliteToneSearchSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->frequency= 0;
    pSettings->frequencyRange = 5 * 1000000;
    pSettings->minRatio.numerator = 5; /* default: 2.5  (2 1/2 = 5/2) */
    pSettings->minRatio.denominator = 2;
}

NEXUS_Error NEXUS_Frontend_SatellitePeakscan( NEXUS_FrontendHandle handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->satellitePeakscan )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SatellitePeakscan(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->satellitePeakscan(handle->pDeviceHandle, pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_GetSatellitePeakscanResult( NEXUS_FrontendHandle handle, NEXUS_FrontendSatellitePeakscanResult *pResult )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->getSatellitePeakscanResult )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatellitePeakscanResult(handle->pParentFrontend, pResult);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSatellitePeakscanResult(handle->pDeviceHandle, pResult);
    }
}

NEXUS_Error NEXUS_Frontend_SatelliteToneSearch( NEXUS_FrontendHandle handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->satelliteToneSearch )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SatelliteToneSearch(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->satelliteToneSearch(handle->pDeviceHandle, pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_GetSatelliteToneSearchResult( NEXUS_FrontendHandle handle, NEXUS_FrontendSatelliteToneSearchResult *pResult )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if ( NULL == handle->getSatelliteToneSearchResult )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatelliteToneSearchResult(handle->pParentFrontend, pResult);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSatelliteToneSearchResult(handle->pDeviceHandle, pResult);
    }
}


NEXUS_Error NEXUS_FrontendDevice_GetSatelliteCapabilities(NEXUS_FrontendDeviceHandle handle, NEXUS_FrontendSatelliteCapabilities *pCapabilities)
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_FrontendDevice_P_CheckOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getSatelliteCapabilities)
    {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    else
    {
        return handle->getSatelliteCapabilities(handle->pDevice, pCapabilities);
    }
}

NEXUS_Error NEXUS_Frontend_GetSatelliteSignalDetectStatus( NEXUS_FrontendHandle handle, NEXUS_FrontendSatelliteSignalDetectStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getSatelliteSignalDetectStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatelliteSignalDetectStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSatelliteSignalDetectStatus(handle->pDeviceHandle, pStatus);
    }
}

static void NEXUS_Frontend_P_GetSatelliteDefaultStatus(NEXUS_FrontendSatelliteAgcStatus *pStatus)
{
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
}

NEXUS_Error NEXUS_Frontend_GetSatelliteAgcStatus(NEXUS_FrontendHandle handle, NEXUS_FrontendSatelliteAgcStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getSatelliteAgcStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatelliteAgcStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        NEXUS_Frontend_P_GetSatelliteDefaultStatus(pStatus);
        return handle->getSatelliteAgcStatus(handle->pDeviceHandle, pStatus);
    }
}

NEXUS_Error NEXUS_Frontend_GetSatelliteRuntimeSettings(NEXUS_FrontendHandle handle, NEXUS_FrontendSatelliteRuntimeSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getSatelliteRuntimeSettings )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetSatelliteRuntimeSettings(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getSatelliteRuntimeSettings(handle->pDeviceHandle, pSettings);
    }
}

NEXUS_Error NEXUS_Frontend_SetSatelliteRuntimeSettings(NEXUS_FrontendHandle handle, const NEXUS_FrontendSatelliteRuntimeSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->setSatelliteRuntimeSettings )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_SetSatelliteRuntimeSettings(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->setSatelliteRuntimeSettings(handle->pDeviceHandle, pSettings);
    }
}


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
NEXUS_Error NEXUS_Frontend_SetStuffBytes(
	NEXUS_FrontendHandle handle,
       uint8_t *ucStuffbyte
)
{
     BDBG_ASSERT(NULL != handle);

       if ( NULL == handle->setStuffBytes)
	{
		if ( handle->pParentFrontend )
		{
			return NEXUS_Frontend_SetStuffBytes(handle->pParentFrontend, ucStuffbyte);
		}
		else
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}
	else
	{
		return handle->setStuffBytes(handle->pDeviceHandle, ucStuffbyte);
	}
}


NEXUS_Error NEXUS_Frontend_SetXportCtrl(
    NEXUS_FrontendHandle handle,
	bool bClkSup
)
{
     BDBG_ASSERT(NULL != handle);

       if ( NULL == handle->setXportCtl)
	{
		if ( handle->pParentFrontend )
		{
			return NEXUS_Frontend_SetXportCtrl(handle->pParentFrontend, bClkSup);
		}
		else
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}
	else
	{
		return handle->setXportCtl(handle->pDeviceHandle, bClkSup);
	}
}


NEXUS_Error NEXUS_Frontend_SetTsOutput(
       NEXUS_FrontendHandle handle,
	bool bEnableOutput
)
{
     BDBG_ASSERT(NULL != handle);

       if ( NULL == handle->setTsOutput)
	{
		if ( handle->pParentFrontend )
		{
			return NEXUS_Frontend_SetTsOutput(handle->pParentFrontend, bEnableOutput);
		}
		else
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}
	else
	{
		return handle->setTsOutput(handle->pDeviceHandle, bEnableOutput);
	}
}

#else
NEXUS_Error NEXUS_Frontend_SetStuffBytes(
	NEXUS_FrontendHandle handle,
       uint8_t *ucStuffbyte
)
{
	return BERR_SUCCESS;
{

NEXUS_Error NEXUS_Frontend_SetXportCtrl(
    NEXUS_FrontendHandle handle,
	bool bClkSup
)
{
	return BERR_SUCCESS;
{

NEXUS_Error NEXUS_Frontend_SetTsOutput(
       NEXUS_FrontendHandle handle,
	bool bEnableOutput
)
{
	return BERR_SUCCESS;
}
#endif
