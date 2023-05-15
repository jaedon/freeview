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
 * @file	  		nx_prism_menu_language.c
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
	eNxSettingsItem_SystemLanguage,
	eNxSettingsItem_AudioLanguage,
	eNxSettingsItem_Audio2ndLanguage,
	eNxSettingsItem_SubtitleLanguage,
	eNxSettingsItem_Subtitle2ndLanguage,
	eNxSettingsItem_Language_MAX
} NxSettingsItem_Language_t;


typedef struct tag_NX_MENULANG_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8				ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HxLANG_Id_e			eSystemLang;		/**< Menu Language */
	HxLANG_Id_e			eAudioLang;			/**< Primary Audio Language */
	HxLANG_Id_e			eAudioLang_2nd;		/**< Secondary Audio Language */
	HxLANG_Id_e			eSbtlLang;			/**< Primary Subtitle Language */
	HxLANG_Id_e			eSbtlLang_2nd;		/**< Secondary Subtitle Language */

	HINT32				lPrevCurIdx;
	HBOOL				bSupportMultiAudioLang;
	HBOOL				bSupportMultiSbtlLang;

	HUINT8				szDate[NX_TEXT_SIZE_32];
	HUINT8				szTime[NX_TEXT_SIZE_32];
} Nx_MenuLangSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuLangSetContent_t			s_stLanguageContent;
static Nx_PrismListDialog_Content_t		s_stPopupListLangSetConts;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuLanguage_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuLanguage_DestroyListPopup(void);
static void 	nx_menuLanguage_CreateListPopup(Nx_MenuLangSetContent_t *pContents, HINT32 ulObjId);

static inline Nx_MenuLangSetContent_t* nx_menuLanguage_GetContents(void);
static HCHAR*			nx_menuLanguage_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuLanguage_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuLanguage_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuLanguage_GetItemSupport(NxSettingsItem_Language_t eNxMenuLanguageItem);
static HBOOL			nx_menuLanguage_GetItemiDisable(NxSettingsItem_Language_t eNxMenuLanguageItem);
static void				nx_menuLanguage_FindNextFocus(Nx_MenuLangSetContent_t *pstContents, HBOOL bUp);
static void				nx_menuLanguage_UpdateCurrentTime(Nx_MenuLangSetContent_t *pstContents);

static ONDK_Result_t	nx_menuLanguage_RefreshSubItem(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuLanguage_RefreshAllSubItems(Nx_MenuLangSetContent_t *pstContents);

static ONDK_Result_t	nx_menuLanguage_UpdateSystemLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_UpdateAudioLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_UpdateAudio2ndLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_UpdateSubtitleLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_UpdateSubtitle2ndLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_UpdateSubItem(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuLanguage_ChangedSubItemValue(Nx_MenuLangSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuLanguage_LoadSettingsItemsValue(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_AddItemSubtitle2ndLanguage(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_AddItemSubtitleLanguage(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_AddItemAudio2ndLanguage(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_AddItemAudioLanguage(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_AddItemSystemLanguage(Nx_MenuLangSetContent_t *pstContents);
static ONDK_Result_t	nx_menuLanguage_InitSettingsItems(Nx_MenuLangSetContent_t *pstContents);

static ONDK_Result_t	nx_menuLanguage_DrawDefaultUI(Nx_MenuLangSetContent_t *pstContents);

static ONDK_Result_t	nx_menuLanguage_MsgGwmCreate(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuLanguage_MsgGwmDestroy(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuLanguage_MsgGwmHapiMsg(Nx_MenuLangSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuLanguage_MsgGwmKeyDown(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuLanguage_MsgGwmTimer(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuLanguage_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListLangSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuLanguage_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuLanguage_DisplayListPopup);
}


static void		nx_menuLanguage_CreateListPopup(Nx_MenuLangSetContent_t *pContents, HINT32 ulObjId)
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
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListLangSetConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
										&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
										ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuLanguage_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuLanguage_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuLangSetContent_t* nx_menuLanguage_GetContents(void)
{
	return	&s_stLanguageContent;
}


static HCHAR*	nx_menuLanguage_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;
	Nx_MenuLangSetContent_t	*pstContents = NULL;

	pstContents = nx_menuLanguage_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	NULL;
	}

	lItemId = nx_menuLanguage_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SystemLanguage:
			return	RES_SYSTEM_LANGUAGE_ID;

		case eNxSettingsItem_AudioLanguage:
			if (pstContents->bSupportMultiAudioLang == TRUE)
			{
				return	RES_PRIMARY_AUDIO_LANGUAGE_ID;
			}
			else
			{
				return	RES_AUDIO_LANGUAGE_ID;
			}

		case eNxSettingsItem_SubtitleLanguage:
			return	RES_SUBTITLE_LANGUAGE_ID;

		case eNxSettingsItem_Audio2ndLanguage:
			if (pstContents->bSupportMultiAudioLang == TRUE)
			{
				return	RES_SECONDARY_AUDIO_LANGUAGE_ID;
			}
			else
			{
				return	RES_AUDIO_LANGUAGE_ID;
			}

		case eNxSettingsItem_Subtitle2ndLanguage:
			return	RES_SUBTITLE_LANGUAGE_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuLanguage_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32			lItemId = 0;

	lItemId = nx_menuLanguage_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SystemLanguage:
			*pulMenuItemId = NX_MENUITEM_ID_SET_SYSTEM_LANG;
			break;

		case eNxSettingsItem_AudioLanguage:
			*pulMenuItemId = NX_MENUITEM_ID_SET_AUDIO_LANG;
			break;

		case eNxSettingsItem_SubtitleLanguage:
			*pulMenuItemId = NX_MENUITEM_ID_SET_SUBTITLE_LANG;
			break;

		case eNxSettingsItem_Audio2ndLanguage:
			*pulMenuItemId = NX_MENUITEM_ID_SET_AUDIO_LANG;
			break;

		case eNxSettingsItem_Subtitle2ndLanguage:
			*pulMenuItemId = NX_MENUITEM_ID_SET_SUBTITLE_LANG;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuLanguage_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Language);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Language_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuLanguage_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Language_MAX))
	{
		return 0;
	}

	return	(eNxSetting_Language | lItemId);
}


static HBOOL	nx_menuLanguage_GetItemSupport(NxSettingsItem_Language_t eNxMenuLanguageItem)
{
	HBOOL			bSupport = FALSE;
	Nx_MenuLangSetContent_t	*pstContents = NULL;

	pstContents = nx_menuLanguage_GetContents();
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		bSupport = FALSE;
		return	bSupport;
	}

	switch (eNxMenuLanguageItem)
	{
		case eNxSettingsItem_SystemLanguage:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_AudioLanguage:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_SubtitleLanguage:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Audio2ndLanguage:
			if (pstContents->bSupportMultiAudioLang == TRUE)
			{
				bSupport = TRUE;
			}
			else
			{
				bSupport = FALSE;
			}
			break;
		case eNxSettingsItem_Subtitle2ndLanguage:
			bSupport = FALSE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuLanguage_GetItemiDisable(NxSettingsItem_Language_t eNxMenuLanguageItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuLanguageItem)
	{
		case eNxSettingsItem_SystemLanguage:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_AudioLanguage:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SubtitleLanguage:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Audio2ndLanguage:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Subtitle2ndLanguage:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuLanguage_UpdateCurrentTime(Nx_MenuLangSetContent_t *pstContents)
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


static void		nx_menuLanguage_FindNextFocus(Nx_MenuLangSetContent_t *pstContents, HBOOL bUp)
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


static ONDK_Result_t	nx_menuLanguage_RefreshSubItem(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
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
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Fail!! lObjId:(0x%x)]\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuLanguage_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuLanguage_GetMenuItemIdFormObjId(lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuId);
		return	eResult;
	}

	lItemIdx = nx_menuLanguage_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_SystemLanguage:
			lSubItem = pstContents->eSystemLang;
			break;

		case eNxSettingsItem_AudioLanguage:
			lSubItem = pstContents->eAudioLang;
			break;

		case eNxSettingsItem_Audio2ndLanguage:
			lSubItem = pstContents->eAudioLang_2nd;
			break;

		case eNxSettingsItem_SubtitleLanguage:
			lSubItem = pstContents->eSbtlLang;
			break;

		case eNxSettingsItem_Subtitle2ndLanguage:
			lSubItem = pstContents->eSbtlLang_2nd;
			break;

		default:
			lSubItem = -1;
			break;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId(ulMenuId, FALSE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuId:(0x%x)]\n", ulMenuId);
		return	ONDK_RET_FAIL;
	}

	pSubItem->ulNumOfItemTextList = ulItems;
	pSubItem->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	pSubItem->lSubItemValue = lSubItem;
	pSubItem->ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	return	ONDK_RET_OK;
}


static void		nx_menuLanguage_RefreshAllSubItems(Nx_MenuLangSetContent_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_SystemLanguage; i < eNxSettingsItem_Language_MAX; i++)
	{
		eResult = nx_menuLanguage_RefreshSubItem(pstContents, nx_menuLanguage_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}


static ONDK_Result_t	nx_menuLanguage_UpdateAudioLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eAudioLang != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->eAudioLang = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetAudioLanuage((HINT32)pstContents->eAudioLang);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_UpdateAudio2ndLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eAudioLang_2nd != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->eAudioLang_2nd = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetSecondAudioLanuage((HINT32)pstContents->eAudioLang_2nd);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_UpdateSubtitleLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eSbtlLang != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->eSbtlLang = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetSubtitleLanuage((HINT32)pstContents->eSbtlLang);

		if (pstContents->eSbtlLang == eLangID_NONE)
			NX_SYSTEM_ShowSubtitle(FALSE);
		else
			NX_SYSTEM_ShowSubtitle(TRUE);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_UpdateSubtitle2ndLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eSbtlLang_2nd != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->eSbtlLang_2nd = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetSecondSubtitleLanuage((HINT32)pstContents->eSbtlLang_2nd);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_UpdateSystemLanguage(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eSystemLang != (HxLANG_Id_e)pSubItem->lSubItemValue)
	{
		pstContents->eSystemLang = (HxLANG_Id_e)pSubItem->lSubItemValue;
		NX_SYSTEM_SetMenuLanuage((HINT32)pstContents->eSystemLang);
		nx_menuLanguage_RefreshAllSubItems(pstContents);
		HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_LANGUAGE_ID), MAX_ELEMENT_TEXT_LENGTH);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_UpdateSubItem(Nx_MenuLangSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuLanguage_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SystemLanguage:
			nx_menuLanguage_UpdateSystemLanguage(pstContents, lObjId);
			break;

		case eNxSettingsItem_AudioLanguage:
			nx_menuLanguage_UpdateAudioLanguage(pstContents, lObjId);
			break;

		case eNxSettingsItem_SubtitleLanguage:
			nx_menuLanguage_UpdateSubtitleLanguage(pstContents, lObjId);
			break;

		case eNxSettingsItem_Audio2ndLanguage:
			nx_menuLanguage_UpdateAudio2ndLanguage(pstContents, lObjId);
			break;

		case eNxSettingsItem_Subtitle2ndLanguage:
			nx_menuLanguage_UpdateSubtitle2ndLanguage(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_ChangedSubItemValue(Nx_MenuLangSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuLanguage_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SystemLanguage:
		case eNxSettingsItem_AudioLanguage:
		case eNxSettingsItem_SubtitleLanguage:
		case eNxSettingsItem_Audio2ndLanguage:
		case eNxSettingsItem_Subtitle2ndLanguage:
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuLanguage_GetMenuItemIdFormObjId(lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
			nx_menuLanguage_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_LoadSettingsItemsValue(Nx_MenuLangSetContent_t *pstContents)
{
	HINT32				lValue = -1;
	HERROR				hError = ERR_FAIL;
	HxLANG_Id_e			eLangId = eLangID_NONE;

	/* Get menu language */
	hError = NX_SYSTEM_GetMenuLanuage(&lValue);
	if (hError == ERR_FAIL)
	{
		eLangId = eLangID_English;
	}
	else
	{
		eLangId = (HxLANG_Id_e)lValue;
	}
	pstContents->eSystemLang = eLangId;

	/* Get audio language */
	hError = NX_SYSTEM_GetAudioLanuage(&lValue);
	if (hError == ERR_FAIL)
	{
		eLangId = eLangID_English;
	}
	else
	{
		eLangId = (HxLANG_Id_e)lValue;
	}
	pstContents->eAudioLang = eLangId;

	/* Get subtitle language */
	hError = NX_SYSTEM_GetSubtitleLanuage(&lValue);
	if (hError == ERR_FAIL)
	{
		eLangId = eLangID_NONE;
	}
	else
	{
		eLangId = (HxLANG_Id_e)lValue;
	}
	pstContents->eSbtlLang = eLangId;

	/* Get audio language */
	hError = NX_SYSTEM_GetSecondAudioLanuage(&lValue);
	if (hError == ERR_FAIL)
	{
		eLangId = eLangID_English;
	}
	else
	{
		eLangId = (HxLANG_Id_e)lValue;
	}
	pstContents->eAudioLang_2nd = eLangId;

	/* Get subtitle language */
	hError = NX_SYSTEM_GetSecondSubtitleLanuage(&lValue);
	if (hError == ERR_FAIL)
	{
		eLangId = eLangID_NONE;
	}
	else
	{
		eLangId = (HxLANG_Id_e)lValue;
	}
	pstContents->eSbtlLang_2nd = eLangId;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_AddItemSubtitle2ndLanguage(Nx_MenuLangSetContent_t *pstContents)
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
	bSupport = nx_menuLanguage_GetItemSupport(eNxSettingsItem_Subtitle2ndLanguage);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Subtitle2ndLanguage(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuLanguage_ConvertItemIdxToObjId(eNxSettingsItem_Subtitle2ndLanguage);

	// Check Disable Item
	bDisable = nx_menuLanguage_GetItemiDisable(eNxSettingsItem_Subtitle2ndLanguage);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuLanguage_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuLanguage_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eSbtlLang), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eSbtlLang;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_AddItemSubtitleLanguage(Nx_MenuLangSetContent_t *pstContents)
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
	bSupport = nx_menuLanguage_GetItemSupport(eNxSettingsItem_SubtitleLanguage);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SubtitleLanguage(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuLanguage_ConvertItemIdxToObjId(eNxSettingsItem_SubtitleLanguage);

	// Check Disable Item
	bDisable = nx_menuLanguage_GetItemiDisable(eNxSettingsItem_SubtitleLanguage);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuLanguage_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuLanguage_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eSbtlLang), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eSbtlLang;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_AddItemAudio2ndLanguage(Nx_MenuLangSetContent_t *pstContents)
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
	bSupport = nx_menuLanguage_GetItemSupport(eNxSettingsItem_Audio2ndLanguage);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Audio2ndLanguage(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuLanguage_ConvertItemIdxToObjId(eNxSettingsItem_Audio2ndLanguage);

	// Check bDisable Item
	bDisable = nx_menuLanguage_GetItemiDisable(eNxSettingsItem_Audio2ndLanguage);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuLanguage_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuLanguage_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eAudioLang), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eAudioLang;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_AddItemAudioLanguage(Nx_MenuLangSetContent_t *pstContents)
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
	bSupport = nx_menuLanguage_GetItemSupport(eNxSettingsItem_AudioLanguage);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_AudioLanguage(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuLanguage_ConvertItemIdxToObjId(eNxSettingsItem_AudioLanguage);

	// Check bDisable Item
	bDisable = nx_menuLanguage_GetItemiDisable(eNxSettingsItem_AudioLanguage);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuLanguage_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuLanguage_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eAudioLang), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eAudioLang;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_AddItemSystemLanguage(Nx_MenuLangSetContent_t *pstContents)
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
	bSupport = nx_menuLanguage_GetItemSupport(eNxSettingsItem_SystemLanguage);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SystemLanguage(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuLanguage_ConvertItemIdxToObjId(eNxSettingsItem_SystemLanguage);

	// Check bDisable Item
	bDisable = nx_menuLanguage_GetItemiDisable(eNxSettingsItem_SystemLanguage);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuLanguage_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuLanguage_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eSystemLang), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eSystemLang;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_InitSettingsItems(Nx_MenuLangSetContent_t *pstContents)
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
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_LANGUAGE_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuLanguage_AddItemSystemLanguage(pstContents);
	nx_menuLanguage_AddItemAudioLanguage(pstContents);
	nx_menuLanguage_AddItemAudio2ndLanguage(pstContents);
	nx_menuLanguage_AddItemSubtitleLanguage(pstContents);
	nx_menuLanguage_AddItemSubtitle2ndLanguage(pstContents);

	// Default Function
	nx_menuLanguage_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_DrawDefaultUI(Nx_MenuLangSetContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuLanguage_UpdateCurrentTime(pstContents);

	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_MsgGwmCreate(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8			*pszPanelStr = NULL;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuLanguage_LoadSettingsItemsValue(pstContents);

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
	}

	pstContents->bSupportMultiAudioLang = FALSE;
	pstContents->bSupportMultiSbtlLang = FALSE;

	switch (eOpType)
	{
		default:
			pstContents->bSupportMultiAudioLang = FALSE;
			pstContents->bSupportMultiSbtlLang = FALSE;
			break;
	}

	// Init Menu Item
	nx_menuLanguage_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuLanguage_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuLanguage_MsgGwmDestroy(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuLangSetContent_t));
	nx_menuLanguage_DestroyListPopup();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuLanguage_MsgGwmHapiMsg(Nx_MenuLangSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuLanguage_MsgGwmKeyDown(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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



static ONDK_Result_t	nx_menuLanguage_MsgGwmTimer(Nx_MenuLangSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuLanguage_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Language_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuLangSetContent_t	*pstContents = NULL;

	pstContents = nx_menuLanguage_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuLanguage_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuLanguage_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuLanguage_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuLanguage_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuLanguage_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuLanguage_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuLanguage_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuLanguage_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuLanguage_ChangedSubItemValue(pstContents, p1);
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
