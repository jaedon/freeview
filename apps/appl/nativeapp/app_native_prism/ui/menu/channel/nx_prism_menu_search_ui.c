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
 * @file	  		nx_prism_menu_search_ui.c
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
#include <nx_port.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_SEARCH_UI_MAX_ANT_NUM_IN_UI					CONFIG_APUI_SAT_ANTENNA_MAX_SLOT
#define	NX_MENU_SEARCH_UI_MAX_SVCTYPE_TV					0 // TV, RADIO
#define	NX_MENU_SEARCH_UI_MAX_SVCTYPE_RADIO					1 // TV, RADIO
#define	NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM					2 // TV, RADIO

#define	NX_MENU_SEARCH_UI_PROG_100_PER						100

#define	NX_MENU_SEARCH_UI_TV_TEXT_X							80
#define	NX_MENU_SEARCH_UI_TV_TEXT_Y							(158 - NX_PRISM_FONT_SIZE_24)
#define	NX_MENU_SEARCH_UI_TV_TEXT_W							530
#define	NX_MENU_SEARCH_UI_TV_TEXT_H							32

#define	NX_MENU_SEARCH_UI_TV_INFO_BOX_X						60
#define	NX_MENU_SEARCH_UI_TV_INFO_BOX_Y						172
#define	NX_MENU_SEARCH_UI_TV_INFO_BOX_W						570
#define	NX_MENU_SEARCH_UI_TV_INFO_BOX_H						248

#define	NX_MENU_SEARCH_UI_RADIO_TEXT_X						670
#define	NX_MENU_SEARCH_UI_RADIO_TEXT_Y						(158 - NX_PRISM_FONT_SIZE_24)
#define	NX_MENU_SEARCH_UI_RADIO_TEXT_W						530
#define	NX_MENU_SEARCH_UI_RADIO_TEXT_H						32

#define	NX_MENU_SEARCH_UI_RADIO_INFO_BOX_X					650
#define	NX_MENU_SEARCH_UI_RADIO_INFO_BOX_Y					172
#define	NX_MENU_SEARCH_UI_RADIO_INFO_BOX_W					570
#define	NX_MENU_SEARCH_UI_RADIO_INFO_BOX_H					248

#define	NX_MENU_SEARCH_UI_TPINFO_TEXT_X						80
#define	NX_MENU_SEARCH_UI_TPINFO_TEXT_Y						(470 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SEARCH_UI_TPINFO_TEXT_W						1000
#define	NX_MENU_SEARCH_UI_TPINFO_TEXT_H						30

#define	NX_MENU_SEARCH_UI_PROG_TEXT_X						1100
#define	NX_MENU_SEARCH_UI_PROG_TEXT_Y						(470 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SEARCH_UI_PROG_TEXT_W						100
#define	NX_MENU_SEARCH_UI_PROG_TEXT_H						30

#define	NX_MENU_SEARCH_UI_PROG_BAR_X						80
#define	NX_MENU_SEARCH_UI_PROG_BAR_Y						485
#define	NX_MENU_SEARCH_UI_PROG_BAR_W						1120
#define	NX_MENU_SEARCH_UI_PROG_BAR_H						3

#define	NX_MENU_SEARCH_UI_TOTAL_TEXT_X						140
#define	NX_MENU_SEARCH_UI_TOTAL_TEXT_Y						(553 - NX_PRISM_FONT_SIZE_32)
#define	NX_MENU_SEARCH_UI_TOTAL_TEXT_W						500
#define	NX_MENU_SEARCH_UI_TOTAL_TEXT_H						40

#define	NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_X				(NX_MENU_SEARCH_UI_TOTAL_TEXT_X + NX_MENU_SEARCH_UI_TOTAL_TEXT_W + 8)
#define	NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_Y				(553 - NX_PRISM_FONT_SIZE_32)
#define	NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_W				500
#define	NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_H				40

#define	NX_MENU_SEARCH_UI_STOP_BUTTON_X						483
#define	NX_MENU_SEARCH_UI_STOP_BUTTON_Y						615
#define	NX_MENU_SEARCH_UI_STOP_BUTTON_W						315
#define	NX_MENU_SEARCH_UI_STOP_BUTTON_H						55

#define	NX_MENU_SEARCH_UI_LIST_ITEM_MAX						7
#define	NX_MENU_SEARCH_UI_LIST_ITEM_LS						32
#define	NX_MENU_SEARCH_UI_LIST_ITEM_X						20
#define	NX_MENU_SEARCH_UI_LIST_ITEM_Y						(36 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SEARCH_UI_LIST_ITEM_W						530


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxSettingsItem_MSearchUi_TV_Text,
	eNxSettingsItem_MSearchUi_TV_Box,
	eNxSettingsItem_MSearchUi_TV_List,
	eNxSettingsItem_MSearchUi_Radio_Text,
	eNxSettingsItem_MSearchUi_Radio_Box,
	eNxSettingsItem_MSearchUi_Radio_List,
	eNxSettingsItem_MSearchUi_TpInfo_Text,
	eNxSettingsItem_MSearchUi_Prog_Text,
	eNxSettingsItem_MSearchUi_Prog_Bar,
	eNxSettingsItem_MSearchUi_Total_Text,
	eNxSettingsItem_MSearchUi_TotalCount_Text,
	eNxSettingsItem_MSearchUi_BTN,
	eNxSettingsItem_MSearchUi_MAX
} NxSettingsItem_SearchUi_t;


typedef enum {
	eSearchUi_state_init = 0,
	eSearchUi_state_searching,
	eSearchUi_state_searchFinish,
	eSearchUi_state_pause,
	eSearchUi_state_saveResult,
	eSearchUi_state_checkAntenna,
	eSearchUi_state_end,
	//eSearchUi_state_cancel,
	//eSearchUi_state_errorOccurred,

	eSearchUi_state_MAX
} NxSettingsItem_SearchState_t;


typedef enum {
	eNxSettingsItem_eSearchUi_DialogType_None = 0,
	eNxSettingsItem_eSearchUi_DialogType_Stop,
	eNxSettingsItem_eSearchUi_DialogType_Save,
	eNxSettingsItem_eSearchUi_DialogType_SaveData,
	eNxSettingsItem_eSearchUi_DialogType_CheckAntenna
} Nx_SettingsItem_SearchUi_DialogType_t;


typedef struct tag_Nx_MenuSearchUi_contents
{
	HBOOL		bSaveResult;
	HBOOL		bKillForced;			// Stand-by 등으로 강제 Kill : Upper Menu 등을 띄우면 안 된다.
	HBOOL		bExitMenu;

	NxSettingsItem_SearchState_t	 eSearchUiState;				// Current Search State :

	HUINT32		ulTunerCount;
	HUINT32		ulNumAnt;
	HUINT32		ulAllTpNum;
	HUINT32		ulSearchAntIdx;
	HINT32		alAntIdxArray[NX_MENU_SEARCH_UI_MAX_ANT_NUM_IN_UI];
	DxAntInfo_t		stAntInfo;
	DxTuneParam_t	stTpInfo;

	HxVector_t	*pstSearchedTPList;

	HUINT32 	ulTotalTuningNum;
	HUINT32		ulCompleteTpNum;
	HUINT32		ulSearchedServiceNum;
	HUINT32		ulTvServiceNum;
	HUINT32		ulRadioServiceNum;

	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HUINT8		szTvNumString[NX_TEXT_SIZE_32];
	HUINT8		szRadioNumString[NX_TEXT_SIZE_32];
	HUINT8		szTpInfoString[NX_TEXT_SIZE_128];
	HUINT8		szAllSvcNumString[NX_TEXT_SIZE_8];
	HUINT8		szProgressPercent[NX_TEXT_SIZE_8];
	HUINT8		szBtnStr[NX_TEXT_SIZE_32];

	HUINT32 	aulNumSvc[NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM];
	HUINT32		aulTotalSvc[NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM];
	HUINT32		aulSvcNameListSize[NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM];
	HUINT8		**pszSvcNameList[NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM];

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_MenuSearchUi_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSearchUi_Contents_t		s_stSearchUiContent;
static NXUI_DialogInstance_t			s_stDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t	*nx_menuSearchChannel_Ui_GetDialogPopupContents(void);

static ONDK_Result_t	nx_menuSearchChannel_Ui_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuSearchChannel_Ui_ProcessingMsgCreate(Nx_SettingsItem_SearchUi_DialogType_t eDialogType);
static void				nx_menuSearchChannel_Ui_ProcessingMsgDestroy(void);

static ONDK_Result_t	nx_menuSearchChannel_Ui_DialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_CreateSelectDialog(Nx_MenuSearchUi_Contents_t *pstContents, Nx_SettingsItem_SearchUi_DialogType_t eDialogType);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DestroySelectDialog(void);


static inline Nx_MenuSearchUi_Contents_t* nx_menuSearchChannel_Ui_GetContents(void);

static HINT32			nx_menuSearchChannel_Ui_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(HINT32 lItemId);

static void				nx_menuSearchChannel_Ui_UpdateCurrentTime(Nx_MenuSearchUi_Contents_t *pstContents);
static HUINT32			nx_menuSearchChannel_Ui_CountServiceName(Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType);
static HUINT8**			nx_menuSearchChannel_Ui_GetServiceNameList (Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ClearServiceNameList (Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_AddServiceNameToList(Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType, HUINT8 *szSvcName);
static NX_SCANPARAMS_t*	nx_menuSearchChannel_Ui_MakeTpList(DxAntInfo_t *pstAntInfo, HINT32 *pTpCount);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ChangedSubItemValue(Nx_MenuSearchUi_Contents_t *pstContents, HUINT32 ulCurIdx);
#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreAntennaConnectStateUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStopForAntennaConnection(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStartForAntennaConnection(Nx_MenuSearchUi_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Ui_LoadAntennaInfos(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_StartSatelliteSearch(Nx_MenuSearchUi_Contents_t *pstContents);
#endif
static ONDK_Result_t	nx_menuSearchChannel_Ui_LoadSearchData(Nx_MenuSearchUi_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionLoadServiceForSaveData(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionSaveServiceForSaveData(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ReloadServiceLists(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionResumeForSearch(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionPauseForSearch(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStopForSearch(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStartForSearch(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_InitSettingsItems(Nx_MenuSearchUi_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateTPStatueInfo(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateServiceInfo(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateButton(Nx_MenuSearchUi_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawList(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawButton(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawTPStatueInfo(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawServiceInfo(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawItem(Nx_MenuSearchUi_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawDefaultUI(Nx_MenuSearchUi_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmCreate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSearchChannel_Ui_MsgGwmDestroy(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmHapiMsg(Nx_MenuSearchUi_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmKeyDown(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmClicked(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreSearchInfoUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreChannelStateUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgAppDlgClicked(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgAppDlgCanceled(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmTimer(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuSearchChannel_Ui_GetDialogPopupContents(void)
{
	return &s_stDialogMessageInstance;
}


/******************************************************************************/
#define __Process_Dialog_Functions_____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuSearchChannel_Ui_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSearchChannel_Ui_GetDialogPopupContents();
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(pstDialog, message, hAction, p1, p2, p3);
}


static void		nx_menuSearchChannel_Ui_ProcessingMsgCreate(Nx_SettingsItem_SearchUi_DialogType_t eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSearchChannel_Ui_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if (eDialogType == eNxSettingsItem_eSearchUi_DialogType_SaveData)
	{
		ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
		pszMsg = RES_SAVING_CHANNELS_ID;
	}
	else if (eDialogType == eNxSettingsItem_eSearchUi_DialogType_CheckAntenna)
	{
		ulDlgAttr = (NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
		pszMsg = RES_MESG_4212_ID;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
		return;
	}

	// TODO: [JHLEE] Time Out 적용해야 하지 않을까?
	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszMsg));

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Ui_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuSearchChannel_Ui_ProcessingMsgProc, 0, 0, 0, 0);
}


static void		nx_menuSearchChannel_Ui_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_menuSearchChannel_Ui_ProcessingMsgProc);
}


/******************************************************************************/
#define __Select_Dialog_Functions_____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuSearchChannel_Ui_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSearchChannel_Ui_GetDialogPopupContents();
	return NX_PRISM_DIALOGUE_Message_Proc (pstDialog, message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_CreateSelectDialog(Nx_MenuSearchUi_Contents_t *pstContents, Nx_SettingsItem_SearchUi_DialogType_t eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSearchChannel_Ui_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if (eDialogType == eNxSettingsItem_eSearchUi_DialogType_Stop)
	{
		ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
		pszMsg = RES_MESG_3997_ID;
	}
	else if (eDialogType == eNxSettingsItem_eSearchUi_DialogType_Save)
	{
		ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_TYPE_MESSAGE);
		pszMsg = RES_MESG_060_ID;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
		return	ONDK_RET_FAIL;
	}

	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString(pszMsg));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Ui_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuSearchChannel_Ui_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DestroySelectDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuSearchChannel_Ui_DialogProc);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Normal_Functions____________________________________________
/******************************************************************************/

static inline Nx_MenuSearchUi_Contents_t* nx_menuSearchChannel_Ui_GetContents(void)
{
	return	&s_stSearchUiContent;
}


static HINT32	nx_menuSearchChannel_Ui_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SearchUi);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_MSearchUi_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_MSearchUi_MAX))
	{
		return 0;
	}

	return	(eNxSetting_SearchUi | lItemId);
}


static void		nx_menuSearchChannel_Ui_UpdateCurrentTime(Nx_MenuSearchUi_Contents_t *pstContents)
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

static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreAntennaConnectStateUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					lProgress = p2;
	NX_BUILDER_Status_e		eBuilderStatus = p1;
	DxAntennaConnection_e	eConnectionType = eDxANTCONN_ONECABLE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents is NULL !! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_APP_Print("===> Antenna Connect State p1:(0x%x), p2:(0x%x), p3:(0x%x)!! ===\n", p1, p2, p3);
	if ((lProgress == NX_MENU_SEARCH_UI_PROG_100_PER) || (eBuilderStatus == eBUILDER_COMPLETE))
	{
		eConnectionType = (DxAntennaConnection_e)p3;
		NX_APP_Error("=== Antenna Connect Complete!!!  eConnectionType:(%d)!! ===\n", eConnectionType);

		if (pstContents->pstSearchedTPList)
		{
			HLIB_VECTOR_Delete(pstContents->pstSearchedTPList);
			pstContents->pstSearchedTPList = NULL;
		}
		nx_menuSearchChannel_Ui_ProcessingMsgDestroy();
		NX_SYSTEM_SetAntennaConnection(eConnectionType);
		nx_menuSearchChannel_Ui_ActionStopForAntennaConnection(pstContents);

		pstContents->eSearchUiState = eSearchUi_state_end;
		pstContents->bExitMenu = TRUE;
		ONDK_GWM_APP_Destroy(0);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStopForAntennaConnection(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_StopScan(NX_SCANTYPE_ANTENNACONNECTION);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_StopScan() - NX_SCANTYPE_ANTENNACONNECTION Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStartForAntennaConnection(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HINT32				i = 0, lSearchedTplength = 0;
	HUINT32 			ulNextTPIndex = 0;
	NX_SCANPARAMS_t 	stScanParam;
	NX_SCANPARAMS_t 	*pstAllScanParams = NULL;
	NX_SCANOPTIONS_t	stScanOption;
	DxTuneParam_t		*pItem = NULL;


	HxSTD_memset(&stScanParam, 0, sizeof(NX_SCANPARAMS_t));
	HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	// Stop Search
	nx_menuSearchChannel_Ui_ActionStopForAntennaConnection(pstContents);

	lSearchedTplength = HLIB_VECTOR_Length(pstContents->pstSearchedTPList);
	if (lSearchedTplength < 1)
	{
		NX_APP_Error("ERROR!!! HLIB_VECTOR_Length() Fail - (%u)!!! \n", lSearchedTplength);
		return	ONDK_RET_FAIL;
	}

	pstAllScanParams = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * lSearchedTplength);
	if (pstAllScanParams == NULL)
	{
		NX_APP_Error("ERROR!!! NX_APP_Calloc() Fail!!! \n");
		return	ONDK_RET_FAIL;
	}

	for (i = 0; i < lSearchedTplength; i++)
	{
		pItem = (DxTuneParam_t*)HLIB_VECTOR_ItemAt(pstContents->pstSearchedTPList, i);
		if (pItem == NULL)
		{
			NX_APP_Error("ERROR!!! HLIB_VECTOR_ItemAt() Fail - index(%u) !! \n", i);
			continue;
		}

		ulNextTPIndex = pItem->eDeliType;

		HxSTD_memset(&stScanParam, 0x00, sizeof(NX_SCANPARAMS_t));

		NX_SEARCH_ConvertSatDxTunParamToChTuningParam(eDxDELIVERY_TYPE_SAT, pItem, &stScanParam);
		NX_APP_Info("Make Antenna Connection - stAntInfo.uid:(x0%x), Name:(%s)\n", stScanParam.data.stDVBSScanParams.antennaId, stScanParam.data.stDVBSScanParams.antennaName);

		HxSTD_memcpy(&pstAllScanParams[ulNextTPIndex], &stScanParam, sizeof(NX_SCANPARAMS_t));
		ulNextTPIndex ++;
	}

	pstContents->eSearchUiState = eSearchUi_state_checkAntenna;
	// Get Search Option
	NX_SEARCH_GetSearchOption(&stScanOption);

	// Add Noti
	NX_Builder_AddNotiSignalInfo();

	hError = NX_Builder_StartScan(NX_SCANTYPE_ANTENNACONNECTION, &stScanOption, lSearchedTplength, pstAllScanParams);
	if (pstAllScanParams)
	{
		NX_APP_Free(pstAllScanParams);
		pstAllScanParams = NULL;
	}

	if (pstContents->pstSearchedTPList)
	{
		HLIB_VECTOR_Delete(pstContents->pstSearchedTPList);
		pstContents->pstSearchedTPList = NULL;
	}

	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_StartScan() - NX_SCANTYPE_ANTENNACONNECTION Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_LoadAntennaInfos(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT32			i = 0, lRealAntNum = 0, lUserAntCount = 0, lAntIdx = NX_ANT_IDX_NULL;
	HUINT32			ulAllTPCount = 0, ulTpCount = 0, ulTunerCount = 0;
	DxAntInfo_t			stAntInfo;
	Nx_TpSelectInfo_t	stTpInfo;


	pstContents->ulNumAnt = 0;
	pstContents->ulAllTpNum = 0;
	pstContents->ulTunerCount = 0;

	// Init Antenna Value
	for (i = 0; i < NX_MENU_SEARCH_UI_MAX_ANT_NUM_IN_UI; i++)
	{
		// Antenna Index
		pstContents->alAntIdxArray[i] = NX_ANT_IDX_NULL;
	}

	// Load Antenna Index
	lRealAntNum = 0;
	ulAllTPCount = 0;
	lUserAntCount = NX_ANTENNA_GetUserAntCountUDB();
	NX_APP_Info("NX_ANTENNA_GetUserAntCountUDB() [%d]\n", lUserAntCount);
	for (i = 0; i < lUserAntCount; i++)
	{
		lAntIdx = NX_ANT_IDX_NULL;
		hError = NX_ANTENNA_GetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), &lAntIdx);
		if (hError == ERR_OK)
		{
			pstContents->alAntIdxArray[lRealAntNum] = lAntIdx;
			lRealAntNum++;
			NX_APP_Info("nx_menuSearchChannel_Ui_LoadAntennaInfos() - lRealAntNum[%d] = [%d]\n", lRealAntNum, lAntIdx);

			// Get All TP Count
			HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
			hError = NX_ANTENNA_GetUserTPInfo((eNxSat_ANTENAA_SLOT1 + i), &stTpInfo);
			if (hError == ERR_OK)
			{
				switch (stTpInfo.eNxSearchTpType)
				{
					case eNxSat_SEARCH_TP_USERDEFINED:
						NX_APP_Info("eNxSat_SEARCH_TP_USERDEFINED \n");
						ulTpCount = 1;
						break;
					case eNxSat_SEARCH_TP_ALL:
						NX_APP_Info("eNxSat_SEARCH_TP_ALL \n");
						hError = NX_ANTENNA_GetInfo(lAntIdx, &stAntInfo);
						if (hError != ERR_OK)
						{
							NX_APP_Error("Error!!! NX_ANTENNA_GetInfo() - lAntIdx:(0x%x)!!\n", lAntIdx);
							ulTpCount = 0;
							break;
						}
						hError = NX_ANTENNA_CountDefaultTpBySatTypeName(stAntInfo.satType, &ulTpCount);
						if (hError != ERR_OK)
						{
							ulTpCount = 0;
						}
						break;
					case eNxSat_SEARCH_TP_DEFAULT:
						NX_APP_Info("eNxSat_SEARCH_TP_DEFAULT \n");
						ulTpCount = 1;
						break;
					case eNxSat_SEARCH_TP_NONE:
						NX_APP_Info("eNxSat_SEARCH_TP_NONE \n");
						ulTpCount = 0;
						break;
					default:
						NX_APP_Error("Error!!! Unknow Type:(%d) !!\n", stTpInfo.eNxSearchTpType);
						ulTpCount = 0;
						break;
				}
			}
			else
			{
				ulTpCount = 0;
				NX_APP_Error("Error!!! NX_ANTENNA_GetUserTPInfo() Fail - Index[%d] !!\n", (eNxSat_ANTENAA_SLOT1 + i));
			}

			ulAllTPCount = ulAllTPCount + ulTpCount;
		}
		else
		{
			NX_APP_Info("nx_menuSearchChannel_Ui_LoadAntennaInfos() Not found Antenna [%d]\n", (eNxSat_ANTENAA_SLOT1 + i));
		}
	}

	NX_APP_Info("lRealAntNum : [%d]\n", lRealAntNum);
	pstContents->ulNumAnt = lRealAntNum;
	if (pstContents->ulNumAnt < 1)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("ERROR!!! Please Check Antenna Setting !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_APP_Info("ulAllTPCount : [%d]\n", ulAllTPCount);
	pstContents->ulAllTpNum = ulAllTPCount;
	if (pstContents->ulAllTpNum < 1)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("ERROR!!! Please Check TP Setting !! \n");
		return	ONDK_RET_FAIL;
	}

	hError = NX_SYSTEM_GetTunerDeviceNum(&ulTunerCount);
	if (hError != ERR_OK)
	{
		pstContents->ulTunerCount = 0;
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetTunerDeviceNum() Fail !! \n");
	}
	else
	{
		pstContents->ulTunerCount = ulTunerCount;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_StartSatelliteSearch(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HINT32				lTpCount = 0;
	HUINT32				i = 0, ulNextTPIndex = 0, ulTPIndex = 0;
	DxAntInfo_t			stAntInfo;
	Nx_TpSelectInfo_t	stTpInfo;
	NX_SCANPARAMS_t		stScanParam;
	NX_SCANPARAMS_t		*pstScanParams = NULL, *pstAllScanParams = NULL;
	NX_SCANOPTIONS_t	stScanOption;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL, stTuneInfo;


	HxSTD_memset(&stScanParam, 0, sizeof(NX_SCANPARAMS_t));
	HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	// Stop Search
	nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);

	pstAllScanParams = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * pstContents->ulAllTpNum);
	if (pstAllScanParams == NULL)
	{
		NX_APP_Error("ERROR!! NX_APP_Calloc() Fail!!! \n");
		return	ONDK_RET_FAIL;
	}

	pstContents->ulSearchAntIdx = 0;

	for (i = 0; i < pstContents->ulNumAnt; i++)
	{
		// Get Antenn Info
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		hError = NX_ANTENNA_GetInfo(pstContents->alAntIdxArray[i], &stAntInfo);
		if ((hError != ERR_OK) || (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) == 0))
		{
			NX_APP_Error("ERROR!! Invlid Satellite Name is NONE index:(%d), uid:(0x%x)!! \n", i, pstContents->alAntIdxArray[i]);
			continue;
		}

		NX_APP_Info("Antenna UID:(0x%x), stAntInfo.uid:(x0%x), Name:(%s)\n", pstContents->alAntIdxArray[i], stAntInfo.uid, stAntInfo.satType);

		// Get TP Info
		HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
		hError = NX_ANTENNA_GetUserTPInfo((eNxSat_ANTENAA_SLOT1 + i), &stTpInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!! NX_ANTENNA_GetUserTPInfo() index:(%d)!!! \n", i);
			continue;
		}

		// Make Search TP List
		switch (stTpInfo.eNxSearchTpType)
		{
			case eNxSat_SEARCH_TP_USERDEFINED:
				NX_APP_Info("eNxSat_SEARCH_TP_USERDEFINED \n");
				lTpCount = 1;
				pstTuneInfo = &(stTpInfo.stNxUserTpInfo);
				NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam(eDxDELIVERY_TYPE_SAT, &stAntInfo, pstTuneInfo, &stScanParam);
				break;
			case eNxSat_SEARCH_TP_ALL:
				NX_APP_Info("eNxSat_SEARCH_TP_ALL \n");
				pstScanParams = nx_menuSearchChannel_Ui_MakeTpList(&stAntInfo, &lTpCount);
				if (pstScanParams == NULL)
				{
					lTpCount = 0;
				}
				break;
			case eNxSat_SEARCH_TP_DEFAULT:
				NX_APP_Info("eNxSat_SEARCH_TP_DEFAULT \n");
				HxSTD_memset(&stTuneInfo, 0x00, sizeof(NX_TP_TuningInfo_t));
				hError = NX_ANTENNA_GetTpBySatelliteTypeName(stAntInfo.satType, stTpInfo.ulTpInfoIndex, &stTuneInfo);
				if (hError != ERR_OK)
				{
					lTpCount = 0;
				}
				else
				{
					lTpCount = 1;
					NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam(eDxDELIVERY_TYPE_SAT, &stAntInfo, &stTuneInfo, &stScanParam);
				}
				break;
			case eNxSat_SEARCH_TP_NONE:
				NX_APP_Info("eNxSat_SEARCH_TP_NONE \n");
				lTpCount = 0;
				break;
			default:
				NX_APP_Error("Error!!! Unknow Type:(%d)\n", stTpInfo.eNxSearchTpType);
				break;
		}

		if (lTpCount == 0)
		{
			if (pstScanParams)
			{
				NX_APP_Free(pstScanParams);
				pstScanParams = NULL;
			}

			NX_APP_Error("ERROR!! index:(%d), Antenna Name[%s] lTpCount = 0 and pstTuneInfo is NULL!!! \n", i, stAntInfo.satType);
			continue;
		}

		if (lTpCount == 1)
		{
			HxSTD_memcpy(&pstAllScanParams[ulNextTPIndex], &stScanParam, sizeof(NX_SCANPARAMS_t));
			ulNextTPIndex ++;
		}
		else
		{
			for (ulTPIndex = 0; ulTPIndex < (HUINT32)lTpCount; ulTPIndex++)
			{
				HxSTD_memcpy(&pstAllScanParams[ulNextTPIndex], &pstScanParams[ulTPIndex], sizeof(NX_SCANPARAMS_t));
				ulNextTPIndex++;
			}
		}

		if (pstScanParams)
		{
			NX_APP_Free(pstScanParams);
			pstScanParams = NULL;
		}
	}

	pstContents->pstSearchedTPList = HLIB_VECTOR_New(NULL, NULL);

	pstContents->eSearchUiState = eSearchUi_state_searching;
	// Get Search Option
	NX_SEARCH_GetSearchOption(&stScanOption);

	// Add Noti
	NX_Builder_AddNotiSignalInfo();

	hError = NX_Builder_StartScan(NX_SCANTYPE_NORMAL, &stScanOption, pstContents->ulAllTpNum, pstAllScanParams);
	if (pstAllScanParams)
	{
		NX_APP_Free(pstAllScanParams);
		pstAllScanParams = NULL;
	}

	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_StartScan() - NX_SCANTYPE_NORMAL Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}

#endif	// End of defined(CONFIG_MW_CH_SATELLITE)


static ONDK_Result_t	nx_menuSearchChannel_Ui_ChangedSubItemValue(Nx_MenuSearchUi_Contents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(ulCurIdx);

	return	ONDK_RET_OK;
}


static HUINT32	nx_menuSearchChannel_Ui_CountServiceName(Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType)
{
	HUINT8		ucType = 0;
	HUINT32		ulCount = 0, ulSvcNum = 0;

	if (eSvcType == eDxSVC_TYPE_All)
	{
		ulSvcNum = 0;

		for (ulCount = 0; ulCount < NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM; ulCount++)
		{
			ulSvcNum += pstContents->aulNumSvc[ulCount];
		}

		return	ulSvcNum;
	}
	else if ((eSvcType == eDxSVC_TYPE_TV) || (eSvcType == eDxSVC_TYPE_RADIO))
	{
		ucType = (eSvcType == eDxSVC_TYPE_TV) ? NX_MENU_SEARCH_UI_MAX_SVCTYPE_TV : NX_MENU_SEARCH_UI_MAX_SVCTYPE_RADIO ;
		return	pstContents->aulNumSvc[ucType];
	}

	return	0;
}


static HUINT8**		nx_menuSearchChannel_Ui_GetServiceNameList(Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType)
{
	HUINT8	ucType = 0;

	if ((eSvcType == eDxSVC_TYPE_TV) || (eSvcType == eDxSVC_TYPE_RADIO))
	{
		ucType = (eSvcType == eDxSVC_TYPE_TV) ? NX_MENU_SEARCH_UI_MAX_SVCTYPE_TV : NX_MENU_SEARCH_UI_MAX_SVCTYPE_RADIO ;
		return	pstContents->pszSvcNameList[ucType];
	}

	return NULL;
}

static ONDK_Result_t	nx_menuSearchChannel_Ui_ClearServiceNameList (Nx_MenuSearchUi_Contents_t *pstContents)
{
	HUINT32			ulSvcIdx = 0, ulSvcListSize = 0;
	HUINT32			ulSvcTypeIdx = 0;
	HUINT8			**pszSvcNameList = NULL;

	for (ulSvcTypeIdx = 0; ulSvcTypeIdx < NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM; ulSvcTypeIdx++)
	{
		ulSvcListSize =  pstContents->aulSvcNameListSize[ulSvcTypeIdx];
		pszSvcNameList = pstContents->pszSvcNameList[ulSvcTypeIdx];

		if (pszSvcNameList != NULL)
		{
			for (ulSvcIdx = 0; ulSvcIdx < ulSvcListSize; ulSvcIdx++)
			{
				if (pszSvcNameList[ulSvcIdx] != NULL)
				{
					NX_APP_Free (pszSvcNameList[ulSvcIdx]);
				}
			}

			NX_APP_Free (pszSvcNameList);
		}
	}

	HxSTD_memset(pstContents->aulNumSvc, 0, sizeof(HUINT32) * NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM);
	HxSTD_memset(pstContents->aulSvcNameListSize, 0, sizeof(HUINT32) * NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM);
	HxSTD_memset(pstContents->pszSvcNameList, 0, sizeof(HUINT8 **) * NX_MENU_SEARCH_UI_MAX_SVCTYPE_NUM);

	return ERR_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_AddServiceNameToList(Nx_MenuSearchUi_Contents_t *pstContents, DxSvcType_e eSvcType, HUINT8 *szSvcName)
{
	HUINT32		ulStrLen = 0, ulNewListSize = 0;
	HUINT8		*szDstSvcName = NULL, **pszSvcNameList = NULL, ucType = 0;

	if (eSvcType <= eDxSVC_TYPE_All || eSvcType >= eDxSVC_TYPE_RFVOD)
	{
		NX_APP_Error("ERROR!! GWM_RET_OUT_OF_RANGE eSvcType [%d] !!! \n", eSvcType);
		return	GWM_RET_OUT_OF_RANGE;
	}

	if (szSvcName == NULL)
	{
		NX_APP_Error("ERROR!! szSvcName is NULL !!! \n");
		return GWM_RET_TARGET_NULL;
	}

	ucType = (eSvcType == eDxSVC_TYPE_TV) ?	NX_MENU_SEARCH_UI_MAX_SVCTYPE_TV : NX_MENU_SEARCH_UI_MAX_SVCTYPE_RADIO ;
	if (pstContents->aulNumSvc[ucType] >= pstContents->aulSvcNameListSize[ucType])
	{
		ulNewListSize = pstContents->aulSvcNameListSize[ucType] + 1000;
		pszSvcNameList = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulNewListSize);
		if (pszSvcNameList == NULL)
		{
			NX_APP_Error("ERROR!! NX_APP_Malloc() Fail !!! \n");
			return	GWM_RET_OUT_OF_MEMORY;
		}

		HxSTD_memset(pszSvcNameList, 0, sizeof(HUINT8 *) * ulNewListSize);
		if (pstContents->pszSvcNameList[ucType] != NULL)
		{
			HxSTD_memcpy(pszSvcNameList, pstContents->pszSvcNameList[ucType], sizeof(HUINT8 *) * pstContents->aulNumSvc[ucType]);
			NX_APP_Free (pstContents->pszSvcNameList[ucType]);
		}

		pstContents->pszSvcNameList[ucType] = pszSvcNameList;
		pstContents->aulSvcNameListSize[ucType] = ulNewListSize;
	}

	ulStrLen = HxSTD_StrLen((HCHAR*)szSvcName) + 1;
	szDstSvcName = NX_APP_Calloc(ulStrLen);
	if (szDstSvcName == NULL)
	{
		NX_APP_Error("ERROR!! NX_APP_Malloc() Fail !!! \n");
		return	GWM_RET_OUT_OF_MEMORY;
	}

	HxSTD_memset(szDstSvcName, 0x0, ulStrLen);
	szDstSvcName[0] = '\0';

	HxSTD_StrNCpy((HCHAR*)szDstSvcName, (HCHAR*)szSvcName, ulStrLen);
	szDstSvcName[ulStrLen-1] = '\0';

	pstContents->pszSvcNameList[ucType][pstContents->aulNumSvc[ucType]] = szDstSvcName;
	pstContents->aulNumSvc[ucType]++;
	pstContents->aulTotalSvc[ucType]++;

	NX_APP_Info("\nAdd Service : eSvcType[%d], szDstSvcName[%s]\n", eSvcType, szDstSvcName);

	return	ONDK_RET_OK;
}


NX_SCANPARAMS_t*	nx_menuSearchChannel_Ui_MakeTpList(DxAntInfo_t *pstAntInfo, HINT32 *pTpCount)
{
	HUINT32		i = 0, ulTpCount = 0;
	HERROR		hError = ERR_FAIL;
	NX_SCANPARAMS_t 	*pstParam  = NULL;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;

	*pTpCount = 0;
	hError = NX_ANTENNA_GetTpListBySatelliteTypeName(pstAntInfo->satType, &ulTpCount, &pstTuneInfo);
	if (hError != ERR_OK)
	{
		NX_APP_Info("ERROR!!! Antenna: [%s] Get TP List fail!!\n", pstAntInfo->satType);
		return	NULL;
	}

	pstParam = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * ulTpCount);
	for (i = 0; i < ulTpCount; i++)
	{
		pstParam[i].tunerType = eDxDELIVERY_TYPE_SAT;
		pstParam[i].data.stDVBSScanParams.antennaId = pstAntInfo->uid;
		HxSTD_snprintf(pstParam[i].data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", pstAntInfo->satType);

		pstParam[i].data.stDVBSScanParams.startFreq = pstTuneInfo[i].sat.ulFrequency;
		pstParam[i].data.stDVBSScanParams.endFreq = pstTuneInfo[i].sat.ulFrequency;
		pstParam[i].data.stDVBSScanParams.modulationModes = pstTuneInfo[i].sat.ePskMod;
		if (pstTuneInfo[i].sat.eTransSpec == eDxSAT_TRANS_DVBS)
		{
			// TODO: Check !!! 왜 이렇게 해야 하나요? 음.....
			pstParam[i].data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
		}
		pstParam[i].data.stDVBSScanParams.polarisation = pstTuneInfo[i].sat.ePolarization;
		pstParam[i].data.stDVBSScanParams.networkId = 0;

		HxSTD_snprintf(pstParam[i].data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstTuneInfo[i].sat.ulSymbolRate));
		switch (pstTuneInfo[i].sat.eFecCodeRate)
		{
			case eDxSAT_CODERATE_1_2:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));	break;
			case eDxSAT_CODERATE_2_3:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));	break;
			case eDxSAT_CODERATE_3_4:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));	break;
			case eDxSAT_CODERATE_5_6:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));	break;
			case eDxSAT_CODERATE_7_8:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));	break;
			case eDxSAT_CODERATE_3_5:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));	break;
			case eDxSAT_CODERATE_4_5:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));	break;
			case eDxSAT_CODERATE_5_11:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));	break;
			case eDxSAT_CODERATE_6_7:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));	break;
			case eDxSAT_CODERATE_8_9:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));	break;
			case eDxSAT_CODERATE_9_10:	HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));	break;
			default:					HxSTD_StrNCpy(pstParam[i].data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));	break;
		}
	}

	*pTpCount = ulTpCount;

	if (pstTuneInfo != NULL)
	{
		NX_ANTENNA_FreeTpListArray(pstTuneInfo);
		pstTuneInfo = NULL;
	}

	return pstParam;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_LoadSearchData(Nx_MenuSearchUi_Contents_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

#if defined(CONFIG_MW_CH_SATELLITE)
	eResult = nx_menuSearchChannel_Ui_LoadAntennaInfos(pstContents);
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	eResult = ONDK_RET_FAIL;
	// TODO: [JHLEE] Add function Load Terrestrial info
#elif defined(CONFIG_MW_CH_CABLE)
	eResult = ONDK_RET_FAIL;
	// TODO: [JHLEE] Add function Load Cable info
#else
	eResult = ONDK_RET_FAIL;
#endif

	return	eResult;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionSaveServiceForSaveData(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_ControlScan(NX_SCANTYPE_SAVE);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_SAVE Fail !! \n");
	}

	NX_CHANNEL_LoadList(TRUE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionLoadServiceForSaveData(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_ControlScan(NX_SCANTYPE_LOAD);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_PAUSE Fail !! \n");
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ReloadServiceLists(Nx_MenuSearchUi_Contents_t *pstContents)
{
	ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_RELOAD_CHANNELLIST_ALL, 0, 0, 0, 0);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionResumeForSearch(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_ControlScan(NX_SCANTYPE_RESUME);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_RESUME Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionPauseForSearch(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_ControlScan(NX_SCANTYPE_PAUSE);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_ControlScan() - NX_SCANTYPE_PAUSE Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStopForSearch(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_Builder_StopScan(NX_SCANTYPE_NORMAL);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_Builder_StopScan() - NX_SCANTYPE_NORMAL Fail !! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_ActionStartForSearch(Nx_MenuSearchUi_Contents_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

#if defined(CONFIG_MW_CH_SATELLITE)
	eResult = nx_menuSearchChannel_Ui_StartSatelliteSearch(pstContents);
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	eResult = ONDK_RET_FAIL;
	// TODO: [JHLEE] Add function Load Terrestrial info
#elif defined(CONFIG_MW_CH_CABLE)
	eResult = ONDK_RET_FAIL;
	// TODO: [JHLEE] Add function Load Cable info
#else
	eResult = ONDK_RET_FAIL;
#endif

	return	eResult;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_InitSettingsItems(Nx_MenuSearchUi_Contents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	HxSTD_memset (pstContents, 0x00, sizeof(Nx_MenuSearchUi_Contents_t));
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SEARCH_CHANNELS_01_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->eSearchUiState = eSearchUi_state_init;
	pstContents->szTpInfoString[0] = '\0';
	pstContents->bExitMenu = FALSE;
	pstContents->ulTvServiceNum = 0;
	pstContents->ulRadioServiceNum = 0;
	pstContents->pszSvcNameList[NX_MENU_SEARCH_UI_MAX_SVCTYPE_TV] = NULL;
	pstContents->pszSvcNameList[NX_MENU_SEARCH_UI_MAX_SVCTYPE_RADIO] = NULL;

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateButton(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	NxSettingsItem_SearchState_t	 eSearchUiState = eSearchUi_state_init;

	// Save Or Exit Buttons
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_BTN);

	eSearchUiState = pstContents->eSearchUiState;
	switch (eSearchUiState)
	{
		case eSearchUi_state_init:
		case eSearchUi_state_searching:
		case eSearchUi_state_pause:
			// Stop
			HxSTD_snprintf((HCHAR*)pstContents->szBtnStr, NX_TEXT_SIZE_32, (HCHAR*)ONDK_GetString(RES_STOP_01_ID));
			ONDK_GWM_COM_Button_ChangeTitle (lObjId, (HCHAR*)pstContents->szBtnStr);
			break;

		case eSearchUi_state_searchFinish:
			// Save
			HxSTD_snprintf((HCHAR*)pstContents->szBtnStr, NX_TEXT_SIZE_32, (HCHAR*)ONDK_GetString(RES_SAVE_01_ID));
			ONDK_GWM_COM_Button_ChangeTitle (lObjId, (HCHAR*)pstContents->szBtnStr);
			break;

		default:
			NX_APP_Info(" eSearchUiState:(%d)\n", eSearchUiState);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateTPStatueInfo(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	HUINT8			*szStr = NULL;
	HUINT32			ulProgress = 0;
	ONDK_GWM_GuiObject_t	UiObj = NULL;

	// TP Info
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TpInfo_Text);
	UiObj = ONDK_GWM_APP_GetObject(lObjId);
	if (UiObj == NULL)
	{
		NX_APP_Error(" ONDK_GWM_APP_GetObject() - lObjId :(0x%x)\n", lObjId);
		return	ONDK_RET_FAIL;
	}

	szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
	if (szStr != NULL)
	{
		HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_128, (HCHAR*)"%s", (HCHAR*)pstContents->szTpInfoString);
		ONDK_GWM_COM_Text_SetText (lObjId, (HCHAR*)szStr);
	}

	// status bar Progress Text
	if (pstContents->ulTotalTuningNum > 0)
	{
		ulProgress = ((pstContents->ulCompleteTpNum * NX_MENU_SEARCH_UI_PROG_100_PER) / pstContents->ulTotalTuningNum);
	}
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Prog_Text);
	UiObj = ONDK_GWM_APP_GetObject(lObjId);
	if(UiObj)
	{
		szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
	}
	if (szStr != NULL)
	{
		HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_8, "%d%%", (int)ulProgress);
		ONDK_GWM_COM_Text_SetText (lObjId, (HCHAR*)szStr);
	}

	// status bar
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Prog_Bar);
	ONDK_GWM_COM_Status_SetValue(lObjId, ulProgress);

	// Total Count
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TotalCount_Text);
	UiObj = ONDK_GWM_APP_GetObject(lObjId);
	if (UiObj)
	{
		szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)UiObj);
		if (szStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_8, "%d", (int)pstContents->ulSearchedServiceNum);
			ONDK_GWM_COM_Text_SetText (lObjId, (HCHAR*)szStr);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_UpdateServiceInfo(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1, lListIndex = 0;
	HUINT8			*szStr = NULL;
	HUINT32			ulTvSvcNum = 0, ulRadioSvcNum = 0;
	HUINT8			**pszSvcNameList = NULL;
	ONDK_GWM_GuiObject_t stUiObj = NULL;


	// TV - TEXT
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TV_Text);
	stUiObj = ONDK_GWM_APP_GetObject(lObjId);
	if (stUiObj)
	{
		szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stUiObj);
		if (szStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_32, (HCHAR*)"%s : %d", ONDK_GetString(RES_TV_ID), (int)pstContents->ulTvServiceNum);
			ONDK_GWM_COM_Text_SetText (lObjId, (HCHAR*)szStr);
		}
	}

	// RADIO - TEXT
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Radio_Text);
	stUiObj = ONDK_GWM_APP_GetObject(lObjId);
	if (stUiObj)
	{
		szStr = ONDK_GWM_COM_Text_GetPtrByObject((TEXT_OBJECT)stUiObj);
		if (szStr != NULL)
		{
			HxSTD_snprintf((HCHAR*)szStr, NX_TEXT_SIZE_32, (HCHAR*)"%s : %d", ONDK_GetString(RES_RADIO_ID), (int)pstContents->ulRadioServiceNum);
			ONDK_GWM_COM_Text_SetText (lObjId, (HCHAR*)szStr);
		}
	}

	// TV List :
	ulTvSvcNum = pstContents->ulTvServiceNum;
	pszSvcNameList	= nx_menuSearchChannel_Ui_GetServiceNameList(pstContents, eDxSVC_TYPE_TV);
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TV_List);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(lObjId), ulTvSvcNum, (GWM_LIST_ITEMS)pszSvcNameList);
	if (ulTvSvcNum >= NX_MENU_SEARCH_UI_LIST_ITEM_MAX)
	{
		lListIndex = (HINT32)(ulTvSvcNum - NX_MENU_SEARCH_UI_LIST_ITEM_MAX);
	}
	else
	{
		lListIndex = 0;
	}
	ONDK_GWM_List_SetCurrentIndex(lObjId, lListIndex);
	ONDK_GWM_APP_InvalidateObject(lObjId);

	// Radio List :
	ulRadioSvcNum = pstContents->ulRadioServiceNum;
	pszSvcNameList	= nx_menuSearchChannel_Ui_GetServiceNameList(pstContents, eDxSVC_TYPE_RADIO);
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Radio_List);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(lObjId), ulRadioSvcNum, (GWM_LIST_ITEMS)pszSvcNameList);
	if (ulRadioSvcNum >= NX_MENU_SEARCH_UI_LIST_ITEM_MAX)
	{
		lListIndex = (HINT32)(ulRadioSvcNum - NX_MENU_SEARCH_UI_LIST_ITEM_MAX);
	}
	else
	{
		lListIndex = 0;
	}
	ONDK_GWM_List_SetCurrentIndex(lObjId, (HINT32)lListIndex);
	ONDK_GWM_APP_InvalidateObject(lObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawList(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			lNumItems = 0, lStartIdx = 0, lCurrIdx = 0, lSelectedIdx = 0, lNumOfPage = NX_MENU_SEARCH_UI_LIST_ITEM_MAX;
	HINT32			lStartX = 0, lEndX = 0, lStartY = 0, lEndY = 0, lItemX = 0, lItemY = 0;
	HINT32			lMaxWidth = 0, lCellHeight = 0, lFirstIdx = 0, lLastIdx = 0, lCount = 0;
	HUINT8			*szTitle = NULL, **pszListStr = NULL;
	ONDKFontHandle	*hFontHandle = NULL;
	ONDK_Color_t	ulFontColor;
	LIST_OBJECT		pstListObj;


	NOT_USED_PARAM(arg);

	lStartX = area->x1 + rect->x;
	lStartY = area->y1 + rect->y;
	lEndX = lStartX + rect->w;
	lEndY = lStartY + rect->h;

	// Get the properties of the list
	pstListObj = (LIST_OBJECT)object;

	ONDK_GWM_List_GetProperty (pstListObj, &lNumItems, &lStartIdx, &lCurrIdx, &lSelectedIdx, &lNumOfPage);
	pszListStr = (HUINT8 **)ONDK_GWM_List_GetArrayPtr(pstListObj);
	if (pszListStr == NULL)
	{
		return	ONDK_RET_INITFAIL;
	}

	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	if (lNumItems > lNumOfPage)
	{
		// 아이템이 10개 이상이므로 스크롤바 또는 화살표 등의 처리가 필요하다.
		if ((lNumItems - lCurrIdx) <= lNumOfPage)
		{
			// current가 last page에 있다. current가 페이지 첫 아이템이 아니다.
			lFirstIdx = (lNumItems - lNumOfPage);
			lLastIdx = (lNumItems - 1);
		}
		else
		{
			// current가 페이지가 첫 아이템이다.
			lFirstIdx = lCurrIdx;
			lLastIdx = (lCurrIdx + lNumOfPage - 1);
		}
	}
	else
	{
		lFirstIdx = 0;
		lLastIdx = (lNumItems - 1);
	}

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	if (hFontHandle == NULL)
	{
		return	ONDK_RET_INITFAIL;
	}

	lItemX = lStartX + NX_MENU_SEARCH_UI_LIST_ITEM_X;
	lItemY = lStartY + NX_MENU_SEARCH_UI_LIST_ITEM_Y;
	lMaxWidth = NX_MENU_SEARCH_UI_LIST_ITEM_W;
	ulFontColor = COL(C_T_Gong03);

	for (lCount = 0; (lCount < NX_MENU_SEARCH_UI_LIST_ITEM_MAX) && ((lFirstIdx + lCount) <= lLastIdx); lCount++)
	{
		szTitle = pszListStr[lFirstIdx + lCount];
		if (szTitle != NULL)
		{
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szTitle, lItemX, lItemY, lMaxWidth, ulFontColor, ONDKSTF_LEFT);
		}

		lItemY += NX_MENU_SEARCH_UI_LIST_ITEM_LS;
	}

	if (hFontHandle)
	{
		ONDK_FONT_Release(hFontHandle);
		hFontHandle = NULL;
	}

	NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawButton(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Search Buttons
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_BTN);
	HxSTD_snprintf((HCHAR*)pstContents->szBtnStr, NX_TEXT_SIZE_32, (HCHAR*)ONDK_GetString(RES_STOP_01_ID));
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_STOP_BUTTON_X, NX_MENU_SEARCH_UI_STOP_BUTTON_Y,
					NX_MENU_SEARCH_UI_STOP_BUTTON_W, NX_MENU_SEARCH_UI_STOP_BUTTON_H);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)pstContents->szBtnStr);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_B1);

	ONDK_GWM_Obj_SetFocus(lObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawTPStatueInfo(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	HUINT32			ulProgress = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// TP Info
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TpInfo_Text);
	HxSTD_snprintf((HCHAR*)pstContents->szTpInfoString, NX_TEXT_SIZE_128, (HCHAR*)" ");
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TPINFO_TEXT_X, NX_MENU_SEARCH_UI_TPINFO_TEXT_Y,
						NX_MENU_SEARCH_UI_TPINFO_TEXT_W, NX_MENU_SEARCH_UI_TPINFO_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szTpInfoString);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Pitch01), COL(C_T_Bang_100));

	// status bar Progress Text
	ulProgress = 0;
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Prog_Text);
	HxSTD_snprintf((HCHAR *)pstContents->szProgressPercent, NX_TEXT_SIZE_8, "%d%%", (int)ulProgress);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_PROG_TEXT_X, NX_MENU_SEARCH_UI_PROG_TEXT_Y,
						NX_MENU_SEARCH_UI_PROG_TEXT_W, NX_MENU_SEARCH_UI_PROG_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szProgressPercent);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Pitch01), COL(C_T_Bang_100));

	// status bar
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Prog_Bar);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_PROG_BAR_X, NX_MENU_SEARCH_UI_PROG_BAR_Y,
						NX_MENU_SEARCH_UI_PROG_BAR_W, NX_MENU_SEARCH_UI_PROG_BAR_H);
	ONDK_GWM_COM_Status_Create(lObjId, &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(lObjId, COL(C_G_Whistle_20), COL(C_G_Pitch01));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);

	// Total Text
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Total_Text);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TOTAL_TEXT_X, NX_MENU_SEARCH_UI_TOTAL_TEXT_Y,
						NX_MENU_SEARCH_UI_TOTAL_TEXT_W, NX_MENU_SEARCH_UI_TOTAL_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)ONDK_GetString(RES_SEARCHED_CHANNELS_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_32);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// Total Count
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TotalCount_Text);
	HxSTD_snprintf((HCHAR *)pstContents->szAllSvcNumString, NX_TEXT_SIZE_8, "%d", (int)pstContents->ulSearchedServiceNum);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_X, NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_Y,
						NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_W, NX_MENU_SEARCH_UI_TOTAL_COUNT_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szAllSvcNumString);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_32);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawServiceInfo(Nx_MenuSearchUi_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	GWM_LIST_ITEMS		 ulSvcNameList = 0;

	// TV - TEXT
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TV_Text);
	HxSTD_snprintf((HCHAR*)pstContents->szTvNumString, NX_TEXT_SIZE_32, (HCHAR*)"%s : %d", ONDK_GetString(RES_TV_ID), (int)pstContents->ulTvServiceNum);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TV_TEXT_X, NX_MENU_SEARCH_UI_TV_TEXT_Y, NX_MENU_SEARCH_UI_TV_TEXT_W, NX_MENU_SEARCH_UI_TV_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szTvNumString);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// TV Box
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TV_Box);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TV_INFO_BOX_X, NX_MENU_SEARCH_UI_TV_INFO_BOX_Y, NX_MENU_SEARCH_UI_TV_INFO_BOX_W, NX_MENU_SEARCH_UI_TV_INFO_BOX_H);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, COL(C_G_Gong02));

	// RADIO - TEXT
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Radio_Text);
	HxSTD_snprintf((HCHAR*)pstContents->szRadioNumString, NX_TEXT_SIZE_32, (HCHAR*)"%s : %d", ONDK_GetString(RES_RADIO_ID), (int)pstContents->ulRadioServiceNum);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_RADIO_TEXT_X, NX_MENU_SEARCH_UI_RADIO_TEXT_Y, NX_MENU_SEARCH_UI_RADIO_TEXT_W, NX_MENU_SEARCH_UI_RADIO_TEXT_H);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szRadioNumString);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// RADIO Box
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Radio_Box);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_RADIO_INFO_BOX_X, NX_MENU_SEARCH_UI_RADIO_INFO_BOX_Y,
						NX_MENU_SEARCH_UI_RADIO_INFO_BOX_W, NX_MENU_SEARCH_UI_RADIO_INFO_BOX_H);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, COL(C_G_Gong02));

	// search tv result list
	ulSvcNameList = (GWM_LIST_ITEMS)nx_menuSearchChannel_Ui_GetServiceNameList(pstContents, eDxSVC_TYPE_TV);
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_TV_List);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_TV_INFO_BOX_X, NX_MENU_SEARCH_UI_TV_INFO_BOX_Y, NX_MENU_SEARCH_UI_TV_INFO_BOX_W, NX_MENU_SEARCH_UI_TV_INFO_BOX_H);
	ONDK_GWM_List_Create(lObjId, &stRect, (HCHAR*)"Search UI TVList", NX_MENU_SEARCH_UI_LIST_ITEM_LS, 0, NX_MENU_SEARCH_UI_LIST_ITEM_MAX, ulSvcNameList);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)nx_menuSearchChannel_Ui_DrawList);
	ONDK_GWM_Obj_DisableObject(lObjId);

	// search radio result list
	ulSvcNameList = 0;
	ulSvcNameList = (GWM_LIST_ITEMS)nx_menuSearchChannel_Ui_GetServiceNameList(pstContents, eDxSVC_TYPE_RADIO);
	lObjId = nx_menuSearchChannel_Ui_ConvertItemIdxToObjId(eNxSettingsItem_MSearchUi_Radio_List);
	stRect = ONDK_Rect(NX_MENU_SEARCH_UI_RADIO_INFO_BOX_X, NX_MENU_SEARCH_UI_RADIO_INFO_BOX_Y, NX_MENU_SEARCH_UI_RADIO_INFO_BOX_W, NX_MENU_SEARCH_UI_RADIO_INFO_BOX_H);
	ONDK_GWM_List_Create(lObjId, &stRect, (HCHAR*)"Search UI RadioList", NX_MENU_SEARCH_UI_LIST_ITEM_LS, 0, NX_MENU_SEARCH_UI_LIST_ITEM_MAX, ulSvcNameList);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, (GWM_ObjDraw_t)nx_menuSearchChannel_Ui_DrawList);
	ONDK_GWM_Obj_DisableObject(lObjId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawItem(Nx_MenuSearchUi_Contents_t *pstContents)
{
	nx_menuSearchChannel_Ui_DrawServiceInfo(pstContents);
	nx_menuSearchChannel_Ui_DrawTPStatueInfo(pstContents);
	nx_menuSearchChannel_Ui_DrawButton(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_DrawDefaultUI(Nx_MenuSearchUi_Contents_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSearchChannel_Ui_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	nx_menuSearchChannel_Ui_DrawItem(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmCreate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	eResult = nx_menuSearchChannel_Ui_InitSettingsItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuSearchChannel_Ui_InitSettingsItems() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	eResult;
	}

	// Load Search Data
	eResult = nx_menuSearchChannel_Ui_LoadSearchData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuSearchChannel_Ui_LoadSearchData() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	eResult;
	}

	// Draw Menu Item
	nx_menuSearchChannel_Ui_DrawDefaultUI(pstContents);

	// Start Search
	eResult = nx_menuSearchChannel_Ui_ActionStartForSearch(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuSearchChannel_Ui_ActionStartForSearch() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	eResult;
	}

	//ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
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


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmDestroy(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL	bExitMenu = FALSE;


	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
	}
	else
	{
		bExitMenu = pstContents->bExitMenu;
		nx_menuSearchChannel_Ui_DestroySelectDialog();
		nx_menuSearchChannel_Ui_ProcessingMsgDestroy();
		nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);
		NX_Builder_ReleaseNotiSignalInfo();

		if (pstContents->pstSearchedTPList)
		{
			HLIB_VECTOR_Delete(pstContents->pstSearchedTPList);
			pstContents->pstSearchedTPList = NULL;
		}

		nx_menuSearchChannel_Ui_ClearServiceNameList(pstContents);
		HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSearchUi_Contents_t));
	}

	NX_PRISM_LIVE_ServiceStartByLastService();

	if (bExitMenu == FALSE)
	{
		ONDK_GWM_Callback_t appToLaunch = NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu();

		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NULL", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								appToLaunch, HANDLE_NULL, 0, 0, 0);
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmHapiMsg(Nx_MenuSearchUi_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmKeyDown(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
		return	GWM_RET_SOURCE_NULL;
	}


	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_GUIDE:
		case KEY_MENU:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		// TODO: [JHLEE] Action key process

		default:
			NX_APP_Print("nx_menuSearchChannel_Ui_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmClicked(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;
	ONDK_Result_t	eResult = ONDK_RET_OK;

	lItemIdx = nx_menuSearchChannel_Ui_ConvertObjIdToItemIdx(lObjId);
	if (lItemIdx == eNxSettingsItem_MSearchUi_BTN)
	{
		if (pstContents->eSearchUiState == eSearchUi_state_searchFinish)
		{
			pstContents->eSearchUiState = eSearchUi_state_saveResult;
			nx_menuSearchChannel_Ui_ProcessingMsgCreate(eNxSettingsItem_eSearchUi_DialogType_SaveData);
			nx_menuSearchChannel_Ui_ActionLoadServiceForSaveData(pstContents);
		}
		else if (pstContents->eSearchUiState == eSearchUi_state_searching)
		{
			eResult = nx_menuSearchChannel_Ui_ActionPauseForSearch(pstContents);
			if (eResult == ONDK_RET_OK)
			{
				pstContents->eSearchUiState = eSearchUi_state_pause;
				nx_menuSearchChannel_Ui_CreateSelectDialog(pstContents, eNxSettingsItem_eSearchUi_DialogType_Stop);
			}
		}
		else
		{
			NX_APP_Info("None Action eSearchUiState:(%d)!!! \n", pstContents->eSearchUiState);
		}

		return	ONDK_MESSAGE_BREAK;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Button Index :(0x%x)!!! \n", lItemIdx);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreSearchInfoUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lStartIndex = 0, lResultNo = 0, i = 0;
	HUINT32 		ulTotalTuningNum = 0, ulCompleteTpNum = 0, ulSearchedServiceNum = 0, ulSearchState = 0, ulFrequency = 0, ulProgress = 0;
	HUINT32			ulTvSvcNum = 0, ulRadioSvcNum = 0;
	NX_BUILDER_Status_e	eBuilderStatus = p1;
	DxDeliveryType_e	eTunerType = eDxDELIVERY_TYPE_ALL;
	DxTuneParam_t		stTpInfo, *pstSearchedTp = NULL;
	DxBuilder_SearchedInfo_t	*pstDetailInfo = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	hError = NX_SEARCH_GetSearchTPInfo(&ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!! NX_SEARCH_GetSearchTPInfo() !!! \n");
	}

	NX_APP_Debug("\n =====> totalTpNum[%d], completeTpNum[%d], searchedSvcNum[%d], ulFrequency[%d], eTunerType[%d] \n", ulTotalTuningNum, ulCompleteTpNum, ulSearchedServiceNum, ulFrequency, eTunerType);
	if (ulFrequency < 1)
	{
		NX_APP_Error("ERROR!! Invalid Searching Info!!! \n");
		return	ONDK_RET_INVARG;
	}

	lStartIndex = nx_menuSearchChannel_Ui_CountServiceName(pstContents, eDxSVC_TYPE_All);
	if (lStartIndex > 0)
	{
		lStartIndex = lStartIndex - 1;
	}

	hError = NX_SEARCH_GetSearchServiceListInfo(lStartIndex, ulSearchedServiceNum, &lResultNo, &pstDetailInfo);
	NX_APP_Debug("=====>[JHLEE] lStartIndex:(%d), ulSearchedServiceNum:(%d), lResultNo:(%d), ulTvServiceNum:(%d), ulRadioServiceNum:(%d)\n",
			lStartIndex, ulSearchedServiceNum, lResultNo, pstContents->ulTvServiceNum, pstContents->ulRadioServiceNum);
	if ((hError == ERR_OK) && (lResultNo > 0) && pstDetailInfo)
	{
		NX_APP_Debug("\nNX_SEARCH_GetSearchServiceListInfo() - lStartIndex[%d], lResultNo[%d] \n", lStartIndex, lResultNo);
		ulTvSvcNum = 0;
		ulRadioSvcNum = 0;
		for (i = 0; i < lResultNo; i++)
		{
			if (pstDetailInfo[i].ulSvcType == eDxSVC_TYPE_TV)
			{
				NX_APP_Info("=====>[JHLEE] TV SVC[%d] : (%s) \n", ulTvSvcNum, pstDetailInfo[i].szSvcName);
				ulTvSvcNum++;
				nx_menuSearchChannel_Ui_AddServiceNameToList(pstContents, pstDetailInfo[i].ulSvcType, pstDetailInfo[i].szSvcName);
			}
			else if (pstDetailInfo[i].ulSvcType == eDxSVC_TYPE_RADIO)
			{
				NX_APP_Info("=====>[JHLEE] Radio SVC[%d] : (%s) \n", ulRadioSvcNum, pstDetailInfo[i].szSvcName);
				ulRadioSvcNum++;
				nx_menuSearchChannel_Ui_AddServiceNameToList(pstContents, pstDetailInfo[i].ulSvcType, pstDetailInfo[i].szSvcName);
			}
			else
			{
				NX_APP_Error("ERROR!! Invalid Service Type : [0x%x]!!! \n", pstDetailInfo[i].ulSvcType);
			}
		}

		if (pstDetailInfo)
		{
			NX_SEARCH_FreeSearchServiceListInfo(pstDetailInfo);
			pstDetailInfo = NULL;
		}

		// Add Searched TP List
		if (pstContents->pstSearchedTPList)
		{
			pstSearchedTp = NX_APP_Calloc(sizeof(DxTuneParam_t));
			if (pstSearchedTp)
			{
				HxSTD_memset(pstSearchedTp, 0x00, sizeof(DxTuneParam_t));
				HxSTD_memcpy(pstSearchedTp, &stTpInfo, sizeof(DxTuneParam_t));
				HLIB_VECTOR_Add(pstContents->pstSearchedTPList, (void*)pstSearchedTp);
				NX_APP_Info("HLIB_VECTOR_Add()\n");
			}
			else
			{
				NX_APP_Error("ERROR!!! NX_APP_Calloc() Faile!!! \n");
			}
		}
	}
	else
	{
		NX_APP_Debug("\nERROR !! NX_SEARCH_GetSearchServiceListInfo()  Not found Service lStartIndex[%d], lResultNo[%d]!!! \n", lStartIndex, lResultNo);
	}

	pstContents->ulTotalTuningNum = ulTotalTuningNum;
	pstContents->ulCompleteTpNum = ulCompleteTpNum;

	pstContents->ulTvServiceNum += ulTvSvcNum;
	pstContents->ulRadioServiceNum += ulRadioSvcNum;

	pstContents->ulSearchedServiceNum = (pstContents->ulTvServiceNum + pstContents->ulRadioServiceNum);
	NX_APP_Debug("=== ulSearchedServiceNum:(%d), ulTvServiceNum:(%d), ulRadioServiceNum:(%d)\n", pstContents->ulSearchedServiceNum, pstContents->ulTvServiceNum, pstContents->ulRadioServiceNum);

	if (pstContents->ulTotalTuningNum > 0)
	{
		ulProgress = ((pstContents->ulCompleteTpNum * NX_MENU_SEARCH_UI_PROG_100_PER) / pstContents->ulTotalTuningNum);
		if (ulProgress == NX_MENU_SEARCH_UI_PROG_100_PER)
		{
			pstContents->eSearchUiState = eSearchUi_state_searchFinish;
			NX_APP_Info("=== Search Complete!!!  eSearchUi_state_searchFinish!! ===\n");
		}
	}

	if (eBuilderStatus == eBUILDER_COMPLETE)
	{
		NX_APP_Info("=== Search Complete!!!  eBuilderStatus = eBUILDER_COMPLETE!! ===\n");
		pstContents->eSearchUiState = eSearchUi_state_searchFinish;
	}

	HxSTD_memcpy(&pstContents->stTpInfo, &stTpInfo, sizeof(DxTuneParam_t));
	NX_PRISM_UTIL_MakeTpStringByTuneParamAndTunerType(eTunerType, &stTpInfo, pstContents->szTpInfoString);
	NX_APP_Debug("\n nx_menuSearchChannel_Ui_MakeTpString() : [%s]!!! \n", pstContents->szTpInfoString);

	nx_menuSearchChannel_Ui_UpdateServiceInfo(pstContents);
	nx_menuSearchChannel_Ui_UpdateTPStatueInfo(pstContents);
	if (pstContents->eSearchUiState == eSearchUi_state_searchFinish)
	{
		nx_menuSearchChannel_Ui_UpdateButton(pstContents);
	}

	if (pstDetailInfo)
	{
		NX_SEARCH_FreeSearchServiceListInfo(pstDetailInfo);
		pstDetailInfo = NULL;
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgCoreChannelStateUpdate(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lSearchedTplength = 0;
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!! pstContents is NULL!!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	nx_menuSearchChannel_Ui_ActionSaveServiceForSaveData(pstContents);
	nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);
	nx_menuSearchChannel_Ui_ReloadServiceLists(pstContents);

	// Delete Saving process
	nx_menuSearchChannel_Ui_ProcessingMsgDestroy();

#if defined(CONFIG_MW_CH_SATELLITE)
	if (pstContents->ulTunerCount > NX_SEARCH_TUNER_SINGLE_COUNT)
	{
		if (pstContents->pstSearchedTPList)
		{
			lSearchedTplength = HLIB_VECTOR_Length(pstContents->pstSearchedTPList);
			NX_APP_Info("Searched TP List Num : (%u)\n", lSearchedTplength);
			if (lSearchedTplength > 0)
			{
				// Check Antenna Connection
				pstContents->eSearchUiState = eSearchUi_state_checkAntenna;
				nx_menuSearchChannel_Ui_ProcessingMsgCreate(eNxSettingsItem_eSearchUi_DialogType_CheckAntenna);
				eResult = nx_menuSearchChannel_Ui_ActionStartForAntennaConnection(pstContents);
				if (eResult != ONDK_RET_OK)
				{
					NX_APP_Error("Error!!! nx_menuSearchChannel_Ui_ActionStartForAntennaConnection() Fail !!\n");
					nx_menuSearchChannel_Ui_ProcessingMsgDestroy();
					pstContents->eSearchUiState = eSearchUi_state_end;
					pstContents->bExitMenu = TRUE;
					ONDK_GWM_APP_Destroy(0);
				}
				return	ONDK_RET_OK;
			}
		}
	}
	else
	{
		// Set eDxANTCONN_ONECABLE if 1 tuner box
		NX_SYSTEM_SetAntennaConnection(eDxANTCONN_ONECABLE);
	}
#endif

	pstContents->eSearchUiState = eSearchUi_state_end;
	pstContents->bExitMenu = TRUE;
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgAppDlgClicked(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Stop)
			{
				nx_menuSearchChannel_Ui_DestroySelectDialog();
				pstContents->eSearchUiState = eSearchUi_state_searchFinish;
				nx_menuSearchChannel_Ui_UpdateButton(pstContents);

				if (pstContents->ulSearchedServiceNum > 0)
				{
					// Show Save Message
					nx_menuSearchChannel_Ui_CreateSelectDialog(pstContents, eNxSettingsItem_eSearchUi_DialogType_Save);
				}
				else
				{
					// Only Stop Search
					nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);
					ONDK_GWM_APP_Destroy(0);
				}
			}
			else if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Save)
			{
				nx_menuSearchChannel_Ui_DestroySelectDialog();
				pstContents->eSearchUiState = eSearchUi_state_saveResult;
				nx_menuSearchChannel_Ui_ProcessingMsgCreate(eNxSettingsItem_eSearchUi_DialogType_SaveData);
				nx_menuSearchChannel_Ui_ActionLoadServiceForSaveData(pstContents);
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;

		case eNX_DLG_CLICKED_BTN_RIGHT:	// NO
			if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Stop)
			{
				nx_menuSearchChannel_Ui_DestroySelectDialog();
				if (pstContents->eSearchUiState == eSearchUi_state_pause)
				{
					pstContents->eSearchUiState = eSearchUi_state_searching;
					nx_menuSearchChannel_Ui_ActionResumeForSearch(pstContents);
				}
			}
			else if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Save)
			{
				// Only Stop Search
				nx_menuSearchChannel_Ui_DestroySelectDialog();
				nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);
				ONDK_GWM_APP_Destroy(0);
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;

		default:
			if ((ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Stop) || (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Save))
			{
				nx_menuSearchChannel_Ui_DestroySelectDialog();
			}
			else
			{
				NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgAppDlgCanceled(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Stop)
	{
		nx_menuSearchChannel_Ui_DestroySelectDialog();
		if (pstContents->eSearchUiState == eSearchUi_state_pause)
		{
			pstContents->eSearchUiState = eSearchUi_state_searching;
			nx_menuSearchChannel_Ui_ActionResumeForSearch(pstContents);
		}
	}
	else if (ulDlgWhoseId == eNxSettingsItem_eSearchUi_DialogType_Save)
	{
		nx_menuSearchChannel_Ui_DestroySelectDialog();
		// Only Stop Search
		nx_menuSearchChannel_Ui_ActionStopForSearch(pstContents);
		ONDK_GWM_APP_Destroy(0);
	}
	else
	{
		NX_APP_Info("Unknow Dialogue ID:(0x%x)\n", ulDlgWhoseId);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSearchChannel_Ui_MsgGwmTimer(Nx_MenuSearchUi_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSearchChannel_Ui_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_MENU_Search_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSearchUi_Contents_t	*pstContents = NULL;

	pstContents = nx_menuSearchChannel_Ui_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSearchChannel_Ui_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSearchChannel_Ui_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSearchChannel_Ui_MsgGwmClicked(pstContents, p1);
			break;

		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSearchChannel_Ui_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuSearchChannel_Ui_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSearchChannel_Ui_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuSearchChannel_Ui_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CANCELED!\n");
			eResult = nx_menuSearchChannel_Ui_MsgAppDlgCanceled(pstContents, p2);
			break;

		case MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE:
			NX_APP_Message("MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE!\n");
			eResult = nx_menuSearchChannel_Ui_MsgCoreSearchInfoUpdate(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION:
			NX_APP_Message("MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION!\n");
			eResult = nx_menuSearchChannel_Ui_MsgCoreAntennaConnectStateUpdate(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_CHANNEL_EVT_STATE_UPDATED:
			NX_APP_Message("MSG_CORE_CHANNEL_EVT_STATE_UPDATED!\n");
			eResult = nx_menuSearchChannel_Ui_MsgCoreChannelStateUpdate(pstContents, p1, p2, p3);
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

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

