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
 * @file	  		nx_prism_menu_pvr.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#if defined(CONFIG_MW_MM_PVR)

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

typedef enum
{
	eNxSettingsItem_IPEpg,
	eNxSettingsItem_RemoteRecord,
	eNxSettingsItem_RecordOption,
	eNxSettingsItem_PlaybackOption,
	eNxSettingsItem_Storage,
	eNxSettingsItem_Pvr_MAX
} NxSettingsItem_Pvr_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SettingsNaviContext_t		s_stMenuPvrContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_SettingsNaviContext_t	*nx_menuPvr_GetContents(void);
static inline HINT32	nx_menuPvr_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents);

static HINT32			nx_menuPvr_ConvertObjIdToCursorIndx(HINT32 lObjId);
static HINT32			nx_menuPvr_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuPvr_ConvertItemIdxToObjId(HINT32 lItemId);

static HBOOL			nx_menuPvr_GetItemiDisable(NxSettingsItem_Pvr_t eNxPvrItem);
static void				nx_menuPvr_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuPvr_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuPvr_AddItemIPEpg(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuPvr_AddItemRemoteRecord(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuPvr_AddItemRecordOption(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuPvr_AddItemPlaybackOption(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuPvr_AddItemStorage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuPvr_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);

static ONDK_Result_t	nx_menuPvr_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuPvr_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuPvr_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPvr_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPvr_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPvr_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPvr_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuPvr_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_SettingsNaviContext_t	*nx_menuPvr_GetContents(void)
{
	return	&s_stMenuPvrContent;
}


static inline HINT32	nx_menuPvr_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
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


static HINT32	nx_menuPvr_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuPvr_GetContents();
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


static HINT32	nx_menuPvr_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Pvr);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Pvr_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuPvr_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Pvr_MAX))
	{
		return 0;
	}

	return	(eNxSetting_Pvr | lItemId);
}


static HBOOL	nx_menuPvr_GetItemiDisable(NxSettingsItem_Pvr_t eNxPvrItem)
{
	HBOOL		bDisable = TRUE;
	HUINT32		ulStorageLength = 0;

	switch (eNxPvrItem)
	{
		case eNxSettingsItem_IPEpg:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_RemoteRecord:
			bDisable = TRUE;
			break;
		case eNxSettingsItem_RecordOption:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_PlaybackOption:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Storage:
			NX_FILEMGR_STORAGE_GetPhysicalStorageListLength(&ulStorageLength);
			if (ulStorageLength > 0)
			{
				bDisable = FALSE;
			}
			else
			{
				bDisable = TRUE;
			}
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuPvr_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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


static ONDK_Result_t	nx_menuPvr_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					lItemIdx = -1;
	HUINT32					ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuPvr_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_IPEpg:
			case eNxSettingsItem_RemoteRecord:
			case eNxSettingsItem_RecordOption:
			case eNxSettingsItem_PlaybackOption:
			case eNxSettingsItem_Storage:
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

static ONDK_Result_t	nx_menuPvr_AddItemIPEpg(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Pvr, eNxSetting_IPEpg);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_Storage);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuPvr_ConvertItemIdxToObjId(eNxSettingsItem_IPEpg);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_IpEpg_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuPvr_GetItemiDisable(eNxSettingsItem_IPEpg);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPvr_AddItemRemoteRecord(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Pvr, eNxSetting_RemoteRecord);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_RemoteRecord);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuPvr_ConvertItemIdxToObjId(eNxSettingsItem_RemoteRecord);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_RemoteRecord_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuPvr_GetItemiDisable(eNxSettingsItem_RemoteRecord);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPvr_AddItemRecordOption(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Pvr, eNxSetting_RecordOption);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_RecordOption);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuPvr_ConvertItemIdxToObjId(eNxSettingsItem_RecordOption);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_RecordOption_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuPvr_GetItemiDisable(eNxSettingsItem_RecordOption);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPvr_AddItemPlaybackOption(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Pvr, eNxSetting_PlaybackOption);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_PlaybackOption);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuPvr_ConvertItemIdxToObjId(eNxSettingsItem_PlaybackOption);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_PlaybackOption_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuPvr_GetItemiDisable(eNxSettingsItem_PlaybackOption);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPvr_AddItemStorage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Pvr, eNxSetting_Storage);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_Storage);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuPvr_ConvertItemIdxToObjId(eNxSettingsItem_Storage);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_Storage_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuPvr_GetItemiDisable(eNxSettingsItem_Storage);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuPvr_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuPvr_UpdateCurrentTime(pstContents);
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
		pstContents->lDefaultFocusIdx = nx_menuPvr_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}

	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuPvr_GetDefaultFocus(pstContents);
		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
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

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_PVR_SETTINGS_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuPvr_ConvertObjIdToCursorIndx;

	ulMenuItemNum = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(eNxSetting_Pvr);
	for (i = 0; i < ulMenuItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(eNxSetting_Pvr, i);
		switch (pstMenuInfo->lMenuId)
		{
			case eNxSetting_IPEpg:
				nx_menuPvr_AddItemIPEpg(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_RemoteRecord:
				nx_menuPvr_AddItemRemoteRecord(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_RecordOption:
				nx_menuPvr_AddItemRecordOption(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_PlaybackOption:
				nx_menuPvr_AddItemPlaybackOption(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_Storage:
				nx_menuPvr_AddItemStorage(pstContents, lPrevMenuIdx);
				break;
			default:
				NX_APP_Error("Error!!! Invalid Menu Item : (0x%x) Fail !! \n", pstMenuInfo->lMenuId);
				break;
		}
	}

	pstContents->lDefaultFocusIdx = nx_menuPvr_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuPvr_InitSettingsItems(pstContents, p3);
	nx_menuPvr_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuPvr_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Settings_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Settings_Proc, 0, 0, 0, 0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
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
			NX_APP_Print("nx_menuPvr_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	nx_menuPvr_LaunchSubMenu(pstContents, p1);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuPvr_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuPvr_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Pvr_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SettingsNaviContext_t	*pstContents = NULL;

	pstContents = nx_menuPvr_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuPvr_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuPvr_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuPvr_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_menuPvr_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			eResult = nx_menuPvr_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuPvr_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			nx_menuPvr_MsgGwmTimer(pstContents, p1, p2, p3);
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
#endif

