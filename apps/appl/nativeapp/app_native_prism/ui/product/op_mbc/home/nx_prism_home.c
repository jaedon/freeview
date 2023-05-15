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
#define	NX_HOMEMENU_FRAME_ID				0x00010000
#define	NX_HOMEMENU_SHAHID_TITLE_ID			0x00020000
#define	NX_HOMEMENU_ARROW_DOWN_ID			0x00021000
#define	NX_HOMEMENU_ARROW_RIGHT_ID			0x00022000
#define	NX_HOMEMENU_MENU_ID					0x00030000
#define	NX_HOMEMENU_IMGN_ID					0x00040000
#define	NX_HOMEMENU_IMGC_ID					0x00050000
#define	NX_HOMEMENU_HOME_LINE_ID			0x00060000
#define	NX_HOMEMENU_RECENT_IMG_ID			0x00070000
#define	NX_HOMEMENU_RECENT_TXT_ID			0x00080000
#define	NX_HOMEMENU_SHAHID_ID				0x00090000
#define	NX_HOMEMENU_HOME_MASK_IMG_ID		0x00100000
#define	NX_HOMEMENU_FRAME_02_ID				0x00011000
#define	NX_HOMEMENU_LOGO_ID					0x00012000
#define	NX_HOMEMENU_NETWORK_ID				0x00110000
#define	NX_HOMEMENU_NETWORK_PROMOTION_ID	0x00110001
#define	NX_HOMEMENU_NETWORK_TEXTHELP_ID		0x00110002
#define	NX_HOMEMENU_NUMBER_OF_ITEM_ID		0x00120000
#define	NX_HOMEMENU_EXPIRE_TIMER_ID			0x00130000

// Support Home menu cound
#define	NX_HOMEMENU_COUNT5				5
#define	NX_HOMEMENU_COUNT6				6

#define	NX_HOMEMENU_FRAME_X				0
#define	NX_HOMEMENU_FRAME_Y				0
#define	NX_HOMEMENU_FRAME_W				NX_PRISM_OSD_HORIZONTAL
#define	NX_HOMEMENU_FRAME_H				NX_PRISM_OSD_VERTICAL

#define	NX_HOMEMENU_FRAME_BG_X			0
#define	NX_HOMEMENU_FRAME_BG_Y			0
#define	NX_HOMEMENU_FRAME_BG_W			NX_HOMEMENU_FRAME_W
#define	NX_HOMEMENU_FRAME_BG_H			NX_HOMEMENU_FRAME_H

#define	NX_HOMEMENU_FRAME_BG_02_X		0
#define	NX_HOMEMENU_FRAME_BG_02_Y		296
#define	NX_HOMEMENU_FRAME_BG_02_W		NX_HOMEMENU_FRAME_W
#define	NX_HOMEMENU_FRAME_BG_02_H		424

#define NX_HOMEMENU_LOGO_X					57
#define	NX_HOMEMENU_LOGO_Y					50
#define	NX_HOMEMENU_LOGO_W					107
#define NX_HOMEMENU_LOGO_H					44

#define NX_HOMEMENU_CURSOR_TEXT_Y			9

#define	NX_HOMEMENU_SHAHID_TITLE_X			95
#define	NX_HOMEMENU_SHAHID_TITLE_Y			555
#define	NX_HOMEMENU_SHAHID_TITLE_FOCUS_Y	466
#define	NX_HOMEMENU_SHAHID_TITLE_W			500
#define	NX_HOMEMENU_SHAHID_TITLE_H			18
#define	NX_HOMEMENU_SHAHID_TITLE_FOCUS_H	24

#define	NX_HOMEMENU_NUMBER_OF_ITEM_Y		472
#define	NX_HOMEMENU_NUMBER_OF_ITEM_W		100
#define	NX_HOMEMENU_NUMBER_OF_ITEM_H		18

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

#define	NX_HOMEMENU_HOME_LINE_X			90
#define	NX_HOMEMENU_HOME_LINE_Y			477
#define	NX_HOMEMENU_HOME_LINE_W			1100
#define	NX_HOMEMENU_HOME_LINE_H			65

#define	NX_HOMEMENU_HOME_MASK_IMG_X		0
#define	NX_HOMEMENU_HOME_MASK_IMG_Y		630
#define	NX_HOMEMENU_HOME_MASK_IMG_W		1280
#define	NX_HOMEMENU_HOME_MASK_IMG_H		90


#define	NX_HOMEMENU_RECENT_IMG_X		(NX_HOMEMENU_ICON_BOX5_X + 400)
#define	NX_HOMEMENU_RECENT_IMG_Y		653
#define	NX_HOMEMENU_RECENT_IMG_W		30
#define	NX_HOMEMENU_RECENT_IMG_H		26

#define	NX_HOMEMENU_RECENT_TXT_X_TAB	10
#define	NX_HOMEMENU_RECENT_TXT_X		(NX_HOMEMENU_RECENT_IMG_X + NX_HOMEMENU_RECENT_IMG_W + NX_HOMEMENU_RECENT_TXT_X_TAB)
#define	NX_HOMEMENU_RECENT_TXT_Y		653
#define	NX_HOMEMENU_RECENT_TXT_W		94
#define	NX_HOMEMENU_RECENT_TXT_H		22

#define	NX_HOMEMENU_LIVETV_X			90
#define	NX_HOMEMENU_LIVETV_Y			477
#define	NX_HOMEMENU_LIVETV_W			249
#define	NX_HOMEMENU_LIVETV_H			55

#define	NX_HOMEMENU_CURSOR_W			68

#define	NX_HOMEMENU_SHAHID_X			340
#define	NX_HOMEMENU_SHAHID_Y			477
#define	NX_HOMEMENU_SHAHID_W			299
#define	NX_HOMEMENU_SHAHID_H			55

#define	NX_HOMEMENU_TVGUIDE_X			640
#define	NX_HOMEMENU_TVGUIDE_Y			477
#define	NX_HOMEMENU_TVGUIDE_W			299
#define	NX_HOMEMENU_TVGUIDE_H			55

#define	NX_HOMEMENU_SETTINGS_X			941
#define	NX_HOMEMENU_SETTINGS_Y			477
#define	NX_HOMEMENU_SETTINGS_W			249
#define	NX_HOMEMENU_SETTINGS_H			55

#define	NX_HOMEMENU_SHAHID_ITEM_X		95
#define	NX_HOMEMENU_SHAHID_ITEM_Y		588
#define	NX_HOMEMENU_SHAHID_ITEM_FOCUS_Y	507
#define	NX_HOMEMENU_SHAHID_ITEM_W		206
#define	NX_HOMEMENU_SHAHID_ITEM_H		150

#define	NX_HOMEMENU_SHAHID_ITEM_THUMBNAIL_H		105
#define	NX_HOMEMENU_SHAHID_ITEM_TEXT_X			10
#define	NX_HOMEMENU_SHAHID_ITEM_TEXT_Y			7
#define	NX_HOMEMENU_SHAHID_ITEM_TEXT_W			186

#define	NX_HOMEMENU_SHAHID_ITEM_PLUS_W	36
#define	NX_HOMEMENU_SHAHID_ITEM_PLUS_H	36

#define	NX_HOMEMENU_SHAHID_ITEM_RANK_W	54
#define	NX_HOMEMENU_SHAHID_ITEM_RANK_H	35


#define	NX_HOMEMENU_SHAHID_ITEM_PERCENT_W	9
#define	NX_HOMEMENU_SHAHID_ITEM_PERCENT_H	8

#define	NX_HOMEMENU_SHAHID_ITEM_COUNT					5
#define	NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT			10

#define	NX_HOMEMENU_NETWORK_X			95
#define	NX_HOMEMENU_NETWORK_Y			580
#define	NX_HOMEMENU_NETWORK_W			360
#define	NX_HOMEMENU_NETWORK_H			55

#define	NX_HOMEMENU_NETWORK_ITEM_IMAGE_X	42
#define	NX_HOMEMENU_NETWORK_ITEM_IMAGE_Y	12
#define	NX_HOMEMENU_NETWORK_ITEM_IMAGE_W	36
#define	NX_HOMEMENU_NETWORK_ITEM_IMAGE_H	35

#define	NX_HOMEMENU_NETWORK_ITEM_TEXT_X		90
#define	NX_HOMEMENU_NETWORK_ITEM_TEXT_Y		17
#define	NX_HOMEMENU_NETWORK_ITEM_TEXT_W		500

#define	NX_HOMEMENU_NETWORK_PROMOTION_X		653
#define	NX_HOMEMENU_NETWORK_PROMOTION_Y		556
#define	NX_HOMEMENU_NETWORK_PROMOTION_W		560
#define	NX_HOMEMENU_NETWORK_PROMOTION_H		124

#define	NX_HOMEMENU_NETWORK_TEXTHELP_X		95
#define	NX_HOMEMENU_NETWORK_TEXTHELP_Y		650
#define	NX_HOMEMENU_NETWORK_TEXTHELP_W		1100
#define	NX_HOMEMENU_NETWORK_TEXTHELP_H		25

#define NX_HOMEMENU_ARROW_DOWN_X		630
#define NX_HOMEMENU_ARROW_DOWN_Y		668
#define NX_HOMEMENU_ARROW_DOWN_W		20
#define NX_HOMEMENU_ARROW_DOWN_H		13

#define NX_HOMEMENU_ARROW_RIGHT_X		1196
#define NX_HOMEMENU_ARROW_RIGHT_Y		572
#define NX_HOMEMENU_ARROW_RIGHT_W		13
#define NX_HOMEMENU_ARROW_RIGHT_H		20

#define	NX_HOMEMENU_EXPIRE_TIME			(3 * NX_COMMON_TIME_SECOND) // 3 seconds

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
	eNxHomeIdx_Shahid,
	eNxHomeIdx_Max
} NxHomeIdx_t;

typedef enum {
	eNxHomeIdx_FOCUS_NO_ITEM	= 0,
	eNxHomeIdx_FOCUS_HOMEMENU,
	eNxHomeIdx_FOCUS_SHAHID,
	eNxHomeIdx_FOCUS_CONTENTS
} NxHome_FocusItem_e;

typedef enum
{
	eNxHome_ConnectionStatus_Status_Init,
	eNxHome_ConnectionStatus_Status_Checking,
	eNxHome_ConnectionStatus_Status_DONE,
	eNxHome_ConnectionStatus_Status_RETRY,
	eNxHome_ConnectionStatus_Status_MAX
} Nx_Home_ConnectionStatusUiStatus_e;


typedef struct NxHome_Shahid_Context
{
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*pstSubItems;

	HUINT32		ulSelectedItem;
	HUINT8		szMainItemText[MAX_ELEMENT_TEXT_LENGTH];

	HUINT32		ulMaxShahidContents;
} NxHome_Shahid_Contents_t;


typedef struct NxHome_Context
{
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*pstSubItems;

	HUINT32		ulSelectedItem;
	HUINT8		szMainItemText[MAX_ELEMENT_TEXT_LENGTH];

	HUINT32		ulMaxHomeMenu;

	//Shahid contents
	NxHome_Shahid_Contents_t stShahid_Contents;

	//Focus item
	NxHome_FocusItem_e	eFocusItem;

	HUINT32		ulShahidCurFocus;
	HINT32		ulShahid_StartIdx;

	//handle for TV and Radio
	ONDKImageHandle hTvImg;

	//Network is disconnected
	HBOOL		isNetWorkDisConnected;

	//Network Information
	NX_NET_NetIfType_e eCurrentIfType;
	HBOOL   bEthernet;
	HBOOL   bInternet;
	NX_NET_NetIFADDR_t *pstNetIf;
	NX_NET_NetDevInfo_t *pstNetDevInfoList;
	Nx_Home_ConnectionStatusUiStatus_e eUiStatus;

} NxHome_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxHome_Contents_t	s_stNxHome_Contents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_HomeMenu_MsgGwmCreate(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_HomeMenu_MsgGwmDestroy(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmHapiMsg(NxHome_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_HomeMenu_MsgGwmObjFocusChanged(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_ConnectionStatus_MsgGwmAPKMessage(NxHome_Contents_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyDown(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmTimer(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_HomeMenu_MsgGwmClick(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static NxHome_Contents_t*	nx_HomeMenu_GetContents(void);
static ONDK_Result_t	nx_HomeMenu_ResetExitHomeMenuTimer(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_Initialize(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemLiveTv(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemShahid(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemGuide(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemSchedule(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemRecord(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemMediaCenter(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemTvApps(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemSettings(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_Shahid_AddItem(NxHome_Contents_t *pstContents, HUINT32 ulIndexId, HUINT32 ulIndexPos);
static ONDK_Result_t	nx_HomeMenu_Shahid_UpdateItem(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_UpdateItem(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemFrame(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddNetWorkDisConnected(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddMaskImage(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddArrowDown(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddArrowRight(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_DeleteMaskImage(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_DeleteArrowDown(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_DeleteArrowRight(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_Shahid_AddItemTitle(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddNumberOfItem(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_AddItemRecent(NxHome_Contents_t *pstContents);

static HBOOL			nx_HomeMenu_CheckFocusItem(NxHome_Contents_t *pstContents, HUINT32 objId);
static HBOOL			nx_HomeMenu_CheckLastItem(NxHome_Contents_t *pstContents, HUINT32 objId);
static HBOOL			nx_HomeMenu_CheckFirstItem(NxHome_Contents_t *pstContents, HUINT32 objId);

static ONDK_Result_t 	nx_HomeMenu_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_DrawFrame02(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_HomeMenu_DrawBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_DrawMask(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_DrawCursorA(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus);
static	ONDK_Result_t	nx_HomeMenu_DrawCursorB(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus);
static	ONDK_Result_t	nx_HomeMenu_DrawCursorC(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus);
static ONDK_Result_t 	nx_HomeMenu_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_Shahid_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_NetWork_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static	ONDK_Result_t	nx_HomeMenu_NetWork_DrawPromotion(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_HomeMenu_UpdateNumOfItem(NxHome_Contents_t *pstContents, HUINT32 ulObjId,HUINT32 ulIndex, HUINT32 ulTotal);
static HINT32			nx_HomeMenu_GetStartPosForHomeMenuCursor(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_LaunchSubMenu(NxHome_Contents_t *pstContents, HUINT32 ulObjId);
static ONDK_Result_t	nx_HomeMenu_DoUpdatingNetworkStatus(NxHome_Contents_t *pstContents);
static ONDK_Result_t	nx_HomeMenu_DoUpdatingUIStatus(NxHome_Contents_t *pstContents);
static ONDK_Result_t 	nx_HomeMenu_Destroy(NxHome_Contents_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxHome_Contents_t* nx_HomeMenu_GetContents(void)
{
	return &s_stNxHome_Contents;
}

static ONDK_Result_t	nx_HomeMenu_ResetExitHomeMenuTimer(NxHome_Contents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_HOMEMENU_EXPIRE_TIMER_ID, NX_HOMEMENU_EXPIRE_TIME);
	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_HOMEMENU_EXPIRE_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}

static ONDK_Result_t	nx_HomeMenu_Initialize(NxHome_Contents_t *pstContents)
{
	HUINT32			ulObjId = 0;
	ONDK_Result_t	eResult = ONDK_RET_OK;
	NX_NET_NetIfType_e	netIfType = eNxNet_INTERFACE_TYPE_MAX;

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
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	pstContents->eFocusItem = eNxHomeIdx_FOCUS_HOMEMENU;

	//Init the handle for TV and Radio
	pstContents->hTvImg = ONDK_IMG_CreateSelf(RES_212_00_THUMB_TV_PNG);

	// Get Max count of Home Menu Button
	NX_PRISM_SPEC_GetHomeMenuUiSupportNum(&pstContents->ulMaxHomeMenu);

	eResult = nx_HomeMenu_AddItemFrame(pstContents);
	//eResult = nx_HomeMenu_Shahid_AddItemTitle(pstContents);

	eResult = nx_HomeMenu_AddItemLiveTv(pstContents);
	eResult = nx_HomeMenu_AddItemShahid(pstContents);
	eResult = nx_HomeMenu_AddItemGuide(pstContents);
	//eResult = nx_HomeMenu_AddItemRecord(pstContents);
	//eResult = nx_HomeMenu_AddItemMediaCenter(pstContents);
	//eResult = nx_HomeMenu_AddItemTvApps(pstContents);
	//eResult = nx_HomeMenu_AddItemSchedule(pstContents);
	eResult = nx_HomeMenu_AddItemSettings(pstContents);

	//eResult = nx_HomeMenu_AddItemRecent(pstContents);

	// Update Title Name
	pstContents->ulCurFocus = 0;
	//nx_HomeMenu_UpdateTitle(pstContents, pstContents->ulCurFocus);
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

	//Check network
	NX_NETWORK_RegisterNotifier();
	nx_HomeMenu_DoUpdatingNetworkStatus(pstContents);

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
	}

	nx_HomeMenu_ResetExitHomeMenuTimer(pstContents);

	return	eResult;
}


static ONDK_Result_t	nx_HomeMenu_AddItemLiveTv(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
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

	stRect = ONDK_Rect(NX_HOMEMENU_LIVETV_X, NX_HOMEMENU_LIVETV_Y, NX_HOMEMENU_LIVETV_W, NX_HOMEMENU_LIVETV_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	stSubItem.childAppToLaunch = NULL;

	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddItemShahid(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUtil_HomeMenu_Shahid);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxUtil_HomeMenu_Shahid(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Set Main String
	pMainStr = ONDK_GetString(RES_SHAHID_02_ID);
	if(pMainStr != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", pMainStr);
	}

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Shahid);

	stRect = ONDK_Rect(NX_HOMEMENU_SHAHID_X, NX_HOMEMENU_SHAHID_Y, NX_HOMEMENU_SHAHID_W, NX_HOMEMENU_SHAHID_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	stSubItem.childAppToLaunch = NX_PRISM_SHAHID_Proc;

	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddItemGuide(NxHome_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HUINT8				*pMainStr = NULL;
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
	stRect = ONDK_Rect(NX_HOMEMENU_TVGUIDE_X, NX_HOMEMENU_TVGUIDE_Y, NX_HOMEMENU_TVGUIDE_W, NX_HOMEMENU_TVGUIDE_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);

	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_GUIDE_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_GUIDE_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

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
#if defined(CONFIG_MW_MM_MEDIA) && !defined(CONFIG_PROD_M1)
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
	stRect = ONDK_Rect(NX_HOMEMENU_SETTINGS_X, NX_HOMEMENU_SETTINGS_Y, NX_HOMEMENU_SETTINGS_W, NX_HOMEMENU_SETTINGS_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	HxSTD_StrNCpy ((char*)stSubItem.szAppName, (char*)"NX_PRISM_MENU_Settings_Proc", (MAX_ELEMENT_TEXT_LENGTH - 1));
	stSubItem.childAppToLaunch = NX_PRISM_MENU_Settings_Proc;
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	// Create OBJ
	ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*)" ");
	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_DrawButton);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_Shahid_AddItem(NxHome_Contents_t *pstContents,HUINT32 ulIndexId, HUINT32 ulIndexPos)
{
	HINT32				lPositionX = 0, lPositionY = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(&stSubItem, 0, sizeof(Nx_SettingsLeafSubItem_t));

	// Add Object Item Info
	stSubItem.ulId = (NX_HOMEMENU_SHAHID_ID|ulIndexId);

	lPositionX = NX_HOMEMENU_SHAHID_ITEM_X;

	if(ulIndexPos> 0)
		lPositionX += (NX_HOMEMENU_SHAHID_ITEM_W + 15) * ulIndexPos;

	lPositionY = NX_HOMEMENU_SHAHID_ITEM_Y;

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID || pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
		lPositionY = NX_HOMEMENU_SHAHID_ITEM_FOCUS_Y;

	stRect = ONDK_Rect(lPositionX, lPositionY, NX_HOMEMENU_SHAHID_ITEM_W, NX_HOMEMENU_SHAHID_ITEM_H);
	ONDK_CopyRect(&stSubItem.rect, &stRect);
	stSubItem.childAppToLaunch = NX_PRISM_SHAHID_Proc;

	NX_PRISM_AppContents_AppendSubitem (&pstContents->stShahid_Contents, &stSubItem);

	// Create OBJ
	if(ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*) "") != ONDK_RET_OK)
	{
		//ONDK_GWM_Obj_Resize(stSubItem.ulId, &stRect);
		ONDK_GWM_Obj_Destroy(stSubItem.ulId);
		ONDK_GWM_COM_Button_Create(stSubItem.ulId, &stRect, (char*) "");
	}

	ONDK_GWM_APP_SetObjectDrawMethod(stSubItem.ulId, (GWM_ObjDraw_t)nx_HomeMenu_Shahid_DrawButton);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_Shahid_UpdateItem(NxHome_Contents_t *pstContents)
{
	HUINT32		    ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_OK;
	Nx_SettingsLeafSubItem_t *pstSubItem = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	for(ulIndex = 0; ulIndex < NX_HOMEMENU_SHAHID_ITEM_COUNT; ulIndex++)
	{
		pstSubItem = NX_PRISM_AppContents_GetSubitem(&pstContents->stShahid_Contents, ulIndex);
		if (pstSubItem != NULL)
		{
			pstSubItem->szItemTextList = NULL;
			pstSubItem->ulNumOfItemTextList = 0;
		}
	}
	NX_PRISM_AppContents_FreeSubitemArray(&pstContents->stShahid_Contents);

	eResult = NX_PRISM_AppContents_InitSubitemArray(&pstContents->stShahid_Contents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	//Draw Shahid Title
	nx_HomeMenu_Shahid_AddItemTitle(pstContents);

	//Draw Number Of Item
	nx_HomeMenu_AddNumberOfItem(pstContents);

	for(ulIndex = 0; ulIndex < NX_HOMEMENU_SHAHID_ITEM_COUNT; ulIndex++)
	{
		nx_HomeMenu_Shahid_AddItem(pstContents, ulIndex, ulIndex);
	}

	//Draw Mask Image
	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		nx_HomeMenu_AddMaskImage(pstContents);
	}

	else
	{
		nx_HomeMenu_DeleteMaskImage(pstContents);
	}

	//Display Arrow Down
	#if 0
	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID)
	{
		nx_HomeMenu_AddArrowDown(pstContents);
	}
	else
	{
		nx_HomeMenu_DeleteArrowDown(pstContents);
	}
	#endif
	//Display Arrow Left
	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		nx_HomeMenu_DeleteArrowRight(pstContents);
	}
	else
	{
		nx_HomeMenu_AddArrowRight(pstContents);
	}

	return	eResult;
}

static ONDK_Result_t	nx_HomeMenu_UpdateItem(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	HUINT32				ulObjId = 0;
	HUINT32				lPositionY = 0;

	lPositionY = NX_HOMEMENU_LIVETV_Y;

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
	{
		lPositionY -= 40;
	}

	//Re-draw home line
	stRect = ONDK_Rect(NX_HOMEMENU_HOME_LINE_X, lPositionY, NX_HOMEMENU_HOME_LINE_W, NX_HOMEMENU_HOME_LINE_H);
	ulObjId = (NX_HOMEMENU_HOME_LINE_ID);
	ONDK_GWM_Obj_Resize(ulObjId, &stRect);

	//re-draw Home Menu
	stRect = ONDK_Rect(NX_HOMEMENU_LIVETV_X, lPositionY, NX_HOMEMENU_LIVETV_W, NX_HOMEMENU_LIVETV_H);
	ulObjId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_LiveTv);
	ONDK_GWM_Obj_Resize(ulObjId, &stRect);

	stRect = ONDK_Rect(NX_HOMEMENU_SHAHID_X, lPositionY, NX_HOMEMENU_SHAHID_W, NX_HOMEMENU_SHAHID_H);
	ulObjId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Shahid);
	ONDK_GWM_Obj_Resize(ulObjId, &stRect);

	stRect = ONDK_Rect(NX_HOMEMENU_TVGUIDE_X, lPositionY, NX_HOMEMENU_TVGUIDE_W, NX_HOMEMENU_TVGUIDE_H);
	ulObjId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Guide);
	ONDK_GWM_Obj_Resize(ulObjId, &stRect);

	stRect = ONDK_Rect(NX_HOMEMENU_SETTINGS_X, lPositionY, NX_HOMEMENU_SETTINGS_W, NX_HOMEMENU_SETTINGS_H);
	ulObjId = (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Settings);
	ONDK_GWM_Obj_Resize(ulObjId, &stRect);

	// Draw - Background 02 Frame
	stRect = ONDK_Rect(NX_HOMEMENU_FRAME_BG_02_X, NX_HOMEMENU_FRAME_BG_02_Y, NX_HOMEMENU_FRAME_BG_02_W, NX_HOMEMENU_FRAME_BG_02_H);
	if(ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_FRAME_02_ID, &stRect, 0) != ONDK_RET_OK)
	{
		ONDK_GWM_Obj_Destroy(NX_HOMEMENU_FRAME_02_ID);
	}
	else
	{
		ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_FRAME_02_ID, nx_HomeMenu_DrawFrame02);
	}

	return ONDK_RET_OK;

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
	ONDK_GWM_APP_SetAppArea(NX_HOMEMENU_FRAME_X, NX_HOMEMENU_FRAME_Y, NX_HOMEMENU_FRAME_W, NX_HOMEMENU_FRAME_BG_02_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_HOMEMENU_FRAME_BG_X, NX_HOMEMENU_FRAME_BG_Y, NX_HOMEMENU_FRAME_BG_W, NX_HOMEMENU_FRAME_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_FRAME_ID, nx_HomeMenu_DrawFrame);

	//Draw Logo
	stRect = ONDK_Rect(NX_HOMEMENU_LOGO_X, NX_HOMEMENU_LOGO_Y, NX_HOMEMENU_LOGO_W, NX_HOMEMENU_LOGO_H);
	ONDK_GWM_COM_Image_Create(NX_HOMEMENU_LOGO_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_HOMEMENU_LOGO_ID, (HCHAR*)RES_600_11_LOGO01_PNG);

	// Draw - Home Line
	stRect = ONDK_Rect(NX_HOMEMENU_HOME_LINE_X, NX_HOMEMENU_HOME_LINE_Y, NX_HOMEMENU_HOME_LINE_W, NX_HOMEMENU_HOME_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_HOME_LINE_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_HOME_LINE_ID, nx_HomeMenu_DrawBox);


	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddNetWorkDisConnected(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - NetWork Setting Button
	stRect = ONDK_Rect(NX_HOMEMENU_NETWORK_X, NX_HOMEMENU_NETWORK_Y, NX_HOMEMENU_NETWORK_W, NX_HOMEMENU_NETWORK_H);

	if(ONDK_GWM_COM_Button_Create(NX_HOMEMENU_NETWORK_ID, &stRect, (char*) "") != ONDK_RET_OK)
	{
		//ONDK_GWM_Obj_Resize(stSubItem.ulId, &stRect);
		ONDK_GWM_Obj_Destroy(NX_HOMEMENU_NETWORK_ID);
		ONDK_GWM_COM_Button_Create(NX_HOMEMENU_NETWORK_ID, &stRect, (char*) "");
	}
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_NETWORK_ID, (GWM_ObjDraw_t)nx_HomeMenu_NetWork_DrawButton);

	stRect = ONDK_Rect(NX_HOMEMENU_NETWORK_PROMOTION_X, NX_HOMEMENU_NETWORK_PROMOTION_Y, NX_HOMEMENU_NETWORK_PROMOTION_W, NX_HOMEMENU_NETWORK_PROMOTION_H);
	ONDK_GWM_COM_Image_Create(NX_HOMEMENU_NETWORK_PROMOTION_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_HOMEMENU_NETWORK_PROMOTION_ID, (HCHAR*)RES_600_11_NETWORK_PROMOTION01_PNG);

	// Draw - Text Help
	stRect = ONDK_Rect(NX_HOMEMENU_NETWORK_TEXTHELP_X, NX_HOMEMENU_NETWORK_TEXTHELP_Y, NX_HOMEMENU_NETWORK_TEXTHELP_W, NX_HOMEMENU_NETWORK_TEXTHELP_H);

	if(ONDK_GWM_COM_Text_Create(NX_HOMEMENU_NETWORK_TEXTHELP_ID, &stRect,(HCHAR*)ONDK_GetString(RES_MESG_6100_ID)) != ONDK_RET_OK)
	{
		ONDK_GWM_Obj_Destroy(NX_HOMEMENU_NETWORK_TEXTHELP_ID);
		ONDK_GWM_COM_Text_Create(NX_HOMEMENU_NETWORK_TEXTHELP_ID, &stRect, (HCHAR*)ONDK_GetString(RES_MESG_6100_ID));
	}

	ONDK_GWM_COM_Text_SetFont(NX_HOMEMENU_NETWORK_TEXTHELP_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_NETWORK_TEXTHELP_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_NETWORK_TEXTHELP_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_NETWORK_TEXTHELP_ID, COL(C_T_Gong03), COL(C_T_Bang_80));

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddMaskImage(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - Mask Image
	stRect = ONDK_Rect(NX_HOMEMENU_HOME_MASK_IMG_X, NX_HOMEMENU_HOME_MASK_IMG_Y, NX_HOMEMENU_HOME_MASK_IMG_W, NX_HOMEMENU_HOME_MASK_IMG_H);

	ONDK_GWM_COM_Rect_Create(NX_HOMEMENU_HOME_MASK_IMG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_HOMEMENU_HOME_MASK_IMG_ID, nx_HomeMenu_DrawMask);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddArrowDown(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - NetWork Setting Button
	stRect = ONDK_Rect(NX_HOMEMENU_ARROW_DOWN_X, NX_HOMEMENU_ARROW_DOWN_Y, NX_HOMEMENU_ARROW_DOWN_W, NX_HOMEMENU_ARROW_DOWN_H);

	ONDK_GWM_COM_Image_Create(NX_HOMEMENU_ARROW_DOWN_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_HOMEMENU_ARROW_DOWN_ID, (HCHAR*)RES_600_11_ARROW_DOWN_PNG);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddArrowRight(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - NetWork Setting Button
	stRect = ONDK_Rect(NX_HOMEMENU_ARROW_RIGHT_X, NX_HOMEMENU_ARROW_RIGHT_Y, NX_HOMEMENU_ARROW_RIGHT_W, NX_HOMEMENU_ARROW_RIGHT_H);

	ONDK_GWM_COM_Image_Create(NX_HOMEMENU_ARROW_RIGHT_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_HOMEMENU_ARROW_RIGHT_ID, (HCHAR*)RES_600_11_ARROW_RIGHT_PNG);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_DeleteMaskImage(NxHome_Contents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ONDK_GWM_Obj_Destroy(NX_HOMEMENU_HOME_MASK_IMG_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_DeleteArrowDown(NxHome_Contents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ONDK_GWM_Obj_Destroy(NX_HOMEMENU_ARROW_DOWN_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_DeleteArrowRight(NxHome_Contents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ONDK_GWM_Obj_Destroy(NX_HOMEMENU_ARROW_RIGHT_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_Shahid_AddItemTitle(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32 		lPositionY = 0, lHeight = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lPositionY = NX_HOMEMENU_SHAHID_TITLE_Y;
	lHeight = NX_HOMEMENU_SHAHID_TITLE_H;
	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
	{
		lPositionY = NX_HOMEMENU_SHAHID_TITLE_FOCUS_Y;
		lHeight = NX_HOMEMENU_SHAHID_TITLE_FOCUS_H;
	}
	// Draw - Title
	stRect = ONDK_Rect(NX_HOMEMENU_SHAHID_TITLE_X, lPositionY, NX_HOMEMENU_SHAHID_TITLE_W, lHeight);

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_CONTENTS)
	{
		if(ONDK_GWM_COM_Text_Create(NX_HOMEMENU_SHAHID_TITLE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SHAHID_FEATURED_ID)) != ONDK_RET_OK)
		{
			ONDK_GWM_Obj_Destroy(NX_HOMEMENU_SHAHID_TITLE_ID);
			ONDK_GWM_COM_Text_Create(NX_HOMEMENU_SHAHID_TITLE_ID, &stRect,(HCHAR*)ONDK_GetString(RES_SHAHID_FEATURED_ID));
		}
	}
	else
	{
		if(ONDK_GWM_COM_Text_Create(NX_HOMEMENU_SHAHID_TITLE_ID, &stRect, (HCHAR*)ONDK_GetString(RES_TOP_CHANNELS_ID)) != ONDK_RET_OK)
		{
			ONDK_GWM_Obj_Destroy(NX_HOMEMENU_SHAHID_TITLE_ID);
			ONDK_GWM_COM_Text_Create(NX_HOMEMENU_SHAHID_TITLE_ID, &stRect,(HCHAR*)ONDK_GetString(RES_TOP_CHANNELS_ID));
		}
	}

	ONDK_GWM_COM_Text_SetFont(NX_HOMEMENU_SHAHID_TITLE_ID, eFont_SystemBold);

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_SHAHID_TITLE_ID, NX_PRISM_FONT_SIZE_18);
		ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_SHAHID_TITLE_ID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_SHAHID_TITLE_ID, COL(C_T_Gong06), COL(C_T_Bang_80));
	}
	else
	{
		ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_SHAHID_TITLE_ID, NX_PRISM_FONT_SIZE_24);
		ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_SHAHID_TITLE_ID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_SHAHID_TITLE_ID, COL(C_T_Gong02), COL(C_T_Bang_80));
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_AddNumberOfItem(NxHome_Contents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32 		lPositionX = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_CONTENTS)
	{
		lPositionX = NX_HOMEMENU_SHAHID_TITLE_X + ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemBold, NX_PRISM_FONT_SIZE_24, "SHAHID FEATURED") + 30;
	}
	else
	{
		lPositionX = NX_HOMEMENU_SHAHID_TITLE_X + ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemBold, NX_PRISM_FONT_SIZE_24, "TOP CHANNELS") + 30;
	}
	// Draw - Title
	stRect = ONDK_Rect(lPositionX, NX_HOMEMENU_NUMBER_OF_ITEM_Y, NX_HOMEMENU_NUMBER_OF_ITEM_W, NX_HOMEMENU_NUMBER_OF_ITEM_H);

	if(ONDK_GWM_COM_Text_Create(NX_HOMEMENU_NUMBER_OF_ITEM_ID, &stRect, "") != ONDK_RET_OK)
	{
		ONDK_GWM_Obj_Destroy(NX_HOMEMENU_NUMBER_OF_ITEM_ID);
		ONDK_GWM_COM_Text_Create(NX_HOMEMENU_NUMBER_OF_ITEM_ID, &stRect, "");
	}

	ONDK_GWM_COM_Text_SetFont(NX_HOMEMENU_NUMBER_OF_ITEM_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_HOMEMENU_NUMBER_OF_ITEM_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_HOMEMENU_NUMBER_OF_ITEM_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_HOMEMENU_NUMBER_OF_ITEM_ID, COL(C_T_Gong06), COL(C_T_Bang_80));

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

static HBOOL	nx_HomeMenu_CheckFocusItem(NxHome_Contents_t *pstContents, HUINT32 objId)
{
	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		if( objId == (NX_HOMEMENU_MENU_ID | eNxHomeIdx_LiveTv))
		{
			return TRUE;
		}

		if( objId == (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Shahid))
		{
			return TRUE;
		}

		if( objId == (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Guide))
		{
			return TRUE;
		}
		if( objId == (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Settings))
		{
			return TRUE;
		}
	}
	else if (pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID)
	{
		HUINT32 ulIndex = 0;
		for(ulIndex = pstContents->ulShahid_StartIdx; ulIndex < NX_HOMEMENU_SHAHID_ITEM_COUNT + pstContents->ulShahid_StartIdx; ulIndex++)
		{
			if( objId == (NX_HOMEMENU_SHAHID_ID | ulIndex))
			{
				return TRUE;
			}
		}

		pstContents->ulShahid_StartIdx++;
		HxLOG_Print("startIdx(%d)\n\n", pstContents->ulShahid_StartIdx);
		if((pstContents->ulShahid_StartIdx + NX_HOMEMENU_SHAHID_ITEM_COUNT) > NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT)
		{
			pstContents->ulShahid_StartIdx--;
		}
	}

	return FALSE;
}

static HBOOL	nx_HomeMenu_CheckLastItem(NxHome_Contents_t *pstContents, HUINT32 objId)
{
	HUINT32 ulLastIndexId = 0;

	ulLastIndexId = NX_HOMEMENU_SHAHID_ID | (NX_HOMEMENU_SHAHID_ITEM_COUNT - 1);
	if(ulLastIndexId == objId)
	{
		pstContents->ulShahid_StartIdx++;
		if((pstContents->ulShahid_StartIdx + NX_HOMEMENU_SHAHID_ITEM_COUNT) > NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT)
		{
			pstContents->ulShahid_StartIdx--;
		}

		return TRUE;
	}

	return FALSE;
}

static HBOOL	nx_HomeMenu_CheckFirstItem(NxHome_Contents_t *pstContents, HUINT32 objId)
{
	HUINT32 ulFirstIndexId = 0;
	ulFirstIndexId = NX_HOMEMENU_SHAHID_ID;
	if(ulFirstIndexId == objId)
	{
		pstContents->ulShahid_StartIdx--;

		if(pstContents->ulShahid_StartIdx < 0)
		{
			pstContents->ulShahid_StartIdx = 0;
		}

		return TRUE;
	}

	return FALSE;
}

static	ONDK_Result_t	nx_HomeMenu_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_601_11_HOME_BG01_PNG, RES_601_11_HOME_BG01_PNG, RES_601_11_HOME_BG01_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_HomeMenu_DrawFrame02(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_601_11_HOME_BG02_PNG, RES_601_11_HOME_BG02_PNG, RES_601_11_HOME_BG02_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_HomeMenu_DrawBox(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;
	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		ONDK_GUICOM_HBox(screen, stRect, RES_601_11_HOME_LINE_PNG, RES_601_11_HOME_LINE_PNG, RES_601_11_HOME_LINE_PNG);
	}
	else
	{
		ONDK_GUICOM_HBox(screen, stRect, RES_601_11_HOME_LINE_D_PNG, RES_601_11_HOME_LINE_D_PNG, RES_601_11_HOME_LINE_D_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_HomeMenu_DrawMask(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_HomeMenu_DrawCursorA(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 }, stRectImg = { 0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL;
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_36);
	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w - NX_HOMEMENU_CURSOR_W;
	stRect.h = rect->h;

	if(isFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Base01_100));
	}

	stRectImg.x = stRect.x + stRect.w;
	stRectImg.y = stRect.y;
	stRectImg.w = NX_HOMEMENU_CURSOR_W;
	stRectImg.h = stRect.h;

	if(isFocus == TRUE)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRectImg, RES_600_11_CURSORR1_PNG);
	}

	//Draw String
	if(isFocus == TRUE)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 14, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 195, COL(C_T_Whistle_100), ONDKSTF_LEFT);
	}
	else
	{
		if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 14, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 195, COL(C_T_Gong04_20), ONDKSTF_LEFT);
		}
		else
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 14, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 195, COL(C_T_Gong04), ONDKSTF_LEFT);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(szStr);
	NX_COMMON_NOT_USED_PARAM(isFocus);

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_HomeMenu_DrawCursorB(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 }, stRectL = { 0, 0, 0, 0 }, stRectM = { 0, 0, 0, 0 }, stRectR = { 0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL;
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_36);
	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	stRectL.x = stRect.x;
	stRectL.y = stRect.y;
	stRectL.w = NX_HOMEMENU_CURSOR_W;
	stRectL.h = stRect.h;

	if(isFocus == TRUE)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_600_11_CURSORL2_PNG);
	}

	stRectM.x = stRectL.x + NX_HOMEMENU_CURSOR_W;
	stRectM.y = stRect.y;
	stRectM.w = stRect.w - NX_HOMEMENU_CURSOR_W - NX_HOMEMENU_CURSOR_W;
	stRectM.h = stRect.h;

	if(isFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stRectM.x, stRectM.y, stRectM.w, stRectM.h, COL(C_G_Base01_100));
	}

	stRectR.x = stRectM.x + stRectM.w;
	stRectR.y = stRect.y;
	stRectR.w = NX_HOMEMENU_CURSOR_W;
	stRectR.h = stRect.h;

	if(isFocus == TRUE)
	{
		ONDK_GUICOM_IMG_Blit(screen, stRectR, RES_600_11_CURSORR1_PNG);
	}

	//Draw String
	if(isFocus == TRUE)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 40, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Whistle_100), ONDKSTF_CENTER);
	}
	else
	{
		if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 40, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Gong04_20), ONDKSTF_CENTER);
		}
		else
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 40, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Gong04), ONDKSTF_CENTER);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(szStr);
	NX_COMMON_NOT_USED_PARAM(isFocus);


	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_HomeMenu_DrawCursorC(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, HCHAR* szStr, HBOOL isFocus)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 }, stRectL = { 0, 0, 0, 0 },stRectR = { 0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL;
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_36);
	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	stRectL.x = stRect.x;
	stRectL.y = stRect.y;
	stRectL.w = NX_HOMEMENU_CURSOR_W;
	stRectL.h = stRect.h;

	if(isFocus == TRUE)
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_600_11_CURSORL1_PNG);

	stRectR.x = stRect.x + NX_HOMEMENU_CURSOR_W;
	stRectR.y = stRect.y;
	stRectR.w = stRect.w - NX_HOMEMENU_CURSOR_W;
	stRectR.h = stRect.h;

	if(isFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stRectR.x, stRectR.y, stRectR.w, stRectR.h, COL(C_G_Base01_100));
	}

	//Draw String
	if(isFocus == TRUE)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 30, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Whistle_100), ONDKSTF_RIGHT);
	}
	else
	{
		if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 30, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Gong04_20), ONDKSTF_RIGHT);
		}
		else
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, szStr, stRect.x + 30, stRect.y + NX_HOMEMENU_CURSOR_TEXT_Y, 219, COL(C_T_Gong04), ONDKSTF_RIGHT);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(szStr);
	NX_COMMON_NOT_USED_PARAM(isFocus);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_HomeMenu_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	HINT32 				push_status, objectId;

	push_status = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)object);
	objectId = ONDK_GWM_Obj_GetObjectID (object);

	switch (objectId)
	{
		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_LiveTv):
			nx_HomeMenu_DrawCursorA(screen, area, rect, object,(HCHAR*)ONDK_GetString(RES_LIVE_TV_ID), (push_status & GUIOBJ_HILIGHTED) ? TRUE : FALSE);
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Shahid):
			nx_HomeMenu_DrawCursorB(screen, area, rect, object, (HCHAR*)ONDK_GetString(RES_SHAHID_02_ID),(push_status & GUIOBJ_HILIGHTED) ? TRUE : FALSE);
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Guide):
			nx_HomeMenu_DrawCursorB(screen, area, rect,object, (HCHAR*)ONDK_GetString(RES_TV_GUIDE_01_ID),(push_status & GUIOBJ_HILIGHTED) ? TRUE : FALSE);
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Recordings):
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_MediaCenter):
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Settings):
			nx_HomeMenu_DrawCursorC(screen, area, rect,object, (HCHAR*)ONDK_GetString(RES_SETTINGS_01_ID),(push_status & GUIOBJ_HILIGHTED) ? TRUE : FALSE);
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_TVApps):
			break;

		case (NX_HOMEMENU_MENU_ID | eNxHomeIdx_Schedule):
			break;

		default:
			NX_APP_Print("nx_HomeMenu_DrawButton() Not used objectId(0x%x)\n", objectId);
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	lRet;
}

static	ONDK_Result_t	nx_HomeMenu_Shahid_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	HINT32 				push_status, objectId, ulIndex = 0;
	HCHAR				*szStr;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 }, stRectThumbnail = {0, 0, 0, 0 }, stRectPlus = {0, 0, 0, 0 };
	ONDK_Rect_t			stRectRank = { 0, 0, 0, 0 }, stRectPercent = { 0, 0, 0, 0 }, stRectText = {0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL, hFontBoldText = NULL, *hFontRank = NULL;
	HCHAR				szProgrammeName[EPG_HUMAX_NAME_LEN];
	NxHome_Contents_t	*pstContents = nx_HomeMenu_GetContents();

	push_status = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)object);
	objectId = ONDK_GWM_Obj_GetObjectID (object);

	szStr = ONDK_GWM_COM_Button_GetTitle(objectId);

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);
	hFontBoldText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_20);
	hFontRank = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_18);

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	if(!(push_status & GUIOBJ_HILIGHTED))
	{
		stRect.h -= 5;
	}

	stRectThumbnail.x = stRect.x;
	stRectThumbnail.y = stRect.y;
	stRectThumbnail.w = stRect.w;
	stRectThumbnail.h = NX_HOMEMENU_SHAHID_ITEM_THUMBNAIL_H;

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
	{
		stRectThumbnail.x += 6;
	}

	if(push_status & GUIOBJ_HILIGHTED)
	{
		ONDK_DRAW_FillRectElement(screen, stRectThumbnail.x, stRectThumbnail.y, stRectThumbnail.w, 3, COL(C_G_Base01_100));
		stRectThumbnail.y += 3;
	}

	ONDK_IMG_StretchBlit(pstContents->hTvImg, screen, stRectThumbnail);

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_CONTENTS)
	{
		//Draw Plus Image
		stRectPlus.x = stRectThumbnail.x + 4;
		stRectPlus.y = stRectThumbnail.y + 4;
		stRectPlus.w = NX_HOMEMENU_SHAHID_ITEM_PLUS_W;
		stRectPlus.h = NX_HOMEMENU_SHAHID_ITEM_PLUS_H;
		ONDK_GUICOM_IMG_Blit(screen, stRectPlus, RES_600_11_PLUS_PNG);
	}
	else
	{
		//Draw Rank Image
		stRectRank.x = stRectThumbnail.x - 6;
		stRectRank.y = stRectThumbnail.y + 3;
		stRectRank.w = NX_HOMEMENU_SHAHID_ITEM_RANK_W;
		stRectRank.h = NX_HOMEMENU_SHAHID_ITEM_RANK_H;
		ONDK_GUICOM_IMG_Blit(screen, stRectRank, RES_601_11_HOME_RATINGBG_PNG);

		HxSTD_sprintf(szProgrammeName,"%s","2.5");
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontRank, (HCHAR*)szProgrammeName, stRectRank.x + 7, stRectRank.y + 3, 50, COL(C_T_Whistle_100), ONDKSTF_LEFT);

		//Draw Percent Image
		stRectPercent.x = stRectRank.x + 7 + ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemBold, NX_PRISM_FONT_SIZE_18, szProgrammeName) + 4;
		stRectPercent.y = stRectRank.y + 7;
		stRectPercent.w = NX_HOMEMENU_SHAHID_ITEM_PERCENT_W;
		stRectPercent.h = NX_HOMEMENU_SHAHID_ITEM_PERCENT_H;
		ONDK_GUICOM_IMG_Blit(screen, stRectPercent, RES_601_11_HOME_RATING_PERCENT_PNG);
	}

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
	{
		stRectText.x = stRectThumbnail.x;
		stRectText.y = stRectThumbnail.y + stRectThumbnail.h;
		stRectText.w = stRectThumbnail.w;
		stRectText.h = stRect.h - stRectThumbnail.h;

		if(push_status & GUIOBJ_HILIGHTED)
		{
			ONDK_DRAW_FillRectElement(screen, stRectText.x, stRectText.y, stRectText.w, stRectText.h, COL(C_G_Base01_100));
		}
		else
		{
			ONDK_DRAW_FillRectElement(screen, stRectText.x, stRectText.y, stRectText.w, stRectText.h, COL(C_T_Gong08_40));
		}

		stRectText.x += NX_HOMEMENU_SHAHID_ITEM_TEXT_X;
		stRectText.y += NX_HOMEMENU_SHAHID_ITEM_TEXT_Y;
		stRectText.w  = NX_HOMEMENU_SHAHID_ITEM_TEXT_W;

		ulIndex = (NX_HOMEMENU_SHAHID_ID ^ objectId) + pstContents->ulShahid_StartIdx;
		HxSTD_sprintf(szProgrammeName,"Program %d",ulIndex);

		if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
		{
			HxSTD_sprintf(szProgrammeName,"Channel %d",ulIndex);
		}

		//Draw String
		if(push_status & GUIOBJ_HILIGHTED)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontBoldText, (HCHAR*)szProgrammeName, stRectText.x, stRectText.y, stRectText.w, COL(C_T_Whistle_100), ONDKSTF_LEFT);
		}
		else
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, (HCHAR*)szProgrammeName, stRectText.x, stRectText.y, stRectText.w, COL(C_T_Gong05), ONDKSTF_LEFT);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	ONDK_FONT_Release(hFontText);

	return	lRet;
}

static	ONDK_Result_t	nx_HomeMenu_NetWork_DrawButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	HINT32 				push_status, objectId;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 }, stRectImage = {0, 0, 0, 0 }, stRectText= {0, 0, 0, 0 };
	ONDKFontHandle		*hFontText = NULL;
	HCHAR				szStr[EPG_HUMAX_NAME_LEN];

	push_status = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)object);
	objectId = ONDK_GWM_Obj_GetObjectID (object);

	hFontText = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	stRectImage.x = stRect.x + NX_HOMEMENU_NETWORK_ITEM_IMAGE_X;
	stRectImage.y = stRect.y + NX_HOMEMENU_NETWORK_ITEM_IMAGE_Y;
	stRectImage.w = NX_HOMEMENU_NETWORK_ITEM_IMAGE_W;
	stRectImage.h = NX_HOMEMENU_NETWORK_ITEM_IMAGE_H;

	if(push_status & GUIOBJ_HILIGHTED)
	{
		ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Base01_100));
	}
	else
	{
		ONDK_DRAW_FillRectElement(screen, stRect.x, stRect.y, stRect.w, stRect.h, COL(C_G_Whistle_15));
	}

	ONDK_GUICOM_IMG_Blit(screen, stRectImage, RES_600_11_NETWORK_DISCONNECTED_PNG);

	stRectText.x = stRect.x + NX_HOMEMENU_NETWORK_ITEM_TEXT_X;
	stRectText.y = stRect.y + NX_HOMEMENU_NETWORK_ITEM_TEXT_Y;
	stRectText.w = NX_HOMEMENU_NETWORK_ITEM_TEXT_W;

	HxSTD_sprintf(szStr,"%s","NETWORK SETTINGS");

	//Draw String
	if(push_status & GUIOBJ_HILIGHTED)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, (HCHAR*)szStr, stRectText.x, stRectText.y, stRectText.w, COL(C_T_Whistle_100), ONDKSTF_LEFT);
	}
	else
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontText, (HCHAR*)szStr, stRectText.x, stRectText.y, stRectText.w, COL(C_T_Gong02), ONDKSTF_LEFT);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	ONDK_FONT_Release(hFontText);

	return	lRet;
}

static	ONDK_Result_t	nx_HomeMenu_NetWork_DrawPromotion(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Result_t		lRet = ONDK_RET_OK;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };


	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_600_11_NETWORK_PROMOTION01_PNG);


	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	lRet;
}

static ONDK_Result_t	nx_HomeMenu_UpdateNumOfItem(NxHome_Contents_t *pstContents, HUINT32 ulObjId,HUINT32 ulIndex, HUINT32 ulTotal)
{
	HCHAR*	szNumOfItem = NULL;//[MAX_ELEMENT_TEXT_LENGTH];

	szNumOfItem = NX_APP_Calloc(sizeof(HCHAR) * MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_sprintf((HCHAR*)szNumOfItem,(HCHAR*)"%d/%d", ulIndex, ulTotal);
	ONDK_GWM_COM_Text_SetText(ulObjId, (HCHAR*)szNumOfItem);

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
		NX_APP_Error("Error!!! (pstSubItem == NULL) \n");
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
			case (NX_HOMEMENU_MENU_ID|eNxHomeIdx_Shahid):
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_SHAHID);
				lActionMenu = eNX_UCT_Shahid;
				break;
			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				lActionMenu = eNX_UCT_None;
				break;
		}

		ONDK_GWM_APP_Destroy(0);
		HxLOG_Print("AppName (%s)\n\n", stSubItem.szAppName);
		ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
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

static ONDK_Result_t	nx_HomeMenu_DoUpdatingNetworkStatus(NxHome_Contents_t *pstContents)
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

static ONDK_Result_t	nx_HomeMenu_DoUpdatingUIStatus(NxHome_Contents_t *pstContents)
{
	ONDK_Result_t eResult = ONDK_RET_FAIL;
	HUINT32 	ulIndex = 0;

	//pstContents->bInternet = FALSE;

	if(pstContents->bInternet == FALSE)
	{
		nx_HomeMenu_AddNetWorkDisConnected(pstContents);
	}
	else
	{
		//Initialize Shahid
		pstContents->stShahid_Contents.ulMaxShahidContents = 10;

		eResult = NX_PRISM_AppContents_InitSubitemArray(&pstContents->stShahid_Contents);
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
			ONDK_GWM_APP_Destroy(0);

			return eResult;
		}

		//Draw Shahid Title
		nx_HomeMenu_Shahid_AddItemTitle(pstContents);

		pstContents->ulShahid_StartIdx = 0;
		for(ulIndex = 0; ulIndex < NX_HOMEMENU_SHAHID_ITEM_COUNT; ulIndex++)
		{
			nx_HomeMenu_Shahid_AddItem(pstContents, ulIndex, ulIndex);
		}
	}

	nx_HomeMenu_AddMaskImage(pstContents);

	return ONDK_RET_OK;
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

	for(i = 0; i < NX_HOMEMENU_SHAHID_ITEM_COUNT; i++)
	{
		pstSubItem = NX_PRISM_AppContents_GetSubitem(&pstContents->stShahid_Contents, i);
		if (pstSubItem != NULL)
		{
			pstSubItem->szItemTextList = NULL;
			pstSubItem->ulNumOfItemTextList = 0;
		}
	}

	NX_PRISM_AppContents_FreeSubitemArray(&pstContents->stShahid_Contents);

	ONDK_IMG_Destroy(pstContents->hTvImg);

	ONDK_GWM_KillTimer(NX_HOMEMENU_EXPIRE_TIMER_ID);

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
	HUINT32			ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	eResult = NX_PRISM_AppContents_GetSubitemIndexByObjId(pstContents, p1, &ulIndex);

	if(pstContents->eFocusItem != eNxHomeIdx_FOCUS_HOMEMENU)
	{
		eResult = NX_PRISM_AppContents_GetSubitemIndexByObjId(&pstContents->stShahid_Contents, p1, &ulIndex);

		if (eResult == ONDK_RET_OK)
		{
			ulIndex += pstContents->ulShahid_StartIdx;
			nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, ulIndex + 1,NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);
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

static ONDK_Result_t	nx_HomeMenu_ConnectionStatus_MsgGwmAPKMessage(NxHome_Contents_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
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
	                nx_HomeMenu_DoUpdatingUIStatus(pstContents);
	              //  nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_SUCCESS :
	                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_SUCCESS \n");
	                //DNS SUCCESS
	                pstContents->bInternet = TRUE;
	                pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_DONE;
	               	nx_HomeMenu_DoUpdatingUIStatus(pstContents);
	               // nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_FAIL :
	                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_FAIL \n");
	                pstContents->bInternet = FALSE;
	                pstContents->eUiStatus = eNxHome_ConnectionStatus_Status_DONE;
	              	 nx_HomeMenu_DoUpdatingUIStatus(pstContents);
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

static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyUp(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_HomeMenu_MsgGwmKeyDown(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lIndex = 0;
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	HINT32			lObjId = 0;
#endif

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	nx_HomeMenu_ResetExitHomeMenuTimer(pstContents);
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
			if((pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID || pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
				&& nx_HomeMenu_CheckFirstItem(pstContents,GWM_GetFocusedObjectID()) == TRUE)
			{
				nx_HomeMenu_Shahid_UpdateItem(pstContents);
				lIndex = NX_HOMEMENU_SHAHID_ID;
				ONDK_GWM_Obj_SetFocus(lIndex);
				NX_PRISM_AppContents_GetSubitemIndexByObjId(&pstContents->stShahid_Contents, lIndex, &lIndex);
				nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, lIndex + pstContents->ulShahid_StartIdx + 1, NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);
			}

			return ONDK_RET_OK;

		case KEY_ARROWRIGHT:
			if((pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID || pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
				&& nx_HomeMenu_CheckLastItem(pstContents,GWM_GetFocusedObjectID()) == TRUE)
			{
				nx_HomeMenu_Shahid_UpdateItem(pstContents);
				lIndex = NX_HOMEMENU_SHAHID_ID | (NX_HOMEMENU_SHAHID_ITEM_COUNT - 1);
				ONDK_GWM_Obj_SetFocus(lIndex);
				NX_PRISM_AppContents_GetSubitemIndexByObjId(&pstContents->stShahid_Contents, lIndex, &lIndex);;
				nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, lIndex + pstContents->ulShahid_StartIdx + 1, NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);
			}
			return	ONDK_RET_OK;
		case KEY_ARROWDOWN:
			if(pstContents->bInternet == TRUE)
			{
				if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
				{

					pstContents->eFocusItem = eNxHomeIdx_FOCUS_SHAHID;
					pstContents->ulShahid_StartIdx = 0;
					nx_HomeMenu_UpdateItem(pstContents);
					nx_HomeMenu_Shahid_UpdateItem(pstContents);
					ONDK_GWM_Obj_SetFocus(NX_HOMEMENU_SHAHID_ID);
					//Update number of item
					nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, 1,NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);

				}
				else if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID)
				{
					HxLOG_Print("Show COntents\n\n");
					#if 0
					pstContents->eFocusItem = eNxHomeIdx_FOCUS_CONTENTS;
					pstContents->ulShahid_StartIdx = 0;

					nx_HomeMenu_Shahid_UpdateItem(pstContents);
					ONDK_GWM_Obj_SetFocus(NX_HOMEMENU_SHAHID_ID);

					nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, 1,NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);
					#endif
				}
			}
			else
			{
				ONDK_GWM_Obj_SetFocus(NX_HOMEMENU_NETWORK_ID);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			if(pstContents->bInternet == TRUE)
			{
				if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_SHAHID)
				{
					HUINT32 ulObjId = 0;
					pstContents->eFocusItem = eNxHomeIdx_FOCUS_HOMEMENU;
					nx_HomeMenu_UpdateItem(pstContents);
					nx_HomeMenu_Shahid_UpdateItem(pstContents);
					NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, 0, &ulObjId);
					ONDK_GWM_Obj_SetFocus(ulObjId);
				}
				else if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_CONTENTS)
				{
					#if 0
					pstContents->eFocusItem = eNxHomeIdx_FOCUS_SHAHID;
					pstContents->ulShahid_StartIdx = 0;
					nx_HomeMenu_Shahid_UpdateItem(pstContents);
					ONDK_GWM_Obj_SetFocus(NX_HOMEMENU_SHAHID_ID);
					//Update number of item
					nx_HomeMenu_UpdateNumOfItem(pstContents, NX_HOMEMENU_NUMBER_OF_ITEM_ID, 1,NX_HOMEMENU_SHAHID_ITEM_MAXIMUM_COUNT);
					#endif
				}
			}
			else if(ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject()) == NX_HOMEMENU_NETWORK_ID)
			{
				HUINT32 ulObjId = 0;
				NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, 0, &ulObjId);
				ONDK_GWM_Obj_SetFocus(ulObjId);
			}
			return ONDK_MESSAGE_BREAK;

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

static ONDK_Result_t	nx_HomeMenu_MsgGwmTimer(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (p1 == NX_HOMEMENU_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_HomeMenu_MsgGwmClick(NxHome_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulObjId = 0, ulIndex = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ulObjId = (HUINT32) p1;
	if(ulObjId == NX_HOMEMENU_NETWORK_ID)
	{
		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEMNU_Network_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
                (ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc, 0, 0, 0, 1);
		return	ONDK_RET_OK;
	}

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		nx_HomeMenu_LaunchSubMenu(pstContents, (HUINT32)p1);
	}
	else
	{
		ulIndex = (NX_HOMEMENU_SHAHID_ID ^ p1) + pstContents->ulShahid_StartIdx;
		HxLOG_Print("\n\n[%s:%d] selected object (%d)\n\n",__FUNCTION__,__LINE__,ulIndex);
		ONDK_GWM_APP_Destroy(0);
		//ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY,
		//	NX_PRISM_SHAHID_Player_Proc, 0, 0, 0, 0);
		NX_PRISM_PLAYMGR_PlayStart(ePlayerType_SHAHID, "http://llmd.shahid.net/mediaDelivery/media/0dcac282e2234427a63716230c605b76.m3u8", 0, FALSE);
	}

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

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, pstContents->ulNumOfSubItem - 1 ,&ulObjId);
		if (eResult == ONDK_RET_OK)
		{
			ONDK_GWM_Obj_SetFocus(ulObjId);
			nx_HomeMenu_MsgGwmObjFocusChanged(pstContents, ulObjId, 0, 0);
		}
		else
		{
			NX_APP_Error("Error!!! (eResult == ONDK_RET_FAIL) \n");
		}
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

	if(pstContents->eFocusItem == eNxHomeIdx_FOCUS_HOMEMENU)
	{
		eResult = NX_PRISM_AppContents_GetObjIdBySubitemIndex(pstContents, 0 ,&ulObjId);
		if (eResult == ONDK_RET_OK)
		{
			ONDK_GWM_Obj_SetFocus(ulObjId);
			nx_HomeMenu_MsgGwmObjFocusChanged(pstContents, ulObjId, 0, 0);
		}
		else
		{
			NX_APP_Error("Error!!! (eResult == ONDK_RET_FAIL) \n");
		}
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
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			nx_HomeMenu_MsgGwmTimer(pstContents, p1, p2, p3);
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
		case MSG_GWM_APK_MESSAGE:
			eResult = nx_HomeMenu_ConnectionStatus_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
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
