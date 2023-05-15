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
#define	NX_PRISM_SMARTSEARCH_ITEM_ID						eMenuFrame_Window_KeywordSearch

#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_ID				(NX_PRISM_SMARTSEARCH_ITEM_ID + 1)
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_X				0
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_Y				0
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_W				610
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_H				600

#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID				(NX_PRISM_SMARTSEARCH_ITEM_ID + 2)
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_X				NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_X
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_Y				NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_Y
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_W				NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_W
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_H				NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_H
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_X		80
#define NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_Y		170 //(66 + 170)


#define NX_KEYWORDSEARCH_RESULTMENU_ITEM_H					106
#define NX_KEYWORDSEARCH_KEYWORD_RESULTMENU_ITEM_H			55
#define NX_KEYWORDSEARCH_RESULTMENU_OBJ_PAGE_COUNT			4

#define NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W				325
#define NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X				188
#define NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_Y				31

#define NX_KEYWORDSEARCH_RESULTMENU_SVCID_ITEM_TEXT_Y		14
#define NX_KEYWORDSEARCH_RESULTMENU_SVCNAME_ITEM_TEXT_Y		39
#define NX_KEYWORDSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y		74

#define NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_X				5
#define NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_Y				5
#define NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_W				170
#define	NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_H				96

#define NX_KEYWORDSEARCH_RESULTMENU_PROGRAM_TITLE_ITEM_TEXT_Y		14
#define NX_KEYWORDSEARCH_RESULTMENU_PROGRAM_INFO_ITEM_TEXT_Y		74


#define	NX_KEYWORDSEARCH_SCROLL_BAR_X						460
#define	NX_KEYWORDSEARCH_SCROLL_BAR_Y						200
#define	NX_KEYWORDSEARCH_SCROLL_BAR_W						3
#define NX_KEYWORDSEARCH_SCROLL_BAR_H						5

#define	NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID				(NX_PRISM_SMARTSEARCH_ITEM_ID + 3)
#define	NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_INTERVAL			5 // 5 second

#define NX_KEYWORDSEARCH_LINE_ID							(NX_PRISM_SMARTSEARCH_ITEM_ID + 5)
#define NX_KEYWORDSEARCH_LINE_X								(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_X + NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_X)
#define NX_KEYWORDSEARCH_LINE_Y								(NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_Y + 170 - 43)
#define NX_KEYWORDSEARCH_LINE_W								4
#define NX_KEYWORDSEARCH_LINE_H								20

#define NX_KEYWORDSEARCH_SELECTED_CATEGORY_X				96
#define NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y				NX_KEYWORDSEARCH_LINE_Y - 3
#define NX_KEYWORDSEARCH_SELECTED_CATEGORY_W				510

#define NX_KEYWORDSEARCH_NUMBER_OF_ITEM_X					550
#define NX_KEYWORDSEARCH_NUMBER_OF_ITEM_Y					NX_KEYWORDSEARCH_LINE_Y
#define NX_KEYWORDSEARCH_NUMBER_OF_ITEM_W					140

#define NX_KEYWORDSEARCH_KEYWORD_PROGRAM_TITLE_X			100
#define NX_KEYWORDSEARCH_KEYWORD_PROGRAM_TITLE_Y			NX_KEYWORDSEARCH_LINE_Y + 57
#define NX_KEYWORDSEARCH_KEYWORD_PROGRAM_TITLE_W			510

#define NX_KEYWORDSEARCH_KEYWORD_DIVISION_LIINE_X			80
#define NX_KEYWORDSEARCH_KEYWORD_DIVISION_LIINE_Y			NX_KEYWORDSEARCH_KEYWORD_PROGRAM_TITLE_Y + 48
#define NX_KEYWORDSEARCH_KEYWORD_DIVISION_LIINE_W			530

#define NX_KEYWORDSEARCH_KEYWORD_CAST_X						100
#define NX_KEYWORDSEARCH_KEYWORD_CAST_Y						NX_KEYWORDSEARCH_KEYWORD_DIVISION_LIINE_Y + 18
#define NX_KEYWORDSEARCH_KEYWORD_CAST_W						490

#define NX_KEYWORDSEARCH_KEYWORD_RESULTMENU_MAIN_OBJ_MARGIN_X			80
#define NX_KEYWORDSEARCH_KEYWORD_RESULTMENU_MAIN_OBJ_MARGIN_Y			NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_Y + 57 + 48 + 18

#define NX_KEYWORDSEARCH_KEYWORD_CAST_LINE_X				80
#define NX_KEYWORDSEARCH_KEYWORD_CAST_LINE_Y				NX_KEYWORDSEARCH_KEYWORD_CAST_Y + 10
#define NX_KEYWORDSEARCH_KEYWORD_CAST_LINE_W				10

#define	NX_KEYWORDSEARCH_EXIT_TIMER_ID						(NX_PRISM_SMARTSEARCH_ITEM_ID + 6)
#define	NX_KEYWORDSEARCH_EXIT_TIMER_INTERVAL				3*60 // 3 minutes

#define NX_KEYWORDSEARCH_ARROW_X							54
#define NX_KEYWORDSEARCH_ARROW_Y							276
#define NX_KEYWORDSEARCH_ARROW_W							26
#define NX_KEYWORDSEARCH_ARROW_H							30

#define NX_KEYWORDSEARCH_ARROW_TEXT_X						54 + 22
#define NX_KEYWORDSEARCH_ARROW_TEXT_Y						(NX_KEYWORDSEARCH_ARROW_Y + NX_KEYWORDSEARCH_ARROW_H + 10)
#define NX_KEYWORDSEARCH_ARROW_TEXT_H						192


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eSMARTSEARCH_MAINMENU,
	eSMARTSEARCH_SUBMENU
} Nx_KeywordSearch_DisPlayType_e;

typedef enum {
	eSMARTSEARCH_NO_PROGRAM	= 0,
	eSMARTSEARCH_TV_PROGRAM,
	eSMARTSEARCH_RADIO_PROGRAM
} Nx_KeywordSearch_ProgramType_e;

typedef enum {
	eSMARTSEARCH_NO_CATEGORY	= 0,
	eSMARTSEARCH_TV_CHANNEL_CATEGORY,
	eSMARTSEARCH_RADIO_CHANNEL_CATEGORY,
	eSMARTSEARCH_TV_PROGRAM_CATEGORY,
	eSMARTSEARCH_RADIO_PROGRAM_CATEGORY
} Nx_KeywordSearch_Selected_Category_e;

typedef struct tagSmartSearchContext {

	HINT32		nChannelNum;
	NX_Channel_t *pstChannels;

	HCHAR 			szInput[EPG_HUMAX_NAME_LEN];
	HCHAR			szKeyWordSeachedInput[EPG_HUMAX_NAME_LEN];
	HCHAR			szTitle[EPG_HUMAX_NAME_LEN];

	HUINT32		ulCurIndexMainMenu;
	HUINT32		ulCurIndexSubMenu;
	HUINT32		ulCurIndexCast;

	HxVector_t	*pstResultList;//list of programmes matched with search criteria

	HxList_t	*pstAllProgramList;//list of all programmes

	Nx_KeywordSearch_DisPlayType_e	eIdxMenu;

	//support for getting the programmmes from the searching results
	HUINT32 	ulIdxStartChannel;
	HUINT32		ulChannelCount;

	//support for dividing the searching query
	HUINT32		ulSystemTime;
	HUINT32		ulStartTimeQueryStart;
	HUINT32 	ulStartTimeQueryEnd;
	HUINT32		ulStartTimeQueryFinish;

	//Selected category
	Nx_KeywordSearch_Selected_Category_e eSelectedCategory;

} Nx_KeywordSearchContext_t;

typedef struct tagGenreResultData {
	HINT32	svcUid;
	HINT32 	lcn;
	HINT32	eventId;
	//HUINT32	startTime;
	HCHAR	svcName[DxNAME_LEN];
	HCHAR	szProgrammeName[EPG_HUMAX_NAME_LEN];

	//list of Program
	Nx_KeywordSearch_ProgramType_e eProgramType;
	HxVector_t *pstProgramList;

	//list of Channel Name
	DxSvcType_e svcType;
	HxVector_t *pstChannelList;

} Nx_KeywordSearchResultData_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_KeywordSearchContext_t		s_stSmartSearchContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_KeywordSearch_GetInputFromKeyBoard(Nx_KeywordSearchContext_t *pstContents, HUINT32 isEscape);
static void				nx_KeywordSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static ONDK_Result_t    nx_KeywordSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    nx_KeywordSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_KeywordSearchResultData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId);
static ONDK_Result_t    nx_KeywordSearch_KeyWord_DrawCursor_A2(ONDKSurfaceHandle *screen, HUINT8 *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId);
static ONDK_Result_t    nx_KeywordSearch_DrawSearchInput(Nx_KeywordSearchContext_t *pstContents, ONDKSurfaceHandle *screen);
static Nx_KeywordSearchResultData_t*	nx_KeywordSearch_GetResultOfServiceType(HxList_t* list,DxSvcType_e svcType);
static Nx_KeywordSearchResultData_t*	nx_KeywordSearch_GetResultOfProgramType(Nx_KeywordSearchContext_t *pstContents,Nx_KeywordSearch_ProgramType_e programType, HCHAR* programName);
static void 			nx_KeywordSearch_ResetResultList(Nx_KeywordSearchContext_t *pstContents);
static void 			nx_KeywordSearch_ResetList(HxList_t* list);
static ONDK_Result_t 	nx_KeywordSearch_CopyResultDataLevel1(Nx_KeywordSearchResultData_t* pstDestination, Nx_KeywordSearchResultData_t* pstSource);
static ONDK_Result_t 	nx_KeywordSearch_CopyResultDataLevel2(Nx_KeywordSearchResultData_t* pstDestination, Nx_KeywordSearchResultData_t* pstSource);
static HINT32 			nx_KeywordSearch_SortByAlphabet(void *pvArg1, void *pvArg2);
static ONDK_Result_t 	nx_KeywordSearch_CopyFromListToVector(HxVector_t* vector, HxList_t* list);
static ONDK_Result_t 	nx_KeywordSearch_StartSearchWithInput(Nx_KeywordSearchContext_t *pstContents, HCHAR* pcInput);
static ONDK_Result_t 	nx_KeywordSearch_GetAllProgrammFromChannels(Nx_KeywordSearchContext_t *pstContents, HUINT32 ulIdxStartChannel, HUINT32 ulIdxEndChannel);
static ONDK_Result_t	nx_KeywordSearch_SearchProgrammesFromChannels(Nx_KeywordSearchContext_t *pstContents);
static void				nx_KeywordSearch_GetProgramFromResults(Nx_KeywordSearchContext_t *pstContents);
static ONDK_Result_t	nx_KeywordSearch_SetTimerGetProgram(void);
static ONDK_Result_t	nx_KeywordSearch_SetTimerExit(void);
static inline Nx_KeywordSearchContext_t	*nx_KeywordSearch_GetContents(void);
static ONDK_Result_t	nx_KeywordSearch_MsgGwmCreate(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_KeywordSearch_MsgGwmDestroy(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_KeywordSearch_MsgGwmKeyDown(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_KeywordSearch_MsgGwmTimer(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_KeywordSearch_GetInputFromKeyBoard(Nx_KeywordSearchContext_t *pstContents, HUINT32 isEscape)
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
		ONDK_GWM_APP_SwapStackOrder(NX_PRISM_KEYWORDSEARCH_Proc,NX_PRISM_SimpleKeyboard_Proc);
		ONDK_GWM_APP_Destroy(NX_PRISM_SimpleKeyboard_Proc);
		return ONDK_RET_OK;
	}

	//After 3 minutes, if there is not any input from users, the SmartSearch Proc will be destroyed
	nx_KeywordSearch_SetTimerExit();

	//Get the inputted string
	pszStr = NX_PRISM_SimpleKeyBoard_GetResultString();
	if(pszStr == NULL)
	{
		HxLOG_Debug("There is no input value");
		return ONDK_RET_OK;
	}

	HxSTD_StrNCpy(pstContents->szInput, pszStr, EPG_HUMAX_NAME_LEN);
	ulLen  =  HxSTD_StrLen(pstContents->szInput);
	HxLOG_Debug("input value (%s) len (%d)", pstContents->szInput, ulLen);
	if(ulLen >= 2)
	{
		HxLOG_Debug("Starting Search");
		//reset the current Index
		pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;
		pstContents->ulCurIndexMainMenu = 0;
		pstContents->ulCurIndexSubMenu = 0;

		nx_KeywordSearch_StartSearchWithInput(pstContents,pstContents->szInput);
	}
	else
	{
		//set the empty data for Result Menu
		nx_KeywordSearch_ResetResultList(pstContents);
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);
	}

    if(NULL != pszStr)
    {
        NX_APP_Free(pszStr);
        pszStr = NULL;
    }

	return ONDK_RET_OK;
}

static void nx_KeywordSearch_CreateResultMenu(HxVector_t *pstResultMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_X, NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_Y, NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_W, NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_H);

	ONDK_GWM_COM_Rect_Create(NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_KEYWORDSEARCH_RESULTMENU_MAIN_BG_ID, NX_PRISM_COMP_DrawFrameBG_SS);

	//Draw Line
	stRect = ONDK_Rect(NX_KEYWORDSEARCH_LINE_X, NX_KEYWORDSEARCH_LINE_Y, NX_KEYWORDSEARCH_LINE_W, NX_KEYWORDSEARCH_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_KEYWORDSEARCH_LINE_ID, &stRect, COL(C_G_Whistle_20));
	//ONDK_GWM_Obj_SetHide(NX_KEYWORDSEARCH_LINE_ID, TRUE);

	// Create first menu
	stRect = ONDK_Rect(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_X, NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_Y, NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_W, NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_H);
	ONDK_GWM_List_Create(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, &stRect, "SMART SEARCH RESULT MENU", NX_KEYWORDSEARCH_RESULTMENU_ITEM_H, nTotal, NX_KEYWORDSEARCH_RESULTMENU_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstResultMenu);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);

}

static ONDK_Result_t    nx_KeywordSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */
	HINT32          nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32          nCellHeight = 0, nCount = 0, nOffsetY = 0, lScroll_height = 0;
	LIST_OBJECT     objList = (LIST_OBJECT)object;
	HINT32          objID;
	HxVector_t      *pstDataList = NULL;
	Nx_KeywordSearchResultData_t *pstData = NULL;

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000, textSelectedCategory = 0x00000000;
	ONDKFontHandle     *hFontHandle = NULL, *hFontDescription = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL, *hBoldFontDescription = NULL,*hNormalFontDescription = NULL;
	ONDKFontHandle		*hFontCategory = NULL, *hFontCast = NULL, *hRotateFont = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	ONDK_Rect_t         stRectArrow = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	Nx_KeywordSearchContext_t* pstContents = NULL;
	HBOOL bNeedScrollBar = FALSE;
	HUINT8	szNumOfItem[MAX_ELEMENT_TEXT_LENGTH] = {0,};

	NOT_USED_PARAM(arg);

	pstContents = nx_KeywordSearch_GetContents();

	/* create font handler */
	hBoldFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_26);
	hNormalFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_26);

	hBoldFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_18);
	hNormalFontDescription = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_18);

	hFontDescription = hNormalFontDescription;
	hFontHandle = hNormalFontHandle;

	textSelectedCategory = COL(C_T_Gong06);
	hFontCategory = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_24);
	hFontCast = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	hRotateFont = ONDK_FONT_CreateRotationFont(eFont_SystemNormal, ONDK_FONT_SIZE_22, -90);
	/* init list coordinate */

	stListRegion.x = area->x1 + rect->x + NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_X;
	stListRegion.y = area->y1 + rect->y + NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_Y;
	stListRegion.w = rect->w - NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_MARGIN_X;
	stListRegion.h = rect->h;

	/* get list data */
	pstDataList = (HxVector_t *)ONDK_GWM_List_GetArrayPtr(objList);
	ONDK_GWM_List_GetProperty (objList, &nTotal, &nStartIdx, &nCurrIdx, &nSelectedIdx, &nNumOfPage);
	objID = ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList);

	if(ONDK_GWM_APP_GetAppTop() == ONDK_GWM_APP_Get(NX_PRISM_KEYWORDSEARCH_Proc))
	{
		//Draw Input
		nx_KeywordSearch_DrawSearchInput(pstContents,screen);
	}

	if(ONDK_GWM_APP_GetAppTop() == ONDK_GWM_APP_Get(NX_PRISM_KEYWORDSEARCH_Proc))
	{
		//Show the Line
		//Draw selected category just for submenu
		if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
		{
			switch(pstContents->eSelectedCategory)
			{
				case eSMARTSEARCH_NO_CATEGORY:
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)"",NX_KEYWORDSEARCH_SELECTED_CATEGORY_X, NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y, NX_KEYWORDSEARCH_SELECTED_CATEGORY_W, textSelectedCategory, ONDKSTF_LEFT);
					break;

				case eSMARTSEARCH_TV_CHANNEL_CATEGORY:
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)ONDK_GetString(RES_TV_CHANNEL_ID),NX_KEYWORDSEARCH_SELECTED_CATEGORY_X, NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y, NX_KEYWORDSEARCH_SELECTED_CATEGORY_W, textSelectedCategory, ONDKSTF_LEFT);
					break;

				case eSMARTSEARCH_RADIO_CHANNEL_CATEGORY:
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)ONDK_GetString(RES_RADIO_CHANNEL_ID),NX_KEYWORDSEARCH_SELECTED_CATEGORY_X, NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y, NX_KEYWORDSEARCH_SELECTED_CATEGORY_W, textSelectedCategory, ONDKSTF_LEFT);
					break;

				case eSMARTSEARCH_TV_PROGRAM_CATEGORY:
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)ONDK_GetString(RES_TV_PROGRAMME_ID),NX_KEYWORDSEARCH_SELECTED_CATEGORY_X, NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y, NX_KEYWORDSEARCH_SELECTED_CATEGORY_W, textSelectedCategory, ONDKSTF_LEFT);
					break;

				case eSMARTSEARCH_RADIO_PROGRAM_CATEGORY:
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)ONDK_GetString(RES_RADIO_PROGRAMME_ID),NX_KEYWORDSEARCH_SELECTED_CATEGORY_X, NX_KEYWORDSEARCH_SELECTED_CATEGORY_Y, NX_KEYWORDSEARCH_SELECTED_CATEGORY_W, textSelectedCategory, ONDKSTF_LEFT);
					break;

				default:
					break;

			}
		}

		//Draw number of item
		HxSTD_sprintf(szNumOfItem,(HCHAR*)"%d/%d", (nCurrIdx + 1),nTotal);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontCategory, (char*)szNumOfItem,NX_KEYWORDSEARCH_NUMBER_OF_ITEM_X, NX_KEYWORDSEARCH_NUMBER_OF_ITEM_Y, NX_KEYWORDSEARCH_NUMBER_OF_ITEM_W, textSelectedCategory, ONDKSTF_LEFT);
	}

	//Draw Previous Arrow
	if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
	{
		stRectArrow = ONDK_Rect(NX_KEYWORDSEARCH_ARROW_X, NX_KEYWORDSEARCH_ARROW_Y, NX_KEYWORDSEARCH_ARROW_W, NX_KEYWORDSEARCH_ARROW_H);

		ONDK_GUICOM_IMG_Blit(screen, stRectArrow, RES_106_00_ARROW_BOX_LEFT_PNG);

		ONDK_FONT_DrawStringAlignAbbr(screen,hRotateFont,(HCHAR*)ONDK_GetString(RES_PREVIOUS_01_ID),NX_KEYWORDSEARCH_ARROW_TEXT_X ,NX_KEYWORDSEARCH_ARROW_TEXT_Y ,NX_KEYWORDSEARCH_ARROW_TEXT_H,COL(C_T_Gong06),ONDKSTF_LEFT);
	}

	//save the Index Pointers
	if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
	{
		pstContents->ulCurIndexMainMenu = nCurrIdx;
	}
	else
	{
		pstContents->ulCurIndexSubMenu = nCurrIdx;
	}

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

	//This is Global Search
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
		if (nDataIdx == nCurrIdx && ONDK_GWM_APP_GetAppTop() == ONDK_GWM_APP_Get(NX_PRISM_KEYWORDSEARCH_Proc)) /* focus */
		{
			textFontColor = COL(C_T_Bang_100);
			bgColor = COL(C_G_Base01_100);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hBoldFontDescription;
		}
		else
		{
			textFontColor = COL(C_T_Whistle_100);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hNormalFontDescription;
		}

		nx_KeywordSearch_DrawCursor_A2(screen, pstData, &stListRegion, nOffsetY, hFontHandle, hFontDescription,textFontColor, bgColor,nCellHeight,
			((nDataIdx == nCurrIdx) && ONDK_GWM_APP_GetAppTop() == ONDK_GWM_APP_Get(NX_PRISM_KEYWORDSEARCH_Proc)), bUiTop, objID);
		nOffsetY += nCellHeight;
		bgColor = 0x00000000;
	}


	/* Scroll */
	#if 0
	if (bNeedScrollBar == TRUE)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (nTotal / nNumOfPage);
		lAllPage += (nTotal % nNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_KEYWORDSEARCH_SCROLL_BAR_H)
		{
			lScrollBall_height = NX_KEYWORDSEARCH_SCROLL_BAR_H;
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

		/* Draw Scroll Bar */
		ONDK_DRAW_FillRectElement(screen, NX_KEYWORDSEARCH_SCROLL_BAR_X, NX_KEYWORDSEARCH_SCROLL_BAR_Y, NX_KEYWORDSEARCH_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(NX_KEYWORDSEARCH_SCROLL_BAR_X, NX_KEYWORDSEARCH_SCROLL_BAR_Y + lScroll_y_offset, NX_KEYWORDSEARCH_SCROLL_BAR_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}
	#endif

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	ONDK_FONT_Release(hNormalFontDescription);
	ONDK_FONT_Release(hBoldFontDescription);

	ONDK_FONT_Release(hFontCategory);
	ONDK_FONT_Release(hFontCast);
	return  ONDK_RET_OK;
}

static ONDK_Result_t    nx_KeywordSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_KeywordSearchResultData_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId)
{
	HCHAR           pText[DxNAME_LEN] = {0, };
	Nx_KeywordSearchContext_t	*pstContents = NULL;
	ONDK_Rect_t         stRectArrow = {0, 0, 0, 0};
	ONDK_Rect_t 	stRectN = { 0, 0, 0, 0 };
	pstContents = nx_KeywordSearch_GetContents();

	if (bIsFocus == TRUE)
	{

		if((pstData->svcType != 0 && pstData->svcUid == 0) || (pstData->eProgramType != 0 && pstData->svcUid ==0))
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
		//draw the channel group information
		HxSTD_sprintf(pText, "CH %02d", pstData->lcn);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_SVCID_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		HxSTD_sprintf(pText, "%s", pstData->svcName);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_SVCNAME_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		if(pstData->svcType == eDxSVC_TYPE_TV)
		{
			//display TV thumbnail
			stRectN = ONDK_Rect(stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_Y, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_W, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_H);
			ONDK_GUICOM_IMG_Blit(screen, stRectN, RES_212_00_THUMB_TV_PNG);
		}
		else if(pstData->svcType == eDxSVC_TYPE_RADIO)
		{
			//display Radio thumbnail
			stRectN = ONDK_Rect(stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_Y, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_W, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_H);
			ONDK_GUICOM_IMG_Blit(screen, stRectN, RES_212_00_THUMB_RADIO_PNG);
		}

		if(pstData->svcUid == 0)
		{
			if (bIsFocus == FALSE)
			{
				textFontColor = COL(C_T_Gong03);
			}
			//show Title + number of channels
			if(pstData->svcType == eDxSVC_TYPE_TV)
			{
				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_TV_CHANNEL_ID),HLIB_VECTOR_Length(pstData->pstChannelList));
			}
			else if(pstData->svcType == eDxSVC_TYPE_RADIO)
			{

				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_RADIO_CHANNEL_ID),HLIB_VECTOR_Length(pstData->pstChannelList));
			}
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
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
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_TYPE_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		}
	}
	else if(pstData->eProgramType != 0)
	{
		//draw the program group information
		HxSTD_sprintf(pText, "%s", pstData->szProgrammeName);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_PROGRAM_TITLE_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		if(pstData->eProgramType == eSMARTSEARCH_TV_PROGRAM)
		{
			//display TV thumbnail
			stRectN = ONDK_Rect(stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_Y, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_W, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_H);
			ONDK_GUICOM_IMG_Blit(screen, stRectN, RES_212_00_THUMB_TV_PNG);
		}
		else if(pstData->eProgramType == eSMARTSEARCH_RADIO_PROGRAM)
		{
			//display Radio thumbnail
			stRectN = ONDK_Rect(stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_Y, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_W, NX_KEYWORDSEARCH_RESULTMENU_THUMBNAIL_H);
			ONDK_GUICOM_IMG_Blit(screen, stRectN, RES_212_00_THUMB_RADIO_PNG);
		}

		if (bIsFocus == FALSE)
		{
			textFontColor = COL(C_T_Gong03);
		}
		if(pstData->svcUid == 0)
		{
			//draw program group title + number of programs
			if(pstData->eProgramType == eSMARTSEARCH_TV_PROGRAM)
			{
				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_TV_PROGRAMME_ID),HLIB_VECTOR_Length(pstData->pstProgramList));
			}
			else if(pstData->eProgramType == eSMARTSEARCH_RADIO_PROGRAM)
			{
				HxSTD_sprintf(pText, "%s (%d)", (char*)ONDK_GetString(RES_RADIO_PROGRAMME_ID),HLIB_VECTOR_Length(pstData->pstProgramList));
			}
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_PROGRAM_INFO_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

		}
		else
		{
			//draw chhanel information
			HxSTD_sprintf(pText, "%s", (char*)pstData->svcName);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (char*)pText, stListRegion->x + NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_KEYWORDSEARCH_RESULTMENU_PROGRAM_INFO_ITEM_TEXT_Y, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t    nx_KeywordSearch_KeyWord_DrawCursor_A2(ONDKSurfaceHandle *screen, HUINT8 *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId)
{
	Nx_KeywordSearchContext_t	*pstContents = NULL;
	ONDK_Rect_t         stRectArrow = {0, 0, 0, 0};
	pstContents = nx_KeywordSearch_GetContents();

	if (bIsFocus == TRUE)
	{
		ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w - 26, ulCellHeight, bgColor);
		// right arrow
		stRectArrow = ONDK_Rect(stListRegion->x + stListRegion->w - 20, stListRegion->y + nOffsetY + 20, 13, 20);
		ONDK_GUICOM_IMG_Blit(screen, stRectArrow, RES_106_00_ARROW_RIGHT_PNG);
	}

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstData, stListRegion->x + 20, stListRegion->y + nOffsetY + 14, NX_KEYWORDSEARCH_RESULTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);

	return ONDK_RET_OK;
}

static ONDK_Result_t    nx_KeywordSearch_DrawSearchInput(Nx_KeywordSearchContext_t *pstContents,ONDKSurfaceHandle *screen)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	//Draw Input
	stRect.x = 80;
	stRect.y = 42;
	stRect.w = 530;
	stRect.h = 65;

	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_590_SS_INPUT_N_PNG);

	stRect.x = 80 + 16;
	stRect.y = 42 + 18;
	stRect.w = 28;
	stRect.h = 28;

	ONDK_GUICOM_IMG_Blit(screen, stRect, RES_590_08_SEARCH_PNG);

	//Show the current inputted keyword in the input area
	ONDK_FONT_DrawStringAlignAbbr(screen, ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22),
		(char*)pstContents->szInput,(80 + 50),(42 + 21), 450, COL(C_T_Gong04), ONDKSTF_LEFT);

	return ONDK_RET_OK;
}

static Nx_KeywordSearchResultData_t*	nx_KeywordSearch_GetResultOfServiceType(HxList_t* list,DxSvcType_e svcType)
{
	//Group the Channelswith service type
	Nx_KeywordSearchResultData_t *pstResultData;

	if(list == NULL)
	{
		return NULL;
	}

	while(list)
	{
		pstResultData = (Nx_KeywordSearchResultData_t *)list->data;

		if(pstResultData->svcType == svcType)
		{
			return pstResultData;
		}
		list = list->next;
	}

	return NULL;
}

static Nx_KeywordSearchResultData_t*	nx_KeywordSearch_GetResultOfProgramType(Nx_KeywordSearchContext_t *pstContents, Nx_KeywordSearch_ProgramType_e programType, HCHAR* programName)
{
	//Group the Programs with program type
	HUINT32 ulIndex = 0, ulLen = 0;
	Nx_KeywordSearchResultData_t *pstResultData;

	ulLen = HLIB_VECTOR_Length(pstContents->pstResultList);

	for(ulIndex = 0; ulIndex < ulLen; ulIndex++)
	{
		pstResultData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, ulIndex);
		if(pstResultData->eProgramType == programType && HxSTD_StrCmp(pstResultData->szProgrammeName, programName) == 0)
		{
			return pstResultData;
		}
	}

	return NULL;
}

static void		nx_KeywordSearch_ResetResultList(Nx_KeywordSearchContext_t *pstContents)
{
	HUINT32 ulIndex = 0, ulLen = 0;
	Nx_KeywordSearchResultData_t *pstResultData;

	ulLen = HLIB_VECTOR_Length(pstContents->pstResultList);
	for(ulIndex = 0; ulIndex < ulLen; ulIndex++)
	{
		pstResultData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, ulIndex);

		if(pstResultData->pstChannelList != NULL)
		{
			HLIB_VECTOR_Delete(pstResultData->pstChannelList);
		}

		if(pstResultData->pstProgramList!= NULL)
		{
			HLIB_VECTOR_Delete(pstResultData->pstProgramList);
		}

	}

	HLIB_VECTOR_RemoveAll(pstContents->pstResultList);

	return;
}

static void		nx_KeywordSearch_ResetList(HxList_t* list)
{
	Nx_KeywordSearchResultData_t *pstResultData;
	HxList_t *tmp = HLIB_LIST_First(list);

	while(tmp)
	{
		HxList_t *next = tmp->next;

		pstResultData = tmp->data;

		if(pstResultData->pstChannelList != NULL)
		{
			HLIB_VECTOR_Delete(pstResultData->pstChannelList);
		}

		if(pstResultData->pstProgramList!= NULL)
		{
			HLIB_VECTOR_Delete(pstResultData->pstProgramList);
		}

		NX_APP_Free(pstResultData);

		HLIB_MEM_Free(tmp);

		tmp = next;
	}

	return;
}

static ONDK_Result_t	nx_KeywordSearch_CopyResultDataLevel1(Nx_KeywordSearchResultData_t* pstDestination, Nx_KeywordSearchResultData_t* pstSource)
{
	HINT32 lIndex = 0, lLen = 0;
	Nx_KeywordSearchResultData_t* tmp = NULL;

	pstDestination->svcUid = pstSource->svcUid;
	pstDestination->lcn = pstSource->lcn;
	pstDestination->eventId = pstSource->eventId;
	HxSTD_StrNCpy(pstDestination->svcName, pstSource->svcName, DxNAME_LEN);
	HxSTD_StrNCpy(pstDestination->szProgrammeName, pstSource->szProgrammeName, EPG_HUMAX_NAME_LEN);

	pstDestination->eProgramType = pstSource->eProgramType;

	if(pstSource->pstProgramList != NULL)
	{
		pstDestination->pstProgramList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
		lLen = HLIB_VECTOR_Length(pstSource->pstProgramList);

		for(lIndex = 0; lIndex < lLen; lIndex++)
		{
			tmp = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));
			if (tmp == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
			HxSTD_MemSet(tmp, 0, sizeof(Nx_KeywordSearchResultData_t));

			nx_KeywordSearch_CopyResultDataLevel2(tmp,HLIB_VECTOR_ItemAt(pstSource->pstProgramList, lIndex));
			HLIB_VECTOR_Add(pstDestination->pstProgramList, tmp);
		}
	}

	pstDestination->svcType = pstSource->svcType;

	if(pstSource->pstChannelList != NULL)
	{
		pstDestination->pstChannelList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
		lLen = HLIB_VECTOR_Length(pstSource->pstChannelList);

		for(lIndex = 0; lIndex < lLen; lIndex++)
		{
			tmp = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));
			if (tmp == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
			HxSTD_MemSet(tmp, 0, sizeof(Nx_KeywordSearchResultData_t));

			nx_KeywordSearch_CopyResultDataLevel2(tmp,HLIB_VECTOR_ItemAt(pstSource->pstChannelList, lIndex));
			HLIB_VECTOR_Add(pstDestination->pstChannelList, tmp);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_KeywordSearch_CopyResultDataLevel2(Nx_KeywordSearchResultData_t* pstDestination, Nx_KeywordSearchResultData_t* pstSource)
{
	pstDestination->svcUid = pstSource->svcUid;
	pstDestination->lcn = pstSource->lcn;
	pstDestination->eventId = pstSource->eventId;
	HxSTD_StrNCpy(pstDestination->svcName, pstSource->svcName, DxNAME_LEN);
	HxSTD_StrNCpy(pstDestination->szProgrammeName, pstSource->szProgrammeName, EPG_HUMAX_NAME_LEN);

	pstDestination->eProgramType = pstSource->eProgramType;

	pstDestination->svcType = pstSource->svcType;

	return ONDK_RET_OK;
}


static HINT32	nx_KeywordSearch_SortByAlphabet(void *pvArg1, void *pvArg2)
{
	Nx_KeywordSearchResultData_t *pstResultData1 = (Nx_KeywordSearchResultData_t *)pvArg1;
	Nx_KeywordSearchResultData_t *pstResultData2 = (Nx_KeywordSearchResultData_t *)pvArg2;

	if(NULL == pstResultData1) return 1;
	if(NULL == pstResultData2) return -1;

	if(pstResultData1->svcType == pstResultData2->svcType && pstResultData1->svcType != 0)
	{
		return HxSTD_StrCmp(pstResultData2->svcName, pstResultData1->svcName);
	}

	if(pstResultData2->eProgramType != 0 && pstResultData1->eProgramType != 0)
	{
		return HxSTD_StrCmp(pstResultData2->szProgrammeName, pstResultData1->szProgrammeName);
	}

	return 1;
}

static ONDK_Result_t	nx_KeywordSearch_CopyFromListToVector(HxVector_t* vector, HxList_t* list)
{
	Nx_KeywordSearchResultData_t* tmp = NULL;
	ONDK_Result_t res = ONDK_RET_FAIL;
	if(list == NULL)
	{
		return ONDK_RET_OK;
	}

	while(list)
	{
		tmp = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));

		if (tmp == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(tmp, 0, sizeof(Nx_KeywordSearchResultData_t));

		res = nx_KeywordSearch_CopyResultDataLevel1(tmp, list->data);
		if(res == ONDK_RET_OK)
		{
			HLIB_VECTOR_Add(vector, tmp);
		}
		else
		{
			NX_APP_Free(tmp);
		}

		list = list->next;
	}
	HLIB_LIST_RemoveAll(list);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_KeywordSearch_StartSearchWithInput(Nx_KeywordSearchContext_t *pstContents, HCHAR* pcInput)
{
	HINT32			lIndex = 0, lChannelNum = 0, lStrLen = 0, curIndex = 0;
	NX_Channel_t	*pstChannelList = NULL, *tmpChannel = NULL;
	DxService_t		*tempSvc = NULL;
	Nx_KeywordSearchResultData_t *pstResultData = NULL, *pstChannelResultData = NULL, *pstAllProgram = NULL, *pstProgramResultData = NULL;
	Nx_KeywordSearchResultData_t* pstCurData = NULL;
	HxList_t	*sortedListTVChannel = NULL, *sortedListRadioChannel = NULL;
	HxList_t	*allProgramList = NULL;

	lStrLen = HxSTD_StrLen(pcInput);
	//start searching only from 2 characters
	if (lStrLen < 2)
	{
		return ONDK_RET_OK;
	}

	//set the empty data for Result Menu
	nx_KeywordSearch_ResetResultList(pstContents);

	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);

	//Search Channels
	lChannelNum = pstContents->nChannelNum;
	pstChannelList = pstContents->pstChannels;

	//Search TV and RADIO channel
	for(lIndex = 0; lIndex < lChannelNum; lIndex++)
	{
		tmpChannel = &pstChannelList[lIndex];
		tempSvc = NX_CHANNEL_FindServiceBySvcUid(tmpChannel->svcUid);

		if(HLIB_STD_StrCaseStr(tempSvc->name, pcInput) != NULL)
		{
			if(tempSvc->svcType == eDxSVC_TYPE_TV)
			{
				pstResultData = nx_KeywordSearch_GetResultOfServiceType(sortedListTVChannel, tempSvc->svcType);
			}
			else if(tempSvc->svcType == eDxSVC_TYPE_RADIO)
			{
				pstResultData = nx_KeywordSearch_GetResultOfServiceType(sortedListRadioChannel, tempSvc->svcType);
			}

			pstChannelResultData = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));

			if (pstChannelResultData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
			HxSTD_MemSet(pstChannelResultData, 0, sizeof(Nx_KeywordSearchResultData_t));

			HxSTD_StrNCpy((char*)pstChannelResultData->svcName,tempSvc->name,MAX_ELEMENT_TEXT_LENGTH);
			pstChannelResultData->svcUid = tmpChannel->svcUid;
			pstChannelResultData->lcn = tempSvc->lcn;

			if(pstChannelResultData->lcn == 0)
			{
				pstChannelResultData->lcn = tmpChannel->lcn;
			}

			pstChannelResultData->svcType = tempSvc->svcType;

			if(pstResultData == NULL)
			{
				pstResultData = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));

				if (pstResultData == NULL)
                {
                    NX_APP_Free(pstChannelResultData);
                    pstChannelResultData = NULL;
                    return GWM_RET_OUT_OF_MEMORY;
                }

				HxSTD_MemSet(pstResultData, 0, sizeof(Nx_KeywordSearchResultData_t));

				pstResultData->svcType = tempSvc->svcType;
				pstResultData->pstChannelList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

				//set the service name and lcn for ChannelList the same as the first channel
				HxSTD_StrNCpy((char*)pstResultData->svcName,tempSvc->name,MAX_ELEMENT_TEXT_LENGTH);
				pstResultData->lcn = pstChannelResultData->lcn;
				pstResultData->svcUid = 0;

				//HLIB_VECTOR_Add(pstContents->pstResultList, pstResultData);
				if(tempSvc->svcType == eDxSVC_TYPE_TV)
				{
					sortedListTVChannel= HLIB_LIST_AppendSortEx(sortedListTVChannel, pstResultData, nx_KeywordSearch_SortByAlphabet);
				}
				else if(tempSvc->svcType == eDxSVC_TYPE_RADIO)
				{
					sortedListRadioChannel	= HLIB_LIST_AppendSortEx(sortedListRadioChannel, pstResultData, nx_KeywordSearch_SortByAlphabet);
				}
			}

			HLIB_VECTOR_Add(pstResultData->pstChannelList, pstChannelResultData);
		}

	}
	//copy the results data from list to vector
	nx_KeywordSearch_CopyFromListToVector(pstContents->pstResultList, sortedListTVChannel);
	nx_KeywordSearch_ResetList(sortedListTVChannel);

	nx_KeywordSearch_CopyFromListToVector(pstContents->pstResultList, sortedListRadioChannel);
	nx_KeywordSearch_ResetList(sortedListRadioChannel);

	lStrLen = HxSTD_StrLen(pcInput);
	//start searching the Program only from 3 characters
	if(lStrLen >= 3)
	{
		//Search TV & Raadio Program
		allProgramList = pstContents->pstAllProgramList;

		while(allProgramList)
		{
			pstAllProgram = allProgramList->data;

			if(HLIB_STD_StrCaseStr(pstAllProgram->szProgrammeName, pcInput) != NULL)
			{
				pstResultData = nx_KeywordSearch_GetResultOfProgramType(pstContents, pstAllProgram->eProgramType, pstAllProgram->szProgrammeName);

				pstProgramResultData = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));
				if (pstProgramResultData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

				HxSTD_MemSet(pstProgramResultData, 0, sizeof(Nx_KeywordSearchResultData_t));
				HxSTD_StrNCpy((char*)pstProgramResultData->szProgrammeName, (char*)pstAllProgram->szProgrammeName, MAX_ELEMENT_TEXT_LENGTH);
				pstProgramResultData->svcUid = pstAllProgram->svcUid;
				pstProgramResultData->eventId = pstAllProgram->eventId;
				HxSTD_StrNCpy((char*)pstProgramResultData->svcName,pstAllProgram->svcName,MAX_ELEMENT_TEXT_LENGTH);
				pstProgramResultData->eProgramType = pstAllProgram->eProgramType;

				if(pstResultData == NULL)
				{
					pstResultData = (Nx_KeywordSearchResultData_t *)NX_APP_Malloc (sizeof(Nx_KeywordSearchResultData_t));
					if (pstResultData == NULL)
	                {
	                    NX_APP_Free(pstProgramResultData);
						pstProgramResultData = NULL;
	                    return GWM_RET_OUT_OF_MEMORY;
	                }

					HxSTD_MemSet(pstResultData, 0, sizeof(Nx_KeywordSearchResultData_t));

					pstResultData->eProgramType = pstAllProgram->eProgramType;
					pstResultData->pstProgramList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
					HxSTD_StrNCpy((char*)pstResultData->szProgrammeName,pstAllProgram->szProgrammeName,MAX_ELEMENT_TEXT_LENGTH);
					pstResultData->svcUid = 0;
					HLIB_VECTOR_Add(pstContents->pstResultList, pstResultData);
				}

				HLIB_VECTOR_Add(pstResultData->pstProgramList, pstProgramResultData);
			}

			allProgramList = allProgramList->next;
		}
	}


	//set the new data for Result Menu

	if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
	{
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);
		ONDK_GWM_List_SetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, pstContents->ulCurIndexMainMenu);
	}
	else
	{

		HxLOG_Debug("Refresh resulted search\n\n");
		curIndex = pstContents->ulCurIndexMainMenu;
		pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, curIndex);

		if(pstCurData->svcType != 0 && pstCurData->svcUid == 0)
		{
			//set selected category
			if(pstCurData->svcType == eDxSVC_TYPE_TV)
			{
				pstContents->eSelectedCategory = eSMARTSEARCH_TV_CHANNEL_CATEGORY;
			}
			else if(pstCurData->svcType == eDxSVC_TYPE_RADIO)
			{
				pstContents->eSelectedCategory = eSMARTSEARCH_RADIO_CHANNEL_CATEGORY;
			}

			ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstCurData->pstChannelList), (GWM_LIST_ITEMS)pstCurData->pstChannelList);
		}
		else if(pstCurData->eProgramType != 0 && pstCurData->svcUid == 0)
		{
			//set selected category
			if(pstCurData->eProgramType== eSMARTSEARCH_TV_PROGRAM)
			{
				pstContents->eSelectedCategory = eSMARTSEARCH_TV_PROGRAM_CATEGORY;
			}
			else if(pstCurData->eProgramType == eSMARTSEARCH_RADIO_PROGRAM)
			{
				pstContents->eSelectedCategory = eSMARTSEARCH_RADIO_PROGRAM_CATEGORY;
			}

			ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstCurData->pstProgramList), (GWM_LIST_ITEMS)pstCurData->pstProgramList);
		}

		ONDK_GWM_List_SetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, pstContents->ulCurIndexSubMenu);
	}

	return ONDK_RET_OK;

}

static ONDK_Result_t nx_KeywordSearch_GetAllProgrammFromChannels(Nx_KeywordSearchContext_t *pstContents, HUINT32 ulIdxStartChannel, HUINT32 ulIdxEndChannel)
{
	HUINT32 ulChannelNum = 0, ulIndex = 0, lStrLen = 0;
	NX_Channel_t* pstChannelList, *tmpChannel;
	HINT32 num = 0, j = 0;
	HBOOL isFoundProgram = FALSE;
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
				HUINT32 ulStartTime = 0;
				HBOOL isValid = FALSE;

				NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, j);

				//check if the found program is in the searched time period
				ulStartTime = NX_METADATA_PROGRAMMES_GetStartTime(item);
				if((pstContents->ulStartTimeQueryStart == pstContents->ulSystemTime) || (ulStartTime > pstContents->ulStartTimeQueryStart && ulStartTime < pstContents->ulStartTimeQueryEnd))
				{
					isValid = TRUE;
				}

				if(isValid == TRUE)
				{
					HCHAR programmeName[EPG_HUMAX_NAME_LEN] = {'\0',};
					Nx_KeywordSearchResultData_t *data = (Nx_KeywordSearchResultData_t*)NX_APP_Calloc(sizeof(Nx_KeywordSearchResultData_t));

					data->svcUid 	= NX_METADATA_PROGRAMMES_GetSvcUID(item);
					data->eventId	= NX_METADATA_PROGRAMMES_GetEventID(item);
					data->lcn		= tempSvc->lcn;
					if(tempSvc->svcType == eDxSVC_TYPE_TV)
					{
						data->eProgramType = eSMARTSEARCH_TV_PROGRAM;
					}
					else if(tempSvc->svcType == eDxSVC_TYPE_RADIO)
					{

						data->eProgramType = eSMARTSEARCH_RADIO_PROGRAM;
					}
					HxSTD_MemSet(data->svcName, 0, DxNAME_LEN);
					HxSTD_sprintf(data->svcName,"%s",tempSvc->name);

					if (NX_METADATA_PROGRAMMES_GetName(item, programmeName) == ERR_OK)
					{
						HxSTD_MemSet(data->szProgrammeName, 0, EPG_HUMAX_NAME_LEN);
						HxSTD_sprintf(data->szProgrammeName,"%s", programmeName);
					}

					HxLOG_Debug("Add Program (%s) svcUid(%d) eventId(%d) \n\n",data->szProgrammeName,data->svcUid,data->eventId);
					//sort the searched programs with alphabet order
					pstContents->pstAllProgramList = HLIB_LIST_AppendSortEx(pstContents->pstAllProgramList, data, nx_KeywordSearch_SortByAlphabet);
					isFoundProgram = TRUE;
				}
			}
		}
		HLIB_VECTOR_Delete(result);
	}

	lStrLen = HxSTD_StrLen(pstContents->szInput);

	if(lStrLen >= 2 && isFoundProgram == TRUE && ONDK_GWM_APP_GetAppTop() != ONDK_GWM_APP_Get(NX_PRISM_KEYWORDSEARCH_Proc))
	{
		nx_KeywordSearch_StartSearchWithInput(pstContents,pstContents->szInput);
	}

	return ONDK_RET_OK;

}

static ONDK_Result_t	nx_KeywordSearch_SearchProgrammesFromChannels(Nx_KeywordSearchContext_t *pstContents)
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

static void nx_KeywordSearch_GetProgramFromResults(Nx_KeywordSearchContext_t *pstContents)
{
	HxLOG_Debug("\n\n\n Search finished\n\n\n");
	if(ONDK_GWM_IsTimer(NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID) == TRUE)
	{
		return;
	}

	pstContents->ulIdxStartChannel = 0;
	pstContents->ulChannelCount = pstContents->nChannelNum/3;

	nx_KeywordSearch_GetAllProgrammFromChannels(pstContents, pstContents->ulIdxStartChannel, pstContents->ulIdxStartChannel + pstContents->ulChannelCount);

	//Set timer for getting Program
	nx_KeywordSearch_SetTimerGetProgram();

	return;
}

static ONDK_Result_t	nx_KeywordSearch_SetTimerGetProgram(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	retTimer = ONDK_GWM_SetTimer(NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID, (NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_INTERVAL) * 1000);

	if (retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_KeywordSearch_SetTimerExit(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	retTimer = ONDK_GWM_SetTimer(NX_KEYWORDSEARCH_EXIT_TIMER_ID, (NX_KEYWORDSEARCH_EXIT_TIMER_INTERVAL) * 1000);

	if (retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_KEYWORDSEARCH_EXIT_TIMER_ID);
	}

	return ONDK_RET_OK;
}


static inline Nx_KeywordSearchContext_t	*nx_KeywordSearch_GetContents(void)
{
	return	&s_stSmartSearchContent;
}

static ONDK_Result_t	nx_KeywordSearch_MsgGwmCreate(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HUINT32 	unixtime = 0, nTotal = 0;
	HINT32			lSortChNum = 0;
	HxList_t		*pSortList = NULL;
	NX_Channel_t	*pstSortChLists = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents is NULL !!\n");
		return ONDK_RET_FAIL;
	}

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

	//Sort Channellist by name
	pSortList = NX_CHANNEL_GROUP_SortNxChannelsByName(pstContents->pstChannels, pstContents->nChannelNum);
	if (pSortList == NULL)
	{
		NX_APP_Error("Error!!! NX_CHANNEL_GROUP_SortNxChannelsByName() fail!!\n");
		return ONDK_RET_FAIL;
	}

	NX_CHANNEL_GROUP_MakeNxChannelListByList(pSortList,  &pstSortChLists, &lSortChNum);

	// sort list remove data
	HLIB_LIST_RemoveAll(pSortList);

	// remove the current channelist
	if (pstContents->pstChannels != NULL)
	{
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// assign the sorted channelist to current context
	pstContents->pstChannels = pstSortChLists;
	pstContents->nChannelNum = lSortChNum;

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
	nx_KeywordSearch_SearchProgrammesFromChannels(pstContents);

	//initiate ResultData
	pstContents->pstResultList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	//pstContents->pstAllProgramList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	//Create result menu
	nTotal = HLIB_VECTOR_Length(pstContents->pstResultList);
	nx_KeywordSearch_CreateResultMenu(pstContents->pstResultList, nTotal, (GWM_ObjDraw_t)nx_KeywordSearch_DrawMenu);

	pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;

	//show simple keyboard
	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SimpleKeyboard_Proc) == NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SimpleKeyboard_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SimpleKeyboard_Proc, HANDLE_NULL, 0, 0, 0);
	}

	nx_KeywordSearch_SetTimerExit();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_KeywordSearch_MsgGwmDestroy(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_Destroy(NX_PRISM_SimpleKeyboard_Proc);
	ONDK_GWM_Obj_Destroy(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);

	if(pstContents->pstResultList)
	{
		nx_KeywordSearch_ResetResultList(pstContents);
		HLIB_VECTOR_Delete(pstContents->pstResultList);
	}

	if(pstContents->pstAllProgramList)
	{
		nx_KeywordSearch_ResetList(pstContents->pstAllProgramList);
	}

	if(pstContents->pstChannels != NULL)
	{
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	ONDK_GWM_KillTimer(NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID); //kill Searching timer
	ONDK_GWM_KillTimer(NX_KEYWORDSEARCH_EXIT_TIMER_ID); //kill exit timer

	HxSTD_MemSet(pstContents, 0x00, sizeof(Nx_KeywordSearchContext_t));

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_KeywordSearch_MsgGwmKeyDown(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	HUINT32 curIndex = 0;
	Nx_KeywordSearchResultData_t* pstCurData;
	HxVector_t* emptyList;

	if (pstContents == NULL)
	{
		NX_APP_Assert(pstContents);
		return	GWM_RET_SOURCE_NULL;
	}

	HxLOG_Debug("\n\n\n receive key value (%d)\n\n\n", lKeyId);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);

	nx_KeywordSearch_SetTimerExit();

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_SMART_SEARCH:
			Key_SmartSearch_Label:

			//ONDK_GWM_APP_SwapStackOrder(NX_PRISM_SimpleKeyboard_Proc,NX_PRISM_KEYWORDSEARCH_Proc);
			//show simple keyboard
			if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SimpleKeyboard_Proc) == NULL)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_SimpleKeyboard_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SimpleKeyboard_Proc, HANDLE_NULL, 0, 0, (HINT32)pstContents->szInput);
			}

			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			Key_ArrowRight_Label:
			{
				if(pstContents->eIdxMenu == eSMARTSEARCH_MAINMENU)
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);
					HxLOG_Debug("\n\n\n curIndex (%d)\n\n\n", curIndex);
					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList, curIndex);

					if(pstCurData->svcType != 0 && pstCurData->svcUid == 0)
					{
						//set selected category
						if(pstCurData->svcType == eDxSVC_TYPE_TV)
						{
							pstContents->eSelectedCategory = eSMARTSEARCH_TV_CHANNEL_CATEGORY;
						}
						else if(pstCurData->svcType == eDxSVC_TYPE_RADIO)
						{
							pstContents->eSelectedCategory = eSMARTSEARCH_RADIO_CHANNEL_CATEGORY;
						}
						//save current index of main menu
						pstContents->ulCurIndexMainMenu = curIndex;
						emptyList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(emptyList), (GWM_LIST_ITEMS)emptyList);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstCurData->pstChannelList), (GWM_LIST_ITEMS)pstCurData->pstChannelList);
					}
					else if(pstCurData->eProgramType != 0 && pstCurData->svcUid == 0)
					{
						//set selected category
						if(pstCurData->eProgramType== eSMARTSEARCH_TV_PROGRAM)
						{
							pstContents->eSelectedCategory = eSMARTSEARCH_TV_PROGRAM_CATEGORY;
						}
						else if(pstCurData->eProgramType == eSMARTSEARCH_RADIO_PROGRAM)
						{
							pstContents->eSelectedCategory = eSMARTSEARCH_RADIO_PROGRAM_CATEGORY;
						}

						//save current index of main menu
						pstContents->ulCurIndexMainMenu = curIndex;
						emptyList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(emptyList), (GWM_LIST_ITEMS)emptyList);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstCurData->pstProgramList), (GWM_LIST_ITEMS)pstCurData->pstProgramList);
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
			else
			{
				goto Key_ArrowLeft_Label;
			}

			break;

		case KEY_ARROWLEFT:
			Key_ArrowLeft_Label:
			{
				if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
				{
					HxVector_t* tmp = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
					pstContents->eIdxMenu = eSMARTSEARCH_MAINMENU;
					//reset selected category
					pstContents->eSelectedCategory = eSMARTSEARCH_NO_CATEGORY;
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(tmp), (GWM_LIST_ITEMS)tmp);
					ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID), HLIB_VECTOR_Length(pstContents->pstResultList), (GWM_LIST_ITEMS)pstContents->pstResultList);
					ONDK_GWM_List_SetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID, pstContents->ulCurIndexMainMenu);
				}
				return ONDK_MESSAGE_BREAK;
			}
		case KEY_ARROWUP:
			curIndex = ONDK_GWM_List_GetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);
			if(curIndex == 0)
			{
				goto Key_SmartSearch_Label;
			}

			break;

		case KEY_ARROWDOWN:
			curIndex = ONDK_GWM_List_GetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);
			break;

		case KEY_OK:
			{
				NX_ChListGroup_t group;
				NX_ChListGroup_t currentGroup;
				NX_Channel_t* pstSelectedChannel = NULL;

				if(pstContents->eIdxMenu == eSMARTSEARCH_SUBMENU)
				{
					curIndex = ONDK_GWM_List_GetCurrentIndex(NX_KEYWORDSEARCH_RESULTMENU_MAIN_OBJ_ID);

					//Get Data of Main Menu
					pstCurData = HLIB_VECTOR_ItemAt(pstContents->pstResultList,pstContents->ulCurIndexMainMenu);
					HxLOG_Debug("main menu index (%d) curIndex(%d)",pstContents->ulCurIndexMainMenu,curIndex);

					if(pstCurData->pstChannelList != NULL)
					{
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
					else if(pstCurData->pstProgramList != NULL)
					{
						pstCurData = HLIB_VECTOR_ItemAt(pstCurData->pstProgramList, curIndex);
						HxLOG_Debug("sub menu program svcUid(%d) eventId(%d)",pstCurData->svcUid,pstCurData->eventId);
						if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)Nx_PRISM_ProgramDetail_Proc) ==NULL )
						{
							ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, pstCurData->svcUid, pstCurData->eventId, eNX_UCT_Guide);
						}
						return ONDK_MESSAGE_CONSUMED;
					}
					break;
				}
				else
				{
					goto Key_ArrowRight_Label;
				}
			}

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			//return	ONDK_MESSAGE_PASS;
			return ONDK_MESSAGE_BREAK;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_KeywordSearch_MsgGwmTimer(Nx_KeywordSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	switch(p1)
	{
		case NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID:
			{
				HUINT32 idxEnd = 0;

				pstContents->ulIdxStartChannel += pstContents->ulChannelCount;
				idxEnd  = pstContents->ulIdxStartChannel + pstContents->ulChannelCount;

				if(idxEnd >= (HUINT32)pstContents->nChannelNum)
				{
					idxEnd = pstContents->nChannelNum;
				}

				nx_KeywordSearch_GetAllProgrammFromChannels(pstContents, pstContents->ulIdxStartChannel, idxEnd);

				if(idxEnd == (HUINT32)pstContents->nChannelNum)
				{
					ONDK_GWM_KillTimer(NX_KEYWORDSEARCH_GET_PROGRAM_TIMER_ID);

					pstContents->ulStartTimeQueryStart = pstContents->ulStartTimeQueryEnd;
					pstContents->ulStartTimeQueryEnd += SECONDS_PER_HOUR * 6;

					if(pstContents->ulStartTimeQueryStart < pstContents->ulStartTimeQueryFinish)
					{
						nx_KeywordSearch_SearchProgrammesFromChannels(pstContents);
					}
					else
					{
						HxLOG_Debug("\n\n\n Finish finding the Program on Channels\n\n\n");
					}
				}
			}
			break;

		case NX_KEYWORDSEARCH_EXIT_TIMER_ID:
			ONDK_GWM_APP_Destroy(0);
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
ONDK_Result_t	NX_PRISM_KEYWORDSEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_KeywordSearchContext_t	*pstContents = NULL;

	pstContents = nx_KeywordSearch_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_KeywordSearch_MsgGwmCreate(pstContents, p1, p2, p3);
			if (eResult != ONDK_RET_OK)
			{
				HxLOG_Error("Failed Init TvGuide!!!\n\n");
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_NotAvailable_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_NotAvailable_Proc, 0, 0, 0, 0);
			}
			break;

		case MSG_GWM_TIMER:
			nx_KeywordSearch_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_KeywordSearch_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_KeywordSearch_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			//eResult = nx_guide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			HxLOG_Debug("\n\n\n  ======>>> RECEIVED MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES IN GENRE SEARCH\n\n\n");
			nx_KeywordSearch_GetProgramFromResults(pstContents);
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
			nx_KeywordSearch_GetInputFromKeyBoard(pstContents,p3);
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



