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
 * @file                        nx_prism_menu_fav_chlist.c
 *
 *      Description:                                                                                                    \n
 *
 * @author                                                                                                                      \n
 * @date                                                                                                                        \n
 * @Attention           Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
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
#define NX_PRISM_FAV_CHLIST_ID                  eMenuFrame_Window_Settings

#define NX_PRISM_FAV_CHLIST_MAIN_NAME_ID           (NX_PRISM_FAV_CHLIST_ID + 1)
#define NX_PRISM_FAV_CHLIST_MAIN_NAME_X            70
#define NX_PRISM_FAV_CHLIST_MAIN_NAME_Y            122
#define NX_PRISM_FAV_CHLIST_MAIN_NAME_W            390
#define NX_PRISM_FAV_CHLIST_MAIN_NAME_H            26

#define NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID    (NX_PRISM_FAV_CHLIST_ID + 2)
#define NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_X     470
#define NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_Y     122
#define NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_W     150
#define NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_H     26

#define NX_PRISM_FAV_CHLIST_FAV_NAME_ID             (NX_PRISM_FAV_CHLIST_ID + 3)
#define NX_PRISM_FAV_CHLIST_FAV_NAME_X              660
#define NX_PRISM_FAV_CHLIST_FAV_NAME_Y              122
#define NX_PRISM_FAV_CHLIST_FAV_NAME_W              390
#define NX_PRISM_FAV_CHLIST_FAV_NAME_H              26

#define NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID      (NX_PRISM_FAV_CHLIST_ID + 4)
#define NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_X       1060
#define NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_Y       122
#define NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_W       150
#define NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_H       26

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxFavChList_None = 0,
	eNxFavChList_Back,
	eNxFavChList_Escape,
} NxFavChList_DialogType_t;

typedef struct
{
	HBOOL                   bSelect;
	NX_Channel_t            stChannel;
	HCHAR                   ucName[DxNAME_LEN];

	HBOOL                   bTV;
	DxVideoType_e           eHighDefinition;
	DxCasType_e             eCasType;

	HBOOL                   bFavourite;

	HBOOL 					bEditFavourite;
	HUINT32					ulCheckNum;
	// HINT32                  nFavInitialIdx;
	// HINT32                  nFavIndex;
} Nx_ChannelData_t;

typedef struct
{
	HCHAR       ucName[DxNAME_LEN];
	HxVector_t  *pstChannelList;
	HBOOL 		bRename;
} Nx_FavGrpData_t;

typedef struct
{
	HUINT32                 ulLcn;
	HCHAR                   ucName[DxNAME_LEN];
	HINT32                  nCurrIdx;
} Nx_MoveCursorInfo_t;

typedef struct tag_NX_MENUFAVCHLIST_SET_CONTENT
{
		HUINT8              ucTitle[MAX_ELEMENT_TEXT_LENGTH];
		HUINT8              ucMainSelectInfo[MAX_ELEMENT_TEXT_LENGTH];
		HUINT8              ucFavSelectInfo[MAX_ELEMENT_TEXT_LENGTH];

		HxVector_t          *pstDataPool; // favourite 1~5 �׷쿡 ���� TV/Radio��� ������(Nx_CahnelData_t)�� �������ִ´�.
		// bFavourite�� ���� main list�� �׷��ش�.
		// favourite group�� channel list�� �̰����� �����͸� �����ͼ� �����Ѵ�.
		HxVector_t          *pstFavGrpList; // favourite 1~5���� �������ְ� item �� Nx_FavGrpData_t�̴�.

		HxVector_t          *pstCurMainList;
		HxVector_t          *pstCurFavList;
		HxVector_t          *pstFocusList;
		HxVector_t          *pstMoveChList;

		Nx_MoveCursorInfo_t *pstMoveCursorInfo;
		HBOOL               bMoveMode;
		HUINT32             ulMainSelectNum;
		HUINT32             ulFavSelectNum;

		NX_ChListGroup_t    eCurrChGroupType;
		NX_FavGroupID_e     eCurrFavGroupType;
		HINT32              nFocusViewID;
		NxFavChList_DialogType_t eCurDialogType;

		HINT32              lPrevCurIdx;

		HUINT8              szDate[NX_TEXT_SIZE_32];
		HUINT8              szTime[NX_TEXT_SIZE_32];
} Nx_MenuFavChListSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuFavChListSetContent_t		s_stFavChListContent;
static NXUI_DialogInstance_t			s_stFavChList_DialogInstance;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_MenuFavChListSetContent_t* nx_menuFavChList_GetContents(void);
static inline NXUI_DialogInstance_t* nx_menuFavChList_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuFavChList_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFavChList_CreateSaveDialog(Nx_MenuFavChListSetContent_t *pstContents, HUINT8 *szStrID);
static ONDK_Result_t	nx_menuFavChList_DestroySaveDialog (void);

static void             nx_menuFavChList_CreateMainChList(HxVector_t *pstChannelList, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static void             nx_menuFavChList_CreateFavChList(HxVector_t *pstChannelList, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static ONDK_Result_t    nx_menuFavChList_DrawChList(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    nx_menuFavChList_DrawCursor_A1(ONDKSurfaceHandle *screen, Nx_ChannelData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HBOOL bIsFocus, int bUiTop, HBOOL bIsFocusView);
static ONDK_Result_t    nx_menuFavChList_DrawMovingCursor(ONDKSurfaceHandle *screen, ONDK_Rect_t *stListRegion, HINT32 nOffsetY,  int bUiTop);
static void             nx_menuFavChList_UpdateCurrentTime(Nx_MenuFavChListSetContent_t *pstContents);
static void             nx_menuFavChList_FindNextFocus(Nx_MenuFavChListSetContent_t *pstContents, HBOOL bUp);
static ONDK_Result_t    nx_menuFavChList_LoadFavDataPool(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_LoadCurrentMainChannelList(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_LoadCurrentFavChannelList(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_InitSettingsItems(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_DrawDefaultUI(Nx_MenuFavChListSetContent_t *pstContents);
static void             nx_menuFavChList_UpdateGroupName(Nx_MenuFavChListSetContent_t *pstContents);
static void             nx_menuFavChList_UpdateSelectInfo(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_ChangedPopupOptValue(Nx_MenuFavChListSetContent_t *pstContents, eNxUiUtil_OptItem_t eCurOpt, NX_ChListGroup_t eChGroupType);
static HBOOL            nx_menuFavChList_FindSameSvc(void *data, void *userData);
static ONDK_Result_t 	nx_menuFavChList_CheckEditData(Nx_MenuFavChListSetContent_t *pstContents, HBOOL *pbEdit);
static ONDK_Result_t	nx_menuFavChList_SaveEditData(Nx_MenuFavChListSetContent_t *pstContents);
static ONDK_Result_t    nx_menuFavChList_MsgGwmCreate(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuFavChList_MsgGwmDestroy(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuFavChList_MsgGwmHapiMsg(Nx_MenuFavChListSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuFavChList_MsgGwmKeyDown(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuFavChList_MsgGwmTimer(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuFavChList_MsgAppDlgClicked(Nx_MenuFavChListSetContent_t *pstContents, HINT32 lClickedBtn);
static ONDK_Result_t	nx_menuFavChList_MsgAppDlgCanceled(Nx_MenuFavChListSetContent_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuFavChListSetContent_t* nx_menuFavChList_GetContents(void)
{
		return  &s_stFavChListContent;
}


static inline NXUI_DialogInstance_t* nx_menuFavChList_GetDialogPopupContents(void)
{
	return &s_stFavChList_DialogInstance;
}


static ONDK_Result_t	nx_menuFavChList_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuFavChList_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuFavChList_CreateSaveDialog(Nx_MenuFavChListSetContent_t *pstContents, HUINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuFavChList_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuFavChList_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuFavChList_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_DestroySaveDialog (void)
{
	ONDK_GWM_APP_Destroy(nx_menuFavChList_DialogProc);

	return	ONDK_RET_OK;
}


static void             nx_menuFavChList_CreateMainChList(HxVector_t *pstChannelList, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_X, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_Y, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_W, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_ID, &stRect, COL(C_G_Gong02));

	// Create fav channel group list
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_X, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_Y, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_W, NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_H);
	ONDK_GWM_List_Create(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, &stRect, "FAV CHANNEL GROUP LIST", NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, nTotal, NX_PRISM_UI_MENU_FAV_CHLIST_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstChannelList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID);
}


static void             nx_menuFavChList_CreateFavChList(HxVector_t *pstChannelList, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_X, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_Y, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_W, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_ID, &stRect, COL(C_G_Gong03_50));

	// Create fav channel favourite list
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_X, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_Y, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_W, NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_H);
	ONDK_GWM_List_Create(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, &stRect, "FAV CHANNEL FAVOURITE LIST", NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, nTotal, NX_PRISM_UI_MENU_FAV_CHLIST_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstChannelList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);
}


static ONDK_Result_t    nx_menuFavChList_DrawChList(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */

	HINT32          nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32          nCellHeight = 0, nCount = 0, nOffsetY = 0;
	LIST_OBJECT     objList = (LIST_OBJECT)object;
	HINT32          objID = 0;
	HxVector_t      *pstDataList = NULL;
	Nx_ChannelData_t  *pstData = NULL;

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000;ONDKFontHandle     *hFontHandle = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	Nx_MenuFavChListSetContent_t* pstContents = NULL;

	NOT_USED_PARAM(arg);
	/* create font handler */
	hBoldFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	hNormalFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22);
	hFontHandle = hNormalFontHandle;
	/* init list coordinate */
	stListRegion.x = area->x1 + rect->x;
	stListRegion.y = area->y1 + rect->y;
	stListRegion.w = rect->w;
	stListRegion.h = rect->h;

	/* get list data */
	pstDataList = (HxVector_t *)ONDK_GWM_List_GetArrayPtr(objList);
	ONDK_GWM_List_GetProperty (objList, &nTotal, &nStartIdx, &nCurrIdx, &nSelectedIdx, &nNumOfPage);
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
	}
	else
	{
		nCount = nTotal;
	}

	pstContents = nx_menuFavChList_GetContents();
	objID = ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList);

	if (pstContents->bMoveMode == TRUE && (objID == pstContents->nFocusViewID))
	{
		HINT32  nDrawItemCnt = 0;
		HBOOL   bDrawCursor = FALSE;
		HINT32  nCursorOffset = 0, nTmpCnt = nCount, nSelectLoc = 0;

		for (i = 0 ; i < nStartIdx + nCount ; i++)
		{
			pstData = HLIB_VECTOR_ItemAt(pstDataList, i);

			if (pstData->bSelect == TRUE)
			{
				nSelectLoc = i;
				break;
			}
		}

		for (i = 0 ; i < nCount; i++)
		{
			nDataIdx = nStartIdx + i;
			if (nSelectLoc < nStartIdx)
			{
				nDataIdx++;
			}
			if (nDataIdx >= nTotal)
			{
				break;
			}
			pstData = HLIB_VECTOR_ItemAt(pstDataList, nDataIdx);

			if (nStartIdx + nDrawItemCnt == nCurrIdx)
			{
				nCursorOffset = nOffsetY;
				nOffsetY += nCellHeight;
				bDrawCursor = TRUE;
				nDrawItemCnt++;
				if (pstData->bSelect == FALSE)
				{
					i--;
					nCount = nTmpCnt + 1;
				}
				continue;
			}

			if (pstData->bSelect == TRUE)
			{
				nCount = nTmpCnt + 1;
				continue;
			}

			textFontColor = COL(C_T_Whistle_100);
			hFontHandle = hNormalFontHandle;
			nx_menuFavChList_DrawCursor_A1(screen, pstData, &stListRegion, nOffsetY, hFontHandle, textFontColor, bgColor, FALSE, bUiTop, (objID == pstContents->nFocusViewID));
			nOffsetY += nCellHeight;
			nDrawItemCnt++;
		}
		if (bDrawCursor == TRUE)
		{
			nx_menuFavChList_DrawMovingCursor(screen, &stListRegion, nCursorOffset, bUiTop);
		}
		else
		{
			nx_menuFavChList_DrawMovingCursor(screen, &stListRegion, nOffsetY, bUiTop);
		}

		pstContents->pstMoveCursorInfo->nCurrIdx = nCurrIdx;
	}
	else
	{
		for (i = 0 ; i < nCount ; i++)
		{
			/* init data index */
			nDataIdx = nStartIdx + i;
			if (nDataIdx >= nTotal)
			{
				break;
			}
			pstData = HLIB_VECTOR_ItemAt(pstDataList, nDataIdx);

			/* make cursor data */
			if (bUiTop > 0)
			{

				if (objID == pstContents->nFocusViewID)
				{
					if (nDataIdx == nCurrIdx) /* focus */
					{
						textFontColor = COL(C_T_Bang_100);
						bgColor = COL(C_G_Base01_100);
						hFontHandle = hBoldFontHandle;
					}
					else
					{
						textFontColor = COL(C_T_Whistle_100);
						hFontHandle = hNormalFontHandle;
					}
				}
				else
				{
					textFontColor = COL(C_T_Gong08_40);
					hFontHandle = hNormalFontHandle;
				}
			}
			else
			{
				if (objID == pstContents->nFocusViewID)
				{
					if (nDataIdx == nCurrIdx) /* history */
					{
						textFontColor = COL(C_T_Whistle_100);
						bgColor = COL(C_G_Base01_20);
						hFontHandle = hBoldFontHandle;
					}
					else
					{
						textFontColor = COL(C_T_Whistle_100);
						hFontHandle = hNormalFontHandle;
					}
				}
				else
				{
					textFontColor = COL(C_T_Gong08_40);
					hFontHandle = hNormalFontHandle;
				}
			}

			nx_menuFavChList_DrawCursor_A1(screen, pstData, &stListRegion, nOffsetY, hFontHandle, textFontColor, bgColor, ((nDataIdx == nCurrIdx)), bUiTop, (objID == pstContents->nFocusViewID));
			nOffsetY += nCellHeight;
			bgColor = 0x00000000;
		}
	}

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_DrawCursor_A1(ONDKSurfaceHandle *screen, Nx_ChannelData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HBOOL bIsFocus, int bUiTop, HBOOL bIsFocusView)
{
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };
	HCHAR           pText[DxNAME_LEN] = {0, };
	HINT32          nCheckBoxX = 0, nCheckBoxY = 0;

	HxSTD_sprintf(pText, "%d %s", pstData->stChannel.lcn, pstData->ucName);

	if (bIsFocus == TRUE && bIsFocusView == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, bgColor);
	}

	nCheckBoxX = stListRegion->x + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_X;
	nCheckBoxY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_Y;
	stRect = ONDK_Rect(nCheckBoxX, nCheckBoxY, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_W, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_CHECKBOX_H);
	if (bIsFocus == TRUE && bIsFocusView == TRUE)
	{
		if (pstData->bSelect == TRUE)
		{
			if (bUiTop > 0)
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_ON_C_PNG);
			}
			else
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_ON_H_PNG);
			}
		}
		else
		{
			if (bUiTop > 0)
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_OFF_C_PNG);
			}
			else
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_OFF_H_PNG);
			}
		}
	}
	else
	{

		if (pstData->bSelect == TRUE)
		{
			if (bIsFocusView == TRUE)
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_ON_N_PNG);
			}
			else
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_ON_D_PNG);
			}
		}
		else
		{
			if (bIsFocusView == TRUE)
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_OFF_N_PNG);
			}
			else
			{
				ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_OFF_D_PNG);
			}
		}

	}

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_X, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_Y, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);


	return ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_DrawMovingCursor(ONDKSurfaceHandle *screen, ONDK_Rect_t *stListRegion, HINT32 nOffsetY,  int bUiTop)
{
	Nx_MenuFavChListSetContent_t* pstContents = NULL;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };
	HCHAR           pText[DxNAME_LEN] = {0, };
	HINT32          nMoveImgX = 0, nMoveImgY = 0;

	pstContents = nx_menuFavChList_GetContents();

	if (bUiTop > 0)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x + stListRegion->w - NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H - NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, stListRegion->w, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, COL(C_G_Base01_100));
	}
	else
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x + stListRegion->w - NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H - NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, stListRegion->w, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_FAV_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_H, COL(C_G_Base01_20));
	}

	if (pstContents->ulFavSelectNum < 2)
	{
		nMoveImgX = stListRegion->x + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_X;
		nMoveImgY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_Y;
		stRect = ONDK_Rect(nMoveImgX, nMoveImgY, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_W, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_IMG_H);
		if (bUiTop > 0)
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_C_PNG);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_H_PNG);
		}
	}
	else
	{
		nMoveImgX = stListRegion->x + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_X;
		nMoveImgY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_Y;
		stRect = ONDK_Rect(nMoveImgX, nMoveImgY, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_W, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_MOVE_GROUP_IMG_H);
		if (bUiTop > 0)
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_GROUP_C_PNG);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_GROUP_H_PNG);
		}
	}

	if (pstContents->ulFavSelectNum < 2)
	{
		ONDKFontHandle      *hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
		ONDK_Color_t        textFontColor = 0x00000000;

		if (bUiTop > 0)
		{
			textFontColor = COL(C_T_Base01);
			hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
		}
		else
		{
			textFontColor = COL(C_T_Base02);
			hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
		}
		HxSTD_sprintf(pText, "%d %s", pstContents->pstMoveCursorInfo->ulLcn, pstContents->pstMoveCursorInfo->ucName);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_X, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_OFFSET_Y, NX_PRISM_UI_MENU_FAV_CHLIST_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
	}

	return ONDK_RET_OK;
}


static void             nx_menuFavChList_UpdateCurrentTime(Nx_MenuFavChListSetContent_t *pstContents)
{
		HERROR                  hError = ERR_FAIL;
		HCHAR                   *pszDayOfWeek;
		HUINT32                 ulUnixtime = 0, ulWeekIndex = 0;
		HxDATETIME_t    stDatetime;

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


static void             nx_menuFavChList_FindNextFocus(Nx_MenuFavChListSetContent_t *pstContents, HBOOL bRight)
{
	if (pstContents->bMoveMode == FALSE)
	{
		if (bRight == TRUE)
		{
			pstContents->nFocusViewID = NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID;
			pstContents->pstFocusList = pstContents->pstCurFavList;
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);
			ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_ID, COL(C_G_Gong03_50));
			ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_ID, COL(C_G_Gong02));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
		}
		else
		{
			pstContents->nFocusViewID = NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID;
			pstContents->pstFocusList = pstContents->pstCurMainList;
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID);
			ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_BG_ID, COL(C_G_Gong02));
			ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_BG_ID, COL(C_G_Gong03_50));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));
		}
	}
}


static ONDK_Result_t    nx_menuFavChList_LoadFavDataPool(Nx_MenuFavChListSetContent_t *pstContents)
{
	HINT32          i = 0, j = 0;
	HINT32          nTVTotal = 0, nRadioTotal = 0, nFavTotal = 0;
	HUINT32         ulExtraFilter = 0;
	HERROR          herror = ERR_FAIL;
	NX_Channel_t    *pstTVChList = NULL, *pstRadioChList = NULL;

	/* Create Data pool */
	/* Load TV data */
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eNxChListGroup_TV);
	herror = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstTVChList, &nTVTotal, eNxChListGroup_TV, ulExtraFilter);
	if (herror != ERR_OK)
	{
		NX_APP_Error("Error!!! (herror != ERR_OK) \n");
		return ONDK_RET_FAIL;
	}
	/* Load Radio data */
	ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eNxChListGroup_RADIO);
	herror = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstRadioChList, &nRadioTotal, eNxChListGroup_RADIO, ulExtraFilter);
	if (herror != ERR_OK)
	{
		NX_APP_Error("Error!!! (herror != ERR_OK) \n");
		return ONDK_RET_FAIL;
	}

	pstContents->pstDataPool = HLIB_VECTOR_NewEasy(NX_SETTING_FAVOURITE_GROUP_NUM, NULL, NULL);
	for (i = 0 ; i < NX_SETTING_FAVOURITE_GROUP_NUM ; i++)
	{
		HxVector_t *pstList = NULL;

		pstList = HLIB_VECTOR_NewEasy((nTVTotal + nRadioTotal), NULL, NULL);
		for (j = 0 ; j < nTVTotal ; j++)
		{
			Nx_ChannelData_t *pstData = (Nx_ChannelData_t *)NX_APP_Malloc(sizeof(Nx_ChannelData_t));
			DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(pstTVChList[j].svcUid);

			HxSTD_memcpy(&(pstData->stChannel), &pstTVChList[j], sizeof(NX_Channel_t));
			pstData->ulCheckNum = pstTVChList[j].lcn;
			HxSTD_sprintf(pstData->ucName, "%s", pstService->name);
			pstData->bTV = TRUE;
			pstData->eHighDefinition = pstService->highDefinition;
			pstData->eCasType = pstService->casType;
			pstData->bFavourite = NX_CHANNEL_FAV_IsFavouriteItem(eNxFavGroupID_User + j, &(pstTVChList[j]));
			pstData->bEditFavourite = FALSE;
			pstData->bSelect = FALSE;

			HLIB_VECTOR_Add(pstList, pstData);
		}
		for (j = 0 ; j < nRadioTotal ; j++)
		{
			Nx_ChannelData_t *pstData = (Nx_ChannelData_t *)NX_APP_Malloc(sizeof(Nx_ChannelData_t));
			DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(pstRadioChList[j].svcUid);

			HxSTD_memcpy(&(pstData->stChannel), &pstRadioChList[j], sizeof(NX_Channel_t));
			pstData->ulCheckNum = pstRadioChList[j].lcn;
			HxSTD_sprintf(pstData->ucName, "%s", pstService->name);
			pstData->bTV = FALSE;
			pstData->eHighDefinition = pstService->highDefinition;
			pstData->eCasType = pstService->casType;
			pstData->bFavourite = NX_CHANNEL_FAV_IsFavouriteItem(eNxFavGroupID_User + j, &(pstRadioChList[j]));
			pstData->bEditFavourite = FALSE;
			pstData->bSelect = FALSE;

			HLIB_VECTOR_Add(pstList, pstData);
		}
		HLIB_VECTOR_Add(pstContents->pstDataPool, pstList);
	}
	/* Create Favourite Group List */
	pstContents->pstFavGrpList = HLIB_VECTOR_NewEasy(NX_SETTING_FAVOURITE_GROUP_NUM, NULL, NULL);

	for (i = 0 ; i < NX_SETTING_FAVOURITE_GROUP_NUM ; i++)
	{
		Nx_FavGrpData_t *pstFavGrp = NULL;
		HxVector_t *pstDataList = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, i);
		NX_Channel_t *pstChannelList = NULL;
		HBOOL		bUserUpdateFlag = FALSE;

		pstFavGrp = (Nx_FavGrpData_t *)NX_APP_Malloc(sizeof(Nx_FavGrpData_t));

		NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User + i, &bUserUpdateFlag);

		if (bUserUpdateFlag == TRUE)
		{
			NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User + i, pstFavGrp->ucName);
		}
		else
		{
			switch(i)
			{
				case 0:
					HxSTD_sprintf(pstFavGrp->ucName, "%s", (HCHAR*)ONDK_GetString(RES_FAVOURITE1_ID));
					break;
				case 1:
					HxSTD_sprintf(pstFavGrp->ucName, "%s", (HCHAR*)ONDK_GetString(RES_FAVOURITE2_ID));
					break;
				case 2:
					HxSTD_sprintf(pstFavGrp->ucName, "%s", (HCHAR*)ONDK_GetString(RES_FAVOURITE3_ID));
					break;
				case 3:
					HxSTD_sprintf(pstFavGrp->ucName, "%s", (HCHAR*)ONDK_GetString(RES_FAVOURITE4_ID));
					break;
				case 4:
					HxSTD_sprintf(pstFavGrp->ucName, "%s", (HCHAR*)ONDK_GetString(RES_FAVOURITE5_ID));
					break;
			}
		}


		pstFavGrp->bRename = FALSE;
		nFavTotal = NX_CHANNEL_FAV_GetFavouriteNum(eNxFavGroupID_User + i);
		if (nFavTotal != 0)
		{
			NX_CHANNEL_FAV_MakeNxChannelListByFavGroup(eNxFavGroupID_User + i, &pstChannelList, &nFavTotal);
			pstFavGrp->pstChannelList = HLIB_VECTOR_NewEasy(nFavTotal, NULL, NULL);
			for (j = 0 ; j < nFavTotal ; j++)
			{
				Nx_ChannelData_t *pstData =  NULL;

				pstData = HLIB_VECTOR_Find(pstDataList, nx_menuFavChList_FindSameSvc, (void*)&(pstChannelList[j]));
				if (pstData != NULL)
				{
					pstData->bFavourite = TRUE;
					pstData->bEditFavourite = FALSE;
					HLIB_VECTOR_Add(pstFavGrp->pstChannelList, pstData);
				}
			}
		}
		else
		{
			pstFavGrp->pstChannelList = HLIB_VECTOR_NewEasy(0, NULL, NULL);
		}

		HLIB_VECTOR_Add(pstContents->pstFavGrpList, pstFavGrp);
	}

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_LoadCurrentMainChannelList(Nx_MenuFavChListSetContent_t *pstContents)
{
	HINT32      nMainListSize = 0;
	HxVector_t *pstDataList = NULL;
	Nx_ChannelData_t *pstData = NULL;

	if (pstContents->pstCurMainList != NULL)
	{
		HLIB_VECTOR_Delete(pstContents->pstCurMainList);
	}

	pstDataList = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, pstContents->eCurrFavGroupType - eNxChListGroup_Favorite1);

	/* Find current group data */
	switch(pstContents->eCurrChGroupType)
	{
		case eNxChListGroup_TV:
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->bTV == TRUE)
				{
					if (pstData->bFavourite != TRUE)
					{
						nMainListSize++;
					}
				}
			});
			pstContents->pstCurMainList = HLIB_VECTOR_NewEasy(nMainListSize, NULL, NULL);
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->bTV == TRUE)
				{
					if (pstData->bFavourite != TRUE)
					{
						HLIB_VECTOR_Add(pstContents->pstCurMainList, pstData);
					}
				}
			});
			break;
		case eNxChListGroup_RADIO:
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->bTV == FALSE)
				{
					if (pstData->bFavourite != TRUE)
					{
						nMainListSize++;
					}
				}
			});
			pstContents->pstCurMainList = HLIB_VECTOR_NewEasy(nMainListSize, NULL, NULL);
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->bTV == FALSE)
				{
					if (pstData->bFavourite != TRUE)
					{
						HLIB_VECTOR_Add(pstContents->pstCurMainList, pstData);
					}
				}
			});
			break;
		case eNxChListGroup_HDTV:
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eHighDefinition == eDxVIDEO_TYPE_HD)
				{
					if (pstData->bFavourite != TRUE)
					{
						nMainListSize++;
					}
				}
			});
			pstContents->pstCurMainList = HLIB_VECTOR_NewEasy(nMainListSize, NULL, NULL);
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eHighDefinition == eDxVIDEO_TYPE_HD)
				{
					if (pstData->bFavourite != TRUE)
					{
						HLIB_VECTOR_Add(pstContents->pstCurMainList, pstData);
					}
				}
			});
			break;
		case eNxChListGroup_FTA:
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eCasType == eDxCAS_TYPE_FTA)
				{
					if (pstData->bFavourite != TRUE)
					{
						nMainListSize++;
					}
				}
			});
			pstContents->pstCurMainList = HLIB_VECTOR_NewEasy(nMainListSize, NULL, NULL);
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eCasType == eDxCAS_TYPE_FTA)
				{
					if (pstData->bFavourite != TRUE)
					{
						HLIB_VECTOR_Add(pstContents->pstCurMainList, pstData);
					}
				}
			});
			break;
		case eNxChListGroup_CAS:
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eCasType != eDxCAS_TYPE_FTA)
				{
					if (pstData->bFavourite != TRUE)
					{
						nMainListSize++;
					}
				}
			});
			pstContents->pstCurMainList = HLIB_VECTOR_NewEasy(nMainListSize, NULL, NULL);
			HxVECTOR_EACH(pstDataList, Nx_ChannelData_t *, pstData, {
				if (pstData->eCasType != eDxCAS_TYPE_FTA)
				{
					if (pstData->bFavourite != TRUE)
					{
						HLIB_VECTOR_Add(pstContents->pstCurMainList, pstData);
					}
				}
			});
			break;
		default:
			return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_LoadCurrentFavChannelList(Nx_MenuFavChListSetContent_t *pstContents)
{
	Nx_FavGrpData_t *pstFavGrp = NULL;

	pstFavGrp = HLIB_VECTOR_ItemAt(pstContents->pstFavGrpList, pstContents->eCurrFavGroupType - eNxChListGroup_Favorite1);
	pstContents->pstCurFavList = pstFavGrp->pstChannelList;

	return ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_InitSettingsItems(Nx_MenuFavChListSetContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_FAVOURITE_CHANNELS_ID), MAX_ELEMENT_TEXT_LENGTH);

	pstContents->nFocusViewID = NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID;

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_DrawDefaultUI(Nx_MenuFavChListSetContent_t *pstContents)
{
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };
	HINT32          nTotal = 0;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuFavChList_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	/* Draw - Group name */
	stRect = ONDK_Rect(NX_PRISM_FAV_CHLIST_MAIN_NAME_X, NX_PRISM_FAV_CHLIST_MAIN_NAME_Y, NX_PRISM_FAV_CHLIST_MAIN_NAME_W, NX_PRISM_FAV_CHLIST_MAIN_NAME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, &stRect, NULL);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	/* Draw - Group select info */
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);
	// HxSTD_sprintf((char*)pstContents->ucMainSelectInfo, "%d / %d", pstContents->ulMainSelectNum, nTotal);
	stRect = ONDK_Rect(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_X, NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_Y, NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_W, NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, &stRect, NULL);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	/* Draw - Favourite group name */
	stRect = ONDK_Rect(NX_PRISM_FAV_CHLIST_FAV_NAME_X, NX_PRISM_FAV_CHLIST_FAV_NAME_Y, NX_PRISM_FAV_CHLIST_FAV_NAME_W, NX_PRISM_FAV_CHLIST_FAV_NAME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, &stRect, NULL);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));

	/* Draw - Favourite group select info */
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
	// HxSTD_sprintf((char*)pstContents->ucFavSelectInfo, "%d / %d", pstContents->ulFavSelectNum, nTotal);
	stRect = ONDK_Rect(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_X, NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_Y, NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_W, NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, &stRect, NULL);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, COL(C_T_Gong08_40), COL(C_T_Whistle_100));

	/* Draw OPT Button */
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_FAV_CHLIST_OPT_X, NX_PRISM_UI_MENU_FAV_CHLIST_OPT_Y, NX_PRISM_UI_MENU_FAV_CHLIST_OPT_W, NX_PRISM_UI_MENU_FAV_CHLIST_OPT_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_FAV_CHLIST_OPT_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_FAV_CHLIST_OPT_ID, NX_PRISM_COMP_DrawEdit_HelpR);
	ONDK_GWM_Obj_DisableObject(NX_PRISM_UI_MENU_FAV_CHLIST_OPT_ID);

	nx_menuFavChList_UpdateGroupName(pstContents);
	nx_menuFavChList_UpdateSelectInfo(pstContents);

	return  ONDK_RET_OK;
}


static void             nx_menuFavChList_UpdateGroupName(Nx_MenuFavChListSetContent_t *pstContents)
{
	HCHAR *pGroupName = "";
	Nx_FavGrpData_t *pstFavGrp = HLIB_VECTOR_ItemAt(pstContents->pstFavGrpList, pstContents->eCurrFavGroupType - eNxChListGroup_Favorite1);

	switch(pstContents->eCurrChGroupType)
	{
		case eNxChListGroup_TV :
			pGroupName = ONDK_GetString(RES_TV_01_ID);
			break;
		case eNxChListGroup_RADIO :
			pGroupName = ONDK_GetString(RES_RADIO_ID);
			break;
		case eNxChListGroup_HDTV :
			pGroupName = ONDK_GetString(RES_HDTV_ID);
			break;
		case eNxChListGroup_FTA :
			pGroupName = ONDK_GetString(RES_FREE_CHANNEL_ID);
			break;
		case eNxChListGroup_CAS :
			pGroupName = ONDK_GetString(RES_PAY_CHANNEL_ID);
			break;
		default:
			break;
	}


	ONDK_GWM_COM_Text_SetText(NX_PRISM_FAV_CHLIST_MAIN_NAME_ID, pGroupName);
	ONDK_GWM_COM_Text_SetText(NX_PRISM_FAV_CHLIST_FAV_NAME_ID, pstFavGrp->ucName);
}

static void             nx_menuFavChList_UpdateSelectInfo(Nx_MenuFavChListSetContent_t *pstContents)
{
	HINT32 nTotal = 0;

	nTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);
	HxSTD_sprintf(pstContents->ucMainSelectInfo, "%d / %d", pstContents->ulMainSelectNum, nTotal);
	ONDK_GWM_COM_Text_SetText(NX_PRISM_FAV_CHLIST_MAIN_SELECT_INFO_ID, pstContents->ucMainSelectInfo);

	nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
	HxSTD_sprintf(pstContents->ucFavSelectInfo, "%d / %d", pstContents->ulFavSelectNum, nTotal);
	ONDK_GWM_COM_Text_SetText(NX_PRISM_FAV_CHLIST_FAV_SELECT_INFO_ID, pstContents->ucFavSelectInfo);
}


static ONDK_Result_t    nx_menuFavChList_ChangedPopupOptValue(Nx_MenuFavChListSetContent_t *pstContents, eNxUiUtil_OptItem_t eCurOpt, NX_ChListGroup_t eChGroupType)
{
	Nx_ChannelData_t *pstData = NULL;

	switch(eCurOpt)
	{
		case eNxUtil_OptItem_AddFav:
			if (pstContents->ulMainSelectNum == 0)
			{
				HINT32 nCurrIdx = 0, nMainTotal = 0, nFavLastIdx = 0;
				Nx_ChannelData_t *pstData = NULL;

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID);
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurMainList, nCurrIdx);
				pstData->bFavourite = TRUE;
				pstData->bEditFavourite = !pstData->bEditFavourite;

				/* Reload MainList */
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
				nMainTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nMainTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);


				nFavLastIdx = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				HLIB_VECTOR_Insert(pstContents->pstCurFavList, nFavLastIdx, pstData);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nFavLastIdx + 1, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
			}
			else
			{
				Nx_ChannelData_t *pstData = NULL;
				HINT32 nMainTotal = 0, nFavLastIdx = 0;

				nFavLastIdx = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				HxVECTOR_EACH(pstContents->pstCurMainList, Nx_ChannelData_t *, pstData, {
					if (pstData->bSelect == TRUE)
					{
						pstData->bFavourite = TRUE;
						pstData->bEditFavourite = !pstData->bEditFavourite;
						pstData->bSelect = FALSE;
						HLIB_VECTOR_Insert(pstContents->pstCurFavList, nFavLastIdx++, pstData);
					}
				});
				/* Reload MainList */
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
				nMainTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nMainTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nFavLastIdx, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				pstContents->ulMainSelectNum = 0;
			}
			nx_menuFavChList_UpdateSelectInfo(pstContents);
			break;
		case eNxUtil_OptItem_Group:
			{
				HINT32 nTotal = 0;

				pstContents->eCurrChGroupType = eChGroupType;
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);

				nTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);

				nx_menuFavChList_UpdateGroupName(pstContents);
				nx_menuFavChList_UpdateSelectInfo(pstContents);
			}
			break;
		case eNxUtil_OptItem_Remove:
			if (pstContents->ulFavSelectNum == 0)
			{
				HINT32 nCurrIdx = 0, nMainTotal = 0, nFavTotal = 0;
				Nx_ChannelData_t *pstData = NULL;

				nFavTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				if (nFavTotal == 0)
				{
					break;
				}

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, nCurrIdx);
				pstData->bFavourite = FALSE;
				pstData->bEditFavourite = !pstData->bEditFavourite;
				HLIB_VECTOR_Remove(pstContents->pstCurFavList, nCurrIdx);
				nFavTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nFavTotal, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);

				/* Reload MainList */
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
				nMainTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nMainTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
			}
			else
			{
				HINT32  i = 0;
				Nx_ChannelData_t *pstData = NULL;
				HINT32 nMainTotal = 0, nFavTotal = 0;

				nFavTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);

				for (i = 0 ; i < nFavTotal ; i++)
				{
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, i);

					if (pstData->bSelect == TRUE)
					{
						pstData->bFavourite = FALSE;
						pstData->bEditFavourite = !pstData->bEditFavourite;
						pstData->bSelect = FALSE;
						HLIB_VECTOR_Remove(pstContents->pstCurFavList, i);
						i--;
						nFavTotal--;
					}
				}
				nFavTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nFavTotal, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);

				/* Reload MainList */
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
				nMainTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nMainTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);

				pstContents->ulFavSelectNum = 0;
			}
			nx_menuFavChList_UpdateSelectInfo(pstContents);
			break;
		case eNxUtil_OptItem_Move:
			{
				HBOOL 	bFirstCheck = FALSE;
				HINT32	nTotal = 0, nStartIdx = 0, nNumOfPage = 0, nCount = 0, nCurrIdx = 0, i = 0;
				Nx_ChannelData_t *pstData = NULL;

				nStartIdx = ONDK_GWM_List_GetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);
				ONDK_GWM_List_GetTotalItem(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, &nTotal);
				nNumOfPage = ONDK_GWM_List_GetNumOfPage(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);

				if (nTotal > nNumOfPage)
				{
					nCount = nNumOfPage;
				}
				else
				{
					nCount = nTotal;
				}

				if (pstContents->ulFavSelectNum > 0)
				{
					i = 0;
					HxVECTOR_EACH(pstContents->pstCurFavList, Nx_ChannelData_t *, pstData, {
						if (pstData->bSelect == TRUE)
						{
							if (bFirstCheck == FALSE)
							{
								if (i < nStartIdx || i >= nStartIdx + nCount)
								{
									nStartIdx = i;
								}
								nCurrIdx = i;
								bFirstCheck = TRUE;
							}
						}
						i++;
					});
				}
				else
				{
					nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID);
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, nCurrIdx);
					pstData->bSelect = TRUE;
					pstContents->ulFavSelectNum++;
				}

				pstContents->pstMoveChList = HLIB_VECTOR_NewEasy((nTotal - pstContents->ulFavSelectNum + 1), NULL, NULL);
				bFirstCheck = FALSE;
				pstContents->bMoveMode = TRUE;

				for (i = 0 ; i < nTotal ; i++)
				{
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, i);
					if (pstData->bSelect == TRUE)
					{
						if (bFirstCheck == FALSE)
						{
							HLIB_VECTOR_Add(pstContents->pstMoveChList, pstData);
							bFirstCheck = TRUE;
						}
						continue;
					}
					HLIB_VECTOR_Add(pstContents->pstMoveChList, pstData);
				}

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), (nTotal - pstContents->ulFavSelectNum + 1), (GWM_LIST_ITEMS)pstContents->pstMoveChList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, nStartIdx);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, nCurrIdx);

				pstContents->pstMoveCursorInfo = (Nx_MoveCursorInfo_t *)NX_APP_Malloc(sizeof(Nx_MoveCursorInfo_t));
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, nCurrIdx);
				pstContents->pstMoveCursorInfo->ulLcn = pstData->stChannel.lcn;
				HxSTD_sprintf(pstContents->pstMoveCursorInfo->ucName, "%s", pstData->ucName);
				break;
			}
		case eNxUtil_OptItem_ChangeFav:
			{
				HINT32 nTotal = 0;

				pstContents->eCurrFavGroupType = eChGroupType;
				nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
				nTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurMainList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID, 0);

				nx_menuFavChList_LoadCurrentFavChannelList(pstContents);
				nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, 0);

				pstContents->ulMainSelectNum = 0;
				pstContents->ulFavSelectNum = 0;

				nx_menuFavChList_UpdateGroupName(pstContents);
				nx_menuFavChList_UpdateSelectInfo(pstContents);
			}
			break;
		case eNxUtil_OptItem_RenameFav:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, 0 /*KEYBOARD_INPUT_NORMAL*/, 0);
			break;
		case eNxUtil_OptItem_SelectAll:
			HxVECTOR_EACH(pstContents->pstFocusList, Nx_ChannelData_t *, pstData, {
				pstData->bSelect = TRUE;
			});
			if (pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID)
			{
				pstContents->ulMainSelectNum = HLIB_VECTOR_Length(pstContents->pstFocusList);
			}
			else
			{
				pstContents->ulFavSelectNum = HLIB_VECTOR_Length(pstContents->pstFocusList);
			}
			nx_menuFavChList_UpdateSelectInfo(pstContents);
			break;
		case eNxUtil_OptItem_DeSelectAll:
			HxVECTOR_EACH(pstContents->pstFocusList, Nx_ChannelData_t *, pstData, {
				pstData->bSelect = FALSE;
			});
			if (pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID)
			{
				pstContents->ulMainSelectNum = 0;
			}
			else
			{
				pstContents->ulFavSelectNum = 0;
			}
			nx_menuFavChList_UpdateSelectInfo(pstContents);
			break;
		default:
			NX_APP_Print("nx_menuFavChList_ChangedPopupOptValue() Not used Current Option Item(%d)\n", eCurOpt);
			break;
	}

	return ONDK_RET_OK;
}


static HBOOL            nx_menuFavChList_FindSameSvc(void *data, void *userData)
{
	Nx_ChannelData_t    *pstData1 = (Nx_ChannelData_t *)data;
	NX_Channel_t        *pstData2 = (NX_Channel_t *)userData;

	if (pstData1->stChannel.svcUid == pstData2->svcUid)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


static ONDK_Result_t 	nx_menuFavChList_CheckEditData(Nx_MenuFavChListSetContent_t *pstContents, HBOOL *pbEdit)
{
	HxVector_t 			*pstChList = NULL;
	Nx_ChannelData_t 	*pstData = NULL;
	Nx_FavGrpData_t 	*pstFavGrp = NULL;
	HINT32 				i = 0, j = 0, nTotal = 0;

	for (i = 0 ; i < NX_SETTING_FAVOURITE_GROUP_NUM ; i++)
	{
		pstChList = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, i);
		pstFavGrp = HLIB_VECTOR_ItemAt(pstContents->pstFavGrpList, i);
		nTotal = HLIB_VECTOR_Length(pstChList);

		for (j = 0 ; j < nTotal ; j++)
		{
			pstData = HLIB_VECTOR_ItemAt(pstChList, j);
			if (pstData->bEditFavourite == TRUE)
			{
				*pbEdit = TRUE;
				return ONDK_RET_OK;
			}
			if (pstData->stChannel.lcn != pstData->ulCheckNum)
			{
				*pbEdit = TRUE;
				return ONDK_RET_OK;
			}
		}
		if (pstFavGrp->bRename == TRUE)
		{
			*pbEdit = TRUE;
			return ONDK_RET_OK;
		}
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_SaveEditData(Nx_MenuFavChListSetContent_t *pstContents)
{
	HINT32				 i = 0, j = 0, nTotal = 0;
	HxVector_t			 *pstChList = NULL;
	HxVector_t 			 *pstSaveChList = NULL;
	Nx_FavGrpData_t 	 *pstFavGrp = NULL;
	Nx_ChannelData_t 	 *pstData = NULL;
	HBOOL bEdit = FALSE;

	NX_CHANNEL_StopChannel();

	for (i = 0 ; i < NX_SETTING_FAVOURITE_GROUP_NUM ; i++)
	{
		bEdit = FALSE;
		pstChList = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, i);
		pstFavGrp = HLIB_VECTOR_ItemAt(pstContents->pstFavGrpList, i);

		// Update favourite group
		nTotal = HLIB_VECTOR_Length(pstChList);

		for (j = 0 ; j < nTotal ; j++)
		{
			pstData = HLIB_VECTOR_ItemAt(pstChList, j);
			if (pstData->bEditFavourite == TRUE)
			{
				bEdit = TRUE;
				break;
			}
			if (pstData->stChannel.lcn != pstData->ulCheckNum)
			{
				bEdit = TRUE;
				break;
			}
		}

		if (bEdit == TRUE)
		{
			pstChList = pstFavGrp->pstChannelList;
			pstSaveChList = HLIB_VECTOR_NewEasy(nTotal, NULL, NULL);
			HxVECTOR_EACH(pstChList, Nx_ChannelData_t*, pstData, {
				if (pstData->bFavourite == TRUE)
				{
					HLIB_VECTOR_Add(pstSaveChList, &(pstData->stChannel));
				}
			});
			NX_CHANNEL_FAV_UpdateFavouriteGroup(eNxFavGroupID_User + i, pstSaveChList);
			HLIB_VECTOR_Delete(pstSaveChList);
		}

		if (pstFavGrp->bRename == TRUE)
		{
			NX_CHANNEL_FAV_SetFavouriteName(eNxFavGroupID_User + i, pstFavGrp->ucName, TRUE);
		}
	}
	NX_PRISM_LIVE_ServiceStartByLastService();

	return ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_MsgGwmCreate(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32      nTotal = 0;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	pstContents->eCurrChGroupType = eNxChListGroup_TV;
	pstContents->eCurrFavGroupType = eNxChListGroup_Favorite1;
	nx_menuFavChList_LoadFavDataPool(pstContents);
	nx_menuFavChList_LoadCurrentMainChannelList(pstContents);
	nx_menuFavChList_LoadCurrentFavChannelList(pstContents);
	pstContents->pstFocusList = pstContents->pstCurMainList;

	// Init Menu Item
	nx_menuFavChList_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;
	// Draw Menu Item
	nx_menuFavChList_DrawDefaultUI(pstContents);
	// Create group list
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurMainList);
	nx_menuFavChList_CreateMainChList(pstContents->pstCurMainList, nTotal, (GWM_ObjDraw_t)nx_menuFavChList_DrawChList);
	// Create Fav list
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
	nx_menuFavChList_CreateFavChList(pstContents->pstCurFavList, nTotal, (GWM_ObjDraw_t)nx_menuFavChList_DrawChList);
	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_MsgGwmDestroy(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
			return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	HLIB_VECTOR_Delete(pstContents->pstDataPool);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuFavChListSetContent_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_MsgGwmHapiMsg(Nx_MenuFavChListSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return  ONDK_MESSAGE_PASS;
}


static ONDK_Result_t    nx_menuFavChList_MsgGwmKeyDown(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32          lKeyId = p1, lPrevCurIdx = 0;
	ONDK_Result_t   eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return  GWM_RET_SOURCE_NULL;
	}

	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	if (eResult == GWM_RET_LINK_CONFLICT)   // Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		 case KEY_ARROWLEFT:
			nx_menuFavChList_FindNextFocus(pstContents, FALSE);
			return ONDK_MESSAGE_BREAK;
		case KEY_ARROWRIGHT:
			nx_menuFavChList_FindNextFocus(pstContents, TRUE);
			return ONDK_MESSAGE_BREAK;
		case KEY_BACK:
		case KEY_EXIT:
			{
				if (pstContents->bMoveMode == TRUE)
				{
					HINT32 nTotal = 0, i = 0, nCurrIdx = 0;
					HBOOL bFirstCheck = FALSE;
					Nx_ChannelData_t *pstData = NULL;

					nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);
					pstContents->bMoveMode = FALSE;
					HxVECTOR_EACH(pstContents->pstCurFavList, Nx_ChannelData_t *, pstData, {
						if (pstData->bSelect == TRUE)
						{
							pstData->bSelect = FALSE;
							if (bFirstCheck	== FALSE)
							{
								nCurrIdx = i;
								bFirstCheck = TRUE;
							}
						}
						i++;
					});
					pstContents->ulFavSelectNum = 0;
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID, nCurrIdx);
					nx_menuFavChList_UpdateSelectInfo(pstContents);
				}
				else
				{
					HBOOL bEdit = FALSE;

					nx_menuFavChList_CheckEditData(pstContents, &bEdit);
					if (bEdit == TRUE)
					{
						if (lKeyId == KEY_BACK)
						{
							pstContents->eCurDialogType = eNxFavChList_Back;
						}
						else
						{
							pstContents->eCurDialogType = eNxFavChList_Escape;
						}
						nx_menuFavChList_CreateSaveDialog(pstContents, (HUINT8*)RES_MESG_3221_ID);
					}
					else
					{
						if (lKeyId == KEY_BACK)
						{
							lPrevCurIdx = pstContents->lPrevCurIdx;
							ONDK_GWM_APP_Destroy(0);
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc,
															 0, 0, 0, lPrevCurIdx);
							return  ONDK_MESSAGE_BREAK;
						}
						else
						{
							ONDK_GWM_APP_Destroy(0);
							return  ONDK_MESSAGE_PASS;
						}
					}
				}
			}
		case KEY_OK:
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
		case KEY_OPT:
			if (pstContents->bMoveMode == FALSE)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_FavChList_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_FavChList_Option_Proc, 0, pstContents->nFocusViewID, pstContents->eCurrChGroupType, pstContents->eCurrFavGroupType);
			}
			return  ONDK_MESSAGE_BREAK;
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;
		default:
				NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
				return  ONDK_MESSAGE_PASS;
	}

	return  ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuFavChList_MsgGwmTimer(Nx_MenuFavChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
			ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
			nx_menuFavChList_UpdateCurrentTime(pstContents);
			NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuFavChList_MsgAppDlgClicked(Nx_MenuFavChListSetContent_t *pstContents, HINT32 lClickedBtn)
{
	HINT32		lPrevCurIdx = 0;

	nx_menuFavChList_DestroySaveDialog();

	switch(lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:		// yes
			nx_menuFavChList_SaveEditData(pstContents);
			if (pstContents->eCurDialogType == eNxFavChList_Back)
			{
				lPrevCurIdx = pstContents->lPrevCurIdx;
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc, 0, 0, 0, lPrevCurIdx);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		default:							// no
			if (pstContents->eCurDialogType == eNxFavChList_Back)
			{
				lPrevCurIdx = pstContents->lPrevCurIdx;
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc, 0, 0, 0, lPrevCurIdx);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;
	}

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuFavChList_MsgAppDlgCanceled(Nx_MenuFavChListSetContent_t *pstContents)
{
	nx_menuFavChList_DestroySaveDialog();

	return ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t   NX_PRISM_MENU_FavChList_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
		ONDK_Result_t                   eResult = ONDK_RET_OK;
		Nx_MenuFavChListSetContent_t   *pstContents = NULL;

		pstContents = nx_menuFavChList_GetContents();

		switch (lMessage)
		{
			case MSG_GWM_CREATE:
				NX_APP_Message("MSG_GWM_CREATE!\n");
				eResult = nx_menuFavChList_MsgGwmCreate(pstContents, p1, p2, p3);
				break;

			case MSG_GWM_KEYUP:
				break;
			case MSG_GWM_KEYDOWN:
				eResult = nx_menuFavChList_MsgGwmKeyDown(pstContents, p1, p2, p3);
				break;
			case MSG_GWM_CLICKED:
			case MSG_GWM_SCROLLRIGHT:
				NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
				nx_menuFavChList_FindNextFocus(pstContents, TRUE);
				break;

			case MSG_GWM_SCROLLUP:
				NX_APP_Message("MSG_GWM_SCROLLUP!\n");
				// nx_menuFavChList_FindNextFocus(pstContents, TRUE);
				break;

			case MSG_GWM_SCROLLDOWN:
				NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
				// nx_menuFavChList_FindNextFocus(pstContents, FALSE);
				break;

			case MSG_GWM_DESTROY:
				NX_APP_Message("MSG_GWM_DESTROY!\n");
				eResult = nx_menuFavChList_MsgGwmDestroy(pstContents, p1, p2, p3);
				break;

			case MSG_GWM_HAPI_MESSAGE:
				NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
				eResult = nx_menuFavChList_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
				break;

			case MSG_GWM_TIMER:
				NX_APP_Message("MSG_GWM_TIMER!\n");
				eResult = nx_menuFavChList_MsgGwmTimer(pstContents, p1, p2, p3);
				break;

			case MSG_APP_SETTING_VALUE_CHANGED:
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
				// nx_menuFavChList_ChangedSubItemValue(pstContents, p1);
				break;

			case MSG_GWM_OBJ_FOCUS_CHANGED:
				NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
				// eResult = nx_menuFavChList_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
				break;
			case MSG_GWM_LIST_SELECTED:
				NX_APP_Message("MSG_GWM_LIST_SELECTED!\n");
				if (pstContents->bMoveMode == TRUE)
				{
					HINT32 		i = 0, j = 0, nTotal = 0, nCurrIdx = 0, nSortCnt = 0;
					HUINT32 	tmpCheckNum = 0;
					Nx_ChannelData_t *pstData = NULL;

					/* Find real current index */
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstMoveChList, pstContents->pstMoveCursorInfo->nCurrIdx);
					nCurrIdx = HLIB_VECTOR_IndexOf (pstContents->pstCurFavList, pstData);
					nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);

					for (i = 0 ; i < nTotal ; i++)
					{

						pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, i);

						if (pstData->bSelect == TRUE)
						{
							if (i < nCurrIdx)
							{
								pstData->bSelect = FALSE;
								pstContents->ulFavSelectNum--;
								tmpCheckNum = pstData->stChannel.lcn;

								/* shift */
								for (j = i ; j < nCurrIdx ; j++)
								{
									Nx_ChannelData_t *pstTemp = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, (j + 1));

									HLIB_VECTOR_Replace(pstContents->pstCurFavList, j, pstTemp);
								}

								/* put */
								HLIB_VECTOR_Replace(pstContents->pstCurFavList, nCurrIdx, pstData);
								for (j = (nCurrIdx - 1) ; j >= i ; j--)
								{
									Nx_ChannelData_t *pstTemp1 = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, j);
									Nx_ChannelData_t *pstTemp2 = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, (j + 1));

									pstTemp2->ulCheckNum = pstTemp1->stChannel.lcn;
								}
								pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, i);
								pstData->ulCheckNum = tmpCheckNum;
								i--;
								nSortCnt = 1;
							}
							else if (i == nCurrIdx)
							{
								pstData->bSelect = FALSE;
								pstContents->ulFavSelectNum--;
								nSortCnt = 1;
							}
							else
							{
								pstData->bSelect = FALSE;
								pstContents->ulFavSelectNum--;
								tmpCheckNum = pstData->stChannel.lcn;

								/* shift */
								for (j = i - 1 ; j >= nCurrIdx + nSortCnt ; j--)
								{
									Nx_ChannelData_t *pstTemp = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, j);

									HLIB_VECTOR_Replace(pstContents->pstCurFavList, (j + 1), pstTemp);
								}

								/* put */
								HLIB_VECTOR_Replace(pstContents->pstCurFavList, nCurrIdx + nSortCnt, pstData);

								for (j = nCurrIdx + nSortCnt ; j < i ; j++)
								{
									Nx_ChannelData_t *pstTemp1 = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, j);
									Nx_ChannelData_t *pstTemp2 = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, j + 1);

									pstTemp1->ulCheckNum = pstTemp2->stChannel.lcn;
								}
								pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, i);
								pstData->ulCheckNum = tmpCheckNum;

								nSortCnt++;
							}
						}
					}
					pstContents->bMoveMode = FALSE;
					HLIB_VECTOR_Delete(pstContents->pstMoveChList);
					pstContents->pstMoveChList = NULL;
					NX_APP_Free(pstContents->pstMoveCursorInfo);
					pstContents->pstMoveCursorInfo = NULL;

					/* change list data */
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurFavList);
					nx_menuFavChList_UpdateSelectInfo(pstContents);
				}
				else
				{
					HINT32	nTotal = 0, nPageNum = 0, nStartIdx = 0;

					nTotal = HLIB_VECTOR_Length(pstContents->pstCurFavList);

					if (nTotal == 0 && pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_FAV_OBJ_ID)
					{
						break;
					}

					if (pstContents->nFocusViewID == NX_PRISM_UI_MENU_FAV_CHLIST_MAIN_OBJ_ID)
					{
						Nx_ChannelData_t *pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurMainList, p2);

						pstData->bSelect = !pstData->bSelect;
						if (pstData->bSelect == TRUE)
						{
							pstContents->ulMainSelectNum++;
						}
						else
						{
							pstContents->ulMainSelectNum--;
						}
					}
					else
					{
						Nx_ChannelData_t *pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurFavList, p2);

						pstData->bSelect = !pstData->bSelect;
						if (pstData->bSelect == TRUE)
						{
							pstContents->ulFavSelectNum++;
						}
						else
						{
							pstContents->ulFavSelectNum--;
						}
					}

					ONDK_GWM_List_GetTotalItem(pstContents->nFocusViewID, &nTotal);
					nPageNum = ONDK_GWM_List_GetNumOfPage(pstContents->nFocusViewID);
					nStartIdx = ONDK_GWM_List_GetStartIndex(pstContents->nFocusViewID);
					if (p2 + 1 == nTotal)
					{
						ONDK_GWM_List_SetCurrentIndex(pstContents->nFocusViewID, 0);
						ONDK_GWM_List_SetStartIndex(pstContents->nFocusViewID, 0);
					}
					else
					{
						ONDK_GWM_List_SetCurrentIndex(pstContents->nFocusViewID, p2 + 1);
						if (p2 + 1 == nStartIdx + nPageNum)
						{
							ONDK_GWM_List_SetStartIndex(pstContents->nFocusViewID, nStartIdx + 1);
						}
					}
					nx_menuFavChList_UpdateSelectInfo(pstContents);
				}
				break;
			case MSG_APP_POPUP_OPT_VALUE_CHANGED:
				NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
				nx_menuFavChList_ChangedPopupOptValue(pstContents, p2, p3);
				break;
			case MSG_APP_KEYBOARD_RESULT:
			{
				HCHAR   *pName = NULL;
				Nx_FavGrpData_t *pstFavGrp = NULL;

				pName = NX_PRISM_KeyBoard_GetResultString();
				pstFavGrp = HLIB_VECTOR_ItemAt(pstContents->pstFavGrpList, pstContents->eCurrFavGroupType - eNxChListGroup_Favorite1);
				HxSTD_sprintf(pstFavGrp->ucName, "%s", pName);
				pstFavGrp->bRename = TRUE;
				ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_DESTROY_KEYBOARD, (Handle_t)NULL, 0, 0, 0);
				NX_APP_Free(pName);

				nx_menuFavChList_UpdateGroupName(pstContents);
				break;
			}
			case MSG_APP_DLG_CLICKED:
			case MSG_APP_DLG_TIMEOUT:
				NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
				eResult = nx_menuFavChList_MsgAppDlgClicked(pstContents, p1);
				break;
			case MSG_APP_DLG_CANCELED:
				NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
				eResult = nx_menuFavChList_MsgAppDlgCanceled(pstContents);
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

		return  ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/*****************************************************************************/