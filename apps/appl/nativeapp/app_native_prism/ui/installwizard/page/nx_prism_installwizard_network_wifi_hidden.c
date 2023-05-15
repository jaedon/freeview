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
#ifdef CONFIG_MW_INET_WLAN
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_INSTALL_NETWORK_HIDDEN_ID                            (NX_PRISM_INSTALL_NETWORK_HIDDEN_ITEM_ID)
#define NX_INSTALL_NETWORK_HIDDEN_DIM_BACKGOURND_ID             (NX_INSTALL_NETWORK_HIDDEN_ID)+1
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_ID                 (NX_INSTALL_NETWORK_HIDDEN_ID)+2
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_ID               (NX_INSTALL_NETWORK_HIDDEN_ID)+3
#define NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_ID          (NX_INSTALL_NETWORK_HIDDEN_ID)+4
#define NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_ID              (NX_INSTALL_NETWORK_HIDDEN_ID)+5
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_NETWORK_NAME_ID          (NX_INSTALL_NETWORK_HIDDEN_ID)+6
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_PASSWORD_ID              (NX_INSTALL_NETWORK_HIDDEN_ID)+7

#define NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_ID         (NX_INSTALL_NETWORK_HIDDEN_ID)+8
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID             (NX_INSTALL_NETWORK_HIDDEN_ID)+9

#define NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_ID                (NX_INSTALL_NETWORK_HIDDEN_ID)+10
#define NX_INSTALL_NETWORK_HIDDEN_BTN_CANCEL_ID                 (NX_INSTALL_NETWORK_HIDDEN_ID)+11
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_ID                 (NX_INSTALL_NETWORK_HIDDEN_ID)+12

#define	NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_X  				60
#define	NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_Y					28
#define	NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_W  				750
#define	NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_H					48

#define NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_X                78
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_Y                134
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_W                750
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_H                22

#define NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_X           60
#define NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_Y           186
#define NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_W           750
#define NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_H           55

#define NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_LS                70
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_X                 78
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_W                 320
#define NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_H                 55

#define NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_X_TAP           7
#define NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_Y_TAP           19
#define	NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_W				 13
#define	NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_H	    		 20

#define NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_X                 170
#define NX_INSTALL_NETWORK_HIDDEN_BTN_Y                         615
#define NX_INSTALL_NETWORK_HIDDEN_BTN_W                         260
#define NX_INSTALL_NETWORK_HIDDEN_BTN_H                         55
#define NX_INSTALL_NETWORK_HIDDEN_BTN_GAP                       10

#define NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_X          410
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_Y          294//256
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_W          400
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_H          55 + 12

#define NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_X              410
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_Y              418
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_W              400
#define NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_H              55 + 12

#define NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX                8

#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_X			815
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_Y			186
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_W			405
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_H			480 //60 * 8

#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_H    	55
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_GAP    5
#define NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_NUM    8

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagInstall_Network_EncryptionList {
    APKE_INet_Wifi_Encypt_e  eType;
    HCHAR  strName[64];
} Nx_Install_Network_HIDDEN_Encrypt_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Install_Network_Context_t		s_stInstallNetworkContent;

static Nx_Install_Network_HIDDEN_Encrypt_t pstNxSecurityTypeList[NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX] ={
        {eINet_Wifi_Encrypt_None,"None"},
        {eINet_Wifi_Encrypt_WEP_64_Hex,"WEP 64-bit Hex"},
        {eINet_Wifi_Encrypt_WEP_64_Ascii,"WEP 64-bit ASCII"},
        {eInet_Wifi_Encrypt_WEP_128_Hex,"WEP 128-bit Hex"},
        {eInet_Wifi_Encrypt_WEP_128_Ascii,"WEP 128-bit ASCII"},
        {eINet_Wifi_Encrypt_WPA_TKIP,"WPA-PSK(TKIP)"},
        {eINet_Wifi_Encrypt_WPA_AES,"WPA-PSK(TKIP+AES)"},
        {eINet_Wifi_Encrypt_WPA_TKIP_AES,"WPA-PSK(AES)"}
};

static HCHAR* pstrNxSecurityList[NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX] ={
"None","WEP 64-bit Hex","WEP 64-bit ASCII","WEP 128-bit Hex","WEP 128-bit ASCII","WPA-PSK(TKIP)","WPA-PSK(TKIP+AES)","WPA-PSK(AES)"
};


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	                    nx_install_network_Hidden_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	                    nx_install_network_Hidden_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	                    nx_install_network_Hidden_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t                    	nx_install_network_Hidden_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_Install_Network_Context_t	*nx_install_network_Hidden_GetContents(void)
{
	return	&s_stInstallNetworkContent;
}


static ONDK_Result_t nx_install_network_Hidden_InitApplication (Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

    pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
    pstContents->apValue = (APKS_NETAP_Info_t *)NX_APP_Malloc (sizeof(APKS_NETAP_Info_t));
    HxSTD_MemSet(pstContents->apValue,0,sizeof(APKS_NETAP_Info_t));

    pstContents->apValue->nEncryptType= eINet_Wifi_Encrypt_None;

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

void nx_install_network_Hidden_DrawTitle(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = (NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_ID);

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_X,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_Y,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_W,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_TITLE_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, (char*)ONDK_GetString(RES_HIDDEN_NETWORK_ID));

   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_36);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_CENTER);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong01), COL(C_T_Gong01));
}

void nx_install_network_Hidden_DrawExplain(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = (NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_ID);

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_X,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_Y,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_W,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_EXPLAIN_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_MESG_4596_ID));

   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemNormal);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Yoyo01), COL(C_T_Yoyo01));
    ONDK_GWM_Obj_DisableObject(ulID);
}



static ONDK_Result_t nx_install_network_Hidden_DrawSecurityTypeBtn(Nx_Install_Network_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    ulId = (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_ID);

    stRect = ONDK_Rect((NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_X),
        (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_Y),
        (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_W),
        (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_H));

    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_SECURITY_TYPE_01_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100),  COL(C_G_Gong02), COL(C_G_Base01_100), COL(C_G_Base01_20));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_LEFT);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
    ONDK_GWM_Obj_SetFocus(ulId);

    //value
    ONDK_GWM_COM_Button_SetExtraText(ulId,(HUINT8*)ONDK_GetString(RES_NONE_01_ID));
    ONDK_GWM_COM_Button_SetExtraAlign(ulId,ONDKSTF_RIGHT);

	// right arrow
	ulId = NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_ID;
	stRect = ONDK_Rect((NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_X)
        +(NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_W)+(NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_X_TAP),
        (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_Y)+(NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_Y_TAP),
        (NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_W), (NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_H));

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_106_00_ARROW_RIGHT_PNG);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);


    //Extand UI

	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_X, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_Y ,
	NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_W, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_H);
    ulId = NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_ID;

	ONDK_GWM_List_Create(ulId, &stRect, NULL, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_H,
    NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_NUM,(GWM_LIST_ITEMS)pstrNxSecurityList);
	ONDK_GWM_List_SetCurrentIndex(ulId, 0);
	ONDK_GWM_List_SetCircularMove(ulId, FALSE);
	ONDK_GWM_List_SetFontHandle(ulId, eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    ONDK_GWM_List_SetOffset(ulId, 18,13,0);
    return	eResult;
}

static ONDK_Result_t nx_install_network_Hidden_ShowSecurityExtendUI(Nx_Install_Network_Context_t *pstContents,HBOOL bShow)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulId = 0;
    HINT32          lIndex = 0;

    for(lIndex =0 ; lIndex < NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX;lIndex++)
    {
        if(pstContents->apValue->nEncryptType== pstNxSecurityTypeList[lIndex].eType)
        {
            break;
        }
    }

    if(TRUE == bShow)
    {
        ulId = NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_ID;
        ONDK_GWM_List_SetCurrentIndex(NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_ID,lIndex);
        ONDK_GWM_Obj_SetFocus(ulId);
        ONDK_GWM_Obj_SetVisible(ulId, bShow);

        ulId = NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_ID;
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    }
    else
    {
        ulId = NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_ID;
        ONDK_GWM_Obj_SetVisible(ulId, bShow);

        ulId = (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_ID);
        ONDK_GWM_Obj_SetFocus(ulId);

        if(lIndex<NX_INSTALL_NETWORK_HIDDEN_ENCRYPTION_MAX)
        {
            ONDK_GWM_COM_Button_SetExtraText(ulId,(HUINT8*)pstNxSecurityTypeList[lIndex].strName);
        }


        ulId = NX_INSTALL_NETWORK_HIDDEN_IMG_ARROW_IMG_ID;
        ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    }

    return eResult;
}

static ONDK_Result_t nx_install_network_Hidden_DrawNetworkName(Nx_Install_Network_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulID = NX_INSTALL_NETWORK_HIDDEN_TEXT_NETWORK_NAME_ID;
    HUINT32         y=0;

    y = (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_Y+NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_H)
        + NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_LS;

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_X,
        y ,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_W,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_NETWORK_NAME_01_ID));
   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_24);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_DisableObject(ulID);

    y = NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_Y;

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_X,
        y ,
        NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_W,
        NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_H);

    ONDK_GWM_COM_Input_Create (NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_ID, &stRect,
        (char *)NULL, NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX,(char *)"" );

    ONDK_GWM_COM_Input_SetStringMode(NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_ID,eGUI_INPUTOBJ_MODE_Normal);


    return	eResult;
}

static ONDK_Result_t nx_install_network_Hidden_DrawPassword(Nx_Install_Network_Context_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulID = NX_INSTALL_NETWORK_HIDDEN_TEXT_PASSWORD_ID;
    HUINT32         y=0;

    y = (NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_Y+NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_H)
        + NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_LS
        +(NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_H + NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_LS);

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_X,
        y ,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_W,
        NX_INSTALL_NETWORK_HIDDEN_TEXT_CURSOR_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_PASSWORD_02_ID));
   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_24);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_DisableObject(ulID);

    y = NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_Y;

	stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_X, y,
        NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_W,
        NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_H);

    ONDK_GWM_COM_Input_Create (NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID, &stRect,
        (char *)NULL, NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX,(char *)"" );

    ONDK_GWM_COM_Input_SetStringMode(NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID,eGUI_INPUTOBJ_MODE_Pin);

    return	eResult;
}

static void nx_install_network_Hidden_DrawDirectionBtn(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    //CONNECT
    ulId = (NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_ID);
    stRect = ONDK_Rect(NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_X,
        NX_INSTALL_NETWORK_HIDDEN_BTN_Y ,
        NX_INSTALL_NETWORK_HIDDEN_BTN_W,
        NX_INSTALL_NETWORK_HIDDEN_BTN_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_MOBILE_0064_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);

    //CANCEL
    ulId = (NX_INSTALL_NETWORK_HIDDEN_BTN_CANCEL_ID);
    stRect = ONDK_Rect((NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_X+NX_INSTALL_NETWORK_HIDDEN_BTN_W)+(NX_INSTALL_NETWORK_HIDDEN_BTN_GAP)
        , NX_INSTALL_NETWORK_HIDDEN_BTN_Y ,
        NX_INSTALL_NETWORK_HIDDEN_BTN_W, NX_INSTALL_NETWORK_HIDDEN_BTN_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_CANCEL_03_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);
}


static ONDK_Result_t	nx_install_network_Hidden_DrawDefaultUI(Nx_Install_Network_Context_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT8          ulPageIndex = 0;
    HUINT8          ulNumofPages = 0;
    HUINT8          ulIndex = 0;
    Nx_InstallSubPage_t	*pstSubItem;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

    for(ulIndex = 0; ulIndex < pstContents->ulNumOfSubPage ; ulIndex++)
    {
        pstSubItem = &(pstContents->pstSubItems[ulIndex]);
        if(pstSubItem->ePageName == pstContents->ePage){
            ulPageIndex = ulIndex;
            break;
        }
    }
    ulNumofPages = pstContents->ulNumOfSubPage;

   	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
   	ONDK_GWM_COM_Rect_Create(NX_INSTALL_NETWORK_HIDDEN_DIM_BACKGOURND_ID, &stRect, COL(C_G_Bang_95));
    ONDK_GWM_Obj_SetVisible(NX_INSTALL_NETWORK_HIDDEN_DIM_BACKGOURND_ID, TRUE);

    nx_install_network_Hidden_DrawTitle();
    nx_install_network_Hidden_DrawExplain();
    nx_install_network_Hidden_DrawSecurityTypeBtn(pstContents);
    nx_install_network_Hidden_DrawNetworkName(pstContents);
    nx_install_network_Hidden_DrawPassword(pstContents);

    nx_install_network_Hidden_DrawDirectionBtn();
    return eResult;
}

static ONDK_Result_t	nx_install_network_Hidden_MsgGwmCreate(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

    HxLOG_Error("\n\n####### nx_install_network_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_network_Hidden_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

    nx_install_network_Hidden_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_install_network_Hidden_MsgGwmDestroy(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    ONDK_GWM_Obj_SetVisible(NX_INSTALL_NETWORK_HIDDEN_DIM_BACKGOURND_ID, FALSE);
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_install_network_Hidden_MsgGwmObjFocusChanged(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_install_network_Hidden_MsgGwmKeyDown(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
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

    switch(pstContents->eUiState)
    {
        case eInstall_Network_WIFI_HIDDEN_KEYBOARD_NAME:
        case eInstall_Network_WIFI_HIDDEN_KEYBOARD_PW:
            //KEYBOARD DESTROY by BACK or EXIT
            pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
            break;
        default:
            break;
    }

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
            switch(pstContents->eUiState)
            {
                case eInstall_Network_WIFI_HIDDEN_NETWORK:
                    ONDK_GWM_SendMessage(NULL, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, 0, 0, 0);
                    ONDK_GWM_APP_Destroy(0);
                    break;
                case eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION:
                    pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
                    nx_install_network_Hidden_ShowSecurityExtendUI(pstContents,FALSE);
                    return ONDK_MESSAGE_BREAK;
                default:
                    break;
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
            switch(pstContents->eUiState)
            {
                case eInstall_Network_WIFI_HIDDEN_NETWORK:
                    switch(lObjId)
                    {
                        case NX_INSTALL_NETWORK_HIDDEN_BTN_SECURITY_TYPE_ID:
                            pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION;
                            nx_install_network_Hidden_ShowSecurityExtendUI(pstContents,TRUE);
                            break;
                        case NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_ID:
                        	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
                				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_NORMAL, 0);
                            pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_KEYBOARD_NAME;
                            break;
                        case NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID:
                        	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
        		        		NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_PASSWD, 0);
                            pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_KEYBOARD_PW;
                            break;
                        case NX_INSTALL_NETWORK_HIDDEN_BTN_CANCEL_ID:
                            ONDK_GWM_SendMessage(NULL, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, 0, 0, 0);
                            ONDK_GWM_APP_Destroy(0);
                            break;
                        case NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_ID:
                            //Check Network Name
                            if(HxSTD_StrCmp("",pstContents->apValue->szESSID)==0)
                            {
                                nx_install_ui_OpenResponsePopupUI((char*)ONDK_GetString(RES_MESG_4598_ID));
                                ONDK_GWM_SetTimer(NX_PRISM_INSTALL_RESPONSE_TIMER_ID, (HUINT32)NX_PRISM_INSTALL_RESPONSE_TIMER_3SEC_PERIOD);
                                break;
                            }
                            else
                            {
                                ONDK_GWM_SendMessage(NULL, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, (HINT32)TRUE, (HINT32)pstContents->apValue, (HINT32)pstContents->strPassword);
                                ONDK_GWM_APP_Destroy(0);
                            }
                            break;
                    }
                    break;
                case eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION:
                    return	ONDK_RET_OK;
                default:
                    break;
            }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
            switch(pstContents->eUiState)
            {
                case eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION:
                    pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
                    nx_install_network_Hidden_ShowSecurityExtendUI(pstContents,FALSE);
                    return ONDK_MESSAGE_BREAK;
                default:
                    break;
            }
            break;
		case KEY_ARROWRIGHT:
            break;
		case KEY_ARROWUP:
            break;
		case KEY_ARROWDOWN:
            if(lObjId == NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID)
            {
                if(HxSTD_StrCmp("",pstContents->apValue->szESSID)!=0)
                {
                    ONDK_GWM_Obj_SetFocus(NX_INSTALL_NETWORK_HIDDEN_BTN_CONNECT_ID);
                }
            }
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

static ONDK_Result_t	nx_install_network_Hidden_MsgGwmListSelected(Nx_Install_Network_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lListIdx = p2;

    pstContents->apValue->nEncryptType= pstNxSecurityTypeList[lListIdx].eType;

    pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;

    nx_install_network_Hidden_ShowSecurityExtendUI(pstContents,FALSE);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);
	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Network_Hidden_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_Install_Network_Context_t	*pstContents = NULL;
    HCHAR 	*pValue = NULL;

	pstContents = nx_install_network_Hidden_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_network_Hidden_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_install_network_Hidden_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
            nx_install_network_Hidden_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
            switch(p1)
            {
                case NX_PRISM_INSTALL_RESPONSE_TIMER_ID:
                {
                    ONDK_GWM_KillTimer(NX_PRISM_INSTALL_RESPONSE_TIMER_ID);
                    nx_install_ui_CloseResponsePopupUI();
                    break;
                }
            }

            break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_network_Hidden_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_LIST_SELECTED:
            if(eInstall_Network_WIFI_HIDDEN_KEYBOARD_SECURITY_OPTION == pstContents->eUiState)
            {
                nx_install_network_Hidden_MsgGwmListSelected(pstContents, p1, p2, p3);
            }
            break;
        case MSG_APP_KEYBOARD_RESULT:
			pValue = NX_PRISM_KeyBoard_GetResultString();
            if(eInstall_Network_WIFI_HIDDEN_KEYBOARD_NAME == pstContents->eUiState)
            {
                HxSTD_memcpy(pstContents->apValue->szESSID,(HCHAR *)pValue,NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX);
                ONDK_GWM_COM_Input_SetText(NX_INSTALL_NETWORK_HIDDEN_INPUT_NETWORK_NAME_ID,
                    NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX,pstContents->apValue->szESSID);
            }
            else if(eInstall_Network_WIFI_HIDDEN_KEYBOARD_PW == pstContents->eUiState)
            {
                HxSTD_memcpy(pstContents->strPassword,(HCHAR *)pValue,NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX);
                ONDK_GWM_COM_Input_SetText(NX_INSTALL_NETWORK_HIDDEN_INPUT_PASSWORD_ID,
                    NX_INSTALL_NETWORK_HIDDEN_INPUT_MAX,pstContents->strPassword);
            }

            if(NULL != pValue)
            {
                NX_APP_Free(pValue);
            }

            pstContents->eUiState = eInstall_Network_WIFI_HIDDEN_NETWORK;
            NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");

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

