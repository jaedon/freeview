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
 * @file	  		nx_prism_menu_powertimersettings.c
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
#include <nx_prism_util.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_MENU_POWERTIMER_SETTINGS_CH_GRP_MAX_NUM				64
#define NX_MENU_POWERTIMER_SETTINGS_CH_TEXT_MAX_NUM				128
// OK Button
#define	NX_MENU_POWERTIMER_SETTINGS_OK_BTN_X					170
#define	NX_MENU_POWERTIMER_SETTINGS_OK_BTN_Y					615
#define	NX_MENU_POWERTIMER_SETTINGS_OK_BTN_W					260
#define	NX_MENU_POWERTIMER_SETTINGS_OK_BTN_H					55

// CANCEL Button
#define	NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_X				440
#define	NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_Y				615
#define	NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_W				260
#define	NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_H				55

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_Time,
	eNxSettingsItem_Channel,
	eNxSettingsItem_Volume,
	eNxSettingsItem_Repeat,
	eNxSettingsItem_Ok,
	eNxSettingsItem_Cancel,
	eNxSettingsItem_PowerTimerSettings_MAX
} NxSettingsItem_PowerTimerSettings_e;

typedef struct tag_NX_MENUPOWERTIMERSETTINGS_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HBOOL					bIsOnTimer;
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT32					ulTime;
	HUINT32					ulChannel;
	HINT32					nVolume;
	DxSCHED_DATE_e			eRepeat;

	HINT32					nVolumeMax;
	HINT32					nVolumeMin;

	ONDK_GWM_Callback_t		cbPrevApp;

	HxVector_t				*pCurChSubVector;
	HxVector_t				*pServiceGroupVector;
	HxVector_t				*pAlphabetGroupVector;
	NX_Channel_t			*pstChannelList;

	HUINT32					ulCurChSubNum;
	HUINT32					ulChGroupNum;
	HUINT32					ulAlphabetGroupNum;
	HUINT32					ulChListNum;

	HUINT32		ulChMenuDepth;
} Nx_MenuPowerTimerSettingsSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuPowerTimerSettingsSetContent_t	s_stPowerTimerSettingsContent;
static Nx_PrismListDialog_Content_t				s_stPopupListPwerTimerSettingSetConts;
static NX_TimeDialogInstance_t					s_stMenuSystem_Time_dInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuPowerTimerSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPowerTimerSettings_DestroyListPopup(void);
static void 			nx_menuPowerTimerSettings_CreateListPopup(Nx_MenuPowerTimerSettingsSetContent_t *pContents, HINT32 ulObjId);
static ONDK_Result_t 	nx_menuPowerTimerSettings_UpdateListPopup(Nx_MenuPowerTimerSettingsSetContent_t *pContents, HUINT8 **ppSubList, HINT32 nNum);

static ONDK_Result_t	nx_menuPowerTimerSettings_DisplayTimeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPowerTimerSettings_DestroyTimeDialog(void);
static void				nx_menuPowerTimerSettings_CreateTimeDialog(HUINT8 *pszDlgMsg);

static inline Nx_MenuPowerTimerSettingsSetContent_t* nx_menuPowerTimerSettings_GetContents(void);
static HCHAR*			nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_GetMenuItemIdFormObjId(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuPowerTimerSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuPowerTimerSettings_GetItemSupport(NxSettingsItem_PowerTimerSettings_e eNxMenuPowerTimerSettingsItem);
static HBOOL			nx_menuPowerTimerSettings_GetItemiDisable(NxSettingsItem_PowerTimerSettings_e eNxMenuPowerTimerSettingsItem);
static void				nx_menuPowerTimerSettings_FindNextFocus(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HBOOL bUp);
static ONDK_Result_t	nx_menuPowerTimerSettings_ChangeFocus(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lKey);

static ONDK_Result_t	nx_menuPowerTimerSettings_RefreshSubItem(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuPowerTimerSettings_RefreshAllSubItems(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateTime(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateChannel(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateRepeat(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateSubItem(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_ChangedSubItemValue(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuPowerTimerSettings_LoadSettingsItemsValue(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemTime(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemChannel(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemRepeat(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_InitChSetting(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_InitVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_getAvailableChannelGroup(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_getAvailableAlphabetGroup(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static ONDK_Result_t	nx_menuPowerTimerSettings_InitSettingsItems(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);

static ONDK_Result_t	nx_menuPowerTimerSettings_DrawDefaultUI(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static void 			nx_menuPowerTimerSettings_ChangeCurChSubVector(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HxVector_t* pVector, HINT32 nNum);
static void 			nx_menuPowerTimerSettings_InitChSubList(Nx_MenuPowerTimerSettingsSetContent_t *pstContents);
static void 			nx_menuPowerTimerSettings_NxGroupItem_Delete(void *item);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmCreate(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuPowerTimerSettings_MsgGwmDestroy(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmHapiMsg(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgAppInputValueChange(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HUINT32 ulUnixtime);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmClick(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 ulObjId);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmKeyDown(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmObjFocusChanged(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuPowerTimerSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListPwerTimerSettingSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuPowerTimerSettings_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuPowerTimerSettings_DisplayListPopup);
}


static void		nx_menuPowerTimerSettings_CreateListPopup(Nx_MenuPowerTimerSettingsSetContent_t *pContents, HINT32 ulObjId)
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
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListPwerTimerSettingSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuPowerTimerSettings_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuPowerTimerSettings_DisplayListPopup, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateListPopup(Nx_MenuPowerTimerSettingsSetContent_t *pContents, HUINT8 **ppSubList, HINT32 nNum)
{
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;
	HERROR 						herror = ERR_OK;
	HUINT32						ulObjId = 0;

	ulObjId = GWM_GetFocusedObjectID();
	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pContents, ulObjId);
	herror = NX_PRISM_MENU_MenuItem_FreeTextList(pSubItem->ulNumOfItemTextList, pSubItem->szItemTextList);
	if (herror != ERR_OK)
	{
		return ONDK_RET_FAIL;
	}

	pSubItem->ulNumOfItemTextList = nNum;
	pSubItem->szItemTextList = ppSubList;
	pSubItem->ulIdxOfTextList = 0;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_DisplayTimeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_TimeDlalog_ProtoProc(&s_stMenuSystem_Time_dInstance, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuPowerTimerSettings_DestroyTimeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuPowerTimerSettings_DisplayTimeDialog);
}

static void				nx_menuPowerTimerSettings_CreateTimeDialog(HUINT8 *pszDlgMsg)
{
	HUINT32			ulAttribute = 0;
	ONDK_Rect_t		stRect = {0, 0, 0, 0};

	stRect = ONDK_Rect(NX_PRISM_UI_CHANGEPIN_DIALOG_X, NX_PRISM_UI_CHANGEPIN_DIALOG_Y, NX_PRISM_UI_CHANGEPIN_DIALOG_WIDTH, NX_PRISM_UI_CHANGEPIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_TimeDlalog_SetDialog(&s_stMenuSystem_Time_dInstance,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), pszDlgMsg, 0, 0);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuPowerTimerSettings_DisplayTimeDialog", APP_DEFAULT_PRIORITY, nx_menuPowerTimerSettings_DisplayTimeDialog, 0, 0, 0, 0);
}

static inline Nx_MenuPowerTimerSettingsSetContent_t* nx_menuPowerTimerSettings_GetContents(void)
{
	return	&s_stPowerTimerSettingsContent;
}

static HCHAR*	nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_Time:
			return	RES_TIME_ID;

		case eNxSettingsItem_Channel:
			return	RES_CHANNEL_ID;

		case eNxSettingsItem_Volume:
			return	RES_VOLUME_ID;

		case eNxSettingsItem_Repeat:
			return	RES_REPEAT_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuPowerTimerSettings_GetMenuItemIdFormObjId(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_Time:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;

		case eNxSettingsItem_Channel:
			*pulMenuItemId = NX_MENUITEM_ID_SET_OFF_ON;
			break;

		case eNxSettingsItem_Volume:
			*pulMenuItemId = NX_MENUITEM_ID_SET_POWERTIMERVOLUME;
			break;

		case eNxSettingsItem_Repeat:
			*pulMenuItemId = NX_MENUITEM_ID_SET_POWERTIMERREPEAT;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_PowerTimer);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_PowerTimerSettings_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuPowerTimerSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_PowerTimerSettings_MAX))
	{
		return 0;
	}

	return	(eNxSetting_PowerTimer | lItemId);
}


static HBOOL	nx_menuPowerTimerSettings_GetItemSupport(NxSettingsItem_PowerTimerSettings_e eNxMenuPowerTimerSettingsItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuPowerTimerSettingsItem)

	{
		case eNxSettingsItem_Time:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Channel:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Volume:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Repeat:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuPowerTimerSettings_GetItemiDisable(NxSettingsItem_PowerTimerSettings_e eNxMenuPowerTimerSettingsItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuPowerTimerSettingsItem)

	{
		case eNxSettingsItem_Time:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Channel:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Volume:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Repeat:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuPowerTimerSettings_FindNextFocus(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HBOOL bUp)
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


static ONDK_Result_t	nx_menuPowerTimerSettings_ChangeFocus(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lKey)
{
	HINT32			lObjId = 0, lNextObjId = 0, lItemIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lKey)
	{
		case KEY_ARROWUP:
			if (lItemIdx == eNxSettingsItem_Time)
			{
				lNextObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_Ok) || (lItemIdx == eNxSettingsItem_Cancel))
			{
				nx_menuPowerTimerSettings_FindNextFocus(pstContents, TRUE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case KEY_ARROWDOWN:
			if (lItemIdx == eNxSettingsItem_Repeat)
			{
				lNextObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_Ok) || (lItemIdx == eNxSettingsItem_Cancel))
			{
				nx_menuPowerTimerSettings_FindNextFocus(pstContents, FALSE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			if (lItemIdx == eNxSettingsItem_Ok)
			{
				lNextObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Cancel);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if (lItemIdx == eNxSettingsItem_Cancel)
			{
				lNextObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateTime(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->ulTime != (HUINT32)pSubItem->lSubItemValue)
	{
		pstContents->ulTime = (HUINT32)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateChannel(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->ulChannel != (HUINT32)pSubItem->lSubItemValue)
	{
		pstContents->ulChannel = (HUINT32)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->nVolume != (HINT32)pSubItem->lSubItemValue)
	{
		pstContents->nVolume = (pstContents->nVolumeMax - pstContents->nVolumeMin)/NX_SYS_VOLUME_STEP * (HINT32)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateRepeat(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eRepeat != (DxSCHED_DATE_e)pSubItem->lSubItemValue)
	{
		pstContents->eRepeat = (DxSCHED_DATE_e)pSubItem->lSubItemValue;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_UpdateSubItem(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_Time:
			nx_menuPowerTimerSettings_UpdateTime(pstContents, lObjId);
			break;

		case eNxSettingsItem_Channel:
			nx_menuPowerTimerSettings_UpdateChannel(pstContents, lObjId);
			break;

		case eNxSettingsItem_Volume:
			nx_menuPowerTimerSettings_UpdateVolume(pstContents, lObjId);
			break;

		case eNxSettingsItem_Repeat:
			nx_menuPowerTimerSettings_UpdateRepeat(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_ChangedSubItemValue(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;



	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_Time:
		case eNxSettingsItem_Volume:
		case eNxSettingsItem_Repeat:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuPowerTimerSettings_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuPowerTimerSettings_UpdateSubItem(pstContents, lObjId);
			break;
		case eNxSettingsItem_Channel:
			if (pstContents->ulChMenuDepth == 0)
			{
				HUINT32			ulIndex = 0;
				HUINT8 			**ppSubList;
				HERROR			hError = ERR_FAIL;
				NX_GroupInfoList_t 	*stSelectedItem;
				HUINT8			stGroupStr[NX_TEXT_SIZE_128] = {'\0',};

				stSelectedItem = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, ulCurIdx);
				if (stSelectedItem->eGroupType == eNxChListGroup_ALPHABET)
				{
					nx_menuPowerTimerSettings_ChangeCurChSubVector(pstContents, pstContents->pAlphabetGroupVector, pstContents->ulAlphabetGroupNum);
					ppSubList = NX_PRISM_MENU_MenuItem_AllocTextList(pstContents->ulCurChSubNum, 4); // because max length is 4 (All)

					for (ulIndex = 0 ; ulIndex < pstContents->ulCurChSubNum ; ulIndex++)
					{
						NX_GroupInfoList_t 	*item = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, ulIndex);
						NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(item->eGroupType, (HCHAR*)stGroupStr);
						HxSTD_sprintf((char*)ppSubList[ulIndex], (char*)"%s", (HCHAR*)stGroupStr);
					}

					nx_menuPowerTimerSettings_UpdateListPopup(pstContents, ppSubList, pstContents->ulCurChSubNum);
					nx_menuPowerTimerSettings_DestroyListPopup();
					nx_menuPowerTimerSettings_CreateListPopup(pstContents, lObjId);
				}
				else
				{
					HUINT32			ulExtraFilter = 0;

					nx_menuPowerTimerSettings_ChangeCurChSubVector(pstContents, pstContents->pServiceGroupVector, pstContents->ulChGroupNum);

					ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(stSelectedItem->eGroupType);
					if (pstContents->pstChannelList != NULL)
					{
						NX_APP_Free(pstContents->pstChannelList);
					}

					hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->ulChListNum,
													stSelectedItem->eGroupType, ulExtraFilter);
					if (hError != ERR_OK)
					{
						return ONDK_RET_FAIL;
					}
					ppSubList = NX_PRISM_MENU_MenuItem_AllocTextList(pstContents->ulChListNum, NX_MENU_POWERTIMER_SETTINGS_CH_TEXT_MAX_NUM);
					if (ppSubList == NULL)
					{
						NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_AllocTextList() Fail!! \n");
						return	ONDK_RET_FAIL;
					}

					for (ulIndex = 0 ; ulIndex < pstContents->ulChListNum ; ulIndex++)
					{
						NX_Channel_t stChannel = pstContents->pstChannelList[ulIndex];
						DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(stChannel.svcUid);
						if (pstService != NULL)
						{
							HxSTD_sprintf((char*)ppSubList[ulIndex], (char*)"%04d %s", stChannel.lcn, pstService->name);
						}
					}

					nx_menuPowerTimerSettings_UpdateListPopup(pstContents, ppSubList, pstContents->ulChListNum);
					nx_menuPowerTimerSettings_DestroyListPopup();
					nx_menuPowerTimerSettings_CreateListPopup(pstContents, lObjId);
					pstContents->ulChMenuDepth = 1;
				}
			}
			else
			{
				HUINT32		ulIndex = 0;
				HUINT8		**ppSubList = NULL;
				NX_Channel_t stChannel = pstContents->pstChannelList[ulCurIdx];
				DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(stChannel.svcUid);

				pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
				if (pstService != NULL)
				{
					HxSTD_sprintf((char*)pSubItem->szSubItemText, "%04d %s", stChannel.lcn, pstService->name);
				}

				ppSubList = NX_PRISM_MENU_MenuItem_AllocTextList(pstContents->ulCurChSubNum, NX_MENU_POWERTIMER_SETTINGS_CH_GRP_MAX_NUM);
				for (ulIndex = 0 ; ulIndex < pstContents->ulCurChSubNum ; ulIndex++)
				{
					NX_GroupInfoList_t *item = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, ulIndex);
					NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(item->eGroupType, (HCHAR*)ppSubList[ulIndex]);
				}
				nx_menuPowerTimerSettings_UpdateListPopup(pstContents, ppSubList, pstContents->ulCurChSubNum);
				pSubItem->lSubItemValue = stChannel.svcUid;
				pstContents->ulChMenuDepth = 0;
				// /* Update value */
				nx_menuPowerTimerSettings_UpdateSubItem(pstContents, lObjId);
			}
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_LoadSettingsItemsValue(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HUINT32				ulTime 		= 0;
	HUINT32				ulChannel 	= 0;
	HINT32				nVolume	= 10;
	DxSCHED_DATE_e		eRepeat 	= eDxSCHED_DATE_ALL;
	HxDATETIME_t		stDateTime;

	/* Get Time */
	(void)NX_SYSTEM_GetTime(&ulTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulTime, &stDateTime);
	stDateTime.stTime.ucHour = 0;
	stDateTime.stTime.ucMinute = 0;
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulTime);
	pstContents->ulTime = ulTime;

	pstContents->ulChannel = ulChannel;

	/* Get Volume */
	pstContents->nVolume = nVolume;

	/* Get Repeat */
	pstContents->eRepeat = eRepeat;

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemTime(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuPowerTimerSettings_GetItemSupport(eNxSettingsItem_Time);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Time(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Time);

	// Check bDisable Item
	bDisable = nx_menuPowerTimerSettings_GetItemiDisable(eNxSettingsItem_Time);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.lSubItemValue = 0;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, "00:00");

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemChannel(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulIndex = 0, ulExtraFilter = 0;
	HUINT8				**ppListOfItem = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;
	HERROR 				hError = ERR_FAIL;
	NX_Channel_t 		stChannel;
	DxService_t			*pstService;
	HUINT8				*pSubItemText[NX_MENU_POWERTIMER_SETTINGS_CH_TEXT_MAX_NUM];
	NX_GroupInfoList_t 	*firstItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuPowerTimerSettings_GetItemSupport(eNxSettingsItem_Channel);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Channel(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Channel);

	// Check bDisable Item
	bDisable = nx_menuPowerTimerSettings_GetItemiDisable(eNxSettingsItem_Channel);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	ulItems = pstContents->ulCurChSubNum;
	if (ulItems == 0)
	{
		NX_APP_Error("Error!!! (ulItems == 0) \n");
		return	eResult;
	}

	/* Memory Alloc */
	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList(ulItems, NX_MENU_POWERTIMER_SETTINGS_CH_GRP_MAX_NUM);
	if (ppListOfItem != NULL)
	{
		/* Convert string list */
		for (ulIndex = 0 ; ulIndex < ulItems ; ulIndex++)
		{
			NX_GroupInfoList_t *item = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, ulIndex);
			NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(item->eGroupType, (HCHAR*)ppListOfItem[ulIndex]);
		}
	}
	else
	{
		NX_APP_Error("Error!!! (ppListOfItem == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	firstItem = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, 0);
	/* get default subitem */
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(firstItem->eGroupType);
	hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstContents->pstChannelList, &pstContents->ulChListNum, firstItem->eGroupType, ulExtraFilter);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! (hError != ERR_OK) \n");
		NX_APP_Free (ppListOfItem);
		return ONDK_RET_FAIL;
	}

	stChannel = pstContents->pstChannelList[0];
	pstContents->ulChannel = stChannel.svcUid;
	pstService = NX_CHANNEL_FindServiceBySvcUid(stChannel.svcUid);
	HxSTD_sprintf((char*)pSubItemText, "%04d %s", stChannel.lcn, pstService->name);

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;
	stSubItem.ulIdxOfTextList = ulListIdx;
	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)pSubItemText);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
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
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuPowerTimerSettings_GetItemSupport(eNxSettingsItem_Volume);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Volume(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Volume);

	// Check bDisable Item
	bDisable = nx_menuPowerTimerSettings_GetItemiDisable(eNxSettingsItem_Volume);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	// stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4653_ID);

	/* Count SubItems */
	nx_menuPowerTimerSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! ( ppListOfItem == NULL ) \n");
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->nVolume), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->nVolume;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_AddItemRepeat(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
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
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuPowerTimerSettings_GetItemSupport(eNxSettingsItem_Repeat);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Repeat(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Repeat);

	// Check bDisable Item
	bDisable = nx_menuPowerTimerSettings_GetItemiDisable(eNxSettingsItem_Repeat);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuPowerTimerSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	// stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4653_ID);

	/* Count SubItems */
	nx_menuPowerTimerSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! ( ppListOfItem == NULL ) \n");
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eRepeat), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eRepeat;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_InitChSetting(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	ONDK_Result_t eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		return ONDK_RET_INITFAIL;
	}

	eResult = nx_menuPowerTimerSettings_getAvailableChannelGroup(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}
	eResult = nx_menuPowerTimerSettings_getAvailableAlphabetGroup(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}
	nx_menuPowerTimerSettings_ChangeCurChSubVector(pstContents, pstContents->pServiceGroupVector, pstContents->ulChGroupNum);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_InitVolume(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	pstContents->nVolumeMin = NX_SYS_VOLUME_MIN;
	NX_SYSTEM_GetVolumeMax(&pstContents->nVolumeMax);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_getAvailableChannelGroup(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HINT32			lCount = 0;
	HERROR 			herror = ERR_FAIL;

	herror = NX_CHANNEL_GROUP_GetIsGroupInfoList(pstContents->pServiceGroupVector, &lCount);
	if (herror != ERR_OK)
	{
		return ONDK_RET_FAIL;
	}
	pstContents->ulChGroupNum = (HUINT32)lCount;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_getAvailableAlphabetGroup(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HINT32			lCount = 0;
	HERROR 			herror = ERR_FAIL;

	herror = NX_CHANNEL_GROUP_GetAvailableAlphabetGroupInfoList(pstContents->pAlphabetGroupVector, &lCount);
	if (herror != ERR_OK)
	{
		return ONDK_RET_FAIL;
	}
	pstContents->ulAlphabetGroupNum = (HUINT32)lCount;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_InitSettingsItems(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	pstContents->pServiceGroupVector = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_menuPowerTimerSettings_NxGroupItem_Delete,
										NULL);
	pstContents->pAlphabetGroupVector = HLIB_VECTOR_NewEx(NULL, 0,
										(void *(*)(void *))NULL,
										(void *(*)(void *))nx_menuPowerTimerSettings_NxGroupItem_Delete,
										NULL);

	// Init Item Value
	nx_menuPowerTimerSettings_InitChSetting(pstContents);
	nx_menuPowerTimerSettings_InitVolume(pstContents);

	if (pstContents->bIsOnTimer == TRUE)
	{
		HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_POWER_ON_TIMER_ID), MAX_ELEMENT_TEXT_LENGTH);

		nx_menuPowerTimerSettings_AddItemTime(pstContents);
		nx_menuPowerTimerSettings_AddItemChannel(pstContents);
		nx_menuPowerTimerSettings_AddItemVolume(pstContents);
		nx_menuPowerTimerSettings_AddItemRepeat(pstContents);
	}
	else
	{
		HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_POWER_OFF_TIMER_ID), MAX_ELEMENT_TEXT_LENGTH);

		nx_menuPowerTimerSettings_AddItemTime(pstContents);
	}

	// Default Function
	nx_menuPowerTimerSettings_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_DrawDefaultUI(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t stFocusItem = pstContents->subItems[pstContents->ulCurFocus];

	NX_PRISM_COMP_Settings_DrawBackgroundPopUp();
	// NX_PRISM_COMP_Settings_DrawTitlePopup(pstContents->ucTitle);
	NX_PRISM_COMP_Settings_DrawTitlePopUp(pstContents->ucTitle);
	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow);

	// Button
	stRect = ONDK_Rect(NX_MENU_POWERTIMER_SETTINGS_OK_BTN_X, NX_MENU_POWERTIMER_SETTINGS_OK_BTN_Y, NX_MENU_POWERTIMER_SETTINGS_OK_BTN_W, NX_MENU_POWERTIMER_SETTINGS_OK_BTN_H);
	lObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_X, NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_Y, NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_W, NX_MENU_POWERTIMER_SETTINGS_CANCEL_BTN_H);
	lObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(stFocusItem.ulId);

	return	ONDK_RET_OK;
}


static void 	nx_menuPowerTimerSettings_ChangeCurChSubVector(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HxVector_t* pVector, HINT32 nNum)
{
	if (pstContents->pCurChSubVector != pVector)
	{
		pstContents->pCurChSubVector = pVector;
		pstContents->ulCurChSubNum = nNum;
	}
}

static void 	nx_menuPowerTimerSettings_InitChSubList(Nx_MenuPowerTimerSettingsSetContent_t *pstContents)
{
	HUINT32		ulIndex = 0;
	HUINT8 **ppSubList = NULL;

	pstContents->ulChMenuDepth = 0;
	pstContents->pCurChSubVector = pstContents->pServiceGroupVector;
	pstContents->ulCurChSubNum = pstContents->ulChGroupNum;

	ppSubList = NX_PRISM_MENU_MenuItem_AllocTextList(pstContents->ulCurChSubNum, NX_MENU_POWERTIMER_SETTINGS_CH_GRP_MAX_NUM);
	for (ulIndex = 0 ; ulIndex < pstContents->ulCurChSubNum ; ulIndex++)
	{
		NX_GroupInfoList_t *item = HLIB_VECTOR_ItemAt(pstContents->pCurChSubVector, ulIndex);
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(item->eGroupType, (HCHAR*)ppSubList[ulIndex]);
	}
	nx_menuPowerTimerSettings_UpdateListPopup(pstContents, ppSubList, pstContents->ulCurChSubNum);
}


static void 	nx_menuPowerTimerSettings_NxGroupItem_Delete(void *item)
{
	if(item){	NX_APP_Free(item);		item = NULL;	}
}

static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmCreate(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Set timer type
	pstContents->bIsOnTimer = (HBOOL)p1;

	// Load Menu Item Value
	nx_menuPowerTimerSettings_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	eResult = nx_menuPowerTimerSettings_InitSettingsItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	pstContents->cbPrevApp = (ONDK_GWM_Callback_t)p3;

	// Draw Menu Item
	nx_menuPowerTimerSettings_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmDestroy(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	HLIB_VECTOR_Delete(pstContents->pServiceGroupVector);
	HLIB_VECTOR_Delete(pstContents->pAlphabetGroupVector);
	if (pstContents->pstChannelList != NULL)
	{
		NX_APP_Free(pstContents->pstChannelList);
	}
	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuPowerTimerSettingsSetContent_t));
	nx_menuPowerTimerSettings_DestroyListPopup();
	nx_menuPowerTimerSettings_DestroyTimeDialog();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmHapiMsg(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_MsgAppInputValueChange(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HUINT32 ulUnixTime)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;
	HxDATETIME_t	stDateTime;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (lItemIdx)
	{
		case eNxSettingsItem_Time:
			HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixTime, &stDateTime);
			pstSubItem->lSubItemValue = (HINT32)ulUnixTime;
			HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
			HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%02d:%02d", stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute);
			nx_menuPowerTimerSettings_UpdateSubItem(pstContents, lObjId);
			break;
		default:
			NX_APP_Error("[%s:%d] Error!! Not Supported itme lItemIdx:[%d]\n", __FUNCTION__, __LINE__, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmClick(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 ulObjId)
{
	HINT32			lObjId = 0, lItemIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuPowerTimerSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_Channel:
		case eNxSettingsItem_Volume:
		case eNxSettingsItem_Repeat:
			nx_menuPowerTimerSettings_CreateListPopup(pstContents, ulObjId);
			break;
		case eNxSettingsItem_Time:
			if (pstContents->bIsOnTimer == TRUE)
			{
				nx_menuPowerTimerSettings_CreateTimeDialog(ONDK_GetString(RES_MESG_3309_ID));
			}
			else
			{
				nx_menuPowerTimerSettings_CreateTimeDialog(ONDK_GetString(RES_MESG_3308_ID));
			}
			break;
		case eNxSettingsItem_Ok:
			if (pstContents->bIsOnTimer == TRUE)
			{
				NX_SCHEDULER_SetPowerOn(pstContents->ulChannel, pstContents->ulTime, (HINT32)pstContents->eRepeat, pstContents->nVolume);
			}
			else
			{
				NX_SCHEDULER_SetPowerOff(pstContents->ulTime, eDxSCHED_DATE_NONE);
			}
			ONDK_GWM_SendMessage(pstContents->cbPrevApp, MSG_APP_SETTING_INPUT_VALUE_CHANGED, (HINT32)NULL, pstContents->ulTime, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		case eNxSettingsItem_Cancel:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmKeyDown(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
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
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			return 	nx_menuPowerTimerSettings_ChangeFocus(pstContents,lKeyId);
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
}

static ONDK_Result_t	nx_menuPowerTimerSettings_MsgGwmObjFocusChanged(Nx_MenuPowerTimerSettingsSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
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
ONDK_Result_t	NX_PRISM_MENU_PowerTimerSettings_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuPowerTimerSettingsSetContent_t	*pstContents = NULL;

	pstContents = nx_menuPowerTimerSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuPowerTimerSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuPowerTimerSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuPowerTimerSettings_MsgGwmClick(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuPowerTimerSettings_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuPowerTimerSettings_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuPowerTimerSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuPowerTimerSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_INPUT_VALUE_CHANGED!\n");
			nx_menuPowerTimerSettings_MsgAppInputValueChange(pstContents, p1);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuPowerTimerSettings_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			eResult = nx_menuPowerTimerSettings_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			{
				HINT32	nFocusObjId = 0, nChMenuObjId = 0;

				nFocusObjId = GWM_GetFocusedObjectID();
				nChMenuObjId = nx_menuPowerTimerSettings_ConvertItemIdxToObjId(eNxSettingsItem_Channel);
				if (nFocusObjId == nChMenuObjId)
				{
					nx_menuPowerTimerSettings_InitChSubList(pstContents);
				}
				break;
			}
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
