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
 * @file	  		nx_prism_menu_search_sat_tpset.c
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
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_FREQUENCY				0
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_POLARIZATION			eDxSAT_POLAR_AUTO
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_SYMBOLRATE			27500000
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_FEC					eDxSAT_CODERATE_AUTO
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_TRANSMISSION			eDxSAT_TRANS_DVBS
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_PSKMODE				eDxSAT_PSK_AUTO
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_PILOT					eDxSAT_PILOT_AUTO
#define	NX_MENU_SRCHSAT_TPSET_DEFAULT_ROLLOFF				eDxSAT_ROLL_035

#define	NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS					32

// Strength TEXT
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_X				75
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_Y				474
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_W				100
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_H				32

// Strength BAR
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_X				186
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_Y				488
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_W				539
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_H				3

// Strength Value
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_X				735
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_Y				474
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_W				60
#define	NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_H				32

// QUALITy TEXT
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_X				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_X
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_Y				(NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_W				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_W
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_H				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_H

// QUALITy BAR
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_X					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_X
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_Y					(NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_W					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_W
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_H					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_H

// QUALITy Value
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_X				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_X
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_Y				(NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_W				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_W
#define	NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_H				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_H

// BER TEXT
#define	NX_MENU_SRCHSAT_TPSET_BER_TEXT_X				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_X
#define	NX_MENU_SRCHSAT_TPSET_BER_TEXT_Y				(NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_BER_TEXT_W				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_W
#define	NX_MENU_SRCHSAT_TPSET_BER_TEXT_H				NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_H

// BER BAR
#define	NX_MENU_SRCHSAT_TPSET_BER_BAR_X					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_X
#define	NX_MENU_SRCHSAT_TPSET_BER_BAR_Y					(NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_BER_BAR_W					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_W
#define	NX_MENU_SRCHSAT_TPSET_BER_BAR_H					NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_H

// BER Value
#define	NX_MENU_SRCHSAT_TPSET_BER_VALUE_X				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_X
#define	NX_MENU_SRCHSAT_TPSET_BER_VALUE_Y				(NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_Y + NX_MENU_SRCHSAT_TPSET_BAR_ITEM_LS)
#define	NX_MENU_SRCHSAT_TPSET_BER_VALUE_W				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_W
#define	NX_MENU_SRCHSAT_TPSET_BER_VALUE_H				NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_H

// OK Button
#define	NX_MENU_SRCHSAT_TPSET_OK_BTN_X						170
#define	NX_MENU_SRCHSAT_TPSET_OK_BTN_Y						615
#define	NX_MENU_SRCHSAT_TPSET_OK_BTN_W						260
#define	NX_MENU_SRCHSAT_TPSET_OK_BTN_H						55

// CANCEL Button
#define	NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_X					440
#define	NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_Y					615
#define	NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_W					260
#define	NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_H					55

// Input Dialog Box
#define	NX_MENU_SRCHSAT_TPSET_FREQUENCY_INTPUT_LEN			5
#define	NX_MENU_SRCHSAT_TPSET_SYMBOLRATE_INTPUT_LEN			5
#define	NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_X				290
#define	NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_Y				266
#define	NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_W				700
#define	NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_H				188


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_TpSet_Frequency,
	eNxSettingsItem_TpSet_Polarization,
	eNxSettingsItem_TpSet_SymbolRate,
	eNxSettingsItem_TpSet_Transmission,
	eNxSettingsItem_TpSet_FEC,
	eNxSettingsItem_TpSet_Ok,
	eNxSettingsItem_TpSet_Cancel,
	eNxSettingsItem_TpSet_Max
} Nx_SettingsItem_TpSetting_t;


typedef struct tag_Nx_MenuTpSetting_contents
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
	DxAntennaType_e		eAntType;

	HINT32				lCurrAntUid;	// Antenna Uid
	HUINT32				ulCurrSatSlotIdx;
	DxAntInfo_t			stAntInfo;		// Current Antenna Info

	NX_TP_TuningInfo_t	stTuneInfo;		// Select/Input TP Info
	HBOOL				bTuneStateLock;

	HUINT8				szSignalStrength[NX_TEXT_SIZE_8];
	HUINT8				szSignalQuality[NX_TEXT_SIZE_8];
	HCHAR				szSignalBER[NX_TEXT_SIZE_8];
} Nx_MenuSatTpSetting_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSatTpSetting_Contents_t		s_stSatTPSettingsContent;
static Nx_PrismListDialog_Content_t			s_stPopupListSatTPSettingsContents;
static Nx_PrismDialogInput_Content_t		s_stDialogInputSatTPSettingsContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuTPSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTPSettings_DestroyListPopup(void);
static void 			nx_menuTPSettings_CreateListPopup(Nx_MenuSatTpSetting_Contents_t *pContents, HINT32 ulObjId);

static ONDK_Result_t	nx_menuTPSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTPSettings_DestroyDialogInput(void);
static void				nx_menuTPSettings_CreateDialogInput(Nx_MenuSatTpSetting_Contents_t *pContents, HINT32 lObjId);

static void				nx_menuTPSettings_StopTune(Nx_MenuSatTpSetting_Contents_t *pstContents);
static void				nx_menuTPSettings_TryTune(Nx_MenuSatTpSetting_Contents_t *pstContents);
static inline Nx_MenuSatTpSetting_Contents_t* nx_menuTPSettings_GetContents(void);
static inline HINT32	nx_menuTPSettings_GetDefaultFocus(void);
static HCHAR*			nx_menuTPSettings_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_GetMenuItemIdFormObjId(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuTPSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuTPSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuTPSettings_GetItemSupport(Nx_MenuSatTpSetting_Contents_t *pstContents, Nx_SettingsItem_TpSetting_t eNxSatSettingItem);
static void				nx_menuTPSettings_FindNextFocus(Nx_MenuSatTpSetting_Contents_t *pstContents, HBOOL bUp);
static ONDK_Result_t	nx_menuTPSettings_ChangeFocus(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lKey);

static ONDK_Result_t	nx_menuTPSettings_RefreshSubItem(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static void				nx_menuTPSettings_RefreshAllSubItems(Nx_MenuSatTpSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuTPSettings_UpdateFEC(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_UpdateTransMisstion(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_UpdateSymbolRate(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_UpdatePolarization(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_UpdateFrequency(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_UpdateSubItem(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);

static ONDK_Result_t	nx_menuTPSettings_SaveSetting(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_CancelSetting(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_ChangeSubitemToKeyInput(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_ChangeSubitemToButton(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_ChageSignalInfoBarText(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulSignalStrength, HUINT32 ulSignalQuality);
static ONDK_Result_t	nx_menuTPSettings_ChangeItemEnableDisableMode(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable);
static ONDK_Result_t	nx_menuTPSettings_ChangedInputValue(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulInputValue);
static ONDK_Result_t	nx_menuTPSettings_ChangedSubItemValue(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuTPSettings_ResetItemFEC(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_AddItemFEC(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_AddItemTransmission(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_AddItemSymbolRate(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_AddItemPolarization(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_AddItemFrequency(Nx_MenuSatTpSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuTPSettings_MakeDefaultTpInfo(NX_TP_TuningInfo_t *pstTuneInfo);
static ONDK_Result_t	nx_menuTPSettings_InitTpSetting(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_InitSatSetting(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_InitAction(Nx_MenuSatTpSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuTPSettings_InitSettingsItems(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lAntennaType, HINT32 lSatSlotIdx, HINT32 lAntUid);

static ONDK_Result_t	nx_menuTPSettings_DrawDefaultUI(Nx_MenuSatTpSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuTPSettings_MsgGwmCreate(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuTPSettings_MsgGwmDestroy(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTPSettings_MsgGwmHapiMsg(Nx_MenuSatTpSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuTPSettings_MsgGwmClicked(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuTPSettings_MsgGwmKeyDown(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

#define _________Dialog_Input_Functions_________________________________________
static ONDK_Result_t	nx_menuTPSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_DialogInput_ProtoProc(&s_stDialogInputSatTPSettingsContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuTPSettings_DestroyDialogInput(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuTPSettings_DisplayDialogInput);
}


static void		nx_menuTPSettings_CreateDialogInput(Nx_MenuSatTpSetting_Contents_t *pContents, HINT32 lObjId)
{
	HUINT8			*pszMessage = NULL;
	HINT32			lItemIdx = -1;
	HUINT32			ulAttribute = 0, ulInputLen = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	Nx_SettingsLeafSubItem_t  *pstSubItem = NULL;

	/* Get subitem matched nObjId */
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pContents, lObjId);
	if ( pstSubItem == NULL )
	{
		NX_APP_Error("[%s:%d] Error!!! pstSubItem is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return;
	}

	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_TpSet_Frequency:
			pszMessage = ONDK_GetString(RES_MESG_4509_ID);
			ulInputLen = NX_MENU_SRCHSAT_TPSET_FREQUENCY_INTPUT_LEN;
			break;
		case eNxSettingsItem_TpSet_SymbolRate:
			pszMessage = ONDK_GetString(RES_MESG_4947_ID);
			ulInputLen = NX_MENU_SRCHSAT_TPSET_SYMBOLRATE_INTPUT_LEN;
			break;
		default:
			NX_APP_Error("[%s:%d] Error!!! This Object was not support input dialog. lItemIdx :[%d]\n", __FUNCTION__, __HxLINE__, lItemIdx);
			return;
	}

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_X, NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_Y,
						NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_W, NX_MENU_SRCHSAT_TPSET_DIALOG_INPUT_H);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_DialogInput_SetDialog (&s_stDialogInputSatTPSettingsContents,
							ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, ulAttribute, COL(C_T_Whistle_100), pszMessage, pstSubItem->lSubItemValue, ulInputLen);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuTPSettings_DisplayDialogInput", APP_DEFAULT_PRIORITY, nx_menuTPSettings_DisplayDialogInput, 0, 0, 0, 0);
}


#define _________PopUp_List_Functions____________________________________________

static ONDK_Result_t	nx_menuTPSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSatTPSettingsContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuTPSettings_DestroyListPopup(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuTPSettings_DisplayListPopup);
}


static void		nx_menuTPSettings_CreateListPopup(Nx_MenuSatTpSetting_Contents_t *pContents, HINT32 ulObjId)
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
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSatTPSettingsContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuTPSettings_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuTPSettings_DisplayListPopup, 0, 0, 0, 0);
}


#define _________Normal_Functions____________________________________________

static void		nx_menuTPSettings_StopTune(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	NX_Builder_StopScan(NX_SCANTYPE_TUNEONLY);

	NX_COMMON_NOT_USED_PARAM(pstContents);
}


static void		nx_menuTPSettings_TryTune(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	NX_SCANPARAMS_t		stScanParam;
	NX_SCANOPTIONS_t	stScanOption;
	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;

	if (HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
	{
		NX_APP_Error("[%s:%d] Antenna Type is NONE\n", __FUNCTION__, __LINE__);
		return;
	}

	nx_menuTPSettings_StopTune(pstContents);

	HxSTD_memset(&stScanParam, 0x00, sizeof(NX_SCANPARAMS_t));
	HxSTD_memset(&stScanOption, 0x00, sizeof(NX_SCANOPTIONS_t));

	NX_SEARCH_GetSearchOption(&stScanOption);

	pstTuneInfo = &(pstContents->stTuneInfo);

	NX_ANTENNA_SetInfo(pstContents->stAntInfo.uid, &(pstContents->stAntInfo));
	NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam(eDxDELIVERY_TYPE_SAT, &(pstContents->stAntInfo), pstTuneInfo, &stScanParam);
	NX_Builder_StartScan(NX_SCANTYPE_TUNEONLY, &stScanOption, NX_SEARCH_MAX_TP_TUNEONLY, &stScanParam);
}


static inline Nx_MenuSatTpSetting_Contents_t* nx_menuTPSettings_GetContents(void)
{
	return	&s_stSatTPSettingsContent;
}


static inline HINT32	nx_menuTPSettings_GetDefaultFocus(void)
{
	return	eNxSettingsItem_TpSet_Frequency ;
}


static HCHAR*	nx_menuTPSettings_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_TpSet_Frequency :
			return	RES_FREQUENCY_ID;

		case eNxSettingsItem_TpSet_Polarization:
			return	RES_POLARIZATION_ID;

		case eNxSettingsItem_TpSet_SymbolRate:
			return	RES_SYMBOL_RATE_ID;

		case eNxSettingsItem_TpSet_Transmission:
			return	RES_TRANSMISSION_ID;

		case eNxSettingsItem_TpSet_FEC:
			return	RES_FEC_ID;

		default:
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuTPSettings_GetMenuItemIdFormObjId(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0, lFindObjId = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lItemId = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_TpSet_Frequency :
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;

		case eNxSettingsItem_TpSet_Polarization:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_POLARIZATION;
			break;

		case eNxSettingsItem_TpSet_SymbolRate:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_SYMBOLRATE;
			break;

		case eNxSettingsItem_TpSet_Transmission:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_TRANSMISSION;
			break;

		case eNxSettingsItem_TpSet_FEC:
			lFindObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Transmission);
			pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lFindObjId);
			if (pstSubItem == NULL)
			{
				NX_APP_Error("[%s:%d] pstSubItem is NULL - lFindObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lFindObjId);
				*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVB;
				break;
			}

			if (pstSubItem->lSubItemValue == eNxPrismSAT_PSK_DVBS)
			{
				*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVB;
			}
			else
			{
				*pulMenuItemId = NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVBS2;
			}
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("[%s:%d] Invalid Item Id : [0x%x][%d]\n", __FUNCTION__, __LINE__, lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuTPSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SatelliteTPSetting);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_TpSet_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuTPSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_TpSet_Max))
	{
		return 0;
	}

	return	(eNxSetting_SatelliteTPSetting | lItemId);
}


static HBOOL	nx_menuTPSettings_GetItemSupport(Nx_MenuSatTpSetting_Contents_t *pstContents, Nx_SettingsItem_TpSetting_t eNxSatSettingItem)
{
	HBOOL	bSupport = TRUE;

	switch (eNxSatSettingItem)
	{
		case eNxSettingsItem_TpSet_Frequency:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_TpSet_Polarization:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_TpSet_SymbolRate:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_TpSet_Transmission:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_TpSet_FEC:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	bSupport;
}


static void		nx_menuTPSettings_FindNextFocus(Nx_MenuSatTpSetting_Contents_t *pstContents, HBOOL bUp)
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

		ONDK_GWM_Obj_SetFocus(pstSubItem->ulId);
		break;
	}
}


static ONDK_Result_t	nx_menuTPSettings_ChangeFocus(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lKey)
{
	HINT32			lObjId = 0, lNextObjId = 0, lItemIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lKey)
	{
		case KEY_ARROWUP:
			if (lItemIdx == eNxSettingsItem_TpSet_Frequency)
			{
				lNextObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_TpSet_Ok) || (lItemIdx == eNxSettingsItem_TpSet_Cancel))
			{
				nx_menuTPSettings_FindNextFocus(pstContents, TRUE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case KEY_ARROWDOWN:
			if (lItemIdx == eNxSettingsItem_TpSet_FEC)
			{
				lNextObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_TpSet_Ok) || (lItemIdx == eNxSettingsItem_TpSet_Cancel))
			{
				nx_menuTPSettings_FindNextFocus(pstContents, FALSE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			if (lItemIdx == eNxSettingsItem_TpSet_Ok)
			{
				lNextObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Cancel);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;

			}
			else if (lItemIdx == eNxSettingsItem_TpSet_Cancel)
			{
				lNextObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_RefreshSubItem(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
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
		NX_APP_Error("Error!!! pstSubItem == NULL - lObjId(0x%x)!! \n", lObjId);
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&pstSubItem->szMainItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_AppContents_ClearLeafSubItem(pstSubItem);

	/* Get Main Text ID */
	pcStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(lObjId);
	if (pcStrId != NULL)
	{
		HxSTD_sprintf((char*)pstSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pcStrId));
	}

	/* Convert ObjId to MenuItem Id. */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuId);

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems (ulMenuId, &ulItems);
	if ( (eResult != ONDK_RET_OK) || (ulItems == 0) )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() - ulMenuId(0x%x)!! \n", ulMenuId);
		return	eResult;
	}

	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_TpSet_Frequency :
			lSubItem = 0;
			break;

		case eNxSettingsItem_TpSet_Polarization:
			lSubItem = 0;
			break;

		case eNxSettingsItem_TpSet_SymbolRate:
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
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuId(0x%x)!! \n", ulMenuId);
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


static void		nx_menuTPSettings_RefreshAllSubItems(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HINT32			i = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	for (i = eNxSettingsItem_TpSet_Frequency; i < eNxSettingsItem_TpSet_Ok; i++)
	{
		eResult = nx_menuTPSettings_RefreshSubItem(pstContents, nx_menuTPSettings_ConvertItemIdxToObjId(i));
		if (eResult != ONDK_RET_OK)
		{
			NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		}
	}
}


static ONDK_Result_t	nx_menuTPSettings_UpdateFEC(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	pstContents->stTuneInfo.sat.eFecCodeRate = (DxSat_CodeRate_e)pstSubItem->lSubItemValue;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_UpdateTransMisstion(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (pstSubItem->lSubItemValue)
	{
		case eNxPrismSAT_PSK_DVBS:
			pstContents->stTuneInfo.sat.eTransSpec = eDxSAT_TRANS_DVBS;
			pstContents->stTuneInfo.sat.ePskMod = eDxSAT_PSK_AUTO;
			break;
		case eNxPrismSAT_PSK_DVBS2_QPSK:
			pstContents->stTuneInfo.sat.eTransSpec = eDxSAT_TRANS_DVBS2;
			pstContents->stTuneInfo.sat.ePskMod = eDxSAT_PSK_QPSK;
			break;
		case eNxPrismSAT_PSK_DVBS2_8PSK:
			pstContents->stTuneInfo.sat.eTransSpec = eDxSAT_TRANS_DVBS2;
			pstContents->stTuneInfo.sat.ePskMod = eDxSAT_PSK_8PSK;
			break;
	}

	nx_menuTPSettings_ResetItemFEC(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_UpdateSymbolRate(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	pstContents->stTuneInfo.sat.ulSymbolRate = (pstSubItem->lSubItemValue * NX_COM_CH_UNIT_MHZ);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_UpdatePolarization(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	pstContents->stTuneInfo.sat.ePolarization = (DxSat_Polarization_e)pstSubItem->lSubItemValue;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_UpdateFrequency(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(lObjId);

	// Not to do - Input Value

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_UpdateSubItem(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_TpSet_Frequency :
			nx_menuTPSettings_UpdateFrequency(pstContents, lObjId);
			break;

		case eNxSettingsItem_TpSet_Polarization:
			eResult = nx_menuTPSettings_UpdatePolarization(pstContents, lObjId);
			break;

		case eNxSettingsItem_TpSet_SymbolRate:
			eResult = nx_menuTPSettings_UpdateSymbolRate(pstContents, lObjId);
			break;

		case eNxSettingsItem_TpSet_Transmission:
			eResult = nx_menuTPSettings_UpdateTransMisstion(pstContents, lObjId);
			break;

		case eNxSettingsItem_TpSet_FEC:
			eResult = nx_menuTPSettings_UpdateFEC(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	if (eResult == ONDK_RET_OK)
	{
		nx_menuTPSettings_TryTune(pstContents);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_SaveSetting(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	Nx_TpSelectInfo_t		stTpInfo;

	HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
	stTpInfo.uid = pstContents->lCurrAntUid;
	stTpInfo.ucAvailable = TRUE;
	stTpInfo.ucUserTpDefined = TRUE;
	stTpInfo.eNxSearchTpType = eNxSat_SEARCH_TP_USERDEFINED;
	stTpInfo.ulTpInfoIndex = 0;
	stTpInfo.eDeliType = eDxDELIVERY_TYPE_SAT;

	HxSTD_memcpy(&(stTpInfo.stNxUserTpInfo), &(pstContents->stTuneInfo), sizeof(NX_TP_TuningInfo_t));
	stTpInfo.stNxUserTpInfo.sat.antuid = pstContents->stAntInfo.uid;
	HxSTD_StrNCpy(stTpInfo.stNxUserTpInfo.sat.satType, pstContents->stAntInfo.satType, DxANTINFO_NAME_LEN);
	NX_ANTENNA_SetUserTPInfo(pstContents->ulCurrSatSlotIdx, &stTpInfo);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_CancelSetting(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_ChageSignalInfoBarText(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulSignalStrength, HUINT32 ulSignalQuality)
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


static ONDK_Result_t	nx_menuTPSettings_ChangeItemEnableDisableMode(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuTPSettings_ConvertItemIdxToObjId(lItemIdx);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	if (bEnable == TRUE)
	{
		ONDK_GWM_Obj_EnableObject(lObjId);
	}
	else
	{
		ONDK_GWM_Obj_DisableObject(lObjId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_ChangedInputValue(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulInputValue)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (lItemIdx)
	{
		case eNxSettingsItem_TpSet_Frequency:
			pstContents->stTuneInfo.sat.ulFrequency = (ulInputValue * NX_COM_CH_UNIT_MHZ);
			pstSubItem->lSubItemValue = (HINT32)ulInputValue;
			HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
			HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%u", ulInputValue);
			break;

		case eNxSettingsItem_TpSet_SymbolRate:
			pstContents->stTuneInfo.sat.ulSymbolRate = (ulInputValue * NX_COM_CH_UNIT_MHZ);
			pstSubItem->lSubItemValue = (HINT32)ulInputValue;
			HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
			HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%u", ulInputValue);
			break;

		default:
			NX_APP_Error("[%s:%d] Error!! Not Supported itme lItemIdx:[%d]\n", __FUNCTION__, __LINE__, lItemIdx);
			return	ONDK_RET_FAIL;
	}

	nx_menuTPSettings_TryTune(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_ChangedSubItemValue(Nx_MenuSatTpSetting_Contents_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1;
	HUINT32 		ulMenuItemId = 0, ulPrevSelectedIdx = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);

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
		case eNxSettingsItem_TpSet_Frequency :
			pstSubItem->lSubItemValue = pstSubItem->ulIdxOfTextList;
			nx_menuTPSettings_UpdateSubItem(pstContents, lObjId);
			break;
		case eNxSettingsItem_TpSet_SymbolRate:
			if (pstSubItem->ulIdxOfTextList != (pstSubItem->ulNumOfItemTextList - 1))	// Normal Value
			{
				nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
				NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));
				nx_menuTPSettings_UpdateSubItem(pstContents, lObjId);
			}
			else	// if Select USER Define
			{
				// TODO: Remain prev value?????
				pstSubItem->lSubItemValue = 0;
				nx_menuTPSettings_CreateDialogInput(pstContents, lObjId);
			}
			break;
		case eNxSettingsItem_TpSet_Polarization:
		case eNxSettingsItem_TpSet_Transmission:
		case eNxSettingsItem_TpSet_FEC:
			/* Update subItem value */
			nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, lObjId, &ulMenuItemId);
			NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));
			nx_menuTPSettings_UpdateSubItem(pstContents, lObjId);
			break;
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_ResetItemFEC(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HCHAR				*pMainStrId = NULL;
	HUINT32 			ulItems = 0, ulMenuItemId = 0, ulListIdx = 0;
	HUINT8				**ppListOfItem = NULL;
	HINT32				lObjId = 0;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	*pstSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	lObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_FEC);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	// Clear the sub item.
	NX_PRISM_AppContents_ClearLeafSubItem(pstSubItem);

	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(lObjId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)pstSubItem->szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, pstSubItem->ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuId(0x%x)!! \n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	pstSubItem->ulNumOfItemTextList = ulItems;
	pstSubItem->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stAntInfo.info.diseqc.b22kTone, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulListIdx, &(pstSubItem->lSubItemValue));
	pstSubItem->ulIdxOfTextList = ulListIdx;
	HxSTD_StrCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[ulListIdx]);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_AddItemFEC(Nx_MenuSatTpSetting_Contents_t *pstContents)
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
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuTPSettings_GetItemSupport(pstContents, eNxSettingsItem_TpSet_FEC);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_TpSet_FEC(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_FEC);


	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuItemId(0x%x)!! \n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stTuneInfo.sat.eFecCodeRate, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stTuneInfo.sat.eFecCodeRate;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_AddItemTransmission(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HINT32				lValue = 0;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuTPSettings_GetItemSupport(pstContents, eNxSettingsItem_TpSet_Transmission);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_TpSet_Transmission(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Transmission);

	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuItemId(0x%x)!! \n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	if ((pstContents->stTuneInfo.sat.eTransSpec == eDxSAT_TRANS_DVBS2) && (pstContents->stTuneInfo.sat.ePskMod == eDxSAT_PSK_QPSK))
	{
		lValue = eNxPrismSAT_PSK_DVBS2_QPSK;
	}
	else if ((pstContents->stTuneInfo.sat.eTransSpec == eDxSAT_TRANS_DVBS2) && (pstContents->stTuneInfo.sat.ePskMod == eDxSAT_PSK_8PSK))
	{
		lValue = eNxPrismSAT_PSK_DVBS2_8PSK;
	}
	else
	{
		lValue = eNxPrismSAT_PSK_DVBS;
	}

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)lValue, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = lValue;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_AddItemSymbolRate(Nx_MenuSatTpSetting_Contents_t *pstContents)
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
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuTPSettings_GetItemSupport(pstContents, eNxSettingsItem_TpSet_SymbolRate);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_TpSet_SymbolRate(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_SymbolRate);

	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuItemId(0x%x)!! \n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->stTuneInfo.sat.ulSymbolRate/NX_COM_CH_UNIT_MHZ), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		// User Defined:
		ulListIdx = ulItems - 1;
	}

	stSubItem.lSubItemValue = (HINT32)(pstContents->stTuneInfo.sat.ulSymbolRate/NX_COM_CH_UNIT_MHZ);
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_AddItemPolarization(Nx_MenuSatTpSetting_Contents_t *pstContents)
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
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuTPSettings_GetItemSupport(pstContents, eNxSettingsItem_TpSet_Polarization);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_TpSet_Polarization(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Polarization);

	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuTPSettings_GetMenuItemIdFormObjId(pstContents, stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() - ulMenuItemId(0x%x)!! \n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->stTuneInfo.sat.ePolarization, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->stTuneInfo.sat.ePolarization;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx]);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_AddItemFrequency(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuTPSettings_GetItemSupport(pstContents, eNxSettingsItem_TpSet_Frequency);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_TpSet_Frequency(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Frequency );

	// Set Main String
	pMainStrId = nx_menuTPSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	// set button item
	stSubItem.lSubItemValue = pstContents->stTuneInfo.sat.ulFrequency;
	snprintf ((char *)stSubItem.szSubItemText, MAX_ELEMENT_TEXT_LENGTH, "%d", (int)pstContents->stTuneInfo.sat.ulFrequency);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_MakeDefaultTpInfo(NX_TP_TuningInfo_t *pstTuneInfo)
{
	if (pstTuneInfo == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! stTuneInfo is NULL\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_FAIL;
	}

	/* DVBS Parameters */
	pstTuneInfo->sat.ulFrequency = NX_MENU_SRCHSAT_TPSET_DEFAULT_FREQUENCY;
	pstTuneInfo->sat.ulSymbolRate = NX_MENU_SRCHSAT_TPSET_DEFAULT_SYMBOLRATE;
	pstTuneInfo->sat.eFecCodeRate = NX_MENU_SRCHSAT_TPSET_DEFAULT_FEC;
	pstTuneInfo->sat.ePolarization = NX_MENU_SRCHSAT_TPSET_DEFAULT_POLARIZATION;

	/* DVBS2 Parameters */
	pstTuneInfo->sat.eTransSpec = NX_MENU_SRCHSAT_TPSET_DEFAULT_TRANSMISSION;
	pstTuneInfo->sat.ePskMod = NX_MENU_SRCHSAT_TPSET_DEFAULT_PSKMODE;
	pstTuneInfo->sat.ePilot = NX_MENU_SRCHSAT_TPSET_DEFAULT_PILOT;
	pstTuneInfo->sat.eRollOff = NX_MENU_SRCHSAT_TPSET_DEFAULT_ROLLOFF;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_InitTpSetting(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	NX_TP_TuningInfo_t		stTuneInfo;

	if (pstContents == NULL)
	{
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(&stTuneInfo, 0x00, sizeof(NX_TP_TuningInfo_t));

	nx_menuTPSettings_MakeDefaultTpInfo(&stTuneInfo);
	HxSTD_StrCpy((char*)stTuneInfo.sat.satType, (char*)pstContents->stAntInfo.satType);
	stTuneInfo.sat.antuid = pstContents->stAntInfo.uid;

	HxSTD_MemCopy(&(pstContents->stTuneInfo), &stTuneInfo, sizeof(DxAntInfo_t));


	// TODO: Add Load User define TP?????

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_InitSatSetting(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	DxAntInfo_t		stAntInfo;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		return	ONDK_RET_INITFAIL;
	}

	HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));

	hError = NX_ANTENNA_GetInfo(pstContents->lCurrAntUid, &stAntInfo);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] Error!!! NX_ANTENNA_GetInfo() lCurrAntUid :[%d]\n", __FUNCTION__, __HxLINE__, pstContents->lCurrAntUid);
		return	ONDK_RET_INITFAIL;
	}
	stAntInfo.antType = pstContents->eAntType;
	HxSTD_memcpy(&(pstContents->stAntInfo), &stAntInfo, sizeof(DxAntInfo_t));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_InitAction(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	// TODO: Add
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_InitSettingsItems(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lAntennaType, HINT32 lSatSlotIdx, HINT32 lAntUid)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// for saving user specific settings
	pstContents->eAntType = (DxAntennaType_e)lAntennaType;
	pstContents->lCurrAntUid = lAntUid;
	pstContents->ulCurrSatSlotIdx = lSatSlotIdx;

	// tune state
	pstContents->bTuneStateLock = FALSE;

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

	// Init Item Value
	nx_menuTPSettings_InitSatSetting(pstContents);
	nx_menuTPSettings_InitTpSetting(pstContents);

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_TRANSPONDER_SETTING_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->ucTitle[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';

	// Default Function
	pstContents->ulCurFocus = nx_menuTPSettings_GetDefaultFocus();

	nx_menuTPSettings_AddItemFrequency(pstContents);
	nx_menuTPSettings_AddItemPolarization(pstContents);
	nx_menuTPSettings_AddItemSymbolRate(pstContents);
	nx_menuTPSettings_AddItemTransmission(pstContents);
	nx_menuTPSettings_AddItemFEC(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_DrawDefaultUI(Nx_MenuSatTpSetting_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	ONDK_Rect_t		stRectBar = { 0, 0, 0, 0 };
	ONDK_Rect_t		stRectvalue = { 0, 0, 0, 0 };

	NX_PRISM_COMP_Settings_DrawBackgroundPopUp();
	NX_PRISM_COMP_Settings_DrawTitlePopUp(pstContents->ucTitle);
	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow);

	// Signal Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_X, NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_Y, NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_W, NX_MENU_SRCHSAT_TPSET_STRENGTH_TEXT_H);
	stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_X, NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_Y, NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_W, NX_MENU_SRCHSAT_TPSET_STRENGTH_BAR_H);
	stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_X, NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_Y, NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_W, NX_MENU_SRCHSAT_TPSET_STRENGTH_VALUE_H);
	HxSTD_snprintf((char *)pstContents->szSignalStrength, sizeof(pstContents->szSignalStrength), "0%%");
	NX_PRISM_COMP_Settings_CreateDrawStrengthBar(&stRect, &stRectBar, &stRectvalue, (HCHAR*)pstContents->szSignalStrength);

	// Quality Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_X, NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_Y, NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_W, NX_MENU_SRCHSAT_TPSET_QUALITY_TEXT_H);
	stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_X, NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_Y, NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_W, NX_MENU_SRCHSAT_TPSET_QUALITY_BAR_H);
	stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_X, NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_Y, NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_W, NX_MENU_SRCHSAT_TPSET_QUALITY_VALUE_H);
	HxSTD_snprintf((char *)pstContents->szSignalQuality, sizeof(pstContents->szSignalQuality), "0%%");
	NX_PRISM_COMP_Settings_CreateDrawQualitytBar(&stRect, &stRectBar, &stRectvalue, (HCHAR*)pstContents->szSignalQuality);

	if (pstContents->bDispBER == TRUE)
	{
		// BER Info
		stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_BER_TEXT_X, NX_MENU_SRCHSAT_TPSET_BER_TEXT_Y, NX_MENU_SRCHSAT_TPSET_BER_TEXT_W, NX_MENU_SRCHSAT_TPSET_BER_TEXT_H);
		stRectBar = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_BER_BAR_X, NX_MENU_SRCHSAT_TPSET_BER_BAR_Y, NX_MENU_SRCHSAT_TPSET_BER_BAR_W, NX_MENU_SRCHSAT_TPSET_BER_BAR_H);
		stRectvalue = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_BER_VALUE_X, NX_MENU_SRCHSAT_TPSET_BER_VALUE_Y, NX_MENU_SRCHSAT_TPSET_BER_VALUE_W, NX_MENU_SRCHSAT_TPSET_BER_VALUE_H);
		HxSTD_snprintf((char *)pstContents->szSignalBER, sizeof(pstContents->szSignalBER), "0%%");
		NX_PRISM_COMP_Settings_CreateDrawBERBar(&stRect, &stRectBar, &stRectvalue, (HCHAR*)pstContents->szSignalBER);
	}

	// Button
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_OK_BTN_X, NX_MENU_SRCHSAT_TPSET_OK_BTN_Y, NX_MENU_SRCHSAT_TPSET_OK_BTN_W, NX_MENU_SRCHSAT_TPSET_OK_BTN_H);
	lObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_X, NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_Y, NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_W, NX_MENU_SRCHSAT_TPSET_CANCEL_BTN_H);
	lObjId = nx_menuTPSettings_ConvertItemIdxToObjId(eNxSettingsItem_TpSet_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_MsgGwmCreate(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Action
	nx_menuTPSettings_InitAction(pstContents);

	// Init Menu Item
	nx_menuTPSettings_InitSettingsItems(pstContents, p1, p2, p3);

	// Draw Menu Item
	nx_menuTPSettings_DrawDefaultUI(pstContents);

	// Try Tune
	NX_Builder_AddNotiSignalInfo();
	nx_menuTPSettings_TryTune(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_MsgGwmDestroy(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	nx_menuTPSettings_StopTune(pstContents);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSatTpSetting_Contents_t));
	nx_menuTPSettings_DestroyListPopup();
	nx_menuTPSettings_DestroyDialogInput();
	NX_Builder_ReleaseNotiSignalInfo();

	NX_PRISM_LIVE_ServiceStartByLastService();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuTPSettings_MsgGwmHapiMsg(Nx_MenuSatTpSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuTPSettings_MsgGwmClicked(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;

	lItemIdx = nx_menuTPSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_TpSet_Frequency :
			nx_menuTPSettings_CreateDialogInput(pstContents, lObjId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_TpSet_Polarization:
		case eNxSettingsItem_TpSet_SymbolRate:
		case eNxSettingsItem_TpSet_Transmission:
		case eNxSettingsItem_TpSet_FEC:
			nx_menuTPSettings_CreateListPopup(pstContents, lObjId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_TpSet_Ok:
			nx_menuTPSettings_SaveSetting(pstContents);
			ONDK_GWM_PostMessage (NULL, MSG_APP_SETTING_TP_SETTING_END, (Handle_t)NULL, TRUE, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_TpSet_Cancel:
			nx_menuTPSettings_CancelSetting(pstContents);
			ONDK_GWM_PostMessage (NULL, MSG_APP_SETTING_TP_SETTING_END, (Handle_t)NULL, FALSE, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		default:
			return	ONDK_RET_OK;
	}
}


static ONDK_Result_t	nx_menuTPSettings_MsgGwmKeyDown(Nx_MenuSatTpSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		//case KEY_ARROWLEFT:
			ONDK_GWM_APP_Destroy(0);
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
			break;

		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			return	nx_menuTPSettings_ChangeFocus(pstContents, lKeyId);

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Search_SatTransponderSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSatTpSetting_Contents_t *pstContents = NULL;

	pstContents = nx_menuTPSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			// p1 : AntennaType, p2 : Antenna Slot index, p3 : Antenna uid
			eResult = nx_menuTPSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuTPSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list & Action */
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuTPSettings_MsgGwmClicked(pstContents, p1);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_SCROLLRIGHT!\n");
			eResult = nx_menuTPSettings_MsgGwmClicked(pstContents, p1);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuTPSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuTPSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuTPSettings_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_INPUT_VALUE_CHANGED!\n");
			nx_menuTPSettings_ChangedInputValue(pstContents, p1);
			break;

		case MSG_CORE_SERVICE_EVT_SIGNAL_STATE:
			NX_APP_Message("MSG_CORE_SERVICE_EVT_SIGNAL_STATE!\n");
			nx_menuTPSettings_ChageSignalInfoBarText(pstContents, p2, p3);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

