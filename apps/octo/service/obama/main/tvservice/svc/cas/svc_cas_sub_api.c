/**
	@file     svc_cas_sub_api.c
	@brief    svc_cas_sub_api.c (CAS Service)

	Description: \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <svc_cas.h>

#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_instance.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_scenario.h>
#include <_svc_cas_mgr_control.h>
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
#include <_svc_cas_mgr_sc.h>
#endif
#include <_svc_cas_sub_api.h>

#include <psvc_cas.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrSub" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrSub_" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

/********************************************************************
* Global Public Functions
* Naming rule : "Module Prefix" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

HERROR svc_cas_MgrSubRegisterCb(SvcCas_InstanceType_e	 eCasInstType,
								DxCAS_GroupId_e			 eCasId,
								HUINT32					 ulSlot,
								SvcCas_CbFuncList_t		*pstCbList,
								HUINT32					 ulNumOfSlot,
								HUINT32					*pulCasInstanceId)
{
	if(pstCbList == NULL || pulCasInstanceId == NULL)
	{
		HxLOG_Error("arg error pstCbList(0x%x), pulCasInstanceId(0x%x)\n", pstCbList, pulCasInstanceId);
		return ERR_FAIL;
	}

	return svc_cas_InstanceRegisterCb(eCasInstType, eCasId, ulSlot, pstCbList, ulNumOfSlot, pulCasInstanceId);
}

/****************************************************************************************
	 Sub CAS에서 MWC_UTIL_PostMsgAP을 통해 바로 AP로 Msg를 날리는 곳들이 있다.
	이는 Cas Mgr에서 관리를 제대로 하지 못하게 하는 요소이다. 이를 추후 제대로
	처리하기 위해 일단 CasMgr에서 받아 보는 Poring 함수를 정의한다.
****************************************************************************************/
/*
	sub cas에서 일반화 되지 않은 다양한 Event를 Manager로 전달할 경우 사용.
*/
HERROR svc_cas_MgrSubUpdateCasEtcEvent (Handle_t		hAction,
										Handle_t		ulCaInstanceId,
										DxCAS_GroupId_e	eCasGrpId,
										HINT32			nMsg,
										HINT32			p1,
										HINT32			p2,
										HINT32			p3)
{
	svc_cas_SvcPortUtilPostMsgToAp(nMsg, hAction, p1, p2, p3);

	return ERR_OK;
}

HERROR svc_cas_MgrSubUpdateCasEtcEventData (Handle_t		hAction,
										Handle_t		ulCaInstanceId,
										DxCAS_GroupId_e	eCasGrpId,
										HINT32			nMsg,
										HINT32			p1,
										HINT32			p2,
										void				*pData,
										HUINT32			ulSize)
{
	svc_cas_SvcPortUtilPostDataToAp(nMsg, hAction, p1, p2, pData, ulSize);

	return ERR_OK;
}

HERROR svc_cas_MgrSubUpdateCasInfo(Handle_t			hCasInstanceId,
									DxCAS_GroupId_e	eCasGroupId,
									HUINT16			usSlotId,
									HUINT16			*pusCaSysId,
									HUINT32			ulNumCaSysId,
									HUINT32			ulMaxNumSvc,
									HBOOL			bAvailable)
{
	HUINT32			nResult = 0;
	SvcCas_Message_t	casMsg;

	if (hCasInstanceId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg.checkDev, 0, sizeof(SvcCas_CheckDeviceMsg_t));

	casMsg.checkDev.command			= CMD_CAS_CHECKDEVICE;
	casMsg.checkDev.eEvtType		= eCAS_DEV_EVT_CasInfo;
	casMsg.checkDev.eCasGroupId		= eCasGroupId;
	casMsg.checkDev.usSlotId		= usSlotId;
	casMsg.checkDev.ulCaInstanceId	= hCasInstanceId;

	casMsg.checkDev.utData.stCasInfo.ulMaxNumSvc	= ulMaxNumSvc;
	casMsg.checkDev.utData.stCasInfo.ulNumCaSysId	= ulNumCaSysId;
	casMsg.checkDev.utData.stCasInfo.bAvailable	= bAvailable;
	VK_memcpy32(&casMsg.checkDev.utData.stCasInfo.ausCaSysId, pusCaSysId, ulNumCaSysId * sizeof(HUINT16));

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

#if defined(CONFIG_MW_CI)
HERROR svc_cas_MgrSubUpdateCamState(HUINT16 usSlotId, SvcCas_CamState_e eCamState)
{
	HUINT32			nResult = 0;
	SvcCas_Message_t	casMsg;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d) wrong slot id \n", usSlotId);
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg.checkDev, 0, sizeof(SvcCas_CheckDeviceMsg_t));
	casMsg.checkDev.command 			= CMD_CAS_CHECKDEVICE;
	casMsg.checkDev.eEvtType			= eCAS_DEV_EVT_CamState;
	casMsg.checkDev.utData.eCamState	= eCamState;
	casMsg.checkDev.usSlotId			= usSlotId;

	HxLOG_Info("Send CMD_CAS_CHECKDEVICE (eCAS_DEV_EVT_CamState) Message \n");

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR svc_cas_MgrSubUpdateCamAppinfo(HUINT16 usSlotId, SvcCas_CiAppInfo_t *pstCasCiAppInfo)
{
	HUINT32 			nResult = 0;
	SvcCas_Message_t	casMsg;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d) wrong slot id \n", usSlotId);
		return ERR_FAIL;
	}

	#if defined(CONFIG_DEBUG)
	HxLOG_Print("============================================\n");
	HxLOG_Print("NotifyAppInfo\n");
	HxLOG_Print("\t nType = 0x%02X\n", pstCasCiAppInfo->eCiAppType);
	HxLOG_Print("\t usManufacturer = 0x%04X\n", pstCasCiAppInfo->usManufacturer);
	HxLOG_Print("\t usManufacturerCode = 0x%04X\n", pstCasCiAppInfo->usManufacturerCode);
	HxLOG_Print("\t szText = %s\n", pstCasCiAppInfo->szText);
	HxLOG_Print("\t ucIsInitialized = %d\n", pstCasCiAppInfo->ucIsInitialized);
	HxLOG_Print("============================================\n");
	#endif

	HxSTD_memset(&casMsg.checkDev, 0, sizeof(SvcCas_CheckDeviceMsg_t));
	casMsg.checkDev.command 			= CMD_CAS_CHECKDEVICE;
	casMsg.checkDev.eEvtType			= eCAS_DEV_EVT_CamAppInfo;
	casMsg.checkDev.usSlotId			= usSlotId;
	casMsg.checkDev.utData.stCasCiAppInfo	= *pstCasCiAppInfo;

	HxLOG_Info("Send CMD_CAS_CHECKDEVICE (eCAS_DEV_EVT_CamAppInfo) Message \n");

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR svc_cas_MgrSubUpdateMultiscreen(Handle_t			 hAction,
									SvcCas_MsgType_e	 eMsgType,
									DxCAS_GroupId_e		 eCasId,
									void				*pvMessage,
									SvcCas_ShowType_e	 eShowType)
{
	HUINT32 nResult = 0;
	SvcCas_Message_t casMsg;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if ( eMsgType >= eCAS_MSG_TYPE_Max || eMsgType == eCAS_MSG_TYPE_None)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg.checkMsg, 0, sizeof(SvcCas_CheckSubCasMsg_t));

	casMsg.checkMsg.command		= CMD_CAS_MULTISCREEN;
	casMsg.checkMsg.hAction		= hAction;
	casMsg.checkMsg.eCasId		= eCasId;
	casMsg.checkMsg.eMsgType	= eMsgType;
	casMsg.checkMsg.pMsg		= pvMessage;
	casMsg.checkMsg.eShowType	= eShowType;

	// 필요하면, Table Id, Unique Id 등도 보내자.
	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR svc_cas_MgrSubUpdateCasMessage(Handle_t			 hAction,
									SvcCas_MsgType_e	 eMsgType,
									DxCAS_GroupId_e		 eCasId,
									void				*pvMessage,
									SvcCas_ShowType_e	 eShowType)
{
	HUINT32 nResult = 0;
	SvcCas_Message_t casMsg;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if ( eMsgType >= eCAS_MSG_TYPE_Max || eMsgType == eCAS_MSG_TYPE_None)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg.checkMsg, 0, sizeof(SvcCas_CheckSubCasMsg_t));

	casMsg.checkMsg.command		= CMD_CAS_CHECKEVENT;
	casMsg.checkMsg.hAction		= hAction;
	casMsg.checkMsg.eCasId		= eCasId;
	casMsg.checkMsg.eMsgType	= eMsgType;
	casMsg.checkMsg.pMsg		= pvMessage;
	casMsg.checkMsg.eShowType	= eShowType;

	// 필요하면, Table Id, Unique Id 등도 보내자.
	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/* sub cas에서 drm info를 전달.
	void *pData : Embedded Cas or CI Plus 등 개별 Sub Cas 단에서 정의한 Data Structure의 Pointer
*/
HERROR svc_cas_MgrSubUpdateDrmInfo(	Handle_t		 hAction,
									Handle_t		 hCaInstanceId,
									void			*pvData,
									DxCopyrightControl_CasCcType_e	 eDrmType,
									HUINT32			 ulVersion)
{
	HUINT32 nResult = 0;
	SvcCas_Message_t casMsg;

	if (hCaInstanceId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg.checkDrm, 0, sizeof(SvcCas_CheckDrmInfo_t));

	casMsg.checkDrm.command = CMD_CAS_CHECKDRM;
	casMsg.checkDrm.hAction = hAction;
	casMsg.checkDrm.ulCaInstIdx = hCaInstanceId;
	casMsg.checkDrm.pData = pvData;
	casMsg.checkDrm.eDrmType = eDrmType;
	casMsg.checkDrm.version = ulVersion;

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_MgrSubUpdateMmi(SvcCas_MmiDataType_e eDataType, HUINT16 usSlotId, HUINT32 ulVersion)
{
	SvcCas_Message_t 	casMsg;
	HUINT32 			nResult = 0;

	HxSTD_memset(&casMsg, 0x00, sizeof(SvcCas_Message_t));

	casMsg.checkMmi.command = CMD_CAS_CHECKMMI;
	casMsg.checkMmi.eDataType = eDataType;
	casMsg.checkMmi.usSlotId = usSlotId;
	casMsg.checkMmi.ulMmiVer = ulVersion;

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*
	inline api, task간 sync 문제 발생 가능.
*/
SvcCas_PmtInfo_t *svc_cas_MgrSubGetPmtInfo(Handle_t hAction)
{
	SvcCas_Context_t *pCasContext = NULL;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("\n");
		return NULL;
	}

	pCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (pCasContext == NULL)
	{
		HxLOG_Error("Context Null\n");
		return NULL;
	}

	return &pCasContext->stPmtInfo;
}

HERROR svc_cas_MgrSubGetPmtSectionState(Handle_t hAction, HUINT32 *pulPmtState)
{
	SvcCas_Context_t *pCasContext = NULL;

	pCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (pCasContext == NULL)
	{
		HxLOG_Error("Context Null\n");
		return ERR_FAIL;
	}

	*pulPmtState = pCasContext->stPmtInfo.ulSectionState;

	return ERR_OK;
}

HERROR svc_cas_MgrSubGetSiTableState(Handle_t hAction, SiTableType_e eType, SvcCas_SiTableState_e *peState)
{
	SvcCas_Context_t *pCasContext = NULL;

	if (peState == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (pCasContext == NULL)
	{
		HxLOG_Error("Context Null\n");
		return ERR_FAIL;
	}

	switch (eType)
	{
	case eSI_TableType_PMT:	*peState = pCasContext->ePmtState;	return ERR_OK;

	#if defined (CAS_SUPPORT_CAT)
	case eSI_TableType_CAT:	*peState = pCasContext->eCatState;	return ERR_OK;
	#endif

	#if defined (CAS_SUPPORT_SDT)
	case eSI_TableType_SDT_ACT:	*peState = pCasContext->eSdtState; return ERR_OK;
	#endif

	#if defined (CAS_SUPPORT_NIT)
	case eSI_TableType_NIT_ACT:	*peState = pCasContext->eNitState; return ERR_OK;
	#endif

	default:	return ERR_FAIL;
	}

}

HERROR svc_cas_MgrSubNotifyRecordOperation(Handle_t hAction, DxCAS_GroupId_e eCasId, HUINT8 ucRecStatus, HUINT16 usSlotId)
{
	HUINT32 nResult = 0;
	SvcCas_Message_t casMsg;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}
	HxSTD_memset(&casMsg.notifyRecOperation, 0, sizeof(SvcCas_NotifyRecordOperationMsg_t));
	casMsg.notifyRecOperation.command		= CMD_CAS_NOTIREC;
	casMsg.notifyRecOperation.hAction		= hAction;
	casMsg.notifyRecOperation.eCasId		= eCasId;
	casMsg.notifyRecOperation.ucRecStatus 	= ucRecStatus;
	casMsg.notifyRecOperation.usSlotId 		= usSlotId;

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

SvcCas_InstanceType_e svc_cas_MgrSubGetCasDevInstType(HUINT32 ulCasInstId)
{
	return svc_cas_InstanceGetCasDevInstType(ulCasInstId);
}
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
HERROR svc_cas_MgrSubSendScCmd2MgrTask(DxCAS_GroupId_e eGroupId, HUINT32 ulCommand, HUINT32 ulMsgKind, HUINT8* pDataToSend)
{
	return svc_cas_ScSendCmd2MgrTask(eGroupId, ulCommand, ulMsgKind, pDataToSend);
}
#endif
/********************************************************
아래 부터는 control 관련..  함수들임~

**********************************************************/

HERROR svc_cas_MgrSubInitControl(DxCAS_GroupId_e eCasId)
{
	return svc_cas_CtrlInit(eCasId);
}

HERROR svc_cas_MgrSubRegisterControl(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList)
{
	return svc_cas_CtrlRegister(eCasId, ulNum, pstCasCtrlList);
}

/********************************************************
아래 부터는 시나리오 관련..  함수들임~

**********************************************************/

/*	등록된 순서대로 우선 순위가 매겨진다.
*/
HERROR svc_cas_MgrSubRegisterPriorityTable(SvcCas_GroupOfPriority_t *pstGroupOfPriority)
{
	return svc_cas_ScenarioRegisterPriorityTable(pstGroupOfPriority);
}

HERROR svc_cas_MgrSubIsExistDevice(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_InstanceType_e *peCasInstType)
{
	return svc_cas_ScenarioIsExistDevice(pstPriorityTree, peCasInstType);
}

SvcCas_GroupOfPriority_t *svc_cas_MgrSubGetPriorityTable(HUINT32 unTableId)
{
	return svc_cas_ScenarioGetPriorityTable(unTableId);
}

HERROR	svc_cas_MgrSubIsCasSystemIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, void *pstCasContext)
{
	return svc_cas_ScenarioIsCasSystemIdMatch(pstItem, (SvcCas_Context_t *)pstCasContext);
}

HERROR svc_cas_MgrSubIsCamIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, void *pstCasContext)
{
	return svc_cas_ScenarioIsCamIdMatch(pstItem, (SvcCas_Context_t *)pstCasContext);
}

void *svc_cas_MgrSubGetActionContextByActionId(HUINT32 ulActionId)
{
	return (void *)svc_cas_MgrGetActionContextByActionId(ulActionId);
}

DxCAS_GroupId_e svc_cas_MgrSubGetCasGroupIdFromCasInst(HUINT32 ulCasInstId)
{
	return svc_cas_ScenarioGetCasGroupIdFromCasInst(ulCasInstId);
}

HERROR svc_cas_MgrSubIsDeviceMatchItem(SvcCas_MgrPriorityTreeItem_t *pstFirstItem, SvcCas_ItemOfPriority_e eItem)
{
	return svc_cas_ScenarioIsDeviceMatchItem(pstFirstItem, eItem);
}

HERROR svc_cas_MgrSubIsSameDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_InstanceType_e eCasInstType, HUINT32 ulSlot)
{
	return svc_cas_ScenarioIsSameDefSlot(pstItem, eCasInstType, ulSlot);
}

HUINT32  svc_cas_MgrSubGetLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree)
{
	return svc_cas_ScenarioGetLargestResourceCount(pstPriorityTree);
}

HUINT32	svc_cas_MgrSubGetNumOfLessItemFromLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum)
{
	return svc_cas_ScenarioGetNumOfLessItemFromLargestResourceCount(pstItem, ulLargestNum);
}

HERROR svc_cas_MgrSubIsLessThenInputArg(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum)
{
	return svc_cas_ScenarioIsLessThenInputArg(pstItem, ulLargestNum);
}

void svc_cas_MgrSubMakePTFromGSlotByDB(SvcCas_MgrPriorityTreeMgr_t	*pstTreeMgr,
									SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
									HUINT32							 ulPriorityTableId,
									HUINT32							 ulReqActionId)
{
	svc_cas_ScenarioMakePTFromGSlotByDB(pstTreeMgr, pstFirstItem, ulPriorityTableId, ulReqActionId);
}

void svc_cas_MgrSubMakePTFromGKindOfAction(	SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32							 ulPriorityTableId,
											HUINT32							 ulReqActionId)
{
	return svc_cas_ScenarioMakePTFromGKindOfAction(pstTreeMgr, pstFirstItem, ulPriorityTableId, ulReqActionId);
}

HERROR svc_cas_MgrSubGetRequestedCamIdByActionId(HUINT32 ulActionId, HUINT8 *pucCamId)
{
	return svc_cas_MgrGetRequestedCamIdByActionId(ulActionId, pucCamId);
}

HUINT32 svc_cas_MgrUtilStrLen(const HUINT8 *pszStr)
{
	if (pszStr == NULL) return 0;

	return HxSTD_StrLen(pszStr);
}

HERROR svc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen)
{
	return psvc_cas_SendCaPmt( usSlotId,pucCaPmt, usCaPmtLen);
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___



/*********************** End of File ******************************/
