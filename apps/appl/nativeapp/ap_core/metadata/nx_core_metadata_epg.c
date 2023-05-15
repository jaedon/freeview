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

/******************************************************************************/
/**
 * @file	  		nx_core_metadata_epg.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_metadata_epg.h>
#include <nx_prism_util.h>

static 	NX_Metadata_EPG_Context_t *stMetaDataEPGContext = NULL;

/**************************************************************************************************/
#define ______________Private_prototype__________________________________________
/**************************************************************************************************/
static NX_Metadata_EPG_Context_t *nx_Epg_GetContext(void);
#if defined(CONFIG_DEBUG)
static void	nx_epg_Command_Init(void);
#endif

/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/
static NX_Metadata_EPG_Context_t *nx_Epg_GetContext(void)
{
	if (stMetaDataEPGContext == NULL)
	{
		stMetaDataEPGContext = (NX_Metadata_EPG_Context_t*)NX_APP_Calloc(sizeof(NX_Metadata_EPG_Context_t));
	}

	return stMetaDataEPGContext;
}

static void nx_metadata_Epg_DeleteProgrammes(NX_Programme_t *pstProgrammes)
{
	if (pstProgrammes)
	{
		if(pstProgrammes->event)
			NX_PORT_EVENT_Delete(pstProgrammes->event);
		NX_APP_Free(pstProgrammes);
		pstProgrammes = NULL;
	}
}

static NX_Programme_t *nx_metadata_Epg_NewProgrammes(DxEvent_t *pstEvent)
{
	NX_Programme_t *pstProgrammes = (NX_Programme_t*)NX_APP_Calloc(sizeof(NX_Programme_t));

	HxLOG_Debug("added svc uid : %d \n", pstEvent->svcuid);
	pstProgrammes->svcuid	= pstEvent->svcuid;
	pstProgrammes->event 	= pstEvent;

	return pstProgrammes;
}

static void 	nx_metadata_Epg_MonitorCB (HxVector_t *pstDataList)
{
	NX_Metadata_EPG_Context_t *pstContext = nx_Epg_GetContext();
	HINT32  nResultNum = 0,	i = 0;

	nResultNum = HLIB_VECTOR_Length(pstDataList);

	HxLOG_Debug("search result num : %d\n", nResultNum);

	if (nResultNum > 0 )
	{
		// 이전 데이터를 먼저 지우고..
		if (pstContext->resultList)
		{
			for (i = 0; i < HLIB_VECTOR_Length(pstContext->resultList); i++)
			{
				NX_Programme_t *programme = (NX_Programme_t *)HLIB_VECTOR_ItemAt(pstContext->resultList, i);
				nx_metadata_Epg_DeleteProgrammes(programme);
			}
			HLIB_VECTOR_Delete(pstContext->resultList);
			pstContext->resultList = NULL;
		}

		pstContext->nCurProgrammesNum	= nResultNum;
		pstContext->resultList = HLIB_VECTOR_New(NULL, (HINT32 (*)(const void *, const void *))HLIB_VECTOR_CompareHINT32);

		for (i = 0; i < nResultNum; i++)
		{
			DxEvent_t *pstEvent = (DxEvent_t*)HLIB_VECTOR_ItemAt(pstDataList, i);
			if (pstEvent )
			{
				NX_Programme_t *item = (NX_Programme_t*)nx_metadata_Epg_NewProgrammes(NX_PORT_EVENT_Incref(pstEvent));
				HLIB_VECTOR_Add(pstContext->resultList, item);
				HxLOG_Debug("received epg data : index(%d), svcuid : (%d) \n", i, pstEvent->svcuid);
			}
		}

		ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES, 0, 0, 0, 0);
	}
	else
	{
		while(pstContext->nCurProgrammesNum--)
		{

		}
		pstContext->nCurProgrammesNum = 0;
	}
}


NX_ProgrammeItem_t * nx_metadata_epg_DumpProgrammeItem(NX_Programme_t *srcItem)
{
	HCHAR eventName[EPG_HUMAX_NAME_LEN] = {'\0',};
	HxDATETIME_t datetime, durationtime;
	HUINT32 weekIndex = 0;
	NX_ProgrammeItem_t *addItem = (NX_ProgrammeItem_t*)NX_APP_Calloc(sizeof(NX_ProgrammeItem_t));
#if defined(CONFIG_PROD_LANG_ARABIC)
	HINT32	currentLanguage = eLangID_NONE;
#endif

	// -------- set item
	addItem->startTime	= NX_METADATA_PROGRAMMES_GetStartTime(srcItem);
	addItem->duration	= NX_METADATA_PROGRAMMES_GetDuration(srcItem);

	// event name
	if (NX_METADATA_PROGRAMMES_GetName(srcItem, eventName) == ERR_OK)
	{
		HxSTD_MemSet(addItem->pszEventName, 0, EPG_HUMAX_NAME_LEN);
		HLIB_STD_StrUtf8NCpy(addItem->pszEventName, eventName, EPG_HUMAX_NAME_LEN);
	}

	addItem->eventId	= NX_METADATA_PROGRAMMES_GetEventID(srcItem);
	addItem->svcUid	= NX_METADATA_PROGRAMMES_GetSvcUID(srcItem);

	// pszduration
	HLIB_DATETIME_ConvertUnixTimeToDateTime(addItem->startTime, &datetime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(
							addItem->startTime +
							addItem->duration, &durationtime);
	weekIndex = HLIB_DATETIME_GetWeekDayIndex(datetime.stDate);

#if defined(CONFIG_PROD_LANG_ARABIC)
	NX_SYSTEM_GetMenuLanuage(&currentLanguage);
	if(currentLanguage == eLangID_Arabic)
	{
		HxSTD_sprintf((HCHAR*)addItem->pszDuration,
				(HCHAR*)".%02d.%02d%s %02d:%02d - %02d:%02d / ",
				datetime.stDate.ucDay, datetime.stDate.ucMonth,
				ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
				datetime.stTime.ucHour, datetime.stTime.ucMinute,
				durationtime.stTime.ucHour, durationtime.stTime.ucMinute);
	}
	else
#endif
	{
		HxSTD_sprintf((HCHAR*)addItem->pszDuration,
				(HCHAR*)"%02d:%02d - %02d:%02d / %s.%02d.%02d",
				datetime.stTime.ucHour, datetime.stTime.ucMinute,
				durationtime.stTime.ucHour, durationtime.stTime.ucMinute,
				ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(weekIndex)),
				datetime.stDate.ucDay, datetime.stDate.ucMonth);
	}

	addItem->bFocus = FALSE;
	addItem->bNoProgramme = FALSE;

	return addItem;

}


/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR NX_METADATA_EPG_Init(void)
{

#if defined(CONFIG_DEBUG)
	nx_epg_Command_Init();
#endif


	return ERR_OK;
}

HERROR	NX_METADATA_EPG_SearchEPG(NX_MetadataSearch_t *pstMetaSearch)
{
	if (pstMetaSearch == NULL)
	{
		HxLOG_Debug("\n pstMetaSearch Data is NULL!\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("\n Call NX_METADATA_EPG_SearchEPG!\n");
		NX_METADATA_SEARCH_SearchEPG(pstMetaSearch, nx_metadata_Epg_MonitorCB, NULL);
	}

	return ERR_OK;
}

HERROR	NX_METADATA_EPG_GetResultProgrammeBySvcUid(HUINT32 svcuid, HxVector_t *resultList, HINT32 *nNum)
{
	HINT32 i = 0, length = 0, lIndex = 0;
	NX_Metadata_EPG_Context_t *pstContext = nx_Epg_GetContext();

	if (pstContext->resultList == NULL)
	{
		*nNum = 0;
		HxLOG_Error("epg data is NULL....!!\n");
		return	ERR_FAIL;
	}

	length = HLIB_VECTOR_Length(pstContext->resultList);
	for (i = 0; i < length; i++)
	{
		NX_Programme_t *item = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstContext->resultList, i);

		if ((item != NULL) && (item->svcuid == svcuid))
		{
			if (resultList)
				HLIB_VECTOR_Add(resultList, item);
			lIndex++;
		}

	}
	*nNum = lIndex;

	return ERR_OK;
}


HERROR	NX_METADATA_EPG_GetResultProgrammeBySvcUidAndEventId(HUINT32 ulSvcUid, HINT32 lEventId, HxVector_t *resultList, HINT32 *nNum)
{
	HINT32 i = 0, length = 0, lIndex = 0;
	NX_Metadata_EPG_Context_t *pstContext = nx_Epg_GetContext();

	if (pstContext->resultList == NULL)
	{
		*nNum = 0;
		HxLOG_Error("epg data is NULL....!!\n");
		return	ERR_FAIL;
	}

	length = HLIB_VECTOR_Length(pstContext->resultList);
	for (i = 0; i < length; i++)
	{
		NX_Programme_t *item = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstContext->resultList, i);

		if ((item != NULL) && (item->svcuid == ulSvcUid) && (item->event->eventId == lEventId))
		{
			if (resultList)
			{
				HLIB_VECTOR_Add(resultList, item);
			}
			lIndex++;
			break;
		}
	}
	*nNum = lIndex;

	return ERR_OK;
}


HERROR	NX_METADATA_EPG_GetResultProgrammeItemBySvcUid(HUINT32 svcuid, HxVector_t *resultList, HINT32 *nNum)
{
	HINT32 i = 0, length = 0, lIndex = 0;
	NX_Metadata_EPG_Context_t *pstContext = nx_Epg_GetContext();

	if (pstContext->resultList == NULL)
	{
		*nNum = 0;
		HxLOG_Error("epg data is NULL....!!\n");
		return	ERR_FAIL;
	}

	length = HLIB_VECTOR_Length(pstContext->resultList);
	for (i = 0; i < length; i++)
	{
		NX_Programme_t *item = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstContext->resultList, i);

		if ((item != NULL) && (item->svcuid == svcuid))
		{
			if (resultList)
			{
				HLIB_VECTOR_Add(resultList, nx_metadata_epg_DumpProgrammeItem(item));
			}
			lIndex++;
		}

	}
	*nNum = lIndex;

	return ERR_OK;
}

HERROR	NX_METADATA_EPG_GetResultProgrammeBySeriesId(HUINT32 ulSvcUid, HUINT32 ulSeriesId, HUINT32 ulTime, HxVector_t *stResultList, HINT32 *lNum)
{
	HINT32				i = 0, length = 0, lIndex = 0;
	HUINT32				ulEventSeriesId = 0, ulEvtEndTime = 0;
	DxEPG_Type_e		eEpgType = eDxEPG_TYPE_NONE;
	NX_Programme_t		*stItem = NULL;
	NX_Metadata_EPG_Context_t	*pstContext = nx_Epg_GetContext();

	if (pstContext->resultList == NULL)
	{
		*lNum = 0;
		HxLOG_Error("Error!!! epg data is NULL....!!\n");
		return	ERR_FAIL;
	}

	if ((ulSeriesId == 0) || (ulTime == 0))
	{
		*lNum = 0;
		HxLOG_Error("Error!! Pls Check ulSeriesId & ulTime ....!!\n");
		return	ERR_FAIL;
	}

	length = HLIB_VECTOR_Length(pstContext->resultList);
	for (i = 0; i < length; i++)
	{
		stItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstContext->resultList, i);
		if (stItem == NULL)
		{
			NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() Fail!! Index:(%d)\n", i);
			continue;
		}

		eEpgType = stItem->event->type;
		ulEvtEndTime = (stItem->event->startTime + stItem->event->duration);
		ulEventSeriesId = 0;
		if (eEpgType == eDxEPG_TYPE_IP)
		{
			ulEventSeriesId = stItem->event->extension.ipEpg.seriesId;
		}
#if defined(CONFIG_MW_EPG_TVTV)
		if (eEpgType == eDxEPG_TYPE_TVTV)
		{
			ulEventSeriesId = stItem->event.extension.tvtvEpg.seriesId;
		}
#endif

		if ((stItem != NULL) && (stItem->svcuid == ulSvcUid) && (ulEventSeriesId == ulSeriesId) && (ulEvtEndTime > ulTime))
		{
			if (stResultList)
				HLIB_VECTOR_Add(stResultList, stItem);
			lIndex++;
		}

	}
	*lNum = lIndex;

	return ERR_OK;
}


HERROR	NX_METADATA_EPG_GetResultProgrammeBySvcUidAndTripleId(HINT32 lEventId, HUINT32 ulSvcUid, HINT32 lSvcId, HINT32 lTsId, HINT32 lOnId, HxVector_t *stResultList, HINT32 *lNum)
{
	HINT32				i = 0, length = 0, lIndex = 0;
	NX_Programme_t		*stItem = NULL;
	NX_Metadata_EPG_Context_t *pstContext = nx_Epg_GetContext();

	if (pstContext->resultList == NULL)
	{
		*lNum = 0;
		HxLOG_Error("Error!!! epg data is NULL....!!\n");
		return	ERR_FAIL;
	}

	length = HLIB_VECTOR_Length(pstContext->resultList);
	for (i = 0; i < length; i++)
	{
		stItem = (NX_Programme_t*)HLIB_VECTOR_ItemAt(pstContext->resultList, i);
		if ((stItem != NULL) && (stItem->svcuid == ulSvcUid) &&
			(stItem->event->svcid == lSvcId) && (stItem->event->tsid == lTsId) &&
			(stItem->event->onid == lOnId) && (stItem->event->eventId == lEventId))
		{
			if (stResultList)
				HLIB_VECTOR_Add(stResultList, stItem);
			lIndex++;
		}

	}
	*lNum = lIndex;

	return ERR_OK;
}



#define ______________RELATED_EPG_COMMAND____________________
/***************************************************************************
Related PF Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static void nx_epg_Command_Init(void)
{
#define cmdHandle		"nxepgcmd"


}

#endif		// #if defined(CONFIG_DEBUG)



