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

#ifdef CONFIG_MW_INET_WLAN

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_prism_install_sub_network.h>
#include <nx_prism_install_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID 	    ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+200)
#define	NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_ID ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+250)
#define	NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_ID ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+251)

// WIFI LIST
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_X			254
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_Y			220
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_W			773
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_H			420 //60 * 7
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_H       	55
#define NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_GAP     	5

#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_OBJ_X     	15
#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_OBJ_Y     	15

#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_X     	694
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_Y     	13
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_W     	22
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_H     	28

#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_X     	725
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_Y     	13
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_W     	33
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_H     	29

#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_X     830
#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_Y     658
#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_W     198
#define NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_H     22

#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_X_GAP    10
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_Y      653 +(5)
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_W      26
#define NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_H      26

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Install_Network_Context_t		s_stInstallNetworkContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	                    nx_install_network_wifi_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	                    nx_install_network_wifi_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_network_wifi_MsgGwmHapiMsg(Nx_Install_Network_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_network_wifi_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_network_wifi_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Network_Context_t	*nx_install_network_wifi_GetContents(void)
{
	return	&s_stInstallNetworkContent;
}


static ONDK_Result_t nx_install_network_wifi_InitApplication (Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    HUINT8 ulNumOfSubPage = 0;


	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_Install_Network_Context_t));

    pstContents->ePage = eNxInstallPage_Network;

    eResult = nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ulNumOfSubPage);

    pstContents->pstSubItems = pstSubItemArray;
    pstContents->ulNumOfSubPage= ulNumOfSubPage;

    pstContents->lLoadingIndex = 0;
    pstContents->eUiState = eInstall_Network_INIT;

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

    //Loading Img
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_F_0] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI00_C_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_F_1] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI01_C_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_F_2] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI02_C_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_F_3] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI03_C_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_F_4] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI04_C_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_N_0] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI00_N_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_N_1] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI01_N_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_N_2] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI02_N_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_N_3] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI03_N_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_N_4] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI04_N_PNG);
    pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_LOCK] = ONDK_IMG_CreateSelf((HUINT8*)RES_211_00_LOCK_PNG);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_install_network_wifi_DrawWifiListUI(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDKFontHandle		hFontHandle;
	HINT32	i = 0;
	HINT32	x1 = 0, y1 = 0;
	HINT32	x = 0, y = 0, w = 0, h = 0;
	HINT32	lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lPageCount = 0, lLastIdx = 0;
	HINT32	lCellHeight = 0, lCursor_y_off = 0;

	HUINT8	*pDisable = NULL;
	HBOOL	cursorEnable = TRUE;
	LIST_OBJECT		objList = (LIST_OBJECT)object;
    APKS_NETAP_Info_t  *pstDataList;
	HINT32				textOffset_x = NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_OBJ_X, textOffset_y = NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_OBJ_Y;
    HINT32	CursorOffset =0;

	ONDK_Color_t		color_text = COL(C_G_Whistle_100);
	ONDK_Color_t		color_focusText = COL(C_G_Bang_100);
	ONDK_Color_t		color_disableText = COL(C_G_Bang_100);

	ONDK_Color_t		color_FocusCursor = COL(C_G_Base01_100);
    ONDK_Color_t		color_Cursor = COL(C_G_Gong02);
	ONDK_Color_t		title_fontColor = 0x00000000;
    Nx_Install_Network_Context_t *pstContents;

    pstContents = nx_install_network_wifi_GetContents();

    if(0 == pstContents->ulNumOfAp)
    {
    	return	ONDK_RET_FAIL;
    }

	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;


	pstDataList = (APKS_NETAP_Info_t *)ONDK_GWM_List_GetArrayPtr(objList);

	ONDK_GWM_List_GetProperty (objList, &lTotal, &lStart, &lCurrent, &lSelect, &lPageCount);
	lCellHeight = ONDK_GWM_List_GetCellHeight(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID);

	pDisable = ONDK_GWM_List_GetItemDisable(objList);
	ONDK_GWM_List_GetFontHandleObj(objList, &hFontHandle);

	if (hFontHandle == NULL)
	{
		return	ONDK_RET_FAIL;
	}

	if(lPageCount < lTotal)
	{
		lLastIdx = lPageCount;
	}
	else
	{
		lLastIdx = lTotal;
	}


	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);

	    //CURSOR
		if(TRUE == cursorEnable)
		{
			if (i + lStart == lCurrent)
			{
				title_fontColor = color_focusText;
				x = x1;
				y = y1 + lCursor_y_off;

				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - CursorOffset, color_FocusCursor);
			}
			else
			{
				title_fontColor = color_text;
				if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
				{
					title_fontColor = color_disableText;
				}
                x = x1;
				y = y1 + lCursor_y_off;
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - CursorOffset, color_Cursor);
			}
		}
		else
		{
			title_fontColor = color_text;
			if ((pDisable != NULL) && (pDisable[i + lStart] == 1))
			{
				title_fontColor = color_disableText;
			}
		}
        //TEXT
		x = x1 + textOffset_x;
		y = y1 + lCursor_y_off + textOffset_y;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pstApInfo->szESSID, x, y, w, title_fontColor, ONDKSTF_LEFT);

        //check next y point
		lCursor_y_off += lCellHeight+NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_GAP;
	}

    lCursor_y_off = 0;

    //LOCK IMG
	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);
        //LAST STEP is alywas SKIP THIS STEP
        if(eInet_Wifi_Sec_Open != pstApInfo->nSecureType && (lStart+i < (lTotal-1)))
        {
        	x = x1 + NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_X;
        	y = y1 + lCursor_y_off + NX_PRISM_INSTALL_NETWORK_WIFI_IMG_LOCK_Y;
            ONDK_IMG_Blit(pstContents->hWifiImgHandleList[eInstall_Network_WIFI_IMG_LOCK], screen, x,y);
        }
        //check next y point
		lCursor_y_off += lCellHeight+NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_GAP;
	}


    //WIFI Strength
    lCursor_y_off = 0;

	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);
        HINT32 strength = 0;
        NxInstall_Network_WIFI_IMG_ID_t imgID = eInstall_Network_WIFI_IMG_F_0;
        strength = pstApInfo->nWaveStrength / 20;

    	if (i + lStart == lCurrent)
		{
            switch(strength)
            {
                case 0:
                    imgID = eInstall_Network_WIFI_IMG_F_0;
                    break;
                case 1:
                    imgID = eInstall_Network_WIFI_IMG_F_1;
                    break;
                case 2:
                    imgID = eInstall_Network_WIFI_IMG_F_2;
                    break;
                case 3:
                    imgID = eInstall_Network_WIFI_IMG_F_3;
                    break;
                default:
                    imgID = eInstall_Network_WIFI_IMG_F_4;
                    break;
            }
		}
		else
		{
            switch(strength)
            {
                case 0:
                    imgID = eInstall_Network_WIFI_IMG_N_0;
                    break;
                case 1:
                    imgID = eInstall_Network_WIFI_IMG_N_1;
                    break;
                case 2:
                    imgID = eInstall_Network_WIFI_IMG_N_2;
                    break;
                case 3:
                    imgID = eInstall_Network_WIFI_IMG_N_3;
                    break;
                default:
                    imgID = eInstall_Network_WIFI_IMG_N_4;
                    break;
            }
		}

      	x = x1 + NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_X;
       	y = y1 + lCursor_y_off + NX_PRISM_INSTALL_NETWORK_WIFI_IMG_WIFI_POWER_Y;
        //LAST STEP is alywas SKIP THIS STEP
        if(lStart+i < (lTotal-1))
        {
            ONDK_IMG_Blit(pstContents->hWifiImgHandleList[imgID], screen, x,y);
        }

        //check next y point
		lCursor_y_off += lCellHeight+NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_GAP;
	}

	return ERR_OK;
}

static ONDK_Result_t	nx_install_network_wifi_DrawWifiList(Nx_Install_Network_Context_t *pstContents)
{
	ONDK_Rect_t	stRect;

    //WiFi list obj.
	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_X, NX_PRISM_INSTALL_NETWORK_WIFI_LIST_Y ,
	NX_PRISM_INSTALL_NETWORK_WIFI_LIST_W, NX_PRISM_INSTALL_NETWORK_WIFI_LIST_H);

	ONDK_GWM_List_Create(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID, &stRect, "WiFi LIST", NX_PRISM_INSTALL_NETWORK_WIFI_LIST_OBJ_H,
        pstContents->ulNumOfAp, 7,(GWM_LIST_ITEMS)pstContents->apList);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID, 0);
	ONDK_GWM_List_SetCircularMove(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID, FALSE);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID);
    ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID, (GWM_ObjDraw_t)nx_install_network_wifi_DrawWifiListUI);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_Wifi_DoFocusablePage(Nx_Install_Network_Context_t *pstContents)
{
    switch(pstContents->eUiState)
    {
        case eInstall_Network_INIT:
        case eInstall_Network_WIFI_RESPONSE_POPUP:
            ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID);
            ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID);
            break;
        default:
            ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_NETWORK_WIFI_LIST_ID);
            break;
    }
    return ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_network_wifi_DoSearchingWifi(Nx_Install_Network_Context_t *pstContents)
{
    //Progress Popup

    pstContents->eUiState = eInstall_Network_WIFI_POPUP;
    nx_install_network_Wifi_DoFocusablePage(pstContents);

    nx_install_ui_SetProcessingPopupUI(TRUE, (char*)ONDK_GetString(RES_SEARCHING_AVAILABLE_NETWORKS_ID));
    ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_ANIMATION_TIMER_PERIOD);
    NX_NETWORK_StartScan();
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_wifi_DoConnectAP(Nx_Install_Network_Context_t *pstContents, APKS_NETAP_Info_t * pstApInfo,HCHAR *keyboardRet)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

    pstContents->eUiState = eInstall_Network_WIFI_POPUP;
    nx_install_network_Wifi_DoFocusablePage(pstContents);

    //Progress Popup
    nx_install_ui_SetProcessingPopupUI(TRUE, (char*)ONDK_GetString(RES_CONNECTING_ID));
    ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_ANIMATION_TIMER_PERIOD);

    eResult = NX_NETWORK_ConnectAP(pstApInfo, keyboardRet);
    return eResult;
}

static ONDK_Result_t	nx_install_network_wifi_DrawHiddenNetworkBtn(Nx_Install_Network_Context_t *pstContents)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulID = 0;

    if(eInstall_Network_WIFI_HIDDEN_NETWORK== pstContents->eUiState)
    {
        ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_ID, FALSE);
        ONDK_GWM_Obj_SetVisible(NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_ID, FALSE);
    }
    else
    {
        ulID = (NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_ID);
    	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_X,
            NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_Y,
            NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_W,
            NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_H);

     	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, (char*)ONDK_GetString(RES_HIDDEN_NETWORK_ID));
        ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
     	ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
        ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_RIGHT);
       	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong02), COL(C_T_Gong02));
        ONDK_GWM_Obj_SetVisible(ulID, TRUE);

        //IMAGE
        ulID = (NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_ID);
    	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_WIFI_TEXT_HIDDEN_X-
            (NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_X_GAP+NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_W),
            NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_Y,
            NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_W,
            NX_PRISM_INSTALL_NETWORK_WIFI_IMG_OPTION_H);
        ONDK_GWM_COM_Image_Create(ulID, &stRect);
        ONDK_GWM_COM_Image_SetImage(ulID, RES_201_00_OPTION_PNG);
        ONDK_GWM_Obj_SetVisible(ulID, TRUE);
    }

    return eResult;
}

static ONDK_Result_t	nx_install_network_wifi_DrawDefaultUI(Nx_Install_Network_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT8          ulPageIndex = 0;
    HUINT8          ulNumofPages = 0;
    HUINT8          ulIndex = 0;
    Nx_InstallSubPage_t	*pstSubItem;

    for(ulIndex = 0; ulIndex < pstContents->ulNumOfSubPage ; ulIndex++)
    {
        pstSubItem = &(pstContents->pstSubItems[ulIndex]);
        if(pstSubItem->ePageName == pstContents->ePage){
            ulPageIndex = ulIndex;
            break;
        }
    }

    ulNumofPages = pstContents->ulNumOfSubPage;

    nx_install_ui_DrawBackgroundUI();
    nx_install_ui_DrawTitle((HUINT8*)ONDK_GetString(RES_NETWORK_CONNECTION_ID));
    nx_install_ui_DrawStepIndicatior(ulPageIndex);
    nx_install_ui_DrawExplain((HCHAR *)ONDK_GetString(RES_MESG_3721_ID));

    //Hidden Network
    nx_install_network_wifi_DrawHiddenNetworkBtn(pstContents);
    return eResult;
}

static ONDK_Result_t	nx_install_network_wifi_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

    HxLOG_Error("\n\n####### nx_install_network_wifi_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_network_wifi_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

    nx_install_network_wifi_DrawDefaultUI(pstContents);

    NX_NETWORK_RegisterNotifier();

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Wizard);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

    //Do Searching Available Networks
    nx_install_network_wifi_DoSearchingWifi(pstContents);

	return	eResult;
}


static ONDK_Result_t	nx_install_network_wifi_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32  lIndex = 0;

	for(lIndex = 0; lIndex<eInstall_Network_WIFI_IMG_MAX;lIndex++)
	{
		ONDK_IMG_Destroy(pstContents->hWifiImgHandleList[lIndex]);
	}

	if (pstContents->ulNumOfAp > 0)
	{
		if (pstContents->apList)
		{
			NX_APP_Free(pstContents->apList);
		}
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_network_wifi_MsgGwmHapiMsg(Nx_Install_Network_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_install_network_wifi_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_wifi_MsgGwmListSelected(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lListIdx = p2;
    ONDK_Result_t eResult = ONDK_RET_OK;
	APKS_NETAP_Info_t *apList = pstContents->apList;

    pstContents->lSelectedItemIndex = lListIdx;

    if(lListIdx == (HINT32)(pstContents->ulNumOfAp-1))
    {
        //SKIP THIS STEP
        eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
        if(ONDK_RET_OK == eResult)
        {
            ONDK_GWM_APP_Destroy(0);
        }
    }
    else
    {
        //check secure
        if(eInet_Wifi_Sec_Open!= apList[lListIdx].nSecureType)
        {
        	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_PASSWD, 0);
        }
        else
        {
            APKS_NETAP_Info_t * pstApInfo;
            pstApInfo = &(pstContents->apList[pstContents->lSelectedItemIndex]);
            nx_install_network_wifi_DoConnectAP(pstContents, pstApInfo,"");
        }
    }
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_network_wifi_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;
	HINT32      lObjId = -1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	lObjId = GWM_GetFocusedObjectID();

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

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			if(pstContents->eUiState == eInstall_Network_INIT)
			{
				eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, 0, 0, 0);
				if (eResult != ONDK_RET_OK)
				{
					NX_APP_Error("Error!!! nx_install_ui_DoNextStep() Fail!! \n");
				}
			    ONDK_GWM_APP_Destroy(0);
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
			switch(lObjId)
			{
				case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID:
					pstContents->eUiState = eInstall_Network_INIT;
					nx_install_network_Wifi_DoFocusablePage(pstContents);
					nx_install_ui_CloseSelectivePopupUI();
					return	ONDK_MESSAGE_BREAK;
				case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID:
					nx_install_ui_CloseSelectivePopupUI();
					pstContents->eUiState = eInstall_Network_INIT;
					nx_install_network_Wifi_DoFocusablePage(pstContents);
					nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, 0, 0, 0);
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID:
					nx_install_ui_CloseConfirmPopupUI();
					pstContents->eUiState = eInstall_Network_INIT;
					nx_install_network_Wifi_DoFocusablePage(pstContents);
					nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, 0, 0, 0);
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;
		case KEY_ARROWLEFT:
		    break;
		case KEY_ARROWRIGHT:
		    break;
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		case KEY_OPT:
			pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
			nx_install_network_Wifi_DoFocusablePage(pstContents);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_Network_Hidden_Proc", APP_DEFAULT_POPUP_PRIORITY,
			NX_PRISM_Install_Network_Hidden_Proc, HANDLE_NULL, 0, 0, 0);
			break;
		case KEY_STANDBY:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		default:
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_wifi_MsgGwmAPKMessage(Nx_Install_Network_Context_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e   eEvent = p1;
    APKS_NETAP_Info_t   *apList;
    HINT32 lNumOfAp      = p3;
    HCHAR   *skipStr = (HCHAR*)ONDK_GetString(RES_SKIP_THIS_STEP_ID);
    HCHAR   *strListTry[3] = {0,};
    HCHAR   *strListSuccess[2]  = {0,};

    strListTry[0] = (HCHAR*)ONDK_GetString(RES_MESG_360_ID);
    strListTry[1] = (HCHAR*)ONDK_GetString(RES_MESG_3211_ID);
    strListTry[2] = (HCHAR*)ONDK_GetString(RES_MESG_3212_ID);

    strListSuccess[0]=(HCHAR*)ONDK_GetString(RES_MESG_3208_ID);
    strListSuccess[1]="Press NEXT to continue.";

    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
            case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS \n\n");
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                pstContents->eUiState = eInstall_Network_WIFI_POPUP;
                nx_install_network_Wifi_DoFocusablePage(pstContents);
                nx_install_ui_OpenConfirmPopupUI(strListSuccess,2);
                break;
            case eNETWORK_STATUS_WIFI_KEY_INVALID:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_KEY_INVALID \n\n");
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                pstContents->eUiState = eInstall_Network_WIFI_RESPONSE_POPUP;
                nx_install_network_Wifi_DoFocusablePage(pstContents);
                nx_install_ui_OpenResponsePopupUI((char*)ONDK_GetString(RES_MESG_3999_ID));
                ONDK_GWM_SetTimer(NX_PRISM_INSTALL_RESPONSE_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_RESPONSE_TIMER_3SEC_PERIOD);
                break;
            case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL \n\n");
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                pstContents->eUiState = eInstall_Network_WIFI_POPUP;
                nx_install_network_Wifi_DoFocusablePage(pstContents);
                nx_install_ui_OpenSelectivePopupUI(strListTry,3,
                    (char*)ONDK_GetString(RES_SKIP_01_ID),
                    (char*)ONDK_GetString(RES_RETRY_01_ID));
                break;
            case eNETWORK_STATUS_WIFI_SCANNING_END:
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                apList = (APKS_NETAP_Info_t *)NX_APP_Malloc (sizeof(APKS_NETAP_Info_t) * (lNumOfAp+1));
                if(lNumOfAp>0)
                {
                    HxSTD_memcpy(apList,(APKS_NETAP_Info_t *)p2, sizeof(APKS_NETAP_Info_t)*lNumOfAp);
                }

                HxSTD_memset(&apList[lNumOfAp],0,sizeof(APKS_NETAP_Info_t));

                HxSTD_memcpy(apList[lNumOfAp].szESSID,skipStr,48);

                pstContents->apList = apList;
                pstContents->ulNumOfAp = (lNumOfAp+1) ;
                nx_install_network_wifi_DrawWifiList(pstContents);
                pstContents->eUiState = eInstall_Network_INIT;
                nx_install_network_Wifi_DoFocusablePage(pstContents);
                break;
            case eNETWORK_STATUS_WIFI_SCANNING_FAIL:
                pstContents->eUiState = eInstall_Network_INIT;
                nx_install_network_Wifi_DoFocusablePage(pstContents);
                break;
            default:
                break;
        }
    }
    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_wifi_MsgGwmTimer(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    switch(p1)
    {
        case NX_PRISM_INSTALL_ANIMATION_TIMER_ID:
            nx_install_ui_UpdateProcessingPopupUI(pstContents->lLoadingIndex);
            if(pstContents->lLoadingIndex<3)
            {
                pstContents->lLoadingIndex++;
            }
            else
            {
                pstContents->lLoadingIndex = 0;
            }
            break;
        case NX_PRISM_INSTALL_RESPONSE_TIMER_ID:
            ONDK_GWM_KillTimer(NX_PRISM_INSTALL_RESPONSE_TIMER_ID);
            nx_install_ui_CloseResponsePopupUI();
            pstContents->eUiState = eInstall_Network_INIT;
            nx_install_network_Wifi_DoFocusablePage(pstContents);
            break;
    }
    return ONDK_RET_OK;
}
/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Network_Wifi_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Network_Context_t	*pstContents = NULL;
	pstContents = nx_install_network_wifi_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_network_wifi_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_install_network_wifi_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
            eResult = nx_install_network_wifi_MsgGwmTimer(pstContents, p1, p2, p3);
            break;
		case MSG_GWM_LIST_SELECTED:
            nx_install_network_wifi_MsgGwmListSelected(pstContents, p1, p2, p3);
            break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_network_wifi_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_install_network_wifi_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
            break;
        case MSG_APP_KEYBOARD_RESULT:
            {
                HCHAR 	*pName = NULL;
                APKS_NETAP_Info_t *pstApInfo;
                if(pstContents->eUiState != eInstall_Network_WIFI_HIDDEN_NETWORK)
                {
                    pstApInfo = &(pstContents->apList[pstContents->lSelectedItemIndex]);
        			pName = NX_PRISM_KeyBoard_GetResultString();
                    nx_install_network_wifi_DoConnectAP(pstContents, pstApInfo,pName);
                }

                if(NULL != pName)
                {
                    NX_APP_Free(pName);
                }

                NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
            }
			break;
        case MSG_APP_NETWORK_HIDDEN_RESULT:
            pstContents->eUiState = eInstall_Network_INIT;
            nx_install_network_Wifi_DoFocusablePage(pstContents);
            if(p1 == TRUE)
            {
                HCHAR 	*pName = NULL;
                APKS_NETAP_Info_t *pstApInfo = NULL;

                pstApInfo = (APKS_NETAP_Info_t*)p2;
    			pName = (HCHAR*)p3;
                nx_install_network_wifi_DoConnectAP(pstContents, pstApInfo,pName);
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

#endif //CONFIG_MW_INET_WLAN
/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

