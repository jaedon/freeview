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
 * @file	  		nx_prism_menu_time.c
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
#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_MENU_TIME_GMT_OFFSET_0000			0
#define NX_MENU_TIME_GMT_OFFSET_0100			60
#define NX_MENU_TIME_GMT_OFFSET_0200			120
#define NX_MENU_TIME_GMT_OFFSET_0300			180
#define NX_MENU_TIME_GMT_OFFSET_0330			210
#define NX_MENU_TIME_GMT_OFFSET_0400			240

#define NX_PRISM_MENU_TIME_ID					eMenuFrame_Window_Settings

#define	NX_PRISM_MENU_TIME_GMT_BACKGROUND_ID	(NX_PRISM_MENU_TIME_ID + 1)
#define	NX_PRISM_MENU_TIME_GMT_BACKGROUND_X		61
#define	NX_PRISM_MENU_TIME_GMT_BACKGROUND_Y		540
#define	NX_PRISM_MENU_TIME_GMT_BACKGROUND_W		749
#define	NX_PRISM_MENU_TIME_GMT_BACKGROUND_H		130

#define	NX_PRISM_MENU_TIME_GMT_ICON_ID			(NX_PRISM_MENU_TIME_ID + 2)
#define	NX_PRISM_MENU_TIME_GMT_ICON_X			102
#define	NX_PRISM_MENU_TIME_GMT_ICON_Y			559
#define	NX_PRISM_MENU_TIME_GMT_ICON_W			92
#define	NX_PRISM_MENU_TIME_GMT_ICON_H			92

#define	NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID		(NX_PRISM_MENU_TIME_ID + 3)
#define	NX_PRISM_MENU_TIME_GMT_TIME_TEXT_X		225
#define	NX_PRISM_MENU_TIME_GMT_TIME_TEXT_Y		580
#define	NX_PRISM_MENU_TIME_GMT_TIME_TEXT_W		546
#define	NX_PRISM_MENU_TIME_GMT_TIME_TEXT_H		20

#define	NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID		(NX_PRISM_MENU_TIME_ID + 4)
#define	NX_PRISM_MENU_TIME_GMT_CITY_TEXT_X		225
#define	NX_PRISM_MENU_TIME_GMT_CITY_TEXT_Y		612
#define	NX_PRISM_MENU_TIME_GMT_CITY_TEXT_W		546
#define	NX_PRISM_MENU_TIME_GMT_CITY_TEXT_H		20

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_GMTOffset,
	eNxSettingsItem_DaylightSavingTime,
	eNxSettingsItem_TIME_MAX
} NxSettingsItem_Time_t;

typedef struct tag_NX_MENUTIME_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8				ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32				nGmtOffset;
	HBOOL				bDaylightSavingTime;

	HINT8				pTimeText[MAX_ELEMENT_TEXT_LENGTH];
	HINT8				pCityText[MAX_ELEMENT_TEXT_LENGTH];
	HINT8				pIconPath[MAX_ELEMENT_TEXT_LENGTH];

	HINT32				lPrevCurIdx;

	HUINT8				szDate[NX_TEXT_SIZE_32];
	HUINT8				szTime[NX_TEXT_SIZE_32];
} Nx_MenuTimeSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuTimeSetContent_t			s_stTimeContent;
static Nx_PrismListDialog_Content_t		s_stPopupListTimeSetConts;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuTime_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTime_DestroyListPopup(void);
static void 			nx_menuTime_CreateListPopup(Nx_MenuTimeSetContent_t *pContents, HINT32 ulObjId);

static inline Nx_MenuTimeSetContent_t* nx_menuTime_GetContents(void);
static HCHAR*			nx_menuTime_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuTime_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuTime_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuTime_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuTime_GetItemSupport(NxSettingsItem_Time_t eNxMenuTimeItem);
static HBOOL			nx_menuTime_GetItemiDisable(NxSettingsItem_Time_t eNxMenuTimeItem);
static void				nx_menuTime_FindNextFocus(Nx_MenuTimeSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuTime_UpdateCurrentTime(Nx_MenuTimeSetContent_t *pstContents);

static ONDK_Result_t	nx_menuTime_UpdateDaylightSavingTime(Nx_MenuTimeSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTime_UpdateSubItem(Nx_MenuTimeSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTime_ChangedSubItemValue(Nx_MenuTimeSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuTime_GetTimeTextFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents);
static ONDK_Result_t	nx_menuTime_GetCityTextFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents);
static ONDK_Result_t	nx_menuTime_GetIconPathFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents);
static ONDK_Result_t	nx_menuTime_DrawGMTInfo(Nx_MenuTimeSetContent_t *pstContents);

static ONDK_Result_t	nx_menuTime_LoadSettingsItemsValue(Nx_MenuTimeSetContent_t *pstContents);
static ONDK_Result_t	nx_menuTime_AddItemDaylightSavingTime(Nx_MenuTimeSetContent_t *pstContents);
static ONDK_Result_t	nx_menuTime_InitSettingsItems(Nx_MenuTimeSetContent_t *pstContents);

static ONDK_Result_t	nx_menuTime_DrawDefaultUI(Nx_MenuTimeSetContent_t *pstContents);

static ONDK_Result_t	nx_menuTime_MsgGwmCreate(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuTime_MsgGwmDestroy(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTime_MsgGwmHapiMsg(Nx_MenuTimeSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTime_MsgGwmKeyDown(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTime_MsgGwmTimer(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuTime_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListTimeSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuTime_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuTime_DisplayListPopup);
}


static void		nx_menuTime_CreateListPopup(Nx_MenuTimeSetContent_t *pContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem;

	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents, ulObjId);
	if ( psubItem == NULL )
	{
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListTimeSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
										&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
										ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuTime_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuTime_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuTimeSetContent_t* nx_menuTime_GetContents(void)
{
	return	&s_stTimeContent;
}


static HCHAR*	nx_menuTime_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuTime_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_DaylightSavingTime:
			return	RES_DAYLIGHT_SAVING_TIME_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuTime_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuTime_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_DaylightSavingTime:
			*pulMenuItemId = NX_MENUITEM_ID_SET_DAYLIGHT_SAVING_TIME;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuTime_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Time);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_TIME_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuTime_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_TIME_MAX))
	{
		return 0;
	}

	return	(eNxSetting_Time | lItemId);
}


static HBOOL	nx_menuTime_GetItemSupport(NxSettingsItem_Time_t eNxMenuTimeItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuTimeItem)

	{
		case eNxSettingsItem_DaylightSavingTime:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuTime_GetItemiDisable(NxSettingsItem_Time_t eNxMenuTimeItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuTimeItem)

	{
		case eNxSettingsItem_DaylightSavingTime:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuTime_UpdateCurrentTime(Nx_MenuTimeSetContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek;
	HUINT32 		ulUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_t 	stDatetime;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &stDatetime);
		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDatetime.stDate);
		pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));

		HxSTD_sprintf((char*)pstContents->szDate, "%02d.%02d.%04d", stDatetime.stDate.ucDay, stDatetime.stDate.ucMonth,
										stDatetime.stDate.usYear);

		HxSTD_sprintf((char*)pstContents->szTime, "%s %02d:%02d", pszDayOfWeek, stDatetime.stTime.ucHour,
												stDatetime.stTime.ucMinute);
	}
	else
	{
		pstContents->szDate[0] = '\0';
		pstContents->szTime[0] = '\0';
	}
}


static void		nx_menuTime_FindNextFocus(Nx_MenuTimeSetContent_t *pstContents, HBOOL bUp)
{
	HUINT32			i = 0, ulSubItems = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;

	ulSubItems = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

	for (i = 0; i < ulSubItems; i++)
	{
		if ( bUp == TRUE )
		{
			pSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, (ulSubItems - 1) - i);
		}
		else
		{
			pSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, i);
		}

		if ((pSubItem == NULL) || (pSubItem->bDisable == TRUE))
		{
			continue;
		}

		pstContents->ulCurFocus = i;
		ONDK_GWM_Obj_SetFocus(pSubItem->ulId);
		break;
	}
}


static ONDK_Result_t	nx_menuTime_UpdateDaylightSavingTime(Nx_MenuTimeSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->bDaylightSavingTime != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->bDaylightSavingTime = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetDayLightSavingTime(pstContents->bDaylightSavingTime);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_UpdateSubItem(Nx_MenuTimeSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuTime_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_DaylightSavingTime:
			nx_menuTime_UpdateDaylightSavingTime(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_ChangedSubItemValue(Nx_MenuTimeSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuTime_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_DaylightSavingTime:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuTime_GetMenuItemIdFormObjId(lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuTime_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuTime_GetTimeTextFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*pszDayOfWeek;
	HUINT32				ulUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_t		stDatetime;

	HINT8				*pGmtText = NULL;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &stDatetime);
		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDatetime.stDate);
		pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));

		switch(pstContents->nGmtOffset)
		{
			case NX_MENU_TIME_GMT_OFFSET_0000:
				pGmtText = ONDK_GetString(RES_GMT_0000_ID);
				break;
			case NX_MENU_TIME_GMT_OFFSET_0100:
				pGmtText = ONDK_GetString(RES_GMT_0100_ID);
				break;
			case NX_MENU_TIME_GMT_OFFSET_0200:
				pGmtText = ONDK_GetString(RES_GMT_0200_ID);
				break;
			case NX_MENU_TIME_GMT_OFFSET_0300:
				pGmtText = ONDK_GetString(RES_GMT_0300_ID);
				break;
			case NX_MENU_TIME_GMT_OFFSET_0330:
				pGmtText = ONDK_GetString(RES_GMT_0330_ID);
				break;
			case NX_MENU_TIME_GMT_OFFSET_0400:
				pGmtText = ONDK_GetString(RES_GMT_0400_ID);
				break;
			default:
				break;
		}

		HxSTD_sprintf((char*)pstContents->pTimeText, "%02d:%02d %s %02d/%02d %d %s",
			stDatetime.stTime.ucHour, stDatetime.stTime.ucMinute, pszDayOfWeek,
			stDatetime.stDate.ucDay, stDatetime.stDate.ucMonth, stDatetime.stDate.usYear,
			(pGmtText != NULL) ? (HCHAR*)pGmtText : (HCHAR*)"");

	}


	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuTime_GetCityTextFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents)
{
	switch(pstContents->nGmtOffset)
	{
		case NX_MENU_TIME_GMT_OFFSET_0000:
			HxSTD_sprintf(pstContents->pCityText, "Morocco");
			break;
		case NX_MENU_TIME_GMT_OFFSET_0100:
			HxSTD_sprintf(pstContents->pCityText, "Algeria, Tunisia");
			break;
		case NX_MENU_TIME_GMT_OFFSET_0200:
			HxSTD_sprintf(pstContents->pCityText, "Turkey, Greece, Egypt");
			break;
		case NX_MENU_TIME_GMT_OFFSET_0300:
			HxSTD_sprintf(pstContents->pCityText, "Bahrain, Iraq, Kuwait, Qatar, Saudi Arabia");
			break;
		case NX_MENU_TIME_GMT_OFFSET_0330:
			HxSTD_sprintf(pstContents->pCityText, "Iran");
			break;
		case NX_MENU_TIME_GMT_OFFSET_0400:
			HxSTD_sprintf(pstContents->pCityText, "UAE(United Arab Emirates), Oman");
			break;
		default:
			break;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuTime_GetIconPathFromGmtOffset(Nx_MenuTimeSetContent_t *pstContents)
{
	switch(pstContents->nGmtOffset)
	{
		case NX_MENU_TIME_GMT_OFFSET_0000:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT000_PNG);
			break;
		case NX_MENU_TIME_GMT_OFFSET_0100:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT100_PNG);
			break;
		case NX_MENU_TIME_GMT_OFFSET_0200:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT200_PNG);
			break;
		case NX_MENU_TIME_GMT_OFFSET_0300:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT300_PNG);
			break;
		case NX_MENU_TIME_GMT_OFFSET_0330:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT330_PNG);
			break;
		case NX_MENU_TIME_GMT_OFFSET_0400:
			HxSTD_sprintf(pstContents->pIconPath, RES_220_00_GMT400_PNG);
			break;
		default:
			break;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuTime_DrawGMTInfo(Nx_MenuTimeSetContent_t *pstContents)
{
	ONDK_Rect_t		stRect = {0, 0, 0, 0};

	nx_menuTime_GetTimeTextFromGmtOffset(pstContents);
	nx_menuTime_GetCityTextFromGmtOffset(pstContents);
	nx_menuTime_GetIconPathFromGmtOffset(pstContents);

	// Draw background
	stRect = ONDK_Rect(NX_PRISM_MENU_TIME_GMT_BACKGROUND_X, NX_PRISM_MENU_TIME_GMT_BACKGROUND_Y, NX_PRISM_MENU_TIME_GMT_BACKGROUND_W, NX_PRISM_MENU_TIME_GMT_BACKGROUND_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_MENU_TIME_GMT_BACKGROUND_ID, &stRect, COL(C_G_Bang_80));

	// Draw icon
	stRect = ONDK_Rect(NX_PRISM_MENU_TIME_GMT_ICON_X, NX_PRISM_MENU_TIME_GMT_ICON_Y, NX_PRISM_MENU_TIME_GMT_ICON_W, NX_PRISM_MENU_TIME_GMT_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_MENU_TIME_GMT_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_MENU_TIME_GMT_ICON_ID, (HCHAR*)pstContents->pIconPath);
	ONDK_GWM_Obj_SetVisible((NX_PRISM_MENU_TIME_GMT_ICON_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(NX_PRISM_MENU_TIME_GMT_ICON_ID);

	// Draw time text
	stRect = ONDK_Rect(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_X, NX_PRISM_MENU_TIME_GMT_TIME_TEXT_Y, NX_PRISM_MENU_TIME_GMT_TIME_TEXT_W, NX_PRISM_MENU_TIME_GMT_TIME_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID, &stRect, (HCHAR*)pstContents->pTimeText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_MENU_TIME_GMT_TIME_TEXT_ID, COL(C_T_Gong01), COL(C_T_Gong01));

	// Draw city text
	stRect = ONDK_Rect(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_X, NX_PRISM_MENU_TIME_GMT_CITY_TEXT_Y, NX_PRISM_MENU_TIME_GMT_CITY_TEXT_W, NX_PRISM_MENU_TIME_GMT_CITY_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID, &stRect, (HCHAR*)pstContents->pCityText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_MENU_TIME_GMT_CITY_TEXT_ID, COL(C_T_Gong01), COL(C_T_Gong01));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_LoadSettingsItemsValue(Nx_MenuTimeSetContent_t *pstContents)
{
	HBOOL				bEnabled = FALSE;
	HINT32				nGmtOffset = 0;

	NX_SYSTEM_GetDayLightSavingTime(&bEnabled);
	NX_SYSTEM_GetGMTOffset(&nGmtOffset);

	pstContents->bDaylightSavingTime = bEnabled;
	pstContents->nGmtOffset = nGmtOffset;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_AddItemDaylightSavingTime(Nx_MenuTimeSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuTime_GetItemSupport(eNxSettingsItem_DaylightSavingTime);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_DaylightSavingTime(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTime_ConvertItemIdxToObjId(eNxSettingsItem_DaylightSavingTime);

	// Check bDisable Item
	bDisable = nx_menuTime_GetItemiDisable(eNxSettingsItem_DaylightSavingTime);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuTime_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4653_ID);

	/* Count SubItems */
	nx_menuTime_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->bDaylightSavingTime), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->bDaylightSavingTime;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_InitSettingsItems(Nx_MenuTimeSetContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_TIME_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuTime_AddItemDaylightSavingTime(pstContents);

	// Default Function
	nx_menuTime_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_DrawDefaultUI(Nx_MenuTimeSetContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuTime_UpdateCurrentTime(pstContents);

	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);

	nx_menuTime_DrawGMTInfo(pstContents);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_MsgGwmCreate(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuTime_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	nx_menuTime_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuTime_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
		return	ONDK_RET_FAIL;
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_MsgGwmDestroy(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuTimeSetContent_t));
	nx_menuTime_DestroyListPopup();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTime_MsgGwmHapiMsg(Nx_MenuTimeSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuTime_MsgGwmKeyDown(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_ARROWLEFT:
			lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_General_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_General_Proc,
							 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_menuTime_MsgGwmTimer(Nx_MenuTimeSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuTime_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Time_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuTimeSetContent_t	*pstContents = NULL;

	pstContents = nx_menuTime_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuTime_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuTime_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuTime_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuTime_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuTime_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuTime_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuTime_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuTime_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuTime_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

