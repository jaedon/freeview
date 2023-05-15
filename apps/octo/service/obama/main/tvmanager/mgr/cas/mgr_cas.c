/**
@file			mgr_cas.c
@brief			MW CAS EVENT를 처리 전담할 procedure

Description:	MW CAS 보다 먼저 생성되어 EVENT를 놓치지 말아야 하며 항상 생성되어 있어야 한다. ==> 현재는 MW_INIT ==> AP_INIT인데 추후 정리하면서 생성 위치를 조정할 것
				(Standby 상태 등에서도 EVENT는 처리되어야 할 것들이 있다.)
				여기서 공통적인 CAS EVENT는 처리되며 specific한 것들은 ap_cas_xxx_mgr.c (xxx는 각 Sub CAS)에서 처리될 것이다.
				UI 처리의 경우 OCTO2 외부로 따로 분리될 것이어서 일단 UI EVENT들도 여기에 두고 점차 수정해 나갈 것이다.
				여기에 더해 PPV / PVR / DRM 관련 EVENT 들도 성격이 명확히 분리되어 각 Sub CAS 처리 경우 처럼 따로 Procedure로
				처리할 것이다. 현재는 ap_cas_mgr에서 이를 생성하도록 할 것이나 추후에는 ap_cas_mgr와 같은 level이 될 수도 있다.
				(OCTO2 진행 사항에 따라 결정될 것)
Module: AP/CAS/MMI			 	\n

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    **************************/
/*******************************************************************/
#include <octo_common.h>

#include <bus.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>
#include <pmgr_cas.h>


/*******************************************************************/
/********************      Global Variables     *********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/



#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define INFO_LOG			HxLOG_Info
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message
#define	FuncENTER			HxLOG_Info("(+)Enter!!\n");
#define	FuncLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	FuncENTER			OCTO_NULL_PRINT
#define	FuncLEAVE(X)		OCTO_NULL_PRINT
#endif


/* local macros using in proc msg  */
#define CASMGR_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

#define CASMGR_POINTER_INVALID_CHECK(pArg, returnValue)			\
			if(pArg == NULL)									\
			{													\
				ERR_LOG("\n\n");									\
				return returnValue;								\
			}

#define CASMGR_HERROR_INVALID_CHECK(pArg, returnValue)				\
			if(pArg != ERR_OK)									\
			{													\
				ERR_LOG("\n\n");									\
				return returnValue;								\
			}

#define CHECK_ACTION_ID()	\
	if(context->ulActionId != ulActionId) \
	{ \
		return MESSAGE_PASS; \
	}

#define CHECK_ACTION_HANDLE() \
	CHECK_ACTION_ID() \
	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK) \
	{ \
		HxLOG_Print("MESSAGE[0x%08X](0x%08x) is expired. Expected 0x%08X\n", message, hAction, context->hAction);\
		return MESSAGE_PASS; \
	}

#if 1 //octo2
/*******************************************************************/
/********************      	Global Prototype		********************/
/*******************************************************************/
/* temp */
extern HERROR	OM_CAS_MMI_SendCasEventDataToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32	ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, void *pData, HUINT32 ulSize);
extern HERROR	OM_CAS_MMI_SendCasEventToCasUiProc(DxCAS_GroupId_e eCasGrpId, HUINT32	ulSlotNum, Handle_t hAction, HINT32 nMessage, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined (CONFIG_DEBUG)
static void CMD_Register_MGR_CAS(void);
#endif

/*******************************************************************/
/********************      	Local Prototype		********************/
/*******************************************************************/


/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/
STATIC BUS_Result_t mgr_cas_EvtCasOkReceived(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		nRet;
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p1;
	HUINT32		ulSlot = 0/*p2*/; //p2로 현재 다른 data가 옴.

	nRet = OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_OK,p1, p2, p3);
	if (ERR_OK != nRet)
	{
		ERR_LOG("\nOM_CAS_MMI_SendCasEventToCasUiProc ret:0x%X\n", nRet);
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasFailReceived(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		nRet;
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p1;
	HUINT32		ulSlot = 0/*p2*/; //p2로 현재 다른 data가 옴.

	nRet = OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_FAIL,p1, p2, p3);
	if (ERR_OK != nRet)
	{
		ERR_LOG("\nOM_CAS_MMI_SendCasEventToCasUiProc ret:0x%X\n", nRet);
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasCmdReceived(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		nRet;
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p1;
	HUINT32		ulSlot = 0/*p2*/; //p2로 현재 다른 data가 옴.

	nRet = OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_CMD,p1, p2, p3);
	if (ERR_OK != nRet)
	{
		ERR_LOG("\nOM_CAS_MMI_SendCasEventToCasUiProc ret:0x%X\n", nRet);
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasCheckReceived(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		nRet;
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p1;
	HUINT32		ulSlot = 0/*p2*/; //p2로 현재 다른 data가 옴.

	nRet = OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_CHECK,p1, p2, p3);
	if (ERR_OK != nRet)
	{
		ERR_LOG("\nOM_CAS_MMI_SendCasEventToCasUiProc ret:0x%X\n", nRet);
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasCamChangeStatus(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;
	HINT32		nMgrMessage;

	switch(message)
	{
		case eSEVT_CAS_CAM_INSERTED:
			nMgrMessage = eMEVT_CAS_CAM_INSERTED;
			break;
		case eSEVT_CAS_CAM_REMOVED:
			nMgrMessage = eMEVT_CAS_CAM_REMOVED;
			break;
		case eSEVT_CAS_CAM_INITIALIZED:
			nMgrMessage = eMEVT_CAS_CAM_INITIALIZED;
			break;
		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	BUS_PostMessageToOM(NULL, eMEVT_SYSCTL_CI_CAM_STATUS, HANDLE_NULL, 0, 0, 0);

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, nMgrMessage, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

#if defined(CONFIG_OP_SES)
	// cam이 제거된경우 msg를 display해주기위해 nagra manager로 msg를 보낸다.
	if(eCasGrpId == eDxCAS_GROUPID_CAM && nMgrMessage == eMEVT_CAS_CAM_REMOVED)
	{
		if(OM_CAS_MMI_SendCasEventToCasUiProc(eDxCAS_GROUPID_NA, 0, hAction, nMgrMessage, p1, p2, p3) != ERR_OK)
		{
			ERR_LOG("\n\n");
		}
	}
#endif

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasMmiMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;
	HINT32		nMgrMessage;

	switch(message)
	{
		case eSEVT_CAS_MMI_OPEN:
			nMgrMessage = eMEVT_CAS_MMI_OPEN;
			break;
		case eSEVT_CAS_MMI_TEXT:
			nMgrMessage = eMEVT_CAS_MMI_TEXT;
			break;
		case eSEVT_CAS_MMI_ENQ:
			nMgrMessage = eMEVT_CAS_MMI_ENQ;
			break;
		case eSEVT_CAS_MMI_MENU:
			nMgrMessage = eMEVT_CAS_MMI_MENU;
			break;
		case eSEVT_CAS_MMI_LIST:
			nMgrMessage = eMEVT_CAS_MMI_LIST;
			break;
		case eSEVT_CAS_MMI_CLOSE:
			nMgrMessage = eMEVT_CAS_MMI_CLOSE;
			break;
		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, nMgrMessage, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

#if 0
STATIC BUS_Result_t mgr_cas_EvtCasDecodeMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulActionId	= SVC_PIPE_GetActionId(hAction);

	switch(message)
	{
		case eSEVT_CAS_DECODE_RESET_IRD:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESET_IRD\n");
			break;
		case eSEVT_CAS_DECODE_RESET_CH_ORDER:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESET_CH_ORDER\n");
			break;
		case eSEVT_CAS_DECODE_RESCAN:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESCAN\n");
			break;
		case eSEVT_CAS_DECODE_RESCAN_FROM_HOME:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESCAN_FROM_HOME\n");
			break;
		case eSEVT_CAS_DECODE_RECONNECT:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RECONNECT\n");
			break;
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t mgr_cas_EvtCasCamTune(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, void *pvData)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;
	SvcCas_TuneInfo_t stTuneInfo;

	HLIB_STD_MemCpySafe((void *)&stTuneInfo, sizeof(SvcCas_TuneInfo_t), (const void *)pvData, sizeof(SvcCas_TuneInfo_t));
	if(OM_CAS_MMI_SendCasEventDataToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_CAM_TUNE, p1, p2, &stTuneInfo, sizeof(SvcCas_TuneInfo_t)) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}


	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, pvData);
	return ERR_BUS_MESSAGE_PASS;
}


STATIC BUS_Result_t mgr_cas_EvtCasCiPlusUpgrade(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_CIPLUS_UPGRADE, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasCiPlusCcReport (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_CIPLUS_CC_REPORT, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasCiPlusOperatorProfile (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HINT32		nMgrMessage;
	HUINT32 	ulSlot = p1;

	switch(message)
	{
		case eSEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
			nMgrMessage = eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ;
			break;
		case eSEVT_CAS_CIPLUS_OPERATOR_TUNE:
			nMgrMessage = eMEVT_CAS_CIPLUS_OPERATOR_TUNE;
			break;
		case eSEVT_CAS_CIPLUS_OPERATOR_NIT:
			nMgrMessage = eMEVT_CAS_CIPLUS_OPERATOR_NIT;
			break;
		default:
			return ERR_BUS_MESSAGE_PASS;
	}
	if(nMgrMessage == eMEVT_CAS_CIPLUS_OPERATOR_TUNE)
	{
		SvcCas_OperatorTune_t	stOpTune;

		HLIB_STD_MemCpySafe(&stOpTune, sizeof(SvcCas_OperatorTune_t), (const void *)p3, sizeof(SvcCas_OperatorTune_t));
		if(OM_CAS_MMI_SendCasEventDataToCasUiProc(eCasGrpId, ulSlot, hAction, nMgrMessage, p1, p2, &stOpTune, sizeof(SvcCas_OperatorTune_t)) != ERR_OK)
		{
			ERR_LOG("\n\n");
		}
	}
	else
	{
		if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, nMgrMessage, p1, p2, p3) != ERR_OK)
		{
			ERR_LOG("\n\n");
		}
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasPpvRequest(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_PPV_REQUEST, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasPinCodeRequest (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_PINCODE_REQUEST, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasSystemInfoUpdated (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_SYSINFO_UPDATED, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasUserDataRequest (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_UDATAREAD_REQUEST, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasScInserted(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_SC_INSERTED, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasScRemoved(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_SC_REMOVED, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasDrm(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
//	HUINT32		ulSlot = p1;
//	HINT32		nMgrMessage;
//	HERROR		nRet;

#if 0	// 현재 DRM 처리하는데 있어서 직접 UI 관련 내용을 수행 할 작업은 없음.
	switch(message)
	{
		case eSEVT_CAS_DRM_SIGNALLED:
			nMgrMessage = eMEVT_CAS_DRM_SIGNALLED;
			break;
		case eSEVT_CAS_DRM_UNKNOWN:
			nMgrMessage = eMEVT_CAS_DRM_UNKNOWN;
			break;
		case eSEVT_CAS_DRM_NONE:
			nMgrMessage = eMEVT_CAS_DRM_NONE;
			break;
 		default:
			return ERR_BUS_MESSAGE_PASS;
	}

	nRet = OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, hAction, nMgrMessage, p1, p2, p3);
	if (ERR_OK != nRet)
	{
		ERR_LOG("OM_CAS_MMI_SendCasEventToCasUiProc ret:0x%X\n",nRet);
	}
#endif
	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasDmv(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;
	HINT32		nMgrMessage;

	switch(message)
	{
	case eSEVT_CAS_DMV_SIGNALLED:
		nMgrMessage = eMEVT_CAS_DMV_SIGNALLED;
		break;
	case eSEVT_CAS_DMV_UNKNOWN:
		nMgrMessage = eMEVT_CAS_DMV_UNKNOWN;
		break;
	case eSEVT_CAS_DMV_NONE:
		nMgrMessage = eMEVT_CAS_DMV_NONE;
		break;
		default:
		return ERR_BUS_MESSAGE_PASS;
	}

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, nMgrMessage, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasUpdateKeyInfo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;
//	HINT32		nMgrMessage;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_UPDATE_KEYINFO, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t mgr_cas_EvtCasPrmPvrUpdateKeyInfo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasGrpId = (DxCAS_GroupId_e)p2;
	HUINT32		ulSlot = p1;

	if(OM_CAS_MMI_SendCasEventToCasUiProc(eCasGrpId, ulSlot, hAction, eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO, p1, p2, p3) != ERR_OK)
	{
		ERR_LOG("\n\n");
	}

	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

#if defined(CONFIG_DEBUG)
static HUINT8 *mgr_cas_GetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eSEVT_CAS_OK);
		ENUM_TO_STR(eSEVT_CAS_FAIL);
		ENUM_TO_STR(eSEVT_CAS_CMD);
		ENUM_TO_STR(eSEVT_CAS_CHECK);

		ENUM_TO_STR(eSEVT_CAS_MMI_OPEN);
		ENUM_TO_STR(eSEVT_CAS_MMI_CLOSE);
		ENUM_TO_STR(eSEVT_CAS_MMI_TEXT);
		ENUM_TO_STR(eSEVT_CAS_MMI_ENQ);
		ENUM_TO_STR(eSEVT_CAS_MMI_MENU);
		ENUM_TO_STR(eSEVT_CAS_MMI_LIST);

		ENUM_TO_STR(eSEVT_CAS_CAM_REMOVED);
		ENUM_TO_STR(eSEVT_CAS_CAM_INSERTED);
		ENUM_TO_STR(eSEVT_CAS_CAM_INITIALIZED);
		ENUM_TO_STR(eSEVT_CAS_CAM_TUNE);
		ENUM_TO_STR(eSEVT_CAS_CIPLUS_CC_REPORT);
		ENUM_TO_STR(eSEVT_CAS_CIPLUS_UPGRADE);

		ENUM_TO_STR(eSEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ);
		ENUM_TO_STR(eSEVT_CAS_CIPLUS_OPERATOR_TUNE);
		ENUM_TO_STR(eSEVT_CAS_CIPLUS_OPERATOR_NIT);

		ENUM_TO_STR(eSEVT_CAS_PPV_REQUEST);
		//ENUM_TO_STR(eSEVT_CAS_PPV_REPLY); //manager --> service
		ENUM_TO_STR(eSEVT_CAS_PINCODE_REQUEST);
		//ENUM_TO_STR(eSEVT_CAS_PINCODE_REPLY); //manager --> service
		ENUM_TO_STR(eSEVT_CAS_UDATAREAD_REQUEST);
		//ENUM_TO_STR(eSEVT_CAS_UDATAREAD_REPLY); //manager --> service

		ENUM_TO_STR(eSEVT_CAS_SYSINFO_UPDATED);
		//ENUM_TO_STR(eSEVT_CAS_DECODE_FORCE_TUNE);		//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_PIN);			//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_IRD);			//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_CH_ORDER);	//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RESCAN);			//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RESCAN_FROM_HOME);	//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_DECODE_RECONNECT);			//현재는 manager-->manager 로 사용됨 (For Irdeto)
		//ENUM_TO_STR(eSEVT_CAS_IR_FTA_BLOCK_CHECK);		//현재는 manager-->manager 로 사용됨 (For Irdeto)

		ENUM_TO_STR(eSEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eSEVT_CAS_SC_INSERTED);

		ENUM_TO_STR(eSEVT_CAS_DRM_SIGNALLED);
		ENUM_TO_STR(eSEVT_CAS_DRM_UNKNOWN);
		ENUM_TO_STR(eSEVT_CAS_DRM_NONE);

		ENUM_TO_STR(eSEVT_CAS_DMV_SIGNALLED);
		ENUM_TO_STR(eSEVT_CAS_DMV_UNKNOWN);
		ENUM_TO_STR(eSEVT_CAS_DMV_NONE);
		ENUM_TO_STR(eSEVT_CAS_NOTIFICATION);
		ENUM_TO_STR(eSEVT_CAS_UPDATE_KEYINFO);
		ENUM_TO_STR(eSEVT_CAS_PRM_PVR_UPDATE_KEYINFO);
		default:
			return NULL;
	}
}

static HUINT8 *mgr_cas_GetUnProcMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eSEVT_CAS_PPV_REPLY); //manager --> service
		ENUM_TO_STR(eSEVT_CAS_PINCODE_REPLY); //manager --> service
		ENUM_TO_STR(eSEVT_CAS_UDATAREAD_REPLY); //manager --> service
		ENUM_TO_STR(eSEVT_CAS_DECODE_FORCE_TUNE);		//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_PIN);			//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_IRD);			//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RESET_CH_ORDER);	//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RESCAN);			//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RESCAN_FROM_HOME);	//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_DECODE_RECONNECT);			//현재는 manager-->manager 로 사용됨 (For Irdeto).
		ENUM_TO_STR(eSEVT_CAS_IR_FTA_BLOCK_CHECK);		//현재는 manager-->manager 로 사용됨 (For Irdeto).
		default:
			return NULL;

	}
}
#endif /* defined(CONFIG_DEBUG) */

/********************************************************************/
/********************      Global Functions	*************************/
/********************************************************************/
STATIC BUS_Result_t proc_CAS(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eGwmResult	= ERR_BUS_NO_ERROR;

#ifdef CONFIG_DEBUG
	HUINT32			ulActionId	= SVC_PIPE_GetActionId(hAction);
	{
		HUINT8 *pMsg = mgr_cas_GetMessageName(message);

		if(pMsg != NULL)
		{
			HxLOG_Info("message(%s), ActId(%x), p1(%x), p2(%x), p3(%x)\n", pMsg, ulActionId, p1, p2, p3);
		}
	}
#endif /* CONFIG_DEBUG */

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("MESSAGE : eMEVT_BUS_CREATE \n");
			eGwmResult = pmgr_cas_CreateSubCasProc(message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("MESSAGE : eMEVT_BUS_DESTROY \n");
			break;

		/* standby에서도 떠 있어야 하므로 그냥 PASS */
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;

		/**********************************************
			CAS service로부터의 시청 허가 이벤트
		***********************************************/
		case eSEVT_CAS_OK:
			eGwmResult = mgr_cas_EvtCasOkReceived(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_FAIL:
			eGwmResult = mgr_cas_EvtCasFailReceived(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CMD :
			eGwmResult = mgr_cas_EvtCasCmdReceived(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK:
			eGwmResult = mgr_cas_EvtCasCheckReceived(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_MMI_OPEN:
		case eSEVT_CAS_MMI_TEXT:
		case eSEVT_CAS_MMI_ENQ:
		case eSEVT_CAS_MMI_MENU:
		case eSEVT_CAS_MMI_LIST:
		case eSEVT_CAS_MMI_CLOSE:
			eGwmResult = mgr_cas_EvtCasMmiMessage(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CAM_INSERTED:
		case eSEVT_CAS_CAM_REMOVED:
		case eSEVT_CAS_CAM_INITIALIZED:
			eGwmResult = mgr_cas_EvtCasCamChangeStatus(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CAM_TUNE:
			eGwmResult = mgr_cas_EvtCasCamTune(message, hAction, p1, p2, (void *)p3);
			break;

		case eSEVT_CAS_CIPLUS_UPGRADE:
			eGwmResult = mgr_cas_EvtCasCiPlusUpgrade(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CIPLUS_CC_REPORT:
			eGwmResult = mgr_cas_EvtCasCiPlusCcReport(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
		case eSEVT_CAS_CIPLUS_OPERATOR_TUNE:
		case eSEVT_CAS_CIPLUS_OPERATOR_NIT:
			eGwmResult = mgr_cas_EvtCasCiPlusOperatorProfile(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_PPV_REQUEST:
			eGwmResult = mgr_cas_EvtCasPpvRequest(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_PINCODE_REQUEST:
			eGwmResult = mgr_cas_EvtCasPinCodeRequest(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_UDATAREAD_REQUEST:
			eGwmResult = mgr_cas_EvtCasUserDataRequest(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SYSINFO_UPDATED:
			eGwmResult = mgr_cas_EvtCasSystemInfoUpdated(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_INSERTED:
			eGwmResult = mgr_cas_EvtCasScInserted(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_REMOVED:
			eGwmResult = mgr_cas_EvtCasScRemoved(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DRM_SIGNALLED:
		case eSEVT_CAS_DRM_UNKNOWN:
		case eSEVT_CAS_DRM_NONE:
			eGwmResult = mgr_cas_EvtCasDrm(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_DMV_SIGNALLED:
		case eSEVT_CAS_DMV_UNKNOWN:
		case eSEVT_CAS_DMV_NONE:
			eGwmResult = mgr_cas_EvtCasDmv(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_NOTIFICATION:
			BUS_SendMessage(NULL, eMEVT_CAS_NOTIFICATION, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_UPDATE_KEYINFO:
			eGwmResult = mgr_cas_EvtCasUpdateKeyInfo(message, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_PRM_PVR_UPDATE_KEYINFO:
			eGwmResult = mgr_cas_EvtCasPrmPvrUpdateKeyInfo(message, hAction, p1, p2, p3);
			break;
		default:
#ifdef CONFIG_DEBUG
			{
				HUINT8 *pMsg = mgr_cas_GetUnProcMessageName(message);

				if(pMsg != NULL)
				{
					HxLOG_Info("UnProc Message[%s], ActId(%x), p1(%x), p2(%x), p3(%x)\n", pMsg, ulActionId, p1, p2, p3);
				}
			}
#endif
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#else

/*******************************************************************/
/********************      	Global Prototype		********************/
/*******************************************************************/
#if defined(CONFIG_MW_CI_PLUS)
extern BUS_Result_t PROC_CAS_TuneMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t AP_CiPlus_UpgradeProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

/*******************************************************************/
/********************      	Local Prototype		********************/
/*******************************************************************/
/* OK/FAIL/Check/CMD 처리 */
static BUS_Result_t mgr_cas_EvtCasCmdReceived(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

/* SC Insert / Remove 처리 */
STATIC BUS_Result_t local_am_EvtCasScInserted(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t mgr_cas_AM_EvtCasScRemoved(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_MW_CI)
/* CAM Insert / Remove 처리 */
STATIC BUS_Result_t local_am_EvtCasCamInsertedRemoved(HINT32 message, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
#endif // #if defined(CONFIG_MW_CI)


#if defined(CONFIG_MW_CAS_IRDETO)
STATIC BUS_Result_t mgr_cas_EvtCasDecodeResetIRD(HUINT32 ulActionId,		HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t mgr_cas_EvtCasDecodeResetChOrder(HUINT32 ulActionId,	HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t mgr_cas_EvtCasDecodeRescan(HUINT32 ulActionId,			HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t mgr_cas_EvtCasDecodeRescanFromHome(HUINT32 ulActionId,	HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t mgr_cas_EvtCasDecodeReconnect(HUINT32 ulActionId,		HINT32 p1, HINT32 p2, HINT32 p3);
#endif // #if defined(CONFIG_MW_CAS_IRDETO)


#if defined(CONFIG_MW_CI_PLUS)
STATIC BUS_Result_t local_am_EvtCasCamTune(Handle_t hAction,		 HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t local_am_EvtCasCiPlusUpgrade(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3);
  #if 0 // menu root UI로 옮겨짐
	STATIC BUS_Result_t local_am_EvtCasCiPlusCcReport (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
  #endif
#endif // #if defined(CONFIG_MW_CI_PLUS)


/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/
STATIC BUS_Result_t mgr_cas_EvtCasCmdReceived(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// check prev. appl exist
	if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
	{
		BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
	}


	HxLOG_Print("MESSAGE : BUS_MGR_Create[AP_CasMain_GetActionProc]\n");
	BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId),
		SVC_PIPE_GetActionHandle(ulActionId), p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}



STATIC BUS_Result_t local_am_EvtCasScInserted(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_OP_APS)
	// APS 사양일 경우, SEARCH 중이 아니면 HD Simulcast를 실행할 수 있다.
	if (MGR_ACTION_GetType(ulActionId) != eActionType_SEARCH)
	{
		AP_ApsLcnSearch_SimulcastPreferredList();
	}
#endif

// ewlee 20100914
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
	if(p1 == eDxCAS_GROUPID_IR)
	{
		p3 = eSEVT_CAS_SC_INSERTED;
		// check prev. appl exist
		if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
		{
			BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
		}

		HxLOG_Print("MESSAGE : BUS_MGR_Create[AP_CasMain_GetActionProc]\n");
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId),
						SVC_PIPE_GetActionHandle(ulActionId), p1, p2, p3);
	}
#endif


	NOT_USED_PARAM(ulActionId);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}





STATIC BUS_Result_t mgr_cas_AM_EvtCasScRemoved(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_MM_PVR)
	MGR_LIVE_PincodeDeleteAllServiceHandle(); /* pvr 관련 pincode 정보 삭제 */
#endif
	DB_SVC_ResetRatingPassedEventId(); /* Live rating passed event ID 기록 reset */

#if defined(CONFIG_MW_CAS_CONAX)
	if(p1 == eDxCAS_GROUPID_CX)
	{
		p3 = eSEVT_CAS_SC_REMOVED;
		// check prev. appl exist
		if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
		{
			BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
		}

		HxLOG_Print("MESSAGE : BUS_MGR_Create[AP_CasMain_GetActionProc]\n");
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId),
						SVC_PIPE_GetActionHandle(ulActionId), p1, p2, p3);
	}
#endif

// ewlee 20100914
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
	if(p1 == eDxCAS_GROUPID_IR)
	{
		p3 = eSEVT_CAS_SC_REMOVED;
		// check prev. appl exist
		if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
		{
			BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
		}

		HxLOG_Print("MESSAGE : BUS_MGR_Create[AP_CasMain_GetActionProc]\n");
		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId),
						SVC_PIPE_GetActionHandle(ulActionId), p1, p2, p3);
	}
#endif

	NOT_USED_PARAM(ulActionId);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}



STATIC BUS_Result_t local_am_EvtCasCamInsertedRemoved(HINT32 message, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ulActionId = MGR_ACTION_GetMainActionId();

	// p1 is slot id;
	// current input (dtv/atv) 한정.
	if( (MGR_ACTION_GetType(ulActionId) == eViewType_ATV) ||
		(MGR_ACTION_GetType(ulActionId) == eViewType_WTV)
		)
	{
		// check prev. appl exist
		if( BUS_MGR_Get(AP_CasMain_GetActionProc(ulActionId)) != NULL )
		{
			BUS_MGR_Destroy(AP_CasMain_GetActionProc(ulActionId));
		}

		BUS_MGR_Create(NULL, APP_CAS_PRIORITY, AP_CasMain_GetActionProc(ulActionId),
						SVC_PIPE_GetActionHandle(ulActionId), message, p1, 0);
	}

	return ERR_BUS_NO_ERROR;
}



#if defined(CONFIG_MW_CAS_IRDETO)
STATIC BUS_Result_t mgr_cas_EvtCasDecodeResetIRD(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MGR_ACTION_RebootSystem();
	//SVC_SYS_Reboot();

	CASMGR_PARAM_WARNING_REMOVE(0, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_cas_EvtCasDecodeResetChOrder(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CASMGR_PARAM_WARNING_REMOVE(0, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_cas_EvtCasDecodeRescan(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CASMGR_PARAM_WARNING_REMOVE(0, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_cas_EvtCasDecodeRescanFromHome(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CASMGR_PARAM_WARNING_REMOVE(0, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_cas_EvtCasDecodeReconnect(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	CASMGR_PARAM_WARNING_REMOVE(0, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}
#endif	// #if defined(CONFIG_MW_CAS_IRDETO)



#if defined(CONFIG_MW_CI_PLUS)
STATIC BUS_Result_t local_am_EvtCasCamTune(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if( BUS_MGR_Get(PROC_CAS_TuneMgr) != NULL )
	{
		BUS_MGR_Destroy(PROC_CAS_TuneMgr);
	}

	BUS_MGR_Create("PROC_CAS_TuneMgr",
				APP_CAS_PRIORITY,
				PROC_CAS_TuneMgr,
				hAction,
				p1,
				p2,
				p3);

	return MESSAGE_BREAK;
}


STATIC BUS_Result_t local_am_EvtCasCiPlusUpgrade(HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT16 			usSlotId;
	HUINT32 			ulVersionID;
	SvcCas_CiPlusUpgrade_t *pstStatus = NULL;
	HERROR				hErr;

	usSlotId = (HUINT16)p1;
	ulVersionID = (HUINT32)p2;

	hErr = SVC_CAS_GetUpdateStatus(usSlotId, ulVersionID, &pstStatus);
	if (hErr != ERR_FAIL)
	{
		if (BUS_MGR_Get(AP_CiPlus_UpgradeProc) == NULL)
		{
			BUS_MGR_Create("AP_CiPlus_UpgradeProc", APP_DEFAULT_PRIORITY, AP_CiPlus_UpgradeProc, 0, 0, 0, 0);
		}

		BUS_SendMessage(AP_CiPlus_UpgradeProc, eSEVT_CAS_CIPLUS_UPGRADE, ulActionId, p1, p2 , 0);
	}

	return ERR_BUS_NO_ERROR;
}

#if 0
STATIC BUS_Result_t local_am_EvtCasCiPlusCcReport (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT16 		usSlotId;
	HUINT32			ulActionId;
	HUINT32 		ulVersionID;
	SvcCas_CcReport_t	*pstCcReport = NULL;
	HERROR			hErr;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	usSlotId = (HUINT16)p1;
	ulVersionID = (HUINT32)p2;

	hErr = SVC_CAS_GetCcReport(usSlotId, ulVersionID, &pstCcReport);
	if (hErr != ERR_FAIL)
	{
		AP_CASUI_CiPlus_ShowMsgPopup(pstCcReport);
	}

	return ERR_BUS_NO_ERROR;
}
#endif	// #if 0
#endif	// #if defined(CONFIG_MW_CI_PLUS)

/********************************************************************/
/********************      Global Functions	*************************/
/********************************************************************/
BUS_Result_t proc_CAS(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eGwmResult	= ERR_BUS_NO_ERROR;
	HUINT32			ulActionId	= SVC_PIPE_GetActionId(hAction);

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("MESSAGE : eMEVT_BUS_CREATE \n");
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("MESSAGE : eMEVT_BUS_DESTROY \n");
			break;

		/**********************************************
			CAS service로부터의 시청 허가 이벤트
		***********************************************/
		case eSEVT_CAS_CMD :
			HxLOG_Print("MESSAGE : EVT_CAS_CMD_RECV\n");
			eGwmResult = mgr_cas_EvtCasCmdReceived(ulActionId, p1, p2, message);
			break;

		/********************************************
			CAS SC 끼워짐.
		*********************************************/
		case eSEVT_CAS_SC_INSERTED:
			HxLOG_Print("MESSAGE : eSEVT_CAS_SC_INSERTED\n");
			eGwmResult = local_am_EvtCasScInserted(ulActionId, p1, p2, p3);
			break;


		/********************************************
			CAS 빠짐.
		*********************************************/
		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print("MESSAGE : eSEVT_CAS_SC_REMOVED(slot id : %d)\n", p1);
			eGwmResult = mgr_cas_AM_EvtCasScRemoved(ulActionId, p1, p2, p3);
			break;

#if defined(CONFIG_MW_CI)
		/*****************
			CI Banner
		******************/
		case eSEVT_CAS_CAM_INSERTED:
			HxLOG_Print("MESSAGE : eSEVT_CAS_CAM_INSERTED\n");
			eGwmResult = local_am_EvtCasCamInsertedRemoved(eSEVT_CAS_CAM_INSERTED, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_CAM_REMOVED:
			HxLOG_Print("MESSAGE : eSEVT_CAS_CAM_REMOVED\n");
			eGwmResult = local_am_EvtCasCamInsertedRemoved(eSEVT_CAS_CAM_REMOVED, ulActionId, p1, p2, p3);
			break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
		case eSEVT_CAS_DECODE_RESET_IRD:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESET_IRD\n");
			eGwmResult = mgr_cas_EvtCasDecodeResetIRD(ulActionId, p1, p2, p3);
			break;
		case eSEVT_CAS_DECODE_RESET_CH_ORDER:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESET_CH_ORDER\n");
			eGwmResult = mgr_cas_EvtCasDecodeResetChOrder(ulActionId, p1, p2, p3);
			break;
		case eSEVT_CAS_DECODE_RESCAN:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESCAN\n");
			eGwmResult = mgr_cas_EvtCasDecodeRescan(ulActionId, p1, p2, p3);
			break;
		case eSEVT_CAS_DECODE_RESCAN_FROM_HOME:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RESCAN_FROM_HOME\n");
			eGwmResult = mgr_cas_EvtCasDecodeRescanFromHome(ulActionId, p1, p2, p3);
			break;
		case eSEVT_CAS_DECODE_RECONNECT:
			HxLOG_Print("MESSAGE : eSEVT_CAS_DECODE_RECONNECT\n");
			eGwmResult = mgr_cas_EvtCasDecodeReconnect(ulActionId, p1, p2, p3);
			break;
#endif	// #if defined(CONFIG_MW_CAS_IRDETO)


		case eSEVT_CAS_PPV_REPLY:
			HxLOG_Print("MESSAGE : EVT_CAS_PPV_REPLY_RECV\n");
			eGwmResult = mgr_cas_EvtCasCmdReceived(ulActionId, p1, p2, message);
			break;

		case eSEVT_CAS_PINCODE_REPLY:
		case eSEVT_CAS_UDATAREAD_REPLY:
			HxLOG_Print("MESSAGE : EVT_CAS_SMARTCARD_RECV\n");
			eGwmResult = mgr_cas_EvtCasCmdReceived(ulActionId, p1, p2, message);
			break;

#if defined(CONFIG_MW_CI_PLUS)
		/*****************
			CI PLUS Message
		******************/
		case eSEVT_CAS_CAM_TUNE:
			HxLOG_Print("MESSAGE : eSEVT_CAS_CAM_TUNE\n");
			eGwmResult = local_am_EvtCasCamTune(hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_CIPLUS_UPGRADE:
			HxLOG_Print("MESSAGE : eSEVT_CAS_CIPLUS_UPGRADE\n");
			eGwmResult = local_am_EvtCasCiPlusUpgrade(ulActionId, p1, p2, p3);
			break;

	  #if 0
		case eSEVT_CAS_CIPLUS_CC_REPORT:
			HxLOG_Print("MESSAGE : eSEVT_CAS_CIPLUS_CC_REPORT\n");
			eGwmResult = local_am_EvtCasCiPlusCcReport(ulActionId, p1, p2, p3);
			break;
	  #endif	// #if 0
	#endif	// #if defined(CONFIG_MW_CI_PLUS)

	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif

HERROR MGR_CAS_Init(void)
{

#if defined (CONFIG_DEBUG)
	CMD_Register_MGR_CAS();
#endif
	return pmgr_cas_Init();
}

HERROR MGR_CAS_Start(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_CAS", APP_CAS_HIGH_PRIORITY, proc_CAS, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR MGR_CAS_TotTdtUpdated(void)
{
	return pmgr_cas_TotTdtUpdated();
}

HERROR MGR_CAS_GetCaSystemId (Handle_t hAction, HUINT32 *pucCaSysId)
{
	if (NULL == pucCaSysId)
		return ERR_FAIL;

	return pmgr_cas_GetCaSystemId(hAction, pucCaSysId);
}

HERROR MGR_CAS_OnShowProgramCasControl (Handle_t hAction, Handle_t hSvc, HUINT32 *pulMsg, HUINT32 *pulParam1, HUINT32 *pulParam2, HUINT32 *pulParam3)
{
	return pmgr_cas_OnShowProgramCasControl (hAction, hSvc, pulMsg, pulParam1, pulParam2, pulParam3);
}

HERROR MGR_CAS_IsSmartcardRemoved(HUINT8 ucSlotIndex, HBOOL *pbRemoved)
{
	if (NULL == pbRemoved)
		return ERR_FAIL;

	return pmgr_cas_IsSmartcardRemoved (ucSlotIndex, pbRemoved);
}

HERROR MGR_CAS_DefaultSetting(void)
{
	return pmgr_cas_DefaultSetting();
}

HERROR	MGR_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, DxCAS_GroupId_e eCasGroupId, void *pvData)
{
	HERROR				hErr;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);
	SIxSpec_e			eSiSpec;
	SIxTextEncoding_e	eTextEnc;
	SvcCas_StartData_t	stStartData;

#if defined(CONFIG_SUPPORT_FCC) && defined(CONFIG_MW_CI)
	// FCC 동작으로 CAS Start 요청이 들어오면, 실제로는 Start를 시키지 않고 OK를 올려준다.
	if ((MGR_ACTION_IsViewAction(ulActionId) == TRUE)
		&& (ulActionId != MGR_ACTION_GetMainActionId()))
	{
		return ERR_OK;
	}

	// 추후 각 CAS service에 대해서도 FCC를 적용 한다면, 전체적인 구조를 고려해서 설계 후 구현 해야함.
#endif

	hErr = SVC_SI_GetSiProperty(ulActionId, &eSiSpec, &eTextEnc);
	if (ERR_OK != hErr)
		return ERR_FAIL;

	HxSTD_MemSet(&stStartData, 0, sizeof(SvcCas_StartData_t));
	stStartData.eCasGroupId	= eCasGroupId;
	stStartData.eSiSpec		= eSiSpec;
	stStartData.eTextEnc	= eTextEnc;

	switch (eCasGroupId)
	{
	case eDxCAS_GROUPID_CAM:
		{
			SvcCas_CamStartData_t	*pstCamStartData = (SvcCas_CamStartData_t *)pvData;

			if (pstCamStartData)
			{
				HxSTD_MemCopy(&stStartData.uData.stCasCamStartData, pstCamStartData, sizeof(SvcCas_CamStartData_t));
			}
		}
		break;
	default:
		break;
	}

	return SVC_CAS_StartService(hAction, hSvc, eActionType, &stStartData);
}

HERROR MGR_CAS_StopService(Handle_t hAction, Handle_t hNextSvc, ApiSyncMode_t ulSyncMode)
{
	return SVC_CAS_StopService(hAction, hNextSvc, ulSyncMode);
}

HBOOL MGR_CAS_NeedCasRatingCheck(void)
{
	return pmgr_cas_NeedCasRatingCheck();
}

HBOOL MGR_CAS_IsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
	return pmgr_cas_IsRatingOK(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
}

HBOOL MGR_CAS_NeedCasRatingWaitCheck(void)
{
	return pmgr_cas_NeedCasRatingWaitCheck();
}

HBOOL MGR_CAS_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
	return pmgr_cas_IsWaitParentalRatingState(pstPgInfo);
}

HERROR MGR_CAS_GetRatingFromCasMsg(void *pvCasMsg, HUINT32 *pulRating)
{
	return pmgr_cas_GetRatingFromCasMsg(pvCasMsg, pulRating);
}

HERROR MGR_CAS_IsSameCasIdInSc(HUINT8 ucSlotId, HUINT16 usCaSysId, HUINT8 *pbSame)
{
	return pmgr_cas_IsSameCasIdInSc(ucSlotId, usCaSysId, pbSame);
}

/********************************************************************
* For Debugging
********************************************************************/
#if defined (CONFIG_DEBUG)
static void CMD_Register_MGR_CAS(void)
{
#define	hCmdHandle		"MGRCAS"

	pmgr_cas_CmdInit(hCmdHandle);

	return;
}
#endif


