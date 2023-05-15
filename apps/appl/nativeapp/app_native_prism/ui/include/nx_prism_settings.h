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
 * @file	  		nx_prism_settings.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_SETTINGS_H__
#define __NX_PRISM_SETTINGS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <ondk.h>
#include <ondk_type.h>
#include <ondk_gfx.h>
#include <hlib.h>

#include <nx_common.h>

//#include <nx_prism_app.h>
//#include <nx_prism_menu_contents.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	MAX_ELEMENT_TEXT_LENGTH						128
#define	MAX_ITEM_TEXT_LENGTH						32

#define	NX_SETTING_DEPTH_BYTE_MASK					0x0F000000
#define	NX_SETTING_DEPTH_BYTE_LSHIFT_BITS			24
#define	NX_SETTING_APP_ID_BYTE_MASK					0x00FF0000
#define	NX_SETTING_APP_ID_LSHIFT_BITS				16

#define NX_SETTING_TIMEOUT_ID						0xf000
#define NX_SETTING_TIMEOUT							(3*60*NX_COMMON_TIME_SECOND)	/* 3 minutes */

#define NX_SETTING_TIME_UPDATE_TIME					(5*NX_COMMON_TIME_SECOND)		/* 5 Second */
#define NX_SETTING_FAVOURITE_GROUP_NUM 				5

#define	NX_SETTING_GET_AP_DEPTH(x)					(((x) & NX_SETTING_DEPTH_BYTE_MASK) >> NX_SETTING_DEPTH_BYTE_LSHIFT_BITS)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	HBOOL	(*Setting_CheckItemIsDisable)(HINT32 lItemIdx);
typedef HINT32	(*Setting_ConvertObjIdToItemIdx)(HINT32 lObjId);
typedef HINT32	(*Setting_ConvertItemIdxToObjId)(HINT32 lIdx);

typedef enum {
	eNxSettingDepth_0 = 0,
	eNxSettingDepth_1,
	eNxSettingDepth_2,
	eNxSettingDepth_3,
	eNxSettingDepth_4,
	eNxSettingDepth_Max,
} NxSetting_AppDepth_t;

typedef enum {
	eNxSettingIndex_0 = 0,
	eNxSettingIndex_1,
	eNxSettingIndex_2,
	eNxSettingIndex_3,
	eNxSettingIndex_4,
	eNxSettingIndex_5,
	eNxSettingIndex_6,
	eNxSettingIndex_7,
	eNxSettingIndex_8,
	eNxSettingIndex_9,
	eNxSettingIndex_10,
	eNxSettingIndex_Max,
} NxSetting_AppIndex_t;

typedef enum tagNxSettingNaviAppId {
	/* top boundary */
	eNxSetting_App_None,

	/* 0 depth */
	eNxSetting_MenuSettings		= ((eNxSettingDepth_0 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	/* 1 depth */
	eNxSetting_General			= ((eNxSettingDepth_1 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Network			= ((eNxSettingDepth_1 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x2 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Pvr				= ((eNxSettingDepth_1 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x3 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_System			= ((eNxSettingDepth_1 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x4 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Channel			= ((eNxSettingDepth_1 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x5 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	/* 2 depth */
	// General
	eNxSetting_ParentalControl	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Language			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x2 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Time				= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x3 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Video			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x4 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Audio			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x5 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_OnScreenDisplay	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x6 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_PrivacyPolicy	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x7 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// Network
	eNxSetting_ConfigureLan		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x8 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_ConfigureWLan	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x9 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_NetworkServer	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xa << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Bluetooth		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xb << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// PVR
	eNxSetting_IPEpg			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xc << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_RemoteRecord		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xd << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_RecordOption		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xe << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_PlaybackOption	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0xf << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_Storage			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x10 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// System Settings
	eNxSetting_SystemInfomation	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x11 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SmartcardInfomation	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x12 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_PowerTimer 		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x13 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_PowerManagement	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x14 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_CommonInterface	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x15 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_FactoryDefault	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x16 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_HiddenInfomation	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x17 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_HDMICEC			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x18 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// Channel Settings
	eNxSetting_EditFavList			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x19 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_EditChList			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1a << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_EditPreferredList	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1b << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SignalTest			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1c << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SearchChannel 		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1d << NX_SETTING_APP_ID_LSHIFT_BITS)),

    // Network
	eNxSetting_ConnectionStatus     = ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1e << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// System
	eNxSetting_CustomerRegistration	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1f << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_IREDETOSoftwareUpdate= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x20 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_ConditionalAccess	= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x21 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_MailMessage			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x22 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	// Channel
	eNxSetting_ImportChannel		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x23 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_ExportChannel		= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x24 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	eNxSetting_UpdateList			= ((eNxSettingDepth_2 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x25 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	/* 3 depth */
	eNxSetting_LipSync				= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x1 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SatelliteSetting		= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x2 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SatelliteTPSetting	= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x3 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SatelliteOption		= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x4 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SearchOption			= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x5 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_SearchUi				= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x6 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_EditChListOption		= ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x7 << NX_SETTING_APP_ID_LSHIFT_BITS)),
	eNxSetting_StorageDeviceSetting = ((eNxSettingDepth_3 << NX_SETTING_DEPTH_BYTE_LSHIFT_BITS) | (0x8 << NX_SETTING_APP_ID_LSHIFT_BITS)),

	eNxSetting_App_MAX,
} NxSetting_AppId_t;


typedef enum {
	eUI_COMP_Button,
	eUI_COMP_Text,
	eUI_COMP_Image,
	eUI_COMP_Max
} UiComponentType_t;


typedef struct tag_tmpItems {
	HUINT32			ulId;
	ONDK_Rect_t		rect;
} Item_t;


typedef struct tagSettingsNaviSubItem {

	HCHAR		*hcTitleLocId;
	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];
} Nx_SettingsNaviSubItem_t;


typedef struct tagSettingsNaviMainItem {

	/* main item title */
	HUINT32		ulMainBtnId;
	HUINT32		ulMainBtnIdx;
	HCHAR		*ucTitleLocId;
	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL		bDisable;
	HBOOL		bHide;

	HINT32		lSubItemValue;							///< sub item value
	HUINT8		ucSubItemString[MAX_ITEM_TEXT_LENGTH];	///< sub item text

	HUINT32		ulNumOfSubItem, ulMaxOfSubItem;
	Nx_SettingsNaviSubItem_t		*subItem;

	HUINT8		ucAppName[MAX_ELEMENT_TEXT_LENGTH];
	ONDK_GWM_Callback_t	appToLaunch;
	Handle_t	hAction;
	HUINT32		ulappParam1;
	HUINT32		ulappParam2;
	HUINT32		ulappParam3;
} Nx_SettingsNaviMainItem_t;


typedef struct tagSettingsContext {

	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
	HINT32		lDefaultFocusIdx;

	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	Setting_ConvertObjIdToItemIdx	pConvFunc;
	Nx_SettingsNaviMainItem_t	*mainItems;

	HINT32		lToUpper;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
} Nx_SettingsNaviContext_t;


typedef struct tagSettingsLeafSubItem {

	HUINT32		ulId;
	ONDK_Rect_t	rect;
	HBOOL		bDisable;							///< if TRUE, is disable
	HUINT8		*pszHelpText;

	HUINT32		ulMainItemId;
	HUINT8		szMainItemText[MAX_ELEMENT_TEXT_LENGTH];
													///< itemTextList value
	HINT32		lSubItemValue;						///< sub item value
	HUINT8		szItemString[MAX_ITEM_TEXT_LENGTH];	///< sub item text
													///< for selector, input obj
	HUINT32		ulNumOfItemTextList;				///< total num of sub item
	HUINT32		ulIdxOfTextList;					///< index of sub item
	HUINT8		**szItemTextList;					///< list of sub item

	/* to launch */
	HUINT8		szSubItemText[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8		szAppName[MAX_ELEMENT_TEXT_LENGTH];
	ONDK_GWM_Callback_t	childAppToLaunch;
	Handle_t	hAction;
	HUINT32		ulappParam1;
	HUINT32		ulappParam2;
	HUINT32		ulappParam3;
} Nx_SettingsLeafSubItem_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
#define _________NX_PRISM_SETTINGS_Prototype_________________________
/*******************************************************************/


#endif // __NX_PRISM_SETTINGS_H__

