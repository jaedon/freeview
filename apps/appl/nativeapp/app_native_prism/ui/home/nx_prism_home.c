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
 * @file	  		nx_prism_home.c
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

//#define CONFIG_APUI_SUPPORT_ANIMATION

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_HOMEMENU_FRAME_ID			0x00010000
#define	NX_HOMEMENU_TITLE_ID			0x00020000
#define	NX_HOMEMENU_MENU_ID				0x00030000
#define	NX_HOMEMENU_IMGN_ID				0x00040000
#define	NX_HOMEMENU_IMGC_ID				0x00050000
#define	NX_HOMEMENU_FRAME_LINE_ID		0x00060000
#define	NX_HOMEMENU_RECENT_IMG_ID		0x00070000
#define	NX_HOMEMENU_RECENT_TXT_ID		0x00080000

// Support Home menu cound
#define	NX_HOMEMENU_COUNT5				5
#define	NX_HOMEMENU_COUNT6				6

#define	NX_HOMEMENU_FRAME_X				0
#define	NX_HOMEMENU_FRAME_Y				0
#define	NX_HOMEMENU_FRAME_W				NX_PRISM_OSD_HORIZONTAL
#define	NX_HOMEMENU_FRAME_H				NX_PRISM_OSD_VERTICAL

#define	NX_HOMEMENU_FRAME_BG_X			0
#define	NX_HOMEMENU_FRAME_BG_Y			320
#define	NX_HOMEMENU_FRAME_BG_W			NX_HOMEMENU_FRAME_W
#define	NX_HOMEMENU_FRAME_BG_H			400

#define	NX_HOMEMENU_TITLE_X				90
#define	NX_HOMEMENU_TITLE_Y				409
#define	NX_HOMEMENU_TITLE_W				1100
#define	NX_HOMEMENU_TITLE_H				64

#define	NX_HOMEMENU_ICON_BOX5_X			184
#define	NX_HOMEMENU_ICON_BOX6_X			123
#define	NX_HOMEMENU_ICON_BOX_Y			506
#define	NX_HOMEMENU_ICON_BOX_W			122
#define	NX_HOMEMENU_ICON_BOX_H			135

#define	NX_HOMEMENU_ICON_BOX_S_X_OFFSET	12
#define	NX_HOMEMENU_ICON_BOX_S_Y		(NX_HOMEMENU_ICON_BOX_Y + 27)
#define	NX_HOMEMENU_ICON_BOX_S_W		(NX_HOMEMENU_ICON_BOX_W - 24)  // 80%
#define	NX_HOMEMENU_ICON_BOX_S_H		(NX_HOMEMENU_ICON_BOX_H - 27) // 80%

#define	NX_HOMEMENU_ICON_BOX_MG			60

#define	NX_HOMEMENU_ICON_BOX_ITEM_W		(NX_HOMEMENU_ICON_BOX_W + NX_HOMEMENU_ICON_BOX_MG)
#define	NX_HOMEMENU_ICON_BOX_TOTOL_W	1092

#define	NX_HOMEMENU_FRAME_LINE_X		63
#define	NX_HOMEMENU_FRAME_LINE_Y		613
#define	NX_HOMEMENU_FRAME_LINE_W		1154
#define	NX_HOMEMENU_FRAME_LINE_H		51

#define	NX_HOMEMENU_RECENT_IMG_X		(NX_HOMEMENU_ICON_BOX5_X + 400)
#define	NX_HOMEMENU_RECENT_IMG_Y		653
#define	NX_HOMEMENU_RECENT_IMG_W		30
#define	NX_HOMEMENU_RECENT_IMG_H		26

#define	NX_HOMEMENU_RECENT_TXT_X_TAB	10
#define	NX_HOMEMENU_RECENT_TXT_X		(NX_HOMEMENU_RECENT_IMG_X + NX_HOMEMENU_RECENT_IMG_W + NX_HOMEMENU_RECENT_TXT_X_TAB)
#define	NX_HOMEMENU_RECENT_TXT_Y		653
#define	NX_HOMEMENU_RECENT_TXT_W		94
#define	NX_HOMEMENU_RECENT_TXT_H		22


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxHomeIdx_LiveTv,
	eNxHomeIdx_Guide,
	eNxHomeIdx_Recordings,
	eNxHomeIdx_MediaCenter,
	eNxHomeIdx_TVApps,
	eNxHomeIdx_Settings,
	eNxHomeIdx_Schedule,
	eNxHomeIdx_Max
} NxHomeIdx_t;

typedef struct NxHome_Context
{
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*pstSubItems;

	HUINT32		ulSelectedItem;
	HUINT8		szMainItemText[MAX_ELEMENT_TEXT_LENGTH];

	HUINT32		ulMaxHomeMenu;
} NxHome_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxHome_Contents_t	s_stNxHome_Contents;
static HUINT32				s_LastCursorIndex = 0;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_HomeMenu_MsgGwmCreate(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_HomeMenu_MsgGwmDestroy(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmHapiMsg(NxHome_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_HomeMenu_MsgGwmObjFocusChanged(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyDown(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmClick(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static NxHome_Contents_t*	nx_HomeMenu_GetContents(void);
static ONDK_Result_t	nx_HomeMenu_Initialize(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemLiveTv(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemGuide(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemSchedule(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemRecord(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemMediaCenter(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemTvApps(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemSettings(NxHome_Contents_t *pstContents);

static ONDK_Result_t	nx_HomeMenu_AddItemFrame(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemTitle(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemRecent(NxHome_Contents_t *pstContents);

static ONDK_Result_t 	nx_HomeMenu_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_HomeMenu_DrawBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_HomeMenu_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_HomeMenu_UpdateTitle(NxHome_Contents_t *pstContents, HUINT32 ulCurFocus);
static HINT32			nx_HomeMenu_GetStartPosForHomeMenuCursor(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_LaunchSubMenu(NxHome_Contents_t *pstContents, HUINT32 ulObjId);
static ONDK_Result_t 	nx_HomeMenu_Destroy(NxHome_Contents_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxHome_Contents_t* nx_HomeMenu_GetContents(void)
{
	return &s_stNxHome_Contents;
}


static ONDK_Result_t	nx_HomeMenu_Initialize(NxHome_Contents_t *pstContents)
{
	HUINT32			ulObjId = 0;
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// Get Max count of Home Menu Button
	NX_PRISM_SPEC_GetHomeMenuUiSupportNum(&pstContents->ulMaxHomeMenu);

	eResult = nx_HomeMenu_AddItemFrame(pstContents);
	eResult = nx_HomeMenu_AddItemTitle(pstContents);

	eResult = nx_HomeMenu_AddItemLiveTv(pstContents);
	eResult = nx_HomeMenu_AddItemGuide(pstContents);
	eResult = nx_HomeMenu_AddItemRecord(pstContents);
	eResult = nx_HomeMenu_AddItemMediaCenter(pstContents);
	eResult = nx_HomeMenu_AddItemTvApps(pstContents);
	eResult = nx_HomeMenu_AddItemSchedule(pstContents);
	eResult = nx_HomeMenu_AddItemSettings(pstContents);

	eResult = nx_HomeMenu_AddItemRecent(pstContents);

	// Update Title Name
	pstContents->ulCurFocus = s_LastCursorIndex;
	nx_HomeMenu_UpdateTitle(pstContents, pstContents->ulCurFocus);
	// Set Cursor
	eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, pstContents->ulCurFocus, &ulObjId);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_GetObjIdBySubitemIndex() Fail Focus:(%d) !!\n", pstContents->ulCurFocus);
		eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, 0, &ulObjId);
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! NX_PRISM_AppContents_GetObjIdBySubitemIndex() Fail!! \n");
		}
	}

	ONDK_GWM_Obj_SetFocus(ulObjId);

	return	eResult;
}


static ONDK_Result_t	nx_HomeMenu_AddItemLiveTv(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_LiveTv);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_LiveTv(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_LIVE_TV_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_LiveTv);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	stSubItem.childAppToLaunch = NULL;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_LiveTv), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_LiveTv), (HCHAR*)RES_371_00_H_LTV_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_LiveTv), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_LiveTv), (HCHAR*)RES_371_00_H_LTV_C_PNG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemGuide(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Guide);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_Guide(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_TV_GUIDE_01_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Guide);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_GUIDE_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_GUIDE_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Guide), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Guide), (HCHAR*)RES_371_00_H_EPG_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Guide), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Guide), (HCHAR*)RES_371_00_H_EPG_C_PNG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemSchedule(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Schedule);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_Schedule(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_SCHEDULES_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Schedule);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);

	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_SCHEDULE_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_SCHEDULE_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Schedule), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Schedule), (HCHAR*)RES_371_00_H_SCH_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Schedule), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Schedule), (HCHAR*)RES_371_00_H_SCH_C_PNG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemRecord(NxHome_Contents_t *pstContents)
{
#if defined(CONFIG_MW_MM_PVR)
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Record);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_Record(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_RECORDINGS_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Recordings);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);

	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_RECORDINGS_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_RECORDINGS_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Recordings), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Recordings), (HCHAR*)RES_371_04_H_REC_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Recordings), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Recordings), (HCHAR*)RES_371_04_H_REC_C_PNG);
#else
	NX_COMMON_NOT_USED_PARAM(pstContents);
#endif
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemMediaCenter(NxHome_Contents_t *pstContents)
{
#if defined(CONFIG_MW_MM_MEDIA)
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_MediaCenter);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_MediaCenter(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_MEDIA_CENTER_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_MediaCenter);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_MEDIA_TypeSelect_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_MEDIA_TypeSelect_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_MediaCenter), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_MediaCenter), (HCHAR*)RES_371_05_H_MC_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_MediaCenter), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_MediaCenter), (HCHAR*)RES_371_05_H_MC_C_PNG);
#else
	NX_COMMON_NOT_USED_PARAM(pstContents);
#endif

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemTvApps(NxHome_Contents_t *pstContents)
{
#if defined(CONFIG_MWC_OSD_PROFILE_BROWSER)
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_TVApps);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_TVApps(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_TV_APPS_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_TVApps);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	stSubItem.childAppToLaunch = NULL;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_TVApps), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_TVApps), (HCHAR*)RES_371_07_H_TA_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_TVApps), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_TVApps), (HCHAR*)RES_371_07_H_TA_C_PNG);
#else
	NX_COMMON_NOT_USED_PARAM(pstContents);
#endif

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemSettings(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	HINT32				lStartX = 0;
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Settings);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_Settings(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_SETTINGS_01_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Settings);
	lStartX = nx_HomeMenu_GetStartPosForHomeMenuCursor(pstContents);
	stRectN = ONDK_Rect(lStartX+NX_HOMEMENU_ICON_BOX_S_X_OFFSET, NX_HOMEMENU_ICON_BOX_S_Y, NX_HOMEMENU_ICON_BOX_S_W, NX_HOMEMENU_ICON_BOX_S_H);
	stRect = ONDK_Rect(lStartX, NX_HOMEMENU_ICON_BOX_Y, NX_HOMEMENU_ICON_BOX_W, NX_HOMEMENU_ICON_BOX_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_MENU_Settings_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_MENU_Settings_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	// Create Image - Normal
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Settings), &stRectN);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Settings), (HCHAR*)RES_371_00_H_SET_N_PNG);

	// Create Image - Cursor
	ONDK_GWM_COM_Image_Create((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Settings), &stRect);
	ONDK_GWM_COM_Image_SetImage((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Settings), (HCHAR*)RES_371_00_H_SET_C_PNG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemFrame(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Set - Frame
	ONDK_GWM_APP_SetAppArea(NX_HOMEMENU_FRAME_X, NX_HOMEMENU_FRAME_Y, NX_HOMEMENU_FRAME_W, NX_HOMEMENU_FRAME_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_HOMEMENU_FRAME_BG_X, NX_HOMEMENU_FRAME_BG_Y, NX_HOMEMENU_FRAME_BG_W, NX_HOMEMENU_FRAME_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_FRAME_ID, nx_HomeMenu_DrawFrame);

	// Draw - Box Line
	stRect = ONDK_Rect(NX_HOMEMENU_FRAME_LINE_X, NX_HOMEMENU_FRAME_LINE_Y, NX_HOMEMENU_FRAME_LINE_W, NX_HOMEMENU_FRAME_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_FRAME_LINE_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_FRAME_LINE_ID, nx_HomeMenu_DrawBox);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemTitle(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - Title
	stRect = ONDK_Rect(NX_HOMEMENU_TITLE_X, NX_HOMEMENU_TITLE_Y, NX_HOMEMENU_TITLE_W, NX_HOMEMENU_TITLE_H);
	ONDK_GWM_COM_Text_Create(NX_HOMEMENU_TITLE_ID, &stRect, (HCHAR*)pstContents->szMainItemText);
	ONDK_GWM_COM_Text_SetFont(NX_HOMEMENU_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_TITLE_ID, NX_PRISM_FONT_SIZE_64);
	ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_TITLE_ID, COL(C_T_Base01), COL(C_T_Bang_80));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_AddItemRecent(NxHome_Contents_t *pstContents)
{
	HBOOL			bSupport = FALSE;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Recent);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support nx_HomeMenu_AddItemRecent(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	// TODO: [JHLEE] Check!!! Image & Text를 Text Size를 고려해서 Center 정렬 해야 한다.
	// Image - Down arrow
	stRect = ONDK_Rect(NX_HOMEMENU_RECENT_IMG_X, NX_HOMEMENU_RECENT_IMG_Y, NX_HOMEMENU_RECENT_IMG_W, NX_HOMEMENU_RECENT_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_HOMEMENU_RECENT_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_HOMEMENU_RECENT_IMG_ID, (HCHAR*)RES_106_00_ARROW_BOX_DOWN_PNG);

	// TEXT - Recent
	stRect = ONDK_Rect(NX_HOMEMENU_RECENT_TXT_X, NX_HOMEMENU_RECENT_TXT_Y, NX_HOMEMENU_RECENT_TXT_W, NX_HOMEMENU_RECENT_TXT_H);
	ONDK_GWM_COM_Text_Create(NX_HOMEMENU_RECENT_TXT_ID, &stRect, (HCHAR*)ONDK_GetString(RES_RECENT_01_ID));
	ONDK_GWM_COM_Text_SetFont(NX_HOMEMENU_RECENT_TXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_RECENT_TXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_RECENT_TXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_RECENT_TXT_ID, COL(C_T_Gong02), COL(C_T_Gong02));

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_HomeMenu_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_HomeMenu_DrawBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_370_00_H_SHELF_1L_PNG, RES_370_00_H_SHELF_2C_PNG, RES_370_00_H_SHELF_3R_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_HomeMenu_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	HINT32 				push_status, objectId;
	HINT32				img;
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	HINT32				alpha = 0;
	ONDK_Rect_t 		drawRect;
#endif

	push_status = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)object);
	objectId = ONDK_GWM_Obj_GetObjectID (object);

	switch (objectId)
	{
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_LiveTv):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_LiveTv)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_LiveTv)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_LiveTv) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_LiveTv);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Guide):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Guide)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Guide)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Guide) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Guide);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Recordings):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Recordings)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Recordings)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Recordings) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Recordings);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_MediaCenter):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_MediaCenter)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_MediaCenter)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_MediaCenter) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_MediaCenter);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Settings):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Settings)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Settings)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Settings) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Settings);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_TVApps):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_TVApps)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_TVApps)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_TVApps) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_TVApps);
			break;
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Schedule):
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Schedule)), FALSE);
			ONDK_GWM_Obj_SetVisible(((NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Schedule)), FALSE);
			img = (push_status & GUIOBJ_HILIGHTED) ? (NX_HOMEMENU_IMGC_ID|eNxHomeIdx_Schedule) : (NX_HOMEMENU_IMGN_ID|eNxHomeIdx_Schedule);
			break;

		default:
			img = 0;
			NX_APP_Print("nx_HomeMenu_DrawButton() Not used objectId(0x%x)\n", objectId);
			break;
	}

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	if (push_status & GUIOBJ_HILIGHTED)
	{
		lRet = ONDK_GWM_O2ANI_GetValue (objectId, &drawRect, &alpha);
		if(ONDK_RET_OK != lRet)
		{
			ONDK_GWM_COM_Image_GetOrgRect(img, &drawRect);
			alpha = 0xFF;
			lRet = ONDK_RET_OK;
		}
		ONDK_GWM_COM_Image_ChangeAlpha((img), alpha);
		ONDK_GWM_COM_Image_ChangeRect((img), &drawRect);
	}
#endif
	ONDK_GWM_Obj_SetVisible((img), TRUE);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	lRet;
}


static ONDK_Result_t	nx_HomeMenu_UpdateTitle(NxHome_Contents_t *pstContents, HUINT32 ulCurFocus)
{
	HUINT32			ulObjId = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, ulCurFocus ,&ulObjId);
	if (eResult == ONDK_RET_OK)
	{
		pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId(pstContents, ulObjId);
		if (pstSubItem != NULL)
		{
			HxSTD_sprintf ((char *)pstContents->szMainItemText, "%s", pstSubItem->szMainItemText);
		}
	}
	else
	{
		return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_HomeMenu_GetStartPosForHomeMenuCursor(NxHome_Contents_t *pstContents)
{
	HINT32		lStartX = 0, lPosition = 0;


	if (pstContents->ulMaxHomeMenu < NX_HOMEMENU_COUNT6)
	{
		lStartX = (NX_HOMEMENU_FRAME_W - (NX_HOMEMENU_ICON_BOX_ITEM_W * pstContents->ulMaxHomeMenu))/2;
	}
	else
	{
		lStartX = NX_HOMEMENU_ICON_BOX6_X;
	}

	if (pstContents->ulNumOfSubItem == 0)
	{
		lPosition = lStartX;
		return	lPosition;
	}
	else
	{
		lPosition = (NX_HOMEMENU_ICON_BOX_W * pstContents->ulNumOfSubItem) + (NX_HOMEMENU_ICON_BOX_MG * pstContents->ulNumOfSubItem) + lStartX;
		return	lPosition;
	}
}


static ONDK_Result_t	nx_HomeMenu_LaunchSubMenu(NxHome_Contents_t *pstContents, HUINT32 ulObjId)
{
	HINT32			lActionMenu = eNX_UCT_None;
	HUINT32			ulProperty = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;
	Nx_SettingsLeafSubItem_t	stSubItem = {0, };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId(pstContents, ulObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_memcpy(&stSubItem, pstSubItem, sizeof(Nx_SettingsLeafSubItem_t));
	if (stSubItem.childAppToLaunch != NULL)
	{
		switch (ulObjId)
		{
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Settings):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				lActionMenu = eNX_UCT_Menu;
				break;
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Guide):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_EPG);
				lActionMenu = eNX_UCT_Guide;
				break;
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Recordings):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_RECORDINGS);
				lActionMenu = eNX_UCT_Record;
				break;
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_MediaCenter):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MEDIA);
				lActionMenu = eNX_UCT_Meida;
				break;
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Schedule):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_RECORDINGS);
				lActionMenu = eNX_UCT_Schedule;
				break;
			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				lActionMenu = eNX_UCT_None;
				break;
		}
		if (ulObjId != (NX_HOMEMENU_MENU_ID|eNxHomeIdx_MediaCenter))
		{
			ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
		}
		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stSubItem.szAppName, ulProperty, stSubItem.childAppToLaunch,
						stSubItem.hAction, stSubItem.ulappParam1, stSubItem.ulappParam2, stSubItem.ulappParam3);
	}
	else
	{
		switch (ulObjId)
		{
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_LiveTv):
				lActionMenu = eNX_UCT_Live;
				break;
			default:
				break;
		}
		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_Destroy(NxHome_Contents_t *pstContents)
{
	HUINT32			i = 0;
	HUINT32			ulNumofSubItems = 0;
	Nx_SettingsLeafSubItem_t *pstSubItem = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// store a last cursor position
	s_LastCursorIndex = pstContents->ulCurFocus;

	ulNumofSubItems = NX_PRISM_AppContents_CountSubitem(pstContents);

	for(i = 0; i < ulNumofSubItems; i++)
	{
		pstSubItem = NX_PRISM_AppContents_GetSubitem(pstContents, i);
		if (pstSubItem != NULL)
		{
			pstSubItem->szItemTextList = NULL;
			pstSubItem->ulNumOfItemTextList = 0;
		}
	}

	NX_PRISM_AppContents_FreeSubitemArray((void *)pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmCreate(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	nx_HomeMenu_Initialize(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmDestroy(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	nx_HomeMenu_Destroy(pstContents);
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmHapiMsg(NxHome_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmObjFocusChanged(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8			*szStr = NULL;
	HUINT32			ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	ONDK_GWM_GuiObject_t	stTextObject;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	eResult = NX_PRISM_AppContents_GetSubitemIndexByObjId(pstContents, p1, &ulIndex);
	if (eResult == ONDK_RET_OK)
	{
		pstContents->ulCurFocus = ulIndex;
		nx_HomeMenu_UpdateTitle(pstContents, pstContents->ulCurFocus);
	}

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId(pstContents, p1);
	if (pstSubItem != NULL)
	{
		stTextObject = ONDK_GWM_APP_GetObject(NX_HOMEMENU_TITLE_ID);
		if (stTextObject)
		{
			szStr = (HUINT8*)ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stTextObject);
			if (szStr != NULL)
			{
				HxSTD_sprintf ((char*)szStr, (char*)"%s", (HCHAR*)pstSubItem->szMainItemText);
				ONDK_GWM_COM_Text_SetText (NX_HOMEMENU_TITLE_ID, (HCHAR*)szStr);
			}
		}
	}

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	{
		ONDK_GWM_O2Ani_Settings_t stO2AniSettings;
		HINT32 			lObjId;
		ONDK_Rect_t 	rt;

		HxSTD_MemSet(&stO2AniSettings, 0, sizeof(ONDK_GWM_O2Ani_Settings_t));

		lObjId = p1;
		ONDK_GWM_Obj_GetObjectRect(lObjId, &rt);
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &rt);
		stO2AniSettings.nSpeed = 7;
		stO2AniSettings.rtEnd.x -= 25;
		stO2AniSettings.rtEnd.y -= 15;
		stO2AniSettings.rtEnd.w += 50;
		stO2AniSettings.rtEnd.h += 50;
		ONDK_GWM_O2ANI_AddItem(lObjId, &stO2AniSettings, 0xFF);
	}
#endif

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyUp(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyDown(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	HINT32			lObjId = 0;
#endif

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);
	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_MENU:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
			{
				ONDK_Rect_t		rt;
				ONDK_Result_t	lResult = ONDK_RET_OK;
				ONDK_GWM_O2Ani_Settings_t stO2AniSettings;

				lObjId = GWM_GetFocusedObjectID();
				ONDK_GWM_Obj_GetObjectRect(lObjId, &rt);

				ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &rt);
				stO2AniSettings.nSpeed = 40;
				stO2AniSettings.nStartAlpha = 100;
				stO2AniSettings.nEndAlpha = 255;
				lResult = ONDK_GWM_O2ANI_AddItem(lObjId, &stO2AniSettings, lKeyId);
				if(GWM_RET_OK == lResult)
				{
					return ONDK_MESSAGE_BREAK;
				}
			}
#endif
			return ONDK_RET_OK;

		case KEY_ARROWLEFT:
			return ONDK_RET_OK;

		case KEY_ARROWRIGHT:
			return	ONDK_RET_OK;

		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			return	ONDK_MESSAGE_PASS;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_PRISM_NotAvailable_Show();
			NX_APP_Print("nx_HomeMenu_MsgGwmKeyDown() blocking keys!!! lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_BREAK;
	}

}


static ONDK_Result_t	nx_HomeMenu_MsgGwmClick(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	nx_HomeMenu_LaunchSubMenu(pstContents, (HUINT32)p1);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmScrollLeft(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				ulObjId = 0;
	ONDK_Result_t			eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, pstContents->ulNumOfSubItem - 1 ,&ulObjId);
	if (eResult == ONDK_RET_OK)
	{
		ONDK_GWM_Obj_SetFocus(ulObjId);
		nx_HomeMenu_MsgGwmObjFocusChanged(pstContents, ulObjId, 0, 0);
	}
	else
	{
		NX_APP_Error("Error!!! (eResult == ONDK_RET_OK) \n");
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmScrollRight(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulObjId = 0;
	ONDK_Result_t			eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, 0 ,&ulObjId);
	if (eResult == ONDK_RET_OK)
	{
		ONDK_GWM_Obj_SetFocus(ulObjId);
		nx_HomeMenu_MsgGwmObjFocusChanged(pstContents, ulObjId, 0, 0);
	}
	else
	{
		NX_APP_Error("Error!!! (eResult == ONDK_RET_OK) \n");
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_HomeMenu_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_HomeMenu_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			eResult = nx_HomeMenu_MsgGwmKeyUp(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_HomeMenu_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_HomeMenu_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_HomeMenu_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_HomeMenu_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			eResult = nx_HomeMenu_MsgGwmClick(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_SCROLLLEFT:
			eResult = nx_HomeMenu_MsgGwmScrollLeft(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_HomeMenu_MsgGwmScrollRight(pstContents, p1, p2, p3);
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
