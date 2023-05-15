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
 * @file	  		nx_prism_menu_audiolipsync.c
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
#define	NX_PRISM_LIPSYNCBAR_VALUE_GRADE				20

typedef enum {
	eLipSyncBarItem_Lip_Sync,
	eLipSyncBarItem_MAX,
} Nx_LipSyncBarItem_t;

typedef enum {
	eLipSyncBarNotFocused_Frame = eLipSyncBarItem_MAX,
	eLipSyncBarNotFocused_MAX,
} Nx_LipSyncBarNotFocusedItem_t;

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tag_NX_AUDIO_SETTING_CONTENT
{
	/**@breif Public value */
	HUINT32						ulCurFocus;
	HUINT32						ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8						ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	ONDK_GWM_Callback_t			parentApp;
	HINT32						nLiSpyncInterval;
	HINT32						nInitValue;
	HUINT8						lipSyncIntervalText[NX_TEXT_SIZE_16];
} Nx_LipSyncSettingContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_LipSyncSettingContent_t		s_lipSyncSettingContent;				/**< Other Setting Contents. */

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_LipSyncSettingContent_t* 	nx_menuLipSyncBar_GetContents(void);
static HINT32								nx_menuLipSyncPopupBar_ConvertItemIdxToObjId(HINT32 nItemIdx);
static HINT32								nx_menuLipSyncPopupBar_ConvertNotFocusedItemIdxToObjId(HINT32 nItemIdx);

static ONDK_Result_t						nx_menuLipSyncPopupBar_DrawFrame (ONDKSurfaceHandle screen, ONDK_Region_t *area,ONDK_Rect_t *rect,
												ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t						nx_menuLipSyncPopupBar_DrawIntervalBar (ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static void 								nx_menuLipSyncPopupBar_DrawDefaultUI(Nx_LipSyncSettingContent_t *pContents);
static ONDK_Result_t 						nx_menuLipSyncPopupBar_SaveLibsyncValue(Nx_LipSyncSettingContent_t *pContents, HINT32 nLiSpync);

static ONDK_Result_t 						nx_menuLipSyncPopupBar_InitItems(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p3);
static ONDK_Result_t						nx_menuLipSyncPopupBar_MsgGwmCreate(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t						nx_menuLipSyncPopupBar_MsgGwmDestroy(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_LipSyncSettingContent_t* 	nx_menuLipSyncBar_GetContents(void)
{
	return	&s_lipSyncSettingContent;
}
static HINT32	nx_menuLipSyncPopupBar_ConvertItemIdxToObjId(HINT32 nItemIdx)
{
	if((nItemIdx < 0) || (nItemIdx >= eLipSyncBarItem_MAX))
	{
		return 0;
	}

	return (eNxSetting_LipSync | nItemIdx);
}
static HINT32	nx_menuLipSyncPopupBar_ConvertNotFocusedItemIdxToObjId(HINT32 nItemIdx)
{
	if((nItemIdx < eLipSyncBarNotFocused_Frame) || (nItemIdx >= eLipSyncBarNotFocused_MAX))
	{
		return 0;
	}

	return (eNxSetting_LipSync | nItemIdx);
}

static ONDK_Result_t	nx_menuLipSyncPopupBar_DrawFrame (ONDKSurfaceHandle screen, ONDK_Region_t *area,ONDK_Rect_t *rect,
												ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32				x = 0, y = 0, w = 0, h = 0;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };
	HUINT8				*szStr = ONDK_GetString(RES_LIP_SYNC_ID);

	/* absolute coordinate */
	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	/* draw box */
	stRect = ONDK_Rect(x, y, w, h);
	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG);

	/* Lip_sync Text */
	stRect = ONDK_Rect(x + NX_PRISM_UI_MENU_LIPSYNC_TXT_X, y + NX_PRISM_UI_MENU_LIPSYNC_TXT_Y, NX_PRISM_UI_MENU_LIPSYNC_TXT_WIDTH, NX_PRISM_UI_MENU_LIPSYNC_TXT_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID, &stRect, (HCHAR*)HLIB_STD_StrUpper((HCHAR*)szStr));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MENU_LIPSYNC_DELAY_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuLipSyncPopupBar_DrawIntervalBar (ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32				x = 0, y = 0, w = 0, h = 0;
	HINT32				nInterval = 0, levelWidth = 0, barWidth = 0;
	HUINT8				*szStr = NULL;
	ONDK_GWM_GuiObject_t	stTextObject;

	/* absolute coordinate */
	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	nInterval = ONDK_GWM_Selector_GetCurrentIndex(ONDK_GWM_Obj_GetObjectID(object));
	barWidth = (NX_PRISM_UI_MENU_LIPSYNC_BAR_WIDTH / NX_PRISM_LIPSYNCBAR_VALUE_GRADE) * nInterval;

	if(barWidth <= 0){
		x = x - 1;
		levelWidth = 1;
	}else{
		levelWidth = barWidth;
	}

	/* level bar */
	ONDK_DRAW_FillRectElement(screen, x, NX_PRISM_UI_MENU_LIPSYNC_BAR_Y, levelWidth, NX_PRISM_UI_MENU_LIPSYNC_BAR_HEIGHT, COL(C_G_Base01_100));

	if(barWidth <= 0){
		x = x + 1;
		levelWidth = 0;
	}else if(NX_PRISM_UI_MENU_LIPSYNC_BAR_WIDTH - barWidth <= 0){
		levelWidth = NX_PRISM_UI_MENU_LIPSYNC_BAR_WIDTH - 1;
	}

	/* background bar */
	ONDK_DRAW_FillRectElement(screen, (x + levelWidth), NX_PRISM_UI_MENU_LIPSYNC_BAR_Y, (NX_PRISM_UI_MENU_LIPSYNC_BAR_WIDTH - levelWidth), NX_PRISM_UI_MENU_LIPSYNC_BAR_HEIGHT, COL(C_G_Whistle_20));

	stTextObject = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID);
	if (stTextObject)
	{
		szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stTextObject);
		if (szStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_16, "+ %d ms", (int)(nInterval * NX_PRISM_LIPSYNCBAR_VALUE_GRADE));
			ONDK_GWM_COM_Text_SetText (NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, (HCHAR*)szStr);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ONDK_RET_OK;
}

static void 	nx_menuLipSyncPopupBar_DrawDefaultUI(Nx_LipSyncSettingContent_t *pContents)
{
	HINT32				nObjId = 0;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };

	/* Title */
	HxSTD_StrNCpy((HCHAR*)pContents->ucTitle, (HCHAR*)ONDK_GetString(RES_LIP_SYNC_ID), MAX_ELEMENT_TEXT_LENGTH);

	/* Frame */
	nObjId = nx_menuLipSyncPopupBar_ConvertNotFocusedItemIdxToObjId(eLipSyncBarNotFocused_Frame);
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_LIPSYNC_DELAY_X, NX_PRISM_UI_MENU_LIPSYNC_DELAY_Y, NX_PRISM_UI_MENU_LIPSYNC_DELAY_WIDTH, NX_PRISM_UI_MENU_LIPSYNC_DELAY_HEIGHT);
	ONDK_GWM_COM_Frame_Create(nObjId, &stRect, (HCHAR*)pContents->ucTitle);
	ONDK_GWM_APP_SetObjectDrawMethod(nObjId, nx_menuLipSyncPopupBar_DrawFrame);

	/* Lip-sync Bar */
	nObjId = nx_menuLipSyncPopupBar_ConvertItemIdxToObjId(eLipSyncBarItem_Lip_Sync);
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_X, NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_Y, NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_WIDTH, NX_PRISM_UI_MENU_LIPSYNC_BAR_WINDOW_HEIGHT);
	ONDK_GWM_Selector_Create(nObjId, &stRect, NULL, (NX_PRISM_LIPSYNCBAR_VALUE_GRADE + 1), NULL);
	ONDK_GWM_Selector_SetCurrentIndex(nObjId, pContents->nLiSpyncInterval);
	ONDK_GWM_APP_SetObjectDrawMethod(nObjId, (GWM_ObjDraw_t)nx_menuLipSyncPopupBar_DrawIntervalBar);

	/* Lip-sync Interval Text */
	HxSTD_snprintf((HCHAR *)pContents->lipSyncIntervalText, NX_TEXT_SIZE_16, "+ %d ms", pContents->nLiSpyncInterval * NX_PRISM_LIPSYNCBAR_VALUE_GRADE);
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_X, NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_Y, NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_WIDTH, NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, &stRect, (HCHAR *)pContents->lipSyncIntervalText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MENU_LIPSYNC_INTERVALTXT_ID, COL(C_T_Base01), COL(C_T_Base01));

	ONDK_GWM_Obj_SetFocus(nObjId);
}

static ONDK_Result_t 	nx_menuLipSyncPopupBar_SaveLibsyncValue(Nx_LipSyncSettingContent_t *pContents, HINT32 nLiSpync)
{
	HERROR					hError = ERR_FAIL;
	HUINT32					uLibsyncVal = (nLiSpync * NX_PRISM_LIPSYNCBAR_VALUE_GRADE);

	hError = NX_SYSTEM_SetLipSyncDelay(uLibsyncVal);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! nx_menuLipSyncPopupBar_SaveLibsyncValue() Fail !!\n");
		return ONDK_RET_FAIL;
	}

	pContents->nLiSpyncInterval = nLiSpync;

	return ONDK_RET_OK;
}

static ONDK_Result_t 	nx_menuLipSyncPopupBar_InitItems(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p3)
{
	/* param check */
	if((pContents == NULL) || (p3 == 0))
	{
		return ONDK_RET_FAIL;
	}

	pContents->nLiSpyncInterval = (p1 / NX_PRISM_LIPSYNCBAR_VALUE_GRADE);
	pContents->nInitValue = pContents->nLiSpyncInterval;
	pContents->parentApp = (ONDK_GWM_Callback_t)p3;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuLipSyncPopupBar_MsgGwmCreate(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	hError = ONDK_RET_FAIL;
	HUINT8		*pszPanelStr = NULL;

	/* Set Area! */
	ONDK_GWM_APP_SetAppArea(0, 0, NX_PRISM_OSD_HORIZONTAL, NX_PRISM_OSD_VERTICAL);

	/* Initialiaze Items */
	/* p1: current lipsync interval */
	/* p3: parent application */
	hError = nx_menuLipSyncPopupBar_InitItems(pContents, p1, p3);
	if(hError == ONDK_RET_FAIL){
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	// ONDK_GWM_APP_SetUIHide_AllexceptMe(ONDK_UPPER_WINDOW);

	// Draw Item
	nx_menuLipSyncPopupBar_DrawDefaultUI(pContents);

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

static ONDK_Result_t	nx_menuLipSyncPopupBar_MsgGwmDestroy(Nx_LipSyncSettingContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pContents);
	HxSTD_memset(pContents, 0x00, sizeof(Nx_LipSyncSettingContent_t));
	// ONDK_GWM_APP_SetUIShow_All();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}
/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_MENU_LipSyncPopupBar_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t				eResult = ONDK_RET_OK;
	Nx_LipSyncSettingContent_t	*pContents = NULL;

	pContents = nx_menuLipSyncBar_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuLipSyncPopupBar_MsgGwmCreate(pContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuLipSyncPopupBar_MsgGwmDestroy(pContents, p1, p2, p3);
			break;

		case MSG_GWM_SELECTOR_CHANGED:
			NX_APP_Message("MSG_GWM_SELECTOR_CHANGED!\n");
			eResult = nx_menuLipSyncPopupBar_SaveLibsyncValue(pContents, p2);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			switch (p1)
			{
				case KEY_ARROWRIGHT:
				case KEY_ARROWLEFT:
					break;

				case KEY_TVRADIO:
				case KEY_VFORMAT:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_TEXT:
					NX_PRISM_NotAvailable_Show();
					return	ONDK_MESSAGE_BREAK;

				/* keys to exit */
				case KEY_BACK:
				case KEY_OK:
					if (p1 == KEY_BACK)
					{
						eResult = nx_menuLipSyncPopupBar_SaveLibsyncValue(pContents, pContents->nInitValue);
					}
					ONDK_GWM_APP_Destroy(NULL);
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Audio_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Audio_Proc, 0, eNxSettingDepth_3, 0, 0);
					return	ONDK_MESSAGE_BREAK;

				case KEY_EXIT:
				case KEY_MENU:
					eResult = nx_menuLipSyncPopupBar_SaveLibsyncValue(pContents, pContents->nInitValue);
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_PASS;

				case KEY_STANDBY:
				case KEY_GUIDE:
					ONDK_GWM_APP_Destroy(NULL);
					return ONDK_MESSAGE_PASS;

			}
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
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
	NX_APP_Trace("NX_PRISM_MENU_LipSyncPopupBar_Proc message(0x%x) -\n", lMessage);

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
