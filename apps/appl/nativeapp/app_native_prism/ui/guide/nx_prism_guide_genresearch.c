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
 * @file	  		nx_prism_guide_genresearch.c
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

#define	NX_PRISM_GUIDE_GENRE_ITEM_ID						(eMenuFrame_Window_GUIDE + 300)

#define	NX_PRISM_GUIDE_GENRE_WINDOW_X						0
#define	NX_PRISM_GUIDE_GENRE_WINDOW_Y						0
#define	NX_PRISM_GUIDE_GENRE_WINDOW_W						1280
#define	NX_PRISM_GUIDE_GENRE_WINDOW_H						720

#define	NX_GENRESEARCH_BACKGROUND_BOX_ID					(NX_PRISM_GUIDE_GENRE_ITEM_ID + 1)
#define	NX_GENRESEARCH_BACKGROUND_BOX_X						0
#define	NX_GENRESEARCH_BACKGROUND_BOX_Y						0
#define	NX_GENRESEARCH_BACKGROUND_BOX_WIDTH					1280
#define	NX_GENRESEARCH_BACKGROUND_BOX_HEIGHT				720

#define	NX_GENRESEARCH_DIVISTION_LINE_ID					(NX_PRISM_GUIDE_GENRE_ITEM_ID + 2)
#define	NX_GENRESEARCH_DIVISTION_LINE_X						480
#define	NX_GENRESEARCH_DIVISTION_LINE_Y						103
#define	NX_GENRESEARCH_DIVISTION_LINE_W						1
#define	NX_GENRESEARCH_DIVISTION_LINE_H						617

#define	NX_GENRESEARCH_BOTTOM_DIM_ID						(NX_PRISM_GUIDE_GENRE_ITEM_ID + 3)
#define	NX_GENRESEARCH_BOTTOM_DIM_X							0
#define	NX_GENRESEARCH_BOTTOM_DIM_Y							630
#define	NX_GENRESEARCH_BOTTOM_DIM_W							1280
#define	NX_GENRESEARCH_BOTTOM_DIM_H							90

#define NX_GENRESEARCH_FIRSTMENU_MAIN_BG_ID				(NX_PRISM_GUIDE_GENRE_ITEM_ID + 4)
#define NX_GENRESEARCH_FIRSTMENU_MAIN_BG_X				60
#define NX_GENRESEARCH_FIRSTMENU_MAIN_BG_Y				103
#define NX_GENRESEARCH_FIRSTMENU_MAIN_BG_W				410
#define NX_GENRESEARCH_FIRSTMENU_MAIN_BG_H				600

#define NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID			(NX_PRISM_GUIDE_GENRE_ITEM_ID + 5)
#define NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_X				NX_GENRESEARCH_FIRSTMENU_MAIN_BG_X
#define NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_Y				NX_GENRESEARCH_FIRSTMENU_MAIN_BG_Y
#define NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_W				NX_GENRESEARCH_FIRSTMENU_MAIN_BG_W
#define NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_H				NX_GENRESEARCH_FIRSTMENU_MAIN_BG_H

#define NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_W				374
#define NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_X				18
#define NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_Y				17

#define NX_GENRESEARCH_FIRSTMENU_ITEM_H					60
#define NX_GENRESEARCH_FIRSTMENU_OBJ_PAGE_COUNT			9

#define NX_GENRESEARCH_FIRSTMENU_NUM						11

#define NX_GENRESEARCH_SECONDMENU_MAIN_BG_ID				(NX_PRISM_GUIDE_GENRE_ITEM_ID + 6)
#define NX_GENRESEARCH_SECONDMENU_MAIN_BG_X				491
#define NX_GENRESEARCH_SECONDMENU_MAIN_BG_Y				103
#define NX_GENRESEARCH_SECONDMENU_MAIN_BG_W				729//716 + 13
#define NX_GENRESEARCH_SECONDMENU_MAIN_BG_H				600

#define NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID			(NX_PRISM_GUIDE_GENRE_ITEM_ID + 7)
#define NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_X			NX_GENRESEARCH_SECONDMENU_MAIN_BG_X
#define NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_Y				NX_GENRESEARCH_SECONDMENU_MAIN_BG_Y
#define NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_W				729//NX_GENRESEARCH_SECONDMENU_MAIN_BG_W
#define NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_H				NX_GENRESEARCH_SECONDMENU_MAIN_BG_H

#define NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_W				693
#define NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_X				18
#define NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_Y				17


#define NX_GENRESEARCH_SECONDMENU_ITEM_H					60
#define NX_GENRESEARCH_SECONDMENU_OBJ_PAGE_COUNT			9

#define	NX_GENRESEARCH_PATH_ID							(NX_PRISM_GUIDE_GENRE_ITEM_ID + 8)
#define	NX_GENRESEARCH_PATH_X								60
#define	NX_GENRESEARCH_PATH_Y								28
#define	NX_GENRESEARCH_PATH_W								800
#define	NX_GENRESEARCH_PATH_H								48

#define	NX_GENRESEARCH_NUMBER_OF_ITEM_ID					(NX_PRISM_GUIDE_GENRE_ITEM_ID + 9)
#define	NX_GENRESEARCH_NUMBER_OF_ITEM_X						1080
#define	NX_GENRESEARCH_NUMBER_OF_ITEM_Y						57
#define	NX_GENRESEARCH_NUMBER_OF_ITEM_W						140
#define	NX_GENRESEARCH_NUMBER_OF_ITEM_H						48

#define NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_W			680
#define NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_X			18
#define NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_Y			12

#define NX_GENRESEARCH_DETAILEDMENU_ITEM_H					81

#define NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_W			680
#define NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_X			18
#define NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_Y			43

#define NX_GENRESEARCH_DETAILEDMENU_OBJ_PAGE_COUNT			6



#define	NX_GENRESEARCH_OPTION_FRAME_ID			(NX_PRISM_GUIDE_GENRE_ITEM_ID + 10)
#define	NX_GENRESEARCH_OPTION_FRAME_X			900
#define	NX_GENRESEARCH_OPTION_FRAME_Y			630
#define	NX_GENRESEARCH_OPTION_FRAME_W			400
#define	NX_GENRESEARCH_OPTION_FRAME_H			90

#define	NX_GENRESEARCH_OPTION_TEXT_ID			(NX_PRISM_GUIDE_GENRE_ITEM_ID + 11)
#define	NX_GENRESEARCH_OPTION_TEXT_X			960
#define	NX_GENRESEARCH_OPTION_TEXT_Y			653
#define	NX_GENRESEARCH_OPTION_TEXT_W			400
#define	NX_GENRESEARCH_OPTION_TEXT_H			28
#define	NX_GENRESEARCH_OPTION_IMG_MG			10

#define	NX_GENRESEARCH_PROCESSING_ID			(NX_PRISM_GUIDE_GENRE_ITEM_ID + 12)

#define	NX_GENRESEARCH_SCROLL_BAR_X_MG				8
#define	NX_GENRESEARCH_SCROLL_BAR_X				1217
#define	NX_GENRESEARCH_SCROLL_BAR_Y				103
#define	NX_GENRESEARCH_SCROLL_BAR_W				3
#define NX_GENRESEARCH_SCROLL_BAR_H				5



/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tagGenreResultData {
	HINT32	svcUid;
	HINT32 	lcn;
	HINT32	eventId;
	HUINT32	startTime;
	HCHAR	eventName[DxNAME_LEN];
	HCHAR	szProgrammeName[EPG_HUMAX_NAME_LEN];
	HCHAR	szEventDescription[MAX_ELEMENT_TEXT_LENGTH];
} Nx_GenreResultData_t;

typedef struct tagGenreSearchContext {

	HUINT8              ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucPath[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucNumberOfItem[MAX_ELEMENT_TEXT_LENGTH];
	HINT32	nChannelNum;
	NX_Channel_t *pstChannels;				// live proc 에서 얻어옴..
	HUINT32	unStandardSearchTime;			// 최초 시작 time 저장.

	HUINT8	majorNibble, minorNibble;

	HxVector_t	*pstResultList;

	HUINT32 	ulFirstMenuNum;//number of the first menu
	HxVector_t  *pstFirstMenu; //list of the first menu
	HUINT32		ulcurIndexFirstMenu;
	ONDK_GWM_GuiObject_t objFirstMenu;

	HxVector_t	*pstSecondMenu;//list of the second menu
	HUINT32		ulcurIndexSecondMenu;
	ONDK_GWM_GuiObject_t objSecondMenu;

	HxVector_t *pstThirdMenu; //list of the third menu
	HUINT32		ulcurIndexThirdMenu;

	HxVector_t	*pstDetailedMenu; //list of searched programme
	HUINT32	ulcurIndexDetailedMenu;


	HUINT8	ucDisplayedDataInObjFirstMenu;// 1 for FirstMenu and 3 for Third Menu

	HUINT8 	ucDisplayedDataInObjSecondMenu; // 2 for SecondMenu and 4 for Detailed Menu

	//Option item
	HUINT8		**szItemTextList;
	NX_Util_OPTIONList_t	stOptList;

	eNxUiUtil_OptItem_t	eChangedSort;					// Sorting Item

	HBOOL 	isSearch;

} Nx_GenreSearchContext_t;

typedef struct tagGenreSearchFirstMenu
{
	HBOOL                   bSelect;
	HCHAR                   ucName[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR					ucEventDescription[MAX_ELEMENT_TEXT_LENGTH];
} Nx_GenreSearchMenuItem_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_GenreSearchContext_t		s_stGenreSearchContent;

static eNxUiUtilGenreNibbleLevel1Type_t s_NxGenreSearch_ArrayOfFirstMenu[NX_GENRESEARCH_FIRSTMENU_NUM]={
	eNxUtil_NibbleLevel1_ArtsCulture, eNxUtil_NibbleLevel1_ChildrensYouth,eNxUtil_NibbleLevel1_EducationScienceFactureTopics,
	eNxUtil_NibbleLevel1_LeisureHobbies, eNxUtil_NibbleLevel1_MovieDrama,eNxUtil_NibbleLevel1_MusicBalletDance,
	eNxUtil_NibbleLevel1_NewsCurrentAffairs, eNxUtil_NibbleLevel1_ShowGameShow, eNxUtil_NibbleLevel1_SocialPoliticalEconomics,
	eNxUtil_NibbleLevel1_SpecialCharacteristics,eNxUtil_NibbleLevel1_Sports

};

// option popup
static Nx_PrismListDialog_Content_t		s_stGenreSearchOptContents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_GenreSearchContext_t	*nx_genreSearch_GetContents(void);
static ONDK_Result_t 	Nx_genreSearch_InitFirstMenuData(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t	Nx_genreSearch_GetSecondMenu(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t 	Nx_genreSearch_GetThirdMenu(Nx_GenreSearchContext_t *pstContents);
static void Nx_genreSearch_CreateFirstMenu(HxVector_t *pstFirstMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod);
static ONDK_Result_t    Nx_genreSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t    Nx_genreSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_GenreSearchMenuItem_t* pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor,HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId);
static ONDK_Result_t 	Nx_genreSearch_DrawPath(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t 	Nx_genreSearch_UpdatePath(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t 	Nx_genreSearch_DrawNumberOfItem(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t 	Nx_genreSearch_UpdateNumberOfItem(Nx_GenreSearchContext_t *pstContents, HBOOL isKeyDown, HBOOL isEmpty);
static ONDK_Result_t 	Nx_genreSearch_UpdateNumberOfItemForPaging(Nx_GenreSearchContext_t *pstContents);
static void				nx_genreSearch_SetUiCreateOptionMenu(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t	nx_genreSearch_OptionInitData(Nx_GenreSearchContext_t *pstContents);
static void				nx_genreSearch_CreateListPopup(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t	nx_genreSearch_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_genreSearch_DestroyListPopup(void);
static ONDK_Result_t	nx_genreSearch_ChangedOptionItemValue(Nx_GenreSearchContext_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t 	Nx_genreSearch_DrawSearchProcessing(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t	nx_genreSearch_Search(Nx_GenreSearchContext_t *pstContents, HUINT8 majorNibble, HUINT8 minorNibble);
static HINT32 			nx_genreSearch_SortByTime (const void *pvArg1, const void *pvArg2);
static HINT32 			nx_genreSearch_SortByLCN (const void *pvArg1, const void *pvArg2);
static HINT32 			nx_genreSearch_SortByName(void *pvArg1,void *pvArg2);
HxList_t *				nx_genreSearch_SortNxChannelsByName(HxVector_t *pstResultData, HINT32 nListNum);
static ONDK_Result_t	nx_genreSearch_UpdateResultData(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t 	nx_genreSearch_GetData(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_genreSearch_DrawBackground(void);
static ONDK_Result_t 	nx_genreSearch_SetUiObject(Nx_GenreSearchContext_t *pstContents);
static ONDK_Result_t	nx_genreSearch_MsgGwmCreate(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_genreSearch_MsgGwmDestroy(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_genreSearch_MsgGwmKeyDown(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_GenreSearchContext_t	*nx_genreSearch_GetContents(void)
{
	return	&s_stGenreSearchContent;
}

static ONDK_Result_t Nx_genreSearch_InitFirstMenuData(Nx_GenreSearchContext_t *pstContents)
{
	HUINT32 ulIndex = 0;
	Nx_GenreSearchMenuItem_t* pstFirstMenu;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	pstContents->ulFirstMenuNum = NX_GENRESEARCH_FIRSTMENU_NUM;
	pstContents->ulcurIndexFirstMenu = 0;

	pstContents->pstFirstMenu = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	HxLOG_Debug("number of first menu (%d)\n", pstContents->ulFirstMenuNum);
	if(pstContents->pstFirstMenu)
	{
		for(ulIndex; ulIndex < pstContents->ulFirstMenuNum; ++ulIndex)
		{
			HxLOG_Debug("first menu (%d)\n", ulIndex);
			pstFirstMenu = (Nx_GenreSearchMenuItem_t *)NX_APP_Malloc (sizeof(Nx_GenreSearchMenuItem_t));
			if (pstFirstMenu == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
			HxSTD_MemSet(pstFirstMenu, 0, sizeof(Nx_GenreSearchMenuItem_t));
			HxSTD_StrNCpy((HCHAR*)pstFirstMenu->ucName, (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel1String(s_NxGenreSearch_ArrayOfFirstMenu[ulIndex])), MAX_ELEMENT_TEXT_LENGTH);
			HLIB_VECTOR_Add(pstContents->pstFirstMenu, pstFirstMenu);		}
	}

	HxVECTOR_DELETE_IF_EMPTY(pstContents->pstFirstMenu);
	HxLOG_Debug("Init FirstMenu Finish!\n");
	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_GetSecondMenu(Nx_GenreSearchContext_t *pstContents)
{
	HUINT32 ulNibbleLevel2 = 0;
	HUINT32 ulcurIndexFirstMenu = 0;
    eNxUiUtilGenreNibbleLevel1Type_t nibbleLevel1 = eNxUtil_NibbleLevel1_Undefined;
	HUINT8* pstrNibbleLevel2 = NULL;
	Nx_GenreSearchMenuItem_t* pstSecondMenu;

	ulcurIndexFirstMenu = pstContents->ulcurIndexFirstMenu;
	nibbleLevel1 = s_NxGenreSearch_ArrayOfFirstMenu[ulcurIndexFirstMenu];

    for(ulNibbleLevel2;;++ulNibbleLevel2)
    {
		pstrNibbleLevel2 = NX_PRISM_UTIL_GetGenreNibbleLevel2String(nibbleLevel1,ulNibbleLevel2);
		if(pstrNibbleLevel2 == NULL)
		{
			break;
		}
		pstSecondMenu = (Nx_GenreSearchMenuItem_t *)NX_APP_Malloc (sizeof(Nx_GenreSearchMenuItem_t));
		if (pstSecondMenu == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

		HxSTD_MemSet(pstSecondMenu, 0, sizeof(Nx_GenreSearchMenuItem_t));
		HxSTD_StrNCpy((HCHAR*)pstSecondMenu->ucName, (HCHAR*)ONDK_GetString(pstrNibbleLevel2), MAX_ELEMENT_TEXT_LENGTH);
		HLIB_VECTOR_Add(pstContents->pstSecondMenu, pstSecondMenu);
	}

	return ONDK_RET_OK;

}

static ONDK_Result_t Nx_genreSearch_GetThirdMenu(Nx_GenreSearchContext_t *pstContents)
{
	HUINT32 ulIndex = 0;
	HUINT32 ulLen = 0;
	Nx_GenreSearchMenuItem_t* pstSecondMenu, *pstThirdMenu;

	ulLen =  pstContents->pstSecondMenu->count;
	HLIB_VECTOR_RemoveAll(pstContents->pstThirdMenu);

	HxLOG_Debug(" Length of Second Menu. (%d)\n", ulLen);

	for(ulIndex; ulIndex < ulLen;++ulIndex)
    {
		pstSecondMenu = HLIB_VECTOR_ItemAt(pstContents->pstSecondMenu, ulIndex);

		pstThirdMenu = (Nx_GenreSearchMenuItem_t *)NX_APP_Malloc (sizeof(Nx_GenreSearchMenuItem_t));
		if (pstThirdMenu == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

		HxSTD_MemSet(pstThirdMenu, 0, sizeof(Nx_GenreSearchMenuItem_t));
		HxSTD_StrNCpy((HCHAR*)pstThirdMenu->ucName, (HCHAR*)pstSecondMenu->ucName, MAX_ELEMENT_TEXT_LENGTH);

		HLIB_VECTOR_Add(pstContents->pstThirdMenu, pstThirdMenu);
	}
	return ONDK_RET_OK;
}


static void Nx_genreSearch_CreateFirstMenu(HxVector_t *pstFirstMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};

	// Create background
	stRect = ONDK_Rect(NX_GENRESEARCH_FIRSTMENU_MAIN_BG_X, NX_GENRESEARCH_FIRSTMENU_MAIN_BG_Y, NX_GENRESEARCH_FIRSTMENU_MAIN_BG_W, NX_GENRESEARCH_FIRSTMENU_MAIN_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_FIRSTMENU_MAIN_BG_ID, &stRect, COL(C_G_Bang_50));

	// Create first menu
	stRect = ONDK_Rect(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_X, NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_Y, NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_W, NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_H);
	ONDK_GWM_List_Create(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID, &stRect, "GENRE SEARCH FIRST MENU", NX_GENRESEARCH_FIRSTMENU_ITEM_H, nTotal, NX_GENRESEARCH_FIRSTMENU_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstFirstMenu);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);
}

static void Nx_genreSearch_CreateSecondMenu(HxVector_t *pstSecondMenu, HINT32 nTotal, GWM_ObjDraw_t objDrawMethod)
{
	ONDK_Rect_t     stRect = {0, 0, 0, 0};
	// Create background
	stRect = ONDK_Rect(NX_GENRESEARCH_SECONDMENU_MAIN_BG_X, NX_GENRESEARCH_SECONDMENU_MAIN_BG_Y, NX_GENRESEARCH_SECONDMENU_MAIN_BG_W, NX_GENRESEARCH_SECONDMENU_MAIN_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_SECONDMENU_MAIN_BG_ID, &stRect, COL(C_G_Bang_50));

	// Create second menu
	stRect = ONDK_Rect(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_X, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_Y, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_W, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_H);
	ONDK_GWM_List_Create(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, &stRect, "GENRE SEARCH FIRST MENU", NX_GENRESEARCH_SECONDMENU_ITEM_H, nTotal, NX_GENRESEARCH_SECONDMENU_OBJ_PAGE_COUNT, (GWM_LIST_ITEMS)pstSecondMenu);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, objDrawMethod);
	ONDK_GWM_List_SetCircularMove(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID);
}


static ONDK_Result_t    Nx_genreSearch_DrawMenu(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */
	HINT32          nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32          nCellHeight = 0, nCount = 0, nOffsetY = 0, lScroll_height = 0;
	LIST_OBJECT     objList = (LIST_OBJECT)object;
	HINT32          objID;
	HxVector_t      *pstDataList = NULL;
	Nx_GenreSearchMenuItem_t*pstData = NULL;

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000;ONDKFontHandle     *hFontHandle = NULL, *hFontDescription = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL, *hBoldFontDescription = NULL,*hNormalFontDescription = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	Nx_GenreSearchContext_t* pstContents = NULL;
	HBOOL bNeedScrollBar = FALSE;
	ONDK_Rect_t			stImgRect;

	NOT_USED_PARAM(arg);

	pstContents = nx_genreSearch_GetContents();

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

	if(objID == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID)
	{
		pstContents->objFirstMenu = (ONDK_GWM_GuiObject_t)objList;
	}
	else if (objID == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID)
	{
		pstContents->objSecondMenu = (ONDK_GWM_GuiObject_t)objList;
	}

	if (pstDataList == NULL || nTotal == 0)
	{
		NX_APP_Error("ERROR!!! (pstDataList == NULL) || (nTotal == 0) !!\n");
		return ONDK_RET_FAIL;
	}

	//set current index
	if(objID == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID)
	{
		if(pstContents->ucDisplayedDataInObjFirstMenu  == 1)
		{
			pstContents->ulcurIndexFirstMenu = nCurrIdx;
		}
		else if(pstContents->ucDisplayedDataInObjFirstMenu  == 3)
		{

			pstContents->ulcurIndexThirdMenu = nCurrIdx;
		}
	}
	else if (objID == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID)
	{
		if(pstContents->ucDisplayedDataInObjSecondMenu == 2)
		{
			pstContents->ulcurIndexSecondMenu= nCurrIdx;
		}
		else if(pstContents->ucDisplayedDataInObjSecondMenu == 4)
		{
			pstContents->ulcurIndexDetailedMenu = nCurrIdx;
		}
	}

	HxLOG_Debug("Current index menu1(%d) menu2(%d) menu3(%d) menu4(%d)\n", pstContents->ulcurIndexFirstMenu,pstContents->ulcurIndexSecondMenu,pstContents->ulcurIndexThirdMenu,pstContents->ulcurIndexDetailedMenu);

	nCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList));

	if(objID == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID && pstContents->ucDisplayedDataInObjSecondMenu == 4)
	{
		nCellHeight = NX_GENRESEARCH_DETAILEDMENU_ITEM_H;
	}

	/* index setting */
	HxLOG_Debug("total (%d) numOfPage (%d)\n",nTotal,nNumOfPage);
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

			if (objID == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID && ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu)
			{
				textFontColor = COL(C_T_Base01);
			}

			bgColor = COL(C_G_Base01_100);
			hFontHandle = hBoldFontHandle;
			hFontDescription = hBoldFontDescription;
		}
		else
		{
			textFontColor = COL(C_T_Whistle_100);

			if (objID == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID && ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu)
			{
				textFontColor = COL(C_T_Gong06);
			}
			hFontHandle = hNormalFontHandle;

			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4)
			{
				hFontHandle = hBoldFontHandle;

			}
			hFontDescription = hNormalFontDescription;
		}

		Nx_genreSearch_DrawCursor_A2(screen, pstData, &stListRegion, nOffsetY, hFontHandle, hFontDescription,textFontColor, bgColor,nCellHeight, ((nDataIdx == nCurrIdx)), bUiTop, objID);
		nOffsetY += nCellHeight;
		bgColor = 0x00000000;
	}


	/* Scroll just for detailed menu */
	if (bNeedScrollBar == TRUE && objID == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID && pstContents->ucDisplayedDataInObjSecondMenu == 4)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (nTotal / nNumOfPage);
		lAllPage += (nTotal % nNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_GENRESEARCH_SCROLL_BAR_H)
		{
			lScrollBall_height = NX_GENRESEARCH_SCROLL_BAR_H;
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
		ONDK_DRAW_FillRectElement(screen, NX_GENRESEARCH_SCROLL_BAR_X, NX_GENRESEARCH_SCROLL_BAR_Y, NX_GENRESEARCH_SCROLL_BAR_W, lScroll_height, COL(C_G_Whistle_20));

		/* Draw Scroll Bar Ball */
		stImgRect = ONDK_Rect(NX_GENRESEARCH_SCROLL_BAR_X, NX_GENRESEARCH_SCROLL_BAR_Y + lScroll_y_offset, NX_GENRESEARCH_SCROLL_BAR_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	ONDK_FONT_Release(hNormalFontDescription);
	ONDK_FONT_Release(hBoldFontDescription);
	return  ONDK_RET_OK;
}

static ONDK_Result_t    Nx_genreSearch_DrawCursor_A2(ONDKSurfaceHandle *screen, Nx_GenreSearchMenuItem_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDKFontHandle *hFontDescription, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop, HUINT32 ulObjId)
{
	HCHAR           pText[DxNAME_LEN] = {0, };
	HCHAR			pDescription[DxNAME_LEN] = {0,};
	Nx_GenreSearchContext_t	*pstContents = NULL;

	pstContents = nx_genreSearch_GetContents();

	HxSTD_sprintf(pText, "%s", pstData->ucName);

	if (bIsFocus == TRUE)
	{
		if( ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject()) == ulObjId && ulObjId == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID)
		{
			ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w - 13, ulCellHeight, bgColor);
		}
		else if(ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject()) == ulObjId && ulObjId == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID)
		{
			ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, ulCellHeight, bgColor);
		}
	}

	if(ulObjId == NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pText, stListRegion->x + NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_Y, NX_GENRESEARCH_FIRSTMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
	}
	else if (ulObjId == NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID)
	{
		if(pstContents->ucDisplayedDataInObjSecondMenu == 2)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pText, stListRegion->x + NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_X, stListRegion->y + nOffsetY + NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_Y, NX_GENRESEARCH_SECONDMENU_ITEM_TEXT_W, textFontColor, ONDKSTF_LEFT);
		}
		else if(pstContents->ucDisplayedDataInObjSecondMenu == 4)
		{
			ONDK_Color_t textColorDescription = COL(C_T_Bang_100);
			if(textFontColor == COL(C_T_Whistle_100))
			{
				textColorDescription = COL(C_T_Gong03);
			}

			HxSTD_sprintf(pDescription, "%s", pstData->ucEventDescription);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pText, stListRegion->x + NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_X, stListRegion->y + nOffsetY + NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_Y, NX_GENRESEARCH_DETAILEDMENU_ITEM1_TEXT_W, textFontColor, ONDKSTF_LEFT);
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontDescription, (HCHAR*)pDescription, stListRegion->x + NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_X, stListRegion->y + nOffsetY + NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_Y, NX_GENRESEARCH_DETAILEDMENU_ITEM2_TEXT_W, textColorDescription, ONDKSTF_LEFT);
		}
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_DrawPath(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32 ulPostionX = NX_GENRESEARCH_PATH_X;
	HUINT32 ulPostionY = NX_GENRESEARCH_PATH_Y;
	//ulPostionX += (HxSTD_StrLen(pstContents->ucTitle) * NX_PRISM_FONT_SIZE_46);
	ulPostionX += ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemBold, NX_PRISM_FONT_SIZE_46, (HCHAR *) pstContents->ucTitle);
	ulPostionY += 24; // 46 - 22

	stRect = ONDK_Rect(ulPostionX, ulPostionY, NX_GENRESEARCH_PATH_W, NX_GENRESEARCH_PATH_H);
	ONDK_GWM_COM_Text_Create(NX_GENRESEARCH_PATH_ID, &stRect, (HCHAR*)pstContents->ucPath);
	ONDK_GWM_COM_Text_SetFont(NX_GENRESEARCH_PATH_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_GENRESEARCH_PATH_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_GENRESEARCH_PATH_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GENRESEARCH_PATH_ID, COL(C_T_Gong01), COL(C_T_Gong01));

	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_DrawNumberOfItem(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_GENRESEARCH_NUMBER_OF_ITEM_X, NX_GENRESEARCH_NUMBER_OF_ITEM_Y, NX_GENRESEARCH_NUMBER_OF_ITEM_W, NX_GENRESEARCH_NUMBER_OF_ITEM_H);
	ONDK_GWM_COM_Text_Create(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, &stRect, (HCHAR*)pstContents->ucNumberOfItem);
	ONDK_GWM_COM_Text_SetFont(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, COL(C_T_Gong07), COL(C_T_Gong07));

	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_UpdateNumberOfItem(Nx_GenreSearchContext_t *pstContents, HBOOL isKeyDown,HBOOL isEmpty)
{
	HUINT32 curIndex = 0;
	if(isEmpty)
	{
		HxSTD_StrNCpy((HCHAR*)pstContents->ucNumberOfItem, (HCHAR*)"", MAX_ELEMENT_TEXT_LENGTH);
		ONDK_GWM_COM_Text_SetText(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, (HCHAR *)pstContents->ucNumberOfItem);
		return ONDK_RET_OK;
	}

	if(pstContents->pstDetailedMenu->count <= 0)
	{
		HxSTD_StrNCpy((HCHAR*)pstContents->ucNumberOfItem, (HCHAR*)"0/0", MAX_ELEMENT_TEXT_LENGTH);
		ONDK_GWM_COM_Text_SetText(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, (HCHAR *)pstContents->ucNumberOfItem);
		return ONDK_RET_OK;
	}

	if(pstContents->ucDisplayedDataInObjSecondMenu == 4)
	{
		if(pstContents->pstDetailedMenu->count > 0 && isKeyDown)
		{
			if(pstContents->ulcurIndexDetailedMenu == pstContents->pstDetailedMenu->count - 1)
			{
				curIndex = 1;
			}
			else
			{
				curIndex = (pstContents->ulcurIndexDetailedMenu + 2)%(pstContents->pstDetailedMenu->count + 2);
				if(curIndex > pstContents->pstDetailedMenu->count)
				{
					curIndex = pstContents->pstDetailedMenu->count;
				}
			}
		}
		else if(pstContents->pstDetailedMenu->count > 0 && !isKeyDown)
		{
			if(pstContents->ulcurIndexDetailedMenu == 0)
			{
				curIndex = pstContents->pstDetailedMenu->count;
			}
			else
			{
				curIndex = pstContents->ulcurIndexDetailedMenu;
			}
		}

		HxSTD_sprintf(pstContents->ucNumberOfItem,"%d/%d", curIndex,pstContents->pstDetailedMenu->count);
		ONDK_GWM_COM_Text_SetText(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, (HCHAR *)pstContents->ucNumberOfItem);
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_UpdateNumberOfItemForPaging(Nx_GenreSearchContext_t *pstContents)
{
	HUINT32 curIndex = 0;

	HxLOG_Debug("draw num of item (%d) \n",pstContents->ulcurIndexDetailedMenu);
	curIndex = pstContents->ulcurIndexDetailedMenu + 1;
	HxSTD_sprintf(pstContents->ucNumberOfItem,"%d/%d", curIndex,pstContents->pstDetailedMenu->count);
	ONDK_GWM_COM_Text_SetText(NX_GENRESEARCH_NUMBER_OF_ITEM_ID, (HCHAR *)pstContents->ucNumberOfItem);

	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_UpdatePath(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_GWM_COM_Text_SetText(NX_GENRESEARCH_PATH_ID, (HCHAR*)pstContents->ucPath);
	return ONDK_RET_OK;
}

static void		nx_genreSearch_SetUiCreateOptionMenu(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_Rect_t		stRect;
	stRect = ONDK_Rect(NX_GENRESEARCH_OPTION_FRAME_X, NX_GENRESEARCH_OPTION_FRAME_Y,
						NX_GENRESEARCH_OPTION_FRAME_W, NX_GENRESEARCH_OPTION_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_OPTION_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GENRESEARCH_OPTION_FRAME_X, NX_PRISM_COMP_DrawFrameBottomDim);

	stRect = ONDK_Rect(NX_GENRESEARCH_OPTION_TEXT_X, NX_GENRESEARCH_OPTION_TEXT_Y,
						NX_GENRESEARCH_OPTION_TEXT_W, NX_GENRESEARCH_OPTION_TEXT_H);

	ONDK_GWM_COM_Multi_Create(NX_GENRESEARCH_OPTION_TEXT_ID, &stRect);

	// Intergrated String by UX
 	#if 0//defined (CONFIG_OP_MBC)
		ONDK_GWM_COM_Multi_SetContents(NX_GENRESEARCH_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HCHAR*)ONDK_GetString( RES_SORT_ORDER_ID), NULL, NX_GENRESEARCH_OPTION_IMG_MG);
	#else
		ONDK_GWM_COM_Multi_SetContents(NX_GENRESEARCH_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HCHAR*)ONDK_GetString(RES_SORT_CHANNELS_01_ID), NULL, NX_GENRESEARCH_OPTION_IMG_MG);
	#endif
	ONDK_GWM_COM_Multi_SetFontHandle(NX_GENRESEARCH_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_GENRESEARCH_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_GENRESEARCH_OPTION_TEXT_ID, FALSE);
}

static ONDK_Result_t	nx_genreSearch_OptionInitData(Nx_GenreSearchContext_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	pstContents->szItemTextList = NULL;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortTime);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortNumber);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortName);
			break;
	}

	// TODO: Add Project config if you want to do

	return	ONDK_RET_OK;
}


static void		nx_genreSearch_CreateListPopup(Nx_GenreSearchContext_t *pstContents)
{
	HINT32		lLineCnt = 0, i = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, ulCurSort = 0;
	HUINT8		**pptextList = NULL;
	ONDK_Rect_t	stRect, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulIndex	= 0;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulCurSort = pstContents->eChangedSort;
	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == ulCurSort)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5651_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);

	// Set Text List
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulTextListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulTextListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stGenreSearchOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stGenreSearchOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stGenreSearchOptContents, ONDK_GetString(RES_MESG_5651_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stGenreSearchOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_genreSearch_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_genreSearch_DisplayListPopup, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_genreSearch_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stGenreSearchOptContents, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_genreSearch_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_genreSearch_DisplayListPopup);
}

static ONDK_Result_t	nx_genreSearch_ChangedOptionItemValue(Nx_GenreSearchContext_t *pstContents, HUINT32 ulCurIdx)
{
	if(pstContents->eChangedSort != pstContents->stOptList.aeOptItemList[ulCurIdx])
	{
		pstContents->eChangedSort = pstContents->stOptList.aeOptItemList[ulCurIdx];

		nx_genreSearch_UpdateResultData(pstContents);
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t Nx_genreSearch_DrawSearchProcessing(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_X, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_Y, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_W, NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_H);
	ONDK_GWM_COM_Text_Create(NX_GENRESEARCH_PROCESSING_ID, &stRect, (HCHAR*)ONDK_GetString(RES_SEARCHING_FOR_PROGRAMMES_ID));
	ONDK_GWM_COM_Text_SetFont(NX_GENRESEARCH_PROCESSING_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_GENRESEARCH_PROCESSING_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_GENRESEARCH_PROCESSING_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_GENRESEARCH_PROCESSING_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_genreSearch_Search(Nx_GenreSearchContext_t *pstContents, HUINT8 majorNibble, HUINT8 minorNibble)
{
	HERROR		hError = ERR_FAIL;
	HINT32 i = 0, cur_index = 0;
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	startQuery;
	HUINT32 	unixtime = 0;
	HCHAR 		value[NX_TEXT_SIZE_64] = {'\0',};

	pstContents->isSearch = TRUE;

	pstContents->majorNibble = majorNibble;
	pstContents->minorNibble = minorNibble;

	hError = NX_SYSTEM_GetTime(&unixtime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("time is Invalid!!!\n");
	}

	NX_APP_Error("\n\n into Search majorNibble(%d) minorNibble(%d) time(%d)\n\n", majorNibble, minorNibble, unixtime);
	//unixtime += SECONDS_PER_HOUR * 24 * 14; // 2 weeks
	HxSTD_sprintf(value, "%u", unixtime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

	//andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	//NX_PORT_META_QUERY_DeleteQuery(endQuery);
	//NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}
	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, startQuery);
	//NX_PORT_META_QUERY_DeleteQuery(andQuery);

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
	return ONDK_RET_OK;
}

static HINT32 nx_genreSearch_SortByTime (const void *pvArg1, const void *pvArg2)
{
	const Nx_GenreResultData_t *pstProgram1 = *(const Nx_GenreResultData_t **)pvArg1;
	const Nx_GenreResultData_t *pstProgram2 = *(const Nx_GenreResultData_t **)pvArg2;

	return (pstProgram1->startTime - pstProgram2->startTime);
}

static HINT32 nx_genreSearch_SortByLCN (const void *pvArg1, const void *pvArg2)
{
	const Nx_GenreResultData_t *pstProgram1 = *(const Nx_GenreResultData_t **)pvArg1;
	const Nx_GenreResultData_t *pstProgram2 = *(const Nx_GenreResultData_t **)pvArg2;

	return (pstProgram1->lcn - pstProgram2->lcn);
}

static HINT32 nx_genreSearch_SortByName(void *pvArg1, void *pvArg2)
{
	Nx_GenreResultData_t *pstProgram1 = (Nx_GenreResultData_t *)pvArg1;
	Nx_GenreResultData_t *pstProgram2 = (Nx_GenreResultData_t *)pvArg2;

	if(NULL == pstProgram1) { return 1;}
	if(NULL == pstProgram2) { return -1;}

	return HxSTD_StrCmp(pstProgram2->eventName, pstProgram1->eventName);
}

HxList_t *	nx_genreSearch_SortNxChannelsByName(HxVector_t *pstResultData, HINT32 nListNum)
{
	HINT32	i = 0;
	HxList_t		*pRetSortList = NULL;

	if (pstResultData == NULL || nListNum <= 0)
	{
		HxLOG_Debug("\nSort data is NULL or Length 0\n");
		return NULL;
	}
	for (i = 0; i < nListNum; i++)
	{
		Nx_GenreResultData_t	*pProgram = HLIB_VECTOR_ItemAt(pstResultData, i);
		pRetSortList = HLIB_LIST_AppendSortEx(pRetSortList, pProgram, nx_genreSearch_SortByName);
	}

	return pRetSortList;
}


static ONDK_Result_t	nx_genreSearch_UpdateResultData(Nx_GenreSearchContext_t *pstContents)
{
	HINT32 i = 0, length = 0;
	Nx_GenreSearchMenuItem_t* pstData;
	Nx_GenreResultData_t* pstResultData;
	HxList_t		*pSortList = NULL;

	length = HLIB_VECTOR_Length(pstContents->pstResultList);

	//set empty data for detailed menu
	HLIB_VECTOR_RemoveAll(pstContents->pstDetailedMenu);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstDetailedMenu), (GWM_LIST_ITEMS)pstContents->pstDetailedMenu);

	//add Sort
	switch(pstContents->eChangedSort)
	{
		case eNxUtil_OptItem_SortTime:
			(void) HLIB_VECTOR_Sort(pstContents->pstResultList, nx_genreSearch_SortByTime);
			break;

		case eNxUtil_OptItem_SortNumber:
			(void) HLIB_VECTOR_Sort(pstContents->pstResultList, nx_genreSearch_SortByLCN);
			break;

		case eNxUtil_OptItem_SortName:
			{
				HxList_t	*list = NULL;
				pSortList = nx_genreSearch_SortNxChannelsByName(pstContents->pstResultList,length);
				HLIB_VECTOR_RemoveAll(pstContents->pstResultList);
				if(pSortList != NULL)
				{
					list = pSortList;
					while(list)
					{
						Nx_GenreResultData_t *item = (Nx_GenreResultData_t*)list->data;
						pstResultData = (Nx_GenreResultData_t *)NX_APP_Calloc(sizeof(Nx_GenreResultData_t));
						if (pstResultData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

						pstResultData->eventId = item->eventId;
						HxSTD_StrNCpy((HCHAR*)pstResultData->eventName, (HCHAR*)item->eventName, DxNAME_LEN);
						pstResultData->lcn = item->lcn;
						pstResultData->startTime = item->startTime;
						pstResultData->svcUid = item->svcUid;
						HxSTD_StrNCpy((HCHAR*)pstResultData->szProgrammeName, (HCHAR*)item->szProgrammeName, MAX_ELEMENT_TEXT_LENGTH);
						HxSTD_StrNCpy((HCHAR*)pstResultData->szEventDescription, (HCHAR*)item->szEventDescription, MAX_ELEMENT_TEXT_LENGTH);
						HLIB_VECTOR_Add(pstContents->pstResultList, pstResultData);
						list = list->next;
					}
					HLIB_LIST_RemoveAll(pSortList);
				}
			}
			break;

		default:
			break;
	}

	for (i = 0; i < length; i++)
	{
		Nx_GenreResultData_t *item = HLIB_VECTOR_ItemAt(pstContents->pstResultList, i);

		pstData = (Nx_GenreSearchMenuItem_t *)NX_APP_Calloc(sizeof(Nx_GenreSearchMenuItem_t));
		if (pstData == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }

		HxSTD_StrNCpy((HCHAR*)pstData->ucName, (HCHAR*)item->szProgrammeName, MAX_ELEMENT_TEXT_LENGTH);
		HxSTD_StrNCpy((HCHAR*)pstData->ucEventDescription, (HCHAR*)item->szEventDescription, MAX_ELEMENT_TEXT_LENGTH);

		HLIB_VECTOR_Add(pstContents->pstDetailedMenu, pstData);

		if (item)
		{
			HxLOG_Debug("result update (%d) : svcuid (%d), eventID (%d) , name (%s) eventName(%s)\n",
							i, item->svcUid, item->eventId, item->szProgrammeName, item->eventName);
		}
	}

	//hide the processing message
	ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_PROCESSING_ID, TRUE);

	ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstDetailedMenu), (GWM_LIST_ITEMS)pstContents->pstDetailedMenu);

	if(pstContents->pstDetailedMenu->count > 0)
	{
		ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID);
	}
	else
	{
		ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);
	}

	//Draw - Number Of Item
	pstContents->ulcurIndexDetailedMenu = 1;
	Nx_genreSearch_UpdateNumberOfItem(pstContents, FALSE,FALSE);

	return ONDK_RET_OK;

}

static ONDK_Result_t 	nx_genreSearch_GetData(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 i = 0, j = 0, cur_index = 0;
	NX_Channel_t *channel 	= NULL;
	HINT32 num = 0;
	HxVector_t *result = NULL;		//= HLIB_VECTOR_New(NULL,NULL);
	DxService_t *tempSvc = NULL;
	HUINT8  searchMajorNibble, searchMinorNibble;
	HUINT8	ucNibblelevel1 , ucNibblelevel2;

	if(pstContents->isSearch == FALSE)
		return ONDK_RET_OK;

	pstContents->isSearch = FALSE;

	searchMajorNibble = pstContents->majorNibble;//		= eNxUtil_NibbleLevel1_MovieDrama;
	searchMinorNibble = pstContents->minorNibble;//		= eNxUtil_NibbleLevel2_0x0;

	HxLOG_Debug("\n search Nibble1 : %d , search Nibble2 : %d \n\n", searchMajorNibble, searchMinorNibble);
	pstContents->pstResultList = HLIB_VECTOR_New(NULL,NULL);

	for (i = 0; i < pstContents->nChannelNum; i++)
	{
		if (cur_index >= pstContents->nChannelNum)
		{
			cur_index = 0;
		}

		channel = &pstContents->pstChannels[cur_index];

		tempSvc = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);

		result = HLIB_VECTOR_New(NULL,NULL);

		NX_METADATA_EPG_GetResultProgrammeBySvcUid(channel->svcUid, result, &num);

		if (num > 0 && (result != NULL))
		{
			for (j = 0; j < num; j++)
			{
				NX_Programme_t *item = HLIB_VECTOR_ItemAt(result, j);


				ucNibblelevel1 = NX_METADATA_PROGRAMMES_GetGenreNibbleLevel1(item);
				ucNibblelevel2 = NX_METADATA_PROGRAMMES_GetGenreNibbleLevel2(item);

				HxLOG_Debug("j(%d) nibble1(0x%x), nibble2(0x%x), searchMajor(0x%x), minor(0x%x)\n",
						 j, ucNibblelevel1, ucNibblelevel2, searchMajorNibble, searchMinorNibble);

				if (searchMajorNibble == ucNibblelevel1 && searchMinorNibble == ucNibblelevel2)
				{
					HCHAR programmeName[EPG_HUMAX_NAME_LEN] = {'\0',};
					HUINT32 ulSeason = 0;
					HUINT32 ulEpisode = 0;
					HUINT32 ulStartTime = 0;
					HUINT32 ulDuration = 0;
					HBOOL isHD = FALSE;
					HUINT32 ulWeekIndex = 0;
					HxDATETIME_t 		dtStartTime;
					HxDATETIME_t 		dtEndTime;

					Nx_GenreResultData_t *data = (Nx_GenreResultData_t*)NX_APP_Calloc(sizeof(Nx_GenreResultData_t));

					data->svcUid 	= NX_METADATA_PROGRAMMES_GetSvcUID(item);
					data->eventId	= NX_METADATA_PROGRAMMES_GetEventID(item);
					data->startTime =  NX_METADATA_PROGRAMMES_GetStartTime(item);
					data->lcn		= tempSvc->lcn;
					HxSTD_MemSet(data->eventName, 0, DxNAME_LEN);
					HxSTD_sprintf(data->eventName,"%s",tempSvc->name);

					ulSeason = NX_METADATA_PROGRAMMES_GetSeason(item);
					ulEpisode = NX_METADATA_PROGRAMMES_GetEpisode(item);
					ulStartTime = NX_METADATA_PROGRAMMES_GetStartTime(item);
					ulDuration = NX_METADATA_PROGRAMMES_GetDuration(item);
					isHD = NX_METADATA_PROGRAMMES_IsHD(item);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &dtStartTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime((ulStartTime + ulDuration), &dtEndTime);
					ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(dtStartTime.stDate);

					HxSTD_MemSet(data->szEventDescription, 0, MAX_ELEMENT_TEXT_LENGTH);
					HxSTD_sprintf(data->szEventDescription,"%04d %s %s / %02d:%02d-%02d:%02d %s %02d %s", tempSvc->lcn, tempSvc->name, isHD == TRUE ? "HD" : "",
									dtStartTime.stTime.ucHour, dtStartTime.stTime.ucMinute, dtEndTime.stTime.ucHour, dtEndTime.stTime.ucMinute,
									ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)), dtStartTime.stDate.ucDay,
									ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(dtStartTime.stDate.ucMonth)));

					if (NX_METADATA_PROGRAMMES_GetName(item, programmeName) == ERR_OK)
					{
						HxSTD_MemSet(data->szProgrammeName, 0, EPG_HUMAX_NAME_LEN);
						if(ulSeason == 0 || ulEpisode == 0)
						{
							HxSTD_sprintf(data->szProgrammeName,"%s", programmeName);
						}
						else
						{
							HxSTD_sprintf(data->szProgrammeName,"%s Season%d Episode %d", programmeName, ulSeason, ulEpisode);
						}
					}
					HLIB_VECTOR_Add(pstContents->pstResultList, data);
				}
			}
		}
		HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.

		cur_index++;
	}

	nx_genreSearch_UpdateResultData(pstContents);

	return	ONDK_RET_OK;
}


static void		nx_genreSearch_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_GENRESEARCH_BACKGROUND_BOX_X, NX_GENRESEARCH_BACKGROUND_BOX_Y, NX_GENRESEARCH_BACKGROUND_BOX_WIDTH, NX_GENRESEARCH_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_90));
}


static ONDK_Result_t 	nx_genreSearch_SetUiObject(Nx_GenreSearchContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	ONDK_GWM_APP_SetAppArea(NX_PRISM_GUIDE_GENRE_WINDOW_X, NX_PRISM_GUIDE_GENRE_WINDOW_Y,
								NX_PRISM_GUIDE_GENRE_WINDOW_W, NX_PRISM_GUIDE_GENRE_WINDOW_H);

	/* Draw Background */
	nx_genreSearch_DrawBackground();

	/* Draw Title Line */
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	//Draw - Path
	Nx_genreSearch_DrawPath(pstContents);

	//Draw - Number Of Item
	Nx_genreSearch_DrawNumberOfItem(pstContents);

	//Draw - Option
	nx_genreSearch_SetUiCreateOptionMenu(pstContents);
	ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_OPTION_TEXT_ID, TRUE);

	//Draw - Search Processing
	Nx_genreSearch_DrawSearchProcessing(pstContents);
	ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_PROCESSING_ID, TRUE);

	/* Draw Devision Line */
	stRect = ONDK_Rect(NX_GENRESEARCH_DIVISTION_LINE_X, NX_GENRESEARCH_DIVISTION_LINE_Y,
						NX_GENRESEARCH_DIVISTION_LINE_W, NX_GENRESEARCH_DIVISTION_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_DIVISTION_LINE_ID, &stRect, COL(C_G_Whistle_20));

	/* Draw Bottom Dime */
	stRect = ONDK_Rect(NX_GENRESEARCH_BOTTOM_DIM_X, NX_GENRESEARCH_BOTTOM_DIM_Y,
						NX_GENRESEARCH_BOTTOM_DIM_W, NX_GENRESEARCH_BOTTOM_DIM_H);
	ONDK_GWM_COM_Rect_Create(NX_GENRESEARCH_BOTTOM_DIM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_GENRESEARCH_BOTTOM_DIM_ID, NX_PRISM_COMP_DrawFrameBottomDim);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_genreSearch_MsgGwmCreate(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;
	HUINT32		StandardStartTime = p1;
	HUINT32		nTotal			  = 0;


	pstContents->unStandardSearchTime = StandardStartTime;
	pstContents->nChannelNum = (HINT32)p2;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();

	// Make Title
	HxSTD_StrNCpy((HCHAR*)pstContents->ucTitle, (HCHAR*)ONDK_GetString(RES_GENRE_07_ID), MAX_ELEMENT_TEXT_LENGTH);

	//Make Path
	HxSTD_StrNCpy((HCHAR*)pstContents->ucPath, (HCHAR*)"", MAX_ELEMENT_TEXT_LENGTH);

	//Make Number Of Item
	HxSTD_StrNCpy((HCHAR*)pstContents->ucNumberOfItem, (HCHAR*)"", MAX_ELEMENT_TEXT_LENGTH);

	//Set type of data displayed in ObjFirstMenu
	pstContents->ucDisplayedDataInObjFirstMenu = 1;
	pstContents->ucDisplayedDataInObjSecondMenu = 2;


	Nx_genreSearch_InitFirstMenuData(pstContents);

	//init Option Item
	nx_genreSearch_OptionInitData(pstContents);
	pstContents->eChangedSort = eNxUtil_OptItem_SortTime;

	nx_genreSearch_SetUiObject(pstContents);


	//Create first menu
	nTotal = HLIB_VECTOR_Length(pstContents->pstFirstMenu);
	Nx_genreSearch_CreateFirstMenu(pstContents->pstFirstMenu, nTotal, (GWM_ObjDraw_t)Nx_genreSearch_DrawMenu);

	//initiate second menu
	pstContents->pstSecondMenu = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
	pstContents->ulcurIndexSecondMenu = 0;

	//create second menu
	nTotal = HLIB_VECTOR_Length(pstContents->pstSecondMenu);
	Nx_genreSearch_CreateSecondMenu(pstContents->pstSecondMenu,nTotal, (GWM_ObjDraw_t)Nx_genreSearch_DrawMenu);

	//create third menu
	pstContents->pstThirdMenu= HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
	pstContents->ulcurIndexThirdMenu = 0;

	//created detailed menu
	pstContents->pstDetailedMenu = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
	pstContents->ulcurIndexDetailedMenu = 0;

	pstContents->isSearch = FALSE;

	ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Guide);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Guide !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_genreSearch_MsgGwmDestroy(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstResultList)
		HLIB_VECTOR_Delete(pstContents->pstResultList);

	if(pstContents->pstFirstMenu)
	{
		HLIB_VECTOR_Delete(pstContents->pstFirstMenu);
	}

	if(pstContents->pstSecondMenu)
	{
		HLIB_VECTOR_Delete(pstContents->pstSecondMenu);
	}

	if(pstContents->pstThirdMenu)
	{
		HLIB_VECTOR_Delete(pstContents->pstThirdMenu);
	}

	if(pstContents->pstDetailedMenu)
	{
		HLIB_VECTOR_Delete(pstContents->pstDetailedMenu);
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	HxSTD_MemSet(pstContents, 0x00, sizeof(Nx_GenreSearchContext_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_genreSearch_MsgGwmKeyDown(Nx_GenreSearchContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{

		case KEY_BACK: // processing for KEY_BACK is the same with KEY_LEFT
		case KEY_ARROWLEFT:
			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 2)
			{
				HLIB_VECTOR_RemoveAll(pstContents->pstSecondMenu);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstSecondMenu), (GWM_LIST_ITEMS)pstContents->pstSecondMenu);
				ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);
				//Set Empty Path
				HxSTD_StrNCpy((HCHAR*)pstContents->ucPath, (HCHAR*)"", MAX_ELEMENT_TEXT_LENGTH);
				Nx_genreSearch_UpdatePath(pstContents);
			}
			else if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4)
			{
				//search the programms
				//reset empty data for second menu
				HLIB_VECTOR_RemoveAll(pstContents->pstDetailedMenu);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstDetailedMenu), (GWM_LIST_ITEMS)pstContents->pstDetailedMenu);

				ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);

			}
			else if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objFirstMenu && pstContents->ucDisplayedDataInObjFirstMenu== 3)
			{
				//set type of data displayed in objFirstMeu is First Menu
				pstContents->ucDisplayedDataInObjFirstMenu = 1;
				pstContents->ucDisplayedDataInObjSecondMenu = 2;

				//Hide Sort Channels
				ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_OPTION_TEXT_ID, TRUE);

				//remove all data of Third Menu
				HLIB_VECTOR_RemoveAll(pstContents->pstThirdMenu);
				//set empty data for first menu
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objFirstMenu, HLIB_VECTOR_Length(pstContents->pstThirdMenu), (GWM_LIST_ITEMS)pstContents->pstThirdMenu);

				//reset data for first menu
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objFirstMenu, HLIB_VECTOR_Length(pstContents->pstFirstMenu), (GWM_LIST_ITEMS)pstContents->pstFirstMenu);
				ONDK_GWM_List_SetCurrentIndex(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID, pstContents->ulcurIndexFirstMenu);
				//HxLOG_Print("\n\n[%s:%d] menu1 curIndex (%d)\n\n",__FUNCTION__,__LINE__,pstContents->ulcurIndexFirstMenu);

				//set empty data for second menu
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstThirdMenu), (GWM_LIST_ITEMS)pstContents->pstThirdMenu);
				ONDK_GWM_List_ChangeNumOfPage((LIST_OBJECT)pstContents->objSecondMenu, NX_GENRESEARCH_SECONDMENU_OBJ_PAGE_COUNT);

				//reset data for second menu
				HLIB_VECTOR_RemoveAll(pstContents->pstSecondMenu);
				//pstContents->pstSecondMenu = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

				Nx_genreSearch_GetSecondMenu(pstContents);
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstSecondMenu), (GWM_LIST_ITEMS)pstContents->pstSecondMenu);
				ONDK_GWM_List_SetCurrentIndex(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, pstContents->ulcurIndexSecondMenu);
				//HxLOG_Print("\n\n[%s:%d] menu2 curIndex (%d)\n\n",__FUNCTION__,__LINE__,pstContents->ulcurIndexSecondMenu);
				ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID);
				//Set Path
				HxSTD_sprintf(pstContents->ucPath,"%s%s","/",(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel1String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu])));
				Nx_genreSearch_UpdatePath(pstContents);

				//Draw - Number Of Item
				Nx_genreSearch_UpdateNumberOfItem(pstContents, TRUE, TRUE);

			}
			else if (ONDK_GWM_Obj_GetFocusedObject() == pstContents->objFirstMenu && pstContents->ucDisplayedDataInObjFirstMenu== 1)
			{

				ONDK_GWM_APP_Destroy(0);
			}
			return ONDK_MESSAGE_BREAK;

		/* keys to exit */
		case KEY_EXIT:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_ARROWRIGHT://the same process with key_ok
		case KEY_OK:
			{
				if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objFirstMenu)
				{

					if(pstContents->ucDisplayedDataInObjFirstMenu == 1)
					{
						HxLOG_Debug("Key down curIndex (%d)", pstContents->ulcurIndexFirstMenu);
						HLIB_VECTOR_RemoveAll(pstContents->pstSecondMenu);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstSecondMenu), (GWM_LIST_ITEMS)pstContents->pstSecondMenu);

						Nx_genreSearch_GetSecondMenu(pstContents);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstSecondMenu), (GWM_LIST_ITEMS)pstContents->pstSecondMenu);

						ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID);

						//Set Path
						HxSTD_sprintf(pstContents->ucPath,"%s%s","/",(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel1String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu])));
						Nx_genreSearch_UpdatePath(pstContents);
					}
					else if(pstContents->ucDisplayedDataInObjFirstMenu == 3)
					{
						//Set Path
						HxSTD_sprintf(pstContents->ucPath,"%s%s%s%s","/",(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel1String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu])),
																		"/",(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel2String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu],pstContents->ulcurIndexThirdMenu)));
						Nx_genreSearch_UpdatePath(pstContents);

						//reset empty data for second menu
						HLIB_VECTOR_RemoveAll(pstContents->pstDetailedMenu);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstDetailedMenu), (GWM_LIST_ITEMS)pstContents->pstDetailedMenu);

						//show the processing message
						ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_PROCESSING_ID, FALSE);

						//search programme
						nx_genreSearch_Search(pstContents, s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu], pstContents->ulcurIndexThirdMenu);
					}
				}
				else if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu)
				{
					if(pstContents->ucDisplayedDataInObjSecondMenu == 2)
					{

						//set type of data displayed in objFirstMeu is ThirdMenu
						pstContents->ucDisplayedDataInObjFirstMenu = 3;
						pstContents->ucDisplayedDataInObjSecondMenu = 4;

						//show Sort Channels
						ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_OPTION_TEXT_ID, FALSE);

						//Set empty data for first menu
						HLIB_VECTOR_RemoveAll(pstContents->pstThirdMenu);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objFirstMenu, HLIB_VECTOR_Length(pstContents->pstThirdMenu), (GWM_LIST_ITEMS)pstContents->pstThirdMenu);

						//Copy the data from second menu into third menu
						Nx_genreSearch_GetThirdMenu(pstContents);

						//reset data for first menu
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objFirstMenu, HLIB_VECTOR_Length(pstContents->pstThirdMenu), (GWM_LIST_ITEMS)pstContents->pstThirdMenu);
						ONDK_GWM_List_SetCurrentIndex(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID, pstContents->ulcurIndexSecondMenu);

						//Set empty data for second menu
						HLIB_VECTOR_RemoveAll(pstContents->pstDetailedMenu);
						ONDK_GWM_List_ChangeItems((LIST_OBJECT)pstContents->objSecondMenu, HLIB_VECTOR_Length(pstContents->pstDetailedMenu), (GWM_LIST_ITEMS)pstContents->pstDetailedMenu);
						ONDK_GWM_List_ChangeNumOfPage((LIST_OBJECT)pstContents->objSecondMenu, NX_GENRESEARCH_DETAILEDMENU_OBJ_PAGE_COUNT);

						ONDK_GWM_Obj_SetFocus(NX_GENRESEARCH_FIRSTMENU_MAIN_OBJ_ID);

							//Set Path
						HxSTD_sprintf(pstContents->ucPath,"/%s/%s",(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel1String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu])),
																		(HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetGenreNibbleLevel2String(s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu],pstContents->ulcurIndexSecondMenu)));
						Nx_genreSearch_UpdatePath(pstContents);

						//show the processing message
						ONDK_GWM_Obj_SetHide(NX_GENRESEARCH_PROCESSING_ID, FALSE);

						//search programme
						nx_genreSearch_Search(pstContents, s_NxGenreSearch_ArrayOfFirstMenu[pstContents->ulcurIndexFirstMenu], pstContents->ulcurIndexSecondMenu);

					}
					else if(pstContents->ucDisplayedDataInObjSecondMenu == 4)
					{
						if(pstContents->pstDetailedMenu->count > 0)
						{
							if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)Nx_PRISM_ProgramDetail_Proc) ==NULL )
							{
								Nx_GenreResultData_t *data = HLIB_VECTOR_ItemAt(pstContents->pstResultList, pstContents->ulcurIndexDetailedMenu);
								HxLOG_Debug("Programme detailed svcid(%d) event(%d) (%s)\n", data->svcUid,data->eventId,data->szProgrammeName);
								ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "Nx_PRISM_ProgramDetail_Proc", APP_DEFAULT_POPUP_PRIORITY, Nx_PRISM_ProgramDetail_Proc, 0, data->svcUid, data->eventId, p3);
							}

						}
					}

				}
			return ONDK_MESSAGE_BREAK;
			}

		case KEY_ARROWUP:
			//Draw - Number Of Item
			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4)
			{
				Nx_genreSearch_UpdateNumberOfItem(pstContents, FALSE, FALSE);
			}
			else
			{
				Nx_genreSearch_UpdateNumberOfItem(pstContents, FALSE, TRUE);
			}
			break;

		case KEY_ARROWDOWN:
			//Draw - Number Of Item
			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4)
			{
				Nx_genreSearch_UpdateNumberOfItem(pstContents, TRUE, FALSE);
			}
			else
			{
				Nx_genreSearch_UpdateNumberOfItem(pstContents, TRUE, TRUE);
			}
			break;

		case KEY_OPT:
			if(ONDK_GWM_Obj_GetHide(NX_GENRESEARCH_OPTION_TEXT_ID) == FALSE)
				nx_genreSearch_CreateListPopup(pstContents);
			return	ONDK_MESSAGE_BREAK;

		case KEY_CH_MINUS:
		case KEY_PAGEDOWN:
			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4 && pstContents->pstDetailedMenu->count > 0)
			{
				pstContents->ulcurIndexDetailedMenu += NX_GENRESEARCH_DETAILEDMENU_OBJ_PAGE_COUNT;
				pstContents->ulcurIndexDetailedMenu = pstContents->ulcurIndexDetailedMenu % pstContents->pstDetailedMenu->count;
				ONDK_GWM_List_SetCurrentIndex(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, pstContents->ulcurIndexDetailedMenu);
				Nx_genreSearch_UpdateNumberOfItemForPaging(pstContents);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_CH_PLUS:
		case KEY_PAGEUP:
			if(ONDK_GWM_Obj_GetFocusedObject() == pstContents->objSecondMenu && pstContents->ucDisplayedDataInObjSecondMenu == 4 && pstContents->pstDetailedMenu->count > 0)
			{
				pstContents->ulcurIndexDetailedMenu -= NX_GENRESEARCH_DETAILEDMENU_OBJ_PAGE_COUNT;
				pstContents->ulcurIndexDetailedMenu = pstContents->ulcurIndexDetailedMenu % pstContents->pstDetailedMenu->count;
				ONDK_GWM_List_SetCurrentIndex(NX_GENRESEARCH_SECONDMENU_MAIN_OBJ_ID, pstContents->ulcurIndexDetailedMenu);
				Nx_genreSearch_UpdateNumberOfItemForPaging(pstContents);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_guide_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_GUIDE_GENRESEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_GenreSearchContext_t	*pstContents = NULL;

	pstContents = nx_genreSearch_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_genreSearch_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			HxLOG_Debug("\n\n\n  ======>>> RECEIVED MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES IN GENRE SEARCH\n\n\n");
			nx_genreSearch_GetData(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_genreSearch_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_genreSearch_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				//NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_genreSearch_DestroyListPopup();
				eResult = nx_genreSearch_ChangedOptionItemValue(pstContents, p1);
			}
			break;
		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Guide)
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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
