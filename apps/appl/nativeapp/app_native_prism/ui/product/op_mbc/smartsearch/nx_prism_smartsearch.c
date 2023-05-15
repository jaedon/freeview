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
 * @file	  		nx_prism_smartsearch.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_core_metadata_epg.h>
#include <nx_core_metadata_pf.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_SMARTSEARCH_ITEM_ID					eMenuFrame_Window_SmartSearch

#define NX_SMARTSEARCH_MAIN_BG_ID						(NX_PRISM_SMARTSEARCH_ITEM_ID + 1)
#define NX_SMARTSEARCH_MAIN_BG_X						0
#define NX_SMARTSEARCH_MAIN_BG_Y						0
#define NX_SMARTSEARCH_MAIN_BG_W						NX_PRISM_OSD_HORIZONTAL
#define NX_SMARTSEARCH_MAIN_BG01_W						900
#define NX_SMARTSEARCH_MAIN_BG_H						NX_PRISM_OSD_VERTICAL

#define NX_SMARTSEARCH_RESULTMENU_MAIN_BG_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 2)
#define NX_SMARTSEARCH_RESULTMENU_MAIN_BG_X				60
#define NX_SMARTSEARCH_RESULTMENU_MAIN_BG_Y				163
#define NX_SMARTSEARCH_RESULTMENU_MAIN_BG_W				530
#define NX_SMARTSEARCH_RESULTMENU_MAIN_BG_H				600

#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 3)
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_X				NX_SMARTSEARCH_RESULTMENU_MAIN_BG_X
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_Y				NX_SMARTSEARCH_RESULTMENU_MAIN_BG_Y
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_W				NX_SMARTSEARCH_RESULTMENU_MAIN_BG_W
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_H				NX_SMARTSEARCH_RESULTMENU_MAIN_BG_H

#define NX_SMARTSEARCH_RESULTMENU_ITEM_H					106
#define NX_SMARTSEARCH_RESULTMENU_OBJ_PAGE_COUNT			5

#define NX_SMARTSEARCH_RESULTMENU_TITLE_X					60
#define NX_SMARTSEARCH_RESULTMENU_TITLE_Y					48
#define NX_SMARTSEARCH_RESULTMENU_TITLE_W					520

#define NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_X			60
#define NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_Y			97
#define NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_W			530
#define NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_H			2

#define NX_SMARTSEARCH_RESULTMENU_LINE_X					60
#define NX_SMARTSEARCH_RESULTMENU_LINE_Y					120
#define NX_SMARTSEARCH_RESULTMENU_LINE_W					4
#define NX_SMARTSEARCH_RESULTMENU_LINE_H					20

#define NX_SMARTSEARCH_RESULTMENU_TEXTHELP_X				73
#define NX_SMARTSEARCH_RESULTMENU_TEXTHELP_Y				117
#define NX_SMARTSEARCH_RESULTMENU_TEXTHELP_W				520

#define NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_X			5
#define NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_Y			5
#define NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_W			170
#define NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_H			96

#define NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_X				9
#define NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_Y				9
#define NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_W				36
#define NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_H				36

#define NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_X				188
#define NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_Y				5
#define NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_W				325

#define NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_X				188
#define NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_Y				76
#define NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_W				325

#define NX_SMARTSEARCH_RESULTMENU_SVCID_ITEM_TEXT_Y			10
#define NX_SMARTSEARCH_RESULTMENU_SVCNAME_ITEM_TEXT_Y		40
#define NX_SMARTSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y			74

#define	NX_SMARTSEARCH_SCROLL_BAR_X				460
#define	NX_SMARTSEARCH_SCROLL_BAR_Y				200
#define	NX_SMARTSEARCH_SCROLL_BAR_W				3
#define NX_SMARTSEARCH_SCROLL_BAR_H				5

#define	NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 4)
#define	NX_SMARTSEARCH_GET_PROGRAM_TIMER_INTERVAL		5 // 5 second

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_X		290
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_Y		213
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_W		700
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_H		364

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_ID	(NX_PRISM_SMARTSEARCH_ITEM_ID + 5)

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_X					30
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_Y					26
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_W					640

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_MSG_Y						61

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_X					20
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_Y					106
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_W					660
#define	NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_H					168

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_X					240
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_Y					294
#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_W					220
#define	NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_H					50

#define NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID					(NX_PRISM_SMARTSEARCH_ITEM_ID + 6)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eSMARTSEARCH_MAINMENU,
	eSMARTSEARCH_SUBMENU
} Nx_SmartSearch_DisPlayType_e;

typedef enum
{
	eNxHome_ConnectionStatus_Status_Init,
	eNxHome_ConnectionStatus_Status_Checking,
	eNxHome_ConnectionStatus_Status_DONE,
	eNxHome_ConnectionStatus_Status_RETRY,
	eNxHome_ConnectionStatus_Status_MAX
} Nx_SmartSearch_ConnectionStatusUiStatus_e;

typedef struct tagSmartSearchContext {

	HINT32	nChannelNum;
	NX_Channel_t *pstChannels;

	HCHAR		szCurProgramName[EPG_HUMAX_NAME_LEN];

	HxVector_t	*pstResultList;//list of programmes matched with search criteria

	Nx_SmartSearch_DisPlayType_e	eIdxMenu;

	HUINT32		ulCurChannel;

	//Network Information
	NX_NET_NetIfType_e eCurrentIfType;
	HBOOL   bEthernet;
    HBOOL   bInternet;
	NX_NET_NetIFADDR_t *pstNetIf;
	NX_NET_NetDevInfo_t *pstNetDevInfoList;
	Nx_SmartSearch_ConnectionStatusUiStatus_e eUiStatus;

} Nx_SmartSearchContext_t;


typedef struct tagGenreResultData {
	HINT32	svcUid;
	HINT32 	lcn;
	HINT32	eventId;
	HUINT32 episode;
	HUINT32 season;
	HUINT32	duration;
	//HUINT32	startTime;
	HCHAR	svcName[DxNAME_LEN];
	HCHAR	szProgrammeName[EPG_HUMAX_NAME_LEN];
	//HCHAR	szEventDescription[MAX_ELEMENT_TEXT_LENGTH];
} Nx_SmartSearchResultData_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SmartSearchContext_t		s_stSmartSearchContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static void 			Nx_SmartSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static void 			Nx_SmartSearch_NetWorkDisConnected(Nx_SmartSearchContext_t *pstContents);
static void 			Nx_SmartSearch_HideWorkDisConnected(Nx_SmartSearchContext_t *pstContents, HBOOL isDisplay);
static	ONDK_Result_t	nx_SmartSearch_DrawBackground(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    Nx_SmartSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    Nx_SmartSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_SmartSearchResultData_t *pstData,
									ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId);
static	ONDK_Result_t	nx_SmartSearch_NetworkDisconnected_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_SmartSearch_NetworkDisconnected_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	Nx_SmartSearch_GetCurProgrammFromMBCChannels(Nx_SmartSearchContext_t *pstContents, HUINT32 curChannel);
static ONDK_Result_t	nx_SmartSearch_DoUpdatingNetworkStatus(Nx_SmartSearchContext_t *pstContents);
static ONDK_Result_t	nx_SmartSearch_DoUpdatingUIStatus(Nx_SmartSearchContext_t *pstContents);
static inline Nx_SmartSearchContext_t	*nx_SmartSearch_GetContents(void);
static ONDK_Result_t	nx_SmartSearch_MsgGwmCreate(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmDestroy(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_ConnectionStatus_MsgGwmAPKMessage(Nx_SmartSearchContext_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmKeyDown(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmClick(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static void Nx_SmartSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	stRect = ONDK_Rect(NX_SMARTSEARCH_MAIN_BG_X, NX_SMARTSEARCH_MAIN_BG_Y, NX_SMARTSEARCH_MAIN_BG01_W, NX_SMARTSEARCH_MAIN_BG_H);
	ONDK_GWM_List_Create(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, &stRect, "SMART SEARCH RESULT MENU", NX_SMARTSEARCH_RESULTMENU_ITEM_H, nTotal, NX_SMARTSEARCH_RESULTMENU_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstResultMenu);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);

}

static void Nx_SmartSearch_NetWorkDisConnected(Nx_SmartSearchContext_t *pstContents)
{
	ONDK_Rect_t         stRect = {0, 0, 0, 0}, stRectBtn = {0,0,0,0};
		// Set - Frame
	ONDK_GWM_APP_SetAppArea(NX_SMARTSEARCH_MAIN_BG_X, NX_SMARTSEARCH_MAIN_BG_Y, NX_SMARTSEARCH_MAIN_BG_W, NX_SMARTSEARCH_MAIN_BG_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_X, NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_Y, NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_W, NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_ID, nx_SmartSearch_NetworkDisconnected_DrawFrame);

	//Draw Button
	stRectBtn.x = NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_X + NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_X;
	stRectBtn.y = NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_Y + NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_Y;
	stRectBtn.w = NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_W;
	stRectBtn.h = NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_H;

	if(ONDK_GWM_COM_Button_Create(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID, &stRectBtn, (char*) "") != ONDK_RET_OK)
	{
		//ONDK_GWM_Obj_Resize(stSubItem.ulId, &stRect);
		ONDK_GWM_Obj_Destroy(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID);
		ONDK_GWM_COM_Button_Create(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID, &stRectBtn, (char*) "");
	}
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID, (GWM_ObjDraw_t)nx_SmartSearch_NetworkDisconnected_DrawButton);
}

static void Nx_SmartSearch_HideWorkDisConnected(Nx_SmartSearchContext_t *pstContents, HBOOL isDisplay)
{

	ONDK_GWM_Obj_SetHide(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BG_ID, isDisplay);
	ONDK_GWM_Obj_SetHide(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID, isDisplay);
}

static	ONDK_Result_t	nx_SmartSearch_NetworkDisconnected_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 },stRectTitle = {0,0,0,0}, stRectImg = {0,0,0,0};
	ONDKFontHandle		*hFontText = NULL, *hFontMsg = NULL;

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	hFontMsg = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);

	//Draw while line top
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, 1, COL(C_G_Whistle_100));

	//Draw background
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y + 1, stRect.w, stRect.h -2, COL(C_G_Bang_90));

	//Draw white line bottom
	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y + stRect.h - 1, stRect.w, 1, COL(C_G_Whistle_100));

	//Draw Title
	stRectTitle.x = stRect.x + NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_X;
	stRectTitle.y = stRect.y + NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_Y;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, (HCHAR*)ONDK_GetString(RES_CONNECT_TO_INTERNET_ID), stRectTitle.x, stRectTitle.y, NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_W, COL(C_T_Whistle_100), ONDKSTF_CENTER);

	//Draw Message
	stRectTitle.y = stRect.y + NX_SMARTSEARCH_NETWORK_DISCONNECTED_MSG_Y;
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontMsg, (HCHAR*)ONDK_GetString(RES_MESG_6100_ID), stRectTitle.x, stRectTitle.y, NX_SMARTSEARCH_NETWORK_DISCONNECTED_TITLE_W, COL(C_T_Whistle_100), ONDKSTF_CENTER);

	//Draw Promotion Image
	stRectImg.x = stRect.x + NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_X;
	stRectImg.y = stRect.y + NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_Y;
	stRectImg.w = NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_W;
	stRectImg.h = NX_SMARTSEARCH_NETWORK_DISCONNECTED_IMAGE_H;
	ONDK_GUICOM_IMG_Blit(screen, stRectImg, RES_600_11_NETWORK_PROMOTION02_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	ONDK_FONT_Release(hFontText);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_SmartSearch_NetworkDisconnected_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL;

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Base01_100));

	//Draw String
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, (HCHAR*)ONDK_GetString(RES_CLOSE_02_ID), stRect.x + 10, stRect.y + 14, 200, COL(C_T_Whistle_100), ONDKSTF_CENTER);


	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	ONDK_FONT_Release(hFontText);

	return	lRet;
}

static	ONDK_Result_t	nx_SmartSearch_DrawBackground(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 }, stRectTitle = {0,0,0,0}, stRectLine = {0,0,0,0};
	ONDK_Rect_t			stRectTextHelp = {0,0,0,0}, stRectDiviLine = {0,0,0,0};
	ONDKFontHandle     *hFontTitle = NULL, *hFontTextHelp = NULL;
	Nx_SmartSearchContext_t	*pstContents = nx_SmartSearch_GetContents();
	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_105_00_BG_SS_PNG, RES_105_00_BG_SS_PNG, RES_105_00_BG_SS_PNG);

	hFontTitle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_32);
	hFontTextHelp = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);

	//Draw Program title of current channel
	stRectTitle.x = stRect.x + NX_SMARTSEARCH_RESULTMENU_TITLE_X;
	stRectTitle.y = stRect.y + NX_SMARTSEARCH_RESULTMENU_TITLE_Y;
	stRectTitle.w = NX_SMARTSEARCH_RESULTMENU_TITLE_W;

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontTitle, (char*)pstContents->szCurProgramName, stRectTitle.x, stRectTitle.y, stRectTitle.w, COL(C_T_Gong03), ONDKSTF_LEFT);

	//Draw division line
	stRectDiviLine.x = stRect.x + NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_X;
	stRectDiviLine.y = stRect.y + NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_Y;
	stRectDiviLine.w = NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_W;
	stRectDiviLine.h = NX_SMARTSEARCH_RESULTMENU_DIVISION_LINE_H;

	ONDK_DRAW_FillRectElement(screen, stRectDiviLine.x, stRectDiviLine.y, stRectDiviLine.w, stRectDiviLine.h, COL(C_G_Whistle_20));

	//Draw Line
	stRectLine.x = stRect.x + NX_SMARTSEARCH_RESULTMENU_LINE_X;
	stRectLine.y = stRect.y + NX_SMARTSEARCH_RESULTMENU_LINE_Y;
	stRectLine.w = NX_SMARTSEARCH_RESULTMENU_LINE_W;
	stRectLine.h = NX_SMARTSEARCH_RESULTMENU_LINE_H;

	ONDK_DRAW_FillRectElement(screen, stRectLine.x, stRectLine.y, stRectLine.w, stRectLine.h, COL(C_G_Whistle_20));

	//Draw Text Help
	stRectTextHelp.x = stRect.x + NX_SMARTSEARCH_RESULTMENU_TEXTHELP_X;
	stRectTextHelp.y = stRect.y + NX_SMARTSEARCH_RESULTMENU_TEXTHELP_Y;
	stRectTextHelp.w = NX_SMARTSEARCH_RESULTMENU_TEXTHELP_W;

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontTextHelp, (char*)ONDK_GetString(RES_MESG_6125_ID), stRectTextHelp.x, stRectTextHelp.y, stRectTextHelp.w, COL(C_T_Gong05), ONDKSTF_LEFT);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t    Nx_SmartSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */
	HINT32          nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32          nCellHeight = 0, nCount = 0, nOffsetY = 0, lScroll_height = 0;
	LIST_OBJECT     objList = (LIST_OBJECT)object;
	HINT32          objID;
	HxVector_t      *pstDataList = NULL;
	Nx_SmartSearchResultData_t *pstData = NULL;

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000;
	ONDKFontHandle     *hFontHandle = NULL, *hFontDescription = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL, *hBoldFontDescription = NULL,*hNormalFontDescription = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	Nx_SmartSearchContext_t* pstContents = NULL;
	HBOOL bNeedScrollBar = FALSE;

	NOT_USED_PARAM(arg);

	pstContents = nx_SmartSearch_GetContents();

	//Draw Background
	nx_SmartSearch_DrawBackground(screen, area, rect, object, arg, bUiTop);

	/* create font handler */
	hBoldFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_26);
	hNormalFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_26);

	hBoldFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_18);
	hNormalFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_18);

	hFontDescription = hNormalFontDescription;
	hFontHandle = hNormalFontHandle;
	/* init list coordinate */
	stListRegion.x = area->x1 + rect->x + NX_SMARTSEARCH_RESULTMENU_MAIN_BG_X;
	stListRegion.y = area->y1 + rect->y + NX_SMARTSEARCH_RESULTMENU_MAIN_BG_Y;
	stListRegion.w = NX_SMARTSEARCH_RESULTMENU_MAIN_BG_W;
	stListRegion.h = rect->h;

	/* get list data */
	pstDataList = (HxVector_t *)ONDK_GWM_List_GetArrayPtr(objList);
	ONDK_GWM_List_GetProperty (objList, &nTotal, &nStartIdx, &nCurrIdx, &nSelectedIdx, &nNumOfPage);
	objID = ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList);
	//HxLOG_Print("\n\n[%s:%d] object id (%d)\n\n",__FUNCTION__,__LINE__,objID);

	if (pstDataList == NULL || nTotal == 0)
	{
		NX_APP_Error("ERROR!!! (pstDataList == NULL) || (nTotal == 0) !!\n");
		return ONDK_RET_FAIL;
	}

	nCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList));
	/* index setting */
	if (nTotal > nNumOfPage)
	{
		nCount = nNumOfPage;
		bNeedScrollBar = TRUE;
		lScroll_height = rect->h;
	}
	else
	{
		nCount = nTotal;
		bNeedScrollBar = FALSE;
	}

	for (i = 0 ; i < nCount ; i++)
	{
		/* init data index */
		nDataIdx = nStartIdx + i;
		if (nDataIdx >= nTotal)
		{
			break;
		}
		pstData = HLIB_VECTOR_ItemAt(pstDataList, nDataIdx);

		objID = ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList);
		/* make cursor data */

		if (nDataIdx == nCurrIdx) /* focus */
		{
			textFontColor = COL(C_T_Whistle_100);
			bgColor = COL(C_G_Base01_100);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hBoldFontDescription;
		}
		else
		{
			textFontColor = COL(C_T_Gong01);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hNormalFontDescription;
		}

		Nx_SmartSearch_DrawCursor_A2(screen, pstData, &stListRegion, nOffsetY, hFontHandle, hFontDescription,textFontColor, bgColor,nCellHeight, ((nDataIdx == nCurrIdx)), bUiTop, objID);
		nOffsetY += nCellHeight;
		bgColor = 0x00000000;
	}

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	ONDK_FONT_Release(hNormalFontDescription);
	ONDK_FONT_Release(hBoldFontDescription);

	return  ONDK_RET_OK;
}

static ONDK_Result_t    Nx_SmartSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_SmartSearchResultData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId)
{
	HCHAR           pText[DxNAME_LEN] = {0, };
	Nx_SmartSearchContext_t	*pstContents = NULL;
	ONDK_Rect_t         stRectThumb = {0, 0, 0, 0}, stRectPlus = {0,0,0,0}, stRectTitle = {0,0,0,0}, stRectDesc = {0,0,0,0};
	ONDK_Color_t 	textDescColor = textFontColor;

	pstContents = nx_SmartSearch_GetContents();

	if (bIsFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, ulCellHeight, bgColor);
	}

	//Draw Thumbnail
	stRectThumb.x = stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_X;
	stRectThumb.y = stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_Y;
	stRectThumb.w = NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_W;
	stRectThumb.h = NX_SMARTSEARCH_RESULTMENU_ITEM_THUMBNAIL_H;
	ONDK_GUICOM_IMG_Blit(screen, stRectThumb, RES_212_00_THUMB_TV_PNG);

	//Draw Plus image
	stRectPlus.x = stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_X;
	stRectPlus.y = stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_Y;
	stRectPlus.w = NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_W;
	stRectPlus.h = NX_SMARTSEARCH_RESULTMENU_ITEM_PLUS_H;
	ONDK_GUICOM_IMG_Blit(screen, stRectPlus, RES_600_11_PLUS_PNG);

	//Draw Program Title
	stRectTitle.x = stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_X;
	stRectTitle.y = stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_Y;
	stRectTitle.w = NX_SMARTSEARCH_RESULTMENU_ITEM_TITLE_W;

	HxSTD_sprintf(pText, "%s", pstData->szProgrammeName);
	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stRectTitle.x, stRectTitle.y, stRectTitle.w, textFontColor, ONDKSTF_LEFT);

	//Draw description
	stRectDesc.x = stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_X;
	stRectDesc.y = stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_Y;
	stRectDesc.w = NX_SMARTSEARCH_RESULTMENU_ITEM_DESC_W;

	HxSTD_sprintf(pText, "%d %s / %s %d / %s %d", pstData->duration,(HCHAR*)ONDK_GetString(RES_MINS_ID),
						(HCHAR*)ONDK_GetString(RES_EPISODE_02_ID), pstData->episode,(HCHAR*)ONDK_GetString(RES_SEASON_ID), pstData->season);

	if(textFontColor == COL(C_T_Gong01))
	{
		textDescColor = COL(C_T_Gong03);
	}

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stRectDesc.x, stRectDesc.y, stRectDesc.w, textDescColor, ONDKSTF_LEFT);

	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_SmartSearch_GetCurProgrammFromMBCChannels(Nx_SmartSearchContext_t *pstContents, HUINT32 curChannel)
{
	HUINT32 ulChannelNum = 0, ulIndex = 0;
	NX_Channel_t* pstChannelList, *tmpChannel;
	NX_Programme_t* programme = NULL;
	HxVector_t *result = NULL;
	DxService_t *tempSvc = NULL;
	HCHAR			szEventName[EPG_HUMAX_NAME_LEN] = {'\0',};
	Nx_SmartSearchResultData_t *pstProgramResultData = NULL;

	ulChannelNum = pstContents->nChannelNum;
	pstChannelList = pstContents->pstChannels;
	if (pstChannelList == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_LIVE_GetLiveChannels() is NULL!!! \n");
		return ONDK_RET_FAIL;
	}

	for(ulIndex = 0; ulIndex < ulChannelNum; ulIndex++)
	{
		tmpChannel = &pstChannelList[ulIndex];

		tempSvc = NX_CHANNEL_FindServiceBySvcUid(tmpChannel->svcUid);

		result = HLIB_VECTOR_New(NULL,NULL);

		programme = NX_METADATA_PF_GetPF(tmpChannel);

		HxLOG_Print("Get Program Info based on PF\n");

		if(programme != NULL)
		{
			pstProgramResultData = (Nx_SmartSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_SmartSearchResultData_t));
			if (pstProgramResultData == NULL)
			{
				NX_APP_Free(programme);
				return GWM_RET_OUT_OF_MEMORY;
			}

			HxSTD_MemSet(pstProgramResultData, 0, sizeof(Nx_SmartSearchResultData_t));
			pstProgramResultData->svcUid = NX_METADATA_PROGRAMMES_GetSvcUID(programme);
			pstProgramResultData->eventId = NX_METADATA_PROGRAMMES_GetEventID(programme);
			pstProgramResultData->episode = NX_METADATA_PROGRAMMES_GetEpisode(programme);
			pstProgramResultData->season = NX_METADATA_PROGRAMMES_GetSeason(programme);
			pstProgramResultData->duration = NX_METADATA_PROGRAMMES_GetDuration(programme)/60;

			if(NX_PF_PROGRAMMES_GetNameByProgramme(programme, szEventName) == ERR_OK)
			{
				HxSTD_StrNCpy((char*)pstProgramResultData->szProgrammeName, szEventName, MAX_ELEMENT_TEXT_LENGTH);
				HxLOG_Print("curProgram(%s) duration(%d) epi(%d) sea(%d)\n\n",szEventName, pstProgramResultData->duration,
															pstProgramResultData->episode, pstProgramResultData->season);
			}

			if(ulIndex == curChannel)
			{
				HxSTD_StrNCpy((char*)pstContents->szCurProgramName, szEventName, MAX_ELEMENT_TEXT_LENGTH);
			}
			HLIB_VECTOR_Add(pstContents->pstResultList, pstProgramResultData);

			//Free programme memory
			NX_APP_Free(programme);
		}

	}

	return ONDK_RET_OK;

}

static ONDK_Result_t	nx_SmartSearch_DoUpdatingNetworkStatus(Nx_SmartSearchContext_t *pstContents)
{
    HERROR          hError  = ERR_FAIL;
    NX_NET_NetDevInfo_t *pstNetDevInfoList = NULL;
    HINT32              devCount = 0;
    HINT32              lIndex = 0;
    NX_NET_NetIfType_e netIfType = eNxNet_INTERFACE_TYPE_LAN;
    NX_NET_NetIFADDR_t *pstNetIf = NULL;

    hError = NX_NETWORK_GetNetDevInfo(&pstNetDevInfoList, &devCount);

    //Get Network Info
    for(lIndex=0; lIndex < devCount;lIndex++)
    {
        if(TRUE == pstNetDevInfoList[lIndex].bConnected)
        {
            if(eNxNet_INTERFACE_TYPE_LAN == pstNetDevInfoList[lIndex].eType)
            {
                hError = NX_NETWORK_GetLanInterface(&(pstNetIf));
                netIfType = eNxNet_INTERFACE_TYPE_LAN;
                break;
            }
            else if(eNxNet_INTERFACE_TYPE_WLAN == pstNetDevInfoList[lIndex].eType)
            {
                hError = NX_NETWORK_GetWLanInterface(&(pstNetIf));
                netIfType = eNxNet_INTERFACE_TYPE_WLAN;
                break;
            }
        }
    }

    pstContents->eCurrentIfType = netIfType;

    if(NULL != pstContents->pstNetDevInfoList)
    {
        NX_APP_Free(pstContents->pstNetDevInfoList);
        pstContents->pstNetDevInfoList = NULL;
    }

    if(NULL != pstContents->pstNetIf)
    {
        NX_APP_Free(pstContents->pstNetIf);
        pstContents->pstNetIf = NULL;
    }

    if(NULL == pstNetIf)
    {
        pstNetIf = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
        HxSTD_MemSet(pstNetIf, 0x00, sizeof(NX_NET_NetIFADDR_t));

        pstContents->bEthernet = FALSE;
        pstContents->bInternet = FALSE;
        //Network Connection has failed.
        pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_RETRY;
    }

    pstContents->pstNetIf = pstNetIf;
    pstContents->pstNetDevInfoList = pstNetDevInfoList;

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_SmartSearch_DoUpdatingUIStatus(Nx_SmartSearchContext_t *pstContents)
{
	HUINT32 	nTotal = 0;

	if(pstContents->bInternet == FALSE)
	{
		//Nx_SmartSearch_NetWorkDisConnected(pstContents);
		Nx_SmartSearch_HideWorkDisConnected(pstContents, FALSE);
		ONDK_GWM_Obj_SetFocus(NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID);
	}
	else
	{
		//Get program information of all MBC channels
		Nx_SmartSearch_GetCurProgrammFromMBCChannels(pstContents,pstContents->ulCurChannel);

		// Set - Frame
		ONDK_GWM_APP_SetAppArea(NX_SMARTSEARCH_MAIN_BG_X, NX_SMARTSEARCH_MAIN_BG_Y, NX_SMARTSEARCH_MAIN_BG01_W, NX_SMARTSEARCH_MAIN_BG_H);

		//Create result menu
		nTotal = HLIB_VECTOR_Length(pstContents->pstResultList);
		Nx_SmartSearch_CreateResultMenu(pstContents->pstResultList, nTotal, (GWM_ObjDraw_t)Nx_SmartSearch_DrawMenu);
	}

	return ONDK_RET_OK;
}

static inline Nx_SmartSearchContext_t	*nx_SmartSearch_GetContents(void)
{
	return	&s_stSmartSearchContent;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmCreate(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_NET_NetIfType_e	netIfType = eNxNet_INTERFACE_TYPE_MAX;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_SmartSearchContext_t));

	NX_CHANNEL_GROUP_MakeNxChannelListAll(&pstContents->pstChannels, &pstContents->nChannelNum );

	if (pstContents->nChannelNum == 0)
	{
		NX_APP_Error("Error!!! NX_PRISM_LIVE_GetChannelListNum() fail --- channel num  0!!\n");
		return ONDK_RET_FAIL;
	}

	//pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_LIVE_GetLiveChannels() is NULL!!! \n");
		return ONDK_RET_FAIL;
	}

	pstContents->ulCurChannel = p1;

	//initiate ResultData
	pstContents->pstResultList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;

	Nx_SmartSearch_NetWorkDisConnected(pstContents);
	Nx_SmartSearch_HideWorkDisConnected(pstContents, TRUE);

	//Check network
    NX_NETWORK_RegisterNotifier();
	nx_SmartSearch_DoUpdatingNetworkStatus(pstContents);

	netIfType = pstContents->eCurrentIfType;

    if(pstContents->eUiStatus != eNxHome_ConnectionStatus_Status_RETRY)
    {
       // nx_menuConnectionStatus_CreateProcessingMsg((char*)ONDK_GetString(RES_CONNECTING_ID));
      	HxLOG_Print("checking gateway\n\n");
        pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_Checking;
        NX_NETWORK_CheckGateway(netIfType);
    }
	else
    {
		HxLOG_Print("network not exists\n\n");
		nx_SmartSearch_DoUpdatingUIStatus(pstContents);
    }

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmDestroy(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_Destroy(NX_PRISM_SimpleKeyboard_Proc);

	if(pstContents->pstResultList)
		HLIB_VECTOR_Delete(pstContents->pstResultList);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SmartSearch_ConnectionStatus_MsgGwmAPKMessage(Nx_SmartSearchContext_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e eEvent = p1;
    HxLOG_Error("\n\n#### nx_menuConnectionStatus_MsgGwmAPKMessage \n");

    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
    		case eNETWORK_CONCHK_ARP_SUCCESS :
                //GW SUCCESS
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_ARP_SUCCESS \n");
                pstContents->bEthernet = TRUE;
                NX_NETWORK_CheckDNS(pstContents->eCurrentIfType);
		    	break;
    		case eNETWORK_CONCHK_ARP_FAIL :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_ARP_FAIL \n");
                pstContents->bEthernet = FALSE;
                pstContents->bInternet = FALSE;
                pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_RETRY;
                nx_SmartSearch_DoUpdatingUIStatus(pstContents);
              //  nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_SUCCESS :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_SUCCESS \n");
                //DNS SUCCESS
                pstContents->bInternet = TRUE;
                pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_DONE;
               	nx_SmartSearch_DoUpdatingUIStatus(pstContents);
               // nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_FAIL :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_FAIL \n");
                pstContents->bInternet = FALSE;
                pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_DONE;
              	nx_SmartSearch_DoUpdatingUIStatus(pstContents);
              //  nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
			case eNETWORK_STATUS_CABLE_LINK_CONNECTED:
			 	HxLOG_Print("LAN connected\n\n");
			 	break;
			case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED:
			 	HxLOG_Print("LAN disconnected\n\n");
			 	break;
            default:
                break;
        }
    }

    return ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmKeyDown(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, ulObjId = 0;
	HUINT32 curIndex = 0;
	Nx_SmartSearchResultData_t* pstCurData;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ulObjId = GWM_GetFocusedObjectID();
	HxLOG_Debug("\n\n\n receive key value (%d)\n\n\n", lKeyId);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_RED:
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			return	ONDK_MESSAGE_BREAK;

		case KEY_BACK:
			if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
			{
				ONDK_GWM_APP_Destroy(0);
				return	ONDK_MESSAGE_BREAK;
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWLEFT:
			if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
			{
				HxVector_t* tmp = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
				pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(tmp), (GWM_LIST_ITEMS)tmp);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);
			}
			return ONDK_MESSAGE_BREAK;
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_OK:
			{
				if(pstContents->bInternet == FALSE)
				{
					HxLOG_Print("\n\n[%s:%d] close\n\n",__FUNCTION__,__LINE__);
				}
				else
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);
					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, curIndex);
					if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)Nx_PRISM_ProgramDetail_Proc) ==NULL )
					{
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, pstCurData->svcUid, pstCurData->eventId, p3);
					}
				}
			}
			break;

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			//return	ONDK_MESSAGE_PASS;
			return ONDK_MESSAGE_BREAK;
	}
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmClick(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulObjId = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ulObjId = (HUINT32) p1;

	if(ulObjId == NX_SMARTSEARCH_NETWORK_DISCONNECTED_BUTTON_ID)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_OK;
	}


	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SMARTSEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SmartSearchContext_t	*pstContents = NULL;

	pstContents = nx_SmartSearch_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_SmartSearch_MsgGwmCreate(pstContents, p1, p2, p3);
			if (eResult != ONDK_RET_OK)
			{
				HxLOG_Error("Failed Init TvGuide!!!\n\n");
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_NotAvailable_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_NotAvailable_Proc, 0, 0, 0, 0);
			}
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_SmartSearch_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			eResult = nx_SmartSearch_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
            eResult = nx_SmartSearch_ConnectionStatus_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_SmartSearch_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			//eResult = nx_guide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			/*NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_guide_DestroyListPopup();
				eResult = nx_guide_OptionChangedItemValue(pstContents, p1);
			}*/
			break;
		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

