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
 * @file	  		nx_prism_menu_search_sat_satsetting.c
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
#include <nx_core_message.h>
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
#define	NX_MENU_SRCHSAT_FIXED_TP_STRING_LEN						64
#define	NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS						32

// Strength TEXT
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_X			75
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_Y			474
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_W			100
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_H			32

// Strength BAR
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_X				186
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_Y				488
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_W				539
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_H				3

// Strength Value
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_X			735
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_Y			474
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_W			60
#define	NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_H			32

// QUALITy TEXT
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_X				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_Y				(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_W				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_H				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_H

// QUALITy BAR
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_X				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_Y				(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_W				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_H				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_H

// QUALITy Value
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_X			NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_Y			(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_W			NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_H			NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_H

// BER TEXT
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_X					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_Y					(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_W					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_H					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_H

// BER BAR
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_X					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_Y					(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_W					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_H					NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_H

// BER Value
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_X				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_X
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_Y				(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_Y + NX_MENU_SRCHSAT_FIXED_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_W				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_W
#define	NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_H				NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_H

// OK Button
#define	NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_X					170
#define	NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_Y					615
#define	NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_W					260
#define	NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_H					55

// CANCEL Button
#define	NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_X				440
#define	NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_Y				615
#define	NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_W				260
#define	NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_H				55

// Input Dialog Box
#define	NX_MENU_SRCHSAT_FIXED_FREQUENCY_INTPUT_LEN				5
#define	NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_X					290
#define	NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_Y					266
#define	NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_W					700
#define	NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_H					188


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_DialogType_None = 0,
	eNxSettingsItem_DialogType_DeleteAllChlist,
} Nx_SettingsItem_SatSetting_DialogType_t;


typedef enum {
	eNxSettingsItem_SatSet_SatelliteName,
	eNxSettingsItem_SatSet_LNB,
	eNxSettingsItem_SatSet_22kHz,
	eNxSettingsItem_SatSet_Diseqc,
	eNxSettingsItem_SatSet_TestTP,
	eNxSettingsItem_SatSet_Ok,
	eNxSettingsItem_SatSet_Cancel,
	eNxSettingsItem_SatSet_Max
} Nx_SettingsItem_SatSetting_t;

typedef struct tag_Nx_MenuSatSetting_contents
{
	/**@breif Public value */
	HUINT32				ulCurFocus;
	HUINT32				ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8				ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HUINT32				ulActionId;		// Not Used
	HUINT32				ulTunerId;		// not used
	HBOOL				bDispBER;
	HBOOL				bTestMode;
	DxAntennaType_e		eAntType;

	HINT32				lCurrAntUid;
	DxAntInfo_t			stAntInfo;
	DxAntInfo_t			stPrevAntInfo;

	HUINT32				ulNumOfTps;
	HUINT32				ulTpInfoIdx;
	NX_TP_TuningInfo_t	*pstTpInfoArray;

	HBOOL				bUserTpSelected;
	//NX_TP_TuningInfo_t	stUserTpInfo;
	Nx_TpSelectInfo_t	stUserTpInfo;

	HINT32				bTuneStateLock;
	DxDeliveryType_e	eDelitype;
	NX_SCANPARAMS_t 	pstScanParams;

	HCHAR				szSignalStrength[NX_TEXT_SIZE_8];
	HCHAR				szSignalQuality[NX_TEXT_SIZE_8];
	HCHAR				szSignalBER[NX_TEXT_SIZE_8];
} Nx_MenuSatSetting_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSatSetting_Contents_t			s_stSatSettingsContent;
static Nx_PrismListDialog_Content_t			s_stPopupListSatSettingsConts;
static Nx_PrismDialogInput_Content_t		s_stDialogInputSatSettingsConts;
static NXUI_DialogInstance_t				s_stDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuSatSettings_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuSatSettings_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSatSettings_CreateDeleteAllSatelliteChlistIDialog(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_DestroyDeleteAllSatelliteChlistDialog (void);

static ONDK_Result_t	nx_menuSatSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSatSettings_DestroyListPopup(void);
static void 			nx_menuSatSettings_CreateListPopup(Nx_MenuSatSetting_Contents_t *pContents, HINT32 ulObjId);

static ONDK_Result_t	nx_menuSatSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSatSettings_DestroyDialogInput(void);
static void				nx_menuSatSettings_CreateDialogInput(Nx_MenuSatSetting_Contents_t *pContents, HINT32 ulObjId);

static void				nx_menuSatSettings_StopTune(Nx_MenuSatSetting_Contents_t *pstContents);
static void				nx_menuSatSettings_TryTune(Nx_MenuSatSetting_Contents_t *pstContents);
static inline Nx_MenuSatSetting_Contents_t* nx_menuSatSettings_GetContents(void);
static inline HINT32	nx_menuSatSettings_GetDefaultFocus(void);
static HCHAR*			nx_menuSatSettings_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuSatSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSatSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuSatSettings_GetItemSupport(Nx_MenuSatSetting_Contents_t *pstContents, Nx_SettingsItem_SatSetting_t eNxSatSettingItem);
static HBOOL			nx_menuSatSettings_GetItemiDisable(Nx_SettingsItem_SatSetting_t eNxSatSettingItem);
static void				nx_menuSatSettings_FindNextFocus(Nx_MenuSatSetting_Contents_t *pstContents, HBOOL bUp);
static ONDK_Result_t	nx_menuSatSettings_ChangeFocus(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lKey);

static ONDK_Result_t	nx_menuSatSettings_RefreshSubItem(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static void				nx_menuSatSettings_RefreshAllSubItems(Nx_MenuSatSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSatSettings_UpdateTestTP(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static HINT32			nx_menuSatSettings_GetDiseqcValue (Nx_MenuSatSetting_Contents_t *pstContents);
static void				nx_menuSatSettings_SetDiseqcValue (Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lValue);
static ONDK_Result_t	nx_menuSatSettings_UpdateDiseqc(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_Update22kHzTone(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_SetUniversalLNBFrequecy(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_UpdateLnbFrequency(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_UpdateSatelliteName(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_UpdateSubItem(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuSatSettings_SaveSetting(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_CancelSetting(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_ChangeSubitemToKeyInput(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_ChangeSubitemToButton(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_ChangeItemEnableDisableMode(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable);

static ONDK_Result_t	nx_menuSatSettings_ResetItemTestTp(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_AddItemTestTp(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_AddItemDiseqc(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_AddItem22kHzTone(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_AddItemLnbFrequency(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_AddItemSatelliteName(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_InitTpSetting(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_InitSatSetting(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_InitAction(Nx_MenuSatSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSatSettings_InitSettingsItems(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lAntennaType, HINT32 lAntUid, HINT32 lTestMode);

static ONDK_Result_t	nx_menuSatSettings_ConvNxAntTypeToDxAntType(NX_SAT_AntennaType_e eNxAntType, DxAntennaType_e* eDxAntType);

static ONDK_Result_t	nx_menuSatSettings_DrawDefaultUI(Nx_MenuSatSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSatSettings_MsgGwmCreate(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSatSettings_MsgGwmDestroy(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSatSettings_MsgGwmHapiMsg(Nx_MenuSatSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSatSettings_MsgGwmClicked(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSatSettings_MsgGwmKeyDown(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_menuSatSettings_MsgAppValueChanged(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuSatSettings_MsgAppInputValueChange(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulInputValue);
static ONDK_Result_t	nx_menuSatSettings_MsgAppTpSettingEnd (Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lSaveFlag);
static ONDK_Result_t	nx_menuSatSettings_MsgAppDlgClicked(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuSatSettings_MsgAppDlgCanceled(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuSatSettings_MsgCoreSignalState(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulSignalStrength, HUINT32 ulSignalQuality);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuSatSettings_GetDialogPopupContents(void)
{
	return &s_stDialogMessageInstance;
}


static ONDK_Result_t	nx_menuSatSettings_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuSatSettings_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuSatSettings_CreateDeleteAllSatelliteChlistIDialog(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSatSettings_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxSettingsItem_DialogType_DeleteAllChlist;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString(RES_MESG_019_ID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Fixed_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuSatSettings_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_DestroyDeleteAllSatelliteChlistDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuSatSettings_DialogProc);

	return	ONDK_RET_OK;
}


#define _________Dialog_Input_Functions_________________________________________
static ONDK_Result_t	nx_menuSatSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_DialogInput_ProtoProc(&s_stDialogInputSatSettingsConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuSatSettings_DestroyDialogInput(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuSatSettings_DisplayDialogInput);
}


static void		nx_menuSatSettings_CreateDialogInput(Nx_MenuSatSetting_Contents_t *pContents, HINT32 ulObjId)
{
	HUINT32			ulAttribute = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t  *pstSubItem = NULL;

	/* Get subitem matched nObjId */
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents, ulObjId);
	if ( pstSubItem == NULL )
	{
		NX_APP_Error("[%s:%d] Error!!! pstSubItem is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return;
	}

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_X, NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_Y,
						NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_W, NX_MENU_SRCHSAT_FIXED_DIALOG_INPUT_H);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_DialogInput_SetDialog (&s_stDialogInputSatSettingsConts,
							ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, ulAttribute, COL(C_T_Whistle_100), (HUINT8*)ONDK_GetString(RES_MESG_3998_ID), pstSubItem->lSubItemValue, NX_MENU_SRCHSAT_FIXED_FREQUENCY_INTPUT_LEN);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSatSettings_DisplayDialogInput", APP_DEFAULT_PRIORITY, nx_menuSatSettings_DisplayDialogInput, 0, 0, 0, 0);
}


#define _________PopUp_List_Functions____________________________________________

static ONDK_Result_t	nx_menuSatSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSatSettingsConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuSatSettings_DestroyListPopup(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuSatSettings_DisplayListPopup);
}


static void		nx_menuSatSettings_CreateListPopup(Nx_MenuSatSetting_Contents_t *pContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList = NULL;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t  *pstSubItem = NULL;

	/* Get subitem matched nObjId */
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents, ulObjId);
	if ( pstSubItem == NULL )
	{
		return;
	}

	pptextList = pstSubItem->szItemTextList;
	ulTextListNum = pstSubItem->ulNumOfItemTextList;
	ulIndex	= pstSubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSatSettingsConts, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSatSettings_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuSatSettings_DisplayListPopup, 0, 0, 0, 0);
}

#define _________Normal_Functions____________________________________________

static void		nx_menuSatSettings_StopTune(Nx_MenuSatSetting_Contents_t *pstContents)
{
	NX_Builder_StopScan(NX_SCANTYPE_TUNEONLY);
	pstContents->bTuneStateLock = FALSE;
	NX_COMMON_NOT_USED_PARAM(pstContents);
}


static void		nx_menuSatSettings_TryTune(Nx_MenuSatSetting_Contents_t *pstContents)
{
	NX_SCANPARAMS_t		stScanParam;
	NX_SCANOPTIONS_t	stScanOption;
	//NX_SCANPARAMS_t		stScanParams;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;

	if (HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
	{
		NX_APP_Error("[%s:%d] Antenna Type is NONE\n", __FUNCTION__, __LINE__);
		return;
	}

	nx_menuSatSettings_StopTune(pstContents);

	HxSTD_memset(&stScanParam, 0, sizeof(NX_SCANPARAMS_t));
	HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));

	NX_SEARCH_GetSearchOption(&stScanOption);

	if (pstContents->bUserTpSelected == TRUE)
	{
		pstTuneInfo = &(pstContents->stUserTpInfo.stNxUserTpInfo);
	}
	else
	{
		pstTuneInfo = &(pstContents->pstTpInfoArray[pstContents->ulTpInfoIdx]);
	}

	NX_ANTENNA_SetInfo(pstContents->stAntInfo.uid, &(pstContents->stAntInfo));
	NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam(eDxDELIVERY_TYPE_SAT, &(pstContents->stAntInfo), pstTuneInfo, &stScanParam);
	NX_Builder_StartScan(NX_SCANTYPE_TUNEONLY, &stScanOption, NX_SEARCH_MAX_TP_TUNEONLY, &stScanParam);
	pstContents->bTuneStateLock = TRUE;

}


static inline Nx_MenuSatSetting_Contents_t* nx_menuSatSettings_GetContents(void)
{
	return	&s_stSatSettingsContent;
}


static inline HINT32	nx_menuSatSettings_GetDefaultFocus(void)
{
	return	eNxSettingsItem_SatSet_SatelliteName ;
}


static HCHAR*	nx_menuSatSettings_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
			return	RES_SATELLITE_ID;

		case eNxSettingsItem_SatSet_LNB:
			return	RES_LNB_FREQUENCY_ID;

		case eNxSettingsItem_SatSet_22kHz:
			return	RES_22KHZ_TONE_01_ID;

		case eNxSettingsItem_SatSet_Diseqc:
			return	RES_DISEQC_ID;

		case eNxSettingsItem_SatSet_TestTP:
			return	RES_TEST_TRANSPONDER_ID;

		default:
			NX_APP_Error("Error!!! Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuSatSettings_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;

		case eNxSettingsItem_SatSet_LNB:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_SATSETTING_LNBFREQUENCY;
			break;

		case eNxSettingsItem_SatSet_22kHz:
			*pulMenuItemId = NX_MENUITEM_ID_SET_ON_OFF;
			break;

		case eNxSettingsItem_SatSet_Diseqc:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_SATSETTING_DISEQC;
			break;

		case eNxSettingsItem_SatSet_TestTP:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Error!!! Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuSatSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SatelliteSetting);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SatSet_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSatSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SatSet_Max))
	{
		return 0;
	}

	return	(eNxSetting_SatelliteSetting | lItemId);
}


static HBOOL	nx_menuSatSettings_GetItemSupport(Nx_MenuSatSetting_Contents_t *pstContents, Nx_SettingsItem_SatSetting_t eNxSatSettingItem)
{
	HBOOL	bSupport = TRUE;

	switch (eNxSatSettingItem)
	{
		case eNxSettingsItem_SatSet_SatelliteName:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_SatSet_LNB:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_SatSet_22kHz:
			if ((pstContents->eAntType == eDxANT_TYPE_DISEQC) || (pstContents->eAntType == eDxANT_TYPE_SCD))
			{
				bSupport = TRUE;
			}
			else
			{
				bSupport = FALSE;
			}
			break;
		case eNxSettingsItem_SatSet_Diseqc:
			if (pstContents->eAntType == eDxANT_TYPE_DISEQC)
			{
				bSupport = TRUE;
			}
			else
			{

				bSupport = FALSE;
			}
			break;
		case eNxSettingsItem_SatSet_TestTP:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static HBOOL	nx_menuSatSettings_GetItemiDisable(Nx_SettingsItem_SatSetting_t eNxSatSettingItem)
{
	HBOOL	bDisable = TRUE;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);

	switch (eNxSatSettingItem)
	{
		case eNxSettingsItem_SatSet_SatelliteName:
			if (eOpType == eDxOPERATOR_AMX)
			{
				bDisable = TRUE;
			}
			else
			{
				bDisable = FALSE;
			}
			break;
		case eNxSettingsItem_SatSet_LNB:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SatSet_22kHz:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SatSet_Diseqc:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_SatSet_TestTP:
			bDisable = FALSE;
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}


static void		nx_menuSatSettings_FindNextFocus(Nx_MenuSatSetting_Contents_t *pstContents, HBOOL bUp)
{
	HUINT32			i = 0, ulSubItems = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	ulSubItems = NX_PRISM_AppContents_CountSubitem((void *)pstContents);

	for (i = 0; i < ulSubItems; i++)
	{
		if ( bUp == TRUE )
		{
			pstSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, (ulSubItems - 1) - i);
		}
		else
		{
			pstSubItem =  NX_PRISM_AppContents_GetSubitem((void *)pstContents, i);
		}

		if ((pstSubItem == NULL) || (pstSubItem->bDisable == TRUE))
		{
			continue;
		}

		pstContents->ulCurFocus = i;
		ONDK_GWM_Obj_SetFocus(pstSubItem->ulId);
		break;
	}
}


static ONDK_Result_t	nx_menuSatSettings_ChangeFocus(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lKey)
{
	HINT32			lObjId = 0, lNextObjId = 0, lItemIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lKey)
	{
		case KEY_ARROWUP:
			if (lItemIdx == eNxSettingsItem_SatSet_SatelliteName)
			{
				lNextObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_SatSet_Ok) || (lItemIdx == eNxSettingsItem_SatSet_Cancel))
			{
				nx_menuSatSettings_FindNextFocus(pstContents, TRUE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case KEY_ARROWDOWN:
			if (lItemIdx == eNxSettingsItem_SatSet_TestTP)
			{
				lNextObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_SatSet_Ok) || (lItemIdx == eNxSettingsItem_SatSet_Cancel))
			{
				nx_menuSatSettings_FindNextFocus(pstContents, FALSE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			if (lItemIdx == eNxSettingsItem_SatSet_Ok)
			{
				lNextObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Cancel);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if (lItemIdx == eNxSettingsItem_SatSet_Cancel)
			{
				lNextObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_RefreshSubItem(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HUINT8		**ppListOfItem = NULL;
	HUINT32		ulMenuId, ulListIdx, ulItems;
	HINT32		lSubItem = 0, lItemIdx = -1;
	HCHAR		*pcStrId = NULL;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	 *pstSubItem;


	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, (HUINT32)lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("Error!!! pstSubItem == NULL !!\n");
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pstSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pstSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pstSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuSatSettings_GetMenuItemIdFormObjId(lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
			lSubItem = 0;
			break;

		case eNxSettingsItem_SatSet_LNB:
			lSubItem = 0;
			break;

		case eNxSettingsItem_SatSet_22kHz:
			lSubItem = 0;
			break;

		default:
			lSubItem = -1;
			break;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(ulMenuId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuId);
		return	ONDK_RET_FAIL;
	}

	pstSubItem->ulNumOfItemTextList = ulItems;
	pstSubItem->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue(ulMenuId, lSubItem, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	pstSubItem->lSubItemValue = lSubItem;
	pstSubItem->ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	return	ONDK_RET_OK;
}


static void		nx_menuSatSettings_RefreshAllSubItems(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_SatSet_SatelliteName; i < eNxSettingsItem_SatSet_Ok; i++)
	{
		eResult = nx_menuSatSettings_RefreshSubItem(pstContents, nx_menuSatSettings_ConvertItemIdxToObjId(i));
	}
}


static ONDK_Result_t	nx_menuSatSettings_UpdateTestTP(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HUINT32					ulProperty = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	pstContents->ulTpInfoIdx = pstSubItem->ulIdxOfTextList;

	// user Define
	if (pstSubItem->ulIdxOfTextList == pstContents->ulNumOfTps)
	{
		pstSubItem->lSubItemValue = 0;
		nx_menuSatSettings_StopTune(pstContents);

		ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatTransponderSetting_Proc",
					ulProperty, NX_PRISM_MENU_Search_SatTransponderSetting_Proc,
					HANDLE_NULL, pstContents->eAntType, eNxSat_ANTENAA_SLOT_TEST, pstContents->lCurrAntUid);
		return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuSatSettings_GetDiseqcValue (Nx_MenuSatSetting_Contents_t *pstContents)
{
	return (HINT32)((pstContents->stAntInfo.info.diseqc.diseqcVersion << 8) | pstContents->stAntInfo.info.diseqc.diseqcInput);
}


static void		nx_menuSatSettings_SetDiseqcValue (Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lValue)
{
	pstContents->stAntInfo.info.diseqc.diseqcVersion = (DxSAT_DiseqcVer_e)(lValue >> 8);
	pstContents->stAntInfo.info.diseqc.diseqcInput = (DxSAT_DiseqcInput_e)(lValue & 0xFF);
}


static ONDK_Result_t	nx_menuSatSettings_UpdateDiseqc(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->stAntInfo.antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
		case eDxANT_TYPE_SCD:
			// NOT Set value
			break;
		case eDxANT_TYPE_DISEQC:
			nx_menuSatSettings_SetDiseqcValue(pstContents, pstSubItem->lSubItemValue);
			break;
		default:
			NX_APP_Error("[%s:%d] Unknow Antenna Type Action! PLS Check eAntType[%d]\n", __FUNCTION__, __LINE__, pstContents->eAntType);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_Update22kHzTone(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->stAntInfo.antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			// NOT Set value
			break;
		case eDxANT_TYPE_DISEQC:
			pstContents->stAntInfo.info.diseqc.b22kTone = (HBOOL)pstSubItem->lSubItemValue;
			break;
		case eDxANT_TYPE_SCD:
			pstContents->stAntInfo.info.scd.b22kTone = (HBOOL)pstSubItem->lSubItemValue;
			break;
		default:
			NX_APP_Error("[%s:%d] Unknow Antenna Type Action! PLS Check eAntType[%d]\n", __FUNCTION__, __LINE__, pstContents->eAntType);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_SetUniversalLNBFrequecy(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HINT32 		l22kHzToneObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL, *pst22KSubItem = NULL;

	/* Enable & Disable Item */
	pstSubItem = NULL;
	l22kHzToneObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_22kHz);
	pst22KSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, l22kHzToneObjId);
	if (pstContents->stAntInfo.antType == eDxANT_TYPE_DISEQC)
	{
		if (pst22KSubItem != NULL)
		{
			if (pstContents->stAntInfo.info.diseqc.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
			{
				HxSTD_StrNCpy((char*)pst22KSubItem->szSubItemText, (char*)ONDK_GetString(RES_AUTO_ID), MAX_ELEMENT_TEXT_LENGTH);
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, FALSE);
			}
			else
			{
				HxSTD_StrNCpy((char*)pst22KSubItem->szSubItemText, (char*)pst22KSubItem->szItemTextList[pst22KSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, TRUE);
			}
		}
	}
	else if (pstContents->stAntInfo.antType == eDxANT_TYPE_SCD)
	{
		if (pst22KSubItem != NULL)
		{
			if (pstContents->stAntInfo.info.scd.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
			{
				HxSTD_StrNCpy((char*)pst22KSubItem->szSubItemText, (char*)ONDK_GetString(RES_AUTO_ID), MAX_ELEMENT_TEXT_LENGTH);
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, FALSE);
			}
			else
			{
				HxSTD_StrNCpy((char*)pst22KSubItem->szSubItemText, (char*)pst22KSubItem->szItemTextList[pst22KSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, TRUE);
			}
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_UpdateLnbFrequency(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->stAntInfo.antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			pstContents->stAntInfo.info.lnb.ulLnbFreq = (HUINT32)pstSubItem->lSubItemValue;
			break;
		case eDxANT_TYPE_DISEQC:
			pstContents->stAntInfo.info.diseqc.ulLnbFreq = (HUINT32)pstSubItem->lSubItemValue;
			break;
		case eDxANT_TYPE_SCD:
			pstContents->stAntInfo.info.scd.ulLnbFreq = (HUINT32)pstSubItem->lSubItemValue;
			break;
		default:
			NX_APP_Error("[%s:%d] Unknow Antenna Type Action! PLS Check eAntType[%d]\n", __FUNCTION__, __LINE__, pstContents->eAntType);
			break;
	}

	/* Enable & Disable Item */
	nx_menuSatSettings_SetUniversalLNBFrequecy(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_UpdateSatelliteName(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			i = 0;
	Nx_SettingsLeafSubItem_t 	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	// Update Antenna Info
	HxSTD_StrNCpy((char*)pstContents->stAntInfo.satType, (char*)pstSubItem->szItemString, DxANTINFO_NAME_LEN);
	HxSTD_StrNCpy((char*)pstContents->stAntInfo.satName, (char*)pstSubItem->szItemString, DxANTINFO_NAME_LEN);

	/* Enable & Disable Item */
	// Select None
	if (HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
	{
		// Stop Tune
		nx_menuSatSettings_StopTune(pstContents);

		// Disable Item
		for (i = eNxSettingsItem_SatSet_LNB; i < eNxSettingsItem_SatSet_Ok; i++)
		{
			nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, i, FALSE);
		}

		return	ONDK_RET_FAIL;
	}
	else
	{
		// Enable / Disable Item
		nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_LNB, TRUE);
		nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_TestTP, TRUE);
		if (pstContents->stAntInfo.antType == eDxANT_TYPE_DISEQC)
		{
			if (pstContents->stAntInfo.info.diseqc.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
			{
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, FALSE);
			}
			else
			{
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, TRUE);
			}

			nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_Diseqc, TRUE);
		}
		else if (pstContents->stAntInfo.antType == eDxANT_TYPE_SCD)
		{
			if (pstContents->stAntInfo.info.scd.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
			{
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, FALSE);
			}
			else
			{
				nx_menuSatSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_SatSet_22kHz, TRUE);
			}
		}
		else
		{
			NX_APP_Info("Unknow Antenna Type Action! PLS Check eAntType[%d]\n", pstContents->stAntInfo.antType);
		}
	}

	// Change Antenna TP info
	nx_menuSatSettings_InitTpSetting(pstContents);
	nx_menuSatSettings_ResetItemTestTp(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_UpdateSubItem(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
			eResult = nx_menuSatSettings_UpdateSatelliteName(pstContents, lObjId);
			break;

		case eNxSettingsItem_SatSet_LNB:
			eResult = nx_menuSatSettings_UpdateLnbFrequency(pstContents, lObjId);
			break;

		case eNxSettingsItem_SatSet_22kHz:
			eResult = nx_menuSatSettings_Update22kHzTone(pstContents, lObjId);
			break;

		case eNxSettingsItem_SatSet_Diseqc:
			eResult = nx_menuSatSettings_UpdateDiseqc(pstContents, lObjId);
			break;

		case eNxSettingsItem_SatSet_TestTP:
			eResult = nx_menuSatSettings_UpdateTestTP(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	if (eResult == ONDK_RET_OK)
	{
		nx_menuSatSettings_TryTune(pstContents);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_SaveSetting(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL	bAddAntenna = FALSE;

	NX_ANTENNA_SetInfo(pstContents->stAntInfo.uid, &(pstContents->stAntInfo));

	if ((HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0) || (HxSTD_StrCmp((char*)pstContents->stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) == 0))
	{
		bAddAntenna = FALSE;
	}
	else
	{
		bAddAntenna = TRUE;
	}

	ONDK_GWM_SendMessage(NULL, MSG_APP_SETTING_ANTENNA_SETTING_CHANGED, (HINT32)NULL, pstContents->stAntInfo.uid, (HINT32)bAddAntenna, 0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_CancelSetting(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HINT32	lPrevAntUid = NX_ANT_IDX_NULL, lAntUid = NX_ANT_IDX_NULL;

	lPrevAntUid = pstContents->stPrevAntInfo.uid;
	if (lPrevAntUid == NX_ANT_IDX_NULL)
	{
		lAntUid = pstContents->stAntInfo.uid;
	}
	HxSTD_memcpy(&(pstContents->stAntInfo), &(pstContents->stPrevAntInfo), sizeof(DxAntInfo_t));
	NX_ANTENNA_SetInfo(lAntUid, &(pstContents->stAntInfo));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_ChangeItemEnableDisableMode(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuSatSettings_ConvertItemIdxToObjId(lItemIdx);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	if (bEnable == TRUE)
	{
		pstSubItem->bDisable = FALSE;
		ONDK_GWM_Obj_EnableObject(lObjId);
	}
	else
	{
		pstSubItem->bDisable = TRUE;
		ONDK_GWM_Obj_DisableObject(lObjId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_ResetItemTestTp(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32 			ulItems = 0, ulIndex = 0;
	HUINT8				**ppListOfItem = NULL;
	HINT32				lObjId = 0, lStrLen = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_TestTP);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	// Clear the sub item.
	NX_PRISM_AppContents_ClearLeafSubItem(pstSubItem);

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(lObjId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)pstSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Current Antenna TP Num + User Define
	ulItems = pstContents->ulNumOfTps + 1;

	// Set the selector item list
	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (ulItems, NX_PORT_MAX_TP_NAME);
	if (ppListOfItem != NULL)
	{
		for (ulIndex = 0; ulIndex < ulItems; ulIndex++)
		{
			if (ulIndex == ulItems - 1) 			// Item Last : User Defined
			{
				lStrLen = HxSTD_StrLen((char*)ppListOfItem[ulIndex]);
				HxSTD_StrNCpy ((char*)ppListOfItem[ulIndex], (char*)ONDK_GetString(RES_USER_DEFINE_ID), (NX_PORT_MAX_TP_NAME -1));
			}
			else									// Item 0 ~ TP Nums : Default TPs
			{
				NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstContents->pstTpInfoArray[ulIndex]), NULL, ppListOfItem[ulIndex]);
			}
		}

		pstSubItem->ulNumOfItemTextList = ulItems;
		pstSubItem->szItemTextList = ppListOfItem;
		pstSubItem->ulIdxOfTextList = 0;
		HxSTD_StrNCpy ((char*)pstSubItem->szSubItemText, (char*)ppListOfItem[0], MAX_ELEMENT_TEXT_LENGTH);
	}

	if (pstContents->ulNumOfTps == 0)
	{
		pstContents->bUserTpSelected = TRUE;
		NX_ANTENNA_GetUserTPInfo (pstContents->lCurrAntUid, &pstContents->stUserTpInfo);

		// 제대로 된 값이 있다면 그 Stream을 보여주도록 한다.
		if (pstContents->lCurrAntUid != NX_ANT_IDX_NULL)
		{
			NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstContents->stUserTpInfo.stNxUserTpInfo), NULL, pstSubItem->szSubItemText);
		}
	}
	else
	{
		pstContents->bUserTpSelected = FALSE;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_AddItemTestTp(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulIndex = 0;
	HUINT8				**ppListOfItem = NULL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuSatSettings_GetItemSupport(pstContents, eNxSettingsItem_SatSet_TestTP);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SatSet_TestTP(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_TestTP);

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// Current Antenna TP Num + User Define
	ulItems	= pstContents->ulNumOfTps + 1;

	// Set the selector item list
	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (ulItems, NX_PORT_MAX_TP_NAME);
	if (ppListOfItem != NULL)
	{
		for (ulIndex = 0; ulIndex < ulItems; ulIndex++)
		{
			if (ulIndex == (ulItems - 1))			// Item Last : User Defined
			{
				HxSTD_StrNCpy ((char*)ppListOfItem[ulIndex], (char*)ONDK_GetString(RES_USER_DEFINE_ID), (NX_PORT_MAX_TP_NAME -1));
			}
			else									// Item 0 ~ TP Nums : Default TPs
			{
				NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstContents->pstTpInfoArray[ulIndex]), NULL, ppListOfItem[ulIndex]);
			}
		}

		stSubItem.ulNumOfItemTextList = ulItems;
		stSubItem.szItemTextList = ppListOfItem;
		stSubItem.ulIdxOfTextList = 0;
		HxSTD_StrNCpy ((char*)stSubItem.szSubItemText, (char*)ppListOfItem[0], MAX_ELEMENT_TEXT_LENGTH);
	}

	pstContents->ulTpInfoIdx = 0;

	if (pstContents->ulNumOfTps == 0)
	{
		pstContents->bUserTpSelected = TRUE;
		NX_ANTENNA_GetUserTPInfo (pstContents->lCurrAntUid, &pstContents->stUserTpInfo);

		// 제대로 된 값이 있다면 그 Stream을 보여주도록 한다.
		if (pstContents->lCurrAntUid != NX_ANT_IDX_NULL)
		{
			NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstContents->stUserTpInfo.stNxUserTpInfo), NULL, stSubItem.szSubItemText);
		}
	}
	else
	{
		pstContents->bUserTpSelected = FALSE;
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_AddItemDiseqc(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HINT32				lDiseqcValue = 0;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuSatSettings_GetItemSupport(pstContents, eNxSettingsItem_SatSet_Diseqc);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SatSet_Diseqc(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Diseqc);

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuSatSettings_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	lDiseqcValue = nx_menuSatSettings_GetDiseqcValue(pstContents);

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)lDiseqcValue, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)lDiseqcValue;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_AddItem22kHzTone(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuSatSettings_GetItemSupport(pstContents, eNxSettingsItem_SatSet_22kHz);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SatSet_22kHz(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_22kHz);

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuSatSettings_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems()!!\n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stAntInfo.info.diseqc.b22kTone, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stAntInfo.info.diseqc.b22kTone;
	stSubItem.ulIdxOfTextList = ulListIdx;

	if ((pstContents->stAntInfo.info.diseqc.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
		|| (pstContents->stAntInfo.info.scd.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
	)
	{
		HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)ONDK_GetString(RES_AUTO_ID), MAX_ELEMENT_TEXT_LENGTH);
	}
	else
	{
		HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_AddItemLnbFrequency(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuSatSettings_GetItemSupport(pstContents, eNxSettingsItem_SatSet_LNB);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SatSet_LNB(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_LNB);

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuSatSettings_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stAntInfo.info.lnb.ulLnbFreq, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stAntInfo.info.lnb.ulLnbFreq;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_AddItemSatelliteName(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	HCHAR				*pMainStrId = NULL, *szSatName = NULL;
	HINT32				lIndex = -1;
	HUINT32				ulSatNum = 0, ulCount = 0;
	HUINT8				**aszItemNames = NULL, *pszFind = NULL;
	HERROR				hError = ERR_FAIL;
	Nx_SAT_SatRegionList_t		*pastSatList = NULL;
	NX_SAT_SatelliteRegionType_t eSatRegionType = eNxSat_SatRegion_Normal;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuSatSettings_GetItemSupport(pstContents, eNxSettingsItem_SatSet_SatelliteName);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_SatSet_SatelliteName(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_SatelliteName);

	// Check bDisable Item
	bDisable = nx_menuSatSettings_GetItemiDisable(eNxSettingsItem_SatSet_SatelliteName);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pMainStrId = nx_menuSatSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}
	HxSTD_sprintf((char*)stSubItem.szSubItemText, (char*)"%s", ONDK_GetString(pMainStrId));

	NX_ANTENNA_GetSatelliteRegionType(&eSatRegionType);
	hError = NX_ANTENNA_GetRegionSatelliteList(eSatRegionType, &pastSatList, &ulSatNum);
	if ((hError != ERR_OK) || (ulSatNum == 0))
	{
		stSubItem.bDisable = TRUE;
		HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)NX_SAT_SATNAME_NONE, MAX_ELEMENT_TEXT_LENGTH);
	}

	aszItemNames = NX_PRISM_MENU_MenuItem_AllocTextList (ulSatNum, NX_PORT_MAX_ANT_NAME);
	if (aszItemNames != NULL)
	{
		// 0 : (NONE)
		HxSTD_StrNCpy((char*)aszItemNames[0],  (char*)ONDK_GetString(RES_NONE_01_ID), (NX_PORT_MAX_ANT_NAME - 1));

		// 1 ~ N : Satellite
		for (ulCount = 0; ulCount < ulSatNum; ulCount++)
		{
			if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_NONE) == 0)
			{
				break;
			}

			if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)pstContents->stAntInfo.satType) == 0)
			{
				stSubItem.ulIdxOfTextList = ulCount + 1;
				lIndex = ulCount + 1;
			}

			pszFind = (HUINT8*)HLIB_STD_StrCaseStr((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE);
			if (pszFind == NULL)
			{
				szSatName = NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(pastSatList[ulCount].szSatTypeName);
				NX_ANTENNA_GetSatelliteNameWithSatPosition (pastSatList[ulCount].szSatTypeName, szSatName, aszItemNames[ulCount + 1]);
			}
			else
			{
				if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE) == 0)
				{
					HxSTD_StrNCpy((char*)aszItemNames[ulCount + 1], (char*)ONDK_GetString(RES_USER_DEFINE_ID), NX_PORT_MAX_ANT_NAME - 1);
				}
				else if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE1) == 0)
				{
					HxSTD_sprintf((char*)aszItemNames[ulCount + 1], "%s %d", (char*)ONDK_GetString(RES_USER_DEFINE_ID), 1);
				}
				else if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE2) == 0)
				{
					HxSTD_sprintf((char*)aszItemNames[ulCount + 1], "%s %d", (char*)ONDK_GetString(RES_USER_DEFINE_ID), 2);
				}
				else if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE3) == 0)
				{
					HxSTD_sprintf((char*)aszItemNames[ulCount + 1], "%s %d", (char*)ONDK_GetString(RES_USER_DEFINE_ID), 3);
				}
				else if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE4) == 0)
				{
					HxSTD_sprintf((char*)aszItemNames[ulCount + 1], "%s %d", (char*)ONDK_GetString(RES_USER_DEFINE_ID), 4);
				}
				else if (HxSTD_StrCmp((char*)pastSatList[ulCount].szSatTypeName, (char*)NX_SAT_SATNAME_USERDEFINE5) == 0)
				{
					HxSTD_sprintf((char*)aszItemNames[ulCount + 1], "%s %d", (char*)ONDK_GetString(RES_USER_DEFINE_ID), 5);
				}
			}

		}

		stSubItem.ulNumOfItemTextList = ulSatNum;
		stSubItem.szItemTextList = aszItemNames;
		stSubItem.lSubItemValue	= lIndex;

		HxSTD_StrNCpy((char*)stSubItem.szItemString, (char*)aszItemNames[stSubItem.ulIdxOfTextList], MAX_ITEM_TEXT_LENGTH);
		stSubItem.szItemString[MAX_ITEM_TEXT_LENGTH - 1] = '\0';
		HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)aszItemNames[stSubItem.ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
		stSubItem.szSubItemText[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_InitSatSetting(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HINT32				ulPrevUid = NX_ANT_IDX_NULL;
	DxAntInfo_t			stAntInfo;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	ONDK_RET_INITFAIL;
	}

	ulPrevUid = pstContents->lCurrAntUid;
	HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));

	if (pstContents->lCurrAntUid == NX_ANT_IDX_NULL)
	{
		hError = NX_ANTENNA_MakeDefaultAntennaInfo(pstContents->eAntType, &stAntInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_ANTENNA_MakeDefaultAntennaInfo() Fail!! - Antenna Type:(%d)\n", pstContents->eAntType);
			return	ONDK_RET_FAIL;
		}

		pstContents->lCurrAntUid = stAntInfo.uid;
	}
	else
	{
		hError = NX_ANTENNA_GetInfo(pstContents->lCurrAntUid, &stAntInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_ANTENNA_MakeDefaultAntennaInfo() Fail!! - Antenna Type:(%d)\n", pstContents->eAntType);
			return	ONDK_RET_FAIL;
		}
		stAntInfo.antType = pstContents->eAntType;
	}

	HxSTD_memcpy(&(pstContents->stAntInfo), &stAntInfo, sizeof(DxAntInfo_t));
	HxSTD_memcpy(&(pstContents->stPrevAntInfo), &stAntInfo, sizeof(DxAntInfo_t));
	pstContents->stPrevAntInfo.uid = ulPrevUid;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_InitTpSetting(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulTotalTpNum = 0;
	NX_TP_TuningInfo_t	*pstTpInfoArray = NULL;

	if (pstContents == NULL)
	{
		return	ONDK_RET_INITFAIL;
	}

	pstContents->ulNumOfTps = 0;
	pstContents->ulTpInfoIdx = 0;
	if (pstContents->pstTpInfoArray != NULL)
	{
		NX_ANTENNA_FreeTpListArray (pstContents->pstTpInfoArray);
		pstContents->pstTpInfoArray = NULL;
	}

	hError = NX_ANTENNA_GetTpListBySatelliteTypeName (pstContents->stAntInfo.satType, &ulTotalTpNum, &pstTpInfoArray);
	if (hError != ERR_OK)
	{
		ulTotalTpNum = 0;
		pstTpInfoArray = NULL;
	}

	pstContents->ulNumOfTps = ulTotalTpNum;
	pstContents->pstTpInfoArray = pstTpInfoArray;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_InitAction(Nx_MenuSatSetting_Contents_t *pstContents)
{
	// Stop Channel
	NX_AV_StopChannel();

	// TODO: Stop PVR Action

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_InitSettingsItems(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lAntennaType, HINT32 lAntUid, HINT32 lTestMode)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	DxAntennaType_e eDxAntType = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_InitSubitemArray() !!\n");
		return eResult;
	}

	nx_menuSatSettings_ConvNxAntTypeToDxAntType(lAntennaType, &eDxAntType);

	// for saving user specific settings
	pstContents->eAntType = eDxAntType;
	pstContents->lCurrAntUid = lAntUid;
	pstContents->bTestMode = (HBOOL)lTestMode;
	pstContents->eDelitype = eDxDELIVERY_TYPE_SAT;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			pstContents->bDispBER = TRUE;
			break;
		default:
			pstContents->bDispBER = FALSE;
			break;
	}

	// tune state
	pstContents->bTuneStateLock = FALSE;

	// Init Item Value
	eResult = nx_menuSatSettings_InitSatSetting(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuSatSettings_InitSatSetting() !!\n");
		return eResult;
	}
	nx_menuSatSettings_InitTpSetting(pstContents);

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SATELLITE_SETTING_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->ucTitle[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';

	// Default Function
	pstContents->ulCurFocus = nx_menuSatSettings_GetDefaultFocus();

	nx_menuSatSettings_AddItemSatelliteName(pstContents);
	nx_menuSatSettings_AddItemLnbFrequency(pstContents);
	nx_menuSatSettings_AddItem22kHzTone(pstContents);
	nx_menuSatSettings_AddItemDiseqc(pstContents);
	nx_menuSatSettings_AddItemTestTp(pstContents);
	nx_menuSatSettings_SetUniversalLNBFrequecy(pstContents);

	nx_menuSatSettings_FindNextFocus(pstContents, FALSE);

 	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_ConvNxAntTypeToDxAntType(NX_SAT_AntennaType_e eNxAntType, DxAntennaType_e* eDxAntType)
{
	switch(eNxAntType)
	{
		case eNxSat_AntType_LNB_Only:
			*eDxAntType = eDxANT_TYPE_LNB_ONLY;
			break;
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
			*eDxAntType = eDxANT_TYPE_DISEQC;
			break;
		case eNxSat_AntType_SCD:
			*eDxAntType = eDxANT_TYPE_SCD;
			break;
		case eNxSat_AntType_Monoblock_LNB:
			*eDxAntType = eDxANT_TYPE_MONOBLOCK_LNB;
			break;
		default:
			return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_DrawDefaultUI(Nx_MenuSatSetting_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	ONDK_Rect_t		stRectBar = { 0, 0, 0, 0 };
	ONDK_Rect_t		stRectvalue = { 0, 0, 0, 0 };

	NX_PRISM_COMP_Settings_DrawBackgroundPopUp();
	NX_PRISM_COMP_Settings_DrawTitlePopUp(pstContents->ucTitle);
	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow);

	// Signal Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_X, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_Y,
						NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_W, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_TEXT_H);
	stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_X, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_Y,
							NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_W, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_BAR_H);
	stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_X, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_Y,
								NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_W, NX_MENU_SRCHSAT_FIXED_SATSET_STRENGTH_VALUE_H);
	HxSTD_snprintf((char *)pstContents->szSignalStrength, sizeof(pstContents->szSignalStrength), "0%%");
	NX_PRISM_COMP_Settings_CreateDrawStrengthBar(&stRect, &stRectBar, &stRectvalue, pstContents->szSignalStrength);

	// Quality Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_X, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_Y,
						NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_W, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_TEXT_H);
	stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_X, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_Y,
							NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_W, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_BAR_H);
	stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_X, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_Y,
							NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_W, NX_MENU_SRCHSAT_FIXED_SATSET_QUALITY_VALUE_H);
	HxSTD_snprintf((char *)pstContents->szSignalQuality, sizeof(pstContents->szSignalQuality), "0%%");
	NX_PRISM_COMP_Settings_CreateDrawQualitytBar(&stRect, &stRectBar, &stRectvalue, pstContents->szSignalQuality);

	if (pstContents->bDispBER == TRUE)
	{
		// BER Info
		stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_X, NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_Y,
							NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_W, NX_MENU_SRCHSAT_FIXED_SATSET_BER_TEXT_H);
		stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_X, NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_Y,
								NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_W, NX_MENU_SRCHSAT_FIXED_SATSET_BER_BAR_H);
		stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_X, NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_Y,
									NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_W, NX_MENU_SRCHSAT_FIXED_SATSET_BER_VALUE_H);
		HxSTD_snprintf((char *)pstContents->szSignalBER, sizeof(pstContents->szSignalBER), "0%%");
		NX_PRISM_COMP_Settings_CreateDrawBERBar(&stRect, &stRectBar, &stRectvalue, pstContents->szSignalBER);
	}

	// Button
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_X, NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_Y, NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_W, NX_MENU_SRCHSAT_FIXED_SATSET_OK_BTN_H);
	lObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_X, NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_Y, NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_W, NX_MENU_SRCHSAT_FIXED_SATSET_CANCEL_BTN_H);
	lObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgGwmCreate(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Action
	nx_menuSatSettings_InitAction(pstContents);

	// Init Menu Item
	eResult = nx_menuSatSettings_InitSettingsItems(pstContents, p1, p2, p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuSatSettings_InitSettingsItems() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	// Draw Menu Item
	nx_menuSatSettings_DrawDefaultUI(pstContents);

	// Try Tune
	NX_Builder_AddNotiSignalInfo();
	nx_menuSatSettings_TryTune(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgGwmDestroy(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	nx_menuSatSettings_StopTune(pstContents);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSatSetting_Contents_t));
	nx_menuSatSettings_DestroyListPopup();
	nx_menuSatSettings_DestroyDialogInput();
	NX_Builder_ReleaseNotiSignalInfo();

	NX_PRISM_LIVE_ServiceStartByLastService();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgGwmHapiMsg(Nx_MenuSatSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSatSettings_MsgGwmClicked(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1, lSatelliteListNum = 0;

	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
		case eNxSettingsItem_SatSet_LNB:
		case eNxSettingsItem_SatSet_22kHz:
		case eNxSettingsItem_SatSet_Diseqc:
		case eNxSettingsItem_SatSet_TestTP:
			nx_menuSatSettings_CreateListPopup(pstContents, lObjId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_SatSet_Ok:
			if (pstContents->bTestMode == TRUE)
			{
				nx_menuSatSettings_CancelSetting(pstContents);
			}
			else
			{
				if (HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
				{
					NX_CHANNEL_GetServiceListBySatelliteName((HUINT8*)pstContents->stPrevAntInfo.satType, NULL, &lSatelliteListNum);
					//NX_APP_Info("NX_CHANNEL_GetServiceListBySatelliteName() Name:(%s), lSatelliteListNum:(%d) \n", pstContents->stPrevAntInfo.satType, lSatelliteListNum);
					HxLOG_Print("=====>[JHLEE] NX_CHANNEL_GetServiceListBySatelliteName() Name:(%s), lSatelliteListNum:(%d)\n", pstContents->stPrevAntInfo.satType, lSatelliteListNum);
					if (lSatelliteListNum > 0)
					{
						nx_menuSatSettings_CreateDeleteAllSatelliteChlistIDialog(pstContents);
						return	ONDK_MESSAGE_BREAK;
					}
				}
				nx_menuSatSettings_SaveSetting(pstContents);
			}
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_SatSet_Cancel:
			nx_menuSatSettings_CancelSetting(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		default:
			return	ONDK_RET_OK;
	}
}


static ONDK_Result_t	nx_menuSatSettings_MsgGwmKeyDown(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL!!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			nx_menuSatSettings_CancelSetting(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			nx_menuSatSettings_CancelSetting(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			return	nx_menuSatSettings_ChangeFocus(pstContents, lKeyId);

		case KEY_STANDBY:
			nx_menuSatSettings_CancelSetting(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgCoreSignalState(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulSignalStrength, HUINT32 ulSignalQuality)
{
	HERROR				hError = ERR_FAIL;
	HINT32				lBERValue = 0, lDecimal = 0, lFloat = 0, lExp = 0;
	NX_SignalInfo_t		stSignalInfo;

	NX_PRISM_COMP_Settings_UpdateDrawStrenghtBar(ulSignalStrength);
	NX_PRISM_COMP_Settings_UpdateDrawQualityBar(ulSignalQuality);

	if (pstContents->bDispBER == TRUE)
	{
		// TODO: [JHLEE] Update mulit tunner
		lBERValue = 0;
		HxSTD_memset(&stSignalInfo, 0x00, sizeof(NX_SignalInfo_t));
		hError = NX_SYSTEM_GetSignalInfo(0, &stSignalInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_GetSignalInfo() \n");
		}

		NX_PRISM_UTIL_GetMakeChannelBerValue((HINT32)stSignalInfo.fBer, &lDecimal, &lFloat, &lExp);
		NX_PRISM_COMP_Settings_UpdateDrawBERBar((HUINT32)ulSignalQuality, lDecimal, lFloat, lFloat);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgAppInputValueChange(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulInputValue)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatSet_LNB:
			pstSubItem->lSubItemValue = (HINT32)ulInputValue;
			HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
			HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%u", ulInputValue);
			break;

		default:
			NX_APP_Error("[%s:%d] Error!! Not Supported itme lItemIdx:[%d]\n", __FUNCTION__, __LINE__, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	nx_menuSatSettings_TryTune(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgAppValueChanged(Nx_MenuSatSetting_Contents_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1, l22kObjId = 0;
	HUINT32 		ulMenuItemId = 0, ulPrevSelectedIdx = 0, ulSatNum = 0;
	HERROR			hError = ERR_FAIL;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL, *pst22KSubItem = NULL;
	Nx_SAT_SatRegionList_t		*pastSatList = NULL;
	NX_SAT_SatelliteRegionType_t eSatRegionType = eNxSat_SatRegion_Normal;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSatSettings_ConvertObjIdToItemIdx(lObjId);

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	ulPrevSelectedIdx = pstSubItem->ulIdxOfTextList;
	pstSubItem->ulIdxOfTextList = ulCurIdx;
	HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatSet_SatelliteName :
			pstSubItem->lSubItemValue = pstSubItem->ulIdxOfTextList;
			NX_ANTENNA_GetSatelliteRegionType(&eSatRegionType);
			hError = NX_ANTENNA_GetRegionSatelliteList(eSatRegionType, &pastSatList, &ulSatNum);
			if ((hError == ERR_OK) && (ulSatNum > 0))
			{
				if (pstSubItem->ulIdxOfTextList == 0)
				{
					HxSTD_StrNCpy((char*)pstSubItem->szItemString, (char*)(char*)NX_SAT_SATNAME_NONE, MAX_ITEM_TEXT_LENGTH);
				}
				else
				{
					HxSTD_StrNCpy((char*)pstSubItem->szItemString, (char*)pastSatList[pstSubItem->ulIdxOfTextList - 1].szSatTypeName, MAX_ITEM_TEXT_LENGTH);
				}
			}
			else
			{
				HxSTD_StrNCpy((char*)pstSubItem->szItemString, (char*)(char*)NX_SAT_SATNAME_NONE, MAX_ITEM_TEXT_LENGTH);
			}
			nx_menuSatSettings_UpdateSubItem(pstContents, lObjId);
			break;
		case eNxSettingsItem_SatSet_LNB:
			if (pstSubItem->ulIdxOfTextList != (pstSubItem->ulNumOfItemTextList - 1)) // Normal Value
			{
				nx_menuSatSettings_GetMenuItemIdFormObjId(lObjId, &ulMenuItemId);
				NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));
				nx_menuSatSettings_UpdateSubItem(pstContents, lObjId);
			}
			else	// if Select USER Define
			{
				if ((pstContents->eAntType == eDxANT_TYPE_DISEQC) || (pstContents->eAntType == eDxANT_TYPE_SCD))
				{
					l22kObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_22kHz);
					pst22KSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, l22kObjId);
					if (pst22KSubItem != NULL)
					{
						HxSTD_StrNCpy((HCHAR*)pst22KSubItem->szSubItemText, (HCHAR*)pst22KSubItem->szItemTextList[pst22KSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						pst22KSubItem->bDisable = FALSE;
						ONDK_GWM_Obj_EnableObject(l22kObjId);
					}
				}

				// TODO: Remain prev value?????
				pstSubItem->lSubItemValue = 0;
				nx_menuSatSettings_CreateDialogInput(pstContents, lObjId);
			}
			break;
		case eNxSettingsItem_SatSet_22kHz:
		case eNxSettingsItem_SatSet_Diseqc:
			/* Update subItem value */
			nx_menuSatSettings_GetMenuItemIdFormObjId(lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));
			nx_menuSatSettings_UpdateSubItem(pstContents, lObjId);
			break;
		case eNxSettingsItem_SatSet_TestTP:
			nx_menuSatSettings_UpdateSubItem(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgAppTpSettingEnd (Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lSaveFlag)
{
	HINT32						lObjId = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuSatSettings_ConvertItemIdxToObjId(eNxSettingsItem_SatSet_TestTP);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);

	if (lSaveFlag == TRUE)	// Save user TP Info
	{
		HxSTD_memset(&(pstContents->stUserTpInfo), 0x00, sizeof(Nx_TpSelectInfo_t));
		NX_ANTENNA_GetUserTPInfo(eNxSat_ANTENAA_SLOT_TEST, &(pstContents->stUserTpInfo));
		if (pstSubItem != NULL)
		{
			NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstContents->stUserTpInfo.stNxUserTpInfo), NULL, pstSubItem->szSubItemText);
			ONDK_GWM_COM_Button_ChangeTitle (pstSubItem->ulId, (char *)pstSubItem->szSubItemText);
		}
		pstContents->bUserTpSelected = TRUE;
	}
	else	// Cancel TP Info
	{
		if (pstSubItem != NULL)
		{
			// Set the index to 0:
			HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[0], MAX_ELEMENT_TEXT_LENGTH);
			pstSubItem->ulIdxOfTextList = 0;
			pstContents->ulTpInfoIdx = 0;
			HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
			ONDK_GWM_COM_Button_ChangeTitle (pstSubItem->ulId, (char *)pstSubItem->szSubItemText);
		}
		pstContents->bUserTpSelected = FALSE;
	}

	// Tune the TP
	nx_menuSatSettings_TryTune(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgAppDlgClicked(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	HERROR		hError = ERR_FAIL;

	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_DeleteAllChlist)
			{
				nx_menuSatSettings_DestroyDeleteAllSatelliteChlistDialog();
				hError = NX_CHANNEL_DeleteServiceListBySatelliteName((HUINT8*)pstContents->stPrevAntInfo.satType);
				if (hError == ERR_OK)
				{
					ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_UPDATE_SERVICE, 0, 0, 0, 0);
				}
				// TODO: [JHLEE] Update Servie Change
				nx_menuSatSettings_SaveSetting(pstContents);
				ONDK_GWM_APP_Destroy(0);
			}
			break;
		default:					// No
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_DeleteAllChlist)
			{
				nx_menuSatSettings_DestroyDeleteAllSatelliteChlistDialog();
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSatSettings_MsgAppDlgCanceled(Nx_MenuSatSetting_Contents_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxSettingsItem_DialogType_DeleteAllChlist)
	{
		nx_menuSatSettings_DestroyDeleteAllSatelliteChlistDialog();
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Search_SatSatelliteSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSatSetting_Contents_t *pstContents = NULL;

	pstContents = nx_menuSatSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			// p1 : AntennaType, p2 : Antenna Slot index, p3 : Antenna uid
			eResult = nx_menuSatSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSatSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list & Action */
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSatSettings_MsgGwmClicked(pstContents, p1);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_SCROLLRIGHT!\n");
			nx_menuSatSettings_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSatSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuSatSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuSatSettings_MsgAppValueChanged(pstContents, p1);
			break;

		case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_INPUT_VALUE_CHANGED!\n");
			nx_menuSatSettings_MsgAppInputValueChange(pstContents, p1);
			break;

		case MSG_CORE_SERVICE_EVT_SIGNAL_STATE:
			NX_APP_Message("MSG_CORE_SERVICE_EVT_SIGNAL_STATE!\n");
			nx_menuSatSettings_MsgCoreSignalState(pstContents, p2, p3);
			break;

		case MSG_APP_SETTING_TP_SETTING_END:
			NX_APP_Message("MSG_APP_SETTING_TP_SETTING_END!\n");
			eResult = nx_menuSatSettings_MsgAppTpSettingEnd(pstContents, p1);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuSatSettings_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CANCELED!\n");
			eResult = nx_menuSatSettings_MsgAppDlgCanceled(pstContents, p2);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				nx_menuSatSettings_CancelSetting(pstContents);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

