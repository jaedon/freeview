/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_exe.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <oapi_exe.h>
#include <octo_common.h>

#include <svc_epg.h>
#if defined(CONFIG_OP_UK_DTT)
#include <svc_si.h>
#endif
#include <mgr_action.h>

#include <mgr_setting.h>
#include <mgr_search.h>
#if defined (CONFIG_MW_SATRECORD)
#include <mgr_satrecord.h>
#endif
#include <mgr_3rdparty.h>
#include <mgr_param.h>
#include <mgr_live.h>
#include <mgr_cas.h>
#include <om_common.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_OP_SES)
extern HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListIdFromDb (HUINT16 *pusSvcListId);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListVerFromDb (HUINT8 *pucSvcListVer);
extern HERROR XMGR_SEARCH_LCNUPDATE_CountLcnSvcList (HUINT32 *pulListNum);
extern HERROR XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo (HUINT32 ulListIdx, HUINT16 *pusSvcListId, HUINT8 *pucSvcListVer, HUINT8 *szSvcListName);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListNameFromDb (HUINT8 *szSvcListName);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnServiceNumberFromDb (HUINT32 *pulTvNum, HUINT32 *pulRadioNum);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnLastUpdatedTimeFromDb (UNIXTIME *putTime);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnLastCheckedTimeFromDb (UNIXTIME *putTime);
extern HERROR XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb(HUINT32 *pulValue);
extern HERROR XMGR_SEARCH_INFO_ClearSesLcnRelatedInfo (HBOOL bSyncDb);
#endif


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____

#define _____OAPI_COMMAND_FUNCTIONS_____

typedef enum
{
	eOmExe_CmdStart,
	eOmExe_CmdStop,
	eOmExe_CmdSet,
	eOmExe_CmdSetByBin,
	eOmExe_CmdEvtNoti,
	eOmExe_CmdNone
}omExe_CMD_e;

typedef struct
{
	HINT32 message;
	Handle_t hHandle;
	HINT32 p1;
	HINT32 p2;
	HINT32 p3;
}omExe_Param_t;


typedef HBOOL	(* omExe_Notifier_t) (const HCHAR * pFunction, omExe_Param_t *data);

typedef struct
{
	omExe_CMD_e eCmd;
	HCHAR procName[OxEXE_PROCNAME_LENGTH];
	HxRPC_Func_t	pfnMethod;
	omExe_Notifier_t pfIsNotiFunc;
}omExe_Method_t;

typedef struct
{
	HxVector_t * pExeMethod;
}omExe_Context_t;

static omExe_Context_t * s_astExeCtx = NULL;
STATIC HCHAR * om_exe_makeFuncName(omExe_CMD_e eCmd, HCHAR * pProcName);

STATIC HINT32 om_exe_PostMessage(HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t	hRes = BUS_PostMessage(NULL, nMsg, hAct, nParam1, nParam2, nParam3);

	return (hRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////// BATCH

STATIC HERROR om_exe_start_batch(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxBatchType_e eBatchType = (DxBatchType_e)HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	HxLOG_Print("om_exe_start_batch type[%d]\n", eBatchType);
	MGR_ACTION_StartBatchProc(eBatchType);
	return ERR_OK;
}

#define MAX_BATCHPROC 16
STATIC HERROR om_exe_set_batch(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxBatchProc_b pbBatchProc[MAX_BATCHPROC];
	HUINT32 * pBatchProc=NULL;
	HUINT32	ulSize = 0;
	HxLOG_Trace();


	memset(pbBatchProc, 0, MAX_BATCHPROC);
	HxLOG_Print("[om_exe_set_batch] nArgc[%d]\n", nArgc);

	if(nArgc != 0)
	{
		HUINT32 i = 0;
		DxBatchType_e eBatchType = (DxBatchType_e)HxOBJECT_INT(apArgv[0]);
		pBatchProc	= (HUINT32*)HxOBJECT_BIN_DATA (apArgv[0]);
		ulSize = (HUINT32)HxOBJECT_BIN_SIZE (apArgv[0]);
		eBatchType = *pBatchProc;
		pBatchProc++;	// bin 의 처음 부분에 eDBatchType 후 proc 정보
		ulSize -=sizeof(HUINT32);

		HxLOG_Print("[om_exe_set_batch] eBatchType = %d\n", eBatchType);
		while(ulSize > 0)
		{
			pbBatchProc[i] = *pBatchProc;
			ulSize -=sizeof(HUINT32);
			HxLOG_Print("[om_exe_set_batch] [%d]pbBatchProc = %d\n", i, pbBatchProc[i]);
			pBatchProc++;
			i++;
		}
		MGR_Action_BatchInstallProc(eBatchType,pbBatchProc);
	}
	return ERR_OK;
}

STATIC HBOOL om_exe_EvtProc_batch(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	switch (msg->message)
	{
	case eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS:
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		HxLOG_Print("[om_exe_set_batch] pFuncName[%s] hErr[%d] eBatchType = %d, proc = %d, result = %d\n", pFuncName, hErr, msg->p1, msg->p2, msg->p3);
		if (hErr == ERR_OK)
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iii", msg->p1/*DxBatchType_e*/, msg->p2/*DxBatchProc_b*/, msg->p3/*eERR*/);
		}
		break;
#if 0	// 추후 사용 예정.
	case eMEVT_ACTION_BATCH_RESULT:
		HxLOG_Print("[om_exe_set_batch] pFuncName[%s] hErr[%d] eBatchType = %d, proc = %d, result = %d\n", pFuncName, hErr, msg->p1, msg->p2, msg->p3);
		if (hErr == ERR_OK)
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iii", msg->p1/*DxBatchType_e*/, msg->p2/*DxBatchProc_b*/, msg->p3/*eProc Reason*/);
		}
#endif
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

/*	For example
STATIC HERROR om_exe_stop_batch(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Trace();
	return ERR_OK;
}

STATIC HBOOL om_exe_EvtProc_batch(omExe_Param_t * data, HINT32 *resultP1, HINT32 *resultP2, HINT32 *resultP3)
{
	return FALSE;
}
*/

STATIC void om_exe_cleanPathForObama(void)
{
	HxLOG_Warning("Warning!! Clean Path During Factory Default....\n");

	HLIB_FILE_Delete(TZ_NAME_PATH);
	HxLOG_Warning("Deleted %s...\n", TZ_NAME_PATH);

#if defined(CONFIG_MW_SI_LOCALTIME)
	HLIB_FILE_Delete(LOCALTIME_NAME_PATH);
	HxLOG_Warning("Deleted %s...\n", LOCALTIME_NAME_PATH);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////// Factory Default

STATIC HERROR om_exe_start_factoryDefault(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	DxFactoryDefault_type_e		 eFactoryDefaultType = HxOBJECT_INT(apArgv[0]);
	HCHAR						*pszFunctionName = NULL;
	HERROR						 hErr = ERR_FAIL;
	HINT32						 nRpcHandle = 0;
	HxLOG_Trace();

	HxLOG_Critical("[%s] nArgc[%d] Factory Type[%d] 0[%d] 1[%d] 2[%d]\n", __FUNCTION__, nArgc, eFactoryDefaultType, HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), HxOBJECT_INT(apArgv[2]));

	om_exe_cleanPathForObama();

	switch (eFactoryDefaultType)
	{
	case eDxFAC_DEF_TYPE_NONE:
		HxLOG_Critical("Factory Default Flag is not SET!!!!\n");
		return ERR_FAIL;

	case eDxFAC_DEF_TYPE_ALL:
	case eDxFAC_DEF_TYPE_KEEP:
		// 1. Clear SVC Table
		DB_SVC_ClearAllInfo();

		// 2. Set Factory Default Value
		hErr = MGR_PARAM_SetFactoryDefault(eFactoryDefaultType);
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("[%s] [%s] MGR_PARAM_SetFactoryDefault(%d) Fail [%d]\n", __FILE__, __FUNCTION__, eFactoryDefaultType, hErr);
		}

		// default setting related cas
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
		hErr = MGR_CAS_DefaultSetting();
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("MGR_CAS_DefaultSetting Fail [%d]\n", hErr);
		}
#endif
#if defined(CONFIG_MW_EPG_TVTV)
		XSVC_EPG_RemoveInfoDirectory_Tvtv();
#endif
		// TODO: SAMA로 대체.
		//MW_RSV_DeleteAllItemFromDB();
		break;

	default:
		HxLOG_Warning("Invalid eDefaultType [%d]!!!!\n", eFactoryDefaultType);
		break;
	}

	// Notify Factory Default Done
	pszFunctionName = om_exe_makeFuncName(eOmExe_CmdEvtNoti, OxEXE_PROC_FACTORYDEFAULT);
	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pszFunctionName, "i", 1);
	}
	else
	{
		HxLOG_Critical("[%s] [%s] Factory No Callback\n", __FILE__, __FUNCTION__);
	}

	if (pszFunctionName) HLIB_STD_MemFree(pszFunctionName);

	return ERR_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////// exportDB
#if defined(CONFIG_DB_SVC_IMPORT_EXPORT)
STATIC HERROR om_exe_start_exportDB(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL bSync = (HBOOL)HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	MGR_SETTING_ExportDb(bSync);
	return 0;
}

STATIC HERROR om_exe_start_importDB(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL bSync = (HBOOL)HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	MGR_SETTING_ImportDb(bSync);
	return 0;
}

STATIC HBOOL om_exe_EvtProc_imexPortDB(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HINT32			message;
	HINT32 			i32Error;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	message = msg->message;
	i32Error   = (HINT32)msg->p1;
	switch (message)
	{
	case eMEVT_SYSCTL_IMPORT_DB_OK:
		if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", 1, 0);
		break;

	case eMEVT_SYSCTL_IMPORT_DB_FAIL:
		if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", 1, i32Error);
		break;

	case eMEVT_SYSCTL_EXPORT_DB_OK:
		if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", 2, 0);
		break;

	case eMEVT_SYSCTL_EXPORT_DB_FAIL:
		if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", 2, i32Error);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

#endif

#if defined(CONFIG_OP_SES)
///////////////////////////////////////////////////////////////////////////////////////////////// LCN UPDATE
STATIC HERROR om_exe_start_lcnUpdate(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 ulSearchMode = (HUINT32)HxOBJECT_INT(apArgv[0]);

	HxLOG_Trace();

	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, MGR_SEARCH_GetLcnUpdateSearchMainProc(), HANDLE_NULL, ulSearchMode, 0, 0) == ERR_OK)
	{
		BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchMainProc(), eMEVT_LCNUPDATE_CMD_START, HANDLE_NULL, 0, 0, 0);
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR om_exe_save_lcnUpdate(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32			ulSearchMode = (HUINT32)HxOBJECT_INT(apArgv[0]);
	HUINT32 		ulSelectedIdx = (HBOOL)HxOBJECT_INT(apArgv[1]);

	HxLOG_Trace();

	if (BUS_MGR_Get(MGR_SEARCH_GetLcnUpdateSearchMainProc()) != NULL)
	{
		BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchMainProc(), eMEVT_LCNUPDATE_CMD_SAVE, HANDLE_NULL, ulSearchMode, ulSelectedIdx, 0);
	}

	return ERR_OK;
}

STATIC HERROR om_exe_stop_lcnUpdate(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Trace();

	if (BUS_MGR_Get(MGR_SEARCH_GetLcnUpdateSearchMainProc()) != NULL)
	{
		BUS_SendMessage(MGR_SEARCH_GetLcnUpdateSearchMainProc(), eMEVT_LCNUPDATE_CMD_STOP, HANDLE_NULL, 0, 0, 0);
	}

	return ERR_OK;
}

STATIC HERROR om_exe_get_lcnUpdateInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR				hErr = ERR_FAIL;
	HxDATETIME_t 		stUpdatedTime, stCheckedTime;
	UNIXTIME			utUpdatedTime, utCheckedTime;

	DxLcnUpdateInfo_t	*pstLcnUpdateInfo = NULL;

	HxLOG_Trace();

	pstLcnUpdateInfo = (DxLcnUpdateInfo_t*)HLIB_STD_MemAlloc(sizeof(DxLcnUpdateInfo_t));
	HxSTD_memset(pstLcnUpdateInfo, 0, sizeof(DxLcnUpdateInfo_t));

	// service list name
	hErr = XMGR_SEARCH_INFO_GetSesLcnServiceListNameFromDb(pstLcnUpdateInfo->szServiceListName);
	if(hErr != ERR_OK)
	{
		MWC_UTIL_DvbStrcpy (pstLcnUpdateInfo->szServiceListName, "");
	}

	// tv num, radio num
	hErr = XMGR_SEARCH_INFO_GetSesLcnServiceNumberFromDb(&pstLcnUpdateInfo->ulTvNum, &pstLcnUpdateInfo->ulRadioNum);
	if(hErr != ERR_OK)
	{
		pstLcnUpdateInfo->ulTvNum = 0;
		pstLcnUpdateInfo->ulRadioNum = 0;
	}

	// last update time
	hErr = XMGR_SEARCH_INFO_GetSesLcnLastUpdatedTimeFromDb(&utUpdatedTime);
	if(hErr != ERR_OK)
	{
		utUpdatedTime = 0;
	}

	// last check time
	hErr = XMGR_SEARCH_INFO_GetSesLcnLastCheckedTimeFromDb(&utCheckedTime);
	if(hErr != ERR_OK)
	{
		utCheckedTime = 0;
	}

	// Check Time 이 Update Time 보다 작을 리가 없다.
	if (utCheckedTime < utUpdatedTime)
	{
		utCheckedTime = utUpdatedTime;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utUpdatedTime, &stUpdatedTime);

	HxSTD_PrintToStrN(pstLcnUpdateInfo->szLastUpdatedTime, 20, "%d:%02d %02d.%02d.%04d",
													stUpdatedTime.stTime.ucHour, stUpdatedTime.stTime.ucMinute,
													stUpdatedTime.stDate.ucDay, stUpdatedTime.stDate.ucMonth,
													stUpdatedTime.stDate.usYear);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utCheckedTime, &stCheckedTime);

	HxSTD_PrintToStrN(pstLcnUpdateInfo->szLastCheckedTime, 20, "%d:%02d %02d.%02d.%04d",
													stCheckedTime.stTime.ucHour, stCheckedTime.stTime.ucMinute,
													stCheckedTime.stDate.ucDay, stCheckedTime.stDate.ucMonth,
													stCheckedTime.stDate.usYear);

	pstLcnUpdateInfo->bDisplayInfo = (utUpdatedTime != 0) ? TRUE : FALSE;

	// Coldboot result
	hErr = XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb(&pstLcnUpdateInfo->ulColdbootResult);
	if(hErr != ERR_OK)
	{
		pstLcnUpdateInfo->ulColdbootResult = 0;
	}

	HLIB_CMD_Printf("\t\t+++++ [%s:%d] service list name : %s, tvnum : %d, radionum : %d\n", __FUNCTION__, __LINE__,
									pstLcnUpdateInfo->szServiceListName, pstLcnUpdateInfo->ulTvNum, pstLcnUpdateInfo->ulRadioNum);
	HLIB_CMD_Printf("\t\t+++++ [%s:%d] updated time : %s, checked time : %s\n",__FUNCTION__, __LINE__,
									pstLcnUpdateInfo->szLastUpdatedTime, pstLcnUpdateInfo->szLastCheckedTime);
	HLIB_CMD_Printf("\t\t+++++ [%s:%d] bDisplayInfo : %d, ulColdbootResult : %d\n", __FUNCTION__, __LINE__, pstLcnUpdateInfo->bDisplayInfo, pstLcnUpdateInfo->ulColdbootResult);

	BUS_PostMessage(NULL, eOEVT_LCNUPDATE_GET_INFO, HANDLE_NULL, (HUINT32)pstLcnUpdateInfo, 0, 0);

	return ERR_OK;
}

STATIC HERROR om_exe_remove_lcnUpdateInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL bSyncMode = (HUINT32)HxOBJECT_INT(apArgv[0]);

	HxLOG_Trace();

	XMGR_SEARCH_INFO_ClearSesLcnRelatedInfo(bSyncMode);

	return ERR_OK;
}

STATIC HBOOL om_exe_EvtProc_lcnUpdate(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	HUINT32			*tempPtr = data;
	HUINT32			svcListSize = 0, newSvcSize = 0, replacedSvcSize = 0;
	HINT32			message = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	message = msg->message;
	switch (message)
	{
	// OK
	case eMEVT_LCNUPDATE_START_OK:
		{
			DxLcnUpdateStartData_t		*pstStartData = (DxLcnUpdateStartData_t *)msg->p1;
			DxLcnUpdateServiceList_t	*svcList = NULL;

			if (pstStartData == NULL)
			{
				HxLOG_Error("[%s, %d] pstStartData(p1) is null! \n", __FUNCTION__, __LINE__);
				return FALSE;
			}

			HxLOG_Debug("[%s,%d]ReceivedMsg [eMEVT_LCNUPDATE_START_OK] NumLcnList : %d\n",
						__FUNCTION__,__LINE__,pstStartData->ulSvcListNum);

			svcList = (pstStartData->ulSvcListNum == 0) ? (DxLcnUpdateServiceList_t*)tempPtr : pstStartData->svcList;
			svcListSize = (pstStartData->ulSvcListNum == 0) ? sizeof(tempPtr) : sizeof(DxLcnUpdateServiceList_t) * pstStartData->ulSvcListNum;

			HxLOG_Debug("svcList : 0x%x, svcListSize : %d\n", svcList, svcListSize);

			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iib",
								eDxLCNUPDATE_START_OK, pstStartData->ulSvcListNum, svcList, svcListSize);

			if (pstStartData->svcList)			HLIB_MEM_Free(pstStartData->svcList);
			if (pstStartData != NULL)					HLIB_MEM_Free(pstStartData);
		}
		break;

	case eMEVT_LCNUPDATE_SAVE_OK:
		{
			DxLcnUpdateSaveData_t		*pstSaveData = (DxLcnUpdateSaveData_t *)msg->p1;
			DxLcnUpdateService_t		*newSvc = NULL, *replacedSvc = NULL;

			if (pstSaveData == NULL)
			{
				HxLOG_Error("[%s, %d] pstSaveData(p1) is null! \n", __FUNCTION__, __LINE__);
				return FALSE;
			}

			HxLOG_Debug("[%s,%d]ReceivedMsg [eMEVT_LCNUPDATE_SAVE_OK] new service : %d, replaced service : %d\n",
						__FUNCTION__, __LINE__, pstSaveData->ulNewSvcNum, pstSaveData->ulReplacedSvcNum);

			newSvc = (pstSaveData->ulNewSvcNum == 0) ? (DxLcnUpdateService_t*)tempPtr : pstSaveData->newSvc;
			newSvcSize = (pstSaveData->ulNewSvcNum == 0) ? sizeof(tempPtr) : sizeof(DxLcnUpdateService_t) * pstSaveData->ulNewSvcNum;

			replacedSvc = (pstSaveData->ulReplacedSvcNum == 0) ? (DxLcnUpdateService_t*)tempPtr : pstSaveData->replacedSvc;
			replacedSvcSize = (pstSaveData->ulReplacedSvcNum == 0) ? sizeof(tempPtr) : sizeof(DxLcnUpdateService_t) * pstSaveData->ulReplacedSvcNum;

			HxLOG_Debug("newSvc : 0x%x, newSvcSize : %d\n", newSvc, newSvcSize);
			HxLOG_Debug("replacedSvc : 0x%x, replacedSvcSize : %d\n", replacedSvc, replacedSvcSize);

			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iibib",
								eDxLCNUPDATE_SAVE_OK, pstSaveData->ulNewSvcNum, newSvc, newSvcSize,
								pstSaveData->ulReplacedSvcNum, replacedSvc, replacedSvcSize);

			if (pstSaveData->newSvc)			HLIB_MEM_Free(pstSaveData->newSvc);
			if (pstSaveData->replacedSvc)					HLIB_MEM_Free(pstSaveData->replacedSvc);
			if (pstSaveData != NULL)					HLIB_MEM_Free(pstSaveData);
		}
		break;

	case eMEVT_LCNUPDATE_STOP_OK:
		{
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", eDxLCNUPDATE_STOP_OK, 0);
		}
		break;

	// Fail
	case eMEVT_LCNUPDATE_START_FAIL:
		{
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", eDxLCNUPDATE_START_FAIL, 0);
		}
		break;

	case eMEVT_LCNUPDATE_SAVE_FAIL:
		{
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", eDxLCNUPDATE_SAVE_FAIL, 0);
		}
		break;

	case eMEVT_LCNUPDATE_STOP_FAIL:
		{
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ii", eDxLCNUPDATE_STOP_FAIL, 0);
		}
		break;

	case eOEVT_LCNUPDATE_GET_INFO:
		{
			DxLcnUpdateInfo_t		*pstLcnUpdateInfo = (DxLcnUpdateInfo_t *)msg->p1;

			if(nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ib", eDxLCNUPDATE_GET_INFO, pstLcnUpdateInfo, sizeof(DxLcnUpdateInfo_t));

			if(pstLcnUpdateInfo)					HLIB_MEM_Free(pstLcnUpdateInfo);
		}
		break;

	default:
		return FALSE;
		break;
	}
	return TRUE;
}
#endif

#if defined(CONFIG_MW_EPG_TVTV)
#ifndef EPG_FS_TVTV_PATH
#define EPG_FS_TVTV_PATH						"/mnt/hd1/TVTV/"
#endif
STATIC HERROR om_exe_init_TVTVEPGDirectory(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
//	HCHAR szCmd[128] = { 0 };

	//	DAMA에서 만든걸 왜 OBAMA에서 바로 지울려고 하지???
#if	0
	HxLOG_Trace();
	HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", EPG_FS_TVTV_PATH);
	VK_SYSTEM_Command (szCmd);
	HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", DLIB_EPG_TVTV_GetDBFullName(0)); //DLIB_TVTVEPG_DBFILE
	VK_SYSTEM_Command (szCmd);
	HxSTD_snprintf( szCmd, sizeof(szCmd), "rm -rf %s", DLIB_SVC_TVTV_GetDBFullName(0));//DLIB_TVTVSVC_DBFILE
	VK_SYSTEM_Command (szCmd);
#endif

	return 0;
}

STATIC HERROR om_exe_init_TVTVEPGRemoveInfoDirectory(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Trace();

	return XSVC_EPG_RemoveInfoDirectory_Tvtv();
}

STATIC HERROR om_exe_init_LoadBaseEPG(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 	actionId;
	Handle_t	hAction;
	Handle_t	hService;
	HERROR		herr ;

	HxLOG_Trace();

	actionId = MGR_ACTION_GetMainActionId();
	if (MGR_ACTION_GetType(actionId) != eViewType_WTV)
	{
		HxLOG_Debug("\n%s() only view action available!\n", __FUNCTION__);
		return ERR_FAIL;
	}
	hAction  = SVC_PIPE_GetActionHandle(actionId);

	herr = MGR_LIVE_CurrentSvcInfoGet(actionId, &hService);
	if (herr != ERR_OK)
	{
		HxLOG_Debug("MGR_LIVE_CurrentSvcInfoGet error !\n");
		return ERR_FAIL;
	}

	herr = SVC_EPG_Stop (eEPG_KIND_TVTV, eEPG_EVT_KIND_ALL , hAction);
	herr |= SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL , hAction, hService, 0, 0, 0);
	herr |= SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, hAction, hService, 0, 0, 0);
	if( herr != ERR_OK)
	{
		HxLOG_Debug("%s() SVC_EPG Fail !\n", __FUNCTION__);
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR om_exe_init_LoadTVTVEPG(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
//	HUINT32 	actionId;
//	Handle_t	hAction;
//	Handle_t	hService;
//	HERROR		herr ;

	HxLOG_Trace();



	//hError = SVC_EPG_UnLoadData(eEPG_KIND_DVB, HANDLE_NULL);
	//hError= SVC_EPG_LoadData(eEPG_KIND_TVTV, HANDLE_NULL, HANDLE_NULL);
	return 0;
}

#endif

#if defined(CONFIG_MW_SATRECORD)
STATIC HERROR om_exe_start_SatRecord(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Trace();

	if (BUS_MGR_Get(MGR_SATRECORD_GetDownloadAction()) == NULL)
	{
		MGR_SATRECORD_StartDownloadSatRec();
	}
	return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////// MHEG
#if 0 // defined(CONFIG_3RD_MHEG)

STATIC HERROR om_exe_start_mheg(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT32 nWndId = HxOBJECT_INT(apArgv[0]);
	HUINT32 nStillWndId = HxOBJECT_INT(apArgv[1]);
	HxLOG_Trace();

	AP_Mheg_Init(nWndId, nStillWndId, om_exe_PostMessage);
	BUS_MGR_Create("PROC_MHEG_Manger", APP_MHEG_MGR_PRIORITY, MGR_MHEG_GetProc(), 0, 0, 0, 0);
	return ERR_OK;
}

STATIC HERROR om_exe_set_mheg_key(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 key = HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	if(BUS_MGR_Get(MGR_MHEG_GetProc()) != NULL)
	{
		BUS_PostMessage(NULL, eOEVT_MHEG_KEYDOWN, (Handle_t)NULL, key, 0, 0);
	}
	return ERR_OK;
}

STATIC HERROR om_exe_set_mheg_deeplink(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HCHAR *pszURL = HxOBJECT_STR(apArgv[0]);
	HUINT32 prevLcn = HxOBJECT_INT(apArgv[1]);
	HxLOG_Trace();
	if(pszURL)	AP_Mheg_SetDeepLinkURL(pszURL, prevLcn);
	return ERR_OK;
}

STATIC HERROR om_exe_set_mheg_opctrl(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32 bControl = HxOBJECT_INT(apArgv[0]);
	HxLOG_Trace();
	AP_Mheg_SetMhegOperation(bControl);
	return ERR_OK;
}

STATIC HBOOL om_exe_EvtProc_mheg(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HUINT32 param1, param2, param3, param4;
	DxMhegEvent_e eMhegEvent = eDxMHEGEVT_NONE;
	DxRect_t	*pstRect = NULL;
	HINT32			message = msg->message;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	switch (message)
	{
		case eMEVT_MHEG_TYPE_LAUNCH_APPLICATION:
			eMhegEvent = eDxMHEGEVT_LAUNCHAPP;
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "is", eMhegEvent, msg->p3);
			break;
		case eMEVT_MHEG_TYPE_FLIP_WINDOW:
			eMhegEvent = eDxMHEGEVT_FLIPWIN;
			pstRect = (DxRect_t*)msg->p3;
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iiiii", eMhegEvent, msg->p1, pstRect->nX, pstRect->nY, pstRect->nW, pstRect->nH);
			break;
		case eMEVT_MHEG_TYPE_MHEG_APPSTATE:
			eMhegEvent = eDxMHEGEVT_APPSTATE;
			if (nRpcHandle) HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "i", eMhegEvent, msg->p1, msg->p2);
			break;
		default:
			return FALSE;
			break;
	}
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_SUPPORT_MHEG_IB)
//	apArgv[0] : sessionid (i)
//	apArgv[1] : nReqNameLen (i)
//	apArgv[2] : szReqName (s)
STATIC HERROR	om_exe_set_mhegib_requestfile(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return MGR_MHEGIB_RequestFileOrData(HxOBJECT_INT(apArgv[0]), 0/*eCiAmmi_ReqType_File*/, HxOBJECT_INT(apArgv[1]), HxOBJECT_STR(apArgv[2]));
}

//	apArgv[0] : sessionid (i)
//	apArgv[1] : nReqNameLen (i)
//	apArgv[2] : szReqName (s)
STATIC HERROR	om_exe_set_mhegib_requestdate(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return MGR_MHEGIB_RequestFileOrData(HxOBJECT_INT(apArgv[0]), 1/*eCiAmmi_ReqType_Date*/, HxOBJECT_INT(apArgv[1]), HxOBJECT_STR(apArgv[2]));
}

//	apArgv[0] : sessionid (i)
STATIC HERROR	om_exe_set_mhegib_abortbyuser(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return MGR_MHEGIB_RequestAppAbort(HxOBJECT_INT(apArgv[0]), 1/*CICtrlAmmiAbortRequestCode_UserCancel*/);
}

//	apArgv[0] : sessionid (i)
STATIC HERROR	om_exe_set_mhegib_abortbysystem(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	return MGR_MHEGIB_RequestAppAbort(HxOBJECT_INT(apArgv[0]), 2/*CICtrlAmmiAbortRequestCode_SystemCancel*/);
}

#endif

#if defined(CONFIG_OP_UK_DTT)
STATIC HERROR om_exe_get_trdconflictInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR			hError = ERR_FAIL;
	DxTrdConflictList_t 	*pstTrdConflictInfo = NULL;

	HUINT32				ulActionId;
	HUINT32 			ulTrdConflictCount = 0;
	HxList_t			*pConflictList = NULL;
	HxList_t			*pstList;
	HUINT8 				*pstTrdSvcInfo = NULL;
	Handle_t			hAction;
	HUINT32				i;
	HUINT32 			ulLen = 0;

	HxLOG_Trace();
	ulActionId = MGR_ACTION_GetMainActionId();
	hAction  = SVC_PIPE_GetActionHandle(ulActionId);

	pstTrdConflictInfo = (DxTrdConflictList_t*)HLIB_STD_MemCalloc(sizeof(DxTrdConflictList_t));

	if(pstTrdConflictInfo == NULL)
	{
		HxLOG_Error("pstTrdConflictInfo MemCalloc Error \n" );
		BUS_PostMessage(NULL, eMEVT_TRDCONFLICTED_GET_INFO, HANDLE_NULL, (HUINT32)0/*ulTrdConflictCount*/, (HUINT32)pstTrdConflictInfo, 0);

		return ERR_FAIL;
	}

	hError = SVC_SI_GetTrdConflictedList(hAction,HxOBJECT_INT(apArgv[0]), &ulTrdConflictCount,&pConflictList);

	HxLOG_Print(" G o G o %d \n", ulTrdConflictCount);

	if(hError == ERR_FAIL || ulTrdConflictCount == 0 || pConflictList == NULL)
	{
		HxLOG_Error("SVC_SI_GetTrdConflictedList Error  == hError(%d) , ulTrdConflictCount(%d) pConflictList (%p) \n" , hError, ulTrdConflictCount, pConflictList);
		BUS_PostMessage(NULL, eMEVT_TRDCONFLICTED_GET_INFO, HANDLE_NULL, (HUINT32)0/*ulTrdConflictCount*/, (HUINT32)pstTrdConflictInfo, 0);

		if(pConflictList != NULL)
			HLIB_LIST_RemoveAll(pConflictList);

		return ERR_FAIL;
	}

	pstList = HLIB_LIST_First(pConflictList);
	while(pstList)
	{
		pstTrdSvcInfo = (HUINT8 *)HLIB_LIST_Data(pstList);
		ulLen = HxSTD_StrLen(pstTrdSvcInfo);
		if(ulLen > 0)
		{
			MWC_UTIL_DvbStrcpy(pstTrdConflictInfo->stTrdConflictedArray[pstTrdConflictInfo->ulArrayNum].szTrdConflictedSvcName,pstTrdSvcInfo);
			pstTrdConflictInfo->stTrdConflictedArray[pstTrdConflictInfo->ulArrayNum].ulLen = ulLen;
			pstTrdConflictInfo->ulArrayNum++;
		}
		pstList = pstList->next;
	}

	HLIB_LIST_RemoveAll(pConflictList);

	HxLOG_Print("\t\t+++++ [%s:%d] pstTrdConflictInfo->ulArrayNum : %d\n", __FUNCTION__, __LINE__,pstTrdConflictInfo->ulArrayNum);
	for(i = 0;i<pstTrdConflictInfo->ulArrayNum;i++)
	{
		HxLOG_Print("\t\t+++++ [%s:%d] array[%d] : %s\n",__FUNCTION__, __LINE__,
										i,pstTrdConflictInfo->stTrdConflictedArray[pstTrdConflictInfo->ulArrayNum].szTrdConflictedSvcName);
	}

	BUS_PostMessage(NULL, eMEVT_TRDCONFLICTED_GET_INFO, HANDLE_NULL, (HUINT32)pstTrdConflictInfo->ulArrayNum, (HUINT32)pstTrdConflictInfo, 0);

	return ERR_OK;
}

STATIC HERROR om_exe_set_trdconflictInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR			hError = ERR_FAIL;

	hError = SVC_SI_SetTrdConflictedList(HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]));
	return ERR_OK;
}

STATIC HBOOL om_exe_EvtProc_trdconflict(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	switch (msg->message)
	{
	case eMEVT_TRDCONFLICTED_GET_INFO:
	{
		HUINT32 ulTrdConflictCount = msg->p1;
		DxTrdConflictList_t *pstTrdConflictInfo = (DxTrdConflictList_t *)msg->p2;
		DxTrdConflictList_t 	stTrdConflictInfo;

		HxSTD_MemSet(&stTrdConflictInfo,0x00,sizeof(DxTrdConflictList_t));

		if(nRpcHandle)
		{
			if(pstTrdConflictInfo == NULL)
			{
				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iib", eDxTRDCONFLICT_GET_INFO, ulTrdConflictCount,&stTrdConflictInfo, sizeof(DxTrdConflictList_t));
			}
			else
			{
				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "iib", eDxTRDCONFLICT_GET_INFO, ulTrdConflictCount,pstTrdConflictInfo, sizeof(DxTrdConflictList_t));

				if(pstTrdConflictInfo)					HLIB_MEM_Free(pstTrdConflictInfo);
			}
		}
	}
		break;


	default:
		return FALSE;
		break;
	}
	return TRUE;
}
#endif

STATIC HERROR om_exe_get_ncdInfo(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{

	DxNcdNetworkInfo_t *pstNetworkMsg = NULL;
	HERROR	hErr = ERR_FAIL;

	pstNetworkMsg = (DxNcdNetworkInfo_t*)HLIB_STD_MemAlloc(sizeof(DxNcdNetworkInfo_t));

	if(pstNetworkMsg == NULL)
	{
		HxLOG_Error("pstNetworkMsg MemAlloc Error \n" );
		BUS_PostMessage(NULL, eMEVT_NCD_GET_INFO, HANDLE_NULL, (HUINT32)pstNetworkMsg, 0, 0);

		return ERR_FAIL;
	}


	HxSTD_memset(pstNetworkMsg, 0x00, sizeof(DxNcdNetworkInfo_t));
	pstNetworkMsg->eventType = eDxNCDEVT_GET_INFO;

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, (HUINT32 *)pstNetworkMsg->szNcdnetworkMsg, (HUINT32)DxNCD_MAX_MESSAGE_LEN);

	if(hErr == ERR_OK)
	{
		BUS_PostMessage(NULL, eMEVT_NCD_GET_INFO, HANDLE_NULL, (HUINT32)pstNetworkMsg, 0, 0);
	}
	else
	{
		if(pstNetworkMsg)					HLIB_MEM_Free(pstNetworkMsg);
		BUS_PostMessage(NULL, eMEVT_NCD_GET_INFO, HANDLE_NULL, (HUINT32)NULL, 0, 0);
		return ERR_FAIL;
	}
	return ERR_OK;
}
STATIC HBOOL om_exe_EvtProc_ncdinfo(const HCHAR * pFuncName , omExe_Param_t * data)
{
	omExe_Param_t * msg = data;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	switch (msg->message)
	{
		case eMEVT_NCD_GET_INFO:
		{
			DxNcdNetworkInfo_t *pstNetworkMsg = (DxNcdNetworkInfo_t *)msg->p1;
			DxNcdNetworkInfo_t 	stNetworkMsg;

			HxSTD_MemSet(&stNetworkMsg,0x00,sizeof(DxNcdNetworkInfo_t));

			if(nRpcHandle)
			{
				if(pstNetworkMsg == NULL)
				{
					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ib", eDxNCDEVT_GET_INFO, &stNetworkMsg,sizeof(DxNcdNetworkInfo_t));
				}
				else
				{
					HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle, pFuncName, "ib", eDxNCDEVT_GET_INFO, pstNetworkMsg, sizeof(DxNcdNetworkInfo_t));

					if(pstNetworkMsg)					HLIB_MEM_Free(pstNetworkMsg);
				}
			}
		}
			break;


		default:
			return FALSE;
			break;
	}
	return TRUE;
}


STATIC HERROR om_exe_set_NcdNotifyData(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR			hError = ERR_FAIL;

	hError = SVC_SI_SetNcdNotifyData(HxOBJECT_INT(apArgv[0]));
	if (hError != ERR_OK)
	{
		HxLOG_Error("SVC_SI_SetNcdNotifyData failed. \n");
	}
	return ERR_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static omExe_Context_t * om_exe_GetMgr(void)
{
	//HxLOG_Trace();

	if(s_astExeCtx == NULL)
	{
		s_astExeCtx = (omExe_Context_t *)HLIB_STD_MemCalloc(sizeof(omExe_Context_t));
		s_astExeCtx->pExeMethod = NULL;
	}

	return s_astExeCtx;
}

static HERROR om_exe_AddMethod(HCHAR *procName, HxRPC_Func_t pfnMethod, omExe_CMD_e eCmd, omExe_Notifier_t pFnNoti)
{
	omExe_Method_t * stExeMethod = NULL;
	omExe_Context_t * stExeMgr = om_exe_GetMgr();
	HxLOG_Trace();

	if(stExeMgr)
	{
		if(stExeMgr->pExeMethod == NULL)
			stExeMgr->pExeMethod = HLIB_VECTOR_New(NULL,NULL);

		stExeMethod = (omExe_Method_t *)HLIB_STD_MemCalloc(sizeof(omExe_Method_t));
		stExeMethod->pfnMethod = pfnMethod;
		stExeMethod->pfIsNotiFunc = pFnNoti;
		snprintf (stExeMethod->procName, OxEXE_PROCNAME_LENGTH, procName);
		stExeMethod->eCmd = eCmd;
		HLIB_VECTOR_Add(stExeMgr->pExeMethod,stExeMethod);
		return ERR_OK;
	}
	else
	{
		HxLOG_Critical("ERROR stExeMgr is Null\n");
	}
	return ERR_FAIL;
}

static HCHAR * om_exe_makeFuncName(omExe_CMD_e eCmd, HCHAR * pProcName)
{
	HCHAR * pFunctionName = NULL;
	HCHAR * pPrefixName = NULL;
	HUINT32 size = 0;

	if(eCmd == eOmExe_CmdStart)
		pPrefixName = RPC_OAPI_EXE_START;
	else if(eCmd == eOmExe_CmdStop)
		pPrefixName = RPC_OAPI_EXE_STOP;
	else if(eCmd == eOmExe_CmdSet)
		pPrefixName = RPC_OAPI_EXE_SET;
	else if(eCmd == eOmExe_CmdSetByBin)
		pPrefixName = RPC_OAPI_EXE_SETBIN;
	else if(eCmd == eOmExe_CmdEvtNoti)
		pPrefixName = RPC_OAPI_EXE_NOTI;
	else
		pPrefixName = "";

	size = HxSTD_StrLen(pProcName) + HxSTD_StrLen(pPrefixName) + 2/* NULL + . */;
	if(size > OxEXE_PROCNAME_LENGTH)
		size = OxEXE_PROCNAME_LENGTH;

	pFunctionName = (HCHAR *)HLIB_STD_MemCalloc(size);
	snprintf(pFunctionName, size, "%s.%s",pPrefixName,  pProcName);
	return pFunctionName;
}

static HBOOL om_exe_registMethod(void *data, void *userdata)
{
	omExe_Method_t * method = (omExe_Method_t *)data;
	HCHAR * pFunctionName = NULL;
	HERROR		hErr = ERR_FAIL;
	HINT32		nRpcHandle = 0;
	HxLOG_Trace();

	pFunctionName = om_exe_makeFuncName(method->eCmd, method->procName);
	HxLOG_Debug("Regist Exe Method Name[%s]\n", pFunctionName);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if(hErr == ERR_OK)
	{
		if(method->eCmd == eOmExe_CmdStart || method->eCmd == eOmExe_CmdStop || method->eCmd == eOmExe_CmdSet)
		{
			HLIB_RPC_RegisterCall (nRpcHandle, pFunctionName, "iii", (HxRPC_Func_t)method->pfnMethod, NULL,
									"exe Proc : (HINT32 param1, HINT32 param2, HINT32 param3)");
		}
		else if(method->eCmd == eOmExe_CmdSetByBin)
		{
			HLIB_RPC_RegisterCall (nRpcHandle, pFunctionName, "b", (HxRPC_Func_t)method->pfnMethod, NULL,
									"exe Proc : (void * pBin)");
	 	}
		else if(method->eCmd == eOmExe_CmdEvtNoti)
		{
			HLIB_RPC_CreateNotification (nRpcHandle, pFunctionName, NULL, "Proc Notifier.\n""\t  - ?????\n");
		}
	}

	if(pFunctionName) HLIB_STD_MemFree(pFunctionName);
	return FALSE;
}

static HERROR om_exe_RegistMethod(void)
{
	omExe_Context_t * stExeMgr = om_exe_GetMgr();
	HxLOG_Trace();
	if(stExeMgr && stExeMgr->pExeMethod)
	{
		if(stExeMgr->pExeMethod != NULL)
		{
			HxLOG_Debug("stExeMgr->pExeMethod count [%d]\n",	HLIB_VECTOR_Length(stExeMgr->pExeMethod));
		}
		else
		{
			HxLOG_Critical("stExeMgr->pExeMethod count 000000\n");
		}
		HLIB_VECTOR_ForEach(stExeMgr->pExeMethod, om_exe_registMethod, NULL);
	}
	return ERR_OK;
}

#define _____PROCEDURE_____

#define ___GLOBAL_FUNCTIONS___

static HBOOL om_exe_proc_noti(void *data, void *userdata)
{
	omExe_Method_t * method = (omExe_Method_t *)data;

	if(method->eCmd == eOmExe_CmdEvtNoti && method->pfIsNotiFunc)
	{
		HCHAR * pFuncName = om_exe_makeFuncName(method->eCmd, method->procName);
		HBOOL err =	method->pfIsNotiFunc(pFuncName, (omExe_Param_t *)userdata);
		if(pFuncName) HLIB_STD_MemFree(pFuncName);
		return err;
	}
	return FALSE;
}


static BUS_Result_t om_exe_SendNotification(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	omExe_Context_t * stExeMgr = om_exe_GetMgr();
	omExe_Param_t data;

	if(stExeMgr && stExeMgr->pExeMethod)
	{
		data.message = message;
		data.hHandle = hHandle;
		data.p1 = p1;
		data.p2 = p2;
		data.p3 = p3;

		HLIB_VECTOR_ForEach(stExeMgr->pExeMethod, om_exe_proc_noti, &data);
		// if the return is false, It will be continue next proc.
	}
	return MESSAGE_PASS;
}

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_exe_notifier(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

			//	WHY???? --> why you get all message from bus???
			//	PLZ fix it.
		case eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS:
#if defined(CONFIG_3RD_BML)
		case eMEVT_BML_VOD_START:
#endif
#if defined(CONFIG_3RD_RFVOD)
		case eMEVT_RFVOD_DESTROYED:
#endif

			om_exe_SendNotification(message, hHandle, p1, p2, p3);
			break;

#if 0 // defined(CONFIG_3RD_MHEG)
		case eMEVT_MHEG_TYPE_LAUNCH_APPLICATION:
		case eMEVT_MHEG_TYPE_FLIP_WINDOW:
		case eMEVT_MHEG_TYPE_MHEG_APPSTATE:
#endif

		case eMEVT_SYSCTL_IMPORT_DB_FAIL:
		case eMEVT_SYSCTL_IMPORT_DB_OK:
		case eMEVT_SYSCTL_EXPORT_DB_FAIL:
		case eMEVT_SYSCTL_EXPORT_DB_OK:
#if defined(CONFIG_OP_SES)
		case eMEVT_LCNUPDATE_START_OK:
		case eMEVT_LCNUPDATE_START_FAIL:
		case eMEVT_LCNUPDATE_STOP_OK:
		case eMEVT_LCNUPDATE_STOP_FAIL:
		case eMEVT_LCNUPDATE_SAVE_OK:
		case eMEVT_LCNUPDATE_SAVE_FAIL:
		case eOEVT_LCNUPDATE_GET_INFO:
#endif
#if defined(CONFIG_OP_UK_DTT)
		case eMEVT_TRDCONFLICTED_GET_INFO:
		case eMEVT_NCD_GET_INFO:
#endif
			om_exe_SendNotification(message, hHandle, p1, p2, p3);
			break;

		default:
			break;
	}

	return MESSAGE_PASS;
}



HERROR OM_EXE_Init(void)
{
	HxLOG_Trace();
	om_exe_AddMethod(OxEXE_PROC_BATCH , om_exe_start_batch, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_BATCH , om_exe_set_batch, eOmExe_CmdSetByBin, NULL);
	om_exe_AddMethod(OxEXE_PROC_BATCH, 	NULL, eOmExe_CmdEvtNoti, om_exe_EvtProc_batch);

#if 0	// warning 제거 freesat 이 삭제 됨.
	om_exe_AddMethod(OxEXE_PROC_FREESATEPG , om_exe_start_freesatEPG, eOmExe_CmdStart, NULL);
#endif

	om_exe_AddMethod(OxEXE_PROC_FACTORYDEFAULT , om_exe_start_factoryDefault, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_FACTORYDEFAULT , NULL, eOmExe_CmdEvtNoti, NULL);

#if defined(CONFIG_DB_SVC_IMPORT_EXPORT)
	om_exe_AddMethod(OxEXE_PROC_EXPORTDB, 		om_exe_start_exportDB, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_IMPORTDB, 		om_exe_start_importDB, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_IMEXPORTDB, 	NULL, eOmExe_CmdEvtNoti, om_exe_EvtProc_imexPortDB);
#endif

#if defined(CONFIG_OP_SES)
	om_exe_AddMethod(OxEXE_PROC_STARTLCNUPDATE, 		om_exe_start_lcnUpdate, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_STOPLCNUPDATE, 			om_exe_stop_lcnUpdate, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_SAVELCNUPDATE, 			om_exe_save_lcnUpdate, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_GETLCNUPDATEINFO, 		om_exe_get_lcnUpdateInfo, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_REMOVELCNUPDATEINFO, 	om_exe_remove_lcnUpdateInfo, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_LCNUPDATE, 	NULL, eOmExe_CmdEvtNoti, om_exe_EvtProc_lcnUpdate);
#endif

#if defined(CONFIG_MW_EPG_TVTV)
	om_exe_AddMethod(OxEXE_PROC_TVTVEPGREMOVEINFODIRECTORY, 		om_exe_init_TVTVEPGRemoveInfoDirectory, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_TVTVEPGINITDIRECTORY, 	om_exe_init_TVTVEPGDirectory, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_LOADBASEEPG, 			om_exe_init_LoadBaseEPG, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_LOADTVTVEPG, 			om_exe_init_LoadTVTVEPG, eOmExe_CmdStart, NULL);
#endif
#if defined(CONFIG_MW_SATRECORD)
	om_exe_AddMethod(OxEXE_PROC_STARTSATRECORD, 		om_exe_start_SatRecord, eOmExe_CmdStart, NULL);
#endif

#if 0 // defined(CONFIG_3RD_MHEG)
	om_exe_AddMethod(OxEXE_PROC_MHEG, 			om_exe_start_mheg, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEG_KEY, 		om_exe_set_mheg_key, eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEG_DEEPLINK, om_exe_set_mheg_deeplink, eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEG_OPCTRL,	om_exe_set_mheg_opctrl, eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEG,			NULL, eOmExe_CmdEvtNoti, om_exe_EvtProc_mheg);
#endif

#if defined(CONFIG_SUPPORT_MHEG_IB)
	om_exe_AddMethod(OxEXE_PROC_MHEGIB_FILEREQ, om_exe_set_mhegib_requestfile, eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEGIB_DATEREQ, om_exe_set_mhegib_requestdate, eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEGIB_ABORTBYUSER,   om_exe_set_mhegib_abortbyuser,       eOmExe_CmdSet, NULL);
	om_exe_AddMethod(OxEXE_PROC_MHEGIB_ABORTBYSYSTEM,   om_exe_set_mhegib_abortbysystem,       eOmExe_CmdSet, NULL);
#endif

#if defined(CONFIG_OP_UK_DTT)
	om_exe_AddMethod(OxEXE_PROC_GETTRDCONFLICTINFO,	om_exe_get_trdconflictInfo, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_SETTRDCONFLICTINFO,	om_exe_set_trdconflictInfo, eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_TRDCONFLICT,		NULL, 						eOmExe_CmdEvtNoti, om_exe_EvtProc_trdconflict);
	om_exe_AddMethod(OxEXE_PROC_GETNCDINFO,			om_exe_get_ncdInfo, 		eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_SETNCDNOTIFYDATA,	om_exe_set_NcdNotifyData,	eOmExe_CmdStart, NULL);
	om_exe_AddMethod(OxEXE_PROC_NCDINFO,			NULL, 						eOmExe_CmdEvtNoti, om_exe_EvtProc_ncdinfo);
#endif

	om_exe_RegistMethod();
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_exe_notifier, 0, 0, 0, 0);
	return ERR_OK;
}




