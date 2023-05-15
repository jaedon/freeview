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
 * @file	  		nx_prism_menu_contents.c
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
#include <stdio.h>
#include <stdlib.h>
#include <nx_prism_app.h>
#include <nx_common.h>
#include <nx_prism_menu_contents.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxUiMainMenuCfg_List		*s_pstMainMenuList = (NxUiMainMenuCfg_List*)NULL;
static NxUiMenuCfg_List			*s_pstMenuList = (NxUiMenuCfg_List*)NULL;


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_AppContents_InitSubitemArray (void *pvContents)
{
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(BaseMenuContents));

	pstSubItemArray = (Nx_SettingsLeafSubItem_t *)NX_APP_Malloc (sizeof(Nx_SettingsLeafSubItem_t) * NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS);
	if (pstSubItemArray == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
	HxSTD_MemSet (pstSubItemArray, 0, sizeof(Nx_SettingsLeafSubItem_t) * NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS);

	pstContents->ulMaxNumOfSubItem	= NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS;
	pstContents->subItems			= pstSubItemArray;

	return ONDK_RET_OK;
}


void	NX_PRISM_AppContents_FreeSubitemArray (void *pvContents)
{
	HUINT32					 ulIndex;
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		if (pstContents->subItems != NULL)
		{
			for (ulIndex = 0; ulIndex < pstContents->ulNumOfSubItem; ulIndex++)
			{
				NX_PRISM_AppContents_ClearLeafSubItem (&(pstContents->subItems[ulIndex]));
			}

			NX_APP_Free (pstContents->subItems);
			pstContents->subItems = NULL;
		}

		HxSTD_MemSet (pstContents, 0, sizeof(BaseMenuContents));
	}
}


ONDK_Result_t	NX_PRISM_AppContents_InitSubitemArray_KeepFocus(void *pvContents)
{
	HUINT32					 ulCurFocus;
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	ulCurFocus = pstContents->ulCurFocus;
	(void)NX_PRISM_AppContents_InitSubitemArray(pvContents);
	pstContents->ulCurFocus = ulCurFocus;

	return ONDK_RET_OK;
}


void	NX_PRISM_AppContents_FreeSubitemArray_KeepFocus(void *pvContents, HUINT32 ulFreeSize)
{
	HUINT32					 ulCurFocus;
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		ulCurFocus = pstContents->ulCurFocus;
		NX_PRISM_AppContents_FreeSubitemArray(pvContents);
		HxSTD_MemSet(pvContents, 0, ulFreeSize);
		pstContents->ulCurFocus = ulCurFocus;
	}
}


ONDK_Result_t	NX_PRISM_AppContents_SetFocus(void *pvContents, HUINT32 ulFocus)
{
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (pstContents->ulNumOfSubItem < ulFocus + 1)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	pstContents->ulCurFocus = ulFocus;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppContents_AppendSubitem (void *pvContents, Nx_SettingsLeafSubItem_t *pstSubItem)
{
	HUINT32					ulMaxNum;
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (pstSubItem == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	// 미리 잡아 놓은 subitem array보다 커질 경우에는 확장해서 할당한 후 바꾼다.
	if (pstContents->ulNumOfSubItem >= pstContents->ulMaxNumOfSubItem)
	{
		ulMaxNum = pstContents->ulMaxNumOfSubItem + NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS;

		pstSubItemArray = (Nx_SettingsLeafSubItem_t *)NX_APP_Malloc (sizeof(Nx_SettingsLeafSubItem_t) * ulMaxNum);
		if (pstSubItemArray == NULL)
		{
			return GWM_RET_OUT_OF_MEMORY;
		}

		HxSTD_MemSet (pstSubItemArray, 0, sizeof(Nx_SettingsLeafSubItem_t) * ulMaxNum);

		if (pstContents->subItems != NULL)
		{
			// 기존 내용을 copy 후 free
			HxSTD_memcpy (pstSubItemArray, pstContents->subItems, sizeof(Nx_SettingsLeafSubItem_t) * pstContents->ulMaxNumOfSubItem);
			NX_APP_Free (pstContents->subItems);
		}

		pstContents->subItems = pstSubItemArray;
	}

	pstSubItemArray = pstContents->subItems;
	HxSTD_memcpy (&(pstSubItemArray[pstContents->ulNumOfSubItem]), pstSubItem, sizeof(Nx_SettingsLeafSubItem_t));

	pstContents->ulNumOfSubItem ++;
	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppContents_DeleteSubitem (void *pvContents, HUINT32 ulSubitemIdx)
{
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstSubItemArray = pstContents->subItems;
	if (pstSubItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (ulSubitemIdx >= pstContents->ulNumOfSubItem)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	// Free the subitem
	NX_PRISM_AppContents_ClearLeafSubItem (&(pstSubItemArray[ulSubitemIdx]));

	// 삭제한 subitem이 마지막이 아닐 경우, 그 뒤에 있는 모든 item들이 앞으로 당겨져야 한다.
	for (; ulSubitemIdx < pstContents->ulNumOfSubItem - 1; ulSubitemIdx++)
	{
		HxSTD_memcpy (&(pstSubItemArray[ulSubitemIdx]), &(pstSubItemArray[ulSubitemIdx + 1]), sizeof(Nx_SettingsLeafSubItem_t));
	}

	// 마지막 item은 clear 시킨다. 이는 나중에 예외상황을 만들지 않기 위한 작업이다.
	HxSTD_MemSet (&(pstSubItemArray[pstContents->ulNumOfSubItem - 1]), 0, sizeof(Nx_SettingsLeafSubItem_t));
	pstContents->ulNumOfSubItem --;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppContents_DeleteSubitemByObjId (void *pvContents, HUINT32 ulObjId)
{
	HUINT32					 ulSubitemIdx;
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return	GWM_RET_TARGET_NULL;
	}

	pstSubItemArray = pstContents->subItems;
	if (pstSubItemArray == NULL)
	{
		return	GWM_RET_TARGET_NULL;
	}

	for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
	{
		if (pstSubItemArray[ulSubitemIdx].ulId == ulObjId)
		{
			// Free the subitem
			NX_PRISM_AppContents_ClearLeafSubItem (&(pstSubItemArray[ulSubitemIdx]));

			// 삭제한 subitem이 마지막이 아닐 경우, 그 뒤에 있는 모든 item들이 앞으로 당겨져야 한다.
			for (; ulSubitemIdx < pstContents->ulNumOfSubItem - 1; ulSubitemIdx++)
			{
				HxSTD_memcpy (&(pstSubItemArray[ulSubitemIdx]), &(pstSubItemArray[ulSubitemIdx + 1]), sizeof(Nx_SettingsLeafSubItem_t));
			}

			// 마지막 item은 clear 시킨다. 이는 나중에 예외상황을 만들지 않기 위한 작업이다.
			HxSTD_MemSet (&(pstSubItemArray[pstContents->ulNumOfSubItem - 1]), 0, sizeof(Nx_SettingsLeafSubItem_t));
			pstContents->ulNumOfSubItem --;

			return	ONDK_RET_OK;
		}
	}

	return	GWM_RET_NO_OBJECT;
}


ONDK_Result_t	NX_PRISM_AppContents_ClearSubitem (void *pvContents, HUINT32 ulSubitemIdx)
{
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstSubItemArray = pstContents->subItems;
	if (pstSubItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (ulSubitemIdx >= pstContents->ulNumOfSubItem)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	// Free the subitem
	NX_PRISM_AppContents_ClearLeafSubItem (&(pstSubItemArray[ulSubitemIdx]));

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppContents_ClearSubitemByObjId (void *pvContents, HUINT32 ulObjId)
{
	HUINT32					 ulSubitemIdx;
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstSubItemArray = pstContents->subItems;
	if (pstSubItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
	{
		if (pstSubItemArray[ulSubitemIdx].ulId == ulObjId)
		{
			// Free the subitem
			NX_PRISM_AppContents_ClearLeafSubItem (&(pstSubItemArray[ulSubitemIdx]));

			return ONDK_RET_OK;
		}
	}

	return GWM_RET_NO_OBJECT;
}


HUINT32	NX_PRISM_AppContents_CountSubitem (void *pvContents)
{
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		if (pstContents->subItems != NULL)
		{
			return pstContents->ulNumOfSubItem;
		}
	}

	return 0;
}


Nx_SettingsLeafSubItem_t *NX_PRISM_AppContents_GetSubitem (void *pvContents, HUINT32 ulSubitemIdx)
{
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		if (pstContents->subItems != NULL)
		{
			if (ulSubitemIdx < pstContents->ulNumOfSubItem)
			{
				return &(pstContents->subItems[ulSubitemIdx]);
			}
		}
	}

	return NULL;
}


Nx_SettingsLeafSubItem_t *NX_PRISM_AppContents_GetCurrentSubitem (void *pvContents)
{
	BaseMenuContents		*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		if (pstContents->subItems != NULL)
		{
			if (pstContents->ulCurFocus < pstContents->ulNumOfSubItem)
			{
				return &(pstContents->subItems[pstContents->ulCurFocus]);
			}
		}
	}

	return NULL;
}


Nx_SettingsLeafSubItem_t *NX_PRISM_AppContents_GetSubitemByObjId (void *pvContents, HUINT32 ulObjId)
{
	HUINT32				 ulSubitemIdx;
	BaseMenuContents	*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL)
	{
		if (pstContents->subItems != NULL)
		{
			for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
			{
				if (pstContents->subItems[ulSubitemIdx].ulId == ulObjId)
				{
					return &(pstContents->subItems[ulSubitemIdx]);
				}
			}
		}
	}

	return NULL;
}


ONDK_Result_t	NX_PRISM_AppContents_GetSubitemIndexByObjId (void *pvContents, HUINT32 ulObjId, HUINT32 *pulIndex)
{
	HUINT32					 ulSubitemIdx;
	BaseMenuContents		*pstContents;
	Nx_SettingsLeafSubItem_t	*pstSubItemArray;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	if (pulIndex == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstSubItemArray = pstContents->subItems;
	if (pstSubItemArray == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
	{
		if (pstSubItemArray[ulSubitemIdx].ulId == ulObjId)
		{
			*pulIndex = ulSubitemIdx;
			return ONDK_RET_OK;
		}
	}

	return GWM_RET_NO_OBJECT;
}


ONDK_Result_t	NX_PRISM_AppContents_GetObjIdBySubitemIndex (void *pvContents, HUINT32 ulSubitemIdx, HUINT32 *pulObjId)
{
	BaseMenuContents	*pstContents;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)								{ return GWM_RET_SOURCE_NULL; }
	if (pulObjId == NULL)									{ return GWM_RET_TARGET_NULL; }

	if (pstContents->subItems == NULL)						{ return GWM_RET_SOURCE_NULL; }
	if (ulSubitemIdx >= pstContents->ulNumOfSubItem)		{ return GWM_RET_OUT_OF_RANGE; }

	*pulObjId = pstContents->subItems[ulSubitemIdx].ulId;
	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppContents_SetUserInputValueToSubitem (void *pvContents, HINT32 lObjId)
{
	int						 nIndex;
	HUINT32					 ulValue;
	HUINT8					*szString = NULL;
	Nx_SettingsLeafSubItem_t	*pstSubItem;
	BaseMenuContents		*pstContents;
	ONDK_Result_t					 hErr;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)								{ return GWM_RET_SOURCE_NULL; }

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if (pstSubItem == NULL)				{ return GWM_RET_TARGET_NULL; }

	switch (ONDK_GWM_Obj_GetObjectType (lObjId))
	{
	case GUIOBJ_SELECTOR:
		nIndex = ONDK_GWM_Selector_GetCurrentIndex (lObjId);
		if (nIndex < 0)					{ return GWM_RET_OUT_OF_RANGE; }

		pstSubItem->ulIdxOfTextList = (HUINT32)nIndex;
		HxSTD_StrNCpy ((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[nIndex], MAX_ELEMENT_TEXT_LENGTH);
		break;

	case GUIOBJ_INPUT:
		szString = (HUINT8 *)ONDK_GWM_COM_Input_GetText (lObjId);
		if (szString == NULL)			{ return GWM_RET_OUT_OF_RANGE; }
		hErr = NX_PRISM_UTIL_ConvertDecStrToInt(szString, &ulValue);
		if (hErr != ERR_OK)				{ return GWM_RET_OUT_OF_RANGE; }

		pstSubItem->lSubItemValue = (HINT32)ulValue;
		HxSTD_MemSet(pstSubItem->szSubItemText, 0, MAX_ELEMENT_TEXT_LENGTH);
		HxSTD_StrNCpy ((char*)pstSubItem->szSubItemText, (char*)szString, MAX_ELEMENT_TEXT_LENGTH);
		break;

	default:
		return GWM_RET_NO_OBJECT;
	}

	return ONDK_RET_OK;
}


void	NX_PRISM_AppContents_ClearLeafSubItem (Nx_SettingsLeafSubItem_t *pstSubItem)
{
	HUINT32			 ulItemIdx;

	if (pstSubItem != NULL)
	{
		if (pstSubItem->szItemTextList != NULL)
		{
			for (ulItemIdx = 0; ulItemIdx < pstSubItem->ulNumOfItemTextList; ulItemIdx++)
			{
				if (pstSubItem->szItemTextList[ulItemIdx] != NULL)
				{
					NX_APP_Free (pstSubItem->szItemTextList[ulItemIdx]);
				}
			}

			NX_APP_Free (pstSubItem->szItemTextList);
		}

		pstSubItem->lSubItemValue = 0;
		pstSubItem->ulNumOfItemTextList = 0;
		pstSubItem->ulIdxOfTextList = 0;
		pstSubItem->szItemTextList = NULL;

		HxSTD_MemSet (pstSubItem->szItemString, 0, 32);
		HxSTD_MemSet (pstSubItem->szSubItemText, 0, MAX_ELEMENT_TEXT_LENGTH);
	}
}


ONDK_Result_t	NX_PRISM_AppContents_GetAvailableFirstSubitemObj (void *pvContents, HINT32 *pnObjId)
{
	HUINT32					 ulIndex;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;
	BaseMenuContents			*pstContents = NULL;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)								{ return GWM_RET_SOURCE_NULL; }
	if (pnObjId == NULL)									{ return GWM_RET_TARGET_NULL; }

	for (ulIndex = 0; ulIndex < pstContents->ulNumOfSubItem; ulIndex++)
	{
		pstSubItem = &(pstContents->subItems[ulIndex]);

		if ((ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_APP_GetObject(pstSubItem->ulId)) & GUIOBJ_DISABLED) == 0)
		{
			*pnObjId = pstSubItem->ulId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


ONDK_Result_t	NX_PRISM_AppContents_GetAvailableLastSubitemObj (void *pvContents, HINT32 *pnObjId)
{
	HINT32						nIndex;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;
	BaseMenuContents		*pstContents = NULL;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents == NULL)								{ return GWM_RET_SOURCE_NULL; }
	if (pnObjId == NULL)									{ return GWM_RET_TARGET_NULL; }

	for (nIndex = (HINT32)pstContents->ulNumOfSubItem - 1; nIndex >= 0; nIndex--)
	{
		pstSubItem = &(pstContents->subItems[nIndex]);

		if ((ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_APP_GetObject(pstSubItem->ulId)) & GUIOBJ_DISABLED) == 0)
		{
			*pnObjId = pstSubItem->ulId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_InitMainItemArray (Nx_SettingsNaviContext_t *pstContents)
{
	HINT32					 nCurrPos;
	Nx_SettingsNaviMainItem_t	*pstMainItemArray = NULL;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	nCurrPos = pstContents->lCurFocus;
	HxSTD_MemSet (pstContents, 0, sizeof(Nx_SettingsNaviContext_t));
	pstContents->lCurFocus = nCurrPos;

	pstMainItemArray = (Nx_SettingsNaviMainItem_t *)NX_APP_Malloc (sizeof(Nx_SettingsNaviMainItem_t) * NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS);
	if (pstMainItemArray == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

	HxSTD_MemSet (pstMainItemArray, 0, sizeof(Nx_SettingsNaviMainItem_t) * NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS);

	pstContents->ulMaxOfMainItem	= NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS;
	pstContents->mainItems			= pstMainItemArray;

	return ONDK_RET_OK;
}


void	NX_PRISM_AppNaviCntxt_FreeMainItemArray (Nx_SettingsNaviContext_t *pstContents)
{
	HUINT32					 ulIndex;

	if (pstContents != NULL)
	{
		if (pstContents->mainItems != NULL)
		{
			for (ulIndex = 0; ulIndex < pstContents->ulNumOfMainItem; ulIndex++)
			{
				NX_PRISM_AppNaviCntxt_ClearNaviMainItem (&(pstContents->mainItems[ulIndex]));
			}

			NX_APP_Free (pstContents->mainItems);
		}

		HxSTD_MemSet (pstContents, 0, sizeof(Nx_SettingsNaviContext_t));
	}
}


void	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(Nx_SettingsNaviContext_t *pstContents)
{
	HINT32 		lCurFocus;

	if (pstContents != NULL)
	{
		lCurFocus = pstContents->lCurFocus;
		NX_PRISM_AppNaviCntxt_FreeMainItemArray(pstContents);
		pstContents->lCurFocus = lCurFocus;
	}
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_AppendMainItem (Nx_SettingsNaviContext_t *pstContents, Nx_SettingsNaviMainItem_t *pstMainItem)
{
	HUINT32					 ulMaxNum;
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (pstMainItem == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	// 미리 잡아 놓은 subitem array보다 커질 경우에는 확장해서 할당한 후 바꾼다.
	if (pstContents->ulNumOfMainItem >= pstContents->ulMaxOfMainItem)
	{
		ulMaxNum = pstContents->ulMaxOfMainItem + NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS;

		pstMainItemArray = (Nx_SettingsNaviMainItem_t *)NX_APP_Malloc (sizeof(Nx_SettingsNaviMainItem_t) * ulMaxNum);
		if (pstMainItemArray == NULL)
		{
			return GWM_RET_OUT_OF_MEMORY;
		}

		HxSTD_MemSet (pstMainItemArray, 0, sizeof(Nx_SettingsNaviMainItem_t) * ulMaxNum);

		if (pstContents->mainItems != NULL)
		{
			// 기존 내용을 copy 후 free
			HxSTD_memcpy (pstMainItemArray, pstContents->mainItems, sizeof(Nx_SettingsNaviMainItem_t) * pstContents->ulMaxOfMainItem);
			NX_APP_Free (pstContents->mainItems);
		}

		pstContents->mainItems = pstMainItemArray;
	}

	pstMainItemArray = pstContents->mainItems;
	HxSTD_memcpy (&(pstMainItemArray[pstContents->ulNumOfMainItem]), pstMainItem, sizeof(Nx_SettingsNaviMainItem_t));

	pstContents->ulNumOfMainItem ++;
	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_DeleteMainItem (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainitemIdx)
{
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstMainItemArray = pstContents->mainItems;
	if (pstMainItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (ulMainitemIdx >= pstContents->ulNumOfMainItem)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	// Free the subitem
	NX_PRISM_AppNaviCntxt_ClearNaviMainItem (&(pstMainItemArray[ulMainitemIdx]));

	// 삭제한 subitem이 마지막이 아닐 경우, 그 뒤에 있는 모든 item들이 앞으로 당겨져야 한다.
	for (; ulMainitemIdx < pstContents->ulNumOfMainItem - 1; ulMainitemIdx++)
	{
		HxSTD_memcpy (&(pstMainItemArray[ulMainitemIdx]), &(pstMainItemArray[ulMainitemIdx + 1]), sizeof(Nx_SettingsNaviMainItem_t));
	}

	// 마지막 item은 clear 시킨다. 이는 나중에 예외상황을 만들지 않기 위한 작업이다.
	HxSTD_MemSet (&(pstMainItemArray[pstContents->ulNumOfMainItem - 1]), 0, sizeof(Nx_SettingsNaviMainItem_t));
	pstContents->ulNumOfMainItem --;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_DeleteMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId)
{
	HUINT32					 ulMainitemIdx;
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstMainItemArray = pstContents->mainItems;
	if (pstMainItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	for (ulMainitemIdx = 0; ulMainitemIdx < pstContents->ulNumOfMainItem; ulMainitemIdx++)
	{
		if (pstMainItemArray[ulMainitemIdx].ulMainBtnId == ulObjId)
		{
			NX_PRISM_AppNaviCntxt_DeleteMainItem (pstContents, ulMainitemIdx);

			return ONDK_RET_OK;
		}
	}

	return GWM_RET_NO_OBJECT;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_ClearMainItem (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainitemIdx)
{
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstMainItemArray = pstContents->mainItems;
	if (pstMainItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if (ulMainitemIdx >= pstContents->ulNumOfMainItem)
	{
		return GWM_RET_OUT_OF_RANGE;
	}

	// Free the subitem
	NX_PRISM_AppNaviCntxt_ClearNaviMainItem (&(pstMainItemArray[ulMainitemIdx]));

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_ClearMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId)
{
	HUINT32					 ulMainitemIdx;
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstMainItemArray = pstContents->mainItems;
	if (pstMainItemArray == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	for (ulMainitemIdx = 0; ulMainitemIdx < pstContents->ulNumOfMainItem; ulMainitemIdx++)
	{
		if (pstMainItemArray[ulMainitemIdx].ulMainBtnId == ulObjId)
		{
			// Free the subitem
			NX_PRISM_AppNaviCntxt_ClearNaviMainItem (&(pstMainItemArray[ulMainitemIdx]));
			return ONDK_RET_OK;
		}
	}

	return GWM_RET_NO_OBJECT;
}


HUINT32	NX_PRISM_AppNaviCntxt_CountMainItem (Nx_SettingsNaviContext_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->mainItems != NULL)
		{
			return pstContents->ulNumOfMainItem;
		}
	}

	return 0;
}

/* item index는 양수 음수 다 쓸 수 있는 상황이며, object id는 양수만 쓸 수 있다.*/
Nx_SettingsNaviMainItem_t *NX_PRISM_AppNaviCntxt_GetMainItem (Nx_SettingsNaviContext_t *pstContents, HINT32 lMainitemIdx)
{
	HUINT32	ulItemIndex = 0;

	if (lMainitemIdx < 0)
	{
		return	NULL;
	}
	ulItemIndex = (HUINT32)lMainitemIdx;

	if (pstContents != NULL)
	{
		if (pstContents->mainItems != NULL)
		{
			if (!(ulItemIndex >= pstContents->ulNumOfMainItem))
			{
				return &(pstContents->mainItems[ulItemIndex]);
			}
		}
	}

	return	NULL;
}

Nx_SettingsNaviMainItem_t *NX_PRISM_AppNaviCntxt_GetCurrentMainItem (Nx_SettingsNaviContext_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->mainItems != NULL)
		{
			if (pstContents->lCurFocus < (HINT32)pstContents->ulNumOfMainItem)
			{
				return &(pstContents->mainItems[pstContents->lCurFocus]);
			}
		}
	}

	return NULL;
}

Nx_SettingsNaviMainItem_t *NX_PRISM_AppNaviCntxt_GetMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId)
{
	HUINT32				 ulMainitemIdx;

	if (pstContents != NULL)
	{
		if (pstContents->mainItems != NULL)
		{
			for (ulMainitemIdx = 0; ulMainitemIdx < pstContents->ulNumOfMainItem; ulMainitemIdx++)
			{
				if (pstContents->mainItems[ulMainitemIdx].ulMainBtnId == ulObjId)
				{
					return &(pstContents->mainItems[ulMainitemIdx]);
				}
			}
		}
	}

	return NULL;
}

ONDK_Result_t	NX_PRISM_AppNaviCntxt_GetMainItemIndexByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId, HUINT32 *pulIndex)
{
	HUINT32					 ulMainitemIdx;
	Nx_SettingsNaviMainItem_t	*pstMainItemArray;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	if (pulIndex == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	pstMainItemArray = pstContents->mainItems;
	if (pstMainItemArray == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	for (ulMainitemIdx = 0; ulMainitemIdx < pstContents->ulNumOfMainItem; ulMainitemIdx++)
	{
		if (pstMainItemArray[ulMainitemIdx].ulMainBtnId == ulObjId)
		{
			*pulIndex = ulMainitemIdx;
			return ONDK_RET_OK;
		}
	}

	return GWM_RET_NO_OBJECT;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_GetObjIdByMainItemIndex (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainItemIdx, HUINT32 *pulObjId)
{
	if (pstContents == NULL)								{ return GWM_RET_SOURCE_NULL; }
	if (pulObjId == NULL)									{ return GWM_RET_TARGET_NULL; }

	if (pstContents->mainItems == NULL)						{ return GWM_RET_SOURCE_NULL; }
	if (ulMainItemIdx >= pstContents->ulNumOfMainItem)		{ return GWM_RET_OUT_OF_RANGE; }

	*pulObjId = pstContents->mainItems[ulMainItemIdx].ulMainBtnId;
	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_InitNaviMainItem (Nx_SettingsNaviMainItem_t *pstMainItem)
{
	if (pstMainItem == NULL)	{ return GWM_RET_SOURCE_NULL; }

	HxSTD_MemSet (pstMainItem, 0, sizeof(Nx_SettingsNaviMainItem_t));

	return ONDK_RET_OK;
}


void	NX_PRISM_AppNaviCntxt_ClearNaviMainItem (Nx_SettingsNaviMainItem_t *pstMainItem)
{
	if (pstMainItem != NULL)
	{
		if (pstMainItem->subItem != NULL)
		{
			NX_APP_Free (pstMainItem->subItem);
		}

		HxSTD_MemSet (pstMainItem, 0, sizeof (Nx_SettingsNaviMainItem_t));
	}
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_AppendSubItemTitleToMainItem (Nx_SettingsNaviMainItem_t *pstMainItem, HCHAR *hcLocId, HUINT8 *szSubItemTitle)
{
	HUINT32					 ulMaxNum;
	Nx_SettingsNaviSubItem_t	*pstSubitemArray;

	if (pstMainItem == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	if((hcLocId == NULL) && (szSubItemTitle == NULL))
	{
		return GWM_RET_SOURCE_NULL;
	}

	// 미리 잡아 놓은 subitem array보다 커질 경우에는 확장해서 할당한 후 바꾼다.
	if (pstMainItem->ulNumOfSubItem >= pstMainItem->ulMaxOfSubItem)
	{
		ulMaxNum = pstMainItem->ulMaxOfSubItem + NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS;

		pstSubitemArray = (Nx_SettingsNaviSubItem_t *)NX_APP_Malloc (sizeof(Nx_SettingsNaviSubItem_t) * ulMaxNum);
		if (pstSubitemArray == NULL)
		{
			return GWM_RET_OUT_OF_MEMORY;
		}

		HxSTD_MemSet (pstSubitemArray, 0, sizeof(Nx_SettingsNaviSubItem_t) * ulMaxNum);

		if (pstMainItem->subItem != NULL)
		{
			// 기존 내용을 copy 후 free
			HxSTD_memcpy (pstSubitemArray, pstMainItem->subItem, sizeof(Nx_SettingsNaviSubItem_t) * pstMainItem->ulMaxOfSubItem);
			NX_APP_Free (pstMainItem->subItem);
		}

		pstMainItem->ulMaxOfSubItem = ulMaxNum;
		pstMainItem->subItem = pstSubitemArray;
	}

	pstSubitemArray = pstMainItem->subItem;
	pstSubitemArray[pstMainItem->ulNumOfSubItem].hcTitleLocId = hcLocId;

	if (hcLocId && (szSubItemTitle != NULL))
	{
		szSubItemTitle = (HUINT8*)ONDK_GetString(hcLocId);
	}

	HxSTD_StrNCpy((char*)pstSubitemArray[pstMainItem->ulNumOfSubItem].ucTitle, (char*)ONDK_GetString(hcLocId), MAX_ELEMENT_TEXT_LENGTH);
	pstSubitemArray[pstMainItem->ulNumOfSubItem].ucTitle[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';
	pstMainItem->ulNumOfSubItem ++;

	return ONDK_RET_OK;
}


/******************************************************************************/
#define ___Menu_Functions_____________________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(Nx_SettingsNaviMainItem_t *pstMenuItem, HINT32 lMainMenuId)
{
	HUINT32				i = 0, ulMenuItemNum = 0;
	NxUiMenuCfgInfo		*pstMenuInfo = NULL;


	ulMenuItemNum = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(lMainMenuId);
	for (i = 0; i < ulMenuItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(lMainMenuId, i);
		NX_PRISM_AppNaviCntxt_AppendSubItemTitleToMainItem (pstMenuItem, pstMenuInfo->pszTitleStrId, NULL);
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_AddMenuItem(NxUiMenuCfgInfo stMenuInfo, HINT32 lMainMenuId)
{
	HUINT32				ulItemIdx = 0;
	NxUiMenuCfg_List	*pstMenuList = s_pstMenuList;

	ulItemIdx = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(lMainMenuId);

	while (pstMenuList != NULL)
	{
		if ((lMainMenuId == pstMenuList->stMenuItem.lMainMenuId) && (stMenuInfo.lMenuId == pstMenuList->stMenuItem.lMenuId))
		{
			NX_APP_Error("Error!!! Alreay lMainMenuId :(0x%x), Add Menu Item : (0x%x)\n", lMainMenuId, stMenuInfo.lMenuId);
			return	ONDK_RET_FAIL;
		}

		pstMenuList = pstMenuList->pNext;
	}

	pstMenuList	= (NxUiMenuCfg_List*)NX_APP_Calloc(sizeof(NxUiMenuCfg_List));
	if (pstMenuList == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail!!\n");
		return	ONDK_RET_FAIL;
	}

	stMenuInfo.lOrder = ulItemIdx;
	pstMenuList->stMenuItem = stMenuInfo;
	pstMenuList->pNext = s_pstMenuList;
	s_pstMenuList = pstMenuList;

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_DeleteAllMenuItem(void)
{
	NxUiMenuCfg_List	*pstMenuList = s_pstMenuList;

    while (pstMenuList)
	{
		s_pstMenuList = pstMenuList->pNext;
		if (pstMenuList)
		{
			NX_APP_Free(pstMenuList);
		}
		pstMenuList = (NxUiMenuCfg_List *)NULL;
		pstMenuList = s_pstMenuList;
    }

    return ONDK_RET_OK;
}


NxUiMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(HINT32 lMainMenuId, HINT32 lIndex)
{
	NxUiMenuCfg_List	*pstMenuList = s_pstMenuList;

	while (pstMenuList != NULL)
	{
		if ((lMainMenuId == pstMenuList->stMenuItem.lMainMenuId) && (lIndex == pstMenuList->stMenuItem.lOrder))
		{
			return &pstMenuList->stMenuItem;
		}

		pstMenuList = pstMenuList->pNext;
	}

	return NULL;
}


NxUiMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(HINT32 lMainMenuId, HINT32 lMenuId)
{
	NxUiMenuCfg_List	*pstMenuList = s_pstMenuList;

	while (pstMenuList != NULL)
	{
		if ((lMainMenuId == pstMenuList->stMenuItem.lMainMenuId) && (lMenuId == pstMenuList->stMenuItem.lMenuId))
		{
			return &pstMenuList->stMenuItem;
		}

		pstMenuList = pstMenuList->pNext;
	}

	return NULL;
}


HUINT32		NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(HINT32 lMainMenuId)
{
	HUINT32				ulCount = 0;
	NxUiMenuCfg_List	*pstMenuList = s_pstMenuList;

	while (pstMenuList != NULL)
	{
		if (lMainMenuId == pstMenuList->stMenuItem.lMainMenuId)
		{
			ulCount++;
		}
		pstMenuList = pstMenuList->pNext;
	}

	return	ulCount;
}


/******************************************************************************/
#define ___Main_Menu_Functions_________________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(Nx_SettingsNaviMainItem_t *pstMainItem)
{
	HUINT32		i = 0, ulMainItemNum = 0;
	NxUiMainMenuCfgInfo		*pstMenuInfo = NULL;


	ulMainItemNum = NX_PRISM_AppNaviCntxt_GetTotalMainMenuItemNum();
	for (i = 0; i < ulMainItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByOrderId(i);
		NX_PRISM_AppNaviCntxt_AppendSubItemTitleToMainItem (pstMainItem, pstMenuInfo->pszTitleStrId, NULL);
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_AddMainMenuItem(NxUiMainMenuCfgInfo stMainMenuInfo)
{
	HUINT32					ulItemIdx = 0;
	NxUiMainMenuCfg_List	*pstMainMenuList = s_pstMainMenuList;

	ulItemIdx = NX_PRISM_AppNaviCntxt_GetTotalMainMenuItemNum();

	while (pstMainMenuList != NULL)
	{
		if (stMainMenuInfo.lMenuId == pstMainMenuList->stMainMenuItem.lMenuId)
		{
			NX_APP_Error("Error!!! Alreay Add Menu Item : (0x%x)\n", stMainMenuInfo.lMenuId);
			return	ONDK_RET_FAIL;
		}

		pstMainMenuList = pstMainMenuList->pNext;
	}

	pstMainMenuList	= (NxUiMainMenuCfg_List*)NX_APP_Calloc(sizeof(NxUiMainMenuCfg_List));
	if (pstMainMenuList == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail!!\n");
		return	ONDK_RET_FAIL;
	}

	stMainMenuInfo.lOrder = ulItemIdx;
	pstMainMenuList->stMainMenuItem = stMainMenuInfo;
	pstMainMenuList->pNext = s_pstMainMenuList;
	s_pstMainMenuList = pstMainMenuList;

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_AppNaviCntxt_DeleteAllMainMenuItem(void)
{
	NxUiMainMenuCfg_List	*pstMainMenuList = s_pstMainMenuList;

    while (pstMainMenuList)
	{
		s_pstMainMenuList = pstMainMenuList->pNext;
		NX_APP_Free(pstMainMenuList);
		pstMainMenuList = (NxUiMainMenuCfg_List *)NULL;
		pstMainMenuList = s_pstMainMenuList;
    }

	NX_PRISM_AppNaviCntxt_DeleteAllMenuItem();

    return ONDK_RET_OK;
}


NxUiMainMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(HINT32 lMenuId)
{
	NxUiMainMenuCfg_List	*pstMainMenuList = s_pstMainMenuList;

	while (pstMainMenuList != NULL)
	{
		if (lMenuId == pstMainMenuList->stMainMenuItem.lMenuId)
		{
			return &pstMainMenuList->stMainMenuItem;
		}

		pstMainMenuList = pstMainMenuList->pNext;
	}

	return NULL;
}


NxUiMainMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMainMenuItemByOrderId(HINT32 lIndex)
{
	NxUiMainMenuCfg_List	*pstMainMenuList = s_pstMainMenuList;

	while (pstMainMenuList != NULL)
	{
		if (lIndex == pstMainMenuList->stMainMenuItem.lOrder)
		{
			return &pstMainMenuList->stMainMenuItem;
		}

		pstMainMenuList = pstMainMenuList->pNext;
	}

	return NULL;
}


HUINT32		NX_PRISM_AppNaviCntxt_GetTotalMainMenuItemNum(void)
{
	HUINT32			ulCount = 0;
	NxUiMainMenuCfg_List	*pstMainMenuList = s_pstMainMenuList;

	while (pstMainMenuList != NULL)
	{
		ulCount++;
		pstMainMenuList = pstMainMenuList->pNext;
	}

	return	ulCount;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

