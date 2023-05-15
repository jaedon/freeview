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
 * @file	  		nx_prism_menu_config.c
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

#include <stdio.h>
#include <stdlib.h>
#include <nx_common.h>
#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum {
	eNxUiConfigIndex_0 = 0,
	eNxUiConfigIndex_1,
	eNxUiConfigIndex_2,
	eNxUiConfigIndex_3,
	eNxUiConfigIndex_4,
	eNxUiConfigIndex_5,
	eNxUiConfigIndex_6,
	eNxUiConfigIndex_7,
	eNxUiConfigIndex_8,
	eNxUiConfigIndex_9,
	eNxUiConfigIndex_10,
	eNxUiConfigIndex_Max,
} NxUiConfig_AppIndex_t;


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define ___General_Setting_____________________________________________________
/******************************************************************************/
static void		nx_prism_menuconfig_InitMenuDataGeneral_Base(void);
static void		nx_prism_menuconfig_InitMenuDataGeneral_MBC(void);
static void		nx_prism_menuconfig_InitMenuDataGeneral_AMX(void);

/******************************************************************************/
#define ___Network_Setting_____________________________________________________
/******************************************************************************/
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
static void		nx_prism_menuconfig_InitMenuDataNetwork_Base(void);
#endif

/******************************************************************************/
#define ___PVR_Setting_____________________________________________________
/******************************************************************************/
#if defined(CONFIG_MW_MM_PVR)
static void		nx_prism_menuconfig_InitMenuDataPVR_Base(void);
#endif

/******************************************************************************/
#define ___System_Setting_____________________________________________________
/******************************************************************************/
static void		nx_prism_menuconfig_InitMenuDataSystem_Base(void);
static void		nx_prism_menuconfig_InitMenuDataSystem_MBC(void);

/******************************************************************************/
#define ___Channel_Setting_____________________________________________________
/******************************************************************************/
static void		nx_prism_menuconfig_InitMenuDataChannel_Base(void);
static void		nx_prism_menuconfig_InitMenuDataChannel_MBC(void);
static void		nx_prism_menuconfig_InitMenuDataChannel_AMX(void);


/******************************************************************************/
#define ___Menu_Config_Setting_________________________________________________
/******************************************************************************/
static void		nx_prism_menuconfig_InitMainMenuData_Base(void);
static void		nx_prism_menuconfig_InitMainMenuData_MBC(void);

static void		nx_prism_menuconfig_InitSystemLanguage(void);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define ___General_Setting_____________________________________________________
/******************************************************************************/

static void		nx_prism_menuconfig_InitMenuDataGeneral_Base(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_General,
			eNxSetting_ParentalControl,
			RES_PARENTAL_CONTROL_02_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_ParentalControl_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_General,
			eNxSetting_Language,
			RES_LANGUAGE_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Language_Proc
		},
		{
			eNxSettingIndex_2,
			eNxSetting_General,
			eNxSetting_Video,
			RES_VIDEO_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Video_Proc
		},
		{
			eNxSettingIndex_3,
			eNxSetting_General,
			eNxSetting_Audio,
			RES_AUDIO_03_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Audio_Proc
		},
		{
			eNxSettingIndex_4,
			eNxSetting_General,
			eNxSetting_OnScreenDisplay,
			RES_ON_SCREEN_DISPLAY_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Osd_Proc
		},
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
		{
			eNxSettingIndex_5,
			eNxSetting_General,
			eNxSetting_PrivacyPolicy,
			RES_PRIVACY_POLICY_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_General);
	}

}


static void		nx_prism_menuconfig_InitMenuDataGeneral_MBC(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_General,
			eNxSetting_ParentalControl,
			RES_PARENTAL_CONTROL_02_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_ParentalControl_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_General,
			eNxSetting_Language,
			RES_LANGUAGE_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Language_Proc
		},
		{
			eNxSettingIndex_2,
			eNxSetting_General,
			eNxSetting_Time,
			RES_TIME_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Time_Proc
		},
		{
			eNxSettingIndex_3,
			eNxSetting_General,
			eNxSetting_Video,
			RES_VIDEO_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Video_Proc
		},
		{
			eNxSettingIndex_4,
			eNxSetting_General,
			eNxSetting_Audio,
			RES_AUDIO_03_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Audio_Proc
		},
		{
			eNxSettingIndex_5,
			eNxSetting_General,
			eNxSetting_OnScreenDisplay,
			RES_ON_SCREEN_DISPLAY_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Osd_Proc
		},
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
		{
			eNxSettingIndex_6,
			eNxSetting_General,
			eNxSetting_PrivacyPolicy,
			RES_PRIVACY_POLICY_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_General);
	}
}


static void		nx_prism_menuconfig_InitMenuDataGeneral_AMX(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_General,
			eNxSetting_ParentalControl,
			RES_PARENTAL_CONTROL_02_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_ParentalControl_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_General,
			eNxSetting_Language,
			RES_LANGUAGE_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Language_Proc
		},
#if defined(CONFIG_OP_AMX)
		{
			eNxSettingIndex_2,
			eNxSetting_General,
			eNxSetting_Time,
			RES_TIME_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_TimeInfo_Proc
		},
#endif
		{
			eNxSettingIndex_3,
			eNxSetting_General,
			eNxSetting_Video,
			RES_VIDEO_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Video_Proc
		},
		{
			eNxSettingIndex_4,
			eNxSetting_General,
			eNxSetting_Audio,
			RES_AUDIO_03_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Audio_Proc
		},
		{
			eNxSettingIndex_5,
			eNxSetting_General,
			eNxSetting_OnScreenDisplay,
			RES_ON_SCREEN_DISPLAY_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Osd_Proc
		},
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
		{
			eNxSettingIndex_6,
			eNxSetting_General,
			eNxSetting_PrivacyPolicy,
			RES_PRIVACY_POLICY_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_General);
	}

}


/******************************************************************************/
#define ___Network_Setting_____________________________________________________
/******************************************************************************/

#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
static void		nx_prism_menuconfig_InitMenuDataNetwork_Base(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
#if defined(CONFIG_MW_INET)
		{
			eNxSettingIndex_0,
			eNxSetting_Network,
			eNxSetting_ConfigureLan,
			RES_CONFIGURE_LAN_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
#if defined(CONFIG_MW_INET_WLAN)

		{
			eNxSettingIndex_1,
			eNxSetting_Network,
			eNxSetting_ConfigureWLan,
			RES_CONFIGURE_WLAN_01_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_Network);
	}

}
#endif

/******************************************************************************/
#define ___PVR_Setting_____________________________________________________
/******************************************************************************/

#if defined(CONFIG_MW_MM_PVR)
static void		nx_prism_menuconfig_InitMenuDataPVR_Base(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
#if defined(CONFIG_SUPPORT_IPEPG_APP)
		{
			eNxSettingIndex_0,
			eNxSetting_Pvr,
			eNxSetting_IPEpg,
			RES_IP_EPG_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_IpEpg_Proc
		},
#endif
		{
			eNxSettingIndex_1,
			eNxSetting_Pvr,
			eNxSetting_RemoteRecord,
			RES_REMOTE_RECORDING_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_RemoteRecord_Proc
		},
		{
			eNxSettingIndex_2,
			eNxSetting_Pvr,
			eNxSetting_RecordOption,
			RES_RECORDING_OPTIONS_01_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_RecordOption_Proc
		},
		{
			eNxSettingIndex_3,
			eNxSetting_Pvr,
			eNxSetting_PlaybackOption,
			RES_PLAYBACK_OPTIONS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_PlaybackOption_Proc
		},
		{
			eNxSettingIndex_4,
			eNxSetting_Pvr,
			eNxSetting_Storage,
			RES_STORAGE_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Storage_Proc
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_Pvr);
	}

}
#endif

/******************************************************************************/
#define ___System_Setting_____________________________________________________
/******************************************************************************/

static void		nx_prism_menuconfig_InitMenuDataSystem_Base(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_System,
			eNxSetting_SystemInfomation,
			RES_SYSTEM_INFORMATION_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SysInfo_Proc
		},
#if defined(CONFIG_MW_CAS_NAGRA)
		{
			eNxSettingIndex_1,
			eNxSetting_System,
			eNxSetting_SmartcardInfomation,
			RES_SMARTCARD_INFORMATION_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SmtCardInfo_NA_Proc
		},
#endif
#if defined(CONFIG_MW_CAS_IREDETO)
		{
			eNxSettingIndex_1,
			eNxSetting_System,
			eNxSetting_SmartcardInfomation,
			RES_SMARTCARD_INFORMATION_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SmtCardInfo_IR_Proc
		},
#endif
		{
			eNxSettingIndex_2,
			eNxSetting_System,
			eNxSetting_PowerTimer,
			RES_POWER_TIMER_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_PowerTimer_Proc
		},
		{
			eNxSettingIndex_3,
			eNxSetting_System,
			eNxSetting_PowerManagement,
			RES_POWER_MANAGEMENT_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_PowerManagement_Proc
		},
#if defined(CONFIG_MW_CEC)
		{
			eNxSettingIndex_4,
			eNxSetting_System,
			eNxSetting_HDMICEC,
			RES_HDMI_CEC_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_HDMICEC_Proc
		},
#endif
		{
			eNxSettingIndex_5,
			eNxSetting_System,
			eNxSetting_FactoryDefault,
			RES_FACTORY_DEFAULT_01_ID,
			(ONDK_GWM_Callback_t)NULL
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_System);
	}

}


static void		nx_prism_menuconfig_InitMenuDataSystem_MBC(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_System,
			eNxSetting_SystemInfomation,
			RES_SYSTEM_INFORMATION_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SysInfo_Proc
		},
//#if defined(CONFIG_MW_CAS_IREDETO)
		{
			eNxSettingIndex_1,
			eNxSetting_System,
			eNxSetting_CustomerRegistration,
			RES_CUSTOMER_REGISTRATION_ID,
			(ONDK_GWM_Callback_t)NULL
		},
		{
			eNxSettingIndex_2,
			eNxSetting_System,
			eNxSetting_IREDETOSoftwareUpdate,
			RES_IREDETO_SOFTWARE_UPDATE_ID,
			(ONDK_GWM_Callback_t)NULL
		},
//#endif
		{
			eNxSettingIndex_3,
			eNxSetting_System,
			eNxSetting_PowerTimer,
			RES_POWER_TIMER_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_PowerTimer_Proc
		},
#if defined(CONFIG_MW_CEC)
		{
			eNxSettingIndex_4,
			eNxSetting_System,
			eNxSetting_HDMICEC,
			RES_HDMI_CEC_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_HDMICEC_Proc
		},
#endif
//#if defined(CONFIG_MW_CAS_IREDETO)
		{
			eNxSettingIndex_5,
			eNxSetting_System,
			eNxSetting_ConditionalAccess,
			RES_CONDITIONAL_ACCESS_01_ID,
			(ONDK_GWM_Callback_t)NULL
		},
		{
			eNxSettingIndex_6,
			eNxSetting_System,
			eNxSetting_MailMessage,
			RES_MAIL_MESSAGE_ID,
			(ONDK_GWM_Callback_t)NULL
		},
//#endif
		{
			eNxSettingIndex_7,
			eNxSetting_System,
			eNxSetting_FactoryDefault,
			RES_FACTORY_DEFAULT_01_ID,
			(ONDK_GWM_Callback_t)NULL
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_System);
	}

}


/******************************************************************************/
#define ___Channel_Setting_____________________________________________________
/******************************************************************************/

static void		nx_prism_menuconfig_InitMenuDataChannel_Base(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_Channel,
			eNxSetting_EditFavList,
			RES_FAVOURITE_CHANNELS_01_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_FavChList_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_Channel,
			eNxSetting_EditChList,
			RES_EDIT_CHANNEL_LIST_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Proc
		},
#if 0
		{
			eNxSettingIndex_2,
			eNxSetting_Channel,
			eNxSetting_EditPreferredList,
			RES_EDIT_PREFERRED_LIST_ID,
			(ONDK_GWM_Callback_t)NULL
		},
#endif
		{
			eNxSettingIndex_3,
			eNxSetting_Channel,
			eNxSetting_SignalTest,
			RES_SIGNAL_TEST_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SignalTest_Proc
		},
		{
			eNxSettingIndex_4,
			eNxSetting_Channel,
			eNxSetting_SearchChannel,
			RES_SEARCH_CHANNELS_ID,
			(ONDK_GWM_Callback_t)NULL
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_Channel);
	}

}


static void		nx_prism_menuconfig_InitMenuDataChannel_MBC(void)
{
	HUINT32				ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo		astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_Channel,
			eNxSetting_EditFavList,
			RES_FAVOURITE_CHANNELS_01_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_FavChList_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_Channel,
			eNxSetting_EditChList,
			RES_EDIT_CHANNEL_LIST_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Proc
		},
		{
			eNxSettingIndex_2,
			eNxSetting_Channel,
			eNxSetting_SignalTest,
			RES_SIGNAL_TEST_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SignalTest_Proc
		},
		{
			eNxSettingIndex_3,
			eNxSetting_Channel,
			eNxSetting_SearchChannel,
			RES_SEARCH_CHANNELS_ID,
			(ONDK_GWM_Callback_t)NULL
		},
		{
			eNxSettingIndex_4,
			eNxSetting_Channel,
			eNxSetting_ImportChannel,
			RES_IMPORT_CHANNELS_ID,
			(ONDK_GWM_Callback_t)NULL
		},
		{
			eNxSettingIndex_5,
			eNxSetting_Channel,
			eNxSetting_ExportChannel,
			RES_EXPORT_CHANNELS_ID,
			(ONDK_GWM_Callback_t)NULL
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_Channel);
	}

}


static void		nx_prism_menuconfig_InitMenuDataChannel_AMX(void)
{
	HUINT32 			ulMenuNum = 0, i = 0;
	NxUiMenuCfgInfo 	astMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_Channel,
			eNxSetting_EditFavList,
			RES_FAVOURITE_CHANNELS_01_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_FavChList_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_Channel,
			eNxSetting_EditChList,
			RES_EDIT_CHANNEL_LIST_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_EditChList_Proc
		},
		{
			eNxSettingIndex_2,
			eNxSetting_Channel,
			eNxSetting_SignalTest,
			RES_SIGNAL_TEST_ID,
#if defined(CONFIG_OP_AMX)
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SignalTestAMX_Proc
#else
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_SignalTest_Proc
#endif
		},
		{
			eNxSettingIndex_3,
			eNxSetting_Channel,
			eNxSetting_SearchChannel,
			RES_SEARCH_CHANNELS_ID,
			(ONDK_GWM_Callback_t)NULL
		},
	};

	ulMenuNum = (sizeof(astMenuInfo) / sizeof(astMenuInfo[0]));
	for (i = 0; i < ulMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMenuItem(astMenuInfo[i], (HINT32)eNxSetting_Channel);
	}

}


/******************************************************************************/
#define ___Menu_Config_Setting_________________________________________________
/******************************************************************************/

static void		nx_prism_menuconfig_InitMainMenuData_Base(void)
{
	HUINT32					ulMainMenuNum = 0, i = 0;
	NxUiMainMenuCfgInfo		astMainMenuInfo[] = {
		{
			eNxSettingIndex_0,
			eNxSetting_General,
			RES_GENERAL_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_General_Proc
		},
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
		{
			eNxSettingIndex_2,
			eNxSetting_Network,
			RES_NETWORK_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc
		},
#endif
#if defined(CONFIG_MW_MM_PVR)
		{
			eNxSettingIndex_3,
			eNxSetting_Pvr,
			RES_PVR_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Pvr_Proc
		},
#endif
		{
			eNxSettingIndex_4,
			eNxSetting_System,
			RES_SYSTEM_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc
		},
		{
			eNxSettingIndex_5,
			eNxSetting_Channel,
			RES_CHANNEL_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc
		},
	};

	ulMainMenuNum = (sizeof(astMainMenuInfo) / sizeof(astMainMenuInfo[0]));
	for (i = 0; i < ulMainMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMainMenuItem(astMainMenuInfo[i]);
	}

}

static void 	nx_prism_menuconfig_InitMainMenuData_MBC(void)
{
	HUINT32 				ulMainMenuNum = 0, i = 0;
	NxUiMainMenuCfgInfo 	astMainMenuInfo[] = {

		{
			eNxSettingIndex_0,
			eNxSetting_System,
			RES_SYSTEM_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc
		},
		{
			eNxSettingIndex_1,
			eNxSetting_General,
			RES_GENERAL_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_General_Proc
		},
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
		{
			eNxSettingIndex_2,
			eNxSetting_Network,
			RES_NETWORK_SETTINGS_ID,
		(ONDK_GWM_Callback_t)NX_PRISM_MEMNU_Network_Proc
		},
#endif
	{
			eNxSettingIndex_3,
			eNxSetting_Channel,
			RES_CHANNEL_SETTINGS_ID,
			(ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc
		},
	};

	ulMainMenuNum = (sizeof(astMainMenuInfo) / sizeof(astMainMenuInfo[0]));
	for (i = 0; i < ulMainMenuNum; i++)
	{
		NX_PRISM_AppNaviCntxt_AddMainMenuItem(astMainMenuInfo[i]);
	}

}


static void		nx_prism_menuconfig_InitSystemLanguage(void)
{
	HINT32	nValue = 0;
	HERROR	hError = ERR_OK;

	hError = NX_SYSTEM_GetMenuLanuage(&nValue);
	if (hError == ERR_OK)
	{
		NX_SYSTEM_SetMenuLanuage(nValue);
	}
}


/******************************************************************************/
#define _________Public_functions____________________________________________
/******************************************************************************/

void	NX_PRISM_MENUCONFIG_InitMenuParentalControlData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_MENU_MenuItem_LoadParentalContrl(FALSE);
			NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_VIEW_ALL, RES_G_ID);
			NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_12, RES_PG_ID);
			NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_12, RES_PG_13_ID);
			NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_16, RES_R_408_ID);
			NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_17, RES_NC_17_ID);
			break;

		default:
			// Load Parental Control List by product country config
			NX_PRISM_MENU_MenuItem_LoadParentalContrl(TRUE);
			break;
	}

}


void	NX_PRISM_MENUCONFIG_InitMenuCountryData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_MENU_MenuItem_LoadCountryList(FALSE);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_OTHER);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SLV);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_GTM);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_HND);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_DOM);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_NIC);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_PAN);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_CRI);
			break;

		case eDxOPERATOR_MBC:
			NX_PRISM_MENU_MenuItem_LoadCountryList(FALSE);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_BHR);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_EGY);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_GRC);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_IRN);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_IRQ);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_KWT);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_OMN);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_QAT);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SAU);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_TUR);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_ARE);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_TUN);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_DZA);
			NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_MAR);
			break;

		default:
			// Load Country List by product country config
			NX_PRISM_MENU_MenuItem_LoadCountryList(TRUE);
			break;
	}

}


void	NX_PRISM_MENUCONFIG_ReLoadResolutionDataByEDID(void)
{
	HERROR		hError = ERR_FAIL;
	HBOOL		bHdmiPlugInOut = FALSE, bSupport = FALSE;
	HUINT32		ulIdx = 0, ulExistCnt = 0, ulCollectionCnt = 0;
	DxOperator_e			eOpType = eDxOPERATOR_None;
	DxResolutionSelect_e 	eResolutionSelect = eDxRESOLUTION_SELECT_UNKNOWN;
	DxResolutionStatus_e	eResolution;
	DxResolutionStatus_b	ulResolutionCapability = 0;


	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	hError = NX_SYSTEM_GetHdmiIsPlugIn(&bHdmiPlugInOut);
	if ((hError != ERR_OK) || (bHdmiPlugInOut == FALSE))
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetHdmiIsPlugIn() hError:(%d), bHdmiPlugInOut:(%d) \n", hError, bHdmiPlugInOut);
		return;
	}
	else
	{
		hError = NX_SYSTEM_GetHdmiEdidResolutionCapability(&ulResolutionCapability);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_GetHdmiEdidResolutionCapability() Fail!! \n");
			return;
		}
		for (ulIdx = 0; ulIdx < eDxRESOLUTION_SELECT_MAX_NUM; ulIdx++)
		{
			if (ulResolutionCapability & (0x1 << ulIdx))
			{
				ulExistCnt++;
			}
		}

		if (ulExistCnt == 0)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_GetHdmiEdidResolutionCapability() Fail!! \n");
			return;
		}

		NX_PRISM_MENU_MenuItem_LoadResolutionList(FALSE);
		for (ulIdx = 0; ulIdx < eDxRESOLUTION_SELECT_MAX_NUM; ulIdx++)
		{
			eResolution = ulResolutionCapability & (0x1 << ulIdx);
			if (eResolution)
			{
				switch(eResolution)
				{
					case eDxRESOLUTION_STATUS_480I:
						eResolutionSelect = eDxRESOLUTION_SELECT_480I;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_480P:
						eResolutionSelect = eDxRESOLUTION_SELECT_480P;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_576I:
						eResolutionSelect = eDxRESOLUTION_SELECT_576I;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_576P:
						eResolutionSelect = eDxRESOLUTION_SELECT_576P;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_720P:
						eResolutionSelect = eDxRESOLUTION_SELECT_720P;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_1080I:
						eResolutionSelect = eDxRESOLUTION_SELECT_1080I;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_1080P:
						eResolutionSelect = eDxRESOLUTION_SELECT_1080P;
						ulCollectionCnt++;
						break;

					case eDxRESOLUTION_STATUS_2160P:
						eResolutionSelect = eDxRESOLUTION_SELECT_2160P;
						ulCollectionCnt++;
						break;

					default:
						eResolutionSelect = eDxRESOLUTION_SELECT_UNKNOWN;
						break;
				}

				bSupport = NX_PRISM_UTIL_IsSupportResolutionList(eResolutionSelect);
				if (bSupport == TRUE)
				{
					NX_PRISM_MENU_MenuItem_AddResolutionList(eResolutionSelect);
				}
			}

			if (ulCollectionCnt == ulExistCnt)
			{
				break;
			}
		}
	}
}


void	NX_PRISM_MENUCONFIG_InitMenuResolutionData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			// Init Resolution
			NX_PRISM_MENU_MenuItem_LoadResolutionList(FALSE);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_480I);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_480P);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_720P);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_1080I);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_1080P);
			NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_2160P);
			break;
		default:
			// Load Default Resolution Value
			NX_PRISM_MENU_MenuItem_LoadResolutionList(TRUE);
			break;
	}
}


void	NX_PRISM_MENUCONFIG_InitMenuLanguageData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG, eLangID_Spanish, eNxUiConfigIndex_0);
			break;

		case eDxOPERATOR_MBC:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG, eLangID_French, eNxUiConfigIndex_3);
			break;

		default:
			// Load Language List by product language config
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG, TRUE);
			break;
	}

	nx_prism_menuconfig_InitSystemLanguage();
}


void	NX_PRISM_MENUCONFIG_InitAudioLanguageData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG, eLangID_Spanish, eNxUiConfigIndex_0);
			break;

		case eDxOPERATOR_MBC:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG, eLangID_French, eNxUiConfigIndex_3);
			break;

		default:
			// Load Language List by product language config
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG, TRUE);
			break;
	}
}


void	NX_PRISM_MENUCONFIG_InitSubtitleLanguageData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, eLangID_Spanish, eNxUiConfigIndex_0);
			NX_PRISM_MENU_MenuItem_InsertLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, eLangID_NONE, eNxUiConfigIndex_0);
			break;

		case eDxOPERATOR_MBC:
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_MoveLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, eLangID_French, eNxUiConfigIndex_3);
			NX_PRISM_MENU_MenuItem_InsertLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, eLangID_NONE, eNxUiConfigIndex_0);
			break;

		default:
			// Load Language List by product language config
			NX_PRISM_MENU_MenuItem_LoadLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, TRUE);
			NX_PRISM_MENU_MenuItem_InsertLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG, eLangID_NONE, eNxUiConfigIndex_0);
			break;
	}
}


void	NX_PRISM_MENUCONFIG_InitMainMenuData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_MBC:
			nx_prism_menuconfig_InitMainMenuData_MBC();
			break;
		default:
			nx_prism_menuconfig_InitMainMenuData_Base();
			break;
	}
}


void	NX_PRISM_MENUCONFIG_InitSubMenuData(void)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		eOpType = eDxOPERATOR_None;
		NX_APP_Error("Error!!! NX_SYSTEM_GetOperatorType() Fail!! \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_MBC:
			nx_prism_menuconfig_InitMenuDataSystem_MBC();
			nx_prism_menuconfig_InitMenuDataGeneral_MBC();
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
			nx_prism_menuconfig_InitMenuDataNetwork_Base();
#endif
			nx_prism_menuconfig_InitMenuDataChannel_MBC();

			break;
		case eDxOPERATOR_AMX:
			nx_prism_menuconfig_InitMenuDataGeneral_AMX();
#if defined(CONFIG_MW_MM_PVR)
			nx_prism_menuconfig_InitMenuDataPVR_Base();
#endif
			nx_prism_menuconfig_InitMenuDataSystem_Base();
			nx_prism_menuconfig_InitMenuDataChannel_AMX();
			break;
		default:
			nx_prism_menuconfig_InitMenuDataGeneral_Base();
#if defined(CONFIG_SUPPORT_NIMA) && (defined(CONFIG_MW_INET) || defined(CONFIG_MW_INET_WLAN))
			nx_prism_menuconfig_InitMenuDataNetwork_Base();
#endif
#if defined(CONFIG_MW_MM_PVR)
			nx_prism_menuconfig_InitMenuDataPVR_Base();
#endif
			nx_prism_menuconfig_InitMenuDataSystem_Base();
			nx_prism_menuconfig_InitMenuDataChannel_Base();
			break;
	}
}


void	NX_PRISM_MENUCONFIG_Init(void)
{
	/* Set Main Menu Config */
	NX_PRISM_MENUCONFIG_InitMainMenuData();

	/* Set Sub Menu Config */
	NX_PRISM_MENUCONFIG_InitSubMenuData();

	/* Set Menu Language Data */
	NX_PRISM_MENUCONFIG_InitMenuLanguageData();
	NX_PRISM_MENUCONFIG_InitAudioLanguageData();
	NX_PRISM_MENUCONFIG_InitSubtitleLanguageData();

	/* Set Resolution Data */
	NX_PRISM_MENUCONFIG_InitMenuResolutionData();
	/* Set Country Data */
	NX_PRISM_MENUCONFIG_InitMenuCountryData();
	/* Set Parental Control Data */
	NX_PRISM_MENUCONFIG_InitMenuParentalControlData();
}

/******************************************************************************/
#define _________End_Of_File___________________________________________________
/******************************************************************************/
