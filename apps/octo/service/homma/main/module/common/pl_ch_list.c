/*************************************************************************************************************
	File            : pl_ch_list.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include "pl_common.h"
#include "pl_ch_list.h"

#define ________________Static_Variables___________________________

typedef struct	_plChannelListMgr_t
{
	HxVector_t		*pSvcVector;		// (svc, ts)
	HxHASH_t		*pSvcHash;
	HINT32			nLastVersion;
	HBOOL			bIsValidTpInfo;
} plChannelListMgr_t;

typedef enum _plChannelListCmd_e
{
	ePLChannelListCmd_None				= 0x0000,

	/* listener를 통해 들어온 message */
	ePLChannelListCmd_DynamicUpdate		= 0x1000,
	ePLChannelListCmd_Loading,

	ePLChannelListCmd_End
} plChannelListCmd_e;

typedef enum _plChannelListSortCmd_e
{
	ePLChannelListSortCmd_None,
	ePLChannelListSortCmd_SortByName,
	ePLChannelListSortCmd_SortByLCN,
	ePLChannelListSortCmd_SortByUID,
} plChannelListSortCmd_e;

typedef struct _plChannelListMsgEvent_t
{

	plChannelListCmd_e	nCmdType;
	HINT32				nLastVersion;
	void				*pUserData;
} plChannelListMsgEvent_t;

STATIC plChannelListMgr_t	 s_stChannelListMgr;
STATIC HUINT32				 s_ulChannelList_SemId;
STATIC ON_CHANNEL_CHANGED   s_cbOnChannelChanged = NULL;

#define ENTER_GSEM							HxSEMT_Get (s_ulChannelList_SemId)
#define LEAVE_GSEM							HxSEMT_Release (s_ulChannelList_SemId)


#define ________________Internal_Functions___________________________
STATIC plChannelListMgr_t *_plChannelList_GetMgr(void)
{
	return &s_stChannelListMgr;
}

STATIC HUINT32	_plChannelList_hash(const void *key)
{
	return (HUINT32)key;
}

STATIC HBOOL	_plChannelList_hash_equal(const void *a, const void *b)
{
	if (a == b)
		return TRUE;

	return FALSE;
}

STATIC void		_plChannelList_hash_value_destory(void *data)
{
	if(data)
	{
		HLIB_STD_MemFree(data);
	}
}

void inline _plChannelList_MemFree(void *mem)
{
	if(mem)	HLIB_STD_MemFree(mem);
}

STATIC CO_CHANNEL_t *_plChannelList_vector_Add(HxVector_t	**vector, DxService_t *item)
{
	CO_CHANNEL_t 	*pChannel = NULL;
	DxAntInfo_t 	*pAntInfo = NULL;

	pChannel = HLIB_STD_MemCalloc(sizeof(CO_CHANNEL_t));
	if(NULL == pChannel)
	{
		return NULL;
	}

	pChannel->svc = HLIB_STD_MemDup(item, sizeof(DxService_t));

	pChannel->ts  = HOMMA_INTEROP_META_SVC_GetTransponder(item->tsuid);
	if(NULL != pChannel->ts)
	{
		// Find the antenna Information
		pAntInfo = HOMMA_INTEROP_META_ANTENNA_GetInfo(pChannel->ts->tuningParam.sat.antInfo.uid);
		if (NULL != pAntInfo)
		{
			HxSTD_MemCopy (&(pChannel->ts->tuningParam.sat.antInfo), pAntInfo, sizeof(DxAntInfo_t));
			pChannel->ts->tuningParam.sat.antennaType = pAntInfo->antType;

			HOMMA_INTEROP_META_ANTENNA_InfoFree(pAntInfo);
		}
	}

	//HxLOG_Debug("[%s:%d] CH(%s) \t\t\t %p %p %p -- \n", __func__, __LINE__, item->name, pChannel->net, pChannel->ts, pChannel->logo);
	if(NULL != vector)
	{
		(void) HLIB_VECTOR_Add(*vector, pChannel);
	}

	return pChannel;
}

STATIC HBOOL _plChannelList_vector_DeleteItem(void * data,void * userdata)
{
	CO_CHANNEL_t *channel = (CO_CHANNEL_t *)data;

	if( NULL != channel)
	{
		if(NULL != channel->svc)
			HLIB_STD_MemFree(channel->svc);

		if(NULL != channel->ts)
			HOMMA_INTEROP_META_SVC_Delete(channel->ts);

		HLIB_STD_MemFree(channel);
	}

	return FALSE;
}

STATIC void _plChannelList_vector_Delete(HxVector_t	*vector)
{
	(void) HLIB_VECTOR_ForEach(vector, _plChannelList_vector_DeleteItem, NULL);
}

// not used
STATIC HINT32		_plChannelList_DxServiceBSortByLCN (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (pstService1->lcn - pstService2->lcn);
}

STATIC HINT32		_plChannelList_SortByUID (void *pvArg1, void *pvArg2)
{
	CO_CHANNEL_t *pstCoCh1 = (CO_CHANNEL_t *)pvArg1;
	CO_CHANNEL_t *pstCoCh2 = (CO_CHANNEL_t *)pvArg2;

	if(NULL == pstCoCh2->svc) return 1;
	if(NULL == pstCoCh1->svc) return -1;

	return (pstCoCh2->svc->uid - pstCoCh1->svc->uid);
}

STATIC HINT32		_plChannelList_SortByName (void *pvArg1, void *pvArg2)
{
	CO_CHANNEL_t *pstCoCh1 = (CO_CHANNEL_t *)pvArg1;
	CO_CHANNEL_t *pstCoCh2 = (CO_CHANNEL_t *)pvArg2;

	if(NULL == pstCoCh2->svc) return 1;
	if(NULL == pstCoCh1->svc) return -1;

	return HxSTD_StrCmp(pstCoCh1->svc->name, pstCoCh2->svc->name);
}


STATIC HINT32		_plChannelList_SortByLCN (void *pvArg1, void *pvArg2)
{
	CO_CHANNEL_t *pstCoCh1 = (CO_CHANNEL_t *)pvArg1;
	CO_CHANNEL_t *pstCoCh2 = (CO_CHANNEL_t *)pvArg2;

	if(NULL == pstCoCh1->svc) return -1;
	if(NULL == pstCoCh2->svc) return 1;

	return (pstCoCh2->svc->lcn - pstCoCh1->svc->lcn);
}


STATIC HxVector_t *_plChannelList_GetServiceVector (void)
{
	HxVector_t		*pVector = NULL;
	HxVector_t		*pApkVector = NULL;
	HINT32			nApkVectorLen = 0;
	DxService_t 	*pstService = NULL;

	HxLOG_Trace();

	pApkVector = HOMMA_INTEROP_META_SVC_GetServiceList();
	if (pApkVector == NULL)
	{
		HxLOG_Warning("Cannot get channel pVector from appkit\n");
		return NULL;
	}

	nApkVectorLen = HLIB_VECTOR_Length(pApkVector);
	if (0 == nApkVectorLen)
	{
		HxLOG_Print("[%s:%d] HLIB_VECTOR_Length == 0 \n", __FUNCTION__, __LINE__);
		HLIB_VECTOR_Delete(pApkVector);
		return NULL;
	}

	pVector = HLIB_VECTOR_NewEx(NULL, (nApkVectorLen + 10), NULL, NULL, NULL);
	if (pVector == NULL)
	{
		HxLOG_Error("HLIB_VECTOR_NewEx fail\n");
		HLIB_VECTOR_Delete(pApkVector);
		return NULL;
	}

	//(void) HLIB_VECTOR_Sort(pApkVector, _plChannelList_DxServiceBSortByLCN);

	HxVECTOR_EACH(pApkVector, DxService_t *, pstService,
			_plChannelList_vector_Add(&pVector, pstService);
			);

	HLIB_VECTOR_Delete(pApkVector);

	HxLOG_Debug("[%s:%d] Antenna Count :: %d \n", __func__, __LINE__, HOMMA_INTEROP_META_ANTENNA_Count());

	return pVector;
}

STATIC HBOOL _plChannelList_CompareService(HxList_t *pSvcList, plChannelListMgr_t *pMgr)
{
	HxList_t *svclist = pSvcList;
	HxHASH_t *svchash = pMgr->pSvcHash;
	HBOOL bPrevTpInfoValid = pMgr->bIsValidTpInfo;
	CO_CHANNEL_t	*Src = NULL;
	DxService_t		*Des = NULL;
	HINT32			nTpCount = 0;

	if(svclist != NULL && svchash == NULL)
	{
		HxLOG_Print("_plChannelList_CompareService[%d] pSvcList != NULL && svcHash == NULL [CHANGED]\n"	,__LINE__);
		return TRUE;
	}
	else if(svclist == NULL && svchash != NULL)
	{
		HxLOG_Print("_plChannelList_CompareService[%d] pSvcList == NULL && svcHash != NULL [CHANGED]\n"	,__LINE__);
		return TRUE;
	}
	else if(svclist == NULL && svchash == NULL)
	{
		HxLOG_Print("_plChannelList_CompareService[%d] pSvcList == NULL && svcHash == NULL [NO-CHANGED]\n"	,__LINE__);
		return FALSE;
	}

	if(HLIB_LIST_Length(svclist) != HLIB_HASH_Size(svchash))
	{
		HxLOG_Print("_plChannelList_CompareService[%d] New Count[%d] Old Count[%d] [CHANGED]\n"	,__LINE__, HLIB_LIST_Length(pSvcList), HLIB_HASH_Size(svchash));
		return TRUE;
	}

	while(svclist)
	{
		Src = HLIB_LIST_Data(svclist);	// Src : CO_CHANNEL_t, Src->svc : DxService_t
		if(Src == NULL)
		{
			HxLOG_Error("Src == NULL\n");
			goto loop;
		}

		if(Src->svc)
		{
			Des = (DxService_t*)HLIB_HASH_Lookup(svchash, (void*)Src->svc->uid);
			if(Des)
			{
				if (0 != HxSTD_MemCmp(Src->svc, Des, sizeof(DxService_t)))
				{
					if ( (Src->svc->uid != Des->uid) ||
						(Src->svc->onid != Des->onid) ||
						(Src->svc->tsid != Des->tsid) ||
						(Src->svc->svcid != Des->svcid) ||
						(Src->svc->lcn != Des->lcn) ||
						(Src->svc->locked != Des->locked) ||
						(HxSTD_StrCmp(Src->svc->name, Des->name) != 0) )
					{
						HxLOG_Print("_plChannelList_CompareService[%d] Service updated!! The tripleid, lcn or name are changed. (ORG LCN : %d) [CHANGED]\n", __LINE__, Des->lcn);
						return TRUE;
					}
					else
					{
						HxLOG_Print("_plChannelList_CompareService[%d] Service updated!! The tripleid, lcn and name are same. (LCN : %d) [NO-CHANGED]\n", __LINE__, Des->lcn);
					}
				}
			}
			else
			{
				HxLOG_Print("_plChannelList_CompareService[%d] NO DES SRC UID[%d] LCN[%d] NAME[%s] [CHANGED]\n" ,__LINE__,Src->svc->uid, Src->svc->lcn, Src->svc->name);
				return TRUE;
			}
		}

		if(Src->ts)
		{
			nTpCount++;
		}

loop:
		svclist = svclist->next;
		Src = NULL;
		Des = NULL;
	}

	if((bPrevTpInfoValid == FALSE) && (nTpCount > 0))
	{
		HxLOG_Print("_plChannelList_CompareService[%d] TP List updated!!\n", __LINE__);
		return TRUE;
	}

	HxLOG_Print("_plChannelList_CompareService[%d]  no change....\n" ,__LINE__);

	return FALSE;
}

STATIC void _plChannelList_UnLoadData(plChannelListMgr_t *pMgr)
{
	if(pMgr->pSvcVector != NULL)
	{
		_plChannelList_vector_Delete(pMgr->pSvcVector);
		HLIB_VECTOR_Delete(pMgr->pSvcVector);
		pMgr->pSvcVector =  NULL;
	}

	if(pMgr->pSvcHash != NULL)
	{
		HLIB_HASH_Destroy(pMgr->pSvcHash);
		pMgr->pSvcHash = NULL;
	}

	pMgr->nLastVersion = 0;
}

STATIC HxList_t *_plChannelList_GetServiceList(HxVector_t *pSvcVector)
{
	CO_CHANNEL_t	*pstCoChannel = NULL, *pItem = NULL;
	HxList_t		*pSvcList = NULL;
	HINT32			i = 0;
	HINT32			n = 0;

	n = HLIB_VECTOR_Length(pSvcVector);
	for (i = 0 ; i < n ; i++)
	{
		pstCoChannel = (CO_CHANNEL_t *)HLIB_VECTOR_ItemAt(pSvcVector, i);
		if(NULL != pstCoChannel)
		{
			//mhkang: too many log
			//HxLOG_Info("ADD CH: name(%s) uid(%d) updateSvcFlag(%d) removed(%d)\n", pstCoChannel->svc->name, pstCoChannel->svc->uid, pstCoChannel->svc->updateSvcFlag, pstCoChannel->svc->removed );
			pItem = HLIB_STD_MemDup(pstCoChannel, sizeof(CO_CHANNEL_t));
			pSvcList = HLIB_LIST_Append(pSvcList, pItem);
		}
	}

	HxLOG_Debug("[%s:%d] Channel Count :: %d \n", __func__, __LINE__, HLIB_LIST_Length(HLIB_LIST_First(pSvcList)));

	return pSvcList;
}

STATIC HxList_t *_plChannelList_GetServiceListWithSort(HxVector_t *pSvcVector, HINT32 (cbCmp)(void *, void *))
{
	CO_CHANNEL_t	*pstCoChannel = NULL, *pItem = NULL;
	HxList_t		*pSvcList = NULL;
	HINT32			i = 0;
	HINT32			n = 0;

	n = HLIB_VECTOR_Length(pSvcVector);
	for (i = 0 ; i < n ; i++)
	{
		pstCoChannel = (CO_CHANNEL_t *)HLIB_VECTOR_ItemAt(pSvcVector, i);
		if(NULL != pstCoChannel)
		{
			//mhkang: too many log
			//HxLOG_Info("ADD CH: name(%s) uid(%d) updateSvcFlag(%d) removed(%d)\n", pstCoChannel->svc->name, pstCoChannel->svc->uid, pstCoChannel->svc->updateSvcFlag, pstCoChannel->svc->removed );
			pItem = HLIB_STD_MemDup(pstCoChannel, sizeof(CO_CHANNEL_t));
			pSvcList = HLIB_LIST_AppendSortEx(pSvcList, pItem, cbCmp);
		}
	}

	HxLOG_Debug("[%s:%d] Channel Count :: %d \n", __func__, __LINE__, HLIB_LIST_Length(HLIB_LIST_First(pSvcList)));

	return pSvcList;
}

STATIC HxList_t *_plChannelList_GetServiceListFromVector(HxVector_t *pSvcVector, HINT32 ulSortCmd)
{
	HxList_t *pSvcList = NULL;
	plChannelListSortCmd_e eSortCmd = (plChannelListSortCmd_e)ulSortCmd;

	if(pSvcVector == NULL)
	{
		return NULL;
	}

	switch(eSortCmd)
	{
		case ePLChannelListSortCmd_None:
			pSvcList = _plChannelList_GetServiceList(pSvcVector);
			break;

		case ePLChannelListSortCmd_SortByName:
			HxLOG_Print("[%s:%d] ePLChannelListSortCmd_SortByName\r\n", __FUNCTION__, __LINE__);
			pSvcList = _plChannelList_GetServiceListWithSort(pSvcVector, _plChannelList_SortByName);
			break;

		case ePLChannelListSortCmd_SortByLCN:
			HxLOG_Print("[%s:%d] ePLChannelListSortCmd_SortByLCN\r\n", __FUNCTION__, __LINE__);
			pSvcList = _plChannelList_GetServiceListWithSort(pSvcVector, _plChannelList_SortByLCN);
			break;

		case ePLChannelListSortCmd_SortByUID:
			HxLOG_Print("[%s:%d] ePLChannelListSortCmd_SortByUID\r\n", __FUNCTION__, __LINE__);
			pSvcList = _plChannelList_GetServiceListWithSort(pSvcVector, _plChannelList_SortByUID);
			break;

		default:
			break;
	}

	return pSvcList;
}

// list -> hash
STATIC HxHASH_t*_plChannelList_GetServiceHashFromList (HxList_t *pServiceList, HBOOL *bValidTpInfo)
{
	HxHASH_t *svchash;
	HxList_t *svclist = NULL;
	HINT32	nTpCount = 0;

	svchash = HLIB_HASH_NewFull(_plChannelList_hash, _plChannelList_hash_equal, NULL, _plChannelList_hash_value_destory);

	svclist = pServiceList;

	while(svclist)
	{
		DxService_t * service = NULL;
		CO_CHANNEL_t *pChannel = NULL;
		pChannel = HLIB_LIST_Data(svclist);
		if(pChannel != NULL)
		{
			if(pChannel->ts)
			{
				nTpCount++;
			}

			if(pChannel->svc)
			{
				service = (DxService_t *)HLIB_STD_MemCalloc(sizeof(DxService_t));
				if(service != NULL)
				{
					HxSTD_MemCopy(service, pChannel->svc, sizeof(DxService_t));
					HLIB_HASH_Insert(svchash, (void*)service->uid, (void*)service);
					//HxLOG_Print("_plChannelList_GetServiceHash() service uid = %x lcn = %d Name : %s\n", service->uid, service->lcn, service->name);
				}
				else
				{
					HxLOG_Error("service == NULL\n");
				}
			}
		}

		svclist = svclist->next;
	}

	if(HLIB_HASH_Size(svchash) == 0)
	{
		HLIB_HASH_Destroy(svchash);
		svchash = NULL;
	}

	*bValidTpInfo = (nTpCount > 0) ? TRUE : FALSE;

	HxLOG_Debug("_plChannelList_GetServiceHashFromList() chlist Count --> [%d], TP Count --> [%d]\n",  HLIB_LIST_Length(pServiceList), nTpCount);

	return svchash;
}

STATIC HBOOL _plChannelList_MakeServiceList(HBOOL bFirstTime)
{
	plChannelListMgr_t	*pMgr = _plChannelList_GetMgr();
	HxVector_t			*pSvcVector = NULL;
	HxHASH_t			*pSvcHash = NULL;
	HxList_t			*pSvcList = NULL;
	HBOOL				bChanged = FALSE;
	HBOOL				bIsValidTpInfo = FALSE;

	// get service list from appkit (set svc, ts structure to vector)
	pSvcVector = _plChannelList_GetServiceVector();

	// vector -> list
	pSvcList = _plChannelList_GetServiceListFromVector(pSvcVector, ePLChannelListSortCmd_None);

	// compare service list between old and new
	bChanged = _plChannelList_CompareService(pSvcList, pMgr);

	if(bChanged == TRUE)
	{
		// make service hash from new service list
		pSvcHash = _plChannelList_GetServiceHashFromList(pSvcList, &bIsValidTpInfo);

		if(bFirstTime != TRUE)
		{
			ENTER_GSEM;
			_plChannelList_UnLoadData(pMgr);
			LEAVE_GSEM;
			HxLOG_Debug("pMgr->pServiceHash uploaded!!\n");
		}
		else
		{
			HxLOG_Debug("don't need to unload data!!\n");
		}

		// update context with new service vector, hash
		ENTER_GSEM;
		pMgr->pSvcVector = pSvcVector;
		pMgr->pSvcHash = pSvcHash;
		pMgr->bIsValidTpInfo = bIsValidTpInfo;
		LEAVE_GSEM;
	}
	else
	{
		if(pSvcVector != NULL)
		{
			_plChannelList_vector_Delete(pSvcVector);
			HLIB_VECTOR_Delete(pSvcVector);
		}
	}

	return bChanged;
}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
STATIC void _plChannelList_EventListener (HINT32 event, const HINT32 *args, void *userdata)
{
	HBOOL bChanged = FALSE;

	HxLOG_Info("(+) event : %d\n", event);

	// pass the event parameter to _plChannelList_MakeServiceList() ???
	bChanged = _plChannelList_MakeServiceList(FALSE);

	if (bChanged == TRUE)
	{
		if (s_cbOnChannelChanged != NULL)
			s_cbOnChannelChanged();
	}

	HxLOG_Info("(-)\n");
}
#else
STATIC void _plChannelList_EventListener (void)
{
	HBOOL bChanged = FALSE;

	HxLOG_Info("[%s] (+)\n", __FUNCTION__);

	// pass the event parameter to _plChannelList_MakeServiceList() ???
	bChanged = _plChannelList_MakeServiceList(FALSE);

	if (bChanged == TRUE)
	{
		if (s_cbOnChannelChanged != NULL)
		{
			s_cbOnChannelChanged();
		}
		else
		{
			HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] Because s_cbOnChannelChanged is not registed, the channel infomration can't be updated.\n"), __FUNCTION__, __LINE__);
		}
	}
	else
	{
		HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] Channel informations were not changed.\n"), __FUNCTION__, __LINE__);
	}

	HxLOG_Info("[%s] (-)\n", __FUNCTION__);
}
#endif

/* Only Mainapp (corsair) can change the channel list.
    It is not necessary to check if service has been updated for other app */
// 2014/11/14 mhkang: IR4100HD #if 0
STATIC HBOOL _plChannelList_UpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated)
{
	DxService_t *pOld = (DxService_t*)_old;
	DxService_t *pNew = (DxService_t*)_new;

    HxLOG_Info("typeName:%s _old:%p _new:%p (+)\n", typeName, _old, _new);

	*updated = FALSE;

#define	INT_UPDATED(prop)	(pOld->prop != pNew->prop)
#define	MEM_UPDATED(prop,s)	(memcmp(pOld->prop, pNew->prop, s) != 0)
#define STR_UPDATED(prop)	(strcmp(pOld->prop, pNew->prop) != 0)

	if (pOld && pNew)	// updated
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
		{
			if (	INT_UPDATED(uid)
				|| INT_UPDATED(tsuid)
				|| INT_UPDATED(prvuid)
				|| INT_UPDATED(antuid)
				|| MEM_UPDATED(grpuids, (sizeof(HINT32) * DxGROUP_LEN))
				|| MEM_UPDATED(bqtuids, (sizeof(HINT32) * DxBOUQUET_LEN))
				|| INT_UPDATED(svcid)
				|| INT_UPDATED(tsid)
				|| INT_UPDATED(onid)
				|| INT_UPDATED(lcn)
				|| INT_UPDATED(locked)
				|| (STR_UPDATED(name) && pNew->renamed)
				|| INT_UPDATED(removed)
//				|| INT_UPDATED(audioPid)
//				|| INT_UPDATED(user.subtitle)
//				|| INT_UPDATED(user.subtitleTrack)
//				|| INT_UPDATED(user.audio)
//				|| INT_UPDATED(user.audioTrack)
//				|| INT_UPDATED(user.ratingPassedEventId)
			) {
				*updated = TRUE;
			}
		}
	}
	else if (pOld)		// removed
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else if (pNew)		// added
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else
	{
		HxLOG_Critical("[%s] WTF!!!!!!\n", __FUNCTION__);
	}

#undef	INT_UPDATED
#undef	MEM_UPDATED
#undef	STR_UPDATED

	HxLOG_Info("(-)\n");

	return TRUE;
}
// 2014/11/14 mhkang: IR4100HD  #endif

#define ________________Public_Functions___________________________
HERROR	PL_COMMON_InitChannelList(void)
{
	HERROR				hErr = ERR_OK;
	plChannelListMgr_t	*pMgr = _plChannelList_GetMgr();

	HxLOG_Info("(+)\n");

	HxSTD_memset(pMgr, 0, sizeof(plChannelListMgr_t));

	if(0 != s_ulChannelList_SemId)
	{
		HxLOG_Error("s_ulChannelList_SemId(%d) is already initialized.\n", s_ulChannelList_SemId);
		return ERR_FAIL;
	}

	hErr = HxSEMT_Create (&s_ulChannelList_SemId, "PLChannelListSem", HxSEMT_FIFO);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("VK_SEM_Create failed(hErr=%d)!!!\n", hErr);
		return ERR_FAIL;
	}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	HOMMA_INTEROP_META_SVC_Init(FALSE);
	HOMMA_INTEROP_META_ANTENNA_Init(FALSE);

	/* Only Mainapp (corsair) can change the channel list.
	It is not necessary to check if service has been updated for other app */
	// 2014/11/14 mhkang: comment out in IR4100HD
//	APK_META_SVC_SetUpdateChecker(_plChannelList_UpdateChecker);
	HOMMA_INTEROP_META_SVC_SetListener(_plChannelList_EventListener, (void *)NULL);
#else
	HOMMA_INTEROP_META_SVC_Init();
	HOMMA_INTEROP_META_SVC_SetListener(_plChannelList_EventListener);

	HOMMA_INTEROP_META_ANTENNA_Init();
#endif
#if 0
	HOMMA_INTEROP_MEDIA_PLAY_Init();
#endif
	HOMMA_INTEROP_SYSINFO_TUNER_Init();
#if 0
	HOMMA_INTEROP_SCENARIO_Init();
#endif

	HxLOG_Info("(-)\n");

	return ERR_OK;
}

HERROR	PL_COMMON_DeInitChannelList(void)
{
	HERROR				hErr = ERR_OK;
	plChannelListMgr_t	*pMgr = _plChannelList_GetMgr();

	HxLOG_Info("(+)\n");

	HxSTD_memset(pMgr, 0, sizeof(plChannelListMgr_t));

	ENTER_GSEM;
	_plChannelList_UnLoadData(pMgr);
	LEAVE_GSEM;

	HxSTD_memset(pMgr, 0, sizeof(plChannelListMgr_t));

	hErr = HxSEMT_Destroy(s_ulChannelList_SemId);
	if (ERR_OK != hErr)
	{
        HxLOG_Error ("VK_SEM_Create failed(hErr=%d)!!!\n", hErr);
		return ERR_FAIL;
	}

    HxLOG_Info("(-)\n");

	return ERR_OK;
}


// not used
HERROR	PL_COMMON_UpdateChannelList(void)
{
#if 0
	DxAntennaConnection_e 		eAntCon = eDxANTCONN_ONECABLE;
	DxDBSetupItem_t				stItemInfo = {0,};

	/* Get Antenna Connection Type */
	HUINT32 id = DAPI_Get(DxDBSETUP_SYSTEM,(HUID)eDxSETUP_SYSTEM_ANTENNA_CONNECTION,__plcommon_GetDbSetupInfo,( HINT32 * )&stItemInfo);
	DAPI_Wait(10000, id);
	eAntCon = stItemInfo.nValue;
#endif

#if 0
	plChannelListMsgEvent_t 	stMsgEvent;

	HxSTD_memset(&stMsgEvent, 0, sizeof(plChannelListMsgEvent_t));
	stMsgEvent.nCmdType = ePLChannelListCmd_Update;

	_plChannelList_ProcessEvent(&stMsgEvent);
#endif
	{
		STATIC UNIXTIME s_utLastUpdate = 0;

		UNIXTIME utCur = 0;

		utCur = HLIB_STD_GetSystemTime();
		if(s_utLastUpdate + 10000 < utCur)
		{
			HOMMA_INTEROP_META_SVC_LoadupdatedList();
			HLIB_STD_TaskSleep(100);
		}
	}

	return ERR_OK;
}

HERROR	PL_COMMON_GetChannelList(HxList_t **pChannelList)
{
	plChannelListMgr_t			*pMgr = _plChannelList_GetMgr();
	HBOOL						bChanged = FALSE;
	HxList_t 					*pSvcList = NULL;

	if(pMgr->pSvcVector == NULL)
	{
		bChanged = _plChannelList_MakeServiceList(TRUE);
	}

	(void)bChanged;

	pSvcList = _plChannelList_GetServiceListFromVector(pMgr->pSvcVector, ePLChannelListSortCmd_SortByLCN);

	HxLOG_Print("[%s:%d] enter sem!\r\n", __FUNCTION__, __LINE__);

	ENTER_GSEM;
	*pChannelList = pSvcList;

	return ERR_OK;
}

HERROR	PL_COMMON_ReleaseChannelList(HxList_t *pChannelList)
{
	if(pChannelList != NULL)
	{
		HLIB_LIST_Foreach(pChannelList , _plChannelList_MemFree);
		HLIB_LIST_RemoveAll(pChannelList);
	}

	LEAVE_GSEM;


	HxLOG_Print("[%s:%d] leave sem !\r\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

HERROR	PL_COMMON_GetChannel(HINT32 uid, CO_CHANNEL_t *pChannel)
{
	CO_CHANNEL_t 				*pstCoCh = NULL;
	plChannelListMgr_t			*pMgr = _plChannelList_GetMgr();
	HxList_t					*pSvcList = NULL;

	if(NULL == pChannel)
	{
		return ERR_FAIL;
	}
	else
	{
		HxSTD_memset(pChannel, 0, sizeof(CO_CHANNEL_t));
		pChannel->bValid = FALSE;
		pChannel->svc    = NULL; /* for NULL comparison */
		pChannel->ts     = NULL;
	}

	pSvcList = _plChannelList_GetServiceListFromVector(pMgr->pSvcVector, ePLChannelListSortCmd_SortByUID);

	ENTER_GSEM;
	while(pSvcList)
	{
		pstCoCh = HLIB_LIST_Data(pSvcList);
		if(NULL != pstCoCh && NULL != pstCoCh->svc)
		{
			if(pstCoCh->svc->uid == uid)
			{
				HxSTD_MemCopy(pChannel, pstCoCh, sizeof(CO_CHANNEL_t));
				pChannel->bValid = TRUE;
				LEAVE_GSEM;
				return ERR_OK;
			}
		}

		pSvcList  = pSvcList->next;
	}
	LEAVE_GSEM;

	if(pSvcList != NULL)
	{
		HLIB_LIST_Foreach(pSvcList , _plChannelList_MemFree);
		HLIB_LIST_RemoveAll(pSvcList);
	}

	return ERR_FAIL;
}

HERROR	PL_COMMON_GetChannelCount(HUINT32 *pCount)
{
	plChannelListMgr_t			*pMgr = _plChannelList_GetMgr();

	ENTER_GSEM;
	*pCount = HLIB_VECTOR_Length(pMgr->pSvcVector);
	LEAVE_GSEM;

	return ERR_OK;
}

HERROR	PL_COMMON_GetLastVersion(HUINT32 *pnLastVersion)
{
	plChannelListMgr_t			*pMgr = _plChannelList_GetMgr();

	ENTER_GSEM;
	*pnLastVersion = pMgr->nLastVersion;
	LEAVE_GSEM;

	return ERR_OK;
}

/* list data : DxAntInfo_t * */
HERROR	PL_COMMON_GetAntenaList(HxList_t **list)
{
	HINT32 		nAntCount = 0;
	HINT32		i = 0;
	HxList_t	*pAntList = NULL;
	DxAntInfo_t *pAntInfo = NULL;

#ifdef HOMMA_USE_APPKIT_DIRECTLY		// will be removed
	HOMMA_INTEROP_META_ANTENNA_Load();
#endif

	nAntCount = HOMMA_INTEROP_META_ANTENNA_Count();

	for(i=0; i < nAntCount; i++)
	{
		pAntInfo = HOMMA_INTEROP_META_ANTENNA_GetInfo(HOMMA_INTEROP_META_ANTENNA_GetUidAt(i));
		if(NULL != pAntInfo)
		{
			pAntList = HLIB_LIST_Append(pAntList,pAntInfo);
		}
	}

	*list = pAntList;

	return ERR_OK;
}

void PL_COMMON_ANTENNA_InfoFree (DxAntInfo_t *p)
{
	if (p != NULL)
	{
		HLIB_STD_MemFree(p);
	}
}

void	PL_COMMON_ReleseAntenaList(HxList_t *list)
{
	HLIB_LIST_RemoveAllFunc(list, (void *)PL_COMMON_ANTENNA_InfoFree);
}


HERROR PL_COMMON_GetCurrentServiceInfo( CO_CHANNEL_t **ppCoChannel, HUINT32 *ppulSvcUID)
{
	DxService_t 		*pstService = NULL;
	HUINT32				ulSvcUID = 0;
	HERROR				hErr;
	HUINT32 			ulViewId;

	hErr =	HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId error!\r\n");
		return ERR_FAIL;
	}

	hErr = HOMMA_INTEROP_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, &ulSvcUID);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("HOMMA_INTEROP_MEDIA_PLAY_GetRequestId error!\r\n");
		return ERR_FAIL;
	}

	pstService = HOMMA_INTEROP_META_SVC_GetService(ulSvcUID);
	if (NULL == pstService)
	{
		HxLOG_Debug("can't get service (ulSvcUID:%d)\r\n", ulSvcUID);
		return ERR_FAIL;
	}

	*ppCoChannel = _plChannelList_vector_Add(NULL, pstService);
	if(NULL != ppulSvcUID)
	{
		*ppulSvcUID = ulSvcUID;
	}

	HOMMA_INTEROP_META_SVC_Delete(pstService);


	return ERR_OK;
}


HERROR PL_COMMON_ReleaseCurrentServiceInfo( CO_CHANNEL_t *pCoChannel)
{
	if(NULL != pCoChannel)
	{
		_plChannelList_vector_DeleteItem(pCoChannel, NULL);
	}
	return ERR_OK;
}


// not used
void	PL_COMMON_RemoveWarningFunction(void)
{
	(void)_plChannelList_DxServiceBSortByLCN;
}

HERROR	PL_COMMON_GetSatIpSignalLevel(HINT32 nIndex, HINT32 *pnLevel)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nSignalLevel = 0; /* percentage */
	HINT32		nSatIpLevel = 0; /* 0~255 */
	HBOOL		bLocked = FALSE;

	if(NULL == pnLevel)
		return ERR_FAIL;

	hErr = HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(nIndex, &bLocked);
	if(ERR_OK != hErr)
		return ERR_FAIL;

	if(bLocked)
	{
		hErr = HOMMA_INTEROP_SYSINFO_TUNER_GetSignalLevel(nIndex, &nSignalLevel);
		if(ERR_OK != hErr)
			return ERR_FAIL;

		*pnLevel = nSignalLevel * 255 / 100;
	}
	else
	{
		*pnLevel = 0;
	}

	HxLOG_Print("[%s:%d] (%d) bLocked = %d. nSignalLevel = %d \r\n", __FUNCTION__, __LINE__, nIndex, bLocked, nSignalLevel);
	(void)nSatIpLevel;

	return ERR_OK;
}


HERROR	PL_COMMON_GetSatIpSignalQuality(HINT32 nIndex, HINT32 *pnQuality)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nSignalQuality = 0; /* percentage */
	HINT32		nSatIpQuality = 0; /* 0~15 */
	HBOOL		bLocked = FALSE;

	if(NULL == pnQuality)
		return ERR_FAIL;

	hErr = HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(nIndex, &bLocked);
	if(ERR_OK != hErr)
		return ERR_FAIL;

	if(bLocked)
	{
		hErr = HOMMA_INTEROP_SYSINFO_TUNER_GetSignalQuality(nIndex, &nSignalQuality);
		if(ERR_OK != hErr)
			return ERR_FAIL;

		*pnQuality = nSignalQuality * 15 / 100;
	}
	else
	{
		*pnQuality = 0;
	}

	HxLOG_Print("[%s:%d] (%d) bLocked = %d. nSignalQuality = %d \r\n", __FUNCTION__, __LINE__, nIndex, bLocked, nSignalQuality);
	(void)nSatIpQuality;

	return ERR_OK;
}

HERROR	PL_COMMON_IsTunerLocked(HINT32 nIndex, HBOOL *pbLocked)
{
	return HOMMA_INTEROP_SYSINFO_TUNER_IsLocked(nIndex, pbLocked);
}

HERROR 	PL_COMMON_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return HOMMA_INTEROP_SCENARIO_GetAntennaConnection(peConnectionType);
}

HERROR	PL_COMMON_GetTuningInfoChannelName(HUID	uUID, DxTuneParam_t	*pstTuningParam, HxSTR_t **ppChNameStr)
{
	HERROR			hErr = ERR_OK;
	HxList_t 		*pChList = NULL;
	HxList_t 		*pRootChList = NULL;
	CO_CHANNEL_t	*pstCoCh = NULL;

	hErr = PL_COMMON_GetChannelList(&pRootChList);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("Error> PL_COMMON_GetChannelList\r\n");
		return eHOM_Result_Error;
	}

	pChList = pRootChList;
	while(pChList)
	{
		pstCoCh = HLIB_LIST_Data(pChList);
		if(NULL != pstCoCh && NULL != pstCoCh->ts)
		{
			if(pstCoCh->svc->uid == uUID)
			{
				*ppChNameStr = HLIB_STR_Append(*ppChNameStr, pstCoCh->svc->name);
				HxSTD_MemCopy(pstTuningParam, &pstCoCh->ts->tuningParam, sizeof(DxTuneParam_t));

				break;
			}
		}
		pChList = HLIB_LIST_Next(pChList);
	}

	PL_COMMON_ReleaseChannelList(pRootChList);

	return eHOM_Result_Ok;
}


/**
  @date 2014/3/17
  @author mhkang
  @brief STB에서 채널정보가 변경(Add/Modify/Delete)되어 _plChannelList_EventListener 함수가 호출되었을 때,
        pl_dms_scan_epg에게 채널 갱신하라고 알려주는 Callback onChanged 등록한다.
  @param onchanged 채널이 변경되었을 때 호출되는 콜백함수.
  @return eHOM_Result_Ok
*/
HERROR	PL_COMMON_SetChannelNotifier(ON_CHANNEL_CHANGED onChanged)
{
    s_cbOnChannelChanged = onChanged;

    return eHOM_Result_Ok;
}

/* end od file */

