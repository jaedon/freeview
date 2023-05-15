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
#include <nx_prism_install_sub_network.h>
#include <nx_prism_install_model.h>

#ifdef CONFIG_MW_INET

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_INSTALL_NETWORK_TEXT_CONTEXT_ID 	    ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+1)

//Description
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_ID 	        ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+2)
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_ID 	        ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+3)
#define	NX_PRISM_INSTALL_NETWORK_TEXT_LAN_ID 	        ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+4)
#define	NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_ID 	        ((NX_PRISM_INSTALL_NETWORK_ITEM_ID)+5)

#define	NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_X 	        313
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_Y 	        287
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_W 	        277
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_H 	        182

#define	NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_X 	        690
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_Y 	        (NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_Y)
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_W 	        (NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_W)
#define	NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_H 	        (NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_H)

#define	NX_PRISM_INSTALL_NETWORK_TEXT_LAN_X             336
#define	NX_PRISM_INSTALL_NETWORK_TEXT_LAN_Y             487
#define	NX_PRISM_INSTALL_NETWORK_TEXT_LAN_W             231
#define	NX_PRISM_INSTALL_NETWORK_TEXT_LAN_H             22

#define	NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_X             713
#define	NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_Y             487
#define	NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_W             (NX_PRISM_INSTALL_NETWORK_TEXT_LAN_W)
#define	NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_H             (NX_PRISM_INSTALL_NETWORK_TEXT_LAN_H)
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

static ONDK_Result_t    nx_install_network_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_install_network_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_network_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_install_network_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_network_MsgGwmAPKMessage(Nx_Install_Network_Context_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Network_Context_t	*nx_install_network_GetContents(void)
{
	return	&s_stInstallNetworkContent;
}


static ONDK_Result_t nx_install_network_InitApplication (Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

	return ONDK_RET_OK;
}

static void nx_install_network_DrawDirectionBtn(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    //PREVIOUS
    ulId = (NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_NETWORK_BTN_X), NX_PRISM_INSTALL_NETWORK_BTN_Y , NX_PRISM_INSTALL_NETWORK_BTN_W, NX_PRISM_INSTALL_NETWORK_BTN_H);
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
    ulId = (NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_NETWORK_BTN_X)+(NX_PRISM_INSTALL_NETWORK_BTN_W)+(NX_PRISM_INSTALL_NETWORK_BTN_GAP), NX_PRISM_INSTALL_NETWORK_BTN_Y ,
        NX_PRISM_INSTALL_NETWORK_BTN_W, NX_PRISM_INSTALL_NETWORK_BTN_H);
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
    ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
}

static void	nx_install_network_DrawNetworkDescriptionUI(void)
{
    ONDK_Rect_t stRect;
    HUINT32     ulId = 0;

    //LAN CABLE

#if defined(CONFIG_MW_INET)
	ulId = NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_ID;

  	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_X,
            NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_Y,
            NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_W,
            NX_PRISM_INSTALL_NETWORK_IMAGE_LAN_H);

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_321_06_WIZ_LAN_PNG);

	ulId = NX_PRISM_INSTALL_NETWORK_TEXT_LAN_ID;

  	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_TEXT_LAN_X,
            NX_PRISM_INSTALL_NETWORK_TEXT_LAN_Y,
            NX_PRISM_INSTALL_NETWORK_TEXT_LAN_W,
            NX_PRISM_INSTALL_NETWORK_TEXT_LAN_H);

    ONDK_GWM_COM_Text_Create(ulId , &stRect, "LAN CABLE" );
	ONDK_GWM_COM_Text_SetFont(ulId , eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(ulId , NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(ulId , TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
#endif

#if defined(CONFIG_MW_INET_WLAN)
    //WLAN
	ulId = NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_ID;

  	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_X,
            NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_Y,
            NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_W,
            NX_PRISM_INSTALL_NETWORK_IMAGE_WLAN_H);

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_321_06_WIZ_WIFI_PNG);

	ulId = NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_ID;

  	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_X,
            NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_Y,
            NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_W,
            NX_PRISM_INSTALL_NETWORK_TEXT_WLAN_H);

    ONDK_GWM_COM_Text_Create(ulId , &stRect, "WLAN" );
	ONDK_GWM_COM_Text_SetFont(ulId , eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(ulId , NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(ulId , TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
	ONDK_GWM_COM_Text_ChangeColor(ulId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
#endif
}

static ONDK_Result_t	nx_install_network_DrawDefaultUI(Nx_Install_Network_Context_t *pstContents)
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
    nx_install_ui_DrawExplain((HCHAR *)ONDK_GetString(RES_MESG_4655_ID));
    nx_install_network_DrawNetworkDescriptionUI();


    //Previous, Next Btn
    nx_install_network_DrawDirectionBtn();

    //Progress Popup
    nx_install_ui_SetProcessingPopupUI(FALSE, (char*)ONDK_GetString(RES_CONNECTING_ID));

    return eResult;
}

static ONDK_Result_t	nx_install_network_DoFocusablePage(Nx_Install_Network_Context_t *pstContents)
{
    switch(pstContents->eUiState)
    {
        case eInstall_Network_INIT:
            ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
            ONDK_GWM_Obj_EnableObject(NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID);
            ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
            ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID,GUIOBJ_HILIGHTED);
            ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID,GUIOBJ_NORMAL);
            break;
        default:
            ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
            ONDK_GWM_Obj_DisableObject(NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID);
            ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID,GUIOBJ_DISABLED);
            ONDK_GWM_COM_Button_SetStatus(NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID,GUIOBJ_DISABLED);
            break;
    }
    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_DoCheckConnection(Nx_Install_Network_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HBOOL           bWire   = TRUE;
    HERROR          hError = ERR_FAIL;

    //check Wire Connection
    pstContents->eUiState = eInstall_Network_LAN_TRYING;
    hError = NX_NETWORK_GetLANConnected(&bWire);

    if(ERR_OK != hError )
    {
        return ONDK_RET_FAIL;
    }

    if(TRUE == bWire)
    {
        //Connection Try
        nx_install_ui_SetProcessingPopupUI(TRUE,NULL);
        ONDK_GWM_SetTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_ANIMATION_TIMER_PERIOD);

        NX_NETWORK_EnableNetInterface(eNxNet_INTERFACE_TYPE_LAN);
    }
    else
    {
#if defined(CONFIG_MW_INET_WLAN)
        //Go To WIFI
        ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_Install_Network_Wifi_Proc", APP_UIMGR_PRIORITY,
        NX_PRISM_Install_Network_Wifi_Proc,  0, 0, 0, 0);
#else
        nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, 0, 0, 0);
#endif
        ONDK_GWM_APP_Destroy(0);
    }
    return eResult;
}


static ONDK_Result_t	nx_install_network_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;
	ONDK_Result_t	eResult = ONDK_RET_OK;

    HxLOG_Error("\n\n####### nx_install_network_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_network_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

    nx_install_network_DrawDefaultUI(pstContents);

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

	return	eResult;
}


static ONDK_Result_t	nx_install_network_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstSubItems)
	{
	    NX_APP_Free(pstContents->pstSubItems);
		pstContents->pstSubItems = NULL;
	}
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}
    return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_install_network_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HINT32			lKeyId = p1;
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

    lObjId = GWM_GetFocusedObjectID();

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
            switch(pstContents->eUiState)
            {
                case eInstall_Network_LAN_CONNECTED:
                    nx_install_ui_CloseConfirmPopupUI();
                    pstContents->eUiState = eInstall_Network_INIT;
                    nx_install_network_DoFocusablePage(pstContents);
                    break;
                case eInstall_Network_LAN_DISCONNECTED:
                    nx_install_ui_CloseSelectivePopupUI();
                    pstContents->eUiState = eInstall_Network_INIT;
                    nx_install_network_DoFocusablePage(pstContents);
                    break;
                case eInstall_Network_INIT:
                    eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, 0, 0, 0);
					if(ONDK_RET_OK == eResult)
                	{
                    	ONDK_GWM_APP_Destroy(0);
					}
                    break;
                default:
                    break;
            }
            return ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
            switch(pstContents->eUiState)
            {
                case eInstall_Network_LAN_CONNECTED:
                    nx_install_ui_CloseConfirmPopupUI();
                    pstContents->eUiState = eInstall_Network_INIT;
                    nx_install_network_DoFocusablePage(pstContents);
                    break;
                case eInstall_Network_LAN_DISCONNECTED:
                    nx_install_ui_CloseSelectivePopupUI();
                    pstContents->eUiState = eInstall_Network_INIT;
                    nx_install_network_DoFocusablePage(pstContents);
                    break;
                default:
                    break;
            }
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
            switch(lObjId)
            {
                case NX_PRISM_INSTALL_NETWORK_BTN_PREVIOUS_ID:
                    //move to previous Step
                    eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
                    if(ONDK_RET_OK == eResult)
                    {
                        ONDK_GWM_APP_Destroy(0);
                    }
                    break;
                case NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID:
                      nx_install_network_DoCheckConnection(pstContents);
                    break;
                case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID:
                    nx_install_ui_CloseSelectivePopupUI();
                    ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_NETWORK_BTN_NEXT_ID);
                    break;
                case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID:
                case NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID:
                      nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, 0, 0, 0);
                      ONDK_GWM_APP_Destroy(0);
                    break;
                default:
                    break;
            }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
            break;
		case KEY_ARROWRIGHT:
            break;
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		case KEY_STANDBY:
            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_MENU:
		case KEY_GUIDE:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;
		default:
			return	ONDK_MESSAGE_PASS;
	}

    NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_MsgGwmAPKMessage(Nx_Install_Network_Context_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e eEvent = p1;
    HCHAR*              strListTry[3]={0,};
    HCHAR*              strListSuccess[2] = {0,};

    strListTry[0] =(HCHAR*)ONDK_GetString(RES_MESG_360_ID);
    strListTry[1] =(HCHAR*)ONDK_GetString(RES_MESG_3211_ID);
    strListTry[2] =(HCHAR*)ONDK_GetString(RES_MESG_3212_ID);

    strListSuccess[0] = (HCHAR*)ONDK_GetString(RES_MESG_3208_ID);
    strListSuccess[1] = "Press NEXT to continue.";

    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
            case eNETWORK_STATUS_CON_FAIL:
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                nx_install_ui_OpenSelectivePopupUI(strListTry,3,
                    (char*)ONDK_GetString(RES_SKIP_01_ID),
                    (char*)ONDK_GetString(RES_RETRY_01_ID));
                pstContents->eUiState = eInstall_Network_LAN_DISCONNECTED;
                nx_install_network_DoFocusablePage(pstContents);
                break;
            case eNETWORK_STATUS_CON_SUCCESS:
                ONDK_GWM_KillTimer(NX_PRISM_INSTALL_ANIMATION_TIMER_ID);
                nx_install_ui_SetProcessingPopupUI(FALSE,NULL);
                nx_install_ui_OpenConfirmPopupUI(strListSuccess,2);
                pstContents->eUiState = eInstall_Network_LAN_CONNECTED;
                nx_install_network_DoFocusablePage(pstContents);
                break;
            default:
                break;
        }
    }

    return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Network_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Network_Context_t	*pstContents = NULL;
	pstContents = nx_install_network_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_network_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_install_network_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
            nx_install_network_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
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
             }
            break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_network_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_install_network_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
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

#endif //CONFIG_MW_INET

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

