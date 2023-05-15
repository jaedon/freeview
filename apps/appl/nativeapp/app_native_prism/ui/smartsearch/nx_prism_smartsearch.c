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

#define NX_SMARTDEARCH_RESULTMENU_MAIN_BG_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 1)
#define NX_SMARTDEARCH_RESULTMENU_MAIN_BG_X				60
#define NX_SMARTDEARCH_RESULTMENU_MAIN_BG_Y				200
#define NX_SMARTDEARCH_RESULTMENU_MAIN_BG_W				410
#define NX_SMARTDEARCH_RESULTMENU_MAIN_BG_H				600

#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 2)
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_X				NX_SMARTDEARCH_RESULTMENU_MAIN_BG_X
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_Y				NX_SMARTDEARCH_RESULTMENU_MAIN_BG_Y
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_W				NX_SMARTDEARCH_RESULTMENU_MAIN_BG_W
#define NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_H				NX_SMARTDEARCH_RESULTMENU_MAIN_BG_H

#define NX_SMARTSEARCH_RESULTMENU_ITEM_H					100
#define NX_SMARTSEARCH_RESULTMENU_OBJ_PAGE_COUNT			5

#define NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W				374
#define NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X				18
#define NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_Y				38

#define NX_SMARTSEARCH_RESULTMENU_SVCID_ITEM_TEXT_Y			10
#define NX_SMARTSEARCH_RESULTMENU_SVCNAME_ITEM_TEXT_Y		40
#define NX_SMARTSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y			74

#define	NX_SMARTSEARCH_SCROLL_BAR_X				460
#define	NX_SMARTSEARCH_SCROLL_BAR_Y				200
#define	NX_SMARTSEARCH_SCROLL_BAR_W				3
#define NX_SMARTSEARCH_SCROLL_BAR_H				5

#define	NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID			(NX_PRISM_SMARTSEARCH_ITEM_ID + 3)
#define	NX_SMARTSEARCH_GET_PROGRAM_TIMER_INTERVAL		5 // 5 second


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eSMARTSEARCH_MAINMENU,
	eSMARTSEARCH_SUBMENU
} Nx_SmartSearch_DisPlayType_e;


typedef struct tagSmartSearchContext {

	HINT32	nChannelNum;
	NX_Channel_t *pstChannels;

	HCHAR 			szInput[DxNAME_LEN];

	HUINT32		ulCurIndexMainMenu;
	HxVector_t	*pstResultList;//list of programmes matched with search criteria

	HxVector_t	*pstAllProgramList;//list of all programmes

	Nx_SmartSearch_DisPlayType_e	eIdxMenu;

	//support for getting the programmmes from the searching results
	HUINT32 	ulIdxStartChannel;
	HUINT32		ulChannelCount;

	//support for dividing the searching query
	HUINT32		ulSystemTime;
	HUINT32		ulStartTimeQueryStart;
	HUINT32 	ulStartTimeQueryEnd;
	HUINT32		ulStartTimeQueryFinish;

} Nx_SmartSearchContext_t;


typedef struct tagGenreResultData {
	HINT32	svcUid;
	HINT32 	lcn;
	HINT32	eventId;
	//HUINT32	startTime;
	HCHAR	svcName[DxNAME_LEN];
	HCHAR	szProgrammeName[EPG_HUMAX_NAME_LEN];
	//HCHAR	szEventDescription[MAX_ELEMENT_TEXT_LENGTH];

	//list of Channel Name
	DxSvcType_e svcType;
	HxVector_t *pstChannelList;
} Nx_SmartSearchResultData_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SmartSearchContext_t		s_stSmartSearchContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SmartSearch_GetInputFromKeyBoard(Nx_SmartSearchContext_t *pstContents, HUINT32 isEscape);
static void Nx_SmartSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static ONDK_Result_t    Nx_SmartSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    Nx_SmartSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_SmartSearchResultData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId);
static Nx_SmartSearchResultData_t* Nx_SmartSearch_GetResultOfServiceType(Nx_SmartSearchContext_t *pstContents,DxSvcType_e svcType);
static void 			Nx_SmartSearch_ResetResultList(Nx_SmartSearchContext_t *pstContents);
static ONDK_Result_t 	Nx_SmartSearch_StartSearchWithInput(Nx_SmartSearchContext_t *pstContents, HCHAR* pcInput);
static ONDK_Result_t 	Nx_SmartSearch_GetAllProgrammFromChannels(Nx_SmartSearchContext_t *pstContents, HUINT32 ulIdxStartChannel, HUINT32 ulIdxEndChannel);
static ONDK_Result_t	nx_SmartSearch_SearchProgrammesFromChannels(Nx_SmartSearchContext_t *pstContents);
static void nx_SmartSearch_GetProgramFromResults(Nx_SmartSearchContext_t *pstContents);
static ONDK_Result_t	nx_SmartSearch_SetTimerGetProgram(void);
static inline Nx_SmartSearchContext_t	*nx_SmartSearch_GetContents(void);
static ONDK_Result_t	nx_SmartSearch_MsgGwmCreate(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmDestroy(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmKeyDown(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SmartSearch_MsgGwmTimer(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


static ONDK_Result_t	nx_SmartSearch_GetInputFromKeyBoard(Nx_SmartSearchContext_t *pstContents, HUINT32 isEscape)
{
	HCHAR			*pszStr = NULL;
	HUINT32			ulLen;

	if(isEscape == 1)
	{
		//exit from Smart Search Proc
		ONDK_GWM_APP_Destroy(0);
	}

	if(isEscape == 2 && HLIB_VECTOR_Length(pstContents->pstResultList) > 0)
	{
		//Set focus on the Result List
		ONDK_GWM_APP_SwapStackOrder(NX_PRISM_SMARTSEARCH_Proc,NX_PRISM_SimpleKeyboard_Proc);
		return ONDK_RET_OK;
	}

	pszStr = NX_PRISM_SimpleKeyBoard_GetResultString();
	if(pszStr == NULL)
	{
		return ONDK_RET_OK;
	}
	HxSTD_StrNCpy(pstContents->szInput, pszStr, DxNAME_LEN);
	ulLen  =  HxSTD_StrLen(pstContents->szInput);

	if(ulLen >= 2)
	{
		HxLOG_Debug("\n\n\n  Starting Search\n\n\n");
		Nx_SmartSearch_StartSearchWithInput(pstContents,pstContents->szInput);

	}
	else
	{
		Nx_SmartSearch_ResetResultList(pstContents);
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);
	}

    if(NULL != pszStr)
    {
        NX_APP_Free(pszStr);
        pszStr = NULL;
    }

	return ONDK_RET_OK;
}

static void Nx_SmartSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_SMARTDEARCH_RESULTMENU_MAIN_BG_X, NX_SMARTDEARCH_RESULTMENU_MAIN_BG_Y, NX_SMARTDEARCH_RESULTMENU_MAIN_BG_W, NX_SMARTDEARCH_RESULTMENU_MAIN_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_SMARTDEARCH_RESULTMENU_MAIN_BG_ID, &stRect, COL(C_G_Bang_50));

	// Create first menu
	stRect = ONDK_Rect(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_X, NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_Y, NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_W, NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_H);
	ONDK_GWM_List_Create(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, &stRect, "SMART SEARCH RESULT MENU", NX_SMARTSEARCH_RESULTMENU_ITEM_H, nTotal, NX_SMARTSEARCH_RESULTMENU_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstResultMenu);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);

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

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000;ONDKFontHandle     *hFontHandle = NULL, *hFontDescription = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL, *hBoldFontDescription = NULL,*hNormalFontDescription = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	Nx_SmartSearchContext_t* pstContents = NULL;
	HBOOL bNeedScrollBar = FALSE;
	ONDK_Rect_t			stImgRect;

	NOT_USED_PARAM(arg);

	pstContents = nx_SmartSearch_GetContents();

	/* create font handler */
	hBoldFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	hNormalFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_24);

	hBoldFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_20);
	hNormalFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);

	hFontDescription = hNormalFontDescription;
	hFontHandle = hNormalFontHandle;
	/* init list coordinate */
	stListRegion.x = area->x1 + rect->x;
	stListRegion.y = area->y1 + rect->y;
	stListRegion.w = rect->w;
	stListRegion.h = rect->h;

	/* get list data */
	pstDataList = (HxVector_t *)ONDK_GWM_List_GetArrayPtr(objList);
	ONDK_GWM_List_GetProperty (objList, &nTotal, &nStartIdx, &nCurrIdx, &nSelectedIdx, &nNumOfPage);
	objID = ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList);

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
			textFontColor = COL(C_T_Bang_100);
			bgColor = COL(C_G_Base01_100);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hBoldFontDescription;
		}
		else
		{
			textFontColor = COL(C_T_Whistle_100);
			hFontHandle = hNormalFontHandle;
			hFontDescription = hNormalFontDescription;
		}

		Nx_SmartSearch_DrawCursor_A2(screen, pstData, &stListRegion, nOffsetY, hFontHandle, hFontDescription,textFontColor, bgColor,nCellHeight, ((nDataIdx == nCurrIdx)), bUiTop, objID);
		nOffsetY += nCellHeight;
		bgColor = 0x00000000;
	}

	/* Scroll */
	if (bNeedScrollBar == TRUE)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (nTotal / nNumOfPage);
		lAllPage += (nTotal % nNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_SMARTSEARCH_SCROLL_BAR_H)
		{
			lScrollBall_height = NX_SMARTSEARCH_SCROLL_BAR_H;
		}

		lHeight = (lScroll_height - lScrollBall_height);
		/* calculate vertical position */
		for (i = 1; nTotal > lHeight; i++)
		{
			nTotal /= i;
		} //while (total > (460 - lScrollBall_height));

		lScrollBall_step = lHeight / (nTotal - 1);
		lScroll_y_offset = (nCurrIdx / i) * lScrollBall_step;
		lScrollBall_height += (lHeight - ((nTotal - 1) * lScrollBall_step));

		//lScrollBall_height /= nTotal;

		/* Draw Scroll Bar */
		ONDK_DRAW_FillRectElement(screen, NX_SMARTSEARCH_SCROLL_BAR_X, NX_SMARTSEARCH_SCROLL_BAR_Y, NX_SMARTSEARCH_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(NX_SMARTSEARCH_SCROLL_BAR_X, NX_SMARTSEARCH_SCROLL_BAR_Y + lScroll_y_offset, NX_SMARTSEARCH_SCROLL_BAR_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
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
	ONDK_Rect_t         stRectArrow = {0, 0, 0, 0};
	pstContents = nx_SmartSearch_GetContents();

	if(pstData->svcType == eDxSVC_TYPE_TV)
	{
		HxSTD_sprintf(pText, "%s", pstData->svcName);
	}
	else if(pstData->svcType == eDxSVC_TYPE_RADIO)
	{
		HxSTD_sprintf(pText, "%s", pstData->svcName);
	}
	else
	{
		HxSTD_sprintf(pText, "%s", pstData->szProgrammeName);
	}

	if (bIsFocus == TRUE)
	{

		if(pstData->svcType != 0 && pstData->svcUid == 0)
		{
			ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w - 26, ulCellHeight, bgColor);
			// right arrow
			stRectArrow = ONDK_Rect(stListRegion->x + stListRegion->w - 20, stListRegion->y + nOffsetY + 40, 13, 20);
			ONDK_GUICOM_IMG_Blit(screen, stRectArrow, RES_106_00_ARROW_RIGHT_PNG);
		}
		else
		{
			ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w - 13, ulCellHeight, bgColor);
		}
	}

	if(pstData->svcType != 0)
	{
		//draw the TV channel inforamtion
		HxSTD_sprintf(pText, "CH %02d", pstData->lcn);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_SVCID_ITEM_TEXT_Y, NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		HxSTD_sprintf(pText, "%s", pstData->svcName);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_SVCNAME_ITEM_TEXT_Y, NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		if(pstData->svcUid == 0)
		{
			//show Title + number of channels
			if(pstData->svcType == eDxSVC_TYPE_TV)
			{
				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_TV_CHANNEL_ID),HLIB_VECTOR_Length(pstData->pstChannelList));
			}
			else if(pstData->svcType == eDxSVC_TYPE_RADIO)
			{

				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_RADIO_CHANNEL_ID),HLIB_VECTOR_Length(pstData->pstChannelList));
			}
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y, NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
		}
		else
		{
			//show just title
			if(pstData->svcType == eDxSVC_TYPE_TV)
			{
				HxSTD_sprintf(pText, "%s", (char*)ONDK_GetString(RES_TV_CHANNEL_ID));
			}
			else if(pstData->svcType == eDxSVC_TYPE_RADIO)
			{

				HxSTD_sprintf(pText, "%s", (char*)ONDK_GetString(RES_RADIO_CHANNEL_ID));
			}
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y, NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		}
	}
	else
	{
		//draw the programme information
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_Y, NX_SMARTSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
	}

	return ONDK_RET_OK;
}

static Nx_SmartSearchResultData_t* Nx_SmartSearch_GetResultOfServiceType(Nx_SmartSearchContext_t *pstContents,DxSvcType_e svcType)
{
	HUINT32 ulIndex = 0, ulLen = 0;
	Nx_SmartSearchResultData_t *pstResultData;

	ulLen = HLIB_VECTOR_Length(pstContents->pstResultList);

	for(ulIndex = 0; ulIndex < ulLen; ulIndex++)
	{
		pstResultData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, ulIndex);
		if(pstResultData->svcType == svcType)
		{
			return pstResultData;
		}
	}

	return NULL;
}

static void Nx_SmartSearch_ResetResultList(Nx_SmartSearchContext_t *pstContents)
{
	HUINT32 ulIndex = 0, ulLen = 0;
	Nx_SmartSearchResultData_t *pstResultData;

	ulLen = HLIB_VECTOR_Length(pstContents->pstResultList);
	for(ulIndex = 0; ulIndex < ulLen; ulIndex++)
	{
		pstResultData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, ulIndex);

		if(pstResultData->pstChannelList != NULL)
		{
			HLIB_VECTOR_Delete(pstResultData->pstChannelList);
		}
	}

	HLIB_VECTOR_RemoveAll(pstContents->pstResultList);

	return;
}

static ONDK_Result_t Nx_SmartSearch_StartSearchWithInput(Nx_SmartSearchContext_t *pstContents, HCHAR* pcInput)
{
	HINT32			lLen = 0, lIndex = 0, lChannelNum = 0, lStrLen = 0;
	NX_Channel_t	*pstChannelList = NULL, *tmpChannel = NULL;
	DxService_t		*tempSvc = NULL;
	Nx_SmartSearchResultData_t *pstResultData = NULL, *pstChannelResultData = NULL, *pstAllProgram = NULL;


	lStrLen = HxSTD_StrLen(pcInput);
	if (lStrLen < 2)
	{
		return ONDK_RET_OK;
	}

	lLen = HLIB_VECTOR_Length(pstContents->pstAllProgramList);
	//set the empty data for Result Menu
	Nx_SmartSearch_ResetResultList(pstContents);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);

	//Search Channels
	lChannelNum = pstContents->nChannelNum;
	pstChannelList = pstContents->pstChannels;

	for(lIndex = 0; lIndex < lChannelNum; lIndex++)
	{
		tmpChannel = &pstChannelList[lIndex];
		tempSvc = NX_CHANNEL_FindServiceBySvcUid(tmpChannel->svcUid);

		if(HLIB_STD_StrCaseStr(tempSvc->name, pcInput) != NULL)
		{
			pstResultData = Nx_SmartSearch_GetResultOfServiceType(pstContents, tempSvc->svcType);

			pstChannelResultData = (Nx_SmartSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_SmartSearchResultData_t));
			if (pstChannelResultData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
			HxSTD_MemSet(pstChannelResultData, 0, sizeof(Nx_SmartSearchResultData_t));

			HxSTD_StrNCpy((char*)pstChannelResultData->svcName,tempSvc->name,MAX_ELEMENT_TEXT_LENGTH);
			pstChannelResultData->svcUid = tmpChannel->svcUid;
			pstChannelResultData->lcn = tempSvc->lcn;
			pstChannelResultData->svcType = tempSvc->svcType;

			if(pstResultData == NULL)
			{
				pstResultData = (Nx_SmartSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_SmartSearchResultData_t));
				if (pstResultData == NULL)
                {
                    if(NULL != pstChannelResultData)
                    {
                        NX_APP_Free(pstChannelResultData);
                        pstChannelResultData = NULL;
                    }
                    return GWM_RET_OUT_OF_MEMORY;
                }
				HxSTD_MemSet(pstResultData, 0, sizeof(Nx_SmartSearchResultData_t));

				pstResultData->svcType = tempSvc->svcType;
				pstResultData->pstChannelList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

				//set the service name and lcn for ChannelList the same as the first channel
				HxSTD_StrNCpy((char*)pstResultData->svcName,tempSvc->name,MAX_ELEMENT_TEXT_LENGTH);
				pstResultData->lcn = tempSvc->lcn;
				pstResultData->svcUid = 0;

				HLIB_VECTOR_Add(pstContents->pstResultList, pstResultData);
			}

			HLIB_VECTOR_Add(pstResultData->pstChannelList, pstChannelResultData);
		}

	}

	//Search Program
	lStrLen = HxSTD_StrLen(pcInput);
	if(lStrLen >= 3)
	{
		for(lIndex = 0; lIndex < lLen; lIndex++)
		{
			pstAllProgram = HLIB_VECTOR_ItemAt(pstContents->pstAllProgramList, lIndex);
			if(HLIB_STD_StrCaseStr(pstAllProgram->szProgrammeName, pcInput) != NULL)
			{
				pstResultData = (Nx_SmartSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_SmartSearchResultData_t));
				if (pstResultData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

				HxSTD_MemSet(pstResultData, 0, sizeof(Nx_SmartSearchResultData_t));
				HxSTD_StrNCpy((char*)pstResultData->szProgrammeName, (char*)pstAllProgram->szProgrammeName, MAX_ELEMENT_TEXT_LENGTH);
				pstResultData->svcUid = pstAllProgram->svcUid;
				pstResultData->eventId = pstAllProgram->eventId;

				HLIB_VECTOR_Add(pstContents->pstResultList, pstResultData);
			}
		}
	}
	//set the new data for Result Menu
	pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);

	return ONDK_RET_OK;

}

static ONDK_Result_t Nx_SmartSearch_GetAllProgrammFromChannels(Nx_SmartSearchContext_t *pstContents, HUINT32 ulIdxStartChannel, HUINT32 ulIdxEndChannel)
{
	HUINT32 ulChannelNum = 0, ulIndex = 0;
	NX_Channel_t* pstChannelList, *tmpChannel;
	HINT32 num = 0, j = 0;
	HxVector_t *result = NULL;
	DxService_t *tempSvc = NULL;

	ulChannelNum = pstContents->nChannelNum;
	pstChannelList = pstContents->pstChannels;

	if (pstChannelList == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_LIVE_GetLiveChannels() is NULL!!! \n");
		return ONDK_RET_FAIL;
	}

	HxLOG_Debug("\n\n\n  numOfChannel (%d) start(%d) end(%d)\n\n\n", ulChannelNum, ulIdxStartChannel, ulIdxEndChannel);
	for(ulIndex = ulIdxStartChannel; ulIndex < ulIdxEndChannel; ulIndex++)
	{
		tmpChannel = &pstChannelList[ulIndex];

		tempSvc = NX_CHANNEL_FindServiceBySvcUid(tmpChannel->svcUid);

		result = HLIB_VECTOR_New(NULL,NULL);

		NX_METADATA_EPG_GetResultProgrammeBySvcUid(tmpChannel->svcUid, result, &num);

		HxLOG_Debug("\n\n\n  get programme of service (%d) (%s) num(%d)\n\n\n", tempSvc->lcn,tempSvc->name,num);
		if (num > 0 && (result != NULL))
		{
			for (j = 0; j < num; j++)
			{
				NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, j);

				HCHAR programmeName[EPG_HUMAX_NAME_LEN] = {'\0',};

				Nx_SmartSearchResultData_t *data = (Nx_SmartSearchResultData_t*)NX_APP_Calloc(sizeof(Nx_SmartSearchResultData_t));

				data->svcUid 	= NX_METADATA_PROGRAMMES_GetSvcUID(item);
				data->eventId	= NX_METADATA_PROGRAMMES_GetEventID(item);
				data->lcn		= tempSvc->lcn;

				HxSTD_MemSet(data->svcName, 0, DxNAME_LEN);
				HxSTD_sprintf(data->svcName,"%s",tempSvc->name);

				if (NX_METADATA_PROGRAMMES_GetName(item, programmeName) == ERR_OK)
				{
					HxSTD_MemSet(data->szProgrammeName, 0, EPG_HUMAX_NAME_LEN);
					HxSTD_sprintf(data->szProgrammeName,"%s", programmeName);
				}
				HLIB_VECTOR_Add(pstContents->pstAllProgramList, data);
			}
		}
	}

	return ONDK_RET_OK;

}


static ONDK_Result_t	nx_SmartSearch_SearchProgrammesFromChannels(Nx_SmartSearchContext_t *pstContents)
{
	HINT32 i = 0, cur_index = 0;
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	startQuery, endQuery, andQuery;
	HUINT32 	queryTime = 0;
	HCHAR 		value[NX_TEXT_SIZE_64] = {'\0',};

	static HUINT32 count = 0;

	HxLOG_Debug("\n\n\n  QQuery Started (%d)\n\n\n",count++);

	if(pstContents->ulStartTimeQueryStart == pstContents->ulSystemTime)
	{
		queryTime = pstContents->ulSystemTime - SECONDS_PER_HOUR * 2;
		HxSTD_sprintf(value, "%u", queryTime);
		endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

		queryTime = pstContents->ulSystemTime + SECONDS_PER_HOUR* 6;
		HxSTD_sprintf(value, "%u", queryTime);
		startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);
	}
	else
	{
		HxLOG_Debug("\n\n\n  Query Started start (%d) end (%d)\n\n\n", pstContents->ulStartTimeQueryStart,pstContents->ulStartTimeQueryEnd);
		queryTime = pstContents->ulStartTimeQueryStart;
		HxSTD_sprintf(value, "%u", queryTime);
		endQuery = NX_METADATA_SEARCH_CreateQuery("startTime", ">", value);

		queryTime = pstContents->ulStartTimeQueryEnd;
		HxSTD_sprintf(value, "%u", queryTime);
		startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);
	}

	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	for (i = 0; i < pstContents->nChannelNum; i++)
	{
		if (cur_index >= pstContents->nChannelNum)
		{
			cur_index = 0;
		}
		HxLOG_Debug("AddChannelConstraint. svcUid(%d), lcn(%d) \n",pstContents->pstChannels[cur_index].svcUid, pstContents->pstChannels[cur_index].lcn);
		NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, &pstContents->pstChannels[cur_index]);

		cur_index++;
	}

	NX_PORT_META_QUERY_SetFilter(pstMetaSearch->context, eNxQUERY_FILTER_ALL);

	NX_METADATA_EPG_SearchEPG(pstMetaSearch);

	HxLOG_Debug("\n\n\n Query Finished\n\n\n");

	return ONDK_RET_OK;
}

static void nx_SmartSearch_GetProgramFromResults(Nx_SmartSearchContext_t *pstContents)
{
	HxLOG_Debug("\n\n\n Search finished\n\n\n");
	pstContents->ulIdxStartChannel = 0;
	pstContents->ulChannelCount = pstContents->nChannelNum/3;

	Nx_SmartSearch_GetAllProgrammFromChannels(pstContents, pstContents->ulIdxStartChannel, pstContents->ulIdxStartChannel + pstContents->ulChannelCount);

	//Set timer for getting Program
	nx_SmartSearch_SetTimerGetProgram();

	return;
}

static ONDK_Result_t	nx_SmartSearch_SetTimerGetProgram(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	//set 100ms for timer
	retTimer = ONDK_GWM_SetTimer(NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID, (NX_SMARTSEARCH_GET_PROGRAM_TIMER_INTERVAL) * 1000);
	if (retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID);
	}

	return ONDK_RET_OK;
}


static inline Nx_SmartSearchContext_t	*nx_SmartSearch_GetContents(void)
{
	return	&s_stSmartSearchContent;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmCreate(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HUINT32 	unixtime = 0, nTotal = 0;

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

	hError = NX_SYSTEM_GetTime(&unixtime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("time is Invalid!!!\n");
	}


	pstContents->ulSystemTime = unixtime;
	pstContents->ulStartTimeQueryStart = unixtime;

	// starttime for each searching query is less than 6 hours
	pstContents->ulStartTimeQueryEnd = pstContents->ulStartTimeQueryStart + SECONDS_PER_HOUR * 6;

	//the total starttime for the searching query is 2 weeks
	pstContents->ulStartTimeQueryFinish= pstContents->ulStartTimeQueryStart + SECONDS_PER_WEEK * 2;

	pstContents->ulIdxStartChannel = 0;
	pstContents->ulChannelCount = pstContents->nChannelNum/3;

	//Search the programmes
	nx_SmartSearch_SearchProgrammesFromChannels(pstContents);

	//initiate ResultData
	pstContents->pstResultList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	pstContents->pstAllProgramList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;

	//show simple keyboard
	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SimpleKeyboard_Proc) == NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SimpleKeyboard_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SimpleKeyboard_Proc, HANDLE_NULL, 0, 0, 0);
	}

	//Create result menu
	nTotal = HLIB_VECTOR_Length(pstContents->pstResultList);
	Nx_SmartSearch_CreateResultMenu(pstContents->pstResultList, nTotal, (GWM_ObjDraw_t)Nx_SmartSearch_DrawMenu);

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

	if(pstContents->pstAllProgramList)
		HLIB_VECTOR_Delete(pstContents->pstAllProgramList);

	ONDK_GWM_KillTimer(NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID); //kill Searching timer

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SmartSearch_MsgGwmKeyDown(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	HUINT32 curIndex = 0;
	Nx_SmartSearchResultData_t* pstCurData;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

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
			ONDK_GWM_APP_SwapStackOrder(NX_PRISM_SimpleKeyboard_Proc,NX_PRISM_SMARTSEARCH_Proc);
			return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWRIGHT:
			{
				HxVector_t* emptyList;
				if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);
					HxLOG_Debug("\n\n\n curIndex (%d)\n\n\n", curIndex);
					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, curIndex);
					if(pstCurData->svcType != 0 && pstCurData->svcUid == 0)
					{
						//save current index of main menu
						pstContents->ulCurIndexMainMenu = curIndex;

						emptyList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(emptyList), (GWM_LIST_ITEMS)emptyList);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstCurData->pstChannelList), (GWM_LIST_ITEMS)pstCurData->pstChannelList);
					}

					pstContents->eIdxMenu = eSMARTSEARCH_SUBMENU;
				}

			}
			return ONDK_MESSAGE_BREAK;

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
				NX_ChListGroup_t group;
				NX_ChListGroup_t currentGroup;
				NX_Channel_t* pstSelectedChannel;
				if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);

					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList,pstContents->ulCurIndexMainMenu);
					pstCurData = HLIB_VECTOR_ItemAt(pstCurData->pstChannelList, curIndex);

					group = eNxChListGroup_ALL;
					currentGroup = NX_PRISM_LIVE_GetCurrentChGroupType();

					pstSelectedChannel = (NX_Channel_t *)NX_APP_Malloc (sizeof(NX_Channel_t));
					if (pstSelectedChannel == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
					HxSTD_MemSet(pstSelectedChannel, 0, sizeof(NX_Channel_t));

					pstSelectedChannel->lcn = pstCurData->lcn;
					pstSelectedChannel->svcUid = pstCurData->svcUid;

					if (NX_CHANNEL_GROUP_IsServiceInGroup(currentGroup, pstSelectedChannel) == TRUE)
					{
						HxLOG_Debug("\n\n\n the same group (%d)\n\n\n", currentGroup);
						NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstSelectedChannel->svcUid, currentGroup, 0);
					}
					else
					{
						NX_CHANNEL_GROUP_GetGroupOfChannel(&group,pstSelectedChannel);
						HxLOG_Debug("\n\n\n the different group (%d)\n\n\n", group);
						NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstSelectedChannel->svcUid, group, 0);
					}
				}
				else if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_SMARTSEARCH_RESULTMENU_MAIN_OBJ_ID);
					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList,curIndex);

					if(pstCurData->svcType == 0)
					{
						if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)Nx_PRISM_ProgramDetail_Proc) ==NULL )
						{
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, pstCurData->svcUid, pstCurData->eventId, p3);
						}
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

static ONDK_Result_t	nx_SmartSearch_MsgGwmTimer(Nx_SmartSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	switch(p1)
	{
		case NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID:
			{
				HUINT32 idxEnd = 0;

				pstContents->ulIdxStartChannel += pstContents->ulChannelCount;
				idxEnd  = pstContents->ulIdxStartChannel + pstContents->ulChannelCount;

				if(idxEnd >= (HUINT32)pstContents->nChannelNum)
				{
					//pstContents->ulIdxStartChannel = 0;
					idxEnd = pstContents->nChannelNum;
					ONDK_GWM_KillTimer(NX_SMARTSEARCH_GET_PROGRAM_TIMER_ID);
				}
				Nx_SmartSearch_GetAllProgrammFromChannels(pstContents, pstContents->ulIdxStartChannel, idxEnd);
				//Nx_SmartSearch_StartSearchWithInput(pstContents, pstContents->szInput);

				if(idxEnd == (HUINT32)pstContents->nChannelNum)
				{
					pstContents->ulStartTimeQueryStart = pstContents->ulStartTimeQueryEnd;
					pstContents->ulStartTimeQueryEnd += SECONDS_PER_HOUR * 6;

					if(pstContents->ulStartTimeQueryStart < pstContents->ulStartTimeQueryFinish)
					{
						nx_SmartSearch_SearchProgrammesFromChannels(pstContents);
					}
					else
					{
						HxLOG_Debug("\n\n\n Finish finding the Program on Channels\n\n\n");
					}
				}
			}
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
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

		case MSG_GWM_TIMER:
			nx_SmartSearch_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_SmartSearch_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_SmartSearch_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			//eResult = nx_guide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			HxLOG_Debug("\n\n\n  ======>>> RECEIVED MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES IN GENRE SEARCH\n\n\n");
			nx_SmartSearch_GetProgramFromResults(pstContents);
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
		case MSG_APP_KEYBOARD_RESULT:
			NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
			nx_SmartSearch_GetInputFromKeyBoard(pstContents,p3);
			eResult = ONDK_MESSAGE_BREAK;
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
