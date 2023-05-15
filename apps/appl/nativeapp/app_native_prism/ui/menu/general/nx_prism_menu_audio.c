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
 * @file	  		nx_prism_menu_audio.c
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
	eNxSettingsItem_SPDIF,
	eNxSettingsItem_HDMI,
	eNxSettingsItem_AudioType,
	eNxSettingsItem_LipSyncDelay,
	eNxSettingsItem_Audio_MAX
} NxSettingsItem_Audio_t;


typedef struct tag_NX_MENUAUDIO_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	DxDigitalAudioFormat_e	eSPDIF;
	DxDigitalAudioFormat_e	eHDMI;
	DxStereoSelect_e		eAudioType;
	HUINT32					ulLipSyncDelay;

	HINT32					lPrevCurIdx;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

} Nx_MenuAudioSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuAudioSetContent_t			s_stVideoContent;
static Nx_PrismListDialog_Content_t		s_stPopupListAudioSetConts;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuAudio_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuAudio_DestroyListPopup(void);
static void 			nx_menuAudio_CreateListPopup(Nx_MenuAudioSetContent_t *pContents, HINT32 ulObjId);

static inline Nx_MenuAudioSetContent_t* nx_menuAudio_GetContents(void);
static HCHAR*			nx_menuAudio_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_GetMenuItemIdFormObjId(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuAudio_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuAudio_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuAudio_GetItemSupport(NxSettingsItem_Audio_t eNxMenuLanguageItem);
static HBOOL			nx_menuAudio_GetItemiDisable(NxSettingsItem_Audio_t eNxMenuLanguageItem);
static void				nx_menuAudio_UpdateCurrentTime(Nx_MenuAudioSetContent_t *pstContents);

static void				nx_menuAudio_FindNextFocus(Nx_MenuAudioSetContent_t *pstContents, HBOOL bUp);

static ONDK_Result_t	nx_menuAudio_RefreshSubItem(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static void				nx_menuAudio_RefreshAllSubItems(Nx_MenuAudioSetContent_t *pstContents);

static ONDK_Result_t	nx_menuAudio_UpdateSPDIF(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_UpdateHDMI(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_UpdateAudioType(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_UpdateSubItem(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_ChangedSubItemValue(Nx_MenuAudioSetContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuAudio_LoadSettingsItemsValue(Nx_MenuAudioSetContent_t *pstContents);
static ONDK_Result_t	nx_menuAudio_AddItemLipSyncDelay(Nx_MenuAudioSetContent_t *pstContents);
static ONDK_Result_t	nx_menuAudio_AddItemAudioType(Nx_MenuAudioSetContent_t *pstContents);
static ONDK_Result_t	nx_menuAudio_AddItemHDMI(Nx_MenuAudioSetContent_t *pstContents);
static ONDK_Result_t	nx_menuAudio_AddItemSPDIF(Nx_MenuAudioSetContent_t *pstContents);
static ONDK_Result_t	nx_menuAudio_InitSettingsItems(Nx_MenuAudioSetContent_t *pstContents);

static ONDK_Result_t	nx_menuAudio_DrawDefaultUI(Nx_MenuAudioSetContent_t *pstContents);

static ONDK_Result_t	nx_menuAudio_MsgGwmCreate(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuAudio_MsgGwmDestroy(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuAudio_MsgGwmHapiMsg(Nx_MenuAudioSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuAudio_MsgGwmClicked(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuAudio_MsgGwmKeyDown(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuAudio_MsgGwmTimer(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuAudio_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListAudioSetConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuAudio_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuAudio_DisplayListPopup);
}


static void		nx_menuAudio_CreateListPopup(Nx_MenuAudioSetContent_t *pContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *pstSubItem;

	/* Get subitem matched nObjId */
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents, ulObjId);
	if ( pstSubItem == NULL )
	{
		return;
	}

	pptextList = pstSubItem->szItemTextList;
	ulTextListNum = pstSubItem->ulNumOfItemTextList;
	ulIndex	= pstSubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListAudioSetConts, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuAudio_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuAudio_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuAudioSetContent_t* nx_menuAudio_GetContents(void)
{
	return	&s_stVideoContent;
}


static HCHAR*	nx_menuAudio_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SPDIF:
			return	RES_S_PDIF_ID;

		case eNxSettingsItem_HDMI:
			return	RES_HDMI_ID;

		case eNxSettingsItem_AudioType:
			return	RES_AUDIO_TYPE_ID;

		case eNxSettingsItem_LipSyncDelay:
			return	RES_LIP_SYNC_DELAY_01_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuAudio_GetMenuItemIdFormObjId(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	NX_COMMON_NOT_USED_PARAM(pstContents);
	lItemId = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SPDIF:
			*pulMenuItemId = NX_MENUITEM_ID_SET_SPDIF;
			break;

		case eNxSettingsItem_HDMI:
			*pulMenuItemId = NX_MENUITEM_ID_SET_HDMI;
			break;

		case eNxSettingsItem_AudioType:
			*pulMenuItemId = NX_MENUITEM_ID_SET_AUDIOTYPE;
			break;
		case eNxSettingsItem_LipSyncDelay:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuAudio_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Audio);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Audio_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuAudio_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Audio_MAX))
	{
		return 0;
	}

	return	(eNxSetting_Audio | lItemId);
}


static HBOOL	nx_menuAudio_GetItemSupport(NxSettingsItem_Audio_t eNxMenuLanguageItem)
{
	HBOOL	bSupport = FALSE;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);

	switch (eNxMenuLanguageItem)
	{
		case eNxSettingsItem_SPDIF:
			if (eOpType == eDxOPERATOR_AMX)
			{
				bSupport = FALSE;
			}
			else
			{
				bSupport = TRUE;
			}
			break;
		case eNxSettingsItem_HDMI:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_AudioType:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_LipSyncDelay:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuAudio_GetItemiDisable(NxSettingsItem_Audio_t eNxMenuLanguageItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxMenuLanguageItem)
	{
		case eNxSettingsItem_SPDIF:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_HDMI:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_AudioType:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_LipSyncDelay:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuAudio_UpdateCurrentTime(Nx_MenuAudioSetContent_t *pstContents)
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


static void		nx_menuAudio_FindNextFocus(Nx_MenuAudioSetContent_t *pstContents, HBOOL bUp)
{
	HUINT32			i = 0, ulSubItemCount = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	ulSubItemCount = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

	for (i = 0; i < ulSubItemCount; i++)
	{
		if ( bUp == TRUE )
		{
			pstSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, (ulSubItemCount - 1) - i);
		}
		else
		{
			pstSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, i);
		}

		if ((pstSubItem == NULL) || (pstSubItem->bDisable == TRUE))
		{
			continue;
		}

		pstContents->ulCurFocus = i;
		ONDK_GWM_Obj_SetFocus(pstSubItem->ulId);
		break;
	}
}


static ONDK_Result_t	nx_menuAudio_RefreshSubItem(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	HUINT8		**ppListOfItem = NULL;
	HUINT32		ulMenuId, ulListIdx, ulItems;
	HINT32		lSubItem = 0, lItemIdx = -1;
	HCHAR		*pcStrId = NULL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	 *pstSubItem;


	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if ( pstSubItem == NULL )
	{
		NX_APP_Error("Error!!! pstSubItem == NULL lObjId : (0x%x)\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pstSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pstSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuAudio_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pstSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuAudio_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() Fail ulMenuId : (0x%x) !! \n", ulMenuId);
		return	eResult;
	}

	lItemIdx = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_SPDIF:
			lSubItem = pstContents->eSPDIF;
			break;

		case eNxSettingsItem_HDMI:
			lSubItem = pstContents->eHDMI;
			break;

		case eNxSettingsItem_AudioType:
			lSubItem = pstContents->eAudioType;
			break;

		default:
			lSubItem = -1;
			break;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(ulMenuId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail!! ulMenuId : (0x%x)\n", ulMenuId);
		return	ONDK_RET_FAIL;
	}

	pstSubItem->ulNumOfItemTextList = ulItems;
	pstSubItem->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	pstSubItem->lSubItemValue = lSubItem;
	pstSubItem->ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	return	ONDK_RET_OK;
}


static void		nx_menuAudio_RefreshAllSubItems(Nx_MenuAudioSetContent_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_SPDIF; i < eNxSettingsItem_Audio_MAX; i++)
	{
		eResult = nx_menuAudio_RefreshSubItem(pstContents, nx_menuAudio_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}


static ONDK_Result_t	nx_menuAudio_UpdateAudioType(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eAudioType != (DxStereoSelect_e)pstSubItem->lSubItemValue)
	{
		pstContents->eAudioType = (DxStereoSelect_e)pstSubItem->lSubItemValue;
		NX_SYSTEM_SetSoundMode(pstContents->eAudioType);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_UpdateHDMI(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eHDMI != (DxDigitalAudioFormat_e)pstSubItem->lSubItemValue)
	{
		pstContents->eHDMI = (DxDigitalAudioFormat_e)pstSubItem->lSubItemValue;
		NX_SYSTEM_SetHdmiAudioOutputMode(pstContents->eHDMI);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_UpdateSPDIF(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eSPDIF != (DxDigitalAudioFormat_e)pstSubItem->lSubItemValue)
	{
		pstContents->eSPDIF = (DxDigitalAudioFormat_e)pstSubItem->lSubItemValue;
		NX_SYSTEM_SetSpdifAudioOutputMode(pstContents->eSPDIF);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_UpdateSubItem(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SPDIF:
			nx_menuAudio_UpdateSPDIF(pstContents, lObjId);
			break;

		case eNxSettingsItem_HDMI:
			nx_menuAudio_UpdateHDMI(pstContents, lObjId);
			break;

		case eNxSettingsItem_AudioType:
			nx_menuAudio_UpdateAudioType(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_ChangedSubItemValue(Nx_MenuAudioSetContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SPDIF:
		case eNxSettingsItem_HDMI:
		case eNxSettingsItem_AudioType:
			pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			pstSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
			nx_menuAudio_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));

			/* Update value */
			nx_menuAudio_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_LoadSettingsItemsValue(Nx_MenuAudioSetContent_t *pstContents)
{
	HERROR						hError = ERR_FAIL;
	HUINT32						ulLipSyncDelay = 0;
	DxDigitalAudioFormat_e		eSPDIF = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;
	DxDigitalAudioFormat_e		eHDMI = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;
	DxStereoSelect_e			eAudioType = eDxSTEREO_SELECT_STEREO;

	hError = NX_SYSTEM_GetSpdifAudioOutputMode(&eSPDIF);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! NX_SYSTEM_GetSpdifAudioOutputMode() Fail Set eDxDIGITALAUDIO_FORMAT_MULTICHANNEL!!\n");
		eSPDIF = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;
	}
	pstContents->eSPDIF = eSPDIF;

	hError = NX_SYSTEM_GetHdmiAudioOutputMode(&eHDMI);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! NX_SYSTEM_GetHdmiAudioOutputMode() Fail Set eDxDIGITALAUDIO_FORMAT_MULTICHANNEL!!\n");
		eHDMI = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;
	}
	pstContents->eHDMI = eHDMI;

	hError = NX_SYSTEM_GetSoundMode(&eAudioType);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! NX_SYSTEM_GetSoundMode() Fail Set eDxSTEREO_SELECT_STEREO!!\n");
		eAudioType = eDxSTEREO_SELECT_STEREO;
	}
	pstContents->eAudioType = eAudioType;

	hError = NX_SYSTEM_GetLipSyncDelay(&ulLipSyncDelay);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! NX_SYSTEM_GetLipSyncDelay() Fail Set 0!!\n");
		ulLipSyncDelay = 0;
	}
	pstContents->ulLipSyncDelay = ulLipSyncDelay;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_AddItemLipSyncDelay(Nx_MenuAudioSetContent_t *pstContents)
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
	bSupport = nx_menuAudio_GetItemSupport(eNxSettingsItem_LipSyncDelay);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_AudioType(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuAudio_ConvertItemIdxToObjId(eNxSettingsItem_LipSyncDelay);

	// Check Disable Item
	bDisable = nx_menuAudio_GetItemiDisable(eNxSettingsItem_LipSyncDelay);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuAudio_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	stSubItem.ulNumOfItemTextList = 0;
	stSubItem.szItemTextList = NULL;
	stSubItem.ulIdxOfTextList = 0;
	stSubItem.lSubItemValue = pstContents->ulLipSyncDelay;

	HxSTD_StrCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_MENU_LipSyncPopupBar_Proc");
	stSubItem.childAppToLaunch = NX_PRISM_MENU_LipSyncPopupBar_Proc;
	HxSTD_sprintf((char*)stSubItem.szSubItemText, (char*)"%d %s", stSubItem.lSubItemValue, ONDK_GetString(RES_MS_ID));

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_AddItemAudioType(Nx_MenuAudioSetContent_t *pstContents)
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
	bSupport = nx_menuAudio_GetItemSupport(eNxSettingsItem_AudioType);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_AudioType(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuAudio_ConvertItemIdxToObjId(eNxSettingsItem_AudioType);

	// Check Disable Item
	bDisable = nx_menuAudio_GetItemiDisable(eNxSettingsItem_AudioType);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuAudio_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuAudio_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail!! ulMenuId : (0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eAudioType), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eAudioType;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_AddItemHDMI(Nx_MenuAudioSetContent_t *pstContents)
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
	bSupport = nx_menuAudio_GetItemSupport(eNxSettingsItem_HDMI);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SPDIF(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuAudio_ConvertItemIdxToObjId(eNxSettingsItem_HDMI);

	// Check bDisable Item
	bDisable = nx_menuAudio_GetItemiDisable(eNxSettingsItem_HDMI);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuAudio_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuAudio_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail!! ulMenuId : (0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eHDMI), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eHDMI;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_AddItemSPDIF(Nx_MenuAudioSetContent_t *pstContents)
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
	bSupport = nx_menuAudio_GetItemSupport(eNxSettingsItem_SPDIF);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SPDIF(%d)\n", bSupport);
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuAudio_ConvertItemIdxToObjId(eNxSettingsItem_SPDIF);

	// Check bDisable Item
	bDisable = nx_menuAudio_GetItemiDisable(eNxSettingsItem_SPDIF);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuAudio_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuAudio_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail!! ulMenuId : (0x%x)\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eSPDIF), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->eSPDIF;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_InitSettingsItems(Nx_MenuAudioSetContent_t *pstContents)
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
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_AUDIO_03_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuAudio_AddItemSPDIF(pstContents);
	nx_menuAudio_AddItemHDMI(pstContents);
	nx_menuAudio_AddItemAudioType(pstContents);
	nx_menuAudio_AddItemLipSyncDelay(pstContents);

	// Default Function
	nx_menuAudio_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_DrawDefaultUI(Nx_MenuAudioSetContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	// Draw - Time
	nx_menuAudio_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_MsgGwmCreate(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuAudio_LoadSettingsItemsValue(pstContents);

	// Init Menu Item
	nx_menuAudio_InitSettingsItems(pstContents);
	pstContents->ulCurFocus = (HUINT32)p1;
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuAudio_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuAudio_MsgGwmDestroy(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuAudioSetContent_t));
	nx_menuAudio_DestroyListPopup();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_MsgGwmHapiMsg(Nx_MenuAudioSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuAudio_MsgGwmClicked(Nx_MenuAudioSetContent_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	stSubItem;
	HINT32						lItemIdx = -1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuAudio_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stSubItem, &(pstContents->subItems[lItemIdx]), sizeof(Nx_SettingsLeafSubItem_t));

	switch (lItemIdx)
	{
		case eNxSettingsItem_SPDIF:
		case eNxSettingsItem_HDMI:
		case eNxSettingsItem_AudioType:
			nx_menuAudio_CreateListPopup(pstContents, lObjId);
			break;
		case eNxSettingsItem_LipSyncDelay:
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stSubItem.szAppName, (APP_DEFAULT_PRIORITY | UISTATE_MENU), stSubItem.childAppToLaunch,
							stSubItem.hAction, stSubItem.lSubItemValue, stSubItem.ulappParam2, (HINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));

			break;
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_MsgGwmKeyDown(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuAudio_MsgGwmTimer(Nx_MenuAudioSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuAudio_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Audio_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuAudioSetContent_t	*pstContents = NULL;

	pstContents = nx_menuAudio_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuAudio_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuAudio_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			nx_menuAudio_MsgGwmClicked(pstContents, p1);
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			nx_menuAudio_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			nx_menuAudio_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuAudio_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuAudio_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuAudio_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			nx_menuAudio_MsgGwmTimer(pstContents, p1, p2, p3);
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
