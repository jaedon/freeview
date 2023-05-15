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
* $brcm_Workfile: nexus_frontend_ofdm.c $
* $brcm_Revision: 15 $
* $brcm_Date: 7/5/12 12:10p $
*
* API Description:
*   API name: Frontend OFDM
*    Generic APIs for OFDM (Orthogonal Frequency-Division Multiplexing) tuning.
*    This is used in DVB-H and DVB-T environments.
*
* Revision History:
*
* $brcm_Log: X:/win_views/7241/nexus/modules/frontend/common/src/nexus_frontend_ofdm.c $
* 
* 15   7/5/12 12:10p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
* 
* 14   3/12/12 5:35p vishk
* SW3461-169: Explicitly set the default acquisition monde to eAuto.
* 
* 13   1/25/12 3:10p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status.
* 
* 3461_PARTIAL_STATUS/2   1/25/12 3:05p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status. Cleanup of enum and api names.
* 
* 3461_PARTIAL_STATUS/1   1/23/12 4:14p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status.
* 
* 12   9/26/11 2:03p vishk
* SW3461-53: 3461 interrupt line 'stuck low' after multiple tune events.
*  Merge to mainline.
* 
* 11   9/7/11 6:10p vishk
* SW3461-36: add DVB T2 status API. Merge to the mainline.
* 
* 10   9/7/11 6:01p vishk
* SW3461-36: add DVB T2 status API. Fix compilation error.
* 
* SW3461-36/2   9/7/11 5:41p vishk
* SW3461-36: add DVB T2 status API.
* 
* SW3461-36/1   8/18/11 4:38p vishk
* SW3461-36: SW3461-27: SW3461-28: SW3461-26: SW3461-24: SW3461-25:
*  SW3461-18: Expose DVBT and DVBT2 status.
* 
* 9   6/6/11 2:47p vishk
* SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
*  Set the right defaults.
* 
* 8   5/24/11 7:07p vishk
* SW7231-32: Implement the Nexus code to support the 3461 frontend chip
* 
* 7   5/18/10 6:05p jgarrett
* SW7550-399: Adding option for cable/terrestrial setup in OFDM
* 
* 6   12/22/09 2:06p maivu
* SW3556-941: Enable CCI mode by default
* 
* 5   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
* 
* 4   7/30/08 12:50p jgarrett
* PR 45002: Adding TPS settings
* 
* 3   7/11/08 5:06p jtna
* PR42924: Add channel change measurements functionality
* 
* 2   2/8/08 6:37p jgarrett
* PR 39100: Adding OFDM support
* 
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   1/10/08 2:33p jgarrett
* PR 38653: Initial version
* 
***************************************************************************/

#include "nexus_frontend_module.h"

BDBG_MODULE(nexus_frontend_ofdm);

BTRC_MODULE_DECLARE(ChnChange_Tune);

/***************************************************************************
Summary:
    Initialize a OFDM settings structure to defaults
See Also:
    NEXUS_Frontend_TuneOfdm
 ***************************************************************************/
void NEXUS_Frontend_GetDefaultOfdmSettings(
    NEXUS_FrontendOfdmSettings *pSettings   /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_FrontendOfdmSettings));
    pSettings->bandwidth = NEXUS_FrontendOfdmBandwidth_e8Mhz;
    pSettings->pullInRange = NEXUS_FrontendOfdmPullInRange_eWide;
    pSettings->cciMode = NEXUS_FrontendOfdmCciMode_eAuto;
    pSettings->terrestrial = true;
    pSettings->acquisitionMode = NEXUS_FrontendOfdmAcquisitionMode_eAuto;
}

/***************************************************************************
Summary:
    Tune to a OFDM channel
See Also:
    NEXUS_Frontend_InitOfdmSettings
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneOfdm(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendOfdmSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pSettings);
    BTRC_TRACE(ChnChange_Tune, START);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
	
    if ( handle->tuneOfdm )
    {
        return handle->tuneOfdm(handle->pDeviceHandle, pSettings);
    }
    else
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_TuneOfdm(handle->pParentFrontend, pSettings);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
}

/***************************************************************************
Summary:
    Get the status of a OFDM tuner
See Also:
    NEXUS_Frontend_TuneOfdm
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOfdmStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOfdmStatus *pStatus /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }
	
    if ( handle->getOfdmStatus )
    {
        return handle->getOfdmStatus(handle->pDeviceHandle, pStatus);
    }
    else
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetOfdmStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
}

/***************************************************************************
Summary:
    Get the status asynchronously of a Ofdm tuner
See Also:
    NEXUS_Frontend_TuneOfdm
    NEXUS_Frontend_RequestOfdmStatus
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOfdmAsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOfdmStatus *pStatus /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pStatus);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getOfdmAsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetOfdmAsyncStatus(handle->pParentFrontend, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getOfdmAsyncStatus(handle->pDeviceHandle, pStatus);
    }
}

/***************************************************************************
Summary:
    Request the status asynchronously of a Ofdm tuner
See Also:
    NEXUS_Frontend_GetOfdmAsyncStatus
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestOfdmAsyncStatus(
    NEXUS_FrontendHandle handle
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->requestOfdmAsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_RequestOfdmAsyncStatus(handle->pParentFrontend);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->requestOfdmAsyncStatus(handle->pDeviceHandle);
    }
}

/***************************************************************************
Summary:
    Request the dvbt2 asynchronous status of NEXUS_FrontendDvbt2AsyncStatus type.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestDvbt2AsyncStatus(
    NEXUS_FrontendHandle handle, 
    NEXUS_FrontendDvbt2StatusType type
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->requestDvbt2AsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_RequestDvbt2AsyncStatus(handle->pParentFrontend, type);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->requestDvbt2AsyncStatus(handle->pDeviceHandle, type);
    }
}
/***************************************************************************
Summary:
    Get the dvbt2 asynchronous status ready type.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDvbt2AsyncStatusReady(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDvbt2StatusReady *pAsyncStatusReady
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getDvbt2AsyncStatusReady )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDvbt2AsyncStatusReady(handle->pParentFrontend, pAsyncStatusReady);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDvbt2AsyncStatusReady(handle->pDeviceHandle, pAsyncStatusReady);
    }
}
/***************************************************************************
Summary:
    Get the dvbt2 asynchronous status.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDvbt2AsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDvbt2StatusType type, 
    NEXUS_FrontendDvbt2Status *pStatus
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getDvbt2AsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDvbt2AsyncStatus(handle->pParentFrontend, type, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDvbt2AsyncStatus(handle->pDeviceHandle, type, pStatus);
    }
}

/***************************************************************************
Summary:
    Request the dvbc2 asynchronous status of NEXUS_FrontendDvbc2AsyncStatus type.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestDvbc2AsyncStatus(
    NEXUS_FrontendHandle handle, 
    NEXUS_FrontendDvbc2StatusType type
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->requestDvbc2AsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_RequestDvbc2AsyncStatus(handle->pParentFrontend, type);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->requestDvbc2AsyncStatus(handle->pDeviceHandle, type);
    }
}
/***************************************************************************
Summary:
    Get the dvbc2 asynchronous status ready type.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDvbc2AsyncStatusReady(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDvbc2StatusReady *pAsyncStatusReady
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getDvbc2AsyncStatusReady )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDvbc2AsyncStatusReady(handle->pParentFrontend, pAsyncStatusReady);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDvbc2AsyncStatusReady(handle->pDeviceHandle, pAsyncStatusReady);
    }
}
/***************************************************************************
Summary:
    Get the dvbc2 asynchronous status.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDvbc2AsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDvbc2StatusType type, 
    NEXUS_FrontendDvbc2Status *pStatus
    )
{
    BDBG_ASSERT(NULL != handle);

    if(NEXUS_Frontend_P_CheckDeviceOpen(handle)){
        return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    if ( NULL == handle->getDvbc2AsyncStatus )
    {
        if ( handle->pParentFrontend )
        {
            return NEXUS_Frontend_GetDvbc2AsyncStatus(handle->pParentFrontend, type, pStatus);
        }
        else
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        return handle->getDvbc2AsyncStatus(handle->pDeviceHandle, type, pStatus);
    }
}
