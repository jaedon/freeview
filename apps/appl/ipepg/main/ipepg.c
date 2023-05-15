/**************************************************************
 *	@file		ipepg.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/

#include <unistd.h>
#include <signal.h>
#if (defined(CONFIG_DEBUG) & defined(OS_LINUX))
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include "../include/ipepg.h"
#include <hapi.h>
#include <hlib.h>
#include <apk.h>
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include <vkernel.h>
#include <oapi.h>
#include <thapi.h>
#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

#if defined(CONFIG_PROD_FVP4000T)
#include "ipepg_fvc.h"
#endif

#if defined(CFG_OPEN_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif


#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
#include	"ipepg_debug.c"
#endif

#define	USERDB_IPEPG_ONOFF							"UDB/IPEPG_OnOff"			// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_RP_ONOFF								"UDB/RP_OnOff"				// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_RP_PERIOD_START						"UDB/RP_PeriodStart"		// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_RP_PERIOD_END						"UDB/RP_PeriodEnd"			// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_RP_INTERVAL							"UDB/RP_Interval"			// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_RP_REG_CODE							"UDB/RP_RegCode"			// opl 에서 관리 ipepg 는 참조만.
#define	USERDB_IPEPG_LASTUPDATETIME					"UDB/IPEPG_UpdateTime"		// ipepg 에서 관리
#define	USERDB_IPEPG_SVCLISTUPDATED					"UDB/IPEPG_SvcListUpdated"	// ipepg 에서 관리
#define	USERDB_IPEPG_DOWNLOADPROGRESS				"UDB/IPEPG_DownProgress"	// ipepg 에서 관리
#define	USERDB_IPEPG_DB_COUNT						"UDB/IPEPG_DB_COUNT"		// ipepg 에서 관리
#define	USERDB_IPEPG_SVC_COUNT						"UDB/IPEPG_SVC_COUNT"		// ipepg 에서 관리
#define	USERDB_IPEPG_EVT_COUNT						"UDB/IPEPG_EVT_COUNT"		// ipepg 에서 관리
#define USERDB_IPEPG_STAMPTIME						"UDB/IPEPG_StampTime"		// ipepg 에서 관리
#define USERDB_PP_VER							"UDB/Privacy_Policy_Ver"	// Privacy Policy Version 

static IPEPG_DATAMGR_t * gDataMgr = NULL;

const IPEPG_ItemInfo_t gIpepgDefaultItems[] =
{
	{	eIPEPG_ITEM_SERVERURL				, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_SYS_ID					, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_SYS_VER					, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_MAC						, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_SVCLIST					, eIPEPG_DATATYPE_HASH		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_NETWORK					, eIPEPG_DATATYPE_BOOL		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_UPDATE_TIME				, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_HDD_STATUS 				, eIPEPG_DATATYPE_BOOL	 	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FLAG_SVC_UPDATED		, eIPEPG_DATATYPE_BOOL		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_STAMP_TIME				, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_LAST_UPDATE_TIME		, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_DOWNLOADPROGRESS		, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FLAG_IPEPG_ONOFF		, eIPEPG_DATATYPE_BOOL		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_SCHEDULE_LOADED			, eIPEPG_DATATYPE_BOOL		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FLAG_RP_ONOFF			, eIPEPG_DATATYPE_BOOL		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_RP_UPDATE_INTERVAL		, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_RP_PERIOD_START			, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_RP_PERIOD_END			, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_RP_UPDATE_TIME			, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_SVC_COUNT				, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_EVT_COUNT				, eIPEPG_DATATYPE_VALUE		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_PRIVACY_POLICY_VER			, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FVC_BASE_URL			, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FVC_AUTH_HASH			, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FVC_AUTH_TIME_STAMP		, eIPEPG_DATATYPE_STRING	,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_FVC_SERIES_EVENT_LIST	, eIPEPG_DATATYPE_LIST		,	0,				NULL, NULL, NULL},
	{	eIPEPG_ITEM_MAX						, eIPEPG_DATATYPE_VALUE 	,	0,				NULL, NULL, NULL},

};

static HUINT32	ipepg_svclist_hash(const void *key)
{
	return (HUINT32)key;
}

static HBOOL	ipepg_svclist_hash_equal(const void *a, const void *b)
{
	if (a == b)
		return TRUE;

	return FALSE;
}

static void		ipepg_svclist_hash_value_destory(void *data)
{
	IPEPG_MemFree(data);
}


static HBOOL ipepg_ItemCompCallback( void *S1, void *S2 )
{
	IPEPG_FvcSeriesEvent_t *pstUserData = S1;
	IPEPG_FvcSeriesEvent_t *pstListData = S2;

	if (pstUserData == NULL || pstListData == NULL)
	{
		return FALSE;
	}
	if (pstUserData->uid == pstListData->uid
		&& pstUserData->usSvcId == pstListData->usSvcId
		&& pstUserData->ulStartTime == pstListData->ulStartTime)
	{
		return TRUE;
	}
	return FALSE;
}


void inline IPEPG_MemFree(void *mem)
{
	if(mem)	HLIB_STD_MemFree(mem);
}

void ipepg_modifyServiceItem(void *pData)
{
	if (gDataMgr != NULL )
	{
		IPEPG_Item_e eItem = eIPEPG_ITEM_SVCLIST;
		IPEPG_ItemInfo_t *pItem = NULL;

		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pIpEPG[eItem];

		if(pItem->eItemType == eIPEPG_DATATYPE_HASH)
		{
			if(pItem->eItem == eIPEPG_ITEM_SVCLIST)
			{
				HxList_t * NewList = (HxList_t*)pData;

				while(NewList)
				{
					IPEPG_SvcInfo_t *Src = NULL, *Des = NULL;
					Src = HLIB_LIST_Data(NewList);

					if(Src)
					{
						Des = (IPEPG_SvcInfo_t*)HLIB_HASH_Lookup(pItem->hash, (void*)Src->ulUid);
						if(Des)
						{
							if (0 != HxSTD_MemCmp(Src, Des, sizeof(IPEPG_SvcInfo_t)))
							{
								HxSTD_MemCopy(Des, Src, sizeof(IPEPG_SvcInfo_t));
							}
						}
					}
					NewList = NewList->next;
				}
				HxLOG_Debug("ipepg_SetItem New chlist Count[%d]\n",  HLIB_LIST_Length(pData));
			}
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
}

void ipepg_SetAPKDBItem(IPEPG_Item_e eItem, void *pData)
{
	if (gDataMgr != NULL )
	{
		IPEPG_ItemInfo_t *pItem = NULL;
		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pIpEPG[eItem];
		if(pItem->eItemType == eIPEPG_DATATYPE_BOOL)
		{
			if (pItem->eItem == eIPEPG_ITEM_FLAG_IPEPG_ONOFF)
			{
				APK_DB_SetInt(USERDB_IPEPG_ONOFF, (HUINT32)pData);
			}
			else if (pItem->eItem == eIPEPG_ITEM_FLAG_RP_ONOFF )
			{
				APK_DB_SetInt(USERDB_RP_ONOFF, (HUINT32)pData);
			}
		}
		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
}


void ipepg_SetItem(IPEPG_Item_e eItem, void *pData)
{
	if (gDataMgr != NULL )
	{
		IPEPG_ItemInfo_t *pItem = NULL;

		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pIpEPG[eItem];


		if(pItem->eItemType == eIPEPG_DATATYPE_BOOL)
		{
			pItem->itemValue = (HUINT32)pData;

			if(pItem->eItem == eIPEPG_ITEM_FLAG_SVC_UPDATED)
			{
				APK_DB_SetInt(USERDB_IPEPG_SVCLISTUPDATED, (HUINT32)pData);
			}
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_VALUE)
		{
			pItem->itemValue = (HUINT32)pData;
			if(pItem->eItem == eIPEPG_ITEM_LAST_UPDATE_TIME)
			{
				APK_DB_SetInt(USERDB_IPEPG_LASTUPDATETIME, (HUINT32)pData);
			}
			else if(pItem->eItem == eIPEPG_ITEM_STAMP_TIME)
			{
				APK_DB_SetInt(USERDB_IPEPG_STAMPTIME, (HUINT32)pData);
				HxLOG_Debug("[%s]Set stampTime (%d)\n", __FUNCTION__,(HUINT32)pData);
			}
			else if(pItem->eItem == eIPEPG_ITEM_DOWNLOADPROGRESS)
			{
				APK_DB_SetInt(USERDB_IPEPG_DOWNLOADPROGRESS, (HUINT32)pData);
				HxLOG_Debug("[%s]Set DOWNLOADPROGRESS (%d)\n", __FUNCTION__,(HUINT32)pData);
			}
			else if(pItem->eItem == eIPEPG_ITEM_SVC_COUNT)
			{
				APK_DB_SetInt(USERDB_IPEPG_SVC_COUNT, (HUINT32)pData);
				HxLOG_Debug("[%s]Set SVC_COUNT (%d)\n", __FUNCTION__,(HUINT32)pData);
			}
			else if(pItem->eItem == eIPEPG_ITEM_EVT_COUNT)
			{
				APK_DB_SetInt(USERDB_IPEPG_EVT_COUNT, (HUINT32)pData);
				HxLOG_Debug("[%s]Set EVT_COUNT (%d)\n", __FUNCTION__,(HUINT32)pData);
			}
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_STRING)
		{
			IPEPG_MemFree(pItem->pucData);
			pItem->pucData = IPEPG_StrDup((HCHAR *)pData);
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_HASH)
		{
			if(pItem->hash != NULL)
			{
				HxLOG_Debug("ipepg_SetItem old chlist Count[%d]\n",  HLIB_HASH_Size(pItem->hash));
				HLIB_HASH_Destroy(pItem->hash);
				pItem->hash = NULL;
			}

			if(pItem->eItem == eIPEPG_ITEM_SVCLIST)
			{
				HxList_t * NewList = (HxList_t*)pData;
				pItem->hash = HLIB_HASH_NewFull(ipepg_svclist_hash, ipepg_svclist_hash_equal, NULL, ipepg_svclist_hash_value_destory);

				while(NewList)
				{
					IPEPG_SvcInfo_t * service = NULL;
					service = (IPEPG_SvcInfo_t *)IPEPG_MemAlloc(sizeof(IPEPG_SvcInfo_t));
					HxSTD_MemCopy(service, NewList->data, sizeof(IPEPG_SvcInfo_t));
					HLIB_HASH_Insert(pItem->hash, (void*)service->ulUid, (void*)service);
					//if(service) HxLOG_Debug("ipepg_SetItem service uid = %x lcn = %d Name : %s\n", service->ulUid, service->ulLcn, service->ucName);
					NewList = NewList->next;
				}

				if(HLIB_HASH_Size(pItem->hash) == 0)
				{
					HLIB_HASH_Destroy(pItem->hash);
					pItem->hash = NULL;
				}

				HxLOG_Debug("ipepg_SetItem New chlist Count[%d]\n",  HLIB_LIST_Length(pData));
			}
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
}

void * ipepg_GetItemPoint(IPEPG_DATATYPE_e eItem)
{
	if(gDataMgr != NULL )
	{
		IPEPG_ItemInfo_t *pItem = NULL;

		HxSEMT_Get(gDataMgr->ulWriteSema);
		pItem = &gDataMgr->pIpEPG[eItem];

		if(pItem->eItemType == eIPEPG_DATATYPE_STRING)
		{
			HUINT8 * pData = NULL;
			if(pItem->pucData)
			{
				pData = IPEPG_StrDup(pItem->pucData);
			}
			HxSEMT_Release(gDataMgr->ulWriteSema);
			return pData;
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_HASH)
		{
			if(pItem->hash && pItem->eItem == eIPEPG_ITEM_SVCLIST)
			{
				IPEPG_SvcInfo_t * oldService = NULL;
				IPEPG_SvcInfo_t * newService = NULL;
				HxHASH_TableIter_t	iter;
				HUINT32				svcUid;
				HxHASH_t * pHash = HLIB_HASH_NewFull(ipepg_svclist_hash, ipepg_svclist_hash_equal, NULL, ipepg_svclist_hash_value_destory);

				HLIB_HASH_IterInit(&iter, pItem->hash);
				while (HLIB_HASH_IterNext(&iter, (void**)&svcUid, (void**)&oldService))
				{
					if(oldService)
					{
						newService = (IPEPG_SvcInfo_t *)IPEPG_MemAlloc(sizeof(IPEPG_SvcInfo_t));
						HxSTD_MemCopy(newService, oldService, sizeof(IPEPG_SvcInfo_t));
						HLIB_HASH_Insert(pHash, (void*)svcUid, (void*)newService);
						//if(newService) HxLOG_Debug("ipepg_GetItemPoint newService uid = %x lcn = %d Name : %s\n", newService->ulUid, newService->ulLcn, newService->ucName);
					}

					oldService = NULL;
					newService = NULL;
				}

				if(HLIB_HASH_Size(pHash) == 0)
				{
					HLIB_HASH_Destroy(pHash);
					pHash = NULL;
				}
				HxSEMT_Release(gDataMgr->ulWriteSema);
				return pHash;
			}
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_LIST)
		{
			if (pItem->eItem == eIPEPG_ITEM_FVC_SERIES_EVENT_LIST)
			{
				HxList_t *pstList = NULL;
				HxList_t *iter = NULL;
				IPEPG_FvcSeriesEvent_t *pstNewEvt = NULL;
				IPEPG_FvcSeriesEvent_t *pstOrgEvt = NULL;

				// make copy
				for (iter = HLIB_LIST_First(pItem->list); iter != NULL; iter = HLIB_LIST_Next(iter))
				{
					pstOrgEvt = (IPEPG_FvcSeriesEvent_t *)HLIB_LIST_Data(iter);
					if (pstOrgEvt == NULL)
					{
						continue;
					}

					pstNewEvt = (IPEPG_FvcSeriesEvent_t *)IPEPG_MemAlloc(sizeof(IPEPG_FvcSeriesEvent_t));
					if (pstNewEvt == NULL)
					{
						HxLOG_Error("malloc failed. \n");
						continue;
					}

					HxSTD_MemCopy(pstNewEvt, pstOrgEvt, sizeof(IPEPG_FvcSeriesEvent_t));
					pstList = HLIB_LIST_Append(pstList, pstNewEvt);
				}

				// return copy
				HxSEMT_Release(gDataMgr->ulWriteSema);
				return pstList;
		}
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
	return NULL;
}

HERROR ipepg_AddItemToPoint (IPEPG_DATATYPE_e eItem, void *userItem)
{
	IPEPG_ItemInfo_t *pItem = NULL;
	HERROR			hErr = ERR_FAIL;

	if(gDataMgr == NULL )
	{
		return ERR_FAIL;
	}

	HxSEMT_Get(gDataMgr->ulWriteSema);

	pItem = &gDataMgr->pIpEPG[eItem];
	IPEPG_GOTO_IF(pItem->eItemType != eIPEPG_DATATYPE_LIST, END_FUNC);

	pItem->list = HLIB_LIST_Append(pItem->list, userItem);

	hErr = ERR_OK;

END_FUNC:
	HxSEMT_Release(gDataMgr->ulWriteSema);

	return hErr;
}

HERROR ipepg_DeleteItemFromPoint (IPEPG_DATATYPE_e eItem, void *userItem)
{
	IPEPG_ItemInfo_t *pItem = NULL;
	HERROR			hErr = ERR_FAIL;
	HxList_t		 *pToDeleteList = NULL;

	if(gDataMgr == NULL )
	{
		return ERR_FAIL;
	}

	HxSEMT_Get(gDataMgr->ulWriteSema);

	pItem = &gDataMgr->pIpEPG[eItem];
	IPEPG_GOTO_IF(pItem->eItemType != eIPEPG_DATATYPE_LIST, END_FUNC);

	pToDeleteList = HLIB_LIST_FindEx(pItem->list, userItem, ipepg_ItemCompCallback);
	IPEPG_GOTO_IF(pToDeleteList == NULL, END_FUNC);
	IPEPG_GOTO_IF(pToDeleteList->data == NULL, END_FUNC);

	pItem->list = HLIB_LIST_Remove(pItem->list, pToDeleteList->data);

	hErr = ERR_OK;

END_FUNC:
	HxSEMT_Release(gDataMgr->ulWriteSema);

	return hErr;
}

HUINT32 ipepg_GetItemValue(IPEPG_Item_e eItem)
{
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
#else
	if(eItem == eIPEPG_ITEM_HDD_STATUS)
	{
		return TRUE;
	}
#endif

	if(gDataMgr != NULL )
	{
		IPEPG_ItemInfo_t *pItem = NULL;
		HUINT32 ulValue = 0;
		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pIpEPG[eItem];

		if(pItem->eItemType == eIPEPG_DATATYPE_BOOL)
		{
			ulValue = (HUINT32)((pItem->itemValue == 0)? FALSE : TRUE);
		}
		else if(pItem->eItemType == eIPEPG_DATATYPE_VALUE)
		{
			ulValue = pItem->itemValue;
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
		return ulValue;
	}

	return FALSE;
}

void		IPEPG_SetItem(IPEPG_Item_e eItem, void *pData)
{
	ipepg_SetItem(eItem, pData);

}
void		*IPEPG_GetItemPoint(IPEPG_Item_e eItem)
{
	return ipepg_GetItemPoint(eItem);

}
HUINT32		IPEPG_GetItemValue(IPEPG_Item_e eItem)
{
	return ipepg_GetItemValue(eItem);

}

HERROR		IPEPG_AddItemToPoint (IPEPG_Item_e eItem, void *userItem)
{
	return ipepg_AddItemToPoint(eItem, userItem);
}

HERROR		IPEPG_DeleteItemFromPoint (IPEPG_Item_e eItem, void *userItem)
{
	return ipepg_DeleteItemFromPoint(eItem, userItem);
}

void		IPEPG_SetAPKDBItem(IPEPG_Item_e eItem, void *pData)
{
	ipepg_SetAPKDBItem(eItem, pData);
}

static HERROR	ipepg_SendCommond(IPEPG_MSG_e message)
{
	return IPEPG_SendCommand(message);
}
#define __DATA_GET_FROM_OCTO_________________________________________________________________________

STATIC void ipepg_ScheduleListener (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule)
{
	HxLOG_Trace();

	HxLOG_Debug("Type[%d] ulSlot[%d] tid(%d)\n", eMsgType, ulSlot, HLIB_STD_GetTID());
	switch (eMsgType)
	{
		case eAPK_METASCH_MSG_TimeUpReady:
			if (pstSchedule != NULL)
			{
				if (pstSchedule->eRsvType == DxRSVTYPE_IP_EPG_GATHERING)
				{
					HxLOG_Debug("DxRSVTYPE_IP_EPG_GATHERING START\n");
					IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
					IPEPG_SetTimer(10);
				}
				else if (pstSchedule->eRsvType == DxRSVTYPE_IP_REMOTE_PROGRAMMING)
				{
					HxLOG_Debug("eIPEPG_MSG_START_RSV_DOWNLOAD START\n");
					ipepg_SendCommond(eIPEPG_MSG_START_RSV_DOWNLOAD);
				}
			}
			break;

		case eAPK_METASCH_MSG_SchedulesUpdated:
			ipepg_SetItem(eIPEPG_ITEM_SCHEDULE_LOADED, (void *)TRUE);
			break;
		default:
			break;

	}
	HxLOG_Debug("%s is OK tid(%d)\n", __FUNCTION__, HLIB_STD_GetTID());
}

void IPEPG_GetScheduleInfoFromOcto(void)
{
	UNIXTIME		 utStartTime, utSystemTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	DxSchedule_t	 stDxSched;
	HERROR			 hErr;

	HxLOG_Trace();
	while(ipepg_GetItemValue(eIPEPG_ITEM_SCHEDULE_LOADED) == FALSE)	// If it would't be loaded the schedule, It must wait.
	{
		HLIB_STD_TaskSleep(1000);

	}
	hErr = APK_META_SCHEDULE_GetScheduleByType(DxRSVTYPE_IP_EPG_GATHERING, 0, &stDxSched);
	if (ERR_OK == hErr)
	{
		HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stDxSched.stStartTime), &utStartTime);
#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
		HxLOG_Print("ipepgSched->startTime[%x] ulSystemTime[%x] diff[%d]\n", utStartTime, utSystemTime, utStartTime - utSystemTime);
#endif

		if (utStartTime > utSystemTime)					// schedule 시간이 지나지 않으면 schedule을 기다린다.
		{
			HxLOG_Debug("Ipepg Schedule in the future. wait (%d) \n ",utStartTime-utSystemTime);
			ipepg_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)utStartTime);
			// Next Update Time을 가지고 온다.
			// download 완료후에 timer를 setting 한 값이다.
		}
		else	// schedule 시간이 지나간경우 다시 받아야 한다. // 좋지 않은 경우임.
		{
			HxLOG_Print("ipepgSched is already past\n");
			ipepg_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
			IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
		}
	}
	else
	{
		// Service list 가 새롭게 update 되어지면 ipepg 를 다시 받도록 시간을 초기화 시킨다.	/* SVCUID 변경시 */
		HxLOG_Print("ipepgSched is NULL\n");
		ipepg_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
		ipepg_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
		IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
	}

	hErr = APK_META_SCHEDULE_GetScheduleByType(DxRSVTYPE_IP_REMOTE_PROGRAMMING, 0, &stDxSched);
	if (ERR_OK == hErr)
	{
		HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stDxSched.stStartTime), &utStartTime);

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
		HxLOG_Print("rpSched->startTime[%x] ulSystemTime[%x] diff[%d]\n", utStartTime, utSystemTime, utStartTime - utSystemTime);
#endif
		ipepg_SetItem(eIPEPG_ITEM_RP_UPDATE_TIME, (void *)utStartTime);
	}
	else
	{
		ipepg_SetItem(eIPEPG_ITEM_RP_UPDATE_TIME, (void *)0);
	}
}

inline HINT32 GetItemValue(const HCHAR *key)
{
	HINT32	nValue;

	if(APK_DB_GetInt(key, &nValue) == ERR_FAIL)
	{
		HxLOG_Print("GetItemValue FAIL key[%s]\n", key);
		return 0;
	}

	return nValue;
}

inline HERROR GetItemStr(const HCHAR *key, HCHAR *pszString, HINT32 nMax)
{
	if(APK_DB_GetStr(key, pszString, nMax) == ERR_FAIL)
	{
		HxLOG_Print("GetItemStr Fail\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

void IPEPG_GetUdbInfoFromWebApp(void)
{
	HxLOG_Trace();
	ipepg_SetItem(eIPEPG_ITEM_FLAG_IPEPG_ONOFF,		(void *)GetItemValue(USERDB_IPEPG_ONOFF));
	ipepg_SetItem(eIPEPG_ITEM_FLAG_RP_ONOFF,		(void *)GetItemValue(USERDB_RP_ONOFF));

	ipepg_SetItem(eIPEPG_ITEM_RP_UPDATE_INTERVAL,	(void *)GetItemValue(USERDB_RP_INTERVAL));
	ipepg_SetItem(eIPEPG_ITEM_RP_PERIOD_START,		(void *)GetItemValue(USERDB_RP_PERIOD_START));
	ipepg_SetItem(eIPEPG_ITEM_RP_PERIOD_END,		(void *)GetItemValue(USERDB_RP_PERIOD_END));
	ipepg_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME,		(void *)GetItemValue(USERDB_IPEPG_LASTUPDATETIME));
	ipepg_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS,		(void *)GetItemValue(USERDB_IPEPG_DOWNLOADPROGRESS));
	ipepg_SetItem(eIPEPG_ITEM_SVC_COUNT,			(void *)GetItemValue(USERDB_IPEPG_SVC_COUNT));
	ipepg_SetItem(eIPEPG_ITEM_EVT_COUNT,			(void *)GetItemValue(USERDB_IPEPG_EVT_COUNT));
	ipepg_SetItem(eIPEPG_ITEM_STAMP_TIME,			(void *)GetItemValue(USERDB_IPEPG_STAMPTIME));
	ipepg_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED,		(void *)GetItemValue(USERDB_IPEPG_SVCLISTUPDATED));

	{
		HCHAR szPPVer[IPEPG_MAX_PPVER_LEN];
		HERROR eErr = ERR_FAIL;

		HxSTR_Memset(szPPVer,0x00,IPEPG_MAX_PPVER_LEN);
		eErr = GetItemStr(USERDB_PP_VER, szPPVer, IPEPG_MAX_PPVER_LEN);
		if(eErr == ERR_OK)
		{
			ipepg_SetItem(eIPEPG_ITEM_PRIVACY_POLICY_VER,szPPVer);
		}
	}

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	{
		HxDATETIME_t stStartTime, stStampTime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ipepg_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME), &stStartTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ipepg_GetItemValue(eIPEPG_ITEM_STAMP_TIME), &stStampTime);

		HxLOG_Debug(
			"IPEPG UDB::\n"
			"\t         bIpEpgEnabled : (%s)\n"
			"\t            bRpEnabled : (%s)\n"
			"\t    ulRpUpdateInterval : %d sec\n"
			"\t       ulRpPeriodStart : %d : %d \n"
			"\t         ulRpPeriodEnd : %d : %d \n"
			"\t      ulLastUpdateTime :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t           ulStampTime :[%04d-%02d-%02d %02d:%02d:%02d] UNIX[%x]\n"
			"\t       bServiceUpdated : %s\n"
			, ipepg_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF)?"On":"Off", ipepg_GetItemValue(eIPEPG_ITEM_FLAG_RP_ONOFF)?"On":"Off"
			, ipepg_GetItemValue(eIPEPG_ITEM_RP_UPDATE_INTERVAL)
			, ipepg_GetItemValue(eIPEPG_ITEM_RP_PERIOD_START)/3600, (ipepg_GetItemValue(eIPEPG_ITEM_RP_PERIOD_START)/60)%60
			, ipepg_GetItemValue(eIPEPG_ITEM_RP_PERIOD_END)/3600, (ipepg_GetItemValue(eIPEPG_ITEM_RP_PERIOD_END)/60)%60
			, stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay , stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond
			, ipepg_GetItemValue(eIPEPG_ITEM_LAST_UPDATE_TIME)
			, stStampTime.stDate.usYear, stStampTime.stDate.ucMonth, stStampTime.stDate.ucDay , stStampTime.stTime.ucHour, stStampTime.stTime.ucMinute, stStampTime.stTime.ucSecond
			, ipepg_GetItemValue(eIPEPG_ITEM_STAMP_TIME)
			, ipepg_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED)?"True":"False"
		);

	}
#endif
}

void IPEPG_GetSystemInfoFromOcto(void)
{
	HCHAR	szSystemId[32];
	HCHAR	szSWVersion[32];
	HERROR	err;
	HUINT32	ulRetrycount = 0;
	while(ulRetrycount++ < 10)
	{
		if(APK_SYSINFO_STB_Initialized() == ERR_OK)
		{
			err =  APK_SYSINFO_STB_GetSystemId(szSystemId, 32);
			err |= APK_SYSINFO_STB_GetSoftwareVersion(szSWVersion, 32);

			if(err != ERR_FAIL)
			{
				HUINT32 ulSystemId	= 0;
				HUINT32 ulSystemVersion = 0;
				HUINT8	szTemp[3];
				HUINT32 tempNum = 0;

				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSystemId[0];
				szTemp[1] = szSystemId[1];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemId |= ((tempNum<<24) &  0xFF000000);
				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSystemId[2];
				szTemp[1] = szSystemId[3];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemId |= ((tempNum<<16) &  0x00FF0000);

				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSystemId[5];
				szTemp[1] = szSystemId[6];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemId |= ((tempNum<<8) &  0x0000FF00);

				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSystemId[7];
				szTemp[1] = szSystemId[8];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemId |= ((tempNum) &  0x000000FF);


				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSWVersion[7];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemVersion |= ((tempNum<<16) &  0x00FF0000);

				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSWVersion[9];
				szTemp[1] = szSWVersion[10];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemVersion |= ((tempNum<<8) &  0x0000FF00);

				HxSTD_MemSet(szTemp,0,3);
				szTemp[0] = szSWVersion[12];
				szTemp[1] = szSWVersion[13];
				tempNum = strtol(szTemp, NULL, 16);
				ulSystemVersion |= ((tempNum) &  0x000000FF);

				HxLOG_Debug("id: %s version : %s  id: %x ver : %x\n\n\n\n",szSystemId, szSWVersion, ulSystemId, ulSystemVersion);
				IPEPG_SetItem(eIPEPG_ITEM_SYS_ID,(void *)ulSystemId);
				IPEPG_SetItem(eIPEPG_ITEM_SYS_VER,(void *)ulSystemVersion);
				return;
			}
			else
			{
				HLIB_STD_TaskSleep(1000);
			}
		}
		else
		{
			HLIB_STD_TaskSleep(1000);
		}
	}
}

void IPEPG_GetMacAddressFromOcto(void)
{
	APKS_NETDEVINFO_t * stDevInfo = NULL;

	HUINT8 szMacStr[IPEPG_MACADDRESS_MAX_LEN];
	HINT32 nIndex = -1;
	HUINT32 ulRetrycount = 0;
	HxLOG_Trace();

	while(ulRetrycount++ < 10)
	{
		if(APK_NETWORK_CONF_GetMacAddr(0, szMacStr, IPEPG_MACADDRESS_MAX_LEN) == ERR_OK)
		{
			break;
		}
		else
		{
			// 정상 적인 Mac Address 값이 입력 되지 않으면 start 하지 않는다.
			// 이미 NAPI Agent 에서 30초 대기 시간이 존재하므로 이 케이스에 들어오는 경우는 거의 없을 것 같음.. ^^
			HxLOG_Debug("[IPEPG] ====================================== Mac Address is not ready\n");
			HLIB_STD_TaskSleep(1000);
		}
	}
	HxLOG_Debug("[%s] Mac Address Received!! [%s]\n", __FUNCTION__, szMacStr);
	IPEPG_SetItem(eIPEPG_ITEM_MAC,(void *)szMacStr);

	APK_NETWORK_CONF_GetDefaultDev(&nIndex);
	if(nIndex != -1)
	{
		HERROR		 hErr;
		stDevInfo = (APKS_NETDEVINFO_t * )IPEPG_MemAlloc(sizeof(APKS_NETDEVINFO_t));
		if (stDevInfo == NULL)
		{
			HxLOG_Error("IPEPG_MemAlloc failed. \n");
			return;
		}
		hErr = APK_NETWORK_CONF_GetDevInfo(nIndex, stDevInfo);
		if (ERR_OK == hErr)
		{
			HxLOG_Debug("[%s] devIndex[%d] bConnected[%d], bNetActive[%d], bRunning[%d]\n", __FUNCTION__, nIndex, stDevInfo->bConnected, stDevInfo->bNetActive, stDevInfo->bRunning);
#if defined(CONFIG_DEBUG)
			if(stDevInfo->bConnected/* && stDevInfo->bNetActive */)
#else
			if(stDevInfo->bConnected && stDevInfo->bNetActive)
#endif
			{
				ipepg_SetItem(eIPEPG_ITEM_NETWORK, (void *)TRUE);
			}
			else
			{
				ipepg_SetItem(eIPEPG_ITEM_NETWORK, (void *)FALSE);
			}
		}
		IPEPG_MemFree(stDevInfo);
	}
}

#if defined(CONFIG_DEBUG)
void ipepg_DEVINFO_Print(DxStorage_Info_t *info)
{
	HINT32	i;

	HxLOG_Print("================ Storage Info ================ \n");
	HxLOG_Print("ulDeviceId : [%d]\n", info->ulDeviceId);
	HxLOG_Print("DevType : [%s]\n", APK_FILEMGR_STORAGE_GetDevTypeString(info->eDevType));
	HxLOG_Print("szMountPath : [%s]\n", info->szMountPath);
	HxLOG_Print("szDevName : [%s] \n", info->szDevName);
	HxLOG_Print("szLabel : [%s]\n", info->szLabel);
	HxLOG_Print("szUUID : [%s]\n", info->szUUID);
	HxLOG_Print("ulFlags : [0x%x]\n", info->ulFlags);
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_CONNECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_MOUNTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_SECURED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_INTERNAL));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_NEEDFORMAT));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_PAIRED));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
	HxLOG_Print("   %s \n", DxSTORAGE_CHECK_FLAG(info->ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));

	HxLOG_Print("\n--------Partition Info--------\n");
	for (i=0; i < info->nPartitionNum ; i++)
	{
		HxLOG_Print("\n   [%d] \n",i);
		HxLOG_Print("   bAvailable : [%d]\n", info->astPartitionInfos[i].bAvailable);
		HxLOG_Print("   FsType: [%s]\n",APK_FILEMGR_STORAGE_GetFsString(info->astPartitionInfos[i].eFsType));
		HxLOG_Print("   nPvrIdx : [%d]\n", info->astPartitionInfos[i].nPvrIdx);
		HxLOG_Print("   szMountPath : [%s]\n", info->astPartitionInfos[i].szMountPath);
		HxLOG_Print("   szTsrPath : [%s]\n", info->astPartitionInfos[i].szTsrPath);
		HxLOG_Print("   szVolumeName : [%s]\n", info->astPartitionInfos[i].szVolumeName);
		HxLOG_Print("   szMediaVideo : [%s]\n", info->astPartitionInfos[i].szMediaVideo);
		HxLOG_Print("   szMediaAudio : [%s]\n", info->astPartitionInfos[i].szMediaAudio);
		HxLOG_Print("   szMediaPic : [%s]\n", info->astPartitionInfos[i].szMediaPic);
		HxLOG_Print("   ulStartUnit : [%d], ulEndUnit : [%d]\n", info->astPartitionInfos[i].ulStartUnit, info->astPartitionInfos[i].ulEndUnit);
		HxLOG_Print("   ulFlags : [0x%x]\n", info->astPartitionInfos[i].ulFlags);
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_MOUNTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_WRITEPROTECTED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SECURED));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_PVRDEVICE));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_DEFAULTHDD));
		HxLOG_Print("      %s \n", DxSTORAGE_CHECK_FLAG(info->astPartitionInfos[i].ulFlags, eDxSTORAGE_FLAG_SUBPVRDEVICE));
	}
	HxLOG_Print("===========================================\n");
}
#endif


void	ipepg_UpdateHddList(void)
{
#define __filter (eDxSTORAGE_FILTER_Internal | eDxSTORAGE_FILTER_Connected | eDxSTORAGE_FILTER_Mounted | eDxSTORAGE_FILTER_PVR | eDxSTORAGE_FILTER_Hdd)

	DxStorage_Info_t * pDevinfoList = NULL;
	HUINT32 ulListCount = 0;

	HxLOG_Trace();

	if (APK_FILEMGR_STORAGE_GetList((HUINT32)__filter, &pDevinfoList, &ulListCount) != ERR_OK) {
		HxLOG_Critical("ipepg_UpdateHddList::APK_FILEMGR_STORAGE_GetList FAIL[%d]\n", __LINE__);
		return;
	}

#if defined(CONFIG_DEBUG)
	{
		HUINT32 i = 0;
		for(i = 0; i < ulListCount ; i++)
		{
			DxStorage_Info_t * pDevinfo = &pDevinfoList[i];
			ipepg_DEVINFO_Print(pDevinfo);
		}
	}
#endif
	APK_FILEMGR_STORAGE_FreeList(pDevinfoList, ulListCount);
	if(ulListCount != 0)
	{
		HxLOG_Print("Internal HDD Connected\n");
		ipepg_SetItem(eIPEPG_ITEM_HDD_STATUS, (void *)TRUE);
	}

#undef __filter

}

void ipepg_CB_HddNotifier(DxStorage_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	// format 이 완료 되고 connected 되면 호출 되어진다.
	HxLOG_Warning("[%s] [%d] [IPEPG] ==== storageInfo updated!!====  \n",__FUNCTION__,__LINE__);
	switch (eType)
	{
		case eDxSTORAGE_NOTI_DevChange:
			{
				HBOOL				bConnect = (HBOOL)nParam1;
				DxStorage_Info_t	*pstInfo = (DxStorage_Info_t	*)nParam2;
				IpEpgMsg_t			stMsg = {0,};

				if (pstInfo->eDevType != eDxSTORAGE_DEVTYPE_INTHDD)
				{
					HxLOG_Debug("Non supported type. type=(%d) \n",pstInfo->eDevType);
					break;
				}

				if (bConnect == TRUE)
				{
					// disconnected -> connected
					if (ipepg_GetItemValue(eIPEPG_ITEM_HDD_STATUS) == FALSE)
					{
						HxLOG_Debug("Internal HDD connected. \n");
						HxLOG_Debug("==> eIPEPG_MSG_START_ALL\n");
						ipepg_SetItem(eIPEPG_ITEM_HDD_STATUS, (void *)TRUE);
					}
					// connected -> connected
					else
					{
						HxLOG_Debug("Hdd status change connected -> connected. \n");
					}
				}
				else
				{
					// connected -> disconnected
					if (ipepg_GetItemValue(eIPEPG_ITEM_HDD_STATUS) == TRUE)
					{
						HxLOG_Debug("Internal HDD disconnected. \n");
						HxLOG_Debug("==> eIPEPG_MSG_STOP\n");
						ipepg_SetItem(eIPEPG_ITEM_HDD_STATUS, (void *)FALSE);
						stMsg.ulMsg = eIPEPG_MSG_STOP;
						stMsg.ulParam1 = eIPEPG_STOPREASON_HDD_OFF;
						IPEPG_SendCommandWithParam(stMsg);
					}
					// disconnected -> disconnected
					else
					{
						HxLOG_Debug("Hdd status change disconnected -> disconnected. \n");
					}
				}
			}
			break;
		default:
			HxLOG_Debug("Not supported noti. noti=(%d) \n",eType);
			break;
	}
}

void IPEPG_AddHddNotifier(void)
{
	APK_FILEMGR_STORAGE_RegisterNotifier(ipepg_CB_HddNotifier);

	while(ipepg_GetItemValue(eIPEPG_ITEM_HDD_STATUS) == FALSE)
	{
		HxLOG_Debug("HDD is Not Mounted\n");
		HLIB_STD_TaskSleep(5*1000);		// HDD 가 init 이 되면 동작 하게 한다. HDD가 mount 가 되어지지 않으면 start 하지 않는다.
		ipepg_UpdateHddList();
	}
}

static void ipepg_NetworkNoti( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HINT32	nDevIndex = ulParam1 ;
	HINT32	nDefaultDevIndex = 0;
	HxLOG_Trace();

	switch (eEvent)
	{
		case eNETWORK_STATUS_CON_SUCCESS :
			ipepg_SetItem(eIPEPG_ITEM_NETWORK, (void *)TRUE);
			HxLOG_Debug("ipepg_NetworkNoti eNETWORK_STATUS_CON_SUCCESS[%d]\n", eEvent);
			if(APK_POWER_IsStandbyMode() != TRUE)
			{
				IPEPG_SetTimer(10);
			}
			break;
		case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED :
		case eNETWORK_STATUS_WIFI_USB_EXTRACTED :
			APK_NETWORK_CONF_GetDefaultDev(&nDefaultDevIndex);
			HxLOG_Debug("ipepg_NetworkNoti deviceid[%d][%d] event[%d]%s\n",nDefaultDevIndex, nDevIndex ,eEvent, (eEvent == eNETWORK_STATUS_CABLE_LINK_DISCONNECTED)?"eNETWORK_STATUS_CABLE_LINK_DISCONNECTED":"eNETWORK_STATUS_WIFI_USB_EXTRACTED");
			if( nDefaultDevIndex == nDevIndex )
			{
				IpEpgMsg_t	stMsg;
				ipepg_SetItem(eIPEPG_ITEM_NETWORK, (void *)FALSE);
				IPEPG_CancelTimer();
				stMsg.ulMsg = eIPEPG_MSG_STOP;
				stMsg.ulParam1 = eIPEPG_STOPREASON_NETWORK;
				IPEPG_SendCommandWithParam(stMsg);
			}
			break;
		default :
			HxLOG_Debug("ipepg_NetworkNoti other[%d]\n", eEvent);
			return;
	}
}

#define __DATA_GET_FROM_DAMA_________________________________________________________________________

static IPEPG_COMP_RESULT_e ipepg_CompareChannelList(HxList_t	* svclist, HxHASH_t *svchash)
{
	IPEPG_COMP_RESULT_e result = eIPEPG_SVC_NO_CHANGED;
	HxList_t * pSvcList = svclist;
	HxHASH_t * pSvcHash = svchash;
	IPEPG_SvcInfo_t *Src = NULL, *Des = NULL;

	HxLOG_Trace();

	if(pSvcList != NULL && svchash == NULL)
	{
		HxLOG_Print("ipepg_CompareChannelList[%d] pSvcList != NULL && svcHash == NULL eIPEPG_SVC_UID_CHANGED\n"	,__LINE__);
		return eIPEPG_SVC_UID_CHANGED;
	}
	else if(pSvcList == NULL && svchash != NULL)
	{
		HxLOG_Print("ipepg_CompareChannelList[%d] pSvcList == NULL && svcHash != NULL eIPEPG_SVC_UID_CHANGED\n"	,__LINE__);
		return eIPEPG_SVC_UID_CHANGED;
	}
	else if(pSvcList == NULL && svchash == NULL)
	{
		HxLOG_Print("ipepg_CompareChannelList[%d] pSvcList == NULL && svcHash == NULL eIPEPG_SVC_NO_CHANGED\n"	,__LINE__);
		return eIPEPG_SVC_NO_CHANGED;
	}

	if(HLIB_LIST_Length(pSvcList) != HLIB_HASH_Size(pSvcHash))
	{
		HxLOG_Print("ipepg_CompareChannelList[%d] New Count[%d] Old Count[%d] eIPEPG_SVC_UID_CHANGED\n"	,__LINE__, HLIB_LIST_Length(pSvcList), HLIB_HASH_Size(pSvcHash));
		return eIPEPG_SVC_UID_CHANGED;
	}

	while(pSvcList)
	{
		Src = HLIB_LIST_Data(pSvcList);

		if(Src)
		{
			Des = (IPEPG_SvcInfo_t*)HLIB_HASH_Lookup(pSvcHash, (void*)Src->ulUid);
			if(Des)
			{
				if (0 != HxSTD_MemCmp(Src, Des, sizeof(IPEPG_SvcInfo_t)))
				{
					if(Src->ulUid != Des->ulUid
						|| Src->stTripleId.usOnId != Des->stTripleId.usOnId
						|| Src->stTripleId.usTsId != Des->stTripleId.usTsId
						|| Src->stTripleId.usSvcId != Des->stTripleId.usSvcId)
					{
						HxLOG_Print("ipepg_CompareChannelList[%d] UID[%d][%d] ONID[%d][%d] TS[%d][%d] SVC[%d][%d] eIPEPG_SVC_UID_CHANGED\n"
							,__LINE__, Src->ulUid, Des->ulUid ,Src->stTripleId.usOnId ,Des->stTripleId.usOnId ,Src->stTripleId.usTsId, Des->stTripleId.usTsId ,Src->stTripleId.usSvcId, Des->stTripleId.usSvcId);
						return eIPEPG_SVC_UID_CHANGED;
					}
					else
					{
						HxLOG_Print("ipepg_CompareChannelList[%d]  SRC LCN[%d] NAME[%s] eIPEPG_SVC_UDATA_CHANGED\n"	,__LINE__, Src->ulLcn, Src->ucName);
						HxLOG_Print("ipepg_CompareChannelList[%d]  DES LCN[%d] NAME[%s] eIPEPG_SVC_UDATA_CHANGED\n\n"	,__LINE__, Des->ulLcn, Des->ucName);
						result = eIPEPG_SVC_UDATA_CHANGED;
					}
				}
			}
			else
			{
				HxLOG_Print("ipepg_CompareChannelList[%d] NO DES SRC UID[%d] LCN[%d] NAME[%s] eIPEPG_SVC_UID_CHANGED\n" ,__LINE__,Src->ulUid, Src->ulLcn, Src->ucName);
				return eIPEPG_SVC_UID_CHANGED;
			}
		}


		pSvcList = pSvcList->next;
		Src = NULL;
		Des = NULL;
	}
	HxLOG_Print("ipepg_CompareChannelList[%d]  RESULT[%d]\n" ,__LINE__, result);
	return result;
}

HxList_t * ipepg_GetServiceFromAPK(void)
{
	HxList_t	* svclist = NULL;
	IPEPG_SvcInfo_t *ipService = NULL;
	DxService_t *service = NULL;
	HxVector_t * apkSvcList = APK_META_SVC_GetServiceList();
	if(apkSvcList == NULL)
		return NULL;

	HxVECTOR_EACH(apkSvcList, DxService_t *, service, {
		if(service != NULL
			&& service->svcid != 0
			&& service->onid != 0
			&& service->tsid != 0
			&& service->lcn != 0
			&& (service->svcType ==eDxSVC_TYPE_TV || service->svcType ==eDxSVC_TYPE_RADIO))
		{
			ipService = (IPEPG_SvcInfo_t *)IPEPG_MemCalloc(sizeof(IPEPG_SvcInfo_t));
			if(ipService != NULL && service != NULL && service->removed != TRUE)
			{
				ipService->stTripleId.usSvcId = service->svcid;
				ipService->stTripleId.usTsId  = service->tsid;
				ipService->stTripleId.usOnId  = service->onid;
				ipService->ulLcn   = service->lcn;
				ipService->ulUid	= service->uid;

				//HxLOG_Debug("TripleID[%08d][%08d][%08d]  [%08d]\n", service->svcid, service->tsid, service->onid, service->lcn);
				HLIB_STD_StrUtf8NCpy(ipService->ucName, service->name, DxNAME_LEN);
				svclist = HLIB_LIST_Append(svclist, ipService);
				ipService = NULL;
			}
			else
				IPEPG_MemFree(ipService);
		}
	});

	HLIB_VECTOR_Delete(apkSvcList);

	HxLOG_Debug("ipepg_GetServiceFromAPK chlist Count[%d]\n",	HLIB_LIST_Length(svclist));
	return svclist;
}

static void	ipepg_SetServiceList (HBOOL IsFirstTime)
{
	HxList_t	* svclist = NULL;
	HxHASH_t * svchash = NULL;
	IPEPG_COMP_RESULT_e eComp = eIPEPG_SVC_NO_CHANGED;
	HCHAR			szMountPath[128];

	HxLOG_Trace();

	svclist = ipepg_GetServiceFromAPK();
	svchash = ipepg_GetItemPoint(eIPEPG_ITEM_SVCLIST);
	eComp = ipepg_CompareChannelList(svclist, svchash);

	if(eComp != eIPEPG_SVC_NO_CHANGED)
	{
		ipepg_SetItem(eIPEPG_ITEM_SVCLIST,(void *)svclist);

		if(IsFirstTime != TRUE)
		{
			HxLOG_Debug("Not First Time Svclist Data from Dama\n");

	 		if(eComp == eIPEPG_SVC_UID_CHANGED)
			{
				HxLOG_Warning("[eIPEPG_MSG_START_ALL : %d]\n", eIPEPG_MSG_START_ALL);
				//uid변경되어...새로 받아야 함...채널로고 수정 필요....
				// delete old Channel Logo data
				HxSTD_snprintf(szMountPath, 128, "%s/.cache", IPEPG_CHANNEL_LOGO_MOUNT_PATH);
				if(HLIB_DIR_IsExist(szMountPath) == TRUE)
				{
					HLIB_DIR_Delete(szMountPath);
				}
				ipepg_SendCommond(eIPEPG_MSG_START_ALL);
			}
			else
			{
				HxLOG_Warning("[eIPEPG_MSG_START_ONLY_SVCLIST : %d]\n", eIPEPG_MSG_START_ONLY_SVCLIST);
				ipepg_SendCommond(eIPEPG_MSG_START_ONLY_SVCLIST);
			}
		}
		else
		{
			HxLOG_Debug("First Time Svclist Data from Dama. Do nothing only save svc list\n");
		}
	}

	if(svclist != NULL)
	{
		HLIB_LIST_Foreach(svclist , IPEPG_MemFree);
		HLIB_LIST_RemoveAll(svclist);
	}

	if(svchash != NULL)
	{
		HLIB_HASH_Destroy(svchash);
	}
}

void ipepg_SvclistAPKNotifier(HINT32 event, const HINT32 *args, void *userdata, HUINT32 ulUpdateByType)
{
	ipepg_SetServiceList(FALSE);

	UNUSED(event);
	UNUSED(args);
	UNUSED(userdata);
	UNUSED(ulUpdateByType);
}

#define __NOTIFY_REGIST_________________________________________________________________________
#define	ToString(type)	#type
static HERROR ipepg_RegistrationOapi(void)
{
	IPEPG_GetSystemInfoFromOcto();
	IPEPG_GetMacAddressFromOcto();
	IPEPG_GetScheduleInfoFromOcto();
	IPEPG_GetUdbInfoFromWebApp();
#if defined(CONFIG_IPEPG_REPOSITORY_HDD)
	IPEPG_AddHddNotifier();
#endif

	return ERR_OK;
}

static void	ipepg_PowerStateChanged(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	APK_POWER_BOXMODE_e		eLastRequestMode = (APK_POWER_BOXMODE_e)ulParam2;

	switch(eLastRequestMode)
	{
		case eAPK_POWER_BOXMODE_STANDBY:
			ipepg_SendCommond(eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI);
			HxLOG_Warning("Going to Shutdown\n");
			break;
		case eAPK_POWER_BOXMODE_REBOOT:
			{
				IpEpgMsg_t	stMsg;

				IPEPG_CancelTimer();
				stMsg.ulMsg		= eIPEPG_MSG_STOP;
				stMsg.ulParam1	= eIPEPG_STOPREASON_REBOOT;
				IPEPG_SendCommandWithParam(stMsg);
				HxLOG_Warning("Going to Reboot\n");
			}
			break;
		default:
			HxLOG_Warning("Last Request Mode: %d\n", eLastRequestMode);
			break;
	}
}

static HERROR	ipepg_onRpScheduled(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	ipepg_SendCommond(eIPEPG_MSG_START_RSV_DOWNLOAD);

	return ERR_OK;
}

static HERROR	ipepg_onIpEpgScheduled(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	IPEPG_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
	IPEPG_SetTimer(10);

	return ERR_OK;
}

static HERROR	ipepg_onIpEpgStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT32	nValue, nOldValue;

	HxLOG_Assert(nArgc == 1);

	nValue = HxOBJECT_INT(apArgv[0]);
	nOldValue = ipepg_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);

	HxLOG_Print("[%s]Set (%d=>%d), IPEPG ON/OFF stampTime(%d)\n", __FUNCTION__, nOldValue, nValue, ipepg_GetItemValue(eIPEPG_ITEM_STAMP_TIME));
	if(nValue != nOldValue)
	{
		ipepg_SetItem(eIPEPG_ITEM_FLAG_IPEPG_ONOFF, (void *)nValue);

		if(nValue)
		{
			if(APK_POWER_IsStandbyMode() != TRUE)
			{
				IPEPG_SetTimer(10);
			}
		}
		else
		{
			IpEpgMsg_t	stMsg;
			IPEPG_CancelTimer();
			stMsg.ulMsg = eIPEPG_MSG_STOP;
			stMsg.ulParam1 = eIPEPG_STOPREASON_ONOFF;
			IPEPG_SendCommandWithParam(stMsg);

			ipepg_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_UPDATE_TIME, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_SVC_COUNT, (void *)0);
			ipepg_SetItem(eIPEPG_ITEM_EVT_COUNT, (void *)0);
			HxLOG_Debug("[%s]Set 0, eIPEPG_ITEM_LAST_UPDATE_TIME,eIPEPG_ITEM_STAMP_TIME (%d)\n", __FUNCTION__, ipepg_GetItemValue(eIPEPG_ITEM_STAMP_TIME));
		}
	}

	return ERR_OK;
}

static HERROR	ipepg_onRpStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT32	nValue;

	HxLOG_Assert(nArgc == 1);

	nValue = HxOBJECT_INT(apArgv[0]);

	if(nValue != 0)
	{
		ipepg_SendCommond(eIPEPG_MSG_START_RSV_DOWNLOAD);
	}
	ipepg_SetItem(eIPEPG_ITEM_FLAG_RP_ONOFF, (void *)nValue);

	return ERR_OK;
}

static HERROR	ipepg_onRpIntervalChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	ipepg_SetItem(eIPEPG_ITEM_RP_UPDATE_INTERVAL, (void *)HxOBJECT_INT(apArgv[0]));
	ipepg_SendCommond(eIPEPG_MSG_START_RSV_DOWNLOAD);

	return ERR_OK;
}

static HERROR	ipepg_onRpPeriodChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 2);

	ipepg_SetItem(eIPEPG_ITEM_RP_PERIOD_START, (void *)HxOBJECT_INT(apArgv[0]));
	ipepg_SetItem(eIPEPG_ITEM_RP_PERIOD_END, (void *)HxOBJECT_INT(apArgv[1]));
	ipepg_SendCommond(eIPEPG_MSG_START_RSV_DOWNLOAD);

	return ERR_OK;
}

static HERROR	ipepg_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	IpEpgMsg_t	stMsg;

	IPEPG_CancelTimer();
	stMsg.ulMsg = eIPEPG_MSG_STOP;
	stMsg.ulParam1 = eIPEPG_STOPREASON_FACTORYRESET;
	IPEPG_SendCommandWithParam(stMsg);

	return ERR_OK;
}

static HERROR	ipepg_onFormatOccurred(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	IpEpgMsg_t	stMsg;

	ipepg_SetItem(eIPEPG_ITEM_HDD_STATUS, (void *)FALSE);
	IPEPG_CancelTimer();
	stMsg.ulMsg = eIPEPG_MSG_STOP;
	stMsg.ulParam1 = eIPEPG_STOPREASON_FORMAT;
	IPEPG_SendCommandWithParam(stMsg);

	return ERR_OK;
}

static HERROR	ipepg_onFvcBaseUrlChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	IPEPG_RETURN_IF(nArgc != 2, ERR_FAIL);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);
	bIsCameFromMe = (IPEPG_StrCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	IPEPG_RETURN_IF(bIsCameFromMe == TRUE, ERR_FAIL);

	ipepg_SetItem(eIPEPG_ITEM_FVC_BASE_URL, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

static HERROR	ipepg_onFvcAuthHashChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	IPEPG_RETURN_IF(nArgc != 2, ERR_FAIL);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);
	bIsCameFromMe = (IPEPG_StrCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	IPEPG_RETURN_IF(bIsCameFromMe == TRUE, ERR_FAIL);

	ipepg_SetItem(eIPEPG_ITEM_FVC_AUTH_HASH, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}
static HERROR	ipepg_onFvcAuthTimeStampChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szCurProcessName = NULL;
	HCHAR	*szSrcProcessName = NULL;
	HBOOL	bIsCameFromMe = FALSE;

	IPEPG_RETURN_IF(nArgc != 2, ERR_FAIL);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	szSrcProcessName = (char*)HxOBJECT_STR(apArgv[1]);
	bIsCameFromMe = (IPEPG_StrCmp(szCurProcessName,szSrcProcessName) == 0) ? TRUE: FALSE;
	IPEPG_RETURN_IF(bIsCameFromMe == TRUE, ERR_FAIL);

	ipepg_SetItem(eIPEPG_ITEM_FVC_AUTH_TIME_STAMP, (char*)HxOBJECT_STR(apArgv[0]));

	return ERR_OK;
}

static HERROR	ipepg_onPPVerChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HCHAR	*szPPVer = NULL;

	IPEPG_RETURN_IF(nArgc != 1, ERR_FAIL);
	szPPVer = (char*)HxOBJECT_STR(apArgv[0]);
	ipepg_SetItem(eIPEPG_ITEM_PRIVACY_POLICY_VER, szPPVer);

	return ERR_OK;
}

static HERROR ipepg_RegistrationHapi(void)
{
	HAPI_ConnectSignal("signal:onIpEPGStateChanged",  NULL, ipepg_onIpEpgStateChanged);
	HAPI_ConnectSignal("signal:onRpStateChanged",     NULL, ipepg_onRpStateChanged);
	HAPI_ConnectSignal("signal:onRpIntervalChanged",  NULL, ipepg_onRpIntervalChanged);
	HAPI_ConnectSignal("signal:onRpPeriodChanged",    NULL, ipepg_onRpPeriodChanged);
	HAPI_ConnectSignal("signal:onFactoryDefault",     NULL, ipepg_onFactoryDefault);
	HAPI_ConnectSignal("signal:onFormatOccurred",     NULL, ipepg_onFormatOccurred);

	HAPI_ConnectSignal("signal:onFvcBaseUrlChanged",     	NULL, ipepg_onFvcBaseUrlChanged);
	HAPI_ConnectSignal("signal:onFvcAuthHashChanged",    	NULL, ipepg_onFvcAuthHashChanged);
	HAPI_ConnectSignal("signal:onFvcAuthTimeStampChanged",  NULL, ipepg_onFvcAuthTimeStampChanged);

	HAPI_ConnectSignal("signal:onPPVerChanged", NULL, ipepg_onPPVerChanged);

	return ERR_OK;
}

#define __SET_gDataMgr_______________________________________________________________________________
IPEPG_SvcInfo_t *IPEPG_GetSvc(HINT32 uid)
{
	IPEPG_SvcInfo_t * svcInfo = NULL, * newSvcInfo = NULL;
	IPEPG_ItemInfo_t *pItem = NULL;

	IPEPG_RETURN_IF(gDataMgr == NULL, NULL);
	HxSEMT_Get(gDataMgr->ulWriteSema);
	pItem = &gDataMgr->pIpEPG[eIPEPG_ITEM_SVCLIST];

	if(pItem->eItemType == eIPEPG_DATATYPE_HASH)
	{
		HxHASH_t * SvcHash = pItem->hash;
		if(SvcHash)
		{
			svcInfo = (IPEPG_SvcInfo_t*)HLIB_HASH_Lookup(SvcHash, (void*)uid);
			if(svcInfo != NULL)
			{
				newSvcInfo = (IPEPG_SvcInfo_t *)IPEPG_MemAlloc(sizeof(IPEPG_SvcInfo_t));
				HxSTD_MemCopy(newSvcInfo, svcInfo, sizeof(IPEPG_SvcInfo_t));
			}
		}
	}
	HxSEMT_Release(gDataMgr->ulWriteSema);

	return newSvcInfo;
}

static void ipepg_ApkEventDispatchTask(void *arg)
{
	while(1)
	{
		if (APK_EVENT_Dispatch() == 0)
			HLIB_STD_TaskSleep(100);
	}
}

void static ipepg_ApkEventDisPatch(void)
{
	HUINT32	ulTimerTaskId = 0;
	VK_TASK_Create(ipepg_ApkEventDispatchTask, VK_TASK_PRIORITY_MW_REF, SIZE_1K, (const char*)"ipepgApkEventTask", (void*)NULL, (unsigned long *)&ulTimerTaskId, 0);
	VK_TASK_Start(ulTimerTaskId);
}
static IPEPG_DATAMGR_t * ipepg_InitDataManager(void)
{
	HxLOG_Trace();

	if(gDataMgr == NULL)
	{
		HUINT32 i = 0;
		gDataMgr = (IPEPG_DATAMGR_t*)IPEPG_MemCalloc(sizeof(IPEPG_DATAMGR_t));
 		gDataMgr->ulWriteSema = 0;
		HxSEMT_Create(&(gDataMgr->ulWriteSema), "DataSema", HxSEMT_FIFO);

 		for(i = 0; i < eIPEPG_ITEM_MAX; i++)
 			HxSTD_MemCopy(&gDataMgr->pIpEPG[i], &gIpepgDefaultItems[i], sizeof(IPEPG_ItemInfo_t));

  	}
  	return gDataMgr;
}

static HBOOL ipepg_ServiceUpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated)
{
	DxService_t *pOld = (DxService_t*)_old;
	DxService_t *pNew = (DxService_t*)_new;

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
				|| (STR_UPDATED(name) && pNew->renamed)
				|| INT_UPDATED(removed)
			) {
				*updated = TRUE;

				HxLOG_Warning("CH DB Update Check [%d%d%d%d:%d%d%d%d:%d%d%d%d]\n",
								INT_UPDATED(uid)
								,INT_UPDATED(tsuid)
								,INT_UPDATED(prvuid)
								,INT_UPDATED(antuid)
								,MEM_UPDATED(grpuids, (sizeof(HINT32) * DxGROUP_LEN))
								,MEM_UPDATED(bqtuids, (sizeof(HINT32) * DxBOUQUET_LEN))
								,INT_UPDATED(svcid)
								,INT_UPDATED(tsid)
								,INT_UPDATED(onid)
								,INT_UPDATED(lcn)
								,(STR_UPDATED(name) && pNew->renamed)
								,INT_UPDATED(removed)
			);

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

	return TRUE;
}


/**************************************************************
	public functions
 **************************************************************/
HINT32	main(HINT32 argc, char ** argv)
{
	IPEPG_DATAMGR_t * pstData = NULL;
	HUINT32			ulSystemTime = 0;
	HCHAR			szMountPath[128];

	HLIB_EXTRA_DropRootPrivileges("ipepg");

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	HLIB_DBG_Init();
#endif

	VK_Init();

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	signal(SIGPIPE, SIG_IGN);
	HxLOG_Info("IP EPG Start!!!\n");
	pstData = ipepg_InitDataManager();
	if( pstData == NULL)
	{
		HxLOG_Critical("Init Com Manager Fail Fail Fail ipepg_InitDownManager Proc FAil\n");
		return 0;
	}

	HLIB_TRANSPORT_HttpInit();
	ipepg_ApkEventDisPatch();
	APK_DB_Init();
	APK_META_SCHEDULE_RegisterNotifier(ipepg_ScheduleListener, APK_META_SCHEDULE_GetInstance());

	while(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) != ERR_OK)
	{
		// 정상 적인 시간이 입력 되지 않으면 start 하지 않는다.
		HxLOG_Debug("====================================== Time is not ready\n");
		HLIB_STD_TaskSleep(5*1000);
	}

	APK_META_ANTENNA_Init(FALSE);
	APK_META_SVC_Init (FALSE);
	ipepg_SetServiceList (TRUE);
	APK_META_SVC_SetUpdateChecker(ipepg_ServiceUpdateChecker);
	APK_META_SVC_SetListener (ipepg_SvclistAPKNotifier, NULL);

	APK_META_RECORD_Init();
	APK_META_RECORD_EnableLoadEvent();
	APK_META_RECORD_AddNotifier(NULL, NULL);	// to delete apk meta record cache
	APK_SYSINFO_STB_Init();
	APK_NETWORK_Init();
	APK_NETWORK_RegisterNotifier(ipepg_NetworkNoti);
	APK_FILEMGR_STORAGE_Init();
	APK_POWER_Init();
	APK_POWER_RegisterNotifier(ipepg_PowerStateChanged);
	APK_META_QUERY_Init(FALSE);

 	ipepg_RegistrationOapi(); //2 wait until HDD mounted here!!!
	ipepg_RegistrationHapi();

#if defined (IPEPG_VERSION_CHECK)
/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	version을 확인하고, 기존의 IPEPG관련 DB파일들을 초기화(삭제) 한다
	따라서, 반영하고자 하는 모델이 있으면 기본적인 로직을 확인하고 작업할 것
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/
	IPEPG_CheckVersionNRemoveDB();
#endif

	// delete old Epg Thumbnail data
	HxSTD_snprintf(szMountPath, 128, "%s/.cache", IPEPG_EVENT_THUMBNAIL_MOUNT_PATH);
	if(HLIB_DIR_IsExist(szMountPath) == TRUE)
	{
		HLIB_DIR_Delete(szMountPath);
	}

	// trashhod 값을 넘는 logo들은 access 시간이 오래 된 순서대로 삭제 된다. (channel logo, event logo)
	THAPI_CheckCacheBuffer(EPG_HUMAX_MOUNT_PATH);

	/****************** IPEPG PROC START!! *****************/
	IPEPG_StartProc(pstData);
	/*******************************************************/

#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)
	ipepg_command_Init();
#endif
    HxLOG_Info("IP EPG End!!!\n");

	while(1)
	{
		HLIB_STD_TaskSleep(100000);
	}

	return 0;
}

