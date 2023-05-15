/**
* di_hdmi.c
*/

/**
* @defgroup		DI_HDMI DI_HDMI : DI_HDMI module
* @author			Chung Dong Chul
* @modified by		Sun-woo Lee
* @note			DI_HDMI APIs
* @brief			Define DI_HDMI APIs
* @file 			di_hdmi.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "bstd.h"

#include "linden_trace.h"

#include "nexus_platform.h"
#include "nexus_hdmi_output_hdcp.h"
#if defined(CONFIG_HDMI_CEC)
#include "nexus_cec.h"
#endif
#if defined(CONFIG_HDMI_IN)
#include "nexus_hdmi_input.h"
#include "nexus_hdmi_input_hdcp.h"
#endif
#include "nexus_display_types.h"
#include "nexus_display.h"
#include "nexus_audio_types.h"

#include "di_err.h"
#include "drv_err.h"
#include "drv_flash.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "drv_hdmi.h"
#include "drv_nvram.h"
#include "di_hdmi.h"
#include "di_nvram.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_pm.h"
#include "di_uart.h"
#include "vkernel.h"
#include "taskdef.h"
#if defined(CONFIG_DI20)
#include "cmdtool.h"
#endif

#include "di_hdmi_priv.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
extern HdmiInstance_t g_hdmi_instance;
HULONG g_ulCecSyncCallEventId;

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/


#if __________p__________
#endif


static void P_HDMI_CEC_DeviceReady_callback(void *context, int param)
{
	HAPPY(context);
	HAPPY(param);

	PrintEnter();

#if defined(CONFIG_DEBUG)
	{
		NEXUS_CecStatus status;
		NEXUS_Cec_GetStatus(g_hdmi_instance.hCec, &status);

		PrintDebug("BCM%d Logical Address <%d> Acquired\n", BCHP_CHIP, status.logicalAddress);
		PrintDebug("BCM%d Physical Address: %X.%X.%X.%X\n", BCHP_CHIP,
														(status.physicalAddress[0] & 0xF0) >> 4,
														(status.physicalAddress[0] & 0x0F),
														(status.physicalAddress[1] & 0xF0) >> 4,
														(status.physicalAddress[1] & 0x0F));

		if ((status.physicalAddress[0] & 0xFF) && (status.physicalAddress[1] = 0xFF))
		{
			PrintDebug("CEC Devie Ready!\n");
		}
	}
#endif
	PrintExit();

	return;
}

static void P_HDMI_CEC_MsgReceived_callback(void *context, int param)
{
	NEXUS_CecStatus status;
	NEXUS_CecReceivedMessage receivedMessage;
	NEXUS_Error eNexusError=NEXUS_SUCCESS;
#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	int i, j;
	char msgBuffer[16];
#endif
	DI_HDMI_CEC_MSG_t 	tDiCecMsg;

	PrintEnter();

	HAPPY(context);
	HAPPY(param);

	eNexusError=NEXUS_Cec_GetStatus(g_hdmi_instance.hCec, &status);
#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	PrintDebug("Msg Received Status for Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
		(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
		(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
		status.logicalAddress);
	PrintDebug("Received Msg Acknowledged: %s\n", status.transmitMessageAcknowledged ? "Yes" : "No");
	PrintDebug("Received Msg Pending: %s\n", status.messageTransmitPending ? "Yes" : "No");
#endif
	PrintDebug("Message Received: %s\n", status.messageReceived ? "Yes" : "No");
	PrintDebug("CEC Device Type: %d\n", status.deviceType);

	if (status.messageReceived==FALSE)
	{
		goto done;
	}

	eNexusError=NEXUS_Cec_ReceiveMessage(g_hdmi_instance.hCec, &receivedMessage);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		PrintError("@@ERROR@@ %s, eNexusError: 0x%0x\n", __func__, eNexusError);
		goto done;
	}

#if defined(DEBUG_HDMI_SEE_PARAMETERS)
	{
		/* For debugging purposes */
		for (i=0, j=0; i<= receivedMessage.data.length; i++)
		{
			j += VK_snprintf(msgBuffer + j, sizeof(msgBuffer)-j, "%02X ", receivedMessage.data.buffer[i]);

		}

		PrintDebug("CEC Message Length %d Received: %s\n", receivedMessage.data.length, msgBuffer);
		PrintDebug("Msg Recd Status from Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
			(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
			(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
			status.logicalAddress);
	}
#endif

	if (g_hdmi_instance.pfCallback[DH_EVENT_CEC_MSG_RECEIVED]!=NULL)
	{
		VK_memset(&tDiCecMsg, 0x00, sizeof(DI_HDMI_CEC_MSG_t));
		tDiCecMsg.ucInitAddr=receivedMessage.data.initiatorAddr;
		tDiCecMsg.ucDestAddr=receivedMessage.data.destinationAddr;
		tDiCecMsg.ucMegLength=receivedMessage.data.length;

		VK_memcpy(tDiCecMsg.ucMessage, receivedMessage.data.buffer, receivedMessage.data.length);


		g_hdmi_instance.pfCallback[DH_EVENT_CEC_MSG_RECEIVED]((void *)&tDiCecMsg);
	}

done:
	PrintExit();

	return;
}

static void P_HDMI_CEC_MsgTransmitted_callback(void *context, int param)
{
	HAPPY(context);
	HAPPY(param);

	PrintEnter();

	VK_EVENT_Send(g_ulCecSyncCallEventId, 0);
#if defined(CONFIG_DEBUG)
	{
		NEXUS_CecStatus status;

		NEXUS_Cec_GetStatus(g_hdmi_instance.hCec, &status);

		PrintDebug("Msg Xmit Status for Phys/Logical Addrs: %X.%X.%X.%X / %d\n",
			(status.physicalAddress[0] & 0xF0) >> 4, (status.physicalAddress[0] & 0x0F),
			(status.physicalAddress[1] & 0xF0) >> 4, (status.physicalAddress[1] & 0x0F),
			status.logicalAddress);
		PrintDebug("Xmit Msg Acknowledged: %s\n", status.transmitMessageAcknowledged ? "Yes" : "No");
		PrintDebug("Xmit Msg Pending: %s\n", status.messageTransmitPending ? "Yes" : "No");
	}
#endif
	PrintExit();

	return;
}

#if __________drv___________
#endif

DRV_Error DRV_HDMI_CEC_GetLogicalAddress(NEXUS_CecHandle hCec, HUINT32 *pulLogicalAddress)
{
	NEXUS_CecStatus 	tCecStatus;
	NEXUS_Error rc = NEXUS_SUCCESS;

	PrintEnter();

	rc = NEXUS_Cec_GetStatus(hCec, &tCecStatus);
	if (rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Cec_GetStatus returned Error!!, error 0x%X\n", rc);
		return DRV_ERR;
	}

	*pulLogicalAddress = tCecStatus.logicalAddress;

	PrintExit();

	return DRV_OK;
}

#if __________di____________
#endif

DI_ERR_CODE DI_HDMI_SendCecMsg(DI_HDMI_CEC_MSG_t* pstCecMsg)
{
	DI_ERR_CODE	eDiError=DI_ERR_OK;
	NEXUS_Error	eNexusError=NEXUS_SUCCESS;
	NEXUS_CecMessage transmitMessage;
	unsigned long		evtResult;
	int					vkResult;

	PrintEnter();

	if (g_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (pstCecMsg == NULL)
	{
		PrintError("pstCecMsg is NULL\n");

		return DI_ERR_ERROR;
	}

	transmitMessage.initiatorAddr=(HUINT8)(pstCecMsg->ucInitAddr);
	transmitMessage.destinationAddr=(HUINT8)(pstCecMsg->ucDestAddr);
	transmitMessage.length = (HUINT8)(pstCecMsg->ucMegLength);
	VK_memcpy(transmitMessage.buffer, pstCecMsg->ucMessage, pstCecMsg->ucMegLength);

	eNexusError=NEXUS_Cec_TransmitMessage(g_hdmi_instance.hCec, &transmitMessage);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		PrintError("[ERROR] Send Cec Message, eNexusError: 0x%x !!\n", eNexusError);
		eDiError=DI_ERR_SEND;
		goto done;
	}

	vkResult = VK_EVENT_ReceiveTimeout(g_ulCecSyncCallEventId, &evtResult, 500);
	if (vkResult!=VK_OK)
	{
		PrintError("[VK_EVENT_ReceiveTimeout] Send Cec Message, vkResult: 0x%x !!\n", vkResult);
		eDiError=DI_ERR_TIMEOUT;
		goto done;
	}

done:
	PrintExit();
	return eDiError;
}

DI_ERR_CODE DI_HDMI_GetLogicalAddress(HUINT8* ucLogicalAddr)
{
	HINT32 vResult;
	DI_ERR_CODE rc=DI_ERR_OK;
	NEXUS_Error	eNexusError=NEXUS_SUCCESS;
	NEXUS_CecStatus 	tCecStatus;

	PrintEnter();

	if (g_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");

		return DI_ERR_ERROR;
	}

	if (ucLogicalAddr == NULL)
	{
		PrintError("ucLogicalAddr is NULL\n");

		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(g_hdmi_instance.ulSemId);
	if (vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");

		return DI_ERR_ERROR;
	}

	*ucLogicalAddr = 0xF;

	eNexusError=NEXUS_Cec_GetStatus(g_hdmi_instance.hCec, &tCecStatus);
	if (eNexusError!=NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_HdmiOutput_GetCecConfiguration Error: 0x%x !!\n", __func__, eNexusError);
		rc=DI_ERR_ERROR;
		goto done;
	}
	else
	{
		*ucLogicalAddr=tCecStatus.logicalAddress;
	}
done:

	VK_SEM_Release(g_hdmi_instance.ulSemId);

	PrintExit();

	return rc;
}


DI_ERR_CODE DI_HDMI_GetCECInfo(DI_HDMI_CEC_STATE* pState)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;
	NEXUS_Error nexus_rc;
	NEXUS_CecStatus	pStatus;

	PrintEnter();

	if(g_hdmi_instance.bInitialized == FALSE)
	{
		PrintError("HDMI driver wasn't initialized\n");
		return DI_ERR_ERROR;
	}

	if(pState == NULL)
	{
		PrintError("pState is NULL\n");
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(g_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	nexus_rc = NEXUS_Cec_GetStatus(g_hdmi_instance.hCec, &pStatus);
	if(nexus_rc!=NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Cec_GetStatus failed\n");
		rc=DI_ERR_ERROR;
		goto done;
	}

	if(pStatus.logicalAddress == 0xFF)
	{
		*pState=DI_HDMI_CEC_NO_DEVICE;
	}
	else
	{
		*pState=DI_HDMI_CEC_DEVICE;
	}

	done:

	VK_SEM_Release(g_hdmi_instance.ulSemId);
	PrintExit();

	return rc;

}

DI_ERR_CODE DI_HDMI_CEC_Init(void)
{
	HINT32 vResult;
	DI_ERR_CODE rc = DI_ERR_OK;

	NEXUS_Error nexus_rc = NEXUS_SUCCESS;
	NEXUS_HdmiOutputStatus hdmiStatus;
	NEXUS_CecSettings cecSettings;

	PrintEnter();

	vResult = VK_EVENT_Create(&g_ulCecSyncCallEventId, "HdmiCecEvent");
	if(vResult != VK_OK)
	{
		PrintError("[%s] Creating Hdmi VK_EVENT_Create failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	vResult = VK_SEM_Get(g_hdmi_instance.ulSemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	nexus_rc=NEXUS_HdmiOutput_GetStatus(g_hdmi_instance.hHdmi, &hdmiStatus);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Hdmioutput_GetStatus error!!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	NEXUS_Cec_GetSettings(g_hdmi_instance.hCec, &cecSettings);

	cecSettings.messageReceivedCallback.callback = P_HDMI_CEC_MsgReceived_callback ;
	cecSettings.messageReceivedCallback.context = NULL;

	cecSettings.messageTransmittedCallback.callback = P_HDMI_CEC_MsgTransmitted_callback;
	cecSettings.messageTransmittedCallback.context = NULL;

	cecSettings.logicalAddressAcquiredCallback.callback = P_HDMI_CEC_DeviceReady_callback ;
	cecSettings.logicalAddressAcquiredCallback.context = NULL;

	PrintDebug("CEC Msg Recd Status from Phys Addrs: %X.%X.%X.%X ",
		(cecSettings.physicalAddress[0] & 0xF0) >> 4, (cecSettings.physicalAddress[0] & 0x0F),
		(cecSettings.physicalAddress[1] & 0xF0) >> 4, (cecSettings.physicalAddress[1] & 0x0F)) ;

	cecSettings.physicalAddress[0]= (hdmiStatus.physicalAddressA << 4)
		| hdmiStatus.physicalAddressB;
	cecSettings.physicalAddress[1]= (hdmiStatus.physicalAddressC << 4)
		| hdmiStatus.physicalAddressD;

	/* Enable CEC core */
	cecSettings.enabled = true;

	nexus_rc=NEXUS_Cec_SetSettings(g_hdmi_instance.hCec, &cecSettings);
	if (nexus_rc)
	{
		PrintError("[%s] NEXUS_Cec_SetSettings error!!\n", __func__);
		rc=DI_ERR_ERROR;
		goto done;
	}

	done:

	VK_SEM_Release(g_hdmi_instance.ulSemId);
	PrintExit();

	return rc;


}



