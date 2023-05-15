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
 * @file	  		nx_prism_menu_factory_default.c
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
	eNxSettingsItem_ResetTVApps,
	eNxSettingsItem_ResetUserSettings,
	eNxSettingsItem_ResetAllSettings,
	eNxSettingsItem_ResetAllSettingsAndFormatHDD,
	eNxSettingsItem_FactoryDefault_MAX
} NxSettingsItem_FactoryDefault_e;

typedef struct tag_NX_MENUFACTORYDEFAULT_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32					lPrevCurIdx;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_MenuFactoryDefaultSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuFactoryDefaultSetContent_t	s_stFactoryDefaultContent;
static NXUI_DialogInstance_t				s_stFactoryDefault_selective_dInstance;
static NX_PinDialogInstance_t				s_stFactoryDefault_pincode_dInstance;
static NXUI_DialogInstance_t				s_stFactoryDefault_progress_dInstance;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuFactoryDefault_SelectiveDialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuFactoryDefault_CreateSelectiveDialog(Nx_MenuFactoryDefaultSetContent_t *pstContents, NxSettingsItem_FactoryDefault_e eDialogType, HUINT8 *szStrID);
static ONDK_Result_t	nx_menuFactoryDefault_DestroySelectiveDialog(void);
static ONDK_Result_t	nx_menuFactoryDefault_PinCodeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuFactoryDefault_CreatePinCodeDialog(NxSettingsItem_FactoryDefault_e eDialogType);
static ONDK_Result_t	nx_menuFactoryDefault_DestroyPinCodeDialog(void);
static ONDK_Result_t	nx_menuFactoryDefault_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuFactoryDefault_CreateProcessingMsg(void);
static ONDK_Result_t	nx_menuFactoryDefault_DestroyProcessingMsg(void);

static inline Nx_MenuFactoryDefaultSetContent_t* nx_menuFactoryDefault_GetContents(void);
static HCHAR*			nx_menuFactoryDefault_GetMenuStrIdFormObjId(HINT32 lObjId);
static HINT32			nx_menuFactoryDefault_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuFactoryDefault_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuFactoryDefault_GetItemSupport(NxSettingsItem_FactoryDefault_e eNxMenuFactoryDefaultItem);
static HBOOL			nx_menuFactoryDefault_GetItemiDisable(NxSettingsItem_FactoryDefault_e eNxMenuFactoryDefaultItem);
static void				nx_menuFactoryDefault_FindNextFocus(Nx_MenuFactoryDefaultSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuFactoryDefault_UpdateCurrentTime(Nx_MenuFactoryDefaultSetContent_t *pstContents);

static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetTVApps(Nx_MenuFactoryDefaultSetContent_t *pstContents);
static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetUserSettings(Nx_MenuFactoryDefaultSetContent_t *pstContents);
static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetAllSettings(Nx_MenuFactoryDefaultSetContent_t *pstContents);
static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetAllSettingsAndFormatHDD(Nx_MenuFactoryDefaultSetContent_t *pstContents);
static ONDK_Result_t	nx_menuFactoryDefault_InitSettingsItems(Nx_MenuFactoryDefaultSetContent_t *pstContents);

static ONDK_Result_t	nx_menuFactoryDefault_DrawDefaultUI(Nx_MenuFactoryDefaultSetContent_t *pstContents);
static ONDK_Result_t	nx_menuFactoryDefault_DoFactoryDefault(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 ulDlgWhoseId);

static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmCreate(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuFactoryDefault_MsgGwmDestroy(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmHapiMsg(Nx_MenuFactoryDefaultSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmClick(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 ulObjId);
static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmKeyDown(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmTimer(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmObjFocusChanged(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFactoryDefault_MsgAppDlgClicked(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuFactoryDefault_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (&s_stFactoryDefault_selective_dInstance, message, hAction,p1, p2, p3);
}


static void	nx_menuFactoryDefault_CreateSelectiveDialog(Nx_MenuFactoryDefaultSetContent_t *pstContents, NxSettingsItem_FactoryDefault_e eDialogType, HUINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stFactoryDefault_selective_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eDialogType;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)"Not exist str id!! Not exist str id!! Not exist str id!! Not exist str id!! Not exist str id!!"/*ONDK_GetString((HINT8*)szStrID)*/);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuFactoryDefault_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuFactoryDefault_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}


static ONDK_Result_t	nx_menuFactoryDefault_DestroySelectiveDialog(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuFactoryDefault_SelectiveDialogProc);
}


static ONDK_Result_t	nx_menuFactoryDefault_PinCodeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stFactoryDefault_pincode_dInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuFactoryDefault_CreatePinCodeDialog(NxSettingsItem_FactoryDefault_e eDialogType)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;
	NX_PinDialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stFactoryDefault_pincode_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NX_PinDialogInstance_t));

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(pstDialog,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eDialogType);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuFactoryDefault_PinCodeDialogProc", APP_DEFAULT_PRIORITY, nx_menuFactoryDefault_PinCodeDialogProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuFactoryDefault_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuFactoryDefault_PinCodeDialogProc);
}


static ONDK_Result_t	nx_menuFactoryDefault_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stFactoryDefault_progress_dInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuFactoryDefault_CreateProcessingMsg(void)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stFactoryDefault_progress_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);

	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(RES_MESG_1428_ID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuFactoryDefault_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuFactoryDefault_ProcessingMsgProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuFactoryDefault_DestroyProcessingMsg(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuFactoryDefault_ProcessingMsgProc);
}


static inline Nx_MenuFactoryDefaultSetContent_t* nx_menuFactoryDefault_GetContents(void)
{
	return	&s_stFactoryDefaultContent;
}


static HCHAR*	nx_menuFactoryDefault_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuFactoryDefault_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_ResetTVApps:
			return	RES_RESET_TV_APPS_ID;

		case eNxSettingsItem_ResetUserSettings:
			return	RES_RESET_USER_SETTINGS_ID;

		case eNxSettingsItem_ResetAllSettings:
			return 	RES_RESET_ALL_SETTINGS_ID;

		case eNxSettingsItem_ResetAllSettingsAndFormatHDD:
			return 	RES_MESG_4749_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static HINT32	nx_menuFactoryDefault_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_FactoryDefault);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_FactoryDefault_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuFactoryDefault_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_FactoryDefault_MAX))
	{
		return 0;
	}

	return	(eNxSetting_FactoryDefault | lItemId);
}


static HBOOL	nx_menuFactoryDefault_GetItemSupport(NxSettingsItem_FactoryDefault_e eNxMenuFactoryDefaultItem)
{
	HBOOL	bSupport = FALSE;

	switch (eNxMenuFactoryDefaultItem)
	{
		case eNxSettingsItem_ResetTVApps:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_ResetUserSettings:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_ResetAllSettings:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_ResetAllSettingsAndFormatHDD:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuFactoryDefault_GetItemiDisable(NxSettingsItem_FactoryDefault_e eNxMenuFactoryDefaultItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuFactoryDefaultItem)

	{
		case eNxSettingsItem_ResetTVApps:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ResetUserSettings:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ResetAllSettings:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ResetAllSettingsAndFormatHDD:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuFactoryDefault_FindNextFocus(Nx_MenuFactoryDefaultSetContent_t *pstContents, HBOOL bUp)
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


static void		nx_menuFactoryDefault_UpdateCurrentTime(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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


static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetTVApps(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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
	bSupport = nx_menuFactoryDefault_GetItemSupport(eNxSettingsItem_ResetTVApps);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ResetTVApps(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuFactoryDefault_ConvertItemIdxToObjId(eNxSettingsItem_ResetTVApps);

	// Check bDisable Item
	bDisable = nx_menuFactoryDefault_GetItemiDisable(eNxSettingsItem_ResetTVApps);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuFactoryDefault_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4750_ID);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetUserSettings(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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
	bSupport = nx_menuFactoryDefault_GetItemSupport(eNxSettingsItem_ResetUserSettings);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ResetUserSettings(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuFactoryDefault_ConvertItemIdxToObjId(eNxSettingsItem_ResetUserSettings);

	// Check bDisable Item
	bDisable = nx_menuFactoryDefault_GetItemiDisable(eNxSettingsItem_ResetUserSettings);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuFactoryDefault_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4751_ID);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetAllSettings(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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
	bSupport = nx_menuFactoryDefault_GetItemSupport(eNxSettingsItem_ResetAllSettings);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ResetAllSettings(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuFactoryDefault_ConvertItemIdxToObjId(eNxSettingsItem_ResetAllSettings);

	// Check bDisable Item
	bDisable = nx_menuFactoryDefault_GetItemiDisable(eNxSettingsItem_ResetAllSettings);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuFactoryDefault_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4752_ID);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuFactoryDefault_AddItemResetAllSettingsAndFormatHDD(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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
	bSupport = nx_menuFactoryDefault_GetItemSupport(eNxSettingsItem_ResetAllSettingsAndFormatHDD);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ResetTVApps(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuFactoryDefault_ConvertItemIdxToObjId(eNxSettingsItem_ResetAllSettingsAndFormatHDD);

	// Check bDisable Item
	bDisable = nx_menuFactoryDefault_GetItemiDisable(eNxSettingsItem_ResetAllSettingsAndFormatHDD
		);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuFactoryDefault_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Set Help String
	stSubItem.pszHelpText = ONDK_GetString(RES_MESG_4753_ID);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuFactoryDefault_InitSettingsItems(Nx_MenuFactoryDefaultSetContent_t *pstContents)
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

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_FACTORY_DEFAULT_03_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuFactoryDefault_AddItemResetTVApps(pstContents);
	nx_menuFactoryDefault_AddItemResetUserSettings(pstContents);
	nx_menuFactoryDefault_AddItemResetAllSettings(pstContents);
	nx_menuFactoryDefault_AddItemResetAllSettingsAndFormatHDD(pstContents);

	// Default Function
	nx_menuFactoryDefault_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFactoryDefault_DrawDefaultUI(Nx_MenuFactoryDefaultSetContent_t *pstContents)
{
	Nx_SettingsLeafSubItem_t stFocusItem = pstContents->subItems[pstContents->ulCurFocus];

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

    nx_menuFactoryDefault_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - HelpText
	NX_PRISM_COMP_Settings_DrawHelpText(stFocusItem.pszHelpText);

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1);
	ONDK_GWM_Obj_SetFocus(stFocusItem.ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmCreate(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	nx_menuFactoryDefault_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuFactoryDefault_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuFactoryDefault_DoFactoryDefault(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 ulDlgWhoseId)
{
	nx_menuFactoryDefault_DestroyPinCodeDialog();
	switch(ulDlgWhoseId)
	{
		case eNxSettingsItem_ResetTVApps:
			break;
		case eNxSettingsItem_ResetUserSettings:
			break;
		case eNxSettingsItem_ResetAllSettings:
			nx_menuFactoryDefault_CreateProcessingMsg();
			NX_SYSTEM_DoFactoryDefault();
			break;
		case eNxSettingsItem_ResetAllSettingsAndFormatHDD:
			break;
		default:
			NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			break;
	}

	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmDestroy(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuFactoryDefaultSetContent_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmHapiMsg(Nx_MenuFactoryDefaultSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmClick(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 ulObjId)
{
	HINT32 nObjId = 0, nItemIdx = -1;
	HUINT8 *pszStrID;
	NxSettingsItem_FactoryDefault_e eDialogType;

	nObjId = GWM_GetFocusedObjectID();
	nItemIdx = nx_menuFactoryDefault_ConvertObjIdToItemIdx(nObjId);

	switch(nItemIdx)
	{
		case eNxSettingsItem_ResetTVApps:
			pszStrID = (HUINT8*)RES_MESG_4752_ID;
			eDialogType = eNxSettingsItem_ResetTVApps;
			break;
		case eNxSettingsItem_ResetUserSettings:
			pszStrID = (HUINT8*)RES_MESG_4752_ID;
			eDialogType = eNxSettingsItem_ResetUserSettings;
			break;
		case eNxSettingsItem_ResetAllSettings:
			pszStrID = (HUINT8*)RES_MESG_4752_ID;
			eDialogType = eNxSettingsItem_ResetAllSettings;
			break;
		case eNxSettingsItem_ResetAllSettingsAndFormatHDD:
			pszStrID = (HUINT8*)RES_MESG_4752_ID;
			eDialogType = eNxSettingsItem_ResetAllSettingsAndFormatHDD;
			break;
		default:
			return	ONDK_RET_FAIL;
	}

	nx_menuFactoryDefault_CreateSelectiveDialog(pstContents, eDialogType, pszStrID);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmKeyDown(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
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


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmTimer(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuFactoryDefault_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFactoryDefault_MsgGwmObjFocusChanged(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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


static ONDK_Result_t	nx_menuFactoryDefault_MsgAppDlgClicked(Nx_MenuFactoryDefaultSetContent_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	nx_menuFactoryDefault_DestroySelectiveDialog();
	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			nx_menuFactoryDefault_CreatePinCodeDialog(ulDlgWhoseId);
		default:
			break;
	}


	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_FactoryDefault_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuFactoryDefaultSetContent_t	*pstContents = NULL;

	pstContents = nx_menuFactoryDefault_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuFactoryDefault_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuFactoryDefault_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			nx_menuFactoryDefault_MsgGwmClick(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuFactoryDefault_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuFactoryDefault_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuFactoryDefault_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuFactoryDefault_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuFactoryDefault_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			eResult = nx_menuFactoryDefault_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuFactoryDefault_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			eResult = nx_menuFactoryDefault_DoFactoryDefault(pstContents, p1);
			break;
		case MSG_APP_SYSTEM_PIN_DENIED:
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