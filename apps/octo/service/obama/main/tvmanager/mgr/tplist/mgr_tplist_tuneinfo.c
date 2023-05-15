/**
	@file     mgr_search_tuneinfo.c
	@brief

	Description :			\n
	Module :				\n
	Remarks :				 \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/07/16		init					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#include <db_svc.h>
#include <db_param.h>
#include <linkedlist.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <mgr_search.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC POINTER_LIST_T		*s_pstSearchTuneInfo = NULL;

/*******************************************************************/
/********************      Function Prototypes  ********************/
/*******************************************************************/
STATIC DbSvc_TuningInfo_t *mgr_tplist_GetFirstTuneInfo (void);
STATIC HERROR mgr_tplist_RemoveFirstTuneInfo (HBOOL bFreeContents);
STATIC DbSvc_TuningInfo_t *mgr_tplist_AllocTuneInfo (void);
STATIC void mgr_tplist_FreeTuneInfo (void *pvTuneInfo);

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


STATIC DbSvc_TuningInfo_t *mgr_tplist_AllocTuneInfo (void)
{
	return (DbSvc_TuningInfo_t *)OxAP_Malloc (sizeof(DbSvc_TuningInfo_t));
}

STATIC void mgr_tplist_FreeTuneInfo (void *pvTuneInfo)
{
	if (pvTuneInfo)
	{
		OxAP_Free ((DbSvc_TuningInfo_t *)pvTuneInfo);
	}
}

STATIC DbSvc_TuningInfo_t *mgr_tplist_GetFirstTuneInfo (void)
{
	if (s_pstSearchTuneInfo != NULL)
	{
		return (DbSvc_TuningInfo_t *)s_pstSearchTuneInfo->pvContents;
	}

	return NULL;
}

STATIC HERROR mgr_tplist_RemoveFirstTuneInfo (HBOOL bFreeContents)
{
	FREE_FUNC		 pfnMemFree;

	pfnMemFree = (bFreeContents == TRUE) ? mgr_tplist_FreeTuneInfo : NULL;
	s_pstSearchTuneInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (s_pstSearchTuneInfo, s_pstSearchTuneInfo, pfnMemFree);
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_TPLIST_GetFirstTuneInfo (DbSvc_TuningInfo_t **ppstTuneInfo)
{
	if (NULL == ppstTuneInfo)
		return ERR_FAIL;

	*ppstTuneInfo = mgr_tplist_GetFirstTuneInfo ();

	return (NULL != *ppstTuneInfo) ? ERR_OK : ERR_FAIL;
}

HERROR MGR_TPLIST_RemoveFirstTuneInfo (HBOOL bFreeContents)
{
	return mgr_tplist_RemoveFirstTuneInfo (bFreeContents);
}

HERROR MGR_TPLIST_ClearTuneInfo (void)
{
	s_pstSearchTuneInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSearchTuneInfo, mgr_tplist_FreeTuneInfo);
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_TPLIST_AddTuneInfo (DbSvc_TuningInfo_t *pstTuneInfo)
{
	DbSvc_TuningInfo_t	*pstSrchTuneInfo;

	if (pstTuneInfo == NULL)			{ return ERR_BUS_SOURCE_NULL; }

	pstSrchTuneInfo = mgr_tplist_AllocTuneInfo();
	if (pstSrchTuneInfo == NULL)		{ return ERR_BUS_OUT_OF_MEMORY; }

	*pstSrchTuneInfo = *pstTuneInfo;
	s_pstSearchTuneInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSearchTuneInfo, (void *)pstSrchTuneInfo);
	return ERR_BUS_NO_ERROR;
}

