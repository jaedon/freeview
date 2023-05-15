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
 * @file	  		nx_prism_menu_system.c
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


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_SystemInformation,
	eNxSettingsItem_SmartcardInformation,
	eNxSettingsItem_UpdatedList,
	eNxSettingsItem_PowerTimer,
	eNxSettingsItem_HDMICEC,
	eNxSettingsItem_PowerManagement,
	eNxSettingsItem_FactoryDefault,
	eNxSettingsItem_CustomerRegistration,
	eNxSettingsItem_IREDETOSoftwareUpdate,
	eNxSettingsItem_ConditionalAccess,
	eNxSettingsItem_MailMessage,
	eNxSettingsItem_System_MAX
} NxSettingsItem_System_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SettingsNaviContext_t		s_stMenuSystemContent;
static NXUI_DialogInstance_t		s_stSystem_selective_dInstance;
static NX_PinDialogInstance_t		s_stSystem_pincode_dInstance;
static NXUI_DialogInstance_t		s_stSystem_progress_dInstance;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuSystem_SelectiveDialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuSystem_CreateSelectiveDialog(HINT8 *szStrID);
static ONDK_Result_t	nx_menuSystem_DestroySelectiveDialog(void);
static ONDK_Result_t	nx_menuSystem_PinCodeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuSystem_CreatePinCodeDialog(void);
static ONDK_Result_t	nx_menuSystem_DestroyPinCodeDialog(void);
static ONDK_Result_t	nx_menuSystem_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuSystem_CreateProcessingMsg(HINT8 *pszStrID);
static ONDK_Result_t	nx_menuSystem_DestroyProcessingMsg(void);

static inline Nx_SettingsNaviContext_t	*nx_menuSystem_GetContents(void);
static inline HINT32	nx_menuSystem_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents);

static HINT32			nx_menuSystem_ConvertObjIdToCursorIndx(HINT32 lObjId);
static HINT32			nx_menuSystem_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSystem_ConvertItemIdxToObjId(HINT32 lItemId);

static HBOOL			nx_menuSystem_GetItemDisable(NxSettingsItem_System_t eNxSystemItem);
static void				nx_menuSystem_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuSystem_ActionOTAForIRDETO(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSystem_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuSystem_AddItemSystemInformation(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemSmartcardInformation(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemUpdatedList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemPowerTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemHDMICEC(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemPowerManagement(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemFactoryDefault(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemCustomerRegistration(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemIREDETOSoftwareUpdate(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemConditionalAccess(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);
static ONDK_Result_t	nx_menuSystem_AddItemMailMessage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);

static ONDK_Result_t	nx_menuSystem_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx);

static ONDK_Result_t	nx_menuSystem_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents);
static ONDK_Result_t	nx_menuSystem_DoFactoryDefault(Nx_SettingsNaviContext_t *pstContents);

static ONDK_Result_t	nx_menuSystem_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSystem_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSystem_MsgAppDlgClicked(Nx_SettingsNaviContext_t *pstContents, HINT32 lClickedBtn);
static ONDK_Result_t	nx_menuSystem_RecvSwUpEvent(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuSystem_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (&s_stSystem_selective_dInstance, message, hAction,p1, p2, p3);
}


static void	nx_menuSystem_CreateSelectiveDialog(HINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stSystem_selective_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuSystem_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuSystem_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}


static ONDK_Result_t	nx_menuSystem_DestroySelectiveDialog(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuSystem_SelectiveDialogProc);
}


static ONDK_Result_t	nx_menuSystem_PinCodeDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stSystem_pincode_dInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuSystem_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;
	NX_PinDialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stSystem_pincode_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NX_PinDialogInstance_t));

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(pstDialog,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSystem_PinCodeDialogProc", APP_DEFAULT_PRIORITY, nx_menuSystem_PinCodeDialogProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuSystem_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuSystem_PinCodeDialogProc);
}


static ONDK_Result_t	nx_menuSystem_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stSystem_progress_dInstance, message, hAction, p1, p2, p3);
}


static void		nx_menuSystem_CreateProcessingMsg(HINT8 *pszStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stSystem_progress_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);

	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszStrID));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSystem_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuSystem_ProcessingMsgProc, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuSystem_DestroyProcessingMsg(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuSystem_ProcessingMsgProc);
}


static inline Nx_SettingsNaviContext_t	*nx_menuSystem_GetContents(void)
{
	return	&s_stMenuSystemContent;
}


static inline HINT32	nx_menuSystem_GetDefaultFocus(Nx_SettingsNaviContext_t *pstContents)
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


static HINT32	nx_menuSystem_ConvertObjIdToCursorIndx(HINT32 lObjId)
{
	HUINT32		ulMainItems = 0, i = 0;
	Nx_SettingsNaviContext_t	*pstContents = NULL;
	Nx_SettingsNaviMainItem_t	*pMainItem = NULL;

	pstContents = nx_menuSystem_GetContents();
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


static HINT32	nx_menuSystem_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_System);

	if((lItemId < 0) || (lItemId >= eNxSettingsItem_System_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSystem_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if((lItemId < 0) || (lItemId >= eNxSettingsItem_System_MAX))
	{
		return 0;
	}

	return	(eNxSetting_System | lItemId);
}


static HBOOL	nx_menuSystem_GetItemDisable(NxSettingsItem_System_t eNxSystemItem)
{
	HBOOL	bDisable = TRUE;

	switch (eNxSystemItem)
	{
		case eNxSettingsItem_SystemInformation:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SmartcardInformation:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_UpdatedList:
			bDisable = TRUE;
			break;
		case eNxSettingsItem_PowerTimer:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_HDMICEC:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_PowerManagement:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_FactoryDefault:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_CustomerRegistration:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_IREDETOSoftwareUpdate:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_ConditionalAccess:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_MailMessage:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuSystem_UpdateCurrentTime(Nx_SettingsNaviContext_t *pstContents)
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


static ONDK_Result_t	nx_menuSystem_ActionOTAForIRDETO(Nx_SettingsNaviContext_t *pstContents)
{
#if defined(CONFIG_OTA_IRDETO)
	HERROR		hError = ERR_FAIL;
	Handle_t	hSwupdateHandle = HANDLE_NULL;

	NX_CHANNEL_StopChannel();

	hError = NX_CORE_SWUP_StartDetectWithType(eDxSwUpdate_SPEC_IRDETO, &hSwupdateHandle);
	if (hError == ERR_FAIL)
	{
		HxLOG_Error("\n\nCan't Not Start S/W Update!!!\n\n");
		return ONDK_RET_FAIL;
	}
	//pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Detect;
#else
	NX_COMMON_NOT_USED_PARAM(pstContents);
#endif

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_LaunchSubMenu(Nx_SettingsNaviContext_t *pstContents, HINT32 lObjId)
{
	HINT32					lItemIdx = -1;
	HUINT32					ulProperty = 0;
	Nx_SettingsNaviMainItem_t	stMainItem;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lItemIdx = nx_menuSystem_ConvertObjIdToItemIdx(lObjId);
	HxSTD_memcpy(&stMainItem, &(pstContents->mainItems[pstContents->lCurFocus]), sizeof(Nx_SettingsNaviMainItem_t));

	if (stMainItem.appToLaunch != NULL)
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_UpdatedList:
			case eNxSettingsItem_PowerTimer:
			case eNxSettingsItem_PowerManagement:
			case eNxSettingsItem_FactoryDefault:
			case eNxSettingsItem_HDMICEC:
			case eNxSettingsItem_ConditionalAccess:
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
	else
	{
		switch (lItemIdx)
		{
			case eNxSettingsItem_FactoryDefault:
				nx_menuSystem_CreateSelectiveDialog((HINT8*)RES_MESG_3231_ID);
				break;

			case eNxSettingsItem_IREDETOSoftwareUpdate:
				nx_menuSystem_ActionOTAForIRDETO(pstContents);
				break;

			default:
				NX_APP_Info("Do not Action Item:(%d)\n", lItemIdx);
				break;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemSystemInformation(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_SystemInfomation);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_SystemInfomation);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_SystemInformation);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_SysInfo_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_SystemInformation);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemSmartcardInformation(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_SmartcardInfomation);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_SmartcardInfomation);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_SmartcardInformation);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_SmtCardInfo_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_SmartcardInformation);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemUpdatedList(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_UpdateList);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSettingsItem_UpdatedList);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_UpdatedList);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_UpdatedList_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_UpdatedList);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemPowerTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_PowerTimer);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_PowerTimer);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_PowerTimer);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_PowerTimer_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_PowerTimer);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemHDMICEC(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_HDMICEC);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_HDMICEC);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_HDMICEC);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_HDMICEC_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_HDMICEC);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_menuSystem_AddItemPowerManagement(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_PowerManagement);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_PowerManagement);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_PowerManagement);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, (char*)"NX_PRISM_MENU_PowerManagement_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_PowerManagement);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemFactoryDefault(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_FactoryDefault);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_FactoryDefault);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_FactoryDefault);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NX_PRISM_MENU_FactoryDefault_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_FactoryDefault);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemCustomerRegistration(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_CustomerRegistration);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_CustomerRegistration);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_CustomerRegistration);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, "NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NULL;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_CustomerRegistration);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemIREDETOSoftwareUpdate(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_IREDETOSoftwareUpdate);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_IREDETOSoftwareUpdate);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_IREDETOSoftwareUpdate);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, "NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NULL;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_IREDETOSoftwareUpdate);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_AddItemConditionalAccess(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_ConditionalAccess);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_ConditionalAccess);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_ConditionalAccess);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing */
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName,(char*)"NX_PRISM_MENU_ConditionalAccess_Proc", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = pstMenuInfo->appToLaunch;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_ConditionalAccess);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSystem_AddItemMailMessage(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	Nx_SettingsNaviMainItem_t	stMainItem = {0, };
	NxUiMenuCfgInfo 			*pstMenuInfo = NULL;


	pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId(eNxSetting_System, eNxSetting_MailMessage);
	if (pstMenuInfo == NULL)
	{
		NX_APP_Info("Not Support NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndMenuId : (0x%x)\n", eNxSetting_MailMessage);
		return	ONDK_RET_FAIL;
	}

	NX_PRISM_AppNaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the prev menu Item Idx */
	stMainItem.lSubItemValue = lPrevMenuIdx;

	/* Set the object ID */
	stMainItem.ulMainBtnId = nx_menuSystem_ConvertItemIdxToObjId(eNxSettingsItem_MailMessage);

	/* set title, Loc Id */
	stMainItem.ucTitleLocId = pstMenuInfo->pszTitleStrId;
	HxSTD_StrNCpy((char*)stMainItem.ucTitle, (char*)ONDK_GetString(pstMenuInfo->pszTitleStrId), MAX_ELEMENT_TEXT_LENGTH);

	/*	Function to launch : Arguments: nothing	*/
	HxSTD_StrNCpy ((char*)stMainItem.ucAppName, "NULL", MAX_ELEMENT_TEXT_LENGTH);
	stMainItem.appToLaunch = NULL;
	stMainItem.bDisable = nx_menuSystem_GetItemDisable(eNxSettingsItem_MailMessage);

	/* sub menu */
	NX_PRISM_AppNaviCntxt_AddSubItemTitleToMainItemList(&stMainItem);

	/* Register the main item to the contents */
	NX_PRISM_AppNaviCntxt_AppendMainItem (pstContents, &stMainItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_DrawDefaultUI(Nx_SettingsNaviContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSystem_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Init Main Items
	NX_PRISM_COMP_Settings_InitNaviMainItems2Depth(pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawNaviMainItem_Cursor_A1);

	// Init Sub Items
	NX_PRISM_COMP_Settings_InitNaviMainSubItemList2Depth(pstContents->mainItems, pstContents->lCurFocus, TRUE);

	/* check focus */
	if(pstContents->mainItems[pstContents->lCurFocus].bDisable)
	{
		/* 현재 선택된 아이템이 Disable 이면 Focus 를 초기화 해야만 SubItemList 정보가 제대로 표시된다.*/
		pstContents->lDefaultFocusIdx = nx_menuSystem_GetDefaultFocus(pstContents);
		pstContents->lCurFocus = pstContents->lDefaultFocusIdx;
	}

	eResult = ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	if (eResult != ONDK_RET_OK)
	{
		pstContents->lCurFocus = nx_menuSystem_GetDefaultFocus(pstContents);
		/* re-set focus */
		ONDK_GWM_Obj_SetFocus(pstContents->mainItems[pstContents->lCurFocus].ulMainBtnId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_DoFactoryDefault(Nx_SettingsNaviContext_t *pstContents)
{
	nx_menuSystem_DestroyPinCodeDialog();
	nx_menuSystem_CreateProcessingMsg((HINT8*)RES_MESG_5383_ID);
	NX_SYSTEM_DoFactoryDefault();

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSystem_InitSettingsItems(Nx_SettingsNaviContext_t *pstContents, HINT32 lPrevMenuIdx)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32				i = 0, ulMenuItemNum = 0;
	NxUiMenuCfgInfo		*pstMenuInfo = NULL;

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
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SYSTEM_SETTINGS_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->pConvFunc = nx_menuSystem_ConvertObjIdToCursorIndx;

	ulMenuItemNum = NX_PRISM_AppNaviCntxt_GetTotalMenuItemNumByMainMenuId(eNxSetting_System);
	for (i = 0; i < ulMenuItemNum; i++)
	{
		pstMenuInfo = NX_PRISM_AppNaviCntxt_GetMenuItemByMainMenuIdAndOrderId(eNxSetting_System, i);
		switch (pstMenuInfo->lMenuId)
		{
			case eNxSetting_SystemInfomation:
				nx_menuSystem_AddItemSystemInformation(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_SmartcardInfomation:
				nx_menuSystem_AddItemSmartcardInformation(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_PowerTimer:
				nx_menuSystem_AddItemPowerTimer(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_PowerManagement:
				nx_menuSystem_AddItemPowerManagement(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_HDMICEC:
				nx_menuSystem_AddItemHDMICEC(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_FactoryDefault:
				nx_menuSystem_AddItemFactoryDefault(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_CustomerRegistration:
				nx_menuSystem_AddItemCustomerRegistration(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_IREDETOSoftwareUpdate:
				nx_menuSystem_AddItemIREDETOSoftwareUpdate(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_ConditionalAccess:
				nx_menuSystem_AddItemConditionalAccess(pstContents, lPrevMenuIdx);
				break;
			case eNxSetting_MailMessage:
				nx_menuSystem_AddItemMailMessage(pstContents, lPrevMenuIdx);
				break;
			default:
				NX_APP_Error("Error!!! Invalid Menu Item : (0x%x) Fail !! \n", pstMenuInfo->lMenuId);
				break;
		}
	}

	//nx_menuSystem_AddItemUpdatedList(pstContents, lPrevMenuIdx);

	pstContents->lDefaultFocusIdx = nx_menuSystem_GetDefaultFocus(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_MsgGwmCreate(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_menuSystem_InitSettingsItems(pstContents, p3);
	nx_menuSystem_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuSystem_MsgGwmDestroy(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	nx_menuSystem_DestroySelectiveDialog();
	nx_menuSystem_DestroyPinCodeDialog();
	nx_menuSystem_DestroyProcessingMsg();
	NX_PRISM_AppNaviCntxt_FreeMainItemArray_KeepFocus(pstContents);

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_MsgGwmHapiMsg(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSystem_MsgGwmKeyDown(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
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


static ONDK_Result_t	nx_menuSystem_MsgGwmObjFocusChanged(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, FALSE, NxPrism_FindFocus_Current);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_MsgGwmClick(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	nx_menuSystem_LaunchSubMenu(pstContents, p1);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_MsgGwmTimer(Nx_SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSystem_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSystem_MsgAppDlgClicked(Nx_SettingsNaviContext_t *pstContents, HINT32 lClickedBtn)
{
	nx_menuSystem_DestroySelectiveDialog();
	switch(lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT: // YES
			nx_menuSystem_CreatePinCodeDialog();
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSystem_RecvSwUpEvent(Nx_SettingsNaviContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_OTA_IRDETO)
	DxSwUpdate_EventData_t *pstDetectedInform = NULL;

	switch (p1)
	{
		case eDxSWUPDATE_EVENT_DetectSuccess:
			pstDetectedInform = NX_CORE_SWUP_GetDetectedInform(eDxSwUpdate_SPEC_IRDETO);
			if (pstDetectedInform != NULL)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MENU_SwUpDownload_Proc", (APP_DEFAULT_PRIORITY | UISTATE_NORMAL),
								NX_PRISM_MENU_SwUpInfo_Proc, 0, 0, pstDetectedInform->eSourceType, eNxSetting_SystemInfomation);
			}
			break;

		case eDxSWUPDATE_EVENT_DetectFail:
			HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_DetectFail)\n");
			//pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
			break;

		case eDxSWUPDATE_EVENT_Erorr:
			HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_Erorr)\n");
			//pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
			break;

		case eDxSWUPDATE_EVENT_Finish:
			HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_Finish)\n");
			break;

		default:
			break;
	}

#endif
	NX_COMMON_NOT_USED_ARGUMENT(handle, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_System_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SettingsNaviContext_t	*pstContents = NULL;

	pstContents = nx_menuSystem_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuSystem_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSystem_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			eResult = nx_menuSystem_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_menuSystem_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_SCROLLUP:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Up);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			NX_PRISM_COMP_Settings_NaviReflectFocus2Depth(pstContents, p1, TRUE, NxPrism_FindFocus_Down);
			return	GWM_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			eResult = nx_menuSystem_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuSystem_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSystem_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuSystem_MsgAppDlgClicked(pstContents, p1);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			eResult = nx_menuSystem_DoFactoryDefault(pstContents);
			break;
		case MSG_APP_SYSTEM_PIN_DENIED:
			break;

		case MSG_CORE_SWUP_NOTIFY:
			nx_menuSystem_RecvSwUpEvent(pstContents, (Handle_t)NULL, p1, p2, p3);
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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

