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
 * @file	  		nx_prism_component_hidden_ap_dialog.c
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
#include <nx_prism_app.h>
#include <nx_prism_util.h>
#include <nx_prism_component.h>
#include <nx_prism_component_hidden_ap_dialog.h>
#if defined (CONFIG_MW_INET_WLAN)
/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DLG_HIDDEN_AP_TIMER_ID						    	1000

#define	NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_X  				60
#define	NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_Y					28
#define	NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_W  				750
#define	NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_H					48

#define NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_X                78
#define NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_Y                134
#define NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_W                750
#define NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_H                22

#define NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_X           60
#define NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_Y           186
#define NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_W           750
#define NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_H           55

#define NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_LS                70
#define NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_X                 78
#define NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_W                 320
#define NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_H                 55

#define NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_X_TAP           7
#define NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_Y_TAP           19
#define	NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_W				 13
#define	NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_H	    		 20

#define NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_X                 170
#define NX_PRISM_UI_HIDDEN_AP_BTN_Y                         615
#define NX_PRISM_UI_HIDDEN_AP_BTN_W                         260
#define NX_PRISM_UI_HIDDEN_AP_BTN_H                         55
#define NX_PRISM_UI_HIDDEN_AP_BTN_GAP                       10

#define NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_X          410
#define NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_Y          294//256
#define NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_W          400
#define NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_H          55 + 12

#define NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_X              410
#define NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_Y              418
#define NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_W              400
#define NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_H              55 + 12

#define NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX                8

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

typedef struct tagNetwork_Dialog_EncryptionList {
    APKE_INet_Wifi_Encypt_e  eType;
    HCHAR  strName[64];
} Nx_Network_HIDDEN_Encrypt_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_Network_HIDDEN_Encrypt_t pstNxSecurityTypeList[NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX] ={
        {eINet_Wifi_Encrypt_None,"None"},
        {eINet_Wifi_Encrypt_WEP_64_Hex,"WEP 64-bit Hex"},
        {eINet_Wifi_Encrypt_WEP_64_Ascii,"WEP 64-bit ASCII"},
        {eInet_Wifi_Encrypt_WEP_128_Hex,"WEP 128-bit Hex"},
        {eInet_Wifi_Encrypt_WEP_128_Ascii,"WEP 128-bit ASCII"},
        {eINet_Wifi_Encrypt_WPA_TKIP,"WPA-PSK(TKIP)"},
        {eINet_Wifi_Encrypt_WPA_AES,"WPA-PSK(TKIP+AES)"},
        {eINet_Wifi_Encrypt_WPA_TKIP_AES,"WPA-PSK(AES)"}
};

static HCHAR* pstrNxSecurityList[NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX] ={
"None","WEP 64-bit Hex","WEP 64-bit ASCII","WEP 128-bit Hex","WEP 128-bit ASCII","WPA-PSK(TKIP)","WPA-PSK(TKIP+AES)","WPA-PSK(AES)"
};



/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t    nx_comp_HiddenApDlalog_SetOSDTimeOut(NX_HiddenApDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_HiddenApDlalog_DrawDefaultUI(NX_HiddenApDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_HiddenApDlalog_InitContents(NX_HiddenApDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_HiddenApDlalog_MsgGwmCreate(NX_HiddenApDialogInstance_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_comp_HiddenApDlalog_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}

static void		nx_comp_HiddenApDlalog_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_comp_HiddenApDlalog_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_comp_HiddenApDlalog_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}

static ONDK_Result_t	nx_comp_HiddenApDlalog_SetOSDTimeOut(NX_HiddenApDialogInstance_t *pstContents)
{
	HUINT32		ulTimeOutTime = 0;

	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		ulTimeOutTime = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_DLG_HIDDEN_AP_TIMER_ID, ulTimeOutTime);
	}

	NX_APP_Info("OSD Time Out Time (%d Sec)\n", ulTimeOutTime);

	return	ONDK_RET_OK;
}


void nx_comp_HiddenApDlalog_DrawTitle(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = (NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_ID);

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_X,
        NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_Y,
        NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_W,
        NX_PRISM_UI_HIDDEN_AP_TEXT_TITLE_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, (char*)ONDK_GetString(RES_HIDDEN_NETWORK_ID));

   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_36);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_CENTER);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong01), COL(C_T_Gong01));
}

void nx_comp_HiddenApDlalog_DrawExplain(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = (NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_ID);

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_X,
        NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_Y,
        NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_W,
        NX_PRISM_UI_HIDDEN_AP_TEXT_EXPLAIN_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_MESG_4596_ID));

   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemNormal);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Yoyo01), COL(C_T_Yoyo01));
    ONDK_GWM_Obj_DisableObject(ulID);
}



static ONDK_Result_t nx_comp_HiddenApDlalog_DrawSecurityTypeBtn(NX_HiddenApDialogInstance_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    ulId = (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_ID);

    stRect = ONDK_Rect((NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_X),
        (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_Y),
        (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_W),
        (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_H));

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
	ulId = NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_ID;
	stRect = ONDK_Rect((NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_X)
        +(NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_W)+(NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_X_TAP),
        (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_Y)+(NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_Y_TAP),
        (NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_W), (NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_H));

    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_106_00_ARROW_RIGHT_PNG);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);


    //Extand UI

	stRect = ONDK_Rect(NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_X, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_Y ,
	NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_W, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_H);
    ulId = NX_PRISM_UI_HIDDEN_AP_LIST_SECURITY_ID;

	ONDK_GWM_List_Create(ulId, &stRect, NULL, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_H,
    NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX, NX_PRISM_INSTALL_NETWORK_HIDDEN_LIST_OBJ_NUM,(GWM_LIST_ITEMS)pstrNxSecurityList);
	ONDK_GWM_List_SetCurrentIndex(ulId, 0);
	ONDK_GWM_List_SetCircularMove(ulId, FALSE);
	ONDK_GWM_List_SetFontHandle(ulId, eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    ONDK_GWM_List_SetOffset(ulId, 18,13,0);
    return	eResult;
}

static ONDK_Result_t nx_comp_HiddenApDlalog_ShowSecurityExtendUI(NX_HiddenApDialogInstance_t *pstContents,HBOOL bShow)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulId = 0;
    HINT32          lIndex = 0;

    for(lIndex =0 ; lIndex < NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX;lIndex++)
    {
        if(pstContents->apValue->nEncryptType== pstNxSecurityTypeList[lIndex].eType)
        {
            break;
        }
    }

    if(TRUE == bShow)
    {
        ulId = NX_PRISM_UI_HIDDEN_AP_LIST_SECURITY_ID;
        ONDK_GWM_List_SetCurrentIndex(ulId,lIndex);
        ONDK_GWM_Obj_SetFocus(ulId);
        ONDK_GWM_Obj_SetVisible(ulId, bShow);

        ulId = NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_ID;
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
    }
    else
    {
        ulId = NX_PRISM_UI_HIDDEN_AP_LIST_SECURITY_ID;
        ONDK_GWM_Obj_SetVisible(ulId, bShow);

        ulId = (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_ID);
        ONDK_GWM_Obj_SetFocus(ulId);
        if(lIndex<NX_PRISM_UI_HIDDEN_AP_ENCRYPTION_MAX)
        {
            ONDK_GWM_COM_Button_SetExtraText(ulId,(HUINT8*)pstNxSecurityTypeList[lIndex].strName);
        }
        ulId = NX_PRISM_UI_HIDDEN_AP_IMG_ARROW_IMG_ID;
        ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    }

    return eResult;
}

static ONDK_Result_t nx_comp_HiddenApDlalog_DrawNetworkName(NX_HiddenApDialogInstance_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulID = NX_PRISM_UI_HIDDEN_AP_TEXT_NETWORK_NAME_ID;
    HUINT32         y=0;

    y = (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_Y+NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_H)
        + NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_LS;

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_X,
        y ,
        NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_W,
        NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_NETWORK_NAME_01_ID));
   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_24);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_DisableObject(ulID);

    y = NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_Y;

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_X,
        y ,
        NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_W,
        NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_H);

    ONDK_GWM_COM_Input_Create (NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_ID, &stRect,
        (char *)NULL, NX_PRISM_UI_HIDDEN_AP_INPUT_MAX,(char *)"" );

    ONDK_GWM_COM_Input_SetStringMode(NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_ID,eGUI_INPUTOBJ_MODE_Normal);


    return	eResult;
}

static ONDK_Result_t nx_comp_HiddenApDlalog_DrawPassword(NX_HiddenApDialogInstance_t *pstContents)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulID = NX_PRISM_UI_HIDDEN_AP_TEXT_PASSWORD_ID;
    HUINT32         y=0;

    y = (NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_Y+NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_H)
        + NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_LS
        +(NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_H + NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_LS);

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_X,
        y ,
        NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_W,
        NX_PRISM_UI_HIDDEN_AP_TEXT_CURSOR_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect,(HCHAR*)ONDK_GetString(RES_PASSWORD_02_ID));
   	ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_24);
   	ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
   	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_DisableObject(ulID);

    y = NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_Y;

	stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_X, y,
        NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_W,
        NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_H);

    ONDK_GWM_COM_Input_Create (NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID, &stRect,
        (char *)NULL, NX_PRISM_UI_HIDDEN_AP_INPUT_MAX,(char *)"" );

    ONDK_GWM_COM_Input_SetStringMode(NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID,eGUI_INPUTOBJ_MODE_Pin);

    return	eResult;
}

static void nx_comp_HiddenApDlalog_DrawDirectionBtn(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulId = 0;

    //CONNECT
    ulId = (NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_ID);
    stRect = ONDK_Rect(NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_X,
        NX_PRISM_UI_HIDDEN_AP_BTN_Y ,
        NX_PRISM_UI_HIDDEN_AP_BTN_W,
        NX_PRISM_UI_HIDDEN_AP_BTN_H);
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
    ulId = (NX_PRISM_UI_HIDDEN_AP_BTN_CANCEL_ID);
    stRect = ONDK_Rect((NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_X+NX_PRISM_UI_HIDDEN_AP_BTN_W)+(NX_PRISM_UI_HIDDEN_AP_BTN_GAP)
        , NX_PRISM_UI_HIDDEN_AP_BTN_Y ,
        NX_PRISM_UI_HIDDEN_AP_BTN_W, NX_PRISM_UI_HIDDEN_AP_BTN_H);
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



static ONDK_Result_t	nx_comp_HiddenApDlalog_DrawDefaultUI(NX_HiddenApDialogInstance_t *pstContents)
{
	ONDK_Rect_t 	stBGRect, stRect;

	HxSTD_memset(&stBGRect, 0x00, sizeof(ONDK_Rect_t));
	HxSTD_memset(&stRect, 0x00, sizeof(ONDK_Rect_t));

	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_OSD_FULL)
	{
		stRect = ONDK_Rect(0, 0, NX_PRISM_OSD_HORIZONTAL, NX_PRISM_OSD_VERTICAL);
	}
	else
	{
		stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	}

	// Set OSD Area
	ONDK_GWM_APP_SetAppArea(stRect.x, stRect.y, stRect.w, stRect.h);

	// Draw Background
	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_BG_DIM)
	{
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_IPDLG_BG_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_IPDLG_BG_ID, NX_PRISM_COMP_DrawWindowPopUp);
	}

    nx_comp_HiddenApDlalog_DrawTitle();
    nx_comp_HiddenApDlalog_DrawExplain();
    nx_comp_HiddenApDlalog_DrawSecurityTypeBtn(pstContents);
    nx_comp_HiddenApDlalog_DrawNetworkName(pstContents);
    nx_comp_HiddenApDlalog_DrawPassword(pstContents);
    nx_comp_HiddenApDlalog_DrawDirectionBtn();

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_HiddenApDlalog_InitContents(NX_HiddenApDialogInstance_t *pstContents)
{
	/* init word */
	ONDK_Result_t	eResult = ONDK_RET_OK;
    pstContents->eUiState = eNetwork_Hidden_Ap_Init;
    pstContents->apValue = (APKS_NETAP_Info_t *)NX_APP_Malloc (sizeof(APKS_NETAP_Info_t));
    HxSTD_MemSet(pstContents->apValue,0,sizeof(APKS_NETAP_Info_t));
    pstContents->apValue->nEncryptType= eINet_Wifi_Encrypt_None;
    pstContents->strPassword = NULL;

	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_HiddenApDlalog_MsgGwmDestroy(NX_HiddenApDialogInstance_t *pstContents)
{

    if(NULL != pstContents->apValue)
    {
        NX_APP_Free(pstContents->apValue);
        pstContents->apValue = NULL;
    }


    if(NULL != pstContents->strPassword)
    {
        NX_APP_Free(pstContents->strPassword);
        pstContents->strPassword = NULL;
    }

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_HiddenApDlalog_MsgGwmCreate(NX_HiddenApDialogInstance_t *pstContents)
{
	nx_comp_HiddenApDlalog_InitContents(pstContents);
	nx_comp_HiddenApDlalog_DrawDefaultUI(pstContents);
	nx_comp_HiddenApDlalog_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_HiddenApDlalog_MsgGwmKeyDown(NX_HiddenApDialogInstance_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
    HINT32      lObjId;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    lObjId = GWM_GetFocusedObjectID();

    switch(pstContents->eUiState)
    {
        case eNetwork_Hidden_Ap_KEYBOARD_NAME:
        case eNetwork_Hidden_Ap_KEYBOARD_PW:
            //KEYBOARD DESTROY by BACK or EXIT
            pstContents->eUiState = eNetwork_Hidden_Ap_Init;
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
                case eNetwork_Hidden_Ap_Init:
                    ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, 0, 0, 0);
                    ONDK_GWM_APP_Destroy(0);
                    break;
                case eNetwork_Hidden_Ap_KEYBOARD_SECURITY_OPTION:
                    pstContents->eUiState = eNetwork_Hidden_Ap_Init;
                    nx_comp_HiddenApDlalog_ShowSecurityExtendUI(pstContents,FALSE);
                    return ONDK_MESSAGE_BREAK;
                default:
                    break;
            }
            return ONDK_MESSAGE_BREAK;
		case KEY_EXIT:
		case KEY_MENU:
		case KEY_GUIDE:
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
            switch(pstContents->eUiState)
            {
                case eNetwork_Hidden_Ap_Init:
                    switch(lObjId)
                    {
                        case NX_PRISM_UI_HIDDEN_AP_BTN_SECURITY_TYPE_ID:
                            pstContents->eUiState = eNetwork_Hidden_Ap_KEYBOARD_SECURITY_OPTION;
                            nx_comp_HiddenApDlalog_ShowSecurityExtendUI(pstContents,TRUE);
                            break;
                        case NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_ID:
                        	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
                				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_NORMAL, 0);
                            pstContents->eUiState = eNetwork_Hidden_Ap_KEYBOARD_NAME;
                            break;
                        case NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID:
                        	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
        		        		NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_PASSWD, 0);
                            pstContents->eUiState = eNetwork_Hidden_Ap_KEYBOARD_PW;
                            break;
                        case NX_PRISM_UI_HIDDEN_AP_BTN_CANCEL_ID:
                            ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, 0, 0, 0);
                            ONDK_GWM_APP_Destroy(0);
                            break;
                        case NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_ID:
                            //Check Network Name
                            if(HxSTD_StrCmp("",pstContents->apValue->szESSID)==0)
                            {
                                nx_comp_HiddenApDlalog_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_4598_ID));
                                break;
                            }
                            else
                            {
                                ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_NETWORK_HIDDEN_RESULT, (Handle_t)NULL, (HINT32)TRUE, (HINT32)pstContents->apValue, (HINT32)pstContents->strPassword);
                                ONDK_GWM_APP_Destroy(0);
                            }
                            break;
                    }
                    break;
                case eNetwork_Hidden_Ap_KEYBOARD_SECURITY_OPTION:
                    return	ONDK_RET_OK;
                default:
                    break;
            }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
            switch(pstContents->eUiState)
            {
                case eNetwork_Hidden_Ap_KEYBOARD_SECURITY_OPTION:
                    pstContents->eUiState = eNetwork_Hidden_Ap_Init;
                    nx_comp_HiddenApDlalog_ShowSecurityExtendUI(pstContents,FALSE);
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
            if(lObjId == NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID)
            {
                if(HxSTD_StrCmp("",pstContents->apValue->szESSID)!=0)
                {
                    ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_HIDDEN_AP_BTN_CONNECT_ID);
                }
            }
			break;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default:
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;

}

static ONDK_Result_t	nx_comp_HiddenApDlalog_MsgGwmListSelected(NX_HiddenApDialogInstance_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lListIdx = p2;

    pstContents->apValue->nEncryptType= pstNxSecurityTypeList[lListIdx].eType;

    pstContents->eUiState = eNetwork_Hidden_Ap_Init;

    nx_comp_HiddenApDlalog_ShowSecurityExtendUI(pstContents,FALSE);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);
	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_COMP_HiddenApDlalog_SetHiddenAPMenu(NX_HiddenApDialogInstance_t *pstContents, ONDK_GWM_Callback_t parentAppCallback)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! pstContents is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	// parent app
	if (parentAppCallback == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! parentAppCallback is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(NX_HiddenApDialogInstance_t));
	pstContents->parentAppCallback = parentAppCallback;
	pstContents->ulDlgAttributes = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM);
	// parent app
	if (parentAppCallback == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! parentAppCallback is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_HiddenApDlalog_ProtoProc(NX_HiddenApDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HCHAR 	*pValue = NULL;
	ONDK_Result_t		eResult = ONDK_RET_OK;

	NX_APP_Trace("NX_PRISM_COMP_TimeDlalog_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message){
		case MSG_GWM_CREATE:
			nx_comp_HiddenApDlalog_MsgGwmCreate(pstContents);
			break;
		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_HIDDEN_AP_TIMER_ID);
			}
            eResult = nx_comp_HiddenApDlalog_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_LIST_SELECTED:
            if(eNetwork_Hidden_Ap_KEYBOARD_SECURITY_OPTION == pstContents->eUiState)
            {
                nx_comp_HiddenApDlalog_MsgGwmListSelected(pstContents, p1, p2, p3);
            }
            break;
        case MSG_APP_KEYBOARD_RESULT:
			pValue = NX_PRISM_KeyBoard_GetResultString();
            if(eNetwork_Hidden_Ap_KEYBOARD_NAME== pstContents->eUiState)
            {
                HxSTD_memcpy(pstContents->apValue->szESSID,(HCHAR*)pValue,NX_PRISM_UI_HIDDEN_AP_INPUT_MAX);
                ONDK_GWM_COM_Input_SetText(NX_PRISM_UI_HIDDEN_AP_INPUT_NETWORK_NAME_ID,
                    NX_PRISM_UI_HIDDEN_AP_INPUT_MAX,pstContents->apValue->szESSID);
            }
            else if(eNetwork_Hidden_Ap_KEYBOARD_PW == pstContents->eUiState)
            {
                if(NULL == pstContents->strPassword)
                {
                    pstContents->strPassword = (HCHAR*)NX_APP_Malloc(sizeof(HCHAR)*NX_PRISM_UI_HIDDEN_AP_INPUT_MAX);
                }
                HxSTD_MemSet(pstContents->strPassword,0,sizeof(HCHAR)*NX_PRISM_UI_HIDDEN_AP_INPUT_MAX);
                HxSTD_memcpy(pstContents->strPassword,(HCHAR *)pValue,NX_PRISM_UI_HIDDEN_AP_INPUT_MAX);
                ONDK_GWM_COM_Input_SetText(NX_PRISM_UI_HIDDEN_AP_INPUT_PASSWORD_ID,
                    NX_PRISM_UI_HIDDEN_AP_INPUT_MAX,pstContents->strPassword);
            }

            if(NULL !=pValue)
            {
                NX_APP_Free(pValue);
            }
            pstContents->eUiState = eNetwork_Hidden_Ap_Init;
            NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
			break;
		case MSG_GWM_TIMER:
			return ONDK_MESSAGE_BREAK;
		case MSG_GWM_DESTROY:
            nx_comp_HiddenApDlalog_MsgGwmDestroy(pstContents);
			break;

	}

	NX_APP_Trace("NX_PRISM_COMP_TimeDlalog_ProtoProc message(0x%x) -\n", message);

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}
	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif //CONFIG_MW_INET_WLAN

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

