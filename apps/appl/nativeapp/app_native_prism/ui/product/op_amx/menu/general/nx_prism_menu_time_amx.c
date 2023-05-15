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
 * @file	  		nx_prism_menu_tiem_amx.c
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

#define	NX_PRISM_MENU_TIMEINFO_ID							eNxSetting_Time

#define	NX_PRISM_MENU_TIMEINFO_INFOBOX_X					60
#define	NX_PRISM_MENU_TIMEINFO_INFOBOX_Y					114
#define	NX_PRISM_MENU_TIMEINFO_INFOBOX_W					750
#define	NX_PRISM_MENU_TIMEINFO_INFOBOX_H					418

#define	NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_X					(NX_PRISM_MENU_TIMEINFO_INFOBOX_X + 18)
#define	NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_Y					(NX_PRISM_MENU_TIMEINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_W					450
#define	NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_LS				40

#define	NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_X					(NX_PRISM_MENU_TIMEINFO_INFOBOX_X + 482)
#define	NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_Y					(NX_PRISM_MENU_TIMEINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_W					250
#define	NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_LS				40

#define	NX_PRISM_MENU_TIMEINFO_DONE_BTN_X					205
#define	NX_PRISM_MENU_TIMEINFO_DONE_BTN_Y					615
#define	NX_PRISM_MENU_TIMEINFO_DONE_BTN_W					315
#define	NX_PRISM_MENU_TIMEINFO_DONE_BTN_H					55

#define	NX_PRISM_MENU_TIMEINFO_ITEM_MAX_NUM					12


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eNxSettingsItemTimeInfo_InfoBox,
	eNxSettingsItemTimeInfo_Country_Label,
	eNxSettingsItemTimeInfo_GMTOffset_Label,
	eNxSettingsItemTimeInfo_DST_Label,
	eNxSettingsItemTimeInfo_Country_Value,
	eNxSettingsItemTimeInfo_GMTOffset_Value,
	eNxSettingsItemTimeInfo_DST_Value,

	eNxSettingsItemTimeInfo_DoneButton,

	eNxSettingsItemTimeInfo_MAX
} NxUi_MenuTimeInfoCompIds_e;


typedef struct
{
	HUINT32		ulItemNum;
	HUINT32		aulItemList[NX_PRISM_MENU_TIMEINFO_ITEM_MAX_NUM];
	HUINT32		aulItemValueList[NX_PRISM_MENU_TIMEINFO_ITEM_MAX_NUM];
} NxUi_MenuTimeInfoItemList_t;


typedef struct
{
	/**@breif Specific value */
	HUINT8	    ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32      lPrevCurIdx;

	HUINT8		**szItemTextList;
	NxUi_MenuTimeInfoItemList_t	stItemList;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

} NxUi_MenuTimeInfoContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUi_MenuTimeInfoContent_t		s_stSysInfoContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline NxUi_MenuTimeInfoContent_t* nx_menuTimeInfo_GetContents(void);

static void				nx_menuTimeInfo_UpdateCurrentTime(NxUi_MenuTimeInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuTimeInfo_DrawDoneButton(void);
static ONDK_Result_t	nx_menuTimeInfo_DrawSystemInfo(NxUi_MenuTimeInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuTimeInfo_DrawInfoBox(NxUi_MenuTimeInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuTimeInfo_DrawDefaultUI(NxUi_MenuTimeInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuTimeInfo_AddItemList(NxUi_MenuTimeInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId);
static ONDK_Result_t	nx_menuTimeInfo_LoadItemsValue(NxUi_MenuTimeInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuTimeInfo_InitItems(NxUi_MenuTimeInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuTimeInfo_MsgGwmCreate(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuTimeInfo_MsgGwmDestroy(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTimeInfo_MsgGwmKeyDown(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTimeInfo_MsgGwmClicked(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTimeInfo_MsgGwmTimer(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline NxUi_MenuTimeInfoContent_t* nx_menuTimeInfo_GetContents(void)
{
	return	&s_stSysInfoContent;
}


static void		nx_menuTimeInfo_UpdateCurrentTime(NxUi_MenuTimeInfoContent_t *pstContents)
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


static ONDK_Result_t    nx_menuTimeInfo_DrawDoneButton(void)
{
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_PRISM_MENU_TIMEINFO_DONE_BTN_X, NX_PRISM_MENU_TIMEINFO_DONE_BTN_Y,
	                    NX_PRISM_MENU_TIMEINFO_DONE_BTN_W, NX_PRISM_MENU_TIMEINFO_DONE_BTN_H);
	ONDK_GWM_COM_Button_Create((NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_DoneButton), &stRect, (HCHAR *)ONDK_GetString(RES_DONE_01_ID));
	ONDK_GWM_APP_SetObjectDrawMethod((NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_DoneButton), NX_PRISM_COMP_DrawButton_B1);

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_DrawSystemInfo(NxUi_MenuTimeInfoContent_t *pstContents)
{
	HINT8			*pstItemStrID = NULL;
	HUINT32			i = 0;
	ONDK_Rect_t     stItemRect = { 0, 0, 0, 0 }, stValueRect = { 0, 0, 0, 0 };

	stItemRect = ONDK_Rect(NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_X, NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_Y, NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_W, NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_LS);
	stValueRect = ONDK_Rect(NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_X, NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_Y, NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_W, NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_LS);

	for (i = 0; i < pstContents->stItemList.ulItemNum; i++)
	{
		switch (pstContents->stItemList.aulItemList[i])
		{
			case eNxSettingsItemTimeInfo_Country_Label:			pstItemStrID = RES_COUNTRY_ID;				break;
			case eNxSettingsItemTimeInfo_GMTOffset_Label:		pstItemStrID = RES_GMT_OFFSET_ID;			break;
			case eNxSettingsItemTimeInfo_DST_Label:				pstItemStrID = RES_DAYLIGHT_SAVING_TIME_ID;	break;
			default:											pstItemStrID = RES_UNKNOWN_ID;				break;
		}

		ONDK_GWM_COM_Text_Create((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemList[i]), &stItemRect, (HCHAR *)ONDK_GetString(pstItemStrID));
		ONDK_GWM_COM_Text_SetFont((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemList[i]), eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemList[i]), TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemList[i]), COL(C_T_Gong03), COL(C_T_Bang_100));

		ONDK_GWM_COM_Text_Create((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemValueList[i]), &stValueRect, (HCHAR *)pstContents->szItemTextList[i]);
		ONDK_GWM_COM_Text_SetFont((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemValueList[i]), eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemValueList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemValueList[i]), TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_ChangeColor((NX_PRISM_MENU_TIMEINFO_ID | pstContents->stItemList.aulItemValueList[i]), COL(C_T_Whistle_100), COL(C_T_Bang_100));

		stItemRect.y += NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_LS;
		stValueRect.y += NX_PRISM_MENU_TIMEINFO_ITEM_VALUE_LS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuTimeInfo_DrawInfoBox(NxUi_MenuTimeInfoContent_t *pstContents)
{
	HINT32			lh = 0;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	if (pstContents->stItemList.ulItemNum > 0)
	{
		lh = (pstContents->stItemList.ulItemNum * NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_LS) + NX_PRISM_MENU_TIMEINFO_ITEM_LABEL_LS;
	}
	else
	{
		lh = NX_PRISM_MENU_TIMEINFO_INFOBOX_H;
	}

	stRect = ONDK_Rect(NX_PRISM_MENU_TIMEINFO_INFOBOX_X, NX_PRISM_MENU_TIMEINFO_INFOBOX_Y, NX_PRISM_MENU_TIMEINFO_INFOBOX_W, lh);
	ONDK_GWM_COM_Rect_Create((NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_InfoBox), &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod((NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_InfoBox), NX_PRISM_COMP_DrawFrameBox_A);

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_DrawDefaultUI(NxUi_MenuTimeInfoContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuTimeInfo_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	nx_menuTimeInfo_DrawInfoBox(pstContents);
	nx_menuTimeInfo_DrawSystemInfo(pstContents);
	nx_menuTimeInfo_DrawDoneButton();


	ONDK_GWM_Obj_SetFocus(NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_DoneButton);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_LoadItemsValue(NxUi_MenuTimeInfoContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT8			*pszStrId = NULL;
	HUINT8			**ppListOfItem = NULL;
	HUINT8			szTextBuffer[NX_TEXT_SIZE_128];
	HINT32			lGmtOffset = 0;
	HUINT32			i = 0;
	HxCountry_e			eCountryCode = eCountryID_GTM;
	eNxUiUtilGMTOffset_t eGMTOffset = eNxUtil_GMTOffset_m06_00;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	hError = NX_SYSTEM_GetCountryCode(&eCountryCode);
	if (hError != ERR_OK)
	{
		eCountryCode = eCountryID_GTM;
		NX_APP_Error("Error!!! NX_SYSTEM_GetCountryCode() Fail!!\n");
	}

	hError = NX_SYSTEM_GetGMTOffset(&lGmtOffset);
	if (hError != ERR_OK)
	{
		lGmtOffset = -360;	// Guatemala - eNxUtil_GMTOffset_m06_00
		NX_APP_Error("Error!!! NX_SYSTEM_GetGMTOffset() Fail!!\n");
	}

	hError = NX_PRISM_UTIL_GetGMTOffsetByTime(lGmtOffset, &eGMTOffset);
	if (hError != ERR_OK)
	{
		eGMTOffset = eNxUtil_GMTOffset_m06_00;
		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetGMTOffsetByTime() Fail!!\n");
	}

	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (pstContents->stItemList.ulItemNum, NX_TEXT_SIZE_128);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_MENU_MenuItem_AllocTextList() Fail!!\n");
		return	ONDK_RET_FAIL;
	}
	else
	{
		for (i = 0; i < pstContents->stItemList.ulItemNum; i++)
		{
			HxSTD_memset(ppListOfItem[i], 0x00, NX_TEXT_SIZE_128);
			ppListOfItem[i][0] = '\0';

			switch (pstContents->stItemList.aulItemList[i])
			{
				case eNxSettingsItemTimeInfo_Country_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					pszStrId = NX_PRISM_UTIL_GetResStringIdByCountryID(eCountryCode);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)ONDK_GetString(pszStrId));
					break;
				case eNxSettingsItemTimeInfo_GMTOffset_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					pszStrId = NX_PRISM_UTIL_GetResStringIdByGMTOffset(eGMTOffset);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)ONDK_GetString(pszStrId));
					break;
				case eNxSettingsItemTimeInfo_DST_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)ONDK_GetString(RES_AUTO_ID));
					break;
				default:
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
					break;
			}
		}

		pstContents->szItemTextList = ppListOfItem;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_AddItemList(NxUi_MenuTimeInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId)
{
	HUINT32		ulIndex = 0;

	if (pstSysItemList->ulItemNum >= NX_PRISM_MENU_TIMEINFO_ITEM_MAX_NUM)
	{
		NX_APP_Error("Error!!! OUT OF RANGE!! pstSysItemList->ulItemNum(%d), NX_UTIL_OPTION_MAX_NUM(%d)\n", pstSysItemList->ulItemNum, NX_PRISM_MENU_TIMEINFO_ITEM_MAX_NUM);
		return	ONDK_RET_FAIL;
	}

	ulIndex = pstSysItemList->ulItemNum;
	pstSysItemList->aulItemList[ulIndex] = ulItemId;
	pstSysItemList->aulItemValueList[ulIndex] = ulItemValueId;
	pstSysItemList->ulItemNum = (ulIndex + 1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_InitItems(NxUi_MenuTimeInfoContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_COUNTRY_TIME_ZONE_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuTimeInfo_AddItemList(&pstContents->stItemList, eNxSettingsItemTimeInfo_Country_Label, eNxSettingsItemTimeInfo_Country_Value);
	nx_menuTimeInfo_AddItemList(&pstContents->stItemList, eNxSettingsItemTimeInfo_GMTOffset_Label, eNxSettingsItemTimeInfo_GMTOffset_Value);
	nx_menuTimeInfo_AddItemList(&pstContents->stItemList, eNxSettingsItemTimeInfo_DST_Label, eNxSettingsItemTimeInfo_DST_Value);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_MsgGwmCreate(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT8		*pszPanelStr = NULL;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	eResult = nx_menuTimeInfo_InitItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuTimeInfo_InitItems() Fail\n");
		return	eResult;
	}
	pstContents->lPrevCurIdx = p3;

	// Load Menu Item Value
	eResult = nx_menuTimeInfo_LoadItemsValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuTimeInfo_LoadItemsValue() Fail\n");
		return	eResult;
	}

	// Draw Menu Item
	nx_menuTimeInfo_DrawDefaultUI(pstContents);

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

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_MsgGwmDestroy(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->stItemList.ulItemNum, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NxUi_MenuTimeInfoContent_t));
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_MsgGwmKeyDown(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;


	if (pstContents == NULL)
	{
		NX_APP_Error("(pstContents == NULL)\n");
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
	        lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_General_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_General_Proc, 0, 0, 0, lPrevCurIdx);
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
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuTimeInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_MsgGwmClicked(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lPrevCurIdx = 0;

	switch ((HUINT32)p1)
	{
		case (NX_PRISM_MENU_TIMEINFO_ID | eNxSettingsItemTimeInfo_DoneButton):
	        lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_General_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_General_Proc, 0, 0, 0, lPrevCurIdx);
			break;

		default:
			break;
	}
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTimeInfo_MsgGwmTimer(NxUi_MenuTimeInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuTimeInfo_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
	else
	{
		NX_APP_Error("Error!!! Invalid Timer ID p1 :(0x%x)\n", p1);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_TimeInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	NxUi_MenuTimeInfoContent_t	*pstContents = NULL;

	pstContents = nx_menuTimeInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuTimeInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuTimeInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuTimeInfo_MsgGwmClicked(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuTimeInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuTimeInfo_MsgGwmTimer(pstContents, p1, p2, p3);
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

