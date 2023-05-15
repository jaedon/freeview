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
 * @file	  		nx_prism_menu_storage_device_setting.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#if defined(CONFIG_MW_MM_PVR)

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
#define	NX_MENU_DEVICE_SETTING_INFO_BOX_X					60
#define	NX_MENU_DEVICE_SETTING_INFO_BOX_Y					478
#define	NX_MENU_DEVICE_SETTING_INFO_BOX_W					750
#define	NX_MENU_DEVICE_SETTING_INFO_BOX_H					182

#define NX_MENU_DEVICE_SETTING_INFO_TEXT_X					(NX_MENU_DEVICE_SETTING_INFO_BOX_X + 18)
#define	NX_MENU_DEVICE_SETTING_INFO_TEXT_Y					34
#define NX_MENU_DEVICE_SETTING_INFO_TEXT_W					210
#define NX_MENU_DEVICE_SETTING_INFO_TEXT_H					(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)
#define NX_MENU_DEVICE_SETTING_INFO_TEXT_LS					32

#define NX_MENU_DEVICE_SETTING_INFO_DATA_X					(NX_MENU_DEVICE_SETTING_INFO_BOX_X + 241)
#define	NX_MENU_DEVICE_SETTING_INFO_DATA_W					140

#define	NX_MENU_DEVICE_SETTING_INFO_LINE_X					399
#define	NX_MENU_DEVICE_SETTING_INFO_LINE_Y					11
#define	NX_MENU_DEVICE_SETTING_INFO_LINE_W					1
#define	NX_MENU_DEVICE_SETTING_INFO_LINE_H					161

#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_X				420
#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_Y				66
#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_W				155
#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_H				(NX_PRISM_FONT_SIZE_28 + NX_PRISM_FONT_VSPACE)
#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_S_H			(NX_PRISM_FONT_SIZE_18 + NX_PRISM_FONT_VSPACE)
#define	NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_LS			24

#define	NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_X			585
#define	NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_W			145

#define	NX_MENU_DEVICE_SETTING_INFO_GRAPH_X					420
#define	NX_MENU_DEVICE_SETTING_INFO_GRAPH_Y					105
#define	NX_MENU_DEVICE_SETTING_INFO_GRAPH_W					310
#define	NX_MENU_DEVICE_SETTING_INFO_GRAPH_H					31
#define	NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H			2

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eNxSettingsItem_Storage_HDDTest,
	eNxSettingsItem_Storage_Format,
	eNxSettingsItem_Storage_Remove,
	eNxSettingsItem_Storage_Pairing,
	eNxSettingsItem_Storage_UnPairing,
	eNxSettingsItem_Storage_InfoBox,
	eNxSettingsItem_Storage_TotalStorageText,
	eNxSettingsItem_Storage_SysUsageText,
	eNxSettingsItem_Storage_UserUsageText,
	eNxSettingsItem_Storage_AvailableText,
	eNxSettingsItem_Storage_TotalStorage,
	eNxSettingsItem_Storage_SysUsage,
	eNxSettingsItem_Storage_UserUsage,
	eNxSettingsItem_Storage_Available,
	eNxSettingsItem_Storage_Line,
	eNxSettingsItem_Storage_UsedText,
	eNxSettingsItem_Storage_UsedPercent,
	eNxSettingsItem_Storage_AvailText,
	eNxSettingsItem_Storage_AvailPercent,
	eNxSettingsItem_Storage_PercentBar,
	eNxSettingsItem_Storage_Max
} Nx_SettingsItem_DeviceSetting_t;

typedef enum
{
	eNxSettingsItem_Storage_Progress_Formatting,
	eNxSettingsItem_Storage_Progress_HDDTesting,
	eNxSettingsItem_Storage_Progress_Pairing,
	eNxSettingsItem_Storage_Progress_UnPairing,
	eNxSettingsItem_Storage_Confirm_HDDTestPass,
	eNxSettingsItem_Storage_Confirm_HDDTestFail,
	eNxSettingsItem_Storage_Confirm_FormatFail,
	eNxSettingsItem_Storage_Selective_Format,
	eNxSettingsItem_Storage_Selective_Pairing,
	eNxSettingsItem_Storage_Selective_UnPairing,
	eNxSettingsItem_Storage_PinCode,
	eNxSettingsItem_Storage_None
} Nx_SettingsItem_Storage_DialogType_t;

typedef struct tag_Nx_MenuDeviceSetting_contents
{
	/**@breif Public value */
	HUINT32				ulCurFocus;
	HUINT32				ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT32				ulDeviceId;
	HUINT32				ulCurrentProgSelectDlg;
	HUINT8				ucLabel[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucTotalStorage[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucSystemUsage[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucUserUsage[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucAvailable[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucUsedPercent[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				ucAvailPercent[MAX_ELEMENT_TEXT_LENGTH];
} Nx_MenuDeviceSetting_Contents_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuDeviceSetting_Contents_t			s_stDeviceSettingContent;
static NXUI_DialogInstance_t					s_stDialogProgressInstance;
static NX_PinDialogInstance_t					s_stDialogPinCodeInstance;
static NXUI_DialogInstance_t					s_stDialogSelectiveInstance;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuDeviceSettings_SelectiveMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void	nx_menuDeviceSettings_Ui_SelectiveMsgCreate(HINT8 *szMsgStrID, HINT8 *szTitleStrID, HUINT32 ulDlgType);
static ONDK_Result_t	nx_menuDeviceSettings_SelectiveMsgDestroy(void);

static ONDK_Result_t	nx_menuDeviceSettings_PinCodeMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuDeviceSettings_Ui_PinCodeMsgCreate(void);
static ONDK_Result_t	nx_menuDeviceSettings_PinCodeMsgDestroy(void);

static ONDK_Result_t	nx_menuDeviceSettings_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_menuDeviceSettings_Ui_ProcessingMsgCreate(Nx_SettingsItem_Storage_DialogType_t eDialogType);
static void				nx_menuDeviceSettings_ProcessingMsgDestroy(void);
static ONDK_Result_t	nx_menuDeviceSettings_ResponseMsgProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);

static inline Nx_MenuDeviceSetting_Contents_t* nx_menuDeviceSettings_GetContents(void);
static HCHAR*			nx_menuDeviceSettings_GetMenuStrIdFormObjId(HINT32 lObjId);

static HINT32			nx_menuDeviceSettings_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuDeviceSettings_ConvertItemIdxToObjId(HINT32 lItemId);
static HBOOL			nx_menuDeviceSettings_GetItemSupport(Nx_SettingsItem_DeviceSetting_t eNxDeviceSettingItem);
static HBOOL			nx_menuDeviceSettings_GetItemiDisable(Nx_SettingsItem_DeviceSetting_t eNxDeviceSettingItem, NX_PhysicalStorageData_t *physicalStorageData);
static HCHAR* 			nx_menuDeviceSettings_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData);
static void				nx_menuDeviceSettings_FindNextFocus(Nx_MenuDeviceSetting_Contents_t *pstContents, HBOOL bUp);

static ONDK_Result_t	nx_menuDeviceSettings_ChangeItemEnableDisableMode(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable);

static ONDK_Result_t	nx_menuDeviceSettings_AddItemHDDTest(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_AddItemFormat(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_AddItemRemove(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_AddItemPairing(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_AddItemUnPairing(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_InitSettingsItems(Nx_MenuDeviceSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuDeviceSettings_CalcUsedAvailPercent(NX_PhysicalStorageData_t *physicalStorageData, HUINT32 *pstUsedPercent, HUINT32 *pstAvailPercent);
static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoPercent(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_menuDeviceSettings_DrawPercentBar(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoSizeText(NX_PhysicalStorageData_t *physicalStorageData, Nx_SettingsItem_DeviceSetting_t lItemidx);
static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoSizeData(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData, Nx_SettingsItem_DeviceSetting_t lItemidx);
static ONDK_Result_t	nx_menuDeviceSettings_DrawDeviceInfo(Nx_MenuDeviceSetting_Contents_t *pstContents);
static ONDK_Result_t	nx_menuDeviceSettings_DrawDefaultUI(Nx_MenuDeviceSetting_Contents_t *pstContents);

static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmCreate(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuDeviceSettings_MsgGwmDestroy(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmHapiMsg(Nx_MenuDeviceSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmClicked(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmKeyDown(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreFormatState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage);
static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreDSTState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuDeviceSettings_MsgCorePairingState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage);
static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreDeviceChange(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define __Select_Dialog_Functions___________________________________________
/******************************************************************************/


#define _________Dialog_Input_Functions_________________________________________

static ONDK_Result_t	nx_menuDeviceSettings_SelectiveMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_Message_Proc(&s_stDialogSelectiveInstance, message, hAction, p1, p2, p3);
}

static void	nx_menuDeviceSettings_Ui_SelectiveMsgCreate(HINT8 *szMsgStrID, HINT8 *szTitleStrID, HUINT32 ulDlgType)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stDialogSelectiveInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if ((ulDlgType == eNxSettingsItem_Storage_Selective_Format) || (ulDlgType == eNxSettingsItem_Storage_Selective_Pairing) || (ulDlgType == eNxSettingsItem_Storage_Selective_UnPairing))
	{
		if (szMsgStrID == NULL)
		{
			NX_APP_Error("Error!!! szMsgStrID is NULL\n");
			return;
		}
		ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
		NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString((HINT8*)szMsgStrID));
	}
	else
	{
		if ((szTitleStrID == NULL) || (szMsgStrID == NULL))
		{
			NX_APP_Error("Error!!! szTitleStrID or szMsgStrID is NULL\n");
			return;
		}
		ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
		NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, ONDK_GetString((HINT8*)szTitleStrID), ONDK_GetString((HINT8*)szMsgStrID));
	}

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_menuDeviceSettings_SelectiveMsgProc",
						APP_DIALOGBOX_PRIORITY,	nx_menuDeviceSettings_SelectiveMsgProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}

static ONDK_Result_t	nx_menuDeviceSettings_SelectiveMsgDestroy(void)
{
	return	ONDK_GWM_APP_Destroy(nx_menuDeviceSettings_SelectiveMsgProc);
}

static ONDK_Result_t	nx_menuDeviceSettings_PinCodeMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stDialogPinCodeInstance, message, hAction, p1, p2, p3);
}

static void		nx_menuDeviceSettings_Ui_PinCodeMsgCreate(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;
	NX_PinDialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stDialogPinCodeInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NX_PinDialogInstance_t));

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(pstDialog,
								ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuDeviceSettings_PinCodeMsgProc", APP_DEFAULT_PRIORITY, nx_menuDeviceSettings_PinCodeMsgProc, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_menuDeviceSettings_PinCodeMsgDestroy(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuDeviceSettings_PinCodeMsgProc);
}

static ONDK_Result_t	nx_menuDeviceSettings_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stDialogProgressInstance, message, hAction, p1, p2, p3);
}

static void		nx_menuDeviceSettings_Ui_ProcessingMsgCreate(Nx_SettingsItem_Storage_DialogType_t eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stDialogProgressInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	if (eDialogType == eNxSettingsItem_Storage_Progress_Formatting || eDialogType == eNxSettingsItem_Storage_Progress_Pairing || eDialogType == eNxSettingsItem_Storage_Progress_UnPairing)
	{
		ulDlgAttr = (NX_DLG_TYPE_NO_STANDBY|NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
		pszMsg = (HINT8*)ONDK_GetString(STR_PROCESSING_ID);
	}
	else if (eDialogType == eNxSettingsItem_Storage_Progress_HDDTesting)
	{
		ulDlgAttr = (NX_DLG_TYPE_NO_STANDBY|NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);
		pszMsg = (HINT8*)ONDK_GetString(STR_MESG_3251_ID);
	}
	else
	{
		NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
		return;
	}

	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)pszMsg);

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuDeviceSettings_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_menuDeviceSettings_ProcessingMsgProc, 0, 0, 0, 0);
}

static void		nx_menuDeviceSettings_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_menuDeviceSettings_ProcessingMsgProc);
}

static ONDK_Result_t	nx_menuDeviceSettings_ResponseMsgProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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

static void		nx_menuDeviceSettings_Ui_ResponseMsgCreate(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuDeviceSettings_ResponseMsgProc", APP_DEFAULT_PRIORITY,
						nx_menuDeviceSettings_ResponseMsgProc, 0, (HINT32)pszString, 0, 0);
}

#define _________Normal_Functions____________________________________________

static inline Nx_MenuDeviceSetting_Contents_t* nx_menuDeviceSettings_GetContents(void)
{
	return	&s_stDeviceSettingContent;
}

static HCHAR*	nx_menuDeviceSettings_GetMenuStrIdFormObjId(HINT32 lObjId)
{
	HINT32		lItemId = 0;

	lItemId = nx_menuDeviceSettings_ConvertObjIdToItemIdx(lObjId);
	switch (lItemId)
	{
		case eNxSettingsItem_Storage_HDDTest:
			return	RES_HDD_TEST_ID;

		case eNxSettingsItem_Storage_Format:
			return	RES_FORMAT_STORAGE_ID;

		case eNxSettingsItem_Storage_Remove:
			return	RES_REMOVE_STORAGE_SAFELY_ID;

		case eNxSettingsItem_Storage_Pairing:
			return	STR_MESG_5544_ID;

		case eNxSettingsItem_Storage_UnPairing:
			return	STR_MESG_5545_ID;

		case eNxSettingsItem_Storage_TotalStorageText:
			return	RES_TOTAL_STORAGE_ID;

		case eNxSettingsItem_Storage_SysUsageText:
			return	RES_SYSTEM_USAGE_ID;

		case eNxSettingsItem_Storage_UserUsageText:
			return	RES_USER_USAGE_ID;

		case eNxSettingsItem_Storage_AvailableText:
			return	RES_AVAILABLE_01_ID;

		default:
			NX_APP_Error("Error!!! Invalid Item Id : [0x%x][%d]\n", lObjId, lItemId);
			return	NULL;
	}
}

static HINT32	nx_menuDeviceSettings_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_StorageDeviceSetting);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Storage_Max))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuDeviceSettings_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Storage_Max))
	{
		return 0;
	}

	return	(eNxSetting_StorageDeviceSetting | lItemId);
}


static HBOOL	nx_menuDeviceSettings_GetItemSupport(Nx_SettingsItem_DeviceSetting_t eNxDeviceSettingItem)
{
	HBOOL	bSupport = TRUE;

	switch (eNxDeviceSettingItem)
	{
		case eNxSettingsItem_Storage_HDDTest:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Storage_Format:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Storage_Remove:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Storage_Pairing:
			bSupport = TRUE;
			break;
		case eNxSettingsItem_Storage_UnPairing:
			bSupport = TRUE;
			break;
		default:
			bSupport = FALSE;
			break;
	}

	return	bSupport;
}

static HBOOL	nx_menuDeviceSettings_GetItemiDisable(Nx_SettingsItem_DeviceSetting_t eNxDeviceSettingItem, NX_PhysicalStorageData_t *physicalStorageData)
{
	HBOOL	bDisable = TRUE;

	switch (eNxDeviceSettingItem)
	{
		case eNxSettingsItem_Storage_HDDTest:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Storage_Format:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Storage_Remove:
			bDisable = FALSE;
			break;
		case eNxSettingsItem_Storage_Pairing:
			if (physicalStorageData->eSort == eStorageSort_externalHDD || physicalStorageData->eSort == eStorageSort_externalPVRHDD)
			{
				if (physicalStorageData->ePairing == eStoragePairing_OK)
				{
					bDisable = TRUE;
				}
				else
				{
					bDisable = FALSE;
				}
			}
			else
			{
				bDisable = TRUE;
			}
			break;
		case eNxSettingsItem_Storage_UnPairing:
			if (physicalStorageData->eSort == eStorageSort_externalHDD || physicalStorageData->eSort == eStorageSort_externalPVRHDD)
			{
				if (physicalStorageData->ePairing == eStoragePairing_OK)
				{
					bDisable = FALSE;
				}
				else
				{
					bDisable = TRUE;
				}
			}
			else
			{
				bDisable = TRUE;
			}
			break;
		default:
			bDisable = TRUE;
			break;
	}

	return	bDisable;
}

static HCHAR* 	nx_menuDeviceSettings_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData)
{
	if (physicalStorageData == NULL)
	{
		NX_APP_Error("ERROR!!! physicalStorageData is NULL !!\n");
		return NULL;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_INTERNAL_HDD_ID);

		case eStorageSort_externalSubPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_DETACHABLE_HDD_ID);

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_UNKNOWN_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		case eStorageSort_SdMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_SD_CARD_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		default:
			return NULL;
	}
}

static void		nx_menuDeviceSettings_FindNextFocus(Nx_MenuDeviceSetting_Contents_t *pstContents, HBOOL bUp)
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

static ONDK_Result_t	nx_menuDeviceSettings_ChangeItemEnableDisableMode(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 lItemIdx, HBOOL bEnable)
{
	HINT32			lObjId = -1;
	Nx_SettingsLeafSubItem_t	*pstSubItem = NULL;

	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemIdx);
	pstSubItem = NX_PRISM_AppContents_GetSubitemByObjId((void *)pstContents, lObjId);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("pstSubItem is NULL - lObjId Id : [0x%x]\n", lObjId);
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

static ONDK_Result_t	nx_menuDeviceSettings_AddItemHDDTest(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HCHAR				*pObjTitle = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuDeviceSettings_GetItemSupport(eNxSettingsItem_Storage_HDDTest);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Storage_HDDTest\n");
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_HDDTest);

	// Check Disable Item
	bDisable = nx_menuDeviceSettings_GetItemiDisable(eNxSettingsItem_Storage_HDDTest, physicalStorageData);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pObjTitle = nx_menuDeviceSettings_GetMenuStrIdFormObjId(eNxSettingsItem_Storage_HDDTest);
	if (pObjTitle != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", (char*)ONDK_GetString(pObjTitle));
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_AddItemFormat(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HCHAR				*pObjTitle = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuDeviceSettings_GetItemSupport(eNxSettingsItem_Storage_Format);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Storage_Format\n");
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_Format);

	// Check Disable Item
	bDisable = nx_menuDeviceSettings_GetItemiDisable(eNxSettingsItem_Storage_Format, physicalStorageData);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pObjTitle = nx_menuDeviceSettings_GetMenuStrIdFormObjId(eNxSettingsItem_Storage_Format);
	if (pObjTitle != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", (char*)ONDK_GetString(pObjTitle));
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_AddItemRemove(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HCHAR				*pObjTitle = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuDeviceSettings_GetItemSupport(eNxSettingsItem_Storage_Remove);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Storage_Remove\n");
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_Remove);

	// Check Disable Item
	bDisable = nx_menuDeviceSettings_GetItemiDisable(eNxSettingsItem_Storage_Remove, physicalStorageData);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pObjTitle = nx_menuDeviceSettings_GetMenuStrIdFormObjId(eNxSettingsItem_Storage_Remove);
	if (pObjTitle != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", (char*)ONDK_GetString(pObjTitle));
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_AddItemPairing(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HCHAR				*pObjTitle = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuDeviceSettings_GetItemSupport(eNxSettingsItem_Storage_Pairing);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Storage_Pairing\n");
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_Pairing);

	// Check Disable Item
	bDisable = nx_menuDeviceSettings_GetItemiDisable(eNxSettingsItem_Storage_Pairing, physicalStorageData);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pObjTitle = nx_menuDeviceSettings_GetMenuStrIdFormObjId(eNxSettingsItem_Storage_Pairing);
	if (pObjTitle != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", (char*)ONDK_GetString(pObjTitle));
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_AddItemUnPairing(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HCHAR				*pObjTitle = NULL;
	HBOOL				bSupport = FALSE, bDisable = FALSE;
	Nx_SettingsLeafSubItem_t	stSubItem;

	NX_COMMON_NOT_USED_PARAM(pstContents);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_INITFAIL;
	}

	// Check Supported Item
	bSupport = nx_menuDeviceSettings_GetItemSupport(eNxSettingsItem_Storage_UnPairing);
	if (bSupport == FALSE)
	{
		NX_APP_Info("Not Support eNxSettingsItem_Storage_UnPairing\n");
		return	ONDK_RET_UNSUPPORTED;
	}

	HxSTD_MemSet(&stSubItem, 0x00, sizeof(Nx_SettingsLeafSubItem_t));
	stSubItem.ulId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_UnPairing);

	// Check Disable Item
	bDisable = nx_menuDeviceSettings_GetItemiDisable(eNxSettingsItem_Storage_UnPairing, physicalStorageData);
	stSubItem.bDisable = bDisable;

	// Set Main String
	pObjTitle = nx_menuDeviceSettings_GetMenuStrIdFormObjId(eNxSettingsItem_Storage_UnPairing);
	if (pObjTitle != NULL)
	{
		HxSTD_sprintf((char*)stSubItem.szMainItemText, (char*)"%s", (char*)ONDK_GetString(pObjTitle));
	}

	/* Append the subitem to the menu contents. */
	NX_PRISM_AppContents_AppendSubitem ((void *)pstContents, &stSubItem);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_InitSettingsItems(Nx_MenuDeviceSetting_Contents_t *pstContents)
{
	ONDK_Result_t				eResult = ONDK_RET_OK;
	HERROR						hError = ERR_FAIL;
	HCHAR						*pLabel = NULL;
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents physicalStorageData NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->ulDeviceId, &physicalStorageData);
	if (hError != ERR_OK || physicalStorageData == NULL)
	{
		NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_InitSubitemArray() !!\n");
		return eResult;
	}

	// Make Storage Label
	pLabel = nx_menuDeviceSettings_GetStorageLabelFromObjInfo(physicalStorageData);
	if (pLabel != NULL)
	{
		HxSTD_StrNCpy((char*)pstContents->ucLabel, pLabel, MAX_ELEMENT_TEXT_LENGTH);
		pstContents->ucLabel[MAX_ELEMENT_TEXT_LENGTH - 1] = '\0';
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			nx_menuDeviceSettings_AddItemHDDTest(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemFormat(pstContents, physicalStorageData);
			break;
		case eStorageSort_externalPVRHDD:
			nx_menuDeviceSettings_AddItemRemove(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemFormat(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemPairing(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemUnPairing(pstContents, physicalStorageData);
			break;

		case eStorageSort_externalSubPVRHDD:
			nx_menuDeviceSettings_AddItemRemove(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemFormat(pstContents, physicalStorageData);
			break;

		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
			nx_menuDeviceSettings_AddItemRemove(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemFormat(pstContents, physicalStorageData);
#else
			nx_menuDeviceSettings_AddItemRemove(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemFormat(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemPairing(pstContents, physicalStorageData);
			nx_menuDeviceSettings_AddItemUnPairing(pstContents, physicalStorageData);
#endif
			break;

		case eStorageSort_SdMemory:
			nx_menuDeviceSettings_AddItemRemove(pstContents, physicalStorageData);
			break;
		default:
			break;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_CalcUsedAvailPercent(NX_PhysicalStorageData_t *physicalStorageData, HUINT32 *pstUsedPercent, HUINT32 *pstAvailPercent)
{
	HUINT32 		ulTotalSize = physicalStorageData->ullTotalSize / (1024 * 1024);
	HUINT32 		ulUsedSize = (physicalStorageData->ullReservedSize + physicalStorageData->ullUsedSize) / (1024 * 1024);

	if (ulTotalSize > 0)
	{
		*pstUsedPercent = (HUINT32)((ulUsedSize * 100) / ulTotalSize);
		if (*pstUsedPercent > 100)
		{
			*pstUsedPercent = 100;
		}
		*pstAvailPercent = 100 - *pstUsedPercent;
	}
	else
	{
		*pstUsedPercent = 0;
		*pstAvailPercent = 100;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoPercent(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData)
{
	HINT32			lItemidx = -1, lObjId;
	HUINT32			ulUsedPercent = 0, ulAvailPercent = 0;
	ONDK_Rect_t		stInfoRect = { 0, 0, 0, 0 };

	if (physicalStorageData == NULL)
	{
		NX_APP_Error("Error!!! physicalStorageData NULL !!\n");
		return ONDK_RET_FAIL;
	}

	nx_menuDeviceSettings_CalcUsedAvailPercent(physicalStorageData, &ulUsedPercent, &ulAvailPercent);

	/* Used Text Draw */
	lItemidx = eNxSettingsItem_Storage_UsedPercent;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_Y - NX_PRISM_FONT_SIZE_28,
							NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_W, NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_H);
	HxSTD_sprintf((char*)pstContents->ucUsedPercent, "%d%%", ulUsedPercent);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stInfoRect, pstContents->ucUsedPercent);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_28);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	lItemidx = eNxSettingsItem_Storage_UsedText;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_LS - NX_PRISM_FONT_SIZE_18,
							NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_W, NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_H);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stInfoRect, (HCHAR*)ONDK_GetString(RES_USED_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	/* Available Text Draw */
	lItemidx = eNxSettingsItem_Storage_AvailPercent;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_Y - NX_PRISM_FONT_SIZE_28,
							NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_W, NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_H);
	HxSTD_sprintf((char*)pstContents->ucAvailPercent, "%d%%", ulAvailPercent);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stInfoRect, pstContents->ucAvailPercent);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_28);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	lItemidx = eNxSettingsItem_Storage_AvailText;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_Y + NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_LS - NX_PRISM_FONT_SIZE_18,
							NX_MENU_DEVICE_SETTING_INFO_AVAIL_TEXT_W, NX_MENU_DEVICE_SETTING_INFO_USED_TEXT_H);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stInfoRect, (HCHAR*)ONDK_GetString(RES_AVAILABLE_01_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Percent Graph */
	lItemidx = eNxSettingsItem_Storage_PercentBar;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_GRAPH_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_GRAPH_Y,
							NX_MENU_DEVICE_SETTING_INFO_GRAPH_W, NX_MENU_DEVICE_SETTING_INFO_GRAPH_H + (NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H * 2));
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_Create(lObjId, &stInfoRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(lObjId, COL(C_G_Bang_100), COL(C_G_Yoyo01));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, nx_menuDeviceSettings_DrawPercentBar);
	ONDK_GWM_COM_Status_SetValue(lObjId, ulUsedPercent /** (NX_MENU_DEVICE_SETTING_INFO_GRAPH_W / 100)*/);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawPercentBar(ONDKSurfaceHandle screen, ONDK_Region_t *area,
														ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	HUINT32		ulPrcentBar = 0, ulValue = 0, ulMax = 0;
	ONDK_Color_t	ulBackColor = 0, ulForcColor = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_COM_Status_GetProperty((STATUS_OBJECT)object, &ulMax, &ulValue, &ulBackColor, &ulForcColor);
	/* Draw Two Line */
	ONDK_DRAW_FillRectElement(screen, x1, y1, w, NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H, C_G_Whistle_100);
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H + NX_MENU_DEVICE_SETTING_INFO_GRAPH_H, w, NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H, C_G_Whistle_100);

	/* Draw BackGround */
	ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H, w, h - (NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H * 2), ulBackColor);

	/* calc bar length */
	ulPrcentBar = (ulValue * w)/ulMax;
	if (ulPrcentBar > (HUINT32)w)
	{
		ulPrcentBar = w;
	}

	if (ulPrcentBar > 0)
	{
		ONDK_DRAW_FillRectElement(screen, x1, y1 + NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H, ulPrcentBar, h - (NX_MENU_DEVICE_SETTING_INFO_GRAPH_LINE_H * 2), ulForcColor);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoSizeText(NX_PhysicalStorageData_t *physicalStorageData, Nx_SettingsItem_DeviceSetting_t lItemidx)
{
	HINT32			lObjId = -1;
	HINT32			x = NX_MENU_DEVICE_SETTING_INFO_TEXT_X, y;
	HINT32			w = NX_MENU_DEVICE_SETTING_INFO_TEXT_W, h = NX_MENU_DEVICE_SETTING_INFO_TEXT_H;
	HCHAR			*textId = NULL;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	switch(lItemidx)
	{
		case eNxSettingsItem_Storage_TotalStorageText:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_TEXT_Y - NX_PRISM_FONT_SIZE_20;
			break;
		case eNxSettingsItem_Storage_SysUsageText:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_TEXT_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 1) - NX_PRISM_FONT_SIZE_20;
			break;
		case eNxSettingsItem_Storage_UserUsageText:
			if (physicalStorageData->eSort == eStorageSort_InternalPVRHDD)
			{
				y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_TEXT_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 2) - NX_PRISM_FONT_SIZE_20;
			}
			else
			{
				y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_TEXT_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 1) - NX_PRISM_FONT_SIZE_20;
			}
			break;
		case eNxSettingsItem_Storage_AvailableText:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_TEXT_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 4) - NX_PRISM_FONT_SIZE_20;
			break;
		default:
			return ONDK_RET_FAIL;
	}

	stRect = ONDK_Rect(x, y, w, h);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	textId = nx_menuDeviceSettings_GetMenuStrIdFormObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)ONDK_GetString(textId));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Gong04), COL(C_T_Bang_100));

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawInfoSizeData(Nx_MenuDeviceSetting_Contents_t *pstContents, NX_PhysicalStorageData_t *physicalStorageData, Nx_SettingsItem_DeviceSetting_t lItemidx)
{
	HINT32			lObjId = -1;
	HINT32			x = NX_MENU_DEVICE_SETTING_INFO_DATA_X, y;
	HINT32			w = NX_MENU_DEVICE_SETTING_INFO_DATA_W, h = NX_MENU_DEVICE_SETTING_INFO_TEXT_H;
	HFLOAT64		fMemory = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	switch(lItemidx)
	{
		case eNxSettingsItem_Storage_TotalStorage:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 1) - NX_PRISM_FONT_SIZE_20;
			break;
		case eNxSettingsItem_Storage_SysUsage:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 2) - NX_PRISM_FONT_SIZE_20;
			break;
		case eNxSettingsItem_Storage_UserUsage:
			if (physicalStorageData->eSort == eStorageSort_InternalPVRHDD)
			{
				y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 3) - NX_PRISM_FONT_SIZE_20;
			}
			else
			{
				y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 2) - NX_PRISM_FONT_SIZE_20;
			}
			break;
		case eNxSettingsItem_Storage_Available:
			y = NX_MENU_DEVICE_SETTING_INFO_BOX_Y + (NX_MENU_DEVICE_SETTING_INFO_TEXT_LS * 5) - NX_PRISM_FONT_SIZE_20;
			break;
		default:
			return ONDK_RET_FAIL;
	}
	stRect = ONDK_Rect(x, y, w, h);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);

	switch(lItemidx)
	{
		case eNxSettingsItem_Storage_TotalStorage:
			fMemory = (double)physicalStorageData->ullTotalSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucTotalStorage, "%.1f%s", fMemory, "GB");
			ONDK_GWM_COM_Text_Create(lObjId, &stRect, pstContents->ucTotalStorage);
			break;
		case eNxSettingsItem_Storage_SysUsage:
			fMemory = (double)physicalStorageData->ullReservedSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucSystemUsage, "%.1f%s", fMemory, "GB");
			ONDK_GWM_COM_Text_Create(lObjId, &stRect, pstContents->ucSystemUsage);
			break;
		case eNxSettingsItem_Storage_UserUsage:
			fMemory = (double)physicalStorageData->ullUsedSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucUserUsage, "%.1f%s", fMemory, "GB");
			ONDK_GWM_COM_Text_Create(lObjId, &stRect, pstContents->ucUserUsage);
			break;
		case eNxSettingsItem_Storage_Available:
			fMemory = (double)physicalStorageData->ullAvailableSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucAvailable, "%.1f%s", fMemory, "GB");
			ONDK_GWM_COM_Text_Create(lObjId, &stRect, pstContents->ucAvailable);
			break;
		default:
			return ONDK_RET_FAIL;
	}
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Gong01), COL(C_T_Bang_100));

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawDeviceInfo(Nx_MenuDeviceSetting_Contents_t *pstContents)
{
	HINT32						lObjId = -1, lItemidx = -1;
	HERROR						hError = ERR_FAIL;
	ONDK_Rect_t 				stInfoRect = { 0, 0, 0, 0 };
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;

	// Info Box Draw
	lItemidx = eNxSettingsItem_Storage_InfoBox;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y, NX_MENU_DEVICE_SETTING_INFO_BOX_W, NX_MENU_DEVICE_SETTING_INFO_BOX_H);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Rect_Create(lObjId, &stInfoRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawFrameBox_A);

	/* line Draw */
	lItemidx = eNxSettingsItem_Storage_Line;
	stInfoRect = ONDK_Rect(NX_MENU_DEVICE_SETTING_INFO_BOX_X + NX_MENU_DEVICE_SETTING_INFO_LINE_X, NX_MENU_DEVICE_SETTING_INFO_BOX_Y + NX_MENU_DEVICE_SETTING_INFO_LINE_Y,
							NX_MENU_DEVICE_SETTING_INFO_LINE_W, NX_MENU_DEVICE_SETTING_INFO_LINE_H);
	lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Rect_Create(lObjId, &stInfoRect, COL(C_G_Whistle_15));

	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->ulDeviceId, &physicalStorageData);
	if (hError != ERR_OK || physicalStorageData == NULL)
	{
		NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
		return ONDK_RET_FAIL;
	}
	nx_menuDeviceSettings_DrawInfoPercent(pstContents, physicalStorageData);

	nx_menuDeviceSettings_DrawInfoSizeText(physicalStorageData, eNxSettingsItem_Storage_TotalStorageText);
	nx_menuDeviceSettings_DrawInfoSizeData(pstContents, physicalStorageData, eNxSettingsItem_Storage_TotalStorage);

	if (physicalStorageData->eSort == eStorageSort_InternalPVRHDD)
	{
		nx_menuDeviceSettings_DrawInfoSizeText(physicalStorageData, eNxSettingsItem_Storage_SysUsageText);
		nx_menuDeviceSettings_DrawInfoSizeData(pstContents, physicalStorageData, eNxSettingsItem_Storage_SysUsage);
	}

	nx_menuDeviceSettings_DrawInfoSizeText(physicalStorageData, eNxSettingsItem_Storage_UserUsageText);
	nx_menuDeviceSettings_DrawInfoSizeData(pstContents, physicalStorageData, eNxSettingsItem_Storage_UserUsage);

	nx_menuDeviceSettings_DrawInfoSizeText(physicalStorageData, eNxSettingsItem_Storage_AvailableText);
	nx_menuDeviceSettings_DrawInfoSizeData(pstContents, physicalStorageData, eNxSettingsItem_Storage_Available);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_DrawDefaultUI(Nx_MenuDeviceSetting_Contents_t *pstContents)
{
	NX_PRISM_COMP_Settings_DrawBackgroundPopUp();
	NX_PRISM_COMP_Settings_DrawTitlePopUp(pstContents->ucLabel);
	nx_menuDeviceSettings_DrawDeviceInfo(pstContents);
	NX_PRISM_COMP_Settings_InitLeafSubItemsInMenuContents((void *)pstContents, (GWM_ObjDraw_t)NX_PRISM_COMP_Settings_DrawPopUpSubItem_Cursor_B1);

	nx_menuDeviceSettings_FindNextFocus(pstContents, FALSE);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmCreate(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	pstContents->ulDeviceId = (HUINT32)p1;
	eResult = nx_menuDeviceSettings_InitSettingsItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_menuDeviceSettings_InitSettingsItems() Fail !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	// Draw Menu Item
	pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
	nx_menuDeviceSettings_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmDestroy(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuDeviceSetting_Contents_t));
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmHapiMsg(Nx_MenuDeviceSetting_Contents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmClicked(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 lObjId)
{
	HINT32						lItemIdx = -1;
	HBOOL						bPaired = FALSE;
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;
	NX_PhysicalStorageData_t 	*otherPhysicalStorageInfo = NULL;
	HERROR						hError = ERR_FAIL;

	lItemIdx = nx_menuDeviceSettings_ConvertObjIdToItemIdx(lObjId);

	switch (lItemIdx)
	{
		case eNxSettingsItem_Storage_HDDTest:
			NX_FILEMGR_STORAGE_Recovery(pstContents->ulDeviceId);
			nx_menuDeviceSettings_Ui_ProcessingMsgCreate(eNxSettingsItem_Storage_Progress_HDDTesting);
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Progress_HDDTesting;
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_Storage_Format:
			hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->ulDeviceId, &physicalStorageData);
			if (hError != ERR_OK || physicalStorageData == NULL)
			{
				NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
				return ONDK_RET_INITFAIL;
			}
			if (physicalStorageData->eSort == eStorageSort_InternalPVRHDD)
			{
				nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_3942_ID, NULL, eNxSettingsItem_Storage_Selective_Format);
			}
			else
			{
				nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_2310_ID, NULL, eNxSettingsItem_Storage_Selective_Format);
			}
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Selective_Format;
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_Storage_Remove:
			NX_FILEMGR_STORAGE_SafeRemove(pstContents->ulDeviceId);
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_Storage_Pairing:
			hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->ulDeviceId, &physicalStorageData);
			if (hError != ERR_OK || physicalStorageData == NULL)
			{
				NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
				return ONDK_RET_INITFAIL;
			}

			NX_FILEMGR_STORAGE_Check_HDD_Paired(&bPaired, &otherPhysicalStorageInfo);
			if (bPaired == TRUE && otherPhysicalStorageInfo != NULL)
			{
				if (otherPhysicalStorageInfo->eInterfaceVersion == eStorageInterfaceVersion_USB3 && physicalStorageData->eInterfaceVersion == eStorageInterfaceVersion_USB2)
				{
					nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_6098_ID, NULL, eNxSettingsItem_Storage_Selective_Pairing);
				}
				else
				{
					nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_4082_ID, NULL, eNxSettingsItem_Storage_Selective_Pairing);
				}
			}
			else
			{
				nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_4074_ID, NULL, eNxSettingsItem_Storage_Selective_Pairing);
			}
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Selective_Pairing;
			return	ONDK_MESSAGE_BREAK;

		case eNxSettingsItem_Storage_UnPairing:
			nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_4163_ID, NULL, eNxSettingsItem_Storage_Selective_UnPairing);
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Selective_UnPairing;
			return	ONDK_MESSAGE_BREAK;

		default:
			return	ONDK_RET_OK;
	}
}


static ONDK_Result_t	nx_menuDeviceSettings_MsgGwmKeyDown(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_GUIDE:
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
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreFormatState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage)
{
	switch(lMessage)
	{
		case MSG_CORE_STORAGE_FORMAT_PROGRESS:
			NX_APP_Print("MSG_CORE_STORAGE_FORMAT_PROGRESS - DeviceId(%d), Progress(%d)\n", p1, p2);
			break;
		case MSG_CORE_STORAGE_FORMAT_FAILED:
			NX_APP_Print("MSG_CORE_STORAGE_FORMAT_FAILED - DeviceId(%d)\n", p1);
			nx_menuDeviceSettings_ProcessingMsgDestroy();
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_412_ID, RES_FAILED_HDD_FORMAT_01_ID, eNxSettingsItem_Storage_Confirm_FormatFail);
			break;
		case MSG_CORE_STORAGE_FORMAT_DONE:
			NX_APP_Print("MSG_CORE_STORAGE_FORMAT_DONE - DeviceId(%d), FormatStatus(%d)\n", p1, p2);
			nx_menuDeviceSettings_ProcessingMsgDestroy();
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			nx_menuDeviceSettings_Ui_ResponseMsgCreate((HUINT8*)ONDK_GetString(RES_MESG_3298_ID));
			break;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreDSTState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p2)
	{
		case eDxSTORAGE_DEV_STATUS_DstResultOk:
			nx_menuDeviceSettings_ProcessingMsgDestroy();
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_2529_ID, RES_PASSED_HDD_TEST_01_ID, eNxSettingsItem_Storage_Confirm_HDDTestPass);
			break;
		case eDxSTORAGE_DEV_STATUS_DstResultOnGoing:
			break;
		case eDxSTORAGE_DEV_STATUS_DstResultFailNoHDD:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTryAgain:
		case eDxSTORAGE_DEV_STATUS_DstResultFailFatalError:
		case eDxSTORAGE_DEV_STATUS_DstResultFailTestElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailElectricalElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailServoElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailReadElementFailed:
		case eDxSTORAGE_DEV_STATUS_DstResultFailHadlingDamange:
		case eDxSTORAGE_DEV_STATUS_DstResultFailUnknown:
			nx_menuDeviceSettings_ProcessingMsgDestroy();
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			nx_menuDeviceSettings_Ui_SelectiveMsgCreate(RES_MESG_3678_ID, RES_FAILED_HDD_TEST_01_ID, eNxSettingsItem_Storage_Confirm_HDDTestPass);
			break;
		default:
			break;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgCorePairingState(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage)
{
	nx_menuDeviceSettings_ProcessingMsgDestroy();
	pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;

	switch (lMessage)
	{
		case MSG_CORE_STORAGE_PAIRING_FAILED:
			break;
		case MSG_CORE_STORAGE_PAIRING_DONE:
			nx_menuDeviceSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_Storage_Pairing, FALSE);
			nx_menuDeviceSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_Storage_UnPairing, TRUE);
			nx_menuDeviceSettings_FindNextFocus(pstContents, TRUE);
			break;
		case MSG_CORE_STORAGE_UNPAIRING_FAILED:
			break;
		case MSG_CORE_STORAGE_UNPAIRING_DONE:
			nx_menuDeviceSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_Storage_Pairing, TRUE);
			nx_menuDeviceSettings_ChangeItemEnableDisableMode(pstContents, eNxSettingsItem_Storage_UnPairing, FALSE);
			nx_menuDeviceSettings_FindNextFocus(pstContents, TRUE);
			break;
		default:
			break;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgCoreDeviceChange(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p2 == eNxStorageState_Detach)
	{
		if (pstContents->ulDeviceId == (HUINT32)p1)
		{
			nx_menuDeviceSettings_Ui_ResponseMsgCreate((HUINT8*)ONDK_GetString(RES_MESG_4367_ID));
			ONDK_GWM_APP_Destroy(0);
		}
	}
	else if (p2 == eNxStorageState_Update)
	{
		if (pstContents->ulDeviceId == (HUINT32)p1)
		{
			NX_PhysicalStorageData_t	*physicalStorageData = NULL;
			HERROR						hError;
			HUINT32						ulUsedPercent = 0, ulAvailPercent = 0;
			HINT32						lObjId;
			HFLOAT64					fMemory = 0;

			hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->ulDeviceId, &physicalStorageData);
			if (hError != ERR_OK || physicalStorageData == NULL)
			{
				NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
				return ONDK_RET_FAIL;
			}
			/* Size Update */
			fMemory = (double)physicalStorageData->ullTotalSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucTotalStorage, "%.1f%s", fMemory, "GB");
			fMemory = (double)physicalStorageData->ullReservedSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucSystemUsage, "%.1f%s", fMemory, "GB");
			fMemory = (double)physicalStorageData->ullUsedSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucUserUsage, "%.1f%s", fMemory, "GB");
			fMemory = (double)physicalStorageData->ullAvailableSize / (1024 * 1024);
			HxSTD_sprintf((char*)pstContents->ucAvailable, "%.1f%s", fMemory, "GB");

			/* Percent Update */
			nx_menuDeviceSettings_CalcUsedAvailPercent(physicalStorageData, &ulUsedPercent, &ulAvailPercent);
			HxSTD_sprintf((char*)pstContents->ucUsedPercent, "%d%%", ulUsedPercent);
			HxSTD_sprintf((char*)pstContents->ucAvailPercent, "%d%%", ulAvailPercent);

			lObjId = nx_menuDeviceSettings_ConvertItemIdxToObjId(eNxSettingsItem_Storage_PercentBar);
			ONDK_GWM_COM_Status_SetValue(lObjId, ulUsedPercent /* * (NX_MENU_DEVICE_SETTING_INFO_GRAPH_W / 100) */);
		}
	}

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuDeviceSettings_MsgAppDlgClicked(Nx_MenuDeviceSetting_Contents_t *pstContents, HINT32 lClickedBtn)
{
	switch(lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT: // YES
			nx_menuDeviceSettings_SelectiveMsgDestroy();
			if (pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Selective_Format)
			{
				nx_menuDeviceSettings_Ui_PinCodeMsgCreate();
				pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_PinCode;
			}
			else if (pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Selective_Pairing)
			{
				nx_menuDeviceSettings_Ui_ProcessingMsgCreate(eNxSettingsItem_Storage_Progress_Pairing);
				pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Progress_Pairing;
				NX_FILEMGR_STORAGE_PAIRING_HDD(pstContents->ulDeviceId, TRUE);
			}
			else if (pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Selective_UnPairing)
			{
				nx_menuDeviceSettings_Ui_ProcessingMsgCreate(eNxSettingsItem_Storage_Progress_UnPairing);
				pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Progress_UnPairing;
				NX_FILEMGR_STORAGE_PAIRING_HDD(pstContents->ulDeviceId, FALSE);
			}
			else
			{
				pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			}
			break;
		default:
			nx_menuDeviceSettings_SelectiveMsgDestroy();
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_None;
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_MESSAGE_BREAK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Storage_DeviceSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuDeviceSetting_Contents_t *pstContents = NULL;

	pstContents = nx_menuDeviceSettings_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			// p1 : AntennaType, p2 : Antenna Slot index, p3 : Antenna uid
			eResult = nx_menuDeviceSettings_MsgGwmCreate(pstContents, p1, p2, p3);
			break;


		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			if (pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Progress_Formatting ||
				pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Progress_HDDTesting ||
				pstContents->ulCurrentProgSelectDlg == eNxSettingsItem_Storage_Progress_Pairing)
			{
				eResult = ONDK_MESSAGE_BREAK;
			}
			else
			{
				eResult = nx_menuDeviceSettings_MsgGwmKeyDown(pstContents, p1, p2, p3);
			}
			break;


		/* popup list & Action */
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuDeviceSettings_MsgGwmClicked(pstContents, p1);
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuDeviceSettings_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuDeviceSettings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_CORE_STORAGE_FORMAT_PROGRESS:
		case MSG_CORE_STORAGE_FORMAT_FAILED:
		case MSG_CORE_STORAGE_FORMAT_DONE:
			eResult = nx_menuDeviceSettings_MsgCoreFormatState(pstContents, p1, p2, lMessage);
			break;

		case MSG_CORE_STORAGE_DST_STATE:
			eResult = nx_menuDeviceSettings_MsgCoreDSTState(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_STORAGE_DEVICE_CHANGE:
			NX_APP_Message("MSG_CORE_STORAGE_DEVICE_CHANGE!\n");
			eResult = nx_menuDeviceSettings_MsgCoreDeviceChange(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_STORAGE_PAIRING_FAILED:
		case MSG_CORE_STORAGE_PAIRING_DONE:
		case MSG_CORE_STORAGE_UNPAIRING_FAILED:
		case MSG_CORE_STORAGE_UNPAIRING_DONE:
			eResult = nx_menuDeviceSettings_MsgCorePairingState(pstContents, p1, p2, lMessage);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_menuDeviceSettings_MsgAppDlgClicked(pstContents, p1);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_FILEMGR_STORAGE_FormatAll(pstContents->ulDeviceId);
			nx_menuDeviceSettings_Ui_ProcessingMsgCreate(eNxSettingsItem_Storage_Progress_Formatting);
			pstContents->ulCurrentProgSelectDlg = eNxSettingsItem_Storage_Progress_Formatting;
			break;
		case MSG_APP_SYSTEM_PIN_DENIED:
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
#endif

