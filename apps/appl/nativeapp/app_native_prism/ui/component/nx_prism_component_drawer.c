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
 * @file	  		nx_prism_object_drawer.c
 *
 * 	Description:  	Object Drawer Funtion							\n
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
#include <nx_common.h>
#include <nx_prism_app.h>
#include <nx_prism_util.h>
#include <nx_prism_ui.h>
#include <nx_core.h>
#include <nx_prism_menu_contents.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_COMP_LISTAPP_PAGEITEM_MAX				9
#define	NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_X				18
#define	NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_Y				8

typedef enum
{
	eNxNaviItem_Item_1 = 0,
	eNxNaviItem_Item_2,
	eNxNaviItem_Item_3,
	eNxNaviItem_Item_4,
	eNxNaviItem_Item_5,
	eNxNaviItem_Item_6,
	eNxNaviItem_Item_7,
	eNxNaviItem_Item_8,
	eNxNaviItem_Item_9,
	eNxNaviItem_Item_10,
	eNxNaviItem_Item_Max
} NX_PRISM_NaviItem_t;

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
HUINT8		szCurrentDateInfoStr[NX_TEXT_SIZE_32] = {0, };
HUINT8		szCurrentTimeInfoStr[NX_TEXT_SIZE_32] = {0, };


static Item_t	s_NxPrismNaviMainItem[NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS]={
	{ NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_X, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_HEIGHT } },
};


static Item_t	s_NxPrismNaviMainItem2Depth[NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS]={
	{ NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN01_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN02_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN03_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN04_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN05_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN06_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN07_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN08_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN09_CURSOR_HEIGHT } },
	{ NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_ID, { NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_X + NX_PRISM_UI_MENU_MAINITEM2DTH_BTN_CURSOR_TAB, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_Y, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_WIDTH, NX_PRISM_UI_MENU_MAINITEM_BTN10_CURSOR_HEIGHT } },
};


static Item_t	s_NxPrismLeafSubItem[NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS]={
	{ NX_PRISM_UI_MENU_SUBITEM_BTN01_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN01_X, NX_PRISM_UI_MENU_SUBITEM_BTN01_Y, NX_PRISM_UI_MENU_SUBITEM_BTN01_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN01_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN02_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN02_X, NX_PRISM_UI_MENU_SUBITEM_BTN02_Y, NX_PRISM_UI_MENU_SUBITEM_BTN02_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN02_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN03_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN03_X, NX_PRISM_UI_MENU_SUBITEM_BTN03_Y, NX_PRISM_UI_MENU_SUBITEM_BTN03_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN03_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN04_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN04_X, NX_PRISM_UI_MENU_SUBITEM_BTN04_Y, NX_PRISM_UI_MENU_SUBITEM_BTN04_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN04_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN05_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN05_X, NX_PRISM_UI_MENU_SUBITEM_BTN05_Y, NX_PRISM_UI_MENU_SUBITEM_BTN05_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN05_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN06_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN06_X, NX_PRISM_UI_MENU_SUBITEM_BTN06_Y, NX_PRISM_UI_MENU_SUBITEM_BTN06_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN06_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN07_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN07_X, NX_PRISM_UI_MENU_SUBITEM_BTN07_Y, NX_PRISM_UI_MENU_SUBITEM_BTN07_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN07_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN08_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN08_X, NX_PRISM_UI_MENU_SUBITEM_BTN08_Y, NX_PRISM_UI_MENU_SUBITEM_BTN08_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN08_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN09_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN09_X, NX_PRISM_UI_MENU_SUBITEM_BTN09_Y, NX_PRISM_UI_MENU_SUBITEM_BTN09_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN09_HEIGHT } },
	{ NX_PRISM_UI_MENU_SUBITEM_BTN10_ID, { NX_PRISM_UI_MENU_SUBITEM_BTN10_X, NX_PRISM_UI_MENU_SUBITEM_BTN10_Y, NX_PRISM_UI_MENU_SUBITEM_BTN10_WIDTH, NX_PRISM_UI_MENU_SUBITEM_BTN10_HEIGHT } },
};


static HUINT8	*s_NxPrismNaviSubItemTextList[NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,};


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_Settings_____________________________________
/******************************************************************************/

void	NX_PRISM_COMP_Settings_InitNaviMainItems1Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HUINT32 ulMaxMainItem, GWM_ObjDraw_t objDrawMethod)
{
	HUINT32					i = 0, ulIndex = 0;
	ONDK_Rect_t				stRect = { 0, 0, 0, 0 };
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	/* init subitems until max count */
	for(i = 0; i < ulMaxMainItem; i++)
	{
		pMainItem = &(pMainItemList[i]);

		if (pMainItem->bHide == TRUE)
		{
			continue;
		}

		stRect = ONDK_Rect(s_NxPrismNaviMainItem[ulIndex].rect.x, s_NxPrismNaviMainItem[ulIndex].rect.y, s_NxPrismNaviMainItem[ulIndex].rect.w, s_NxPrismNaviMainItem[ulIndex].rect.h);
		ONDK_GWM_COM_Button_Create(pMainItem->ulMainBtnId, &stRect, (HCHAR*)pMainItem->ucTitle);
		ONDK_GWM_COM_Button_SetExtraText(pMainItem->ulMainBtnId, (HUINT8*)pMainItem->ucSubItemString);
		if (pMainItem->bDisable == TRUE)
		{
			ONDK_GWM_Obj_DisableObject(pMainItem->ulMainBtnId);
		}
		ONDK_GWM_APP_SetObjectDrawMethod(pMainItem->ulMainBtnId, objDrawMethod);

		ulIndex++;
	}
}


HERROR	NX_PRISM_COMP_Settings_InitNaviMainSubItemList1Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HINT32 lMainItemIndex, HBOOL bFirst)
{
	HUINT32					i = 0;
	ONDK_Rect_t				stRect = { 0, 0, 0, 0 };
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;


	if (pMainItemList == NULL)
	{
		return	ERR_FAIL;
	}

	pMainItem = &(pMainItemList[lMainItemIndex]);

	for (i = 0; i < NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS; i++)
	{
		if (i < pMainItem->ulNumOfSubItem)
		{
			s_NxPrismNaviSubItemTextList[i] = pMainItem->subItem[i].ucTitle;
		}
		else
		{
			s_NxPrismNaviSubItemTextList[i] = NULL;
		}
	}

	if (bFirst == TRUE)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_X, NX_PRISM_UI_MENU_MAINITEM_SUBLIST_Y,
							NX_PRISM_UI_MENU_MAINITEM_SUBLIST_WIDTH, NX_PRISM_UI_MENU_MAINITEM_SUBLIST_HEIGHT);

		ONDK_GWM_List_Create(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, &stRect, "Navi_SubItemList1Depth", NX_PRISM_SETTINGS_TEXT_LS_60, pMainItem->ulNumOfSubItem,
												NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS, (GWM_LIST_ITEMS)s_NxPrismNaviSubItemTextList);
		ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviSubItemList_1Depth);
		ONDK_GWM_Obj_DisableObject(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID);
	}
	else
	{
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID), (int)pMainItem->ulNumOfSubItem, (GWM_LIST_ITEMS)s_NxPrismNaviSubItemTextList);
	}

	return ERR_OK;
}


HERROR		NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(Nx_SettingsNaviContext_t *pstContents, HINT32 lSubItemObjId, HBOOL bFindNext, HINT32 lDirection)
{
	HINT32		lPrevIdx = 0, lSubItemIdx = -1;

	if ((pstContents == NULL) || (pstContents->pConvFunc == NULL))
	{
		return ERR_FAIL;
	}

	lSubItemIdx = pstContents->pConvFunc(lSubItemObjId);

	if (bFindNext == TRUE)
	{
		lPrevIdx = pstContents->lCurFocus;
		pstContents->lCurFocus = NX_PRISM_COMP_Settings_NaviGetNextFocusIdx(pstContents, lPrevIdx, lDirection);
	}
	else
	{
		pstContents->lCurFocus = lSubItemIdx;
	}

	NX_PRISM_COMP_Settings_InitNaviMainSubItemList1Depth(pstContents->mainItems, pstContents->lCurFocus, FALSE);
	ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);

	return ERR_OK;
}


void	NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HUINT32 ulMaxMainItem, GWM_ObjDraw_t objDrawMethod)
{
	HUINT32					i = 0, ulIndex = 0;
	ONDK_Rect_t				stRect = { 0, 0, 0, 0 };
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	/* init subitems until max count */
	for (i = 0; i < ulMaxMainItem; i++)
	{
		pMainItem = &(pMainItemList[i]);

		if (pMainItem->bHide == TRUE)
		{
			continue;
		}

		stRect = ONDK_Rect(s_NxPrismNaviMainItem2Depth[ulIndex].rect.x, s_NxPrismNaviMainItem2Depth[ulIndex].rect.y, s_NxPrismNaviMainItem2Depth[ulIndex].rect.w, s_NxPrismNaviMainItem2Depth[ulIndex].rect.h);
		ONDK_GWM_COM_Button_Create(pMainItem->ulMainBtnId, &stRect, (HCHAR*)pMainItem->ucTitle);
		ONDK_GWM_COM_Button_SetExtraText(pMainItem->ulMainBtnId, (HUINT8*)pMainItem->ucSubItemString);
		if (pMainItem->bDisable == TRUE)
		{
			ONDK_GWM_Obj_DisableObject(pMainItem->ulMainBtnId);
		}
		ONDK_GWM_APP_SetObjectDrawMethod(pMainItem->ulMainBtnId, objDrawMethod);

		ulIndex++;
	}
}


HERROR	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(Nx_SettingsNaviMainItem_t *pMainItemList, HINT32 lMainItemIndex, HBOOL bFirst)
{
	HUINT32					i = 0;
	ONDK_Rect_t				stRect = { 0, 0, 0, 0 };
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;


	if (pMainItemList == NULL)
	{
		return	ERR_FAIL;
	}

	pMainItem = &(pMainItemList[lMainItemIndex]);

	for (i = 0; i < NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS; i++)
	{
		if (i < pMainItem->ulNumOfSubItem)
		{
			s_NxPrismNaviSubItemTextList[i] = pMainItem->subItem[i].ucTitle;
		}
		else
		{
			s_NxPrismNaviSubItemTextList[i] = NULL;
		}
	}

	if (bFirst == TRUE)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_X - NX_PRISM_UI_MENU_MAINITEM2DTH_SUBLIST_TAB, NX_PRISM_UI_MENU_MAINITEM_SUBLIST_Y,
							NX_PRISM_UI_MENU_MAINITEM_SUBLIST_WIDTH, NX_PRISM_UI_MENU_MAINITEM_SUBLIST_HEIGHT);
		ONDK_GWM_List_Create(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, &stRect, "Navi_SubItemList2Depth", NX_PRISM_SETTINGS_TEXT_LS_60, pMainItem->ulNumOfSubItem,
																NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS, (GWM_LIST_ITEMS)s_NxPrismNaviSubItemTextList);
		ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, pMainItem->lSubItemValue);
		ONDK_GWM_List_SetSelectedIndex(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, pMainItem->lSubItemValue);
		ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID,eFont_SystemNormal,NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviSubItemList_2Depth);
		ONDK_GWM_Obj_DisableObject(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID);
	}
	else
	{
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_MAINITEM_SUBLIST_ID), (int)pMainItem->ulNumOfSubItem, (GWM_LIST_ITEMS)s_NxPrismNaviSubItemTextList);
	}

	return ERR_OK;
}


HERROR		NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(Nx_SettingsNaviContext_t *pstContents, HINT32 lSubItemObjId, HBOOL bFindNext, HINT32 lDirection)
{
	HINT32		lPrevIdx = 0, lSubItemIdx = -1;

	if ((pstContents == NULL) || (pstContents->pConvFunc == NULL))
	{
		return ERR_FAIL;
	}

	lSubItemIdx = pstContents->pConvFunc(lSubItemObjId);

	if (bFindNext == TRUE)
	{
		lPrevIdx = pstContents->lCurFocus;
		pstContents->lCurFocus = NX_PRISM_COMP_Settings_NaviGetNextFocusIdx(pstContents, lPrevIdx, lDirection);
	}
	else
	{
		pstContents->lCurFocus = lSubItemIdx;
	}

	ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);

	return ERR_OK;
}


HINT32		NX_PRISM_COMP_Settings_NaviGetNextFocusIdx(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjIdx, HINT32 lDirection)
{
	HBOOL		bWrapped = FALSE;
	HINT32		i = 0;
	HUINT32		lResult = lObjIdx;

	/* check parameter */
	if ((pstContents == NULL) || (lDirection == 0) || (lObjIdx < 0) || (lObjIdx >= pstContents->ulNumOfMainItem))
	{
		/* 오류이면 그냥 인풋을 바이패스한다. */
		return	lObjIdx;
	}

	/* wrap around 해서 찾아준다. */
	for (i = lObjIdx + lDirection;/* nothing */;/* get next */ i += ((lDirection > 0)? (1) : (-1)))
	{
		if (((lDirection > 0) && (i >= pstContents->ulNumOfMainItem)) || ((lDirection < 0) && (i < 0)))
		{
			if (bWrapped == FALSE)
			{
				bWrapped = TRUE;
				i = ((lDirection > 0)? (0) : (pstContents->ulNumOfMainItem - 1));
			}
			else
			{
				/* already wrapped, so cant find, error */
				break;
			}
		}

		/* Hide Item */
		if (pstContents->mainItems[i].bHide == TRUE)
		{
			continue;
		}

		/* bDisable이 FALSE이면 focus를 가질 수 있다. */
		if (pstContents->mainItems[i].bDisable == FALSE)
		{
			/* find */
			lResult = i;
			break;
		}
	}

	/* 찾으면 next idx, 못찾으면 그냥 인풋을 바이패스한다. */
	return	lResult;
}


void	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents (void *pvContents, GWM_ObjDraw_t pfDrawMethod)
{
	HUINT32 				 ulSubitemIdx = 0;
	BaseMenuContents			*pstContents = NULL;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL && pstContents->subItems != NULL)
	{
		for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
		{
			pstSubItem = &(pstContents->subItems[ulSubitemIdx]);
			pstSubItem->rect = ONDK_Rect(s_NxPrismLeafSubItem[ulSubitemIdx].rect.x, s_NxPrismLeafSubItem[ulSubitemIdx].rect.y, s_NxPrismLeafSubItem[ulSubitemIdx].rect.w + NX_PRISM_UI_MENU_SUBITEM_ICON_MG, s_NxPrismLeafSubItem[ulSubitemIdx].rect.h);

			ONDK_GWM_COM_Button_Create (pstSubItem->ulId, &(pstSubItem->rect), (char*)pstSubItem->szSubItemText);
			ONDK_GWM_COM_Button_SetExtraText (pstSubItem->ulId, pstSubItem->szMainItemText);
			ONDK_GWM_APP_SetObjectDrawMethod (pstSubItem->ulId, pfDrawMethod);

			if (pstSubItem->bDisable == TRUE)
			{
				ONDK_GWM_Obj_DisableObject(pstSubItem->ulId);
			}
		}
	}
}


void	NX_PRISM_COMP_Settings_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_80));
}


void	NX_PRISM_COMP_Settings_DrawTitle(HUINT8 *pTitle)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_X, NX_PRISM_UI_SET_TITLE_Y, NX_PRISM_UI_SET_TITLE_WIDTH, NX_PRISM_UI_SET_TITLE_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SET_TITLE_ID, &stRect, (HCHAR*)pTitle);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SET_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SET_TITLE_ID, NX_PRISM_FONT_SIZE_46);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SET_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SET_TITLE_ID, COL(C_T_Gong01), COL(C_T_Gong01));
}


void	NX_PRISM_COMP_Settings_DrawCurrentDateTime(HUINT8 *pszDate, HUINT8 *pszTime)
{
	UNIXTIME		ulUnixTime = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HERROR			hError = ERR_FAIL;

	hError = NX_SYSTEM_GetTime(&ulUnixTime);
	if (hError != ERR_OK)
	{
		NX_APP_Info("[%s:%d] Error!!! NX_SYSTEM_GetTime() \n", __FUNCTION__, __HxLINE__);
		return;
	}

	if ((pszDate == NULL) || (pszTime == NULL))
	{
		NX_APP_Error("Error!!! pszDate == NULL or pszTime == NULL\n");
		return;
	}

	stRect = ONDK_Rect(NX_PRISM_UI_SET_CUR_DATE_X, NX_PRISM_UI_SET_CUR_DATE_Y, NX_PRISM_UI_SET_CUR_DATE_WIDTH, NX_PRISM_UI_SET_CUR_DATE_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SET_CUR_DATE_ID, &stRect, (HCHAR*)pszDate);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SET_CUR_DATE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SET_CUR_DATE_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SET_CUR_DATE_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SET_CUR_DATE_ID, COL(C_T_Gong01), COL(C_T_Gong01));

	stRect = ONDK_Rect(NX_PRISM_UI_SET_CUR_TIME_X, NX_PRISM_UI_SET_CUR_TIME_Y, NX_PRISM_UI_SET_CUR_TIME_WIDTH, NX_PRISM_UI_SET_CUR_TIME_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SET_CUR_TIME_ID, &stRect, (HCHAR*)pszTime);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SET_CUR_TIME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SET_CUR_TIME_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SET_CUR_TIME_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SET_CUR_TIME_ID, COL(C_T_Gong01), COL(C_T_Gong01));
}


void	NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(HUINT8 *pszDate, HUINT8 *pszTime)
{
	UNIXTIME		ulUnixTime = 0;
	HERROR			hError = ERR_FAIL;
	ONDK_GWM_GuiObject_t	programmeImg = NULL;

	hError = NX_SYSTEM_GetTime(&ulUnixTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] Error!!! NX_SYSTEM_GetTime() \n", __FUNCTION__, __HxLINE__);
		return;
	}

	programmeImg = ONDK_GWM_APP_GetObject(NX_PRISM_UI_SET_CUR_DATE_ID);
	if (programmeImg != NULL)
	{
		ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_SET_CUR_DATE_ID, (HCHAR*)pszDate);
	}

	programmeImg = ONDK_GWM_APP_GetObject(NX_PRISM_UI_SET_CUR_TIME_ID);
	if (programmeImg != NULL)
	{
		ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_SET_CUR_TIME_ID, (HCHAR*)pszTime);
	}

}

#if 0
void	NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(void)
{
	HUINT8			*szStr = NULL;
	ONDK_GWM_GuiObject_t	stTextObject;

	// TEXT - DATE
	stTextObject = ONDK_GWM_APP_GetObject(NX_PRISM_UI_SET_CUR_DATE_ID);
	szStr = (HUINT8*)ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stTextObject);
	if (szStr != NULL)
	{
		//HxSTD_sprintf ((char*)szStr, (char*)"%s", pstSubItem.szMainItemText);
		ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_SET_CUR_DATE_ID, (HCHAR*)szStr);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_SET_CUR_DATE_ID);
	}

	// TEXT - TIME
	stTextObject = ONDK_GWM_APP_GetObject(NX_PRISM_UI_SET_CUR_TIME_ID);
	szStr = (HUINT8*)ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stTextObject);
	if (szStr != NULL)
	{
		//HxSTD_sprintf ((char*)szStr, (char*)"%s", pstSubItem.szMainItemText);
		ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_SET_CUR_TIME_ID, (HCHAR*)szStr);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_SET_CUR_TIME_ID);
	}
}
#endif

void	NX_PRISM_COMP_Settings_DrawTitleLine(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_LINE_X, NX_PRISM_UI_SET_TITLE_LINE_Y, NX_PRISM_UI_SET_TITLE_LINE_WIDTH, NX_PRISM_UI_SET_TITLE_LINE_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_SET_TITLE_LINE_ID, &stRect, COL(C_G_Whistle_30));
}


void	NX_PRISM_COMP_Settings_DrawCursor(ONDK_Rect_t *pRect)
{
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_MAINITEM_CURSOR_ID, pRect, COL(C_G_Base01_100));
}


void	NX_PRISM_COMP_Settings_DrawCursorArrowImage(ONDK_Rect_t *pRect)
{
	ONDK_GWM_COM_Image_Create(NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID, pRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID, (HCHAR*)RES_106_00_ARROW_RIGHT_C_PNG);
	ONDK_GWM_Obj_SetVisible((NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID);
}


void	NX_PRISM_COMP_Settings_DrawUpdateCursor(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					nSubItemIdx = -1, i = 0, lMainItemTextId = -1;
	HUINT32			 		ulColor = 0;
	ONDK_Rect_t 			stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t 			eFontId = eFont_SystemNormal;


	ONDK_GWM_Obj_GetObjectRect(lObjId, &stRect);

	// Re-Draw Curosr
	ONDK_GWM_Obj_Resize(NX_PRISM_UI_MENU_MAINITEM_CURSOR_ID, &stRect);

	// Re-Draw Arrow image
	stRect = ONDK_Rect(stRect.x + NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_X_TAB, stRect.y+ NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_Y_TAB,
						NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_WIDTH, NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_HEIGHT);
	ONDK_GWM_Obj_Resize(NX_PRISM_UI_MENU_MAINITEM_ARROW_IMAGE_ID, &stRect);

	nSubItemIdx = pstContents->pConvFunc(lObjId);

	for (i = 0; i < NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS; i++)
	{
		lMainItemTextId = (s_NxPrismNaviMainItem[i].ulId|NX_PRISM_UI_MENU_MAINITEM_TEXT_ID);
		if (i == nSubItemIdx)
		{
			eFontId = eFont_SystemBold;
			ulColor = C_T_Bang_100;
		}
		else
		{
			eFontId = eFont_SystemNormal;
			ulColor = C_T_Whistle_100;
		}
		ONDK_GWM_COM_Text_SetFont(lMainItemTextId, eFontId);
		ONDK_GWM_COM_Text_ChangeColor(lMainItemTextId, COL(ulColor), COL(ulColor));
	}
}

ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Value(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*szTitle = NULL, *szExtraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* get text  - Item Value*/
	szTitle = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	szExtraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (szExtraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	ulFontSize = NX_PRISM_FONT_SIZE_26;
	eFontId = eFont_SystemNormal;
	lCursorMg = 0;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, NX_PRISM_UI_MENU_MAINITEM_TEXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

        // Value
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szExtraText, x + NX_PRISM_UI_MENU_SUBITEM_SUBTXT_X_TAB, y, NX_PRISM_UI_MENU_SUBITEM_SUBTXT_WIDTH-(NX_PRISM_UI_MENU_MAINITEM_TEXT_X_TAB), value_fontColor, ONDKSTF_RIGHT);

		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*szTitle = NULL, *szExtraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* get text  - Item Value*/
	szTitle = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	szExtraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (szExtraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	ulFontSize = NX_PRISM_FONT_SIZE_26;
	eFontId = eFont_SystemNormal;
	lCursorMg = 0;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, NX_PRISM_UI_MENU_MAINITEM_TEXT_WIDTH, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*szTitle = NULL, *szExtraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* get text  - Item Value*/
	szTitle = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	szExtraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (szExtraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	ulFontSize = NX_PRISM_FONT_SIZE_26;
	eFontId = eFont_SystemNormal;
	lCursorMg = 0;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// right arrow
			stRect = ONDK_Rect(x1 + w - NX_PRISM_UI_MENU_MAINITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_MAINITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_MAINITEM_ICON_WIDTH, NX_PRISM_UI_MENU_MAINITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// right arrow
			stRect = ONDK_Rect(x1 + w - NX_PRISM_UI_MENU_MAINITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_MAINITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_MAINITEM_ICON_WIDTH, NX_PRISM_UI_MENU_MAINITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_C_PNG);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_MAINITEM_TEXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, NX_PRISM_UI_MENU_MAINITEM_TEXT_WIDTH, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviSubItemList_1Depth(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lNumOfPage = 0;
	HINT32		lCellHeight = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT8		**pList = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDKFontHandle	hFontHandle;
	LIST_OBJECT		obj = (LIST_OBJECT)object;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL(C_T_Gong03);

	pList = (HUINT8	**)ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	if ((pList == NULL) || (lTotal == 0) || (lTotal > NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS))
	{
		return ONDK_RET_FAIL;
	}

	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lText_y_off = NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_Y;

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_FAIL;
	}

	for(i = 0; i < lTotal; i++)
	{
		x = x1 + NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pList[i], x, y, w, title_fontColor, ONDKSTF_LEFT);
		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ERR_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawNaviSubItemList_2Depth(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lNumOfPage = 0;
	HINT32		lCellHeight = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT8		**pList = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDKFontHandle	hFontHandle;
	LIST_OBJECT		obj = (LIST_OBJECT)object;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL(C_T_Gong03);

	pList = (HUINT8	**)ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	if ((pList == NULL) || (lTotal == 0) || (lTotal > NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS))
	{
		return ONDK_RET_FAIL;
	}

	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lText_y_off = NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_Y;

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_FAIL;
	}

	for (i = 0; i < lTotal; i++)
	{
		if (i + lStart == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Whistle_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - NX_PRISM_SETTINGS_CURSOR_MG, COL(C_G_Base01_20));
		}
		else
		{
			title_fontColor = COL(C_T_Gong03);
		}

		x = x1 + NX_PRISM_UI_MENU_MAINITEM_SUBLIST_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off;
		y = y1 + lCursor_y_off + lText_y_off;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pList[i], x, y, w, title_fontColor, ONDKSTF_LEFT);
		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ERR_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			// stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			// ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			// stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			// ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_ARROW_RIGHT_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, NX_PRISM_UI_MENU_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

		// SubItem - Value
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x + NX_PRISM_UI_MENU_SUBITEM_SUBTXT_X_TAB, y, NX_PRISM_UI_MENU_SUBITEM_SUBTXT_WIDTH, value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, NX_PRISM_UI_MENU_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

		// SubItem - Value
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x + NX_PRISM_UI_MENU_SUBITEM_SUBTXT_X_TAB - 30, y, NX_PRISM_UI_MENU_SUBITEM_SUBTXT_WIDTH + 30, value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_B1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Gong01);

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Gong03);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, NX_PRISM_UI_MENU_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

		// SubItem - Value
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x + NX_PRISM_UI_MENU_SUBITEM_SUBTXT_X_TAB, y, NX_PRISM_UI_MENU_SUBITEM_SUBTXT_WIDTH, value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawStrengthBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text)
{
	// Title
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_STRENGTH_TITLE_ID, rectTitle, (HCHAR*)ONDK_GetString(RES_STRENGTH_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_STRENGTH_TITLE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_STRENGTH_TITLE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_STRENGTH_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_STRENGTH_TITLE_ID, COL(C_T_Orgel01), COL(C_T_Orgel01));

	// Bar
	ONDK_GWM_COM_Status_Create(NX_PRISM_UI_BAR_STRENGTH_BAR_ID, rectBar, 100, 0);
	ONDK_GWM_COM_Status_SetColor(NX_PRISM_UI_BAR_STRENGTH_BAR_ID, COL(C_G_Whistle_20), COL(C_G_Orgel02));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_BAR_STRENGTH_BAR_ID, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);

	// Value
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, rectValue, (HCHAR*)text);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, COL(C_T_Orgel01), COL(C_T_Orgel01));

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawStrenghtBar(HUINT32 ulStrength)
{
	HUINT8	*szStrengthStr = NULL;
	ONDK_GWM_GuiObject_t	UiObj = NULL;

	// Update Level
	ONDK_GWM_COM_Status_SetValue(NX_PRISM_UI_BAR_STRENGTH_BAR_ID, ulStrength);

	// Update Level Text
	UiObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_BAR_STRENGTH_VALUE_ID);
	if (UiObj)
	{
		szStrengthStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
		if (szStrengthStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szStrengthStr, NX_TEXT_SIZE_8, (HCHAR*)"%d%%", (int)ulStrength);
			ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_BAR_STRENGTH_VALUE_ID, (HCHAR*)szStrengthStr);
		}
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawQualitytBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text)
{
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_QUALITY_TITLE_ID, rectTitle, (HCHAR*)ONDK_GetString(RES_QUALITY_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_QUALITY_TITLE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_QUALITY_TITLE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_QUALITY_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_QUALITY_TITLE_ID, COL(C_T_Groove01), COL(C_T_Groove01));

	// Bar
	ONDK_GWM_COM_Status_Create(NX_PRISM_UI_BAR_QUALITY_BAR_ID, rectBar, 100, 0);
	ONDK_GWM_COM_Status_SetColor(NX_PRISM_UI_BAR_QUALITY_BAR_ID, COL(C_G_Whistle_20), COL(C_G_Groove01));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_BAR_QUALITY_BAR_ID, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);

	// Value
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_QUALITY_VALUE_ID, rectValue, (HCHAR*)text);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_QUALITY_VALUE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_QUALITY_VALUE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_QUALITY_VALUE_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_QUALITY_VALUE_ID, COL(C_T_Groove01), COL(C_T_Groove01));

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawQualityBar(HUINT32 ulQuality)
{
	HUINT8	*szQualityStr = NULL;
	ONDK_GWM_GuiObject_t	UiObj = NULL;

	// Update Level
	ONDK_GWM_COM_Status_SetValue (NX_PRISM_UI_BAR_QUALITY_BAR_ID, ulQuality);

	// Update Level Text
	UiObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_BAR_QUALITY_VALUE_ID);
	if (UiObj)
	{
		szQualityStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
		if (szQualityStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szQualityStr, NX_TEXT_SIZE_8, (HCHAR*)"%d%%", (int)ulQuality);
			ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_BAR_QUALITY_VALUE_ID, (HCHAR*)szQualityStr);
		}
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_CreateDrawBERBar(ONDK_Rect_t *rectTitle, ONDK_Rect_t *rectBar, ONDK_Rect_t *rectValue, HCHAR *text)
{
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_BER_TITLE_ID, rectTitle, (HCHAR*)ONDK_GetString(RES_BER_03_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_BER_TITLE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_BER_TITLE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_BER_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_BER_TITLE_ID, COL(C_T_Rythme01), COL(C_T_Rythme01));

	// Bar
	ONDK_GWM_COM_Status_Create(NX_PRISM_UI_BAR_BER_BAR_ID, rectBar, 100, 0);
	ONDK_GWM_COM_Status_SetColor(NX_PRISM_UI_BAR_BER_BAR_ID, COL(C_G_Whistle_20), COL(C_T_Rythme01));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_BAR_BER_BAR_ID, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);

	// Value
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_BAR_BER_VALUE_ID, rectValue, (HCHAR*)text);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_BAR_BER_VALUE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_BAR_BER_VALUE_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_BAR_BER_VALUE_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_BAR_BER_VALUE_ID, COL(C_T_Rythme01), COL(C_T_Rythme01));

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_Settings_UpdateDrawBERBar(HUINT32 ulBER, HINT32 lDecimal, HINT32 lFloat, HINT32 lExp)
{
	HUINT8	*szQualityStr = NULL;
	ONDK_GWM_GuiObject_t	UiObj = NULL;

	// Update Level
	ONDK_GWM_COM_Status_SetValue (NX_PRISM_UI_BAR_BER_BAR_ID, ulBER);

	// Update Level Text
	UiObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_BAR_BER_VALUE_ID);
	if (UiObj)
	{
		szQualityStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
		if (szQualityStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szQualityStr, NX_TEXT_SIZE_8, "%d.%dE-%d", lDecimal, lFloat, lExp);
			ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_BAR_BER_VALUE_ID, (HCHAR*)szQualityStr);
		}
	}

	return	ONDK_RET_OK;
}


void	NX_PRISM_COMP_Settings_DrawHelpText(HUINT8 *pszMessage)
{
	HINT32			lLineCnt = 0, y = 0, h = 0;
	ONDK_Rect_t 	stRect = {0, 0, 0, 0};

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pszMessage, NX_PRISM_UI_SET_HELP_TEXT_WIDTH);

	y = NX_PRISM_UI_SET_HELP_TEXT_Y - (NX_PRISM_UI_SET_HELP_TEXT_LS * lLineCnt);
	h = NX_PRISM_UI_SET_HELP_TEXT_HEIGHT + (NX_PRISM_UI_SET_HELP_TEXT_LS * lLineCnt);

	stRect = ONDK_Rect(NX_PRISM_UI_SET_HELP_TEXT_X, y, NX_PRISM_UI_SET_HELP_TEXT_WIDTH, h + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SET_HELP_TEXT_ID, &stRect, (HCHAR*)pszMessage);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SET_HELP_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SET_HELP_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SET_HELP_TEXT_ID, (TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SET_HELP_TEXT_ID, COL(C_T_Yoyo01), COL(C_T_Bang_100));
}

void	NX_PRISM_COMP_Settings_UpdateHelpText(HUINT8 *pszMessage)
{
	ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_SET_HELP_TEXT_ID, (HCHAR*)pszMessage);
}

void	NX_PRISM_COMP_Settings_UpdateHelpLocation(HUINT8 *pszMessage)
{
	HINT32			lLineCnt = 0, y = 0, h = 0;
	ONDK_Rect_t 	stRect = {0, 0, 0, 0};

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pszMessage, NX_PRISM_UI_SET_HELP_TEXT_WIDTH);

	y = NX_PRISM_UI_SET_HELP_TEXT_Y - (NX_PRISM_UI_SET_HELP_TEXT_LS * lLineCnt);
	h = NX_PRISM_UI_SET_HELP_TEXT_HEIGHT + (NX_PRISM_UI_SET_HELP_TEXT_LS * lLineCnt);

	stRect = ONDK_Rect(NX_PRISM_UI_SET_HELP_TEXT_X, y, NX_PRISM_UI_SET_HELP_TEXT_WIDTH, h + NX_PRISM_FONT_VSPACE);

	ONDK_GWM_COM_Text_ChangeLocation(NX_PRISM_UI_SET_HELP_TEXT_ID, &stRect);
}

void	NX_PRISM_COMP_Settings_DrawBackgroundPopUp(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_X, NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_Y, NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_POPUP_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_95));
}


void	NX_PRISM_COMP_Settings_DrawTitlePopUp(HUINT8 *pTitle)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_POPUP_SET_TITLE_X, NX_PRISM_UI_MENU_POPUP_SET_TITLE_Y, NX_PRISM_UI_MENU_POPUP_SET_TITLE_WIDTH, NX_PRISM_UI_MENU_POPUP_SET_TITLE_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID, &stRect, (HCHAR*)pTitle);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MENU_POPUP_SET_TITLE_ID, COL(C_T_Gong01), COL(C_T_Gong01));
}

ONDK_Result_t	NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, lTextValueX = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if(extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Gong01);

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Gong03);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Gong03);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

		// SubItem - Value
		lTextValueX = x + NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_WIDTH + NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_X_TAB;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, lTextValueX, y, NX_PRISM_UI_MENU_POPUP_SUBITEM_SUBTXT_WIDTH, value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, lTextValueX = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if(extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	value_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Gong01);

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Gong03);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_MENU_SUBITEM_ICON_X_TAB, y1 + NX_PRISM_UI_MENU_SUBITEM_ICON_Y_TAB, NX_PRISM_UI_MENU_SUBITEM_ICON_WIDTH, NX_PRISM_UI_MENU_SUBITEM_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			value_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Gong03);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);

		// SubItem - Value
		lTextValueX = x + NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_WIDTH + NX_PRISM_UI_MENU_POPUP_SUBITEM_MAINTXT_X_TAB;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, lTextValueX, y, NX_PRISM_UI_MENU_POPUP_SUBITEM_SUBTXT_WIDTH, value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_Media__________________________________
/******************************************************************************/
void	NX_PRISM_COMP_Media_DrawTitle(HUINT8 *pTitle)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_X, NX_PRISM_UI_SET_TITLE_Y, NX_PRISM_UI_SET_TITLE_WIDTH, NX_PRISM_UI_SET_TITLE_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_TITLE_ID, &stRect, (HCHAR*)pTitle);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_TITLE_ID, NX_PRISM_FONT_SIZE_46);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_TITLE_ID, COL(C_T_Gong01), COL(C_T_Gong01));
}

void	NX_PRISM_COMP_Media_DrawTitleLine(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_LINE_X, NX_PRISM_UI_SET_TITLE_LINE_Y, NX_PRISM_UI_SET_TITLE_LINE_WIDTH, NX_PRISM_UI_SET_TITLE_LINE_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_TITLE_LINE_ID, &stRect, COL(C_G_Whistle_30));
}

/******************************************************************************/
#define _________Public_Functions_PopUp_Dialogue_Window________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_DrawWindowPopUp(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	stRect = ONDK_Rect(x1, y1, x2 - x1, y2 - y1);

	// TODO: Check 너무 어둡다. C_G_Bang_50 변경 필요
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Bang_85));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawWindowOptionLeft(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_105_00_BG_COM_CONTEXT_L01_PNG, RES_105_00_BG_COM_CONTEXT_L01_PNG, RES_105_00_BG_COM_CONTEXT_L01_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawWindowOptionRight(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_105_00_BG_COM_CONTEXT_R01_PNG, RES_105_00_BG_COM_CONTEXT_R01_PNG, RES_105_00_BG_COM_CONTEXT_R01_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawImageOptionLeftPointer(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_216_00_OPT_0POINTER_L_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawImageOptionRightPointer(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_216_00_OPT_0POINTER_R_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawDialugeWindow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	stRect = ONDK_Rect(x1, y1, x2 - x1, y2 - y1);
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Bang_90));

	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, NX_PRISM_UI_DIALOG_LINE_H, COL(C_G_Whistle_100));
	ONDK_DRAW_FillRectElement(screen, stRect.x, (y2 - NX_PRISM_UI_DIALOG_LINE_H), stRect.w, NX_PRISM_UI_DIALOG_LINE_H, COL(C_G_Whistle_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


void	NX_PRISM_COMP_DrawDialugeWindowMessage(ONDK_Rect_t *rect, HUINT8 *pMMessage, ONDK_Color_t color)
{
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DIALOG_MESSAGE_ID, rect, (HCHAR*)pMMessage);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DIALOG_MESSAGE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DIALOG_MESSAGE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DIALOG_MESSAGE_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED | TEXT_WORDWRAP ));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DIALOG_MESSAGE_ID, color, color);
}


ONDK_Result_t	NX_PRISM_COMP_DrawDialugeInputButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, i = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, lMaxDigit = 0, lPos = 0;
	HINT32		lInputMode = 0, lCharWidth = 0, fw = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HCHAR		*szText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	INPUT_OBJECT	objInput = (INPUT_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_32;
	lCursorMg = 0;

	szText = ONDK_GWM_COM_Input_GetTextPtrFromByObject(objInput);		/* get input value string */
	lMaxDigit = ONDK_GWM_COM_Input_GetDigitByObject(objInput);					/* get max digit boundary */
	lPos = ONDK_GWM_COM_Input_GetCurrentPositionByObject(objInput);			/* get current digit position */
	lInputMode = ONDK_GWM_COM_Input_IsPasswordModeByObject(objInput);		/* get string type */
	ulStatus = ONDK_GWM_COM_Input_IsFocusedByObject(objInput);

	y = y1;
	w = NX_PRISM_UI_DIALOG_BUTTON_INPUT_W;
	h = NX_PRISM_UI_DIALOG_BUTTON_INPUT_H;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	fw = ONDK_GWM_GFX_GetStringWidth(hFontHandle, "0");
	lText_x_offset = (w - fw) / 2;
	lText_y_offset = (h - ulFontSize) / 2;
	for (i = 0; i <lMaxDigit; i++)
	{
		cursorColor = (i == lPos) ? COL(C_G_Base01_100) : COL(C_G_Gong01);
		x = x1 + (i*NX_PRISM_UI_DIALOG_BUTTON_INPUT_W);
		ONDK_DRAW_FillRectElement(screen,x, y, w, h, cursorColor);

		title_fontColor = (i == lPos) ? COL(C_T_Bang_100) : COL(C_T_Gong02);
		// TEXT - Input Value
		if (hFontHandle)
		{
			ONDK_FONT_DrawOneCharacter(screen, hFontHandle, &szText[i], x + lText_x_offset, y + lText_y_offset, title_fontColor, &lCharWidth);
		}
	}
	ONDK_FONT_Release(hFontHandle);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawDCNInputButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, i = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, lMaxDigit = 0, lPos = 0;
	HINT32		lInputMode = 0, lCharWidth = 0, fw = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HCHAR		*szText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	INPUT_OBJECT	objInput = (INPUT_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_46;
	lCursorMg = 0;

	szText = ONDK_GWM_COM_Input_GetTextPtrFromByObject(objInput);		/* get input value string */
	lMaxDigit = ONDK_GWM_COM_Input_GetDigitByObject(objInput);					/* get max digit boundary */
	lPos = ONDK_GWM_COM_Input_GetCurrentPositionByObject(objInput);			/* get current digit position */
	lInputMode = ONDK_GWM_COM_Input_IsPasswordModeByObject(objInput);		/* get string type */
	ulStatus = ONDK_GWM_COM_Input_IsFocusedByObject(objInput);

	y = y1;
	w = NX_PRISM_UI_DCN_BUTTON_INPUT_W;
	h = NX_PRISM_UI_DCN_BUTTON_INPUT_H;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	fw = ONDK_GWM_GFX_GetStringWidth(hFontHandle, "0");
	lText_x_offset = (w - fw) / 2;
	lText_y_offset = (h - ulFontSize) / 2;
	for (i = 0; i <lMaxDigit; i++)
	{
//		cursorColor = (i == lPos) ? COL(C_G_Base01_100) : COL(C_G_Gong01);
		x = x1 + (i*NX_PRISM_UI_DCN_BUTTON_INPUT_W);
//		ONDK_DRAW_FillRectElement(screen,x, y, w, h, cursorColor);

		title_fontColor = COL(C_G_Base01_100);
		// TEXT - Input Value
		if (hFontHandle)
		{
			ONDK_FONT_DrawOneCharacter(screen, hFontHandle, &szText[i], x + lText_x_offset, y + lText_y_offset, title_fontColor, &lCharWidth);
		}
	}
	ONDK_FONT_Release(hFontHandle);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_OPTION_Mneu__________________________________
/******************************************************************************/

void	NX_PRISM_COMP_OPT_InitMenuContents (void *pvContents, GWM_ObjDraw_t pfDrawMethod)
{
	HUINT32 				 ulSubitemIdx = 0;
	BaseMenuContents			*pstContents = NULL;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstContents = (BaseMenuContents *)pvContents;
	if (pstContents != NULL && pstContents->subItems != NULL)
	{
		for (ulSubitemIdx = 0; ulSubitemIdx < pstContents->ulNumOfSubItem; ulSubitemIdx++)
		{
			pstSubItem = &(pstContents->subItems[ulSubitemIdx]);
			pstSubItem->rect.w;// += NX_PRISM_UI_MENU_SUBITEM_ICON_MG;

			ONDK_GWM_COM_Button_Create (pstSubItem->ulId, &(pstSubItem->rect), (char*)pstSubItem->szSubItemText);
			ONDK_GWM_COM_Button_SetExtraText (pstSubItem->ulId, pstSubItem->szMainItemText);
			ONDK_GWM_APP_SetObjectDrawMethod (pstSubItem->ulId, pfDrawMethod);

			if (pstSubItem->bDisable == TRUE)
			{
				ONDK_GWM_Obj_DisableObject(pstSubItem->ulId);
			}
		}
	}
}


ONDK_Result_t	NX_PRISM_COMP_OPT_DrawCursor_A1_Arrow(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, lText_Item_w = 0;

	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL, *extraText = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_OPT_MENU_ICON_MG;
	h = rect->h;

	/* get text  - Item Value*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);

	/* get text  - Item Name*/
	extraText = (HUINT8*)ONDK_GWM_COM_Button_GetExtraTextByObj(objBtn);
	if (extraText == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	title_fontColor = COL (C_T_Gong02);
	value_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);

	ulFontSize = NX_PRISM_FONT_SIZE_22;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_OPT_MENU_CURSOR_MG;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_10);
			value_fontColor = COL(C_T_Gong08_10);
			cursorColor = COL(C_G_Gong03_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_OPT_MENU_ICON_X_TAB, y1 + NX_PRISM_UI_OPT_MENU_ICON_Y_TAB,
								NX_PRISM_UI_OPT_MENU_ICON_WIDTH, NX_PRISM_UI_OPT_MENU_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);

			// right arrow
			stRect = ONDK_Rect(x1 + w + NX_PRISM_UI_OPT_MENU_ICON_X_TAB, y1 + NX_PRISM_UI_OPT_MENU_ICON_Y_TAB,
								NX_PRISM_UI_OPT_MENU_ICON_WIDTH, NX_PRISM_UI_OPT_MENU_ICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_RIGHT_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_10);
			value_fontColor = COL(C_T_Gong08_10);
			cursorColor = COL(C_G_Gong03_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_05);
			value_fontColor = COL(C_T_Whistle_05);
			cursorColor = COL(C_G_Gong01_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_OPT_MENU_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_OPT_MENU_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// Item - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;
		lText_Item_w = (w/2);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)extraText, x, y, (lText_Item_w - lText_x_offset), title_fontColor, ONDKSTF_LEFT);

		// Item - Value
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, (x + lText_Item_w - lText_x_offset), y, (lText_Item_w - lText_x_offset), value_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_OPT_DrawButton_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_24;
	lCursorMg = 0;

	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_10);
			cursorColor = COL(C_G_Gong03_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_05);
			cursorColor = COL(C_G_Gong01_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}

	}

	lText_x_offset = NX_PRISM_UI_BUTTION_TEXT_X;
	lText_y_offset = NX_PRISM_UI_BUTTION_TEXT_Y;
	text_w = w - (NX_PRISM_UI_BUTTION_TEXT_X*2);

	// TEXT
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		x = x1;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_Button_______________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_DrawButton_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_24;
	lCursorMg = 0;

	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
		}
		else
		{
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	lText_x_offset = NX_PRISM_UI_BUTTION_TEXT_X;
	lText_y_offset = NX_PRISM_UI_BUTTION_TEXT_Y;
	text_w = w - (NX_PRISM_UI_BUTTION_TEXT_X*2);

	// TEXT
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		x = x1;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawButton_B1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0;

	HUINT16		usFontSize = 0;
	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	HUINT32 push_status;
	ONDK_Size_t stImgSize;
	ONDKImageHandle		hNormalImg, hFocusedImg, hDisabledImg, hPushedImg;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);
	ulFontSize = NX_PRISM_FONT_SIZE_24;
	lCursorMg = 0;

	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	ONDK_GWM_COM_Button_GetTitleSizeByObj(objBtn, &usFontSize);
	if (usFontSize < 1)
	{
		ulFontSize = NX_PRISM_FONT_SIZE_24;
	}
	else
	{
		ulFontSize = (HUINT32)usFontSize;
	}

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Whistle_15);
		}
		else
		{
			title_fontColor = COL (C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	lText_x_offset = NX_PRISM_UI_BUTTION_TEXT_X;
	lText_y_offset = NX_PRISM_UI_BUTTION_TEXT_Y;
	text_w = w - (NX_PRISM_UI_BUTTION_TEXT_X*2);

	// TEXT - TP Info Title
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		x = x1;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	// Draw image in button
	ONDK_GWM_COM_Button_GetImageHandleByObj(objBtn, &hNormalImg, &hFocusedImg, &hDisabledImg, &hPushedImg);
	push_status = ONDK_GWM_Obj_GetObjectStatus((ONDK_GWM_GuiObject_t)objBtn);

	if (( push_status& GUIOBJ_DISABLED) && (hDisabledImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hDisabledImg);
		ONDK_IMG_Blit(hDisabledImg, screen, (x1+(w/2)-(stImgSize.w/2)), (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_PUSHED) && (hPushedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hPushedImg);
		ONDK_IMG_Blit(hPushedImg, screen, (x1+(w/2)-(stImgSize.w/2)), (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if ((push_status & GUIOBJ_HILIGHTED) && (hFocusedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hFocusedImg);
		ONDK_IMG_Blit(hFocusedImg, screen, (x1+(w/2)-(stImgSize.w/2)), (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}
	else if (hNormalImg!= NULL)
	{
		stImgSize = ONDK_IMG_GetSize(hNormalImg);
		ONDK_IMG_Blit(hNormalImg, screen, (x1+(w/2)-(stImgSize.w/2)), (y1+(h/2)-(stImgSize.h/2)));
		return ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawButton_C1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, text_w = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	cursorColor = COL(C_G_Whistle_15);
	title_fontColor = COL (C_T_Whistle_100);
	ulFontSize = NX_PRISM_FONT_SIZE_22;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			/* Disable Item cursor */
			cursorColor = COL(C_G_Whistle_05);
			title_fontColor = COL (C_T_Bang_100);
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			cursorColor = COL(C_G_Base01_100);
			title_fontColor = COL (C_T_Bang_100);
			eFontId = eFont_SystemBold;
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			cursorColor = COL(C_G_Base01_100);
			title_fontColor = COL (C_T_Bang_100);
			eFontId = eFont_SystemBold;
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			cursorColor = COL(C_G_Base01_20);
			title_fontColor = COL (C_T_Whistle_100);
			eFontId = eFont_SystemBold;
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			cursorColor = COL(C_G_Whistle_05);
			title_fontColor = COL (C_T_Bang_100);
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	lText_x_offset = NX_PRISM_UI_BUTTION_TEXT_X;
	lText_y_offset = NX_PRISM_UI_BUTTION_TEXT_Y;
	text_w = w - (NX_PRISM_UI_BUTTION_TEXT_X*2);

	// TEXT
	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		x = x1;
		y = y1 + lText_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawButton_Scroll_ArrowLBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32 		ulStatus = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;
	w = rect->w;
	h = rect->h;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	stRect = ONDK_Rect(x1, y1, w, h);

	 if (ulStatus & GUIOBJ_PUSHED)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_LEFT_C_PNG);
	}
	else if (ulStatus & GUIOBJ_HILIGHTED)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_LEFT_C_PNG);
	}
	else
	{
		/* Normal Item cursor */
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_LEFT_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}



ONDK_Result_t	NX_PRISM_COMP_DrawButton_Scroll_ArrowRBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32 		ulStatus = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;
	w = rect->w;
	h = rect->h;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	stRect = ONDK_Rect(x1, y1, w, h);

	 if (ulStatus & GUIOBJ_PUSHED)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_RIGHT_C_PNG);
	}
	else if (ulStatus & GUIOBJ_HILIGHTED)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_RIGHT_C_PNG);
	}
	else
	{
		/* Normal Item cursor */
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_106_00_ARROW_BOX_RIGHT_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_DrawButton_PinCodeButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x = 0, y = 0, w = 0, h = 0;
	HUINT32 		ulStatus = 0, ulBtnId = 0;
	HUINT8			*pStr = NULL;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;

	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ulBtnId = (HUINT32)ONDK_GWM_Obj_GetObjectID(object);
	pStr = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	switch (ulBtnId)
	{
		case NX_PRISM_UI_PIN_CODE_ID:
		case (NX_PRISM_UI_PIN_CODE_ID + 1):
		case (NX_PRISM_UI_PIN_CODE_ID + 2):
		case (NX_PRISM_UI_PIN_CODE_ID + 3):

		case NX_PRISM_UI_CHANGEPIN_BTN_ID:
		case (NX_PRISM_UI_CHANGEPIN_BTN_ID + 1):
		case (NX_PRISM_UI_CHANGEPIN_BTN_ID + 2):
		case (NX_PRISM_UI_CHANGEPIN_BTN_ID + 3):

		case NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID:
		case (NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + 1):
		case (NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + 2):
		case (NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + 3):
			break;

		default:
			NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
			return ERR_FAIL;
	}
	stRect = ONDK_Rect(x, y, w, h);

	if (bUiTop > 0)
	{
		if (*pStr == 0x2d /* '-' */)	/* dash */
		{
			if (ulStatus & GUIOBJ_HILIGHTED)
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Base01_100));
				stRect = ONDK_Rect(x, y-2, w, h);
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN01_C_PNG);
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Gong01));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN01_N_PNG);
			}
		}
		else	/* astric */
		{
			if (ulStatus & GUIOBJ_HILIGHTED)
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Base01_100));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN02_C_PNG);
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Gong01));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN02_N_PNG);
			}
		}
	}
	else
	{
		if (*pStr == 0x2d /* '-' */) /* dash */
		{
			if (ulStatus & GUIOBJ_HILIGHTED)
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Base01_20));
				stRect = ONDK_Rect(x, y-2, w, h);
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN01_C_PNG);
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Gong01));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN01_N_PNG);
			}
		}
		else	/* astric */
		{
			if (ulStatus & GUIOBJ_HILIGHTED)
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Base01_20));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN02_C_PNG);
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, h, COL(C_G_Gong01));
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_311_00_DW_PIN_BTN02_N_PNG);
			}
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ERR_OK;
}

//IP4 Input Button
ONDK_Result_t	NX_PRISM_COMP_DrawButton_IpDataButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, i = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, lMaxDigit = 0, lPos = 0;
	HINT32		lInputMode = 0, lCharWidth = 0, fw = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HCHAR		*szText = NULL;
	HBOOL		bIsValid = TRUE;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
    ONDK_Color_t	fontNormalColor = COL (C_T_Gong02), fontFocusColor = COL (C_T_Bang_100);
    ONDK_Color_t	cursorNormalColor = COL (C_G_Gong01), cursorFocusColor = COL (C_G_Base01_100);
	ONDKFontHandle	*hFontHandle = NULL;
	INPUT_OBJECT	objInput = (INPUT_OBJECT)object;
	HINT32 			lDotIndex = 0, lNumberIndex = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_32;
	lCursorMg = 0;

	szText = ONDK_GWM_COM_Input_GetTextPtrFromByObject(objInput);		/* get input value string */
	lMaxDigit = ONDK_GWM_COM_Input_GetDigitByObject(objInput);					/* get max digit boundary */
	lPos = ONDK_GWM_COM_Input_GetCurrentPositionByObject(objInput);			/* get current digit position */
	lInputMode = ONDK_GWM_COM_Input_IsPasswordModeByObject(objInput);		/* get string type */
	ulStatus = ONDK_GWM_COM_Input_IsFocusedByObject(objInput);

    bIsValid = TRUE;

	y = y1;
	w = NX_PRISM_UI_DIALOG_BUTTON_INPUT_W;
	h = NX_PRISM_UI_DIALOG_BUTTON_INPUT_H;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	fw = ONDK_GWM_GFX_GetStringWidth(hFontHandle, "0");

	lText_x_offset = (w - fw) / 2;
	lText_y_offset = (h - ulFontSize) / 2;

	for (i = 0; i <lMaxDigit; i++)
	{
        if(szText[i] == '.')
        {
            lDotIndex++;
        }
        else
        {
   			cursorColor = (i == lPos) ? cursorFocusColor : cursorNormalColor;
   			title_fontColor = (i == lPos) ? fontFocusColor : fontNormalColor;

    		x = x1 + (lNumberIndex*NX_PRISM_UI_DIALOG_BUTTON_INPUT_W);
    		x =((lNumberIndex>2)?(x+(NX_PRISM_UI_IP_DOT_W*(lDotIndex))):(x));

    		ONDK_DRAW_FillRectElement(screen,x, y, w, h, cursorColor);

    		// TEXT - Input Value
    		if (hFontHandle)
    		{
    			ONDK_FONT_DrawOneCharacter(screen, hFontHandle, &szText[i], x + lText_x_offset, y + lText_y_offset, title_fontColor, &lCharWidth);
    		}
            lNumberIndex++;
        }
	}

    lDotIndex =0;
    lNumberIndex = -1;
    //Draw Dot

	for (i = 0; i <lMaxDigit; i++)
	{
	    if(szText[i] == '.')
        {
    		title_fontColor = fontNormalColor;
    		x = x1 + (lNumberIndex*NX_PRISM_UI_DIALOG_BUTTON_INPUT_W);
    		x =((lNumberIndex>2)?(x+(NX_PRISM_UI_IP_DOT_W*(lDotIndex))):(x));
       		ONDK_FONT_DrawOneCharacter(screen, hFontHandle, &szText[i], x +(lText_x_offset/2) +w, y+(h/3)+(lText_y_offset/3), title_fontColor, &lCharWidth);
            lDotIndex++;
        }
        else
        {
            lNumberIndex++;
        }
	}

	ONDK_FONT_Release(hFontHandle);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_DrawButton_TimeDataButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0, i = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0, lMaxDigit = 0, lPos = 0;
	HINT32		lInputMode = 0, lCharWidth = 0, fw = 0;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HCHAR		*szText = NULL;
	HBOOL		bIsValid = TRUE;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;
	INPUT_OBJECT	objInput = (INPUT_OBJECT)object;
	HINT32 			objId = ONDK_GWM_Obj_GetObjectID(object);

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	cursorColor = COL(C_G_Gong01);
	ulFontSize = NX_PRISM_FONT_SIZE_32;
	lCursorMg = 0;

	szText = ONDK_GWM_COM_Input_GetTextPtrFromByObject(objInput);		/* get input value string */
	lMaxDigit = ONDK_GWM_COM_Input_GetDigitByObject(objInput);					/* get max digit boundary */
	lPos = ONDK_GWM_COM_Input_GetCurrentPositionByObject(objInput);			/* get current digit position */
	lInputMode = ONDK_GWM_COM_Input_IsPasswordModeByObject(objInput);		/* get string type */
	ulStatus = ONDK_GWM_COM_Input_IsFocusedByObject(objInput);

	if (lPos == 1 && (szText[0] - '0') > 2)
	{
		bIsValid = FALSE;
	}
	if (lPos == 2)
	{
		if (szText[0] == '2' && (szText[1] - '0') > 4)
		{
			bIsValid = FALSE;
		}
	}
	if (lPos == 3 || lPos == 4)
	{
		if (szText[0] == 2 && szText[1] == 4 && (szText[lPos-1] - '0') > 0)
		{
			bIsValid = FALSE;
		}
	}
	if (lPos == 3 && (szText[2] - '0') > 5)
	{
		bIsValid = FALSE;
	}

	y = y1;
	w = NX_PRISM_UI_DIALOG_BUTTON_INPUT_W;
	h = NX_PRISM_UI_DIALOG_BUTTON_INPUT_H;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	fw = ONDK_GWM_GFX_GetStringWidth(hFontHandle, "0");
	lText_x_offset = (w - fw) / 2;
	lText_y_offset = (h - ulFontSize) / 2;
	for (i = 0; i <lMaxDigit; i++)
	{
		if (bIsValid == TRUE)
		{
			cursorColor = (i == lPos) ? COL(C_G_Base01_100) : COL(C_G_Gong01);
			title_fontColor = (i == lPos) ? COL(C_T_Bang_100) : COL(C_T_Gong02);
		}
		else
		{
			cursorColor = (i == lPos - 1) ? COL(C_G_Base01_100) : COL(C_G_Gong01);
			title_fontColor = (i == lPos - 1) ? COL(C_T_Bang_100) : COL(C_T_Gong02);
		}

		x = x1 + (i*NX_PRISM_UI_DIALOG_BUTTON_INPUT_W);
		x = (i > 1) ? x + NX_PRISM_UI_DIALOG_BUTTON_INPUT_W : x;
		ONDK_DRAW_FillRectElement(screen,x, y, w, h, cursorColor);

		// TEXT - Input Value
		if (hFontHandle)
		{
			if (i == (lPos - 1) &&  bIsValid == FALSE)
			{
				ONDK_FONT_DrawOneCharacter(screen, hFontHandle, "-", x + lText_x_offset, y + lText_y_offset, title_fontColor, &lCharWidth);
				ONDK_GWM_COM_Input_SetPosition(objId, lPos - 1);
			}
			else
			{
				ONDK_FONT_DrawOneCharacter(screen, hFontHandle, &szText[i], x + lText_x_offset, y + lText_y_offset, title_fontColor, &lCharWidth);
			}
		}
	}
	ONDK_FONT_Release(hFontHandle);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_HELP_________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_DrawOption_HelpR(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, stringPixelLength = 0;

	HUINT32 	ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	ulFontSize = NX_PRISM_FONT_SIZE_22;

	title = ONDK_GetString(RES_OPTION_01_ID);
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	if (hFontHandle)
	{
		x = x1;
		y = y1;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}

	stringPixelLength = ONDK_GWM_GFX_GetStringWidth(hFontHandle, (HCHAR*)title);
	//x = x2 - stringPixelLength - NX_PRISM_UI_HELP_R_ICON_MG - NX_PRISM_UI_HELP_OPT_ICON_W;
	//y = y1;
	x = x1 + rect->w - stringPixelLength - NX_PRISM_UI_HELP_R_ICON_MG - NX_PRISM_UI_HELP_OPT_ICON_W;
	y = rect->y;
	stRect = ONDK_Rect(x, y1, NX_PRISM_UI_HELP_OPT_ICON_W, NX_PRISM_UI_HELP_OPT_ICON_H);
	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_201_00_OPTION_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawEdit_HelpR(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, stringPixelLength = 0;

	HUINT32 	ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	ONDK_Color_t	title_fontColor = 0x00000000;
	ONDKFontHandle	*hFontHandle = NULL;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontColor = COL (C_T_Gong02);
	ulFontSize = NX_PRISM_FONT_SIZE_22;

	title = ONDK_GetString(RES_EDIT_02_ID);
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	if (hFontHandle)
	{
		x = x1;
		y = y1;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontColor, ONDKSTF_RIGHT);
		ONDK_FONT_Release(hFontHandle);
	}

	stringPixelLength = ONDK_GWM_GFX_GetStringWidth(hFontHandle, (HCHAR*)title);
	//x = x2 - stringPixelLength - NX_PRISM_UI_HELP_R_ICON_MG - NX_PRISM_UI_HELP_OPT_ICON_W;
	//y = y1;
	x = x1 + rect->w - stringPixelLength - NX_PRISM_UI_HELP_R_ICON_MG - NX_PRISM_UI_HELP_OPT_ICON_W;
	y = rect->y;
	stRect = ONDK_Rect(x, y1, NX_PRISM_UI_HELP_OPT_ICON_W, NX_PRISM_UI_HELP_OPT_ICON_H);
	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_201_00_OPTION_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_BAR_________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarA1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32		ulPrcentBar = 0, ulStatusValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulStatusValue, &ulBackColor, &ulForcColor);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_BAR_A_1_BG_MG, w, NX_PRISM_UI_BAR_A_1_BG_H, ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulStatusValue*w)/ulMax;
	if (ulPrcentBar > (HUINT32)w )
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1, ulPrcentBar, h, ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB1_B2_B3(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32		ulPrcentBar = 0, ulStatusValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulStatusValue, &ulBackColor, &ulForcColor);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_BAR_B_123_BG_MG, w, NX_PRISM_UI_BAR_B_123_BG_H, ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulStatusValue*w)/ulMax;
	if (ulPrcentBar > (HUINT32)w )
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1, ulPrcentBar, h, ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB4(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32		ulPrcentBar = 0, ulStatusValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulStatusValue, &ulBackColor, &ulForcColor);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_BAR_B_4_BG_MG, w, NX_PRISM_UI_BAR_B_4_BG_H, ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulStatusValue*w)/ulMax;
	if (ulPrcentBar > (HUINT32)w )
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1, ulPrcentBar, h, ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawStatusBarB5(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32		ulPrcentBar = 0, ulStatusValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulStatusValue, &ulBackColor, &ulForcColor);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_BAR_B_5_BG_MG, w, NX_PRISM_UI_BAR_B_5_BG_H, ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulStatusValue*w)/ulMax;
	if (ulPrcentBar > (HUINT32)w )
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1, ulPrcentBar, h, ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_DrawWEmpty(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_Frame_______________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_DrawFrameBG_SS(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_105_00_BG_SS_PNG, RES_105_00_BG_SS_PNG, RES_105_00_BG_SS_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameBottomDim(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameBox_A(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	stRect = ONDK_Rect(x1, y1, x2 - x1, y2 - y1);
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Bang_50));
	ONDK_DRAW_StrokeRoundRect(screen, x1, y1, x2 - NX_PRISM_UI_FRAME_BOX_ROUND_PX,
			y2 - NX_PRISM_UI_FRAME_BOX_ROUND_PX, NX_PRISM_UI_FRAME_BOX_ROUND_PX, COL(C_G_Whistle_15));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_A1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, COL(C_G_Base01_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_C2(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		lText_x_offset = 0, lText_y_offset = 0;

	HUINT16		usFontSize = 0;
	HUINT32		ulStatus = 0, ulFontSize = 0;
	HUINT8		*title = NULL;

	ONDK_Color_t	title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* get text - Item Name*/
	title = (HUINT8*)ONDK_GWM_COM_Button_GetTitleByObj(objBtn);
	if (title == NULL)
	{
		NX_APP_Error("[%s:%d] Assert!!! Please Check!! \n", __FUNCTION__, __HxLINE__);
		return ONDK_RET_FAIL;
	}

	title_fontColor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_10);
	ulFontSize = NX_PRISM_FONT_SIZE_22;
	eFontId = eFont_SystemNormal;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ONDK_GWM_COM_Button_GetTitleSizeByObj(objBtn, &usFontSize);
	if (usFontSize < 1)
	{
		ulFontSize = (HUINT32)NX_PRISM_FONT_SIZE_22;
	}
	else
	{
		ulFontSize = (HUINT32)usFontSize;
	}

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontColor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			title_fontColor = COL (C_T_Gong02);
			cursorColor = COL(C_G_Whistle_10);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			title_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);
			eFontId = eFont_SystemBold;

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	/* custom value for subitem text for right alignment*/
	lText_x_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_X_TAB;
	lText_y_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		// SubItem - Title
		x = x1 + lText_x_offset;
		y = y1 + lText_y_offset;

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, (w - lText_x_offset), title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}
	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_DrawFrameCursor_D1(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{

	ONDK_Color_t	strokeColor = COL (C_G_Base01_100);

	ONDK_DRAW_StrokeRect(screen, rect->x, rect->y, (rect->x + rect->w), (rect->y + rect->h), 4, strokeColor);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_DrawFrameScroll_BG(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, COL(C_G_Whistle_20));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, COL(C_G_Bang_90));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_BottomLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;
	ONDK_Color_t	ulColor = 0;
	RECT_OBJECT		objRect = (RECT_OBJECT)object;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ulColor = ONDK_GWM_COM_Rect_GetColor(objRect);

	// Frame Color
	ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - NX_PRISM_UI_FRAME_BOX_LINE_H, ulColor);

	// Bottom Line
	ONDK_DRAW_FillRectElement(screen, x1, y1 + h - NX_PRISM_UI_FRAME_BOX_LINE_H, w, NX_PRISM_UI_FRAME_BOX_LINE_H, COL(C_G_Whistle_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_TopLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;
	ONDK_Color_t	ulColor = 0;
	RECT_OBJECT 	objRect = (RECT_OBJECT)object;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ulColor = ONDK_GWM_COM_Rect_GetColor(objRect);

	// TOP Line
	ONDK_DRAW_FillRectElement(screen, x1, y1, w, NX_PRISM_UI_FRAME_BOX_LINE_H, COL(C_G_Whistle_100));

	// Frame Color
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_FRAME_BOX_LINE_H, w, h - NX_PRISM_UI_FRAME_BOX_LINE_H, ulColor);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_COMP_DrawFrameDialogueWindowBG_TBLine(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			x1 = 0, y1 = 0, w = 0, h = 0;
	ONDK_Color_t	ulColor = 0;
	RECT_OBJECT 	objRect = (RECT_OBJECT)object;

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	ulColor = ONDK_GWM_COM_Rect_GetColor(objRect);

	// TOP Line
	ONDK_DRAW_FillRectElement(screen, x1, y1, w, NX_PRISM_UI_FRAME_BOX_LINE_H, COL(C_G_Whistle_100));

	// Frame Color
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_PRISM_UI_FRAME_BOX_LINE_H, w, h - (NX_PRISM_UI_FRAME_BOX_LINE_H*2), ulColor);

	// Bottom Line
	ONDK_DRAW_FillRectElement(screen, x1, (y1 + h - NX_PRISM_UI_FRAME_BOX_LINE_H), w, NX_PRISM_UI_FRAME_BOX_LINE_H, COL(C_G_Whistle_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_List_______________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_DrawList_Cursor_A1(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lTextW = 0;
	HINT32		lCellHeight = 0, lLastIdx = 0, lNumOfPage = NX_PRISM_COMP_LISTAPP_PAGEITEM_MAX, lCellMg = 0;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lText_y_off = 0;
	HUINT32		ulItem_addr = 0;
	HUINT8		*szTitle = NULL, *pDisable = NULL;
	ONDK_Color_t		title_fontColor = 0x00000000;
	ONDKFontHandle		hFontHandle;
	LIST_OBJECT	obj=(LIST_OBJECT)object;

	/* make coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	/* set properties */
	ulItem_addr = ONDK_GWM_List_GetArrayPtr(obj);
	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		NX_APP_Error("ERROR!!! (ulItem_addr == 0) || (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_List_GetFontHandleObj(obj, &hFontHandle);
	if (hFontHandle == NULL)
	{
		NX_APP_Error("ERROR!!! ONDK_GWM_List_GetFontHandleObj() was NULL!!\n");
		return	ONDK_RET_FAIL;
	}

	pDisable = ONDK_GWM_List_GetItemDisable(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	lCellMg = ONDK_GWM_List_GetLineInterval(ONDK_GWM_Obj_GetObjectID(object));

	lText_y_off = NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_Y;

	if (lNumOfPage < lTotal)
	{
		lLastIdx = lNumOfPage;
	}
	else
	{
		lLastIdx = lTotal;
	}

	for (i = 0; i < lLastIdx; i++)
	{
		if (i + lStart == lCurrent)	/* focus */
		{
			title_fontColor = COL(C_T_Bang_100);
			x = x1;
			y = y1 + lCursor_y_off;

			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCellMg, COL(C_G_Base01_100));
		}
		else
		{
			title_fontColor = COL(C_T_Whistle_100);
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = COL(C_T_Gong08_40);
			}
		}

		szTitle = (HUINT8 *)(((HUINT8 **)ulItem_addr)[lStart+i]);
		if (szTitle == NULL)
		{
			NX_APP_Error("ERROR!!! szTitle was NULL!!\n");
			return ONDK_RET_FAIL;
		}

		NX_APP_Info("[%d][%s]\n", lStart+i, szTitle);

		x = x1 + NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_X;
		y = y1 + lCursor_y_off + lText_y_off;
		lTextW = (w - (NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_X * 2));
		if (lTextW < 0)
		{
			lTextW = NX_PRISM_COMP_LISTAPP_TEXT_OFFSET_X;
		}

		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, x, y, lTextW, title_fontColor, ONDKSTF_CENTER);

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

