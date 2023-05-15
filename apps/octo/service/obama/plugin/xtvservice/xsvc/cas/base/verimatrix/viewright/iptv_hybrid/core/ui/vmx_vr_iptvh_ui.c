/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_ui.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_param.h>
#include "_xsvc_cas_vmx_iptv_main.h"
#include "_xsvc_cas_vmx_iptvh_ui.h"
#include "_xsvc_cas_vmx_os.h"
#include "vmx_vr_iptvh_mgr.h"
#include "vmx_vr_iptvh_api.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
VmxIptv_DisplayUi_t stDisplayUi;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
STATIC void iptvh_ui_GetMsgName(unsigned long lChannel, eVmDecodeResult eReason)
{
	HUINT8 reason_str[50];
#ifdef CONFIG_DEBUG
	switch( eReason )
	{
		case eVmDecodeInit:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeInit");
			break;
		case eVmDecodeOk:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeOk");
			break;
		case eVmDecodeFailurePinRequired:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeFailurePinRequired");
			break;
		case eVmDecodeFailureKeyNotAvailable:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable");
			break;
		case eVmDecodeFailureBlackout:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeFailureBlackout");
			break;
		case eVmDecodeFailureOther:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeFailureOther");
			break;
		case eVmDecodeFailureTA:
			sprintf(reason_str, "%s", "eVmxIptv_ErrMsg_DecodeFailureTA");
			break;
		default:
			sprintf(reason_str, "%s", "default");
			break;
	}

	HxLOG_Error( "DecodeFailCB on %lu  (0x%08x) for reason %s(%d)\n" , lChannel, lChannel, reason_str, eReason);
#endif
}

STATIC HERROR iptvh_ui_Init(void)
{
	HxSTD_MemSet(&stDisplayUi, 0, sizeof(stDisplayUi));

	return ERR_OK;
}


STATIC void iptvh_ui_UpdateCasMessage(Handle_t hAction, VmxIptv_DisplayType_e eDisplayType, void *pvMessage)
{
	SvcCas_MsgType_e		eMsgType = eVmxIptv_Display_Max;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;
	DxCAS_GroupId_e 		eCasId = eDxCAS_GROUPID_VX;

	HxLOG_Print("[Enter]\n");

	switch(eDisplayType)
	{
		case eVmxIptv_Display_FingerPrinting:
			eMsgType = eCAS_MSG_TYPE_Command;
			eShowType = eCasShowType_Both;
			break;
		case eVmxIptv_Display_OsdMessage:
			eMsgType = eCAS_MSG_TYPE_Command;
			eShowType = eCasShowType_Both;
			break;
		case eVmxIptv_Display_ErrorMessage:
			eMsgType = eCAS_MSG_TYPE_Fail;
			eShowType = eCasShowType_Both;
			break;
		case eVmxIptv_Display_PinDialog:
			eMsgType = eCAS_MSG_TYPE_Fail;
			eShowType = eCasShowType_Both;
			break;
		default:
			HxLOG_Error("Error! Invalid display type (%d)\n", eDisplayType);
			return;
	}

	HxLOG_Error( "hAction(0x%08x), eMsgType(%d)\n" , hAction, eMsgType);

	if (xsvc_cas_vmx_UpdateCasMessage(hAction, eMsgType, eCasId, pvMessage, eShowType) != ERR_OK)
	{
		// error�Ͻ� memory free�ϵ��� ����.
		HxLOG_Error(" error in xsvc_cas_vmx_UpdateCasMessage\n");
	}

	HxLOG_Print("[Exit]\n");
	return;
}

STATIC Handle_t iptvh_ui_GetActionHandleFromUserparam(void *pUserParam, HUINT32 *pulActionId)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pstAction = NULL;
	Handle_t 			 hAction = HANDLE_NULL;
	HUINT32 			 i = 0;

	HxLOG_Print("[Enter]\n");
	if (pulActionId == NULL)
	{
		HxLOG_Error("Error! NULL pulActionId! \n");
		return HANDLE_NULL;
	}

	if ((pUserParam != NULL) && (pstInst != NULL))
	{
		HxLOG_Print("UserParam is exist\n");
		pstAction = (Vmx_ActionTable_t *)pUserParam;

		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			if(pstAction == &pstInst->stActionList[i])
			{
				hAction =  pstInst->stActionList[i].hAction;
				*pulActionId = PAL_PIPE_GetActionId(hAction);
			}
		}
	}
	else
	{
		HxLOG_Error("Error! Invalid param. pUserParam(%p) pstInst(%p)\n", pUserParam, pstInst);
	}

	HxLOG_Print("[Exit]\n");

	return hAction;
}

/*
	The OSD Fingerprinting message controls the insertion of an OSD overlay display on the
	output video of a client device. A client application (middleware or STB firmware subsystem)
	receives this data via a callback function registered through the ViewRight client API.
*/
// TODO_VMX : pFingerPrintingData, pLocalData�� ���ؼ� vmclient���� free���� �ƴϸ� MW���E ���� verimatrix�� �����غ�����.
STATIC void iptvh_ui_FingerPrintingCallback(void *pvUserParam, HINT32 lFingerPrintingdataLength, void *pvFingerPrintingData, HINT32 lLocaldataLength, void *pvLocalData)
{
#if 1
	/**
	  * 		OP���� Finger print�� ������ ������� �ʴ´ٰ� �� ���Ƶд�.
	  *		������ OP�� enable�Ѵٰ� �ص� �Ʒ� ������ �׽�Ʈ�� �� ��� ������ ����ų ������ �ִ�
	  *
	  *		We have no demand from contend owner to have Finger Printing. In the future we maybe decide to enable this feature.
	  *		- 2015.11.20 from Peter Haas
	**/
	return;
#else
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	VmxIptv_Fingerprinting_t *pstFP = NULL;
	Handle_t hAction = HANDLE_NULL;
	HUINT16 nActionId = 0;
	HUINT16 offset = 0;
	HUINT8 *pFormatBuf = NULL;
	HUINT8 *pLocalDataBuf = NULL;
	HUINT16 i;

	HxLOG_Print("[Enter]\n");

	if(pvUserParam == NULL || pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	hAction = iptvh_ui_GetActionHandleFromUserparam(pvUserParam, &nActionId);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	stDisplayUi.hAction = hAction;
	stDisplayUi.eSolution = eVmx_Solution_Iptv_Hybrid;
	stDisplayUi.eType = eVmxIptv_Display_FingerPrinting;
	stDisplayUi.eMode = eVmxIptv_Mode_Timeout;

	// TODO_VMX
	// �Ʒ� �ҽ��� �ӽ÷� ���� ����..���� �׽�Ʈ�� �ҽ� debug �ʿ�
	// �Ʒ� memory�� ���� free�� xmgr���� free���ֵ��� ����.
	pFormatBuf = (HUINT8 *)VMX_MEM_Alloc (sizeof(HUINT8) * lFingerPrintingdataLength);
	pLocalDataBuf = (HUINT8 *)VMX_MEM_Alloc (sizeof(HUINT8) * lLocaldataLength);

	if(pFormatBuf == NULL || pLocalDataBuf== NULL)
	{
		if(pFormatBuf)
		{
		       VMX_MEM_Free(pFormatBuf);
		}
		if(pLocalDataBuf)
		{
			VMX_MEM_Free(pLocalDataBuf);
		}
		return;
	}

	HxSTD_MemSet(pFormatBuf, 0, sizeof(HUINT8) * lFingerPrintingdataLength);
	HxSTD_MemSet(pLocalDataBuf, 0, sizeof(HUINT8) * lLocaldataLength);
	HxSTD_MemCopy(pFormatBuf, pvFingerPrintingData, sizeof(HUINT8) * lFingerPrintingdataLength);
	HxSTD_MemCopy(pLocalDataBuf, pvLocalData, sizeof(HUINT8) * lLocaldataLength);

	pstFP = &stDisplayUi.item.stFingrPrinting;

	if(pvFingerPrintingData && lFingerPrintingdataLength > 0)
	{
		pstFP->usFormatLenght = lFingerPrintingdataLength;

		for(i=0;i<lFingerPrintingdataLength;i++)
		{
			if(i==0)
				pstFP->stFormat.usType = pFormatBuf[offset++];
			else if(i==1)
				pstFP->stFormat.usType = (pstFP->stFormat.usType <<8 | pFormatBuf[offset++]);
			else if(i==2)
				pstFP->stFormat.ucDuration = pFormatBuf[offset++];
			else if(i==3)
				pstFP->stFormat.eQuadrant = pFormatBuf[offset++];
			else if(i==4)
			{
				// reserved for future use : value == 0x00
			}
			else
			{
				HxLOG_Error("\r\n");
				break;
			}
		}
	}

	offset = 0;

	if(pvLocalData && lLocaldataLength > 0)
	{
		VmxIptv_Fingerprinting_LocalData_t *pLD = NULL;
		pstFP->uslLocaldataLength = lLocaldataLength;

		for(i=0;i<3;i++)
		{
			pLD = &pstFP->stLocalData[i];
			pLD->usType = (pLocalDataBuf[offset] << 8 | pLocalDataBuf[offset+1]) & 0xffff;
			offset += 2;
			pLD->ucLength = (pLocalDataBuf[offset] << 8 | pLocalDataBuf[offset+1]) & 0xffff;
			offset += 2;
			pLD->pData = (HUINT8 *)VMX_MEM_Alloc (sizeof(HUINT8) * pLD->ucLength +1);
			if(pLD->pData)
			{
				HxSTD_MemSet(pLD->pData, 0, sizeof(HUINT8) * pLD->ucLength);
				HxSTD_MemCopy(pLD->pData, &pLocalDataBuf[offset], sizeof(HUINT8) * pLD->ucLength);
				offset += pLD->ucLength;
			}
		}
	}

	#if 0 // test only
	stDisplayUi.hAction = hAction;
	stDisplayUi.eType = eVmxIptv_Display_FingerPrinting;

	hAction = xsvc_vmx_main_GetActionHandleFromActionId(0);
	pstFP = &stDisplayUi.item.stFingrPrinting;

	pstFP->uslLocaldataLength = 10;
	pstFP->usFormatLenght = 20;
	pstFP->stFormat.usType = 0x1234;
	pstFP->stFormat.ucDuration = 100;
	pstFP->stFormat.eQuadrant = 40;
	#endif

	iptvh_ui_UpdateCasMessage(hAction, stDisplayUi.eType, (void*)&stDisplayUi);

	HxLOG_Print("[Exit]\n");

	return;
#endif
}

/*
	VCAS can send the OSD message to specific STB(s) global, group, or unique addresses using
	EMM. The ViewRight 3.x Client is only responsible for passing through the OSD message to
	the STB. The STB must register the OSD Message Callback function and implement the
	displaying of this message on-screen. The format of the OSD message should be defined by
	STB vendor and then shared with the Service Operator.
*/
// TODO_VMX : pOsdMessage�� ���ؼ� vmclient���� free���� �ƴϸ� MW���E ���� verimatrix�� �����غ�����.
STATIC void iptvh_ui_OSDMessgaeCallback(void *pvUserParam, HUINT32 ulDataSize, void *pvOsdMessage)
{
#if 1
	// op���� ������� �ʾ� ���Ƶд�
	// �Ʒ� ������ ���� ������ Ȯ�ε� �� ���� �ڵ��̹Ƿ� ��� enable��Ű���� test stream �޾Ƽ� �����ؾ� ��
	return;
#else
	Vmx_Instance_t *pstInst = xsvc_vmx_main_GetInstance();
	Handle_t hAction = HANDLE_NULL;
	HUINT16 nActionId = 0;

	HxLOG_Print("[Enter]\n");

	if(pstInst == NULL || pUserParam == NULL || pOsdMessage == NULL || nDataSize == 0)
	{
		HxLOG_Error("\r\n");
		return;
	}

	hAction = iptvh_ui_GetActionHandleFromUserparam(pUserParam, &nActionId);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	// TODO_VMX : ���� test stream���� test & debug �ʿ�
	// �׸��� OSD Message�� OP�� ������ ��� ������ �������� �����ؾ��Ѵ�.
	stDisplayUi.hAction = hAction;
	stDisplayUi.eSolution = eVmx_Solution_Iptv_Hybrid;
	stDisplayUi.eType = eVmxIptv_Display_OsdMessage;
	stDisplayUi.eMode = eVmxIptv_Mode_User_Exit;

	stDisplayUi.item.stPopupItem.pucMsg= (HUINT8 *)VMX_MEM_Alloc (sizeof(HUINT8) * nDataSize);
	if(stDisplayUi.item.stPopupItem.pucMsg == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	stDisplayUi.item.stPopupItem.usLenght = nDataSize;
	HxSTD_MemSet(stDisplayUi.item.stPopupItem.pucMsg, 0, sizeof(HUINT8) * nDataSize);
	HxSTD_MemCopy(stDisplayUi.item.stPopupItem.pucMsg, pOsdMessage, sizeof(HUINT8) * nDataSize);

	iptvh_ui_UpdateCasMessage(hAction, stDisplayUi.eType, (void*)&stDisplayUi);

	HxLOG_Print("[Exit]\n");
#endif
}

#if 0
HBOOL vmxiptv_ui_IsRatingOK(Handle_t hAction, DxRatingAge_e eRatingSetup)
{
	Vmx_ActionTable_t *pAct = NULL;
	HBOOL bRatingOK = TRUE;
	HUINT8 ucChannelRating = eDxRATING_VIEW_ALL;
	HUINT8 ucChannelRatingLevel;
	HUINT8 ucCurrentRatingLevel;
	HUINT8 ucOride;

	HxLOG_Print("[Enter]\n");

	pAct = xsvc_vmx_main_GetActionByActionHandle(hAction);
	if(pAct == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	vmx_vr_iptvh_api_GetRatingLevel( pAct->pContext, &ucOride, &ucChannelRatingLevel, &ucCurrentRatingLevel);

	#if 1
	// TODO_VMX : ucCurrentRatingLevel�� eRatingSetup�� convert���־���Ѵ�.
	// vmxiptv_ui_ConvertVmxRatingToBaseRating(&eRatingSetup)
	#endif

	if(eRatingSetup == eDxRATING_AGE_LOCK_ALL)
	{
		bRatingOK = FALSE;
	}
	else if(eRatingSetup == eDxRATING_VIEW_ALL)
	{ /* View all���������� ������ �� �� �ִ�. */
		bRatingOK = TRUE;
	}
	else
	{ /* �� �ܿ��� ���ؾ� �ϴµ�... */
		if(ucChannelRating > eDxRATING_AGE_18) /* �� �� eDxRATING_AGE_18 �̻� ���� spec �����̹Ƿ� �ɷ�����. */
		{
			bRatingOK = TRUE;
		}
		else if(ucChannelRating > eRatingSetup) /* Content�� rating�� �¾����� ũ�Ƿ� PIN code check �ؾ� ��. */
		{
			bRatingOK = FALSE;
		}
		else
		{ /* Content�� rating�� �¾����� ���ų� �۴�. OK to watch. */
			bRatingOK = TRUE;
		}
	}

	HxLOG_Print("[Exit]\n");

	return bRatingOK;
}
#endif

void iptvh_ui_DecodeCallback(void *pvUserParam, HULONG ulChannel,  eVmDecodeResult eReason)
{
	Vmx_Instance_t 		*pstInst = xsvc_vmx_main_GetInstance();
	Vmx_ActionTable_t 	*pAct = NULL;
	DxRatingAge_e 		 eParentalSetup = eDxRATING_AGE_LOCK_ALL;
	Handle_t 			 hAction = HANDLE_NULL;
	HUINT32 			 ulActionId = 0;

	HUINT8 				 ucChannelRatingLevel;
	HUINT8 				 ucCurrentRatingLevel;
	HUINT8 				 ucCurrentStbRatingLevel;
	HUINT8 				 ucOride;

	// TODO_VMX

	HxLOG_Print("[Enter]\n");

	iptvh_ui_GetMsgName(ulChannel, eReason);

	if(pvUserParam==NULL || pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return;
	}

	hAction = iptvh_ui_GetActionHandleFromUserparam(pvUserParam, &ulActionId);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("iptvh_ui_GetActionHandleFromUserparam() fail.\n");
		return;
	}

	switch( eReason )
	{
		case eVmDecodeInit:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeInit;
			break;
		case eVmDecodeOk:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeOk;
			break;
		case eVmDecodeFailurePinRequired:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeFailurePinRequired;
			break;
		case eVmDecodeFailureKeyNotAvailable:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeFailureKeyNotAvailable;
			break;
		case eVmDecodeFailureBlackout:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeFailureBlackout;
			break;
		case eVmDecodeFailureOther:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeFailureOther;
			break;
		case eVmDecodeFailureTA:
			stDisplayUi.item.stErrorMsg.eErrMsg = eVmxIptv_ErrMsg_DecodeFailureTA;
			break;
		default:
			HxLOG_Error("Error! Unknown reason (%d) \n", eReason);
			break;
	}

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eParentalSetup, 0) != ERR_OK)
	{
		eParentalSetup = eDxRATING_AGE_LOCK_ALL;
	}

	// TODO_VMX : ���� test stream���� test & debug �ʿ�
	stDisplayUi.hAction = hAction;
	stDisplayUi.eSolution = eVmx_Solution_Iptv_Hybrid;

	HxLOG_Print("* pContext[%d]=(0x%08x), ActType(%d), eParentalSetup(%d)\n", ulActionId, pstInst->stActionList[ulActionId].pContext, pstInst->stActionList[ulActionId].eActType, eParentalSetup);
	HxLOG_Error("actionId(%d) eReason(%d)", ulActionId, eReason);

	pAct = xsvc_vmx_main_GetActionByActionHandle(hAction);

	// vmclient���� decode ok �ΰ��
	if(eReason == eVmDecodeOk)
	{
		return;
	}
	// vmclient���� pin-code�� ��û�ϴ°��
	else if(eReason == eVmDecodeFailurePinRequired)
	{
		//TODO_VMX : ���⼭ ������ eParentalSetup�� ucChannelRatingLevel�� ���ؼ� lock popup�� display���� �����ؼ� �÷�����.
		// ���� video�� ��� ����ϸ� lock base�� �̵��ϵ��� ����.
		if(pAct)
		{
			if(vmx_vr_iptvh_api_OverrideRatingLevel(pAct->pContext, &ucCurrentStbRatingLevel) != ERR_OK)
			{
				HxLOG_Error("vmx_vr_iptvh_api_OverrideRatingLevel() fail. \n");
			}
		}

		// live view all & rec�ΰ�� pin-code�� ��û�� �ʿ䰡 ���� ������ decryption�� ���Ѿ��Ѵ�.
		if(	(pstInst->stActionList[ulActionId].eActType ==eCAS_ActionType_Live && eParentalSetup == eDxRATING_VIEW_ALL) 	||
			(pstInst->stActionList[ulActionId].eActType == eCAS_ActionType_Recording) 										||
			(pstInst->stActionList[ulActionId].eActType == eCAS_ActionType_Tsr_Rec) )
		{
			#if 0
			if(pAct)
			{
				if(vmx_vr_iptvh_api_OverrideRatingLevel(pAct->pContext, &ucCurrentStbRatingLevel) != ERR_OK)
				{
					HxLOG_Error("\r\n");
				}
			}
			#endif
			return;
		}
		else
		{
			if(pAct)
			{
				vmx_vr_iptvh_api_GetRatingLevel( pAct->pContext, &ucOride, &ucChannelRatingLevel, &ucCurrentRatingLevel);
				#if 0
				// TODO_VMX : ucCurrentRatingLevel�� eRatingSetup�� convert���־���Ѵ�.
				// vmxiptv_ui_ConvertVmxRatingToBaseRating(ucChannelRatingLevel, &eConvertedRatingSetup)
				#endif
				#if 0
				// watch tv
				if(vmxiptv_ui_IsRatingOK(hAction, eParentalSetup) == ERR_OK)
				{
					return;
				}
				#endif
				stDisplayUi.item.stErrorMsg.ulValue = ucCurrentRatingLevel;
				stDisplayUi.eType = eVmxIptv_Display_PinDialog;
				//stDisplayUi.eMode = eVmxIptv_Mode_User_Exit;
			}
		}
	}
	else
	{
		stDisplayUi.eType = eVmxIptv_Display_ErrorMessage;
		stDisplayUi.eMode = eVmxIptv_Mode_Always_On_Top;
	}

	iptvh_ui_UpdateCasMessage(hAction, stDisplayUi.eType, (void*)&stDisplayUi);

	HxLOG_Print("[Exit]\n");
}

void vmx_vr_iptvh_ui_FingerPrintingCallback(void *pvUserParam, HINT32 lFingerPrintingdataLength, void *pvFingerPrintingData, HINT32 lLocaldataLength, void *pvLocalData)
{
	iptvh_ui_FingerPrintingCallback(pvUserParam, lFingerPrintingdataLength, pvFingerPrintingData, lLocaldataLength, pvLocalData);
}

void vmx_vr_iptvh_ui_OSDMessgaeCallback(void *pvUserParam, HUINT32 ulDataSize, void *pvOsdMessage)
{
	iptvh_ui_OSDMessgaeCallback(pvUserParam, ulDataSize, pvOsdMessage);
}

void vmx_vr_iptvh_ui_DecodeCallback(void *pvUserParam, HULONG ulChannel,  eVmDecodeResult eReason)
{
	iptvh_ui_DecodeCallback(pvUserParam, ulChannel, eReason);
}

HERROR vmx_vr_iptvh_ui_Init(void)
{
	return iptvh_ui_Init();
}

