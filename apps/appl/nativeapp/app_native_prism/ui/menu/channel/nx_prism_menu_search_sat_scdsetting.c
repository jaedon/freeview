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
 * @file	  		nx_prism_menu_search_sat_scdsettings.c
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
#define	NX_MENU_SRCHSAT_SCD_TP_STRING_LEN						64

// Tuner Info
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_X						60
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_Y						368
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_W						179
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_H						139
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_TAB					11
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_STROKE					1

// Tuner Info Text - ��� ��ǥ
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_X					10
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_Y					20
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_W					159
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_H					28

#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_X				10
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_Y				66
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_W				159
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_H				28

#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_X				10
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_Y				94
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_W				159
#define	NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_H				20

// OK Button
#define	NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_X						170
#define	NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_Y						615
#define	NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_W						260
#define	NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_H						55

// CANCEL Button
#define	NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_X					440
#define	NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_Y					615
#define	NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_W					260
#define	NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_H					55

// Input Dialog Box
#define	NX_MENU_SRCHSAT_SCD_FREQUENCY_INTPUT_LEN				5
#define	NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_X						290
#define	NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_Y						266
#define	NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_W						700
#define	NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_H						188

#define	NX_MENU_SRCHSAT_SCD_CHANNEL_NUM							8
#define	NX_MENU_SRCHSAT_SCD_BANDFREQ_MAX_NUM					4
/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_DialogType_None = 0,
	eNxSettingsItem_DialogType_Selective,
	eNxSettingsItem_DialogType_Response,
	eNxSettingsItem_DialogType_Confirm
} Nx_SettingsItem_ScdSetting_DialogType_t;

typedef enum {
	eNxSettingsItem_Antenna1 = 0,
	eNxSettingsItem_Antenna2,
} Nx_SettingsItem_ScdSetting_Ant_t;

typedef enum {
	eNxSettingsItem_Tuner1 = 1,
	eNxSettingsItem_Tuner2,
	eNxSettingsItem_Tuner3,
	eNxSettingsItem_Tuner4
} Nx_SettingsItem_ScdSetting_Tuner_t;

typedef enum {
	eNxSettingsItem_None = 0,
	eNxSettingsItem_UserBand1,
	eNxSettingsItem_UserBand2,
	eNxSettingsItem_UserBand3,
	eNxSettingsItem_UserBand4,
	eNxSettingsItem_UserBand5,
	eNxSettingsItem_UserBand6,
	eNxSettingsItem_UserBand7,
	eNxSettingsItem_UserBand8
} Nx_SettingsItem_ScdSetting_UserBand_t;

typedef enum {
	eNxSettingsItem_ScdSet_Tuner,
	eNxSettingsItem_ScdSet_UserBand,
	eNxSettingsItem_ScdSet_BandFreq,
	eNxSettingsItem_ScdSet_Info,
	eNxSettingsItem_ScdSet_Ok,
	eNxSettingsItem_ScdSet_Cancel,
	eNxSettingsItem_ScdSet_Max
} Nx_SettingsItem_SatSetting_t;

typedef struct tag_Nx_MenuScdSetting_contents
{
	/**@breif Public value */
	HUINT32				ulCurFocus;
	HUINT32				ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8				ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32				*pAntUidList;
	HUINT32				ulNumAnt, ulNumTuner;

	DxAntennaType_e		eAntType;

	HINT32				nCurrAntUid;

	DxAntInfo_t			stAntInfo;
	DxAntInfo_t			stPrevAntInfo;

	HINT32				nCurrTuner;
	HINT32				nUserBandOfTuner1, nUserBandOfTuner2, nUserBandOfTuner3, nUserBandOfTuner4;
	HINT32				nBandFreqOfTuner1, nBandFreqOfTuner2, nBandFreqOfTuner3, nBandFreqOfTuner4;
} Nx_MenuScdSetting_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuScdSetting_Contents_t			s_stSatSettingsContent;
static Nx_PrismListDialog_Content_t			s_stPopupListSatSettingsConts;
static Nx_PrismDialogInput_Content_t		s_stDialogInputSatSettingsConts;
static NXUI_DialogInstance_t				s_stDialogMessageInstance;

static HINT32								s_ScdSet_ScdFreqTable[NX_MENU_SRCHSAT_SCD_CHANNEL_NUM + 1][NX_MENU_SRCHSAT_SCD_BANDFREQ_MAX_NUM] =
{
	{    0,    0,    0,    0 },			// None
	{ 1210, 1284, 1400,   -1 },			// Channel 1
	{ 1400, 1420, 1561,   -1 },			// Channel 2
	{ 1516, 1632, 1680,   -1 },			// Channel 3
	{ 1632,	1748, 2040,   -1 },			// Channel 4
	{ 1748,	  -1,    0,    0 },			// Channel 5
	{ 1864,   -1,    0,    0 },			// Channel 6
	{ 1980,   -1,    0,    0 },			// Channel 7
	{ 2096,   -1,    0,    0 }			// Channel 8
};



/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuScdSettings_GetDialogPopupContents(void);

static ONDK_Result_t	nx_menuScdSettings_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_CreateSelectiveDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message);
static ONDK_Result_t	nx_menuScdSettings_DestroySelectiveDialog(void);

static ONDK_Result_t	nx_menuScdSettings_ResponseDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_CreateResponseDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message);
static ONDK_Result_t	nx_menuScdSettings_DestroyResponseDialog(void);

static ONDK_Result_t	nx_menuScdSettings_ConfirmDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_CreateConfirmDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message);
static ONDK_Result_t	nx_menuScdSettings_DestroyConfirmDialog(void);

static ONDK_Result_t	nx_menuScdSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_DestroyListPopup(void);
static void 			nx_menuScdSettings_CreateListPopup(Nx_MenuScdSetting_Contents_t *pContents, HINT32 ulObjId);

static ONDK_Result_t	nx_menuScdSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_DestroyDialogInput(void);
static void				nx_menuScdSettings_CreateDialogInput(Nx_MenuScdSetting_Contents_t *pContents, HINT32 ulObjId);

//static void				nx_menuScdSettings_StopTune(Nx_MenuScdSetting_Contents_t *pstContents);
//static void				nx_menuScdSettings_TryTune(Nx_MenuScdSetting_Contents_t *pstContents);
static inline Nx_MenuScdSetting_Contents_t* nx_menuScdSettings_GetContents(void);
static inline HINT32	nx_menuScdSettings_GetDefaultFocus(void);
static HCHAR*			nx_menuScdSettings_GetMenuStrIdFormObjId(HINT32 lObjId);
static ONDK_Result_t	nx_menuScdSettings_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId);

static HINT32			nx_menuScdSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuScdSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuScdSettings_GetItemSupport(Nx_MenuScdSetting_Contents_t *pstContents, Nx_SettingsItem_SatSetting_t eNxSatSettingItem);
static void				nx_menuScdSettings_FindNextFocus(Nx_MenuScdSetting_Contents_t *pstContents, HBOOL bUp);
static ONDK_Result_t	nx_menuScdSettings_ChangeFocus(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lKey);

static ONDK_Result_t	nx_menuScdSettings_UpdateSubItem(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuScdSettings_CountBandFreqSubItem(HUINT32 ulUserBand, HUINT32 *pulFreqNum);
static HUINT8**			nx_menuScdSettings_MakeBandFreqTextList(HUINT32 ulUserBand, HUINT32 ulFreqNum);
static void				nx_menuScdSettings_FreeBandFreqTextList(HUINT32 ulNumItem, HUINT8 **ppucTextArray);
static ONDK_Result_t	nx_menuScdSettings_GetTunerNoStr(HINT32 nIndex, HUINT8 **pStr);
static ONDK_Result_t	nx_menuScdSettings_GetUserBandStr(HINT32 nUserBand, HUINT8 *pStr);
static ONDK_Result_t	nx_menuScdSettings_GetIndexBandFreq(HUINT32 ulUserBand, HUINT32	ulBandFreq, HUINT32 *pulListIdx);
static ONDK_Result_t	nx_menuScdSettings_GetUserBand(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 nCurrTuner, HINT32 *pnUserBand);
static ONDK_Result_t	nx_menuScdSettings_GetBandFreq(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 nCurrTuner, HINT32 *pnBandFreq);
static void				nx_menuScdSettings_ConvDxUserBandToNxUserBand(HINT32 nDxUserBand, HINT32 *nNxUserBand);
static void				nx_menuScdSettings_ConvNxUserBandToDxUserBand(HINT32 nNxUserBand, HINT32 *nDxUserBand);
static ONDK_Result_t	nx_menuScdSettings_UpdateBandFreqSubMenu(Nx_MenuScdSetting_Contents_t *pstContents, HBOOL bIsTunerChange);
static HBOOL			nx_menuScdSettings_IsOverlapUserBand(Nx_MenuScdSetting_Contents_t *pstContents, HINT32	nUserBand);
static HBOOL			nx_menuScdSettings_IsOverlapBandFreq(Nx_MenuScdSetting_Contents_t *pstContents, HINT32	nBandFreq);
static HBOOL			nx_menuScdSettings_IsAllNoneUserBand(Nx_MenuScdSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuScdSettings_SaveSetting(Nx_MenuScdSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuScdSettings_CancelSetting(Nx_MenuScdSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuScdSettings_ChangeItemEnableDisableMode(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable);

static ONDK_Result_t	nx_menuScdSettings_AddItemTuner(Nx_MenuScdSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuScdSettings_AddItemUserBand(Nx_MenuScdSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuScdSettings_AddItemBandFreq(Nx_MenuScdSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuScdSettings_InitScdSetting(Nx_MenuScdSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuScdSettings_InitSettingsItems(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 *pAntUidList, HUINT32 ulNumAnt);

static ONDK_Result_t	nx_menuScdSettings_DrawTunerInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_menuScdSettings_DrawDefaultUI(Nx_MenuScdSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuScdSettings_MsgGwmCreate(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuScdSettings_MsgGwmDestroy(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_MsgGwmHapiMsg(Nx_MenuScdSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuScdSettings_MsgGwmClicked(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuScdSettings_MsgGwmKeyDown(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_menuScdSettings_MsgAppValueChanged(Nx_MenuScdSetting_Contents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_menuScdSettings_MsgAppInputValueChange(Nx_MenuScdSetting_Contents_t *pstContents, HUINT32 ulInputValue);
static ONDK_Result_t	nx_menuScdSettings_MsgAppDlgClicked(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_menuScdSettings_MsgAppDlgCanceled(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_menuScdSettings_GetDialogPopupContents(void)
{
	return &s_stDialogMessageInstance;
}


static ONDK_Result_t	nx_menuScdSettings_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuScdSettings_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuScdSettings_CreateSelectiveDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuScdSettings_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxSettingsItem_DialogType_Selective;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)message);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuScdSettings_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuScdSettings_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_DestroySelectiveDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuScdSettings_SelectiveDialogProc);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_ResponseDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuScdSettings_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuScdSettings_CreateResponseDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuScdSettings_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxSettingsItem_DialogType_Response;
	ulDlgAttr = (NX_DLG_NO_BUTTON|NX_DLG_TIMEOUT_3SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)message);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuScdSettings_ResponseDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuScdSettings_ResponseDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_DestroyResponseDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuScdSettings_ResponseDialogProc);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_ConfirmDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_menuScdSettings_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_menuScdSettings_CreateConfirmDialog(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 message)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_menuScdSettings_GetDialogPopupContents();
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxSettingsItem_DialogType_Confirm;
	ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_2MIN|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)message);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuScdSettings_ConfirmDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuScdSettings_ConfirmDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_DestroyConfirmDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_menuScdSettings_ConfirmDialogProc);

	return ONDK_RET_OK;
}


#define _________Dialog_Input_Functions_________________________________________
static ONDK_Result_t	nx_menuScdSettings_DisplayDialogInput(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_DialogInput_ProtoProc(&s_stDialogInputSatSettingsConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuScdSettings_DestroyDialogInput(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuScdSettings_DisplayDialogInput);
}


static void		nx_menuScdSettings_CreateDialogInput(Nx_MenuScdSetting_Contents_t *pContents, HINT32 ulObjId)
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

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_X, NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_Y,
						NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_W, NX_MENU_SRCHSAT_SCD_DIALOG_INPUT_H);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_COMP_DialogInput_SetDialog (&s_stDialogInputSatSettingsConts,
							ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, ulAttribute, COL(C_T_Whistle_100), (HUINT8*)ONDK_GetString(RES_MESG_3998_ID), pstSubItem->lSubItemValue, NX_MENU_SRCHSAT_SCD_FREQUENCY_INTPUT_LEN);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuScdSettings_DisplayDialogInput", APP_DEFAULT_PRIORITY, nx_menuScdSettings_DisplayDialogInput, 0, 0, 0, 0);
}


#define _________PopUp_List_Functions____________________________________________

static ONDK_Result_t	nx_menuScdSettings_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSatSettingsConts, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuScdSettings_DestroyListPopup(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuScdSettings_DisplayListPopup);
}


static void		nx_menuScdSettings_CreateListPopup(Nx_MenuScdSetting_Contents_t *pContents, HINT32 ulObjId)
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
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuScdSettings_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuScdSettings_DisplayListPopup, 0, 0, 0, 0);
}

#define _________Normal_Functions____________________________________________

//static void		nx_menuScdSettings_StopTune(Nx_MenuScdSetting_Contents_t *pstContents)
//{
//	NX_Builder_StopScan(NX_SCANTYPE_TUNEONLY);
//	pstContents->bTuneStateLock = FALSE;
//	NX_COMMON_NOT_USED_PARAM(pstContents);
//}
//
//
//static void		nx_menuScdSettings_TryTune(Nx_MenuScdSetting_Contents_t *pstContents)
//{
//	NX_SCANPARAMS_t		stScanParam;
//	NX_SCANOPTIONS_t	stScanOption;
//	//NX_SCANPARAMS_t		stScanParams;
//	NX_TP_TuningInfo_t	*pstTuneInfo = NULL;
//
//	if (HxSTD_StrCmp((char*)pstContents->stAntInfo.satType, (char*)NX_SAT_SATNAME_NONE) == 0)
//	{
//		NX_APP_Error("[%s:%d] Antenna Type is NONE\n", __FUNCTION__, __LINE__);
//		return;
//	}
//
//	nx_menuScdSettings_StopTune(pstContents);
//
//	HxSTD_memset(&stScanParam, 0, sizeof(NX_SCANPARAMS_t));
//	HxSTD_memset(&stScanOption, 0, sizeof(NX_SCANOPTIONS_t));
//
//	NX_SEARCH_GetSearchOption(&stScanOption);
//
//	if (pstContents->bUserTpSelected == TRUE)
//	{
//		pstTuneInfo = &(pstContents->stUserTpInfo.stNxUserTpInfo);
//	}
//	else
//	{
//		pstTuneInfo = &(pstContents->pstTpInfoArray[pstContents->ulTpInfoIdx]);
//	}
//
//	NX_ANTENNA_SetInfo(pstContents->stAntInfo.uid, &(pstContents->stAntInfo));
//	NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam(eDxDELIVERY_TYPE_SAT, &(pstContents->stAntInfo), pstTuneInfo, &stScanParam);
//	NX_Builder_StartScan(NX_SCANTYPE_TUNEONLY, &stScanOption, NX_SEARCH_MAX_TP_TUNEONLY, &stScanParam);
//	pstContents->bTuneStateLock = TRUE;
//
//}


static inline Nx_MenuScdSetting_Contents_t* nx_menuScdSettings_GetContents(void)
{
	return	&s_stSatSettingsContent;
}


static inline HINT32	nx_menuScdSettings_GetDefaultFocus(void)
{
	return	eNxSettingsItem_ScdSet_Tuner;
}


static HCHAR*	nx_menuScdSettings_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuScdSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_ScdSet_Tuner:
			return RES_TUNER_ID;

		case eNxSettingsItem_ScdSet_UserBand:
			return RES_USER_BAND_ID;

		case eNxSettingsItem_ScdSet_BandFreq:
			return RES_BAND_FREQUENCY_ID;

		default:
			NX_APP_Error("Error!!! Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}


static ONDK_Result_t	nx_menuScdSettings_GetMenuItemIdFormObjId(HINT32 lObjId, HUINT32 *pulMenuItemId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuScdSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_ScdSet_Tuner:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_SCDSETTING_TUNER;
			break;
		case eNxSettingsItem_ScdSet_UserBand:
			*pulMenuItemId = NX_MENUITEM_ID_SEARCH_SCDSETTING_USERBAND;
			break;
		case eNxSettingsItem_ScdSet_BandFreq:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			break;
		default:
			*pulMenuItemId = NX_MENUITEM_ID_NONE;
			NX_APP_Error("Error!!! Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static HINT32	nx_menuScdSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SatelliteSetting);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_ScdSet_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuScdSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_ScdSet_Max))
	{
		return 0;
	}

	return	(eNxSetting_SatelliteSetting | lItemId);
}


static HBOOL	nx_menuScdSettings_GetItemSupport(Nx_MenuScdSetting_Contents_t *pstContents, Nx_SettingsItem_SatSetting_t eNxSatSettingItem)
{
	HBOOL	bSupport = TRUE;

	switch (eNxSatSettingItem)
	{
		case eNxSettingsItem_ScdSet_Tuner:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_ScdSet_UserBand:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_ScdSet_BandFreq:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}


static void		nx_menuScdSettings_FindNextFocus(Nx_MenuScdSetting_Contents_t *pstContents, HBOOL bUp)
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


static ONDK_Result_t	nx_menuScdSettings_ChangeFocus(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lKey)
{
	HINT32			lObjId = 0, lNextObjId = 0, lItemIdx = -1;

	lObjId = GWM_GetFocusedObjectID();
	lItemIdx = nx_menuScdSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lKey)
	{
		case KEY_ARROWUP:
			if (lItemIdx == eNxSettingsItem_ScdSet_Tuner)
			{
				lNextObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_ScdSet_Ok) || (lItemIdx == eNxSettingsItem_ScdSet_Cancel))
			{
				nx_menuScdSettings_FindNextFocus(pstContents, TRUE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case KEY_ARROWDOWN:
			if (lItemIdx == eNxSettingsItem_ScdSet_BandFreq)
			{
				lNextObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if ((lItemIdx == eNxSettingsItem_ScdSet_Ok) || (lItemIdx == eNxSettingsItem_ScdSet_Cancel))
			{
				nx_menuScdSettings_FindNextFocus(pstContents, FALSE);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			if (lItemIdx == eNxSettingsItem_ScdSet_Ok)
			{
				lNextObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Cancel);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			else if (lItemIdx == eNxSettingsItem_ScdSet_Cancel)
			{
				lNextObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Ok);
				ONDK_GWM_Obj_SetFocus (lNextObjId);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_UpdateTuner(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	pstContents->nCurrTuner = pstSubItem->lSubItemValue;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_UpdateUserBand(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch(pstContents->nCurrTuner)
	{
		case eNxSettingsItem_Tuner1:
			pstContents->nUserBandOfTuner1 = pstSubItem->lSubItemValue;
			break;
		case eNxSettingsItem_Tuner2:
			pstContents->nUserBandOfTuner2 = pstSubItem->lSubItemValue;
			break;
		case eNxSettingsItem_Tuner3:
			pstContents->nUserBandOfTuner3 = pstSubItem->lSubItemValue;
			break;
		case eNxSettingsItem_Tuner4:
			pstContents->nUserBandOfTuner4 = pstSubItem->lSubItemValue;
			break;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuScdSettings_UpdateBandFreq(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId)
{
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->nCurrTuner)
	{
		case eNxSettingsItem_Tuner1:
			pstContents->nBandFreqOfTuner1 = pstSubItem->lSubItemValue;
			if (pstContents->nUserBandOfTuner1 == eNxSettingsItem_None)
			{
				pstContents->nBandFreqOfTuner1 = 0;
			}
			break;
		case eNxSettingsItem_Tuner2:
			pstContents->nBandFreqOfTuner2 = pstSubItem->lSubItemValue;
			if (pstContents->nUserBandOfTuner2 == eNxSettingsItem_None)
			{
				pstContents->nBandFreqOfTuner2 = 0;
			}
			break;
		case eNxSettingsItem_Tuner3:
			pstContents->nBandFreqOfTuner3 = pstSubItem->lSubItemValue;
			if (pstContents->nUserBandOfTuner3 == eNxSettingsItem_None)
			{
				pstContents->nBandFreqOfTuner3 = 0;
			}
			break;
		case eNxSettingsItem_Tuner4:
			pstContents->nBandFreqOfTuner4 = pstSubItem->lSubItemValue;
			if (pstContents->nUserBandOfTuner4 == eNxSettingsItem_None)
			{
				pstContents->nBandFreqOfTuner4 = 0;
			}
			break;
	}


	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_UpdateSubItem(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	lItemIdx = nx_menuScdSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_ScdSet_Tuner:
			eResult = nx_menuScdSettings_UpdateTuner(pstContents, lObjId);
			break;

		case eNxSettingsItem_ScdSet_UserBand:
			eResult = nx_menuScdSettings_UpdateUserBand(pstContents, lObjId);
			break;

		case eNxSettingsItem_ScdSet_BandFreq:
			eResult = nx_menuScdSettings_UpdateBandFreq(pstContents, lObjId);
			break;

		default:
			return	ONDK_RET_FAIL;
	}

	if (eResult == ONDK_RET_OK)
	{
//		nx_menuScdSettings_TryTune(pstContents);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_CountBandFreqSubItem(HUINT32 ulUserBand, HUINT32 *pulFreqNum)
{
	HUINT32		ulCount = 0;

	if (pulFreqNum == NULL)
	{
		return ONDK_RET_FAIL;
	}
	if (ulUserBand > NX_MENU_SRCHSAT_SCD_CHANNEL_NUM)
	{
		return ONDK_RET_FAIL;
	}

	while (s_ScdSet_ScdFreqTable[ulUserBand][ulCount] > 0)
	{
		ulCount++;
	}

	ulCount++; // User define

	*pulFreqNum = ulCount;

	return ONDK_RET_OK;
}

static HUINT8**		nx_menuScdSettings_MakeBandFreqTextList(HUINT32 ulUserBand, HUINT32 ulFreqNum)
{
	HINT32		i = 0;
	HUINT8		**ppucTextArray = NULL;
	HUINT8		*pucText = NULL;

	ppucTextArray = (HUINT8**)NX_APP_Malloc(sizeof(HUINT8 *) * ulFreqNum);
	if (ppucTextArray == NULL)
	{
		return NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulFreqNum);
	for (i = 0; i < (ulFreqNum - 1); i++)
	{
		pucText = (HUINT8 *)NX_APP_Malloc(sizeof(HUINT8) * MAX_ITEM_TEXT_LENGTH);
		if (pucText == NULL)
		{
			for (i = 0; i < ulFreqNum; i++)
			{
				if (ppucTextArray[i] != NULL)
				{
					NX_APP_Free(ppucTextArray[i]);
				}
			}
			NX_APP_Free(ppucTextArray);

			return NULL;
		}

		HxSTD_memset(pucText, 0, sizeof(HUINT8) * MAX_ITEM_TEXT_LENGTH);
		pucText[0] = '\0';

		HxSTD_sprintf((char*)pucText, (char*)"%d", s_ScdSet_ScdFreqTable[ulUserBand][i]);
		ppucTextArray[i] = pucText;
	}

	pucText = (HUINT8 *)NX_APP_Malloc(sizeof(HUINT8) * MAX_ITEM_TEXT_LENGTH);
	if (pucText == NULL)
	{
		for (i = 0; i < ulFreqNum; i++)
		{
			if (ppucTextArray[i] != NULL)
			{
				NX_APP_Free(ppucTextArray[i]);
			}
		}
		NX_APP_Free(ppucTextArray);

		return NULL;
	}
	HxSTD_memset(pucText, 0, sizeof(HUINT8) * MAX_ITEM_TEXT_LENGTH);
	pucText[0] = '\0';
	HxSTD_sprintf((char*)pucText, (char*)"%s", ONDK_GetString(RES_USER_DEFINE_ID));
	ppucTextArray[i] = pucText;

	return ppucTextArray;
}


static void		nx_menuScdSettings_FreeBandFreqTextList(HUINT32 ulNumItem, HUINT8 **ppucTextArray)
{
	HUINT32		 ulIndex = 0;

	if ((ulNumItem == 0) || (ppucTextArray == NULL))
	{
		NX_APP_Error("Error!!! ((ulNumItem == 0) || (ppucTextArray == NULL)) Fail!! \n");
		return;
	}

	for (ulIndex = 0; ulIndex < ulNumItem; ulIndex++)
	{
		if (ppucTextArray[ulIndex] != NULL)
		{
			NX_APP_Free (ppucTextArray[ulIndex]);
		}
	}

	NX_APP_Free (ppucTextArray);
}


static ONDK_Result_t	nx_menuScdSettings_GetTunerNoStr(HINT32 nIndex, HUINT8 **pStr)
{
	nIndex += 1;

	switch(nIndex)
	{
		case eNxSettingsItem_Tuner1:
			*pStr = ONDK_GetString(RES_TUNER_1_ID);
			break;
		case eNxSettingsItem_Tuner2:
			*pStr = ONDK_GetString(RES_TUNER_2_ID);
			break;
		case eNxSettingsItem_Tuner3:
			*pStr = ONDK_GetString(RES_TUNER_3_ID);
			break;
		case eNxSettingsItem_Tuner4:
			*pStr = ONDK_GetString(RES_TUNER_4_ID);
			break;
		default:
			return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_GetUserBandStr(HINT32 nUserBand, HUINT8 *pStr)
{
	switch(nUserBand)
	{
		case eNxSettingsItem_UserBand1:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_1_ID));
			break;
		case eNxSettingsItem_UserBand2:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_2_ID));
			break;
		case eNxSettingsItem_UserBand3:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_3_ID));
			break;
		case eNxSettingsItem_UserBand4:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_4_ID));
			break;
		case eNxSettingsItem_UserBand5:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_5_ID));
			break;
		case eNxSettingsItem_UserBand6:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_6_ID));
			break;
		case eNxSettingsItem_UserBand7:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_7_ID));
			break;
		case eNxSettingsItem_UserBand8:
			HxSTD_sprintf((char*)pStr, (char*)"%s", ONDK_GetString(RES_USER_BAND_8_ID));
			break;
		default:
			break;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_GetIndexBandFreq(HUINT32 ulUserBand, HUINT32 ulBandFreq, HUINT32 *pulListIdx)
{
	HUINT32		i = 0;
	HBOOL		bFound = FALSE;

	for (i = 0; i < NX_MENU_SRCHSAT_SCD_BANDFREQ_MAX_NUM; i++)
	{
		if (ulBandFreq == s_ScdSet_ScdFreqTable[ulUserBand][i])
		{
			bFound = TRUE;
			break;
		}
	}

	*pulListIdx = i;

	if (bFound == TRUE)
	{
		return ONDK_RET_OK;
	}
	else
	{
		return ONDK_RET_FAIL;
	}
}


static ONDK_Result_t	nx_menuScdSettings_GetUserBand(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 nCurrTuner, HINT32 *pnUserBand)
{
	HINT32	nUserBand = 0;

	switch (nCurrTuner)
	{
		case eNxSettingsItem_Tuner1:
			nUserBand = pstContents->nUserBandOfTuner1;
			break;
		case eNxSettingsItem_Tuner2:
			nUserBand = pstContents->nUserBandOfTuner2;
			break;
		case eNxSettingsItem_Tuner3:
			nUserBand = pstContents->nUserBandOfTuner3;
			break;
		case eNxSettingsItem_Tuner4:
			nUserBand = pstContents->nUserBandOfTuner4;
			break;
		default:
			return ONDK_RET_FAIL;
	}
	*pnUserBand = nUserBand;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_GetBandFreq(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 nCurrTuner, HINT32 *pnBandFreq)
{
	HINT32	nBandFreq = 0;

	switch (nCurrTuner)
	{
		case eNxSettingsItem_Tuner1:
			nBandFreq = pstContents->nBandFreqOfTuner1;
			break;
		case eNxSettingsItem_Tuner2:
			nBandFreq = pstContents->nBandFreqOfTuner2;
			break;
		case eNxSettingsItem_Tuner3:
			nBandFreq = pstContents->nBandFreqOfTuner3;
			break;
		case eNxSettingsItem_Tuner4:
			nBandFreq = pstContents->nBandFreqOfTuner4;
			break;
		default:
			return ONDK_RET_FAIL;
	}
	*pnBandFreq = nBandFreq;

	return ONDK_RET_OK;
}


static void				nx_menuScdSettings_ConvDxUserBandToNxUserBand(HINT32 nDxUserBand, HINT32 *nNxUserBand)
{
	HINT32	nUserBand = 0;

	switch(nDxUserBand)
	{
		case 0:
			nUserBand = eNxSettingsItem_UserBand1;
			break;
		case 1:
			nUserBand = eNxSettingsItem_UserBand2;
			break;
		case 2:
			nUserBand = eNxSettingsItem_UserBand3;
			break;
		case 3:
			nUserBand = eNxSettingsItem_UserBand4;
			break;
		case 4:
			nUserBand = eNxSettingsItem_UserBand5;
			break;
		case 5:
			nUserBand = eNxSettingsItem_UserBand6;
			break;
		case 6:
			nUserBand = eNxSettingsItem_UserBand7;
			break;
		case 7:
			nUserBand = eNxSettingsItem_UserBand8;
			break;
	}

	*nNxUserBand = nUserBand;
}


static void				nx_menuScdSettings_ConvNxUserBandToDxUserBand(HINT32 nNxUserBand, HINT32 *nDxUserBand)
{
	HINT32	nUserBand = 0;

	switch(nNxUserBand)
	{
		case eNxSettingsItem_None:
		case eNxSettingsItem_UserBand1:
			nUserBand = 0;
			break;
		case eNxSettingsItem_UserBand2:
			nUserBand = 1;
			break;
		case eNxSettingsItem_UserBand3:
			nUserBand = 2;
			break;
		case eNxSettingsItem_UserBand4:
			nUserBand = 3;
			break;
		case eNxSettingsItem_UserBand5:
			nUserBand = 4;
			break;
		case eNxSettingsItem_UserBand6:
			nUserBand = 5;
			break;
		case eNxSettingsItem_UserBand7:
			nUserBand = 6;
			break;
		case eNxSettingsItem_UserBand8:
			nUserBand = 7;
			break;
	}

	*nDxUserBand = nUserBand;
}


static ONDK_Result_t	nx_menuScdSettings_UpdateBandFreqSubMenu(Nx_MenuScdSetting_Contents_t *pstContents, HBOOL bIsTunerChange)
{
	HINT32						nBandFreqId = -1, nUserBand = 0;
	HUINT32						ulFreqNum = 0, ulBandFreqIdx = 0;
	HUINT8						**ppListOfItem = NULL;
	Nx_SettingsLeafSubItem_t 	*pstBandFreqSubItem =  NULL;
	ONDK_Result_t				eResult = ONDK_RET_OK;

	nx_menuScdSettings_GetUserBand(pstContents, pstContents->nCurrTuner, &nUserBand);

	/* Load new submenu */
	nx_menuScdSettings_CountBandFreqSubItem(nUserBand, &ulFreqNum);
	ppListOfItem = nx_menuScdSettings_MakeBandFreqTextList(nUserBand, ulFreqNum);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuScdSettings_MakeBandFreqTextList() nUserBand : (0x%x)!!\n", nUserBand);
		return	ONDK_RET_FAIL;
	}

	/* Get Band Freq submenu */
	nBandFreqId= nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_BandFreq);
	pstBandFreqSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nBandFreqId);
	if (pstBandFreqSubItem != NULL)
	{
		/* Clear prev submenu */
		eResult = NX_PRISM_AppContents_ClearSubitemByObjId(pstContents, nBandFreqId);
		if (eResult != ONDK_RET_OK)
		{
			if (NULL != ppListOfItem)
			{
				nx_menuScdSettings_FreeBandFreqTextList(ulFreqNum, ppListOfItem);
			}
			return eResult;
		}

		/* Update submenu */
		pstBandFreqSubItem->ulNumOfItemTextList = ulFreqNum;
		if (bIsTunerChange == TRUE)
		{
			switch(pstContents->nCurrTuner)
			{
				case eNxSettingsItem_Tuner1:
					eResult = nx_menuScdSettings_GetIndexBandFreq(nUserBand, pstContents->nBandFreqOfTuner1, &ulBandFreqIdx);
					break;
				case eNxSettingsItem_Tuner2:
					eResult = nx_menuScdSettings_GetIndexBandFreq(nUserBand, pstContents->nBandFreqOfTuner2, &ulBandFreqIdx);
					break;
				case eNxSettingsItem_Tuner3:
					eResult = nx_menuScdSettings_GetIndexBandFreq(nUserBand, pstContents->nBandFreqOfTuner3, &ulBandFreqIdx);
					break;
				case eNxSettingsItem_Tuner4:
					eResult = nx_menuScdSettings_GetIndexBandFreq(nUserBand, pstContents->nBandFreqOfTuner4, &ulBandFreqIdx);
					break;
				default:
					if (NULL != ppListOfItem)
					{
						nx_menuScdSettings_FreeBandFreqTextList(ulFreqNum, ppListOfItem);
					}
					return ONDK_RET_FAIL;
			}
			if (eResult != ONDK_RET_OK)
			{
				ulBandFreqIdx = 0;
			}
			pstBandFreqSubItem->lSubItemValue = s_ScdSet_ScdFreqTable[nUserBand][ulBandFreqIdx];
			pstBandFreqSubItem->ulIdxOfTextList = ulBandFreqIdx;
		}
		else
		{
			pstBandFreqSubItem->lSubItemValue = s_ScdSet_ScdFreqTable[nUserBand][0];
			pstBandFreqSubItem->ulIdxOfTextList = 0;
		}

		pstBandFreqSubItem->szItemTextList = ppListOfItem;

		HxSTD_sprintf((HCHAR*)pstBandFreqSubItem->szSubItemText, "%s", (HCHAR*)pstBandFreqSubItem->szItemTextList[pstBandFreqSubItem->ulIdxOfTextList]);

		/* Update value */
		nx_menuScdSettings_UpdateSubItem(pstContents, nBandFreqId);
	}
	else
	{
		if (NULL != ppListOfItem)
		{
			nx_menuScdSettings_FreeBandFreqTextList(ulFreqNum, ppListOfItem);
		}
	}

	return ONDK_RET_OK;
}


static HBOOL	nx_menuScdSettings_IsOverlapUserBand(Nx_MenuScdSetting_Contents_t *pstContents, HINT32	nUserBand)
{
	HINT32	i = 0;
	HBOOL	isOverlap = FALSE;

	if (nUserBand == eNxSettingsItem_None)
	{
		return FALSE;
	}

	for (i = eNxSettingsItem_Tuner1; i <= pstContents->ulNumTuner; i++)
	{

		switch(i)
		{
			case eNxSettingsItem_Tuner1:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner1 && nUserBand == pstContents->nUserBandOfTuner1)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner2:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner2 && nUserBand == pstContents->nUserBandOfTuner2)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner3:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner3 && nUserBand == pstContents->nUserBandOfTuner3)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner4:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner4 && nUserBand == pstContents->nUserBandOfTuner4)
				{
					isOverlap = TRUE;
				}
				break;
			default:
				NX_APP_Error("Error!!! nx_menuScdSettings_IsOverlapUserBand pstContents->ulNumTuner: %d!!\n", pstContents->ulNumTuner);
				break;
		}
	}

	return isOverlap;
}


static HBOOL	nx_menuScdSettings_IsOverlapBandFreq(Nx_MenuScdSetting_Contents_t *pstContents, HINT32	nBandFreq)
{
	HINT32	i = 0;
	HBOOL	isOverlap = FALSE;

	for (i = eNxSettingsItem_Tuner1; i <= pstContents->ulNumTuner; i++)
	{
		switch(i)
		{
			case eNxSettingsItem_Tuner1:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner1 && nBandFreq == pstContents->nBandFreqOfTuner1)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner2:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner2 && nBandFreq == pstContents->nBandFreqOfTuner2)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner3:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner3 && nBandFreq == pstContents->nBandFreqOfTuner3)
				{
					isOverlap = TRUE;
				}
				break;
			case eNxSettingsItem_Tuner4:
				if (pstContents->nCurrTuner != eNxSettingsItem_Tuner4 && nBandFreq == pstContents->nBandFreqOfTuner4)
				{
					isOverlap = TRUE;
				}
				break;
			default:
				NX_APP_Error("Error!!! nx_menuScdSettings_IsOverlapUserBand pstContents->ulNumTuner: %d!!\n", pstContents->ulNumTuner);
				break;
		}
	}


	return isOverlap;
}


static HBOOL			nx_menuScdSettings_IsAllNoneUserBand(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HINT32	i = 0;
	HBOOL	bIsAllNone = TRUE;

	for (i = eNxSettingsItem_Tuner1; i <= pstContents->ulNumTuner; i++)
	{
		switch(i)
		{
			case eNxSettingsItem_Tuner1:
				if (pstContents->nUserBandOfTuner1 != eNxSettingsItem_None)
				{
					bIsAllNone = FALSE;
				}
				break;
			case eNxSettingsItem_Tuner2:
				if(pstContents->nUserBandOfTuner2 != eNxSettingsItem_None)
				{
					bIsAllNone = FALSE;
				}
				break;
			case eNxSettingsItem_Tuner3:
				if(pstContents->nUserBandOfTuner3 != eNxSettingsItem_None)
				{
					bIsAllNone = FALSE;
				}
				break;
			case eNxSettingsItem_Tuner4:
				if(pstContents->nUserBandOfTuner4 != eNxSettingsItem_None)
				{
					bIsAllNone = FALSE;
				}
				break;
			default:
				NX_APP_Error("Error!!! nx_menuScdSettings_IsOverlapUserBand pstContents->ulNumTuner: %d!!\n", pstContents->ulNumTuner);
				break;
		}
	}

	return bIsAllNone;
}


static ONDK_Result_t	nx_menuScdSettings_SaveSetting(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HINT32			i = 0, nAntUid = 0;
	DxAntInfo_t		stAntInfo;
	HERROR			hError = ERR_OK;

	for (i = 0; i < pstContents->ulNumAnt; i++)
	{
		nAntUid = pstContents->pAntUidList[i];
		hError = NX_ANTENNA_GetInfo(nAntUid, &stAntInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_ANTENNA_MakeDefaultAntennaInfo() Fail!! - Antenna Type:(%d)\n", pstContents->eAntType);
			return	ONDK_RET_FAIL;
		}

		stAntInfo.antType = pstContents->eAntType;
		switch(i)
		{
			case eNxSettingsItem_Antenna1:
				stAntInfo.info.scd.scdInput = eDxSCD_INPUT_SCD_A;
				break;
			case eNxSettingsItem_Antenna2:
				stAntInfo.info.scd.scdInput = eDxSCD_INPUT_SCD_B;
				break;
		}
		nx_menuScdSettings_ConvNxUserBandToDxUserBand(pstContents->nUserBandOfTuner1, &(stAntInfo.info.scd.ulScdUserBandNum));
		nx_menuScdSettings_ConvNxUserBandToDxUserBand(pstContents->nUserBandOfTuner2, &(stAntInfo.info.scd.ulScdUserBandNum1));
		nx_menuScdSettings_ConvNxUserBandToDxUserBand(pstContents->nUserBandOfTuner3, &(stAntInfo.info.scd.ulScdUserBandNum2));
		nx_menuScdSettings_ConvNxUserBandToDxUserBand(pstContents->nUserBandOfTuner4, &(stAntInfo.info.scd.ulScdUserBandNum3));
		stAntInfo.info.scd.ulScdUserBandFreq  = pstContents->nBandFreqOfTuner1;
		stAntInfo.info.scd.ulScdUserBandFreq1 = pstContents->nBandFreqOfTuner2;
		stAntInfo.info.scd.ulScdUserBandFreq2 = pstContents->nBandFreqOfTuner3;
		stAntInfo.info.scd.ulScdUserBandFreq3 = pstContents->nBandFreqOfTuner4;

		NX_ANTENNA_SetInfo(stAntInfo.uid, &stAntInfo);
		ONDK_GWM_SendMessage(NULL, MSG_APP_SETTING_ANTENNA_SETTING_CHANGED, (HINT32)NULL, stAntInfo.uid, 0, 0);
	}

	NX_ANTENNA_Sync();


	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_CancelSetting(Nx_MenuScdSetting_Contents_t *pstContents)
{

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuScdSettings_ChangeItemEnableDisableMode(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuScdSettings_ConvertItemIdxToObjId(lItemIdx);
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


static ONDK_Result_t	nx_menuScdSettings_AddItemTuner(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HINT32				i = 0;
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL, **ppTmpListOfItem = NULL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuScdSettings_GetItemSupport(pstContents, eNxSettingsItem_ScdSet_Tuner);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ScdSet_Tuner(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Tuner);

	// Set Main String
	pMainStrId = nx_menuScdSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuScdSettings_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	/* List�� �����. */
	ppTmpListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppTmpListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	/* List�� tuner ������ŭ �����Ѵ�. */
	ppListOfItem = NX_APP_Malloc(sizeof(HUINT8 *) * pstContents->ulNumTuner);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!! \n");

        if(NULL != ppTmpListOfItem)
        {
        	for (i = pstContents->ulNumTuner; i < ulItems; i++)
        	{
        		NX_APP_Free(ppTmpListOfItem[i]);
        	}
        	NX_APP_Free(ppTmpListOfItem);
        }

		return ONDK_RET_FAIL;
	}
	HxSTD_memset(ppListOfItem, 0, sizeof(HUINT8 *) * pstContents->ulNumTuner);
	HxSTD_memcpy(ppListOfItem, ppTmpListOfItem, sizeof(HUINT8 *) * pstContents->ulNumTuner);
	for (i = pstContents->ulNumTuner; i < ulItems; i++)
	{
		NX_APP_Free(ppTmpListOfItem[i]);
	}
	NX_APP_Free(ppTmpListOfItem);


	stSubItem.ulNumOfItemTextList = pstContents->ulNumTuner;
	stSubItem.szItemTextList = ppListOfItem;

	/* List�� Index�� ���´�. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->nCurrTuner, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->nCurrTuner;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_AddItemUserBand(Nx_MenuScdSetting_Contents_t *pstContents)
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

	bSupport = nx_menuScdSettings_GetItemSupport(pstContents, eNxSettingsItem_ScdSet_UserBand);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ScdSet_UserBand(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_UserBand);

	// Set Main String
	pMainStrId = nx_menuScdSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	/* Count SubItems */
	nx_menuScdSettings_GetMenuItemIdFormObjId(stSubItem.ulId, &ulMenuItemId);
	eResult = NX_PRISM_MENU_Menuitem_CountSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	/* List�� �����. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List�� Index�� ���´�. */
	eResult = NX_PRISM_MENU_MenuItem_GetIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)pstContents->nUserBandOfTuner1, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)pstContents->nUserBandOfTuner1;
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}
static ONDK_Result_t	nx_menuScdSettings_AddItemBandFreq(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HBOOL				bSupport = FALSE;
	HCHAR				*pMainStrId = NULL;
	HUINT32				ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	HUINT8				**ppListOfItem = NULL;
	HINT32				nUserBand = 0;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return	GWM_RET_SOURCE_NULL;
	}

	bSupport = nx_menuScdSettings_GetItemSupport(pstContents, eNxSettingsItem_ScdSet_BandFreq);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_ScdSet_BandFreq(%d)\n", bSupport);
		return	ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_BandFreq);

	// Set Main String
	pMainStrId = nx_menuScdSettings_GetMenuStrIdFormObjId(stSubItem.ulId);
	if (pMainStrId != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", ONDK_GetString(pMainStrId));
	}

	nx_menuScdSettings_GetUserBand(pstContents, pstContents->nCurrTuner, &nUserBand);

	/* Count SubItems */
	eResult = nx_menuScdSettings_CountBandFreqSubItem(nUserBand, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountSubitems() !!\n");
		return	eResult;
	}

	/* List�� �����. */
	ppListOfItem = nx_menuScdSettings_MakeBandFreqTextList(nUserBand, ulItems);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId() ulMenuId : (0x%x)!!\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	stSubItem.ulNumOfItemTextList = ulItems;
	stSubItem.szItemTextList = ppListOfItem;

	/* List�� Index�� ���´�. */
	eResult = nx_menuScdSettings_GetIndexBandFreq(pstContents->nUserBandOfTuner1, pstContents->nBandFreqOfTuner1, &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	stSubItem.lSubItemValue = (HINT32)s_ScdSet_ScdFreqTable[nUserBand][ulListIdx];
	stSubItem.ulIdxOfTextList = ulListIdx;

	HxSTD_StrNCpy((char*)stSubItem.szSubItemText, (char*)stSubItem.szItemTextList[ulListIdx], MAX_ELEMENT_TEXT_LENGTH);

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);
	return ONDK_RET_OK;
}



static ONDK_Result_t	nx_menuScdSettings_InitScdSetting(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT32			nPrevUid = NX_ANT_IDX_NULL;
	DxAntInfo_t		stAntInfo;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	nPrevUid = pstContents->nCurrAntUid;
	HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));

	if (pstContents->nCurrAntUid == NX_ANT_IDX_NULL)
	{
		hError = NX_ANTENNA_MakeDefaultAntennaInfo(pstContents->eAntType, &stAntInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_ANTENNA_MakeDefaultAntennaInfo() Fail!! - Antenna Type:(%d)\n", pstContents->eAntType);
			return	ONDK_RET_FAIL;
		}

		pstContents->nCurrAntUid = stAntInfo.uid;
		pstContents->ulNumAnt++;
	}
	else
	{
		hError = NX_ANTENNA_GetInfo(pstContents->nCurrAntUid, &stAntInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_ANTENNA_MakeDefaultAntennaInfo() Fail!! - Antenna Type:(%d)\n", pstContents->eAntType);
			return	ONDK_RET_FAIL;
		}
	}

	NX_SYSTEM_GetTunerDeviceNum(&pstContents->ulNumTuner);

	nx_menuScdSettings_ConvDxUserBandToNxUserBand(stAntInfo.info.scd.ulScdUserBandNum, &(pstContents->nUserBandOfTuner1));
	nx_menuScdSettings_ConvDxUserBandToNxUserBand(stAntInfo.info.scd.ulScdUserBandNum1, &(pstContents->nUserBandOfTuner2));
	nx_menuScdSettings_ConvDxUserBandToNxUserBand(stAntInfo.info.scd.ulScdUserBandNum2, &(pstContents->nUserBandOfTuner3));
	nx_menuScdSettings_ConvDxUserBandToNxUserBand(stAntInfo.info.scd.ulScdUserBandNum3, &(pstContents->nUserBandOfTuner4));
	pstContents->nBandFreqOfTuner1 = stAntInfo.info.scd.ulScdUserBandFreq;
	pstContents->nBandFreqOfTuner2 = stAntInfo.info.scd.ulScdUserBandFreq1;
	pstContents->nBandFreqOfTuner3 = stAntInfo.info.scd.ulScdUserBandFreq2;
	pstContents->nBandFreqOfTuner4 = stAntInfo.info.scd.ulScdUserBandFreq3;

	if (pstContents->nBandFreqOfTuner1 == 0)
	{
		pstContents->nUserBandOfTuner1 = eNxSettingsItem_None;
	}
	if (pstContents->nBandFreqOfTuner2 == 0)
	{
		pstContents->nUserBandOfTuner2 = eNxSettingsItem_None;
	}
	if (pstContents->nBandFreqOfTuner3 == 0)
	{
		pstContents->nUserBandOfTuner3 = eNxSettingsItem_None;
	}
	if (pstContents->nBandFreqOfTuner4 == 0)
	{
		pstContents->nUserBandOfTuner4 = eNxSettingsItem_None;
	}

	pstContents->nCurrTuner = eNxSettingsItem_Tuner1;
	HxSTD_memcpy(&(pstContents->stAntInfo), &stAntInfo, sizeof(DxAntInfo_t));
	HxSTD_memcpy(&(pstContents->stPrevAntInfo), &stAntInfo, sizeof(DxAntInfo_t));
	pstContents->stPrevAntInfo.uid = nPrevUid;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_InitSettingsItems(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 *pAntUidList, HUINT32 ulNumAnt)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

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

	pstContents->pAntUidList = pAntUidList;
	pstContents->ulNumAnt = ulNumAnt;
	if (ulNumAnt == 0)
	{
		pstContents->nCurrAntUid = NX_ANT_IDX_NULL;
	}
	else
	{
		pstContents->nCurrAntUid = pAntUidList[0];
	}
	pstContents->eAntType = eDxANT_TYPE_SCD;

	// Init Item Value
	eResult = nx_menuScdSettings_InitScdSetting(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuScdSettings_InitScdSetting() !!\n");
		return eResult;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SCD_SETTING_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->ucTitle[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';

	// Default Function
	pstContents->ulCurFocus = nx_menuScdSettings_GetDefaultFocus();

	nx_menuScdSettings_AddItemTuner(pstContents);
	nx_menuScdSettings_AddItemUserBand(pstContents);
	nx_menuScdSettings_AddItemBandFreq(pstContents);

	if (pstContents->nUserBandOfTuner1 == eNxSettingsItem_None)
	{
		nx_menuScdSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_ScdSet_BandFreq, FALSE);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_DrawTunerInfo(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32			i = 0, x = 0, y = 0;
	ONDKFontHandle	*hFontBoldHandle = NULL, hFontNormalHandle = NULL;
	ONDK_Color_t	fontColor = 0x00000000;
	HUINT8 			*pTunerNoStr = NULL, pUserBandStr[DxNAME_LEN] = {0, }, pBandFreqStr[DxNAME_LEN] = {0, };
	Nx_MenuScdSetting_Contents_t	*pstContents = NULL;
	HINT32			nUserBand = 0, nBandFreq = 0;

	pstContents = nx_menuScdSettings_GetContents();
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	/* init coordinate */
	y = area->y1 + rect->y;

	hFontBoldHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_20);
	hFontNormalHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);
	fontColor = COL(C_T_Gong03);


	for (i = 0; i < pstContents->ulNumTuner; i++)
	{
		x = NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_X + (NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_W + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_TAB) * i;
		nx_menuScdSettings_GetUserBand(pstContents, i + 1, &nUserBand);
		nx_menuScdSettings_GetBandFreq(pstContents, i + 1, &nBandFreq);

		HxSTD_memset(pUserBandStr, 0, sizeof(HUINT8) * DxNAME_LEN);
		HxSTD_memset(pBandFreqStr, 0, sizeof(HUINT8) * DxNAME_LEN);
		if (nUserBand == eNxSettingsItem_None)
		{
			HxSTD_sprintf((char*)pUserBandStr, (char*)"%s", (HCHAR*)ONDK_GetString(RES_NONE_01_ID));
			HxSTD_sprintf((char*)pBandFreqStr, (char*)"%s", (HCHAR*)"-");
		}
		else
		{
			nx_menuScdSettings_GetUserBandStr(nUserBand, pUserBandStr);
			HxSTD_sprintf((char*)pBandFreqStr, (char*)"%dMhz", nBandFreq);
		}

		// Draw box
		ONDK_DRAW_StrokeRect(screen, x, y, x + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_W, y + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_H, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_STROKE, COL(C_G_Whistle_15));

		// Draw No
		nx_menuScdSettings_GetTunerNoStr(i, &pTunerNoStr);
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontBoldHandle, (HCHAR*)pTunerNoStr, x + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_X, y + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_Y, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_NO_W, fontColor, ONDKSTF_CENTER);

		// Draw UserBand
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontNormalHandle, (HCHAR*)pUserBandStr, x + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_X, y + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_Y, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_USERBAND_W, fontColor, ONDKSTF_CENTER);

		// Draw BandFreq
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontNormalHandle, (HCHAR*)pBandFreqStr, x + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_X, y + NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_Y, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_BANDFREQ_W, fontColor, ONDKSTF_CENTER);
	}

	ONDK_FONT_Release(hFontBoldHandle);
	ONDK_FONT_Release(hFontNormalHandle);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_DrawDefaultUI(Nx_MenuScdSetting_Contents_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	NX_PRISM_COMP_Settings_DrawBackgroundPopUp();
	NX_PRISM_COMP_Settings_DrawTitlePopUp(pstContents->ucTitle);
	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1_Arrow);

	// Tuner Info
	stRect = ONDK_Rect(0, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_Y, 1280, NX_MENU_SRCHSAT_SCD_SCDSET_TUNER_H);
	lObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Info);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuScdSettings_DrawTunerInfo);

	// Button
	stRect = ONDK_Rect(NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_X, NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_Y, NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_W, NX_MENU_SRCHSAT_SCD_SCDSET_OK_BTN_H);
	lObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Ok);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_OK_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	stRect = ONDK_Rect(NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_X, NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_Y, NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_W, NX_MENU_SRCHSAT_SCD_SCDSET_CANCEL_BTN_H);
	lObjId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_Cancel);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (char*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(pstContents->subItems[pstContents->ulCurFocus].ulId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgGwmCreate(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	eResult = nx_menuScdSettings_InitSettingsItems(pstContents, (HINT32 *)p1, (HUINT32)p2);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuScdSettings_InitSettingsItems() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	// Draw Menu Item
	nx_menuScdSettings_DrawDefaultUI(pstContents);

	// Try Tune
//	NX_Builder_AddNotiSignalInfo();
//	nx_menuScdSettings_TryTune(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgGwmDestroy(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

//	nx_menuScdSettings_StopTune(pstContents);

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuScdSetting_Contents_t));
	nx_menuScdSettings_DestroyListPopup();
	nx_menuScdSettings_DestroyDialogInput();
	NX_Builder_ReleaseNotiSignalInfo();

	NX_PRISM_LIVE_ServiceStartByLastService();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgGwmHapiMsg(Nx_MenuScdSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuScdSettings_MsgGwmClicked(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32			lItemIdx = -1;
	HBOOL			bIsAllNone = FALSE;

	lItemIdx = nx_menuScdSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_ScdSet_Tuner:
		case eNxSettingsItem_ScdSet_UserBand:
		case eNxSettingsItem_ScdSet_BandFreq:
			nx_menuScdSettings_CreateListPopup(pstContents, lObjId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_ScdSet_Ok:
			bIsAllNone = nx_menuScdSettings_IsAllNoneUserBand(pstContents);
			if (bIsAllNone == TRUE)
			{
				nx_menuScdSettings_CreateConfirmDialog(pstContents, (HINT32)ONDK_GetString(RES_MESG_5326_ID));
			}
			else
			{
				nx_menuScdSettings_SaveSetting(pstContents);
				ONDK_GWM_APP_Destroy(0);
			}

			return	ONDK_MESSAGE_BREAK;
		case eNxSettingsItem_ScdSet_Cancel:
			nx_menuScdSettings_CancelSetting(pstContents);
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		default:
			return	ONDK_RET_OK;
	}
}


static ONDK_Result_t	nx_menuScdSettings_MsgGwmKeyDown(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		//case KEY_ARROWLEFT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_MENU:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OK:
			break;

		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			return	nx_menuScdSettings_ChangeFocus(pstContents, lKeyId);

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgCoreSignalState(HUINT32 ulSignalStrength, HUINT32 ulSignalQuality)
{
	NX_PRISM_COMP_Settings_UpdateDrawStrenghtBar(ulSignalStrength);
	NX_PRISM_COMP_Settings_UpdateDrawQualityBar(ulSignalQuality);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgAppInputValueChange(Nx_MenuScdSetting_Contents_t *pstContents, HUINT32 ulInputValue)
{
	HINT32			lObjId = 0, i = 0, nUserBand = 0;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = GWM_GetFocusedObjectID();
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, lObjId);
		return	ONDK_RET_FAIL;
	}

	nx_menuScdSettings_GetUserBand(pstContents, pstContents->nCurrTuner, &nUserBand);

	/* Find user define index */
	while(s_ScdSet_ScdFreqTable[nUserBand][i] != -1)
	{
		i++;
	}

	pstSubItem->lSubItemValue = (HINT32)ulInputValue;
	pstSubItem->ulIdxOfTextList = i;
	HxSTD_memset(pstSubItem->szSubItemText, 0x00, MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_sprintf((char*)pstSubItem->szSubItemText, "%u", ulInputValue);

	nx_menuScdSettings_UpdateSubItem(pstContents, lObjId);

//	nx_menuScdSettings_TryTune(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgAppValueChanged(Nx_MenuScdSetting_Contents_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32			nObjId = 0, nItemIdx = -1;
	HUINT32			ulPrevSelectedIdx = 0, ulMenuItemId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	nObjId = GWM_GetFocusedObjectID();
	nItemIdx = nx_menuScdSettings_ConvertObjIdToItemIdx(nObjId);

	switch (nItemIdx)
	{
		case eNxSettingsItem_ScdSet_Tuner:
			{
				HINT32	nValue = -1, nUserBandId = -1, nBandFreqId = -1;
				HUINT32	ulUserBandMenuItemId = -1;
				Nx_SettingsLeafSubItem_t *pstUserBandSubItem =  NULL, *pstBandFreqSubItem = NULL;

				pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nObjId);
				if (pstSubItem == NULL)
				{
					NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, nObjId);
					return	ONDK_RET_FAIL;
				}

				ulPrevSelectedIdx = pstSubItem->ulIdxOfTextList;
				pstSubItem->ulIdxOfTextList = ulCurIdx;
				HxSTD_sprintf((char*)pstSubItem->szSubItemText, (char*)"%s", pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList]);

				/* Update subItem value */
				nx_menuScdSettings_GetMenuItemIdFormObjId(nObjId, &ulMenuItemId);
				NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &nValue);
				pstSubItem->lSubItemValue = nValue;

				if (pstContents->nCurrTuner != nValue)
				{
					/* Update tuner value */
					nx_menuScdSettings_UpdateSubItem(pstContents, nObjId);

					/* Enable Band Freq */
					nBandFreqId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_BandFreq);
					pstBandFreqSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nBandFreqId);
					if (pstBandFreqSubItem == NULL)
					{
						return ONDK_RET_FAIL;
					}
					if (pstBandFreqSubItem->bDisable == TRUE)
					{
						pstBandFreqSubItem->bDisable = FALSE;
						ONDK_GWM_Obj_EnableObject(nBandFreqId);
					}

					/* Update UserBand */
					nUserBandId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_UserBand);
					pstUserBandSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nUserBandId);
					if (pstUserBandSubItem != NULL)
					{
						/* Update subItem value */
						nx_menuScdSettings_GetMenuItemIdFormObjId(nUserBandId, &ulUserBandMenuItemId);
						NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulUserBandMenuItemId, nValue, &(pstUserBandSubItem->lSubItemValue));
						pstUserBandSubItem->ulIdxOfTextList = nValue;
						HxSTD_sprintf((HCHAR*)pstUserBandSubItem->szSubItemText, "%s", (HCHAR*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList]);

						/* Update user band value */
						nx_menuScdSettings_UpdateSubItem(pstContents, nUserBandId);
					}

					/* Update BandFreq */
					nx_menuScdSettings_UpdateBandFreqSubMenu(pstContents, TRUE);
				}

				break;
			}

		case eNxSettingsItem_ScdSet_UserBand:
			{
				HINT32	nValue = -1, nBandFreqId = -1, nUserBand = 0;
				Nx_SettingsLeafSubItem_t *pstUserBandSubItem = NULL, *pstBandFreqSubItem = NULL;
				HBOOL	isOverlap = FALSE;

				pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nObjId);
				if (pstSubItem == NULL)
				{
					NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, nObjId);
					return	ONDK_RET_FAIL;
				}

				ulPrevSelectedIdx = pstSubItem->ulIdxOfTextList;
				pstSubItem->ulIdxOfTextList = ulCurIdx;
				HxSTD_sprintf((char*)pstSubItem->szSubItemText, (char*)"%s", pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList]);

				/* Update subItem value */
				nx_menuScdSettings_GetMenuItemIdFormObjId(nObjId, &ulMenuItemId);
				NX_PRISM_MENU_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &nValue);
				pstSubItem->lSubItemValue = nValue;

				nx_menuScdSettings_GetUserBand(pstContents, pstContents->nCurrTuner, &nUserBand);

				if (nUserBand != nValue)
				{
					nBandFreqId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_BandFreq);
					pstBandFreqSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nBandFreqId);
					if (pstBandFreqSubItem == NULL)
					{
						return ONDK_RET_FAIL;
					}

					// check overlap user band
					isOverlap = nx_menuScdSettings_IsOverlapUserBand(pstContents, nValue);
					if (isOverlap == TRUE)
					{
						nx_menuScdSettings_CreateResponseDialog(pstContents, (HINT32)ONDK_GetString(RES_MESG_3295_ID));
						pstUserBandSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nObjId);
						pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner1;
						pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner1;
						HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						break;
					}

					if (nValue == eNxSettingsItem_None)	// None
					{
						nx_menuScdSettings_CreateSelectiveDialog(pstContents, (HINT32)ONDK_GetString(RES_MESG_5325_ID));
					}
					else
					{
						// check overlap band freq
						isOverlap = nx_menuScdSettings_IsOverlapBandFreq(pstContents, s_ScdSet_ScdFreqTable[nValue][0]);
						if (isOverlap == TRUE)
						{
							nx_menuScdSettings_CreateResponseDialog(pstContents, (HINT32)ONDK_GetString(RES_MESG_3294_ID));
							pstUserBandSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nObjId);
							pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner1;
							pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner1;
							HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
							break;
						}
						/* Update user band value */
						nx_menuScdSettings_UpdateSubItem(pstContents, nObjId);

						/* Enable Band Freq */
						pstBandFreqSubItem->bDisable = FALSE;
						ONDK_GWM_Obj_EnableObject(nBandFreqId);

						/* Update BandFreq */
						nx_menuScdSettings_UpdateBandFreqSubMenu(pstContents, FALSE);
					}
				}
				break;
			}
		case eNxSettingsItem_ScdSet_BandFreq:
			{
				HINT32 nBandFreq = 0, nUserBand = 0;

				nx_menuScdSettings_GetUserBand(pstContents, pstContents->nCurrTuner, &nUserBand);

				nBandFreq = s_ScdSet_ScdFreqTable[nUserBand][ulCurIdx];

				pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, nObjId);
				if (pstSubItem == NULL)
				{
					NX_APP_Error("[%s:%d] pstSubItem is NULL - lObjId Id : [0x%x]\n", __FUNCTION__, __LINE__, nObjId);
					return	ONDK_RET_FAIL;
				}

				if (nBandFreq == -1) // User define
				{
					nx_menuScdSettings_CreateDialogInput(pstContents, nObjId);
				}
				else
				{
					/* Update subItem value */
					pstSubItem->lSubItemValue = nBandFreq;
					pstSubItem->ulIdxOfTextList = ulCurIdx;
					HxSTD_sprintf((char*)pstSubItem->szSubItemText, (char*)"%s", pstSubItem->szItemTextList[pstSubItem->ulIdxOfTextList]);

					/* Update value */
					nx_menuScdSettings_UpdateSubItem(pstContents, nObjId);
				}

				break;
			}
		default:
			return ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgAppDlgClicked(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_Selective)
			{
				HINT32	nUserBandId = 0, nBandFreqId = 0;

				nx_menuScdSettings_DestroySelectiveDialog();

				nUserBandId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_UserBand);
				nBandFreqId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_BandFreq);

				nx_menuScdSettings_UpdateSubItem(pstContents, nUserBandId);
				nx_menuScdSettings_UpdateSubItem(pstContents, nBandFreqId);

				nx_menuScdSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_ScdSet_BandFreq, FALSE);
			}
			break;
		default:					// No
			if (ulDlgWhoseId == eNxSettingsItem_DialogType_Selective)
			{
				HINT32	nUserBandId = 0;
				Nx_SettingsLeafSubItem_t *pstUserBandSubItem = NULL;

				nx_menuScdSettings_DestroySelectiveDialog();

				nUserBandId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_UserBand);
				pstUserBandSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *) pstContents, nUserBandId);

				switch(pstContents->nCurrTuner)
				{
					case eNxSettingsItem_Tuner1:
						pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner1;
						pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner1;
						HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						break;
					case eNxSettingsItem_Tuner2:
						pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner2;
						pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner2;
						HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						break;
					case eNxSettingsItem_Tuner3:
						pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner3;
						pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner3;
						HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						break;
					case eNxSettingsItem_Tuner4:
						pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner4;
						pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner4;
						HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
						break;
					default:
						return ONDK_RET_FAIL;
				}
			}
			else if (ulDlgWhoseId == eNxSettingsItem_DialogType_Confirm)
			{
				nx_menuScdSettings_DestroyConfirmDialog();
			}
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_menuScdSettings_MsgAppDlgCanceled(Nx_MenuScdSetting_Contents_t *pstContents, HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxSettingsItem_DialogType_Selective)
	{
		HINT32	nUserBandId = 0;
		Nx_SettingsLeafSubItem_t *pstUserBandSubItem = NULL;

		nx_menuScdSettings_DestroySelectiveDialog();

		nUserBandId = nx_menuScdSettings_ConvertItemIdxToObjId(eNxSettingsItem_ScdSet_UserBand);
		pstUserBandSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *) pstContents, nUserBandId);

		switch(pstContents->nCurrTuner)
		{
			case eNxSettingsItem_Tuner1:
				pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner1;
				pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner1;
				HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				break;
			case eNxSettingsItem_Tuner2:
				pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner2;
				pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner2;
				HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				break;
			case eNxSettingsItem_Tuner3:
				pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner3;
				pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner3;
				HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				break;
			case eNxSettingsItem_Tuner4:
				pstUserBandSubItem->ulIdxOfTextList = pstContents->nUserBandOfTuner4;
				pstUserBandSubItem->lSubItemValue = pstContents->nUserBandOfTuner4;
				HxSTD_StrNCpy((char*)pstUserBandSubItem->szSubItemText, (char*)pstUserBandSubItem->szItemTextList[pstUserBandSubItem->ulIdxOfTextList], MAX_ELEMENT_TEXT_LENGTH);
				break;
			default:
				return ONDK_RET_FAIL;
		}
	}
	else if (ulDlgWhoseId == eNxSettingsItem_DialogType_Confirm)
	{
		nx_menuScdSettings_DestroyConfirmDialog();
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Search_ScdSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuScdSetting_Contents_t *pstContents = NULL;

	pstContents = nx_menuScdSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			// p1 : AntennaType, p2 : Antenna Slot index, p3 : Antenna uid
			eResult = nx_menuScdSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuScdSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		/* popup list & Action */
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuScdSettings_MsgGwmClicked(pstContents, p1);
			break;

		/* popup list�� ���� �ش� */
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_SCROLLRIGHT!\n");
			nx_menuScdSettings_CreateListPopup(pstContents, p1);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuScdSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuScdSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			nx_menuScdSettings_MsgAppValueChanged(pstContents, p1);
			break;

		case MSG_APP_SETTING_INPUT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_INPUT_VALUE_CHANGED!\n");
			nx_menuScdSettings_MsgAppInputValueChange(pstContents, p1);
			break;

		case MSG_CORE_SERVICE_EVT_SIGNAL_STATE:
			NX_APP_Message("MSG_CORE_SERVICE_EVT_SIGNAL_STATE!\n");
			nx_menuScdSettings_MsgCoreSignalState(p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuScdSettings_MsgAppDlgClicked(pstContents, p1, p2);
			break;

		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CANCELED!\n");
			eResult = nx_menuScdSettings_MsgAppDlgCanceled(pstContents, p2);
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

