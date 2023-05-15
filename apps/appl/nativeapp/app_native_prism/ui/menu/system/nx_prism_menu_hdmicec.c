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
 * @file	  		nx_prism_menu_hdmicec.c
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
	NxUiCEC_DialogType_None = 0,
	NxUiCEC_DialogType_VolumeCtrl,
}  NxUiCEC_DialogType_t;

typedef enum {
	eNxSettingsItem_HDMICEC_Power,
	eNxSettingsItem_HDMICEC_Volume,
	eNxSettingsItem_HDMICEC_MAX
} NxSettingsItem_HDMICEC_e;


typedef struct tag_nx_menuHDMICEC_SET_CONTENT
{
	/**@breif Public value */
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL		bCECPowerCtrl;
	HINT32		bCECVolumeCtrl;

	HINT32		lPrevCurIdx;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_MenuHDMICECSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuHDMICECSetContent_t		s_stPowerManagementContent;
static Nx_PrismListDialog_Content_t		s_stPopupListHdmiCECSetConts;
static NXUI_DialogInstance_t			s_stCECDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuHDMICEC_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_DestroyListPopup(void);
static void 			nx_menuHDMICEC_CreateListPopup(Nx_MenuHDMICECSetContent_t *pContents, HINT32 ulObjId);

static NXUI_DialogInstance_t	*nx_menuHDMICEC_GetDialogContents(void);
static ONDK_Result_t	nx_menuHDMICEC_DialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_DestroySelectDialog(void);
static ONDK_Result_t	nx_menuHDMICEC_CreateSelectDialog(Nx_MenuHDMICECSetContent_t *pstContents, NxUiCEC_DialogType_t eDialogType);

static inline Nx_MenuHDMICECSetContent_t* nx_menuHDMICEC_GetContents(void);
static HCHAR*			nx_menuHDMICEC_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuHDMICEC_GetMenuItemIdFormObjId(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuHDMICEC_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuHDMICEC_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuHDMICEC_GetItemSupport(NxSettingsItem_HDMICEC_e eNxMenuPowerManagementItem);
static HBOOL			nx_menuHDMICEC_GetItemiDisable(NxSettingsItem_HDMICEC_e eNxMenuPowerManagementItem);
static void				nx_menuHDMICEC_FindNextFocus(Nx_MenuHDMICECSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuHDMICEC_UpdateCurrentTime(Nx_MenuHDMICECSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHDMICEC_RefreshSubItem(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuHDMICEC_RefreshAllSubItems(Nx_MenuHDMICECSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHDMICEC_UpdateCECPower(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuHDMICEC_SetCECVolume(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId, HBOOL bSetVolume);
static ONDK_Result_t	nx_menuHDMICEC_UpdateCECVolume(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuHDMICEC_UpdateSubItem(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuHDMICEC_ChangeItemEnableDisableMode(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lItemIdx, HBOOL bEnable);
static ONDK_Result_t	nx_menuHDMICEC_ChangedSubItemValue(Nx_MenuHDMICECSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuHDMICEC_AddItemCECPower(Nx_MenuHDMICECSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHDMICEC_AddItemCECVolume(Nx_MenuHDMICECSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHDMICEC_LoadSettingsItemsValue(Nx_MenuHDMICECSetContent_t *pstContents);
static ONDK_Result_t	nx_menuHDMICEC_InitSettingsItems(Nx_MenuHDMICECSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHDMICEC_DrawDefaultUI(Nx_MenuHDMICECSetContent_t *pstContents);

static ONDK_Result_t	nx_menuHDMICEC_MsgGwmCreate(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuHDMICEC_MsgGwmDestroy(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_MsgGwmHapiMsg(Nx_MenuHDMICECSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_MsgGwmKeyDown(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_MsgGwmTimer(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_MsgGwmObjFocusChanged(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuHDMICEC_MsgAppDlgClicked(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuHDMICEC_MsgAppDlgCanceled(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __PopUplist_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuHDMICEC_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListHdmiCECSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuHDMICEC_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuHDMICEC_DisplayListPopup);
}


static void		nx_menuHDMICEC_CreateListPopup(Nx_MenuHDMICECSetContent_t *pContents, HINT32 ulObjId)
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
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListHdmiCECSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuHDMICEC_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuHDMICEC_DisplayListPopup, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuHDMICEC_GetDialogContents(void)
{
	return &s_stCECDialogMessageInstance;
}


static ONDK_Result_t	nx_menuHDMICEC_DialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_Message_Proc(nx_menuHDMICEC_GetDialogContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuHDMICEC_DestroySelectDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuHDMICEC_DialogProc);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_CreateSelectDialog(Nx_MenuHDMICECSetContent_t *pstContents, NxUiCEC_DialogType_t eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;


	pstDialog = nx_menuHDMICEC_GetDialogContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if (eDialogType == NxUiCEC_DialogType_VolumeCtrl)
	{
		ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
		pszMsg = RES_MESG_5558_ID;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
		return	ONDK_RET_FAIL;
	}

	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszMsg));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuHDMICEC_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuHDMICEC_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static inline Nx_MenuHDMICECSetContent_t* nx_menuHDMICEC_GetContents(void)
{
	return	&s_stPowerManagementContent;
}


static HCHAR*	nx_menuHDMICEC_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_HDMICEC_Power:
			return	RES_TV_POWER_CONTROL_ID;

		case eNxSettingsItem_HDMICEC_Volume:
			return	RES_TV_VOLUME_CONTROL_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuHDMICEC_GetMenuItemIdFormObjId(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_HDMICEC_Power:
			*pulMenuItemId = NX_MENUITEM_ID_SET_OFF_ON;
			break;

		case eNxSettingsItem_HDMICEC_Volume:
			*pulMenuItemId = NX_MENUITEM_ID_SET_OFF_ON;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static HINT32	nx_menuHDMICEC_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_HDMICEC);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_HDMICEC_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuHDMICEC_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_HDMICEC_MAX))
	{
		return 0;
	}

	return	(eNxSetting_HDMICEC | lItemId);
}


static HBOOL	nx_menuHDMICEC_GetItemSupport(NxSettingsItem_HDMICEC_e eNxMenuPowerManagementItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuPowerManagementItem)
	{
		case eNxSettingsItem_HDMICEC_Power:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_HDMICEC_Volume:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuHDMICEC_GetItemiDisable(NxSettingsItem_HDMICEC_e eNxMenuPowerManagementItem)
{
	HBOOL	bDisable = TRUE;
	Nx_MenuHDMICECSetContent_t	*pstContents = NULL;

	pstContents = nx_menuHDMICEC_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	FALSE;
	}

	switch (eNxMenuPowerManagementItem)
	{
		case eNxSettingsItem_HDMICEC_Power:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_HDMICEC_Volume:
			if (pstContents->bCECPowerCtrl == FALSE)
			{
				bDisable = TRUE;
			}
			else
			{
				bDisable = FALSE;
			}
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuHDMICEC_FindNextFocus(Nx_MenuHDMICECSetContent_t *pstContents, HBOOL bUp)
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


static void		nx_menuHDMICEC_UpdateCurrentTime(Nx_MenuHDMICECSetContent_t *pstContents)
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


static ONDK_Result_t	nx_menuHDMICEC_RefreshSubItem(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId)
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
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Fail - lObjId:(0x%x) !!\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuHDMICEC_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() Fail - ulMenuId:(0x%x) !!\n", ulMenuId);
		return	eResult;
	}

	lItemIdx = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_HDMICEC_Power:
			lSubItem = pstContents->bCECPowerCtrl;
			break;

		case eNxSettingsItem_HDMICEC_Volume:
			lSubItem = pstContents->bCECVolumeCtrl;
			break;

		default:
			lSubItem = -1;
			break;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(ulMenuId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail - ulMenuId:(0x%x) !!\n", ulMenuId);
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


static void		nx_menuHDMICEC_RefreshAllSubItems(Nx_MenuHDMICECSetContent_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_HDMICEC_Power; i < eNxSettingsItem_HDMICEC_MAX; i++)
	{
		eResult = nx_menuHDMICEC_RefreshSubItem(pstContents, nx_menuHDMICEC_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}


static ONDK_Result_t	nx_menuHDMICEC_UpdateCECPower(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId)
{
	HERROR		hError = ERR_FAIL;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->bCECPowerCtrl != (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->bCECPowerCtrl = (HBOOL)pSubItem->lSubItemValue;
		hError = NX_SYSTEM_SetHdmiCecMode(pstContents->bCECPowerCtrl);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetHdmiCecMode() - bCECPowerCtrl:(%d)\n", pstContents->bCECPowerCtrl);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_SetCECVolume(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId, HBOOL bSetVolume)
{
	HINT32			lItemIdx = -1, lValue = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lItemIdx = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_HDMICEC_Volume:
			/* Get Value Index */
			nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, (HUINT32)bSetVolume, &lValue);

			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = lValue;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, lValue, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuHDMICEC_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("Error!!! Only Action Volume ctrl lItemIdx:(%d)\n", lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_UpdateCECVolume(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId)
{
	HERROR		hError = ERR_FAIL;
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->bCECVolumeCtrl != (HBOOL)pSubItem->lSubItemValue)
	{
		pstContents->bCECVolumeCtrl = (HBOOL)pSubItem->lSubItemValue;
		hError = NX_SYSTEM_SetHdmiCecVolumeMode(pstContents->bCECVolumeCtrl);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetHdmiCecVolumeMode() - bCECVolumeCtrl:(%d)\n", pstContents->bCECVolumeCtrl);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_UpdateSubItem(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_HDMICEC_Power:
			nx_menuHDMICEC_UpdateCECPower(pstContents, lObjId);
			break;

		case eNxSettingsItem_HDMICEC_Volume:
			nx_menuHDMICEC_UpdateCECVolume(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_ChangeItemEnableDisableMode(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lItemIdx, HBOOL bEnable)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuHDMICEC_ConvertItemIdxToObjId(lItemIdx);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("pstSubItem is NULL - lObjId Id : [0x%x]\n",lObjId);
		return	ONDK_RET_FAIL;
	}

	if (bEnable == TRUE)
	{
		pstSubItem->bDisable = FALSE;
		ONDK_GWM_Obj_EnableObject(lObjId);
	}
	else
	{
		pstSubItem->bDisable = TRUE;
		ONDK_GWM_Obj_DisableObject(lObjId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_ChangedSubItemValue(Nx_MenuHDMICECSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HBOOL 			bEnable = FALSE;
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_HDMICEC_Volume:
			if (pstContents->bCECVolumeCtrl != (HBOOL)ulCurIdx)
			{
				if ((HBOOL)ulCurIdx == TRUE)
				{
					nx_menuHDMICEC_CreateSelectDialog(pstContents, NxUiCEC_DialogType_VolumeCtrl);
				}
				else
				{
					pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
					pSubItem->ulIdxOfTextList = ulCurIdx;
					HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

					/* Update subItem value */
					nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
					NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

					/* Update value */
					nx_menuHDMICEC_UpdateSubItem(pstContents, lObjId);
				}
			}
			break;

		case eNxSettingsItem_HDMICEC_Power:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuHDMICEC_UpdateSubItem(pstContents, lObjId);

			// Updte CEC Volume
			if ((pstContents->bCECPowerCtrl == FALSE) && (pstContents->bCECVolumeCtrl == TRUE))
			{
				lObjId = nx_menuHDMICEC_ConvertItemIdxToObjId(eNxSettingsItem_HDMICEC_Volume);
				lItemIdx = nx_menuHDMICEC_ConvertObjIdToItemIdx(lObjId);
				nx_menuHDMICEC_SetCECVolume(pstContents, lObjId, FALSE);
			}

			bEnable = (pstContents->bCECPowerCtrl == TRUE) ? TRUE : FALSE;
			nx_menuHDMICEC_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_HDMICEC_Volume, bEnable);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_AddItemCECVolume(Nx_MenuHDMICECSetContent_t *pstContents)
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
		NX_APP_Error("Error!!! (pstContents == NULL) !!\n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuHDMICEC_GetItemSupport(eNxSettingsItem_HDMICEC_Volume);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_HDMICEC_Volume(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuHDMICEC_ConvertItemIdxToObjId(eNxSettingsItem_HDMICEC_Volume);

	// Check bDisable Item
	bDisable = nx_menuHDMICEC_GetItemiDisable(eNxSettingsItem_HDMICEC_Volume);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuHDMICEC_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_5557_ID);

	/* Count SubItems */
	nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() Fail - ulMenuItemId:(0x%x) !!\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail - ulMenuItemId:(0x%x) !!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->bCECVolumeCtrl), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->bCECVolumeCtrl;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_AddItemCECPower(Nx_MenuHDMICECSetContent_t *pstContents)
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
		NX_APP_Error("Error!!! (pstContents == NULL) !!\n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuHDMICEC_GetItemSupport(eNxSettingsItem_HDMICEC_Power);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_HDMICEC_Power(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuHDMICEC_ConvertItemIdxToObjId(eNxSettingsItem_HDMICEC_Power);

	// Check bDisable Item
	bDisable = nx_menuHDMICEC_GetItemiDisable(eNxSettingsItem_HDMICEC_Power);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuHDMICEC_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_5556_ID);

	/* Count SubItems */
	nx_menuHDMICEC_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() Fail - ulMenuItemId:(0x%x) !!\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail - ulMenuItemId:(0x%x) !!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->bCECPowerCtrl), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->bCECPowerCtrl;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_LoadSettingsItemsValue(Nx_MenuHDMICECSetContent_t *pstContents)
{
	HBOOL		bCECPowerCtrl = FALSE, bCECVolumeCtrl = FALSE;

	/* Get CEC HDMI Power Mode */
	NX_SYSTEM_GetHdmiCecMode(&bCECPowerCtrl);
	pstContents->bCECPowerCtrl = bCECPowerCtrl;

	/* Get CEC Volume Mode */
	NX_SYSTEM_GetHdmiCecVolumeMode(&bCECVolumeCtrl);
	pstContents->bCECVolumeCtrl = bCECVolumeCtrl;

	if ((pstContents->bCECPowerCtrl == FALSE) && (pstContents->bCECVolumeCtrl == TRUE))
	{
		NX_APP_Error("Error!!! Only Support CEC volume control when set on the CEC Power contorl !!\n");
		pstContents->bCECVolumeCtrl = FALSE;
		NX_SYSTEM_SetHdmiCecVolumeMode(pstContents->bCECVolumeCtrl);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_InitSettingsItems(Nx_MenuHDMICECSetContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) !!\n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_InitSubitemArray() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_HDMI_CEC_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuHDMICEC_AddItemCECPower(pstContents);
	nx_menuHDMICEC_AddItemCECVolume(pstContents);

	// Default Function
	nx_menuHDMICEC_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_DrawDefaultUI(Nx_MenuHDMICECSetContent_t *pstContents)
{
	Nx_SettingsLeafSubItem_t stFocusItem = pstContents->subItems[pstContents->ulCurFocus];

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

        nx_menuHDMICEC_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - HelpText
	NX_PRISM_COMP_Settings_DrawHelpText(stFocusItem.pszHelpText);

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);
	ONDK_GWM_Obj_SetFocus(stFocusItem.ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmCreate(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuHDMICEC_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	nx_menuHDMICEC_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuHDMICEC_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmDestroy(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) !!\n");
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuHDMICECSetContent_t));
	nx_menuHDMICEC_DestroyListPopup();
	nx_menuHDMICEC_DestroySelectDialog();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmHapiMsg(Nx_MenuHDMICECSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmKeyDown(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) !!\n");
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
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_System_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc,
							 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_STANDBY:
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

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmTimer(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuHDMICEC_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgGwmObjFocusChanged(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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


static ONDK_Result_t	nx_menuHDMICEC_MsgAppDlgClicked(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	HINT32		lObjId = 0;

	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == NxUiCEC_DialogType_VolumeCtrl)
			{
				nx_menuHDMICEC_DestroySelectDialog();
				lObjId = GWM_GetFocusedObjectID();
				nx_menuHDMICEC_SetCECVolume(pstContents, lObjId, TRUE);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
		default:						// No
			if (ulDlgWhoseId == NxUiCEC_DialogType_VolumeCtrl)
			{
				nx_menuHDMICEC_DestroySelectDialog();
				lObjId = GWM_GetFocusedObjectID();
				nx_menuHDMICEC_SetCECVolume(pstContents, lObjId, FALSE);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
	}


	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuHDMICEC_MsgAppDlgCanceled(Nx_MenuHDMICECSetContent_t *pstContents, HINT32 ulDlgWhoseId)
{
	HINT32		lObjId = 0;

	if (ulDlgWhoseId == NxUiCEC_DialogType_VolumeCtrl)
	{
		nx_menuHDMICEC_DestroySelectDialog();
		lObjId = GWM_GetFocusedObjectID();
		nx_menuHDMICEC_SetCECVolume(pstContents, lObjId, FALSE);
	}
	else
	{
		NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
	}

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_HDMICEC_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuHDMICECSetContent_t	*pstContents = NULL;

	pstContents = nx_menuHDMICEC_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuHDMICEC_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuHDMICEC_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuHDMICEC_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuHDMICEC_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuHDMICEC_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuHDMICEC_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuHDMICEC_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuHDMICEC_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuHDMICEC_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			eResult = nx_menuHDMICEC_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuHDMICEC_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
			eResult = nx_menuHDMICEC_MsgAppDlgCanceled(pstContents, p2);
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
