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
 * @file	  		nx_prism_menu_connection_status.c
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
#if defined (CONFIG_MW_INET)
/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_CONNECTION_STATUS_ID					eNxSetting_ConnectionStatus

#define NX_MENU_CONNECTION_STATUS_INFO_BOX_X            60
#define NX_MENU_CONNECTION_STATUS_INFO_BOX_Y            114
#define NX_MENU_CONNECTION_STATUS_INFO_BOX_W            750
#define NX_MENU_CONNECTION_STATUS_INFO_BOX_H            298

#define NX_MENU_CONNECTION_STATUS_INFO_BOX2_X           60
#define NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y           427
#define NX_MENU_CONNECTION_STATUS_INFO_BOX2_W           750
#define NX_MENU_CONNECTION_STATUS_INFO_BOX2_H           110

#define NX_MENU_CONNECTION_STATUS_LABEL_X               18
#define NX_MENU_CONNECTION_STATUS_LABEL_Y               18
#define NX_MENU_CONNECTION_STATUS_LABEL_W               450
#define NX_MENU_CONNECTION_STATUS_LABEL_H               22
#define NX_MENU_CONNECTION_STATUS_LABEL_LS              40

#define NX_MENU_CONNECTION_STATUS_LABEL_VALUE_X               482
#define NX_MENU_CONNECTION_STATUS_LABEL_VALUE_Y               18
#define NX_MENU_CONNECTION_STATUS_LABEL_VALUE_W               250
#define NX_MENU_CONNECTION_STATUS_LABEL_VALUE_H               22
#define NX_MENU_CONNECTION_STATUS_LABEL_VALUE_LS              40

#define NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_X            30
#define NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_Y            15
#define NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_W            690
#define NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_H            56

#define NX_MENU_CONNECTION_STATUS_TEXT_STB_X            15
#define NX_MENU_CONNECTION_STATUS_TEXT_STB_Y            83
#define NX_MENU_CONNECTION_STATUS_TEXT_STB_W            170
#define NX_MENU_CONNECTION_STATUS_TEXT_STB_H            20

#define NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_X         295
#define NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_Y         (NX_MENU_CONNECTION_STATUS_TEXT_STB_Y)
#define NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_W         170
#define NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_H         20

#define NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_X       575
#define NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_Y       (NX_MENU_CONNECTION_STATUS_TEXT_STB_Y)
#define NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_W       170
#define NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_H       20

#define	NX_MENU_CONNECTION_STATUS_SUB_ITEM_MAX_NUM      7

#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_X            305
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_Y    		615
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_W	    	260
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_H		    55


#define	NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR	        (16)
#define	NX_MENU_CONNECTION_STATUS_MAX_MACADDR       	(20)
#define NX_MENU_CONNECTION_STATUS_MAX_IP6ADDR       	(40)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum Nx_MenuConnectionStatusUicompIds
{
	eNxMenuConnectionStatus_TEXT_LABEL_Ethernet_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_Internet_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_IpAddress_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_Netmask_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_Gateway_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_PrimaryDNS_ID,
	eNxMenuConnectionStatus_TEXT_LABEL_SecondaryDNS_ID,

	eNxMenuConnectionStatus_TEXT_VALUE_Ethernet_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_Internet_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_IpAddress_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_Netmask_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_Gateway_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_PrimaryDNS_ID,
	eNxMenuConnectionStatus_TEXT_VALUE_SecondaryDNS_ID,

	eNxMenuConnectionStatus_RECT_INFOBOX1_ID,

	eNxMenuConnectionStatus_RECT_INFOBOX2_ID,
	eNxMenuConnectionStatus_IMG_CONNECTION_STATUS01_ID,
	eNxMenuConnectionStatus_IMG_CONNECTION_STATUS02_ID,
	eNxMenuConnectionStatus_IMG_CONNECTION_STATUS03_ID,
    eNxMenuConnectionStatus_IMG_CONNECTION_STATUS04_ID,
	eNxMenuConnectionStatus_TEXT_STB_ID,
	eNxMenuConnectionStatus_TEXT_ROUTER_ID,
	eNxMenuConnectionStatus_TEXT_INTERNET_ID,

	eNxMenuConnectionStatus_BTN_DONE_ID,

	eNxMenuConnectionStatus_MAX
} Nx_MenuConnectionStatusUicompIds_e;

typedef enum Nx_MenuConnectionStatusUiStatus
{
	eNxMenuConnectionStatus_Status_Init,
	eNxMenuConnectionStatus_Status_Checking,
	eNxMenuConnectionStatus_Status_DONE,
	eNxMenuConnectionStatus_Status_RETRY,
	eNxMenuConnectionStatus_Status_MAX
} Nx_MenuConnectionStatusUiStatus_e;

typedef struct tag_NX_MENU_CONNECTION_STATUS_CONTENT
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

    //Information
    HBOOL   bEthernet;
    HBOOL   bInternet;
	HUINT8		**szItemTextList;
    NX_NET_NetIFADDR_t *pstNetIf;
	HCHAR 	pszIpAddr[NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR];
	HCHAR 	pszNetmask[NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR];
	HCHAR 	pszGateway[NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR];
	HCHAR 	pszDns1[NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR];
	HCHAR 	pszDns2[NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR];
    Nx_MenuConnectionStatusUiStatus_e eUiStatus;
    NX_NET_NetDevInfo_t *pstNetDevInfoList;
    NX_NET_NetIfType_e eCurrentIfType;
} Nx_MenuConnectionStatusContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuConnectionStatusContent_t		s_stLanConfigureContent;
static NXUI_DialogInstance_t			s_stDialogMessageInstance;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static HERROR	        nx_menuConnectionStatus_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize );
static HERROR           nx_menuConnectionStatus_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr );
static ONDK_Result_t	nx_menuConnectionStatus_InitApplication(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static inline Nx_MenuConnectionStatusContent_t* nx_menuConnectionStatus_GetContents(void);
static ONDK_Result_t	nx_menuConnectionStatus_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void		        nx_menuConnectionStatus_CreateProcessingMsg(HCHAR *pzMsg);
static void		        nx_menuConnectionStatus_ProcessingMsgDestroy(void);
static ONDK_Result_t	nx_menuConnectionStatus_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static HINT32	        nx_menuConnectionStatus_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32       	nx_menuConnectionStatus_ConvertItemIdxToObjId(HINT32 lItemId);
static void		        nx_menuConnectionStatus_UpdateCurrentTime(Nx_MenuConnectionStatusContent_t *pstContents);
static HCHAR*           nx_menuConnectionStatus_GetLabelName(Nx_MenuConnectionStatusUicompIds_e index);
static ONDK_Result_t	nx_menuConnectionStatus_GetConfigureLanItemsValue(Nx_MenuConnectionStatusContent_t *pstContents,Nx_MenuConnectionStatusUicompIds_e ulId,HCHAR** pszBuf);
static void		        nx_menuConnectionStatus_CreateResponseDialouge(HUINT8* pszString);
static ONDK_Result_t	nx_menuConnectionStatus_DrawMainTitle(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawInfoBox(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawLabel(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DoUpdatingNetworkStatus(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawImageConnection(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawTextConnection(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawLabelValue(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawDoneBtn(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_DrawDefaultUI(Nx_MenuConnectionStatusContent_t *pstContents);
static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmCreate(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmDestroy(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmKeyDown(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuConnectionStatus_MsgGwmClicked(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmTimer(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmAPKMessage(Nx_MenuConnectionStatusContent_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


static HERROR	nx_menuConnectionStatus_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

static HERROR   nx_menuConnectionStatus_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
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

static ONDK_Result_t	nx_menuConnectionStatus_InitApplication(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_MenuConnectionStatusContent_t));
    ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);
	pstContents->lPrevCurIdx = p3;
    pstContents->pstNetIf = NULL;
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_NETWORK_CONNECTION_STATUS_ID), MAX_ELEMENT_TEXT_LENGTH);

	return ERR_OK;
}

static inline Nx_MenuConnectionStatusContent_t* nx_menuConnectionStatus_GetContents(void)
{
	return	&s_stLanConfigureContent;
}

static ONDK_Result_t	nx_menuConnectionStatus_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stDialogMessageInstance, message, hAction, p1, p2, p3);
}

static void		nx_menuConnectionStatus_CreateProcessingMsg(HCHAR *pzMsg)
{
	HUINT32 	ulDlgAttr = 0;

	HxSTD_memset (&s_stDialogMessageInstance, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
	NX_PRISM_DIALOGUE_SetDialog(&s_stDialogMessageInstance, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)pzMsg);

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuConnectionStatus_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuConnectionStatus_ProcessingMsgProc, 0, 0, 0, 0);
}

static void		nx_menuConnectionStatus_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_menuConnectionStatus_ProcessingMsgProc);
}

static ONDK_Result_t	nx_menuConnectionStatus_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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

static HINT32	nx_menuConnectionStatus_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~NX_MENU_CONNECTION_STATUS_ID);

	if ((lItemId < 0) || (lItemId >= eNxMenuConnectionStatus_MAX))
	{
		return -1;
	}

	return	lItemId;
}

static HINT32	nx_menuConnectionStatus_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxMenuConnectionStatus_MAX))
	{
		return 0;
	}

	return	(NX_MENU_CONNECTION_STATUS_ID | lItemId);
}


static void		nx_menuConnectionStatus_UpdateCurrentTime(Nx_MenuConnectionStatusContent_t *pstContents)
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

static HCHAR*   nx_menuConnectionStatus_GetLabelName(Nx_MenuConnectionStatusUicompIds_e index)
{

    switch(index)
    {
        case    eNxMenuConnectionStatus_TEXT_LABEL_Ethernet_ID:
            return (HCHAR*)ONDK_GetString(RES_ETHERNET_06_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_Internet_ID:
            return (HCHAR*)ONDK_GetString(RES_INTERNET_04_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_IpAddress_ID:
            return (HCHAR*)ONDK_GetString(RES_IP_ADDRESS_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_Netmask_ID:
            return (HCHAR*)ONDK_GetString(RES_NETMASK_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_Gateway_ID:
            return (HCHAR*)ONDK_GetString(RES_GATEWAY_ADDRESS_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_PrimaryDNS_ID:
            return (HCHAR*)ONDK_GetString(RES_PRIMARY_DNS_SERVER_ID);
        case	eNxMenuConnectionStatus_TEXT_LABEL_SecondaryDNS_ID:
            return (HCHAR*)ONDK_GetString(RES_SECONDARY_DNS_SERVER_ID);
        default:
            return "";
    }
}

static ONDK_Result_t	nx_menuConnectionStatus_GetConfigureLanItemsValue(Nx_MenuConnectionStatusContent_t *pstContents,Nx_MenuConnectionStatusUicompIds_e ulId,HCHAR** pszBuf)
{
    NX_NET_NetIFADDR_t *pstNetIf;
    HCHAR   *pszRet = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	if (NULL != *pszBuf)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    pstNetIf = pstContents->pstNetIf;

    if(FALSE == pstNetIf->bIpv6Mode)
    {
        //IP4
        switch(ulId)
        {
            case eNxMenuConnectionStatus_TEXT_VALUE_Ethernet_ID:
                if(TRUE == pstContents->bEthernet)
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_CONNECTED_ID);
                }
                else
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_DISCONNECTED_ID);
                }
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_Internet_ID:
                if(TRUE == pstContents->bInternet)
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_CONNECTED_ID);
                }
                else
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_DISCONNECTED_ID);
                }
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_IpAddress_ID:
                pszRet = pstContents->pszIpAddr;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                nx_menuConnectionStatus_AddrN2A(pstNetIf->ulIpAddr,pszRet,NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_Netmask_ID:
                pszRet = pstContents->pszNetmask;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                nx_menuConnectionStatus_AddrN2A(pstNetIf->ulNetmask,pszRet,NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_Gateway_ID:
                pszRet = pstContents->pszGateway;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                nx_menuConnectionStatus_AddrN2A(pstNetIf->ulGateway,pszRet,NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_PrimaryDNS_ID:
                pszRet = pstContents->pszDns1;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                nx_menuConnectionStatus_AddrN2A(pstNetIf->ulDns1,pszRet,NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
               break;
            case eNxMenuConnectionStatus_TEXT_VALUE_SecondaryDNS_ID:
                pszRet = pstContents->pszDns2;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                nx_menuConnectionStatus_AddrN2A(pstNetIf->ulDns2,pszRet,NX_MENU_CONNECTION_STATUS_MAX_IP4ADDR);
                break;
            default:
                break;
        }
    }
    *pszBuf = pszRet;
	return	ONDK_RET_OK;
}

static void		nx_menuConnectionStatus_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuConnectionStatus_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_menuConnectionStatus_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


static ONDK_Result_t	nx_menuConnectionStatus_DrawMainTitle(Nx_MenuConnectionStatusContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_CONFIGURE_LAN_01_ID), MAX_ELEMENT_TEXT_LENGTH);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawInfoBox(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HUINT32     convertedId = 0;
    ONDK_Rect_t stRect = {0,};
    HINT32  x = 0, y = 0, w = 0, h = 0;

    //Draw Info Box1
    convertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_RECT_INFOBOX1_ID);

    x = NX_MENU_CONNECTION_STATUS_INFO_BOX_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX_Y;
    w = NX_MENU_CONNECTION_STATUS_INFO_BOX_W;
    h = NX_MENU_CONNECTION_STATUS_INFO_BOX_H;
    stRect = ONDK_Rect(x,y,w,h);

    ONDK_GWM_COM_Rect_Create(convertedId, &stRect, COL(C_G_Bang_50));
	ONDK_GWM_COM_Rect_SetStroke(convertedId,1,COL(C_G_Whistle_15));

    //Draw Info Box2

    convertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_RECT_INFOBOX2_ID);

    x = NX_MENU_CONNECTION_STATUS_INFO_BOX2_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y;
    w = NX_MENU_CONNECTION_STATUS_INFO_BOX2_W;
    h = NX_MENU_CONNECTION_STATUS_INFO_BOX2_H;
    stRect = ONDK_Rect(x,y,w,h);

    ONDK_GWM_COM_Rect_Create(convertedId, &stRect, COL(C_G_Bang_50));
	ONDK_GWM_COM_Rect_SetStroke(convertedId,1,COL(C_G_Whistle_15));

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawLabel(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HINT32  lIndex = 0;
    Nx_MenuConnectionStatusUicompIds_e ulId = eNxMenuConnectionStatus_TEXT_LABEL_Ethernet_ID;
    ONDK_Rect_t stRect = {0,};
    HINT32  x = 0, y = 0, w = 0, h = 0;
    HCHAR*  pszLabel;
    HUINT32 _ulID;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    //Draw Label

    x = NX_MENU_CONNECTION_STATUS_INFO_BOX_X + NX_MENU_CONNECTION_STATUS_LABEL_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX_Y + NX_MENU_CONNECTION_STATUS_LABEL_Y;
    w = NX_MENU_CONNECTION_STATUS_LABEL_W;
    h = NX_MENU_CONNECTION_STATUS_LABEL_H;

    for(lIndex = 0 ; lIndex<NX_MENU_CONNECTION_STATUS_SUB_ITEM_MAX_NUM;lIndex++)
    {
        stRect = ONDK_Rect(x,y,w,h);
        pszLabel = nx_menuConnectionStatus_GetLabelName(ulId);

        _ulID = nx_menuConnectionStatus_ConvertItemIdxToObjId(ulId);
        ONDK_GWM_COM_Text_Create(_ulID, &stRect, pszLabel);
    	ONDK_GWM_COM_Text_SetFont(_ulID, eFont_SystemNormal);
    	ONDK_GWM_COM_Text_SetSize(_ulID, NX_PRISM_FONT_SIZE_22);
    	ONDK_GWM_COM_Text_SetAlign(_ulID,TEXT_ALIGN_LEFT | TEXT_ALIGN_MIDDLE);
    	ONDK_GWM_COM_Text_ChangeColor(_ulID, COL(C_T_Gong03), COL(C_T_Gong03));
        ONDK_GWM_Obj_SetVisible(_ulID, TRUE);
        ONDK_GWM_Obj_DisableObject(_ulID);

        y+=(NX_MENU_CONNECTION_STATUS_LABEL_LS);
        ulId++;
    }

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DoUpdatingNetworkStatus(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HERROR          hError  = ERR_FAIL;
    NX_NET_NetDevInfo_t *pstNetDevInfoList = NULL;
    HINT32              devCount = 0;
    HINT32              lIndex = 0;
    NX_NET_NetIfType_e netIfType = eNxNet_INTERFACE_TYPE_LAN;
    NX_NET_NetIFADDR_t *pstNetIf = NULL;

    hError = NX_NETWORK_GetNetDevInfo(&pstNetDevInfoList, &devCount);

    //Get Network Info
    for(lIndex=0; lIndex < devCount;lIndex++)
    {
        if(TRUE == pstNetDevInfoList[lIndex].bConnected)
        {
            if(eNxNet_INTERFACE_TYPE_LAN == pstNetDevInfoList[lIndex].eType)
            {
                hError = NX_NETWORK_GetLanInterface(&(pstNetIf));
                netIfType = eNxNet_INTERFACE_TYPE_LAN;
                break;
            }
            else if(eNxNet_INTERFACE_TYPE_WLAN == pstNetDevInfoList[lIndex].eType)
            {
                hError = NX_NETWORK_GetWLanInterface(&(pstNetIf));
                netIfType = eNxNet_INTERFACE_TYPE_WLAN;
                break;
            }
        }
    }

    pstContents->eCurrentIfType = netIfType;

    if(NULL != pstContents->pstNetDevInfoList)
    {
        NX_APP_Free(pstContents->pstNetDevInfoList);
        pstContents->pstNetDevInfoList = NULL;
    }

    if(NULL != pstContents->pstNetIf)
    {
        NX_APP_Free(pstContents->pstNetIf);
        pstContents->pstNetIf = NULL;
    }



    if(NULL == pstNetIf)
    {
        pstNetIf = (NX_NET_NetIFADDR_t*)NX_APP_Malloc(sizeof(NX_NET_NetIFADDR_t));
        HxSTD_MemSet(pstNetIf, 0x00, sizeof(NX_NET_NetIFADDR_t));

        pstContents->bEthernet = FALSE;
        pstContents->bInternet = FALSE;
        //Network Connection has failed.
        pstContents->eUiStatus = eNxMenuConnectionStatus_Status_RETRY;
    }

    pstContents->pstNetIf = pstNetIf;
    pstContents->pstNetDevInfoList = pstNetDevInfoList;

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(Nx_MenuConnectionStatusContent_t *pstContents)
{
    Nx_MenuConnectionStatusUicompIds_e ulId = eNxMenuConnectionStatus_IMG_CONNECTION_STATUS01_ID;
    HULONG ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS01_ID);
    HINT32  lIndex = 0;
    HCHAR   *pzRes = NULL;
    HINT32  bEthernet = pstContents->bEthernet, bInternet = pstContents->bInternet;
    //Initialize
    for(lIndex = 0; lIndex <4;lIndex++)
    {
        ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(ulId);
        ONDK_GWM_Obj_SetVisible(ulConvertedId, FALSE);
        ulId++;
    }

    if(eNxMenuConnectionStatus_Status_Init == pstContents->eUiStatus)
    {
        pzRes = RES_433_06_NETWORK_STAUS01_PNG;
        ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS01_ID);
    }
    else if((TRUE == bEthernet) && (TRUE == bInternet))
    {
        pzRes = RES_433_06_NETWORK_STAUS02_PNG;
        ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS02_ID);
    }
    else if((TRUE == bEthernet) && (FALSE == bInternet))
    {
        pzRes = RES_433_06_NETWORK_STAUS03_PNG;
        ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS03_ID);
    }
    else if((FALSE == bEthernet) && (FALSE == bInternet))
    {
        pzRes = RES_433_06_NETWORK_STAUS04_PNG;
        ulConvertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS04_ID);
    }

    ONDK_GWM_COM_Image_SetImage(ulConvertedId, pzRes);
    ONDK_GWM_Obj_SetVisible(ulConvertedId, TRUE);

    nx_menuConnectionStatus_DrawLabelValue(pstContents);
    nx_menuConnectionStatus_DrawDoneBtn(pstContents);
    return GWM_RET_OK;
}


static ONDK_Result_t	nx_menuConnectionStatus_DrawImageConnection(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HULONG ulId = 0;
    ONDK_Rect_t stRect = {0,};
    HINT32  x=0,y=0,w=0,h=0;
    ONDK_Result_t ret = GWM_RET_OK;

    x = NX_MENU_CONNECTION_STATUS_INFO_BOX2_X +  NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y +  NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_Y;
    w = NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_W;
    h = NX_MENU_CONNECTION_STATUS_IMG_CONNECTION_H;
	stRect = ONDK_Rect(x,y,w,h);

    ulId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS01_ID);
    ret = ONDK_GWM_COM_Image_Create(ulId, &stRect);
    if(ret == GWM_RET_OK)
    {
        ONDK_GWM_COM_Image_SetImage(ulId, RES_433_06_NETWORK_STAUS01_PNG);
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
        ONDK_GWM_Obj_DisableObject(ulId);
    }


    ulId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS02_ID);
    ret = ONDK_GWM_COM_Image_Create(ulId, &stRect);
    if(ret == GWM_RET_OK)
    {
        ONDK_GWM_COM_Image_SetImage(ulId, RES_433_06_NETWORK_STAUS02_PNG);
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
        ONDK_GWM_Obj_DisableObject(ulId);
    }

    ulId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS03_ID);
    ret = ONDK_GWM_COM_Image_Create(ulId, &stRect);
    if(ret == GWM_RET_OK)
    {
        ONDK_GWM_COM_Image_SetImage(ulId, RES_433_06_NETWORK_STAUS03_PNG);
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
        ONDK_GWM_Obj_DisableObject(ulId);
    }

    ulId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_IMG_CONNECTION_STATUS04_ID);
    ret = ONDK_GWM_COM_Image_Create(ulId, &stRect);
    if(ret == GWM_RET_OK)
    {
        ONDK_GWM_COM_Image_SetImage(ulId, RES_433_06_NETWORK_STAUS04_PNG);
        ONDK_GWM_Obj_SetVisible(ulId, FALSE);
        ONDK_GWM_Obj_DisableObject(ulId);
    }
    nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(pstContents);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawTextConnection(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HUINT32     convertedId = 0;
    ONDK_Rect_t stRect = {0,};
    HINT32  x = 0, y = 0, w = 0, h = 0;

    //STB
    x = NX_MENU_CONNECTION_STATUS_INFO_BOX2_X +  NX_MENU_CONNECTION_STATUS_TEXT_STB_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y +  NX_MENU_CONNECTION_STATUS_TEXT_STB_Y;
    w = NX_MENU_CONNECTION_STATUS_TEXT_STB_W;
    h = NX_MENU_CONNECTION_STATUS_TEXT_STB_H;
    stRect = ONDK_Rect(x,y,w,h);

    convertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_TEXT_STB_ID);
    ONDK_GWM_COM_Text_Create(convertedId, &stRect, (HCHAR *)ONDK_GetString(RES_STB_ID));

   	ONDK_GWM_COM_Text_SetFont(convertedId, eFont_SystemNormal);
   	ONDK_GWM_COM_Text_SetSize(convertedId, NX_PRISM_FONT_SIZE_20);
   	ONDK_GWM_COM_Text_SetAlign(convertedId,TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
   	ONDK_GWM_COM_Text_ChangeColor(convertedId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_SetVisible(convertedId, TRUE);
    ONDK_GWM_Obj_DisableObject(convertedId);

    //Router
    x = NX_MENU_CONNECTION_STATUS_INFO_BOX2_X +  NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y +  NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_Y;
    w = NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_W;
    h = NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_H;
    stRect = ONDK_Rect(x,y,w,h);

    convertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_TEXT_ROUTER_ID);
    ONDK_GWM_COM_Text_Create(convertedId, &stRect, (HCHAR *)ONDK_GetString(RES_ROUTER_04_ID));

   	ONDK_GWM_COM_Text_SetFont(convertedId, eFont_SystemNormal);
   	ONDK_GWM_COM_Text_SetSize(convertedId, NX_PRISM_FONT_SIZE_20);
   	ONDK_GWM_COM_Text_SetAlign(convertedId,TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
   	ONDK_GWM_COM_Text_ChangeColor(convertedId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_SetVisible(convertedId, TRUE);
    ONDK_GWM_Obj_DisableObject(convertedId);

    //Internet
    x = NX_MENU_CONNECTION_STATUS_INFO_BOX2_X +  NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX2_Y +  NX_MENU_CONNECTION_STATUS_TEXT_INTERNET_Y;
    w = NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_W;
    h = NX_MENU_CONNECTION_STATUS_TEXT_ROUTER_H;
    stRect = ONDK_Rect(x,y,w,h);

    convertedId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_TEXT_INTERNET_ID);
    ONDK_GWM_COM_Text_Create(convertedId, &stRect, (HCHAR *)ONDK_GetString(RES_INTERNET_ID));

   	ONDK_GWM_COM_Text_SetFont(convertedId, eFont_SystemNormal);
   	ONDK_GWM_COM_Text_SetSize(convertedId, NX_PRISM_FONT_SIZE_20);
   	ONDK_GWM_COM_Text_SetAlign(convertedId,TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE);
   	ONDK_GWM_COM_Text_ChangeColor(convertedId, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_Obj_SetVisible(convertedId, TRUE);
    ONDK_GWM_Obj_DisableObject(convertedId);

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawLabelValue(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HINT32  lIndex = 0;
    Nx_MenuConnectionStatusUicompIds_e ulId = eNxMenuConnectionStatus_TEXT_VALUE_Ethernet_ID;
    HCHAR*  pszValue = NULL;
    NX_NET_NetIFADDR_t *pstNetIf;
    ONDK_Rect_t stRect = {0,};
    HINT32  x = 0, y = 0, w = 0, h = 0;
    HUINT32 _ulID = 0;
    ONDK_Color_t    textColor = COL(C_T_Gong03);
    ONDK_Result_t ret = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    pstNetIf = pstContents->pstNetIf;

    //Draw Label

    x = NX_MENU_CONNECTION_STATUS_INFO_BOX_X + NX_MENU_CONNECTION_STATUS_LABEL_VALUE_X;
    y = NX_MENU_CONNECTION_STATUS_INFO_BOX_Y + NX_MENU_CONNECTION_STATUS_LABEL_VALUE_Y;
    w = NX_MENU_CONNECTION_STATUS_LABEL_VALUE_W;
    h = NX_MENU_CONNECTION_STATUS_LABEL_VALUE_H;

    for(lIndex = 0 ; lIndex<NX_MENU_CONNECTION_STATUS_SUB_ITEM_MAX_NUM;lIndex++)
    {
        stRect = ONDK_Rect(x,y,w,h);
        nx_menuConnectionStatus_GetConfigureLanItemsValue(pstContents,ulId,&pszValue);

        _ulID = nx_menuConnectionStatus_ConvertItemIdxToObjId(ulId);
        ret = ONDK_GWM_COM_Text_Create(_ulID, &stRect, pszValue);
        if(ONDK_RET_OK == ret)
        {
            ONDK_GWM_COM_Text_SetFont(_ulID, eFont_SystemNormal);
        	ONDK_GWM_COM_Text_SetSize(_ulID, NX_PRISM_FONT_SIZE_22);
        	ONDK_GWM_COM_Text_SetAlign(_ulID,TEXT_ALIGN_RIGHT | TEXT_ALIGN_MIDDLE);
            ONDK_GWM_Obj_SetVisible(_ulID, TRUE);
            ONDK_GWM_Obj_DisableObject(_ulID);
        }
        else if(GWM_RET_LINK_CONFLICT == ret)
        {
            ONDK_GWM_COM_Text_SetText(_ulID, pszValue);
        }

        switch(ulId)
        {
            case eNxMenuConnectionStatus_TEXT_VALUE_Ethernet_ID:
                if(TRUE == pstContents->bEthernet)
                {
                   textColor = COL(C_T_Yoyo01);
                }
                else
                {
                   textColor = COL(C_T_Rythme01);
                }
                break;
            case eNxMenuConnectionStatus_TEXT_VALUE_Internet_ID:
                if(TRUE == pstContents->bInternet)
                {
                   textColor = COL(C_T_Yoyo01);
                }
                else
                {
                   textColor = COL(C_T_Rythme01);
                }
				break;
            default:
                textColor = COL(C_T_Gong03);
                break;
        }

    	ONDK_GWM_COM_Text_ChangeColor(_ulID, textColor, textColor);

        pszValue = NULL;
        y+=(NX_MENU_CONNECTION_STATUS_LABEL_VALUE_LS);
        ulId++;
    }
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawDoneBtn(Nx_MenuConnectionStatusContent_t *pstContents)
{
    HULONG ulId = nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_BTN_DONE_ID);
    ONDK_Rect_t stRect = {0,};
    ONDK_Result_t ret = ONDK_RET_OK;
	// Apply Buttons
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_APPLY_BUTTON_X, NX_MENU_SEARCH_UI_APPLY_BUTTON_Y,
					NX_MENU_SEARCH_UI_APPLY_BUTTON_W, NX_MENU_SEARCH_UI_APPLY_BUTTON_H);
	ret = ONDK_GWM_COM_Button_Create(ulId, &stRect,(HCHAR*)ONDK_GetString(RES_DONE_01_ID));

    if(ONDK_RET_OK == ret)
    {
        ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
        ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
        ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
        ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100),  COL(C_G_Whistle_15), COL(C_G_Base01_100), COL(C_G_Whistle_05));
        ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

     	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
        ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
        ONDK_GWM_Obj_SetVisible(ulId, TRUE);
        ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);
    }

    if((FALSE == pstContents->bEthernet) && (FALSE == pstContents->bInternet) )
    {
        ONDK_GWM_COM_Button_ChangeTitle(ulId, (HCHAR*)ONDK_GetString(RES_RETRY_01_ID));
    }
    else
    {
        ONDK_GWM_COM_Button_ChangeTitle(ulId, (HCHAR*)ONDK_GetString(RES_DONE_01_ID));
    }

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_DrawDefaultUI(Nx_MenuConnectionStatusContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuConnectionStatus_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

    nx_menuConnectionStatus_DrawInfoBox(pstContents);
    nx_menuConnectionStatus_DrawLabel(pstContents);
    nx_menuConnectionStatus_DrawLabelValue(pstContents);

    nx_menuConnectionStatus_DrawImageConnection(pstContents);
    nx_menuConnectionStatus_DrawTextConnection(pstContents);

    nx_menuConnectionStatus_DrawDoneBtn(pstContents);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmCreate(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;
    NX_NET_NetIfType_e	netIfType = eNxNet_INTERFACE_TYPE_MAX;


    NX_NETWORK_RegisterNotifier();
    nx_menuConnectionStatus_InitApplication(pstContents,p1,p2,p3);
    nx_menuConnectionStatus_DoUpdatingNetworkStatus(pstContents);

    netIfType = pstContents->eCurrentIfType;

	// Draw Menu Item
	nx_menuConnectionStatus_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

    if(pstContents->eUiStatus != eNxMenuConnectionStatus_Status_RETRY)
    {
        nx_menuConnectionStatus_CreateProcessingMsg((char*)ONDK_GetString(RES_CONNECTING_ID));
        pstContents->eUiStatus = eNxMenuConnectionStatus_Status_Checking;
        NX_NETWORK_CheckGateway(netIfType);
    }

    //Default Focus
    ONDK_GWM_Obj_SetFocus(nx_menuConnectionStatus_ConvertItemIdxToObjId(eNxMenuConnectionStatus_BTN_DONE_ID));

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


static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmDestroy(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

    if(NULL != pstContents->pstNetDevInfoList)
    {
        NX_APP_Free(pstContents->pstNetDevInfoList);
    }

    if(NULL != pstContents->pstNetIf)
    {
        NX_APP_Free(pstContents->pstNetIf);
    }

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuConnectionStatusContent_t));

    NX_NETWORK_UnRegisterNotifier();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmKeyDown(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0,ulConvertedID;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         lObjId =0;
    NX_NET_NetIFADDR_t *pstNetIf;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    lObjId = GWM_GetFocusedObjectID();
    ulConvertedID = nx_menuConnectionStatus_ConvertObjIdToItemIdx(lObjId);
	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);

	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

    pstNetIf = pstContents->pstNetIf;

    if(pstContents->eUiStatus == eNxMenuConnectionStatus_Status_Checking)
    {
        switch(lKeyId)
        {
            case KEY_OK:
                return	ONDK_MESSAGE_BREAK;
            default:
                break;
        }
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
            switch(pstContents->eUiStatus)
            {
                case eNxMenuConnectionStatus_Status_RETRY:
                    nx_menuConnectionStatus_CreateProcessingMsg((char*)ONDK_GetString(RES_CONNECTING_ID));
                    pstContents->eUiStatus = eNxMenuConnectionStatus_Status_Checking;
                    nx_menuConnectionStatus_DoUpdatingNetworkStatus(pstContents);
                    NX_NETWORK_CheckGateway(pstContents->eCurrentIfType);
        			return	ONDK_MESSAGE_BREAK;
                case eNxMenuConnectionStatus_Status_DONE:
        	        lPrevCurIdx = pstContents->lPrevCurIdx;
        			ONDK_GWM_APP_Destroy(0);
        			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEMNU_Network_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
        	                (ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc, 0, 0, 0, lPrevCurIdx);
        			return	ONDK_MESSAGE_BREAK;
                default:
                    break;
            }
            break;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
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

		default:
			NX_APP_Print("nx_menuSysInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuConnectionStatus_MsgGwmClicked(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmTimer(Nx_MenuConnectionStatusContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    switch(p1)
    {
        case NX_SETTING_TIMEOUT_ID:
            ONDK_GWM_APP_Destroy(0);
            break;
        case NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID:
    		nx_menuConnectionStatus_UpdateCurrentTime(pstContents);
	    	NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
            break;
        default:
            break;
    }

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConnectionStatus_MsgGwmAPKMessage(Nx_MenuConnectionStatusContent_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e eEvent = p1;
    HxLOG_Error("\n\n#### nx_menuConnectionStatus_MsgGwmAPKMessage \n");
    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
    		case eNETWORK_CONCHK_ARP_SUCCESS :
                //GW SUCCESS
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_ARP_SUCCESS \n");
                pstContents->bEthernet = TRUE;
                NX_NETWORK_CheckDNS(pstContents->eCurrentIfType);
		    	break;
    		case eNETWORK_CONCHK_ARP_FAIL :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_ARP_FAIL \n");
                pstContents->bEthernet = FALSE;
                pstContents->bInternet = FALSE;
                pstContents->eUiStatus = eNxMenuConnectionStatus_Status_RETRY;
                nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(pstContents);
                nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_SUCCESS :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_SUCCESS \n");
                //DNS SUCCESS
                pstContents->bInternet = TRUE;
                pstContents->eUiStatus = eNxMenuConnectionStatus_Status_DONE;
                nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(pstContents);
                nx_menuConnectionStatus_ProcessingMsgDestroy();
		    	break;
    		case eNETWORK_CONCHK_RESOLV_FAIL :
                HxLOG_Error("\n\n#### eNETWORK_CONCHK_RESOLV_FAIL \n");
                pstContents->bInternet = FALSE;
                pstContents->eUiStatus = eNxMenuConnectionStatus_Status_DONE;
                nx_menuConnectionStatus_DoUpdatingConnectionUIStatus(pstContents);
                nx_menuConnectionStatus_ProcessingMsgDestroy();
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
ONDK_Result_t	NX_PRISM_MENU_ConnectionStatus_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuConnectionStatusContent_t	*pstContents = NULL;

	pstContents = nx_menuConnectionStatus_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuConnectionStatus_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuConnectionStatus_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuConnectionStatus_MsgGwmClicked(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuConnectionStatus_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_menuConnectionStatus_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
            break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuConnectionStatus_MsgGwmTimer(pstContents, p1, p2, p3);
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

#endif//#if defined (CONFIG_MW_INET)
/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

