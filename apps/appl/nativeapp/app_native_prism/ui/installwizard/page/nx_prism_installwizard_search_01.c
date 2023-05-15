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
 * @file	  		nx_prism_installwizard.c
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
#include <nx_prism_install_sub_search.h>
#include <nx_prism_install_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Install_Search_Context_t		s_stInstallSearchContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_Install_Search_Context_t	*nx_install_search_GetContents(void);
static ONDK_Result_t                        nx_install_search_InitApplication (Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                        nx_install_search_UpdateExtendOptBtn(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_UpdateOptBtn(Nx_Install_Search_Context_t *pstContents);
static void                                 nx_install_search_DrawExplain(void);
static void                                 nx_install_search_DrawDirectionBtn(void);
static ONDK_Result_t                        nx_install_search_DrawExtendOptBtn(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_DrawOptBtn(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_DrawSearchTextInfo(void);
static ONDK_Result_t                        nx_install_search_DrawDefaultUI(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_DoLoadingDefaultAntena (Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_DoDiseqSignal(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_MsgGwmCreate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_search_MsgGwmDestroy(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_search_MsgGwmHapiMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_search_MsgGwmObjFocusChanged(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                        nx_install_search_MsgGwmKeyDown(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                        nx_install_search_MsgCoreDiseqDetectionUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Search_Context_t	*nx_install_search_GetContents(void)
{
	return	&s_stInstallSearchContent;
}

static ONDK_Result_t nx_install_search_InitApplication (Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    HUINT8 ulNumOfSubPage = 0;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_Install_Search_Context_t));

    pstContents->ePage = eNxInstallPage_Search;

    eResult = nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ulNumOfSubPage);

    pstContents->pstSubItems = pstSubItemArray;
    pstContents->ucNumOfSubPage= ulNumOfSubPage;

    pstContents->bAutoSearch = TRUE;
    pstContents->bAutoExtendOpt = FALSE;

    pstContents->eSearchState = eInstall_Search_init;
    pstContents->lLoadingIndex = 0;

    pstContents->stAntInfoArray = NULL;
    pstContents->lAntenaCount = 0;

	// Add Noti
	NX_Builder_AddNotiSignalInfo();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_install_search_UpdateExtendOptBtn(Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HBOOL           bAutoExtendOpt = FALSE;
    HBOOL           bAutoSearch = FALSE;

    bAutoExtendOpt = pstContents->bAutoExtendOpt;
    bAutoSearch    = pstContents->bAutoSearch;

    ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID, bAutoExtendOpt);
    ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID, bAutoExtendOpt);

    nx_install_ui_SetDimBackgroundUI(bAutoExtendOpt);


    if(TRUE == bAutoSearch)
    {
        ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID,GUIOBJ_HILIGHTED);         // molink_ksjung_0151017
        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID);
        ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID,GUIOBJ_NORMAL);             // molink_ksjung_0151017
    }else{
        ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID,GUIOBJ_HILIGHTED);         // molink_ksjung_0151017
        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID);
        ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID,GUIOBJ_NORMAL);             // molink_ksjung_0151017
    }

    if(TRUE == bAutoExtendOpt)
    {
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID);
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID);
    }else{
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID);
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID);
    }

    return eResult;
}

static ONDK_Result_t nx_install_search_UpdateOptBtn(Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HBOOL           bAutoExtendOpt = FALSE;
    HBOOL           bAutoSearch = FALSE;

    bAutoExtendOpt = pstContents->bAutoExtendOpt;
    bAutoSearch    = pstContents->bAutoSearch;
    if(TRUE == bAutoExtendOpt)
    {
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID);
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID);
    }
    else
    {
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID);
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID);
    }

    //value
    if(bAutoSearch){
        ONDK_GWM_COM_Button_SetExtraText(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID,(HUINT8*)ONDK_GetString(RES_ON_21_ID));
    }else{
        ONDK_GWM_COM_Button_SetExtraText(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID,(HUINT8*)ONDK_GetString(RES_SKIP_01_ID));
    }

    return eResult;
}

static void nx_install_search_DrawExplain(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HBOOL			bIsZapperOnly = FALSE;
	HINT8			*pStrId = NULL;
	HUINT32         ulId = 0, ulIndex = 0;
	HCHAR			*pszContex = NULL;

	for(ulIndex = 0; ulIndex<2; ulIndex++)
	{
		ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_ID)+ulIndex;
		stRect = ONDK_Rect(NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_X,
							(NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_Y) + ((NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_H) * ulIndex),
							NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_W, NX_PRISM_INSTALL_SEARCH_TEXT_CONTEXT_H);

		switch(ulIndex)
		{
			case 0 :
				bIsZapperOnly = NX_PRISM_SPEC_GetUiSupportZapperOnly();
				if (bIsZapperOnly == TRUE)
				{
					pStrId = RES_MESG_6106_ID;
				}
				else
				{
					pStrId = RES_MESG_4139_ID;
				}
				pszContex = (HCHAR *)ONDK_GetString(pStrId);
				break;

			case 1 :
				pszContex = (char*)ONDK_GetString(RES_MESG_4887_ID);
				break;
		}

		if (pszContex != NULL)
		{
			ONDK_GWM_COM_Text_Create(ulId, &stRect,pszContex);
			ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
			ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
			ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_CENTER|TEXT_ALIGN_MIDDLE);
			ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Gong01), COL(C_T_Gong01));
		}
	}
}

static void nx_install_search_DrawDirectionBtn(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    //PREVIOUS
    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BTN_X), NX_PRISM_INSTALL_SEARCH_BTN_Y , NX_PRISM_INSTALL_SEARCH_BTN_W, NX_PRISM_INSTALL_SEARCH_BTN_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_PREVIOUS_01_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);

    //NEXT
    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BTN_X)+(NX_PRISM_INSTALL_SEARCH_BTN_W)+(NX_PRISM_INSTALL_SEARCH_BTN_GAP), NX_PRISM_INSTALL_SEARCH_BTN_Y , NX_PRISM_INSTALL_SEARCH_BTN_W, NX_PRISM_INSTALL_SEARCH_BTN_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_NEXT_02_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
}

static ONDK_Result_t nx_install_search_DrawExtendOptBtn(Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HBOOL           bAutoExtendOpt = FALSE;
    HBOOL           bAutoSearch = FALSE;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;


    bAutoExtendOpt = pstContents->bAutoExtendOpt;
    bAutoSearch    = pstContents->bAutoSearch;

    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_X),(NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_Y),
        (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_W), (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_ON_21_ID));
    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Bang_100),COL(C_Table_MAX) , COL(C_Table_MAX));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);
 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_LEFT);


    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_X),
        (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_Y) + (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_H),
        (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_W), (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_VALUE_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_SKIP_01_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Bang_100), COL(C_Table_MAX), COL(C_Table_MAX));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);
 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_LEFT);
    return	eResult;
}

static ONDK_Result_t nx_install_search_DrawOptBtn(Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;
    HBOOL bAutoSearch = TRUE;

    bAutoSearch = pstContents->bAutoSearch;

    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_X),(NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_Y),
        (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_W), (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_AUTOMATIC_SEARCH_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100),  COL(C_G_Gong02), COL(C_G_Base01_100), COL(C_G_Base01_20));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_LEFT);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);

    //value
    if(bAutoSearch){
        ONDK_GWM_COM_Button_SetExtraText(ulId,(HUINT8*)ONDK_GetString(RES_ON_21_ID));
    }else{
        ONDK_GWM_COM_Button_SetExtraText(ulId,(HUINT8*)ONDK_GetString(RES_SKIP_01_ID));
    }

    ONDK_GWM_COM_Button_SetExtraAlign(ulId,ONDKSTF_RIGHT);

	// right arrow
	ulId = NX_PRISM_INSTALL_SEARCH_AUTO_ARROW_IMG_ID;
	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_X)+(NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_W)+(NX_PRISM_INSTALL_SEARCH_ARROW_IMG_X_TAB),
        (NX_PRISM_INSTALL_SEARCH_AUTO_OPT_BTN_Y)+(NX_PRISM_INSTALL_SEARCH_ARROW_IMG_Y_TAB),
        (NX_PRISM_INSTALL_SEARCH_ARROW_IMG_W), (NX_PRISM_INSTALL_SEARCH_ARROW_IMG_H));

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_106_00_ARROW_RIGHT_PNG);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    return	eResult;
}

static ONDK_Result_t    nx_install_search_DrawSearchTextInfo(void)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;
    HCHAR           *context;

    //IMG
	ulId = NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_ID;
	stRect = ONDK_Rect(NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_X ,(NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_Y),
        (NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_W),(NX_PRISM_INSTALL_SEARCH_SAT_INFO_IMG_H));

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_321_01_WIZ_CABLE01_PNG);

    //TEXT
	stRect = ONDK_Rect(NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_X,
	NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_Y,
    NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_W,
    NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_H);

    ulId = NX_PRISM_INSTALL_SEARCH_SAT_INFO_TEXT_ID;

//  context = (char*)ONDK_GetString(RES_MESG_4140_ID);
    //It is temprary Text. Must use RES_
    context = "Check you satellite antenna cable first.";

    ONDK_GWM_COM_Text_Create(ulId , &stRect, context );
	ONDK_GWM_COM_Text_SetFont(ulId , eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(ulId , NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(ulId , TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Yoyo02), COL(C_T_Yoyo02));

    return eResult;
}


static ONDK_Result_t	nx_install_search_DrawDefaultUI(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT8          ulPageIndex = 0;
    HUINT8          ulNumofPages = 0;
    HUINT8          ulIndex = 0;
    Nx_InstallSubPage_t	*pstSubItem;

    for(ulIndex = 0; ulIndex < pstContents->ucNumOfSubPage ; ulIndex++)
    {
        pstSubItem = &(pstContents->pstSubItems[ulIndex]);
        if(pstSubItem->ePageName == pstContents->ePage){
            ulPageIndex = ulIndex;
            break;
        }
    }

    ulNumofPages = pstContents->ucNumOfSubPage;

    nx_install_ui_DrawBackgroundUI();
    nx_install_ui_DrawTitle((HUINT8*)ONDK_GetString(RES_CHANNEL_SEARCH_01_ID));
    nx_install_ui_DrawStepIndicatior(ulPageIndex);
    nx_install_search_DrawExplain();

    nx_install_search_DrawSearchTextInfo();

    //Previous, Next Btn
    nx_install_search_DrawDirectionBtn();

    //dim
    nx_install_ui_SetDimBackgroundUI(FALSE); //For Extend Btn Highlight

    //select btn
    nx_install_search_DrawOptBtn(pstContents);

    //extend btn
    nx_install_search_DrawExtendOptBtn(pstContents);

    nx_install_search_UpdateOptBtn(pstContents);
    nx_install_search_UpdateExtendOptBtn(pstContents);

    ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID);

    //Progress Popup
    nx_install_ui_SetProcessingPopupUI(FALSE, (char*)ONDK_GetString(RES_MESG_3214_ID));

    return eResult;
}


static ONDK_Result_t nx_install_search_DoLoadingDefaultAntena (Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HINT32			lAntIndex = 0;
    DxAntInfo_t     *stAntInfo;
    HERROR			hError = ERR_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);
        eResult = ONDK_RET_INITFAIL;
		return eResult;
	}

    eResult = nx_install_model_GetAntenaInfo(&(pstContents->stAntInfoArray) ,&(pstContents->lAntenaCount));

    if(ONDK_RET_OK == eResult)
    {
        for(lAntIndex = 0; lAntIndex < pstContents->lAntenaCount ; lAntIndex++)
        {
            stAntInfo = &(pstContents->stAntInfoArray[lAntIndex]);
    	 	hError = NX_ANTENNA_SetInfo(stAntInfo->uid, stAntInfo);
	    	hError = NX_ANTENNA_SetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1+lAntIndex, stAntInfo->uid);
        }
    }

	return eResult;
}

static ONDK_Result_t nx_install_search_DoDiseqSignal(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t   	eResult = ONDK_RET_OK;
    HERROR			    hError = ERR_FAIL;
	HUINT32		    	i = 0;
	HUINT32			    ulTpCount = 0, ulAntCount = 0 , lUid = 0;
	DxAntInfo_t			*pstAntInfo = NULL;
    NX_SCANPARAMS_t 	*pstParam  = NULL;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;
    NX_SCANOPTIONS_t	stScanOption;
    NX_TP_TuningInfo_t	*pstDiseqTuneInfo = NULL;
    HUINT32             ulDiseqTpCount =0;
    HUINT32             ulDiseqTpIdx =0;

	hError = NX_Builder_StopScan(NX_SCANTYPE_DISEQCDETECTION);

    //check if antena info is.
    ulAntCount = NX_ANTENNA_Count();

    HxLOG_Error("\n\n####### ulAntCount (%d) ########\n\n",ulAntCount);

    if(ulAntCount < 1)
    {
        //No Antena How to Set?
        eResult = nx_install_search_DoLoadingDefaultAntena(pstContents);
        ulAntCount = pstContents->lAntenaCount;
    }
    else
    {
        //use saved Antena Info
        pstContents->lAntenaCount = ulAntCount;
        pstContents->stAntInfoArray = (DxAntInfo_t *)NX_APP_Malloc (sizeof(DxAntInfo_t) * ulAntCount);

    	for (i = 0; i < ulAntCount; i++)
    	{
    	    pstAntInfo = &(pstContents->stAntInfoArray[i]);
    	    NX_ANTENNA_GetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1+i,(HINT32*)&lUid);
    		HxSTD_memset(pstAntInfo, 0x00, sizeof(DxAntInfo_t));
            hError = NX_ANTENNA_GetInfo(lUid, pstAntInfo);
            if ((hError == ERR_OK)	&& (HxSTD_StrCmp(pstAntInfo->satName, (char*)NX_SAT_SATNAME_NONE) != 0))
    		{
    			HxLOG_Error("\n\nCheck Antenna !! [%d], Uid:[%d], Name:[%s] antType[%d]\n", i, lUid, pstAntInfo->satName,pstAntInfo->antType);
    		}
    	}
    }

    //Get TP LIST

    pstAntInfo = &(pstContents->stAntInfoArray[0]);//ToDo : check If Antena is over 2, Which one do use antena for diseq scan?

	hError = NX_ANTENNA_GetTpListBySatelliteTypeName(pstAntInfo->satType, &ulTpCount, &pstTuneInfo);

    if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR!!! Antenna: [%s] Get TP List fail!!\n", pstAntInfo->satType);
		return	ONDK_RET_FAIL;
	}

    //Diseq TP List
    for(i = 0; i < ulTpCount ; i++)
    {
        if(TRUE == pstTuneInfo[i].sat.bFindSat)
        {
            ulDiseqTpCount+=1;
        }
    }

    if(ulDiseqTpCount == 0)
    {
        HxLOG_Error("ERROR!!! No Diseq TP \n");

		if (pstTuneInfo != NULL)
		{
			NX_ANTENNA_FreeTpListArray(pstTuneInfo);
			pstTuneInfo = NULL;
		}
		return	ONDK_RET_FAIL;
    }

    pstDiseqTuneInfo = (NX_TP_TuningInfo_t *)NX_APP_Malloc (sizeof(NX_TP_TuningInfo_t) * ulDiseqTpCount);

	HxSTD_memset(pstDiseqTuneInfo, 0x00, sizeof(NX_TP_TuningInfo_t) * ulDiseqTpCount);

    ulDiseqTpIdx = 0;

    for(i = 0; i < ulTpCount ; i++)
    {
        if(TRUE == pstTuneInfo[i].sat.bFindSat)  // check Diseq TP for Diseq searching
        {
            HxSTD_memcpy(&pstDiseqTuneInfo[ulDiseqTpIdx],&pstTuneInfo[i], sizeof(NX_TP_TuningInfo_t));
            ulDiseqTpIdx++;
        }
    }

    pstParam = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * ulDiseqTpCount);

#if defined(CONFIG_MW_CH_SATELLITE)
    for (i = 0; i < ulDiseqTpCount; i++)
	{
		pstParam[i].tunerType = eDxDELIVERY_TYPE_SAT;

		pstParam[i].data.stDVBSScanParams.antennaId = pstAntInfo->uid;
		HxSTD_snprintf(pstParam[i].data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", pstAntInfo->satType);

		pstParam[i].data.stDVBSScanParams.startFreq = pstDiseqTuneInfo[i].sat.ulFrequency;
		pstParam[i].data.stDVBSScanParams.endFreq = pstDiseqTuneInfo[i].sat.ulFrequency;
		pstParam[i].data.stDVBSScanParams.modulationModes = pstDiseqTuneInfo[i].sat.ePskMod;

		if (pstDiseqTuneInfo[i].sat.eTransSpec == eDxSAT_TRANS_DVBS)
		{
			pstParam[i].data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
		}

		pstParam[i].data.stDVBSScanParams.polarisation = pstDiseqTuneInfo[i].sat.ePolarization;
		pstParam[i].data.stDVBSScanParams.networkId = 0;

		HxSTD_snprintf(pstParam[i].data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstDiseqTuneInfo[i].sat.ulSymbolRate));

		switch (pstDiseqTuneInfo[i].sat.eFecCodeRate)
		{
			case eDxSAT_CODERATE_1_2:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));	break;
			case eDxSAT_CODERATE_2_3:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));	break;
			case eDxSAT_CODERATE_3_4:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));	break;
			case eDxSAT_CODERATE_5_6:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));	break;
			case eDxSAT_CODERATE_7_8:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));	break;
			case eDxSAT_CODERATE_3_5:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));	break;
			case eDxSAT_CODERATE_4_5:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));	break;
			case eDxSAT_CODERATE_5_11:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));	break;
			case eDxSAT_CODERATE_6_7:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));	break;
			case eDxSAT_CODERATE_8_9:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));	break;
			case eDxSAT_CODERATE_9_10:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));	break;
			default:					HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));	break;
		}
	}
#endif

	if (pstTuneInfo != NULL)
	{
		NX_ANTENNA_FreeTpListArray(pstTuneInfo);
		pstTuneInfo = NULL;
	}

	if (pstDiseqTuneInfo != NULL)
	{
		NX_ANTENNA_FreeTpListArray(pstDiseqTuneInfo);
		pstDiseqTuneInfo = NULL;
	}

    HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	// Get Search Option
	NX_SEARCH_GetSearchOption(&stScanOption);

    // Start Scan
    hError = NX_Builder_StartScan(NX_SCANTYPE_DISEQCDETECTION, &stScanOption, ulDiseqTpCount, pstParam);

	if (pstParam)
	{
		NX_APP_Free(pstParam);
		pstParam = NULL;
	}

	if (hError != ERR_OK)
	{
		HxLOG_Error("Error!!! NX_Builder_StartScan() - NX_SCANTYPE_NORMAL Fail !! \n");
		return	ONDK_RET_FAIL;
	}

    pstContents->eSearchState = eInstall_Search_checkingDiseq;

    return eResult;
}

static ONDK_Result_t	nx_install_search_MsgGwmCreate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_search_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	nx_install_search_DrawDefaultUI(pstContents);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Wizard);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	eResult;
}


static ONDK_Result_t	nx_install_search_MsgGwmDestroy(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->stAntInfoArray)
	{
    	NX_APP_Free(pstContents->stAntInfoArray);
	}
	if (pstContents->pstSubItems)
	{
   		NX_APP_Free(pstContents->pstSubItems);
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_search_MsgGwmHapiMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_install_search_MsgGwmObjFocusChanged(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HINT32 lSubItemObjId = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    lSubItemObjId = p1;

    //Arrow UI Update
    switch(lSubItemObjId){
        case NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID:
        case NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID:
            ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_AUTO_ARROW_IMG_ID, FALSE);
            break;
        default:
            ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_AUTO_ARROW_IMG_ID, TRUE);
            break;
    }

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
    return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_install_search_MsgGwmKeyDown(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HINT32			lKeyId = p1;
    Nx_InstallSubPage_t *pstSubItem = NULL;
    HINT8           zIndex, zCurrentStep = 0;
    HINT32          lNumOfPages = 0;
    HINT32      lObjId;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    //Check common key
    {
        ONDK_Result_t ret = ONDK_MESSAGE_PASS;
        ret = nx_install_ui_MsgGwmKeyDown(p1,p2,p3);
        if(ret != ONDK_MESSAGE_PASS)
        {
            return ret;
        }
    }

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

    if(eInstall_Search_init!= pstContents->eSearchState && KEY_STANDBY!=lKeyId)
    {
        return	ONDK_MESSAGE_BREAK;
    }

    lObjId = GWM_GetFocusedObjectID();
    lNumOfPages = pstContents->ucNumOfSubPage;
    for(zIndex=0;zIndex<lNumOfPages;zIndex++)
    {
        pstSubItem = &(pstContents->pstSubItems[zIndex]);
        if(pstContents->ePage == pstSubItem->ePageName){
            zCurrentStep = zIndex;
        }
    }

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
            {
                if(TRUE == pstContents->bAutoExtendOpt)
                {
                    switch(lObjId)
                    {
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID:
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID:
                        pstContents->bAutoExtendOpt = FALSE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
                        break;
                    }
                }
                else
                {
                    //move to previous Step
                    eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
                    if(ONDK_RET_OK == eResult)
                    {
                        ONDK_GWM_APP_Destroy(0);
                    }
                }
            }
            return ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_MENU:
		case KEY_GUIDE:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
             {
                //Do something

                //move to Progress proc
                switch(lObjId){
                    case NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID:
                         //move to previous Step
                        eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
                        if(ONDK_RET_OK == eResult)
                        {
                            ONDK_GWM_APP_Destroy(0);
                        }
                        break;
                    case NX_PRISM_INSTALL_SEARCH_BTN_NEXT_ID:
                        if(TRUE == pstContents->bAutoSearch)
                        {
                            nx_install_ui_SetProcessingPopupUI(TRUE, NULL);
                            ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_SEARCH_ANIMATION_TIMER_PERIOD);
                            nx_install_search_DoDiseqSignal(pstContents);
                        }else
                        {
                            nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
                            ONDK_GWM_APP_Destroy(0);
                        }
                        break;
                    case NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID:
                        pstContents->bAutoExtendOpt = TRUE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        break;
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID:
                        pstContents->bAutoExtendOpt = FALSE;
                        pstContents->bAutoSearch = TRUE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
                        break;
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID:
                        pstContents->bAutoExtendOpt = FALSE;
                        pstContents->bAutoSearch = FALSE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
                        break;
                }
             }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
            switch(lObjId)
            {
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_ON_ID:
                        pstContents->bAutoExtendOpt = FALSE;
                        pstContents->bAutoSearch = TRUE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
                        break;
                    case NX_PRISM_INSTALL_SEARCH_BTN_OPT_SKIP_ID:
                        pstContents->bAutoExtendOpt = FALSE;
                        pstContents->bAutoSearch = FALSE;
                        nx_install_search_UpdateOptBtn(pstContents);
                        nx_install_search_UpdateExtendOptBtn(pstContents);
                        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID);
                        break;
            }
            break;
		case KEY_ARROWRIGHT:
            switch(lObjId)
            {
              case NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID:
                   pstContents->bAutoExtendOpt = TRUE;
                   nx_install_search_UpdateOptBtn(pstContents);
                   nx_install_search_UpdateExtendOptBtn(pstContents);
                    break;
            }
            break;
		case KEY_ARROWUP:
            break;
		case KEY_ARROWDOWN:
            if(NX_PRISM_INSTALL_SEARCH_BTN_AUTO_OPT_ID == lObjId)
            {
               nx_install_search_UpdateOptBtn(pstContents);
               ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_AUTO_ARROW_IMG_ID, FALSE);
               ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_PREVIOUS_ID);
            }

            break;
		case KEY_STANDBY:
            HxLOG_Error("KEY_STANDBY\n");
            if(pstContents->eSearchState == eInstall_Search_checkingDiseq)
            {
                HxLOG_Error("Error!!! KEY_STANDBY STOP NX_SCANTYPE_DISEQCDETECTION \n");
                //Power KEY
                NX_Builder_StopScan(NX_SCANTYPE_DISEQCDETECTION);
                NX_Builder_ReleaseNotiSignalInfo();
            }

            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		default:
			NX_APP_Print("nx_menuSettings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_install_search_MsgCoreDiseqDetectionUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					lProgress = p2;
	NX_BUILDER_Status_e		eBuilderStatus = p1;
    HINT32                  lSearchedServiceNum = p3;
    HUINT32                 lUid =0;
    DxAntInfo_t 			stAntInfo;
    HERROR					hError = ERR_FAIL;

	HxSTD_memset(&stAntInfo, 0, sizeof(DxAntInfo_t));
	stAntInfo.antType = eDxANT_TYPE_LNB_ONLY;

	if (pstContents == NULL)
	{
		HxLOG_Error("Error!!! pstContents is NULL !! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	HxLOG_Error("===> DiseqDetection p1:(0x%x), p2:(0x%x), p3:(0x%x)!! ===\n", p1, p2, p3);

	if ((lProgress == 100) || (eBuilderStatus == eBUILDER_COMPLETE))
	{
        NX_Builder_ReleaseNotiSignalInfo();
        hError = NX_Builder_ControlScan(NX_SCANTYPE_LOAD);
        hError = NX_Builder_ControlScan(NX_SCANTYPE_SAVE);
	    hError = NX_Builder_StopScan(NX_SCANTYPE_DISEQCDETECTION);

		HxLOG_Error("=== DiseqDetection Complete!!!  lSearchedServiceNum:(0x%x)!! ===\n", lSearchedServiceNum);

        if(lSearchedServiceNum >= 1)
        {
            //set Diseq Antana Info
            NX_ANTENNA_GetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1,(HINT32*)&lUid);
            hError = NX_ANTENNA_GetInfo(lUid, &stAntInfo);
            HxLOG_Error("\n MsgCoreDiseqDetectionUpdate lUid[%d] \n", lUid);

            if ((hError == ERR_OK)	&& (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) != 0))
            {
        		HxLOG_Error("\nAntenna Setting!! Uid:[%d], Name:[%s]\n", lUid, stAntInfo.satName);
                HxLOG_Error("\nAntenna Setting!! Uid:[%d], antType:[%d]\n", lUid, stAntInfo.antType);
        	}
        }
        NX_ANTENNA_SetAntennaType(stAntInfo.antType);
        pstContents->eSearchState = eInstall_Search_searchFinish;
	}
    else if((eBuilderStatus == eBUILDER_ABORTED))
    {
        NX_Builder_ReleaseNotiSignalInfo();
        pstContents->eSearchState = eInstall_Search_errorOccurred; //Searching Fail
    }

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Search_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Search_Context_t	*pstContents = NULL;
	pstContents = nx_install_search_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_search_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_install_search_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
            nx_install_search_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			if (p1 == NX_PRISM_INSTALL_ANIMATION_TIMER_ID)
            {
                nx_install_ui_UpdateProcessingPopupUI(pstContents->lLoadingIndex);
                if(pstContents->lLoadingIndex<3)
                {
                    pstContents->lLoadingIndex++;
                }
                else
                {
                    pstContents->lLoadingIndex = 0;
                }

                if( (eInstall_Search_searchFinish == pstContents->eSearchState))
                {
                    ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                    ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_Install_Search_Progress_Proc", APP_UIMGR_PRIORITY,
                    NX_PRISM_Install_Search_Progress_Proc,  0, pstContents->ePage, p2, p3);
                    ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_Install_Search_Proc);
                }
                else if( (eInstall_Search_errorOccurred == pstContents->eSearchState))
                {
                    ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                    nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
                    ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_Install_Search_Proc);
                }
            }
            break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_search_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_install_search_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_CORE_SERVICE_EVT_DISEQCDETECTION:
            HxLOG_Error("MSG_CORE_SERVICE_EVT_DISEQCDETECTION!\n");
            eResult = nx_install_search_MsgCoreDiseqDetectionUpdate(pstContents, p1, p2, p3);
			break;
        case MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE:
            HxLOG_Error("MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE!\n");
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

