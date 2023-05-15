/**
	@file     	xmgr_recorder_ukdtt.c
	@brief    	recording control application for Ukdtt.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 		ap/pvr			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_OP_UK_DTT)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>

#include <octo_common.h>

#include <db_svc.h>
#include <xdb_svc.h>
#include <isosvc.h>

#include <svc_pipe.h>
#include <svc_rec.h>
#include <xsvc_pvr.h>


#include <bus.h>

#include <mgr_action.h>
#include <mgr_storage.h>
#include <mgr_live.h>
#include "../local_include/_xmgr_recorder.h"
#include "../base/local_include/_xmgr_recorder_base.h"
#include "../../schedule/local_include/_xmgr_schedule.h"
#include <xmgr_cas.h>
#include <mgr_copyrightcontrol.h>
#include <svc_sys.h>
#include <svc_cas.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define FTA_MANAGED_COPY_BIT_MASK			0xFF

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#if defined (CONFIG_DEBUG)
void xmgr_rec_PrintKey(HUINT8 *pucKey);
STATIC void xmgr_rec_PrintMetaInfo_Ukdtt(xmgrRec_Context_t *pstContext);
STATIC void xmgr_rec_PrintRecInfo_Ukdtt(xmgrRec_Context_t *pstContext);
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________LOCAL_FUNC__________
#if defined(CONFIG_APCORE_IP_EPG)
STATIC void xmgr_rec_OnDamaSeriesDataReceived_Ukdtt (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	SvcMeta_Record_t * pSvcRecMeta = (SvcMeta_Record_t *)userdata;
	DxEvent_t		*pstEvent;
	DAxIterator_t	iter;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pstEvent)
		{
			pSvcRecMeta->ulEpisodeNum = pstEvent->extension.ipEpg.episodeNum;
			pSvcRecMeta->ulSeason = pstEvent->extension.ipEpg.season;
			DLIB_FreeType(iter.name, pstEvent);
			break;
		}
	}
}

STATIC	HERROR xmgr_rec_FindEpisodeAndSeasonFromIPEPG_Ukdtt(DxSchedule_t	*pstSchedule, SvcMeta_Record_t *pstMetaInfo)
{
	HCHAR		query[256];
	HUINT32 	id;

	if(pstSchedule==NULL || pstMetaInfo==NULL)
	{
		HxLOG_Error ("pstSchedule or pstMetaInfo is NULL.\n");
		return ERR_FAIL;
	}

	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE svcuid=%d AND eventid=%d"
		, DxNAMEOF(eDxEPG_TYPE_IP)
		, pstSchedule->ulSvcUid
		, pstSchedule->uExtInfo.stRec.nEvtId
	);

	id = DAPI_Query(query, (DAPI_Getter)xmgr_rec_OnDamaSeriesDataReceived_Ukdtt, (void *)pstMetaInfo);
	DAPI_Wait(2000, id);
	if (id == 0)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("Query Success \n");
		return ERR_OK;
	}
}
#endif


STATIC HERROR xmgr_rec_ConvertUtf8ToByte_Ukdtt(HUINT8 *pszEvtName, HUINT8 *pszFileName)
{
	HINT32 i=0;
	const HINT32 BUFF_LEN = 256;
	HUINT8	szChangedString[BUFF_LEN];

	HxSTD_MemSet(&szChangedString, 0x00, sizeof(HUINT8)*BUFF_LEN);
	HLIB_STD_StrUtf8NCpy ( szChangedString,
						pszEvtName,
						MAX_REC_FILE_NAME_LEN-1);

	/* delete space in first character */
	for( i=0  ; i<BUFF_LEN ; i++ )
	{
		if( HxSTD_IsWhiteSpace( szChangedString[i] ) )
			continue;
		else break;
	}


	HLIB_STD_StrUtf8NCpy(pszFileName, &szChangedString[i], MAX_REC_FILE_NAME_LEN-1);
	return ERR_OK;
}

STATIC const HUINT8 *xmgr_rec_MakeStrFtaState_Ukdtt(DxUkdtt_PVR_FtaContentMgmtState_e eFtaContentState)
{
	switch (eFtaContentState)
	{
		ENUM_TO_STR(eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc);
		ENUM_TO_STR(eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc_NoEnc);
		ENUM_TO_STR(eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy);
		ENUM_TO_STR(eDxUkdtt_CONTENT_MGMT_STATE_Reserved);
		ENUM_TO_STR(eDxUkdtt_CONTENT_MGMT_STATE_None);
	}
	return ("FTA content management state : Unknown");
}

STATIC const HINT8 * xmgr_rec_MakeStrFtaContentLevel_Ukdtt(SvcSi_FTA_CONTENT_e eFtaContentLevel)
{
	switch (eFtaContentLevel)
	{
		ENUM_TO_STR(eFTA_CONTENT_LEVEL_NONE);
		ENUM_TO_STR(eFTA_CONTENT_NET_LEVEL);
		ENUM_TO_STR(eFTA_CONTENT_TS_LEVEL);
		ENUM_TO_STR(eFTA_CONTENT_SVC_LEVEL);
		ENUM_TO_STR(eFTA_CONTENT_EVT_LEVEL);
		default:
			break;
	}
	return ("Unknown");
}
STATIC DxUkdtt_PVR_FtaContentMgmtState_e xmgr_rec_GetCopyControlState_Ukdtt (HUINT8 ucDoNotScramble, HUINT8 ucCtrlRemoteAccess)
{
	if (ucDoNotScramble == 0)
	{
		if ((ucCtrlRemoteAccess & FTA_MANAGED_COPY_BIT_MASK) > 0)
		{
			return eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy;
		}
		else
		{
			return eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc;
		}
	}
	else
	{
		if ((ucCtrlRemoteAccess & FTA_MANAGED_COPY_BIT_MASK) > 0)
		{
			return eDxUkdtt_CONTENT_MGMT_STATE_Reserved;
		}
		else
		{
			return eDxUkdtt_CONTENT_MGMT_STATE_FreeAcc_NoEnc;
		}
	}

}

STATIC void			xmgr_rec_CheckPMT ( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 						hErr;
	DbSvc_Info_t				stSvcInfo;
	HUINT16 					usVideoPid;
	DxSvcType_e					eSvcType;

	/*get svcinfo*/
	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo failed. hSvc=(%d) \n",pstContext->stSetup.hCurrSvc);
		return;
	}

	usVideoPid = DbSvc_GetVideoPid(&stSvcInfo);
	eSvcType = DbSvc_GetSvcType(&stSvcInfo);
	HxLOG_Debug("SvcType=(%d) Video PID=(%d)(0x%x) \n",eSvcType, usVideoPid, usVideoPid);

	if (eSvcType != eDxSVC_TYPE_TV)
	{
		// check video pid for tv type only
		return ;
	}

	if (usVideoPid != PID_NULL)
	{
		HxLOG_Debug("There is valid video pid. \n");
		return ;
	}

	HxLOG_Error("There is no video pid in PMT. set Failed eMgrRecFailed_NO_VIDEO_AUDIO \n");
	hErr = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_NO_VIDEO_AUDIO);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_RECORDER_SetRecordFailReason failed. contentid=(%d) \n",pstContext->ulContentId);
	}

	NOT_USED_PARAM(hAction);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ;
}

STATIC HUINT32		xmgr_rec_GetFTAContentmgrFromNetwork_Ukdtt(xmgrRec_Context_t *pstContext)
{
	DxUkdtt_PVR_FtaContentMgmtState_e	eFtaContentMgr = eDxUkdtt_CONTENT_MGMT_STATE_None;
	HERROR				hErr;
	DbSvc_NetInfo_t		*pstNetInfoList = NULL;
	DbSvc_NetInfo_t		*pstNetInfoInDb = NULL;
	DbSvc_Info_t		stSvcInfo;
	HINT32				i;
	DbSvc_TsCondition_t	stTsAttr;
	HINT32				nTsCnt, nNetInfoCnt;
	DbSvc_TsInfo_t		*pstTsInfoList= NULL, *pstTsInfo = NULL;
	SvcSi_FTA_CONTENT_e	eFtaContentLevel;
	HUINT32				ulFTAContentMgrValue;
	HUINT8			ucDoNotScramble, ucCtrlRemoteAccess, ucDoNotApplyRevocation;

	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error  !! \n");
		goto END_FUNC;
	}

	DB_SVC_InitTsFindCondition(&stTsAttr);
	stTsAttr.nDeliType = eDxDELIVERY_TYPE_TER;

	hErr = DB_SVC_FindTsInfoList(&stTsAttr, &nTsCnt, &pstTsInfoList);
	if(hErr != ERR_OK || nTsCnt <= 0 || pstTsInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindTsInfoList Error  !! \n");
		goto END_FUNC;
	}

	hErr = DB_SVC_FindNetworkInfoByDeliType(eDxDELIVERY_TYPE_TER, &nNetInfoCnt, &pstNetInfoList);
	if(hErr != ERR_OK || nNetInfoCnt <= 0 || pstNetInfoList == NULL)
	{
		HxLOG_Error("DB_SVC_FindNetworkInfoByDeliType Error  !! \n");
		goto END_FUNC;
	}


	for(i = 0; i < nTsCnt; i++)
	{
		pstTsInfo = &pstTsInfoList[i];
		if (pstTsInfo->usTsIdx != DbSvc_GetTsIdx(&stSvcInfo))
		{
			HxLOG_Debug("ts idx is different. db(%d) svc(%d) \n",pstTsInfo->usTsId,DbSvc_GetTsId(&stSvcInfo));
			continue;
		}

		ulFTAContentMgrValue = XdbSvc_TvAnyTimeTsGetFTAContentMgr(pstTsInfo);
		eFtaContentLevel = (HUINT8)((ulFTAContentMgrValue >> 24) & 0xFF);
		if (eFtaContentLevel == eFTA_CONTENT_TS_LEVEL)
		{
			ucDoNotScramble = (HUINT8)((ulFTAContentMgrValue >> 16) & 0xFF);
			ucCtrlRemoteAccess = (HUINT8)((ulFTAContentMgrValue >> 8) & 0xFF);
			ucDoNotApplyRevocation = (HUINT8)(ulFTAContentMgrValue & 0xFF);

			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG level : %s \n"),  xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotScramble : %d  \n"),  ucDoNotScramble);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucCtrlRemoteAccess : %d  \n"),  ucCtrlRemoteAccess);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotApplyRevocation : %d  \n"),  ucDoNotApplyRevocation);

			eFtaContentMgr = xmgr_rec_GetCopyControlState_Ukdtt(ucDoNotScramble, ucCtrlRemoteAccess);
		}
		else
		{
			HxLOG_Debug("invalid level (%d)[%s] \n",eFtaContentLevel,xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
		}
		break;
	}

	if (eFtaContentMgr != eDxUkdtt_CONTENT_MGMT_STATE_None)
	{
		HxLOG_Debug("Fta Content Mgr Found in Ts. fta(%d) \n",eFtaContentMgr);
		goto END_FUNC;
	}

	if (pstTsInfo == NULL)
	{
		HxLOG_Debug("Can't find ts info. tsid=(%d) \n",DbSvc_GetTsId(&stSvcInfo));
		goto END_FUNC;
	}

	for(i = 0; i < nNetInfoCnt; i++)
	{
		pstNetInfoInDb = &pstNetInfoList[i];
		if (pstNetInfoInDb->usNetIdx != pstTsInfo->usNetIdx)
		{
			HxLOG_Debug("network idx is different. net(%d) ts(%d) \n",pstNetInfoInDb->usNetIdx,pstTsInfo->usNetIdx);
			continue;
		}

		ulFTAContentMgrValue = XdbSvc_TvAnyTimeNetGetFTAContentMgr(pstNetInfoInDb);
		eFtaContentLevel = (HUINT8)((ulFTAContentMgrValue >> 24) & 0xFF);
		if (eFtaContentLevel == eFTA_CONTENT_NET_LEVEL)
		{
			ucDoNotScramble = (HUINT8)((ulFTAContentMgrValue >> 16) & 0xFF);
			ucCtrlRemoteAccess = (HUINT8)((ulFTAContentMgrValue >> 8) & 0xFF);
			ucDoNotApplyRevocation = (HUINT8)(ulFTAContentMgrValue & 0xFF);

			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG level : %s \n"),  xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotScramble : %d  \n"),  ucDoNotScramble);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucCtrlRemoteAccess : %d  \n"),  ucCtrlRemoteAccess);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotApplyRevocation : %d  \n"),  ucDoNotApplyRevocation);

			eFtaContentMgr = xmgr_rec_GetCopyControlState_Ukdtt(ucDoNotScramble, ucCtrlRemoteAccess);
		}
		else
		{
			HxLOG_Debug("invalid level (%d)[%s] \n",eFtaContentLevel,xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
		}
		break;
	}

END_FUNC:
	if (pstNetInfoList!=NULL)
	{
		DB_SVC_FreeNetInfoList(pstNetInfoList);
	}
	if (pstTsInfoList !=NULL)
	{
		DB_SVC_FreeTsInfoList (pstTsInfoList);
	}

	return eFtaContentMgr;

}
STATIC HUINT32		xmgr_rec_GetFTAContentMgrFromService_Ukdtt(xmgrRec_Context_t *context)
{
	DxUkdtt_PVR_FtaContentMgmtState_e	eFtaContentMgr = eDxUkdtt_CONTENT_MGMT_STATE_None;
	HERROR				hErr;
	SvcSi_FTA_CONTENT_e	eFtaContentLevel;
	HUINT8				ucDoNotScramble, ucCtrlRemoteAccess, ucDoNotApplyRevocation;
	DbSvc_Info_t		stSvcInfo;
	HUINT32				ulFTAContentMgrValue;

	hErr = DB_SVC_GetServiceInfo(context->stSetup.hMasterSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo failed. hSvc(0x%x) \n",context->stSetup.hMasterSvc);
		goto END_FUNC;
	}

	ulFTAContentMgrValue = XdbSvc_TvAnyTimeGetContentMgr(&stSvcInfo);
	eFtaContentLevel = (HUINT8)((ulFTAContentMgrValue >> 24) & 0xFF);
	if (eFtaContentLevel == eFTA_CONTENT_SVC_LEVEL)
	{
		ucDoNotScramble = (HUINT8)((ulFTAContentMgrValue >> 16) & 0xFF);
		ucCtrlRemoteAccess = (HUINT8)((ulFTAContentMgrValue >> 8) & 0xFF);
		ucDoNotApplyRevocation = (HUINT8)(ulFTAContentMgrValue & 0xFF);

		HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG level : %s \n"),  xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
		HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotScramble : %d  \n"),  ucDoNotScramble);
		HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucCtrlRemoteAccess : %d  \n"),  ucCtrlRemoteAccess);
		HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotApplyRevocation : %d  \n"),  ucDoNotApplyRevocation);

		eFtaContentMgr = xmgr_rec_GetCopyControlState_Ukdtt(ucDoNotScramble, ucCtrlRemoteAccess);
	}
	else
	{
		HxLOG_Debug("invalid level (%d)[%s] \n",eFtaContentLevel,xmgr_rec_MakeStrFtaContentLevel_Ukdtt(eFtaContentLevel));
	}


END_FUNC:
	return eFtaContentMgr;
}

STATIC HUINT32		xmgr_rec_GetFTAContentMgrFromEvent_Ukdtt(xmgrRec_Context_t *pstContext)
{
	DxUkdtt_PVR_FtaContentMgmtState_e	eFtaContentMgr = eDxUkdtt_CONTENT_MGMT_STATE_None;
	DbSvc_Info_t		stSvcInfo;
	SvcSi_Event_t		stEvtInfo;
	SvcSi_ExtEvent_t	stExtEvtInfo;
	DbSvc_TripleId_t	stTripleId;
	HERROR				hErr;
	HxList_t *pstList = NULL;
	HBOOL	bFoundPrivate = FALSE;

	HxSTD_MemSet(&stEvtInfo, 0, sizeof(SvcSi_Event_t) );
	HxSTD_MemSet(&stExtEvtInfo, 0, sizeof(SvcSi_ExtEvent_t));

	// 1. EIT
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo Error  !! \n");
		goto END_FUNC;
	}

	stTripleId.usOnId	= DbSvc_GetOnId(&stSvcInfo);
	stTripleId.usTsId	= DbSvc_GetTsId(&stSvcInfo);
	stTripleId.usSvcId	= DbSvc_GetSvcId(&stSvcInfo);

	hErr = SVC_SI_GetEvtInfoFromPfEvent(pstContext->ulActionId, stTripleId, TRUE, TRUE, &stEvtInfo, &stExtEvtInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_SI_GetEvtInfoFromPfEvent Error  !! \n");
		goto END_FUNC;
	}

	for (pstList = stEvtInfo.pstEitDescList; pstList != NULL; pstList = pstList->next)
	{
		SIxDescriptor_t *pstDes = (SIxDescriptor_t *)HLIB_LIST_Data(pstList);
		if (pstDes == NULL) 	{ continue; }

		switch (pstDes->stRaw.eTag)
		{
			case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
			{
				SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)pstDes;

				if(pPrivDataSpecDes != NULL)
				{
					if (pPrivDataSpecDes->ulPrivateDataSpec == 0x0000233a)	// PRIVATE_DATA_SPECIFIER_FOR_UK_DTT
					{
						HxLOG_Debug("Found private data. \n");
						bFoundPrivate = TRUE;
					}
					else
					{
						HxLOG_Warning("Found FOREIGN private data");
						bFoundPrivate = FALSE;
					}
				}

				break;
			}

			default:
				break;
		}
	}
	if (bFoundPrivate == TRUE)
	{
		// FTA Content management descriptor From EIT
		if(stEvtInfo.ucDoNotScramble != 0xff) // received from EIT
		{
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotScramble : %d  \n"),  stEvtInfo.ucDoNotScramble);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucCtrlRemoteAccess : %d  \n"),  stEvtInfo.ucCtrlRAccOverINet);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucDoNotApplyRevocation : %d  \n"),  stEvtInfo.ucDoNotApplyRevocation);

			eFtaContentMgr = xmgr_rec_GetCopyControlState_Ukdtt(stEvtInfo.ucDoNotScramble, stEvtInfo.ucCtrlRAccOverINet);
		}
		else
		{
			HxLOG_Debug("invalid Value(same as init) dns(%d) craoi(%d) dnar(%d) \n ",stEvtInfo.ucDoNotScramble,stEvtInfo.ucCtrlRAccOverINet,stEvtInfo.ucDoNotApplyRevocation);
		}
	}

END_FUNC:
	if (SVC_SI_FreeEventInfo(&stEvtInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
	}
	return eFtaContentMgr;
}
STATIC HERROR		xmgr_rec_UpdateFTAContentMgr_Ukdtt (xmgrRec_Context_t *pstContext)
{
	DxUkdtt_PVR_FtaContentMgmtState_e	eFtaContentMgr = eDxUkdtt_CONTENT_MGMT_STATE_None;
	HERROR				hErr = ERR_FAIL;
	SvcMeta_Record_t	stRecInfo;

	hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo failed. cid=(%d) \n",pstContext->ulContentId);
		return ERR_FAIL;
	}

	eFtaContentMgr = xmgr_rec_GetFTAContentMgrFromEvent_Ukdtt(pstContext);
	if (eFtaContentMgr == eDxUkdtt_CONTENT_MGMT_STATE_None)
	{
		HxLOG_Debug("No FtaContentMgr in event. Try service \n");
		eFtaContentMgr = xmgr_rec_GetFTAContentMgrFromService_Ukdtt(pstContext);
		if (eFtaContentMgr == eDxUkdtt_CONTENT_MGMT_STATE_None)
		{
			HxLOG_Debug("No FtaContentMgr in service. Try network \n");
			eFtaContentMgr = xmgr_rec_GetFTAContentmgrFromNetwork_Ukdtt(pstContext);
			if (eFtaContentMgr == eDxUkdtt_CONTENT_MGMT_STATE_None)
			{
				HxLOG_Error("FtaContent signalled but No FtaContentMgr.\n");
				return ERR_FAIL;
			}
		}
	}
	HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG change %d->%d. cid=(%d)  \n"), stRecInfo.ulFtaCmVal,eFtaContentMgr, pstContext->ulContentId);
	if (stRecInfo.ulFtaCmVal == eFtaContentMgr)
	{
		HxLOG_Debug("Same fta cm value (%d) \n",eFtaContentMgr);
		return ERR_OK;
	}

	stRecInfo.ulFtaCmVal = eFtaContentMgr;
	if(eFtaContentMgr == eDxUkdtt_CONTENT_MGMT_STATE_ManagedCopy)
	{
		stRecInfo.ucCopyTime = DxUKDTT_PVR_AVAILABLE_CONTENT_COPY_NUMBER;
	}
	else if (eFtaContentMgr == eDxUkdtt_CONTENT_MGMT_STATE_Reserved)
	{
		stRecInfo.ucCopyTime = 0;
	}

	HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ulFtaCmVal : %d  \n"), stRecInfo.ulFtaCmVal);
	HxLOG_Debug(HxANSI_COLOR_YELLOW("CMG ucCopyTime : %d  \n"), stRecInfo.ucCopyTime);

	hErr = SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_META_SetRecordInfo failed. cid=(%d) \n",pstContext->ulContentId);
	}

	return ERR_OK;
}
#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Result_t xmgr_rec_MsgBusTimer_Ukdtt (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
 	HINT32 nTimerID = 0 ;

	/* Instant Recording 의 Endtime 처리*/
	nTimerID = p1;

	switch(nTimerID)
	{
		case DELAYED_FTA_CMG_TIMER_ID:
			HxLOG_Debug("DELAYED_FTA_CMG_TIMER_ID \n");
			xmgr_rec_UpdateFTAContentMgr_Ukdtt(pstContext);
			BUS_KillTimer(DELAYED_FTA_CMG_TIMER_ID);
			break;
		case PANEL_STRING_TIMER_ID:
			BUS_KillTimer(PANEL_STRING_TIMER_ID);
			break;

		default:
			break;
	}

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t xmgr_rec_MsgEvtSiEitReceived_Ukdtt( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 			hErr = ERR_FAIL;
	HxDATETIME_t		stDateTime;

	/* startpid eit-pid */
	{
		pstContext->stRecPidInfo.stKnown.stSI[eREC_SiEIT].usPid	=	EIT_PID;
		hErr = SVC_REC_SetPID( pstContext->hAction, &pstContext->stRecPidInfo, TRUE );
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("[%s:%d] SVC_REC_StartPID hErr: hAction(%d)\n", __FUNCTION__, __LINE__, pstContext->hAction);
		}
	}

	/* save event meta */
	{
		DbSvc_Info_t	stSvcInfo;

		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
		if (ERR_OK == hErr)
		{
			SvcSi_Event_t		stEvtInfo;
			SvcSi_ExtEvent_t	stExtEvtInfo;
			DbSvc_TripleId_t  	stTripleId;

			HxSTD_MemSet(&stEvtInfo, 0, sizeof(SvcSi_Event_t) );
			HxSTD_MemSet(&stExtEvtInfo, 0, sizeof(SvcSi_ExtEvent_t) );
			HxSTD_MemSet(&stTripleId, 0, sizeof(DbSvc_TripleId_t) );

			stTripleId.usOnId	= DbSvc_GetOnId(&stSvcInfo);
			stTripleId.usTsId	= DbSvc_GetTsId(&stSvcInfo);
			stTripleId.usSvcId	= DbSvc_GetSvcId(&stSvcInfo);

			hErr = SVC_SI_GetEvtInfoFromPfEvent(pstContext->ulActionId, stTripleId, TRUE, TRUE, &stEvtInfo, &stExtEvtInfo);
			if (ERR_OK == hErr)
			{
				SvcMeta_Event_t 	stMetaEvt;
				SvcMeta_Record_t	stRecInfo;
				HBOOL 	bSave = TRUE;
				HINT32	nExtTextLen = 0;
				HxList_t *pstList = NULL;
				HBOOL	bFoundPrivate = FALSE;
				HBOOL	bFoundguidance = FALSE;

				if ( pstContext->usBaseEventId != 0xffff						/* 0xffff: 초기화된 값 */
					 && (pstContext->usBaseEventId != stEvtInfo.nEvtId) )
				{
					// event가 끝나도 schedule에서 stop이 안 되어서(end transition의 처리가 늦어서), 다른 event info를 받는 경우 무시하도록 예외처리
					HxLOG_Debug("Received eventId=(%d), rec eventId=(%d) \n",stEvtInfo.nEvtId, pstContext->usBaseEventId);
					if ((pstContext->stSetup.eRecType == eMgrRecType_EBR || pstContext->stSetup.eRecType == eMgrRecType_SBR))
					{
						HxLOG_Error("Event Based Recording. Ignore received eit. \n");
						if (SVC_SI_FreeEventInfo(&stEvtInfo) != ERR_OK)
						{
							HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
						}
						return ERR_OK;
					}
				}

				HxSTD_MemSet(&stMetaEvt, 0, sizeof(SvcMeta_Event_t) );
				stMetaEvt.pstExtEvt = NULL;

				/* convert si_eitinfo to pvr_meta */
				{
					stMetaEvt.nEventId				=	stEvtInfo.nEvtId;
					stMetaEvt.ulStartTime			=	stEvtInfo.ulStartTime;
					stMetaEvt.ulDuration			=	stEvtInfo.ulDuration;
					stMetaEvt.u3RunnigStatus		=	stEvtInfo.ucRunStatus;
					stMetaEvt.ucParentalRatingNum	=	stEvtInfo.ucParentRating;
					stMetaEvt.ucContentNibble		=	stEvtInfo.ucContent;
					stMetaEvt.ucUserNibble			=	stEvtInfo.ucContent2nd;
					stMetaEvt.u1FreeCaMode			=	stEvtInfo.ulCasType;
					stMetaEvt.pNext					=	NULL;//(struct tagPVR_EVT *)stEvtInfo.pNext;

					HxSTD_MemCopy(stMetaEvt.aucEventName, stEvtInfo.szName, PVR_SHORT_EVENT_NAME_LEN);
					HxSTD_MemCopy(stMetaEvt.aucEventText, stEvtInfo.szText, PVR_EVENT_TEXT);

					HxLOG_Debug("\033[30;43m ##########Service (0x%04x : %s)   ########\033[0m\n",DbSvc_GetSvcId(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
					HxLOG_Debug("\033[30;43m ########## Event (0x%x) Metadata ########  \033[0m\n",stMetaEvt.nEventId);
					HxLOG_Debug("\033[30;43m Name : %s  \033[0m\n", stMetaEvt.aucEventName);
					HxLOG_Debug("\033[30;43m Text : %s  \033[0m\n", stMetaEvt.aucEventText);

					HLIB_DATETIME_ConvertUnixTimeToDateTime(stMetaEvt.ulStartTime, &stDateTime);

					HxLOG_Debug("\033[30;43m Starttime : %d  (%02d/%02d - %02d:%02d )\033[0m\n",
						stMetaEvt.ulStartTime, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute);
					HxLOG_Debug("\033[30;43m Duration : %d  \033[0m\n", stMetaEvt.ulDuration);

				}

				//	stMetaEvt.ucReserved
				//	stMetaEvt.astParentalRating[PVR_PARENTAL_RATING_MAX];
				//	stMetaEvt.stComponent[EPG_COMPONENT_DESC_MAX];
				//	stMetaEvt.ulOffsetOfPlayTime;
				//	stMetaEvt.pstExtEvt;
				//		stEvtInfo.ulVidComponentStatus;
				//		stEvtInfo.ulAudComponentStatus;
				//		stEvtInfo.ulAuxComponentStatus;
				//		stEvtInfo.bExtEvtInfo;
				//		stEvtInfo.bComponent;

				hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo);
				if(hErr == ERR_OK)
				{
					// Guidance Info descriptor
					for (pstList = stEvtInfo.pstEitDescList; pstList != NULL; pstList = pstList->next)
					{
						SIxDescriptor_t *pstDes = (SIxDescriptor_t *)HLIB_LIST_Data(pstList);
						if (bFoundguidance == TRUE ) { continue; }
						if (pstDes == NULL) 	{ continue; }

						switch (pstDes->stRaw.eTag)
						{
							case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
							{
								SIxPrivateDataSpecDes_t *pPrivDataSpecDes = (SIxPrivateDataSpecDes_t *)pstDes;

								if(pPrivDataSpecDes != NULL)
								{
									if (pPrivDataSpecDes->ulPrivateDataSpec == 0x0000233a)	// PRIVATE_DATA_SPECIFIER_FOR_UK_DTT
									{
										HxLOG_Debug("Found private data. \n");
										bFoundPrivate = TRUE;
									}
									else
									{
										HxLOG_Warning("Found FOREIGN private data");
										bFoundPrivate = FALSE;
									}
								}

								break;
							}
							case eSIxDESCTAG_UKDTT_GUIDANCE:
							{
								SIxUkdttGuidanceDes_t	*pstUkdttGuidanceDesc = (SIxUkdttGuidanceDes_t *)pstDes;
								HUINT32 				 ulStrLen = 0;

								if (bFoundPrivate != TRUE)
								{
									HxLOG_Debug("Guidance found but coudn't found private data \n");
									break;
								}

								HxLOG_Debug("Guidance found \n");
								ulStrLen = HxSTD_StrLen(pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance);
								if (ulStrLen > DxMAX_GUIDANCE_DATA_LEN)
								{
									ulStrLen = DxMAX_GUIDANCE_DATA_LEN;
								}

								stMetaEvt.stGuidance.eType = pstUkdttGuidanceDesc->ucGuidanceType;

								if(stMetaEvt.stGuidance.eType == eDxUkdtt_GUIDANCE_TYPE_LANGCODE || stMetaEvt.stGuidance.eType == eDxUkdtt_GUIDANCE_TYPE_MODE_SPECIFIED)
								{
									stMetaEvt.stGuidance.ucMode = 0xff;
									HxSTD_StrNCpy(stMetaEvt.stGuidance.stLangCode, pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->szIso639LangCode, LANG_639_CODE_MAX);
									HxSTD_StrNCpy(stMetaEvt.stGuidance.stData, pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance, ulStrLen);
									if (stMetaEvt.stGuidance.eType == eDxUkdtt_GUIDANCE_TYPE_MODE_SPECIFIED)
									{
										stMetaEvt.stGuidance.ucMode = pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->ucGuidanceMode;
									}
								}
								else
								{
									HxSTD_StrNCpy(stMetaEvt.stGuidance.stData, pstUkdttGuidanceDesc->unInfo.pUkDttGuidanceInfo->pszGuidance, ulStrLen);
								}

								stMetaEvt.bUseGuidance = TRUE;
								HxLOG_Print("\033[30;43m[%s : %d] Guidance eType : %d  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.stGuidance.eType);
								HxLOG_Print("\033[30;43m[%s : %d] Guidance ucMode : %d	\033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.stGuidance.ucMode);
								HxLOG_Print("\033[30;43m[%s : %d] Guidance LangCode : %s  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.stGuidance.stLangCode);
								HxLOG_Print("\033[30;43m[%s : %d] Guidance data : %s  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.stGuidance.stData);

								bFoundguidance = TRUE;
								break;
							}

							default:
								break;
						}
					}
					if (bFoundPrivate == TRUE)
					{
						// FTA Content management descriptor From EIT
						if(stEvtInfo.ucDoNotScramble != 0xff) // received from EIT
						{
							stMetaEvt.stFtaCntMgmt.ucDoNotScramble = stEvtInfo.ucDoNotScramble;
							stMetaEvt.stFtaCntMgmt.ucCtrlRAccOverINet = stEvtInfo.ucCtrlRAccOverINet;
							stMetaEvt.stFtaCntMgmt.ucDoNotApplyRevocation = stEvtInfo.ucDoNotApplyRevocation;

							stMetaEvt.bUseFtaCntMgmt = TRUE;

							HxLOG_Debug(HxANSI_COLOR_YELLOW("set DELAYED_FTA_CMG_TIMER_ID \n"));
							BUS_SetTimer(DELAYED_FTA_CMG_TIMER_ID, DELAYED_FTA_CMG_TIME_OUT);


							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucDoNotScramble : %d	\n"),  stMetaEvt.stFtaCntMgmt.ucDoNotScramble);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucCtrlRAccOverINet : %d  \n"),	stMetaEvt.stFtaCntMgmt.ucCtrlRAccOverINet);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucDoNotApplyRevocation : %d  \n"),	stMetaEvt.stFtaCntMgmt.ucDoNotApplyRevocation);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ulFtaCmVal : %d  \n"),	stRecInfo.ulFtaCmVal);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucCopyTime : %d  \n"),	stRecInfo.ucCopyTime);

						}
						else
						{
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ulFtaCmVal : %d  \n"),	stRecInfo.ulFtaCmVal);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucCopyTime : %d  \n"),	stRecInfo.ucCopyTime);

							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucDoNotScramble : %d	\n"),  stEvtInfo.ucDoNotScramble);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucCtrlRAccOverINet : %d  \n"),	stEvtInfo.ucCtrlRAccOverINet);
							HxLOG_Debug(HxANSI_COLOR_YELLOW(" CMG ucDoNotApplyRevocation : %d  \n"),	stEvtInfo.ucDoNotApplyRevocation);
							// Received From SDT
						}
					}

					hErr = SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo);
					if (hErr != ERR_OK)
					{
						HxLOG_Error("SVC_META_SetRecordInfo failed. cid=(%d) \n",pstContext->ulContentId);
					}
				}


				// Extended Event info
				if (TRUE == stEvtInfo.bExtEvtInfo)
				{
					HUINT32 i = 0;
					HUINT32 num = 0;

					stMetaEvt.pstExtEvt	=	(SvcMeta_ExtEvt_t*)OxMGR_Calloc(sizeof(SvcMeta_ExtEvt_t));
					if(stMetaEvt.pstExtEvt !=  NULL)
					{
						HxSTD_MemCopy(stMetaEvt.pstExtEvt->szIso639LangCode, stExtEvtInfo.aucLang, 4);
						HxSTD_MemSet (stMetaEvt.pstExtEvt, 0, sizeof(SvcMeta_ExtEvt_t));
						if (stExtEvtInfo.szTextChar)
						{
							nExtTextLen = HLIB_STD_StrNLen(stExtEvtInfo.szTextChar, PVR_EVENT_TEXT);

							if (nExtTextLen > 0 )
							{
								stMetaEvt.pstExtEvt->pszChar	=	(HUINT8*)OxMGR_Calloc( nExtTextLen );
								HxSTD_MemCopy(stMetaEvt.pstExtEvt->pszChar, stExtEvtInfo.szTextChar, nExtTextLen);
							}
						}
						HxLOG_Print("\033[30;43m[%s : %d] Long Text : %s[%s]  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.pstExtEvt->pszChar, stMetaEvt.pstExtEvt->szIso639LangCode);

						if(stExtEvtInfo.nNumItem > PVR_EXT_ITEM_MAX)
							num = PVR_EXT_ITEM_MAX;
						else
							num = stExtEvtInfo.nNumItem;
						HxLOG_Print("\033[30;43m[%s : %d] Ext info num : %d  \033[0m\n", __FUNCTION__, __LINE__, num);
						HxLOG_Print("\033[30;43m[%s : %d] {  \033[0m\n", __FUNCTION__, __LINE__);
						for(i=0; i<num; i++)
						{
							HUINT32 len = 0;
							len = HxSTD_StrLen(stExtEvtInfo.astEvtItem[i].szItemChar);
							if(len > 0)
							{
								stMetaEvt.pstExtEvt->astItem[i].pszItem = OxMGR_Calloc(len + 1);
								if(stMetaEvt.pstExtEvt->astItem[i].pszItem)
								{
									HxSTD_StrNCpy(stMetaEvt.pstExtEvt->astItem[i].pszItem, stExtEvtInfo.astEvtItem[i].szItemChar, len);
									HxLOG_Print("\033[30;43m[%s : %d] \tItem : %s  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.pstExtEvt->astItem[i].pszItem);
								}
							}
							len = HxSTD_StrLen(stExtEvtInfo.astEvtItem[i].szItemDesc);
							if(len > 0)
							{
								stMetaEvt.pstExtEvt->astItem[i].pszDescription = OxMGR_Calloc(len + 1);
								if(stMetaEvt.pstExtEvt->astItem[i].pszDescription)
								{
									HxSTD_StrNCpy(stMetaEvt.pstExtEvt->astItem[i].pszDescription, stExtEvtInfo.astEvtItem[i].szItemDesc, len);
									HxLOG_Print("\033[30;43m[%s : %d] \tDesc : %s  \033[0m\n", __FUNCTION__, __LINE__, stMetaEvt.pstExtEvt->astItem[i].pszDescription);
								}
							}
						}
						HxLOG_Print("\033[30;43m[%s : %d] }  \033[0m\n", __FUNCTION__, __LINE__);
					}
				}


				SVC_META_SetEventInfo( pstContext->ulContentId, &stMetaEvt, bSave);

				/* timestamp */
				{
					SvcRec_RecInfo stMetaInfo;
					HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcRec_RecInfo));

					hErr = SVC_REC_GetInfo (hAction, &stMetaInfo);

					SVC_META_SetEventTimeStamp(pstContext->ulContentId, stMetaInfo.ulDuration, &stMetaEvt);
				}

				if (NULL != stExtEvtInfo.szTextChar)
					OxMGR_Free(stExtEvtInfo.szTextChar);

				if (NULL != stMetaEvt.pstExtEvt )
				{
					if (NULL != stMetaEvt.pstExtEvt->pszChar)
						OxMGR_Free (stMetaEvt.pstExtEvt->pszChar);

					OxMGR_Free (stMetaEvt.pstExtEvt);
				}
			}

			if (SVC_SI_FreeEventInfo(&stEvtInfo) != ERR_OK)
			{
				HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
			}
		}
	}

	return ERR_OK;
}

#define _____PROTO_PROCEDURE_____
STATIC BUS_Result_t xproc_rec_Proto_Ukdtt (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		/*MSG*/
		case eMEVT_BUS_READY_SHUTDOWN :
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusGoStandby_Base(pstContext, hAction, p1, p2, p3);
			return MESSAGE_PASS;

		case eMEVT_BUS_CREATE:
			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : RECORDING : Ukdtt :CREATE \n");
			HxLOG_Warning ("********************************************************************************\n");

			xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
			xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);

			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);

			// Always encryption for ukdtt. Need to implement.
			eBusResult = xmgr_rec_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

			/* update status to svc_meta */
			{
				HERROR hErr;
				hErr = SVC_META_RegisterPvrAction(pstContext->hAction, pstContext->ulContentId, eSvcMeta_UsingActType_Record);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_META_RegisterPvrAction. Error\n");
				}
			}
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
#if defined (CONFIG_DEBUG)
			xmgr_rec_PrintRecInfo_Ukdtt(pstContext);
#endif
			eBusResult = xmgr_rec_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

			HxLOG_Warning ("********************************************************************************\n");
			HxLOG_Warning ("*  XMGR : RECORDING : Ukdtt : DESTROY \n");
			HxLOG_Warning ("********************************************************************************\n");
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusTimer_Ukdtt(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_ENDTIME_UPDATE :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_PVR_RECORD_ENDTIME_UPDATE\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-SI*/
		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			xmgr_rec_CheckPMT(pstContext, hAction, p1, p2, p3);
			//Need to implement ukdtt spec
			// TODO : eBusResult = xmgr_rec_CiPlusMsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT :
		case eSEVT_SI_PMT_TIMEOUT :
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			//Need to implement ukdtt spec
			eBusResult = xmgr_rec_MsgEvtSiEitReceived_Ukdtt(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_MULTIFEED_DETECTED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_MULTIFEED_DETECTED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiMultifeedDetected_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_FREE_CA_MODE_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_SDT_FREE_CA_MODE_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_SDT_RUNNING_STATUS_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_FTA_CONTENTS_SIGNAL: // from NIT, SDT
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_FTA_CONTENTS_SIGNAL\n", ulActionId);
			HxLOG_Debug(HxANSI_COLOR_YELLOW("set DELAYED_FTA_CMG_TIMER_ID p1(%d) p2(%d) p3(%d)\n"),p1,p2,p3);
			BUS_SetTimer(DELAYED_FTA_CMG_TIMER_ID, DELAYED_FTA_CMG_TIME_OUT);
			break;

		/*EVT-CH*/
		case eSEVT_CH_NO_SIGNAL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_NO_SIGNAL\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChNoSignal_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CH_MOVING_MOTOR :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_MOVING_MOTOR\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChMovingMotor_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CH_LOCKED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_LOCKED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChLocked_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : EVT_FS_NOTIFY_USB_DISCONNECTED\n", ulActionId);
			if (p2 == FALSE)	// Ext Hdd
			{
				eBusResult = xmgr_rec_MsgEvtExtStorageDisconnected_Base (pstContext, hAction, p1, p2, p3);
			}
			break;

		/* EVT-DB */
		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_DB_CHECK_SVC_DB\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtDbCheckSvcDb_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-SVC_Rec */
		case eSEVT_REC_PID_CHANGED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecPidChanged_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_TIMECODE_CHANGE :
			CHECK_REC_ACTION_HANDLE();
//			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_SCRAMBLED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecScrambled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-Xmgr_Rec */
		case eSEVT_REC_REQ_COMMAND:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_REQ_COMMAND\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtReqCommand_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_NO_VIDEO_IN_PMT:
			// Need to implement.
			break;

		default :
			break;
	}

	HxLOG_Info("RunState message(0x%x) hAction(0x%x) p1(0x%x) p2(0x%x) p3(0x%x) \n",message, hAction, p1, p2, p3);
	xmgr_rec_RunState(pstContext);

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC BUS_Result_t xproc_rec_Action0_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (0);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action1_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (1);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action2_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (2);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action3_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (3);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action4_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (4);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action5_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (5);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action6_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (6);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xproc_rec_Action7_Ukdtt (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (7);

	return xproc_rec_Proto_Ukdtt (pstContext, message, hAction, p1, p2, p3);
}

#define _________PLUGIN_FUNC__________
HCHAR*	xmgr_recorder_GetProcName_Ukdtt (HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return "xmgr_rec_Proc0_Ukdtt";

		case (eActionId_REC_FIRST + 1):
			return "xmgr_rec_Proc1_Ukdtt";

		case (eActionId_REC_FIRST + 2):
			return "xmgr_rec_Proc2_Ukdtt";

		case (eActionId_REC_FIRST + 3):
			return "xmgr_rec_Proc3_Ukdtt";

		case (eActionId_REC_FIRST + 4):
			return "xmgr_rec_Proc4_Ukdtt";

		case (eActionId_REC_FIRST + 5):
			return "xmgr_rec_Proc5_Ukdtt";

		case (eActionId_REC_FIRST + 6):
			return "xmgr_rec_Proc6_Ukdtt";

		case (eActionId_REC_FIRST + 7):
			return "xmgr_rec_Proc7_Ukdtt";

		default:
			break;
		}
	}

	return NULL;
}

BUS_Callback_t xmgr_recorder_GetProcFunc_Ukdtt (HUINT32 ulActionId)
{
	// Code 내에서 Record 갯수가 최대 몇개 인지 알기 힘드므로
	// switch 함수 위에 ulActionID 영역을 확인하여 실제로 넘치는 값이어도 logic상 들어오지 않게 만든다.

	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return xproc_rec_Action0_Ukdtt;

		case (eActionId_REC_FIRST + 1):
			return xproc_rec_Action1_Ukdtt;

		case (eActionId_REC_FIRST + 2):
			return xproc_rec_Action2_Ukdtt;

		case (eActionId_REC_FIRST + 3):
			return xproc_rec_Action3_Ukdtt;

		case (eActionId_REC_FIRST + 4):
			return xproc_rec_Action4_Ukdtt;

		case (eActionId_REC_FIRST + 5):
			return xproc_rec_Action5_Ukdtt;

		case (eActionId_REC_FIRST + 6):
			return xproc_rec_Action6_Ukdtt;

		case (eActionId_REC_FIRST + 7):
			return xproc_rec_Action7_Ukdtt;

		default:
			break;
		}
	}

	return NULL;
}

HERROR xmgr_recorder_CreateRecordFailReason_Ukdtt (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule)
{
	HUINT32			 ulContentId;
	UNIXTIME		 utStartTime;
	HCHAR			 szFileName[FS_MAX_FILE_NAME_LEN];
//	DxSchedule_t	 stSchedule;
	SvcMeta_Record_t	 stMetaInfo;
	HERROR			 hErr;

	if (pstRecSetup == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("invalid param. setup(0x%x) schedule(0x%x) \n",pstRecSetup, pstSchedule);
		return ERR_FAIL;
	}

	if (eEndReason == eMgrRecFailed_HIGHER_PRIORITY)
	{
		HxLOG_Debug("Pending broken meta file. \n");
	}

	/* Recording 이 시작하기 전에 hErr 가 발생한 것을 meta 파일로 만든다.*/
	HxLOG_Debug("eEndReason: %d \n", eEndReason);
	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));

	stMetaInfo.ulFtaCmVal = eDxUkdtt_CONTENT_MGMT_STATE_None;

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utStartTime);

	stMetaInfo.ulStartTime		= utStartTime - pstSchedule->uExtInfo.stRec.ulPreOffsetTime;
	stMetaInfo.ulEndTime			= utStartTime + 1;
	stMetaInfo.ucEndCondition	= (HUINT8)eEndReason;
	stMetaInfo.ucRecordStatus	= eMgrRec_STS_INCOMPLETED;
	stMetaInfo.nEventId			= pstSchedule->uExtInfo.stRec.nEvtId;
	if ((DxRSVTYPE_RECORD_TBR != pstSchedule->eRsvType) && (DxRSVTYPE_RECORD_SATRECORD_TBR != pstSchedule->eRsvType))
	{
		stMetaInfo.u2cEventBase	= TRUE;
	}

	HxLOG_Info ("evtName(%s) evtId(0x%04x)\n", pstSchedule->uExtInfo.stRec.szEventName, pstSchedule->uExtInfo.stRec.nEvtId);
	snprintf(stMetaInfo.aucPath, FS_MAX_FILE_NAME_LEN, "%s", (char *)xmgr_recorder_GetRecordPath_Base(pstRecSetup->ulPvrIdx));

	xmgr_rec_ConvertUtf8ToByte_Ukdtt (pstSchedule->uExtInfo.stRec.szEventName, szFileName);
	HLIB_STD_StrUtf8NCpy (stMetaInfo.aucDisplayEvtName, szFileName, PVR_MAX_TITLE_LEN - 1);

	if (pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR || pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
	{
		if (pstSchedule->uExtInfo.stRec.ex.tva.ucCRIDType == DxCRID_TYPE_PROG)
		{
			stMetaInfo.stCrid[0].ucType = DxCRID_TYPE_PROG;
			HxSTD_snprintf(stMetaInfo.stCrid[0].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pstSchedule->uExtInfo.stRec.ex.tva.szCRID);
		}
		else if (pstSchedule->uExtInfo.stRec.ex.tva.ucCRIDType == DxCRID_TYPE_SERIES)
		{
			stMetaInfo.bSeriesRec = TRUE;

			stMetaInfo.stCrid[0].ucType = DxCRID_TYPE_SERIES;
			HxSTD_snprintf(stMetaInfo.stCrid[0].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pstSchedule->uExtInfo.stRec.ex.tva.szCRID);
			if (!(HxSTD_StrEmpty(pstSchedule->uExtInfo.stRec.ex.tva.szProgCRID)))
			{
				stMetaInfo.stCrid[1].ucType = DxCRID_TYPE_PROG;
				HxSTD_snprintf(stMetaInfo.stCrid[1].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pstSchedule->uExtInfo.stRec.ex.tva.szProgCRID);
			}
		}
	}

	hErr = SVC_META_MakeFileName (stMetaInfo.ulStartTime, szFileName, stMetaInfo.aucFileName, HxPATH_MAX);
	hErr = SVC_META_AddContentsInfo (pstRecSetup->hCurrSvc, &stMetaInfo, &ulContentId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PVR_AddContentsInfo err: SVCHandle:(0x%08x)\n", pstRecSetup->hCurrSvc);
		return ERR_FAIL;
	}

	BUS_PostMessage(NULL, eSEVT_META_UPDATED, (Handle_t)HANDLE_NULL, ulContentId, 1, 0);
	return ERR_OK;
}


#if defined (CONFIG_DEBUG)
void xmgr_rec_PrintKey(HUINT8 *pucKey)
{
	HUINT8 k = 0;
	HxLOG_Print("[%s:%d] aucKey= [ ", __FUNCTION__, __LINE__);
	for(k=0;k<16;k++)
	{
		HxLOG_Print("%02x ", pucKey[k]);
	}
	HxLOG_Print(" ]\n");
}

STATIC void xmgr_rec_PrintMetaInfo_Ukdtt(xmgrRec_Context_t *pstContext)
{
	SvcMeta_Record_t stMetaInfo;
	if(pstContext== NULL)
	{
		return;
	}
	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));
	SVC_META_GetRecordInfo(pstContext->ulContentId, &stMetaInfo);

	HxLOG_Debug("++++++++++++++ pstContext->stMetaInfo ++++++++++++++\n");
	HxLOG_Debug("+ path = %s\n",  pstContext->stMetaInfo.aucPath);
	HxLOG_Debug("+ fileName = %s\n",  pstContext->stMetaInfo.aucFileName);
	HxLOG_Debug("+ ulStartTime(%d), ulEndTime(%d), ulRecTime(%d), ulOrgDuration(%d\n", pstContext->stMetaInfo.ulStartTime, pstContext->stMetaInfo.ulEndTime, pstContext->stMetaInfo.ulRecTime,  pstContext->stMetaInfo.ulOrgDuration);
	HxLOG_Debug("+ ucRecordStatus(%d)\n", pstContext->stMetaInfo.ucRecordStatus);
	HxLOG_Debug("+ u2cEventBase(%d)\n", pstContext->stMetaInfo.u2cEventBase);
	HxLOG_Debug("+ nEventId(%d)\n", pstContext->stMetaInfo.nEventId);
	HxLOG_Debug("+ u1cLocked(%d)\n", pstContext->stMetaInfo.u1cLocked);
	HxLOG_Debug("+ u1cWatched(%d)\n", pstContext->stMetaInfo.u1cWatched);
	HxLOG_Debug("+ u1cScrambled(%d)\n", pstContext->stMetaInfo.u1cScrambled); // exist any scramble position in record file
	HxLOG_Debug("+ u1cEncryption(%d)\n", pstContext->stMetaInfo.u1cEncryption);
	HxLOG_Debug("+ u1cAutoDeleted(%d)\n", pstContext->stMetaInfo.u1cAutoDeleted);
	HxLOG_Debug("+ ucEndCondition(%d)\n", pstContext->stMetaInfo.ucEndCondition);			// reason to Record Fail, Recocrd Split
	HxLOG_Debug("+ ulLastWatchtime(%d)\n", pstContext->stMetaInfo.ulLastWatchtime);
	HxLOG_Debug("+ bUserLocked(%d)\n", pstContext->stMetaInfo.bUserLocked);			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HxLOG_Debug("+ bWillBeDeleted(%d)\n", pstContext->stMetaInfo.bWillBeDeleted);
//	HUINT16			usNumOfBookmark;
//	SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	HxLOG_Debug("+ aucDisplayEvtName = %s\n", pstContext->stMetaInfo.aucDisplayEvtName);
	HxLOG_Debug("+ bSeriesRec(%d)\n", pstContext->stMetaInfo.bSeriesRec);
	HxLOG_Debug("+ szSeriesId(%s)\n", pstContext->stMetaInfo.szSeriesId);
	HxLOG_Debug("+ ulEpisodeNum(%d)\n", pstContext->stMetaInfo.ulEpisodeNum);
	HxLOG_Debug("+ ulSeason(%d)\n", pstContext->stMetaInfo.ulSeason);

	HxLOG_Debug("++++++++++++++ SVC_META_GetRecordInfo() -> MetaInfo ++++++++++++++\n");
	HxLOG_Debug("+ path = %s\n",  stMetaInfo.aucPath);
	HxLOG_Debug("+ fileName = %s\n",  stMetaInfo.aucFileName);
	HxLOG_Debug("+ ulStartTime(%d), ulEndTime(%d), ulRecTime(%d), ulOrgDuration(%d\n", stMetaInfo.ulStartTime, stMetaInfo.ulEndTime, stMetaInfo.ulRecTime,  stMetaInfo.ulOrgDuration);
	HxLOG_Debug("+ ucRecordStatus(%d)\n", stMetaInfo.ucRecordStatus);
	HxLOG_Debug("+ u2cEventBase(%d)\n", stMetaInfo.u2cEventBase);
	HxLOG_Debug("+ nEventId(%d)\n", stMetaInfo.nEventId);
	HxLOG_Debug("+ u1cLocked(%d)\n", stMetaInfo.u1cLocked);
	HxLOG_Debug("+ u1cWatched(%d)\n", stMetaInfo.u1cWatched);
	HxLOG_Debug("+ u1cScrambled(%d)\n", stMetaInfo.u1cScrambled); // exist any scramble position in record file
	HxLOG_Debug("+ u1cEncryption(%d)\n", stMetaInfo.u1cEncryption);
	HxLOG_Debug("+ u1cAutoDeleted(%d)\n", stMetaInfo.u1cAutoDeleted);
	HxLOG_Debug("+ ucEndCondition(%d)\n", stMetaInfo.ucEndCondition);			// reason to Record Fail, Recocrd Split
	HxLOG_Debug("+ ulLastWatchtime(%d)\n", stMetaInfo.ulLastWatchtime);
	HxLOG_Debug("+ bUserLocked(%d)\n", stMetaInfo.bUserLocked);			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HxLOG_Debug("+ bWillBeDeleted(%d)\n", stMetaInfo.bWillBeDeleted);
//	HUINT16			usNumOfBookmark;
//	SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	HxLOG_Debug("+ aucDisplayEvtName = %s\n", stMetaInfo.aucDisplayEvtName);
	HxLOG_Debug("+ bSeriesRec(%d)\n", stMetaInfo.bSeriesRec);
	HxLOG_Debug("+ szSeriesId(%s)\n", stMetaInfo.szSeriesId);
	HxLOG_Debug("+ ulEpisodeNum(%d)\n", stMetaInfo.ulEpisodeNum);
	HxLOG_Debug("+ ulSeason(%d)\n", stMetaInfo.ulSeason);

}

STATIC void xmgr_rec_PrintRecInfo_Ukdtt(xmgrRec_Context_t *pstContext)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("NULL context \n");
		return;
	}

	HxLOG_Debug("+ ulActionId(%d)\n", pstContext->ulActionId);
	HxLOG_Debug("+ hAction(%d)\n", pstContext->hAction);
	HxLOG_Debug("+ eRecState(%d)\n", pstContext->eRecState);
//	SvcRec_PIDList_t	stRecPidInfo;
	HxLOG_Debug("+ ulContentId(%d)\n", pstContext->ulContentId);
//	MgrRec_RecSetup_t	stSetup;
//	SvcMeta_Record_t 	stMetaInfo;
	HxLOG_Debug("+ bCasOk(%d)\n", pstContext->bCasOk);				// CAS에게 시청 허가
	HxLOG_Debug("+ nCasErrCode(%d)\n", pstContext->nCasErrCode);		// CAS error code
	HxLOG_Debug("+ bPMTValid(%d)\n", pstContext->bPMTValid);
	HxLOG_Debug("+ bCasDrmValid(%d)\n", pstContext->bCasDrmValid);		// CAS로부터 Drm&Copy protection 정보가 detect 됨. SI보다는 CAS가 우선 순위가 있음.
	HxLOG_Debug("+ bCasDrmNone(%d)\n", pstContext->bCasDrmNone);		// cas로부터 Drm 정보가 없다는 메시지를 받음
// 	CopyrightControl_t			stNewDrmInfo;		// SI, CAS중 최종 젹용할 drm info, Split 후 drm
	HxLOG_Debug("+ ulPauseTypeFlag(%d)\n", pstContext->ePauseFlag);	// pause 의 이유를 bit flag 함
	HxLOG_Debug("+ usBaseEventId(%d)\n", pstContext->usBaseEventId);		//	recording name에 대한 Base event id.
	HxLOG_Debug("+ ulStartTimeFromTimeCode(%d)\n", pstContext->ulStartTimeFromTimeCode);
	HxLOG_Debug("+ ulTsrtoDelayTime(%d)\n", pstContext->ulTsrtoDelayTime);
	HxLOG_Debug("+ nRecvEventNum(%d)\n", pstContext->nRecvEventNum);
	HxLOG_Debug("+ szDelayCopyFileName(%s)\n", pstContext->szDelayCopyFileName);
	HxLOG_Debug("+ bDelayedCopy(%d)\n", pstContext->bDelayedCopy);
//	xmgrRec_RecvEventInfo_t astRecvEventInfo[PVR_MAX_RECV_EVENT_NUM];
}
#endif

#endif
