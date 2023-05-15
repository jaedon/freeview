/**
	@file     default_ter_info.c
	@brief    file_name & simple comment.

	Description: dimension header file for kraken \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/07/16		init					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

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

STATIC POINTER_LIST_T		*s_pstSrchTpList = NULL;					// Contents : DbSvc_TsInfo_t

/*******************************************************************/
/********************      Functions            ********************/
/*******************************************************************/

STATIC void mgr_tplist_TsInfoFreeTsInfo (void *pvTsInfo);


#define _____INTERNAL_FUNCTIONS_____

STATIC void mgr_tplist_TsInfoFreeTsInfo (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}


#define _____EXTERNAL_FUNCTIONS_____

HERROR MGR_TPLIST_ClearTsInfo (void)
{
	if (s_pstSrchTpList != NULL)
	{
		s_pstSrchTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (s_pstSrchTpList, mgr_tplist_TsInfoFreeTsInfo);
	}

	return ERR_OK;
}

HERROR MGR_TPLIST_AddTsInfo (DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstNewTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_FAIL; }

	pstNewTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstNewTsInfo == NULL)			{ return ERR_FAIL; }

	*pstNewTsInfo = *pstTsInfo;
	s_pstSrchTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (s_pstSrchTpList, (void *)pstNewTsInfo);
	return ERR_OK;
}

HERROR MGR_TPLIST_CountTsInfo (HUINT32 *pulTpNum)
{
	HUINT16		 usCount;

	if (pulTpNum == NULL)				{ return ERR_FAIL; }

	usCount = UTIL_LINKEDLIST_GetNumListItemsInPointerList (s_pstSrchTpList, NULL, NULL);
	if (usCount == 0)
	{
		return ERR_FAIL;
	}

	*pulTpNum = (HUINT32)usCount;
	return ERR_OK;
}

HERROR MGR_TPLIST_GetTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstTsInfoItem;
	POINTER_LIST_T		*pstListItem;

	if (pstTsInfo == NULL)				{ return ERR_FAIL; }

	pstListItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (s_pstSrchTpList, (HUINT16)ulIndex, NULL, NULL);
	if (pstListItem != NULL)
	{
		pstTsInfoItem = (DbSvc_TsInfo_t *)pstListItem->pvContents;
		if (pstTsInfoItem != NULL)
		{
			*pstTsInfo = *pstTsInfoItem;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR MGR_TPLIST_GetTsList (HUINT32 *pulNumTp, DbSvc_TsInfo_t **ppstTsList)
{
	HUINT32				 ulCount, ulNumTp;
	DbSvc_TsInfo_t		*pstTsArray;
	DbSvc_TsInfo_t		*pstTsInfoItem;
	POINTER_LIST_T		*pstListItem;
	HERROR				 hErr;

	if (pulNumTp == NULL || ppstTsList == NULL)		{ return ERR_FAIL; }

	*pulNumTp = 0; *ppstTsList = NULL;

	hErr = MGR_TPLIST_CountTsInfo (&ulNumTp);
	if (hErr != ERR_OK || ulNumTp == 0)					{ return ERR_FAIL; }

	pstTsArray = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulNumTp);
	if (pstTsArray == NULL)								{ return ERR_FAIL; }

	ulCount = 0;
	pstListItem = s_pstSrchTpList;
	while (pstListItem != NULL)
	{
		pstTsInfoItem = (DbSvc_TsInfo_t *)pstListItem->pvContents;
		if (pstTsInfoItem != NULL)
		{
			pstTsArray[ulCount] = *pstTsInfoItem;
			ulCount ++;
		}

		pstListItem = pstListItem->pNext;
		if (pstListItem == s_pstSrchTpList)			{ break; }
	}

	if (ulCount == 0)
	{
		OxAP_Free (pstTsArray);
		return ERR_FAIL;
	}

	*pulNumTp = ulCount;
	*ppstTsList = pstTsArray;
	return ERR_OK;
}

HERROR MGR_TPLIST_FreeTsList (DbSvc_TsInfo_t *pstTsList)
{
	if (pstTsList != NULL)
	{
		OxAP_Free (pstTsList);
	}

	return ERR_OK;
}

