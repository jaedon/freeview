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
 * @file	  		nx_core_metadata_search.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/

#include <nx_core_metadata_search.h>
#include <nx_core_system.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_RETURN_CHECK_PARAM(expr, err)	do{if(expr == NULL){HxLOG_Debug("[%s is NULL!!\n", #expr);return (err);}}while(0)
#define	NX_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_CORE_METADATA_EPG_EIT_MAIN_CONTENTS_NUM					15


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

NX_MetadataSearchContext_t *s_pstMetaSearchCtx = NULL;


/**************************************************************************************************/
#define ______________Private_prototype__________________________________________
/**************************************************************************************************/
#if defined(CONFIG_DEBUG)
static void	nx_metadata_Search_Command_Init(void);
#endif
static NX_MetadataSearchContext_t* 	nx_MetadataSearch_GetContext(void);
static void 	nx_MetadataSearch_PfMonitor (NX_MetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata);
static void		nx_MetadataSearch_StartPFMonitor(NX_MetadataSearch_t *pMetaSearchData, HUINT32 svcuid);
static void 	nx_MetadataSearch_EpgMonitor (NX_MetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata);
static void		nx_MetadataSearch_StartEPGMonitor(NX_MetadataSearch_t *pMetaSearchData);
static void		nx_MetadataSearch_SearchEPG(NX_MetadataSearch_t *pMetaSearchData);

/**************************************************************************************************/
#define _____________Programmes_Prototype_______________________________________
/**************************************************************************************************/
static HxLANG_Id_e nx_metadata_Search_GetMainLanguage(void);
static HBOOL	nx_metadata_Search_FinedEventString(void *data, void *language);


/**************************************************************************************************/
#define ______________Private_functions__________________________________________
/**************************************************************************************************/
static NX_MetadataSearchContext_t* 	nx_MetadataSearch_GetContext(void)
{
	if (s_pstMetaSearchCtx == NULL)
	{
		s_pstMetaSearchCtx = (NX_MetadataSearchContext_t*)NX_APP_Calloc(sizeof(NX_MetadataSearchContext_t));

	}
	return s_pstMetaSearchCtx;
}

static void 	nx_MetadataSearch_PfMonitor (NX_MetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	NX_MetadataSearch_t *pstMetaSearch = (NX_MetadataSearch_t*)userdata;
	HxVector_t			*result = NULL;

	if (pstMetaSearch !=  NULL)
	{
		HxLOG_Debug("psMetaSearch context(0x%X), svcuid(%d) \n", pstMetaSearch->context, pstMetaSearch->monitor.svcuid);
	}
	else
	{
		HxLOG_Debug("pstMetaSearch context is NULL.\n");
	}

	if (event == eNxQUERY_EVENT_NEW_VERSION)
	{
		if(pstMetaSearch)
		{
			nx_MetadataSearch_StartPFMonitor(pstMetaSearch, pstMetaSearch->monitor.svcuid);
		}
	}
	else if (event == eNxQUERY_EVENT_FINISHED)
	{
		if(pstMetaSearch)
		{
			result = NX_PORT_META_QUERY_Result(pstMetaSearch->context);

			if ( (result) && (pstMetaSearch->monitor.callback))
			{
				pstMetaSearch->monitor.callback(pstMetaSearch->monitor.svcuid, result);
				HLIB_VECTOR_Delete(result);
			}

			NX_PORT_META_QUERY_Delete(pstMetaSearch->context);
			NX_APP_Free(pstMetaSearch);
		}
	}

	NX_COMMON_NOT_USED_PARAM(args);
	NX_COMMON_NOT_USED_PARAM(self);
}


static void		nx_MetadataSearch_StartPFMonitor(NX_MetadataSearch_t *pMetaSearchData, HUINT32 svcuid)
{
	NX_QUERY_t query;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	NX_Channel_t	stChannel;
	DxService_t		*pstFindService = NULL;

#endif

	NX_PORT_META_QUERY_Abort(pMetaSearchData->context);

	query = NX_PORT_META_QUERY_NewQuery("endtime", ">=", "0");
	NX_PORT_META_QUERY_SetQuery(pMetaSearchData->context, query);
	NX_PORT_META_QUERY_DeleteQuery(query);

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	NX_PORT_META_QUERY_AddSvcConstraint(pMetaSearchData->context, (HINT32)svcuid);
#else
	pstFindService = NX_CHANNEL_FindServiceBySvcUid(svcuid);
	stChannel = OPL_Channel_FindChannelByUID (NULL, svcuid);
	NX_PORT_META_QUERY_AddSvcConstraint(pMetaSearchData->context, pstFindService->onid, pstFindService->tsid, pstFindService->svcid);
#endif
	NX_PORT_META_QUERY_SetFilter(pMetaSearchData->context, eNxQUERY_FILTER_PF);

	NX_PORT_META_QUERY_Search(pMetaSearchData->context, FALSE);
}

static void 	nx_MetadataSearch_EpgMonitor (NX_MetaQHandle_t self, HINT32 event, const HINT32 *args, void *userdata)
{
	NX_MetadataSearch_t *pstMetaSearch = (NX_MetadataSearch_t*)userdata;
	HxVector_t			*result = NULL;

	if (pstMetaSearch != NULL)
	{
		HxLOG_Debug("psMetaSearch context(0x%X)\n", 	pstMetaSearch->context);
	}
	else
	{
		HxLOG_Error("Error!!! pstMetaSearch context is NULL.\n");
		return;
	}

	if (event == eNxQUERY_EVENT_NEW_VERSION)
	{
		HxLOG_Debug("event is eNxQUERY_EVENT_NEW_VERSION\n");
		//nx_MetadataSearch_StartEPGMonitor(pstMetaSearch);
	}
	else if (event == eNxQUERY_EVENT_FINISHED)
	{
		HxLOG_Debug("event is eNxQUERY_EVENT_FINISHED\n");
		result = NX_PORT_META_QUERY_Result(pstMetaSearch->context);

		if ( (result) && (pstMetaSearch->resultListener))
		{
			pstMetaSearch->resultListener(result);
			HLIB_VECTOR_Delete(result);
		}
		else
		{
			HxLOG_Debug("pstMetaSearch->resultListener (0x%X), result (0x%X)\n", pstMetaSearch->resultListener, result);
		}
		NX_PORT_META_QUERY_Delete(pstMetaSearch->context);
		NX_APP_Free(pstMetaSearch);
	}

	NX_COMMON_NOT_USED_PARAM(args);
	NX_COMMON_NOT_USED_PARAM(self);
}

static void		nx_MetadataSearch_StartEPGMonitor(NX_MetadataSearch_t *pMetaSearchData)
{
	NX_QUERY_t query1, query2, andQuery;
#ifndef CONFIG_FUNC_EPG_USE_SVCUID
	NX_Channel_t	stChannel;
	DxService_t		*pstFindService = NULL;

#endif
	HUINT32 	unixtime = 0;
	HCHAR 		value[64] = {'\0',};

	if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		return;
	}

#if defined(CONFIG_DEBUG)
{
	HxDATETIME_t datetime;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
	HxLOG_Debug("\n end time: %4d.%2d.%2d : %2d:%2d:%2d\n\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond);
}
#endif
	HxSTD_sprintf(value, "%u", unixtime);
	query1 = NX_PORT_META_QUERY_NewQuery("endTime", ">", value);


	unixtime += SECONDS_PER_HOUR * 4; // plus 3 hour
#if defined(CONFIG_DEBUG)
{
	HxDATETIME_t datetime;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &datetime);
	HxLOG_Debug("\n start time: %4d.%2d.%2d : %2d:%2d:%2d\n\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond);
}
#endif
	HxSTD_sprintf(value, "%u", unixtime);
	query2 = NX_PORT_META_QUERY_NewQuery("startTime", "<", value);

	andQuery = NX_PORT_META_QUERY_AND(query1, query2);

	NX_PORT_META_QUERY_DeleteQuery(query1);
	NX_PORT_META_QUERY_DeleteQuery(query2);

	NX_PORT_META_QUERY_SetQuery(pMetaSearchData->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	NX_PORT_META_QUERY_SetFilter(pMetaSearchData->context, eNxQUERY_FILTER_ALL);

	NX_PORT_META_QUERY_Search(pMetaSearchData->context, FALSE);
}

static void		nx_MetadataSearch_SearchEPG(NX_MetadataSearch_t *pMetaSearchData)
{
	NX_PORT_META_QUERY_SetFilter(pMetaSearchData->context, eNxQUERY_FILTER_ALL);

	NX_PORT_META_QUERY_Search(pMetaSearchData->context, FALSE);
}

/**************************************************************************************************/
#define _____________Public_functions____________________________________________
/**************************************************************************************************/
HERROR NX_METADATA_SEARCH_Init(void)
{
	NX_MetadataSearchContext_t *pstContext = NULL;

	pstContext = nx_MetadataSearch_GetContext();

#if defined(CONFIG_DEBUG)
	nx_metadata_Search_Command_Init();
#endif

	return ERR_OK;
}

NX_MetadataSearch_t* NX_METADATA_SEARCH_New(NX_MetadataSearchTarget_e target)
{
	NX_MetadataSearch_t *pstMetaDataSearch = NULL;
	NX_META_QUERY_Target_e	eTarget = eQUERY_TARGET_UNKNOWN;

	switch(target)
	{
	case eNX_META_SEARCH_TARGET_SCHEDULED:	eTarget = eNxQUERY_TARGET_EPG;	break;
	case eNX_META_SEARCH_TARGET_ONDEMAND:	eTarget = eNxQUERY_TARGET_VOD;	break;
	default:
		HxLOG_Error("Unknown Target (%d)!!\n", target);
	}

	pstMetaDataSearch = (NX_MetadataSearch_t*)NX_APP_Calloc(sizeof(NX_MetadataSearch_t) * 2);
	pstMetaDataSearch->context = NX_PORT_META_QUERY_New(eTarget);

	if (pstMetaDataSearch->context == NULL)
	{
		NX_APP_Free(pstMetaDataSearch);
		HxLOG_Error("Out of memory!!\n");
		return NULL;
	}

	// pstMetaSearch 관리 모델을 필요하면 구현해야 하는데... 2015.03.26 mykim
	// .....
	return pstMetaDataSearch;

}

HERROR	NX_METADATA_SEARCH_SearchPF(NX_MetadataSearch_t *pMetaSearchData, HUINT32 svcuid, NX_SearchPfMonitor_t monitor, void *userdata)
{
	if (pMetaSearchData == NULL)
	{
		HxLOG_Error("pMetaSearchData is NULLL\n");
		return ERR_FAIL;
	}
	if (pMetaSearchData)
	{
		pMetaSearchData->monitor.svcuid 	= svcuid;
		pMetaSearchData->monitor.userdata 	= pMetaSearchData;
		pMetaSearchData->monitor.callback	= monitor;

		NX_PORT_META_QUERY_SetNotifier(pMetaSearchData->context, nx_MetadataSearch_PfMonitor, (void *)pMetaSearchData);

		HxLOG_Debug("\n Star tPF MONITOR \n\n");
		nx_MetadataSearch_StartPFMonitor(pMetaSearchData, svcuid);
	}
	else
	{

	}

	NX_COMMON_NOT_USED_PARAM(userdata);
	return ERR_OK;
}

HERROR NX_METADATA_SEARCH_SearchEPG(NX_MetadataSearch_t *pMetaSearchData, NX_SearchResultListener_t listener, void *userdata)
{
	if (pMetaSearchData == NULL)
	{
		HxLOG_Error("pMetaSearchData is NULL\n");
		return ERR_FAIL;
	}

	if (pMetaSearchData)
	{
		pMetaSearchData->resultListener = listener;

		NX_PORT_META_QUERY_SetNotifier(pMetaSearchData->context, nx_MetadataSearch_EpgMonitor, (void *)pMetaSearchData);

		HxLOG_Debug("\n Start EPG MONITOR \n\n");
		nx_MetadataSearch_SearchEPG(pMetaSearchData);
		//nx_MetadataSearch_StartEPGMonitor(pMetaSearchData);
	}
	else
	{

	}
	NX_COMMON_NOT_USED_PARAM(userdata);

	return ERR_OK;
}

HERROR NX_METADATA_SEARCH_AddChannelConstraint(NX_MetadataSearch_t *pMetaSearchData, NX_Channel_t *channel)
{
#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
	NX_Channel_t	stChannel;
	DxService_t 	*pstFindService = NULL;
#endif

	if (pMetaSearchData == NULL)
	{
		HxLOG_Debug("pMetaSearchData is NULL!\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	NX_PORT_META_QUERY_AddSvcConstraint(pMetaSearchData->context, (HINT32)channel->svcUid);
#else
	pstFindService = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);
	stChannel = NX_CHANNEL_FindServiceBySvcUid (channel->svcUid);
	NX_PORT_META_QUERY_AddSvcConstraint(pMetaSearchData->context, pstFindService->onid, pstFindService->tsid, pstFindService->svcid);
#endif
	return ERR_OK;
}


NX_QUERY_t 	NX_METADATA_SEARCH_CreateQuery(const HCHAR *field, const HCHAR *comparison, const HCHAR *value)
{
	NX_QUERY_t 	retQuery = NULL;

	HxLOG_Assert(field);
	HxLOG_Assert(comparison);
	HxLOG_Assert(value);

	retQuery = NX_PORT_META_QUERY_NewQuery(field, comparison, value);

	return retQuery;
}

#define ______________RELATED_METADATA_SEARCH_COMMAND____________________
/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static void nx_metadata_Search_Command_Init(void)
{
#define cmdHandle		"nxmetasearchcmd"


}

#endif		// #if defined(CONFIG_DEBUG)

