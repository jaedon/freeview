/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________THMA_Private_Include_________________________________________________
/**************************************************************************************************/

#include "thma_int.h"

/**************************************************************************************************/
#define _________THMA_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef	enum
{
	eTHMA_DATATYPE_BOOL,
	eTHMA_DATATYPE_VALUE,
	eTHMA_DATATYPE_STRING
} THMA_DATATYPE_e;

typedef struct
{
	THMA_ITEM_e			eItem;
	THMA_DATATYPE_e 	eItemType;
	HUINT32 			itemValue;
	HUINT8 				*pucData;
	HxList_t			*list;
} THMA_ItemInfo_t;

typedef struct
{
	HUINT32				ulWriteSema;
 	THMA_ItemInfo_t		pstThmaItemList[eTHMA_ITEM_MAX];
 } THMA_DATAMGR_t;

/**************************************************************************************************/
#define _________THMA_Private_Value_________________________________________________
/**************************************************************************************************/
static THMA_DATAMGR_t * gDataMgr = NULL;

const THMA_ItemInfo_t gThmaDefaultItems[] =
{
	{	eTHMA_ITEM_FVC_BASE_URL				, eTHMA_DATATYPE_STRING	,	0,				NULL, NULL},
	{	eTHMA_ITEM_FVC_AUTH_HASH			, eTHMA_DATATYPE_STRING	,	0,				NULL, NULL},
	{	eTHMA_ITEM_FVC_AUTH_TIME_STAMP		, eTHMA_DATATYPE_STRING	,	0,				NULL, NULL},
};



/**************************************************************************************************/
#define _________THMA_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Init__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Process__________________________________________________________
/**************************************************************************************************/


static void thma_item_SetItem(THMA_ITEM_e eItem, void *pData)
{
	THMA_ItemInfo_t *pItem = NULL;

	if (gDataMgr != NULL )
	{
		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pstThmaItemList[eItem];

		if(pItem->eItemType == eTHMA_DATATYPE_BOOL)
		{
			pItem->itemValue = (HUINT32)pData;
		}
		else if(pItem->eItemType == eTHMA_DATATYPE_VALUE)
		{
			pItem->itemValue = (HUINT32)pData;
		}
		else if(pItem->eItemType == eTHMA_DATATYPE_STRING)
		{
			TH_MEM_Free(pItem->pucData);
			pItem->pucData = TH_MEM_StrDup((HCHAR *)pData);
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
}

static void * thma_item_GetItemPoint(THMA_DATATYPE_e eItem)
{
	THMA_ItemInfo_t *pItem = NULL;
	HUINT8 			*pData = NULL;

	if(gDataMgr != NULL )
	{
		HxSEMT_Get(gDataMgr->ulWriteSema);
		pItem = &gDataMgr->pstThmaItemList[eItem];

		if (pItem->eItemType == eTHMA_DATATYPE_STRING)
		{
			if(pItem->pucData)
			{
				pData = TH_MEM_StrDup(pItem->pucData);
			}
		}
		HxSEMT_Release(gDataMgr->ulWriteSema);
	}

	return pData;
}


static HUINT32 thma_item_GetItemValue(THMA_ITEM_e eItem)
{
	THMA_ItemInfo_t *pItem = NULL;
	HUINT32 ulValue = 0;

	if (gDataMgr != NULL )
	{

		HxSEMT_Get(gDataMgr->ulWriteSema);

		pItem = &gDataMgr->pstThmaItemList[eItem];


		if (pItem->eItemType == eTHMA_DATATYPE_BOOL)
		{
			ulValue = (HUINT32)((pItem->itemValue == 0)? FALSE : TRUE);
		}
		else if (pItem->eItemType == eTHMA_DATATYPE_VALUE)
		{
			ulValue = pItem->itemValue;
		}

		HxSEMT_Release(gDataMgr->ulWriteSema);
	}
	return ulValue;
}

/**************************************************************************************************/
#define _________THMA_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Public_Functions__________________________________________________________

void		THMA_ITEM_SetItem(THMA_ITEM_e eItem, void *pData)
{
	thma_item_SetItem(eItem, pData);

}
void		*THMA_ITEM_GetItemPoint(THMA_ITEM_e eItem)
{
	return thma_item_GetItemPoint(eItem);

}
HUINT32		THMA_ITEM_GetItemValue(THMA_ITEM_e eItem)
{
	return thma_item_GetItemValue(eItem);
}

void THMA_ITEM_InitDataManager(void)
{
	HxLOG_Trace();

	if (gDataMgr == NULL)
	{
		HUINT32 i = 0;
		gDataMgr = (THMA_DATAMGR_t*)TH_MEM_Calloc(sizeof(THMA_DATAMGR_t));
 		gDataMgr->ulWriteSema = 0;
		HxSEMT_Create(&(gDataMgr->ulWriteSema), "ThmaDataSema", HxSEMT_FIFO);

 		for(i = 0; i < eTHMA_ITEM_MAX; i++)
 			HxSTD_MemCopy(&gDataMgr->pstThmaItemList[i], &gThmaDefaultItems[i], sizeof(THMA_ItemInfo_t));
  	}
}

/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_End_Of_File____________________________________________________
/**************************************************************************************************/
