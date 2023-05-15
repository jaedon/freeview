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
 * @file	  		nx_prism_menu_items.c
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
#include <nx_prism_menu_items.h>
#include <nx_core.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define NX_PRISM_UI_MENU_ITEMS_SHOW_NUMBER_LEN			4


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static HxVector_t		*s_stMenuLanguageList;
static HxVector_t		*s_stAudioLanguageList;
static HxVector_t		*s_stSubtitleLanguageList;
static HxVector_t		*s_stResolutionList;
static HxVector_t		*s_stCountryList;
static HxVector_t		*s_stParentalContrlList;


/******************************************************************************/
#define ___Common_Item_________________________________________________________
/******************************************************************************/

static NxMenuItemText_t		s_stOffOnSetting[] = {
	{FALSE,		RES_OFF_ID,		NULL },
	{TRUE, 		RES_ON_ID,		NULL },
};
#define	NX_MENU_OFF_ON_ITEMS						(sizeof(s_stOffOnSetting) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stOnOffSetting[] = {
	{TRUE, 		RES_ON_ID,		NULL },
	{FALSE,		RES_OFF_ID,		NULL },
};
#define	NX_MENU_ON_OFF_ITEMS						(sizeof(s_stOnOffSetting) / sizeof(NxMenuItemText_t))


/******************************************************************************/
#define ___MENU_Item_________________________________________________________
/******************************************************************************/

static	NxMenuItemText_t		s_stScreenAspectRatio[] = {
	{ eDxASPECTRATIO_4X3,		RES_43_ID, 		"4:3",	},
	{ eDxASPECTRATIO_16X9,		RES_169_ID, 	"16:9",	},
};
#define	NX_MENU_ASPECT_RATIO_ITEMS					(sizeof(s_stScreenAspectRatio) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t		s_stDisplayFormat_4_3[] = {
	{ eDxDFC_SELECT_DEFAULT,		RES_AUTO_ID,		"Auto" 		},
	{ eDxDFC_SELECT_LETTERBOX, 		RES_LETTERBOX_ID,	"Letterbox"	},
	{ eDxDFC_SELECT_CENTERCUTOUT, 	RES_CENTRE_ID,		"Centre"	},
};
#define	NX_MENU_DISPLY_FORMAT_43_ITEMS				(sizeof(s_stDisplayFormat_4_3) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t		s_stDisplayFormat_16_9[] = {
	{ eDxDFC_SELECT_DEFAULT, 	RES_AUTO_ID, 		"Auto"		},
	{ eDxDFC_SELECT_PILLARBOX, 	RES_PILLARBOX_ID,	"pillarbox"	},
	{ eDxDFC_SELECT_ZOOM, 		RES_ZOOM_ID,		"Zoom"		},
	{ eDxDFC_SELECT_FULLSCREEN,	RES_FULL_ID,		"Full"		},

};
#define	NX_MENU_DISPLY_FORMAT_169_ITEMS			(sizeof(s_stDisplayFormat_16_9) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t		s_stAudio_SPDIF[] = {
	{ eDxDIGITALAUDIO_FORMAT_PCM,			RES_STEREO_ID,	NULL},
	{ eDxDIGITALAUDIO_FORMAT_MULTICHANNEL,	RES_DOLBY_ID,	NULL},
};
#define	NX_MENU_AUDIO_SPDIF_ITEMS			(sizeof(s_stAudio_SPDIF) / sizeof(NxMenuItemText_t))


static	NxMenuItemText_t		s_stAudio_HDMI[] = {
	{ eDxDIGITALAUDIO_FORMAT_PCM,			RES_STEREO_ID,	NULL},
	{ eDxDIGITALAUDIO_FORMAT_MULTICHANNEL,	RES_DOLBY_ID,	NULL},
};
#define	NX_MENU_AUDIO_HDMI_ITEMS			(sizeof(s_stAudio_HDMI) / sizeof(NxMenuItemText_t))


static	NxMenuItemText_t		s_stAudio_AudioType[] = {
	{ eDxSTEREO_SELECT_STEREO,		RES_STEREO_ID,		NULL},
	{ eDxSTEREO_SELECT_MONOLEFT,	RES_LEFT_ID,		NULL},
	{ eDxSTEREO_SELECT_MONORIGHT,	RES_RIGHT_ID,		NULL},
};
#define	NX_MENU_AUDIO_AUDIOTYPE_ITEMS			(sizeof(s_stAudio_AudioType) / sizeof(NxMenuItemText_t))


static  NxMenuItemText_t        s_stInfoDisplayTime[] = {
    { 0,        RES_OFF_ID,     NULL},
    { 1,        RES_1_SEC_ID,   NULL},
    { 2,        RES_2_SEC_ID,   NULL},
    { 3,        RES_3_SEC_ID,   NULL},
    { 4,        RES_4_SEC_ID,   NULL},
    { 5,        RES_5_SEC_ID,   NULL},
    { 6,        RES_6_SEC_ID,   NULL},
    { 7,        RES_7_SEC_ID,   NULL},
    { 8,        RES_8_SEC_ID,   NULL},
    { 9,        RES_9_SEC_ID,   NULL},
    { 10,       RES_10_SEC_ID,  NULL},
    { 11,       RES_11_SEC_ID,  NULL},
    { 12,       RES_12_SEC_ID,  NULL},
    { 13,       RES_13_SEC_ID,  NULL},
    { 14,       RES_14_SEC_ID,  NULL},
    { 15,       RES_15_SEC_ID,  NULL},
    { 16,       RES_16_SEC_ID,  NULL},
    { 17,       RES_17_SEC_ID,  NULL},
    { 18,       RES_18_SEC_ID,  NULL},
    { 19,       RES_19_SEC_ID,  NULL},
    { 20,       RES_20_SEC_ID,  NULL},
};
#define NX_MENU_INFO_DISPLAY_TIME_ITEMS         (sizeof(s_stInfoDisplayTime) / sizeof(NxMenuItemText_t))


static  NxMenuItemText_t        s_stSubtitleFont[] = {
    { TRUE,     RES_VARIABLE_ID,    "Variable"},
    { FALSE,    RES_FIXED_ID,       "Fixed"},
};
#define NX_MENU_SUBTITLE_FONT_ITEMS         (sizeof(s_stSubtitleFont) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stRepeatOption[] = {
	{eNxMediaPlay_Video_RepeatAll, 		RES_REPEAT_ALL_ID,	NULL},
	{eNxMediaPlay_Video_RepeatOnce, 	RES_ONCE_ID,		NULL},
	{eNxMediaPlay_Video_Repeat1, 		RES_REPEAT_1_ID,	NULL},
};
#define	NX_MENU_REPEATOPTION_ITEMS			(sizeof(s_stRepeatOption) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stTextPosition[] = {
	{ 644,		NULL,		"-1" },
	{ 600,		NULL,		"0" },
	{ 556,		NULL,		"+1" },
};
#define	NX_MENU_TEXTPOSITION_ITEMS			(sizeof(s_stTextPosition) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stFontSize[] = {
	{ 30,		RES_SMALL_ID,		NULL },
	{ 33,		RES_MEDIUM_ID,		NULL },
	{ 36,		RES_LARGE_ID,		NULL },
};
#define	NX_MENU_FONTSIZE_ITEMS				(sizeof(s_stFontSize) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSystemSetPowerOnTimer[] = {
	{FALSE,		RES_OFF_ID,				NULL },
	{TRUE, 		RES_SET_TIMER_ID,		NULL },
};
#define	NX_MENU_POWERONTIMER_ITEMS	(sizeof(s_stSystemSetPowerOnTimer) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSystemSetPowerOffTimer[] = {
	{FALSE,		RES_OFF_ID,				NULL },
	{TRUE, 		RES_SET_TIMER_ID,		NULL },
};
#define	NX_MENU_POWEROFFTIMER_ITEMS	(sizeof(s_stSystemSetPowerOffTimer) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stPowerTimerSettingsVolume[] = {
	{ 0,        NULL,   "0"},
    { 1,        NULL,   "1"},
    { 2,        NULL,   "2"},
    { 3,        NULL,   "3"},
    { 4,        NULL,   "4"},
    { 5,        NULL,   "5"},
    { 6,        NULL,   "6"},
    { 7,        NULL,   "7"},
    { 8,        NULL,   "8"},
    { 9,        NULL,   "9"},
    { 10,       NULL,  "10"},
    { 11,       NULL,  "11"},
    { 12,       NULL,  "12"},
    { 13,       NULL,  "13"},
    { 14,       NULL,  "14"},
    { 15,       NULL,  "15"},
    { 16,       NULL,  "16"},
    { 17,       NULL,  "17"},
    { 18,       NULL,  "18"},
    { 19,       NULL,  "19"},
    { 20,       NULL,  "20"},
};
#define	NX_MENU_POWERTIMERSETTINGS_VOLUME_ITEMS	(sizeof(s_stPowerTimerSettingsVolume) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t 	s_stPowerTimerSettingsRepeat[] = {
	{eDxSCHED_DATE_NONE, RES_ONCE_ID,		NULL},
	{eDxSCHED_DATE_ALL, RES_DAILY_ID,		NULL},
	{eDxSCHED_DATE_SUN, RES_WEEKLY_ID,		NULL},
};
#define NX_MENU_POWERTIMERSETTINGS_REPEAT_ITEMS (sizeof(s_stPowerTimerSettingsRepeat) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t 	s_stScrambledType[] = {
	{eDxCAS_TYPE_All, 	 	RES_ALL_ID, 	NULL},
	{eDxCAS_TYPE_FTA, 		RES_FTA_ID,		NULL},
	{eDxCAS_TYPE_ALLCAS,	RES_CAS_ID,		NULL},
};
#define NX_MENU_SCRAMBLED_TYPE_ITEMS (sizeof(s_stScrambledType) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t 	s_stChannelType[] = {
	{eDxSVC_TYPE_All,	RES_ALL_ID,			NULL},
	{eDxSVC_TYPE_TV,	RES_TV_ID,		 	NULL},
	{eDxSVC_TYPE_RADIO, RES_RADIO_02_ID,	NULL},
};
#define NX_MENU_CHANNEL_TYPE_ITEMS (sizeof(s_stChannelType) / sizeof(NxMenuItemText_t))


/******************************************************************************/
#define ___Satellite_Item______________________________________________________
/******************************************************************************/
#if defined(CONFIG_MW_CH_SATELLITE)
// ----    Search Channels (Fixed Antenna)	  ---- //
static NxMenuItemText_t		s_stSearchFixedSwitchType[] = {
	{eNxSat_AntType_LNB_Only,		RES_LNB_ONLY_ID,	NULL },
	{eNxSat_AntType_DiSEqC_1_0, 	RES_DISEQC_ID,		NULL },
};
#define	NX_MENU_SEARCHFIXED_SWITCHTYPE_ITEMS		(sizeof(s_stSearchFixedSwitchType) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSearchFixedOption[] = {
	{0,		RES_ANTENNA_TYPE_ID,	NULL },
	{1, 	RES_SEARCH_OPTION_ID,	NULL },
};
#define	NX_MENU_SEARCHFIXED_OPTION_ITEMS		(sizeof(s_stSearchFixedOption) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSearchFixedOptionImageN[] = {
	{0,		NULL,	RES_216_01_OPT_ANTE_N_PNG },
	{1, 	NULL,	RES_216_00_OPT_SEARCH_N_PNG },
};
#define	NX_MENU_SEARCHFIXED_OPTION_IMAGE_N_ITEMS		(sizeof(s_stSearchFixedOptionImageN) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSearchFixedOptionImageC[] = {
	{0,		NULL,	RES_216_01_OPT_ANTE_C_PNG },
	{1, 	NULL,	RES_216_00_OPT_SEARCH_C_PNG },
};
#define	NX_MENU_SEARCHFIXED_OPTION_IMAGE_C_ITEMS		(sizeof(s_stSearchFixedOptionImageC) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stSatSettingtLnbFreqInfo[] = {
	{ NX_COM_CH_UNIVERSAL_LNB_FREQ, 	RES_UNIVERSAL_ID,			NULL },
	{ 9750, 							RES_9750_ID,				NULL },
	{ 10600,				 			RES_10600_ID,				NULL },
	{ 5150,				 				RES_5150_ID,				NULL },
	{ 11475,				 			RES_11475_ID,				NULL },
	{ 0,				 				RES_USER_DEFINE_ID,			NULL },
};
#define	NX_MENU_SEARCH_SATSET_LNBFREQ_ITEMS			(sizeof(s_stSatSettingtLnbFreqInfo) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stSatSettingDiseqcInfo[] = {
	{((eDxDISEQC_VER_1_0 << 8) | eDxDISEQC_INPUT_A),	RES_DISEQC_A_21_ID,	NULL, },
	{((eDxDISEQC_VER_1_0 << 8) | eDxDISEQC_INPUT_B),	RES_DISEQC_B_21_ID,	NULL, },
	{((eDxDISEQC_VER_1_0 << 8) | eDxDISEQC_INPUT_C),	RES_DISEQC_C_21_ID,	NULL, },
	{((eDxDISEQC_VER_1_0 << 8) | eDxDISEQC_INPUT_D),	RES_DISEQC_D_21_ID,	NULL, },
	{((eDxDISEQC_VER_2_0 << 8) | eDxDISEQC_INPUT_A),	RES_DISEQC_A_20_ID,	NULL, },
	{((eDxDISEQC_VER_2_0 << 8) | eDxDISEQC_INPUT_B),	RES_DISEQC_B_20_ID,	NULL, },
	{((eDxDISEQC_VER_2_0 << 8) | eDxDISEQC_INPUT_C),	RES_DISEQC_C_20_ID,	NULL, },
	{((eDxDISEQC_VER_2_0 << 8) | eDxDISEQC_INPUT_D),	RES_DISEQC_D_20_ID,	NULL, },
};
#define	NX_MENU_SEARCH_SATSET_DISEQC_ITEMS			(sizeof(s_stSatSettingDiseqcInfo) / sizeof(NxMenuItemText_t))

// ----    Transponder Setting 	  ---- //
static	NxMenuItemText_t	s_stTpSettingtPolarization[] = {
	{ eDxSAT_POLAR_AUTO, 	RES_AUTO_ID,			NULL },
	{ eDxSAT_POLAR_HOR, 	RES_HORIZONTAL_ID,		NULL },
	{ eDxSAT_POLAR_VER,		RES_VERTICAL_ID,		NULL },
};
#define	NX_MENU_SEARCH_TPSET_POLARIZATION_ITEMS			(sizeof(s_stTpSettingtPolarization) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stTpSettingtSymbolRate[] = {
	{ 22000,				RES_22000_ID,			"22000",		},
	{ 26000,				RES_26000_ID,			"26000",		},
	{ 27500,				RES_27500_ID,			"27500",		},
	{ 0,					RES_USER_DEFINE_ID,		"User Define",	},
};
#define	NX_MENU_SEARCH_TPSET_SYMBOLRATE_ITEMS			(sizeof(s_stTpSettingtSymbolRate) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stTpSettingtTransMission[] = {
	{ eNxPrismSAT_PSK_DVBS, 		RES_DVB_ID,				NULL },
	{ eNxPrismSAT_PSK_DVBS2_QPSK, 	RES_DVB_S2_QPSK_ID,		NULL },
	{ eNxPrismSAT_PSK_DVBS2_8PSK,	RES_DVB_S2_8PSK_ID,		NULL },
};
#define	NX_MENU_SEARCH_TPSET_TRANSMISSION_ITEMS			(sizeof(s_stTpSettingtTransMission) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stTpSettingtFECDVB[] = {
	{ eDxSAT_CODERATE_AUTO, 	RES_AUTO_ID,		NULL },
	{ eDxSAT_CODERATE_1_2, 		RES_1_2_ID,			"1/2" },
	{ eDxSAT_CODERATE_2_3,		RES_2_3_ID,			"2/3" },
	{ eDxSAT_CODERATE_3_4,		RES_3_4_ID,			"3/4" },
	{ eDxSAT_CODERATE_5_6,		RES_5_6_ID,			"5/6" },
	{ eDxSAT_CODERATE_7_8,		RES_7_8_ID, 		"7/8" },
};
#define	NX_MENU_SEARCH_TPSET_FEC_DVB_ITEMS			(sizeof(s_stTpSettingtFECDVB) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stTpSettingtFECDVBS2[] = {
	{ eDxSAT_CODERATE_AUTO, 	RES_AUTO_ID,		NULL },
	{ eDxSAT_CODERATE_1_2, 		RES_1_2_ID,			"1/2" },
	{ eDxSAT_CODERATE_3_5,		RES_3_5_ID,			"3/5" },
	{ eDxSAT_CODERATE_2_3,		RES_2_3_ID,			"2/3" },
	{ eDxSAT_CODERATE_3_4,		RES_3_4_ID,			"3/4" },
	{ eDxSAT_CODERATE_4_5,		RES_4_5_ID, 		"4/5" },
	{ eDxSAT_CODERATE_5_6,		RES_5_6_ID, 		"5/6" },
	{ eDxSAT_CODERATE_8_9,		RES_8_9_ID, 		"8/9" },
	{ eDxSAT_CODERATE_9_10,		RES_9_10_ID, 		"9/10" },
};
#define	NX_MENU_SEARCH_TPSET_FEC_DVBS2_ITEMS			(sizeof(s_stTpSettingtFECDVBS2) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stScdSettingTunerInfo[] = {
	{ 1,				RES_TUNER_1_ID,				NULL },
	{ 2,				RES_TUNER_2_ID,				NULL },
	{ 3,				RES_TUNER_3_ID, 			NULL },
	{ 4,				RES_TUNER_4_ID, 			NULL },
};
#define	NX_MENU_SEARCH_SCDSET_TUNER_ITEMS				(sizeof(s_stScdSettingTunerInfo) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stScdSettingUserBandInfo[] = {
	{ 0,				RES_NONE_01_ID,					NULL },
	{ 1,				RES_USER_BAND_1_ID, 			NULL },
	{ 2,				RES_USER_BAND_2_ID, 			NULL },
	{ 3,				RES_USER_BAND_3_ID, 			NULL },
	{ 4,				RES_USER_BAND_4_ID, 			NULL },
	{ 5,				RES_USER_BAND_5_ID, 			NULL },
	{ 6,				RES_USER_BAND_6_ID, 			NULL },
	{ 7,				RES_USER_BAND_7_ID, 			NULL },
	{ 8,				RES_USER_BAND_8_ID, 			NULL },
};
#define	NX_MENU_SEARCH_SCDSET_USERBAND_ITEMS			(sizeof(s_stScdSettingUserBandInfo) / sizeof(NxMenuItemText_t))
#endif

static	NxMenuItemText_t	s_stScheduleRepeat[] = {
	{ DxRSVREPEAT_ONCE, 	RES_ONCE_ID,		NULL },
	{ DxRSVREPEAT_DAILY, 	RES_DAILY_ID,		NULL },
	{ DxRSVREPEAT_WEEKLY,	RES_WEEKLY_ID,		NULL },
};
#define	NX_MENU_SCHEDULE_REPEAT_ITEMS			(sizeof(s_stScheduleRepeat) / sizeof(NxMenuItemText_t))

static	NxMenuItemText_t	s_stScheduleMode[] = {
	{ eNxSchedle_ModeRecord, 	RES_RECORD_01_ID,		NULL },
	{ eNxSchedle_ModeWatching, 	RES_REMINDER_ID,		NULL },
};
#define	NX_MENU_SCHEDULE_MODE_ITEMS			(sizeof(s_stScheduleMode) / sizeof(NxMenuItemText_t))

#if defined(CONFIG_MW_INET)

static NxMenuItemText_t		s_stSettingNetworkConfigureIP[] = {
	{TRUE,  RES_DHCP_ID,		NULL },
	{FALSE, RES_MANUAL_ID,		NULL },
};
#define	NX_MENU_SETTING_NETWORK_CONFIGURE_IP_ITEMS        (sizeof(s_stSettingNetworkConfigureIP) / sizeof(NxMenuItemText_t))

#endif

#if	defined(CONFIG_MW_MM_PVR)

static NxMenuItemText_t		s_stSettingPVRInstantReplayTime[] = {
	{7,		RES_7_SEC_ID,		NULL },
	{15,	RES_15_SEC_ID,		NULL },
	{30,	RES_30_SEC_ID,		NULL },
};
#define	NX_MENU_SETTING_PVR_INSTANT_REPLAY_TIME        (sizeof(s_stSettingPVRInstantReplayTime) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSettingPVRSkipForwardTime[] = {
	{30,	RES_30_SEC_ID,		NULL },
	{60,	RES_60_SEC_ID,		NULL },
	{120,	RES_120_SEC_ID,		NULL },
	{240,	RES_240_SEC_ID,		NULL },
};
#define	NX_MENU_SETTING_PVR_SKIP_FORWARD_TIME	        (sizeof(s_stSettingPVRSkipForwardTime) / sizeof(NxMenuItemText_t))

static NxMenuItemText_t		s_stSettingPVRPaddingTime[] = {
	{0,		RES_OFF_ID,			NULL },
	{180,	RES_3_MINS_ID,		NULL },
	{300,	RES_5_MINS_ID,		NULL },
	{600,	RES_10_MINS_ID,		NULL },
	{1200,	RES_20_MINS_ID,		NULL },
};
#define	NX_MENU_SETTING_PVR_PADDING_TIME				(sizeof(s_stSettingPVRPaddingTime) / sizeof(NxMenuItemText_t))

#endif

static NxMenuItem_t		s_stTotalMenuItems[NX_MENUITEM_ID_MAX_ITEMS] = {
	{ NX_MENUITEM_ID_NONE,						0,										NULL,							},

	{ NX_MENUITEM_ID_SET_OFF_ON,				NX_MENU_OFF_ON_ITEMS,					s_stOffOnSetting				},
	{ NX_MENUITEM_ID_SET_ON_OFF,				NX_MENU_ON_OFF_ITEMS,					s_stOnOffSetting				},

	{ NX_MENUITEM_ID_SET_CHANGE_PW,				0,										NULL							},

	{ NX_MENUITEM_ID_SET_DAYLIGHT_SAVING_TIME,	NX_MENU_OFF_ON_ITEMS,					s_stOffOnSetting,		},

	{ NX_MENUITEM_ID_SET_ASPECT_RATIO,			NX_MENU_ASPECT_RATIO_ITEMS, 			s_stScreenAspectRatio	},
	{ NX_MENUITEM_ID_SET_4_3_DISPLAY_FORMAT,	NX_MENU_DISPLY_FORMAT_43_ITEMS, 		s_stDisplayFormat_4_3	},
	{ NX_MENUITEM_ID_SET_16_9_DISPLAY_FORMAT,	NX_MENU_DISPLY_FORMAT_169_ITEMS, 		s_stDisplayFormat_16_9	},

	{ NX_MENUITEM_ID_SET_SPDIF,					NX_MENU_AUDIO_SPDIF_ITEMS,				s_stAudio_SPDIF			},
	{ NX_MENUITEM_ID_SET_HDMI,					NX_MENU_AUDIO_HDMI_ITEMS, 				s_stAudio_HDMI			},
	{ NX_MENUITEM_ID_SET_AUDIOTYPE,				NX_MENU_AUDIO_AUDIOTYPE_ITEMS, 			s_stAudio_AudioType		},

    { NX_MENUITEM_ID_SET_INFO_DISPLAY_TIME,     NX_MENU_INFO_DISPLAY_TIME_ITEMS,        s_stInfoDisplayTime     },
    { NX_MENUITEM_ID_SET_SUBTITLE_FONT,         NX_MENU_SUBTITLE_FONT_ITEMS,            s_stSubtitleFont        },
	{ NX_MENUITEM_ID_SET_REPEATOPTION,			NX_MENU_REPEATOPTION_ITEMS,				s_stRepeatOption		},
	{ NX_MENUITEM_ID_SET_TEXTPOSITION,			NX_MENU_TEXTPOSITION_ITEMS,				s_stTextPosition		},
    { NX_MENUITEM_ID_SET_FONTSIZE,				NX_MENU_FONTSIZE_ITEMS,					s_stFontSize			},

	// ----    System    ---- //
	{ NX_MENUITEM_ID_SET_POWERONTIMER,			NX_MENU_POWERONTIMER_ITEMS,				s_stSystemSetPowerOnTimer},
	{ NX_MENUITEM_ID_SET_POWEROFFTIMER,			NX_MENU_POWEROFFTIMER_ITEMS,			s_stSystemSetPowerOffTimer},
	{ NX_MENUITEM_ID_SET_POWERTIMERVOLUME,		NX_MENU_POWERTIMERSETTINGS_VOLUME_ITEMS, s_stPowerTimerSettingsVolume},
	{ NX_MENUITEM_ID_SET_POWERTIMERREPEAT,		NX_MENU_POWERTIMERSETTINGS_REPEAT_ITEMS, s_stPowerTimerSettingsRepeat},

	{ NX_MENUITEM_ID_SEARCH_OPTION_SCRAMBLEDTYPE,	NX_MENU_SCRAMBLED_TYPE_ITEMS, 		s_stScrambledType		},
	{ NX_MENUITEM_ID_SEARCH_OPTION_CHANNELTYPE,		NX_MENU_CHANNEL_TYPE_ITEMS,			s_stChannelType			},

	// ----    Search Channels (Fixed Antenna)    ---- //
#if defined(CONFIG_MW_CH_SATELLITE)
	{ NX_MENUITEM_ID_SEARCH_FIXED_SWITCHTYPE,			NX_MENU_SEARCHFIXED_SWITCHTYPE_ITEMS,		s_stSearchFixedSwitchType,	},
	{ NX_MENUITEM_ID_SEARCH_FIXED_OPTION,				NX_MENU_SEARCHFIXED_OPTION_ITEMS,			s_stSearchFixedOption,		},
	{ NX_MENUITEM_ID_SEARCH_FIXED_OPTION_IMAGE_N,		NX_MENU_SEARCHFIXED_OPTION_IMAGE_N_ITEMS,	s_stSearchFixedOptionImageN,},
	{ NX_MENUITEM_ID_SEARCH_FIXED_OPTION_IMAGE_C,		NX_MENU_SEARCHFIXED_OPTION_IMAGE_C_ITEMS,	s_stSearchFixedOptionImageC,},
	{ NX_MENUITEM_ID_SEARCH_SATSETTING_LNBFREQUENCY,	NX_MENU_SEARCH_SATSET_LNBFREQ_ITEMS,		s_stSatSettingtLnbFreqInfo,	},
	{ NX_MENUITEM_ID_SEARCH_SATSETTING_DISEQC, 			NX_MENU_SEARCH_SATSET_DISEQC_ITEMS,			s_stSatSettingDiseqcInfo,	},

	{ NX_MENUITEM_ID_SEARCH_TPSETTING_POLARIZATION,		NX_MENU_SEARCH_TPSET_POLARIZATION_ITEMS,	s_stTpSettingtPolarization,	},
	{ NX_MENUITEM_ID_SEARCH_TPSETTING_SYMBOLRATE, 		NX_MENU_SEARCH_TPSET_SYMBOLRATE_ITEMS,		s_stTpSettingtSymbolRate,	},
	{ NX_MENUITEM_ID_SEARCH_TPSETTING_TRANSMISSION, 	NX_MENU_SEARCH_TPSET_TRANSMISSION_ITEMS,	s_stTpSettingtTransMission,	},
	{ NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVB, 			NX_MENU_SEARCH_TPSET_FEC_DVB_ITEMS,			s_stTpSettingtFECDVB,		},
	{ NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVBS2,		NX_MENU_SEARCH_TPSET_FEC_DVBS2_ITEMS, 		s_stTpSettingtFECDVBS2,		},

	{ NX_MENUITEM_ID_SEARCH_SCDSETTING_TUNER,			NX_MENU_SEARCH_SCDSET_TUNER_ITEMS, 			s_stScdSettingTunerInfo,	},
	{ NX_MENUITEM_ID_SEARCH_SCDSETTING_USERBAND,		NX_MENU_SEARCH_SCDSET_USERBAND_ITEMS,		s_stScdSettingUserBandInfo,	},
#endif

#if defined(CONFIG_MW_INET)
    { NX_MENUITEM_ID_SET_NETWORK_CONFIGURE_IP,			NX_MENU_SETTING_NETWORK_CONFIGURE_IP_ITEMS,		s_stSettingNetworkConfigureIP,  },
#endif

#if	defined(CONFIG_MW_MM_PVR)
	{ NX_MENUITEM_ID_SET_PVR_INSTANT_REPLAY_TIME,		NX_MENU_SETTING_PVR_INSTANT_REPLAY_TIME,	s_stSettingPVRInstantReplayTime,	},
	{ NX_MENUITEM_ID_SET_PVR_SKIP_FORWARD_TIME,			NX_MENU_SETTING_PVR_SKIP_FORWARD_TIME,		s_stSettingPVRSkipForwardTime,		},
	{ NX_MENUITEM_ID_SET_PVR_PADDING_TIME,				NX_MENU_SETTING_PVR_PADDING_TIME,			s_stSettingPVRPaddingTime,			},
#endif

	// ----    ELSE    ---- //
	{ NX_MENUITEM_ID_SCHEDULE_REPEAT,			NX_MENU_SCHEDULE_REPEAT_ITEMS,			s_stScheduleRepeat		},
	{ NX_MENUITEM_ID_SCHEDULE_MODE, 			NX_MENU_SCHEDULE_MODE_ITEMS, 			s_stScheduleMode 		},
};

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxMenuItem_t*		nx_menuitem_GetMenuItem(Nx_MenuSubItemId_t eMenuItemId);
static NxMenuItemText_t*	nx_menuitem_GetSubItem (NxMenuItem_t *pstItem, HUINT32 ulIndex);
static HUINT8*				nx_menuitem_GetSubItemString (NxMenuItem_t *pstItem, HUINT32 ulIndex);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxMenuItem_t*	nx_menuitem_GetMenuItem(Nx_MenuSubItemId_t eMenuItemId)
{
	HUINT32		 ulIndex = 0;

	for (ulIndex = 0; ulIndex < NX_MENUITEM_ID_MAX_ITEMS; ulIndex++)
	{
		if (s_stTotalMenuItems[ulIndex].eSubitemId == eMenuItemId)
		{
			return &(s_stTotalMenuItems[ulIndex]);
		}
	}

	return NULL;
}


static NxMenuItemText_t*	nx_menuitem_GetSubItem (NxMenuItem_t *pstItem, HUINT32 ulIndex)
{
	if (pstItem == NULL)
	{
		return NULL;
	}

	if (ulIndex >= pstItem->ulItemNum)
	{
		return NULL;
	}

	return &(pstItem->pstItem[ulIndex]);
}


static HUINT8 *nx_menuitem_GetSubItemString (NxMenuItem_t *pstItem, HUINT32 ulIndex)
{
	NxMenuItemText_t	*pstItemText;

	if (pstItem == NULL)					{ return NULL; }
	if (ulIndex >= pstItem->ulItemNum)		{ return NULL; }

	pstItemText = &(pstItem->pstItem[ulIndex]);

	if (pstItemText->ucStrId == NULL)
	{
		return	(HUINT8*)pstItemText->szText;
	}

	return (HUINT8 *)ONDK_GetString (pstItemText->ucStrId);
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_MENU_Menuitem_CountSubitems (Nx_MenuSubItemId_t eMenuItemId, HUINT32 *pulNum)
{
	NxMenuItem_t	*pstItem = NULL;

	if (pulNum == NULL)
	{
		NX_APP_Error("Error!!! (pulNum == NULL) Fail!! \n", eMenuItemId);
		return	ONDK_RET_FAIL;
	}

	pstItem = nx_menuitem_GetMenuItem(eMenuItemId);
	if (pstItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetMenuItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_INITFAIL;
	}

	*pulNum = pstItem->ulItemNum;

	return ONDK_RET_OK;
}

HUINT8 **NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(Nx_MenuSubItemId_t eMenuItemId)
{
	HUINT32			ulIndex = 0, ulArraySize = 0;
	HUINT32			ulTextLen = 0;
	HUINT8		 	*pucSrcText = NULL, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;
	NxMenuItem_t	*pstItem = NULL;


	pstItem = nx_menuitem_GetMenuItem(eMenuItemId);
	if (pstItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetMenuItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	NULL;
	}

	ulArraySize = pstItem->ulItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error("Error!!! (ulArraySize == 0) Fail!! \n", eMenuItemId);
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!! \n");
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		pucSrcText = nx_menuitem_GetSubItemString (pstItem, ulIndex);
		if (pucSrcText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}


void	NX_PRISM_MENU_MenuItem_DeleteTextListFromMenuItemId(Nx_MenuSubItemId_t eMenuItemId, HUINT8 **ppucTextArray)
{
	HUINT32			ulIndex = 0, ulArraySize = 0;
	NxMenuItem_t	*pstItem = NULL;

	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! (ppucTextArray == NULL) - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return;
	}

	pstItem = nx_menuitem_GetMenuItem(eMenuItemId);
	if (pstItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetMenuItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return;
	}

	ulArraySize = pstItem->ulItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error("Error!!! (ulArraySize == 0) - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return;
	}

	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		if (ppucTextArray[ulIndex] != NULL)
		{
			NX_APP_Free(ppucTextArray[ulIndex]);
		}
	}

	if (ppucTextArray != NULL)
	{
		NX_APP_Free(ppucTextArray);
	}
}


HUINT8 **NX_PRISM_MENU_MenuItem_AllocTextList (HUINT32 ulNumItem, HUINT32 ulTextLen)
{
	HUINT32		ulIndex = 0;
	HUINT8		*pucDstText = NULL;
	HUINT8		**ppucTextArray = NULL;

	if (ulNumItem == 0)
	{
		NX_APP_Error("Error!!! (ulNumItem == 0) Fail!! \n");
		return NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc (sizeof(HUINT8 *) * ulNumItem);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!! \n");
		return	NULL;
	}

	HxSTD_MemSet (ppucTextArray, 0x00, sizeof(HUINT8 *) * ulNumItem);

	for (ulIndex = 0; ulIndex < ulNumItem; ulIndex++)
	{
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulNumItem; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}


HERROR	NX_PRISM_MENU_MenuItem_FreeTextList (HUINT32 ulNumItem, HUINT8 **ppucTextArray)
{
	HUINT32		 ulIndex = 0;

	if ((ulNumItem == 0) || (ppucTextArray == NULL))
	{
		NX_APP_Error("Error!!! ((ulNumItem == 0) || (ppucTextArray == NULL)) Fail!! \n");
		return	ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < ulNumItem; ulIndex++)
	{
		if (ppucTextArray[ulIndex] != NULL)
		{
			NX_APP_Free (ppucTextArray[ulIndex]);
		}
	}

	NX_APP_Free (ppucTextArray);

	return	ERR_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetIndexFromValue (Nx_MenuSubItemId_t eMenuItemId, HINT32 lValue, HUINT32 *pulIndex)
{
	HUINT32		 		ulIndex;
	NxMenuItem_t		*pstMenuItem = NULL;
	NxMenuItemText_t	*pstSubItem = NULL;

	if (pulIndex == NULL)
	{
		NX_APP_Error("Error!!! (pulIndex == NULL) - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_INITFAIL;
	}

	pstMenuItem = nx_menuitem_GetMenuItem (eMenuItemId);
	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetMenuItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_FAIL;
	}

	for (ulIndex = 0; ulIndex < pstMenuItem->ulItemNum; ulIndex++)
	{
		pstSubItem = &(pstMenuItem->pstItem[ulIndex]);

		if (pstSubItem->lValue == lValue)
		{
			*pulIndex = ulIndex;
			return	ONDK_RET_OK;
		}
	}

	return	ONDK_RET_FAIL;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetValueFromIndex (Nx_MenuSubItemId_t eMenuItemId, HUINT32 ulIndex, HINT32 *pnValue)
{
	NxMenuItem_t		*pstMenuItem = NULL;
	NxMenuItemText_t	*pstSubItem = NULL;

	if (pnValue == NULL)
	{
		NX_APP_Error("Error!!! (pnValue == NULL) Fail!! \n");
		return	ONDK_RET_INITFAIL;
	}

	pstMenuItem = nx_menuitem_GetMenuItem(eMenuItemId);
	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetMenuItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_FAIL;
	}

	pstSubItem = nx_menuitem_GetSubItem (pstMenuItem, ulIndex);
	if (pstSubItem == NULL)
	{
		NX_APP_Error("Error!!! nx_menuitem_GetSubItem() - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_FAIL;
	}

	*pnValue = pstSubItem->lValue;

	return	ONDK_RET_OK;
}


ONDK_Result_t		NX_PRISM_MENU_Menuitem_CountExpandSubitems(Nx_MenuSubItemId_t eMenuItemId, HUINT32 *pulNum)
{
	HINT32			lTotalCnt = 0;
	HxVector_t		*pstItemList = NULL;


	if (pulNum == NULL)
	{
		NX_APP_Error("Error!!! (pulNum == NULL) Fail!! \n", eMenuItemId);
		return	ONDK_RET_FAIL;
	}

	*pulNum = 0;

	switch (eMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			if (s_stMenuLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stMenuLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stMenuLanguageList;
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			if (s_stAudioLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stAudioLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stAudioLanguageList;
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			if (s_stSubtitleLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stSubtitleLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stSubtitleLanguageList;
			break;

		case NX_MENUITEM_ID_SET_RESOLUTION:
			if (s_stResolutionList == NULL)
			{
				NX_APP_Error("Error!!! (s_stResolutionList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stResolutionList;
			break;

		case NX_MENUITEM_ID_SET_COUNTRYSETTING:
			if (s_stCountryList == NULL)
			{
				NX_APP_Error("Error!!! (s_stCountryList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stCountryList;
			break;

		case NX_MENUITEM_ID_SET_MATURITY_RATING:
			if (s_stParentalContrlList == NULL)
			{
				NX_APP_Error("Error!!! (s_stParentalContrlList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stParentalContrlList;
			break;

		default:
			NX_APP_Error("Error!!! Only Support Language Item :(0x%x) Fail!! \n", eMenuItemId);
			return	ONDK_RET_INITFAIL;
	}

	lTotalCnt = HLIB_VECTOR_Length(pstItemList);
	if (lTotalCnt < 1)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_Length(s_stMenuLanguageList) : (%d) Fail!! \n", lTotalCnt);
		return	ONDK_RET_INITFAIL;
	}

	*pulNum = (HUINT32)lTotalCnt;

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue (Nx_MenuSubItemId_t eMenuItemId, HINT32 lValue, HUINT32 *pulIndex)
{
	HINT32		 		lIndex = 0, lTotalCnt = 0;
	HxVector_t			*pstItemList = NULL;

	if (pulIndex == NULL)
	{
		NX_APP_Error("Error!!! (pulIndex == NULL) - eMenuItemId:(0x%x) Fail!! \n", eMenuItemId);
		return	ONDK_RET_INITFAIL;
	}

	*pulIndex = 0;

	switch (eMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			if (s_stMenuLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stMenuLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stMenuLanguageList;
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			if (s_stAudioLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stAudioLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stAudioLanguageList;
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			if (s_stSubtitleLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stSubtitleLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stSubtitleLanguageList;
			break;

		case NX_MENUITEM_ID_SET_RESOLUTION:
			if (s_stResolutionList == NULL)
			{
				NX_APP_Error("Error!!! (s_stResolutionList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stResolutionList;
			break;

		case NX_MENUITEM_ID_SET_COUNTRYSETTING:
			if (s_stCountryList == NULL)
			{
				NX_APP_Error("Error!!! (s_stCountryList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stCountryList;
			break;

		case NX_MENUITEM_ID_SET_MATURITY_RATING:
			if (s_stParentalContrlList == NULL)
			{
				NX_APP_Error("Error!!! (s_stParentalContrlList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stParentalContrlList;
			break;

		default:
			NX_APP_Error("Error!!! Only Support Language Item :(0x%x) Fail!! \n", eMenuItemId);
			return	ONDK_RET_INITFAIL;
	}

	lTotalCnt = HLIB_VECTOR_Length(pstItemList);
	if (lTotalCnt < 1)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_Length(s_stMenuLanguageList) : (%d) Fail!! \n", lTotalCnt);
		return	ONDK_RET_INITFAIL;
	}

	for (lIndex = 0; lIndex < lTotalCnt; lIndex++)
	{
		NxMenuItemText_t *pstItemText = (NxMenuItemText_t*)HLIB_VECTOR_ItemAt(pstItemList, lIndex);

		if (pstItemText == NULL)
		{
			NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() - lIndex(%d) Fail!! \n", lIndex);
			continue;
		}

		if (pstItemText->lValue == lValue)
		{
			*pulIndex = (HUINT32)lIndex;
			return	ONDK_RET_OK;
		}
	}

	return	ONDK_RET_FAIL;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex (Nx_MenuSubItemId_t eMenuItemId, HUINT32 ulIndex, HINT32 *pnValue)
{
	HINT32				lTotalCnt = 0;
	NxMenuItemText_t	*pstItemText = NULL;
	HxVector_t			*pstItemList = NULL;


	if (pnValue == NULL)
	{
		NX_APP_Error("Error!!! (pnValue == NULL) Fail!! \n");
		return	ONDK_RET_INITFAIL;
	}

	switch (eMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			if (s_stMenuLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stMenuLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stMenuLanguageList;
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			if (s_stAudioLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stAudioLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stAudioLanguageList;
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			if (s_stSubtitleLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stSubtitleLanguageList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stSubtitleLanguageList;
			break;

		case NX_MENUITEM_ID_SET_RESOLUTION:
			if (s_stResolutionList == NULL)
			{
				NX_APP_Error("Error!!! (s_stResolutionList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stResolutionList;
			break;

		case NX_MENUITEM_ID_SET_COUNTRYSETTING:
			if (s_stCountryList == NULL)
			{
				NX_APP_Error("Error!!! (s_stCountryList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stCountryList;
			break;

		case NX_MENUITEM_ID_SET_MATURITY_RATING:
			if (s_stParentalContrlList == NULL)
			{
				NX_APP_Error("Error!!! (s_stParentalContrlList == NULL) Fail!! \n");
				return	ONDK_RET_INITFAIL;
			}
			pstItemList = s_stParentalContrlList;
			break;

		default:
			NX_APP_Error("Error!!! Only Support Language Item :(0x%x) Fail!! \n", eMenuItemId);
			return	ONDK_RET_INITFAIL;
	}

	lTotalCnt = HLIB_VECTOR_Length(pstItemList);
	if (lTotalCnt < 1)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_Length(s_stMenuLanguageList) : (%d) Fail!! \n", lTotalCnt);
		return	ONDK_RET_INITFAIL;
	}

	pstItemText = (NxMenuItemText_t*)HLIB_VECTOR_ItemAt(pstItemList, ulIndex);
	if (pstItemText == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() - ulIndex(%d) Fail!! \n", ulIndex);
		return	ONDK_RET_INITFAIL;
	}

	*pnValue = pstItemText->lValue;

	return	ONDK_RET_OK;
}


HUINT8 **NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId(Nx_MenuSubItemId_t eMenuItemId, HBOOL bShowNumber)
{
	HINT32			lTotalCnt = 0, lIndex = 0;
	HUINT32			ulTextLen = 0;
	HUINT8		 	*pucSrcText = NULL, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;
	HxVector_t		*pstItemList = NULL;


	switch (eMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			if (s_stMenuLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stMenuLanguageList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stMenuLanguageList;
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			if (s_stAudioLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stAudioLanguageList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stAudioLanguageList;
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			if (s_stSubtitleLanguageList == NULL)
			{
				NX_APP_Error("Error!!! (s_stSubtitleLanguageList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stSubtitleLanguageList;
			break;

		case NX_MENUITEM_ID_SET_RESOLUTION:
			if (s_stResolutionList == NULL)
			{
				NX_APP_Error("Error!!! (s_stResolutionList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stResolutionList;
			break;

		case NX_MENUITEM_ID_SET_COUNTRYSETTING:
			if (s_stCountryList == NULL)
			{
				NX_APP_Error("Error!!! (s_stCountryList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stCountryList;
			break;

		case NX_MENUITEM_ID_SET_MATURITY_RATING:
			if (s_stParentalContrlList == NULL)
			{
				NX_APP_Error("Error!!! (s_stParentalContrlList == NULL) Fail!! \n");
				return	NULL;
			}
			pstItemList = s_stParentalContrlList;
			break;

		default:
			NX_APP_Error("Error!!! Only Support Language Item :(0x%x) Fail!! \n", eMenuItemId);
			return	NULL;
	}

	lTotalCnt = HLIB_VECTOR_Length(pstItemList);
	if (lTotalCnt < 1)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_Length(s_stMenuLanguageList) : (%d) Fail!! \n", lTotalCnt);
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * lTotalCnt);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!! \n");
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * lTotalCnt);
	for (lIndex = 0; lIndex < lTotalCnt; lIndex++)
	{
		NxMenuItemText_t *pstItemText = (NxMenuItemText_t*)HLIB_VECTOR_ItemAt(pstItemList, lIndex);

		if (pstItemText == NULL)
		{
			NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() - lIndex(%d) Fail!! \n", lIndex);
			continue;
		}

		if (pstItemText->ucStrId == NULL)
		{
			pucSrcText = (HUINT8*)pstItemText->szText;
		}
		else
		{
			pucSrcText = (HUINT8 *)ONDK_GetString (pstItemText->ucStrId);
		}

		// Source Text
		if (pucSrcText == NULL)
		{
			for (lIndex = 0; lIndex < lTotalCnt; lIndex++)
			{
				if (ppucTextArray[lIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[lIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);
		if (bShowNumber == TRUE)
		{
			ulTextLen += NX_PRISM_UI_MENU_ITEMS_SHOW_NUMBER_LEN;
		}

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (lIndex = 0; lIndex < lTotalCnt; lIndex++)
			{
				if (ppucTextArray[lIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[lIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		if (bShowNumber == TRUE)
		{
			HxSTD_snprintf((char*)pucDstText, ulTextLen, "%03d %s", (lIndex + 1), pucSrcText);
		}
		else
		{
			HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		}
		ppucTextArray[lIndex] = pucDstText;
	}

	return	ppucTextArray;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_MoveLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId, HINT32 lIndex)
{
	HINT8				*pszStrId = NULL;
	HINT32				lTotalCnt = 0, i = 0;
	NxMenuItemText_t	*pstItemText = NULL;
	HxVector_t 			*pstItemList = NULL;
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	pszStrId = NX_PRISM_UTIL_GetResStringIdByLangId(eLangId);
	if (pszStrId == NULL)
	{
		if (pstMenuItem != NULL)
		{
			NX_APP_Free(pstMenuItem);
			pstMenuItem = NULL;
		}
		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetResStringIdByLangId() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	switch (ulMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:		pstItemList = s_stMenuLanguageList;			break;
		case NX_MENUITEM_ID_SET_AUDIO_LANG:			pstItemList = s_stAudioLanguageList;		break;
		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:		pstItemList = s_stSubtitleLanguageList;		break;
		default:
			NX_APP_Error("Error!!! Invlaid Item ID : (0x%x)\n", ulMenuItemId);
            if (NULL != pstMenuItem)
            {
                NX_APP_Free(pstMenuItem);
                pstMenuItem = NULL;
            }
			break;
	}

	// Remove Item
	lTotalCnt = HLIB_VECTOR_Length(pstItemList);
	if (lTotalCnt < 1)
	{
		lTotalCnt = 0;
	}

	for (i = 0; i < lTotalCnt; i++)
	{
		pstItemText = (NxMenuItemText_t*)HLIB_VECTOR_ItemAt(pstItemList, i);
		if (pstItemText == NULL)
		{
			NX_APP_Error("Error!!! HLIB_VECTOR_ItemAt() - (ulMenuItemId:0x%x) - i(%u) Fail!! \n", ulMenuItemId, i);
			continue;
		}

		if ((HxLANG_Id_e)pstItemText->lValue == eLangId)
		{
			HLIB_VECTOR_Remove(pstItemList, i);
			break;
		}
	}

	// Add Item
	pstMenuItem->lValue = eLangId;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	HLIB_VECTOR_Insert(pstItemList, lIndex, (void*)pstMenuItem);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_InsertLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId, HINT32 lIndex)
{
	HINT8				*pszStrId = NULL;
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	pszStrId = NX_PRISM_UTIL_GetResStringIdByLangId(eLangId);
	if (pszStrId == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetResStringIdByLangId() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	pstMenuItem->lValue = eLangId;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	switch (ulMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			HLIB_VECTOR_Insert(s_stMenuLanguageList, lIndex, (void*)pstMenuItem);
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			HLIB_VECTOR_Insert(s_stAudioLanguageList, lIndex, (void*)pstMenuItem);
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			HLIB_VECTOR_Insert(s_stSubtitleLanguageList, lIndex, (void*)pstMenuItem);
			break;

		default:
			NX_APP_Error("Error!!! Invlaid Item ID : (0x%x)\n", ulMenuItemId);
            if(NULL != pstMenuItem)
            {
                NX_APP_Free(pstMenuItem);
                pstMenuItem = NULL;
            }

			break;
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId)
{
	HINT8	*pszStrId = NULL;
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	pszStrId = NX_PRISM_UTIL_GetResStringIdByLangId(eLangId);
	if (pszStrId == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetResStringIdByLangId() - eLangId:(0x%x) Fail!!", eLangId);
		return	ONDK_RET_FAIL;
	}

	pstMenuItem->lValue = eLangId;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	switch (ulMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			HLIB_VECTOR_Add(s_stMenuLanguageList, (void*)pstMenuItem);
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			HLIB_VECTOR_Add(s_stAudioLanguageList, (void*)pstMenuItem);
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			HLIB_VECTOR_Add(s_stSubtitleLanguageList, (void*)pstMenuItem);
			break;

		default:
			NX_APP_Error("Error!!! Invlaid Item ID : (0x%x)\n", ulMenuItemId);
            if(NULL != pstMenuItem)
            {
                NX_APP_Free(pstMenuItem);
                pstMenuItem = NULL;
            }
			break;
	}

	return	ONDK_RET_OK;
}


void	NX_PRISM_MENU_MenuItem_SetDefaultLanguageList(HUINT32 ulMenuItemId)
{
#if defined(CONFIG_PROD_LANG_ARABIC)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Arabic);
#endif
#if defined(CONFIG_PROD_LANG_BULGARIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Bulgarian);
#endif
#if defined(CONFIG_PROD_LANG_CATALAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Catalan);
#endif
#if defined(CONFIG_PROD_LANG_CROATIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Croatian);
#endif
#if defined(CONFIG_PROD_LANG_CZECH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Czech);
#endif
#if defined(CONFIG_PROD_LANG_DANISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Danish);
#endif
#if defined(CONFIG_PROD_LANG_DUTCH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Dutch);
#endif
#if defined(CONFIG_PROD_LANG_ENGLISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_English);
#endif
#if defined(CONFIG_PROD_LANG_FINNISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Finnish);
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_French);
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_German);
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Greek);
#endif
#if defined(CONFIG_PROD_LANG_HUNGARIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Hungarian);
#endif
#if defined(CONFIG_PROD_LANG_HINDI)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Hindi);
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Italian);
#endif
#if defined(CONFIG_PROD_LANG_NORWEGIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Norwegian);
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Persian);
#endif
#if defined(CONFIG_PROD_LANG_POLISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Polish);
#endif
#if defined(CONFIG_PROD_LANG_PORTUGUESE)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Portuguese);
#endif
#if defined(CONFIG_PROD_LANG_ROMANIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Romanian);
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Russian);
#endif
#if defined(CONFIG_PROD_LANG_SERBIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Serbian);
#endif
#if defined(CONFIG_PROD_LANG_SLOVAK)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Slovak);
#endif
#if defined(CONFIG_PROD_LANG_SLOVENIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Slovenian);
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Spanish);
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Swedish);
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Turkish);
#endif
#if defined(CONFIG_PROD_LANG_UKRAINIAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Ukrainian);
#endif
#if defined(CONFIG_PROD_LANG_THAI)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Thai);
#endif
#if defined(CONFIG_PROD_LANG_JAPAN)
	NX_PRISM_MENU_MenuItem_AddLanguageList(ulMenuItemId, eLangID_Japanese);
#endif
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadSystemLanguageList(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stMenuLanguageList == NULL)
	{
		s_stMenuLanguageList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stMenuLanguageList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stMenuLanguageList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stMenuLanguageList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stMenuLanguageList);
	}

	if (bSetDefault == TRUE)
	{
		NX_PRISM_MENU_MenuItem_SetDefaultLanguageList(NX_MENUITEM_ID_SET_SYSTEM_LANG);
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadAudioLanguageList(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stAudioLanguageList == NULL)
	{
		s_stAudioLanguageList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stAudioLanguageList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stAudioLanguageList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stAudioLanguageList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stAudioLanguageList);
	}

	if (bSetDefault == TRUE)
	{
		NX_PRISM_MENU_MenuItem_SetDefaultLanguageList(NX_MENUITEM_ID_SET_AUDIO_LANG);
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadSubtitleLanguageList(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stSubtitleLanguageList == NULL)
	{
		s_stSubtitleLanguageList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stSubtitleLanguageList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stSubtitleLanguageList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stSubtitleLanguageList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stSubtitleLanguageList);
	}

	if (bSetDefault == TRUE)
	{
		NX_PRISM_MENU_MenuItem_SetDefaultLanguageList(NX_MENUITEM_ID_SET_SUBTITLE_LANG);
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadLanguageList(HUINT32 ulMenuItemId, HBOOL bSetDefault)
{
	switch (ulMenuItemId)
	{
		case NX_MENUITEM_ID_SET_SYSTEM_LANG:
			NX_PRISM_MENU_MenuItem_LoadSystemLanguageList(bSetDefault);
			break;

		case NX_MENUITEM_ID_SET_AUDIO_LANG:
			NX_PRISM_MENU_MenuItem_LoadAudioLanguageList(bSetDefault);
			break;

		case NX_MENUITEM_ID_SET_SUBTITLE_LANG:
			NX_PRISM_MENU_MenuItem_LoadSubtitleLanguageList(bSetDefault);
			break;

		default:
			NX_APP_Error("Error!!! Invlaid Item ID : (0x%x)\n", ulMenuItemId);
			break;
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddResolutionList(DxResolutionSelect_e eResolution)
{
	HINT8	*pszStrId = NULL;
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eResolution:(0x%x) Fail!!", eResolution);
		return	ONDK_RET_FAIL;
	}

	if (s_stResolutionList == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! s_stResolutionList == NULL Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	pszStrId = NX_PRISM_UTIL_GetResStringIdByResolution(eResolution);
	if (pszStrId == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetResStringIdByResolution() - eResolution:(0x%x) Fail!!", eResolution);
		return	ONDK_RET_FAIL;
	}

	pstMenuItem->lValue = eResolution;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	HLIB_VECTOR_Add(s_stResolutionList, (void*)pstMenuItem);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadResolutionList(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stResolutionList == NULL)
	{
		s_stResolutionList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stResolutionList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stResolutionList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stResolutionList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stResolutionList);
	}

	if (bSetDefault == TRUE)
	{
		NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_576I);
		NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_576P);
		NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_720P);
		NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_1080I);
		NX_PRISM_MENU_MenuItem_AddResolutionList(eDxRESOLUTION_SELECT_1080P);
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddCountryList(HxCountry_e eCountryIdx)
{
	HINT8	*pszStrId = NULL;
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eCountryIdx:(0x%x) Fail!!", eCountryIdx);
		return	ONDK_RET_FAIL;
	}

	if (s_stCountryList == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! s_stResolutionList == NULL Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	pszStrId = NX_PRISM_UTIL_GetResStringIdByCountryID(eCountryIdx);
	if (pszStrId == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! NX_PRISM_UTIL_GetResStringIdByResolution() - eCountryIdx:(0x%x) Fail!!", eCountryIdx);
		return	ONDK_RET_FAIL;
	}

	pstMenuItem->lValue = eCountryIdx;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	HLIB_VECTOR_Add(s_stCountryList, (void*)pstMenuItem);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadCountryList(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stCountryList == NULL)
	{
		s_stCountryList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stCountryList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stCountryList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stCountryList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stCountryList);
	}

	if (bSetDefault == TRUE)
	{
#if defined(CONFIG_PROD_COUNTRY_AUSTRALIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_AUS);
#endif
#if defined(CONFIG_PROD_COUNTRY_AUSTRIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_AUT);
#endif
#if defined(CONFIG_PROD_COUNTRY_BULGARIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_BGR);
#endif
#if defined(CONFIG_PROD_COUNTRY_CAMEROON)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_CMR);
#endif
#if defined(CONFIG_PROD_COUNTRY_CROATIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_HRV);
#endif
#if defined(CONFIG_PROD_COUNTRY_CZECH)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_CZE);
#endif
#if defined(CONFIG_PROD_COUNTRY_DENMARK)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_DNK);
#endif
#if defined(CONFIG_PROD_COUNTRY_ENGLAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_GBR);
#endif
#if defined(CONFIG_PROD_COUNTRY_ESTONIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_EST);
#endif
#if defined(CONFIG_PROD_COUNTRY_FINLAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_FIN);
#endif
#if defined(CONFIG_PROD_COUNTRY_FRANCE)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_FRA);
#endif
#if defined(CONFIG_PROD_COUNTRY_GERMANY)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_DEU);
#endif
#if defined(CONFIG_PROD_COUNTRY_GREECE)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_GRC);
#endif
#if defined(CONFIG_PROD_COUNTRY_HUNGARY)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_HUN);
#endif
#if defined(CONFIG_PROD_COUNTRY_IRELAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_IRL);
#endif
#if defined(CONFIG_PROD_COUNTRY_ITALY)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_ITA);
#endif
#if defined(CONFIG_PROD_COUNTRY_LATVIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_LVA);
#endif
#if defined(CONFIG_PROD_COUNTRY_LITHUANIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_LTU);
#endif
#if defined(CONFIG_PROD_COUNTRY_NETHERLANDS)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_NLD);
#endif
#if defined(CONFIG_PROD_COUNTRY_NORWAY)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_NOR);
#endif
#if defined(CONFIG_PROD_COUNTRY_POLAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_POL);
#endif
#if defined(CONFIG_PROD_COUNTRY_PORTUGAL)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_PRT);
#endif
#if defined(CONFIG_PROD_COUNTRY_RUSSIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_RUS);
#endif
#if defined(CONFIG_PROD_COUNTRY_SERBIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SRB);
#endif
#if defined(CONFIG_PROD_COUNTRY_SINGAPORE)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SGP);
#endif
#if defined(CONFIG_PROD_COUNTRY_SLOVAKIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SVK);
#endif
#if defined(CONFIG_PROD_COUNTRY_SLOVENIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SVN);
#endif
#if defined(CONFIG_PROD_COUNTRY_SPAIN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_ESP);
#endif
#if defined(CONFIG_PROD_COUNTRY_SWEDEN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SWE);
#endif
#if defined(CONFIG_PROD_COUNTRY_TURKEY)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_TUR);
#endif
#if defined(CONFIG_PROD_COUNTRY_UKRAINE)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_UKR);
#endif
#if defined(CONFIG_PROD_COUNTRY_USA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_USA);
#endif
#if defined(CONFIG_PROD_COUNTRY_VIETNAM)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_VNM);
#endif
#if defined(CONFIG_PROD_COUNTRY_BELGIUM)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_BEL);
#endif
#if defined(CONFIG_PROD_COUNTRY_SWITZERLAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_CHE);
#endif
#if defined(CONFIG_PROD_COUNTRY_LUXEMBOURG)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_LUX);
#endif
#if defined(CONFIG_PROD_COUNTRY_ROMANIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_ROU);
#endif
#if defined(CONFIG_PROD_COUNTRY_KAZAKHSTAN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_KAZ);
#endif
#if defined(CONFIG_PROD_COUNTRY_THAILAND)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_THA);
#endif
#if defined(CONFIG_PROD_COUNTRY_UAE)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_ARE);
#endif
#if defined(CONFIG_PROD_COUNTRY_BAHRAIN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_BHR);
#endif
#if defined(CONFIG_PROD_COUNTRY_EGYPT)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_EGY);
#endif
#if defined(CONFIG_PROD_COUNTRY_IRAN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_IRN);
#endif
#if defined(CONFIG_PROD_COUNTRY_IRAQ)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_IRQ);
#endif
#if defined(CONFIG_PROD_COUNTRY_KUWAIT)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_KWT);
#endif
#if defined(CONFIG_PROD_COUNTRY_OMAN)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_OMN);
#endif
#if defined(CONFIG_PROD_COUNTRY_QATAR)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_QAT);
#endif
#if defined(CONFIG_PROD_COUNTRY_SAUDIARABIA)
		NX_PRISM_MENU_MenuItem_AddCountryList(eCountryID_SAU);
#endif
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddParentalContrlList(DxRatingAge_e eRatingAgeIdx, HINT8 *pszStrId)
{
	NxMenuItemText_t	*pstMenuItem = (NxMenuItemText_t*)NX_APP_Calloc(sizeof(NxMenuItemText_t));


	if (pstMenuItem == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() - eRatingAgeIdx:(0x%x) Fail!!", eRatingAgeIdx);
		return	ONDK_RET_FAIL;
	}

	if (s_stParentalContrlList == NULL)
	{
        if (pstMenuItem != NULL)
        {
            NX_APP_Free(pstMenuItem);
            pstMenuItem = NULL;
        }

		NX_APP_Error("Error!!! s_stResolutionList == NULL Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	pstMenuItem->lValue = eRatingAgeIdx;
	pstMenuItem->ucStrId = pszStrId;
	pstMenuItem->szText = NULL;

	HLIB_VECTOR_Add(s_stParentalContrlList, (void*)pstMenuItem);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadParentalContrl(HBOOL bSetDefault)
{
	HINT32		listLen = 0;

	if (s_stParentalContrlList == NULL)
	{
		s_stParentalContrlList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
									(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	}

	if (s_stParentalContrlList == NULL)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_NewEx() - s_stParentalContrlList Fail!! \n");
		return	ONDK_RET_FAIL;
	}

	listLen = HLIB_VECTOR_Length(s_stParentalContrlList);
	if (listLen > 0)
	{
		HLIB_VECTOR_RemoveAll(s_stParentalContrlList);
	}

	if (bSetDefault == TRUE)
	{
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_VIEW_ALL, RES_VIEW_ALL_ID);
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_7, RES_7_ID);
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_12, RES_12_ID);
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_15, RES_15_ID);
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_18, RES_18_ID);
		NX_PRISM_MENU_MenuItem_AddParentalContrlList(eDxRATING_AGE_LOCK_ALL, RES_LOCK_ALL_ID);
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
