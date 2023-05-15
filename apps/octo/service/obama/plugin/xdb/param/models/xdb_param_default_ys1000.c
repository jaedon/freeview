/**
	@file     db_param_default_ys1000.c
	@brief    YS1000 project default data

	Description: default db 생성을 위한 default value 모음 (for CONFIG_PROD_YS1000)
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>

#include <octo_common.h>

#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <_db_main.h>
#include <_db_param_default.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC const dbParam_DefaultItem_t	s_astHardCodingSystemItemList_YS1000[] =
{
	{ eDB_PARAM_ITEM_OPERATOR_TYPE,						{ eDxOPERATOR_Tdc,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_MAIN,							{ eLangID_English,			"", 0, 0 } },
//	{ eDB_PARAM_ITEM_LANG_EVENT,						{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO0,						{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO1,						{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL, 			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT,				{ eLangID_Danish,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE0,					{ eLangID_Danish,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE1, 					{ eLangID_English, 			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL,			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT,				{ eLangID_Danish,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_MATURITYRATING,					{ eDxRATING_VIEW_ALL,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_HARDOFHEARING,						{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUDIO_DESCRIPTION,					{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_STEREO_SELECT,						{ 0,						DxSTR_STEREO_SELECT_STEREO, 0, 0 } },
	{ eDB_PARAM_ITEM_DUALMONO_SELECT,					{ 0,						DxSTR_DUALMONO_SELECT_BOTH, 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT,					{ 0,DxSTR_DIGITALAUDIO_FORMAT_PCM, 0, 0 } },
	{ eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT,				{ 0,DxSTR_DIGITALAUDIO_FORMAT_PCM, 0, 0 } },
	{ eDB_PARAM_ITEM_TVSCART_FORMAT, 				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_VCRSCART_FORMAT,				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_SCART_PATH,					{ 0,						DxSTR_SCART_PATH_STB2TV, 0, 0 } },
	{ eDB_PARAM_ITEM_TV_ASPECTRATIO,				{ 0,						DxSTR_ASPECTRATIO_16X9, 0, 0 } },
	{ eDB_PARAM_ITEM_DFC_SELECT,					{ 0,						DxSTR_DFC_SELECT_DEFAULT, 0, 0 } },
	{ eDB_PARAM_ITEM_RESOLUTION_SELECT,				{ 0,						DxSTR_RESOLUTION_SELECT_1080I, 0, 0 } },
	{ eDB_PARAM_ITEM_3D_SELECT,						{ 0,						DxSTR_HDMI_3DSELECT_OFF, 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_R,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_G,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_B,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_SKIPFORWARDSEC,					{ 30,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_INSTANTREPLAYSEC,					{ 15,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_COUNTRY_ID,						{ eCountryID_DNK,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_PADDINGTIME_START, 				{ 180,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_PADDINGTIME_STOP,					{ 180,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_TSR_ONOFF,							{ TRUE,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUTO_DELETE,						{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_2ND_INT_HDD_UUID,					{ FALSE,					DbParam_UNDEFINED, 0, 0 } },
	{ eDB_PARAM_ITEM_INVISIBLE_SEARCH, 					{ FALSE,					"", 0, 0 } },

	{ eDB_PARAM_ITEM_ANT_CONN_TYPE,						{ eDxANTCONN_MAX,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_PINCODE,							{ 0,						"0000", 0, 0 } },
	{ eDB_PARAM_ITEM_CHANNEL_SETUP_PIN,					{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_GUIDANCE_POLICY, 					{ eDxGUIDANCEPOLICY_OFF,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_SUBTITLE_SETTING,					{ TRUE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_CEC_ENABLE, 					{ TRUE, 					"", 0, 0 } },
	{ eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, 			{ TRUE,						"", 0, 0 } },


	{ eDB_PARAM_ITEM_CUR_SVC,							{ HANDLE_NULL,				"", 0, 0 } },
	{ eDB_PARAM_ITEM_LAST_VOLUME,						{ 50,						{0, }, 0, NULL } },
	{ eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN,				{ 14400000,					"", 0, 0 } },	//	4hours (60 * 60 * 4 * 1000)
	{ eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT,				{ TRUE,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_PVR_DEFAULT_HDD_UUID,				{ 0,						DbParam_UNDEFINED, 0, 0} },

	{ eDB_PARAM_ITEM_GMT_OFFSET_MINUTES,				{ 60,						"", 0, 0 } },
	/* power management */
#if defined(CONFIG_MW_EPG_TVTV)
	{ eDB_PARAM_ITEM_TVTV_ENABLE, 						{ TRUE,						"", 0, 0 } }, // enable
	{ eDB_PARAM_ITEM_TVTV_DOWNTYPE,						{ 0,						"", 0, 0 } }, // at active standby
#endif

	/* edit channel list */

	/* installation */
	{ eDB_PARAM_ITEM_ANTSET_SAT_TUNER01,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_FREQUENCY,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_POLARIZATION, 				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_FEC,						{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_TRANSSPEC,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_MODULATION,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_PILOT,						{ 0,						"", 0, 0 } },
//	no one get value from SEARCH_ACN
	{ eDB_PARAM_ITEM_ANTSET_ANTENNATYPE,				{ eUiAntType_LNB_Only,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_SAT_NETWORK_SRCH,					{ 0,						"", 0, 0 } },


	/* SCD Setting */
	/* nothing */

	/* DisEqc1.3 MyPosition */
	{ eDB_PARAM_ITEM_MY_LONGITUDE,						{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MY_LATITUDE, 						{ 0,						"", 0, 0 } },

	/* Time Settings */
	{ eDB_PARAM_ITEM_DAYLIGHT_SAVING,					{ FALSE,					"", 0, 0 } },	/* on/off */

	/* Virtual Secure CW mode */
	{ eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING,			{ FALSE,					"", 0, 0 } },	/* on/off */

	/* Update time on specific TP */
	{ eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP,		{ FALSE,					"", 0, 0 } },	/* Fixed: FALSE  ,  Variable : TRUE   */
	{ eDB_PARAM_ITEM_LAST_MIC_VOLUME,					{ 100,						{0, }, 0, NULL } },
};

HINT32 xdb_default_GetSystemConfigItemNum_YS1000(void)
{
	return sizeof(s_astHardCodingSystemItemList_YS1000) / sizeof(dbParam_DefaultItem_t);
}

dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_YS1000(HUINT32 ulItemIdx)
{
	if (xdb_default_GetSystemConfigItemNum_YS1000() <= ulItemIdx)
		return NULL;

	return (dbParam_DefaultItem_t*)&s_astHardCodingSystemItemList_YS1000[ulItemIdx];
}

/*********************** End of File ******************************/

