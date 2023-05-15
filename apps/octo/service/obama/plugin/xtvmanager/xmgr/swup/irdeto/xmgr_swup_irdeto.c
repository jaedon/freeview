/**
	@file     xmgr_sw_update_irdeto.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <napi_netconf.h>
#include <svc_pipe.h>
#include <svc_sys.h>
#include <svc_resources.h>
#include <svc_swup.h>
#include <svc_cas.h>
#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_swup.h>
#include <xmgr_swup.h>

#include <otl.h>

#include <_mgr_swup.h>

#if defined(CONFIG_MW_CAS_IRDETO)
#include <ir_ctrl.h>
#endif

#define _____typedef_____
typedef	struct
{
	MgrSwup_BaseContents_t		stBaseContents;

	HBOOL					bUserTP;
	DbSvc_TsInfo_t			stTsInfo;

	DxSwUpdate_Source_e			eSource;
	SvcSwUpdate_DetectParamRf_t	stDetectRfParam;

	Handle_t						hDownload;

	SvcSwUpdate_EmergencyUpdateInfo_t	stSourceInfo;

	HCHAR					*pszIpDetectServerUrl;
} xmgrSwup_IrdetoContents_t;

#define _____Static_Function_____

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_EnterActionInit_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t	*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s][%04d] pstContents->bUserTP [%d]\n",  __FUNCTION__, __LINE__, pstContents->bUserTP);

	if (pstContents->bUserTP == TRUE)
	{
		MGR_SWUPINFO_ResetHomeTp();
		MGR_SWUPINFO_AddOtaHomeTp(&pstContents->stTsInfo);
	}
	else
	{
		MGR_SWUP_SetDefaultTp();
	}

	// irdeto ota 는 시나리오상 RF 만 시도...
	pstContents->eSource = eDxSWUPDATE_SOURCE_Rf;

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_EnterActionConnect_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	switch (pstContents->eSource)
	{
		case eDxSWUPDATE_SOURCE_Rf:
			MGR_SWUP_PostMessage(hSWUpdate, eMEVT_SWUP_DETECT_START, HANDLE_NULL, eDxSWUPDATE_SOURCE_Rf, (HINT32)&pstContents->stDetectRfParam, 0);
			break;

		default:
			HxLOG_Error("Invalid eSource Type...\n");
			break;
	}
	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_EnterActionDetect_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t *)pvUserData;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d]  hSwUpHandle(0x%x) eSource(RF) \n", __FUNCTION__, __LINE__, hSWUpdate);
#endif

	if (pstContents->eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		MGR_SWUP_DetectCreate(0, hSWUpdate);
	}
	else
	{
		HxLOG_Error("Invalid eSource Type (0x%x). irdeto support RF OTA only\n", pstContents->eSource);
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_EnterActionDownload_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t	*pstContents 	= (xmgrSwup_IrdetoContents_t*)pvUserData;
	SvcSwUpdate_DetectResult_t	*pstDetectResult	= NULL;
	HERROR 						 hError;
	DbSvc_TsInfo_t				 stTsInfo;
	SvcSi_SwupSignal_t 			 stOtaInfo;

	DxTuneParam_t				 stChTuneInfo;
	HUINT32 					 ulDiTuneDataSize=0;
	void						*pvDiTuneData = NULL;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

	MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);
	pstContents->eSource = pstDetectResult->eSource;

	//IRDETO supports only RF, so other source error
	if(pstContents->eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		// RF 의 경우.. serched info 가 존재하는지 check
		hError = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
		if (hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			if (pstDetectResult != NULL)
				HLIB_STD_MemFree(pstDetectResult);
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("eSource  is not eDxSWUPDATE_SOURCE_Rf \n");
		if (pstDetectResult != NULL)
			HLIB_STD_MemFree(pstDetectResult);
		return ERR_FAIL;
	}

	HxSTD_MemSet(&stOtaInfo, 0x00, sizeof(SvcSi_SwupSignal_t));
	HxSTD_MemSet(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));
	HxSTD_MemSet(&stChTuneInfo, 0x00, sizeof(DxTuneParam_t));

	// serched info 가 존재하는지 check
	 hError = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
	if (hError != ERR_OK)
	{
		if (pstDetectResult != NULL)
			HLIB_STD_MemFree(pstDetectResult);
		HxLOG_Error("MGR_SWUPINFO_GetSearchedInfo Error!!!!\n");
		return ERR_FAIL;
	}

	// 1. Tuning Info 를 DD용으로 converting 한다.
	// 1-1. DB Tuning Info -> Ch Tuning Info
	hError = OTL_CONV_DbTuningInfo2DxTuneParam ((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
	if (hError != ERR_OK)
	{
		if (pstDetectResult != NULL)
			HLIB_STD_MemFree(pstDetectResult);
		HxLOG_Error("OTL_CONV_DbTuningInfo2DxTuneParam Error!!!!\n");
		return ERR_FAIL;
	}

	// 1-2. Ch Tuning Info -> DD Tuning Data
	 hError = SVC_CH_MakeDiTuningDataByChTuningParam(&stChTuneInfo, &ulDiTuneDataSize, &pvDiTuneData);
	 if (hError != ERR_OK)
	 {
		if (pstDetectResult != NULL)
			HLIB_STD_MemFree(pstDetectResult);
		 HxLOG_Error("SVC_CH_MakeDiTuningDataByChTuningParam Error!!!!\n");
		 return ERR_FAIL;
	 }

#if defined(CONFIG_MW_CAS_IRDETO)
	// 2. Set OTA Info to NVRAM
	xsvc_cas_IrOtaSetChannelParam(stTsInfo.ucTunerId, pvDiTuneData);
#endif

	// 3. Cancel 한 정보 등을 다 삭제하도록 한다.
	MGR_SWUPINFO_ResetSearchedInfo();
	MGR_SWUPINFO_ResetOtaRetryCount();
	MGR_SWUPINFO_SetOtaCanceledVersion (0, 0);

	// 4. Now, store the informations
	//	당장 Reboot 하지 않아도 곧 Reboot할 수 있으니 적어놓도록 한다.
	SVC_SYS_SetOtaType(eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DVBSSU); // RF는 eOTA_INFO_SLOT_0를 사용한다.
	SVC_SYS_SetOtaPid(eSWUP_INFO_SLOT_0, stOtaInfo.usPid);
	SVC_SYS_SetOtaTuningInfo(eSWUP_INFO_SLOT_0, &stChTuneInfo);
	SVC_CH_FreeDiTuningData(pvDiTuneData);

#if defined(CONFIG_MW_CAS_IRDETO)
	xsvc_cas_IrOtaSetFlagUp(TRUE, stOtaInfo.usPid, stOtaInfo.ucTableId);
#endif
	if (pstDetectResult != NULL) {
		HLIB_STD_MemFree(pstDetectResult);
	}
#if 1 // HM-9503HD OTA Trigger하고 나서 OTA Fail나는 문제 때문에 Reboot을 Destory에서 한다. 하는데..... 우선은 이곳엣 reboot()!!!
	MGR_ACTION_RebootSystem(); // destroy 하고 reboot 하는 것으로 수정. 이전에 ota fail 나는 이슈 존재 ...
#endif

	return ERR_OK;
}

// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_EnterActionInstall_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t				*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	SvcSwUpdate_EmergencyUpdateInfo_t	*pstSourceInfo;
	HUINT8								*pszSourceFile;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	pszSourceFile = (HUINT8 *)DDB_SAVE_FILE_PATH;
	pstSourceInfo = &pstContents->stSourceInfo;

	pstSourceInfo->eSource = pstContents->eSource;
	if(pstSourceInfo->eSource != eDxSWUPDATE_SOURCE_Rf)
	{
		HxLOG_Error("pstSourceInfo->eSource is not  eDxSWUPDATE_SOURCE_Rf\n");
		return ERR_FAIL;
	}

	//	아래 값을들 다 채워 줘여 Emergency OTA가 동작한다.
	//	Emergency OTA는 Freesat Spec이 아님.
	switch (pstSourceInfo->eSource)
	{
		case eDxSWUPDATE_SOURCE_Rf:
			HxLOG_Debug("eDxSWUPDATE_SOURCE_RfInstall....\n");
			pstSourceInfo->unInfo.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;
			break;

		default:
			break;
	}

	MGR_SWUP_InstallStart(hSWUpdate, pszSourceFile, pstSourceInfo);

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_EnterActionError_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	// nParam3 에는 mgr단의 fail reason, nParam1에는 svc단의 fail reason... 이 부분 정리.

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_ERROR, HANDLE_NULL, (HINT32)hSWUpdate, 0, nParam3);

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_EnterActionFinish_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);

	return ERR_OK;
}

// eSWUP_STATE_Init
STATIC HERROR xmgr_swup_LeaveActionInit_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Connect
STATIC HERROR xmgr_swup_LeaveActionConnect_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	// TODO: Destory Connect Action

//	MGR_SWUP_PostMessage(ulTag, eMEVT_SWUP_DETECT_START, ulTag, (HINT32)eDxSWUPDATE_SOURCE_Ip, (HINT32)&pstContents->stDetectParamIp, 0);

	return ERR_OK;
}

// eSWUP_STATE_Detect
STATIC HERROR xmgr_swup_LeaveActionDetect_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(nMessage == eMEVT_SWUP_NOTIFY_DETECT_SUCCESS)
	{
		SvcSwUpdate_DetectResult_t			*pstDetectResult;
		SvcCas_CtrlParam_t					stIrCtrlParam;
#if defined(CONFIG_MW_CAS_IRDETO)
		IR_LOADER_GetLoadSqeNumOutParam_t	stLoadSqeNum;
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
		pstDetectResult	= (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));

		stIrCtrlParam.pvOut 	= &stLoadSqeNum;
		stIrCtrlParam.ulControlType = eCACTRL_IR_LOADER_GetLoadSeqNum;
		SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoadSeqNum, &stIrCtrlParam);

		MGR_SWUP_GetDetectInfo(hSWUpdate, pstDetectResult);

		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, (HINT32)hSWUpdate, (HINT32)stLoadSqeNum.usLoadSeqNum, (HINT32)pstDetectResult);
#endif

		MGR_SWUP_DetectDestroy();

		pstContents->eSource = eDxSWUPDATE_SOURCE_None;
	}
	else if(nMessage ==  eMEVT_SWUP_NOTIFY_DETECT_FAIL)
	{
		HxLOG_Print("eSWUP_STATE_Detect : DETECT_FAIL!!!\n");
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, (HINT32)hSWUpdate, 0, 0);
		pstContents->eSource = eDxSWUPDATE_SOURCE_None;
		MGR_SWUP_DetectDestroy();
	}

	if(pstContents->eSource == eDxSWUPDATE_SOURCE_None)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_ACTION_NOTIFY_RESULT, (Handle_t)NULL, eMGR_EVENT_SWUP_DETECT_FINISH, 0, 0);
	}

	return ERR_OK;
}

// eSWUP_STATE_Download
STATIC HERROR xmgr_swup_LeaveActionDownload_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	HERROR						hError;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(pstContents->eSource != eDxSWUPDATE_SOURCE_Rf)
	{
		HxLOG_Error("pstSourceInfo->eSource is not  eDxSWUPDATE_SOURCE_Rf\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("pstContents->eSource : [%x]\n", pstContents->eSource);

	hError = MGR_SWUP_DownloadDestroy(eDxSWUPDATE_SOURCE_Rf);

	//	Download Destroy이에 Release Action
	MGR_ACTION_Release(eActionId_VIEW_0, eActionType_SEARCH);

	return ERR_OK;
}

// eSWUP_STATE_Install
STATIC HERROR xmgr_swup_LeaveActionInstall_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	MGR_SWUP_InstallStop();

	return ERR_OK;
}

// eSWUP_STATE_Error
STATIC HERROR xmgr_swup_LeaveActionError_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

// eSWUP_STATE_Finish
STATIC HERROR xmgr_swup_LeaveActionFinish_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

#define _____Member_Function_____

BUS_Result_t	XMGR_SWUP_EnterAction_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e ePrevState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t	*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	HERROR						 hError = ERR_FAIL;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return MESSAGE_BREAK;
	}

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hError = xmgr_swup_EnterActionInit_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hError = xmgr_swup_EnterActionConnect_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hError = xmgr_swup_EnterActionDetect_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hError = xmgr_swup_EnterActionDownload_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hError = xmgr_swup_EnterActionInstall_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hError = xmgr_swup_EnterActionError_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hError = xmgr_swup_EnterActionFinish_Irdeto(hSWUpdate, ePrevState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	return MESSAGE_PASS;
}

BUS_Result_t	XMGR_SWUP_LeaveAction_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eCurrState, MgrSwup_State_e eNextState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	HERROR					 	hError = ERR_FAIL;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return MESSAGE_BREAK;
	}

	switch (eCurrState)
	{
	case eSWUP_STATE_Init:
		hError = xmgr_swup_LeaveActionInit_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Connect:
		hError = xmgr_swup_LeaveActionConnect_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Detect:
		hError = xmgr_swup_LeaveActionDetect_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Download:
		hError = xmgr_swup_LeaveActionDownload_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Install:
		hError = xmgr_swup_LeaveActionInstall_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Error:
		hError = xmgr_swup_LeaveActionError_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	case eSWUP_STATE_Finish:
		hError = xmgr_swup_LeaveActionFinish_Irdeto(hSWUpdate, eNextState, pvUserData, nMessage, hAction, nParam1, nParam2, nParam3);
		break;
	default:
		break;
	}

	return MESSAGE_PASS;
}

MgrSwup_State_e XMGR_SWUP_GetNextState_Irdeto(Handle_t hSWUpdate, MgrSwup_State_e eState, void *pvUserData, HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	xmgrSwup_IrdetoContents_t		*pstContents = (xmgrSwup_IrdetoContents_t*)pvUserData;
	MgrSwup_State_e				 eNextState = eSWUP_STATE_Unknown;

	#if defined(CONFIG_DEBUG)
		HxLOG_Print("[%s][%04d] (+) Current eState[%s] Message [%s]\n", __FUNCTION__, __LINE__, XMGR_SWUP_GetEnumStrOfState(eState), XMGR_SWUP_GetEnumStrOfMessage(nMessage));
	#endif

	if(nMessage == eMEVT_SWUP_CANCEL || nMessage == eMEVT_SWUP_DOWNLOAD_STOP)
	{
		eNextState = eSWUP_STATE_Finish;
		return eNextState;
	}

	switch (eState)
	{
		case eSWUP_STATE_Init:
			HxLOG_Debug("nMessage : [%x] [%x]\n", nMessage, eMEVT_SWUP_DETECT_START);
			switch (nMessage)
			{
				case eMEVT_SWUP_CONNECT_START:
					eNextState = eSWUP_STATE_Connect;
					break;
				case eMEVT_SWUP_DOWNLOAD_START:
					eNextState = eSWUP_STATE_Download;
					break;
				case eMEVT_SWUP_INSTALL_START:
					eNextState = eSWUP_STATE_Install;
					break;
				default:
					HxLOG_Error("eSWUP_STATE_Init :: Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Connect:
			switch (nMessage)
			{
				case eMEVT_SWUP_DETECT_START:
					eNextState = eSWUP_STATE_Detect;
					break;
				default:
					HxLOG_Error("eSWUP_STATE_Connect :: Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Detect:
			switch (nMessage)
			{
				case eMEVT_SWUP_NOTIFY_DETECT_SUCCESS:
					if(pstContents->stBaseContents.eBatchProcess == eDxBAT_TYPE_STANDBY)
					{
						eNextState = eSWUP_STATE_Download;
					}
					else
					{
						eNextState = eSWUP_STATE_Finish;
					}
					break;
				case eMEVT_SWUP_NOTIFY_DETECT_FAIL:
					eNextState = eSWUP_STATE_Error;
					break;
				default:
					break;
			}
			break;

		case eSWUP_STATE_Download:
			switch (nMessage)
			{
				case eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS:
					eNextState = XMGR_SWUP_GetNextState_AfterDownload(pstContents->eSource);
					break;
				case eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL:
					eNextState = eSWUP_STATE_Error;
					break;
				case eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS:
					if (pstContents->eSource == eDxSWUPDATE_SOURCE_Rf)
					{
						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam1, 100);
					}
					else
					{
						pstContents->hDownload = nParam3;
						BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DOWNLOAD_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
					}
					eNextState = eSWUP_STATE_Download;
					break;
				default:
					HxLOG_Error("eSWUP_STATE_Download :: Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Install:
			switch (nMessage)
			{
				case eMEVT_SWUP_INSTALL_SUCCESS:
					eNextState = eSWUP_STATE_Finish;
					break;
				case eMEVT_SWUP_NOTIFY_INSTALL_FAIL:
					eNextState = eSWUP_STATE_Error;
					break;
				case eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS:
					BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_PROGRESS, HANDLE_NULL, (HINT32)hSWUpdate, nParam2, 100);
					eNextState = eSWUP_STATE_Install;
					break;
				default:
					HxLOG_Error("eSWUP_STATE_Install :: Invalid nMessage [%x]\n", nMessage);
					break;
			}
			break;

		case eSWUP_STATE_Error:
			eNextState = eSWUP_STATE_Finish;
			break;

		case eSWUP_STATE_Finish:
			eNextState = eSWUP_STATE_Finish;
			break;

		default:
			break;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s][%04d] (-) hSWUpdate (0x%X)  Change eState [%s -> %s  ->%s]  \n",
		__FUNCTION__, __LINE__, hSWUpdate, XMGR_SWUP_GetEnumStrOfState(eState), XMGR_SWUP_GetEnumStrOfMessage(nMessage), XMGR_SWUP_GetEnumStrOfState(eNextState));
#endif
	return eNextState;
}

void * XMGR_SWUP_Create_Irdeto(void *pvUserData, DxBatchType_e eBatchProcess)
{
	DbSvc_TsInfo_t			*pstSvcTsInfo = (DbSvc_TsInfo_t *)pvUserData;
	xmgrSwup_IrdetoContents_t	*pstContents;


	pstContents = (xmgrSwup_IrdetoContents_t*)HLIB_STD_MemAlloc(sizeof(xmgrSwup_IrdetoContents_t));
	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents MemAlloc Error [%x]\n");
		return NULL;
	}

	pstContents->stBaseContents.eBatchProcess = eBatchProcess;

	pstContents->bUserTP = FALSE;
	if (pstSvcTsInfo)
	{
		pstContents->bUserTP = TRUE;
		HxSTD_MemCopy(&pstContents->stTsInfo, pstSvcTsInfo, sizeof(DbSvc_TsInfo_t));
	}

	return (void*)pstContents;
}

HERROR XMGR_SWUP_Destroy_Irdeto(void *pvContents)
{
	if(pvContents)
	{
		HLIB_STD_MemFree(pvContents);
	}
	return ERR_OK;
}

HERROR	XMGR_SWUP_SetManualInfo_Irdeto(DbSvc_TsInfo_t *pstDstHomeTsInfo, DbSvc_TsInfo_t *pstUserHomeTsInfo)
{
	if ( !pstDstHomeTsInfo )
		return ERR_FAIL;

	HxSTD_MemSet(pstDstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	pstDstHomeTsInfo->eDeliType							=	pstUserHomeTsInfo->eDeliType;

#if defined(CONFIG_MW_CH_SATELLITE)
	pstDstHomeTsInfo->stTuningParam.sat.ulFrequency		=	pstUserHomeTsInfo->stTuningParam.sat.ulFrequency;
	pstDstHomeTsInfo->stTuningParam.sat.ulSymbolRate	=	pstUserHomeTsInfo->stTuningParam.sat.ulSymbolRate;
	pstDstHomeTsInfo->stTuningParam.sat.eTransSpec		=	pstUserHomeTsInfo->stTuningParam.sat.eTransSpec;
	pstDstHomeTsInfo->stTuningParam.sat.eSatType		=	eSatType_ALL;
	pstDstHomeTsInfo->stTuningParam.sat.eFecCodeRate	=	pstUserHomeTsInfo->stTuningParam.sat.eFecCodeRate;
	pstDstHomeTsInfo->stTuningParam.sat.ePskMod			=	pstUserHomeTsInfo->stTuningParam.sat.ePskMod;
	pstDstHomeTsInfo->stTuningParam.sat.ePilot			=	eDxSAT_PILOT_OFF;
	pstDstHomeTsInfo->stTuningParam.sat.ePolarization	=	pstUserHomeTsInfo->stTuningParam.sat.ePolarization;
	pstDstHomeTsInfo->stTuningParam.sat.eRollOff		=	eDxSAT_ROLL_020;
	pstDstHomeTsInfo->stTuningParam.sat.ucAntId			=	2;
#endif

	return ERR_OK;
}

HERROR	XMGR_SWUP_GetHomeTpInfo_Irdeto(DbSvc_TsInfo_t *pstHomeTsInfo)
{
	HERROR hError		=	ERR_FAIL;


	if (!pstHomeTsInfo)
	{
		HxLOG_Error("pstHomeTsInfo is NULL \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstHomeTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	// 정리 필요..
	return hError;
}

