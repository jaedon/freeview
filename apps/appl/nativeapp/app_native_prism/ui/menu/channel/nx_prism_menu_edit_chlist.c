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
 * @file	  		nx_prism_menu_edit_chlist.c
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
#define NX_PRISM_EDIT_CHLIST_ID					eMenuFrame_Window_Settings

#define NX_PRISM_EDIT_CHLIST_GROUP_ID			(NX_PRISM_EDIT_CHLIST_ID + 1)
#define NX_PRISM_EDIT_CHLIST_GROUP_X			210
#define NX_PRISM_EDIT_CHLIST_GROUP_Y			122
#define NX_PRISM_EDIT_CHLIST_GROUP_W			690
#define NX_PRISM_EDIT_CHLIST_GROUP_H			26

#define NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID		(NX_PRISM_EDIT_CHLIST_ID + 2)
#define NX_PRISM_EDIT_CHLIST_SELECT_INFO_X		920
#define NX_PRISM_EDIT_CHLIST_SELECT_INFO_Y		122
#define NX_PRISM_EDIT_CHLIST_SELECT_INFO_W		150
#define NX_PRISM_EDIT_CHLIST_SELECT_INFO_H		26

#define NX_PRISM_EDIT_CHLIST_CHANGE_GROUP_NUM	5

#define NX_PRISM_EDIT_CHLIST_SAVING_MSG_MINIMUM_NUM 100

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxEditChList_None = 0,
	eNxEditChList_Back,
	eNxEditChList_Escape,
} NxEditChList_DialogType_t;

typedef struct
{
	HBOOL 					bSelect;
	NX_Channel_t			stChannel;
	HUINT32					ulLcn;
	HINT32					nDxLcn;
	HCHAR					ucName[DxNAME_LEN];
	HBOOL 					bDeleted;
	HBOOL 					bLocked;
	DxVideoType_e			eHighDefinition;
	DxCasType_e				eCasType;
	HBOOL 					bTV;

	HBOOL 					bEditName;
	HBOOL 					bEditLock;
} Nx_EditChData_t;

typedef struct
{
	HUINT32					ulLcn;
	HCHAR 					ucName[DxNAME_LEN];
	HINT32					nCurrIdx;
} Nx_MoveCursorInfo_t;

typedef struct tag_NX_MENUEDITCHLIST_SET_CONTENT
{
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8					ucSelectInfo[MAX_ELEMENT_TEXT_LENGTH];
	HxVector_t				*pstDataPool; /* Include TV and Radio data */
	HxVector_t 				*pstCurGroupList;
	HxVector_t		 		*pstMoveChList;


	HBOOL					bIsEdit;
	NX_ChListGroup_t		eCurrChGroupType;

	HUINT32					ulSelectNum;
	HBOOL 					bMoveMode;
	Nx_MoveCursorInfo_t		*pstMoveCursorInfo;
	HINT32					nDeleteCnt;
	HINT32 					nUpdateCnt;
	HINT32					nRcvDeleteMsgCnt;
	HINT32					nRcvUpdateMsgCnt;
	NxEditChList_DialogType_t eCurDialogType;

	HINT32					lPrevCurIdx;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_MenuEditChListSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuEditChListSetContent_t	s_stEditChListContent;
static NXUI_DialogInstance_t			s_stEditChList_DialogInstance;
static NXUI_DialogInstance_t			s_stEditChList_ProcessInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_MenuEditChListSetContent_t* nx_menuEditChList_GetContents(void);
static inline NXUI_DialogInstance_t* nx_menuEditChList_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuEditChList_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuEditChList_CreateSaveDialog(Nx_MenuEditChListSetContent_t *pstContents, HUINT8 *szStrID);
static ONDK_Result_t	nx_menuEditChList_DestroySaveDialog (void);
static ONDK_Result_t	nx_menuEditChList_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuEditChList_CreateProcessingMsg(void);
static ONDK_Result_t	nx_menuEditChList_DestroyProcessingMsg(void);
static void				nx_menuEditChList_CreateChList(HxVector_t *pstChannelList, HUINT32 ulTotal, GWM_ObjDraw_t objDrawMethod);
static ONDK_Result_t	nx_menuEditChList_DrawChList(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_menuEditChList_DrawCursor_A1(ONDKSurfaceHandle *screen, Nx_EditChData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HBOOL bIsFocus, int bUiTop);
static ONDK_Result_t	nx_menuEditChList_DrawMovingCursor(ONDKSurfaceHandle *screen, ONDK_Rect_t *stListRegion, HINT32 nOffsetY,  int bUiTop);
static void				nx_menuEditChList_UpdateCurrentTime(Nx_MenuEditChListSetContent_t *pstContents);
static ONDK_Result_t	nx_menuEditChList_LoadAllChannelList(Nx_MenuEditChListSetContent_t *pstContents);
static ONDK_Result_t	nx_menuEditChList_LoadChannelListByGroup(Nx_MenuEditChListSetContent_t *pstContents, NX_ChListGroup_t eChGroupType);
static ONDK_Result_t	nx_menuEditChList_InitSettingsItems(Nx_MenuEditChListSetContent_t *pstContents);
static ONDK_Result_t	nx_menuEditChList_DrawDefaultUI(Nx_MenuEditChListSetContent_t *pstContents);
static void				nx_menuEditChList_UpdateSelectInfo(Nx_MenuEditChListSetContent_t *pstContents);
static void				nx_menuEditChList_UpdateGroupName(NX_ChListGroup_t eChGroupType);
static ONDK_Result_t	nx_menuEditChList_ChangedPopupOptValue(Nx_MenuEditChListSetContent_t *pstContents, eNxUiUtil_OptItem_t eCurOpt, NX_ChListGroup_t eChGroupType);
static HBOOL 			nx_menuEditChList_FindLockData(void *data, void *userData);
static ONDK_Result_t 	nx_menuEditChList_CheckEditData(Nx_MenuEditChListSetContent_t *pstContents, HINT32 *pnDeleteCnt, HINT32 *pnUpdateCnt);
static ONDK_Result_t	nx_menuEditChList_SaveEditData(Nx_MenuEditChListSetContent_t *pstContents);
static ONDK_Result_t	nx_menuEditChList_MsgGwmCreate(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuEditChList_MsgGwmDestroy(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuEditChList_MsgGwmHapiMsg(Nx_MenuEditChListSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuEditChList_MsgGwmKeyDown(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuEditChList_MsgGwmTimer(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuEditChList_MsgAppDlgClicked(Nx_MenuEditChListSetContent_t *pstContents, HINT32 lClickedBtn);
static ONDK_Result_t	nx_menuEditChList_MsgAppDlgCanceled(Nx_MenuEditChListSetContent_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuEditChListSetContent_t* nx_menuEditChList_GetContents(void)
{
	return	&s_stEditChListContent;
}


static inline NXUI_DialogInstance_t* nx_menuEditChList_GetDialogPopupContents(void)
{
	return &s_stEditChList_DialogInstance;
}


static ONDK_Result_t	nx_menuEditChList_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuEditChList_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuEditChList_CreateSaveDialog(Nx_MenuEditChListSetContent_t *pstContents, HUINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuEditChList_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuEditChList_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuEditChList_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_DestroySaveDialog (void)
{
	ONDK_GWM_APP_Destroy(nx_menuEditChList_DialogProc);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stEditChList_ProcessInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuEditChList_CreateProcessingMsg(void)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stEditChList_ProcessInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));
	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(RES_SAVING_ID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuEditChList_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuEditChList_ProcessingMsgProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuEditChList_DestroyProcessingMsg(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuEditChList_ProcessingMsgProc);
}


static void		nx_menuEditChList_CreateChList(HxVector_t *pstChannelList, HUINT32 ulTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t 	stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_X, NX_PRISM_UI_MENU_EDIT_CHLIST_BG_Y, NX_PRISM_UI_MENU_EDIT_CHLIST_BG_W, NX_PRISM_UI_MENU_EDIT_CHLIST_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_EDIT_CHLIST_BG_ID, &stRect, COL(C_G_Gong02));

	// Create edit channel list
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_X, NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_Y, NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_W, NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_H);
	ONDK_GWM_List_Create(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, &stRect, "EDIT CHANNEL LIST", NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, ulTotal, NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstChannelList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
}


static ONDK_Result_t	nx_menuEditChList_DrawChList(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */
	HINT32			nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32			nCellHeight = 0, nCount = 0, nOffsetY = 0;
	LIST_OBJECT		objList = (LIST_OBJECT)object;
	HxVector_t 		*pstDataList = NULL;
	Nx_EditChData_t *pstData = NULL;

	ONDK_Color_t		textFontColor = 0x00000000, bgColor = 0x00000000;ONDKFontHandle		*hFontHandle = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL;
	ONDK_Rect_t			stListRegion = {0, 0, 0, 0};
	HINT32				i = 0, nDataIdx = 0;
	Nx_MenuEditChListSetContent_t* pstContents = NULL;

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

	pstContents = nx_menuEditChList_GetContents();

	if (pstContents->bMoveMode == TRUE)
	{
		HINT32 	nDrawItemCnt = 0;
		HBOOL 	bDrawCursor = FALSE;
		HINT32	nCursorOffset = 0, nTmpCnt = nCount, nSelectLoc = 0;

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
			nx_menuEditChList_DrawCursor_A1(screen, pstData, &stListRegion, nOffsetY, hFontHandle, textFontColor, bgColor, FALSE, bUiTop);
			nOffsetY += nCellHeight;
			nDrawItemCnt++;
		}
		if (bDrawCursor == TRUE)
		{
			nx_menuEditChList_DrawMovingCursor(screen, &stListRegion, nCursorOffset, bUiTop);
		}
		else
		{
			nx_menuEditChList_DrawMovingCursor(screen, &stListRegion, nOffsetY, bUiTop);
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

			nx_menuEditChList_DrawCursor_A1(screen, pstData, &stListRegion, nOffsetY, hFontHandle, textFontColor, bgColor, (nDataIdx == nCurrIdx), bUiTop);
			nOffsetY += nCellHeight;
			bgColor = 0x00000000;
		}
	}

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_DrawCursor_A1(ONDKSurfaceHandle *screen, Nx_EditChData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HBOOL bIsFocus, int bUiTop)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	HCHAR 			pText[DxNAME_LEN] = {0, };
	HINT32 			nCheckBoxX = 0, nCheckBoxY = 0, nIconX = 0, nIconY = 0;

	HxSTD_sprintf(pText, "%d %s", pstData->ulLcn, pstData->ucName);

	if (bIsFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, bgColor);
	}

	nCheckBoxX = stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_X;
	nCheckBoxY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_Y;
	stRect = ONDK_Rect(nCheckBoxX, nCheckBoxY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_CHECKBOX_H);
	if (bIsFocus == TRUE)
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
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_ON_N_PNG);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_112_00_CHECK01_OFF_N_PNG);
		}

	}

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_X, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_Y, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

	nIconX = stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_X;
	nIconY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_Y;
	switch(pstData->eCasType)
	{
		case eDxCAS_TYPE_ALLCAS:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_NAGRA:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_NDS:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_VIACCESS:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_IRDETO:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_CRYPTOWORKS:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		case eDxCAS_TYPE_CONAX:
			stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
			nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_202_00_CAS_MULTI_PNG);
			break;
		default:
			NX_APP_Info("It is a cas type that does not support the icon. (casType: %d)\n", pstData->eCasType);
			break;
	}
	if (pstData->eHighDefinition == eDxVIDEO_TYPE_HD)
	{
		stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
		nIconX -= (NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_MG + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W);
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_203_00_RESOLUTION_HD_PNG);
	}
	if (pstData->bLocked == TRUE)
	{
		stRect = ONDK_Rect(nIconX, nIconY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_ICON_H);
		ONDK_GUICOM_IMG_Blit(screen, stRect, RES_211_00_LOCK_PNG);
	}
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_DrawMovingCursor(ONDKSurfaceHandle *screen, ONDK_Rect_t *stListRegion, HINT32 nOffsetY,  int bUiTop)
{
	Nx_MenuEditChListSetContent_t* pstContents = NULL;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	HCHAR 			pText[DxNAME_LEN] = {0, };
	HINT32 			nMoveImgX = 0, nMoveImgY = 0;

	pstContents = nx_menuEditChList_GetContents();

	if (bUiTop > 0)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x + stListRegion->w - NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H - NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, stListRegion->w, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_100));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, COL(C_G_Base01_100));
	}
	else
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x + stListRegion->w - NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H - NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, stListRegion->w, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, COL(C_G_Base01_20));
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, NX_PRISM_UI_MENU_EDIT_CHLIST_MOVING_ITEM_STROKE, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_H, COL(C_G_Base01_20));
	}

	if (pstContents->ulSelectNum < 2)
	{
		nMoveImgX = stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_X;
		nMoveImgY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_Y;
		stRect = ONDK_Rect(nMoveImgX, nMoveImgY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_IMG_H);
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
		nMoveImgX = stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_X;
		nMoveImgY = stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_Y;
		stRect = ONDK_Rect(nMoveImgX, nMoveImgY, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_W, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_MOVE_GROUP_IMG_H);
		if (bUiTop > 0)
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_GROUP_C_PNG);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stRect, RES_430_00_SET_MOVE_GROUP_H_PNG);
		}
	}

	if (pstContents->ulSelectNum < 2)
	{
		ONDKFontHandle		*hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
		ONDK_Color_t 		textFontColor = 0x00000000;

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
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_X, stListRegion->y + nOffsetY + NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_OFFSET_Y, NX_PRISM_UI_MENU_EDIT_CHLIST_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
	}

	return ONDK_RET_OK;
}


static void		nx_menuEditChList_UpdateCurrentTime(Nx_MenuEditChListSetContent_t *pstContents)
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


static ONDK_Result_t	nx_menuEditChList_LoadAllChannelList(Nx_MenuEditChListSetContent_t *pstContents)
{
	HINT32					i = 0;
	HERROR 					herror = ERR_FAIL;
	HUINT32					ulExtraFilter = 0, nTVTotal = 0, nRadioTotal;
	NX_Channel_t 			*pstTVChList = NULL, *pstRadioChList = NULL;

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

	/* Init data pool */
	pstContents->pstDataPool = HLIB_VECTOR_NewEasy((nTVTotal + nRadioTotal), NULL, NULL);

	for (i = 0 ; i < nTVTotal ; i++)
	{
		Nx_EditChData_t *pstData = (Nx_EditChData_t*)NX_APP_Malloc(sizeof(Nx_EditChData_t));
		DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(pstTVChList[i].svcUid);

		HxSTD_memcpy(&(pstData->stChannel), &pstTVChList[i], sizeof(NX_Channel_t));
		pstData->ulLcn = pstTVChList[i].lcn;
		pstData->nDxLcn = pstService->lcn;
		HxSTD_sprintf(pstData->ucName, "%s", pstService->name);
		pstData->bLocked = pstService->locked;
		pstData->eHighDefinition = pstService->highDefinition;
		pstData->eCasType = pstService->casType;
		pstData->bTV = TRUE;
		pstData->bSelect = FALSE;
		pstData->bDeleted = FALSE;
		pstData->bEditName = FALSE;
		pstData->bEditLock = FALSE;

		HLIB_VECTOR_Add(pstContents->pstDataPool, pstData);
	}

	for (i = 0 ; i < nRadioTotal ; i++)
	{
		Nx_EditChData_t *pstData = (Nx_EditChData_t*)NX_APP_Malloc(sizeof(Nx_EditChData_t));
		DxService_t *pstService = NX_CHANNEL_FindServiceBySvcUid(pstRadioChList[i].svcUid);


		HxSTD_memcpy(&(pstData->stChannel), &pstRadioChList[i], sizeof(NX_Channel_t));
		pstData->ulLcn = pstRadioChList[i].lcn;
		pstData->nDxLcn = pstService->lcn;
		HxSTD_sprintf(pstData->ucName, "%s", pstService->name);
		pstData->bLocked = pstService->locked;
		pstData->eHighDefinition = pstService->highDefinition;
		pstData->eCasType = pstService->casType;
		pstData->bTV = FALSE;
		pstData->bSelect = FALSE;
		pstData->bDeleted = FALSE;
		pstData->bEditName = FALSE;
		pstData->bEditLock = FALSE;

		HLIB_VECTOR_Add(pstContents->pstDataPool, pstData);
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_LoadChannelListByGroup(Nx_MenuEditChListSetContent_t *pstContents, NX_ChListGroup_t eChGroupType)
{
	HINT32 		nSize = 0;
	Nx_EditChData_t *pstData = NULL;

	if (pstContents->pstCurGroupList != NULL)
	{
		HLIB_VECTOR_Delete(pstContents->pstCurGroupList);
	}

	switch(eChGroupType)
	{
		case eNxChListGroup_TV:
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->bTV == TRUE && pstData->bDeleted == FALSE)
				{
					nSize++;
				}
			});

			pstContents->pstCurGroupList = HLIB_VECTOR_NewEasy(nSize, NULL, NULL);
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->bTV == TRUE && pstData->bDeleted == FALSE)
				{
					HLIB_VECTOR_Add(pstContents->pstCurGroupList, pstData);
				}
			});
			break;
		case eNxChListGroup_RADIO:
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->bTV == FALSE && pstData->bDeleted == FALSE)
				{
					nSize++;
				}
			});

			pstContents->pstCurGroupList = HLIB_VECTOR_NewEasy(nSize, NULL, NULL);
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->bTV == FALSE && pstData->bDeleted == FALSE)
				{
					HLIB_VECTOR_Add(pstContents->pstCurGroupList, pstData);
				}
			});
			break;
		case eNxChListGroup_HDTV :
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eHighDefinition == eDxVIDEO_TYPE_HD && pstData->bDeleted == FALSE)
				{
					nSize++;
				}
			});

			pstContents->pstCurGroupList = HLIB_VECTOR_NewEasy(nSize, NULL, NULL);
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eHighDefinition == eDxVIDEO_TYPE_HD && pstData->bDeleted == FALSE)
				{
					HLIB_VECTOR_Add(pstContents->pstCurGroupList, pstData);
				}
			});
			break;
		case eNxChListGroup_FTA :
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eCasType == eDxCAS_TYPE_FTA && pstData->bDeleted == FALSE)
				{
					nSize++;
				}
			});

			pstContents->pstCurGroupList = HLIB_VECTOR_NewEasy(nSize, NULL, NULL);
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eCasType == eDxCAS_TYPE_FTA && pstData->bDeleted == FALSE)
				{
					HLIB_VECTOR_Add(pstContents->pstCurGroupList, pstData);
				}
			});
			break;
		case eNxChListGroup_CAS :
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eCasType != eDxCAS_TYPE_FTA && pstData->bDeleted == FALSE)
				{
					nSize++;
				}
			});

			pstContents->pstCurGroupList = HLIB_VECTOR_NewEasy(nSize, NULL, NULL);
			HxVECTOR_EACH(pstContents->pstDataPool, Nx_EditChData_t *, pstData, {
				if (pstData->eCasType != eDxCAS_TYPE_FTA && pstData->bDeleted == FALSE)
				{
					HLIB_VECTOR_Add(pstContents->pstCurGroupList, pstData);
				}
			});
			break;
		default:
			return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}


static HBOOL 			nx_menuEditChList_FindLockData(void *data, void *userData)
{
	Nx_EditChData_t *pstData = (Nx_EditChData_t *)data;
	HBOOL 	bLocked = (HBOOL)userData;

	if (pstData->bLocked == bLocked)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


static ONDK_Result_t 	nx_menuEditChList_CheckEditData(Nx_MenuEditChListSetContent_t *pstContents, HINT32 *pnDeleteCnt, HINT32 *pnUpdateCnt)
{
	Nx_EditChData_t *pstData = NULL;
	HINT32 			i = 0, nTotal = 0, nDeleteCnt = 0, nUpdateCnt = 0;

	nTotal = HLIB_VECTOR_Length(pstContents->pstDataPool);
	for (i = 0 ; i < nTotal ; i++)
	{
		pstData = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, i);
		if (pstData->stChannel.lcn != pstData->ulLcn)
		{
			nUpdateCnt++;
			continue;
		}
		if (pstData->bEditName == TRUE)
		{
			nUpdateCnt++;
			continue;
		}
		if (pstData->bDeleted == TRUE)
		{
			nDeleteCnt++;
			continue;
		}
		if (pstData->bEditLock == TRUE)
		{
			nUpdateCnt++;
			continue;
		}
	}
	*pnDeleteCnt = nDeleteCnt;
	*pnUpdateCnt = nUpdateCnt;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_SaveEditData(Nx_MenuEditChListSetContent_t *pstContents)
{
	HINT32				i = 0, nTotal = 0;
	Nx_EditChData_t		*pstData = NULL;
	DxService_t			*pstService = NULL;
	HBOOL 				bEdit = FALSE;

	NX_CHANNEL_StopChannel();
	if (pstContents->nDeleteCnt >= NX_PRISM_EDIT_CHLIST_SAVING_MSG_MINIMUM_NUM)
	{
		nx_menuEditChList_CreateProcessingMsg();
	}
	nTotal = HLIB_VECTOR_Length(pstContents->pstDataPool);
	for (i = 0 ; i < nTotal ; i++)
	{
		bEdit = FALSE;
		pstData = HLIB_VECTOR_ItemAt(pstContents->pstDataPool, i);
		pstService = NX_CHANNEL_FindServiceBySvcUid(pstData->stChannel.svcUid);
		if (pstData->bDeleted == TRUE)
		{
			NX_CHANNEL_RemoveService(pstService);
			continue;
		}
		if (pstData->stChannel.lcn != pstData->ulLcn)
		{
			pstService->lcn = pstData->nDxLcn;
			bEdit = TRUE;
		}
		if (pstData->bEditName == TRUE)
		{
			HxSTD_sprintf(pstService->name, "%s", pstData->ucName);
			pstService->renamed = TRUE;
			bEdit = TRUE;
		}
		if (pstData->bEditLock == TRUE)
		{
			pstService->locked = pstData->bLocked;
			bEdit = TRUE;
		}
		if (bEdit == TRUE)
		{
			NX_CHANNEL_UpdateService(pstService);
		}
	}
	NX_PRISM_LIVE_ServiceStartByLastService();
	if (pstContents->nDeleteCnt >= NX_PRISM_EDIT_CHLIST_SAVING_MSG_MINIMUM_NUM)
	{
		nx_menuEditChList_DestroyProcessingMsg();
	}

	ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_RELOAD_CHANNELLIST_ALL, 0, 0, 0, 0);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_InitSettingsItems(Nx_MenuEditChListSetContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_EDIT_CHANNEL_LIST_01_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->eCurrChGroupType = eNxChListGroup_TV;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_DrawDefaultUI(Nx_MenuEditChListSetContent_t *pstContents)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	HINT32 			nTotal = 0;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

    nx_menuEditChList_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	/* Draw - Group name */
	stRect = ONDK_Rect(NX_PRISM_EDIT_CHLIST_GROUP_X, NX_PRISM_EDIT_CHLIST_GROUP_Y, NX_PRISM_EDIT_CHLIST_GROUP_W, NX_PRISM_EDIT_CHLIST_GROUP_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_EDIT_CHLIST_GROUP_ID, &stRect, (HCHAR*)ONDK_GetString(RES_TV_01_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_EDIT_CHLIST_GROUP_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_EDIT_CHLIST_GROUP_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_EDIT_CHLIST_GROUP_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_EDIT_CHLIST_GROUP_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
	/* Draw - Select Info */
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
	HxSTD_sprintf((char*)pstContents->ucSelectInfo, "%d / %d", pstContents->ulSelectNum, nTotal);
	stRect = ONDK_Rect(NX_PRISM_EDIT_CHLIST_SELECT_INFO_X, NX_PRISM_EDIT_CHLIST_SELECT_INFO_Y, NX_PRISM_EDIT_CHLIST_SELECT_INFO_W, NX_PRISM_EDIT_CHLIST_SELECT_INFO_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, &stRect, pstContents->ucSelectInfo);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	/* Draw OPT Button */
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_X, NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_Y, NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_W, NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MENU_EDIT_CHLIST_OPT_ID, NX_PRISM_COMP_DrawEdit_HelpR);

	return	ONDK_RET_OK;
}


static void				nx_menuEditChList_UpdateSelectInfo(Nx_MenuEditChListSetContent_t *pstContents)
{
	HINT32 	nTotal = 0;

	nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
	HxSTD_sprintf(pstContents->ucSelectInfo, "%d / %d", pstContents->ulSelectNum, nTotal);
	ONDK_GWM_COM_Text_SetText(NX_PRISM_EDIT_CHLIST_SELECT_INFO_ID, pstContents->ucSelectInfo);
}


static void				nx_menuEditChList_UpdateGroupName(NX_ChListGroup_t eChGroupType)
{
	HCHAR *pGroupName = "";
	switch(eChGroupType)
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
	ONDK_GWM_COM_Text_SetText(NX_PRISM_EDIT_CHLIST_GROUP_ID, pGroupName);
}


static ONDK_Result_t	nx_menuEditChList_ChangedPopupOptValue(Nx_MenuEditChListSetContent_t *pstContents, eNxUiUtil_OptItem_t eCurOpt, NX_ChListGroup_t eChGroupType)
{
	Nx_EditChData_t *pstData = NULL;
	HINT32			nCurGroupTotal = 0;

	nCurGroupTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
	if (nCurGroupTotal == 0 && eCurOpt != eNxUtil_OptItem_Group)
	{
		return ONDK_RET_OK;
	}

	switch(eCurOpt)
	{
		case eNxUtil_OptItem_Delete:
			if (pstContents->ulSelectNum == 0)
			{
				HINT32 nCurrIdx = 0, nTotal = 0;
				Nx_EditChData_t *pstData;

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				ONDK_GWM_List_GetTotalItem(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, &nTotal);
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
				pstData->bDeleted = TRUE;
				HLIB_VECTOR_Remove(pstContents->pstCurGroupList, nCurrIdx);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal - 1, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
			}
			else
			{
				HINT32 i = 0, nTotal = 0;
				Nx_EditChData_t *pstData = NULL;

				nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
				for (i = 0 ; i < nTotal ; i++)
				{
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, i);
					if (pstData->bSelect == TRUE)
					{
						pstData->bDeleted = TRUE;
						pstData->bSelect = FALSE;
						HLIB_VECTOR_Remove(pstContents->pstCurGroupList, i);
						i--;
						nTotal--;
					}
				}

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
				pstContents->ulSelectNum = 0;
				nx_menuEditChList_UpdateSelectInfo(pstContents);
			}
			break;
		case eNxUtil_OptItem_Lock:
			if (pstContents->ulSelectNum == 0)
			{
				HINT32 nCurrIdx = 0;
				Nx_EditChData_t *pstData = NULL;

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
				pstData->bLocked = TRUE;
				pstData->bEditLock = !pstData->bEditLock;
			}
			else
			{
				HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
					if (pstData->bSelect == TRUE)
					{
						pstData->bLocked = TRUE;
						pstData->bEditLock = !pstData->bEditLock;
						pstData->bSelect = FALSE;
					}
				});
				pstContents->ulSelectNum = 0;
			}
			break;
		case eNxUtil_OptItem_UnLock:
			if (pstContents->ulSelectNum == 0)
			{
				HINT32 nCurrIdx = 0;
				Nx_EditChData_t *pstData = NULL;

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
				pstData->bLocked = FALSE;
				pstData->bEditLock = !pstData->bEditLock;
			}
			else
			{
				HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
					if (pstData->bSelect == TRUE)
					{
						pstData->bLocked = FALSE;
						pstData->bEditLock = !pstData->bEditLock;
						pstData->bSelect = FALSE;
					}
				});
				pstContents->ulSelectNum = 0;
			}
			break;
		case eNxUtil_OptItem_Rename:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, 0 /*KEYBOARD_INPUT_NORMAL*/, 0);
			break;
		case eNxUtil_OptItem_Move:
			{
				HBOOL 	bFirstCheck = FALSE;
				HINT32	nTotal = 0, nStartIdx = 0, nNumOfPage = 0, nCount = 0, nCurrIdx = 0, i = 0;
				Nx_EditChData_t *pstData = NULL;

				nStartIdx = ONDK_GWM_List_GetStartIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				ONDK_GWM_List_GetTotalItem(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, &nTotal);
				nNumOfPage = ONDK_GWM_List_GetNumOfPage(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);

				if (nTotal > nNumOfPage)
				{
					nCount = nNumOfPage;
				}
				else
				{
					nCount = nTotal;
				}

				if (pstContents->ulSelectNum > 0)
				{
					i = 0;
					HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
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
					nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
					pstData->bSelect = TRUE;
					pstContents->ulSelectNum++;
				}

				pstContents->pstMoveChList = HLIB_VECTOR_NewEasy((nTotal - pstContents->ulSelectNum + 1), NULL, NULL);
				bFirstCheck = FALSE;
				pstContents->bMoveMode = TRUE;

				for (i = 0 ; i < nTotal ; i++)
				{
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, i);
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

				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), (nTotal - pstContents->ulSelectNum + 1), (GWM_LIST_ITEMS)pstContents->pstMoveChList);
				ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, nStartIdx);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, nCurrIdx);

				pstContents->pstMoveCursorInfo = (Nx_MoveCursorInfo_t *)NX_APP_Malloc(sizeof(Nx_MoveCursorInfo_t));
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
				pstContents->pstMoveCursorInfo->ulLcn = pstData->ulLcn;
				HxSTD_sprintf(pstContents->pstMoveCursorInfo->ucName, "%s", pstData->ucName);
				break;
			}
		case eNxUtil_OptItem_Group:
			{
				HINT32 		nTotal = 0;

				nx_menuEditChList_LoadChannelListByGroup(pstContents, eChGroupType);
				HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
					pstData->bSelect = FALSE;
				});

				pstContents->ulSelectNum = 0;
				pstContents->eCurrChGroupType = eChGroupType;
				nx_menuEditChList_UpdateGroupName(eChGroupType);
				nx_menuEditChList_UpdateSelectInfo(pstContents);
				nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, 0);
			}
			break;
		case eNxUtil_OptItem_SelectAll:
			HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
					pstData->bSelect = TRUE;
			});
			pstContents->ulSelectNum = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
			nx_menuEditChList_UpdateSelectInfo(pstContents);
			break;
		case eNxUtil_OptItem_DeSelectAll:
			HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
					pstData->bSelect = FALSE;
			});
			pstContents->ulSelectNum = 0;
			nx_menuEditChList_UpdateSelectInfo(pstContents);
			break;

		default:
			NX_APP_Print("nx_menuEditChList_ChangedPopupOptValue() Not used Current Option Item(%d)\n", eCurOpt);
			break;
	}
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_MsgGwmCreate(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 		nTotal = 0;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Load Menu Item Value
	nx_menuEditChList_LoadAllChannelList(pstContents);
	nx_menuEditChList_LoadChannelListByGroup(pstContents, eNxChListGroup_TV);

	pstContents->lPrevCurIdx = p3;
	// Init Menu Item
	nx_menuEditChList_InitSettingsItems(pstContents);

	// Draw Menu Item
	nx_menuEditChList_DrawDefaultUI(pstContents);

	// Create channel list
	nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
	nx_menuEditChList_CreateChList(pstContents->pstCurGroupList, nTotal, (GWM_ObjDraw_t)nx_menuEditChList_DrawChList);

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


static ONDK_Result_t	nx_menuEditChList_MsgGwmDestroy(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	if (pstContents->pstCurGroupList != NULL)
	{
		HLIB_VECTOR_Delete(pstContents->pstCurGroupList);
	}
	if (pstContents->pstDataPool != NULL)
	{
		HLIB_VECTOR_Delete(pstContents->pstDataPool);
	}
	if (pstContents->pstMoveCursorInfo != NULL)
	{
		NX_APP_Free(pstContents->pstMoveCursorInfo);
	}
	if (pstContents->pstMoveChList != NULL)
	{
		NX_APP_Free(pstContents->pstMoveChList);
	}
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuEditChListSetContent_t));

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Option_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_MENU_EditChList_Option_Proc);
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_MsgGwmHapiMsg(Nx_MenuEditChListSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuEditChList_MsgGwmKeyDown(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
			{
				if (pstContents->bMoveMode == TRUE)
				{
					HINT32 nTotal = 0, i = 0, nCurrIdx = 0;
					HBOOL bFirstCheck = FALSE;
					Nx_EditChData_t *pstData = NULL;

					nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
					pstContents->bMoveMode = FALSE;
					HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
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
					pstContents->ulSelectNum = 0;
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, nCurrIdx);
					nx_menuEditChList_UpdateSelectInfo(pstContents);
				}
				else
				{
					nx_menuEditChList_CheckEditData(pstContents, &pstContents->nDeleteCnt, &pstContents->nUpdateCnt);
					if (pstContents->nDeleteCnt > 0 || pstContents->nUpdateCnt > 0)
					{
						pstContents->eCurDialogType = eNxEditChList_Back;
						nx_menuEditChList_CreateSaveDialog(pstContents, (HUINT8*)RES_MESG_3221_ID);
					}
					else
					{
						lPrevCurIdx = pstContents->lPrevCurIdx;
						ONDK_GWM_APP_Destroy(0);
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc,
										 0, 0, 0, lPrevCurIdx);
					}
				}
			}

			return	ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
			{
				nx_menuEditChList_CheckEditData(pstContents, &pstContents->nDeleteCnt, &pstContents->nUpdateCnt);
				if (pstContents->nDeleteCnt > 0 || pstContents->nUpdateCnt > 0)
				{
					pstContents->eCurDialogType = eNxEditChList_Escape;
					nx_menuEditChList_CreateSaveDialog(pstContents, (HUINT8*)RES_MESG_3221_ID);
					return	ONDK_MESSAGE_BREAK;
				}
				else
				{
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_PASS;
				}
			}
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

		case KEY_OPT:
			if (pstContents->bMoveMode == FALSE)
			{
				if (pstContents->ulSelectNum == 0)
				{
					HINT32 nCurrIdx = 0, nCurGroupTotal = 0;
					Nx_EditChData_t *pstData = NULL;

					nCurGroupTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
					if (nCurGroupTotal == 0)
					{
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_EditChList_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Option_Proc, 0, pstContents->ulSelectNum, pstContents->eCurrChGroupType, FALSE);
					}
					else
					{
						nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
						pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_EditChList_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Option_Proc, 0, pstContents->ulSelectNum, pstContents->eCurrChGroupType, pstData->bLocked);
					}
				}
				else
				{
					HBOOL 	bLocked = FALSE;
					NX_Channel_t *pstData = NULL;


					pstData = HLIB_VECTOR_Find(pstContents->pstCurGroupList, nx_menuEditChList_FindLockData, (void*)TRUE);
					if (pstData != NULL)
					{
						bLocked = TRUE;
					}
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_EditChList_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Option_Proc, 0, pstContents->ulSelectNum, pstContents->eCurrChGroupType, bLocked);

				}
			}
			return	ONDK_MESSAGE_BREAK;
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;
		case KEY_TVRADIO:
			{
				HINT32 		nTotal = 0;
				Nx_EditChData_t *pstData = NULL;

				if (pstContents->eCurrChGroupType == eNxChListGroup_TV)
				{
					nx_menuEditChList_LoadChannelListByGroup(pstContents, eNxChListGroup_RADIO);
					HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
						pstData->bSelect = FALSE;
					});

					pstContents->ulSelectNum = 0;
					pstContents->eCurrChGroupType = eNxChListGroup_RADIO;
					nx_menuEditChList_UpdateGroupName(eNxChListGroup_RADIO);
					nx_menuEditChList_UpdateSelectInfo(pstContents);
					nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, 0);
				}
				else
				{
					nx_menuEditChList_LoadChannelListByGroup(pstContents, eNxChListGroup_TV);
					HxVECTOR_EACH(pstContents->pstCurGroupList, Nx_EditChData_t *, pstData, {
						pstData->bSelect = FALSE;
					});

					pstContents->ulSelectNum = 0;
					pstContents->eCurrChGroupType = eNxChListGroup_TV;
					nx_menuEditChList_UpdateGroupName(eNxChListGroup_TV);
					nx_menuEditChList_UpdateSelectInfo(pstContents);
					nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, 0);
				}

				return	ONDK_MESSAGE_BREAK;
			}
		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_MsgGwmTimer(Nx_MenuEditChListSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuEditChList_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuEditChList_MsgAppDlgClicked(Nx_MenuEditChListSetContent_t *pstContents, HINT32 lClickedBtn)
{
	HINT32		lPrevCurIdx = 0;

	nx_menuEditChList_DestroySaveDialog();

	switch(lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:		// yes
			nx_menuEditChList_SaveEditData(pstContents);
			if (pstContents->eCurDialogType == eNxEditChList_Back)
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
			if (pstContents->eCurDialogType == eNxEditChList_Back)
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


static ONDK_Result_t	nx_menuEditChList_MsgAppDlgCanceled(Nx_MenuEditChListSetContent_t *pstContents)
{
	nx_menuEditChList_DestroySaveDialog();

	return ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_EditChList_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuEditChListSetContent_t	*pstContents = NULL;

	pstContents = nx_menuEditChList_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuEditChList_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuEditChList_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			// nx_menuEditChList_FindNextFocus(pstContents, TRUE);
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			// nx_menuEditChList_FindNextFocus(pstContents, FALSE);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuEditChList_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuEditChList_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuEditChList_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			// nx_menuEditChList_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_FOCUS_CHANGED!\n");
			// eResult = nx_menuEditChList_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED!\n");
			if (pstContents->bMoveMode == TRUE)
			{
				HINT32 		i = 0, j = 0, nTotal = 0, nCurrIdx = 0, nSortCnt = 0, tmpDxLcn = 0;
				HUINT32	 	tmpLcn = 0;
				Nx_EditChData_t *pstData = NULL;

				/* Find real current index */
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstMoveChList, pstContents->pstMoveCursorInfo->nCurrIdx);
				nCurrIdx = HLIB_VECTOR_IndexOf (pstContents->pstCurGroupList, pstData);
				nTotal = HLIB_VECTOR_Length(pstContents->pstCurGroupList);

				for (i = 0 ; i < nTotal ; i++)
				{
					pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, i);

					if (pstData->bSelect == TRUE)
					{
						if (i < nCurrIdx)
						{
							pstData->bSelect = FALSE;
							pstContents->ulSelectNum--;
							tmpLcn = pstData->ulLcn;
							tmpDxLcn = pstData->nDxLcn;

							/* shift */
							for (j = i ; j < nCurrIdx ; j++)
							{
								Nx_EditChData_t *pstTemp = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, (j + 1));

								HLIB_VECTOR_Replace(pstContents->pstCurGroupList, j, pstTemp);
							}

							/* put */
							HLIB_VECTOR_Replace(pstContents->pstCurGroupList, nCurrIdx, pstData);
							for (j = (nCurrIdx - 1) ; j >= i ; j--)
							{
								Nx_EditChData_t *pstTemp1 = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, j);
								Nx_EditChData_t *pstTemp2 = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, (j + 1));

								pstTemp2->ulLcn = pstTemp1->ulLcn;
								pstTemp2->nDxLcn = pstTemp1->nDxLcn;
							}
							pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, i);
							pstData->ulLcn = tmpLcn;
							pstData->nDxLcn = tmpDxLcn;
							i--;
							nSortCnt = 1;
						}
						else if (i == nCurrIdx)
						{
							pstData->bSelect = FALSE;
							pstContents->ulSelectNum--;
							nSortCnt = 1;
						}
						else
						{
							pstData->bSelect = FALSE;
							pstContents->ulSelectNum--;
							tmpLcn = pstData->ulLcn;
							tmpDxLcn = pstData->nDxLcn;

							/* shift */
							for (j = i - 1 ; j >= nCurrIdx + nSortCnt ; j--)
							{
								Nx_EditChData_t *pstTemp = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, j);

								HLIB_VECTOR_Replace(pstContents->pstCurGroupList, (j + 1), pstTemp);
							}

							/* put */
							HLIB_VECTOR_Replace(pstContents->pstCurGroupList, nCurrIdx + nSortCnt, pstData);

							for (j = nCurrIdx + nSortCnt ; j < i ; j++)
							{
								Nx_EditChData_t *pstTemp1 = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, j);
								Nx_EditChData_t *pstTemp2 = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, j + 1);

								pstTemp1->ulLcn = pstTemp2->ulLcn;
								pstTemp2->nDxLcn = pstTemp2->nDxLcn;
							}

							pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, i);
							pstData->ulLcn = tmpLcn;
							pstData->nDxLcn = tmpDxLcn;

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
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID), nTotal, (GWM_LIST_ITEMS)pstContents->pstCurGroupList);
				nx_menuEditChList_UpdateSelectInfo(pstContents);
			}
			else
			{
				Nx_EditChData_t *pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, p2);
				HINT32	nTotal = 0, nPageNum = 0, nStartIdx = 0;

				pstData->bSelect = !pstData->bSelect;
				if (pstData->bSelect == TRUE)
				{
					pstContents->ulSelectNum++;
				}
				else
				{
					pstContents->ulSelectNum--;
				}
				ONDK_GWM_List_GetTotalItem(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, &nTotal);
				nPageNum = ONDK_GWM_List_GetNumOfPage(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				nStartIdx = ONDK_GWM_List_GetStartIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				if (p2 + 1 == nTotal)
				{
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, 0);
					ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, 0);
				}
				else
				{
					ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, p2 + 1);
					if (p2 + 1 == nStartIdx + nPageNum)
					{
						ONDK_GWM_List_SetStartIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID, nStartIdx + 1);
					}
				}
				nx_menuEditChList_UpdateSelectInfo(pstContents);
			}
			break;

		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			nx_menuEditChList_ChangedPopupOptValue(pstContents, p2, p3);
			break;

		case MSG_APP_KEYBOARD_RESULT:
			{
				HINT32 	nCurrIdx = 0;
				HCHAR 	*pName = NULL;
				Nx_EditChData_t *pstData = NULL;

				nCurrIdx = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MENU_EDIT_CHLIST_OBJ_ID);
				pName = NX_PRISM_KeyBoard_GetResultString();
				pstData = HLIB_VECTOR_ItemAt(pstContents->pstCurGroupList, nCurrIdx);

				HxSTD_sprintf(pstData->ucName, "%s", pName);
				pstData->bEditName = TRUE;
				ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_DESTROY_KEYBOARD, (Handle_t)NULL, 0, 0, 0);
				NX_APP_Free(pName);
				break;
			}
		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuEditChList_MsgAppDlgClicked(pstContents, p1);
			break;
		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
			eResult = nx_menuEditChList_MsgAppDlgCanceled(pstContents);
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
