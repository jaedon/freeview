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
 * @file	  		nx_prism_menu_wlan.c
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
#include <nx_prism_component_hidden_ap_dialog.h>
#if defined (CONFIG_MW_INET_WLAN)
/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_CONFIGURE_WLAN_ID					eNxSetting_ConfigureWLan

// WIFI LIST
#define NX_MENU_CONFIGURE_WLAN_LIST_X			60
#define NX_MENU_CONFIGURE_WLAN_LIST_Y			163
#define NX_MENU_CONFIGURE_WLAN_LIST_W			750
#define NX_MENU_CONFIGURE_WLAN_LIST_H			540 //60 * 9
#define NX_MENU_CONFIGURE_WLAN_LIST_OBJ_H       	55
#define NX_MENU_CONFIGURE_WLAN_LIST_OBJ_GAP     	5

#define NX_MENU_CONFIGURE_WLAN_LIST_FOCUSABLE_NUM   8

#define NX_MENU_CONFIGURE_WLAN_TEXT_OBJ_X     	15
#define NX_MENU_CONFIGURE_WLAN_TEXT_OBJ_Y     	15

#define NX_MENU_CONFIGURE_WLAN_IMG_LOCK_X     	670
#define NX_MENU_CONFIGURE_WLAN_IMG_LOCK_Y     	13
#define NX_MENU_CONFIGURE_WLAN_IMG_LOCK_W     	22
#define NX_MENU_CONFIGURE_WLAN_IMG_LOCK_H     	28

#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_X     	699
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_Y     	13
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_W     	33
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_H     	29

#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_X_GAP     7
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_Y     	18
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_W     	26
#define NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_H     	20

#define NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_X     930
#define NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_Y     658
#define NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_W     198
#define NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_H     22

#define NX_MENU_CONFIGURE_WLAN_IMG_OPTION_X_GAP    10
#define NX_MENU_CONFIGURE_WLAN_IMG_OPTION_Y      653 +(5)
#define NX_MENU_CONFIGURE_WLAN_IMG_OPTION_W      26
#define NX_MENU_CONFIGURE_WLAN_IMG_OPTION_H      26

#define NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_X       78
#define NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_Y       134
#define NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_W       750
#define NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_H       22

#define NX_MENU_CONFIGURE_WLAN_RECT_DIM_X           0
#define NX_MENU_CONFIGURE_WLAN_RECT_DIM_Y           630
#define NX_MENU_CONFIGURE_WLAN_RECT_DIM_W           1280
#define NX_MENU_CONFIGURE_WLAN_RECT_DIM_H           90

#define NX_MENU_CONFIGURE_WLAN_BTN_BACK_X           305
#define NX_MENU_CONFIGURE_WLAN_BTN_BACK_Y           615
#define NX_MENU_CONFIGURE_WLAN_BTN_BACK_W           260
#define NX_MENU_CONFIGURE_WLAN_BTN_BACK_H           55

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum Nx_MenuConfigureWLanUicompIds
{
    eNxMenuConfigureWLan_LIST_ID,
    eNxMenuConfigureWLan_IMG_OPTION_ID,
    eNxMenuConfigureWLan_TEXT_HIDDEN_ID,
    eNxMenuConfigureWLan_TEXT_EXPLAIN_ID,
    eNxMenuConfigureWLan_IMG_DIM_ID,
    eNxMenuConfigureWLan_IMG_AP_CHECK_ID,
    eNxMenuConfigureWLan_BTN_BACK_ID,
	eNxMenuConfigureWLan_MAX
} Nx_MenuConfigureWLanUicompIds_e;

typedef enum Nx_MenuConfigureWLanUiStatus
{
	eNxMenuConfigureWLan_Status_Init,
    eNxMenuConfigureWLan_Status_Hidden_network,
    eNxMenuConfigureWLan_Status_SearchingAP,
    eNxMenuConfigureWLan_Status_ConnectingAP,
	eNxMenuConfigureWLan_Status_MAX
} Nx_MenuConfigureWLanUiStatus_e;

typedef enum tagConfigureWLanIMG{
	eConfigureWLan_Network_WIFI_IMG_F_0 = 0,
	eConfigureWLan_Network_WIFI_IMG_F_1,
	eConfigureWLan_Network_WIFI_IMG_F_2,
	eConfigureWLan_Network_WIFI_IMG_F_3,
	eConfigureWLan_Network_WIFI_IMG_F_4,
	eConfigureWLan_Network_WIFI_IMG_N_0,
	eConfigureWLan_Network_WIFI_IMG_N_1,
	eConfigureWLan_Network_WIFI_IMG_N_2,
	eConfigureWLan_Network_WIFI_IMG_N_3,
	eConfigureWLan_Network_WIFI_IMG_N_4,
	eConfigureWLan_Network_WIFI_IMG_LOCK,
	eConfigureWLan_Network_WIFI_IMG_CHECK_C,
	eConfigureWLan_Network_WIFI_IMG_CHECK_H,
	eConfigureWLan_Network_WIFI_IMG_CHECK_N,
	eConfigureWLan_Network_WIFI_IMG_MAX
} Nx_MenuConfigureWLan_IMG_ID_t;


typedef struct tag_NX_MENU_WLAN_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8	    ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32      lPrevCurIdx;
	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

    //WLAN Information
    ONDKImageHandle hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_MAX];
    HUINT32     ulNumOfAp;
	APKS_NETAP_Info_t *apList;
	HINT32 lSelectedItemIndex;
    APKS_NETAP_Info_t *stConnectingApInfo;
    Nx_MenuConfigureWLanUiStatus_e eUiStatus;
} Nx_MenuNetworkWLanContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuNetworkWLanContent_t		s_stLanConfigureContent;
static Nx_PrismListDialog_Content_t		s_stPopupListConfigureLanConts;
static NX_HiddenApDialogInstance_t		s_stHiddenApMenu;


static NXUI_DialogInstance_t			s_stDialogMessageInstance;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	            nx_menuConfigureWLan_MsgGwmCreate(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t             	nx_menuConfigureWLan_MsgGwmDestroy(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	            nx_menuConfigureWLan_MsgGwmKeyDown(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	            nx_menuConfigureWLan_MsgGwmClicked(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	            nx_menuConfigureWLan_MsgGwmTimer(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR                   	nx_menuConfigureWLan_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize );
static HERROR                       nx_menuConfigureWLan_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr );
static Nx_MenuNetworkWLanContent_t* nx_menuConfigureWLan_GetContents(void);
static ONDK_Result_t	            nx_menuConfigureWLan_InitApplication(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t	            nx_menuConfigureWLan_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	            nx_menuConfigureWLan_DisplayHiddenApMenu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	            nx_menuConfigureWLan_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void		                    nx_menuConfigureWLan_ProcessingMsgDestroy(void);
static ONDK_Result_t	            nx_menuConfigureWLan_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void		                    nx_menuConfigureWLan_CreateResponseDialouge(HUINT8* pszString);
static HINT32	                    nx_menuConfigureWLan_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32                       nx_menuConfigureWLan_ConvertItemIdxToObjId(HINT32 lItemId);
static void                 		nx_menuConfigureWLan_UpdateCurrentTime(Nx_MenuNetworkWLanContent_t *pstContents);
static void	                    	nx_menuConfigureWLan_CreateProcessingMsg(HCHAR *pzMsg);
static void		                    nx_menuConfigureWLan_CreateHiddenApMenu(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t	            nx_menuConfigureWLan_DrawHiddenNetworkBtn(Nx_MenuNetworkWLanContent_t *pstContents);
static void                         nx_menuConfigureWLan_DrawLastDim(HBOOL bShow);
static void                         nx_menuConfigureWLan_DrawExplain(HCHAR *pText);
static ONDK_Result_t	            nx_menuConfigureWLan_DrawDefaultUI(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t	            nx_menuConfigureWLan_DrawWifiListUI(ONDKSurfaceHandle screen, ONDK_Region_t *area,ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t            	nx_menuConfigureWLan_DrawWifiList(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t	            nx_menuConfigureWLan_DoFocusablePage(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t            	nx_menuConfigureWLan_DoSearchingWifi(Nx_MenuNetworkWLanContent_t *pstContents);
static ONDK_Result_t            	nx_menuConfigureWLan_DoConnectAP(Nx_MenuNetworkWLanContent_t *pstContents, APKS_NETAP_Info_t * pstApInfo,HCHAR *keyboardRet);
static ONDK_Result_t            	nx_menuConfigureWLan_DoApToConnectedStatus(Nx_MenuNetworkWLanContent_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


static HERROR	nx_menuConfigureWLan_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

static HERROR   nx_menuConfigureWLan_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
{
	HULONG	octet[4] = { 0 };

	sscanf ( pszAddr, "%ld.%ld.%ld.%ld", &octet[0], &octet[1], &octet[2], &octet[3] );

	*pulAddr = 0;
	*pulAddr |=	octet[0];
	*pulAddr |= ( octet[1] << 8 );
	*pulAddr |= ( octet[2] << 16 );
	*pulAddr |= ( octet[3] << 24 );

	return ERR_OK;
}


static inline Nx_MenuNetworkWLanContent_t* nx_menuConfigureWLan_GetContents(void)
{
	return	&s_stLanConfigureContent;
}


static ONDK_Result_t	nx_menuConfigureWLan_InitApplication(Nx_MenuNetworkWLanContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_MenuNetworkWLanContent_t));

    pstContents->eUiStatus = eNxMenuConfigureWLan_Status_Init;

    //Loading Img
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_F_0] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI00_C_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_F_1] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI01_C_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_F_2] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI02_C_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_F_3] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI03_C_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_F_4] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI04_C_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_N_0] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI00_N_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_N_1] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI01_N_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_N_2] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI02_N_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_N_3] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI03_N_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_N_4] = ONDK_IMG_CreateSelf((HUINT8*)RES_207_06_WIFI04_N_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_LOCK] = ONDK_IMG_CreateSelf((HUINT8*)RES_211_00_LOCK_PNG);

    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_CHECK_C] = ONDK_IMG_CreateSelf((HUINT8*)RES_112_00_CHECK04_C_PNG );
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_CHECK_H] = ONDK_IMG_CreateSelf((HUINT8*)RES_112_00_CHECK04_H_PNG);
    pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_CHECK_N] = ONDK_IMG_CreateSelf((HUINT8*)RES_112_00_CHECK04_N_PNG);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureWLan_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListConfigureLanConts, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_menuConfigureWLan_DisplayHiddenApMenu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_HiddenApDlalog_ProtoProc(&s_stHiddenApMenu, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_menuConfigureWLan_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stDialogMessageInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuConfigureWLan_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_menuConfigureWLan_ProcessingMsgProc);
}

static void		nx_menuConfigureWLan_DestroyHiddenApMenu(void)
{
	ONDK_GWM_APP_Destroy(nx_menuConfigureWLan_DisplayHiddenApMenu);
}

static ONDK_Result_t	nx_menuConfigureWLan_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_menuConfigureWLan_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuConfigureWLan_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_menuConfigureWLan_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}



static HINT32	nx_menuConfigureWLan_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~NX_MENU_CONFIGURE_WLAN_ID);

	if ((lItemId < 0) || (lItemId >= eNxMenuConfigureWLan_MAX))
	{
		return -1;
	}

	return	lItemId;
}

static HINT32	nx_menuConfigureWLan_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxMenuConfigureWLan_MAX))
	{
		return 0;
	}

	return	(NX_MENU_CONFIGURE_WLAN_ID | lItemId);
}


static void		nx_menuConfigureWLan_UpdateCurrentTime(Nx_MenuNetworkWLanContent_t *pstContents)
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

static void		nx_menuConfigureWLan_CreateProcessingMsg(HCHAR *pzMsg)
{
	HUINT32 		ulDlgAttr = 0;

	HxSTD_memset (&s_stDialogMessageInstance, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
	NX_PRISM_DIALOGUE_SetDialog(&s_stDialogMessageInstance, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)pzMsg);

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Ui_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuConfigureWLan_ProcessingMsgProc, 0, 0, 0, 0);
}

static void		nx_menuConfigureWLan_CreateHiddenApMenu(Nx_MenuNetworkWLanContent_t *pstContents)
{
	NX_PRISM_COMP_HiddenApDlalog_SetHiddenAPMenu(&s_stHiddenApMenu, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
   	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuNetwork_DisplayHiddenApMenu", APP_DEFAULT_PRIORITY,
            	nx_menuConfigureWLan_DisplayHiddenApMenu, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_menuConfigureWLan_DrawHiddenNetworkBtn(Nx_MenuNetworkWLanContent_t *pstContents)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulID = 0;


    if(eNxMenuConfigureWLan_Status_Hidden_network == pstContents->eUiStatus)
    {
        ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_TEXT_HIDDEN_ID);
        ONDK_GWM_Obj_SetVisible(ulID, FALSE);
        ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_IMG_OPTION_ID);
        ONDK_GWM_Obj_SetVisible(ulID, FALSE);
    }
    else
    {
        ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_TEXT_HIDDEN_ID);
    	stRect = ONDK_Rect(NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_X,
            NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_Y,
            NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_W,
            NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_H);

     	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, (char*)ONDK_GetString(RES_HIDDEN_NETWORK_ID));
        ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
     	ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
        ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_RIGHT);
       	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong02), COL(C_T_Gong02));
        ONDK_GWM_Obj_SetVisible(ulID, TRUE);

        //IMAGE
        ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_IMG_OPTION_ID);
    	stRect = ONDK_Rect(NX_MENU_CONFIGURE_WLAN_TEXT_HIDDEN_X-
            (NX_MENU_CONFIGURE_WLAN_IMG_OPTION_X_GAP+NX_MENU_CONFIGURE_WLAN_IMG_OPTION_W),
            NX_MENU_CONFIGURE_WLAN_IMG_OPTION_Y,
            NX_MENU_CONFIGURE_WLAN_IMG_OPTION_W,
            NX_MENU_CONFIGURE_WLAN_IMG_OPTION_H);
        ONDK_GWM_COM_Image_Create(ulID, &stRect);
        ONDK_GWM_COM_Image_SetImage(ulID, RES_201_00_OPTION_PNG);
        ONDK_GWM_Obj_SetVisible(ulID, TRUE);
    }

    return eResult;
}

static void nx_menuConfigureWLan_DrawLastDim(HBOOL bShow)
{
    HUINT32 ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_IMG_DIM_ID);
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;

	stRect = ONDK_Rect(NX_MENU_CONFIGURE_WLAN_RECT_DIM_X, NX_MENU_CONFIGURE_WLAN_RECT_DIM_Y,
        NX_MENU_CONFIGURE_WLAN_RECT_DIM_W, NX_MENU_CONFIGURE_WLAN_RECT_DIM_H);

     eResult = ONDK_GWM_COM_Image_Create(ulID, &stRect);

     if(eResult==ONDK_RET_OK)
     {
         ONDK_GWM_COM_Image_SetImage(ulID, RES_105_00_BG_BOTTOM_90_PNG);
     }
     ONDK_GWM_Obj_SetVisible(ulID, bShow);
}

static void nx_menuConfigureWLan_DrawExplain(HCHAR *pText)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    ONDK_Result_t	eResult = ONDK_RET_FAIL;

    HUINT32         ulID = 0;

    ulID = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_TEXT_EXPLAIN_ID);

	stRect = ONDK_Rect(NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_X,
        NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_Y,
        NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_W,
        NX_MENU_CONFIGURE_WLAN_TEXT_EXPLAIN_H);

 	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, pText);

    if(eResult == ONDK_RET_OK)
    {
        ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemNormal);
    	ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
	    ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
    	ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong04), COL(C_T_Gong01));
    }
    else if( (eResult == GWM_RET_LINK_CONFLICT) && (NULL != pText) )
    {
        //update Text
        ONDK_GWM_COM_Text_SetText(ulID, pText);
    }
}

static ONDK_Result_t	nx_menuConfigureWLan_DrawDefaultUI(Nx_MenuNetworkWLanContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_CONFIGURE_WLAN_ID), MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuConfigureWLan_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();
    nx_menuConfigureWLan_DrawHiddenNetworkBtn(pstContents);

	return	ONDK_RET_OK;
}

ONDK_Result_t	nx_menuConfigureWLan_DrawWifiListUI(ONDKSurfaceHandle screen, ONDK_Region_t *area,
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
	HINT32				textOffset_x = NX_MENU_CONFIGURE_WLAN_TEXT_OBJ_X, textOffset_y = NX_MENU_CONFIGURE_WLAN_TEXT_OBJ_Y;
    HINT32	CursorOffset =0;

	ONDK_Color_t		color_text = COL(C_G_Whistle_100);
	ONDK_Color_t		color_focusText = COL(C_G_Bang_100);
	ONDK_Color_t		color_disableText = COL(C_G_Bang_100);

	ONDK_Color_t		color_FocusCursor = COL(C_G_Base01_100);
    ONDK_Color_t		color_Cursor = COL(C_G_Whistle_05);
	ONDK_Color_t		title_fontColor = 0x00000000;
    Nx_MenuNetworkWLanContent_t *pstContents;
    HUINT32             ulId = 0;
    HBOOL               bLastData = FALSE;

    pstContents = nx_menuConfigureWLan_GetContents();

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

    ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_LIST_ID);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ulId);

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

    if(lTotal>NX_MENU_CONFIGURE_WLAN_LIST_FOCUSABLE_NUM)
    {
        bLastData = TRUE;
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
		lCursor_y_off += lCellHeight+NX_MENU_CONFIGURE_WLAN_LIST_OBJ_GAP;
	}

    //Draw Last One
    if(TRUE == bLastData)
    {
        APKS_NETAP_Info_t *pstApInfo;
        HINT32            nextIndex = (lStart+i > (lTotal-1))? 0 : (lStart+i);

        pstApInfo = &(pstDataList[nextIndex]);
	    //CURSOR
		if(TRUE == cursorEnable)
		{
			title_fontColor = color_text;
            x = x1;
    		y = y1 + lCursor_y_off;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - CursorOffset, color_Cursor);
        }
        //TEXT
		x = x1 + textOffset_x;
		y = y1 + lCursor_y_off + textOffset_y;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (HCHAR*)pstApInfo->szESSID, x, y, w, title_fontColor, ONDKSTF_LEFT);
    }

    //CONNECTED
    lCursor_y_off = 0;

	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);
        //LAST STEP is alywas SKIP THIS STEP
        if(TRUE == pstApInfo->bIsConnected)
        {
            HUINT32 ulStrWidth =0;
            Nx_MenuConfigureWLan_IMG_ID_t imgID = eConfigureWLan_Network_WIFI_IMG_CHECK_C;
        	ulStrWidth = ONDK_FONT_GetStringWidth(hFontHandle, (HCHAR*)pstApInfo->szESSID);
        	x = x1 + textOffset_x + ulStrWidth + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_X_GAP;
        	y = y1 + lCursor_y_off + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_Y;
            w = NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_W;
            h = NX_MENU_CONFIGURE_WLAN_IMG_WIFI_CHECK_H;
			if (i + lStart == lCurrent)
            {
                imgID = eConfigureWLan_Network_WIFI_IMG_CHECK_C;
            }
            else
            {
                imgID = eConfigureWLan_Network_WIFI_IMG_CHECK_H;
            }
             ONDK_IMG_Blit(pstContents->hWifiImgHandleList[imgID], screen, x,y);
        }
        //check next y point
		lCursor_y_off += lCellHeight+NX_MENU_CONFIGURE_WLAN_LIST_OBJ_GAP;
	}


    //LOCK IMG
    lCursor_y_off = 0;

	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);
        //LAST STEP is alywas SKIP THIS STEP
        if(eInet_Wifi_Sec_Open != pstApInfo->nSecureType)
        {
        	x = x1 + NX_MENU_CONFIGURE_WLAN_IMG_LOCK_X;
        	y = y1 + lCursor_y_off + NX_MENU_CONFIGURE_WLAN_IMG_LOCK_Y;
            ONDK_IMG_Blit(pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_LOCK], screen, x,y);
        }
        //check next y point
		lCursor_y_off += lCellHeight+NX_MENU_CONFIGURE_WLAN_LIST_OBJ_GAP;
	}

    //Draw LOCK IMG Last One
    if(TRUE == bLastData)
    {
        APKS_NETAP_Info_t *pstApInfo;
        HINT32            nextIndex = (lStart+i > (lTotal-1))? 0 : (lStart+i);
        pstApInfo = &(pstDataList[nextIndex]);
        //LAST STEP is alywas SKIP THIS STEP
        if(eInet_Wifi_Sec_Open != pstApInfo->nSecureType)
        {
        	x = x1 + NX_MENU_CONFIGURE_WLAN_IMG_LOCK_X;
        	y = y1 + lCursor_y_off + NX_MENU_CONFIGURE_WLAN_IMG_LOCK_Y;
            ONDK_IMG_Blit(pstContents->hWifiImgHandleList[eConfigureWLan_Network_WIFI_IMG_LOCK], screen, x,y);
        }
    }

    //WIFI Strength
    lCursor_y_off = 0;

	for(i = 0; i < lLastIdx; i++)
	{
	    APKS_NETAP_Info_t *pstApInfo = &(pstDataList[lStart+i]);
        HINT32 strength = 0;
        Nx_MenuConfigureWLan_IMG_ID_t imgID = eConfigureWLan_Network_WIFI_IMG_F_0;
        strength = pstApInfo->nWaveStrength / 20;

    	if (i + lStart == lCurrent)
		{
            switch(strength)
            {
                case 0:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_0;
                    break;
                case 1:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_1;
                    break;
                case 2:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_2;
                    break;
                case 3:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_3;
                    break;
                default:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_4;
                    break;
            }
		}
		else
		{
            switch(strength)
            {
                case 0:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_0;
                    break;
                case 1:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_1;
                    break;
                case 2:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_2;
                    break;
                case 3:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_3;
                    break;
                default:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_4;
                    break;
            }
		}

      	x = x1 + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_X;
       	y = y1 + lCursor_y_off + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_Y;

        ONDK_IMG_Blit(pstContents->hWifiImgHandleList[imgID], screen, x,y);
        //check next y point
		lCursor_y_off += lCellHeight+NX_MENU_CONFIGURE_WLAN_LIST_OBJ_GAP;
	}

    //Draw Last One
    if(TRUE == bLastData)
    {
        APKS_NETAP_Info_t *pstApInfo;
        HINT32            nextIndex = (lStart+i > (lTotal-1))? 0 : (lStart+i);
        HINT32 strength = 0;
        Nx_MenuConfigureWLan_IMG_ID_t imgID = eConfigureWLan_Network_WIFI_IMG_F_0;

        pstApInfo = &(pstDataList[nextIndex]);
        strength = pstApInfo->nWaveStrength / 20;

    	if (i + lStart == lCurrent)
		{
            switch(strength)
            {
                case 0:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_0;
                    break;
                case 1:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_1;
                    break;
                case 2:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_2;
                    break;
                case 3:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_3;
                    break;
                default:
                    imgID = eConfigureWLan_Network_WIFI_IMG_F_4;
                    break;
            }
		}
		else
		{
            switch(strength)
            {
                case 0:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_0;
                    break;
                case 1:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_1;
                    break;
                case 2:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_2;
                    break;
                case 3:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_3;
                    break;
                default:
                    imgID = eConfigureWLan_Network_WIFI_IMG_N_4;
                    break;
            }
		}

      	x = x1 + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_X;
       	y = y1 + lCursor_y_off + NX_MENU_CONFIGURE_WLAN_IMG_WIFI_POWER_Y;

        ONDK_IMG_Blit(pstContents->hWifiImgHandleList[imgID], screen, x,y);
    }

	return ERR_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_DrawWifiSearchingFail(Nx_MenuNetworkWLanContent_t *pstContents)
{
    HUINT32 ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_BTN_BACK_ID);
	ONDK_Rect_t	stRect = {0,};
    HINT32  x = 0, y = 0, w = 0, h = 0;
    ONDK_Result_t eRet = ONDK_RET_OK;

    x = NX_MENU_CONFIGURE_WLAN_BTN_BACK_X;
    y = NX_MENU_CONFIGURE_WLAN_BTN_BACK_Y;
    w = NX_MENU_CONFIGURE_WLAN_BTN_BACK_W;
    h = NX_MENU_CONFIGURE_WLAN_BTN_BACK_H;

    stRect = ONDK_Rect(x, y , w, h);
    eRet = ONDK_GWM_COM_Button_Create (ulId, &stRect, (char*)ONDK_GetString(RES_BACK_01_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_Obj_SetFocus(ulId);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);

    nx_menuConfigureWLan_DrawExplain((char*)ONDK_GetString(RES_MESG_3827_ID));

    return eRet;
}

static ONDK_Result_t	nx_menuConfigureWLan_DrawWifiList(Nx_MenuNetworkWLanContent_t *pstContents)
{
	ONDK_Rect_t	stRect = {0,};
    HUINT32     ulId = 0;

    //WiFi list obj.
	stRect = ONDK_Rect(NX_MENU_CONFIGURE_WLAN_LIST_X, NX_MENU_CONFIGURE_WLAN_LIST_Y ,
	NX_MENU_CONFIGURE_WLAN_LIST_W, NX_MENU_CONFIGURE_WLAN_LIST_H);

    ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_LIST_ID);

	ONDK_GWM_List_Create(ulId, &stRect, "WiFi LIST", NX_MENU_CONFIGURE_WLAN_LIST_OBJ_H,
        pstContents->ulNumOfAp, NX_MENU_CONFIGURE_WLAN_LIST_FOCUSABLE_NUM,(GWM_LIST_ITEMS)pstContents->apList);
	ONDK_GWM_List_SetCurrentIndex(ulId, 0);
	ONDK_GWM_List_SetCircularMove(ulId, TRUE);
	ONDK_GWM_List_SetFontHandle(ulId, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_Obj_SetFocus(ulId);
    ONDK_GWM_APP_SetObjectDrawMethod(ulId, (GWM_ObjDraw_t)nx_menuConfigureWLan_DrawWifiListUI);

    if(pstContents->ulNumOfAp>NX_MENU_CONFIGURE_WLAN_LIST_FOCUSABLE_NUM)
    {
        nx_menuConfigureWLan_DrawLastDim(TRUE);
    }

    nx_menuConfigureWLan_DrawExplain((char*)ONDK_GetString(RES_MESG_3721_ID));

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_DoFocusablePage(Nx_MenuNetworkWLanContent_t *pstContents)
{
    HUINT32 ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_LIST_ID);
    switch(pstContents->eUiStatus)
    {
        case eNxMenuConfigureWLan_Status_Init:
            if(pstContents->ulNumOfAp>0)
            {
                ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_LIST_ID);
            }
            else
            {
                ulId = nx_menuConfigureWLan_ConvertItemIdxToObjId(eNxMenuConfigureWLan_BTN_BACK_ID);
            }
            ONDK_GWM_Obj_EnableObject(ulId);
            ONDK_GWM_Obj_SetFocus(ulId);
            break;
        default:
            ONDK_GWM_Obj_DisableObject(ulId);
            break;
    }
    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_DoSearchingWifi(Nx_MenuNetworkWLanContent_t *pstContents)
{
    //Progress Popup
    nx_menuConfigureWLan_CreateProcessingMsg((char*)ONDK_GetString(RES_CONNECTING_ID));
    NX_NETWORK_StartScan();
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_DoConnectAP(Nx_MenuNetworkWLanContent_t *pstContents, APKS_NETAP_Info_t * pstApInfo,HCHAR *keyboardRet)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

    pstContents->eUiStatus = eNxMenuConfigureWLan_Status_ConnectingAP;
    nx_menuConfigureWLan_DoFocusablePage(pstContents);

    //Progress Popup
    nx_menuConfigureWLan_CreateProcessingMsg((char*)ONDK_GetString(RES_CONNECTING_ID));

    if(NULL!=pstContents->stConnectingApInfo)
    {
        NX_APP_Free(pstContents->stConnectingApInfo);
    }

    pstContents->stConnectingApInfo = (APKS_NETAP_Info_t *)NX_APP_Malloc (sizeof(APKS_NETAP_Info_t));
    HxSTD_memcpy(pstContents->stConnectingApInfo,pstApInfo,sizeof(APKS_NETAP_Info_t));

    eResult = NX_NETWORK_ConnectAP(pstApInfo, keyboardRet);
    return eResult;
}

static ONDK_Result_t	nx_menuConfigureWLan_DoApToConnectedStatus(Nx_MenuNetworkWLanContent_t *pstContents)
{
    HUINT32  lIndex =0;
    APKS_NETAP_Info_t *stConnectingAp = NULL;
    APKS_NETAP_Info_t *stApListItem = NULL;

    if(NULL == pstContents->stConnectingApInfo)
    {
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
    }

    stConnectingAp = pstContents->stConnectingApInfo;

    for(lIndex = 0; lIndex < pstContents->ulNumOfAp ; lIndex++)
    {
        stApListItem = &(pstContents->apList[lIndex]);
        if( (HxSTD_StrCmp(stApListItem->szESSID, stConnectingAp->szESSID) == 0)&&
            (HxSTD_StrCmp(stApListItem->szMacAddr, stConnectingAp->szMacAddr) == 0) &&
            (TRUE == stConnectingAp->bIsConnected)
          )
        {
            stApListItem->bIsConnected = TRUE;
        }
        else
        {
            stApListItem->bIsConnected = FALSE;
        }
    }
    ONDK_GWM_APP_InvalidateRelativeRect(NX_MENU_CONFIGURE_WLAN_LIST_X,NX_MENU_CONFIGURE_WLAN_LIST_Y,
        NX_MENU_CONFIGURE_WLAN_LIST_W,NX_MENU_CONFIGURE_WLAN_LIST_H);

    return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmCreate(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HERROR          hError  = ERR_FAIL;
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);
	pstContents->lPrevCurIdx = p3;
    hError = nx_menuConfigureWLan_InitApplication(pstContents);

    if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	// Draw Menu Item
	nx_menuConfigureWLan_DrawDefaultUI(pstContents);

    NX_NETWORK_RegisterNotifier();

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);
    NX_NETWORK_RegisterNotifier();
    nx_menuConfigureWLan_DoSearchingWifi(pstContents);
    nx_menuConfigureWLan_DrawLastDim(FALSE);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmDestroy(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HINT32  lIndex = 0;
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);


    for(lIndex = 0; lIndex<eConfigureWLan_Network_WIFI_IMG_MAX;lIndex++)
    {
    	ONDK_IMG_Destroy(pstContents->hWifiImgHandleList[lIndex]);
    }

    if(pstContents->ulNumOfAp>0)
    {
        NX_APP_Free(pstContents->apList);
    }

    if(NULL != pstContents->stConnectingApInfo)
    {
        NX_APP_Free(pstContents->stConnectingApInfo);
    }

    if(eNxMenuConfigureWLan_Status_Hidden_network == pstContents->eUiStatus)
    {
        nx_menuConfigureWLan_DestroyHiddenApMenu();
    }

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuNetworkWLanContent_t));

    NX_NETWORK_UnRegisterNotifier();
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmKeyDown(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0,ulConvertedID;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         lObjId =0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

    lObjId = GWM_GetFocusedObjectID();
    ulConvertedID = nx_menuConfigureWLan_ConvertObjIdToItemIdx(lObjId);
	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);

	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

    if(pstContents->eUiStatus == eNxMenuConfigureWLan_Status_SearchingAP)
    {
        return	ONDK_MESSAGE_BREAK;
    }

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
	        lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEMNU_Network_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
	                (ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc, 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;
		case KEY_OK:
            if(eNxMenuConfigureWLan_BTN_BACK_ID == ulConvertedID)
            {
    	        lPrevCurIdx = pstContents->lPrevCurIdx;
    			ONDK_GWM_APP_Destroy(0);
    			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEMNU_Network_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
    	                (ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc, 0, 0, 0, lPrevCurIdx);
    			return	ONDK_MESSAGE_BREAK;
            }
            break;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
            break;
		case KEY_ARROWUP:
            NX_APP_Print("KEY_ARROWUP");
            break;
		case KEY_ARROWDOWN:
            NX_APP_Print("KEY_ARROWDOWN");
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;
		case KEY_OPT:
            pstContents->eUiStatus=eNxMenuConfigureWLan_Status_Hidden_network;
            nx_menuConfigureWLan_CreateHiddenApMenu(pstContents);
            return	ONDK_MESSAGE_BREAK;
		default:
			NX_APP_Print("nx_menuSysInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuConfigureWLan_MsgGwmClicked(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch ((HUINT32)p1)
	{
	default:
		break;
	}
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmTimer(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    switch(p1)
    {
        case NX_SETTING_TIMEOUT_ID:
            ONDK_GWM_APP_Destroy(0);
            break;
        case NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID:
    		nx_menuConfigureWLan_UpdateCurrentTime(pstContents);
	    	NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
            break;
        default:
            break;
    }

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}



static ONDK_Result_t    nx_menuConfigureWLan_MsgGwmListSelected(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lListIdx = (p2);
	APKS_NETAP_Info_t *apList = pstContents->apList;

    pstContents->lSelectedItemIndex = (lListIdx);

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
        nx_menuConfigureWLan_DoConnectAP(pstContents, pstApInfo,"");
    }

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmFocusChanged(Nx_MenuNetworkWLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HINT32			lObjId = 0;
    HINT32			lConvertedID = 0;
    if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = GWM_GetFocusedObjectID();
    lConvertedID = nx_menuConfigureWLan_ConvertObjIdToItemIdx(lObjId);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

    return eResult;

}

static ONDK_Result_t	nx_menuConfigureWLan_MsgGwmAPKMessage(Nx_MenuNetworkWLanContent_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e   eEvent = p1;
    APKS_NETAP_Info_t   *apList;
    HINT32 lNumOfAp      = p3;
    HCHAR*              strSuccess = (HCHAR*)ONDK_GetString(RES_MESG_3242_ID);
    HCHAR*              strFailed = (HCHAR*)ONDK_GetString(RES_MESG_360_ID);

    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
            case eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_AP_CONNECT_SUCCESS \n\n");
                pstContents->eUiStatus = eNxMenuConfigureWLan_Status_Init;
                nx_menuConfigureWLan_ProcessingMsgDestroy();
                nx_menuConfigureWLan_DoFocusablePage(pstContents);
                nx_menuConfigureWLan_CreateResponseDialouge((HUINT8*)strSuccess);
                if(NULL != pstContents->stConnectingApInfo)
                {
                    pstContents->stConnectingApInfo->bIsConnected = TRUE;
                }
                nx_menuConfigureWLan_DoApToConnectedStatus(pstContents);
                break;
            case eNETWORK_STATUS_WIFI_KEY_INVALID:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_KEY_INVALID \n\n");
                pstContents->eUiStatus = eNxMenuConfigureWLan_Status_Init;
                nx_menuConfigureWLan_ProcessingMsgDestroy();
                nx_menuConfigureWLan_DoFocusablePage(pstContents);
                nx_menuConfigureWLan_CreateResponseDialouge((HUINT8*)strFailed);
                break;
            case eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL:
                HxLOG_Error("\n\n####### eNETWORK_STATUS_WIFI_AP_CONNECT_FAIL \n\n");
                pstContents->eUiStatus = eNxMenuConfigureWLan_Status_Init;
                nx_menuConfigureWLan_ProcessingMsgDestroy();
                nx_menuConfigureWLan_DoFocusablePage(pstContents);
                nx_menuConfigureWLan_CreateResponseDialouge((HUINT8*)strFailed);
                if(NULL != pstContents->stConnectingApInfo)
                {
                    pstContents->stConnectingApInfo->bIsConnected = FALSE;
                }
                nx_menuConfigureWLan_DoApToConnectedStatus(pstContents);
                break;
            case eNETWORK_STATUS_WIFI_SCANNING_END:
                nx_menuConfigureWLan_ProcessingMsgDestroy();

                if(lNumOfAp>0)
                {
                    apList = (APKS_NETAP_Info_t *)NX_APP_Malloc (sizeof(APKS_NETAP_Info_t) * (lNumOfAp));
                    HxSTD_memset(apList,0,sizeof(APKS_NETAP_Info_t)* (lNumOfAp));

                    HxSTD_memcpy(apList,(APKS_NETAP_Info_t *)p2, sizeof(APKS_NETAP_Info_t)*lNumOfAp);
                    pstContents->apList = apList;
                }

                pstContents->ulNumOfAp = (lNumOfAp) ;
                if(lNumOfAp>0)
                {
                    nx_menuConfigureWLan_DrawWifiList(pstContents);
                }
                else
                {
                    nx_menuConfigureWLan_DrawWifiSearchingFail(pstContents);
                }
                pstContents->eUiStatus= eNxMenuConfigureWLan_Status_Init;
                nx_menuConfigureWLan_DoFocusablePage(pstContents);
                break;
            case eNETWORK_STATUS_WIFI_SCANNING_FAIL:
                pstContents->eUiStatus= eNxMenuConfigureWLan_Status_Init;
                nx_menuConfigureWLan_DoFocusablePage(pstContents);
                //Draw Wireless network is not detected
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
ONDK_Result_t	NX_PRISM_MENU_ConfigureWLan_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuNetworkWLanContent_t	*pstContents = NULL;

	pstContents = nx_menuConfigureWLan_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuConfigureWLan_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuConfigureWLan_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuConfigureWLan_MsgGwmClicked(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_OBJ_FOCUS_CHANGED:
            eResult = nx_menuConfigureWLan_MsgGwmFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuConfigureWLan_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuConfigureWLan_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_menuConfigureWLan_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
            break;
        case MSG_GWM_LIST_SELECTED:
            eResult =nx_menuConfigureWLan_MsgGwmListSelected(pstContents,p1, p2, p3);
            break;
        case MSG_APP_KEYBOARD_RESULT:
            {
                HCHAR 	*pName = NULL;
                APKS_NETAP_Info_t *pstApInfo;
                if(pstContents->eUiStatus!= eNxMenuConfigureWLan_Status_Hidden_network )
                {
                    pstApInfo = &(pstContents->apList[pstContents->lSelectedItemIndex]);
        			pName = NX_PRISM_KeyBoard_GetResultString();
                    nx_menuConfigureWLan_DoConnectAP(pstContents, pstApInfo,pName);
                    NX_APP_Free(pName);
                }

                NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
            }
			break;
        case MSG_APP_NETWORK_HIDDEN_RESULT:
            pstContents->eUiStatus = eNxMenuConfigureWLan_Status_Init;
            nx_menuConfigureWLan_DoFocusablePage(pstContents);
            if(p1 == TRUE)
            {
                nx_menuConfigureWLan_DoConnectAP(pstContents, (APKS_NETAP_Info_t*)p2,(HCHAR*)p3);
            }
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
#endif//CONFIG_MW_INET_WLAN

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

