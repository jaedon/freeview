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
 * @file	  		nx_prism_menu_storage.c
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
#define	NX_MENU_STORAGE_MAX_NUMBER							10
#define NX_MENU_STORAGE_LIST_ID								1

#define	NX_MENU_STORAGE_SUBITEM_WIDTH						750
#define	NX_MENU_STORAGE_SUBITEM_HEIGHT						55

#define	NX_MENU_STORAGE_SUBITEM_MAINTXT_X_TAB				54
#define	NX_MENU_STORAGE_SUBITEM_MAINTXT_WIDTH				675
#define	NX_MENU_STORAGE_SUBITEM_MAINICON_X_TAB				8
#define	NX_MENU_STORAGE_SUBITEM_MAINICON_Y_TAB				9
#define	NX_MENU_STORAGE_SUBITEM_MAINICON_WIDTH				36
#define	NX_MENU_STORAGE_SUBITEM_MAINICON_HEIGHT				36

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tag_NX_MENUSTORAGE_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulStorageListNum;

	HUINT32					ulListId;

	HUINT32			    	*pStorageDevIdList;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32					lPrevCurIdx;

	HUINT8					szDate[NX_TEXT_SIZE_32];
	HUINT8					szTime[NX_TEXT_SIZE_32];
} Nx_MenuStorageSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuStorageSetContent_t			s_stStorageContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_MenuStorageSetContent_t* nx_menuStorage_GetContents(void);
static HCHAR* 			nx_menuStorage_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData);
static HCHAR* 			nx_menuStorage_GetStorageIconIdFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData, HBOOL bFocus);

static HINT32			nx_menuStorage_ConvertItemIdxToObjId(HINT32 lItemId);
static void				nx_menuStorage_UpdateCurrentTime(Nx_MenuStorageSetContent_t *pstContents);
static ONDK_Result_t	nx_menuStorage_DrawList_Cursor_C3(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_menuStorage_FreeDeviceIdList (HUINT32 *pDevIdList);
static ONDK_Result_t	nx_menuStorage_LoadStorageDevIdList(Nx_MenuStorageSetContent_t *pstContents);
static ONDK_Result_t	nx_menuStorage_DrawDefaultUI(Nx_MenuStorageSetContent_t *pstContents);

static ONDK_Result_t	nx_menuStorage_MsgGwmCreate(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuStorage_MsgGwmDestroy(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuStorage_MsgGwmHapiMsg(Nx_MenuStorageSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuStorage_MsgGwmKeyDown(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuStorage_MsgGwmTimer(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuStorage_MsgGwmListSelected(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuStorage_MsgCoreDeviceChange(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuStorageSetContent_t* nx_menuStorage_GetContents(void)
{
	return	&s_stStorageContent;
}

static HCHAR* 	nx_menuStorage_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData)
{
	if (physicalStorageData == NULL)
	{
		NX_APP_Error("ERROR!!! physicalStorageData is NULL !!\n");
		return NULL;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_INTERNAL_HDD_ID);

		case eStorageSort_externalSubPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_DETACHABLE_HDD_ID);

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_UNKNOWN_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		case eStorageSort_SdMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_SD_CARD_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		default:
			return NULL;
	}
}

static HCHAR* 	nx_menuStorage_GetStorageIconIdFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData, HBOOL bFocus)
{
	if (physicalStorageData == NULL)
	{
		NX_APP_Error("ERROR!!! physicalStorageData is NULL !!\n");
		return NULL;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_IN_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_IN_N_PNG;
			}

		case eStorageSort_externalSubPVRHDD:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_ESATA_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_ESATA_N_PNG;
			}

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_USB_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_USB_N_PNG;
			}

		case eStorageSort_SdMemory:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_SD_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_SD_N_PNG;
			}

		default:
			return NULL;
	}
}

static HINT32	nx_menuStorage_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if (lItemId < 0)
	{
		return 0;
	}
	return	(eNxSetting_Storage | lItemId);
}

static void		nx_menuStorage_UpdateCurrentTime(Nx_MenuStorageSetContent_t *pstContents)
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

ONDK_Result_t	nx_menuStorage_DrawList_Cursor_C3(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HUINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HUINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HUINT32		i, lCellHeight = 0, lNumOfPage = NX_MENU_STORAGE_MAX_NUMBER;
	HUINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0;
	HUINT32		lText_x_offset = NX_MENU_STORAGE_SUBITEM_MAINTXT_X_TAB, lText_y_offset = NX_PRISM_UI_MENU_SUBITEM_MAINTXT_Y_TAB;

	HUINT32		ulStatus = 0, ulFontSize = 0, lItem = 0;
	HUINT8		*title = NULL;
	HCHAR		*imgId = NULL;
	HBOOL		bFocus = FALSE;
	HERROR		hError = ERR_FAIL;

	ONDK_Rect_t 	stImgRect;
	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;

	//BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;
	LIST_OBJECT 		obj = (LIST_OBJECT)object;
	Nx_MenuStorageSetContent_t	*pstContents = nx_menuStorage_GetContents();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w - NX_PRISM_UI_MENU_SUBITEM_ICON_MG;
	h = rect->h;

	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if (lTotal == 0)
	{
		NX_APP_Error("ERROR!!! (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	lCursorMg = NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG;

	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	/* active, front most */
	for (i = 0; i < lTotal; i++)
	{
		lItem = (HUINT32)(lStart + i);

		if (lItem >= pstContents->ulStorageListNum)
		{
			NX_APP_Error("ERROR!!! up to max schedule count !!\n");
			break;
		}

		hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[i], &physicalStorageData);
		if (hError != ERR_OK || physicalStorageData == NULL)
		{
			NX_APP_Error("ERROR!!! Get Storage Info Fail !!\n");
			break;
		}

		title = nx_menuStorage_GetStorageLabelFromObjInfo(physicalStorageData);
		if (title == NULL)
		{
			NX_APP_Error("Assert!!! Please Check!! \n");
			break;
		}

		x = x1;
		y = y1 + lCursor_y_off;
		if (lItem == lCurrent)
		{
			/* get focused */
			bFocus = TRUE;
			title_fontColor = COL (C_T_Bang_100);
			value_fontColor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);
			eFontId = eFont_SystemBold;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			bFocus = FALSE;
			title_fontColor = COL (C_T_Whistle_100);
			value_fontColor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Whistle_15);
			eFontId = eFont_SystemNormal;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);
		}

		hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
		if (hFontHandle)
		{
			// SubItem - Title
			x = x1 + lText_x_offset;
			y = y1 + lCursor_y_off + lText_y_offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, NX_MENU_STORAGE_SUBITEM_MAINTXT_WIDTH, title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hFontHandle);
		}

		imgId = nx_menuStorage_GetStorageIconIdFromObjInfo(physicalStorageData, bFocus);
		if (imgId != NULL)
		{
			stImgRect = ONDK_Rect(x1 + NX_MENU_STORAGE_SUBITEM_MAINICON_X_TAB, y1 + lCursor_y_off + NX_MENU_STORAGE_SUBITEM_MAINICON_Y_TAB,
										NX_MENU_STORAGE_SUBITEM_MAINICON_WIDTH, NX_MENU_STORAGE_SUBITEM_MAINICON_HEIGHT);
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, imgId);
		}

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_FreeDeviceIdList (HUINT32 *pDevIdList)
{
	if(pDevIdList != NULL)
	{
		NX_APP_Free (pDevIdList);
		pDevIdList = NULL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_LoadStorageDevIdList(Nx_MenuStorageSetContent_t *pstContents)
{
	HERROR 		hError = ERR_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("pstContents is NULL!\n");
		return ONDK_RET_FAIL;
	}

	//Get Storage DevId List
	nx_menuStorage_FreeDeviceIdList(pstContents->pStorageDevIdList);
	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageDevIdList(&pstContents->pStorageDevIdList, &pstContents->ulStorageListNum);
	if (hError != ERR_OK || pstContents->pStorageDevIdList == NULL)
	{
		NX_APP_Error("physicalStorageDevIdList is NULL!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->ulStorageListNum <= 0)
	{
		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Pvr_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Pvr_Proc,
							 0, 0, 0, pstContents->lPrevCurIdx);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_DrawDefaultUI(Nx_MenuStorageSetContent_t *pstContents)
{
	ONDK_Rect_t		stRect;
	HINT32			lListId;
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);
	nx_menuStorage_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - Storage List
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_BTN01_X, NX_PRISM_UI_MENU_SUBITEM_BTN01_Y,
							NX_MENU_STORAGE_SUBITEM_WIDTH, NX_MENU_STORAGE_SUBITEM_HEIGHT * NX_MENU_STORAGE_MAX_NUMBER);
	lListId = nx_menuStorage_ConvertItemIdxToObjId(NX_MENU_STORAGE_LIST_ID);
	ONDK_GWM_List_Create(lListId, &stRect, "NxStorageList", (NX_MENU_STORAGE_SUBITEM_HEIGHT + NX_PRISM_UI_MENU_SUBITEM_CURSOR_MG),
							pstContents->ulStorageListNum, NX_MENU_STORAGE_MAX_NUMBER, 0);
	ONDK_GWM_List_SetFontHandle(lListId, eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_List_SetCurrentIndex(lListId, pstContents->ulCurFocus);
	ONDK_GWM_List_SetSelectedIndex(lListId, pstContents->ulCurFocus);
	ONDK_GWM_APP_SetObjectDrawMethod(lListId, (GWM_ObjDraw_t)nx_menuStorage_DrawList_Cursor_C3);

	if (pstContents->ulStorageListNum > 0)
	{
		ONDK_GWM_Obj_SetFocus(lListId);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_MsgGwmCreate(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Storage Value
	nx_menuStorage_LoadStorageDevIdList(pstContents);
	pstContents->lPrevCurIdx = p3;
	pstContents->ulCurFocus = 0;

	// Set Title
	HxSTD_sprintf((char*)pstContents->ucTitle, "%s", (char*)ONDK_GetString(RES_STORAGE_01_ID));

	// Draw Menu Item
	nx_menuStorage_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuStorage_MsgGwmDestroy(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	nx_menuStorage_FreeDeviceIdList(pstContents->pStorageDevIdList);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuStorageSetContent_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuStorage_MsgGwmHapiMsg(Nx_MenuStorageSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuStorage_MsgGwmKeyDown(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;

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
		case KEY_ARROWLEFT:
			lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Pvr_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Pvr_Proc,
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
			NX_APP_Print("nx_menuStorage_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_MsgGwmTimer(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuStorage_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_MsgGwmListSelected(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lListIdx = p2;
	HUINT32			ulProperty = 0;

	ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
#if defined(CONFIG_MW_MM_PVR)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Storage_DeviceSetting_Proc",
									ulProperty, NX_PRISM_MENU_Storage_DeviceSetting_Proc,
									HANDLE_NULL, pstContents->pStorageDevIdList[lListIdx], 0, 0);
#endif

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuStorage_MsgCoreDeviceChange(Nx_MenuStorageSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32						lListId;
	HUINT32						ulListIdx;

	if (p2 == eNxStorageState_Detach)
	{
		for (ulListIdx = 0; ulListIdx < pstContents->ulStorageListNum; ulListIdx++)
		{
			if (pstContents->pStorageDevIdList[ulListIdx] == (HUINT32)p1)
			{
				nx_menuStorage_LoadStorageDevIdList(pstContents);
				lListId = nx_menuStorage_ConvertItemIdxToObjId(NX_MENU_STORAGE_LIST_ID);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(lListId), pstContents->ulStorageListNum, 0);
				ONDK_GWM_APP_InvalidateObject(lListId);
				break;
			}
		}
	}
	else if (p2 == eNxStorageState_Attach)
	{
		nx_menuStorage_LoadStorageDevIdList(pstContents);
		lListId = nx_menuStorage_ConvertItemIdxToObjId(NX_MENU_STORAGE_LIST_ID);
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(lListId), pstContents->ulStorageListNum, 0);
		ONDK_GWM_APP_InvalidateObject(lListId);
	}

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Storage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuStorageSetContent_t	*pstContents = NULL;

	pstContents = nx_menuStorage_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuStorage_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuStorage_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			break;

		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			eResult = nx_menuStorage_MsgGwmListSelected(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuStorage_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuStorage_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuStorage_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_STORAGE_DEVICE_CHANGE:
			NX_APP_Message("MSG_CORE_STORAGE_DEVICE_CHANGE!\n");
			eResult = nx_menuStorage_MsgCoreDeviceChange(pstContents, p1, p2, p3);
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
