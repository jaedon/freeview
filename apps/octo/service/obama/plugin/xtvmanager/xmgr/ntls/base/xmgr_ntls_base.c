/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_ntls_base.c
	@brief

	Description:  									\n
	Module: XMGR / NTLS								\n

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

#include <octo_common.h>
#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_pvr_action.h>
#include <mgr_rscmap.h>
#include <mgr_ntls.h>
#include <mgr_live.h>

#include <svc_ch.h>
#include <svc_rec.h>

#include <_xmgr_ntls_base.h>

#include <otl.h>



#define CHECK_NTLS_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);	\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)	\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
	} while(0)

#define NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3)	\
			{														\
				(void)pstContext;									\
				(void)hAction;										\
				(void)p1;											\
				(void)p2;				 							\
				(void)p3;					 						\
			}


#define ___LOCAL_VARIABLES___
STATIC xMgrNtlsContext_t	s_astNtlsCtx[NUM_BG_ACTION];


#define ___LOCAL_FUNCTION___
STATIC xMgrNtlsContext_t* xmgr_ntls_base_GetContext (HUINT32 ulIndex)
{
	if (ulIndex >= NUM_BG_ACTION)
		return NULL;

	return &s_astNtlsCtx[ulIndex];
}

STATIC void xmgr_ntls_base_ClearContext (xMgrNtlsContext_t *pstContext)
{
	if (NULL == pstContext)
		return;

	HxSTD_MemSet (pstContext, 0, sizeof(xMgrNtlsContext_t));
	pstContext->hAction	=	HANDLE_NULL;
	pstContext->eState	=	eNtlsState_IDLE;

	return ;
}

STATIC HBOOL xmgr_ntls_base_CheckValidPidList (HINT32 nPidsCount, HUINT16 *pusPids)
{
	if ( (nPidsCount <= 0) || (DxRSV_PIDTABLE_COUNT <= nPidsCount) )
	{
		HxLOG_Error ("Error. : nPidsCount is invaild (%d)\n", nPidsCount);
		return FALSE;
	}

	if (NULL == pusPids)
	{
		HxLOG_Error ("Error. : Invaild Arguments\n");
		return FALSE;
	}

	return TRUE;
}

STATIC HERROR xmgr_ntls_base_SetupAction (HUINT32 ulActionId, MgrNtls_Setup_t *pstSetup)
{
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;
	HERROR						 hErr = ERR_FAIL;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
	if (ERR_OK == hErr)
	{
		switch (eLsPriority)
		{
		case eDxLsPriority_EQUAL_REC:
			eActPriority = eAmPriority_RECORD;
			break;

		case eDxLsPriority_EQUAL_VIEW:
			eActPriority = eAmPriority_VIEW_ACTION;
			break;

		case eDxLsPriority_BELOW_VIEW:
			eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		case eDxLsPriority_UPPER_REC:
			eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		default:
			break;
		}
	}

	hErr = MGR_ACTION_SetupNtls (ulActionId, pstSetup->hSvc, eActPriority);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("can't take MGR_ACTION_SetupNtls() err=%d\n", hErr);
		return hErr;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_ntls_base_StartTune(xMgrNtlsContext_t *pstContext)
{
	HERROR	hErr	= ERR_FAIL;

	DxTuneParam_t		stTuningInfo;
	SvcRsc_Info_t		stRscInfo;
	DbSvc_TsInfo_t		stTsInfo;

	HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
	HxSTD_MemSet(&stRscInfo, 0, sizeof(SvcRsc_Info_t));
	HxSTD_MemSet(&stTuningInfo, 0, sizeof(DxTuneParam_t));

	hErr = DB_SVC_GetTsInfoByServiceHandle(pstContext->stSetup.hSvc, &stTsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error!!!\n");
		return hErr;
	}

	/* DB에서 tuning paramter 가져옴. */
	hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuningInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error!!!\n");
		return hErr;
	}

	hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &stRscInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error!!!\n");
		return hErr;
	}

	hErr = SVC_CH_StartTune(pstContext->hAction, &stRscInfo, stTsInfo.usUniqueId, &stTuningInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("Error!!!\n");
		return hErr;
	}

	/*save MGR-STATE*/
	pstContext->eState = eNtlsState_TUNE;

	return hErr;
}

STATIC HERROR xmgr_ntls_base_StartSi(xMgrNtlsContext_t *pstContext)
{
	HERROR				hErr = ERR_FAIL;
	SvcSi_RecSpec_t 	stSiSpec;

	hErr = MGR_RECORDER_GetRecSiSpec (&stSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_RECORDER_GetRecSiSpec err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hSvc);
		return ERR_FAIL;
	}

	hErr = SVC_SI_StartRec (pstContext->hAction, &stSiSpec, pstContext->stSetup.hSvc, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SI_StartRec hErr: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hSvc);
		return ERR_FAIL;
	}

	HxLOG_Debug("ulActionId(%d)\n", pstContext->ulActionId);
	return hErr;
}

STATIC HERROR xmgr_ntls_base_StopSi(xMgrNtlsContext_t *pstContext)
{
	HERROR				hErr = ERR_FAIL;

	hErr = SVC_SI_StopRec(pstContext->hAction, pstContext->stSetup.hSvc, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("ERROR : \n");
	}

	HxLOG_Debug("ulActionId(%d)\n", pstContext->ulActionId);
	return hErr;
}

STATIC void xmgr_ntls_base_SetSubtitleInfo(SvcSi_SubtitleList_t *pInput, SvcMeta_SbtlInfoList_t *pstSbtlInfoList)
{
	HUINT32 i;

	if(pInput == NULL || pstSbtlInfoList == NULL)
	{
		return;
	}

	pstSbtlInfoList->ulItemNum = pInput->ulSubtitleNum;

	for(i=0; i< pInput->ulSubtitleNum; i++)
	{
		switch(pInput->pstSubtitleInfo[i].eType)
		{
		case eSI_SUBTITLE_DVB:
			pstSbtlInfoList->astSbtlInfo[i].ucComponentType = pInput->pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
			HxSTD_MemCopy(pstSbtlInfoList->astSbtlInfo[i].aucLangCode,pInput->pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode,LANG_639_CODE_MAX+1);
			break;
		case eSI_SUBTITLE_EBU:
			pstSbtlInfoList->astSbtlInfo[i].ucComponentType = pInput->pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
			HxSTD_MemCopy(pstSbtlInfoList->astSbtlInfo[i].aucLangCode,pInput->pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode,LANG_639_CODE_MAX+1);
			break;
		case eSI_SUBTITLE_SUPERCC:
			break;
		default:
			break;
		}
		HxLOG_Debug("ucComponentType[%d]\n",pstSbtlInfoList->astSbtlInfo[i].ucComponentType);
	}
}

STATIC void xmgr_ntls_base_FreeAitPidList (void *pvData)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT32		*pulAitPid = (HUINT32 *)pvData;

	if (NULL != pulAitPid)
	{
		HLIB_STD_MemFree(pulAitPid);
	}
#endif

	return;
}

STATIC void xmgr_ntls_base_DebugPrintRecPidInfo(SvcRec_PIDList_t *pstPIDList, HCHAR *funcName, HINT32 nLine)
{
#ifdef CONFIG_DEBUG
	HINT32		i = 0;


	HxLOG_Print("called from " HxANSI_COLOR_YELLOW("[%s:%d]\n"), funcName, nLine);

	if(NULL == pstPIDList)
	{
		HxLOG_Error("\t pstPIDList is NULL\n");
		return;
	}

	HxLOG_Print("\t pstPIDList->ePidSetType      = %d \n", pstPIDList->ePidSetType);

	if(eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSI[i].usPid)
			{
				HxLOG_Print("\t stKnown.stSI[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSI[i].usPid, pstPIDList->stKnown.stSI[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_VIDEO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stVideo[i].usPid)
			{
				HxLOG_Print("\t stKnown.stVideo[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stVideo[i].usPid, pstPIDList->stKnown.stVideo[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_AUDIO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stAudio[i].usPid)
			{
				HxLOG_Print("\t stKnown.stAudio[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stAudio[i].usPid, pstPIDList->stKnown.stAudio[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_SUBTTL_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSubtitle[i].usPid)
			{
				HxLOG_Print("\t stKnown.stSubtitle[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSubtitle[i].usPid, pstPIDList->stKnown.stSubtitle[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_TELTXT_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stTeletext[i].usPid)
			{
				HxLOG_Print("\t stKnown.stTeletext[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stTeletext[i].usPid, pstPIDList->stKnown.stTeletext[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CAPTION_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stCaption[i].usPid)
			{
				HxLOG_Print("\t stKnown.stCaption[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stCaption[i].usPid, pstPIDList->stKnown.stCaption[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEcm[i].usPid)
			{
				HxLOG_Print("\t stKnown.stEcm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEcm[i].usPid, pstPIDList->stKnown.stEcm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEmm[i].usPid)
			{
				HxLOG_Print("\t stKnown.stEmm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEmm[i].usPid, pstPIDList->stKnown.stEmm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_DATA_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stData[i].usPid)
			{
				HxLOG_Print("\t stKnown.stData[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stData[i].usPid, pstPIDList->stKnown.stData[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		for(i = 0; i < DxRSV_PIDTABLE_COUNT; i++)
		{
			if(PID_NULL != pstPIDList->stUnknown.stPIDs[i].usPid)
			{
				HxLOG_Print("\t stUnknown.stPIDs[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stUnknown.stPIDs[i].usPid, pstPIDList->stUnknown.stPIDs[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_All == pstPIDList->ePidSetType)
	{
		HxLOG_Print("\t stUnknown.stPIDs\t= (PID:ALL, ReqID:%d) \n", pstPIDList->stUnknown.stPIDs[i].ulRequestId);
	}

	HxLOG_Print("---------------------------------------------------------------\n");
#endif
}

#define ___MSG_FUNCTION___
BUS_Result_t	XMGR_NTLS_MsgBusCreate_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr		= 	ERR_OK;
	HUINT32 			ulActionId	=	SVC_PIPE_GetActionId(hAction);
	MgrNtls_Setup_t		*pstNtlsSetup = (MgrNtls_Setup_t *)p1;

	if(NULL == pstNtlsSetup)
	{
		HxLOG_Error("ERROR : pstNtlsSetup is NULL, hAction=0x%x\n", hAction);
		return MESSAGE_BREAK;
	}

	/*resource check*/
	{
		hErr = MGR_ACTION_Take(ulActionId, eActionType_NTLS, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : MGR_ACTION_Take() hAction=0x%x\n", hAction);
			return MESSAGE_BREAK;
		}
	}

	/* setup ntls pstContext */
	{
		pstContext->ulActionId		= ulActionId;
		pstContext->hAction 		= SVC_PIPE_IncreaseActionVersion(ulActionId);
		pstContext->eState			= eNtlsState_IDLE;
		pstContext->pfActionProc	= BUS_MGR_GetMgrCallback(BUS_MGR_GetThis());

		//Set PID_NULL
		SVC_REC_CleanReqPidInfo (eRec_PidSetType_Unknown, &pstContext->stRecPidInfo);

		HxSTD_memcpy (&pstContext->stSetup, pstNtlsSetup, sizeof(MgrNtls_Setup_t));
	}

	/* start tune */
	{
		hErr = xmgr_ntls_base_StartTune(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : hSvc=0x%x\n", pstContext->stSetup.hSvc);
			return MESSAGE_BREAK;
		}
	}

	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

BUS_Result_t	XMGR_NTLS_MsgBusDestroy_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eNtlsState_IDLE != pstContext->eState)
	{
		HERROR	hErr		= ERR_OK;

		/* stop service */
		hErr = SVC_SI_StopRec(pstContext->hAction, pstContext->stSetup.hSvc, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : \n");
		}

		hErr = SVC_REC_Stop(pstContext->hAction, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : \n");
		}

		hErr = SVC_REC_Close(pstContext->hAction, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : \n");
		}

		hErr = SVC_CH_StopTune(pstContext->hAction, (HUINT16)-1, eSyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("ERROR : \n");
		}

		BUS_SendMessage(NULL, eMEVT_NTLS_NOTIFY_START_STOPPED, pstContext->hAction, 0, 0, 0);

		/* release context */
		MGR_ACTION_Release(pstContext->ulActionId, eActionType_NTLS);
	}

	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}


BUS_Result_t  	XMGR_NTLS_MsgBusGoStandby_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}


BUS_Result_t	XMGR_NTLS_MsgEvtChLocked_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_FAIL;
	DbSvc_Info_t		stSvcInfo;

	/* SI Start */
	hErr = xmgr_ntls_base_StartSi(pstContext);
	if (ERR_OK != hErr)
		return hErr;

	/*save MGR-STATE*/
	pstContext->eState = eNtlsState_TUNE;


	/*
		locked -> record pat pid -> pmt received -> record pmt, es, ...

		pat pid 먼저 걸어 줌
	*/

	/*get svcinfo*/
	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("ERROR : \n");
		return MESSAGE_PASS;
	}

	/* REC Setup */
	{
		SvcRec_Setup_t stSetup;
		HxSTD_MemSet(&stSetup, 0, sizeof(SvcRec_Setup_t));

		stSetup.eRecActType =	eRec_ActType_Ntls;
		stSetup.eRecOutType =	eRec_OutType_MemoryBuf;

		stSetup.eSvcType	=	DbSvc_GetSvcType(&stSvcInfo);
		stSetup.bEncryption =	FALSE;
		stSetup.eVideoCodec =	eDxVIDEO_CODEC_MPEG2;//stSvcInfo.eVideoCodec;
		stSetup.ulStartTime =	0;

		stSetup.unOutArg.hMemoryBuf	=	pstContext->stSetup.hRingBuf;

		hErr = SVC_REC_Setup (pstContext->hAction, &stSetup, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("Error : \n");
			return ERR_FAIL;
		}
	}

	/* set pid 전 pid info 확인용 */
	pstContext->stRecPidInfo.stUnknown.stPIDs[0].usPid = PAT_PID;
	xmgr_ntls_base_DebugPrintRecPidInfo(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);
	hErr = SVC_REC_SetPID( pstContext->hAction, &pstContext->stRecPidInfo, TRUE );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_SetPID. Error\n");
		return hErr;
	}

	hErr = SVC_REC_Start (pstContext->hAction, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_Start. Error\n");
		return hErr;
	}

	/*save MGR-STATE*/
	pstContext->eState = eNtlsState_RECORDING;


	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

BUS_Result_t	XMGR_NTLS_MsgEvtChNoSignal_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/*
	 * Fail 처리
	 */

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t	XMGR_NTLS_MsgEvtChMovingMotor_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/*save MGR-STATE*/
	pstContext->eState = eNtlsState_NO_SIGNAL;

	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_PASS;
}



BUS_Result_t XMGR_NTLS_MsgEvtSiPmtReceived_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr;
	HBOOL			bChanged = FALSE;
	DbSvc_Info_t	stOrgSvcInfo, stDstSvcInfo;


	/*get svcinfo*/
	HxSTD_MemSet(&stOrgSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hSvc, &stOrgSvcInfo);

	/*update pids*/
	hErr = SVC_SI_GetLiveServiceInfoPid (pstContext->hAction, (HUINT32)DbSvc_GetSvcId(&stOrgSvcInfo), &stOrgSvcInfo, &stDstSvcInfo, &bChanged);
	if(TRUE == bChanged)
	{
		HxLOG_Print("ORG v_pid: 0x%x, a_pid: 0x%x, ttx_pid: 0x%x, subttl_idx: 0x%x, pcr_pid: 0x%x\n", DbSvc_GetVideoPid(&stOrgSvcInfo), DbSvc_GetAudioPid(&stOrgSvcInfo), DbSvc_GetTtxPid(&stOrgSvcInfo), DbSvc_GetSubttlIdx(&stOrgSvcInfo), DbSvc_GetPcrPid(&stOrgSvcInfo));
		HxLOG_Print("DST v_pid: 0x%x, a_pid: 0x%x, ttx_pid: 0x%x, subttl_idx: 0x%x, pcr_pid: 0x%x\n", DbSvc_GetVideoPid(&stDstSvcInfo), DbSvc_GetAudioPid(&stDstSvcInfo), DbSvc_GetTtxPid(&stDstSvcInfo), DbSvc_GetSubttlIdx(&stDstSvcInfo), DbSvc_GetPcrPid(&stDstSvcInfo));
	}

	if (DbSvc_GetVideoPid(&stDstSvcInfo) == PID_NULL)
	{
		/* ignore codec-changed caused by removal of video pid  */
		DbSvc_SetVideoCodec(&stDstSvcInfo, DbSvc_GetVideoCodec(&stOrgSvcInfo));
	}

	if(DbSvc_GetVideoCodec(&stOrgSvcInfo) != DbSvc_GetVideoCodec(&stDstSvcInfo))
	{
		HxLOG_Debug("VideoCodec Org(%d), Dst(%d) \n", DbSvc_GetVideoCodec(&stOrgSvcInfo), DbSvc_GetVideoCodec(&stDstSvcInfo));
		bChanged = TRUE;
	}

	if (ERR_OK == hErr)
	{
		if (TRUE == bChanged)
		{
			HxLOG_Info("update svc info \n");
			HxLOG_Debug("eVideoCodec(%d) \n", DbSvc_GetVideoCodec(&stDstSvcInfo));
			hErr = DB_SVC_UpdateServiceInfo (pstContext->stSetup.hSvc, &stDstSvcInfo);
			//BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_SvcInfoUpdated, 0, 0);

			#if 0
			{
				SvcRec_Setup_t stSvcRecSetup;
				MGR_RECORDER_SetRecordSetup (&pstContext->stSetup, &stSvcRecSetup, &pstContext->stMetaInfo, pstContext->hAction);
				xmgr_rec_GetServiceInfoFromSvc (pstContext->stSetup.hCurrSvc, &stSvcRecSetup);

				hErr = SVC_REC_UpdateVideoCodec(pstContext->hAction, &stSvcRecSetup, TRUE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_REC_UpdateVideoCodec. Error\n");
				}
			}
			#endif
		}
	}
	else
	{
		HxLOG_Error ("SVC_SI_GetLiveServiceInfoPid err\n");
	}

	/* pid start */
	{
		HINT32					nPidCnt = 0, i = 0, nMinTrack = 0;
		SvcSi_AudioList_t		stAudioList;
		SvcSi_SubtitleList_t	stSubtitleList;
		HUINT32					ulActionId = pstContext->ulActionId;


		SVC_REC_CleanReqPidInfo (eRec_PidSetType_Unknown, &pstContext->stRecPidInfo);

		pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid = PAT_PID;
		nPidCnt++;

		if(PID_NULL > DbSvc_GetPmtPid(&stDstSvcInfo))
		{
			pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid = DbSvc_GetPmtPid(&stDstSvcInfo);
			HxLOG_Debug("usPmtPid = 0x%x\n", DbSvc_GetPmtPid(&stDstSvcInfo));
			nPidCnt++;
		}
		if(PID_NULL > DbSvc_GetPcrPid(&stDstSvcInfo))
		{
			pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid = DbSvc_GetPcrPid(&stDstSvcInfo);
			HxLOG_Debug("usPcrPid = 0x%x\n", DbSvc_GetPcrPid(&stDstSvcInfo));
			nPidCnt++;
		}
		if(PID_NULL > DbSvc_GetVideoPid(&stDstSvcInfo))
		{
			pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid = DbSvc_GetVideoPid(&stDstSvcInfo);
			HxLOG_Debug("usVideoPid = 0x%x\n", DbSvc_GetVideoPid(&stDstSvcInfo));
			nPidCnt++;
		}

		/*audio*/
		HxSTD_MemSet( &stAudioList, 0, sizeof(SvcSi_AudioList_t) );
		hErr = SVC_SI_GetAudioList (ulActionId, DbSvc_GetSvcId(&stDstSvcInfo), &stAudioList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stAudioList.nAudio<SvcSi_MAX_AUDIO_ES_NUM)? stAudioList.nAudio : SvcSi_MAX_AUDIO_ES_NUM;
			for( i=0 ; i< nMinTrack ; i++ )
			{
				pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid  = stAudioList.pstAudioInfo[i].usPid;
				HxLOG_Debug("stAudioList.pstAudioInfo[%d].usPid = 0x%x\n", i, stAudioList.pstAudioInfo[i].usPid);
				nPidCnt++;
			}

			SVC_SI_FreeAudioList(&stAudioList); //free
		}

		/*subtitle*/
		HxSTD_MemSet( &stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t) );
		hErr = SVC_SI_GetSubtitleList (ulActionId, DbSvc_GetSvcId(&stDstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum<SvcSi_MAX_SUBTTL_ES_NUM)? stSubtitleList.ulSubtitleNum : SvcSi_MAX_SUBTTL_ES_NUM;
			for( i=0 ; i< nMinTrack ; i++ )
			{
				pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid  = stSubtitleList.pstSubtitleInfo[i].usPid;
				HxLOG_Debug("stSubtitleList.pstSubtitleInfo[%d].usPid = 0x%x\n", i, stSubtitleList.pstSubtitleInfo[i].usPid);
				nPidCnt++;
			}

			SVC_SI_FreeSubtitleList(&stSubtitleList);	//free
		}

		/*ttx*/
		if(PID_NULL > DbSvc_GetTtxPid(&stDstSvcInfo))
		{
			pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid  = DbSvc_GetTtxPid(&stDstSvcInfo);
			HxLOG_Debug("usTtxPid = 0x%x\n", DbSvc_GetTtxPid(&stDstSvcInfo));
			nPidCnt++;
		}

		/*super CC*/
		HxSTD_MemSet( &stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t) );
		hErr = SVC_SI_GetSuperCcList (ulActionId, DbSvc_GetSvcId(&stDstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum<SvcSi_MAX_CAPTION_ES_NUM)? stSubtitleList.ulSubtitleNum : SvcSi_MAX_CAPTION_ES_NUM;
			for( i=0 ; i< nMinTrack ; i++ )
			{
				pstContext->stRecPidInfo.stUnknown.stPIDs[nPidCnt].usPid  = stSubtitleList.pstSubtitleInfo[i].usPid;
				HxLOG_Debug("stSubtitleList.pstSubtitleInfo[%d].usPid = 0x%x\n", i, stSubtitleList.pstSubtitleInfo[i].usPid);
				nPidCnt++;
			}

			SVC_SI_FreeSuperCcList(&stSubtitleList);	//free
		}

		xmgr_ntls_base_DebugPrintRecPidInfo(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);
		SVC_REC_SetPID( pstContext->hAction, &pstContext->stRecPidInfo, TRUE );
		//////////////////////////////////////////////////////////////////////////////
	}

	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

BUS_Result_t	XMGR_NTLS_MsgEvtSiPmtPidChanged_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr;
	DbSvc_Info_t 		stSvcInfo;
	HUINT16 			usPmtPid;

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
		return hErr;

	SVC_SI_GetPmtPidBySvcId(pstContext->hAction, DbSvc_GetSvcId(&stSvcInfo), &usPmtPid);
	HxLOG_Print(HxANSI_COLOR_YELLOW("[NTLS:%d] SI - usPmtPid = 0x%x, stSvcInfo.usPmtPid = 0x%x\n"), pstContext->ulActionId, usPmtPid, DbSvc_GetPmtPid(&stSvcInfo));

	DbSvc_SetPmtPid(&stSvcInfo, usPmtPid);
	DbSvc_SetPcrPid(&stSvcInfo, PID_NULL);
	DbSvc_SetTtxPid(&stSvcInfo, PID_NULL);
	DbSvc_SetVideoPid(&stSvcInfo, PID_NULL);
	DbSvc_SetAudioPid(&stSvcInfo, PID_NULL);
	DbSvc_SetDolbyPid(&stSvcInfo,PID_NULL);
	DbSvc_SetAudioAuxPid(&stSvcInfo, PID_NULL);

	hErr = DB_SVC_UpdateServiceInfo(pstContext->stSetup.hSvc, &stSvcInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_UpdateServiceInfo failed: hSvc(0x%08x)\n", pstContext->stSetup.hSvc);
	}

	{
		DbSvc_Info_t 		stTempSvcInfo;
		HERROR				hErr;
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hSvc, &stTempSvcInfo);
		if (hErr == ERR_OK)
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("[NTLS:%d] Get : usPmtPid = 0x%x\n"), pstContext->ulActionId, DbSvc_GetPmtPid(&stTempSvcInfo));
		}
	}

	xmgr_ntls_base_StopSi(pstContext);

	hErr = xmgr_ntls_base_StartSi(pstContext);
	if (ERR_OK != hErr)
		return hErr;

	NTLS_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}


#define ___PROC_FUNCTION___
STATIC BUS_Result_t xmgr_ntls_base_protoProc (xMgrNtlsContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
   		case eMEVT_BUS_READY_SHUTDOWN :
			HxLOG_Debug("[MGR:NTLS(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", ulActionId);
			eBusResult = XMGR_NTLS_MsgBusGoStandby_Base(pstContext, hAction, p1, p2, p3);
			return MESSAGE_PASS;
		/*MSG*/
		case eMEVT_BUS_CREATE:
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eMEVT_BUS_CREATE\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eMEVT_BUS_DESTROY\n"), pstContext->ulActionId);
			eBusResult = XMGR_NTLS_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-SI*/
		case eSEVT_SI_PMT:
			CHECK_NTLS_ACTION_HANDLE();
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eSEVT_SI_PMT\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_NTLS_ACTION_HANDLE();
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eSEVT_SI_PMT_PIDCHANGED\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-CH*/
		case eSEVT_CH_NO_SIGNAL :
			CHECK_NTLS_ACTION_HANDLE();
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eSEVT_CH_NO_SIGNAL\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgEvtChNoSignal_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CH_MOVING_MOTOR :
			CHECK_NTLS_ACTION_HANDLE();
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eSEVT_CH_MOVING_MOTOR\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgEvtChMovingMotor_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CH_LOCKED :
			CHECK_NTLS_ACTION_HANDLE();
			HxLOG_Debug(HxANSI_COLOR_YELLOW("(%d) eSEVT_CH_LOCKED\n"), ulActionId);
			eBusResult = XMGR_NTLS_MsgEvtChLocked_Base(pstContext, hAction, p1, p2, p3);
			break;

		default :
			break;
	}


	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (0), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (1), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (2), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (3), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg4 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (4), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg5 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (5), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg6 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (6), message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_ntls_base_procBg7 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
	}

	return xmgr_ntls_base_protoProc (xmgr_ntls_base_GetContext (7), message, hHandle, p1, p2, p3);
}

STATIC BUS_Callback_t xmgr_ntls_base_GetActionProc (HUINT32 ulActionId)
{
	if (xmgr_ntls_base_GetContext (ulActionId-eActionId_BG_FIRST) == NULL)
	{
		HxLOG_Error ("Action ID(%d) is not allowed for NTLS\n");
		return NULL;
	}

	switch (ulActionId)
	{
	case (eActionId_BG_FIRST + 0):
		return xmgr_ntls_base_procBg0;

	case (eActionId_BG_FIRST + 1):
		return xmgr_ntls_base_procBg1;

	case (eActionId_BG_FIRST + 2):
		return xmgr_ntls_base_procBg2;

	case (eActionId_BG_FIRST + 3):
		return xmgr_ntls_base_procBg3;

	case (eActionId_BG_FIRST + 4):
		return xmgr_ntls_base_procBg4;

	case (eActionId_BG_FIRST + 5):
		return xmgr_ntls_base_procBg5;

	case (eActionId_BG_FIRST + 6):
		return xmgr_ntls_base_procBg6;

	case (eActionId_BG_FIRST + 7):
		return xmgr_ntls_base_procBg7;

	default:
		break;
	}

	return NULL;

}

#define ___PLUGIN_FUNCTION___
HERROR xmgr_ntls_Init_BASE (void)
{
	/* Clear Context */
	{
		HINT32 i;
		for (i=0 ; i<NUM_BG_ACTION ; i++)
		{
			xmgr_ntls_base_ClearContext (&s_astNtlsCtx[i]);
		}
	}
	return ERR_OK;
}

HERROR xmgr_ntls_Start_BASE (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulActionId = 0;

	if ( (NULL == pstSetup) || (NULL == pulActionId) )
	{
		HxLOG_Error ("Error. : Invaild Arguments\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_GetEmptyBgActionId(&ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error. : Alloc ActionId\n");
		return ERR_FAIL;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("ulActionId = %d\n"), ulActionId);

	hErr = xmgr_ntls_base_SetupAction (ulActionId, pstSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error. : Setup Action\n");
		return ERR_FAIL;
	}

	/* Create MSG Proc */
	{
		Handle_t hAction = SVC_PIPE_GetActionHandle (ulActionId);

		hErr = BUS_MGR_Create (	NULL,
								APP_ACTION_PRIORITY,
								xmgr_ntls_base_GetActionProc (ulActionId),
								hAction,
								(HINT32)pstSetup,
								0,
								0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("Error. : Create Proc\n");
			return ERR_FAIL;
		}
	}

	/* output */
	*pulActionId = ulActionId;

	return ERR_OK;
}

HERROR xmgr_ntls_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	xMgrNtlsContext_t* pstContext = NULL;
	SvcRec_PIDList_t	stRecPidInfo;

	pstContext = xmgr_ntls_base_GetContext (ulActionId - eActionId_BG_FIRST);
	if (NULL == pstContext)
		return ERR_FAIL;
/*
	if (eNtlsState_Running > pstContext->eState)
	{
		HxLOG_Error ("!!! NTLS not running !!!\n");
		return ERR_FAIL;
	}
*/
	if( xmgr_ntls_base_CheckValidPidList (nPidsCount, pusPids)	== FALSE )
		return ERR_FAIL;

	/*PID copy*/
	{
		HINT32 i;
		SVC_REC_CleanReqPidInfo (eRec_PidSetType_Unknown, &stRecPidInfo);

		for (i=0 ; i<nPidsCount ; i++ )
		{
			stRecPidInfo.stUnknown.stPIDs[i].usPid = pusPids[i];
		}
	}

	SVC_REC_SetPID( pstContext->hAction, &stRecPidInfo, TRUE );

	return ERR_OK;
}

HERROR xmgr_ntls_Stop_BASE (HUINT32 ulActionId)
{
	xMgrNtlsContext_t* pstContext = NULL;

	pstContext = xmgr_ntls_base_GetContext (ulActionId - eActionId_BG_FIRST);

	if (NULL != pstContext)
	{
		if(NULL != pstContext->pfActionProc)
		{
			BUS_MGR_Destroy (pstContext->pfActionProc);
		}

		HxLOG_Print(HxANSI_COLOR_YELLOW("ulActionId = %d\n"), ulActionId);
	}
	else
	{
		HxLOG_Error("Error. not found NTLS's context\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
