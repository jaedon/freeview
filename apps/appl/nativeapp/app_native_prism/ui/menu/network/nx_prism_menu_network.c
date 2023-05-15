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
#ifdef CONFIG_MW_INET
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum NxSettingsItem_Network_Item {
	eNxSettingsItem_ConfigureLAN,
	eNxSettingsItem_ConfigureWLAN,
	eNxSettingsItem_NetworkConnectionStatus,
	eNxSettingsItem_Network_MAX
} NxSettingsItem_Network_t;

typedef struct tag_NX_MENU_NETWORK
{
    NX_NET_NetDevInfo_t *pstNetDevInfoList;
    HINT32              lNetDevNum;
    NX_NET_NetIfType_e eCurrentIfType;
} Nx_MenuNetworkContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_SettingsNaviContext_t		s_stMenuNetworkNaviContext;
static NXUI_DialogInstance_t		s_stNetwork_selective_dInstance;
static Nx_MenuNetworkContent_t      s_stMenuNetworkContent;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t    nx_menuNetwork_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuNetwork_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuNetwork_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t    nx_menuNetwork_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuNetwork_MsgGwmAPKMessage(Nx_SettingsNaviContext_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);



/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_SettingsNaviContext_t	*nx_menuNetwork_GetNaviContents(void)
{
	return	&s_stMenuNetworkNaviContext;
}

static inline Nx_MenuNetworkContent_t	*nx_menuNetwork_GetNetworkContents(void)
{
	return	&s_stMenuNetworkContent;
}

static HINT32	nx_menuNetwork_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuNetwork_GetNaviContents();
	ulMainItems = pstContents->ulNumOfMainItem;
	for (i = 0; i < ulMainItems; i++)
	{
		pMainItem =  NX_PRISM_AppNaviCntxt_GetMainItem((void *)pstContents, i);
		if (pMainItem == NULL)
		{
			continue;
		}

		if (pMainItem->ulMainBtnId == (HUINT32)lObjId)
		{
			break;
		}
	}

	if (i >= ulMainItems)
	{
		i = 0;
	}

	return	(HINT32)i;
}

static HINT32	nx_menuNetwork_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Network_MAX))
	{
		return 0;
	}

	return	(eNxSetting_Network | lItemId);
}


static HINT32	nx_menuNetwork_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Network);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Network_MAX))
	{
		return -1;
	}

	return	lItemId;
}

static ONDK_Result_t	nx_menuNetwork_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (&s_stNetwork_selective_dInstance, message, hAction,p1, p2, p3);
}



static void	nx_menuNetwork_CreateSelectiveDialog(HINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stNetwork_selective_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuSystem_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuNetwork_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}

static HCHAR*   nx_menuNetwork_GetItemValue(NxSettingsItem_Network_t eNxNetworkItem)
{
    HCHAR   *pszRet = NULL;
    HINT32  lIndex = 0;
    Nx_MenuNetworkContent_t	* pstNetworkContents =  nx_menuNetwork_GetNetworkContents();
    NX_NET_NetDevInfo_t *pstNetDevInfoList;
    HBOOL   bConnected = FALSE;

    pstNetDevInfoList = pstNetworkContents->pstNetDevInfoList;

    if(eNxSettingsItem_ConfigureLAN == eNxNetworkItem)
    {
        //Get Network Info
        for(lIndex=0; lIndex < pstNetworkContents->lNetDevNum ;lIndex++)
        {
            if( (TRUE == pstNetDevInfoList[lIndex].bConnected)
                && (eNxNet_INTERFACE_TYPE_LAN == pstNetDevInfoList[lIndex].eType))
            {
                bConnected = TRUE;
                break;
            }
        }

        if(TRUE == bConnected)
        {
            pszRet = (HCHAR*)ONDK_GetString(RES_CONNECTED_ID);
        }
        else
        {
            pszRet = (HCHAR*)ONDK_GetString(RES_DISCONNECTED_ID);
        }
    }
    else if(eNxSettingsItem_ConfigureWLAN == eNxNetworkItem)
    {
        //Get Network Info
        for(lIndex=0; lIndex < pstNetworkContents->lNetDevNum ;lIndex++)
        {
            if( (TRUE == pstNetDevInfoList[lIndex].bConnected)
                && (eNxNet_INTERFACE_TYPE_WLAN == pstNetDevInfoList[lIndex].eType))
            {
                bConnected = TRUE;
                break;
            }
        }

        if(TRUE == bConnected)
        {
            pszRet = (HCHAR*)ONDK_GetString(RES_CONNECTED_ID);
        }
        else
        {
            pszRet = (HCHAR*)ONDK_GetString(RES_DISCONNECTED_ID);
        }
    }
    else
    {
        pszRet = "";
    }

    return pszRet;
}

static HBOOL	nx_menuNetwork_GetItemDisable(NxSettingsItem_Network_t eNxNetworkItem)
{
	HBOOL	bDisable = TRUE;
    Nx_MenuNetworkContent_t	* pstNetworkContents =  nx_menuNetwork_GetNetworkContents();

    if(pstNetworkContents->eCurrentIfType == eNxNet_INTERFACE_TYPE_MAX)
    {
        //LAN DISCONNECTED
        //WLAN ENABLE
        switch (eNxNetworkItem)
    	{
    		case eNxSettingsItem_ConfigureLAN:
                bDisable = FALSE;
    			break;
    		case eNxSettingsItem_ConfigureWLAN:
                bDisable = FALSE;
    			break;
    		case eNxSettingsItem_NetworkConnectionStatus:
    			bDisable = FALSE;
    			break;
    		default:
    			bDisable = TRUE;
    			break;
	    }
        return bDisable;
    }

	switch (eNxNetworkItem)
	{
		case eNxSettingsItem_ConfigureLAN:
            if(pstNetworkContents->eCurrentIfType == eNxNet_INTERFACE_TYPE_LAN )
            {
                bDisable = FALSE;
            }
            else
            {
                bDisable = TRUE;
            }
			break;
		case eNxSettingsItem_ConfigureWLAN:
            if(pstNetworkContents->eCurrentIfType == eNxNet_INTERFACE_TYPE_WLAN )
            {
                bDisable = FALSE;
            }
            else
            {
                bDisable = TRUE;
            }
			break;
		case eNxSettingsItem_NetworkConnectionStatus:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}

static ONDK_Result_t	nx_menuNetwork_AddItemConfigureLAN(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t		stMainItem = {0, };

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuNetwork_ConvertItemIdxToObjId(eNxSettingsItem_ConfigureLAN);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = RES_CONFIGURE_LAN_ID;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(RES_CONFIGURE_LAN_ID), MAX_ELEMENT_TEXT_LENGTH);

    /* Set Value */
    HxSTD_StrNCpy ((char*)stMainItem.ucSubItemString,nx_menuNetwork_GetItemValue(eNxSettingsItem_ConfigureLAN), sizeof(HUINT8)*MAX_ITEM_TEXT_LENGTH);


	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_ConfigureLan_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NX_PRISM_MENU_ConfigureLan_Proc;
	stMainItem.bDisable = nx_menuNetwork_GetItemDisable(eNxSettingsItem_ConfigureLAN);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_AddItemConfigureWLAN(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t		stMainItem = {0, };

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuNetwork_ConvertItemIdxToObjId(eNxSettingsItem_ConfigureWLAN);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = RES_CONFIGURE_WLAN_01_ID;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(RES_CONFIGURE_WLAN_01_ID), MAX_ELEMENT_TEXT_LENGTH);

    /* Set Value */
    HxSTD_StrNCpy ((char*)stMainItem.ucSubItemString, nx_menuNetwork_GetItemValue(eNxSettingsItem_ConfigureWLAN), sizeof(HUINT8)*MAX_ITEM_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_ConfigureWLAN_Proc", MAX_ELEMENT_TEXT_LENGTH);

	stMainItem.appToLaunch = NX_PRISM_MENU_ConfigureWLan_Proc;
	stMainItem.bDisable = nx_menuNetwork_GetItemDisable(eNxSettingsItem_ConfigureWLAN);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_AddItemConnectionStatus(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t		stMainItem = {0, };

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuNetwork_ConvertItemIdxToObjId(eNxSettingsItem_NetworkConnectionStatus);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = RES_NETWORK_CONNECTION_STATUS_ID;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(RES_NETWORK_CONNECTION_STATUS_ID), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_ConnectionStatus_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NX_PRISM_MENU_ConnectionStatus_Proc;
	stMainItem.bDisable = nx_menuNetwork_GetItemDisable(eNxSettingsItem_NetworkConnectionStatus);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static HINT32 nx_menuNetwork_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
{
	HUINT32			i = 0, ulMainItems = 0;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	ulMainItems = pstContents->ulNumOfMainItem;
	for (i = 0; i < ulMainItems; i++)
	{
		pMainItem =  NX_PRISM_AppNaviCntxt_GetMainItem((void *)pstContents, i);
		if ((pMainItem == NULL) || (pMainItem->bDisable == TRUE))
		{
			continue;
		}

		break;
	}

	return	i;
}

static ONDK_Result_t	nx_menuNetwork_DoUpdatingNetworkStatusUI(Nx_SettingsNaviContext_t *pstContents , Nx_MenuNetworkContent_t *pstNetworkContent)
{
    HUINT32 ulID = 0;
    NxSettingsItem_Network_t eNetworkID = eNxSettingsItem_ConfigureLAN;
    Nx_SettingsNaviMainItem_t *stNaviOBJ;

    for(eNetworkID = eNxSettingsItem_ConfigureLAN ; eNetworkID <eNxSettingsItem_NetworkConnectionStatus; eNetworkID++)
    {

        ulID = nx_menuNetwork_ConvertItemIdxToObjId(eNetworkID);
        stNaviOBJ = NX_PRISM_AppNaviCntxt_GetMainItemByObjId(pstContents, ulID);

        /* Set Value */
        HxSTD_MemSet(stNaviOBJ->ucSubItemString, 0x00, sizeof(HUINT8)*MAX_ITEM_TEXT_LENGTH);
        HxSTD_StrNCpy ((char*)stNaviOBJ->ucSubItemString,nx_menuNetwork_GetItemValue(eNetworkID), sizeof(HUINT8)*MAX_ITEM_TEXT_LENGTH);
    	stNaviOBJ->bDisable = nx_menuNetwork_GetItemDisable(eNetworkID);
        if(TRUE == stNaviOBJ->bDisable)
        {
            ONDK_GWM_Obj_DisableObject(ulID);
        }
        else
        {
            ONDK_GWM_Obj_EnableObject(ulID);
            ONDK_GWM_Obj_SetFocus(ulID);
        }

        ONDK_GWM_APP_InvalidateObject(ulID);
    }
    return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuNetwork_DoUpdatingNetworkStatus(Nx_MenuNetworkContent_t *pstNetworkContent)
{
    HERROR          hError  = ERR_FAIL;
    NX_NET_NetDevInfo_t *pstNetDevInfoList = NULL;
    HINT32              devCount = 0;
    HINT32              lIndex = 0;
    NX_NET_NetIfType_e netIfType = eNxNet_INTERFACE_TYPE_MAX;
    NX_NET_NetIFADDR_t *pstNetIf = NULL;

    if(NULL == pstNetworkContent)
    {
        return ONDK_RET_INITFAIL;
    }

    hError = NX_NETWORK_GetNetDevInfo(&pstNetDevInfoList, &devCount);
    pstNetworkContent->lNetDevNum = devCount;

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
                if( (netIfType != eNxNet_INTERFACE_TYPE_MAX) && (NULL != pstNetIf) )
                {
                    //Updating Cuurrent Network Status LAN -> WLAN
                    NX_APP_Free(pstNetIf);
                    pstNetIf = NULL;
                }

                hError = NX_NETWORK_GetWLanInterface(&(pstNetIf));
                netIfType = eNxNet_INTERFACE_TYPE_WLAN;
                break;
            }
        }
    }

    pstNetworkContent->eCurrentIfType = netIfType;

    if(NULL != pstNetworkContent->pstNetDevInfoList)
    {
        NX_APP_Free(pstNetworkContent->pstNetDevInfoList);
        pstNetworkContent->pstNetDevInfoList = NULL;
    }

    if(NULL != pstNetIf)
    {
        NX_APP_Free(pstNetIf);
        pstNetIf = NULL;
    }

    pstNetworkContent->pstNetDevInfoList = pstNetDevInfoList;
    return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
    Nx_MenuNetworkContent_t *pstNetworkContent = NULL;
	/* param check */
	if(pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		return	ONDK_RET_FAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = NX_PRISM_AppNaviCntxt_InitMainItemArray(pstContents);
	if(eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SETTINGS_01_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuNetwork_ConvertObjIdToCursorIndx;

    //Check Network Context
    pstNetworkContent = nx_menuNetwork_GetNetworkContents();
	HxSTD_MemSet (pstNetworkContent, 0, sizeof(Nx_MenuNetworkContent_t));

    pstNetworkContent->eCurrentIfType = eNxNet_INTERFACE_TYPE_MAX;
    pstNetworkContent->pstNetDevInfoList = NULL;

    nx_menuNetwork_DoUpdatingNetworkStatus(pstNetworkContent);

	nx_menuNetwork_AddItemConfigureLAN(pstContents, lPrevMenuIdx);
    nx_menuNetwork_AddItemConfigureWLAN(pstContents, lPrevMenuIdx);
    nx_menuNetwork_AddItemConnectionStatus(pstContents, lPrevMenuIdx);

	pstContents->lDefaultFocusIdx = nx_menuNetwork_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}

static void		nx_menuNetwork_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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



static ONDK_Result_t	nx_menuNetwork_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					lItemIdx = -1;
	HUINT32					ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuNetwork_ConvertObjIdToItemIdx(lObjId);

	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_ConfigureLAN:
			case eNxSettingsItem_ConfigureWLAN:
			case eNxSettingsItem_NetworkConnectionStatus:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				break;

			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				break;
		}

		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stMainItem.ucAppName, ulProperty, stMainItem.appToLaunch,
						stMainItem.hAction, stMainItem.ulappParam1, stMainItem.ulappParam2, stMainItem.lSubItemValue/* Prev Cursor*/);
	}
	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_menuNetwork_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuNetwork_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Init Main Items
	NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1_Value);

	// Init Sub Items
	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(pstContents->mainItems, pstContents->lCurFocus, TRUE);

	/* check focus */
	if(pstContents->mainItems[pstContents->lCurFocus].bDisable)
	{
		pstContents->lDefaultFocusIdx = nx_menuNetwork_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}
	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuNetwork_GetDefaultFocus(pstContents);
		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuNetwork_InitSettingsItems(pstContents, p3);
	nx_menuNetwork_DrawDefaultUI(pstContents);

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
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuNetwork_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);

    NX_NETWORK_UnRegisterNotifier();

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuNetwork_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_ARROWLEFT:
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Settings_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (ONDK_GWM_Callback_t)NX_PRISM_MENU_Settings_Proc,
							 0, 0, 0, 0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;
		default:
			NX_APP_Print("nx_menuSystem_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	nx_menuNetwork_LaunchSubMenu(pstContents, p1);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuNetwork_MsgGwmAPKMessage(Nx_SettingsNaviContext_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)handle;
    APKE_NET_STATUS_e eEvent = p1;
    Nx_MenuNetworkContent_t	*pstNetworkContents;

    pstNetworkContents =  nx_menuNetwork_GetNetworkContents();

    if(GWM_APK_NETWORK == eAPK_Type)
    {
        switch(eEvent)
        {
            case eNETWORK_STATUS_CABLE_LINK_CONNECTED:
            case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED:
            case eNETWORK_STATUS_WIFI_AP_DISCONNECTED:
            case eNETWORK_STATUS_WIFI_AP_RECONNECTED:
                HxLOG_Error("\n\n#### nx_menuNetwork_MsgGwmAPKMessage(%d) \n",eEvent);
                nx_menuNetwork_DoUpdatingNetworkStatus(pstNetworkContents);
                nx_menuNetwork_DoUpdatingNetworkStatusUI(pstContents , pstNetworkContents);
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
ONDK_Result_t	NX_PRISM_MEMNU_Network_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	pstContents = nx_menuNetwork_GetNaviContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuNetwork_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuNetwork_MsgGwmClick(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuNetwork_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_OBJ_FOCUS_CHANGED:
            nx_menuNetwork_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_menuNetwork_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	ONDK_MESSAGE_BREAK;
		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus1Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	ONDK_MESSAGE_BREAK;
		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;
        case MSG_GWM_APK_MESSAGE:
            eResult = nx_menuNetwork_MsgGwmAPKMessage(pstContents,handle, p1, p2, p3);
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

