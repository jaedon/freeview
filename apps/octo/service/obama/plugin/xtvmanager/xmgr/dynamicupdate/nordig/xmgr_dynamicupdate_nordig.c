/**
	@file     xmgr_dynamicupdate_nordig.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <otl.h>

#include <mgr_common.h>
#include <svc_pipe.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_si.h>

#include <xsvc_si.h>

#include <mgr_search.h>

#include "../local_include/_xmgr_dynamicupdate.h"
#include <_mgr_dynamicupdate.h>
#include <_xsvc_si_lib_lcn_base.h>

#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagDynamicUpdateContent_Nordig
{
	DynamicUpdate_Content_t			stBaseContents;

	// 향별 추가되는 field는 여기에. 밑으로.
	DxSvcType_e						eServiceTypeOption;

	HUINT32							ulNetNum;
	HUINT32							ulTsNum;
	HUINT32							ulPrvNum;
	HUINT32							ulSvcNum;
	HUINT32							ulBqNum;
	HBOOL							*pbNewTs;

	DbSvc_NetInfo_t 				*pstNetInfo;
	DbSvc_TsInfo_t					*pstTsInfo;
	DbSvc_PrvInfo_t 				*pstPrvArray;
	DbSvc_Info_t					*pstSvcArray;
	DbSvc_BouqInfo_t				*pstBqArray;
} DynamicUpdate_Content_Nordig_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC DynamicUpdate_Content_Nordig_t	s_stDynamicUpdateContent_Nordig;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_dynamicupdate_Nordig(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
STATIC HERROR		xmgr_DynamicUpdate_InitContext_Nordig(DynamicUpdate_Content_Nordig_t *pstContents);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

#ifdef CONFIG_DEBUG
const HINT8* INT_UTIL_GETENUMSTR_DU_UpdateFlag(DYNAMICUPDATE_UpdateFlag_e eUpdate)
{
	switch (eUpdate)
	{
	ENUM_TO_STR(eDYNAMICUPDATE_INIT);
	ENUM_TO_STR(eDYNAMICUPDATE_EXCLUDE);
	ENUM_TO_STR(eDYNAMICUPDATE_IDENTICAL);
	ENUM_TO_STR(eDYNAMICUPDATE_DELETE);
	ENUM_TO_STR(eDYNAMICUPDATE_ADD);
	default:
		break;
	}

	return ("Update");
}

const HINT8* INT_UTIL_GETENUMSTR_DU_AppendFlag(SRCHSVC_AppendFlag_t eAppendFlag)
{
	switch (eAppendFlag)
	{
	ENUM_TO_STR(eSRCHSVC_DONTADD);
	ENUM_TO_STR(eSRCHSVC_ADD);
	ENUM_TO_STR(eSRCHSVC_UPDATE);
	ENUM_TO_STR(eSRCHSVC_UPDATE_CHANGING_SVCNUM);
	ENUM_TO_STR(eSRCHSVC_SKIP);
	ENUM_TO_STR(eSRCHSVC_DELETE);
	default:
		break;
	}

	return ("Unknown");
}
#endif

INLINE STATIC DynamicUpdate_Content_Nordig_t *xmgr_DynamicUpdate_GetContent_Nordig (void)
{
	return &s_stDynamicUpdateContent_Nordig;
}

STATIC HERROR	xmgr_DynamicUpdate_InitContext_Nordig(DynamicUpdate_Content_Nordig_t *pstContents)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	if(pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}

	if(pstContents->pstPrvArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstPrvArray);
		pstContents->pstPrvArray = NULL;
	}

	if(pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(DynamicUpdate_Content_Nordig_t));

	pstContents->eServiceTypeOption = eDxSVC_TYPE_All;

	pstContents->stBaseContents.stDynamicUpdateSpec.eActionKind = eSVCSI_ACT_KIND_DynamicUpdate;
	pstContents->stBaseContents.stDynamicUpdateSpec.ulActionSpec = eSVCSI_ACT_SPEC_Nordig;

	/*
		여기로 왔다는 것은,
		이미 dynamic update를 동작시키겠다는 것이므로, On/Off의 의미가 없다.
		Off 시킬 때는 Action도 release되고, Proc도 destrory되는 게 맞음.
		불필요하게 item 읽지 말자
	*/
	pstContents->stBaseContents.bOnOff = TRUE;

	pstContents->stBaseContents.ulBGActionId = eActionId_Unknown;

	return hErr;
}

STATIC HERROR xmgr_dynamicupdate_msgCheckPreCondition_Nordig (DynamicUpdate_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	Handle_t 		hSvc = (Handle_t)p1;
	DbSvc_Info_t	stSvcInfo = {0, };
	HUINT16			usOnId = 0;

	hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);

	if(hErr == ERR_OK)
	{
#if defined (CONFIG_OP_TDC)
		// If this Service is manual searched channel, DU doesn't need to be performed.
		// LCN 채널이 아니라면 DU 할 필요 없지 않을까?  다른 Nordig model에도 공통 적용 필요
		if (DbSvc_GetLcnFlag(&stSvcInfo) == FALSE)
		{
			HxLOG_Debug("Not a LCN Channel... Skip DU!!\n");
			hErr = ERR_FAIL;
		}
#else
		usOnId = DbSvc_GetOnId(&stSvcInfo);

		if(xsvc_si_LcnCheckOnId_Nordig(usOnId) == FALSE)
		{
			hErr = ERR_FAIL;
		}

#endif
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return hErr;
}


INLINE STATIC HBOOL xmgr_dynamicupdate_isIdenticalService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew)
{
	if ((DbSvc_GetSvcId(sOld) != DbSvc_GetSvcId(sNew)) || (DbSvc_GetTsId(sOld) != DbSvc_GetTsId(sNew)) || (DbSvc_GetOnId(sOld) != DbSvc_GetOnId(sNew)))
	{
		return FALSE;
	}
	if (DbSvc_GetSvcType(sOld) != DbSvc_GetSvcType(sNew))
	{
		return FALSE;
	}

	return TRUE;
}

INLINE STATIC HBOOL xmgr_dynamicupdate_isModifiedService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew, SRCHSVC_AppendFlag_t eLcn)
{
	if (eLcn == eSRCHSVC_UPDATE || eLcn == eSRCHSVC_ADD)
	{
		if ((DbSvc_GetLcnFlag(sOld) == TRUE) && (DbSvc_GetLcn(sOld) != DbSvc_GetLcn(sNew)))
		{
			return TRUE;
		}
	}

	if ((DbSvc_GetRenamed(sOld) == FALSE) && (MWC_UTIL_DvbStrncmp((const HUINT8*)DbSvc_GetName(sOld), (const HUINT8*)DbSvc_GetName(sNew), /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()) != 0))
	{
		return TRUE;
	}

	if (DbSvc_GetVisibleFlag(sOld) != DbSvc_GetVisibleFlag(sNew))
	{
		return TRUE;
	}

	if ( (DbSvc_GetSimulcastOnId(sOld) != DbSvc_GetSimulcastOnId(sNew)) || (DbSvc_GetSimulcastTsId(sOld) != DbSvc_GetSimulcastTsId(sNew)) || (DbSvc_GetSimulcastSvcId(sOld) != DbSvc_GetSimulcastSvcId(sNew)) )
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR xmgr_dynamicupdate_saveResults_Nordig (DynamicUpdate_Content_Nordig_t* pstContents, HBOOL *pbUpdated)
{
	Handle_t				hSvc;
	HUINT16					usPrvIdx = 0;
	HUINT16					usNetIdx = 0;
	HUINT16					usTsIdx = 0;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_Condition_t		stSvcCond;
	HUINT32					ulTsNumInDb = 0;
	DbSvc_TsInfo_t			*pstTsInfoInDb = NULL;
	DbSvc_Info_t			*pstSvcInfoListNew = NULL, *pstSvcInfoListOld = NULL;
	Handle_t				*phListOld = NULL;
	DYNAMICUPDATE_UpdateFlag_e	*peSvcFlagArrayNew = NULL, *peSvcFlagArrayOld = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArrayForLcn = NULL;
	HUINT32					ulIndex, ulSubIndex, ulOldIdx, ulNewIdx;
	HUINT32					ulAdded, ulDeleted, ulUpdated, ulIdentical;
	HUINT32					ulPrvAdded, ulNetAdded, ulTsAdded, ulSvcAdded;
	HUINT32					ulTsUpdated, ulSvcUpdated;
	HUINT32					ulTsDeleted, ulSvcDeleted;
	HUINT32					ulCountNew = 0, ulCountOld = 0;
	HERROR					hErr = ERR_FAIL;

	// For TP Delete
	HUINT32					ulTsNumByOnIDInDb = 0;
	DbSvc_TsInfo_t			*pstTsInfoByOnIDInDb = NULL;
	DYNAMICUPDATE_UpdateFlag_e	*peTsFlagArrayOld = NULL;


	HxLOG_Trace();

	ulAdded = ulDeleted = ulUpdated = ulIdentical = 0;
	ulPrvAdded = ulNetAdded = ulTsAdded = ulSvcAdded = 0;
	ulTsUpdated = ulSvcUpdated = 0;
	ulTsDeleted = ulSvcDeleted = 0;

	if (pbUpdated == NULL)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	*pbUpdated = FALSE;
	if (pstContents->ulSvcNum == 0 || pstContents->pstSvcArray == NULL)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Add or Update a provider
	if (pstContents->ulPrvNum > 0 && pstContents->pstPrvArray != NULL)
	{
		for (ulIndex = 0; ulIndex < pstContents->ulPrvNum; ulIndex++)
		{
			hErr = DB_SVC_FindProviderIndexByName(pstContents->pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddProviderInfo(&(pstContents->pstPrvArray[ulIndex]), &usPrvIdx);
				ulPrvAdded++;
				HxLOG_Debug("[ADD-Provider]Provider Name : %02s, PrvIdx : %d\n",
					pstContents->pstPrvArray[ulIndex].szPrvName,
					usPrvIdx);
			}
			else
			{
				HxLOG_Debug("[FND-Provider]Provider Name : %02s, PrvIdx : %d\n",
					pstContents->pstPrvArray[ulIndex].szPrvName,
					usPrvIdx);
			}
			pstContents->pstPrvArray[ulIndex].usPrvIdx = usPrvIdx;
		}
	}

	// Add or Update a network
	if (pstContents->ulNetNum > 0 && pstContents->pstNetInfo != NULL)
	{
		for (ulIndex = 0; ulIndex < pstContents->ulNetNum; ulIndex++)
		{
			pstContents->pstNetInfo[ulIndex].eOrgDeliType = eDxDELIVERY_TYPE_CAB;
			hErr = DB_SVC_FindNetworkIndexById(pstContents->pstNetInfo[ulIndex].eOrgDeliType, pstContents->pstNetInfo[ulIndex].usNetId, &usNetIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddNetworkInfo (pstContents->pstNetInfo, &usNetIdx);
				ulNetAdded++;
				HxLOG_Debug("[ADD-Network]Network Name : %02s, usNetIdx : %d\n",
					pstContents->pstNetInfo[ulIndex].szNetName,
					usNetIdx);
			}
			else
			{
				HxLOG_Debug("[FND-Network]Network Name : %02s, usNetIdx : %d\n",
					pstContents->pstNetInfo[ulIndex].szNetName,
					usNetIdx);
			}
			pstContents->pstNetInfo[ulIndex].usNetIdx = usNetIdx;
		}
	}

	HxLOG_Debug("============================================\r\n");
	HxLOG_Debug(" NET : %d\r\n", pstContents->ulNetNum);
	HxLOG_Debug(" PRV : %d\r\n", pstContents->ulPrvNum);
	HxLOG_Debug(" TS : %d\r\n", pstContents->ulTsNum);
	HxLOG_Debug(" SVC : %d\r\n", pstContents->ulSvcNum);
	HxLOG_Debug("============================================\r\n");

	// Add or Update a transponder & service
	if (pstContents->ulTsNum > 0 && pstContents->pstTsInfo != NULL)
	{
		// ^O^, scipio, 2014-04-17, For TP Delete
		// 원래는 NetworkID로 DB에서 TP List를 가져와야 할 듯 한데...
		// 방법을 모르겠다. 어차피 Network 1개만 사용하고, NIT Actual밖에 사용하지 않아서
		// 아래와 같이 비교한다.
		DB_SVC_InitTsFindCondition(&stTsCond);
		stTsCond.nDeliType	= eDxDELIVERY_TYPE_CAB;
		stTsCond.nOnId		= (HINT32)pstContents->pstTsInfo[0].usOnId;
		hErr = DB_SVC_FindTsInfoList (&stTsCond, &ulTsNumByOnIDInDb, &pstTsInfoByOnIDInDb);

		if (ulTsNumByOnIDInDb > 0)
		{
			peTsFlagArrayOld = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulTsNumByOnIDInDb);

			if (peTsFlagArrayOld == NULL)
			{
				hErr = ERR_BUS_OUT_OF_MEMORY;
				goto ERROR_FAULT_OCCURRED;
			}

			for (ulIndex = 0; ulIndex < ulTsNumByOnIDInDb; ulIndex++)
			{
				peTsFlagArrayOld[ulIndex] = eDYNAMICUPDATE_DELETE;
			}
		}
		// ;-)

		for (ulIndex = 0; ulIndex < pstContents->ulTsNum; ulIndex++)
		{
/*********************************************************************/
			ulTsNumInDb = 0;
			pstTsInfoInDb = NULL;

			DB_SVC_InitTsFindCondition(&stTsCond);
			stTsCond.nDeliType	= pstContents->pstTsInfo[ulIndex].eDeliType;
			stTsCond.nOnId		= (HINT32)pstContents->pstTsInfo[ulIndex].usOnId;
			stTsCond.nTsId		= (HINT32)pstContents->pstTsInfo[ulIndex].usTsId;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &ulTsNumInDb, &pstTsInfoInDb);
			// ^O^, scipio, 2014-04-17, For TP Delete
			for (ulSubIndex = 0; ulSubIndex < ulTsNumByOnIDInDb; ulSubIndex++)
			{
				if ((pstContents->pstTsInfo[ulIndex].usOnId == pstTsInfoByOnIDInDb[ulSubIndex].usOnId)
				&& (pstContents->pstTsInfo[ulIndex].usTsId == pstTsInfoByOnIDInDb[ulSubIndex].usTsId))
				{
					peTsFlagArrayOld[ulSubIndex] = eDYNAMICUPDATE_EXCLUDE;
				}
			}
			// ;-)

			if (hErr == ERR_OK && ulTsNumInDb > 0 && pstTsInfoInDb != NULL)
			{
				pstContents->pstTsInfo[ulIndex].usTsIdx = pstTsInfoInDb[0].usTsIdx;
				if (DB_SVC_UtilCompareDbTuningParam(pstTsInfoInDb[0].eDeliType, &(pstContents->pstTsInfo[ulIndex].stTuningParam), &(pstTsInfoInDb[0].stTuningParam)) != 0)
				{
					DB_SVC_UpdateTsInfo(pstTsInfoInDb[0].usTsIdx, &pstContents->pstTsInfo[ulIndex]);
					ulTsUpdated++;
					HxLOG_Debug("======= TS INFO UPDATED =======\n");
					HxLOG_Debug("usTsIdx : %d, OID : 0x%04x, TID : 0x%04x\n",
						pstContents->pstTsInfo[ulIndex].usTsIdx,
						pstContents->pstTsInfo[ulIndex].usOnId,
						pstContents->pstTsInfo[ulIndex].usTsId);
					HxLOG_Debug("===============================\n");
				}
			}
			else
			{
				pstContents->pstTsInfo[ulIndex].usNetIdx = usNetIdx;
				DB_SVC_AddTsInfo(&pstContents->pstTsInfo[ulIndex], &usTsIdx);
				pstContents->pstTsInfo[ulIndex].usTsIdx = usTsIdx;
				ulTsAdded++;
				HxLOG_Debug("======= TS INFO ADDED =======\n");
				HxLOG_Debug("usTsIdx : %d, OID : 0x%04x, TID : 0x%04x\n",
					pstContents->pstTsInfo[ulIndex].usTsIdx,
					pstContents->pstTsInfo[ulIndex].usOnId,
					pstContents->pstTsInfo[ulIndex].usTsId);
				HxLOG_Debug("===============================\n");
			}

			if (pstTsInfoInDb != NULL)
			{
				DB_SVC_FreeTsInfoList(pstTsInfoInDb);
				pstTsInfoInDb = NULL;
			}
/*********************************************************************/

			HxLOG_Print("(%d) [TS:%d] onid/tsid/freq = %d/%d/%d\n", ulIndex, pstContents->pstTsInfo[ulIndex].usTsIdx
				, pstContents->pstTsInfo[ulIndex].usOnId, pstContents->pstTsInfo[ulIndex].usTsId, pstContents->pstTsInfo[ulIndex].stTuningParam.cab.ulFreq);


			ulCountOld = ulCountNew = 0;
			for (ulNewIdx = 0; ulNewIdx < pstContents->ulSvcNum; ulNewIdx++)
			{
				// 해당 TS에 해당하는 Service 개수를 가져온다.
				if((pstContents->pstTsInfo[ulIndex].usOnId == DbSvc_GetOnId(&(pstContents->pstSvcArray[ulNewIdx])))
			 		&& (pstContents->pstTsInfo[ulIndex].usTsId == DbSvc_GetTsId(&(pstContents->pstSvcArray[ulNewIdx]))))
				{
			 		ulCountNew++;
				}
			}
			HxLOG_Print("ulCountNew = %d\n", ulCountNew);

			// TP List에 해당 TP가 있으나, Service가 없는 것으로 보고, 삭제등의 처리를 하지 않는다.
			if (ulCountNew == 0)
			{
				HxLOG_Error("Cannot find new service list in TP (OID : 0x%04x, TID : 0x%04x)\n",
					pstContents->pstTsInfo[ulIndex].usOnId,
					pstContents->pstTsInfo[ulIndex].usTsId);
				continue;
			}

			if (pstSvcInfoListNew != NULL)
			{
				HLIB_STD_MemFree(pstSvcInfoListNew);
				pstSvcInfoListNew = NULL;
			}
			pstSvcInfoListNew = (DbSvc_Info_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_Info_t) * ulCountNew);
			if (pstSvcInfoListNew == NULL)
			{
				hErr = ERR_BUS_OUT_OF_MEMORY;
				goto ERROR_FAULT_OCCURRED;
			}

			ulCountNew = 0;
			for (ulNewIdx = 0; ulNewIdx < pstContents->ulSvcNum; ulNewIdx++)
			{
				// 해당 TS에 해당하는 Service를 따로 저장한다.
				if((pstContents->pstTsInfo[ulIndex].usOnId == DbSvc_GetOnId(&(pstContents->pstSvcArray[ulNewIdx])))
		 		&& (pstContents->pstTsInfo[ulIndex].usTsId == DbSvc_GetTsId(&(pstContents->pstSvcArray[ulNewIdx]))))
				{
					DbSvc_SetTsIdx(&(pstContents->pstSvcArray[ulNewIdx]), pstContents->pstTsInfo[ulIndex].usTsIdx);
					if ((pstContents->pstPrvArray != NULL) && (0 < pstContents->ulPrvNum))
					{
						DbSvc_SetPrvIdx(&(pstContents->pstSvcArray[ulNewIdx]), usPrvIdx);
					}
					else
					{
						DbSvc_SetPrvIdx(&(pstContents->pstSvcArray[ulNewIdx]), 0);
					}

					HxSTD_MemCopy(&pstSvcInfoListNew[ulCountNew], &pstContents->pstSvcArray[ulNewIdx], sizeof(DbSvc_Info_t));
					ulCountNew++;
				}
			}

			DB_SVC_InitSvcFindCondition(&stSvcCond);
			stSvcCond.nOrgDeliType = pstContents->stBaseContents.stBgTuneInfo.eDeliType;
			stSvcCond.nOnId = pstContents->pstTsInfo[ulIndex].usOnId;
			stSvcCond.nTsId = pstContents->pstTsInfo[ulIndex].usTsId;

			if (phListOld != NULL)
			{
				DB_SVC_FreeServiceInfoList(NULL, phListOld, NULL);
				phListOld = NULL;
			}

			if (pstSvcInfoListOld != NULL)
			{
				DB_SVC_FreeServiceInfoList(NULL, NULL, pstSvcInfoListOld);
				pstSvcInfoListOld = NULL;
			}

			hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulCountOld, &pstSvcInfoListOld);
			hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulCountOld, &phListOld);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("cannot find old service list(0x%x, 0x%x)\n",
					pstContents->pstTsInfo[ulIndex].usOnId,
					pstContents->pstTsInfo[ulIndex].usTsId);
			}

			HxLOG_Debug("======= [CHECK DYNAMIC UPDATE] =======\n");
			HxLOG_Debug("OID : 0x%04x, TID : 0x%04x, OldSvcCnt : %d, NewSvcCnt : %d\n",
				pstContents->pstTsInfo[ulIndex].usOnId,
				pstContents->pstTsInfo[ulIndex].usTsId,
				ulCountOld, ulCountNew);

			// 1. 기존 서비스 Flag 설정
			if (ulCountOld > 0)
			{
				if (peSvcFlagArrayOld != NULL)
				{
					HLIB_STD_MemFree(peSvcFlagArrayOld);
					peSvcFlagArrayOld = NULL;
				}
				peSvcFlagArrayOld = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulCountOld);
				if (peSvcFlagArrayOld == NULL)
				{
					hErr = ERR_BUS_OUT_OF_MEMORY;
					goto ERROR_FAULT_OCCURRED;
				}

				// 왜 초기값을 이렇게 설정하지? 잘 모르겠다.
				for (ulOldIdx = 0; ulOldIdx < ulCountOld; ulOldIdx++)
				{
					if (DbSvc_GetDeliType(pstSvcInfoListOld[ulOldIdx]) == pstContents->stBaseContents.stBgTuneInfo.eDeliType)
					{
						peSvcFlagArrayOld[ulOldIdx] = eDYNAMICUPDATE_DELETE;
					}
					else
					{
						peSvcFlagArrayOld[ulOldIdx] = eDYNAMICUPDATE_EXCLUDE;
					}
				}
			}

			// 2. 신규 서비스 Flag 설정
			if (ulCountNew > 0)
			{
				if (peSvcFlagArrayNew != NULL)
				{
					HLIB_STD_MemFree(peSvcFlagArrayNew);
					peSvcFlagArrayNew = NULL;
				}
				peSvcFlagArrayNew = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulCountNew);
				if (peSvcFlagArrayNew == NULL)
				{
					hErr = ERR_BUS_OUT_OF_MEMORY;
					goto ERROR_FAULT_OCCURRED;
				}

				for (ulNewIdx = 0; ulNewIdx < ulCountNew; ulNewIdx++)
				{
					peSvcFlagArrayNew[ulNewIdx] = eDYNAMICUPDATE_ADD;
				}
			}

			// LCN 변경 여부 체크
			if (peSvcFlagArrayForLcn != NULL)
			{
				HLIB_STD_MemFree(peSvcFlagArrayForLcn);
				peSvcFlagArrayForLcn = NULL;
			}

			peSvcFlagArrayForLcn = (SRCHSVC_AppendFlag_t *)HLIB_STD_MemCalloc (sizeof(SRCHSVC_AppendFlag_t) * ulCountNew);
			if (peSvcFlagArrayForLcn == NULL)
			{
				hErr = ERR_BUS_OUT_OF_MEMORY;
				goto ERROR_FAULT_OCCURRED;
			}

			hErr = SVC_SI_SetLcnServiceNumber(pstContents->stBaseContents.hBGAction, pstContents->pbNewTs[ulIndex], ulCountNew, pstSvcInfoListNew, peSvcFlagArrayForLcn);
			if (hErr != ERR_OK)
			{
				hErr = ERR_BUS_NO_OBJECT;
				goto ERROR_FAULT_OCCURRED;
			}

			// 3. 동일 서비스인지 확인
			for (ulOldIdx = 0; ulOldIdx < ulCountOld; ulOldIdx++)
			{
				if (peSvcFlagArrayOld[ulOldIdx] == eDYNAMICUPDATE_DELETE)
				{
					for (ulNewIdx = 0; ulNewIdx < ulCountNew; ulNewIdx++)
					{
						// Triplet과 svcType을 비교해서 같으면 동일 서비스로 간주
						if (TRUE == xmgr_dynamicupdate_isIdenticalService(&pstSvcInfoListOld[ulOldIdx], &pstSvcInfoListNew[ulNewIdx]))
						{
							if (TRUE == xmgr_dynamicupdate_isModifiedService(&pstSvcInfoListOld[ulOldIdx], &pstSvcInfoListNew[ulNewIdx], peSvcFlagArrayForLcn[ulNewIdx]))
							{
								peSvcFlagArrayOld[ulOldIdx] = eDYNAMICUPDATE_UPDATE + ulNewIdx;
								peSvcFlagArrayNew[ulNewIdx] = eDYNAMICUPDATE_UPDATE + ulOldIdx;
							}
							else
							{
								peSvcFlagArrayOld[ulOldIdx] = eDYNAMICUPDATE_IDENTICAL;
								peSvcFlagArrayNew[ulNewIdx] = eDYNAMICUPDATE_IDENTICAL;
#if defined(CONFIG_DEBUG)
								HxLOG_Debug("[==]0x%04x, 0x%04x, 0x%04x, %d, %d, %d, %s -> 0x%04x, 0x%04x, 0x%04x, %d, %d, %d, %s\n",
									DbSvc_GetOnId(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetTsId(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetSvcId(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetTsIdx(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetPrvIdx(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetLcn(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetName(&(pstSvcInfoListOld[ulOldIdx])),
									DbSvc_GetOnId(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetTsId(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetSvcId(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetTsIdx(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetPrvIdx(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetLcn(&(pstSvcInfoListNew[ulNewIdx])),
									DbSvc_GetName(&(pstSvcInfoListNew[ulNewIdx]))
									);
#endif
							}
						}
					}
				}
			}

			// 6. ADD New Service
			HxLOG_Debug("======= NEW ======= : %d\n", ulCountNew);
			for (ulNewIdx = 0; ulNewIdx < ulCountNew; ulNewIdx++)
			{
#if defined(CONFIG_DEBUG)
				if (peSvcFlagArrayNew[ulNewIdx] != eDYNAMICUPDATE_IDENTICAL)
				{
					HxLOG_Debug("\t[%d/%d]%s | %s | OPTYPE : %d, 0x%04x, 0x%04x, 0x%04x, %d, %d, %d, %s\n",
						(ulNewIdx + 1), ulCountNew,
						INT_UTIL_GETENUMSTR_DU_UpdateFlag(peSvcFlagArrayNew[ulNewIdx]),
						INT_UTIL_GETENUMSTR_DU_AppendFlag(peSvcFlagArrayForLcn[ulNewIdx]),
						DbSvc_GetOpType(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetOnId(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetTsId(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetSvcId(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetTsIdx(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetPrvIdx(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetLcn(&(pstSvcInfoListNew[ulNewIdx])),
						DbSvc_GetName(&(pstSvcInfoListNew[ulNewIdx]))
					);
				}
#endif

				if (peSvcFlagArrayNew[ulNewIdx] == eDYNAMICUPDATE_ADD)
				{
					DB_SVC_AddServiceInfo(&pstSvcInfoListNew[ulNewIdx], NULL);
					ulSvcAdded++;
				}
			}


			// 7. Update Old Service or Delete Old Service
			HxLOG_Debug("======= OLD ======= : %d\n", ulCountOld);
			for (ulOldIdx = 0; ulOldIdx < ulCountOld; ulOldIdx++)
			{
#if defined(CONFIG_DEBUG)
				if (peSvcFlagArrayOld[ulOldIdx] != eDYNAMICUPDATE_IDENTICAL)
				{
					HxLOG_Debug("\t[%d/%d]%s || OPTYPE : %d, 0x%04x, 0x%04x, 0x%04x, %d, %d, %d, %s\n",
						(ulOldIdx + 1), ulCountOld,
						INT_UTIL_GETENUMSTR_DU_UpdateFlag(peSvcFlagArrayOld[ulOldIdx]),
						DbSvc_GetOpType(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetOnId(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetTsId(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetSvcId(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetTsIdx(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetPrvIdx(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetLcn(&(pstSvcInfoListOld[ulOldIdx])),
						DbSvc_GetName(&(pstSvcInfoListOld[ulOldIdx]))
						);
				}
#endif

				if (peSvcFlagArrayOld[ulOldIdx] >= eDYNAMICUPDATE_UPDATE)
				{
					DbSvc_Info_t	stSvcInfo;
					hSvc = phListOld[ulOldIdx];
					ulNewIdx = peSvcFlagArrayOld[ulOldIdx] - eDYNAMICUPDATE_UPDATE;

					HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
					HxSTD_MemCopy(&stSvcInfo, &pstSvcInfoListOld[ulOldIdx], sizeof(DbSvc_Info_t));

					DbSvc_SetLcn(&stSvcInfo, DbSvc_GetLcn(&pstSvcInfoListNew[ulNewIdx]));
					DbSvc_SetName(&stSvcInfo, DbSvc_GetName(&pstSvcInfoListNew[ulNewIdx]));
					DbSvc_SetVisibleFlag(&stSvcInfo, DbSvc_GetVisibleFlag(&pstSvcInfoListNew[ulNewIdx]));

					DbSvc_SetSimulcastOnId(&stSvcInfo, DbSvc_GetSimulcastOnId(&pstSvcInfoListNew[ulNewIdx]));
					DbSvc_SetSimulcastTsId(&stSvcInfo, DbSvc_GetSimulcastTsId(&pstSvcInfoListNew[ulNewIdx]));
					DbSvc_SetSimulcastSvcId(&stSvcInfo, DbSvc_GetSimulcastSvcId(&pstSvcInfoListNew[ulNewIdx]));

					xmgr_dynamicupdate_updateServiceInfo(hSvc, &(pstSvcInfoListOld[ulOldIdx]), &stSvcInfo);
					ulSvcUpdated++;
				}
				else if (peSvcFlagArrayOld[ulOldIdx] == eDYNAMICUPDATE_DELETE)
				{
					hSvc = phListOld[ulOldIdx];
					DB_SVC_DeleteServiceInfo(hSvc);
					ulSvcDeleted++;
				}
				else if (peSvcFlagArrayOld[ulOldIdx] == eDYNAMICUPDATE_IDENTICAL)
				{
					ulIdentical++;
				}
			}
		}

		// ^O^, scipio, 2014-04-17, For TP Delete
		HxLOG_Debug("======= CHECK TP LIST =======\n");
		HxLOG_Debug("\tOld TP List : %d\n", ulTsNumByOnIDInDb);
		for (ulIndex = 0; ulIndex < ulTsNumByOnIDInDb; ulIndex++)
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("\t[%d/%d]%s %d, 0x%04x, 0x%04x, 0x%04x, 0x%04x, %d, %d\n",
				(ulIndex + 1), ulTsNumByOnIDInDb,
				INT_UTIL_GETENUMSTR_DU_UpdateFlag(peTsFlagArrayOld[ulIndex]),
				pstTsInfoByOnIDInDb[ulIndex].usTsIdx,
				pstTsInfoByOnIDInDb[ulIndex].usUniqueId,
				pstTsInfoByOnIDInDb[ulIndex].usNetIdx,
				pstTsInfoByOnIDInDb[ulIndex].usTsId,
				pstTsInfoByOnIDInDb[ulIndex].usOnId,
				pstTsInfoByOnIDInDb[ulIndex].ucTunerId,
				pstTsInfoByOnIDInDb[ulIndex].eDeliType
			);
#endif

			if (peTsFlagArrayOld[ulIndex] == eDYNAMICUPDATE_DELETE)
			{
				HINT32			nCnt = 0;
#if 0
				DB_SVC_InitSvcFindCondition(&stSvcCond);
				stSvcCond.nTsIdx	= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usTsIdx;
				stSvcCond.nTsId		= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usTsId;
				stSvcCond.nOnId		= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usOnId;
				stSvcCond.nOrgDeliType	= (HINT32)pstTsInfoByOnIDInDb[ulIndex].eDeliType;
				// After deleting services in a TP, The TP will be automatically deleted.
				DB_SVC_DeleteServiceList(&stSvcCond);
				DB_SVC_DeleteTsInfo(pstTsInfoByOnIDInDb[ulIndex].usTsIdx);
#else
				Handle_t		*phList = NULL;
				DbSvc_Info_t	*pstSvcInfoList = NULL;
				DB_SVC_InitSvcFindCondition(&stSvcCond);
				stSvcCond.nTsIdx	= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usTsIdx;
				stSvcCond.nTsId		= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usTsId;
				stSvcCond.nOnId		= (HINT32)pstTsInfoByOnIDInDb[ulIndex].usOnId;
				stSvcCond.nOrgDeliType	= (HINT32)pstTsInfoByOnIDInDb[ulIndex].eDeliType;

				hErr = DB_SVC_FindSvcInfoList(&stSvcCond, &nCnt, &pstSvcInfoList);
				hErr |= DB_SVC_FindSvcHandleList(&stSvcCond, &nCnt, &phList);
				if (hErr != ERR_OK || nCnt <= 0)
				{
					HxLOG_Error("[%s:%d]Can't find any service.\n", __FUNCTION__, __LINE__);
				}
				else
				{
					for (ulOldIdx = 0; ulOldIdx < nCnt; ulOldIdx++)
					{
						HxLOG_Debug("\t\t[%d/%d]OPTYPE : %d, 0x%04x, 0x%04x, 0x%04x, %d, %d, %d, %s\n",
							(ulOldIdx + 1), nCnt,
							DbSvc_GetOpType(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetOnId(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetTsId(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetSvcId(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetTsIdx(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetPrvIdx(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetLcn(&(pstSvcInfoList[ulOldIdx])),
							DbSvc_GetName(&(pstSvcInfoList[ulOldIdx]))
							);
						hSvc = phList[ulOldIdx];
						DB_SVC_DeleteServiceInfo(hSvc);
						ulSvcDeleted++;
					}
				}
				DB_SVC_DeleteTsInfo(pstTsInfoByOnIDInDb[ulIndex].usTsIdx);
				DB_SVC_FreeServiceInfoList (NULL, phList, pstSvcInfoList);
#endif
				ulTsDeleted++;
			}
		}

		HxLOG_Debug("\tNew TP List : %d\n", pstContents->ulTsNum);
#if defined(CONFIG_DEBUG)
		for (ulIndex = 0; ulIndex < pstContents->ulTsNum; ulIndex++)
		{
			HxLOG_Debug("\t[%d/%d]%d, 0x%04x, 0x%04x, 0x%04x, 0x%04x, %d, %d\n",
				(ulIndex + 1), pstContents->ulTsNum,
				pstContents->pstTsInfo[ulIndex].usTsIdx,
				pstContents->pstTsInfo[ulIndex].usUniqueId,
				pstContents->pstTsInfo[ulIndex].usNetIdx,
				pstContents->pstTsInfo[ulIndex].usTsId,
				pstContents->pstTsInfo[ulIndex].usOnId,
				pstContents->pstTsInfo[ulIndex].ucTunerId,
				pstContents->pstTsInfo[ulIndex].eDeliType
			);
		}
#endif
		// ;-)
	}
	ulAdded = ulPrvAdded + ulNetAdded + ulTsAdded + ulSvcAdded;
	ulUpdated = ulTsUpdated + ulSvcUpdated;
	ulDeleted = ulTsDeleted + ulSvcDeleted;

	// Nordig LCN & Simulcast
	{
		DefaultLcnPolicy_t			eLcnPolicy;
		PreProgrammedChNumPolicy_t	eChNumPolicy;

		SVC_SI_PostProcessForSimulcastService(pstContents->stBaseContents.hBGAction);

		/* LCN 스펙 중, non-LCN에 대한 처리가 없으면, non-LCN 서비스는 LCN 마지막에 덧붙여 할당한다 (Humax Rule) */
		MGR_SEARCH_GetLcnPolicy (&eLcnPolicy, &eChNumPolicy);
		SVC_SI_PostProcessForSvcNumbering (pstContents->stBaseContents.hBGAction, eLcnPolicy, eDxDELIVERY_TYPE_CAB);
	}

	hErr = ERR_OK;

ERROR_FAULT_OCCURRED:
	if (phListOld != NULL)
	{
		DB_SVC_FreeServiceInfoList(NULL, phListOld, NULL);
		phListOld = NULL;
	}

	if (pstSvcInfoListOld != NULL)
	{
		DB_SVC_FreeServiceInfoList(NULL, NULL, pstSvcInfoListOld);
		pstSvcInfoListOld = NULL;
	}

	if (pstTsInfoInDb != NULL)
	{
		DB_SVC_FreeTsInfoList(pstTsInfoInDb);
		pstTsInfoInDb = NULL;
	}

	// ^O^, scipio, 2014-04-17, For TP Delete
	if (pstTsInfoByOnIDInDb != NULL)
	{
		DB_SVC_FreeTsInfoList(pstTsInfoByOnIDInDb);
		pstTsInfoByOnIDInDb = NULL;
	}
	// ;-)

	if (pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	if (pstContents->pbNewTs != NULL)
	{
		HLIB_STD_MemFree (pstContents->pbNewTs);
		pstContents->pbNewTs = NULL;
	}

	if (pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}

	if (pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}

	if (pstContents->pstPrvArray != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstPrvArray);
		pstContents->pstPrvArray = NULL;
	}

	if (pstSvcInfoListNew != NULL)
	{
		HLIB_STD_MemFree (pstSvcInfoListNew);
		pstSvcInfoListNew = NULL;
	}

	if (peSvcFlagArrayNew != NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayNew);
		peSvcFlagArrayNew = NULL;
	}

	if (peSvcFlagArrayForLcn!= NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayForLcn);
		peSvcFlagArrayForLcn = NULL;
	}

	if (peSvcFlagArrayOld != NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayOld);
		peSvcFlagArrayOld = NULL;
	}

	// ^O^, scipio, 2014-04-17, For TP Delete
	if (peTsFlagArrayOld != NULL)
	{
		HLIB_STD_MemFree (peTsFlagArrayOld);
		peTsFlagArrayOld = NULL;
	}
	// ;-)

	DB_SVC_RemoveSearchFlag();

	if (ulAdded > 0 || ulUpdated > 0 || ulDeleted > 0)
	{
		if (NULL != pbUpdated)
			*pbUpdated = TRUE;
		HxLOG_Error("================== [ DYNAMIC UPDATE RESULT] ======================\n");
		HxLOG_Error("\tProvider> Added : %d\n", ulPrvAdded);
		HxLOG_Error("\tNetwork> Added : %d\n", ulNetAdded);
		HxLOG_Error("\tTrasnport Stream> Added : %d, Updated : %d, Deleted : %d\n", ulTsAdded, ulTsUpdated, ulTsDeleted);
		HxLOG_Error("\tService> Added : %d, Updated : %d, Deleted : %d\n", ulSvcAdded, ulSvcUpdated, ulSvcDeleted);
		HxLOG_Error("================== [ DYNAMIC UPDATE RESULT] ======================\n\n");
	}
	else
	{
		HxLOG_Error("================== [ DYNAMIC UPDATE RESULT] ======================\n");
		HxLOG_Error("\t no changes...\n");
		HxLOG_Error("================== [ DYNAMIC UPDATE RESULT] ======================\n\n");
	}

	HxLOG_Debug("hErr: %d \n", hErr);

	return hErr;
}

STATIC HERROR xmgr_dynamicupdate_retrieveResults_Nordig (DynamicUpdate_Content_Nordig_t* pstContents)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulInfoNum = 0;
	HUINT32					i;
	SvcSi_SrchInfoType_e	eTsInfoType = eSI_SRCH_INFOTYPE_Delivery;

	HxLOG_Trace();

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Delivery, &(pstContents->ulTsNum));
	if (hErr != ERR_OK)
	{
		/*
			NIT Act based는 delivery로 ts info 구성하고 있고,
			NIT Oth based는 transponder로 ts info 구성하고 있는데, 하나로 통일할 필요가 있어 보이고,
			현재는 NIT Oth based에서 dynamic update가 동작하지 않을 것이므로, 아래와 같이 수정
		*/
		hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Transponder, &(pstContents->ulTsNum));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("\n\n");
			hErr = ERR_BUS_NO_OBJECT;
			goto ERROR_FAULT_OCCURRED;
		}
		eTsInfoType = eSI_SRCH_INFOTYPE_Transponder;
	}

	// Network에 속해 있는 모든 TS 갯수
	HxLOG_Debug("pstContents->ulTsNum: %d \n", pstContents->ulTsNum);

	if (pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}

	pstContents->pstTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_TsInfo_t) * pstContents->ulTsNum);
	if (pstContents->pstTsInfo == NULL)
	{
		HxLOG_Error("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pbNewTs != NULL)
	{
		HLIB_STD_MemFree(pstContents->pbNewTs);
		pstContents->pbNewTs = NULL;
	}

	pstContents->pbNewTs = (HBOOL *)HLIB_STD_MemCalloc(sizeof(HBOOL) * (pstContents->ulTsNum));
	if (pstContents->pbNewTs == NULL)
	{
		HxLOG_Error("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eTsInfoType, pstContents->ulTsNum, &ulInfoNum, (void *)pstContents->pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// 4. SI에서 network list를 가져온다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Network, &pstContents->ulNetNum);
	if (hErr != ERR_OK || pstContents->ulNetNum == 0)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	pstContents->pstNetInfo = (DbSvc_NetInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_NetInfo_t) * pstContents->ulNetNum);
	if (pstContents->pstNetInfo == NULL)
	{
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Network, pstContents->ulNetNum, &ulInfoNum, (void *)pstContents->pstNetInfo);
	if (hErr != ERR_OK)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// 5. SI에서 Provider list를 가져온다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Provider, &(pstContents->ulPrvNum));
	if (hErr != ERR_OK || pstContents->ulPrvNum == 0)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pstPrvArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstPrvArray);
		pstContents->pstPrvArray = NULL;
	}

	pstContents->pstPrvArray = (DbSvc_PrvInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_PrvInfo_t) * (pstContents->ulPrvNum));
	if (pstContents->pstPrvArray != NULL)
	{
		hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Provider, pstContents->ulPrvNum, &ulInfoNum, (void *)pstContents->pstPrvArray);
		if (hErr != ERR_OK)
		{
			pstContents->ulPrvNum = 0;
			HLIB_STD_MemFree (pstContents->pstPrvArray);
			pstContents->pstPrvArray = NULL;
		}
	}

	// 6. SI에서 Service list를 가져온다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Service, &(pstContents->ulSvcNum));

	if (hErr != ERR_OK || pstContents->ulSvcNum == 0)
	{
		HxLOG_Error("cannot get dynamic update result,  eSI_SRCH_INFOTYPE_Service\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}
	pstContents->pstSvcArray = (DbSvc_Info_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_Info_t) * (pstContents->ulSvcNum));
	if (pstContents->pstSvcArray  == NULL)
	{
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Service, pstContents->ulSvcNum, &ulInfoNum, (void *)pstContents->pstSvcArray);
	HxLOG_Print("pstContents->ulSvcNum : %d\n", pstContents->ulSvcNum);
	if (pstContents->pstSvcArray == NULL)
	{
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	for (i = 0; i < pstContents->ulSvcNum; i++)
	{
		DbSvc_SetDeliType(pstContents->pstSvcArray[i], eDxDELIVERY_TYPE_CAB);
	}

	hErr = ERR_BUS_NO_ERROR;

ERROR_FAULT_OCCURRED:

	HxLOG_Debug("hErr: %d \n", hErr);

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_DONE:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateDone_Nordig (DynamicUpdate_Content_Nordig_t *pstContents, HBOOL *pbUpdated)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = xmgr_dynamicupdate_retrieveResults_Nordig (pstContents);

	if (hErr == ERR_OK)
	{
		hErr = xmgr_dynamicupdate_saveResults_Nordig (pstContents, pbUpdated);
	}

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_FAIL:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateFail_Nordig (DynamicUpdate_Content_Nordig_t *pstContents)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = ERR_OK;

	return hErr;
}

STATIC BUS_Result_t	proc_dynamicupdate_Nordig(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DynamicUpdate_Content_Nordig_t		*pstContents = xmgr_DynamicUpdate_GetContent_Nordig();
	HERROR								hErr = ERR_FAIL;
	HBOOL								bUpdated = FALSE;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			(void)xmgr_DynamicUpdate_InitContext_Nordig(pstContents);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate = %d\n", pstContents->stBaseContents.bStartedDyUpdate);
			xmgr_dynamicupdate_releaseBackGroundAction(&(pstContents->stBaseContents));
			(void)xmgr_DynamicUpdate_InitContext_Nordig(pstContents);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_STOPPED !!\n");
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Nordig(pstContents);
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED !!\n");
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Nordig(pstContents);
			}

			hErr = xmgr_dynamicupdate_msgCheckPreCondition_Nordig (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				hErr = xmgr_dynamicupdate_mevtLiveNotifySvcChanged(pstContents, hAction, p1, p2, p3);
			}
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			HxLOG_Print("eSEVT_CH_LOCKED !! (ActionId = %d, BgActionId = %d)\n", SVC_PIPE_GetActionId(hAction), pstContents->stBaseContents.ulBGActionId);
			hErr = xmgr_dynamicupdate_evtChLocked (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			break;

		// si dynamic update event.
		case eSEVT_SI_DYNAMIC_UPDATE_DONE:
			HxLOG_Print("eSEVT_SI_DYNAMIC_UPDATE_DONE\n");
			hErr = xmgr_dynamicupdate_checkActionId (&(pstContents->stBaseContents), hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}
			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateDone_Nordig (pstContents, &bUpdated);
			HxLOG_Debug("hErr: %d, bUpdated: %d \n", hErr, bUpdated);
			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateDone(&(pstContents->stBaseContents), bUpdated);
			return MESSAGE_BREAK;

		case eSEVT_SI_DYNAMIC_UPDATE_FAIL:
			HxLOG_Print("eSEVT_SI_DYNAMIC_UPDATE_FAIL\n");
			hErr = xmgr_dynamicupdate_checkActionId (&(pstContents->stBaseContents), hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}

			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateFail_Nordig (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_BREAK;
			}
			break;

		case eSEVT_DB_PARAM_UPDATED:
			if(p1 == eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING)
			{
				HxLOG_Print("eSEVT_DB_PARAM_UPDATED (eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING)\n");

				if(pstContents->stBaseContents.bStartedDyUpdate == FALSE)
				{
					hErr = xmgr_dynamicupdate_setupBackGroundAction(pstContents);
					HxLOG_Debug("xmgr_dynamicupdate_setupBackGroundAction() retrun hErr: %d \n", hErr);
				}
				else
				{
					HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate is already TRUE \n");
				}
			}
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(lMessage, hAction, p1, p2, p3);
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	xmgr_dynamicupdate_Start_Nordig(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	HxLOG_Trace();

	eRes = BUS_MGR_Create((char*)"proc_dynamicupdate_Nordig", APP_ACTION_PRIORITY, proc_dynamicupdate_Nordig, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_dynamicupdate_Stop_Nordig(void)
{

	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	HxLOG_Trace();

	eRes = BUS_MGR_DestroyWithReason(proc_dynamicupdate_Nordig, eDESTROY_BUS_MYSELF, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_dynamicupdate_SvcStop_Nordig(Handle_t hAction)
{
	DynamicUpdate_Content_Nordig_t		*pstContents = xmgr_DynamicUpdate_GetContent_Nordig();
	HERROR								hErr = ERR_FAIL;

	hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, 0, 0, 0);
	if(hErr == ERR_OK)
	{
		(void)xmgr_DynamicUpdate_InitContext_Nordig(pstContents);
	}

	return ERR_OK;
}

// for batch
BUS_Callback_t xmgr_dynamicupdate_GetProc_Nordig(void)
{
	return proc_dynamicupdate_Nordig;
}

/*********************** End of File ******************************/


