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
static HUINT32	                    	s_ulSatlliteSearchingTime; //Wait Time

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t                    	nx_install_search_Progress_MsgGwmCreate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_search_Progress_MsgGwmDestroy(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_search_Progress_MsgGwmHapiMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_search_Progress_MsgGwmKeyDown(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_search_Progress_MsgGwmTimerMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                        nx_install_search_Progress_MsgCoreAntennaConnectStateUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_search_Progress_MsgCoreChannelStateUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static inline Nx_Install_Search_Context_t	*nx_install_search_Progress_GetContents(void);
static ONDK_Result_t	                    nx_install_search_Progress_InitApplication (Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static void                                 nx_install_search_Progress_DrawDirectionBtn(void);
static ONDK_Result_t                        nx_install_search_Progress_DrawTVInfo(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_Progress_DrawRadioInfo(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_Progress_UpdateProgressLevel(Nx_Install_Search_Context_t *pstContents,HUINT32 ulLevelPercent);
static ONDK_Result_t                        nx_install_search_Progress_DrawProgress(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_Progress_DrawSelectivePopup(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t                        nx_install_search_Progress_DoSelectivePopup(Nx_Install_Search_Context_t *pstContents, HBOOL bShowPopup);
static ONDK_Result_t	                    nx_install_search_Progress_DoSavingChannel(Nx_Install_Search_Context_t *pstContents);
static ONDK_Result_t	                    nx_install_search_Progress_DrawDefaultUI(Nx_Install_Search_Context_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Search_Context_t	*nx_install_search_Progress_GetContents(void)
{
	return	&s_stInstallSearchContent;
}

static ONDK_Result_t	nx_install_search_Progress_InitApplication (Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTunerCount = 0;
	HUINT8		ucNumOfSubPage = 0;
	HERROR		hError = ERR_FAIL;
	Nx_InstallSubPage_t *pstSubItemArray = NULL;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_Install_Search_Context_t));

    if(p1 == eNxInstallPage_Search || p1 == eNxInstallPage_Search_Auto)
    {
    	pstContents->ePage = p1;
    }
    else
    {
    	pstContents->ePage = eNxInstallPage_Search;
    }



	nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ucNumOfSubPage);

	pstContents->pstSubItems = pstSubItemArray;
	pstContents->ucNumOfSubPage = ucNumOfSubPage;
	pstContents->ulSearchLevel = 0;
	pstContents->eSearchState = eInstall_Search_init;
	pstContents->szTpInfoString[0] = '\0';
	pstContents->pStrResult = (HCHAR*)HxSTR_MemAlloc(sizeof(HCHAR)*NX_TEXT_SIZE_32);

	s_ulSatlliteSearchingTime = 0;

	HxSTD_memset(pstContents->aulNumSvc, 0, sizeof(HUINT32) * NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM);
	HxSTD_memset(pstContents->aulTotalSvc, 0, sizeof(HUINT32) * NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM);
	HxSTD_memset(pstContents->aulSvcNameListSize, 0, sizeof(HUINT32) * NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM);
	HxSTD_memset(pstContents->pszSvcNameList, 0, sizeof(HUINT8 **) * NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM);
    HxSTD_MemSet(pstContents->pStrResult, 0x00,sizeof(HCHAR)*NX_TEXT_SIZE_32);

	hError = NX_SYSTEM_GetTunerDeviceNum(&ulTunerCount);
	if (hError != ERR_OK)
	{
		pstContents->ulTunerCount = 0;
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetTunerDeviceNum() Fail !! \n");
	}
	else
	{
		pstContents->ulTunerCount = ulTunerCount;
	}



	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static void nx_install_search_Progress_DrawDirectionBtn(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32         ulId = 0;

	//STOP
	ulId = (NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID);
	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_STOP_BTN_X), NX_PRISM_INSTALL_SEARCH_STOP_BTN_Y ,
	    NX_PRISM_INSTALL_SEARCH_STOP_BTN_W, NX_PRISM_INSTALL_SEARCH_STOP_BTN_H);
	ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_STOP_01_ID));

	ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
	ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
	ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Bang_100), COL(C_Table_MAX), COL(C_Table_MAX));
	ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
	ONDK_GWM_Obj_SetVisible(ulId, TRUE);
	ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
	ONDK_GWM_Obj_SetFocus(ulId);
}

static ONDK_Result_t nx_install_search_Progress_DrawTVInfo(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32         ulId = 0;
	HCHAR*          pStrTVTitle = NULL;
	HUINT32         ulIndex = 0;
	const HCHAR*    pStrTitleTemplet = "%s : %d";

	pStrTVTitle = HxSTR_MemAlloc(sizeof(HUINT8)*NX_TEXT_SIZE_32);

	HxSTD_MemSet(pStrTVTitle, 0x00, NX_TEXT_SIZE_32);
	HxSTD_snprintf((char*)pStrTVTitle, NX_TEXT_SIZE_32, pStrTitleTemplet, (char*)ONDK_GetString(RES_TV_ID),0);

	//TV BOX
	ulId = (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_ID);
	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_X), (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_Y),
						(NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_W), (NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_H));
	ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Gong02));

	//TV Text
	for(ulIndex = 0; ulIndex<(NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT);ulIndex++)
	{
		ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_ID) + ulIndex;
		stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_X)+(NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_X),
							(NX_PRISM_INSTALL_SEARCH_BOX_TV_INFO_Y)+(NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_Y)
							+((NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_H)*ulIndex) ,
							(NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_W), (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_H));

		ONDK_GWM_COM_Text_Create(ulId, &stRect,"");
		ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Gong03), COL(C_G_Gong02));
		ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_LEFT);
		ONDK_GWM_Obj_SetVisible(ulId, TRUE);
	}

	//TV TITLE
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_ID);
	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_X), (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_Y),
						(NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_W), (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_H));

	ONDK_GWM_COM_Text_Create(ulId, &stRect,pStrTVTitle);
	ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_LEFT);

	ONDK_GWM_Obj_SetVisible(ulId, TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return eResult;
}


static ONDK_Result_t nx_install_search_Progress_DrawRadioInfo(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT32         ulId = 0;
	HCHAR*         pStrTVTitle = NULL;
	HUINT32         ulIndex = 0;
	const HCHAR*          pStrTitleTemplet = "%s : %d";

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	pStrTVTitle = HxSTR_MemAlloc(sizeof(HUINT8)*32);
	HxSTD_MemSet(pStrTVTitle, 0x00, 32);

	HxSTD_snprintf((char*)pStrTVTitle, 32,pStrTitleTemplet ,(char*)ONDK_GetString(RES_RADIO_ID),0);

	//RADIO BOX
	ulId = (NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_ID);
	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_X), (NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_Y),
	    (NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_W), (NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_H));

	ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Gong02));
	//RADIO Text
	for(ulIndex = 0; ulIndex<(NX_PRISM_INSTALL_SEARCH_RADIO_INFO_LIMIT);ulIndex++)
	{
	    ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_ID) + ulIndex;
	    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_X)+(NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_X),
	        (NX_PRISM_INSTALL_SEARCH_BOX_RADIO_INFO_Y)+(NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_Y)
	        +((NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_H)*ulIndex) ,
	        (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_W), (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_H));

		ONDK_GWM_COM_Text_Create(ulId, &stRect,"");
		ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Gong03), COL(C_G_Gong02));
		ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_LEFT);
		ONDK_GWM_Obj_SetVisible(ulId, TRUE);
	}

	//RADIO TITLE
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_ID);

	stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_X), (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_Y),
	    (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_W), (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_H));

	ONDK_GWM_COM_Text_Create(ulId, &stRect,pStrTVTitle);
	ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Bang_100));
	ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_LEFT);

	ONDK_GWM_Obj_SetVisible(ulId, TRUE);

	return eResult;
}

static ONDK_Result_t nx_install_search_Progress_UpdateProgressLevel(Nx_Install_Search_Context_t *pstContents,HUINT32 ulLevelPercent)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT32         ulId = 0;
	HUINT32         ulDefaultValue = 0, ulSearchedWidth = 0;
	HUINT32         ulPercent;
	HCHAR*         pStrPercentText = NULL;
    const HCHAR*    pStrTitleTemplet = "%s %d";

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	ulPercent = ulLevelPercent;

	if(ulPercent>100){
	    ulPercent = 100;
	}

	//Progress Bar
	ulId = (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_ID);

	ulDefaultValue = (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_W)/100;

	if(ulPercent >= 100)
	{
	    ulSearchedWidth = (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_W);
	    pstContents->eSearchState = eInstall_Search_searchFinish;

	    //[STOP] to [SAVE}
	    ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID,
	    (char*)ONDK_GetString(RES_SAVE_01_ID));
	}
	else if(ulPercent == 0)
	{
	    ulSearchedWidth = 0;
        if(eInstall_Search_pause != pstContents->eSearchState)
        {
            pstContents->eSearchState = eInstall_Search_searching;
        }
	}
	else{
	    ulSearchedWidth = ulDefaultValue * ulPercent;
        if(eInstall_Search_pause != pstContents->eSearchState)
        {
            pstContents->eSearchState = eInstall_Search_searching;
    	    ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID,
	        (char*)ONDK_GetString(RES_STOP_01_ID));
        }
	}

	if(ulSearchedWidth>0){
	    ONDK_GWM_Obj_SetVisible(ulId,TRUE);
	    ONDK_GWM_COM_Rect_ChangeSize(ulId,
	    (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_X),
	    (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_Y),
	    ulSearchedWidth,
	    (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_H)
	    );
	}else{
	    ONDK_GWM_Obj_SetVisible(ulId,FALSE);
	}

	//TP INFO TEXT
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID);
	ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*)pstContents->szTpInfoString);

	//Text
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_ID);
	pStrPercentText = HxSTR_MemAlloc(sizeof(HUINT8)*32);
	HxSTD_MemSet(pStrPercentText, 0x00, 32);

	HxSTD_snprintf(pStrPercentText, 32,"%d%%",ulPercent);
	ONDK_GWM_COM_Text_SetText(ulId, pStrPercentText);
	pstContents->ulSearchLevel = ulPercent;

    //Result
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_ID);
	HxSTD_MemSet(pstContents->pStrResult, 0x00,sizeof(HCHAR)*NX_TEXT_SIZE_32);
    HxSTD_snprintf(pstContents->pStrResult, NX_TEXT_SIZE_32,pStrTitleTemplet ,(char*)ONDK_GetString(RES_SEARCHED_CHANNELS_ID),(HINT32)pstContents->ulSearchedServiceNum);
	ONDK_GWM_COM_Text_SetText(ulId, pstContents->pStrResult);

	return eResult;
}

static ONDK_Result_t nx_install_search_Progress_DrawProgress(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;
    const HCHAR*    pStrTitleTemplet = "%s %d";

    //Text

    ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_X), (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_Y),
        (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_W), (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_H));

    ONDK_GWM_COM_Text_Create(ulId, &stRect,"");
    ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Pitch01), COL(C_T_Pitch01));
    ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_LEFT);

    ONDK_GWM_Obj_SetVisible(ulId, TRUE);

    //Percent
    ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_X),
        (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_Y),
        (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_W),
        (NX_PRISM_INSTALL_SEARCH_TEXT_PROGRESS_RATE_H));

    ONDK_GWM_COM_Text_Create(ulId, &stRect,"0%");
    ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Pitch01), COL(C_T_Pitch01));
    ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_RIGHT);

    ONDK_GWM_Obj_SetVisible(ulId, TRUE);

    //Progress Bar BG

    ulId = (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_X),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_Y),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_W),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_H));

    ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Whistle_20));

    //Progress Bar Level

    ulId = (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_X),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_BG_Y),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_W),
        (NX_PRISM_INSTALL_SEARCH_BOX_PROGRESS_LEVEL_H));

    ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Pitch01));
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_ID);

	// Total Text
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_ID);
	stRect = ONDK_Rect(NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_X,
        NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_Y,
        NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_W,
        NX_PRISM_INSTALL_SEARCH_TEXT_SEARCHED_H);

	// TV - TEXT
	HxSTD_MemSet(pstContents->pStrResult, 0x00,sizeof(HCHAR)*NX_TEXT_SIZE_32);
    HxSTD_snprintf(pstContents->pStrResult, NX_TEXT_SIZE_32,pStrTitleTemplet ,(char*)ONDK_GetString(RES_SEARCHED_CHANNELS_ID),(HINT32)pstContents->ulSearchedServiceNum);

	ONDK_GWM_COM_Text_Create(ulId, &stRect, pstContents->pStrResult);
	ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_32);
	ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Bang_100));
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

  	return eResult;
}

static ONDK_Result_t nx_install_search_Progress_DrawSelectivePopup(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    //dim
    nx_install_ui_SetDimBackgroundUI(FALSE); //For Extend Btn Highlight

    //Popup BG
    ulId = (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_W),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_H));
    ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Bang_90));

    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    //Top Frame
    ulId = (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME01_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_W),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME01_H));
    ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Whistle_100));
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    //Bottom Frame
    ulId = (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME02_ID);

    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BOX_POPUP_X),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_Y)+(NX_PRISM_INSTALL_SEARCH_BOX_POPUP_H),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_W),
        (NX_PRISM_INSTALL_SEARCH_BOX_POPUP_FRAME02_H));
    ONDK_GWM_COM_Rect_Create(ulId, &stRect, COL(C_G_Whistle_100));
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    //CONTEXT
    ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_X),
        (NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_Y),
        (NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_W),
        (NX_PRISM_INSTALL_SEARCH_TEXT_POPUP_CONTEXT_H));

    ONDK_GWM_COM_Text_Create(ulId, &stRect,(char*)ONDK_GetString(RES_MESG_3215_ID));
    ONDK_GWM_COM_Text_SetFont(ulId, eFont_SystemNormal);
    ONDK_GWM_COM_Text_SetSize(ulId, NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_COM_Text_SetAlign(ulId, TEXT_ALIGN_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);

    //YES
    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_X),
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_Y) ,
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_W),
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_YES_01_ID));
    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_T_Whistle_100), COL(C_T_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong01), COL(C_Table_MAX), COL(C_Table_MAX));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    ONDK_GWM_Obj_DisableObject(ulId);

    //NO
    ulId = (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_X),
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_Y) ,
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_W),
        (NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_NO_03_ID));
    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_T_Whistle_100), COL(C_T_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong01), COL(C_Table_MAX), COL(C_Table_MAX));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    ONDK_GWM_Obj_DisableObject(ulId);

  	return eResult;
}

ONDK_Result_t	nx_install_search_Progress_DrawDefaultUI(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT8          ulPageIndex = 0;
    HUINT8          ucNumofPages = 0, ulIndex = 0;
    Nx_InstallSubPage_t	*pstSubItem;

    for(ulIndex = 0; ulIndex < pstContents->ucNumOfSubPage ; ulIndex++)
    {
        pstSubItem = &(pstContents->pstSubItems[ulIndex]);
        if(pstSubItem->ePageName == pstContents->ePage){
            ulPageIndex = ulIndex;
            break;
        }
    }

    ucNumofPages = pstContents->ucNumOfSubPage;

    nx_install_ui_DrawBackgroundUI();
    nx_install_ui_DrawTitle((HUINT8*)ONDK_GetString(RES_CHANNEL_SEARCH_01_ID));
    nx_install_ui_DrawStepIndicatior(ulPageIndex);

    nx_install_search_Progress_DrawTVInfo(pstContents);
    nx_install_search_Progress_DrawRadioInfo(pstContents);

    nx_install_search_Progress_DrawProgress(pstContents);
    nx_install_search_Progress_UpdateProgressLevel(pstContents,0);

    //STOP Btn
    nx_install_search_Progress_DrawDirectionBtn();

    //Popup
    nx_install_search_Progress_DrawSelectivePopup(pstContents);
    nx_install_search_Progress_DoSelectivePopup(pstContents,FALSE);

    //Progress Popup
    nx_install_ui_SetProcessingPopupUI(FALSE, (char*)ONDK_GetString(RES_SAVING_CHANNELS_ID));

    return eResult;
}


static ONDK_Result_t	nx_install_search_Progress_DoSaveServiceForSaveData(Nx_Install_Search_Context_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	NX_APP_Trace("[%s:%d]\n", __FUNCTION__, __LINE__);

	hError = NX_Builder_ControlScan(NX_SCANTYPE_SAVE);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_SAVE Fail !! \n");
	}

	NX_CHANNEL_LoadList(TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_search_Progress_DoLoadServiceForSaveData(Nx_Install_Search_Context_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	NX_APP_Trace("[%s:%d]\n", __FUNCTION__, __LINE__);

	hError = NX_Builder_ControlScan(NX_SCANTYPE_LOAD);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_PAUSE Fail !! \n");
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t nx_install_search_Progress_DoSelectivePopup(Nx_Install_Search_Context_t *pstContents, HBOOL bShowPopup)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulIndex =0;

    //dim
    nx_install_ui_SetDimBackgroundUI(bShowPopup); //For Extend Btn Highlight

    for(ulIndex=0;ulIndex<NX_PRISM_INSTALL_SEARCH_OBJ_POPUP_NUM;ulIndex++)
    {
        ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_SEARCH_POPUP_ID +(ulIndex), bShowPopup);
    }

    if(TRUE == bShowPopup)
    {
        //Default Focus NO Btn
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_ID);
        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID);
        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID);
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID);
    }
    else
    {
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_ID);
        ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID);

        ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID);
        ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID);
    }

    return eResult;
}

static ONDK_Result_t nx_install_search_Progress_DoChSearching(Nx_Install_Search_Context_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulAntIndex = 0;
	HUINT32				i = 0, ulNextTPIndex = 0,ulAllTPCount = 0,ulTpCount = 0;
	DxAntInfo_t			*pstAntInfo;
	Nx_TpSelectInfo_t	stTpInfo;
	NX_SCANOPTIONS_t	stScanOption;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;
    HUINT32			    ulAntCount = 0;
    HINT32              lUid = -1;

    NX_SCANPARAMS_t		*pstAllScanParams = NULL;

	// Stop Search
	hError = NX_Builder_StopScan(NX_SCANTYPE_NORMAL);

	// Load Default Antenna Info
	ulAntCount = NX_ANTENNA_Count();
    if(ulAntCount < 1)
    {
        HxLOG_Error("ERROR!! NO Antenna!! \n");
        return ONDK_RET_FAIL;
    }
    else
    {
        pstContents->lAntenaCount = ulAntCount;
        pstContents->stAntInfoArray = (DxAntInfo_t *)NX_APP_Malloc (sizeof(DxAntInfo_t) * ulAntCount);

    	for (i = 0; i < ulAntCount; i++)
    	{
    	    pstAntInfo = &(pstContents->stAntInfoArray[i]);
    	    NX_ANTENNA_GetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1+i,&lUid);
    		HxSTD_memset(pstAntInfo, 0x00, sizeof(DxAntInfo_t));
            hError = NX_ANTENNA_GetInfo(lUid, pstAntInfo);
            if ((hError == ERR_OK)	&& (HxSTD_StrCmp(pstAntInfo->satName, (char*)NX_SAT_SATNAME_NONE) != 0))
    		{
    			HxLOG_Error("\n\n Check Antenna !! [%d], Uid:[%d], Name:[%s] antType[%d]\n", i, lUid, pstAntInfo->satName,pstAntInfo->antType);
    		}
    	}
    }

    //check All TP NUM

    for(ulAntIndex = 0; ulAntIndex < ulAntCount;ulAntIndex++)
    {
  	    pstAntInfo = &(pstContents->stAntInfoArray[ulAntIndex]);
        //Get TP LIST
	    hError = NX_ANTENNA_GetTpListBySatelliteTypeName(pstAntInfo->satType, &ulTpCount, &pstTuneInfo);
        if (hError != ERR_OK)
	    {
		    HxLOG_Error("ERROR!!! Antenna: [%s] Get TP List fail!!\n", pstAntInfo->satType);
    		return	ONDK_RET_FAIL;
	    }
        ulAllTPCount += ulTpCount;

       	if (pstTuneInfo != NULL)
    	{
    		NX_ANTENNA_FreeTpListArray(pstTuneInfo);
    		pstTuneInfo = NULL;
    	}
    }

	pstAllScanParams = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * ulAllTPCount);

	if (pstAllScanParams == NULL)
	{
		NX_APP_Error("ERROR!! NX_APP_Calloc() Fail!!! \n");
		return	ONDK_RET_FAIL;
	}


	for (ulAntIndex = 0; ulAntIndex < ulAntCount; ulAntIndex++)
	{
  	    pstAntInfo = &(pstContents->stAntInfoArray[ulAntIndex]);

		if ((hError != ERR_OK) || (HxSTD_StrCmp(pstAntInfo->satName, (char*)NX_SAT_SATNAME_NONE) == 0))
		{
			HxLOG_Error("ERROR!! Invlid Satellite Name is NONE index:(%d), uid:(0x%x)!! \n", i, lUid);
			continue;
		}

		// Get TP Info
		HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
        ulTpCount = 0;
    	hError = NX_ANTENNA_GetTpListBySatelliteTypeName(pstAntInfo->satType, &ulTpCount, &pstTuneInfo);

		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!! NX_ANTENNA_GetUserTPInfo() index:(%d)!!! \n", ulAntIndex);
			continue;
		}

        for (i = 0; i < ulTpCount; i++)
    	{
    		pstAllScanParams[ulNextTPIndex].tunerType = eDxDELIVERY_TYPE_SAT;
    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.antennaId =pstAntInfo->uid;
    		HxSTD_snprintf(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", pstAntInfo->satType);
    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.startFreq = pstTuneInfo[i].sat.ulFrequency;
    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.endFreq = pstTuneInfo[i].sat.ulFrequency;
    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.modulationModes = pstTuneInfo[i].sat.ePskMod;

    		if (pstTuneInfo[i].sat.eTransSpec == eDxSAT_TRANS_DVBS)
    		{
    			pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
    		}

    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.polarisation = pstTuneInfo[i].sat.ePolarization;
    		pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.networkId = 0;
    		HxSTD_snprintf(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstTuneInfo[i].sat.ulSymbolRate));

    		switch (pstTuneInfo[i].sat.eFecCodeRate)
    		{
    			case eDxSAT_CODERATE_1_2:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));	break;
    			case eDxSAT_CODERATE_2_3:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));	break;
    			case eDxSAT_CODERATE_3_4:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));	break;
    			case eDxSAT_CODERATE_5_6:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));	break;
    			case eDxSAT_CODERATE_7_8:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));	break;
    			case eDxSAT_CODERATE_3_5:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));	break;
    			case eDxSAT_CODERATE_4_5:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));	break;
    			case eDxSAT_CODERATE_5_11:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));	break;
    			case eDxSAT_CODERATE_6_7:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));	break;
    			case eDxSAT_CODERATE_8_9:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));	break;
    			case eDxSAT_CODERATE_9_10:	HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));	break;
    			default:					HxSTD_StrNCpy(pstAllScanParams[ulNextTPIndex].data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));	break;
    		}
            ulNextTPIndex++;
    	}

        if (pstTuneInfo != NULL)
        {
    		NX_ANTENNA_FreeTpListArray(pstTuneInfo);
    		pstTuneInfo = NULL;
    	}
	}


	HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	// Get Search Option
	NX_SEARCH_GetSearchOption(&stScanOption);

    stScanOption.enableNetworkScan = FALSE;
    stScanOption.channelType = 100;
    stScanOption.isReplaceChannel = TRUE;

	pstContents->pstSearchedTPList = HLIB_VECTOR_New(NULL, NULL);

	hError = NX_Builder_StartScan(NX_SCANTYPE_NORMAL, &stScanOption, ulAllTPCount, pstAllScanParams);

	if (pstAllScanParams)
	{
		NX_APP_Free(pstAllScanParams);
		pstAllScanParams = NULL;
	}

	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_StartScan() - NX_SCANTYPE_NORMAL Fail !! \n");
		return	ONDK_RET_FAIL;
	}

    pstContents->eSearchState = eInstall_Search_searching;

    HxLOG_Error("\n nx_install_search_Progress_DoChSearching TEST4 \n");
	NX_COMMON_NOT_USED_PARAM(pstContents);
    return	ONDK_RET_OK;
}

static ONDK_Result_t nx_install_search_Progress_DoSearchingSatelliteSignal(Nx_Install_Search_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HERROR			hError = ERR_FAIL;
	HUINT32			uIndex = 0;
	HUINT32		    ulTpCount = 0, ulAntCount = 0;
    HINT32          lUid = -1;
	DxAntInfo_t			stAntInfo;
	Nx_TpSelectInfo_t	stTpInfo;

    NX_SCANPARAMS_t 	*pstParam  = NULL;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;
    HINT32 pTpCount = 0;
    NX_SCANOPTIONS_t	stScanOption;


    // check Antena Dual

    HxLOG_Error("\n\n####### nx_install_search_Progress_DoSearchingSatelliteSignal ########\n\n");

    HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
    HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	hError = NX_Builder_StopScan(NX_SCANTYPE_ANTENNACONNECTION);

	// Load Default Antenna Info
    hError = NX_ANTENNA_Refresh();

	ulAntCount = NX_ANTENNA_Count();
    HxLOG_Error("\n\n####### ulAntCount (%d) ########\n\n",ulAntCount);

    if(ulAntCount < 1)
    {
        //No Antena How to Set???
        HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		NX_ANTENNA_MakeDefaultAntennaInfo(eDxANT_TYPE_LNB_ONLY, &stAntInfo);
		stAntInfo.uid = eNxSat_ANTENAA_SLOT1;
		NX_ANTENNA_SetInfo(eNxSat_ANTENAA_SLOT1, &stAntInfo);
        ulAntCount = 1;
    }

	for (uIndex= 0;uIndex < ulAntCount; uIndex++)
	{
//	    lUid = NX_ANTENNA_GetId(i); // molink_ksjung_0151029
        NX_ANTENNA_GetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1+uIndex, &lUid);
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
        hError = NX_ANTENNA_GetInfo(lUid, &stAntInfo);
        if ((hError == ERR_OK)	&& (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) != 0))
		{
			HxLOG_Error("Antenna Setting!! [%d], Uid:[%d], Name:[%s]\n", uIndex, lUid, stAntInfo.satName);
            HxLOG_Error("\nAntenna Setting!! [%d], Uid:[%d], antType:[%d]\n", uIndex, lUid, stAntInfo.antType);
//            stAntInfo.info.diseqc.ulLnbFreq // molink_ksjung_0151029
		}
	}

    //Get TP LIST
	pTpCount = 0;
	hError = NX_ANTENNA_GetTpListBySatelliteTypeName(stAntInfo.satType, &ulTpCount, &pstTuneInfo);

    if (hError != ERR_OK)
	{
		HxLOG_Error("ERROR!!! Antenna: [%s] Get TP List fail!!\n", stAntInfo.satType);
		return	ONDK_RET_FAIL;
	}

    HxLOG_Error("NX_ANTENNA_GetTpListBySatelliteTypeName [%d] \n", hError);
    HxLOG_Error("\nERROR!!! Tp NUM [%u] \n", ulTpCount);

    pstParam = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * ulTpCount);

    for (uIndex = 0; uIndex< ulTpCount; uIndex++)
	{
		pstParam[uIndex].tunerType = eDxDELIVERY_TYPE_SAT;
		pstParam[uIndex].data.stDVBSScanParams.antennaId = stAntInfo.uid;
		HxSTD_snprintf(pstParam[uIndex].data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", stAntInfo.satType);

		pstParam[uIndex].data.stDVBSScanParams.startFreq = pstTuneInfo[uIndex].sat.ulFrequency;
		pstParam[uIndex].data.stDVBSScanParams.endFreq = pstTuneInfo[uIndex].sat.ulFrequency;
		pstParam[uIndex].data.stDVBSScanParams.modulationModes = pstTuneInfo[uIndex].sat.ePskMod;

		if (pstTuneInfo[uIndex].sat.eTransSpec == eDxSAT_TRANS_DVBS)
		{
			pstParam[uIndex].data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
		}

		pstParam[uIndex].data.stDVBSScanParams.polarisation = pstTuneInfo[uIndex].sat.ePolarization; //eDxSAT_POLAR_AUTO
		pstParam[uIndex].data.stDVBSScanParams.networkId = 0;

		HxSTD_snprintf(pstParam[uIndex].data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstTuneInfo[uIndex].sat.ulSymbolRate));

		switch (pstTuneInfo[uIndex].sat.eFecCodeRate)
		{
			case eDxSAT_CODERATE_1_2:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));	break;
			case eDxSAT_CODERATE_2_3:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));	break;
			case eDxSAT_CODERATE_3_4:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));	break;
			case eDxSAT_CODERATE_5_6:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));	break;
			case eDxSAT_CODERATE_7_8:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));	break;
			case eDxSAT_CODERATE_3_5:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));	break;
			case eDxSAT_CODERATE_4_5:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));	break;
			case eDxSAT_CODERATE_5_11:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));	break;
			case eDxSAT_CODERATE_6_7:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));	break;
			case eDxSAT_CODERATE_8_9:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));	break;
			case eDxSAT_CODERATE_9_10:	HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));	break;
			default:					HxSTD_StrNCpy(pstParam[uIndex].data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));	break;
		}
	}

	if (pstTuneInfo != NULL)
	{
		NX_ANTENNA_FreeTpListArray(pstTuneInfo);
		pstTuneInfo = NULL;
	}

	// Get Search Option
	NX_SEARCH_GetSearchOption(&stScanOption);

    // Start Scan
    hError = NX_Builder_StartScan(NX_SCANTYPE_ANTENNACONNECTION, &stScanOption, ulTpCount, pstParam);

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
    pstContents->eSearchState = eInstall_Search_searchingSattllite;
    return eResult;
}


ONDK_Result_t	nx_install_search_Progress_DoSavingChannel(Nx_Install_Search_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
	HxLOG_Error("nx_install_search_Progress_DoSavingChannel!! \n");
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

   	// Stop Search
    NX_Builder_StopScan(NX_SCANTYPE_NORMAL);

    //Save Channel
    if(eInstall_Search_searchFinish == pstContents->eSearchState)
    {
        pstContents->eSearchState = eInstall_Search_saveResult;
        nx_install_ui_SetProcessingPopupUI(TRUE,NULL);
        ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_SEARCH_ANIMATION_TIMER_PERIOD);

        if(pstContents->ulTvServiceNum>0 || pstContents->ulRadioServiceNum>0 )
        {
            nx_install_search_Progress_DoLoadServiceForSaveData(pstContents);
        }
        else
        {
            //Do Next Step
            nx_install_search_Progress_MsgCoreChannelStateUpdate(pstContents,0,0,0);
        }
    }
    return eResult;
}

static ONDK_Result_t	nx_install_search_Progress_AddServiceNameToList(Nx_Install_Search_Context_t *pstContents, DxSvcType_e eSvcType, HUINT8 *szSvcName)
{
	HUINT32		ulStrLen = 0, ulNewListSize = 0;
	HUINT8		*szDstSvcName = NULL, **pszSvcNameList = NULL, ucType = 0;
	if (eSvcType <= eDxSVC_TYPE_All || eSvcType >= eDxSVC_TYPE_RFVOD)
	{
		HxLOG_Error("[%s:%d] ERROR!! GWM_RET_OUT_OF_RANGE eSvcType [%d] !!! \n", __FUNCTION__, __LINE__, eSvcType);
		return	GWM_RET_OUT_OF_RANGE;
	}

	if (szSvcName == NULL)
	{
		HxLOG_Error("[%s:%d] ERROR!! szSvcName is NULL !!! \n", __FUNCTION__, __LINE__);
		return GWM_RET_TARGET_NULL;
	}
	ucType = (eSvcType == eDxSVC_TYPE_TV) ?	NX_PRISM_INSTALL_SEARCH_SVCTYPE_TV : NX_PRISM_INSTALL_SEARCH_SVCTYPE_RADIO ;

	HxLOG_Error("\n\n nx_install_search_Progress_AddServiceNameToList TEST 2 ucType(%d) aulNumSvc(%d) aulSvcNameListSize(%d) \n",ucType,pstContents->aulNumSvc[ucType]
        ,pstContents->aulSvcNameListSize[ucType] );

	if (pstContents->aulNumSvc[ucType] >= pstContents->aulSvcNameListSize[ucType])
	{
		ulNewListSize = pstContents->aulSvcNameListSize[ucType] + 1000;
		pszSvcNameList = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulNewListSize);

		if (pszSvcNameList == NULL)
		{
			HxLOG_Error("[%s:%d] ERROR!! NX_APP_Malloc() Fail !!! \n", __FUNCTION__, __LINE__);
			return	GWM_RET_OUT_OF_MEMORY;
		}

		HxSTD_memset(pszSvcNameList, 0, sizeof(HUINT8 *) * ulNewListSize);

		if (pstContents->pszSvcNameList[ucType] != NULL)
		{
			HxSTD_memcpy(pszSvcNameList, pstContents->pszSvcNameList[ucType], sizeof(HUINT8 *) * pstContents->aulNumSvc[ucType]);
			NX_APP_Free (pstContents->pszSvcNameList[ucType]);
		}

		pstContents->pszSvcNameList[ucType] = pszSvcNameList;
		pstContents->aulSvcNameListSize[ucType] = ulNewListSize;
	}

	ulStrLen = HxSTD_StrLen((HCHAR*)szSvcName) + 1;
	szDstSvcName = NX_APP_Malloc(ulStrLen);
	if (szDstSvcName == NULL)
	{
		HxLOG_Error("[%s:%d] ERROR!! NX_APP_Malloc() Fail !!! \n", __FUNCTION__, __LINE__);
		return	GWM_RET_OUT_OF_MEMORY;
	}

	HxSTD_StrNCpy((HCHAR*)szDstSvcName, (HCHAR*)szSvcName, ulStrLen - 1);
	szDstSvcName[ulStrLen-1] = '\0';

	pstContents->pszSvcNameList[ucType][pstContents->aulNumSvc[ucType]] = szDstSvcName;
	pstContents->aulNumSvc[ucType]++;
	pstContents->aulTotalSvc[ucType]++;

	HxLOG_Error("\nAdd Service : eSvcType[%d], szDstSvcName[%s]\n", eSvcType, szDstSvcName);

	return	ONDK_RET_OK;
}

static HUINT32	nx_install_search_Progress_CountServiceName(Nx_Install_Search_Context_t *pstContents, DxSvcType_e eSvcType)
{
	HUINT8		ucType = 0;
	HUINT32		ulCount = 0, ulSvcNum = 0;

	if (eSvcType == eDxSVC_TYPE_All)
	{
		ulSvcNum = 0;

		for (ulCount = 0; ulCount < NX_PRISM_INSTALL_SEARCH_SVCTYPE_NUM; ulCount++)
		{
			ulSvcNum += pstContents->aulNumSvc[ulCount];
		}

		return	ulSvcNum;

	}
	else if ((eSvcType == eDxSVC_TYPE_TV) || (eSvcType == eDxSVC_TYPE_RADIO))
	{
		ucType = (eSvcType == eDxSVC_TYPE_TV) ? NX_PRISM_INSTALL_SEARCH_SVCTYPE_TV : NX_PRISM_INSTALL_SEARCH_SVCTYPE_RADIO ;
		return	pstContents->aulNumSvc[ucType];
	}

	return	0;
}

static HUINT8**		nx_install_search_Progress_GetServiceNameList(Nx_Install_Search_Context_t *pstContents, DxSvcType_e eSvcType)
{
	HUINT8	ucType = 0;

	if ((eSvcType == eDxSVC_TYPE_TV) || (eSvcType == eDxSVC_TYPE_RADIO))
	{
		ucType = (eSvcType == eDxSVC_TYPE_TV) ? NX_PRISM_INSTALL_SEARCH_SVCTYPE_TV : NX_PRISM_INSTALL_SEARCH_SVCTYPE_RADIO ;
		return	pstContents->pszSvcNameList[ucType];
	}

	return NULL;
}


static ONDK_Result_t    nx_install_search_Progress_UpdateServiceInfo(Nx_Install_Search_Context_t *pstContents)
{
	HUINT8			*pStrTVTitle = NULL;
	HUINT8          *pStrRadioTitle = NULL;
	HUINT32			ulTvSvcNum = 0, ulRadioSvcNum = 0;
	HUINT8			**pszSvcNameList = NULL;
	HUINT32         ulId = 0;
	HINT32          ulIndex =0;
	const HCHAR*    pStrTitleTemplet = "%s : %d";

	ulTvSvcNum = pstContents->ulTvServiceNum;
	ulRadioSvcNum = pstContents->ulRadioServiceNum;

	// TV - TEXT
	pStrTVTitle = HxSTR_MemAlloc(sizeof(HUINT8)*NX_TEXT_SIZE_32);
	HxSTD_MemSet(pStrTVTitle, 0x00, NX_TEXT_SIZE_32);
	HxSTD_snprintf((char*)pStrTVTitle, NX_TEXT_SIZE_32,pStrTitleTemplet ,(char*)ONDK_GetString(RES_TV_ID),(int)ulTvSvcNum);
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_TV_TITLE_ID);
	ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*) pStrTVTitle);

	// RADIO - TEXT
	pStrRadioTitle = HxSTR_MemAlloc(sizeof(HUINT8)*NX_TEXT_SIZE_32);
	HxSTD_MemSet(pStrRadioTitle, 0x00, NX_TEXT_SIZE_32);
	HxSTD_snprintf((char*)pStrRadioTitle, NX_TEXT_SIZE_32,pStrTitleTemplet ,(char*)ONDK_GetString(RES_RADIO_ID),(int)ulRadioSvcNum);
	ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_TITLE_ID);
	ONDK_GWM_COM_Text_SetText(ulId, (HCHAR*)pStrRadioTitle);

	// TV List
	if (ulTvSvcNum > 0)
	{
		pszSvcNameList = nx_install_search_Progress_GetServiceNameList(pstContents, eDxSVC_TYPE_TV);

		for (ulIndex = 0; ulIndex < NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT; ulIndex++)
		{
			if (ulTvSvcNum < NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT)
			{
				ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_ID) + ulIndex;
				ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*)pszSvcNameList[ulIndex]);
			}
#if 0
			else
			{
				ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_TV_INFO_ID) + (NX_PRISM_INSTALL_SEARCH_TV_INFO_LIMIT - (ulIndex+1));
				ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*)pszSvcNameList[((ulTvSvcNum-1)-ulIndex)]);
			}
#endif
		}
	}

	// Radio List
	if (ulRadioSvcNum > 0)
	{
		pszSvcNameList	= nx_install_search_Progress_GetServiceNameList(pstContents, eDxSVC_TYPE_RADIO);
		for (ulIndex = 0 ; ulIndex < NX_PRISM_INSTALL_SEARCH_RADIO_INFO_LIMIT; ulIndex++)
		{
			if (ulIndex < NX_PRISM_INSTALL_SEARCH_RADIO_INFO_LIMIT)
			{
				ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_ID) + ulIndex;
				ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*)pszSvcNameList[ulIndex]);
			}
#if 0
			else
			{
				ulId = (NX_PRISM_INSTALL_SEARCH_TEXT_RADIO_INFO_ID) + (NX_PRISM_INSTALL_SEARCH_RADIO_INFO_LIMIT - (ulIndex+1));
				ONDK_GWM_COM_Text_SetText(ulId,(HCHAR*)pszSvcNameList[((ulRadioSvcNum-1)-ulIndex)]);
			}
#endif
		}
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_install_search_Progress_MsgCoreAntennaConnectStateUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HERROR	        		hError = ERR_FAIL;
	HINT32					lProgress = p2;
	NX_BUILDER_Status_e		eBuilderStatus = p1;
	DxAntennaConnection_e	eConnectionType = eDxANTCONN_ONECABLE;

	if (pstContents == NULL)
	{
		HxLOG_Error("Error!!! pstContents is NULL !! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	HxLOG_Error("===> Antenna Connect State p1:(0x%x), p2:(0x%x), p3:(0x%x)!! ===\n", p1, p2, p3);

	if ((lProgress == 100) || (eBuilderStatus == eBUILDER_COMPLETE))
	{
		eConnectionType = (DxAntennaConnection_e)p3;
		HxLOG_Error("=== Antenna Connect Complete!!!  eConnectionType:(%d)!! ===\n", eConnectionType);
       	HxLOG_Error("=== Stop Scan  NX_SCANTYPE_ANTENNACONNECTION ===\n");
    	hError = NX_Builder_StopScan(NX_SCANTYPE_ANTENNACONNECTION);

		if (pstContents->pstSearchedTPList)
		{
			HLIB_VECTOR_Delete(pstContents->pstSearchedTPList);
			pstContents->pstSearchedTPList = NULL;
		}

        if(eDxANTCONN_ONECABLE == eConnectionType)
        {
        	HxLOG_Error("=== Antenna Connect Complete!!!  ONECABLE: ===\n");
            pstContents->eSearchState = eInstall_Search_processEnd;
        }
        else
        {
            //Dual
    		HxLOG_Error("=== Antenna Connect Complete!!!  Dual eConnectionType:(%d)!! ===\n", eConnectionType);
            pstContents->eSearchState = eInstall_Search_processEnd;
        }
        NX_SYSTEM_SetAntennaConnection(eConnectionType);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_search_Progress_MsgCoreSearchInfoUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HERROR			hError = ERR_FAIL;
	HINT32			lStartIndex = 0, lResultNo = 0, i = 0;
	HUINT32 		ulTotalTuningNum = 0, ulCompleteTpNum = 0, ulSearchedServiceNum = 0, ulSearchState = 0, ulFrequency = 0, ulProgress = 0;
	HUINT32			ulTvSvcNum = 0, ulRadioSvcNum = 0;
	NX_BUILDER_Status_e	eBuilderStatus = p1;
	DxDeliveryType_e	eTunerType = eDxDELIVERY_TYPE_ALL;
	DxTuneParam_t		stTpInfo, *pstSearchedTp = NULL;
	DxBuilder_SearchedInfo_t	*pstDetailInfo = NULL;

	if (pstContents == NULL)
	{
		HxLOG_Error("[%s:%d] ERROR!! pstContents is NULL!!! \n", __FUNCTION__, __LINE__);
		return	GWM_RET_SOURCE_NULL;
	}

	hError = NX_SEARCH_GetSearchTPInfo(&ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[%s:%d] ERROR!! NX_SEARCH_GetSearchTPInfo() !!! \n", __FUNCTION__, __LINE__);
	}

	if (ulFrequency < 1)
	{
		HxLOG_Error("[%s:%d] ERROR!! Invalid Searching Info!!! \n", __FUNCTION__, __LINE__);
		return	ONDK_RET_INVARG;
	}

	lStartIndex = nx_install_search_Progress_CountServiceName(pstContents, eDxSVC_TYPE_All);

	if (lStartIndex > 0)
	{
		lStartIndex = lStartIndex - 1;
	}

	hError = NX_SEARCH_GetSearchServiceListInfo(lStartIndex, ulSearchedServiceNum, &lResultNo, &pstDetailInfo);

	HxLOG_Error("\n\n[%s:%d] lResultNo (%d) hError(%d) \n", __FUNCTION__, __LINE__,lResultNo,hError);

	if ((hError == ERR_OK) && (lResultNo > 0) && pstDetailInfo)
	{
		HxLOG_Error("\n[%s] NX_SEARCH_GetSearchServiceListInfo() - lStartIndex[%d], lResultNo[%d] \n", __FUNCTION__, lStartIndex, lResultNo);
		ulTvSvcNum = 0;
		ulRadioSvcNum = 0;
		for (i = 0; i < lResultNo; i++)
		{
			if (pstDetailInfo[i].ulSvcType == eDxSVC_TYPE_TV)
			{
				NX_APP_Info("=====>[JHLEE] TV SVC[%d] : (%s) \n", ulTvSvcNum, pstDetailInfo[i].szSvcName);
				ulTvSvcNum++;
				nx_install_search_Progress_AddServiceNameToList(pstContents, pstDetailInfo[i].ulSvcType, pstDetailInfo[i].szSvcName);
			}
			else if (pstDetailInfo[i].ulSvcType == eDxSVC_TYPE_RADIO)
			{
				NX_APP_Info("=====>[JHLEE] Radio SVC[%d] : (%s) \n", ulRadioSvcNum, pstDetailInfo[i].szSvcName);
				ulRadioSvcNum++;
				nx_install_search_Progress_AddServiceNameToList(pstContents, pstDetailInfo[i].ulSvcType, pstDetailInfo[i].szSvcName);
			}
			else
			{
				NX_APP_Error("ERROR!! Invalid Service Type : [0x%x]!!! \n", pstDetailInfo[i].ulSvcType);
			}
		}
		if (pstDetailInfo)
		{
			NX_SEARCH_FreeSearchServiceListInfo(pstDetailInfo);
			pstDetailInfo = NULL;
		}

		// Add Searched TP List
		if (pstContents->pstSearchedTPList)
		{
			pstSearchedTp = NX_APP_Calloc(sizeof(DxTuneParam_t));
			if (pstSearchedTp)
			{
				HxSTD_memset(pstSearchedTp, 0x00, sizeof(DxTuneParam_t));
				HxSTD_memcpy(pstSearchedTp, &stTpInfo, sizeof(DxTuneParam_t));
				HLIB_VECTOR_Add(pstContents->pstSearchedTPList, (void*)pstSearchedTp);
				NX_APP_Info("HLIB_VECTOR_Add()\n");
			}
			else
			{
				NX_APP_Error("ERROR!!! NX_APP_Calloc() Faile!!! \n");
			}
		}
	}
	else
	{
		HxLOG_Error("\n[%s] ERROR !! NX_SEARCH_GetSearchServiceListInfo()  Not found Service lStartIndex[%d], lResultNo[%d]!!! \n", __FUNCTION__, lStartIndex, lResultNo);
	}
	pstContents->ulTotalTuningNum = ulTotalTuningNum;
	pstContents->ulCompleteTpNum = ulCompleteTpNum;
	pstContents->ulTvServiceNum += ulTvSvcNum;
	pstContents->ulRadioServiceNum += ulRadioSvcNum;

	pstContents->ulSearchedServiceNum = (pstContents->ulTvServiceNum + pstContents->ulRadioServiceNum);
	NX_APP_Debug("=== ulSearchedServiceNum:(%d), ulTvServiceNum:(%d), ulRadioServiceNum:(%d)\n", pstContents->ulSearchedServiceNum, pstContents->ulTvServiceNum, pstContents->ulRadioServiceNum);

	if (pstContents->ulTotalTuningNum > 0)
	{
		ulProgress = ((pstContents->ulCompleteTpNum * 100) / pstContents->ulTotalTuningNum);

		if (ulProgress == 100)
		{
			pstContents->eSearchState = eInstall_Search_searchFinish;
			NX_APP_Info("=== Search Complete!!!  eSearchUi_state_searchFinish!! ===\n");
		}
	}

	if (eBuilderStatus == eBUILDER_COMPLETE)
	{
		HxLOG_Error("=== Search Complete!!!  eBuilderStatus = eBUILDER_COMPLETE!! ===\n");
		pstContents->eSearchState = eInstall_Search_searchFinish;
	}

	NX_PRISM_UTIL_MakeTpStringByTuneParamAndTunerType(eTunerType, &stTpInfo, pstContents->szTpInfoString);
    nx_install_search_Progress_UpdateProgressLevel(pstContents,ulProgress);
	nx_install_search_Progress_UpdateServiceInfo(pstContents);

	if (pstDetailInfo)
	{
		NX_SEARCH_FreeSearchServiceListInfo(pstDetailInfo);
		pstDetailInfo = NULL;
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;


}

static ONDK_Result_t	nx_install_search_Progress_MsgGwmCreate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

    HxLOG_Error("\n\n####### nx_Install_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_search_Progress_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	nx_install_search_Progress_DrawDefaultUI(pstContents);

	// Add Noti
	NX_Builder_AddNotiSignalInfo();

    nx_install_search_Progress_DoChSearching(pstContents);

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


static ONDK_Result_t	nx_install_search_Progress_MsgGwmDestroy(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HxLOG_Error("\n\n####### nx_Install_MsgGwmDestroy #######\n\n");

	if (pstContents->pstSubItems)
	{
   		NX_APP_Free(pstContents->pstSubItems);
	}
    NX_Builder_ReleaseNotiSignalInfo();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_search_Progress_MsgGwmHapiMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_install_search_Progress_MsgGwmTimerMsg(Nx_Install_Search_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

    if (p1 == NX_PRISM_INSTALL_ANIMATION_TIMER_ID)
    {
        //UI Timely Update
        if(eInstall_Search_processEnd != pstContents->eSearchState)
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
        }

        //check UI Next Step
        switch(pstContents->eSearchState)
        {
            case eInstall_Search_processEnd:
                 ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                 nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                 eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
                 if(ONDK_RET_OK == eResult)
                 {
                    ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_Install_Search_Progress_Proc);
                 }
                 break;
            default:
               break;
        }
    }

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_install_search_Progress_MsgGwmKeyDown(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
    HINT32      lObjId;
    NxInstall_SearchState_t eSearchState; //current status

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

    lObjId = GWM_GetFocusedObjectID();
    eSearchState = pstContents->eSearchState;

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
            {
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
                switch(eSearchState){
                    case eInstall_Search_searchFinish:
                        nx_install_search_Progress_DoSavingChannel(pstContents);
                        break;
                    case eInstall_Search_init:
                    case eInstall_Search_searching:
                        {
                            switch(lObjId)
                            {
                                case NX_PRISM_INSTALL_SEARCH_BTN_STOP_ID:
                                    if(eInstall_Search_searching == eSearchState)
                                    {
                                    	NX_Builder_ControlScan(NX_SCANTYPE_PAUSE);
                                        pstContents->eSearchState = eInstall_Search_pause;
                                    }
                                    nx_install_search_Progress_DoSelectivePopup(pstContents, TRUE);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case eInstall_Search_pause:
                        {
                            switch(lObjId)
                            {
                                case NX_PRISM_INSTALL_SEARCH_BTN_POPUP_YES_ID:
                                    if((eInstall_Search_searching == pstContents->eSearchState) ||
                                        (eInstall_Search_pause == pstContents->eSearchState))
                                    {
                                        NX_Builder_StopScan(NX_SCANTYPE_NORMAL);
                                    }
                                    pstContents->eSearchState = eInstall_Search_searchFinish;
                                    nx_install_search_Progress_DoSelectivePopup(pstContents, FALSE);
                                    nx_install_search_Progress_DoSavingChannel(pstContents);
                                    break;
                                case NX_PRISM_INSTALL_SEARCH_BTN_POPUP_NO_ID:
                                    if(eSearchState == eInstall_Search_pause)
                                    {
                                    	NX_Builder_ControlScan(NX_SCANTYPE_RESUME);
                                        pstContents->eSearchState = eInstall_Search_searching;
                                    }
                                    nx_install_search_Progress_DoSelectivePopup(pstContents, FALSE);
                                    break;
                                default:
                                    break;
                            }

                        }
                        break;
                    default:
                        break;
                }
             }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
        case KEY_ARROWUP:
        case KEY_ARROWDOWN:
            break;
		case KEY_STANDBY:
            if((eInstall_Search_searching == pstContents->eSearchState) ||
                (eInstall_Search_pause == pstContents->eSearchState))
            {
                HxLOG_Error("\n\n NX_Builder_StopScan NX_SCANTYPE_NORMAL \n\n");
                NX_Builder_StopScan(NX_SCANTYPE_NORMAL);
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

static ONDK_Result_t	nx_install_search_Progress_MsgCoreChannelStateUpdate(Nx_Install_Search_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lSearchedTplength = 0;
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HERROR			hError = ERR_FAIL;
    HBOOL       bCheckAntennaType = FALSE;

	if (pstContents == NULL)
	{
		NX_APP_Error("[%s:%d] ERROR!! pstContents is NULL!!! \n", __FUNCTION__, __LINE__);
		return	GWM_RET_SOURCE_NULL;
	}

	nx_install_search_Progress_DoSaveServiceForSaveData(pstContents);
	// Stop Search
	hError = NX_Builder_StopScan(NX_SCANTYPE_NORMAL);

	// Delete Saving process
    ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
    nx_install_ui_SetProcessingPopupUI(FALSE,NULL);

#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstContents->ulTunerCount > 1) //Single Tuner?
	{
		if (pstContents->pstSearchedTPList)
		{
			lSearchedTplength = HLIB_VECTOR_Length(pstContents->pstSearchedTPList);
			HxLOG_Error("Searched TP List Num : (%u)\n", lSearchedTplength);
			if (lSearchedTplength > 0) // need to check TP NUM?
			{
				// Check Antenna Connection
                nx_install_ui_SetProcessingPopupUI(TRUE, (char*)ONDK_GetString(RES_MESG_4212_ID));
                ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_SEARCH_ANIMATION_TIMER_PERIOD);
                eResult = nx_install_search_Progress_DoSearchingSatelliteSignal(pstContents);
				if (eResult != ONDK_RET_OK)
				{
					pstContents->eSearchState= eInstall_Search_processEnd;
				}
                bCheckAntennaType = TRUE;
			}
		}
	}
#else
	NX_SYSTEM_SetAntennaConnection(eDxANTCONN_ONECABLE);
#endif
    if( FALSE == bCheckAntennaType)
    {
		// Set eDxANTCONN_ONECABLE if 1 tuner box
		ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_SEARCH_ANIMATION_TIMER_PERIOD);
		NX_SYSTEM_SetAntennaConnection(eDxANTCONN_ONECABLE);
    	pstContents->eSearchState= eInstall_Search_processEnd;
    }

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Search_Progress_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Search_Context_t	*pstContents = NULL;
	pstContents = nx_install_search_Progress_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_search_Progress_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_install_search_Progress_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_search_Progress_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_install_search_Progress_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
           	eResult = nx_install_search_Progress_MsgGwmTimerMsg(pstContents, handle, p1, p2, p3);
            break;
		case MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION:
			HxLOG_Error("\nMSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION!\n");
			eResult = nx_install_search_Progress_MsgCoreAntennaConnectStateUpdate(pstContents, p1, p2, p3);
			break;
		case MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE:
			HxLOG_Error("\nMSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE!\n");
            nx_install_search_Progress_MsgCoreSearchInfoUpdate(pstContents, p1, p2, p3);
			break;
		case MSG_CORE_CHANNEL_EVT_STATE_UPDATED:
			HxLOG_Error("\nMSG_CORE_CHANNEL_EVT_STATE_UPDATED!\n");
            nx_install_search_Progress_MsgCoreChannelStateUpdate(pstContents, p1, p2, p3);
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

