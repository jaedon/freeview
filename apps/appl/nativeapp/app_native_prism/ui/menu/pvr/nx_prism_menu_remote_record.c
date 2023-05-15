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
 * @file	  		nx_prism_menu_remote_record.c
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


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_RemoteRecord_EnableRemoteRecord,
	eNxSettingsItem_RemoteRecord_UpdatePeriod,
	eNxSettingsItem_RemoteRecord_UpdateInterval,
	eNxSettingsItem_RemoteRecord_RegistrationCode,
	eNxSettingsItem_RemoteRecord_MAX
} NxSettingsItem_RemoteRecord_e;


typedef struct tag_NX_MENU_REMOTERECORD_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL					eEnableRemoteRecord;
	HUINT32					ulUpdatePeriod;
	HUINT32					ulUpdateInterval;
	HUINT32					ulRegistrationCode;

	HINT32					lPrevCurIdx;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_MenuRemoteRecordSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuRemoteRecordSetContent_t	s_stRemoteRecordContent;
static Nx_PrismListDialog_Content_t			s_stPopupListPwrMgrSetConts;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuRemoteRecord_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuRemoteRecord_DestroyListPopup(void);
static void 			nx_menuRemoteRecord_CreateListPopup(Nx_MenuRemoteRecordSetContent_t *pContents, HINT32 ulObjId);

static inline Nx_MenuRemoteRecordSetContent_t* nx_menuRemoteRecord_GetContents(void);
static HCHAR*			nx_menuRemoteRecord_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuRemoteRecord_GetMenuItemIdFormObjId(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuRemoteRecord_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuRemoteRecord_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuRemoteRecord_GetItemSupport(NxSettingsItem_RemoteRecord_e eNxMenuRemoteRecordItem);
static HBOOL			nx_menuRemoteRecord_GetItemiDisable(NxSettingsItem_RemoteRecord_e eNxMenuRemoteRecordItem);
static void				nx_menuRemoteRecord_FindNextFocus(Nx_MenuRemoteRecordSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuRemoteRecord_UpdateCurrentTime(Nx_MenuRemoteRecordSetContent_t *pstContents);

static ONDK_Result_t	nx_menuRemoteRecord_RefreshSubItem(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuRemoteRecord_RefreshAllSubItems(Nx_MenuRemoteRecordSetContent_t *pstContents);

static ONDK_Result_t	nx_menuRemoteRecord_UpdateRemoteRecord(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuRemoteRecord_UpdateUpdatePeriod(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuRemoteRecord_UpdateUpdateInterval(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuRemoteRecord_UpdateSubItem(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuRemoteRecord_ChangedSubItemValue(Nx_MenuRemoteRecordSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuRemoteRecord_LoadSettingsItemsValue(Nx_MenuRemoteRecordSetContent_t *pstContents);
static ONDK_Result_t	nx_menuRemoteRecord_AddItemRemoteRecord(Nx_MenuRemoteRecordSetContent_t *pstContents);
static ONDK_Result_t	nx_menuRemoteRecord_AddItemUpdatePeriod(Nx_MenuRemoteRecordSetContent_t *pstContents);
static ONDK_Result_t	nx_menuRemoteRecord_AddItemUpdateInterval(Nx_MenuRemoteRecordSetContent_t *pstContents);
static ONDK_Result_t	nx_menuRemoteRecord_AddItemRegistrationCode(Nx_MenuRemoteRecordSetContent_t *pstContents);
static ONDK_Result_t	nx_menuRemoteRecord_InitSettingsItems(Nx_MenuRemoteRecordSetContent_t *pstContents);

static ONDK_Result_t	nx_menuRemoteRecord_DrawDefaultUI(Nx_MenuRemoteRecordSetContent_t *pstContents);

static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmCreate(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuRemoteRecord_MsgGwmDestroy(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmHapiMsg(Nx_MenuRemoteRecordSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmKeyDown(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmTimer(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmObjFocusChanged(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuRemoteRecord_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListPwrMgrSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuRemoteRecord_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuRemoteRecord_DisplayListPopup);
}


static void		nx_menuRemoteRecord_CreateListPopup(Nx_MenuRemoteRecordSetContent_t *pContents, HINT32 ulObjId)
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
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListPwrMgrSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuRemoteRecord_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuRemoteRecord_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuRemoteRecordSetContent_t* nx_menuRemoteRecord_GetContents(void)
{
	return	&s_stRemoteRecordContent;
}


static HCHAR*	nx_menuRemoteRecord_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuRemoteRecord_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			return	RES_REMOTE_RECORDING_ID;

		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
			return	RES_UPDATE_PERIOD_ID;

		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			return	RES_UPDATE_INTERVAL_ID;

		case eNxSettingsItem_RemoteRecord_RegistrationCode:
			return	RES_RETRIEVE_REGISTRATION_CODE_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuRemoteRecord_GetMenuItemIdFormObjId(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuRemoteRecord_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			*pulMenuItemId = NX_MENUITEM_ID_SET_ON_OFF;
			break;

		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
			*pulMenuItemId = NX_MENUITEM_ID_SET_ON_OFF;
			break;

		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			*pulMenuItemId = NX_MENUITEM_ID_SET_ON_OFF;
			break;

		case eNxSettingsItem_RemoteRecord_RegistrationCode:
			*pulMenuItemId = NX_MENUITEM_ID_SET_ON_OFF;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static HINT32	nx_menuRemoteRecord_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_RemoteRecord);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_RemoteRecord_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuRemoteRecord_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_RemoteRecord_MAX))
	{
		return 0;
	}

	return	(eNxSetting_RemoteRecord | lItemId);
}


static HBOOL	nx_menuRemoteRecord_GetItemSupport(NxSettingsItem_RemoteRecord_e eNxMenuRemoteRecordItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuRemoteRecordItem)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			bSupport = TRUE;
			break;

		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
			bSupport = TRUE;
			break;

		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			bSupport = TRUE;
			break;

		case eNxSettingsItem_RemoteRecord_RegistrationCode:
			bSupport = TRUE;
			break;

		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuRemoteRecord_GetItemiDisable(NxSettingsItem_RemoteRecord_e eNxMenuRemoteRecordItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuRemoteRecordItem)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			bDisable = FALSE;
			break;

		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
			bDisable = TRUE;
			break;

		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			bDisable = TRUE;
			break;

		case eNxSettingsItem_RemoteRecord_RegistrationCode:
			bDisable = TRUE;
			break;

		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuRemoteRecord_FindNextFocus(Nx_MenuRemoteRecordSetContent_t *pstContents, HBOOL bUp)
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


static void		nx_menuRemoteRecord_UpdateCurrentTime(Nx_MenuRemoteRecordSetContent_t *pstContents)
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


static ONDK_Result_t	nx_menuRemoteRecord_RefreshSubItem(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId)
{
	HUINT8		**ppListOfItem = NULL;
	HUINT32		ulMenuId, ulListIdx, ulItems;
	HINT32		lSubItem = 0, lItemIdx = -1;
	HCHAR		*pcStrId = NULL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	 *pSubItem;


	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if ( pSubItem == NULL )
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuRemoteRecord_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	lItemIdx = nx_menuRemoteRecord_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			lSubItem = pstContents->eEnableRemoteRecord;
			break;

		default:
			lSubItem = -1;
			break;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(ulMenuId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	pSubItem->ulNumOfItemTextList = ulItems;
	pSubItem->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	pSubItem->lSubItemValue = lSubItem;
	pSubItem->ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	return	ONDK_RET_OK;
}


static void		nx_menuRemoteRecord_RefreshAllSubItems(Nx_MenuRemoteRecordSetContent_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_RemoteRecord_EnableRemoteRecord; i < eNxSettingsItem_RemoteRecord_MAX; i++)
	{
		eResult = nx_menuRemoteRecord_RefreshSubItem(pstContents, nx_menuRemoteRecord_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}

static ONDK_Result_t	nx_menuRemoteRecord_UpdateRemoteRecord(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eEnableRemoteRecord != (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->eEnableRemoteRecord = (HBOOL)pSubItem->lSubItemValue;
		//NX_SYSTEM_SetAutoDelete(pstContents->eEnableRemoteRecord);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_UpdateUpdatePeriod(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->ulUpdatePeriod!= (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->ulUpdatePeriod = (HBOOL)pSubItem->lSubItemValue;
		//NX_SYSTEM_SetStartPaddingTime(pstContents->ulUpdatePeriod);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_UpdateUpdateInterval(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->ulUpdateInterval != (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->ulUpdateInterval = (HBOOL)pSubItem->lSubItemValue;
		//NX_SYSTEM_SetEndPaddingTime(pstContents->ulUpdateInterval);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_UpdateSubItem(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuRemoteRecord_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
			nx_menuRemoteRecord_UpdateRemoteRecord(pstContents, lObjId);
			break;

		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
			nx_menuRemoteRecord_UpdateUpdatePeriod(pstContents, lObjId);
			break;

		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			nx_menuRemoteRecord_UpdateUpdateInterval(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_ChangedSubItemValue(Nx_MenuRemoteRecordSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuRemoteRecord_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_RemoteRecord_EnableRemoteRecord:
		case eNxSettingsItem_RemoteRecord_UpdatePeriod:
		case eNxSettingsItem_RemoteRecord_UpdateInterval:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			nx_menuRemoteRecord_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_LoadSettingsItemsValue(Nx_MenuRemoteRecordSetContent_t *pstContents)
{
	HBOOL		eEnableRemoteRecord 	= TRUE;
	HUINT32		ulUpdatePeriod = 0, ulUpdateInterval = 0;

	//NX_SYSTEM_GetAutoDelete(&eAutoDelete);
	pstContents->eEnableRemoteRecord = eEnableRemoteRecord;

	//NX_SYSTEM_GetStartPaddingTime(&ulStartPaddingTime);
	pstContents->ulUpdatePeriod = ulUpdatePeriod;

	//NX_SYSTEM_GetEndPaddingTime(&ulEndPaddingTime);
	pstContents->ulUpdateInterval = ulUpdateInterval;

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_AddItemRemoteRecord(Nx_MenuRemoteRecordSetContent_t *pstContents)
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
	bSupport = nx_menuRemoteRecord_GetItemSupport(eNxSettingsItem_RemoteRecord_EnableRemoteRecord);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_RemoteRecord_EnableRemoteRecord(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuRemoteRecord_ConvertItemIdxToObjId(eNxSettingsItem_RemoteRecord_EnableRemoteRecord);

	// Check bDisable Item
	bDisable = nx_menuRemoteRecord_GetItemiDisable(eNxSettingsItem_RemoteRecord_EnableRemoteRecord);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuRemoteRecord_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eEnableRemoteRecord), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eEnableRemoteRecord;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_AddItemUpdatePeriod(Nx_MenuRemoteRecordSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32 			ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
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
	bSupport = nx_menuRemoteRecord_GetItemSupport(eNxSettingsItem_RemoteRecord_UpdatePeriod);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_RemoteRecord_UpdatePeriod(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuRemoteRecord_ConvertItemIdxToObjId(eNxSettingsItem_RemoteRecord_UpdatePeriod);

	// Check bDisable Item
	bDisable = nx_menuRemoteRecord_GetItemiDisable(eNxSettingsItem_RemoteRecord_UpdatePeriod);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuRemoteRecord_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->ulUpdatePeriod), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->ulUpdatePeriod;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_AddItemUpdateInterval(Nx_MenuRemoteRecordSetContent_t *pstContents)
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
	bSupport = nx_menuRemoteRecord_GetItemSupport(eNxSettingsItem_RemoteRecord_UpdateInterval);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_RemoteRecord_UpdateInterval(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuRemoteRecord_ConvertItemIdxToObjId(eNxSettingsItem_RemoteRecord_UpdateInterval);

	// Check bDisable Item
	bDisable = nx_menuRemoteRecord_GetItemiDisable(eNxSettingsItem_RemoteRecord_UpdateInterval);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuRemoteRecord_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->ulUpdateInterval), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->ulUpdateInterval;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_AddItemRegistrationCode(Nx_MenuRemoteRecordSetContent_t *pstContents)
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
	bSupport = nx_menuRemoteRecord_GetItemSupport(eNxSettingsItem_RemoteRecord_RegistrationCode);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_RemoteRecord_RegistrationCode(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuRemoteRecord_ConvertItemIdxToObjId(eNxSettingsItem_RemoteRecord_RegistrationCode);

	// Check bDisable Item
	bDisable = nx_menuRemoteRecord_GetItemiDisable(eNxSettingsItem_RemoteRecord_RegistrationCode);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuRemoteRecord_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuRemoteRecord_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
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
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->ulRegistrationCode), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->ulRegistrationCode;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuRemoteRecord_InitSettingsItems(Nx_MenuRemoteRecordSetContent_t *pstContents)
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
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_REMOTE_RECORDING_01_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuRemoteRecord_AddItemRemoteRecord(pstContents);
	nx_menuRemoteRecord_AddItemUpdatePeriod(pstContents);
	nx_menuRemoteRecord_AddItemUpdateInterval(pstContents);
	nx_menuRemoteRecord_AddItemRegistrationCode(pstContents);

	// Default Function
	nx_menuRemoteRecord_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_DrawDefaultUI(Nx_MenuRemoteRecordSetContent_t *pstContents)
{
	Nx_SettingsLeafSubItem_t stFocusItem = pstContents->subItems[pstContents->ulCurFocus];

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

    nx_menuRemoteRecord_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - HelpText
	NX_PRISM_COMP_Settings_DrawHelpText(stFocusItem.pszHelpText);

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);
	ONDK_GWM_Obj_SetFocus(stFocusItem.ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmCreate(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuRemoteRecord_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	nx_menuRemoteRecord_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuRemoteRecord_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmDestroy(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuRemoteRecordSetContent_t));
	nx_menuRemoteRecord_DestroyListPopup();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmHapiMsg(Nx_MenuRemoteRecordSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmKeyDown(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Pvr_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Pvr_Proc,
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


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmTimer(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuRemoteRecord_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuRemoteRecord_MsgGwmObjFocusChanged(Nx_MenuRemoteRecordSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	eResult = NX_PRISM_AppContents_GetSubitemIndexByObjId(pstContents, p1, &ulIndex);
	if (eResult == ONDK_RET_OK)
	{
		pstContents->ulCurFocus = ulIndex;
		NX_PRISM_COMP_Settings_UpdateHelpText(pstContents->subItems[pstContents->ulCurFocus].pszHelpText);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_RemoteRecord_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuRemoteRecordSetContent_t	*pstContents = NULL;

	pstContents = nx_menuRemoteRecord_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuRemoteRecord_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuRemoteRecord_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuRemoteRecord_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuRemoteRecord_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuRemoteRecord_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuRemoteRecord_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuRemoteRecord_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuRemoteRecord_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuRemoteRecord_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			eResult = nx_menuRemoteRecord_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
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
