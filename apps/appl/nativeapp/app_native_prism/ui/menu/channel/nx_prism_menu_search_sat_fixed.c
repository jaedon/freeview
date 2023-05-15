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
 * @file	  		nx_prism_menu_search_sat_fixed.c
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

#define	NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI					CONFIG_APUI_SAT_ANTENNA_MAX_SLOT
#define	NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE					4

/* Antenna Info */
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_X							60
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_Y							198
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_W							178
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_H							270
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_TAB						12

// Antenna Center
#define	NX_MENU_SRCHSAT_FIXED_ANTENNA_CENTER_X					360

// Antenna Image
#define	NX_MENU_SRCHSAT_FIXED_ANT_IMG_X							49
#define	NX_MENU_SRCHSAT_FIXED_ANT_IMG_Y							168
#define	NX_MENU_SRCHSAT_FIXED_ANT_IMG_W							80
#define	NX_MENU_SRCHSAT_FIXED_ANT_IMG_H							68

// Antenna Info Text - 상대 좌료
#define	NX_MENU_SRCHSAT_FIXED_ANT_TEXT_X						10	// Antenna Info TEXT
#define	NX_MENU_SRCHSAT_FIXED_ANT_TEXT_Y						22
#define	NX_MENU_SRCHSAT_FIXED_ANT_TEXT_W						158
#define	NX_MENU_SRCHSAT_FIXED_ANT_TEXT_H						22
#define	NX_MENU_SRCHSAT_FIXED_ANT_TEXT_LS						30

/* Transponder Info */
#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_CENTER_X				NX_MENU_SRCHSAT_FIXED_ANTENNA_CENTER_X

#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X						NX_MENU_SRCHSAT_FIXED_ANTENNA_X
#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_Y						473
#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W						NX_MENU_SRCHSAT_FIXED_ANTENNA_W
#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_H						74
#define	NX_MENU_SRCHSAT_FIXED_TRANSPONDER_TAB					NX_MENU_SRCHSAT_FIXED_ANTENNA_TAB

// TP Info Text - 상대 좌료
#define	NX_MENU_SRCHSAT_FIXED_TP_TEXT_X							10	// TP Info TEXT
#define	NX_MENU_SRCHSAT_FIXED_TP_TEXT_Y							14
#define	NX_MENU_SRCHSAT_FIXED_TP_TEXT_W							158
#define	NX_MENU_SRCHSAT_FIXED_TP_TEXT_H							20
#define	NX_MENU_SRCHSAT_FIXED_TP_TEXT_LS						30

// Search Button
#define	NX_MENU_SRCHSAT_FIXED_SEARCH_BT_X						305
#define	NX_MENU_SRCHSAT_FIXED_SEARCH_BT_Y						615
#define	NX_MENU_SRCHSAT_FIXED_SEARCH_BT_W						260
#define	NX_MENU_SRCHSAT_FIXED_SEARCH_BT_H						55

// Option
#define	NX_MENU_SRCHSAT_FIXED_OPT_X								820
#define	NX_MENU_SRCHSAT_FIXED_OPT_Y								674
#define	NX_MENU_SRCHSAT_FIXED_OPT_W								400
#define	NX_MENU_SRCHSAT_FIXED_OPT_H								30

#define	NX_MENU_SRCHSAT_FIXED_TPLIST_X							0
#define	NX_MENU_SRCHSAT_FIXED_TPLIST_Y							(NX_MENU_SRCHSAT_FIXED_ANTENNA_Y - 68)
#define	NX_MENU_SRCHSAT_FIXED_TPLIST_W							405
#define	NX_MENU_SRCHSAT_FIXED_TPLIST_H							540

#define	NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_X						NX_MENU_SRCHSAT_FIXED_ANTENNA_X
#define	NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_Y						NX_MENU_SRCHSAT_FIXED_ANTENNA_Y
#define	NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_W						26
#define	NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_H						30

#define	NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_X						NX_MENU_SRCHSAT_FIXED_ANTENNA_X
#define	NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_Y						NX_MENU_SRCHSAT_FIXED_ANTENNA_Y
#define	NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_W						26
#define	NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_H						30


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_SatFixedSwitchType = 0,
	eNxSettingsItem_SatFixedANT1,
	eNxSettingsItem_SatFixedANT2,
	eNxSettingsItem_SatFixedANT3,
	eNxSettingsItem_SatFixedANT4,
	eNxSettingsItem_SatFixedTP1,
	eNxSettingsItem_SatFixedTP2,
	eNxSettingsItem_SatFixedTP3,
	eNxSettingsItem_SatFixedTP4,
	eNxSettingsItem_SatFixedSearchBtn,
	eNxSettingsItem_SatFixedOpt,
	eNxSettingsItem_SatFixedArrowLeft,
	eNxSettingsItem_SatFixedArrowRight,
	eNxSettingsItem_SatFixed_MAX
} NxSettingsItem_SearchSatFixed_t;

typedef enum {
	eNxSettingsItem_AntInfo_LNB = 0,
	eNxSettingsItem_AntInfo_22kHTone,
	eNxSettingsItem_AntInfo_Diseqc,
	eNxSettingsItem_AntInfo_MAX
} NxSettingsItem_SearchSatAntInfo_t;

typedef enum {
	eNxSettingsItem_DialogType_None = 0,
	eNxSettingsItem_DialogType_SwitchType,
	eNxSettingsItem_DialogType_AntennaType,
} NxSettingsItem_DialogType_t;

typedef struct tag_Nx_MenuSearchSat_contents
{
	// Base Contents :
	HUINT32				ulCurFocus;
	HUINT32				ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	// 여기서부터 Base Contents 이외 사항
	HUINT8				ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HBOOL				bPageOver;

	// Antenna Type
	NX_SAT_AntennaType_e	eAntType, eNewAntType;

	// Antena Informations
	HUINT32				ulNumAnt;
	HINT32				alAntIdxArray[NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI];
	DxAntInfo_t			astAntInfo[NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI];

	// TP Information
	Nx_TpSelectInfo_t	astTpSelectInfo[NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI];

	// Balloon Pop-up
	HINT32				ulListItemNum;
	HUINT8				**pszListText;

	HINT32				lPrevCurIdx;

	HINT32				bSupportOpt;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

} Nx_MenuSearchSat_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSearchSat_Contents_t			s_stSearchSatFixed_Contents;
static NXUI_DialogInstance_t				s_stSearchSatFixed_DialogInstance;
static Nx_PrismListDialog_Content_t			s_stPopupListSearchSatFixedConts;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t	*nx_menuSearchChannel_Fixed_GetDialogPopupContents(void);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(Nx_MenuSearchSat_Contents_t *pstContents, NxSettingsItem_DialogType_t eDialogType, HUINT8 *szStrID);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog(void);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DestroyListPopup(void);
static void 			nx_menuSearchChannel_Fixed_CreateListPopup(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 ulObjId);
static void				nx_menuSearchChannel_Fixed_CreateTPListPopup(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 ulObjId);

static Nx_MenuSearchSat_Contents_t*	nx_menuSearchChannel_Fixed_GetContents(void);

static HCHAR*			nx_menuSearchChannel_Fixed_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(HINT32 lItemId);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_SetAntennaType(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_SetTpValue(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lTpSlotIdx, NX_SAT_SearchTpType_t eTpType);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckFixedAntennaType(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckSCDAntennaType(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckMotorAntennaType(Nx_MenuSearchSat_Contents_t *pstContents);
static HBOOL			nx_menuSearchChannel_Fixed_CheckTPSettingState(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangeAntennaType(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntennType);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaNameString(DxAntInfo_t *pstAntInfo, HUINT8 *szString);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaLnbFreqString(DxAntInfo_t *pstAntInfo, HUINT8 *szString);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntenna22KToneString(DxAntInfo_t *pstAntInfo, HUINT8 *szString);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaDiseqcString(DxAntInfo_t *pstAntInfo, HUINT8 *szString);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaTpList(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSlotIdx, HINT32 *pnListNum, HUINT8 ***ppszTpList);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_FreeAntennaTpList(HINT32 lTPListItemNum, HUINT8 **pszListText);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangeFocus(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lKey);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateSwitchType(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DeleteAllAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_ResetUserTPInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSlotIdx);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReOrderUserTPInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntIdx);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReOrderAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntIdx);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntUid, HBOOL bAddAntenna);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateSubItem(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangedSubItemValue(Nx_MenuSearchSat_Contents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateEnableDisableAntennaSlot(Nx_MenuSearchSat_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_AddItemSwitchType(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_LoadAntennaInfos(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_LoadItemValue(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_InitSettingsItems(Nx_MenuSearchSat_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawAntennaInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawTpInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawOption(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawSearchButton(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawLnbOnlyAntenna(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawDiseqcAntenna(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawDefaultUI(Nx_MenuSearchSat_Contents_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReCreateWindow(Nx_MenuSearchSat_Contents_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmCreate(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSearchChannel_Fixed_MsgGwmDestroy(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmHapiMsg(Nx_MenuSearchSat_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmKeyDown(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmClicked(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmTimer(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppTpSettingEnd(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSaveFlag);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppDlgClicked(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppDlgCanceled(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuSearchChannel_Fixed_GetDialogPopupContents(void)
{
	return &s_stSearchSatFixed_DialogInstance;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DialogProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuSearchChannel_Fixed_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(Nx_MenuSearchSat_Contents_t *pstContents, NxSettingsItem_DialogType_t eDialogType, HUINT8 *szStrID)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuSearchChannel_Fixed_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eDialogType;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)(HINT32)ONDK_GetString((HINT8*)szStrID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Fixed_DialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuSearchChannel_Fixed_DialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog (void)
{
	ONDK_GWM_APP_Destroy(nx_menuSearchChannel_Fixed_DialogProc);

	return	ONDK_RET_OK;
}


#define _________PopUp_List_Functions____________________________________________

static ONDK_Result_t	nx_menuSearchChannel_Fixed_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSearchSatFixedConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuSearchChannel_Fixed_DisplayListPopup);
}


static void		nx_menuSearchChannel_Fixed_CreateListPopup(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 ulObjId)
{
	HUINT32			ulTextListNum = 0, ulIndex = 0;
	HUINT8			**pptextList;
	ONDK_Rect_t		stRect;
	Nx_SettingsLeafSubItem_t  *psubItem;

	/* Get subitem matched nObjId */
	psubItem = NX_PRISM_AppContents_GetSubitemByObjId ((void *)pstContents, ulObjId);
	if ( psubItem == NULL )
	{
		return;
	}

	pptextList = psubItem->szItemTextList;
	ulTextListNum = psubItem->ulNumOfItemTextList;
	ulIndex	= psubItem->ulIdxOfTextList;

	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SUBITEM_POPUP_X, NX_PRISM_UI_MENU_SUBITEM_POPUP_Y, NX_PRISM_UI_MENU_SUBITEM_POPUP_WIDTH, NX_PRISM_UI_MENU_SUBITEM_POPUP_HEIGHT);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSearchSatFixedConts, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											ulTextListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, ulIndex, (HUINT32)pptextList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Fixed_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuSearchChannel_Fixed_DisplayListPopup, 0, 0, 0, 0);
}


static void		nx_menuSearchChannel_Fixed_CreateTPListPopup(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32				lItemId = -1, lTpSlotIdx = -1, lCurTPIndex = -1;
	HINT32				lTPListNum = 0;
	HUINT8				**pszTpList = NULL;
	ONDK_Rect_t			stRect;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;


	lItemId = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	if (pstContents->eAntType == eNxSat_AntType_LNB_Only)
	{
		stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_CENTER_X + NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W, NX_MENU_SRCHSAT_FIXED_TPLIST_Y,
							NX_MENU_SRCHSAT_FIXED_TPLIST_W, NX_MENU_SRCHSAT_FIXED_TPLIST_H);
	}
	else
	{
		switch (lItemId)
		{
			case eNxSettingsItem_SatFixedTP1:
				stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X, NX_MENU_SRCHSAT_FIXED_TPLIST_Y, NX_MENU_SRCHSAT_FIXED_TPLIST_W, NX_MENU_SRCHSAT_FIXED_TPLIST_H);
				break;
			case eNxSettingsItem_SatFixedTP2:
				stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X, NX_MENU_SRCHSAT_FIXED_TPLIST_Y, NX_MENU_SRCHSAT_FIXED_TPLIST_W, NX_MENU_SRCHSAT_FIXED_TPLIST_H);
				stRect.x = (NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W + NX_MENU_SRCHSAT_FIXED_TRANSPONDER_TAB) * 1;
				break;
			case eNxSettingsItem_SatFixedTP3:
				stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X, NX_MENU_SRCHSAT_FIXED_TPLIST_Y, NX_MENU_SRCHSAT_FIXED_TPLIST_W, NX_MENU_SRCHSAT_FIXED_TPLIST_H);
				stRect.x = (NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W + NX_MENU_SRCHSAT_FIXED_TRANSPONDER_TAB) * 2;
				break;
			case eNxSettingsItem_SatFixedTP4:
				stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X, NX_MENU_SRCHSAT_FIXED_TPLIST_Y, NX_MENU_SRCHSAT_FIXED_TPLIST_W, NX_MENU_SRCHSAT_FIXED_TPLIST_H);
				stRect.x = (NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W + NX_MENU_SRCHSAT_FIXED_TRANSPONDER_TAB) * 3;
				break;
			default:
				NX_APP_Debug("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
				return;
		}
	}

	lTpSlotIdx = (pstContents->bPageOver) ? (lItemId - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemId - eNxSettingsItem_SatFixedTP1) ;

	if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
	{
		NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
		return;
	}

	lTPListNum = 0;
	pszTpList = NULL;
	eResult = nx_menuSearchChannel_Fixed_GetAntennaTpList(pstContents, lTpSlotIdx, &lTPListNum, &pszTpList);
	if ((eResult != ONDK_RET_OK) || (lTPListNum <= 0))
	{
		nx_menuSearchChannel_Fixed_FreeAntennaTpList(lTPListNum, pszTpList);
		pszTpList = NULL;
		return;
	}

	switch (pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType)
	{
		case eNxSat_SEARCH_TP_ALL:
			lCurTPIndex = eNxSat_SEARCH_TP_ALL;
			break;

		case eNxSat_SEARCH_TP_NONE:
			lCurTPIndex = eNxSat_SEARCH_TP_NONE;
			break;

		case eNxSat_SEARCH_TP_USERDEFINED:
			lCurTPIndex = eNxSat_SEARCH_TP_USERDEFINED;
			break;

		case eNxSat_SEARCH_TP_DEFAULT:
		default:
			lCurTPIndex = (HINT32)pstContents->astTpSelectInfo[lTpSlotIdx].ulTpInfoIndex + eNxSat_SEARCH_TP_DEFAULT;
			break;
	}

	nx_menuSearchChannel_Fixed_FreeAntennaTpList(pstContents->ulListItemNum, pstContents->pszListText);
	pstContents->pszListText = NULL;
	pstContents->ulListItemNum = (HUINT32)lTPListNum;
	pstContents->pszListText = pszTpList;

	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSearchSatFixedConts, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
											&stRect, NX_PRISM_UI_MENU_SUBITEM_POPUP_CELL_HEIGHT, eListDlg_BG_MenuSubItemApp,
											lTPListNum, NX_PRISM_COMP_POPUPLISTAPP_ITEM_MAX, lCurTPIndex, (HUINT32)pszTpList);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Fixed_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuSearchChannel_Fixed_DisplayListPopup, 0, 0, 0, 0);
}


#define _________Normal_Functions____________________________________________

static Nx_MenuSearchSat_Contents_t*	nx_menuSearchChannel_Fixed_GetContents(void)
{
	return	&s_stSearchSatFixed_Contents;
}


static void		nx_menuSearchChannel_Fixed_UpdateCurrentTime(Nx_MenuSearchSat_Contents_t *pstContents)
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


static HCHAR*	nx_menuSearchChannel_Fixed_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SatFixedSwitchType:
			return	RES_SWITCH_TYPE_ID;

		default:
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SatFixedSwitchType:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_FIXED_SWITCHTYPE;
			break;

		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SearchChannel);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SatFixed_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SatFixed_MAX))
	{
		return 0;
	}

	return	(eNxSetting_SearchChannel | lItemId);
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_SetAntennaType(Nx_MenuSearchSat_Contents_t *pstContents)
{
	if (pstContents->eNewAntType != pstContents->eAntType)
	{
		pstContents->eAntType = pstContents->eNewAntType;
		NX_CHANNEL_StopChannel();										// Stop Channel
		NX_ANTENNA_SetAntennaType(pstContents->eAntType);				// Change Antenna Type
		NX_CHANNEL_RemoveChannelListAll();								// Delete All Service
		ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_DELETE_SERVICE_ALL, (HINT32)NULL, 0, 0, 0);
		nx_menuSearchChannel_Fixed_DeleteAllAntennaInfo(pstContents);	// Delete All Antenna Info

		switch (pstContents->eNewAntType)
		{
			case eNxSat_AntType_LNB_Only:
			case eNxSat_AntType_Monoblock_LNB:
			case eNxSat_AntType_DiSEqC_1_0:
			case eNxSat_AntType_DiSEqC_2_0:
			case eNxSat_AntType_Multiswitch:
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatFixedAntna_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
										NX_PRISM_MENU_Search_SatFixedAntna_Proc, HANDLE_NULL, 0, 0, 0);
				break;
			case eNxSat_AntType_SCD:
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatScdAntena_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),	NX_PRISM_MENU_Search_SatScdAntena_Proc, HANDLE_NULL, 0, 0, 0);
				break;
			case eNxSat_AntType_Motor:
				ONDK_GWM_APP_Destroy(0);
				// TODO: [JHLEE] Add Function
				//ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatFixedAntna_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
				//						NX_PRISM_MENU_Search_SatFixedAntna_Proc, HANDLE_NULL, 0, 0, 0);
				break;
			default:
				NX_APP_Error("Error!!! Unknow Antenna Type:(%d) !!\n", pstContents->eNewAntType);
				break;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_SetTpValue(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lTpSlotIdx, NX_SAT_SearchTpType_t eTpType)
{
	HERROR		hError = ERR_FAIL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_APP_Debug("nx_menuSearchChannel_Fixed_SetTpValue() - lTpSlotIdx:[%d], eTpType:[%d] \n", lTpSlotIdx, eTpType);

	if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
	{
		NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
		return	ONDK_RET_FAIL;
	}

	switch (eTpType)
	{
		case eNxSat_SEARCH_TP_ALL:
			pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType = eNxSat_SEARCH_TP_ALL;
			break;

		case eNxSat_SEARCH_TP_NONE:
			pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType = eNxSat_SEARCH_TP_NONE;
			break;

		case eNxSat_SEARCH_TP_USERDEFINED:
			pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType = eNxSat_SEARCH_TP_USERDEFINED;
			break;

		case eNxSat_SEARCH_TP_DEFAULT:
		default:
			pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType = eNxSat_SEARCH_TP_DEFAULT;
			pstContents->astTpSelectInfo[lTpSlotIdx].ulTpInfoIndex = (HUINT32)eTpType - eNxSat_SEARCH_TP_DEFAULT;
			break;
	}

	pstContents->astTpSelectInfo[lTpSlotIdx].ucAvailable = TRUE;
	HxSTD_StrNCpy(pstContents->astTpSelectInfo[lTpSlotIdx].stNxUserTpInfo.sat.satType, pstContents->astAntInfo[lTpSlotIdx].satType, DxANTINFO_NAME_LEN);
	pstContents->astTpSelectInfo[lTpSlotIdx].stNxUserTpInfo.sat.antuid = pstContents->astAntInfo[lTpSlotIdx].uid;
	pstContents->astTpSelectInfo[lTpSlotIdx].uid = pstContents->astAntInfo[lTpSlotIdx].uid;

	hError = NX_ANTENNA_SetUserTPInfo(eNxSat_ANTENAA_SLOT1 + lTpSlotIdx, &(pstContents->astTpSelectInfo[lTpSlotIdx]));
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!! NX_ANTENNA_SetUserTPInfo() Fail!! \n");
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckFixedAntennaType(Nx_MenuSearchSat_Contents_t *pstContents)
{
	switch (pstContents->eAntType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_Monoblock_LNB:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
		case eNxSat_AntType_Multiswitch:
			NX_APP_Error("Error!!! Alreay Setted Type !!\n");
			break;
		default:
			pstContents->eNewAntType = eNxSat_AntType_LNB_Only;
			nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(pstContents, eNxSettingsItem_DialogType_AntennaType, (HUINT8*)RES_MESG_083_ID);
			break;
	}
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckSCDAntennaType(Nx_MenuSearchSat_Contents_t *pstContents)
{
	switch (pstContents->eAntType)
	{
		case eNxSat_AntType_SCD:
			NX_APP_Error("Error!!! Alreay Setted Type !!\n");
			break;
		default:
			pstContents->eNewAntType = eNxSat_AntType_SCD;
			nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(pstContents, eNxSettingsItem_DialogType_AntennaType, (HUINT8*)RES_MESG_083_ID);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_CheckMotorAntennaType(Nx_MenuSearchSat_Contents_t *pstContents)
{
	switch (pstContents->eAntType)
	{
		case eNxSat_AntType_Motor:
			NX_APP_Error("Error!!! Alreay Setted Type !!\n");
			break;
		default:
			pstContents->eNewAntType = eNxSat_AntType_Motor;
			nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(pstContents, eNxSettingsItem_DialogType_AntennaType, (HUINT8*)RES_MESG_083_ID);
			break;
	}

	return	ONDK_RET_OK;
}


static HBOOL	nx_menuSearchChannel_Fixed_CheckTPSettingState(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HBOOL		bTPState = FALSE;
	HUINT32		i = 0;

	bTPState = FALSE;
	for (i = 0; i < pstContents->ulNumAnt; i++)
	{
		if ((pstContents->astTpSelectInfo[i].ucAvailable == TRUE) && (pstContents->astTpSelectInfo[i].eNxSearchTpType != eNxSat_SEARCH_TP_NONE))
		{
			bTPState = TRUE;
		}
	}

	return	bTPState;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangeAntennaType(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntennType)
{
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;

	eOptItem = (eNxUiUtil_OptItem_t)lAntennType;

	switch (eOptItem)
	{
		case eNxUtil_OptItem_AntennaTypeFixed:
			nx_menuSearchChannel_Fixed_CheckFixedAntennaType(pstContents);
			break;
		case eNxUtil_OptItem_AntennaTypeScd:
			nx_menuSearchChannel_Fixed_CheckSCDAntennaType(pstContents);
			break;
		case eNxUtil_OptItem_AntennaTypeMotor:
			nx_menuSearchChannel_Fixed_CheckMotorAntennaType(pstContents);
			break;
		default:
			NX_APP_Error("Error!!! Invalid Option Item :(%d) !!\n", eOptItem);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaNameString(DxAntInfo_t *pstAntInfo, HUINT8 *szString)
{
	HCHAR	*szDispSatName = NULL;


	if (szString == NULL)
	{
		NX_APP_Error("Error!!! szString == NULL !! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if (pstAntInfo == NULL)
	{
		HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_SATELLITE_ID), DxANTINFO_NAME_LEN);
	}
	else
	{
		szDispSatName = NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(pstAntInfo->satName);
		HxSTD_StrNCpy((char*)szString, (char*)szDispSatName, DxANTINFO_NAME_LEN);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaLnbFreqString(DxAntInfo_t *pstAntInfo, HUINT8 *szString)
{
	if (szString == NULL)
	{
		NX_APP_Error("Error!! szString NULL \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if (pstAntInfo == NULL)
	{
		HxSTD_StrCpy((char*)szString, (char*)"-");
		return	ONDK_RET_OK;
	}

	if (pstAntInfo->antType == eDxANT_TYPE_LNB_ONLY)
	{
		if (pstAntInfo->info.lnb.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
		{
			HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_UNIVERSAL_ID), NX_TEXT_SIZE_64);
		}
		else
		{
			HxSTD_sprintf((char*)szString, (char*)"%d", pstAntInfo->info.lnb.ulLnbFreq);
		}
	}
	else if (pstAntInfo->antType == eDxANT_TYPE_DISEQC)
	{
		if (pstAntInfo->info.diseqc.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
		{
			HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_UNIVERSAL_ID), NX_TEXT_SIZE_64);
		}
		else
		{
			HxSTD_sprintf((char*)szString, (char*)"%d", pstAntInfo->info.diseqc.ulLnbFreq);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntenna22KToneString(DxAntInfo_t *pstAntInfo, HUINT8 *szString)
{
	if (szString == NULL)
	{
		NX_APP_Error("Error!! szString NULL \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if (pstAntInfo == NULL)
	{
		HxSTD_StrCpy((char*)szString, (char*)"-");
		return	ONDK_RET_OK;
	}

	if (pstAntInfo->antType == eDxANT_TYPE_DISEQC)
	{
		if (pstAntInfo->info.diseqc.ulLnbFreq == NX_COM_CH_UNIVERSAL_LNB_FREQ)
		{
			HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_AUTO_ID), NX_TEXT_SIZE_64);
		}
		else if (pstAntInfo->info.diseqc.b22kTone == TRUE)
		{
			HxSTD_sprintf((char*)szString, (char*)"22KHz %s", ONDK_GetString(RES_ON_ID));
		}
		else
		{
			HxSTD_sprintf((char*)szString, (char*)"22KHz %s", ONDK_GetString(RES_OFF_ID));
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaDiseqcString(DxAntInfo_t *pstAntInfo, HUINT8 *szString)
{
	if (szString == NULL)
	{
		NX_APP_Error("Error!! szString NULL \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if (pstAntInfo == NULL)
	{
		HxSTD_StrCpy((char*)szString, (char*)"-");
		return	ONDK_RET_OK;
	}

	if (pstAntInfo->antType == eDxANT_TYPE_DISEQC)
	{
		if (pstAntInfo->info.diseqc.diseqcInput == eDxDISEQC_INPUT_A)
		{
			HxSTD_sprintf((char*)szString, (char*)"%s A", ONDK_GetString(RES_DISEQC_ID));
		}
		else if (pstAntInfo->info.diseqc.diseqcInput == eDxDISEQC_INPUT_B)
		{
			HxSTD_sprintf((char*)szString, (char*)"%s B", ONDK_GetString(RES_DISEQC_ID));
		}
		else if (pstAntInfo->info.diseqc.diseqcInput == eDxDISEQC_INPUT_C)
		{
			HxSTD_sprintf((char*)szString, (char*)"%s C", ONDK_GetString(RES_DISEQC_ID));
		}
		else if (pstAntInfo->info.diseqc.diseqcInput == eDxDISEQC_INPUT_D)
		{
			HxSTD_sprintf((char*)szString, (char*)"%s D", ONDK_GetString(RES_DISEQC_ID));
		}
	}

	return ERR_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_GetAntennaTpList (Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSlotIdx, HINT32 *pnListNum, HUINT8 ***ppszTpList)
{
	HINT32				lIndex = 0, lListNum = 0;
	HUINT32				ulTpNum = 0;
	HUINT8				**pszTpList = NULL, *pszFind = NULL;
	HERROR				hError = ERR_FAIL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	NX_TP_TuningInfo_t	*pstTpInfoArray = NULL;

	if ((pnListNum == NULL) || (ppszTpList == NULL))
	{
		NX_APP_Error("Error!! pnListNum or ppszTpList is NULL \n");
		return	ONDK_RET_INITFAIL;
	}

	pszFind = (HUINT8*)HLIB_STD_StrCaseStr((char*)pstContents->astAntInfo[lSlotIdx].satType, (char*)NX_SAT_SATNAME_USERDEFINE);
	if (pszFind == NULL)
	{
		hError = NX_ANTENNA_GetTpListBySatelliteTypeName (pstContents->astAntInfo[lSlotIdx].satType, &ulTpNum, &pstTpInfoArray);
		if (hError != ERR_OK)
		{
			ulTpNum = 0;
			pstTpInfoArray = NULL;
			return	ONDK_RET_FAIL;
		}
	}
	else
	{
		ulTpNum = 0;
		pstTpInfoArray = NULL;
	}

	lListNum = (HINT32)ulTpNum + eNxSat_SEARCH_TP_DEFAULT;
	pszTpList = (HUINT8 **)NX_APP_Malloc (sizeof(HUINT8 *) * lListNum);
	if (pszTpList == NULL)
	{
		NX_APP_Error("Error!! pszTpList is NX_APP_Malloc() Fail!! \n");
		eResult = GWM_RET_OUT_OF_MEMORY;
		goto Error;
	}

	HxSTD_memset(pszTpList, 0x00, sizeof(HUINT8 *) * lListNum);
	for (lIndex = 0; lIndex < lListNum; lIndex++)
	{
		pszTpList[lIndex] = (HUINT8 *)NX_APP_Malloc (NX_TEXT_SIZE_32);
		if (pszTpList[lIndex] == NULL)
		{
			eResult = GWM_RET_OUT_OF_MEMORY;
			goto Error;
		}
	}

	HxSTD_StrCpy((char*)pszTpList[eNxSat_SEARCH_TP_ALL], (char*)ONDK_GetString(RES_ALL_ID));
	HxSTD_StrCpy((char*)pszTpList[eNxSat_SEARCH_TP_NONE], (char*)ONDK_GetString(RES_NONE_01_ID));
	HxSTD_StrCpy((char*)pszTpList[eNxSat_SEARCH_TP_USERDEFINED], (char*)ONDK_GetString(RES_USER_DEFINE_ID));
	for (lIndex = 0; lIndex < (HINT32)ulTpNum; lIndex++)
	{
		NX_ANTENNA_MakeTpString (eDxDELIVERY_TYPE_SAT, &(pstTpInfoArray[lIndex]), NULL, pszTpList[lIndex + eNxSat_SEARCH_TP_DEFAULT]);
	}

	*pnListNum = lListNum;
	*ppszTpList = pszTpList;

	pszTpList = NULL;
	eResult = ONDK_RET_OK;

Error:
	if (pstTpInfoArray != NULL)
	{
		NX_ANTENNA_FreeTpListArray(pstTpInfoArray);
		pstTpInfoArray = NULL;
	}

	if (pszTpList != NULL)
	{
		for (lIndex = 0; lIndex < lListNum; lIndex++)
		{
			if (pszTpList[lIndex] != NULL)
			{
				NX_APP_Free(pszTpList[lIndex]);
			}
		}

		NX_APP_Free(pszTpList);
		pszTpList = NULL;
	}

	return	eResult;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_FreeAntennaTpList (HINT32 lTPListItemNum, HUINT8 **pszListText)
{
	HINT32		lIndex = 0;

	if (pszListText != NULL)
	{
		for (lIndex = 0; lIndex < lTPListItemNum; lIndex++)
		{
			if (pszListText[lIndex] != NULL)
			{
				NX_APP_Free (pszListText[lIndex]);
				pszListText[lIndex] = NULL;
			}
		}

		NX_APP_Free (pszListText);
		pszListText = NULL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangeFocus(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lKey)
{
	HBOOL			TPState = FALSE;
	HINT32			lObjId = 0, lNextObjId = 0, lItemIdx = -1, lNextItemIdx = -1;
	HUINT32 		ulObjStatus = 0, ulPageNum = 0, ulAntenaNumMax = 0;
	ONDK_GWM_GuiObject_t	stGwmObject = NULL;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	ulPageNum = (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE;
	ulAntenaNumMax = (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI;

	switch (lKey)
	{
		case KEY_ARROWUP:
			if (lItemIdx == eNxSettingsItem_SatFixedSearchBtn)
			{
				if (pstContents->ulNumAnt < 1)
				{
					lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1);
					ONDK_GWM_Obj_SetFocus (lNextObjId);
					return	ONDK_MESSAGE_BREAK;
				}
			}
			else if(lItemIdx >= eNxSettingsItem_SatFixedANT1 && lItemIdx <= eNxSettingsItem_SatFixedANT4)
			{
				lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSwitchType);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case KEY_ARROWDOWN:
			if (lItemIdx == eNxSettingsItem_SatFixedSwitchType)
			{
				if (pstContents->ulNumAnt < 1)
				{
					lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1);
					ONDK_GWM_Obj_SetFocus (lNextObjId);
					return	ONDK_MESSAGE_BREAK;
				}
			}
			else if (lItemIdx >= eNxSettingsItem_SatFixedANT1 && lItemIdx <= eNxSettingsItem_SatFixedANT4)
			{
				ONDK_GWM_GuiObject_t objId = NULL;

				lNextItemIdx = eNxSettingsItem_SatFixedTP1 + (lItemIdx - eNxSettingsItem_SatFixedANT1);
				lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(lNextItemIdx);
				objId = ONDK_GWM_APP_GetObject(lNextObjId);
				if(objId)
				{
					ulObjStatus = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)objId);
					if (ulObjStatus & GUIOBJ_DISABLED)
					{
						// TP Button이 Disable 이면 Search Button이 다음
						lNextItemIdx = eNxSettingsItem_SatFixedSearchBtn;
						lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(lNextItemIdx);
						stGwmObject = ONDK_GWM_APP_GetObject(lNextObjId);
						if (stGwmObject != NULL)
						{
							ulObjStatus = ONDK_GWM_COM_Button_GetStatusByObj((BUTTON_OBJECT)stGwmObject);
							if (ulObjStatus & GUIOBJ_DISABLED)
							{
								// Search Button이 Disable이면 Switch Type
								lNextItemIdx = eNxSettingsItem_SatFixedSwitchType;
							}
						}
					}
				}

				lNextObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(lNextItemIdx);
				ONDK_GWM_Obj_SetFocus (lNextObjId);

				return	ONDK_MESSAGE_BREAK;
			}
			else if (lItemIdx >= eNxSettingsItem_SatFixedTP1 && lItemIdx <= eNxSettingsItem_SatFixedTP4)
			{
				TPState = nx_menuSearchChannel_Fixed_CheckTPSettingState(pstContents);
				if (TPState == FALSE)
				{
					return	ONDK_MESSAGE_BREAK;
				}
			}
			break;

		case KEY_ARROWRIGHT:
			lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
			if (ulAntenaNumMax > ulPageNum)
			{
				if (pstContents->bPageOver == FALSE)
				{
					if ((pstContents->ulNumAnt >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) && (lItemIdx == eNxSettingsItem_SatFixedANT4))
					{
						pstContents->bPageOver = TRUE;
						nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);
						lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1);
						ONDK_GWM_Obj_SetFocus(lObjId);
						return	ONDK_MESSAGE_BREAK;
					}
					else
					{
						if (lItemIdx == eNxSettingsItem_SatFixedANT4)
						{
							return	ONDK_MESSAGE_BREAK;
						}
					}
				}
				else
				{
					if (lItemIdx == eNxSettingsItem_SatFixedANT4)
					{
						return	ONDK_MESSAGE_BREAK;
					}
				}
			}
			break;

		case KEY_ARROWLEFT:
			lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
			if (ulAntenaNumMax > ulPageNum)
			{
				if ((pstContents->bPageOver == TRUE) && (lItemIdx == eNxSettingsItem_SatFixedANT1))
				{
					pstContents->bPageOver = FALSE;
					nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);
					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT4);
					ONDK_GWM_Obj_SetFocus(lObjId);
					return	ONDK_MESSAGE_BREAK;
				}
				else
				{
					if (lItemIdx == eNxSettingsItem_SatFixedANT1)
					{
						return	ONDK_MESSAGE_BREAK;
					}
				}
			}
			else
			{
				if (lItemIdx == eNxSettingsItem_SatFixedANT1)
				{
					return	ONDK_MESSAGE_BREAK;
				}
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DeleteAllAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32		i = 0;

	NX_ANTENNA_RemoveAll();	// Delete Antenna Info

	// Delete User Antenna slot
	for (i = 0; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI; i++)
	{
		NX_ANTENNA_SetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), NX_ANT_IDX_NULL);
		nx_menuSearchChannel_Fixed_ResetUserTPInfo(pstContents, i);
	}

	nx_menuSearchChannel_Fixed_FreeAntennaTpList(pstContents->ulListItemNum, pstContents->pszListText);
	pstContents->pszListText = NULL;
	nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ResetUserTPInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSlotIdx)
{
	Nx_TpSelectInfo_t	stTpInfo;

	HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));

	stTpInfo.uid = NX_ANT_IDX_NULL;
	stTpInfo.ucAvailable = FALSE;
	stTpInfo.ucUserTpDefined = FALSE;
	stTpInfo.eNxSearchTpType = eNxSat_SEARCH_TP_ALL;
	stTpInfo.ulTpInfoIndex = 0;
	stTpInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stTpInfo.stNxUserTpInfo.sat.antuid = NX_ANT_IDX_NULL;
	HxSTD_StrNCpy(stTpInfo.stNxUserTpInfo.sat.satType, (char*)ONDK_GetString(RES_SATELLITE_ID), DxANTINFO_NAME_LEN);

	NX_ANTENNA_SetUserTPInfo((eNxSat_ANTENAA_SLOT1 + lSlotIdx), &stTpInfo);
	//pstContents->astTpSelectInfo[lSlotIdx] = stTpInfo;
	HxSTD_memcpy(&pstContents->astTpSelectInfo[lSlotIdx], &stTpInfo, sizeof(Nx_TpSelectInfo_t));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReOrderUserTPInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntIdx)
{
	HERROR		hError = ERR_FAIL;
	HINT32		lTpSlotIdx = lAntIdx;
	HINT32		i = 0, lNextItem = 0;
	Nx_TpSelectInfo_t	stTpInfo;

	HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));

	// Reset Current TP
	nx_menuSearchChannel_Fixed_ResetUserTPInfo(pstContents, lTpSlotIdx);

	for (i = lTpSlotIdx; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI; i++)
	{
		if (i == (NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI - 1))
		{
			nx_menuSearchChannel_Fixed_ResetUserTPInfo(pstContents, i);
		}
		else
		{
			lNextItem = (i + 1);

			// Get next TP info
			HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
			hError = NX_ANTENNA_GetUserTPInfo((eNxSat_ANTENAA_SLOT1 + lNextItem), &stTpInfo);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_ANTENNA_GetUserTPInfo(%d) Fail!! \n", (eNxSat_ANTENAA_SLOT1 + lNextItem));
				continue;
			}

			// Set Current TP info
			hError = NX_ANTENNA_SetUserTPInfo((eNxSat_ANTENAA_SLOT1 + i), &stTpInfo);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_ANTENNA_SetUserTPInfo(%d) Fail!! \n", (eNxSat_ANTENAA_SLOT1 + i));
				continue;
			}
			HxSTD_memcpy(&pstContents->astTpSelectInfo[i], &stTpInfo, sizeof(Nx_TpSelectInfo_t));
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReOrderAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntIdx)
{
	HERROR		hError = ERR_FAIL;
	HINT32		i = 0, lAntennaUid = (HINT32)NX_ANT_IDX_NULL, lNextItem = 0;


	for (i = lAntIdx; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI; i++)
	{
		if (i == (NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI - 1))
		{
			pstContents->alAntIdxArray[i] = (HINT32)NX_ANT_IDX_NULL;
			hError = NX_ANTENNA_SetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), NX_ANT_IDX_NULL);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_ANTENNA_SetUserAntIndexUDB(%d) Fail!! \n", (eNxSat_ANTENAA_SLOT1 + i));
			}
		}
		else
		{
			lNextItem = (i + 1);
			lAntennaUid = (HINT32)NX_ANT_IDX_NULL;
			hError = NX_ANTENNA_GetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + lNextItem), &lAntennaUid);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_ANTENNA_GetUserAntIndexUDB(%d) Fail!! \n", (eNxSat_ANTENAA_SLOT1 + lNextItem));
				continue;
			}

			NX_ANTENNA_SetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), lAntennaUid);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_ANTENNA_SetUserAntIndexUDB(%d) Fail!! \n", (eNxSat_ANTENAA_SLOT1 + i));
				continue;
			}

			pstContents->alAntIdxArray[i] = pstContents->alAntIdxArray[lNextItem];
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateAntennaInfo(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lAntUid, HBOOL bAddAntenna)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bSetDefaultTP = FALSE;
	HINT32			lObjId = 0, lItemIdx = -1, lAntIdx = 0;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	NX_SAT_SearchTpType_t eTpType = eNxSat_SEARCH_TP_ALL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatFixedANT1:
		case eNxSettingsItem_SatFixedANT2:
		case eNxSettingsItem_SatFixedANT3:
		case eNxSettingsItem_SatFixedANT4:
			if (pstContents->bPageOver)
			{
				lAntIdx = lItemIdx - eNxSettingsItem_SatFixedANT1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE;
			}
			else
			{
				lAntIdx = lItemIdx - eNxSettingsItem_SatFixedANT1;
			}

			if (lAntIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
			{
				NX_APP_Error("Error!!! Out of range Max :(%d) lAntIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lAntIdx);
				return	ONDK_RET_FAIL;
			}

			if (bAddAntenna == TRUE)
			{
				// Add New Antenna Slot
				if (pstContents->alAntIdxArray[lAntIdx] == (HINT32)NX_ANT_IDX_NULL)
				{
					// Set NEW Antenna
					NX_ANTENNA_SetUserAntIndexUDB(eNxSat_ANTENAA_SLOT1 + lAntIdx, lAntUid);
					bSetDefaultTP = TRUE;
				}

				nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);

				// Set Default TP Value
				if (bSetDefaultTP == TRUE)
				{
					switch (eOpType)
					{
						case eDxOPERATOR_MBC:	eTpType = eNxSat_SEARCH_TP_NONE;	break;
						default:				eTpType = eNxSat_SEARCH_TP_ALL;		break;
					}
					nx_menuSearchChannel_Fixed_SetTpValue(pstContents, lAntIdx, eTpType);
				}

				ONDK_GWM_APP_InvalidateObject(lObjId);
			}
			else
			{
				// Delete Antenna Slot and Re ordering Antenna Index Slot
				nx_menuSearchChannel_Fixed_ReOrderAntennaInfo(pstContents, lAntIdx);
				nx_menuSearchChannel_Fixed_ReOrderUserTPInfo(pstContents, lAntIdx);
				nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);
				ONDK_GWM_APP_InvalidateObject(lObjId);
			}
			break;
		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateSwitchType(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t 	*pSubItem = NULL;

	pSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstContents->eAntType != pstContents->eNewAntType)
	{
		pstContents->eAntType = pstContents->eNewAntType;

		// Stop Channel
		NX_CHANNEL_StopChannel();

		// Change Antenna Type
		NX_ANTENNA_SetAntennaType(pstContents->eAntType);

		// Delete All Service
		NX_CHANNEL_RemoveChannelListAll();
		ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_DELETE_SERVICE_ALL, (HINT32)NULL, 0, 0, 0);

		// Delete All Antenna Info
		nx_menuSearchChannel_Fixed_DeleteAllAntennaInfo(pstContents);

		// Re-Create Window
		nx_menuSearchChannel_Fixed_ReCreateWindow(pstContents);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateSubItem(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatFixedSwitchType:
			nx_menuSearchChannel_Fixed_UpdateSwitchType(pstContents, lObjId);
			break;

		default:
			NX_APP_Error("ERROR!!! lItemIdx:[%d] Who are you! \n", lItemIdx);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ChangedSubItemValue(Nx_MenuSearchSat_Contents_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			lObjId = 0, lItemIdx = -1, lTpSlotIdx = 0;
	HUINT32 		ulMenuItemId = 0, ulProperty = 0, ulListIdx = 0;
	//HERROR			hError = ERR_FAIL;
	ONDK_Result_t	hResult = ONDK_RET_FAIL;
	NX_SAT_AntennaType_e		eAntennaType = eNxSat_AntType_LNB_Only;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_SatFixedSwitchType:
			pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
			if (pstSubItem == NULL)
			{
				NX_APP_Error("Error!!! NX_PRISM_AppContents_GetSubitemByObjId() Fail lObjId:(0x%x)!! \n", lObjId);
				return ONDK_RET_FAIL;
			}
			nx_menuSearchChannel_Fixed_GetMenuItemIdFormObjId(pstSubItem->ulId, &ulMenuItemId);
			hResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)ulCurIdx, &ulListIdx);
			if (hResult != ONDK_RET_OK)
			{
				eAntennaType = pstContents->eAntType;
			}
			else
			{
				eAntennaType = (NX_SAT_AntennaType_e)ulListIdx;
			}

			pstContents->eNewAntType = eAntennaType;
			if (pstContents->eAntType != pstContents->eNewAntType)
			{
				nx_menuSearchChannel_Fixed_CreateChangAntTypeDialog(pstContents, eNxSettingsItem_DialogType_SwitchType, (HUINT8*)RES_MESG_3238_ID);
			}
			break;

		case eNxSettingsItem_SatFixedTP1:
		case eNxSettingsItem_SatFixedTP2:
		case eNxSettingsItem_SatFixedTP3:
		case eNxSettingsItem_SatFixedTP4:
			NX_APP_Debug("lItemIdx:[%d], lTpSlotIdx : [%d], ulCurIdx :[%d] \n", lItemIdx, lTpSlotIdx, ulCurIdx);

			lTpSlotIdx = (pstContents->bPageOver) ? (lItemIdx - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemIdx - eNxSettingsItem_SatFixedTP1) ;

			hResult = nx_menuSearchChannel_Fixed_SetTpValue(pstContents, lTpSlotIdx, (NX_SAT_SearchTpType_t)ulCurIdx);
			if (hResult != ONDK_RET_OK)
			{
				return	ONDK_RET_FAIL;
			}

			ONDK_GWM_APP_InvalidateObject(lObjId);

			if (ulCurIdx == eNxSat_SEARCH_TP_USERDEFINED)
			{
				ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatTransponderSetting_Proc",
							ulProperty, NX_PRISM_MENU_Search_SatTransponderSetting_Proc, HANDLE_NULL, pstContents->eAntType,
							(eNxSat_ANTENAA_SLOT1 + lTpSlotIdx), pstContents->astTpSelectInfo[lTpSlotIdx].stNxUserTpInfo.sat.antuid);
				return	ONDK_RET_FAIL;
			}
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_AddItemSwitchType(Nx_MenuSearchSat_Contents_t *pstContents)
{
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

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSwitchType);

	// Set Main String
	pMainStrId = nx_menuSearchChannel_Fixed_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuSearchChannel_Fixed_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
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
		return	ONDK_RET_FAIL;
	}

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eAntType), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
		NX_PRISM_MENU_MenuItem_GetValueFromIndex (ulMenuItemId, ulListIdx, &lValue);
		pstContents->eAntType = lValue;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;
	stSubItem.lSubItemValue = (HINT32)pstContents->eAntType;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_UpdateEnableDisableAntennaSlot(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32			lPrevFocusObjId = -1, lObjId = -1, lTpSlotIdx = 0, i = 0, lCount = 0;
	HINT32			lAntObjId = -1, lTpObjId = -1, lItemIdx = 0;

	switch (pstContents->eAntType)
	{
		case eNxSat_AntType_LNB_Only:
			lTpSlotIdx = (pstContents->bPageOver) ? (eNxSettingsItem_SatFixedTP1 - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (eNxSettingsItem_SatFixedTP1 - eNxSettingsItem_SatFixedTP1) ;

			if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
			{
				NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
				return	ONDK_RET_FAIL;
			}

			if (pstContents->ulNumAnt == 0)
			{
				// Disalbe - TP & Search Button
				pstContents->astTpSelectInfo[lTpSlotIdx].ucAvailable = FALSE;
				lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1);
				ONDK_GWM_Obj_DisableObject(lObjId);
				lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSearchBtn);
				ONDK_GWM_Obj_DisableObject(lObjId);
			}
			else
			{
				// Enable - TP & Search Button
				pstContents->astTpSelectInfo[lTpSlotIdx].ucAvailable = TRUE;
				lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1);
				ONDK_GWM_Obj_EnableObject(lObjId);
				lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSearchBtn);
				ONDK_GWM_Obj_EnableObject(lObjId);
			}
			break;

		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
		case eNxSat_AntType_Multiswitch:
			lObjId = GWM_GetFocusedObjectID();
			lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
			if ((lItemIdx >= eNxSettingsItem_SatFixedANT1) && (lItemIdx <= eNxSettingsItem_SatFixedANT4))
			{
				lPrevFocusObjId = lObjId;
			}

			// Disable All
			for (i = 0; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE; i++)
			{
				lAntObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1 + i);
				ONDK_GWM_Obj_DisableObject(lAntObjId);

				lTpObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1 + i);
				ONDK_GWM_Obj_DisableObject(lTpObjId);
			}

			// Enable Object Manager
			if (pstContents->ulNumAnt == 0)
			{
				lAntObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1);
				ONDK_GWM_Obj_EnableObject(lAntObjId);
				ONDK_GWM_Obj_SetFocus(lAntObjId);
			}
			else
			{
				// Enable object until antenna count
				lCount = (pstContents->bPageOver == TRUE) ? (pstContents->ulNumAnt - NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : pstContents->ulNumAnt;
				for (i = 0; i < lCount; i++)
				{
					lAntObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1 + i);
					ONDK_GWM_Obj_EnableObject(lAntObjId);

					lTpObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1 + i);
					ONDK_GWM_Obj_EnableObject(lTpObjId);
				}
				// Enable object of next antenna
				if (lCount != NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE)
				{
					lAntObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1 + lCount);
					ONDK_GWM_Obj_EnableObject(lAntObjId);
				}
				if (lPrevFocusObjId != -1)
				{
					// Set Focus
					ONDK_GWM_Obj_SetFocus(lPrevFocusObjId);
				}

				if (pstContents->bPageOver == TRUE)
				{
					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowLeft);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_LEFT_C_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);

					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowRight);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_RIGHT_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);
				}
				else if ((pstContents->bPageOver == FALSE) && (pstContents->ulNumAnt >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE))
				{
					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowLeft);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_LEFT_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);

					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowRight);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_RIGHT_C_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);
				}
				else
				{
					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowLeft);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_LEFT_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);

					lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowRight);
					ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_RIGHT_PNG);
					ONDK_GWM_APP_InvalidateObject(lObjId);
				}

			}
			break;

		default:
			NX_APP_Error("Error!! Invalid Support Type [%d]\n", pstContents->eAntType);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_LoadAntennaInfos(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32				i = 0, lRealAntNum = 0, lUserAntCount = 0, lAntIdx = NX_ANT_IDX_NULL;
	HERROR				hError = ERR_FAIL;
	DxAntInfo_t			stAntInfo;
	Nx_TpSelectInfo_t	stTpInfo;


	pstContents->ulNumAnt = 0;
	HxSTD_memset (pstContents->astAntInfo, 0x00, sizeof(DxAntInfo_t) * NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI);

	// Init Antenna Value
	for (i = 0; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI; i++)
	{
		// Antenna Index
		pstContents->alAntIdxArray[i] = NX_ANT_IDX_NULL;

		// Antenna Info
		pstContents->astAntInfo[i].uid = NX_ANT_IDX_NULL;
		HxSTD_StrCpy(pstContents->astAntInfo[i].satName, (char*)NX_SAT_SATNAME_NONE);
		HxSTD_StrCpy(pstContents->astAntInfo[i].satType, (char*)NX_SAT_SATNAME_NONE);

		// Tp Info
		pstContents->astTpSelectInfo[i].uid = NX_ANT_IDX_NULL;
		pstContents->astTpSelectInfo[i].eNxSearchTpType = eNxSat_SEARCH_TP_ALL;
		pstContents->astTpSelectInfo[i].eDeliType = eDxDELIVERY_TYPE_SAT;
	}

	// Load Antenna Index
	lRealAntNum = 0;
	lUserAntCount = NX_ANTENNA_GetUserAntCountUDB();
	NX_APP_Debug("NX_ANTENNA_GetUserAntCountUDB() [%d]\n", lUserAntCount);
	for (i = 0; i < lUserAntCount; i++)
	{
		lAntIdx = NX_ANT_IDX_NULL;
		hError = NX_ANTENNA_GetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), &lAntIdx);
		if (hError == ERR_OK)
		{
			pstContents->alAntIdxArray[lRealAntNum] = lAntIdx;
			lRealAntNum++;
			NX_APP_Debug("NX_ANTENNA_GetUserAntIndexUDB()lRealAntNum[%d] = [%d]\n", lRealAntNum, lAntIdx);
		}
		else
		{
			NX_APP_Debug("NX_ANTENNA_GetUserAntIndexUDB() Not found Antenna [%d]\n", (eNxSat_ANTENAA_SLOT1 + i));
		}
	}

	NX_APP_Debug("lRealAntNum : [%d]\n", lRealAntNum);

	// Load Antenna Info
	for (i = 0; i < lRealAntNum; i++)
	{
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		hError = NX_ANTENNA_GetInfo(pstContents->alAntIdxArray[i], &stAntInfo);
		if ((hError == ERR_OK) && (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) != 0))
		{
			pstContents->astAntInfo[i] = stAntInfo;
		}
	}
	pstContents->ulNumAnt = lRealAntNum;

	// Load User TP
	for (i = 0; i < lRealAntNum; i++)
	{
		HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
		hError = NX_ANTENNA_GetUserTPInfo((eNxSat_ANTENAA_SLOT1 + i), &stTpInfo);
		if (hError == ERR_OK)
		{
			pstContents->astTpSelectInfo[i] = stTpInfo;
		}
		else
		{
			pstContents->astTpSelectInfo[i].ucAvailable	= FALSE;
			pstContents->astTpSelectInfo[i].ucUserTpDefined	= FALSE;
			pstContents->astTpSelectInfo[i].uid = pstContents->alAntIdxArray[i];
			pstContents->astTpSelectInfo[i].eNxSearchTpType	= eNxSat_SEARCH_TP_ALL;
			pstContents->astTpSelectInfo[i].ulTpInfoIndex = 0;
			pstContents->astTpSelectInfo[i].eDeliType = eDxDELIVERY_TYPE_SAT;
		}
	}

	nx_menuSearchChannel_Fixed_UpdateEnableDisableAntennaSlot(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_LoadItemValue(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32		lValue = 0;
	HERROR		hError = ERR_FAIL;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;

	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_OK)
	{
		eAntennaType = eNxSat_AntType_LNB_Only;
		NX_ANTENNA_SetAntennaType(eAntennaType);
	}
	else
	{
		eAntennaType = (NX_SAT_AntennaType_e)lValue;
	}

	switch (eAntennaType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_Monoblock_LNB:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
		case eNxSat_AntType_Multiswitch:
			break;

		default:
			NX_APP_Error("Error!! Invalid Antenna Type : [%d]\n", eAntennaType);
			eAntennaType = eNxSat_AntType_LNB_Only;
			NX_ANTENNA_SetAntennaType(eAntennaType);
			break;
	}

	pstContents->eAntType = eAntennaType;
	pstContents->eNewAntType = eAntennaType;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_InitSettingsItems(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	ONDK_Result_t	eResult = ONDK_RET_OK;
	DxOperator_e	eOpType = eDxOPERATOR_None;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSearchSat_Contents_t));
	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	// Load the antenna slot & antenna type
	nx_menuSearchChannel_Fixed_LoadItemValue(pstContents);

	// Load Appkit Antenna Setting & Set Port Antenna Settings
	NX_ANTENNA_LoadAntennaSetting();

	// Load the antenna lists (1 ~ 8)
	nx_menuSearchChannel_Fixed_LoadAntennaInfos(pstContents);

	pstContents->ulListItemNum = 0;
	pstContents->bPageOver = FALSE;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			pstContents->bSupportOpt = FALSE;
			break;
		default:
			pstContents->bSupportOpt = TRUE;
			break;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SEARCH_CHANNELS_01_ID), MAX_ELEMENT_TEXT_LENGTH);

	nx_menuSearchChannel_Fixed_AddItemSwitchType(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawAntennaInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		text_x_offset = 0, text_y_offset = 0, i = 0, lObjId = -1, lItemIdx = 0, lAntSlotIdx = NX_ANT_IDX_NULL;

	HUINT32 	ulStatus = 0, ulFontSize = 0;
	HUINT8		szString[NX_TEXT_SIZE_64];

	ONDK_Color_t	title_fontcolor = 0x00000000, value_fontcolor = 0x00000000, cursorColor = 0x00000000;
	ONDK_Rect_t 	stRectAntImg = { 0, 0, 0, 0 };
	ONDKFontHandle	*hFontHandle = NULL;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;

	DxAntInfo_t		*pstAntennaInfo = NULL;
	Nx_MenuSearchSat_Contents_t	*pstContents = NULL;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontcolor = COL (C_T_Whistle_100);
	value_fontcolor = COL (C_T_Gong03);
	cursorColor = COL(C_G_Whistle_15);

	lCursorMg = 0;
	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);
	stRectAntImg = ONDK_Rect(x1 + NX_MENU_SRCHSAT_FIXED_ANT_IMG_X, y1 + NX_MENU_SRCHSAT_FIXED_ANT_IMG_Y, NX_MENU_SRCHSAT_FIXED_ANT_IMG_W, NX_MENU_SRCHSAT_FIXED_ANT_IMG_H);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontcolor = COL(C_T_Gong08_40);
			value_fontcolor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_D_PNG);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontcolor = COL (C_T_Bang_100);
			value_fontcolor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_C_PNG);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontcolor = COL (C_T_Bang_100);
			value_fontcolor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_C_PNG);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_N_PNG);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontcolor = COL (C_T_Gong08_40);
			value_fontcolor = COL (C_T_Gong08_40);
			cursorColor = COL(C_G_Base01_20);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_D_PNG);
		}
		else
		{
			title_fontcolor = COL (C_T_Gong08_40);
			value_fontcolor = COL (C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
			// Antenna Image
			ONDK_GUICOM_IMG_Blit(screen, stRectAntImg, RES_431_01_SET_SATELLITE_D_PNG);
		}
	}

	pstContents = nx_menuSearchChannel_Fixed_GetContents();
	lObjId = ONDK_GWM_Obj_GetObjectID (object);
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	if ((lItemIdx >= eNxSettingsItem_SatFixedANT1) && (lItemIdx <= eNxSettingsItem_SatFixedANT4))
	{
		if (pstContents->bPageOver)
		{
			lAntSlotIdx = lItemIdx - eNxSettingsItem_SatFixedANT1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE;
		}
		else
		{
			lAntSlotIdx = lItemIdx - eNxSettingsItem_SatFixedANT1;
		}
	}
	else
	{
		NX_APP_Error("Invalid Object - lObjId:(0x%x), Index:(%d) \n", lObjId, lItemIdx);
		return	ONDK_RET_FAIL;
	}

	if (lAntSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
	{
		NX_APP_Error("Error!!! Out of range Max :(%d) lAntSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lAntSlotIdx);
		return	ONDK_RET_FAIL;
	}

	pstAntennaInfo = (pstContents->alAntIdxArray[lAntSlotIdx] < NX_ANT_IDX_NULL) ? &(pstContents->astAntInfo[lAntSlotIdx]) : NULL;

	/* custom value for subitem text for right alignment*/
	text_x_offset = NX_MENU_SRCHSAT_FIXED_ANT_TEXT_X;
	text_y_offset = NX_MENU_SRCHSAT_FIXED_ANT_TEXT_Y;

	// TEXT - Antenna Name
#if defined(CONFIG_OP_AMX)
	ulFontSize = NX_PRISM_FONT_SIZE_18;
#else
	ulFontSize = NX_PRISM_FONT_SIZE_22;
#endif
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ulFontSize);
	if (hFontHandle)
	{
		HxSTD_memset(szString, 0x00, NX_TEXT_SIZE_64);
		szString[0] = '\0';

		nx_menuSearchChannel_Fixed_GetAntennaNameString(pstAntennaInfo, szString);
		x = x1;
		y = y1 + text_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szString, x, y, w, title_fontcolor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	// TEXT - Antenna Info
	y1 += NX_MENU_SRCHSAT_FIXED_ANT_TEXT_LS;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);
	if (hFontHandle)
	{
		switch (pstContents->eAntType)
		{
			case eNxSat_AntType_DiSEqC_1_0:
			case eNxSat_AntType_DiSEqC_2_0:
			case eNxSat_AntType_Multiswitch:
				for (i = 0; i < eNxSettingsItem_AntInfo_MAX; i++)
				{
					HxSTD_memset(szString, 0x00, NX_TEXT_SIZE_64);
					szString[0] = '\0';

					if (i == eNxSettingsItem_AntInfo_LNB)
					{
						nx_menuSearchChannel_Fixed_GetAntennaLnbFreqString(pstAntennaInfo, szString);
					}
					else if (i == eNxSettingsItem_AntInfo_22kHTone)
					{
						nx_menuSearchChannel_Fixed_GetAntenna22KToneString(pstAntennaInfo, szString);
					}
					else if (i == eNxSettingsItem_AntInfo_Diseqc)
					{
						nx_menuSearchChannel_Fixed_GetAntennaDiseqcString(pstAntennaInfo, szString);
					}
					x = x1;
					y = y1 + text_y_offset;
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szString, x, y, w, title_fontcolor, ONDKSTF_CENTER);

					y1 += NX_MENU_SRCHSAT_FIXED_ANT_TEXT_LS;
				}
				break;

			default:
				HxSTD_memset(szString, 0x00, NX_TEXT_SIZE_64);
				szString[0] = '\0';
				nx_menuSearchChannel_Fixed_GetAntennaLnbFreqString(pstAntennaInfo, szString);
				x = x1;
				y = y1 + text_y_offset;
				ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szString, x, y, w, title_fontcolor, ONDKSTF_CENTER);
				break;
		}
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawTpInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		text_x_offset = 0, text_y_offset = 0, lObjId = -1, lItemIdx = 0, lTpSlotIdx = 0;

	HUINT32 	ulStatus = 0;
	HUINT8		*title = NULL, szString[NX_TEXT_SIZE_64];

	HERROR				hError = ERR_FAIL;
	ONDK_Color_t		title_fontcolor = 0x00000000, value_fontcolor = 0x00000000, cursorColor = 0x00000000;
	ONDKFontHandle		*hFontHandle = NULL;
	BUTTON_OBJECT		objBtn = (BUTTON_OBJECT)object;
	NX_TP_TuningInfo_t 	stTpInfo;
	Nx_MenuSearchSat_Contents_t	*pstContents = NULL;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	title_fontcolor = COL (C_T_Whistle_100);
	value_fontcolor = COL (C_T_Whistle_100);
	cursorColor = COL(C_G_Whistle_15);
	lCursorMg = 0;

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		/* active, front most */
		if (ulStatus & GUIOBJ_DISABLED)
		{
			title_fontcolor = COL(C_T_Gong08_40);
			value_fontcolor = COL(C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			/* Disable Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			title_fontcolor = COL (C_T_Bang_100);
			value_fontcolor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			title_fontcolor = COL (C_T_Bang_100);
			value_fontcolor = COL (C_T_Bang_100);
			cursorColor = COL(C_G_Base01_100);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			/* Normal Item cursor */
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}
	else
	{
		/* deactivate, under-covered */
		if (ulStatus & GUIOBJ_HILIGHTED)
		{
			title_fontcolor = COL (C_T_Whistle_100);
			value_fontcolor = COL (C_T_Whistle_100);
			cursorColor = COL(C_G_Base01_20);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
		else
		{
			title_fontcolor = COL (C_T_Gong08_40);
			value_fontcolor = COL (C_T_Gong08_40);
			cursorColor = COL(C_G_Whistle_05);

			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h - lCursorMg, cursorColor);
		}
	}

	pstContents = nx_menuSearchChannel_Fixed_GetContents();
	lObjId = ONDK_GWM_Obj_GetObjectID (object);
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	if ((lItemIdx >= eNxSettingsItem_SatFixedTP1) && (lItemIdx <= eNxSettingsItem_SatFixedTP4))
	{
		lTpSlotIdx = (pstContents->bPageOver) ? (lItemIdx - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemIdx - eNxSettingsItem_SatFixedTP1) ;
	}
	else
	{
		NX_APP_Error("ERROR!! lItemIdx : [%d] Fail!! \n", lItemIdx);
		return	ONDK_RET_FAIL;
	}

	if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
	{
		NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
		return	ONDK_RET_FAIL;
	}

	text_x_offset = NX_MENU_SRCHSAT_FIXED_TP_TEXT_X;
	text_y_offset = NX_MENU_SRCHSAT_FIXED_TP_TEXT_Y;

	// TEXT - TP Info Title
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_18);
	if (hFontHandle)
	{
		title = ONDK_GetString(RES_TRANSPONDER_ID);
		x = x1;
		y = y1 + text_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, w, title_fontcolor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	// TEXT - TP Info
	y1 += NX_MENU_SRCHSAT_FIXED_TP_TEXT_LS;
	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_18);
	if (hFontHandle)
	{
		HxSTD_memset(szString, 0x00, NX_TEXT_SIZE_64);
		szString[0] = '\0';

		if (pstContents->astTpSelectInfo[lTpSlotIdx].ucAvailable == TRUE)
		{
			switch (pstContents->astTpSelectInfo[lTpSlotIdx].eNxSearchTpType)
			{
				case eNxSat_SEARCH_TP_ALL:
					HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_ALL_ID), NX_TEXT_SIZE_64);
					break;
				case eNxSat_SEARCH_TP_NONE:
					HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_NONE_01_ID), NX_TEXT_SIZE_64);
					break;
				case eNxSat_SEARCH_TP_USERDEFINED:
					if (pstContents->astTpSelectInfo[lTpSlotIdx].ucUserTpDefined == (HUINT8)TRUE)
					{
						NX_ANTENNA_MakeTpString(pstContents->astTpSelectInfo[lTpSlotIdx].eDeliType, &(pstContents->astTpSelectInfo[lTpSlotIdx].stNxUserTpInfo), NULL, szString);
					}
					else
					{
						HxSTD_StrNCpy((char*)szString, (char*)ONDK_GetString(RES_USER_DEFINE_ID), NX_TEXT_SIZE_64);
					}
					break;
				case eNxSat_SEARCH_TP_DEFAULT:
					hError = NX_ANTENNA_GetTpBySatelliteTypeName(pstContents->astAntInfo[lTpSlotIdx].satType, pstContents->astTpSelectInfo[lTpSlotIdx].ulTpInfoIndex, &stTpInfo);
					if (hError == ERR_OK)
					{
						NX_ANTENNA_MakeTpString(pstContents->astTpSelectInfo[lTpSlotIdx].eDeliType, &stTpInfo, NULL, szString);
					}
					else
					{
						HxSTD_StrNCpy ((char*)szString, (char*)"-", (NX_TEXT_SIZE_64 - 1));
						NX_APP_Error("ERROR!! NX_ANTENNA_GetTpBySatelliteTypeName() Fail!! \n");
					}
					break;
				default:
					HxSTD_StrNCpy ((char*)szString, (char*)"-", (NX_TEXT_SIZE_64 - 1));
					break;
			}
		}
		else
		{
			HxSTD_StrNCpy ((char*)szString, (char*)"-", (NX_TEXT_SIZE_64 - 1));
		}
		x = x1;
		y = y1 + text_y_offset;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)szString, x, y, w, title_fontcolor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawOption(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	if (pstContents->bSupportOpt == FALSE)
	{
		NX_APP_Info("Not Support Option - bSupportOpt : (%d) \n", pstContents->bSupportOpt);
		return	ONDK_RET_OK;
	}

	// Option
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_OPT_X, NX_MENU_SRCHSAT_FIXED_OPT_Y, NX_MENU_SRCHSAT_FIXED_OPT_W, NX_MENU_SRCHSAT_FIXED_OPT_H);
	lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedOpt);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawOption_HelpR);
	ONDK_GWM_Obj_DisableObject(lObjId);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawSearchButton(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Search Buttons
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_SEARCH_BT_X, NX_MENU_SRCHSAT_FIXED_SEARCH_BT_Y, NX_MENU_SRCHSAT_FIXED_SEARCH_BT_W, NX_MENU_SRCHSAT_FIXED_SEARCH_BT_H);
	lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSearchBtn);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_SEARCH_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_B1);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawLnbOnlyAntenna(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Antenna Slot Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_ANTENNA_CENTER_X, NX_MENU_SRCHSAT_FIXED_ANTENNA_Y, NX_MENU_SRCHSAT_FIXED_ANTENNA_W, NX_MENU_SRCHSAT_FIXED_ANTENNA_H);
	lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuSearchChannel_Fixed_DrawAntennaInfo);

	// Transponder Slot Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_CENTER_X, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_Y, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_H);
	lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuSearchChannel_Fixed_DrawTpInfo);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawDiseqcAntenna(Nx_MenuSearchSat_Contents_t *pstContents)
{
	HINT32			lObjId = -1, i = 0, x = 0, y = 0;
	HUINT32 		ulPageNum = 0, ulAntenaNumMax = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	ulPageNum = (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE;
	ulAntenaNumMax = (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI;

	// Antenna Slot Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_ANTENNA_X, NX_MENU_SRCHSAT_FIXED_ANTENNA_Y, NX_MENU_SRCHSAT_FIXED_ANTENNA_W, NX_MENU_SRCHSAT_FIXED_ANTENNA_H);
	for (i = 0; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE; i++)
	{
		lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedANT1 + i);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, NULL);
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuSearchChannel_Fixed_DrawAntennaInfo);
		stRect.x += (NX_MENU_SRCHSAT_FIXED_ANTENNA_W + NX_MENU_SRCHSAT_FIXED_ANTENNA_TAB);
	}

	// Transponder Slot Info
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_FIXED_TRANSPONDER_X, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_Y, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W, NX_MENU_SRCHSAT_FIXED_TRANSPONDER_H);
	for (i = 0; i < NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE; i++)
	{
		lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedTP1 + i);
		ONDK_GWM_COM_Button_Create(lObjId, &stRect, NULL);
		ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuSearchChannel_Fixed_DrawTpInfo);
		stRect.x += (NX_MENU_SRCHSAT_FIXED_TRANSPONDER_W + NX_MENU_SRCHSAT_FIXED_TRANSPONDER_TAB);
	}

	if (ulAntenaNumMax > ulPageNum)
	{
		x = (NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_X - (NX_MENU_SRCHSAT_FIXED_ANTENNA_TAB *3));
		y = (NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_Y + (NX_MENU_SRCHSAT_FIXED_ANTENNA_H/2));
		stRect = ONDK_Rect(x, y, NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_W, NX_MENU_SRCHSAT_FIXED_ARROW_LEFT_H);
		lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowLeft);
		ONDK_GWM_COM_Image_Create(lObjId, &stRect);
		ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_LEFT_PNG);
		ONDK_GWM_Obj_DisableObject(lObjId);

		x = (NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_X + (NX_MENU_SRCHSAT_FIXED_ANTENNA_W * NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE)
			+ (NX_MENU_SRCHSAT_FIXED_ANTENNA_TAB * NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE));
		y = (NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_Y + (NX_MENU_SRCHSAT_FIXED_ANTENNA_H/2));
		stRect = ONDK_Rect(x, y, NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_W, NX_MENU_SRCHSAT_FIXED_ARROW_RIGHT_H);
		lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedArrowRight);
		ONDK_GWM_COM_Image_Create(lObjId, &stRect);
		ONDK_GWM_COM_Image_SetImage(lObjId, RES_106_00_ARROW_BOX_RIGHT_PNG);
		ONDK_GWM_Obj_DisableObject(lObjId);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_DrawDefaultUI(Nx_MenuSearchSat_Contents_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSearchChannel_Fixed_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawLeafSubItem_Cursor_C1_Arrow);

	if (pstContents->eAntType == eNxSat_AntType_LNB_Only)
	{
		nx_menuSearchChannel_Fixed_DrawLnbOnlyAntenna(pstContents);
	}
	else
	{
		nx_menuSearchChannel_Fixed_DrawDiseqcAntenna(pstContents);
	}

	nx_menuSearchChannel_Fixed_DrawSearchButton(pstContents);
	nx_menuSearchChannel_Fixed_DrawOption(pstContents);

	nx_menuSearchChannel_Fixed_UpdateEnableDisableAntennaSlot(pstContents);
	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_ReCreateWindow(Nx_MenuSearchSat_Contents_t *pstContents)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);

	ONDK_GWM_APP_Destroy(0);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatFixedAntna_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
		NX_PRISM_MENU_Search_SatFixedAntna_Proc, HANDLE_NULL, 0, 0, 0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmCreate(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	nx_menuSearchChannel_Fixed_InitSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuSearchChannel_Fixed_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmDestroy(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	nx_menuSearchChannel_Fixed_FreeAntennaTpList(pstContents->ulListItemNum, pstContents->pszListText);
	pstContents->pszListText = NULL;

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSearchSat_Contents_t));

	nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
	nx_menuSearchChannel_Fixed_DestroyListPopup();

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_SetOption_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_MENU_Search_SetOption_Proc);
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmHapiMsg(Nx_MenuSearchSat_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmKeyDown(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
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

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc, 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
			break;
		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			return	nx_menuSearchChannel_Fixed_ChangeFocus(pstContents, lKeyId);

		case KEY_OPT:
			if (pstContents->bSupportOpt == TRUE)
			{
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SetOption_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
									(ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_SetOption_Proc, 0, 0, 0, 0);
			}
			else
			{
				NX_PRISM_NotAvailable_Show();
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmClicked(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemId = -1, lAntIdx = -1, lTpSlotIdx = -1;
	HUINT32		ulProperty = 0;

	ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MENU);
	lItemId = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_SatFixedSwitchType:	// Antenna Type
			nx_menuSearchChannel_Fixed_CreateListPopup(pstContents, lObjId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_SatFixedANT1:			// Satellite Slot
		case eNxSettingsItem_SatFixedANT2:
		case eNxSettingsItem_SatFixedANT3:
		case eNxSettingsItem_SatFixedANT4:
			lAntIdx = (pstContents->bPageOver) ? (lItemId - eNxSettingsItem_SatFixedANT1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemId - eNxSettingsItem_SatFixedANT1) ;

			if (lAntIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
			{
				NX_APP_Error("Error!!! Out of range Max :(%d) lAntIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lAntIdx);
				return	ONDK_RET_FAIL;
			}

			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatSatelliteSetting_Proc",
						ulProperty, NX_PRISM_MENU_Search_SatSatelliteSetting_Proc,
						HANDLE_NULL, pstContents->eAntType, pstContents->alAntIdxArray[lAntIdx], 0);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_SatFixedTP1:			// TP Slot
		case eNxSettingsItem_SatFixedTP2:
		case eNxSettingsItem_SatFixedTP3:
		case eNxSettingsItem_SatFixedTP4:
			lTpSlotIdx = (pstContents->bPageOver) ? (lItemId - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemId - eNxSettingsItem_SatFixedTP1) ;

			if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
			{
				NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
				return	ONDK_RET_FAIL;
			}

			if (pstContents->alAntIdxArray[lTpSlotIdx] != NX_ANT_IDX_NULL)
			{
				nx_menuSearchChannel_Fixed_CreateTPListPopup(pstContents, lObjId);
			}
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_SatFixedSearchBtn:		// Search Button
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_UI_Proc",
						ulProperty, NX_PRISM_MENU_Search_UI_Proc, HANDLE_NULL, 0, 0, 0);
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Error("Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppTpSettingEnd (Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lSaveFlag)
{
	HINT32				lObjId = 0, lItemIdx = -1, lTpSlotIdx = 0;
	HERROR				hError = ERR_FAIL;
	Nx_TpSelectInfo_t 	stTpInfo;


	NX_APP_Debug("lItemIdx:[%d], lSaveFlag : [%d]\n", lItemIdx, lSaveFlag);

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(lObjId);
	switch (lItemIdx)
	{
		case eNxSettingsItem_SatFixedTP1:
		case eNxSettingsItem_SatFixedTP2:
		case eNxSettingsItem_SatFixedTP3:
		case eNxSettingsItem_SatFixedTP4:
			lTpSlotIdx = (pstContents->bPageOver) ? (lItemIdx - eNxSettingsItem_SatFixedTP1 + NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_PAGE) : (lItemIdx - eNxSettingsItem_SatFixedTP1) ;
			break;
		default:
			NX_APP_Error("Invalid lItemIdx:[%d]\n", lItemIdx);
			return	ONDK_MESSAGE_BREAK;
	}

	if (lTpSlotIdx >= NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI)
	{
		NX_APP_Error("Error!!! Out of range Max :(%d) lTpSlotIdx:(%d) \n", (HUINT32)NX_MENU_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI, lTpSlotIdx);
		return	ONDK_RET_FAIL;
	}

	HxSTD_memset(&stTpInfo, 0x00, sizeof(Nx_TpSelectInfo_t));
	hError = NX_ANTENNA_GetUserTPInfo ((eNxSat_ANTENAA_SLOT1 + lTpSlotIdx), &stTpInfo);
	if (hError == ERR_OK)
	{
		HxSTD_memcpy(&pstContents->astTpSelectInfo[lTpSlotIdx], &stTpInfo, sizeof(Nx_TpSelectInfo_t));
	}
	else
	{
		NX_APP_Error("ERROR!!! NX_ANTENNA_GetUserTPInfo() Fail!!! \n");
	}

	ONDK_GWM_APP_InvalidateObject(lObjId);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppDlgClicked(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	HINT32			lObjId = 0;
	HUINT32 		ulMenuItemId = 0, ulCurIdx = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;


	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_SwitchType)
			{
				nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
				lObjId = nx_menuSearchChannel_Fixed_ConvertItemIdxToObjId(eNxSettingsItem_SatFixedSwitchType);
				ulCurIdx = pstContents->eNewAntType;
				pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
				pstSubItem->ulIdxOfTextList = ulCurIdx;
				HxSTD_StrNCpy((char*)pstSubItem->szSubItemText, (char*)pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);

				/* Update subItem value */
				nx_menuSearchChannel_Fixed_GetMenuItemIdFormObjId(lObjId, &ulMenuItemId);
				NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pstSubItem->lSubItemValue));

				/* Update value */
				nx_menuSearchChannel_Fixed_UpdateSubItem(pstContents, lObjId);
			}
			else if (ulDlgWhoseId == eNxSettingsItem_DialogType_AntennaType)
			{
				nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
				nx_menuSearchChannel_Fixed_SetAntennaType(pstContents);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
		default:						// No
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_SwitchType)
			{
				nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
				pstContents->eNewAntType = pstContents->eAntType;
			}
			else if (ulDlgWhoseId == eNxSettingsItem_DialogType_AntennaType)
			{
				nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
				pstContents->eNewAntType = pstContents->eAntType;
			}
			else
			{
				NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
			}
			break;
	}

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgAppDlgCanceled(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxSettingsItem_DialogType_SwitchType)
	{
		nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
		pstContents->eNewAntType = pstContents->eAntType;
	}
	else if (ulDlgWhoseId == eNxSettingsItem_DialogType_AntennaType)
	{
		nx_menuSearchChannel_Fixed_DestroyChangeAntTypeDialog();
		pstContents->eNewAntType = pstContents->eAntType;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow ulDlgWhoseId ID : (0x%x) !!\n", ulDlgWhoseId);
	}

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuSearchChannel_Fixed_MsgGwmTimer(Nx_MenuSearchSat_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSearchChannel_Fixed_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Timer ID : (0x%x) !!\n", p1);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Search_SatFixedAntna_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					lItemId = -1;
	ONDK_Result_t			eResult = ONDK_RET_OK;
	NX_UI_CallerType_t		eCurOptType = eNX_UCT_None;
	Nx_MenuSearchSat_Contents_t	*pstContents = NULL;

	pstContents = nx_menuSearchChannel_Fixed_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuSearchChannel_Fixed_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSearchChannel_Fixed_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			nx_menuSearchChannel_Fixed_MsgGwmClicked(pstContents, p1);
			break;

		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_SCROLLRIGHT!\n");
			lItemId = nx_menuSearchChannel_Fixed_ConvertObjIdToItemIdx(p1);
			if (lItemId == eNxSettingsItem_SatFixedSwitchType)
			{
				nx_menuSearchChannel_Fixed_MsgGwmClicked(pstContents, p1);
			}
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSearchChannel_Fixed_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuSearchChannel_Fixed_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSearchChannel_Fixed_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_ANTENNA_SETTING_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_ANTENNA_SETTING_CHANGED!\n");
			nx_menuSearchChannel_Fixed_UpdateAntennaInfo(pstContents, p1, (HBOOL)p2);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuSearchChannel_Fixed_ChangedSubItemValue(pstContents, p1);
			break;

		case MSG_APP_SETTING_TP_SETTING_END:
			NX_APP_Message("MSG_APP_SETTING_TP_SETTING_END!\n");
			eResult = nx_menuSearchChannel_Fixed_MsgAppTpSettingEnd(pstContents, p1);
			break;

		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			eCurOptType = (NX_UI_CallerType_t)p1;
			if (eCurOptType == eNX_UCT_Menu)
			{
				nx_menuSearchChannel_Fixed_ChangeAntennaType(pstContents, p3);
			}
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED! || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuSearchChannel_Fixed_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CLICKED!\n");
			eResult = nx_menuSearchChannel_Fixed_MsgAppDlgCanceled(pstContents, p2);
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
