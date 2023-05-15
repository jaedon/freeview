/**
	@file	mgr_antenna_slot.c
	@brief	\n

	Description: 					\n
	Module:						\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>

#include <mgr_common.h>
#include <mgr_antenna.h>

#if defined(CONFIG_MW_CH_SATELLITE)
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	LOG					HxLOG_Print
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message

#define MAX_ANT_SLOT						16

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eAntItemState_NotExist	= 0,

	eAntItemState_NoChange,
	eAntItemState_Added,
	eAntItemState_Deleted,
	eAntItemState_Modified,
} MgrAnt_ItemState_e;

typedef struct
{
	HUINT32				 ulAntIdx;
	MgrAnt_ItemState_e	 eState;
	DbSvc_AntInfo_t			 stOrgAntInfo;
	DbSvc_AntInfo_t			 stNewAntInfo;
} MgrAnt_Slot_t;


typedef struct
{
	HUINT32				 ulAntNum;
	MgrAnt_Slot_t		 astAntArray[MAX_ANT_SLOT];
} MgrAnt_Session_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


STATIC HERROR mgr_antslot_loadAntList (MgrAnt_Session_t *pstSession)
{
	HINT32			 nIndex, nAntCnt = 0, nAntNum = 0;
	HINT32			*pnAntIdxList = NULL;
	HERROR			 hErr;

	HxSTD_memset (pstSession, 0, sizeof(MgrAnt_Session_t));

	hErr = DB_SVC_FindAllAntIdxList (&nAntNum, &pnAntIdxList);
	if ((ERR_OK != hErr) || (nAntNum <= 0) || (NULL == pnAntIdxList))
	{
		nAntNum = 0;
	}

HxLOG_Print ("[%s:%d] Antenna List Start\n", __FUNCTION__, __LINE__);

	for (nIndex = 0; nIndex < nAntNum; nIndex++)
	{
		DbSvc_AntInfo_t		 stAntInfo;

		hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxList[nIndex], &stAntInfo);
		if ((ERR_OK == hErr) && (nAntCnt < MAX_ANT_SLOT))
		{
			HxLOG_Print ("   - AntName[%s], SlotId(%d), AntIdx(%d)\n", stAntInfo.szName, nAntCnt, (HUINT32)pnAntIdxList[nIndex]);
			pstSession->astAntArray[nAntCnt].eState			=  eAntItemState_NoChange;
			pstSession->astAntArray[nAntCnt].ulAntIdx		= (HUINT32)pnAntIdxList[nIndex];
			pstSession->astAntArray[nAntCnt].stOrgAntInfo	= stAntInfo;
			pstSession->astAntArray[nAntCnt].stNewAntInfo	= stAntInfo;

			nAntCnt ++;
		}
	}

	HxLOG_Print ("[%s:%d] Antenna List End\n", __FUNCTION__, __LINE__);

	pstSession->ulAntNum = (HUINT32)nAntCnt;

	if (NULL != pnAntIdxList)
	{
		DB_SVC_FreeAntList (pnAntIdxList);
	}

	return ERR_OK;
}
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

Handle_t MGR_ANTENNASLOT_Create (void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	MgrAnt_Session_t	*pstSession = NULL;

	pstSession = (MgrAnt_Session_t *)OxAP_Malloc (sizeof(MgrAnt_Session_t));
	if (NULL == pstSession)
	{
		return (Handle_t)NULL;
	}

	mgr_antslot_loadAntList (pstSession);

	return (Handle_t)pstSession;
#else
	return HANDLE_NULL;
#endif
}

HERROR MGR_ANTENNASLOT_Destroy (Handle_t hAntSession)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))
	{
		return ERR_FAIL;
	}

	OxAP_Free (pstSession);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_CountAntenna (Handle_t hAntSession, HUINT32 *pulAntNum)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulIndex, ulCount = 0;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if (NULL == pulAntNum)
	{
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < pstSession->ulAntNum; ulIndex++)
	{
		if (eAntItemState_Deleted != pstSession->astAntArray[ulIndex].eState)
		{
			ulCount ++;
		}
	}

	*pulAntNum = ulCount;
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_GetAllAntenna (Handle_t hAntSession, HUINT32 *pulAntNum, DbSvc_AntInfo_t **ppstAntArray)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulIndex, ulCount = 0;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	DbSvc_AntInfo_t		*pstAntArray;
	HERROR			 hErr;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if ((NULL == pulAntNum) || (NULL == ppstAntArray))
	{
		return ERR_FAIL;
	}

	*pulAntNum = 0;
	*ppstAntArray = NULL;

	hErr = MGR_ANTENNASLOT_CountAntenna (hAntSession, &ulCount);
	if ((ERR_OK != hErr) || (0 == ulCount))
	{
		return ERR_FAIL;
	}

	pstAntArray = (DbSvc_AntInfo_t *)OxAP_Malloc (sizeof(DbSvc_AntInfo_t) * ulCount);
	if (NULL == pstAntArray)
	{
		return ERR_FAIL;
	}

	for (ulIndex = 0, ulCount = 0; ulIndex < pstSession->ulAntNum; ulIndex++)
	{
		pstAntSlot = &(pstSession->astAntArray[ulIndex]);
		if (eAntItemState_Deleted == pstAntSlot->eState)		{ continue; }

		pstAntArray[ulCount] = pstAntSlot->stNewAntInfo;
		ulCount ++;
	}

	*pulAntNum = ulCount;
	*ppstAntArray = pstAntArray;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_FreeAntennaList (DbSvc_AntInfo_t *pstAntArray)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	if (NULL == pstAntArray)
	{
		return ERR_FAIL;
	}

	OxAP_Free (pstAntArray);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_FindSlotIdByAntName (Handle_t hAntSession, HCHAR *szAntName, HUINT32 *pulSlotId)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HERROR	hErr;
	HINT32	nCnt,*pnAntIdxList,idx;
	DbSvc_AntInfo_t	stAntInfo;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if ((NULL == szAntName) || (NULL == pulSlotId))
	{
		return ERR_FAIL;
	}

	*pulSlotId = 0xFFFFFFFF;

	nCnt = 0;
	pnAntIdxList = NULL;
	hErr = DB_SVC_FindAllAntIdxList(&nCnt, &pnAntIdxList);
	if ((ERR_OK != hErr) || (nCnt < 1) || (NULL == pnAntIdxList))
	{
		DB_SVC_FreeAntList(pnAntIdxList);
		return ERR_FAIL;
	}

	for (idx=0; idx < nCnt; idx++)
	{
		if (ERR_OK == DB_SVC_GetAntInfo(pnAntIdxList[idx], &stAntInfo))
		{
			if (0 == VK_strncmp (szAntName, stAntInfo.szName, MAX_ANT_NAME))
			{
				*pulSlotId = pnAntIdxList[idx];
				DB_SVC_FreeAntList(pnAntIdxList);
				return ERR_OK;
			}

		}
	}

	DB_SVC_FreeAntList(pnAntIdxList);

	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_GetAntennaBySlotId (Handle_t hAntSession, HUINT32 ulSlotId, DbSvc_AntInfo_t *pstAntInfo, HUINT32 *pulAntIdx)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession) || (NULL == pstAntInfo))
	{
		return ERR_FAIL;
	}
#if 1
	{
		if (ERR_OK != DB_SVC_GetAntInfo(ulSlotId, pstAntInfo))
			return ERR_FAIL;

		*pulAntIdx = ulSlotId;
	}
#else
	if (ulSlotId >= pstSession->ulAntNum)
	{
		return ERR_FAIL;
	}
	pstAntSlot = &(pstSession->astAntArray[ulSlotId]);
	if ((eAntItemState_Deleted == pstAntSlot->eState) ||
		(eAntItemState_NotExist == pstAntSlot->eState))
	{
		return ERR_FAIL;
	}

	*pstAntInfo = pstAntSlot->stNewAntInfo;
	if (NULL != pulAntIdx)
	{
		*pulAntIdx = pstAntSlot->ulAntIdx;
	}
#endif
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR MGR_ANTENNASLOT_GetAntennaByIndex (Handle_t hAntSession, HUINT32 ulAntIdx, DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	HUINT32 		 ulSlotId;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession) || (NULL == pstAntInfo))
	{
		return ERR_FAIL;
	}

	for (ulSlotId = 0; ulSlotId < pstSession->ulAntNum; ulSlotId++)
	{
		pstAntSlot = &(pstSession->astAntArray[ulSlotId]);
		if ((eAntItemState_Deleted == pstAntSlot->eState) ||
			(eAntItemState_NotExist == pstAntSlot->eState))
		{
			continue;
		}

		if (pstAntSlot->ulAntIdx == ulAntIdx)
		{
			*pstAntInfo = pstAntSlot->stNewAntInfo;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
#else
	return ERR_FAIL;
#endif
}


HERROR MGR_ANTENNASLOT_AddAntenna (Handle_t hAntSession, DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulSlotId;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	HERROR			 hErr;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if (NULL == pstAntInfo)
	{
		return ERR_FAIL;
	}

	hErr = MGR_ANTENNASLOT_FindSlotIdByAntName (hAntSession, (HCHAR*)pstAntInfo->szName, &ulSlotId);
	if (ERR_OK == hErr)
	{
		return MGR_ANTENNASLOT_ModifyAntenna (hAntSession, NULL, pstAntInfo);
	}

	if (pstSession->ulAntNum >= MAX_ANT_SLOT)
	{
		return ERR_FAIL;
	}

	pstAntSlot = &(pstSession->astAntArray[pstSession->ulAntNum]);

	pstAntSlot->eState = eAntItemState_Added;
	pstAntSlot->stNewAntInfo = *pstAntInfo;

	pstSession->ulAntNum ++;
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_ModifyAntenna (Handle_t hAntSession, HCHAR *szOrgAntName, DbSvc_AntInfo_t *pstAntInfo)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulSlotId;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	HERROR			 hErr;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if (NULL == pstAntInfo)
	{
		return ERR_FAIL;
	}

	if (NULL == szOrgAntName)
	{
		szOrgAntName = (HCHAR*)pstAntInfo->szName;
	}

	hErr = MGR_ANTENNASLOT_FindSlotIdByAntName (hAntSession, szOrgAntName, &ulSlotId);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	pstAntSlot = &(pstSession->astAntArray[ulSlotId]);
	pstAntSlot->stNewAntInfo = *pstAntInfo;

	if (eAntItemState_Added != pstAntSlot->eState)
	{
		pstAntSlot->eState = eAntItemState_Modified;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_DeleteAntenna (Handle_t hAntSession, HCHAR *szAntName)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulSlotId;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	HERROR			 hErr;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }
	if (NULL == szAntName)
	{
		return ERR_FAIL;
	}

	hErr = MGR_ANTENNASLOT_FindSlotIdByAntName (hAntSession, szAntName, &ulSlotId);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	pstAntSlot = &(pstSession->astAntArray[ulSlotId]);

	if (eAntItemState_Added == pstAntSlot->eState)
	{
		pstAntSlot->eState = eAntItemState_NotExist;
	}
	else if (eAntItemState_NotExist != pstAntSlot->eState)
	{
		pstAntSlot->eState = eAntItemState_Deleted;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_DeleteAllAntenna (Handle_t hAntSession)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32			 ulIndex;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }

	for (ulIndex = 0; ulIndex < pstSession->ulAntNum; ulIndex++)
	{
		pstAntSlot = &(pstSession->astAntArray[ulIndex]);

		if (eAntItemState_Added == pstAntSlot->eState)
		{
			pstAntSlot->eState = eAntItemState_NotExist;
		}
		else if (eAntItemState_NotExist != pstAntSlot->eState)
		{
			pstAntSlot->eState = eAntItemState_Deleted;
		}
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_ANTENNASLOT_Undo (Handle_t hAntSession)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }

	mgr_antslot_loadAntList (pstSession);

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

#if 0
HERROR MGR_ANTENNASLOT_Save (Handle_t hAntSession)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HBOOL			 bModified = FALSE;
	HUINT16			 usAntIdx;
	HUINT32			 ulIndex;
	MgrAnt_Session_t	*pstSession = (MgrAnt_Session_t *)hAntSession;
	MgrAnt_Slot_t		*pstAntSlot;
	HERROR			 hErr;

	if ((NULL == pstSession) || (HANDLE_NULL == hAntSession))				{ return ERR_FAIL; }

	for (ulIndex = 0; ulIndex < MAX_ANT_SLOT; ulIndex++)
	{
		pstAntSlot = &(pstSession->astAntArray[ulIndex]);

		switch (pstAntSlot->eState)
		{
		case eAntItemState_Added:
			hErr = DB_SVC_AddAntInfo (&(pstAntSlot->stNewAntInfo), &usAntIdx);
			if (ERR_OK == hErr)
			{
				bModified = TRUE;
			}

			break;

		case eAntItemState_Deleted:
			hErr = DB_SVC_DeleteAntInfo ((HUINT16)pstAntSlot->ulAntIdx);
			if (ERR_OK == hErr)
			{
				DbSvc_Condition_t		 stSvcCond;

				bModified = TRUE;

				DB_SVC_InitSvcFindCondition (&stSvcCond);
				stSvcCond.nAntIdx = (HINT32)pstAntSlot->ulAntIdx;

				DB_SVC_DeleteServiceList (&stSvcCond);
				DB_SVC_Sync(eSyncMode);
			}

			break;

		case eAntItemState_Modified:
			if (HxSTD_memcmp (&(pstAntSlot->stNewAntInfo), &(pstAntSlot->stOrgAntInfo), sizeof(DbSvc_AntInfo_t)) != 0)
			{
				hErr = DB_SVC_UpdateAntInfo ((HUINT16)pstAntSlot->ulAntIdx, &(pstAntSlot->stNewAntInfo));
				if (ERR_OK == hErr)
				{
					bModified = TRUE;
				}
			}

			break;

		default:
			break;
		}
	}

	if (TRUE == bModified)
	{
		DB_PARAM_Sync();
		DB_SVC_AntennaSync();
	}

	mgr_antslot_loadAntList (pstSession);

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}
#endif

