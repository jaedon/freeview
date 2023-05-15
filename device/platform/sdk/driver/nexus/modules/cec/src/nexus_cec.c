/***************************************************************************
*	   (c)2007-2012 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_cec.c $
* $brcm_Revision: 9 $
* $brcm_Date: 10/12/12 3:12p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/cec/src/nexus_cec.c $
* 
* 9   10/12/12 3:12p erickson
* SW7435-419: change proc to BDBG_LOG to that they are still available
*  with B_REFSW_DEBUG_LEVEL=err
* 
* 8   9/11/12 6:41p rgreen
* SW7425-3776: Merge Changes
* 
* cng_unittest/3   8/23/12 3:41p cng
* SW7425-3776: Corrected naming of cecVersion in settings
* 
* cng_unittest/2   8/22/12 1:14p cng
* SW7425-3776: Revert previous changes in settings deviceType and added
*  default deviceType in getDefaultSettings()
* 
* cng_unittest/1   8/21/12 10:24a cng
* SW7425-3776: Add CEC version support in CecSettings. Corrected value of
*  device type.
* 
* 7   8/9/12 3:39p rgreen
* SW7425-3724,SW7425-2851: Fix memset coverity issue
* 
* 6   6/20/12 1:34p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 5   2/29/12 6:51p gmohile
* SW7425-2289 : Add cec standby
* 
* 4   2/22/12 6:56p vle
* SW7425-2279: Fix compile warning and issue when re-open NEXUS CEC
*  module
* 
* 3   2/17/12 3:45p vle
* SW7425-2279: Merge to mainline
* 
* SW7425-2279/3   2/16/12 4:55p vle
* SW7425-2279: Additional updates after 2nd review. Still need to rework
*  Standby/Resume function to use CEC autoOn feature.
* 
* SW7425-2279/2   2/14/12 5:57p vle
* SW7425-2279: More updates improving implementations.
* 
* SW7425-2279/1   2/9/12 3:36p vle
* SW7425-2279: Update BCEC PI implementations. Use separate events for
*  transmit and receive.
* 
* 2   12/21/11 2:52p vle
* SW7405-4781: Merge to mainline
* 
* SW7405-4781/1   12/21/11 12:12p vle
* SW7405-4781: allow CEC device type to be configurable
* 
* 1   12/15/11 4:27p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
* 
* SW7425-1140/1   12/9/11 4:41p vle
* SW7425-1140: Add NEXUS_Cec module support
* 
***************************************************************************/

#include "nexus_cec_module.h"
#include "nexus_cec.h"
#include "priv/nexus_core.h"


BDBG_MODULE(nexus_cec);

static NEXUS_Cec g_cec;
static void NEXUS_Cec_P_AllocateLogicalAddress(NEXUS_CecHandle handle);


void NEXUS_CecModule_Print(void)
{
	BDBG_LOG(("CEC: %s",g_cec.opened ? "opened" : "closed"));
}

NEXUS_Error NEXUS_Cec_P_Shutdown(void)
{
	if ( g_cec.opened )
	{
		BDBG_ERR(("Force closing CEC interface"));
		NEXUS_Cec_Close(&g_cec);
	}
	
	return NEXUS_SUCCESS;
}


void NEXUS_Cec_P_TransmittedCallback(void *pContext)
{
	NEXUS_CecHandle handle = (NEXUS_CecHandle)pContext;
	BCEC_MessageStatus stMessageStatus;
	NEXUS_Error rc;
	
	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);

	rc = BCEC_GetTransmitMessageStatus(handle->cecHandle, &stMessageStatus);
	if (rc) {
		BERR_TRACE(rc);
		return ;
	}
	
	if (handle->searchState < NEXUS_CecLogicalAddrSearch_eReady) 
	{
		if (!stMessageStatus.uiStatus)
			handle->searchState = NEXUS_CecLogicalAddrSearch_eReady;
		else 
			handle->searchState = NEXUS_CecLogicalAddrSearch_eNext;
		NEXUS_Cec_P_AllocateLogicalAddress(handle);
	}
	else 
	{
		handle->status.messageTransmitPending = false;
		NEXUS_TaskCallback_Fire(handle->messageTransmittedCallback);
	}
}


void NEXUS_Cec_P_ReceivedCallback(void *pContext)
{
	NEXUS_CecHandle handle = (NEXUS_CecHandle)pContext;
	BCEC_MessageStatus stMessageStatus;
	NEXUS_Error rc;
	
	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);
	
	rc = BCEC_GetReceivedMessageStatus(handle->cecHandle, &stMessageStatus);
	if (rc) {
		BERR_TRACE(rc);
		return ;
	}

	handle->status.messageReceived = true;
	NEXUS_TaskCallback_Fire(handle->messageReceivedCallback);			
	return;
}


/**
Summary:
Open a CEC interface
**/
NEXUS_CecHandle NEXUS_Cec_Open( /* attr{destructor=NEXUS_Cec_Close} */
	unsigned index,
	const NEXUS_CecSettings *pSettings
)
{
	BERR_Code errCode;
	NEXUS_Cec *pCec;
	BKNI_EventHandle cecTransmittedEvent;
	BKNI_EventHandle cecReceivedEvent;
	BCEC_Dependencies stCecDependencies;


	if (index > 0) {
		/* only one for now */
		BERR_TRACE(NEXUS_NOT_AVAILABLE);
		return NULL;
	}

	pCec = &g_cec;
	
	if ( pCec->opened )
	{
		BDBG_ERR(("CEC interface already opened"));
		return NULL;
	}

	NEXUS_OBJECT_INIT(NEXUS_Cec, pCec);

	pCec->cecSettings = *pSettings;
	pCec->messageTransmittedCallback = NEXUS_TaskCallback_Create(pCec, NULL);
	pCec->messageReceivedCallback = NEXUS_TaskCallback_Create(pCec, NULL);
	pCec->logicalAddressAcquiredCallback = NEXUS_TaskCallback_Create(pCec, NULL);		

	/* must init CEC before allows CEC callback */
	pCec->status.physicalAddress[0] = 0xFF;
	pCec->status.physicalAddress[1] = 0xFF;
	pCec->status.logicalAddress = BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR;

	/* save dependencies */
	stCecDependencies.hChip = g_pCoreHandles->chp;
	stCecDependencies.hRegister = g_pCoreHandles->reg;
	stCecDependencies.hInterrupt = g_pCoreHandles->bint;

	errCode = BCEC_Open(&pCec->cecHandle, &stCecDependencies);
	if ( errCode )
	{
		errCode = BERR_TRACE(errCode);
		goto err_cec;
	}

	/* Transmit event */
	errCode = BCEC_GetEventHandle(pCec->cecHandle, BCEC_EventCec_eTransmitted, &cecTransmittedEvent) ;
	if (errCode)
		goto err_cec;
	pCec->cecTransmittedEventCallback = NEXUS_RegisterEvent(cecTransmittedEvent, NEXUS_Cec_P_TransmittedCallback, pCec);
	if (!pCec->cecTransmittedEventCallback)
		goto err_cec;

	/* Receive event */
	errCode = BCEC_GetEventHandle(pCec->cecHandle, BCEC_EventCec_eReceived, &cecReceivedEvent) ;
	if (errCode)
		goto err_cec;
	pCec->cecReceivedEventCallback = NEXUS_RegisterEvent(cecReceivedEvent, NEXUS_Cec_P_ReceivedCallback, pCec);
	if (!pCec->cecReceivedEventCallback)
		goto err_cec;


	pCec->opened = true;
	return pCec;


err_cec:
	
	NEXUS_Cec_Close(pCec);
	return NULL;
}


/**
Summary:
Close the Cec interface
**/
static void NEXUS_Cec_P_Finalizer(	NEXUS_CecHandle handle)
{
	NEXUS_OBJECT_ASSERT(NEXUS_Cec, handle);

	if ( handle->cecTransmittedEventCallback ) {
		NEXUS_UnregisterEvent(handle->cecTransmittedEventCallback);
	}	

	if ( handle->cecReceivedEventCallback ) {
		NEXUS_UnregisterEvent(handle->cecReceivedEventCallback);
	}

	if (handle->cecHandle) {
		BCEC_Close(handle->cecHandle);
	}
	if (handle->messageTransmittedCallback) {
		NEXUS_TaskCallback_Destroy(handle->messageTransmittedCallback);
	}
	if (handle->messageReceivedCallback) {
		NEXUS_TaskCallback_Destroy(handle->messageReceivedCallback);
	}
	if (handle->logicalAddressAcquiredCallback) {
		NEXUS_TaskCallback_Destroy(handle->logicalAddressAcquiredCallback);
	}
	
	/* Wipe object w/ non-zero values. easier to see it core dumps. */
	NEXUS_OBJECT_DESTROY(NEXUS_Cec, handle);
	handle->opened = false;
	
	return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Cec, NEXUS_Cec_Close);

void NEXUS_Cec_GetDefaultSettings(
	NEXUS_CecSettings *pSettings /* [out] */
)
{
	BKNI_Memset(pSettings, 0, sizeof(*pSettings));
	
	pSettings->physicalAddress[0]=0xFF;
	pSettings->physicalAddress[1]=0xFF;	
	pSettings->deviceType = NEXUS_CecDeviceType_eTuner;
	return;
}

	
void NEXUS_Cec_GetSettings(
	NEXUS_CecHandle handle,
	NEXUS_CecSettings *pSettings /* [out] */
)
{
	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);
	*pSettings = handle->cecSettings;
}


NEXUS_Error NEXUS_Cec_SetSettings(
	NEXUS_CecHandle handle,
	const NEXUS_CecSettings *pSettings
)	
{
	NEXUS_Error rc = 0;
	BCEC_Settings stCecSettings;

	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);
	
	/* save physical address & device type */
	BKNI_Memcpy(handle->status.physicalAddress, pSettings->physicalAddress, sizeof(pSettings->physicalAddress));
	handle->status.deviceType = pSettings->deviceType;

	BCEC_GetDefaultSettings(&stCecSettings);
	stCecSettings.enable = pSettings->enabled;
	
	rc = BCEC_SetSettings(handle->cecHandle, &stCecSettings);
	if (rc) return BERR_TRACE(rc);

	if (pSettings->enabled && (handle->cecSettings.enabled != pSettings->enabled
	 || handle->status.logicalAddress == BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR))
	{
		handle->searchState = NEXUS_CecLogicalAddrSearch_eInit;
		NEXUS_Cec_P_AllocateLogicalAddress(handle);
	}

	/* Set callbacks */
	NEXUS_TaskCallback_Set(handle->messageTransmittedCallback, &pSettings->messageTransmittedCallback);
	NEXUS_TaskCallback_Set(handle->messageReceivedCallback, &pSettings->messageReceivedCallback);
	NEXUS_TaskCallback_Set(handle->logicalAddressAcquiredCallback, &pSettings->logicalAddressAcquiredCallback);	

	handle->cecSettings = *pSettings;
	return rc;
}


NEXUS_Error NEXUS_Cec_GetStatus(
	NEXUS_CecHandle handle,
	NEXUS_CecStatus *pStatus /* [out] */
)
{
	NEXUS_Error rc = BERR_SUCCESS;
	BCEC_Settings stCecSettings;
	BCEC_MessageStatus stMessageStatus;
	
	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);
	
	rc = BCEC_GetSettings(handle->cecHandle, &stCecSettings);	
	/* device logical address has not yet been established */
	if (rc == BERR_NOT_INITIALIZED)
		handle->status.ready = false;
	else
		handle->status.ready = true;


	/* Get device logical and physical address */
	/* logical and physical address */
	handle->status.logicalAddress = stCecSettings.CecLogicalAddr;
	BKNI_Memcpy(handle->status.physicalAddress, stCecSettings.CecPhysicalAddr, 
			 sizeof(stCecSettings.CecPhysicalAddr));
	
	/* Device type */
	handle->status.deviceType = (NEXUS_CecDeviceType) stCecSettings.eDeviceType;
	
	/* CEC Version */
	handle->status.cecVersion = stCecSettings.cecVersion;

	/* message status */
	rc = BCEC_GetTransmitMessageStatus(handle->cecHandle, &stMessageStatus);
	if (rc)
	{
		BDBG_ERR(("Error getting CEC message info"));
		return BERR_TRACE(rc);
	}
	handle->status.transmitMessageAcknowledged = (bool) stMessageStatus.uiStatus;


	/* return status */
	*pStatus = handle->status;
	return rc;
}


NEXUS_Error NEXUS_Cec_ReceiveMessage(
	NEXUS_CecHandle handle,
	NEXUS_CecReceivedMessage *pReceivedMessage /* [out] */
)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	BAVC_HDMI_CEC_MessageData stMessageData;
	BCEC_MessageStatus stMessageStatus;

	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);

	/* reset status */
	handle->status.messageReceived = false;

	rc = BCEC_GetReceivedMessage(handle->cecHandle, &stMessageData);
	if (rc)
	{
		BDBG_ERR(("Error receiving CEC Msg")) ;
		return BERR_TRACE(rc);
	}

	pReceivedMessage->data.initiatorAddr = stMessageData.initiatorAddr;
	pReceivedMessage->data.destinationAddr = stMessageData.destinationAddr;
	pReceivedMessage->data.length = stMessageData.messageLength;
	BKNI_Memcpy(&pReceivedMessage->data.buffer, &stMessageData.messageBuffer,
			(sizeof(uint8_t) * stMessageData.messageLength));

	/* message status */
	rc = BCEC_GetReceivedMessageStatus(handle->cecHandle, &stMessageStatus);
	if (rc)
	{
		BDBG_ERR(("Error getting CEC message info"));
		goto done;
	}
	pReceivedMessage->receivedStatus.receivedMessageAcknowledged = (bool) stMessageStatus.uiStatus;
	pReceivedMessage->receivedStatus.endOfMessage = (bool) stMessageStatus.uiEOM;


	/* Re-enable the CEC core to receive the next incoming CEC message */	
	rc = BCEC_EnableReceive(handle->cecHandle);
	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Error enable CEC core to receive messages"));
		goto done;
	}

done:	
	return rc;
}


void NEXUS_Cec_GetDefaultMessageData(
	NEXUS_CecMessage *pMessage /* [out] */
)	
{
	BKNI_Memset(pMessage, 0, sizeof(NEXUS_CecMessage)) ;
	return;
}


NEXUS_Error NEXUS_Cec_TransmitMessage(
	NEXUS_CecHandle handle,
	const NEXUS_CecMessage *pMessage
)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	BAVC_HDMI_CEC_MessageData stCecMessageData;

	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);

	if (handle->status.messageTransmitPending)
	{
		rc = NEXUS_NOT_AVAILABLE;
		goto done;
	}

	/* prepare CEC message data */
	stCecMessageData.initiatorAddr = pMessage->initiatorAddr;
	stCecMessageData.destinationAddr = pMessage->destinationAddr;
	stCecMessageData.messageLength = pMessage->length;
	BKNI_Memcpy(stCecMessageData.messageBuffer, pMessage->buffer, 
			sizeof(pMessage->buffer));

	/* transmit message */
	rc = BCEC_XmitMessage(handle->cecHandle, &stCecMessageData);

	/* set Pending if message successfuly sent */
	if (!rc)
		handle->status.messageTransmitPending = true;

done:
	return rc;
}


static const uint8_t g_logicalAddrArray[] =
{
	BAVC_HDMI_CEC_StbDevices_eSTB1,
	BAVC_HDMI_CEC_StbDevices_eSTB2,
	BAVC_HDMI_CEC_StbDevices_eSTB3,
	BAVC_HDMI_CEC_StbDevices_eSTB4,
	BAVC_HDMI_CEC_StbDevices_eFreeUse,
	BAVC_HDMI_CEC_AllDevices_eUnRegistered
};


static void NEXUS_Cec_P_AllocateLogicalAddress(NEXUS_CecHandle handle)
{
	NEXUS_Error rc;
	uint8_t addr;
	BCEC_Settings stCecSettings;

	BDBG_OBJECT_ASSERT(handle, NEXUS_Cec);

	switch (handle->searchState) 
	{
		
	default:
	case NEXUS_CecLogicalAddrSearch_eInit:	/* ping first Address */
		addr = g_logicalAddrArray[handle->logAddrSearchIndex];
		BDBG_MSG(("Starting search for CEC Logical Addr: %d...", addr)) ;
		handle->logAddrSearchIndex = 0;
		handle->status.logicalAddress = BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR;
		rc = BCEC_PingLogicalAddr(handle->cecHandle, addr);
		if (rc) rc = BERR_TRACE(rc);

		break;

	case NEXUS_CecLogicalAddrSearch_eNext:
		handle->logAddrSearchIndex++;
		addr = g_logicalAddrArray[handle->logAddrSearchIndex];
		BDBG_MSG(("Continuing search for CEC Logical Addr: %d...", addr)) ;

		if (handle->logAddrSearchIndex == sizeof(g_logicalAddrArray)/sizeof(g_logicalAddrArray[0]) - 1) 
		{
			BDBG_WRN(("All CEC Addrs used; device unregistered")) ;
			/* Update state and fall through */
			handle->searchState = NEXUS_CecLogicalAddrSearch_eReady;
		}	
		else {
			rc = BCEC_PingLogicalAddr(handle->cecHandle, addr);
			if (rc) rc = BERR_TRACE(rc);
			break;
		}

	case NEXUS_CecLogicalAddrSearch_eReady:
		handle->status.logicalAddress = g_logicalAddrArray[handle->logAddrSearchIndex];

		BDBG_MSG(("Found CEC Logical Addr: %d", handle->status.logicalAddress)) ;

		/* Get cec Settings. Ignore uninitialized error due to uninitialize Logical Address */
		BCEC_GetSettings(handle->cecHandle, &stCecSettings);
		
			stCecSettings.enable = true;
			stCecSettings.CecLogicalAddr = handle->status.logicalAddress;
			BKNI_Memcpy(stCecSettings.CecPhysicalAddr, handle->status.physicalAddress, 
				sizeof(handle->status.physicalAddress));
			stCecSettings.eDeviceType = handle->status.deviceType;

		/* Apply logical address, physical Address and other cec settings */
		rc = BCEC_SetSettings(handle->cecHandle, &stCecSettings);
		if (rc) 
		{
			rc = BERR_TRACE(rc);
			return;
		}
		else 
		{ 
			/* Report Physical Address */
			rc = BCEC_ReportPhysicalAddress(handle->cecHandle);
			if (rc) rc = BERR_TRACE(rc);

			/* always enable receive after CEC msg is processed */
			rc = BCEC_EnableReceive(handle->cecHandle);
			if (rc) rc = BERR_TRACE(rc);

			NEXUS_TaskCallback_Fire(handle->logicalAddressAcquiredCallback);
		}
		break;
	}
}

NEXUS_Error NEXUS_CecModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    NEXUS_Cec *pCec = &g_cec;
    BCEC_Settings cecSettings;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BSTD_UNUSED(pSettings);

    if(pCec->opened) {	
	if(enabled) {
	    BCEC_GetSettings(pCec->cecHandle, &cecSettings);
	    cecSettings.enableAutoOn = true;
	    BCEC_SetSettings(pCec->cecHandle, &cecSettings);
	    rc = BCEC_Standby(pCec->cecHandle, NULL);
	} else {
	    rc = BCEC_Resume(pCec->cecHandle);
	    BCEC_GetSettings(pCec->cecHandle, &cecSettings);
	    cecSettings.enableAutoOn = false;
	    BCEC_SetSettings(pCec->cecHandle, &cecSettings);
	}
    }

    return rc;
	
#else 
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif
}
