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
	eNxSettingsItem_General,
	eNxSettingsItem_Network,
	eNxSettingsItem_Pvr,
	eNxSettingsItem_System,
	eNxSettingsItem_Channel,
	eNxSettingsItem_MAX
} NxSettingsItem_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SettingsNaviContext_t		s_stMenuSettingsContent;
static NX_PinDialogInstance_t		s_stMenuSetting_pincode_dInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_SettingsNaviContext_t	*nx_menuSettings_GetContents(void);
static inline HINT32	nx_menuSettings_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents);
static HINT32			nx_menuSettings_ConvertObjIdToCursorIndx(HINT32 lObjId);
static HINT32			nx_menuSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuSettings_GetItemiDisable(NxSettingsItem_t eNxSettingslItem);

static void				nx_menuSettings_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSettings_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_DestroyPinCodeDialog(void);
static void				nx_menuSettings_CreatePinCodeDialog(void);
static ONDK_Result_t	nx_menuSettings_AddItemGeneralSettings(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_AddItemNetworkSettings(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_AddItemPVRSettings(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_AddItemSystemSettings(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_AddItemChannelSettings(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSettings_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuSettings_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuSettings_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSettings_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSettings_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_SettingsNaviContext_t	*nx_menuSettings_GetContents(void)
{
	return	&s_stMenuSettingsContent;
}


static inline HINT32	nx_menuSettings_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
{
	HUINT32 		i = 0, ulMainItems = 0;
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


static HINT32	nx_menuSettings_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuSettings_GetContents();
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


static HINT32	nx_menuSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_MenuSettings);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_MAX))
	{
		return 0;
	}

	return	(eNxSetting_MenuSettings | lItemId);
}


static HBOOL	nx_menuSettings_GetItemiDisable(NxSettingsItem_t eNxSettingslItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxSettingslItem)
	{
		case eNxSettingsItem_General:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Network:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Pvr:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_System:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Channel:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuSettings_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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


static ONDK_Result_t	nx_menuSettings_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					lItemIdx = -1;
	HUINT32					ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuSettings_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_General:
			case eNxSettingsItem_System:
			case eNxSettingsItem_Network:
			case eNxSettingsItem_Pvr:
			case eNxSettingsItem_Channel:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				break;

			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				break;
		}

		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stMainItem.ucAppName, ulProperty, stMainItem.appToLaunch,
						stMainItem.hAction, stMainItem.ulappParam1, stMainItem.ulappParam2, stMainItem.lSubItemValue);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSettings_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stMenuSetting_pincode_dInstance, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_menuSettings_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuSettings_DisplayPinCodeDialog);
}

static void		nx_menuSettings_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stMenuSetting_pincode_dInstance,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ChannelSetting);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuGeneral_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_menuSettings_DisplayPinCodeDialog, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuSettings_AddItemGeneralSettings(Nx_SettingsNaviContext_t *pstContents)
{
	NxUiMainMenuCfgInfo			*pstMenuInfo = NULL;
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(eNxSetting_General);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(0x%x)\n", eNxSetting_General);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSettings_ConvertItemIdxToObjId(eNxSettingsItem_General);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_General_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.lSubItemValue = pstMenuInfo->lOrder;
	stMainItem.bDisable = nx_menuSettings_GetItemiDisable(eNxSettingsItem_General);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(&stMainItem, eNxSetting_General);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_AddItemNetworkSettings(Nx_SettingsNaviContext_t *pstContents)
{
	NxUiMainMenuCfgInfo			*pstMenuInfo = NULL;
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(eNxSetting_Network);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(0x%x)\n", eNxSetting_Network);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSettings_ConvertItemIdxToObjId(eNxSettingsItem_Network);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MEMNU_Network_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.lSubItemValue = pstMenuInfo->lOrder;
	stMainItem.bDisable = nx_menuSettings_GetItemiDisable(eNxSettingsItem_Network);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(&stMainItem, eNxSetting_Network);
#if defined(CONFIG_MW_INET)
	NX_PRISM_AppNaviCntxt_AppendSubItemTitleToMainItem (&stMainItem, RES_NETWORK_CONNECTION_STATUS_ID, NULL);
#endif

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_AddItemPVRSettings(Nx_SettingsNaviContext_t *pstContents)
{
	NxUiMainMenuCfgInfo			*pstMenuInfo = NULL;
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(eNxSetting_Pvr);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(0x%x)\n", eNxSetting_Pvr);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSettings_ConvertItemIdxToObjId(eNxSettingsItem_Pvr);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Pvr_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.lSubItemValue = pstMenuInfo->lOrder;
	stMainItem.bDisable = nx_menuSettings_GetItemiDisable(eNxSettingsItem_Pvr);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(&stMainItem, eNxSetting_Pvr);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_AddItemSystemSettings(Nx_SettingsNaviContext_t *pstContents)
{
	NxUiMainMenuCfgInfo			*pstMenuInfo = NULL;
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(eNxSetting_System);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(0x%x)\n", eNxSetting_System);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSettings_ConvertItemIdxToObjId(eNxSettingsItem_System);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_System_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.lSubItemValue = pstMenuInfo->lOrder;
	stMainItem.bDisable = nx_menuSettings_GetItemiDisable(eNxSettingsItem_System);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(&stMainItem, eNxSetting_System);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_AddItemChannelSettings(Nx_SettingsNaviContext_t *pstContents)
{
	NxUiMainMenuCfgInfo			*pstMenuInfo = NULL;
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(eNxSetting_Channel);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(0x%x)\n", eNxSetting_Channel);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSettings_ConvertItemIdxToObjId(eNxSettingsItem_Channel);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NX_PRISM_MENU_Channel_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.lSubItemValue = pstMenuInfo->lOrder;
	stMainItem.bDisable = nx_menuSettings_GetItemiDisable(eNxSettingsItem_Channel);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(&stMainItem, eNxSetting_Channel);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32			i = 0, ulMainItemNum = 0;
	NxUiMainMenuCfgInfo		*pstMenuInfo = NULL;

	/* param check */
	if (pstContents == NULL)
	{
		NX_APP_Info("Error!!! pstContents == NULL !! \n");
		return	ONDK_RET_FAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = NX_PRISM_AppNaviCntxt_InitMainItemArray(pstContents);
	if(eResult != ONDK_RET_OK)
	{
		NX_APP_Info("Error!!! NX_PRISM_AppNaviCntxt_InitMainItemArray() Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SETTINGS_01_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuSettings_ConvertObjIdToCursorIndx;

	ulMainItemNum = NX_PRISM_AppNaviCntxt_GetTotalMainMenuItemNum();
	for (i = 0; i < ulMainItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMainMenuItemByOrderId(i);
		switch (pstMenuInfo->lMenuId)
		{
			case eNxSetting_General:
				nx_menuSettings_AddItemGeneralSettings(pstContents);
				break;
			case eNxSetting_Network:
				nx_menuSettings_AddItemNetworkSettings(pstContents);
				break;
			case eNxSetting_Pvr:
				nx_menuSettings_AddItemPVRSettings(pstContents);
				break;
			case eNxSetting_System:
				nx_menuSettings_AddItemSystemSettings(pstContents);
				break;
			case eNxSetting_Channel:
				nx_menuSettings_AddItemChannelSettings(pstContents);
				break;
			default:
				NX_APP_Error("Error!!! Invalid Menu Item : (0x%x) Fail !! \n", pstMenuInfo->lMenuId);
				break;
		}
	}

	pstContents->lDefaultFocusIdx = nx_menuSettings_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	// Draw - Time
	nx_menuSettings_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Init Main Items
	NX_PRISM_COMP_Settings_InitNaviMainItems1Depth(pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Arrow);

	// Init Sub Items
	NX_PRISM_COMP_Settings_InitNaviMainSubItemList1Depth(pstContents->mainItems, pstContents->lCurFocus, TRUE);

	/* check focus */
	if(pstContents->mainItems[pstContents->lCurFocus].bDisable)
	{
		/* 현재 선택된 아이템이 Disable 이면 Focus 를 초기화 해야만 SubItemList 정보가 제대로 표시된다.*/
		pstContents->lDefaultFocusIdx = nx_menuSettings_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}

	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuSettings_GetDefaultFocus(pstContents);

		/* init sub items */
		NX_PRISM_COMP_Settings_InitNaviMainSubItemList1Depth(pstContents->mainItems, pstContents->lCurFocus, FALSE);

		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuSettings_InitSettingsItems(pstContents);
	nx_menuSettings_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuSettings_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);
	nx_menuSettings_DestroyPinCodeDialog();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
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
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		case KEY_ARROWLEFT:
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
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuSettings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL	bChMenuLock = FALSE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if(nx_menuSettings_ConvertObjIdToItemIdx(p1) == eNxSettingsItem_Channel)
	{
		NX_SYSTEM_GetChannelSetupPIN(&bChMenuLock);
		if (bChMenuLock == TRUE)
		{
			nx_menuSettings_CreatePinCodeDialog();
		}
		else
		{
			nx_menuSettings_LaunchSubMenu(pstContents, p1);
		}
	}
	else
	{
		nx_menuSettings_LaunchSubMenu(pstContents, p1);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSettings_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSettings_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Settings_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SettingsNaviContext_t	*pstContents = NULL;

	pstContents = nx_menuSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_menuSettings_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuSettings_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			eResult = nx_menuSettings_LaunchSubMenu(pstContents, eNxSettingsItem_Channel);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_menuSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			nx_menuSettings_MsgGwmTimer(pstContents, p1, p2, p3);
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

