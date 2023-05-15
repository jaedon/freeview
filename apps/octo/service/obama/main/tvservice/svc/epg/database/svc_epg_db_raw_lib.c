/**
@file     file_name.c File Name을 적습니다.
@brief    file_name & simple comment.

Description: File에 대한 설명을 적습니다.		\n
Module: Layer와 모듈 명을 적습니다.			 	\n
Remarks : 										\n

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdlib.h>
#include <octo_common.h>
#include <dapi.h>
#include <dlib.h>
#include "linkedlist.h"
#include <pal_sef.h>
#include <pal_dsmcc.h>

#include <db_param.h>
#include <db_svc.h>
#include "isosvc.h"

#include <svc_epg.h>
#include <mwc_codepage.h>
#include <util.h>
#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_db_raw_lib.h"
#include "../local_include/_svc_epg_manager.h"

#include <psvc_epg.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define _____STATIC_FUNC_____

STATIC void	svc_epglib_FreeSectionListContents(void *pvContents)
{
	HUINT8				*pucSection = NULL;

	if(pvContents == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}
	pucSection = (HUINT8*)pvContents;

	OxSI_Free(pucSection);

EXIT_EPGRAWDB_FUNC :

	return;
}

STATIC HERROR	svc_epglib_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem)
{
	HERROR				hError = ERR_EPG_FAIL;
	HTREEITEM			hFirstChild = NULL;
	HTREEITEM			hTempItem = NULL;
	HUINT32				ulItemDelievery = 0;

	if(phDeliItem == NULL || hTree == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}
	*phDeliItem = NULL;

	hError = UTIL_TREEUTIL_GetFirstChildItem(hTree, &hFirstChild);
	if(hError != ERR_TREEUTIL_OK)
	{
		HxLOG_Print("GetFirstChildItem fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(HUINT32), (void*)&ulItemDelievery);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				HxLOG_Print("GetNextItem fail!!\n");
				goto EXIT_EPGRAWDB_FUNC;
			}
			continue;
		}
		if(ulItemDelievery == ulDelievery)
		{
			*phDeliItem = hTempItem;
			break;
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetNextItem fail!!\n");
			goto EXIT_EPGRAWDB_FUNC;
		}
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_FUNC :

	return hError;
}


STATIC HERROR	svc_epglib_FindTsItem(SvcEpg_ServiceTsInfoRaw_t *pEpgTsInfo, HTREEITEM hDeliItem, HTREEITEM *phTsItem)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
	SvcEpg_ServiceTsInfoRaw_t		stTempTsInfo;

	if(phTsItem == NULL || hDeliItem == NULL || pEpgTsInfo == NULL)
	{
		HxLOG_Print("param fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}
	*phTsItem = NULL;

	hError = UTIL_TREEUTIL_GetChildItem(hDeliItem, &hFirstChild);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetChildItem fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(SvcEpg_ServiceTsInfoRaw_t), (void*)&stTempTsInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				HxLOG_Print("GetNextItem fail!!\n");
				goto EXIT_EPGRAWDB_FUNC;
			}
			continue;
		}
		if(	stTempTsInfo.usTsId == pEpgTsInfo->usTsId
			&& stTempTsInfo.usOnId == pEpgTsInfo->usOnId
			&& stTempTsInfo.ucAntId == pEpgTsInfo->ucAntId)
		{
			*phTsItem = hTempItem;
			break;
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetNextItem fail!!\n");
			goto EXIT_EPGRAWDB_FUNC;
		}
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_FUNC :

	return hError;
}

STATIC HERROR	svc_epglib_FindServiceItem(HUINT16 usSvcId, HTREEITEM hTsItem, HTREEITEM *phSvcItem)
{
	HERROR						hError = ERR_EPG_FAIL;
	HTREEITEM					hFirstChild = NULL;
	HTREEITEM					hTempItem = NULL;
	HUINT16						usTempSvcId = 0;

	if(phSvcItem == NULL || hTsItem == NULL)
	{
		HxLOG_Print("param fail!!- usSvcId:0x%X,hTsItem:0x%X,phSvcItem:0x%X\n", usSvcId,hTsItem,phSvcItem);
		goto EXIT_EPGRAWDB_FUNC;
	}
	*phSvcItem = NULL;

	hError = UTIL_TREEUTIL_GetChildItem(hTsItem, &hFirstChild);
	if(hError != ERR_EPG_OK)
	{
		HxLOG_Print("GetChildItem fail!!\n");
		goto EXIT_EPGRAWDB_FUNC;
	}

	hTempItem = hFirstChild;
	while(hTempItem)
	{
		hError = UTIL_TREEUTIL_GetItemUserInfo(hTempItem, sizeof(HUINT16), (void*)&usTempSvcId);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetItemUserInfo fail!!\n");
			hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
			if(hError != ERR_OK)
			{
				HxLOG_Print("GetNextItem fail!!\n");
				goto EXIT_EPGRAWDB_FUNC;
			}
			continue;
		}
		if(usSvcId == usTempSvcId)
		{
			*phSvcItem = hTempItem;
			break;
		}

		hError = UTIL_TREEUTIL_GetNextItem(hTempItem, &hTempItem);
		if(hError != ERR_OK)
		{
			HxLOG_Print("GetNextItem fail!!\n");
			goto EXIT_EPGRAWDB_FUNC;
		}
	}

	hError = ERR_EPG_OK;

EXIT_EPGRAWDB_FUNC :

	return hError;
}

#define ____GLOBAL_FUNC____

HERROR	SVC_EPGLIB_FindDelieveryItem(HTREEMGR hTree, HUINT32 ulDelievery, HTREEITEM *phDeliItem)
{
	return svc_epglib_FindDelieveryItem(hTree, ulDelievery, phDeliItem);
}

HERROR	SVC_EPGLIB_FindTsItem(SvcEpg_ServiceTsInfoRaw_t *pEpgTsInfo, HTREEITEM hDeliItem, HTREEITEM *phTsItem)
{
	return svc_epglib_FindTsItem(pEpgTsInfo, hDeliItem, phTsItem);
}

HERROR	SVC_EPGLIB_FindServiceItem(HUINT16 usSvcId, HTREEITEM hTsItem, HTREEITEM *phSvcItem)
{
	return svc_epglib_FindServiceItem(usSvcId, hTsItem, phSvcItem);
}



HERROR	SVC_EPGLIB_AddSectionList(	Handle_t hSvcHandle,
											EPG_SpecType_e eSpec,
											HUINT32 ulOrgDeliType,
											HUINT8 ucAntId,
											HUINT16	usTblId,
											POINTER_LIST_T *pSectionList)
{
	SvcEpg_Type_b	bwType = eOxEPG_TYPE_NONE;
	DxEPG_Type_e	dxtype = eDxEPG_TYPE_NONE;
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulCount = 0;


	if(pSectionList == NULL)
	{
		HxLOG_Error(": @Err@ \n");
		return ERR_FAIL;
	}

	hError = psvc_epg_GetEpgTypeByTableId(usTblId, &bwType);


	if (bwType != eOxEPG_TYPE_NONE)
	{
		POINTER_LIST_T	*iter;
		HUINT8			*section;

		dxtype	 = svc_epg_ToDamaType(bwType);
		DAPI_BatchBegin(DxEPG_NAMEOF(dxtype));

		for (iter = pSectionList
			; iter
			; iter = iter->pNext)
		{
			section = (HUINT8 *)UTIL_LINKEDLIST_GetContents(iter);
			if (section)
			{
				ulCount += svc_epg_PutSection(bwType, (HINT32)ucAntId, (DxDeliveryType_e)ulOrgDeliType, section);
			}
		}
		DAPI_BatchEnd(DxEPG_NAMEOF(dxtype));
		HxLOG_Debug("===============>ulCount[%d] DxEPG_NAMEOF([%s]\n", ulCount, DxEPG_NAMEOF(dxtype));

		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSectionList, svc_epglib_FreeSectionListContents);
		pSectionList = NULL;
		hError = ERR_OK;
	}

	return hError;
}

void		SVC_EPGLIB_FreeSectionListContents(void *pvContents)
{
	svc_epglib_FreeSectionListContents(pvContents);
}

/*********************** End of File *******************************/
