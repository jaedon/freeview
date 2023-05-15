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
 * @file	  		nx_prism_menu_settings.c
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

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_ParentalControl,
	eNxSettingsItem_Language,
	eNxSettingsItem_Time,
	eNxSettingsItem_Video,
	eNxSettingsItem_Audio,
	eNxSettingsItem_OnScreenDisplay,
	eNxSettingsItem_PrivacyPolicy,
	eNxSettingsItem_General_MAX
} NxSettingsItem_General_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SettingsNaviContext_t		s_stMenuGeneralContent;
static NX_PinDialogInstance_t		s_stMenuGeneral_pincode_dInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_SettingsNaviContext_t	*nx_menuGeneral_GetContents(void);
static inline HINT32	nx_menuGeneral_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents);

static HINT32			nx_menuGeneral_ConvertObjIdToCursorIndx(HINT32 lObjId);
static HINT32			nx_menuGeneral_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuGeneral_ConvertItemIdxToObjId(HINT32 lItemId);

static HBOOL			nx_menuGeneral_GetItemiDisable(NxSettingsItem_General_t eNxGenerallItem);
static void				nx_menuGeneral_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuGeneral_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuGeneral_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_DestroyPinCodeDialog(void);
static void 			nx_menuGeneral_CreatePinCodeDialog(void);

static ONDK_Result_t	nx_menuGeneral_AddItemParentalControl(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemLanguage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemTime(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemVideo(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemAudio(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemOnScreenDisplay(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_AddItemPrivacyPolicy(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuGeneral_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);

static ONDK_Result_t	nx_menuGeneral_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuGeneral_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuGeneral_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuGeneral_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_SettingsNaviContext_t	*nx_menuGeneral_GetContents(void)
{
	return	&s_stMenuGeneralContent;
}


static inline HINT32	nx_menuGeneral_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
{
	HUINT32			i = 0, ulMainItems = 0;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	ulMainItems = pstContents->ulNumOfMainItem;
	for (i = 0; i < ulMainItems; i++)
	{
		pMainItem =  NX_PRISM_AppNaviCntxt_GetMainItem((void *)pstContents, i);
		if ((pMainItem == NULL) || (pMainItem->bDisable == TRUE))
		{
			continue;
		}

		break;
	}

	return	i;
}


static HINT32	nx_menuGeneral_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuGeneral_GetContents();
	ulMainItems = pstContents->ulNumOfMainItem;
	for (i = 0; i < ulMainItems; i++)
	{
		pMainItem =  NX_PRISM_AppNaviCntxt_GetMainItem((void *)pstContents, i);
		if (pMainItem == NULL)
		{
			continue;
		}

		if (pMainItem->ulMainBtnId == (HUINT32)lObjId)
		{
			break;
		}
	}

	if (i >= ulMainItems)
	{
		i = 0;
	}

	return	(HINT32)i;
}


static HINT32	nx_menuGeneral_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_General);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_General_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuGeneral_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_General_MAX))
	{
		return 0;
	}

	return	(eNxSetting_General | lItemId);
}


static HBOOL	nx_menuGeneral_GetItemiDisable(NxSettingsItem_General_t eNxGenerallItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxGenerallItem)
	{
		case eNxSettingsItem_ParentalControl:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Language:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Video:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Time:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Audio:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_OnScreenDisplay:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_PrivacyPolicy:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuGeneral_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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


static ONDK_Result_t	nx_menuGeneral_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					lItemIdx = -1;
	HUINT32					ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuGeneral_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_ParentalControl:
			case eNxSettingsItem_Language:
			case eNxSettingsItem_Time:
			case eNxSettingsItem_Video:
			case eNxSettingsItem_Audio:
			case eNxSettingsItem_OnScreenDisplay:
			case eNxSettingsItem_PrivacyPolicy:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				break;

			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				break;
		}

		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stMainItem.ucAppName, ulProperty, stMainItem.appToLaunch,
						stMainItem.hAction, stMainItem.ulappParam1, stMainItem.ulappParam2, stMainItem.lSubItemValue/* Prev Cursor*/);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuGeneral_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stMenuGeneral_pincode_dInstance, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_menuGeneral_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuGeneral_DisplayPinCodeDialog);
}

static void		nx_menuGeneral_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stMenuGeneral_pincode_dInstance,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuGeneral_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_menuGeneral_DisplayPinCodeDialog, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_menuGeneral_AddItemParentalControl(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_ParentalControl);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_ParentalControl);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_ParentalControl);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_ParentalControl_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_ParentalControl);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemLanguage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_Language);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_Language);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_Language);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Language_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_Language);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemTime(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_Time);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_Time);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_Time);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = RES_VIDEO_ID;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(RES_TIME_ID), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Time_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_Time);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemVideo(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_Video);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_Video);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_Video);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = RES_VIDEO_ID;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(RES_VIDEO_ID), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Video_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NX_PRISM_MENU_Video_Proc;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_Video);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemAudio(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_Audio);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_Audio);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_Audio);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Audio_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_Audio);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemOnScreenDisplay(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_OnScreenDisplay);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_OnScreenDisplay);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_OnScreenDisplay);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NX_PRISM_MENU_Osd_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_OnScreenDisplay);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_AddItemPrivacyPolicy(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_General, eNxSetting_PrivacyPolicy);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_PrivacyPolicy);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuGeneral_ConvertItemIdxToObjId(eNxSettingsItem_PrivacyPolicy);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuGeneral_GetItemiDisable(eNxSettingsItem_PrivacyPolicy);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuGeneral_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Init Main Items
	NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1);

	// Init Sub Items
	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(pstContents->mainItems, pstContents->lCurFocus, TRUE);

	/* check focus */
	if(pstContents->mainItems[pstContents->lCurFocus].bDisable)
	{
		/* 현재 선택된 아이템이 Disable 이면 Focus 를 초기화 해야만 SubItemList 정보가 제대로 표시된다.*/
		pstContents->lDefaultFocusIdx = nx_menuGeneral_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}

	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuGeneral_GetDefaultFocus(pstContents);
		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	HUINT32				i = 0, ulMenuItemNum = 0;
	NxUiMenuCfgInfo		*pstMenuInfo = NULL;

	/* param check */
	if(pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = NX_PRISM_AppNaviCntxt_InitMainItemArray(pstContents);
	if(eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_GENERAL_SETTINGS_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuGeneral_ConvertObjIdToCursorIndx;

	ulMenuItemNum = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(eNxSetting_General);
	for (i = 0; i < ulMenuItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(eNxSetting_General, i);
		switch (pstMenuInfo->lMenuId)
		{
			case eNxSetting_ParentalControl:
				nx_menuGeneral_AddItemParentalControl(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_Language:
				nx_menuGeneral_AddItemLanguage(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_Time:
				nx_menuGeneral_AddItemTime(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_Video:
				nx_menuGeneral_AddItemVideo(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_Audio:
				nx_menuGeneral_AddItemAudio(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_OnScreenDisplay:
				nx_menuGeneral_AddItemOnScreenDisplay(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_PrivacyPolicy:
				nx_menuGeneral_AddItemPrivacyPolicy(pstContents, lPrevMenuIdx);
				break;
			default:
				NX_APP_Error("Error!!! Invalid Menu Item : (0x%x) Fail !! \n", pstMenuInfo->lMenuId);
				break;
		}
	}

	pstContents->lDefaultFocusIdx = nx_menuGeneral_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuGeneral_InitSettingsItems(pstContents, p3);
	nx_menuGeneral_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuGeneral_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);
	nx_menuGeneral_DestroyPinCodeDialog();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_ARROWLEFT:
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_menuGeneral_DisplayPinCodeDialog) != NULL)
			{
				nx_menuGeneral_DestroyPinCodeDialog();
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Settings_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Settings_Proc, 0, 0, 0, 0);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_menuGeneral_DisplayPinCodeDialog) != NULL)
			{
				nx_menuGeneral_DestroyPinCodeDialog();
				return	ONDK_MESSAGE_PASS;
			}
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_GUIDE:
		case KEY_STANDBY:
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
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if(nx_menuGeneral_ConvertObjIdToItemIdx(p1) == eNxSettingsItem_ParentalControl)
	{
		nx_menuGeneral_CreatePinCodeDialog();
	}
	else
	{
		nx_menuGeneral_LaunchSubMenu(pstContents, p1);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuGeneral_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuGeneral_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_General_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SettingsNaviContext_t	*pstContents = NULL;

	pstContents = nx_menuGeneral_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuGeneral_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuGeneral_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuGeneral_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_menuGeneral_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			eResult = nx_menuGeneral_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuGeneral_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			nx_menuGeneral_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			eResult = nx_menuGeneral_LaunchSubMenu(pstContents, eNxSettingsItem_ParentalControl);
			return ONDK_MESSAGE_BREAK;

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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

