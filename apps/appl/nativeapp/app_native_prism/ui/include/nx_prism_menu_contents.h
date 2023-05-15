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
 * @file	  		nx_prism_menu_contents.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


#ifndef __NX_PRISM_MENU_CONTENTS_H__
#define __NX_PRISM_MENU_CONTENTS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ondk.h>
#include <ondk_type.h>
#include <ondk_gfx.h>
#include <hlib.h>
#include <nx_prism_settings.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_PRISM_DEFAULT_SUBITEM_NUM_IN_CONTENTS					10


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
// 모든 메뉴 Contents structure는 앞 부분에 BaseMenuContents와 동일한 포맷을 반드시 맞추어야 한다.
// 공통함수화하기 위한 제1 조건이다.
typedef struct tag_Menu_BaseContents
{
	/* for drawing application window */
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

} BaseMenuContents;


typedef	struct	tagMenuCfgContents {
	HINT32			lOrder;
	HINT32			lMainMenuId;
	HINT32			lMenuId;
	HINT8			*pszTitleStrId;

	ONDK_GWM_Callback_t appToLaunch;
} NxUiMenuCfgInfo;

typedef	struct	tagMenuCfg_LinkedList {
	NxUiMenuCfgInfo		stMenuItem;

	struct	tagMenuCfg_LinkedList	*pNext;
}	NxUiMenuCfg_List;


typedef	struct	tagMainMenuCfgContents {
	HINT32			lOrder;
	HINT32			lMenuId;
	HINT8			*pszTitleStrId;

	ONDK_GWM_Callback_t appToLaunch;
} NxUiMainMenuCfgInfo;


typedef	struct	tagMainMenuCfg_LinkedList {
	NxUiMainMenuCfgInfo		stMainMenuItem;

	struct	tagMainMenuCfg_LinkedList	*pNext;
} NxUiMainMenuCfg_List;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
ONDK_Result_t		NX_PRISM_AppContents_InitSubitemArray (void *pvContents);
void				NX_PRISM_AppContents_FreeSubitemArray (void *pvContents);
ONDK_Result_t		NX_PRISM_AppContents_InitSubitemArray_KeepFocus(void *pvContents);
void				NX_PRISM_AppContents_FreeSubitemArray_KeepFocus(void *pvContents, HUINT32 ulFreeSize);
ONDK_Result_t		NX_PRISM_AppContents_SetFocus(void *pvContents, HUINT32 ulFocus);
ONDK_Result_t		NX_PRISM_AppContents_AppendSubitem (void *pvContents, Nx_SettingsLeafSubItem_t *pstSubItem);
ONDK_Result_t		NX_PRISM_AppContents_DeleteSubitem (void *pvContents, HUINT32 ulSubitemIdx);
ONDK_Result_t		NX_PRISM_AppContents_DeleteSubitemByObjId (void *pvContents, HUINT32 ulObjId);
ONDK_Result_t		NX_PRISM_AppContents_ClearSubitemByObjId (void *pvContents, HUINT32 ulObjId);
HUINT32				NX_PRISM_AppContents_CountSubitem (void *pvContents);
Nx_SettingsLeafSubItem_t*	NX_PRISM_AppContents_GetSubitem (void *pvContents, HUINT32 ulSubitemIdx);
Nx_SettingsLeafSubItem_t*	NX_PRISM_AppContents_GetCurrentSubitem (void *pvContents);
Nx_SettingsLeafSubItem_t*	NX_PRISM_AppContents_GetSubitemByObjId (void *pvContents, HUINT32 ulObjId);
ONDK_Result_t		NX_PRISM_AppContents_GetSubitemIndexByObjId (void *pvContents, HUINT32 ulObjId, HUINT32 *pulIndex);
ONDK_Result_t		NX_PRISM_AppContents_GetObjIdBySubitemIndex (void *pvContents, HUINT32 ulSubitemIdx, HUINT32 *pulObjId);
ONDK_Result_t		NX_PRISM_AppContents_SetUserInputValueToSubitem (void *pvContents, HINT32 lObjId);
void				NX_PRISM_AppContents_ClearLeafSubItem (Nx_SettingsLeafSubItem_t *pstSubItem);
ONDK_Result_t		NX_PRISM_AppContents_GetAvailableFirstSubitemObj (void *pvContents, HINT32 *pnObjId);
ONDK_Result_t		NX_PRISM_AppContents_GetAvailableLastSubitemObj (void *pvContents, HINT32 *pnObjId);

// Nx_SettingsNaviContext_t
ONDK_Result_t		NX_PRISM_AppNaviCntxt_InitMainItemArray (Nx_SettingsNaviContext_t *pstContents);
void				NX_PRISM_AppNaviCntxt_FreeMainItemArray (Nx_SettingsNaviContext_t *pstContents);
void				NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus (Nx_SettingsNaviContext_t *pstContents);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AppendMainItem (Nx_SettingsNaviContext_t *pstContents, Nx_SettingsNaviMainItem_t *pstMainItem);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_DeleteMainItem (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainitemIdx);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_DeleteMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_ClearMainItem (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainitemIdx);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_ClearMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId);
HUINT32				NX_PRISM_AppNaviCntxt_CountMainItem (Nx_SettingsNaviContext_t *pstContents);
Nx_SettingsNaviMainItem_t*	NX_PRISM_AppNaviCntxt_GetMainItem (Nx_SettingsNaviContext_t *pstContents, HINT32 lMainitemIdx);
Nx_SettingsNaviMainItem_t*	NX_PRISM_AppNaviCntxt_GetCurrentMainItem (Nx_SettingsNaviContext_t *pstContents);
Nx_SettingsNaviMainItem_t*	NX_PRISM_AppNaviCntxt_GetMainItemByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_GetMainItemIndexByObjId (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulObjId, HUINT32 *pulIndex);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_GetObjIdByMainItemIndex (Nx_SettingsNaviContext_t *pstContents, HUINT32 ulMainItemIdx, HUINT32 *pulObjId);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_InitNaviMainItem (Nx_SettingsNaviMainItem_t *pstMainItem);
void				NX_PRISM_AppNaviCntxt_ClearNaviMainItem (Nx_SettingsNaviMainItem_t *pstMainItem);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AppendSubItemTitleToMainItem (Nx_SettingsNaviMainItem_t *pstMainItem, HCHAR *hcLocId, HUINT8 *szSubItemTitle);

// Main Menu
void				NX_PRISM_MENUCONFIG_InitMainMenuData(void);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(Nx_SettingsNaviMainItem_t *pstMainItem);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AddMainMenuItem(NxUiMainMenuCfgInfo stMainMenuInfo);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_DeleteAllMainMenuItem(void);
NxUiMainMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMainMenuItemByMenuId(HINT32 lMenuId);
NxUiMainMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMainMenuItemByOrderId(HINT32 lIndex);
HUINT32				NX_PRISM_AppNaviCntxt_GetTotalMainMenuItemNum(void);

// MMenu
void				NX_PRISM_MENUCONFIG_InitSubMenuData(void);
void				NX_PRISM_MENUCONFIG_InitMenuLanguageData(void);
void				NX_PRISM_MENUCONFIG_InitAudioLanguageData(void);
void				NX_PRISM_MENUCONFIG_InitSubtitleLanguageData(void);
void				NX_PRISM_MENUCONFIG_InitMenuResolutionData(void);
void				NX_PRISM_MENUCONFIG_ReLoadResolutionDataByEDID(void);
void				NX_PRISM_MENUCONFIG_InitMenuCountryData(void);
void				NX_PRISM_MENUCONFIG_InitMenuParentalControlData(void);

ONDK_Result_t		NX_PRISM_AppNaviCntxt_DeleteAllMenuItem(void);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AddSubItemTitleToMenuItemList(Nx_SettingsNaviMainItem_t *pstMenuItem, HINT32 lMainMenuId);
ONDK_Result_t		NX_PRISM_AppNaviCntxt_AddMenuItem(NxUiMenuCfgInfo stMenuInfo, HINT32 lMainMenuId);
HUINT32				NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(HINT32 lMainMenuId);
NxUiMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(HINT32 lMainMenuId, HINT32 lIndex);
NxUiMenuCfgInfo* 	NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(HINT32 lMainMenuId, HINT32 lMenuId);

#endif // __NX_PRISM_MENU_CONTENTS_H__

