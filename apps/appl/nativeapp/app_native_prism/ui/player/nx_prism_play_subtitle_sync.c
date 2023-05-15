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
 * @file	  		nx_prism_play_subtitle_sync.c
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
#define	NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT			120
#define	NX_PRISM_UI_SUBTITLE_VALUE_GRADE					500

#define	NX_PRISM_LIVE_OPT_ITEM_ID							eMenuFrame_Window_LiveOpt

#define	NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID					(NX_PRISM_LIVE_OPT_ITEM_ID + 1)
#define	NX_PRISM_UI_SUBTITLE_SYNC_DELAY_X					0
#define	NX_PRISM_UI_SUBTITLE_SYNC_DELAY_Y					320
#define	NX_PRISM_UI_SUBTITLE_SYNC_DELAY_WIDTH				NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_SUBTITLE_SYNC_DELAY_HEIGHT				400

#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_X				289
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_Y				320
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_WIDTH			NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_HEIGHT			400

#define	NX_PRISM_UI_SUBTITLE_SYNC_TXT_X						120
#define	NX_PRISM_UI_SUBTITLE_SYNC_TXT_Y						320
#define	NX_PRISM_UI_SUBTITLE_SYNC_TXT_WIDTH					150
#define	NX_PRISM_UI_SUBTITLE_SYNC_TXT_HEIGHT				24

#define	NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID			(NX_PRISM_LIVE_OPT_ITEM_ID + 2)
#define	NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_X				1050
#define	NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_Y				640
#define	NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_WIDTH			140
#define	NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_HEIGHT		24

#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_X						290
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_Y						650
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH					720
#define	NX_PRISM_UI_SUBTITLE_SYNC_BAR_HEIGHT				5

#define	NX_PRISM_UI_SUBTITLE_SYNC_CENTER_ID					(NX_PRISM_LIVE_OPT_ITEM_ID + 3)
#define	NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH				2
#define	NX_PRISM_UI_SUBTITLE_SYNC_CENTER_HEIGHT_MARGIN		4

#define	NX_PRISM_LIVE_OPT_MAINITEM_ID						(NX_PRISM_LIVE_OPT_ITEM_ID|1000)

typedef enum {
	eSubSyncBarItem_Sub_Sync,
	eSubSyncBarItem_MAX,
} Nx_SubSyncBarItem_t;

typedef enum {
	eSubSyncBarNotFocused_Frame = eSubSyncBarItem_MAX,
	eSubSyncBarNotFocused_MAX,
} Nx_SubSyncBarNotFocusedItem_t;

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tag_NX_SUB_SYNC_CONTENT
{
	HUINT32			ulSyncTime;
	HUINT32			ulViewId;

	HUINT8			SubSyncIntervalText[NX_TEXT_SIZE_16];
} Nx_SubSyncContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SubSyncContent_t		s_SubSyncContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_SubSyncContent_t* 			nx_subSyncBar_GetContents(void);
static HINT32								nx_subSyncPopupBar_ConvertItemIdxToObjId(HINT32 nItemIdx);
static HINT32								nx_subSyncPopupBar_ConvertNotFocusedItemIdxToObjId(HINT32 nItemIdx);

static ONDK_Result_t						nx_subSyncPopupBar_DrawFrame (ONDKSurfaceHandle screen, ONDK_Region_t *area,ONDK_Rect_t *rect,
												ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t						nx_subSyncPopupBar_DrawIntervalBar (ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static void 								nx_subSyncPopupBar_DrawDefaultUI(Nx_SubSyncContent_t *pContents);
static ONDK_Result_t 						nx_subSyncPopupBar_SaveSubSyncValue(Nx_SubSyncContent_t *pContents, HINT32 nLiSpync);

static ONDK_Result_t						nx_subSyncPopupBar_MsgGwmCreate(Nx_SubSyncContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t						nx_subSyncPopupBar_MsgGwmDestroy(Nx_SubSyncContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_SubSyncContent_t* 	nx_subSyncBar_GetContents(void)
{
	return	&s_SubSyncContent;
}

static HINT32	nx_subSyncPopupBar_ConvertItemIdxToObjId(HINT32 nItemIdx)
{
	if ((nItemIdx < 0) || (nItemIdx >= eSubSyncBarItem_MAX))
	{
		return 0;
	}

	return	(NX_PRISM_LIVE_OPT_MAINITEM_ID | nItemIdx);
}

static HINT32	nx_subSyncPopupBar_ConvertNotFocusedItemIdxToObjId(HINT32 nItemIdx)
{
	if((nItemIdx < eSubSyncBarNotFocused_Frame) || (nItemIdx >= eSubSyncBarNotFocused_MAX))
	{
		return 0;
	}

	return (NX_PRISM_LIVE_OPT_MAINITEM_ID | nItemIdx);
}

static HINT32	nx_subSyncPopupBar_ConvertSyncTimeToIndex(HINT32 lSyncTime)
{
	HINT32	lLowest = (HINT32)(NX_PRISM_UI_SUBTITLE_VALUE_GRADE * -(NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT / 2));
	HINT32	lIndex = 0;

	lIndex = (HINT32)((lSyncTime - lLowest) / NX_PRISM_UI_SUBTITLE_VALUE_GRADE);

	return lIndex;
}

static ONDK_Result_t	nx_subSyncPopupBar_DrawFrame (ONDKSurfaceHandle screen, ONDK_Region_t *area,ONDK_Rect_t *rect,
												ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32				x = 0, y = 0, w = 0, h = 0;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };
	HUINT8				*szStr = ONDK_GetString(RES_SYNC_01_ID);

	/* absolute coordinate */
	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	/* draw box */
	stRect = ONDK_Rect(x, y, w, h);
	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG);

	/* Sub_sync Text */
	stRect = ONDK_Rect(x + NX_PRISM_UI_SUBTITLE_SYNC_TXT_X, y + NX_PRISM_UI_SUBTITLE_SYNC_TXT_Y, NX_PRISM_UI_SUBTITLE_SYNC_TXT_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_TXT_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID, &stRect, (HCHAR*)HLIB_STD_StrUpper((HCHAR*)szStr));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_subSyncPopupBar_DrawIntervalBar (ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32				x = 0, y = 0, w = 0, h = 0;
	HINT32				nInterval = 0, barWidth = 0;
	HINT32				lCenter = NX_PRISM_UI_SUBTITLE_SYNC_BAR_X + (NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH / 2);
	HINT32				lAbValueCount = NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT / 2;
	HBOOL				bPlusValue = FALSE;
	HFLOAT64			flSyncTime = 0;
	ONDK_GWM_GuiObject_t	stTextObject;
	Nx_SubSyncContent_t	*pContents = nx_subSyncBar_GetContents();

	/* absolute coordinate */
	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	nInterval = ONDK_GWM_Selector_GetCurrentIndex(ONDK_GWM_Obj_GetObjectID(object));

	if (nInterval <= lAbValueCount)
	{
		barWidth = (NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH / NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT) * (lAbValueCount - nInterval);
		bPlusValue = FALSE;
	}
	else
	{
		barWidth = (NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH / NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT) * (nInterval - lAbValueCount);
		bPlusValue = TRUE;
	}

	/* background bar */
	ONDK_DRAW_FillRectElement(screen, x, NX_PRISM_UI_SUBTITLE_SYNC_BAR_Y, NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_BAR_HEIGHT, COL(C_G_Whistle_20));

	/* level bar */
	if (barWidth > 0)
	{
		if (bPlusValue == TRUE)
		{
				ONDK_DRAW_FillRectElement(screen, (lCenter + NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH), NX_PRISM_UI_SUBTITLE_SYNC_BAR_Y,
							(barWidth - NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH), NX_PRISM_UI_SUBTITLE_SYNC_BAR_HEIGHT, COL(C_G_Base01_100));
		}
		else
		{
				ONDK_DRAW_FillRectElement(screen, (lCenter - barWidth - NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH),
						NX_PRISM_UI_SUBTITLE_SYNC_BAR_Y, (barWidth + NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH), NX_PRISM_UI_SUBTITLE_SYNC_BAR_HEIGHT, COL(C_G_Base01_100));
		}
	}

	stTextObject = ONDK_GWM_APP_GetObject(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID);
	if (stTextObject)
	{
		flSyncTime = ((HFLOAT64)((nInterval - lAbValueCount) * NX_PRISM_UI_SUBTITLE_VALUE_GRADE) / 1000);
		HxSTD_snprintf((HCHAR *)pContents->SubSyncIntervalText, NX_TEXT_SIZE_16, "%.1lf %s", flSyncTime, (HCHAR*)ONDK_GetString(RES_SEC_ID));
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return ONDK_RET_OK;
}

static void 	nx_subSyncPopupBar_DrawDefaultUI(Nx_SubSyncContent_t *pContents)
{
	HINT32				nObjId = 0;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };
	HFLOAT64			flSyncTime = 0;

	/* Frame */
	nObjId = nx_subSyncPopupBar_ConvertNotFocusedItemIdxToObjId(eSubSyncBarNotFocused_Frame);
	stRect = ONDK_Rect(NX_PRISM_UI_SUBTITLE_SYNC_DELAY_X, NX_PRISM_UI_SUBTITLE_SYNC_DELAY_Y, NX_PRISM_UI_SUBTITLE_SYNC_DELAY_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_DELAY_HEIGHT);
	ONDK_GWM_COM_Frame_Create(nObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SYNC_01_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(nObjId, nx_subSyncPopupBar_DrawFrame);

	/* Center */
	stRect = ONDK_Rect(NX_PRISM_UI_SUBTITLE_SYNC_BAR_X + (NX_PRISM_UI_SUBTITLE_SYNC_BAR_WIDTH / 2), NX_PRISM_UI_SUBTITLE_SYNC_BAR_Y - (NX_PRISM_UI_SUBTITLE_SYNC_CENTER_HEIGHT_MARGIN / 2),
			NX_PRISM_UI_SUBTITLE_SYNC_CENTER_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_BAR_HEIGHT + NX_PRISM_UI_SUBTITLE_SYNC_CENTER_HEIGHT_MARGIN);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_SUBTITLE_SYNC_CENTER_ID, &stRect, COL(C_G_Whistle_100));

	/* Sub-sync Bar */
	nObjId = nx_subSyncPopupBar_ConvertItemIdxToObjId(eSubSyncBarItem_Sub_Sync);
	stRect = ONDK_Rect(NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_X, NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_Y, NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_BAR_WINDOW_HEIGHT);
	ONDK_GWM_Selector_Create(nObjId, &stRect, NULL, (NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT + 1), NULL);
	ONDK_GWM_Selector_SetCurrentIndex(nObjId, nx_subSyncPopupBar_ConvertSyncTimeToIndex((HINT32)pContents->ulSyncTime));
	ONDK_GWM_APP_SetObjectDrawMethod(nObjId, (GWM_ObjDraw_t)nx_subSyncPopupBar_DrawIntervalBar);

	/* Sub-sync Interval Text */
	flSyncTime = ((HFLOAT64)(pContents->ulSyncTime) / 1000);
	HxSTD_snprintf((HCHAR *)pContents->SubSyncIntervalText, NX_TEXT_SIZE_16, "%.1lf %s", flSyncTime, (HCHAR*)ONDK_GetString(RES_SEC_ID));
	stRect = ONDK_Rect(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_X, NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_Y, NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_WIDTH, NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_HEIGHT);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID, &stRect, (HCHAR *)pContents->SubSyncIntervalText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_SUBTITLE_SYNC_INTERVALTXT_ID, COL(C_T_Base01), COL(C_T_Base01));

	ONDK_GWM_Obj_SetFocus(nObjId);
}

static ONDK_Result_t 	nx_subSyncPopupBar_SaveSubSyncValue(Nx_SubSyncContent_t *pContents, HINT32 lSyncIdx)
{

	HERROR					hError = ERR_FAIL;
	HINT32					lSyncValue = 0;

	lSyncValue = (HINT32)((lSyncIdx - (NX_PRISM_UI_SUBTITLE_SELECTOR_ITEM_COUNT / 2)) * NX_PRISM_UI_SUBTITLE_VALUE_GRADE);

	hError = NX_MEDIA_PLAY_SetSubtitleSyncTime(pContents->ulViewId, lSyncValue);
	if (hError != ERR_OK)
	{
		NX_APP_Info("Error!!! NX_MEDIA_PLAY_SetSubtitleSyncTime() Fail !!\n");
		return ONDK_RET_FAIL;
	}

	pContents->ulSyncTime = lSyncValue;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_subSyncPopupBar_MsgGwmCreate(Nx_SubSyncContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pContents == NULL)
	{
		ONDK_GWM_APP_Destroy(NULL);
		NX_APP_Error("Error!!! pContents is NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	ONDK_GWM_APP_SetAppArea(0, 0, NX_PRISM_OSD_HORIZONTAL, NX_PRISM_OSD_VERTICAL);

	NX_MEDIA_PLAY_GetMainViewId(&pContents->ulViewId);
	NX_MEDIA_PLAY_GetSubtitleSyncTime(pContents->ulViewId, &pContents->ulSyncTime);

	nx_subSyncPopupBar_DrawDefaultUI(pContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_subSyncPopupBar_MsgGwmDestroy(Nx_SubSyncContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	HxSTD_memset(pContents, 0x00, sizeof(Nx_SubSyncContent_t));

	NX_COMMON_NOT_USED_PARAM(pContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}
/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_SUBTITLE_Sync_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_SubSyncContent_t		*pContents = NULL;

	pContents = nx_subSyncBar_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_subSyncPopupBar_MsgGwmCreate(pContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_subSyncPopupBar_MsgGwmDestroy(pContents, p1, p2, p3);
			break;

		case MSG_GWM_SELECTOR_CHANGED:
			NX_APP_Message("MSG_GWM_SELECTOR_CHANGED!\n");
			eResult = nx_subSyncPopupBar_SaveSubSyncValue(pContents, p2);
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
					ONDK_GWM_APP_Destroy(NULL);
#if defined(CONFIG_MW_MM_MEDIA)
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SUBTITLE_Option_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SUBTITLE_Option_Proc, 0, p1, p2, p3);
#endif
					return	ONDK_MESSAGE_BREAK;

				case KEY_EXIT:
				case KEY_MENU:
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_PASS;

				case KEY_STANDBY:
				case KEY_GUIDE:
					ONDK_GWM_APP_Destroy(NULL);
					return ONDK_MESSAGE_PASS;

				case KEY_SPECIAL_FASTFORWARD:
				case KEY_SPECIAL_REWIND:
				case KEY_PLAY:
				case KEY_PAUSE:
				case KEY_STOP:
				case KEY_OPT:
					return ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_APP_PLAYER_DESTROY:
			ONDK_GWM_APP_Destroy(0);
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
