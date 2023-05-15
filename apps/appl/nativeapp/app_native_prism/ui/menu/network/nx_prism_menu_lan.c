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
 * @file	  		nx_prism_menu_sysinfo.c
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
#include <nx_prism_component_ipdialog.h>
#if defined (CONFIG_MW_INET)
/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_CONFIGURE_LAN_ID					eNxSetting_ConfigureLan

#define NX_MENU_CONFIGURE_LAN_BTN_X                 60
#define NX_MENU_CONFIGURE_LAN_BTN_Y                 103
#define NX_MENU_CONFIGURE_LAN_BTN_W                 750
#define NX_MENU_CONFIGURE_LAN_BTN_H                 55
#define NX_MENU_CONFIGURE_LAN_BTN_LS                60

#define	NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_W		13
#define	NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_H	    20
#define NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_X_TAP   7
#define NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_Y_TAP   ((NX_MENU_CONFIGURE_LAN_BTN_H/2) -(NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_H/2))

#define	NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM      6

#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_X            305
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_Y    		615
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_W	    	260
#define	NX_MENU_SEARCH_UI_APPLY_BUTTON_H		    55


#define	NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR	        (16)
#define	NX_MENU_CONFIGURE_LAN_MAX_MACADDR       	(20)
#define NX_MENU_CONFIGURE_LAN_MAX_IP6ADDR       	(40)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum Nx_MenuConfigureLanUicompIds
{
	eNxMenuConfigureLan_BTN_ConfigureIP_ID,
	eNxMenuConfigureLan_BTN_IP_Address_ID,
	eNxMenuConfigureLan_BTN_Netmask_ID,
	eNxMenuConfigureLan_BTN_GatewayAddress_ID,
	eNxMenuConfigureLan_BTN_PrimaryDNS_ID,
	eNxMenuConfigureLan_BTN_SecondaryDNS_ID,
	eNxMenuConfigureLan_BTN_Apply_ID,
    eNxMenuConfigureLan_BTN_MAX,
    eNxMenuConfigureLan_IMG_ConfigureIP_ARROW_ID,
    eNxMenuConfigureLan_IMG_Loading_Bg_ID,
    eNxMenuConfigureLan_IMG_Loading_Icon_ID,
    eNxMenuConfigureLan_TEXT_Loading_ID,
    eNxMenuConfigureLan_OBJ_ConfigureIP_POPUP_ID,
    eNxMenuConfigureLan_OBJ_Loading_Timer_ID,
	eNxMenuConfigureLan_MAX
} Nx_MenuConfigureLanUicompIds_e;

typedef enum Nx_MenuConfigureLanUiStatus
{
	eNxMenuConfigureLan_Status_Init,
	eNxMenuConfigureLan_Status_Applying,
	eNxMenuConfigureLan_Status_MAX
} Nx_MenuConfigureLanUiStatus_e;


typedef struct tagConfigureLan_LIST
{
	HUINT32		ulItemNum;
	HUINT32		aulItemList[NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM];
	HUINT32		aulValueList[NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM];
} NxUiConfigureLanItemList_t;

typedef struct tag_NX_MENU_LAN_CONTENT
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

    //LAN Information
	HUINT8		**szItemTextList;
    NX_NET_NetIFADDR_t *pstNetIf;
	HCHAR 	pszIpAddr[NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR];
	HCHAR 	pszNetmask[NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR];
	HCHAR 	pszGateway[NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR];
	HCHAR 	pszDns1[NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR];
	HCHAR 	pszDns2[NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR];

    //Loading Img
    HINT32		nLoadingImgIndex;
    Nx_MenuConfigureLanUiStatus_e eUiStatus;
} Nx_MenuNetworkLanContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuNetworkLanContent_t		s_stLanConfigureContent;
static Nx_PrismListDialog_Content_t		s_stPopupListConfigureLanConts;
static NX_IpDialogInstance_t            s_stMenuNetwork_Ip_dInstance;
static NXUI_DialogInstance_t			s_stDialogMessageInstance;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuConfigureLan_MsgGwmCreate(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuConfigureLan_MsgGwmDestroy(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConfigureLan_MsgGwmKeyDown(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConfigureLan_MsgGwmClicked(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConfigureLan_MsgGwmTimer(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuConfigureLan_MsgGwmFocusChanged(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);



/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


static HERROR	nx_menuConfigureLan_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%ld.%ld.%ld.%ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

static HERROR   nx_menuConfigureLan_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
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


static inline Nx_MenuNetworkLanContent_t* nx_menuConfigureLan_GetContents(void)
{
	return	&s_stLanConfigureContent;
}


static ONDK_Result_t	nx_menuConfigureLan_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListConfigureLanConts, message, hAction, p1, p2, p3);
}



static ONDK_Result_t	nx_menuConfigureLan_DisplayIpInputDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_IpDlalog_ProtoProc(&s_stMenuNetwork_Ip_dInstance, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuConfigureLan_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stDialogMessageInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuConfigureLan_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_menuConfigureLan_ProcessingMsgProc);
}

static ONDK_Result_t	nx_menuConfigureLan_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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

static HINT32	nx_menuConfigureLan_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~NX_MENU_CONFIGURE_LAN_ID);

	if ((lItemId < 0) || (lItemId >= eNxMenuConfigureLan_MAX))
	{
		return -1;
	}

	return	lItemId;
}

static HINT32	nx_menuConfigureLan_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxMenuConfigureLan_MAX))
	{
		return 0;
	}

	return	(NX_MENU_CONFIGURE_LAN_ID | lItemId);
}


static void		nx_menuConfigureLan_UpdateCurrentTime(Nx_MenuNetworkLanContent_t *pstContents)
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

static ONDK_Result_t	nx_menuConfigureLan_SetFocusableItems(Nx_MenuNetworkLanContent_t *pstContents)
{
    HINT32  lIndex = 0;
    Nx_MenuConfigureLanUicompIds_e ulId = eNxMenuConfigureLan_BTN_ConfigureIP_ID;
    NX_NET_NetIFADDR_t *pstNetIf = NULL;
    HINT32 convertedUlID, ulFocusID;
    HUINT32 lObjId;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    pstNetIf = pstContents->pstNetIf;
	lObjId = GWM_GetFocusedObjectID();
    ulFocusID = nx_menuConfigureLan_ConvertObjIdToItemIdx(lObjId);

    for(lIndex = 0 ; lIndex<NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM;lIndex++)
    {
        convertedUlID = nx_menuConfigureLan_ConvertItemIdxToObjId(ulId);
        if(TRUE == pstNetIf->bDhcpOn)
        {
            if(eNxMenuConfigureLan_BTN_ConfigureIP_ID!=ulId)
            {
                ONDK_GWM_COM_Button_SetStatus(convertedUlID ,GUIOBJ_DISABLED);
            }
        }
        else
        {
            if(ulFocusID ==ulId)
            {
                ONDK_GWM_COM_Button_SetStatus(convertedUlID,GUIOBJ_HILIGHTED);
                ONDK_GWM_Obj_SetFocus(convertedUlID);
            }
            else
            {
                ONDK_GWM_COM_Button_SetStatus(convertedUlID,GUIOBJ_NORMAL);
            }
        }
        ulId++;

    }
	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_menuConfigureLan_DrawSubBtnValue(HUINT32 ulId, HCHAR* pszLabelValue)
{
    HUINT32 _ulID = (NX_MENU_CONFIGURE_LAN_ID | ulId);
    ONDK_GWM_COM_Button_SetExtraText(_ulID,(HUINT8*)pszLabelValue);
    ONDK_GWM_COM_Button_SetExtraAlign(_ulID,ONDKSTF_RIGHT);
    return	ONDK_RET_OK;
}

static HCHAR*   nx_menuCOnfigureLan_GetLabelName(Nx_MenuConfigureLanUicompIds_e index)
{
    switch(index)
    {
        case    eNxMenuConfigureLan_BTN_ConfigureIP_ID:
            return (HCHAR*)ONDK_GetString(RES_CONFIGURE_IP_ID);
        case	eNxMenuConfigureLan_BTN_IP_Address_ID:
            return (HCHAR*)ONDK_GetString(RES_IP_ADDRESS_ID);
        case	eNxMenuConfigureLan_BTN_Netmask_ID:
            return (HCHAR*)ONDK_GetString(RES_NETMASK_ID);
        case	eNxMenuConfigureLan_BTN_GatewayAddress_ID:
            return (HCHAR*)ONDK_GetString(RES_GATEWAY_ADDRESS_ID);
        case	eNxMenuConfigureLan_BTN_PrimaryDNS_ID:
            return (HCHAR*)ONDK_GetString(RES_PRIMARY_DNS_SERVER_ID);
        case	eNxMenuConfigureLan_BTN_SecondaryDNS_ID:
            return (HCHAR*)ONDK_GetString(RES_SECONDARY_DNS_SERVER_ID);
        default:
            return "";
    }
}

static ONDK_Result_t	nx_menuConfigureLan_GetConfigureLanItemsValue(Nx_MenuNetworkLanContent_t *pstContents,Nx_MenuConfigureLanUicompIds_e ulId,HCHAR** pszBuf)
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
            case eNxMenuConfigureLan_BTN_ConfigureIP_ID:
                if(TRUE == pstNetIf->bDhcpOn)
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_DHCP_ID);
                }
                else
                {
                     pszRet = (HCHAR*)ONDK_GetString(RES_MANUAL_ID);
                }
                break;
            case eNxMenuConfigureLan_BTN_IP_Address_ID:
                pszRet = pstContents->pszIpAddr;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                nx_menuConfigureLan_AddrN2A(pstNetIf->ulIpAddr,pszRet,NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                break;
            case eNxMenuConfigureLan_BTN_Netmask_ID:
                pszRet = pstContents->pszNetmask;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                nx_menuConfigureLan_AddrN2A(pstNetIf->ulNetmask,pszRet,NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                break;
            case eNxMenuConfigureLan_BTN_GatewayAddress_ID:
                pszRet = pstContents->pszGateway;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                nx_menuConfigureLan_AddrN2A(pstNetIf->ulGateway,pszRet,NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                break;
            case eNxMenuConfigureLan_BTN_PrimaryDNS_ID:
                pszRet = pstContents->pszDns1;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                nx_menuConfigureLan_AddrN2A(pstNetIf->ulDns1,pszRet,NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
               break;
            case eNxMenuConfigureLan_BTN_SecondaryDNS_ID:
                pszRet = pstContents->pszDns2;
          		HxSTD_MemSet(pszRet, 0x00, NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                nx_menuConfigureLan_AddrN2A(pstNetIf->ulDns2,pszRet,NX_MENU_CONFIGURE_LAN_MAX_IP4ADDR);
                break;
			default:
				return ONDK_RET_FAIL;
        }
    }
    *pszBuf = pszRet;
	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_menuConfigureLan_AddConfigureIPSubItem(Nx_MenuNetworkLanContent_t *pContents)
{
    HUINT32             ulMenuItemId = NX_MENUITEM_ID_SET_NETWORK_CONFIGURE_IP;
    HUINT32				ulItems = 0, ulListIdx = 0;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
   	Nx_SettingsLeafSubItem_t	stSubItem;
	HUINT8				**ppListOfItem = NULL;
    NX_NET_NetIFADDR_t *pstNetIf;

    pstNetIf = pContents->pstNetIf;

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
    stSubItem.ulId = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_OBJ_ConfigureIP_POPUP_ID);

	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);

	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId,(HINT32)pstNetIf->bDhcpOn, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstNetIf->bDhcpOn;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pContents, &stSubItem);

    return ONDK_RET_OK;
}

static void		nx_menuConfigureLan_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuConfigureLan_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_menuConfigureLan_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


static void		nx_menuConfigureLan_CreateListPopup(Nx_MenuNetworkLanContent_t *pContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem;
    HUINT32         ulConvertedID = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_OBJ_ConfigureIP_POPUP_ID);

    psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents,ulConvertedID);
	if ( psubItem == NULL )
	{
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListConfigureLanConts,	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
										&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
										ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuConfigureLan_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuConfigureLan_DisplayListPopup, 0, 0, 0, 0);
}

static ONDK_Result_t    nx_menuConfigureLan_DrawSubBtn(HUINT32 ulId ,ONDK_Rect_t stRect, HCHAR* pszLabelName)
{
    HUINT32 _ulID = nx_menuConfigureLan_ConvertItemIdxToObjId(ulId);
    ONDK_GWM_COM_Button_Create (_ulID, &stRect,pszLabelName);
    ONDK_GWM_COM_Button_SetFontColor(_ulID, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(_ulID, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(_ulID,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(_ulID,  COL(C_G_Base01_100),  COL(C_G_Whistle_15), COL(C_G_Base01_100), COL(C_G_Whistle_05));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(_ulID,100);

 	ONDK_GWM_COM_Button_SetTitleSize(_ulID,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(_ulID,ONDKSTF_LEFT);
    ONDK_GWM_Obj_SetVisible(_ulID, TRUE);
    ONDK_GWM_COM_Button_SetStatus(_ulID,GUIOBJ_NORMAL);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureLan_DrawMainTitle(Nx_MenuNetworkLanContent_t *pstContents)
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


static ONDK_Result_t	nx_menuConfigureLan_DrawConfigureLanItems(Nx_MenuNetworkLanContent_t *pstContents)
{
    HINT32  lIndex = 0;
    Nx_MenuConfigureLanUicompIds_e ulId = eNxMenuConfigureLan_BTN_ConfigureIP_ID;
    ONDK_Rect_t stRect = {0,};
    HINT32  x = NX_MENU_CONFIGURE_LAN_BTN_X,y = NX_MENU_CONFIGURE_LAN_BTN_Y,w = NX_MENU_CONFIGURE_LAN_BTN_W,h = NX_MENU_CONFIGURE_LAN_BTN_H;
    HCHAR*  pszLabel;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    for(lIndex = 0 ; lIndex<NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM;lIndex++)
    {
        stRect = ONDK_Rect(x,y,w,h);
        pszLabel = nx_menuCOnfigureLan_GetLabelName(ulId);
        nx_menuConfigureLan_DrawSubBtn(ulId,stRect,pszLabel);
        y+=(NX_MENU_CONFIGURE_LAN_BTN_LS);
        ulId++;
    }

    ONDK_GWM_Obj_SetFocus((NX_MENU_CONFIGURE_LAN_ID|eNxMenuConfigureLan_BTN_ConfigureIP_ID) );


	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureLan_DrawConfigureLanItemsValue(Nx_MenuNetworkLanContent_t *pstContents)
{
    HINT32  lIndex = 0;
    Nx_MenuConfigureLanUicompIds_e ulId = eNxMenuConfigureLan_BTN_ConfigureIP_ID;
    HCHAR*  pszValue = NULL;
    NX_NET_NetIFADDR_t *pstNetIf;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

    pstNetIf = pstContents->pstNetIf;

    for(lIndex = 0 ; lIndex<NX_MENU_CONFIGURE_LAN_SUB_ITEM_MAX_NUM;lIndex++)
    {
        nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,ulId,&pszValue);
        nx_menuConfigureLan_DrawSubBtnValue(ulId, pszValue);
        pszValue = NULL;
        ulId++;
    }
    nx_menuConfigureLan_SetFocusableItems(pstContents);
    ONDK_GWM_Obj_SetFocus((NX_MENU_CONFIGURE_LAN_ID|eNxMenuConfigureLan_BTN_ConfigureIP_ID) );

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureLan_DrawConfigureLanArrow(Nx_MenuNetworkLanContent_t *pstContents)
{

    HULONG ulId = (NX_MENU_CONFIGURE_LAN_ID|eNxMenuConfigureLan_IMG_ConfigureIP_ARROW_ID);
    ONDK_Rect_t stRect = {0,};
    HINT32  x=0,y=0,w=0,h=0;

    x = NX_MENU_CONFIGURE_LAN_BTN_X + NX_MENU_CONFIGURE_LAN_BTN_W + NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_X_TAP;
    y = NX_MENU_CONFIGURE_LAN_BTN_Y + NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_Y_TAP;
    w = NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_W;
    h = NX_MENU_CONFIGURE_LAN_IMG_ARROW_IMG_H;

	// right arrow
	stRect = ONDK_Rect(x,y,w,h);
    ONDK_GWM_COM_Image_Create(ulId, &stRect);
    ONDK_GWM_COM_Image_SetImage(ulId, RES_106_00_ARROW_RIGHT_PNG);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_Obj_DisableObject(ulId);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureLan_DrawApplyBtn(Nx_MenuNetworkLanContent_t *pstContents)
{
    HULONG ulId = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_BTN_Apply_ID);
    ONDK_Rect_t stRect = {0,};

	// Apply Buttons
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_APPLY_BUTTON_X, NX_MENU_SEARCH_UI_APPLY_BUTTON_Y,
					NX_MENU_SEARCH_UI_APPLY_BUTTON_W, NX_MENU_SEARCH_UI_APPLY_BUTTON_H);
	ONDK_GWM_COM_Button_Create(ulId, &stRect,(HCHAR*)ONDK_GetString(RES_APPLY_01_ID));
    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100),  COL(C_G_Whistle_15), COL(C_G_Base01_100), COL(C_G_Whistle_05));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);

    return ONDK_RET_OK;
}

static void		nx_menuConfigureLan_CreateProcessingMsg(Nx_MenuNetworkLanContent_t *pstContents)
{
	HUINT32 		ulDlgAttr = 0;
	HxSTD_memset (&s_stDialogMessageInstance, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
	NX_PRISM_DIALOGUE_SetDialog(&s_stDialogMessageInstance, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(RES_CONNECTING_ID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Ui_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuConfigureLan_ProcessingMsgProc, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_menuConfigureLan_DrawDefaultUI(Nx_MenuNetworkLanContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuConfigureLan_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

    nx_menuConfigureLan_DrawMainTitle(pstContents);
    nx_menuConfigureLan_DrawConfigureLanItems(pstContents);
    nx_menuConfigureLan_DrawConfigureLanItemsValue(pstContents);
    nx_menuConfigureLan_DrawConfigureLanArrow(pstContents);
    nx_menuConfigureLan_DrawApplyBtn(pstContents);
//    nx_menuConfigureLan_AddItemLoading(pstContents);
	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureLan_MsgGwmCreate(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    HERROR		hError  = ERR_FAIL;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);
	pstContents->lPrevCurIdx = p3;
    pstContents->pstNetIf = NULL;
    pstContents->nLoadingImgIndex = 0;
    //Get Network Info
    hError = NX_NETWORK_GetLanInterface(&(pstContents->pstNetIf));

	// Draw Menu Item
	nx_menuConfigureLan_DrawDefaultUI(pstContents);
    nx_menuConfigureLan_AddConfigureIPSubItem(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);
    NX_NETWORK_RegisterNotifier();

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


static ONDK_Result_t	nx_menuConfigureLan_MsgGwmDestroy(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuNetworkLanContent_t));

    NX_NETWORK_UnRegisterNotifier();
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureLan_MsgGwmKeyDown(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0,ulConvertedID;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         lObjId =0, ulAttribute = 0;
    HCHAR*          pszDlgMsg = NULL;
    HULONG 	        ulDlgValue = 0;
    NX_NET_NetIFADDR_t *pstNetIf;
    HERROR          hError  = ERR_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    lObjId = GWM_GetFocusedObjectID();
    ulConvertedID = nx_menuConfigureLan_ConvertObjIdToItemIdx(lObjId);
	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);

	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

    pstNetIf = pstContents->pstNetIf;

    if(pstContents->eUiStatus == eNxMenuConfigureLan_Status_Applying)
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
            switch(ulConvertedID)
            {
                case    eNxMenuConfigureLan_BTN_ConfigureIP_ID:
                            nx_menuConfigureLan_CreateListPopup(pstContents,p1);
                			return	ONDK_MESSAGE_BREAK;
                case    eNxMenuConfigureLan_BTN_IP_Address_ID:
                            pszDlgMsg = (HCHAR*)ONDK_GetString(RES_MESG_4656_ID);
                            ulDlgValue = pstNetIf->ulIpAddr;
                            break;
                case    eNxMenuConfigureLan_BTN_Netmask_ID:
                            pszDlgMsg = (HCHAR*)ONDK_GetString(RES_MESG_4656_ID);
                            ulDlgValue = pstNetIf->ulNetmask;
                            break;
                case	eNxMenuConfigureLan_BTN_GatewayAddress_ID:
                            pszDlgMsg = (HCHAR*)ONDK_GetString(RES_MESG_4656_ID);
                            ulDlgValue = pstNetIf->ulGateway;
                            break;
                case	eNxMenuConfigureLan_BTN_PrimaryDNS_ID:
                            pszDlgMsg = (HCHAR*)ONDK_GetString(RES_MESG_4656_ID);
                            ulDlgValue = pstNetIf->ulDns1;
                            break;
                case	eNxMenuConfigureLan_BTN_SecondaryDNS_ID:
                            pszDlgMsg = (HCHAR*)ONDK_GetString(RES_MESG_4656_ID);
                            ulDlgValue = pstNetIf->ulDns2;
                            break;
                case    eNxMenuConfigureLan_BTN_Apply_ID:
                            hError = NX_NETWORK_SetLANNetInterface(pstNetIf);
                            if(ERR_OK == hError)
                            {
                                pstContents->eUiStatus = eNxMenuConfigureLan_Status_Applying;
                                nx_menuConfigureLan_CreateProcessingMsg(pstContents);
                            }
                            break;
                default:
                    break;
            }

            if(ulConvertedID>eNxMenuConfigureLan_BTN_ConfigureIP_ID && ulConvertedID<=eNxMenuConfigureLan_BTN_SecondaryDNS_ID)
            {
            	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
            	NX_PRISM_COMP_IpDlalog_SetIp4InputDialog(&s_stMenuNetwork_Ip_dInstance,
							ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),ulAttribute, pszDlgMsg, ulDlgValue);
            	/* Create Pop Up */
            	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuNetwork_DisplayIpDialog", APP_DEFAULT_PRIORITY,
            	nx_menuConfigureLan_DisplayIpInputDialog, 0, 0, 0, 0);
                return ONDK_MESSAGE_BREAK;
            }

            break;
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
            if(ulConvertedID == eNxMenuConfigureLan_BTN_ConfigureIP_ID) // Focus loop
            {
            	HINT32			_ulConvertedID = 0;
                _ulConvertedID = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_BTN_Apply_ID);
                ONDK_GWM_Obj_SetFocus(_ulConvertedID);
                nx_menuConfigureLan_MsgGwmFocusChanged(pstContents,p1,p2,p3);
                return	ONDK_MESSAGE_BREAK;
            }
            break;
		case KEY_ARROWDOWN:
            if(ulConvertedID == eNxMenuConfigureLan_BTN_Apply_ID) // Focus loop
            {
            	HINT32			_ulConvertedID = 0;
                _ulConvertedID = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_BTN_ConfigureIP_ID);
                ONDK_GWM_Obj_SetFocus(_ulConvertedID);
                nx_menuConfigureLan_MsgGwmFocusChanged(pstContents,p1,p2,p3);
                return	ONDK_MESSAGE_BREAK;
            }
			break;
		case KEY_ARROWRIGHT:
            if(ulConvertedID == eNxMenuConfigureLan_BTN_ConfigureIP_ID)
            {
                nx_menuConfigureLan_CreateListPopup(pstContents,p1);
            	return	ONDK_MESSAGE_BREAK;
            }
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


static ONDK_Result_t    nx_menuConfigureLan_MsgGwmClicked(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

static ONDK_Result_t	nx_menuConfigureLan_MsgGwmTimer(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
    switch(p1)
    {
        case NX_SETTING_TIMEOUT_ID:
            ONDK_GWM_APP_Destroy(0);
            break;
        case NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID:
    		nx_menuConfigureLan_UpdateCurrentTime(pstContents);
	    	NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
            break;
        default:
            break;
    }

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuConfigureLan_MsgGwmFocusChanged(Nx_MenuNetworkLanContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
    HINT32			lObjId = 0;
    HINT32			lImgArrowId = 0;
    HINT32			lConvertedID = 0;
    if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = GWM_GetFocusedObjectID();

    lConvertedID = nx_menuConfigureLan_ConvertObjIdToItemIdx(lObjId);
    lImgArrowId = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_IMG_ConfigureIP_ARROW_ID);

    if(lConvertedID != eNxMenuConfigureLan_BTN_ConfigureIP_ID)
    {
        ONDK_GWM_Obj_SetVisible(lImgArrowId, FALSE);
    }
    else
    {
        ONDK_GWM_Obj_SetVisible(lImgArrowId, TRUE);
    }


	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

    return eResult;

}

static ONDK_Result_t	nx_menuConfigureLan_MsgGwmAPKMessage(Nx_MenuNetworkLanContent_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e eEvent = p1;
    HCHAR*              strSuccess = (HCHAR*)ONDK_GetString(RES_MESG_3242_ID);
    HCHAR*              strFailed = (HCHAR*)ONDK_GetString(RES_MESG_360_ID);
    HULONG ulId = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_BTN_Apply_ID);
    HxLOG_Error("\n\n####### nx_menuConfigureLan_MsgGwmAPKMessage ########\n\n");
    if(GWM_APK_NETWORK == eAPK_Type)
    {
        //Loading
        switch(eEvent)
        {
            case eNETWORK_STATUS_CON_FAIL:
            case eNETWORK_STATUS_CON_SUCCESS:
                HxLOG_Error("\n\n####### %d ########\n\n",eEvent);
                pstContents->eUiStatus = eNxMenuConfigureLan_Status_Init;
                nx_menuConfigureLan_ProcessingMsgDestroy();
                break;
            default:
                break;
        }
        //Show Result Popup
        switch(eEvent)
        {
            case eNETWORK_STATUS_CON_FAIL:
                nx_menuConfigureLan_CreateResponseDialouge((HUINT8*)strFailed);
                ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
                break;
            case eNETWORK_STATUS_CON_SUCCESS:
                nx_menuConfigureLan_CreateResponseDialouge((HUINT8*)strSuccess);
                ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
                break;
            default:
                break;
        }
    }

    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuConfigureLan_ChangedSubItemValue(Nx_MenuNetworkLanContent_t *pstContents, HLONG ulValue)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0;
	Nx_SettingsLeafSubItem_t	*pSubItem = NULL;
    NX_NET_NetIFADDR_t *pstNetIf;
    HUINT32         ulCurIdx, ulConvertedID = 0;
    HCHAR*  pszValue = NULL;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuConfigureLan_ConvertObjIdToItemIdx(lObjId);
    pstNetIf = pstContents->pstNetIf;

	switch (lItemIdx)
	{
	    case eNxMenuConfigureLan_BTN_ConfigureIP_ID:
            ulCurIdx = ulValue;
            ulConvertedID = nx_menuConfigureLan_ConvertItemIdxToObjId(eNxMenuConfigureLan_OBJ_ConfigureIP_POPUP_ID);
			pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents,ulConvertedID );
			pSubItem->ulIdxOfTextList = ulCurIdx;
			HxSTD_StrNCpy((char*)pSubItem->szSubItemText, (char*)pSubItem->szItemTextList[pSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

			/* Update subItem value */
            ulMenuItemId = NX_MENUITEM_ID_SET_NETWORK_CONFIGURE_IP;
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->lSubItemValue));

			/* Update value */
            pstNetIf->bDhcpOn = pSubItem->lSubItemValue;
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, (char*)pSubItem->szSubItemText);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
			break;
        case eNxMenuConfigureLan_BTN_IP_Address_ID:
            /* Update value */
            pstNetIf->ulIpAddr = ulValue;
            nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,lItemIdx,&pszValue);
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, pszValue);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
            break;
        case eNxMenuConfigureLan_BTN_Netmask_ID:
            pstNetIf->ulNetmask = ulValue;
            nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,lItemIdx,&pszValue);
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, pszValue);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
            break;
        case eNxMenuConfigureLan_BTN_GatewayAddress_ID:
            /* Update value */
            pstNetIf->ulGateway= ulValue;
            nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,lItemIdx,&pszValue);
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, pszValue);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
            break;
        case eNxMenuConfigureLan_BTN_PrimaryDNS_ID:
            /* Update value */
            pstNetIf->ulDns1 = ulValue;
            nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,lItemIdx,&pszValue);
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, pszValue);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
            break;
        case eNxMenuConfigureLan_BTN_SecondaryDNS_ID:
            /* Update value */
            pstNetIf->ulDns2 = ulValue;
            nx_menuConfigureLan_GetConfigureLanItemsValue(pstContents,lItemIdx,&pszValue);
            nx_menuConfigureLan_DrawSubBtnValue(lItemIdx, pszValue);
            nx_menuConfigureLan_SetFocusableItems(pstContents);
            break;
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_ConfigureLan_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuNetworkLanContent_t	*pstContents = NULL;

	pstContents = nx_menuConfigureLan_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuConfigureLan_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuConfigureLan_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuConfigureLan_MsgGwmClicked(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_OBJ_FOCUS_CHANGED:
            eResult = nx_menuConfigureLan_MsgGwmFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuConfigureLan_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuConfigureLan_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_APP_SETTING_VALUE_CHANGED:
        case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuConfigureLan_ChangedSubItemValue(pstContents, p1);
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_menuConfigureLan_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
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

#endif//#if defined (CONFIG_MW_INET)
/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

