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
 * $brcm_Workfile: nexus_hdmi_output_hdcp.c $
 * $brcm_Revision: 30 $
 * $brcm_Date: 7/5/12 1:18p $
 *
 * Module Description:
 *                      HdmiOutput: Specific interfaces for an HDMI/DVI output.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/7400/src/nexus_hdmi_output_hdcp.c $
 * 
 * 30   7/5/12 1:18p vle
 * SW7425-1988: Merge to mainline
 * 
 * SW7425-1988/2   6/29/12 3:44p vle
 * SW7425-1988: Add device count and KSV to KSV list if attached device is
 *  a repeater. This resolve compliance test 3C-II-01 and 3C-II-06.
 * 
 * SW7425-1988/1   6/22/12 5:43p vle
 * SW7425-1988: update the MAX_DEV_EXCEED and MAX_CASCADE_EXCEED error
 *  from the hdmi_output to the hdmi_input. This is to resolve the
 *  incorrect status update in 7425 repeater test 3C-II-08 and 3C-II-09.
 *  The tests still fail due to some other errors.
 * 
 * 29   4/30/12 2:42p vle
 * SW7425-2666, SW7405-5150: Merge to mainline
 * 
 * SW7425-2666_FifoTest/3   4/30/12 2:28p vle
 * SW7425-2666, SW7405-5150: update variable names in nexus
 * 
 * SW7425-2666_FifoTest/2   4/27/12 3:02p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 *  HDCP state machine and add configurable delay for reading KVS List in
 *  addition to the required 5seconds time out. - Take 2
 * 
 * SW7425-2666_FifoTest/1   4/25/12 5:18p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 *  HDCP state machine and add configurable delay for reading KVS List in
 *  addition to the required 5seconds time out.
 * 
 * 28   10/6/11 3:08p rgreen
 * SW7425-158: display UNSUPPORTED error trace only if HDCP authentication
 *  is attempted; not during initialization
 * 
 * 27   10/6/11 2:57p rgreen
 * SW7425-158: return success and display UNSUPPORTED error trace when
 *  HDCP support is disabled;
 * 
 * 26   9/21/11 4:29p vle
 * SW7420-2041: Avoid unnecessary attempts to check hdcp status when HDMI
 *  Rx is disconnected/unpowered - Take 2
 * 
 * 25   9/9/11 3:19p vle
 * SW7420-2041: Avoid unnecessary attempts to check hdcp status when HDMI
 *  Rx is disconnected/unpowered
 * 
 * 24   8/19/11 9:09a dlwin
 * SW7125-1083: Resolved coverity:32292
 * 
 * 23   7/15/11 5:48p vle
 * SW7405-5395: NEXUS_HdmiOutput_GetHdcpStatus should return error when
 *  appropriate
 * 
 * 22   5/21/11 4:03p rgreen
 * SW7422-186: Free previously alloctated memory for revoked KSV list
 * 
 * 21   5/20/11 2:53p rgreen
 * SW7422-186: Free previously alloctated memory for revoked KSV list
 * 
 * 20   5/12/11 2:32p rgreen
 * SW7422-186: fix compilation warnings when HDCP is disabled
 * 
 * 19   4/27/11 3:46p erickson
 * SW7422-186: add stubs
 * 
 * 18   4/20/11 6:14p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 17   4/18/11 1:14p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * SW7422-186/2   4/15/11 2:56p rgreen
 * SW7422-186: nexus API review updates
 * 
 * SW7422-186/1   4/11/11 5:26p rgreen
 * SW7422-186: Add HDMI repeater support
 * 
 * 15   3/3/10 12:41p vle
 * SW7405-3987: Merge to main branch
 * 
 * SW7405-3987/1   3/2/10 5:39p vle
 * SW7405-3987: Report specific HDCP error status to high level
 *  application.
 * 
 * 14   1/28/10 1:59p vle
 * SW7420-489: Merge to main branch
 * 
 * SW7420-489/1   1/26/10 3:39p vle
 * SW7420-489: propagate max_device_count and max_depth to application
 * 
 * 13   5/18/09 2:43p erickson
 * PR55263: fix NEXUS_HdmiOutputHdcpStatus.transmittingEncrypted
 *
 * 12   1/26/09 1:55p erickson
 * PR51468: global variable naming convention
 *
 * 11   12/17/08 11:38p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 10   11/26/08 4:51p erickson
 * PR49583: merge
 *
 * PR49583/1   11/24/08 9:28p vle
 * PR49583: Fix HDCP certification issue.
 *
 * 9   4/2/08 6:46p jgarrett
 * PR 41233: Implementing Pj check option
 *
 * 8   4/2/08 11:52a jgarrett
 * PR 41215: Coverity CID 7892
 *
 * 7   4/2/08 11:47a jgarrett
 * PR 41214: Coverity CID 7893
 *
 * 6   3/31/08 1:53p erickson
 * PR41075: add BDBG_OBJECT
 *
 * 5   3/17/08 12:39p erickson
 * PR40103: replace NEXUS_QueueCallback with NEXUS_TaskCallback
 *
 * 4   2/29/08 10:12a jgarrett
 * PR 39017: Adding HDCP failed and success callbacks
 *
 * 3   2/26/08 5:42p jgarrett
 * PR 39017: Removing automatic retries
 *
 * 2   2/21/08 6:26p jgarrett
 * PR 39017: Updating to new syslib api
 *
 * 1   2/21/08 5:59p jgarrett
 * PR 39017: Adding HDCP
 *
 **************************************************************************/
#include "nexus_hdmi_output_module.h"
#include "priv/nexus_core.h"
#include "nexus_hdmi_types.h"

BDBG_MODULE(nexus_hdmi_output_hdcp);

#if NEXUS_HAS_SECURITY
#include "priv/nexus_security_priv.h"

static void NEXUS_HdmiOutput_P_RiCallback(void *pContext);
static void NEXUS_HdmiOutput_P_PjCallback(void *pContext);
static void NEXUS_HdmiOutput_P_HdcpTimerCallback(void *pContext);
static void NEXUS_HdmiOutput_P_UpdateHdcpState(NEXUS_HdmiOutputHandle handle);

#define LOCK_SECURITY() NEXUS_Module_Lock(g_NEXUS_hdmiOutputModuleSettings.modules.security)
#define UNLOCK_SECURITY() NEXUS_Module_Unlock(g_NEXUS_hdmiOutputModuleSettings.modules.security)

NEXUS_Error NEXUS_HdmiOutput_P_InitHdcp(NEXUS_HdmiOutputHandle output)
{
    BHDCPlib_Dependencies hdcpDependencies;
    BHDCPlib_Configuration *pHdcpConfig;
    BKNI_EventHandle hdmEvent;
    NEXUS_Error errCode;

    int i;
    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    pHdcpConfig = BKNI_Malloc(sizeof(BHDCPlib_Configuration));
    if ( NULL == pHdcpConfig )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Open HDCPlib */
    LOCK_SECURITY();
    BHDCPlib_GetDefaultConfiguration(pHdcpConfig);
    BHDCPlib_GetDefaultDependencies(&hdcpDependencies);
    hdcpDependencies.hHdm = output->hdmHandle;
    NEXUS_Security_GetHsm_priv(&hdcpDependencies.hHsm);
    hdcpDependencies.hTmr = g_pCoreHandles->tmr;
    errCode = BHDCPlib_Open(&output->hdcpHandle, &hdcpDependencies);
    UNLOCK_SECURITY();
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_hdcp;
    }
	
    /* Set default HDCP settings (rest are initialized to zero) */
    output->hdcpSettings.anValue = NEXUS_HdmiOutputHdcpAnValue_eRandom;
    output->hdcpSettings.transmitEncrypted = true;
    output->hdcpSettings.waitForValidVideo = pHdcpConfig->msWaitForValidVideo;
    output->hdcpSettings.waitForRxR0Margin = pHdcpConfig->msWaitForRxR0Margin;
    output->hdcpSettings.waitForKsvFifoMargin = pHdcpConfig->msWaitForKsvFifoMargin;
    output->hdcpSettings.maxDeviceCountSupported = pHdcpConfig->uiMaxDeviceCount;
    output->hdcpSettings.maxDepthSupported = pHdcpConfig->uiMaxDepth;


    /* Retrieve HDCP keys and set HDCP configuration */
    errCode = BHDCPlib_GetKeySet(pHdcpConfig->TxKeySet.TxAksv, pHdcpConfig->TxKeySet.TxKeyStructure);
    if (errCode != BERR_SUCCESS)
    {
        BDBG_ERR(("Error retrieving HDCP key set"));
        goto err_hdcp;
    }

    BKNI_Memcpy(output->hdcpSettings.aksv.data, pHdcpConfig->TxKeySet.TxAksv, BAVC_HDMI_HDCP_KSV_LENGTH);
    for ( i = 0; i < BAVC_HDMI_HDCP_N_PRIVATE_KEYS; i++ )
    {
        BKNI_Memcpy(&output->hdcpSettings.encryptedKeySet[i], &pHdcpConfig->TxKeySet.TxKeyStructure[i], sizeof(NEXUS_HdmiOutputHdcpKey));
    }

    /* get HDCP Ri Event Handle */
    errCode = BHDM_GetEventHandle(output->hdmHandle, BHDM_EventHDCPRiValue, &hdmEvent);
    if ( errCode )
        goto err_ri;
    output->riCallback = NEXUS_RegisterEvent(hdmEvent, NEXUS_HdmiOutput_P_RiCallback, output);
    if ( NULL == output->riCallback )
        goto err_ri;

    /* get HDCP Pj Event Handle */
    errCode = BHDM_GetEventHandle(output->hdmHandle, BHDM_EventHDCPPjValue, &hdmEvent);
    if ( errCode )
        goto err_pj;
    output->pjCallback = NEXUS_RegisterEvent(hdmEvent, NEXUS_HdmiOutput_P_PjCallback, output);
    if ( NULL == output->pjCallback )
        goto err_pj;

    BKNI_Free(pHdcpConfig);

    return BERR_SUCCESS;

err_pj:
    NEXUS_UnregisterEvent(output->riCallback);
err_ri:
    LOCK_SECURITY();
    BHDCPlib_Close(output->hdcpHandle);
    UNLOCK_SECURITY();
err_hdcp:
    BKNI_Free(pHdcpConfig);
    return BERR_NOT_SUPPORTED;
}

NEXUS_Error NEXUS_HdmiOutput_P_UninitHdcp(NEXUS_HdmiOutputHandle output)
{
    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    (void)NEXUS_HdmiOutput_DisableHdcpAuthentication(output);

    NEXUS_UnregisterEvent(output->pjCallback);
    NEXUS_UnregisterEvent(output->riCallback);
    LOCK_SECURITY();
    BHDCPlib_Close(output->hdcpHandle);
    UNLOCK_SECURITY();
    if (output->pRevokedKsvs)
        BKNI_Free(output->pRevokedKsvs);

    return BERR_SUCCESS;
}

void NEXUS_HdmiOutput_P_HdcpNotifyHotplug(NEXUS_HdmiOutputHandle output)
{
    BHDCPlib_Event event = {BHDM_EventHotPlug};

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    /* Any hotplug event should stop the state machine */
    if ( output->hdcpTimer )
    {
        NEXUS_CancelTimer(output->hdcpTimer);
        output->hdcpTimer = NULL;
    }

    LOCK_SECURITY();
    BHDCPlib_ProcessEvent(output->hdcpHandle, &event);
    UNLOCK_SECURITY();

    NEXUS_HdmiOutput_P_UpdateHdcpState(output);
}

static void NEXUS_HdmiOutput_P_RiCallback(void *pContext)
{
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle)pContext;
    BHDCPlib_Event event = {BHDM_EventHDCPRiValue};

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    LOCK_SECURITY();
    BHDCPlib_ProcessEvent(output->hdcpHandle, &event);
    UNLOCK_SECURITY();

    NEXUS_HdmiOutput_P_UpdateHdcpState(output);
}

static void NEXUS_HdmiOutput_P_PjCallback(void *pContext)
{
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle)pContext;
    BHDCPlib_Event event = {BHDM_EventHDCPPjValue};

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    LOCK_SECURITY();
    BHDCPlib_ProcessEvent(output->hdcpHandle, &event);
    UNLOCK_SECURITY();

    NEXUS_HdmiOutput_P_UpdateHdcpState(output);
}

/**
Summary:
Get HDCP Settings
**/
void NEXUS_HdmiOutput_GetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpSettings *pSettings /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->hdcpSettings;
}

/**
Summary:
Set HDCP Settings
**/
NEXUS_Error NEXUS_HdmiOutput_SetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpSettings *pSettings
    )
{
    BHDCPlib_Configuration *pHdcpConfig;
    BHDM_HDCP_OPTIONS hdcpOptions;
    NEXUS_Error errCode;
    int i;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pSettings);

    /* Install callback */
    NEXUS_TaskCallback_Set(handle->hdcpStateChangedCallback, &pSettings->stateChangedCallback);
    NEXUS_TaskCallback_Set(handle->hdcpSuccessCallback, &pSettings->successCallback);
    NEXUS_TaskCallback_Set(handle->hdcpFailureCallback, &pSettings->failureCallback);

    pHdcpConfig = BKNI_Malloc(sizeof(*pHdcpConfig));
    if ( NULL == pHdcpConfig )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BDBG_CASSERT(BAVC_HDMI_HDCP_KSV_LENGTH == NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH);
    BDBG_CASSERT(BAVC_HDMI_HDCP_N_PRIVATE_KEYS == NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS);
    BDBG_CASSERT(sizeof(BHDCPlib_EncryptedHdcpKeyStruct) == sizeof(NEXUS_HdmiOutputHdcpKey));

    /* Update HDCP Configuration */

    LOCK_SECURITY();
    BHDCPlib_GetConfiguration(handle->hdcpHandle, pHdcpConfig);
    UNLOCK_SECURITY();

    BKNI_Memcpy(pHdcpConfig->TxKeySet.TxAksv, pSettings->aksv.data, BAVC_HDMI_HDCP_KSV_LENGTH);
    for ( i = 0; i < BAVC_HDMI_HDCP_N_PRIVATE_KEYS; i++ )
    {
        BKNI_Memcpy(&pHdcpConfig->TxKeySet.TxKeyStructure[i], &pSettings->encryptedKeySet[i], sizeof(NEXUS_HdmiOutputHdcpKey));
    }

    pHdcpConfig->msWaitForValidVideo = pSettings->waitForValidVideo;
    pHdcpConfig->msWaitForRxR0Margin = pSettings->waitForRxR0Margin;
    pHdcpConfig->msWaitForKsvFifoMargin = pSettings->waitForKsvFifoMargin;

    LOCK_SECURITY();
    errCode = BHDCPlib_SetConfiguration(handle->hdcpHandle, pHdcpConfig);
    UNLOCK_SECURITY();

    BKNI_Free(pHdcpConfig);

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Set Pj Checking option */
    BHDM_HDCP_GetOptions(handle->hdmHandle, &hdcpOptions);
    hdcpOptions.PjChecking = pSettings->pjCheckEnabled;
    errCode = BHDM_HDCP_SetOptions(handle->hdmHandle, &hdcpOptions);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Store Settings */
    handle->hdcpSettings = *pSettings;

    /* May need to enable encryption if the setting changed.  Check now */
    if ( pSettings->transmitEncrypted )
    {
        /* Check if encryption needs to be enabled and fire callback */
        NEXUS_HdmiOutput_P_UpdateHdcpState(handle);
    }

    return BERR_SUCCESS;
}

/**
Summary:
Establish list of Revoked KSV's
**/
NEXUS_Error NEXUS_HdmiOutput_SetHdcpRevokedKsvs(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpKsv *pRevokedKsvs,    /* attr{nelem=numKsvs;nelem_convert=NEXUS_P_HDMI_OUTPUT_HDCP_KSV_SIZE} array of revoked ksv's */
    uint16_t numKsvs                                /* Number of ksvs in the array provided */
    )
{
    NEXUS_Error errCode;
    BHDCPlib_RevokedKsvList ksvList;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    /* Free old list */
    if ( NULL != handle->pRevokedKsvs )
    {
        BKNI_Free(handle->pRevokedKsvs);
        handle->pRevokedKsvs = NULL;
        handle->numRevokedKsvs = 0;
    }

    if ( NULL != pRevokedKsvs )
    {
        int size = numKsvs * sizeof(NEXUS_HdmiOutputHdcpKsv);

        /* Create new list */
        BDBG_ASSERT(numKsvs > 0);
        handle->pRevokedKsvs = BKNI_Malloc(size);
        if ( NULL == handle->pRevokedKsvs )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
        handle->numRevokedKsvs = numKsvs;
        BKNI_Memcpy(handle->pRevokedKsvs, pRevokedKsvs, size);
    }

    ksvList.Ksvs = (void *)handle->pRevokedKsvs;
    ksvList.uiNumRevokedKsvs = handle->numRevokedKsvs;

    LOCK_SECURITY();
    errCode = BHDCPlib_SetRevokedKSVs(handle->hdcpHandle, &ksvList);
    UNLOCK_SECURITY();

    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        BKNI_Free(handle->pRevokedKsvs);
        handle->pRevokedKsvs = NULL;
        handle->numRevokedKsvs = 0;
    }

    return errCode;
}

/**
Summary:
Initiate HDCP authentication

Description:
Calls to NEXUS_HdmiOutput_SetHdcpSettings() and NEXUS_HdmiOutput_SetHdcpRevokedKsvs()
should be made prior to starting authentication.

See Also:
NEXUS_HdmiOutput_SetHdcpSettings
NEXUS_HdmiOutput_SetHdcpRevokedKsvs
**/
NEXUS_Error NEXUS_HdmiOutput_StartHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    )
{
    NEXUS_Error errCode;
    NEXUS_HdmiOutputState state;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);


    /* Check for device */
    state = NEXUS_HdmiOutput_P_GetState(handle);
    if ( state != NEXUS_HdmiOutputState_eConnected )
    {
        BDBG_ERR(("Can not start authentication; Output State: %d", state));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Clean up any pending state */
    NEXUS_HdmiOutput_DisableHdcpAuthentication(handle);

    BDBG_MSG(("Starting HDCP"));
    handle->hdcpStarted = true;

    /* Reset Auth State */
    LOCK_SECURITY();
    errCode = BHDCPlib_StartAuthentication(handle->hdcpHandle);
    UNLOCK_SECURITY();

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* State Changed */
    NEXUS_HdmiOutput_P_UpdateHdcpState(handle);

    /* Kickstart state machine by faking timer callback */
    NEXUS_HdmiOutput_P_HdcpTimerCallback(handle);

    return BERR_SUCCESS;
}

/**
Summary:
Terminate HDCP authentication
**/
NEXUS_Error NEXUS_HdmiOutput_DisableHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    )
{
    NEXUS_Error errCode;
    NEXUS_HdmiOutputState state;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    handle->hdcpStarted = false;
    handle->hdcpError = NEXUS_HdmiOutputHdcpError_eSuccess;

    /* Clean up any pending timers */
    if ( NULL != handle->hdcpTimer )
    {
        NEXUS_CancelTimer(handle->hdcpTimer);
        handle->hdcpTimer = NULL;
    }

    /* Check for device */
    state = NEXUS_HdmiOutput_P_GetState(handle);
    if ( state != NEXUS_HdmiOutputState_eConnected )
        goto done;
    
    BDBG_MSG(("Disabling HDCP"));

    LOCK_SECURITY();
    errCode = BHDCPlib_DisableAuthentication(handle->hdcpHandle);
    UNLOCK_SECURITY();

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    NEXUS_HdmiOutput_P_UpdateHdcpState(handle);

done:
    
    return BERR_SUCCESS;
}

static void NEXUS_HdmiOutput_P_HdcpTimerCallback(void *pContext)
{
    NEXUS_HdmiOutputHandle output = (NEXUS_HdmiOutputHandle)pContext;
    BHDCPlib_Status hdcpStatus;
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(output, NEXUS_HdmiOutput);

    BDBG_MSG(("HDCP State Machine Timer"));

    /* Mark timer as expired */
    output->hdcpTimer = NULL;

    LOCK_SECURITY();
    errCode = BHDCPlib_ProcessAuthentication(output->hdcpHandle, &hdcpStatus);
    UNLOCK_SECURITY();

    if ( errCode )
    {
        /* All failures will eventually wind up here */
        errCode = BERR_TRACE(errCode);
        BDBG_ERR(("HDCP error occurred, aborting authentication"));
        NEXUS_HdmiOutput_P_UpdateHdcpState(output);
        NEXUS_TaskCallback_Fire(output->hdcpFailureCallback);
        return;
    }

    NEXUS_HdmiOutput_P_UpdateHdcpState(output);

    if ( hdcpStatus.msRecommendedWaitTime > 0 )
    {
        BDBG_MSG(("Arming HDCP State Machine Timer for %u ms", hdcpStatus.msRecommendedWaitTime));
        output->hdcpTimer = NEXUS_ScheduleTimer(hdcpStatus.msRecommendedWaitTime,
                                                NEXUS_HdmiOutput_P_HdcpTimerCallback,
                                                pContext);
        /* This is basically impossible to recover from */
        BDBG_ASSERT(NULL != output->hdcpTimer);
    }
    else
    {
        BDBG_WRN(("HDCP recommended no wait time.  HDCP state machine will stop now."));
    }
}

static void NEXUS_HdmiOutput_P_UpdateHdcpState(NEXUS_HdmiOutputHandle handle)
{
    bool ready;
    BHDCPlib_Status hdcpStatus;
    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);

    LOCK_SECURITY();
    BHDCPlib_GetHdcpStatus(handle->hdcpHandle, &hdcpStatus);
    ready = BHDCPlib_LinkReadyForEncryption(handle->hdcpHandle);
    UNLOCK_SECURITY();

    /* Save last HDCP error */
    handle->hdcpError = hdcpStatus.eHdcpError;

    BDBG_MSG(("Updating HDCP State from %d -> %d", handle->hdcpState, hdcpStatus.eAuthenticationState));

    if ( hdcpStatus.eAuthenticationState != handle->hdcpState )
    {
        handle->hdcpState = hdcpStatus.eAuthenticationState;
        NEXUS_TaskCallback_Fire(handle->hdcpStateChangedCallback);
    }

    if ( ready )
    {
        BDBG_MSG(("Authentication complete"));

        if ( handle->hdcpSettings.transmitEncrypted )
        {
            NEXUS_Error errCode;

            BDBG_MSG(("Enabling Encryption"));

            LOCK_SECURITY();
            errCode = BHDCPlib_TransmitEncrypted(handle->hdcpHandle);
            UNLOCK_SECURITY();
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }

        NEXUS_TaskCallback_Fire(handle->hdcpSuccessCallback);
    }
}


NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamInfo(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpDownStreamInfo *pDownstream 
    )
{
    BERR_Code rc = BERR_SUCCESS ;
    NEXUS_HdmiOutputHdcpStatus hdmiAttachedRxStatus ;


    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdmiAttachedRxStatus) ;
    BKNI_Memset(pDownstream, 0, sizeof(NEXUS_HdmiHdcpDownStreamInfo));

    /* if only one device is downstream, return with its KSV */
    if (!hdmiAttachedRxStatus.isHdcpRepeater)
    {

        /* only 1 rx device is attached */
        pDownstream->devices = 1 ;
        pDownstream->depth = 1 ;

        goto done ;
    }

    if (hdmiAttachedRxStatus.hdcpState == NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure)
    {
        pDownstream->maxDepthExceeded= 
            (hdmiAttachedRxStatus.hdcpError == NEXUS_HdmiOutputHdcpError_eRepeaterDepthExceeded);
        
        pDownstream->maxDevicesExceeded= 
            (hdmiAttachedRxStatus.hdcpError == NEXUS_HdmiOutputHdcpError_eRxDevicesExceeded);
    }

    pDownstream->isRepeater = hdmiAttachedRxStatus.isHdcpRepeater;
    BKNI_Memcpy(&pDownstream->repeaterKsv, &hdmiAttachedRxStatus.bksv, sizeof(NEXUS_HdmiHdcpKsv));

    /* multiple devices are downstream  */
    /* determine downstream depth levels and device count */
    rc = BHDM_HDCP_GetRepeaterDepth(handle->hdmHandle, &pDownstream->depth) ;
    rc = BERR_TRACE(rc) ;
    
    rc = BHDM_HDCP_GetRepeaterDeviceCount(handle->hdmHandle, &pDownstream->devices) ;
    rc = BERR_TRACE(rc) ;
    

done:
    return NEXUS_SUCCESS ;
}


    
NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamKsvs(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpKsv *pKsvs, /* attr{nelem=numDevices;nelem_out=pNumRead} */
    unsigned numDevices,
    unsigned *pNumRead
    )
{
    BERR_Code rc = BERR_SUCCESS ;
    NEXUS_HdmiOutputHdcpStatus hdmiAttachedRxStatus ;

    /* Coverity: 32292  */
    if( pKsvs == NULL ) {
    	return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    NEXUS_HdmiOutput_GetHdcpStatus(handle, &hdmiAttachedRxStatus) ;

    /* if only one device is downstream, return with its KSV */
    if (!hdmiAttachedRxStatus.isHdcpRepeater)
    {
        /* only 1 rx device is attached */
        *pNumRead = 1 ;
        BKNI_Memcpy(&pKsvs->data, &hdmiAttachedRxStatus.bksv, NEXUS_HDMI_HDCP_KSV_LENGTH) ;

        goto done ;
    }

    /* retrieve KSVs for each downtream device */
    rc = BHDM_HDCP_GetRepeaterKsvFifo(handle->hdmHandle, 
        (uint8_t *) pKsvs, (uint16_t) ((uint8_t) numDevices * NEXUS_HDMI_HDCP_KSV_LENGTH)) ;
    rc = BERR_TRACE(rc) ;

done:
    return NEXUS_SUCCESS ;
}


    
/**
Summary:
Get HDCP Status
**/
NEXUS_Error NEXUS_HdmiOutput_GetHdcpStatus(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpStatus *pStatus /* [out] */
    )
{
    NEXUS_Error errCode;
    BHDCPlib_RxInfo rxInfo;
    NEXUS_HdmiOutputState state;

    BDBG_OBJECT_ASSERT(handle, NEXUS_HdmiOutput);
    BDBG_ASSERT(NULL != pStatus);

    BDBG_CASSERT(NEXUS_HdmiOutputHdcpState_eMax == (BHDCPlib_State_ePjLinkIntegrityFailure+1));
    BDBG_CASSERT(NEXUS_HdmiOutputHdcpError_eMax == (NEXUS_HdmiOutputHdcpError)BHDCPlib_HdcpError_eCount);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    pStatus->hdcpState = handle->hdcpState;
    pStatus->hdcpError = handle->hdcpError;

    state = NEXUS_HdmiOutput_P_GetState(handle);
    if ( state != NEXUS_HdmiOutputState_eConnected )
        goto done;

    LOCK_SECURITY();
    pStatus->linkReadyForEncryption = BHDCPlib_LinkReadyForEncryption(handle->hdcpHandle);
    pStatus->transmittingEncrypted = (handle->hdcpState == BHDCPlib_State_eEncryptionEnabled);
    errCode = BHDCPlib_GetReceiverInfo(handle->hdcpHandle, &rxInfo);
    UNLOCK_SECURITY();

    if ( BERR_SUCCESS == errCode )
    {
        pStatus->isHdcpRepeater = rxInfo.bIsHdcpRepeater;
        pStatus->ksvFifoReady = (rxInfo.uiRxBCaps & BHDM_HDCP_RxCaps_eKsvFifoReady)?true:false;
        pStatus->i2c400Support = (rxInfo.uiRxBCaps & BHDM_HDCP_RxCaps_eI2c400KhzSupport)?true:false;
        pStatus->hdcp1_1Features = (rxInfo.uiRxBCaps & BHDM_HDCP_RxCaps_eHDCP_1_1_Features)?true:false;
        pStatus->fastReauthentication = (rxInfo.uiRxBCaps & BHDM_HDCP_RxCaps_eFastReauth)?true:false;
        BKNI_Memcpy(&pStatus->bksv.data, &rxInfo.RxBksv[0], sizeof(rxInfo.RxBksv));
    }
    else
    {
        return BERR_TRACE(errCode);
    }

done:
    
    return BERR_SUCCESS;
}

#else
/* No security - use stubs for HDCP */
NEXUS_Error NEXUS_HdmiOutput_P_InitHdcp(NEXUS_HdmiOutputHandle output)
{
    BSTD_UNUSED(output);
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_HdmiOutput_P_UninitHdcp(NEXUS_HdmiOutputHandle output)
{
    BSTD_UNUSED(output);
    return BERR_SUCCESS;
}

void NEXUS_HdmiOutput_P_HdcpNotifyHotplug(NEXUS_HdmiOutputHandle output)
{
    BSTD_UNUSED(output);
    return;
}

void NEXUS_HdmiOutput_GetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpSettings *pSettings /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_HdmiOutput_SetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpSettings *pSettings
    )
{
    NEXUS_TaskCallback_Set(handle->hdcpFailureCallback, &pSettings->failureCallback);
    NEXUS_TaskCallback_Set(handle->hdcpStateChangedCallback, &pSettings->stateChangedCallback);
    NEXUS_TaskCallback_Set(handle->hdcpSuccessCallback, &pSettings->successCallback);

    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOutput_SetHdcpRevokedKsvs(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpKsv *pRevokedKsvs,    /* attr{nelem=numKsvs;nelem_convert=NEXUS_P_HDMI_OUTPUT_HDCP_KSV_SIZE} array of revoked ksv's */
    uint16_t numKsvs                                /* Number of keys in the array provided */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pRevokedKsvs);
    BSTD_UNUSED(numKsvs);

    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOutput_StartHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOutput_DisableHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOutput_GetHdcpStatus(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpStatus *pStatus /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamInfo(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpDownStreamInfo *pDownstream 
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pDownstream);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}

NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamKsvs(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpKsv *pKsvs, /* attr{nelem=numDevices;nelem_out=pNumRead} */
    unsigned numDevices,
    unsigned *pNumRead
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pKsvs);
    BSTD_UNUSED(numDevices);
    BSTD_UNUSED(pNumRead);

    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NEXUS_SUCCESS ;
}

#endif
