/**
	@file     xdb_param.c
	@brief    icordhdplus project default data

	Description: default db 생성을 위한 default value 모음
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <_db_main.h>

#include "./local_include/_xdb_param.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC void xdb_param_InitDefaultKeepingItem(dbParam_KeepingItem_t *pstItem)
{
	HxSTD_MemSet(pstItem, 0, sizeof(dbParam_KeepingItem_t));

	pstItem->stInfo.szItemText[0] = '\0';
}

HERROR xdb_param_KeepingItemCreateList(const DbParam_Item_e *astKeyList, const HINT32 nListNum, dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum)
{
	HERROR							 hErr = ERR_FAIL;
	HINT32							 i, nCount;
	dbParam_KeepingItem_t			*astItem = NULL, stItem;
	DbParam_Type_e 					 eDataType;

	if (NULL == astKeyList)
	{
		HxLOG_Error("NULL == astKeyList\n");
		return ERR_FAIL;
	}

	if (0 >= nListNum)
	{
		HxLOG_Error("0 >= nListNum\n");
		return ERR_FAIL;
	}

	// 1. Alloc Item Array
	astItem = OxDB_Malloc(sizeof(dbParam_KeepingItem_t)*nListNum);
	if (astItem)
	{
		HxSTD_MemSet(astItem, 0, sizeof(dbParam_KeepingItem_t)*nListNum);
	}

	if (NULL == astItem)
	{
		HxLOG_Error("NULL == astItem\n");
		return ERR_FAIL;
	}

	// 2. Copy(clone) Current Item Data
	nCount = 0;
	for(i = 0; i < nListNum; i++)
	{
		xdb_param_InitDefaultKeepingItem(&stItem);
		stItem.eItem = (DbParam_Item_e)astKeyList[i];

		// keeping해야할 data	type이 integer 뿐이라 우선 nItemVallue에 값을 저장하지만, String이나 binary 일 경우 수정이 필요하다.
		eDataType = eDB_PARAM_TYPE_Max;
		hErr = DB_PARAM_GetItemType(stItem.eItem, &eDataType);
		if(ERR_OK == hErr)
		{
			HxLOG_Debug("astKeyList[%d] {Key:%d, Type:%d}\n", i, astKeyList[i], eDataType);

			switch (eDataType)
			{
			case eDB_PARAM_TYPE_Int:
				hErr = DB_PARAM_GetItem(stItem.eItem, (HUINT32 *)&(stItem.stInfo.nItemValue), 0);
				HxLOG_Debug("eDB_PARAM_TYPE_Int : astKeyList[%d] {Value:%d}\n", i, stItem.stInfo.nItemValue);
				break;
			case eDB_PARAM_TYPE_String:
				hErr = DB_PARAM_GetItem(stItem.eItem, (HUINT32 *)(stItem.stInfo.szItemText), 0);
				HxLOG_Debug("eDB_PARAM_TYPE_String : astKeyList[%d] {Value:%s}\n", i, stItem.stInfo.szItemText);
				break;
			case eDB_PARAM_TYPE_Enum:
				hErr = DB_PARAM_GetItem(stItem.eItem, (HUINT32 *)&(stItem.stInfo.nItemValue), 0);
				HxLOG_Debug("eDB_PARAM_TYPE_Enum : astKeyList[%d] {Value:%d}\n", i, stItem.stInfo.nItemValue);
				break;
			case eDB_PARAM_TYPE_Bin:
				if (0 < stItem.stInfo.nDataSize)
				{
					void *pvData = DB_MEM_Alloc(sizeof(HUINT8)*stItem.stInfo.nDataSize);

					if (pvData)
					{
						hErr = DB_PARAM_GetItem(stItem.eItem, (HUINT32 *)pvData, (HUINT32)stItem.stInfo.nDataSize);
						if (ERR_OK == hErr)
						{
							stItem.stInfo.pucData = pvData;
							HxLOG_Debug("eDB_PARAM_TYPE_Bin : astKeyList[%d] {Size:%d}\n", i, stItem.stInfo.nDataSize);
							HLIB_LOG_Dump(stItem.stInfo.pucData, stItem.stInfo.nDataSize, 0, FALSE);
						}

						DB_MEM_Free(pvData);
					}
				}
				break;
			default:
				hErr = ERR_FAIL;
				HxLOG_Warning("Invalid Type [%d]\n", eDataType);
				break;
			}

			if(ERR_OK == hErr)
			{
				HxSTD_memcpy(&astItem[nCount], &stItem, sizeof(dbParam_KeepingItem_t));
				nCount++;
			}
		}
	}

	if (nCount <= 0)
	{
		if (astItem)
			OxDB_Free(astItem);
		astItem = NULL;
		*pastItem = NULL;
		*pnItemNum = 0;
		hErr = ERR_FAIL;
	}
	else
	{
		*pastItem = astItem;
		*pnItemNum = nCount;
		hErr = ERR_OK;
	}

	return hErr;
}

HERROR xdb_param_KeepingItemFree(dbParam_KeepingItem_t *astItem, HINT32 nItemNum)
{
	dbParam_KeepingItem_t	*pstKeepingItem;
	DbParam_Type_e			 eDataType;
	HINT32					 i;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == astItem)
		return ERR_OK;

	if (0 >= nItemNum)
		return ERR_OK;

	for (i = 0; i < nItemNum; i++)
	{
		pstKeepingItem = &astItem[i];
		eDataType = eDB_PARAM_TYPE_Max;
		hErr = DB_PARAM_GetItemType(pstKeepingItem->eItem, &eDataType);
		if (ERR_OK == hErr)
		{
			if (eDB_PARAM_TYPE_Bin == eDataType)
			{
				if (pstKeepingItem->stInfo.pucData)
				{
					OxDB_Free(pstKeepingItem->stInfo.pucData);
					pstKeepingItem->stInfo.pucData = NULL;
				}
			}
		}
	}

	OxDB_Free(astItem);

	return ERR_OK;
}

/*********************** End of File ******************************/

