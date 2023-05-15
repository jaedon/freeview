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
 * @file	  		nx_prism_menu_channel.c
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
#include <nx_port.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_FavouriteChannels,
	eNxSettingsItem_EditChannelList,
	eNxSettingsItem_EditPreferredList,
	eNxSettingsItem_SignalTest,
	eNxSettingsItem_SearchChannels,
	eNxSettingsItem_ImportChannels,
	eNxSettingsItem_ExportChannels,
	eNxSettingsItem_Channel_Max
} NxSettingsItem_Channel_t;

typedef enum {
	eNxChannelErrType_ErrorNone = 0,
	eNxChannelErrType_ErrorFailure,
	eNxChannelErrType_ErrorNoUsb,
	eNxChannelErrType_ErrorNofile
} NxChannelErrType_t;

typedef enum {
	eNxSettingsItem_DialogType_None = 0,
	eNxSettingsItem_DialogType_Reboot,
} NxSettingsItem_Channel_DialogType_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SettingsNaviContext_t		s_ChannelContent;
static NXUI_DialogInstance_t		s_stDialogMessageInstance;
static NX_PinDialogInstance_t		s_stMenuChannel_pincode_dInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t	*nx_menuChannel_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuChannel_SelectiveDialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuChannel_CreateSelectiveDialog(HINT8 *szStrID, HUINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuChannel_DestroySelectiveDialog(void);
static ONDK_Result_t	nx_menuChannel_ResponseDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuChannel_CreateResponseDialog(HINT8 *szStrID);
static ONDK_Result_t	nx_menuChannel_DestroyResponseDialog(void);
static ONDK_Result_t	nx_menuChannel_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuChannel_CreateProcessingMsg(HINT8 *pszStrID);
static ONDK_Result_t	nx_menuChannel_DestroyProcessingMsg(void);
static ONDK_Result_t	nx_menuChannel_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_DestroyPinCodeDialog(void);
static void				nx_menuChannel_CreatePinCodeDialog(void);

static inline Nx_SettingsNaviContext_t	*nx_menuChannel_GetContents(void);
static inline HINT32	nx_menuChannel_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents);
static HINT32			nx_menuChannel_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuChannel_ConvertItemIdxToObjId(HINT32 lItemId);
static HINT32			nx_menuChannel_ConvertObjIdToCursorIndx(HINT32 lObjId);
static HBOOL			nx_menuChannel_GetItemiDisable(NxSettingsItem_Channel_t eNxChannellItem);

static void				nx_menuChannel_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents);
#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_GWM_Callback_t		nx_menuChannel_GetSearchChannelsLaunchSubMenu_Satellite(void);
#endif
static ONDK_GWM_Callback_t		nx_menuChannel_GetSearchChannelsLaunchSubMenu(void);
static HBOOL			nx_menuChannel_CheckPinCodeForLaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lItemIdx);
static ONDK_Result_t	nx_menuChannel_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuChannel_AddItemFavouriteChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemEditChannelList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemEditPreferredList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemSignalTest(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemSearchChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemImportChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_AddItemExportChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuChannel_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);

static ONDK_Result_t	nx_menuChannel_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuChannel_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuChannel_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuChannel_MsgGwmImportExportNotify(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);



/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t	*nx_menuChannel_GetDialogPopupContents(void)
{
	return &s_stDialogMessageInstance;
}


/******************************************************************************/
#define __Select_Dialog_Functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuChannel_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuChannel_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static void	nx_menuChannel_CreateSelectiveDialog(HINT8 *szStrID, HUINT32 ulDlgWhoseId)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuChannel_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));
	pstDialog->ulDlgWhoseId = ulDlgWhoseId;

	ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuChannel_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuChannel_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}


static ONDK_Result_t	nx_menuChannel_DestroySelectiveDialog(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuChannel_SelectiveDialogProc);
}


/******************************************************************************/
#define __Response_Dialog_Functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuChannel_ResponseDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuChannel_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static void	nx_menuChannel_CreateResponseDialog(HINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuChannel_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));
	pstDialog->ulDlgWhoseId = eNxSettingsItem_DialogType_None;

	ulDlgAttr = (NX_DLG_NO_BUTTON|NX_DLG_TIMEOUT_3SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuChannel_ResponseDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuChannel_ResponseDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}


static ONDK_Result_t	nx_menuChannel_DestroyResponseDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuChannel_ResponseDialogProc);

	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Processing_Dialog_Functions_________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuChannel_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(nx_menuChannel_GetDialogPopupContents(), message, hAction, p1, p2, p3);
}


static void		nx_menuChannel_CreateProcessingMsg(HINT8 *pszStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuChannel_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);

	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszStrID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuChannel_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuChannel_ProcessingMsgProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuChannel_DestroyProcessingMsg(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuChannel_ProcessingMsgProc);
}


/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuChannel_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stMenuChannel_pincode_dInstance, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_menuChannel_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuChannel_DisplayPinCodeDialog);
}

static void		nx_menuChannel_CreatePinCodeDialog(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0, ulPinCodeType = 0;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			ulPinCodeType = eNxPinCodeDataType_Installation;
			break;
		default:
			ulPinCodeType = eNxPinCodeDataType_ChannelSetting;
			break;
	}

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stMenuChannel_pincode_dInstance, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID), ulPinCodeType);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuGeneral_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_menuChannel_DisplayPinCodeDialog, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static inline Nx_SettingsNaviContext_t	*nx_menuChannel_GetContents(void)
{
	return	&s_ChannelContent;
}


static inline HINT32	nx_menuChannel_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
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


static HINT32	nx_menuChannel_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_Channel);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Channel_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuChannel_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_Channel_Max))
	{
		return 0;
	}

	return	(eNxSetting_Channel | lItemId);
}


static HINT32	nx_menuChannel_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuChannel_GetContents();
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


static HBOOL	nx_menuChannel_GetItemiDisable(NxSettingsItem_Channel_t eNxChannellItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxChannellItem)
	{
		case eNxSettingsItem_FavouriteChannels:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_EditChannelList:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_EditPreferredList:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SignalTest:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SearchChannels:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ImportChannels:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ExportChannels:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuChannel_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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


#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_GWM_Callback_t		nx_menuChannel_GetSearchChannelsLaunchSubMenu_Satellite(void)
{
	HINT32		lValue = 0;
	HERROR		hError = ERR_FAIL;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;


	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_ANTENNA_GetAntennaType() Fail !!\n");
		eAntennaType = eNxSat_AntType_LNB_Only;
	}
	else
	{
		eAntennaType = (NX_SAT_AntennaType_e)lValue;
	}

	switch (eAntennaType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
			return	(ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_SatFixedAntna_Proc;
			break;
		case eNxSat_AntType_SCD:
			return	(ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_SatScdAntena_Proc;
			break;
		case eNxSat_AntType_Motor:
			// TODO: Add Code
			return	(ONDK_GWM_Callback_t)NULL;

		default:
			NX_APP_Error("Error!!! Not Support eAntennaType:(%d) Fail !!\n", eAntennaType);
			return	(ONDK_GWM_Callback_t)NULL;
	}
}
#endif


static ONDK_GWM_Callback_t		nx_menuChannel_GetSearchChannelsLaunchSubMenu(void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	return	nx_menuChannel_GetSearchChannelsLaunchSubMenu_Satellite();
#else
	return	(ONDK_GWM_Callback_t)NULL;
#endif
}


static HBOOL	nx_menuChannel_CheckPinCodeForLaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lItemIdx)
{
	HBOOL			bCheckPinCode = FALSE;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (lItemIdx)
	{
		case eNxSettingsItem_SearchChannels:
			if (eOpType == eDxOPERATOR_AMX)
			{
				bCheckPinCode = TRUE;
			}
			else
			{
				bCheckPinCode = FALSE;
			}
			break;
		default:
			bCheckPinCode = FALSE;
			break;
	}

	return	bCheckPinCode;
}


static ONDK_Result_t	nx_menuChannel_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	HUINT32			ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuChannel_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_SignalTest:
			case eNxSettingsItem_SearchChannels:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				break;

			default:
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_NORMAL);
				break;
		}

		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)stMainItem.ucAppName, ulProperty, stMainItem.appToLaunch,
						stMainItem.hAction, stMainItem.ulappParam1, stMainItem.ulappParam2, stMainItem.lSubItemValue);
	}
	else
	{
		switch(lItemIdx)
		{
			case eNxSettingsItem_ImportChannels:
				nx_menuChannel_CreateProcessingMsg(RES_IMPORTING_CHANNELS_ID);
				NX_SYSTEM_StartChannelImport();
				break;
			case eNxSettingsItem_ExportChannels:
				nx_menuChannel_CreateProcessingMsg(RES_EXPORTING_CHANNELS_ID);
				NX_SYSTEM_StartChannelExport();
				break;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemFavouriteChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_EditFavList);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_EditFavList);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_FavouriteChannels);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_FavChList_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_FavouriteChannels);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemEditChannelList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_EditChList);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_EditChList);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_EditChannelList);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_EditChList_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_EditChannelList);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemEditPreferredList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_EditPreferredList);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_EditPreferredList);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_EditPreferredList);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_EditPreferredList);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemSignalTest(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_SignalTest);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_SignalTest);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_SignalTest);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_SignalTest);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemSearchChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_SearchChannel);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_SearchChannel);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_SearchChannels);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
	if (pstMenuInfo->appToLaunch == NULL)
	{
		stMainItem.appToLaunch = NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu();
	}
	else
	{
		stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	}

	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_SearchChannels);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemImportChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_ImportChannel);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_ImportChannel);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_ImportChannels);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
//	if (pstMenuInfo->appToLaunch == NULL)
//	{
//		stMainItem.appToLaunch = NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu();
//	}
//	else
//	{
		stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
//	}

	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_ImportChannels);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_AddItemExportChannels(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_Channel, eNxSetting_ExportChannel);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_ExportChannel);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_ExportChannels);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NULL", MAX_ELEMENT_TEXT_LENGTH);
//	if (pstMenuInfo->appToLaunch == NULL)
//	{
//		stMainItem.appToLaunch = NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu();
//	}
//	else
//	{
		stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
//	}

	stMainItem.bDisable = nx_menuChannel_GetItemiDisable(eNxSettingsItem_ExportChannels);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuChannel_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);
	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Init Main Items
	NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1);

	// Init Sub Items
	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(pstContents->mainItems, pstContents->lCurFocus, TRUE);

	/* check focus */
	if (pstContents->mainItems[pstContents->lCurFocus].bDisable)
	{
		/* 현재 선택된 아이템이 Disable 이면 Focus 를 초기화 해야만 SubItemList 정보가 제대로 표시된다.*/
		pstContents->lDefaultFocusIdx = nx_menuChannel_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}

	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuChannel_GetDefaultFocus(pstContents);

		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32				i = 0, ulMenuItemNum = 0;
	NxUiMenuCfgInfo		*pstMenuInfo = NULL;

	/* param check */
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL!! \n");
		return	ONDK_RET_FAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = NX_PRISM_AppNaviCntxt_InitMainItemArray(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppNaviCntxt_InitMainItemArray() !! \n");;
		return	ONDK_RET_FAIL;
	}

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_CHANNEL_SETTINGS_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuChannel_ConvertObjIdToCursorIndx;

	ulMenuItemNum = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(eNxSetting_Channel);
	for (i = 0; i < ulMenuItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(eNxSetting_Channel, i);
		switch (pstMenuInfo->lMenuId)
		{
			case eNxSetting_EditFavList:
				nx_menuChannel_AddItemFavouriteChannels(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_EditChList:
				nx_menuChannel_AddItemEditChannelList(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_EditPreferredList:
				nx_menuChannel_AddItemEditPreferredList(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_SignalTest:
				nx_menuChannel_AddItemSignalTest(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_SearchChannel:
				nx_menuChannel_AddItemSearchChannels(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_ImportChannel:
				nx_menuChannel_AddItemImportChannels(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_ExportChannel:
				nx_menuChannel_AddItemExportChannels(pstContents, lPrevMenuIdx);
				break;
			default:
				NX_APP_Error("Error!!! Invalid Menu Item : (0x%x) Fail !! \n", pstMenuInfo->lMenuId);
				break;
		}
	}

	pstContents->lDefaultFocusIdx = nx_menuChannel_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuChannel_InitSettingsItems(pstContents, p3);
	nx_menuChannel_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

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


static ONDK_Result_t	nx_menuChannel_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	nx_menuChannel_DestroyPinCodeDialog();
	nx_menuChannel_DestroyProcessingMsg();
	nx_menuChannel_DestroyResponseDialog();
	nx_menuChannel_DestroySelectiveDialog();

	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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

#if defined(CONFIG_OP_AMX)
	{
		// Check Hidden key
		HBOOL		bHiddenMenu = FALSE;

		bHiddenMenu = NX_PRISM_MENU_HiddenManagement_CheckHiddenTechnicalMenuKeySeq(lKeyId);
		if (bHiddenMenu == TRUE)
		{
			NX_PRISM_MENU_HiddenTechnicalMenu();
			return	ONDK_MESSAGE_BREAK;
		}
	}
#endif

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_ARROWLEFT:
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Settings_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_Settings_Proc, 0, 0, 0, 0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
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
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuChannel_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL			bCheckPinCode = FALSE;
	HINT32			lItemIdx = -1;
	Nx_SettingsNaviMainItem_t	stMainItem;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuChannel_ConvertObjIdToItemIdx(p1);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));
	if (stMainItem.appToLaunch != NULL)
	{
		bCheckPinCode = nx_menuChannel_CheckPinCodeForLaunchSubMenu(pstContents, lItemIdx);
		if (bCheckPinCode == TRUE)
		{
			nx_menuChannel_CreatePinCodeDialog();
			return	ONDK_RET_OK;
		}
	}

	nx_menuChannel_LaunchSubMenu(pstContents, p1);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuChannel_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuChannel_MsgGwmImportExportNotify(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulType = 0, ulResult = 0;

	ulType = (HUINT32)p1;
	ulResult = (HUINT32)p2;

	nx_menuChannel_DestroyProcessingMsg();

	if (ulResult == eNxChannelErrType_ErrorNone)
	{
		if (ulType == 1)	/* Import */
		{
			nx_menuChannel_CreateSelectiveDialog(RES_MESG_6126_ID, eNxSettingsItem_DialogType_Reboot);
		}
		else				/* Export */
		{
			nx_menuChannel_CreateResponseDialog(STR_MESG_808_ID);
		}
	}
	else if (ulResult == eNxChannelErrType_ErrorFailure)
	{
		if (ulType == 1)
		{
			nx_menuChannel_CreateResponseDialog(RES_MESG_4199_ID);
		}
		else
		{
			nx_menuChannel_CreateResponseDialog(RES_MESG_4200_ID);
		}
	}
	else if (ulResult == eNxChannelErrType_ErrorNoUsb)
	{
		nx_menuChannel_CreateResponseDialog(RES_MESG_811_ID);
	}
	else if (ulResult == eNxChannelErrType_ErrorNofile)
	{
		nx_menuChannel_CreateResponseDialog(RES_NO_FILES_DETECTED_01_ID);
	}

	return ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Channel_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	HINT32 				lObjId = -1;
	Nx_SettingsNaviContext_t	*pstContents = NULL;

	pstContents = nx_menuChannel_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuChannel_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuChannel_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuChannel_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_menuChannel_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			eResult = nx_menuChannel_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuChannel_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			nx_menuChannel_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			lObjId = nx_menuChannel_ConvertItemIdxToObjId(eNxSettingsItem_SearchChannels);
			eResult = nx_menuChannel_LaunchSubMenu(pstContents, lObjId);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_CORE_IMPORT_EXPORT_NOTIFY:
			NX_APP_Message("MSG_CORE_IMPORT_EXPORT_NOTIFY\n");
			nx_menuChannel_MsgGwmImportExportNotify(pstContents, p1, p2, p3);
			break;

		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


ONDK_GWM_Callback_t		NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu(void)
{
	return nx_menuChannel_GetSearchChannelsLaunchSubMenu();
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

