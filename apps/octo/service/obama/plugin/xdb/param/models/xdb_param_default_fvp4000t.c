/**
	@file     db_param_default_hms1000t.c
	@brief    hms-1000t project default data

	Description: default db 생성을 위한 default value 모음 (for CONFIG_PROD_HMS1000T)
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

STATIC HUINT8 dbDefaultBuf = {0,};

STATIC const dbParam_DefaultItem_t	s_astHardCodingSystemItemList_Fvp4000t[] =
{
	{ eDB_PARAM_ITEM_OPERATOR_TYPE,					{ eDxOPERATOR_BBC,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_MAIN,						{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_EVENT,					{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO0,					{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO1,					{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL, 			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT,				{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE0,				{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE1, 				{ eLangID_English, 			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL,			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT,				{ eLangID_English,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_MATURITYRATING,				{ eDxRATING_VIEW_ALL,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_HARDOFHEARING,					{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUDIO_DESCRIPTION,				{ FALSE,					"", 0, 0 } },

	{ eDB_PARAM_ITEM_STEREO_SELECT, 				{ 0,						DxSTR_STEREO_SELECT_STEREO, 0, 0 } },
	{ eDB_PARAM_ITEM_DUALMONO_SELECT,				{ 0,						DxSTR_DUALMONO_SELECT_BOTH, 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT,				{ 0,						DxSTR_DIGITALAUDIO_FORMAT_PCM, 0, 0 } },
	{ eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT,			{ 0,						DxSTR_DIGITALAUDIO_FORMAT_MULTICHANNEL, 0, 0 } },

	{ eDB_PARAM_ITEM_TVSCART_FORMAT, 				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_VCRSCART_FORMAT,				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_SCART_PATH,					{ 0,						DxSTR_SCART_PATH_STB2TV, 0, 0 } },
	{ eDB_PARAM_ITEM_TV_ASPECTRATIO,				{ 0,						DxSTR_ASPECTRATIO_16X9, 0, 0 } },
	{ eDB_PARAM_ITEM_DFC_SELECT,					{ 0,						DxSTR_DFC_SELECT_DEFAULT, 0, 0 } },
	{ eDB_PARAM_ITEM_RESOLUTION_SELECT,				{ 0,						DxSTR_RESOLUTION_SELECT_1080I, 0, 0 } },
	{ eDB_PARAM_ITEM_3D_SELECT,						{ 0,						DxSTR_HDMI_3DSELECT_OFF, 0, 0 } }, // 26
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_R,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_G,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_B,			{ 0,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_SKIPFORWARDSEC,				{ 120,						"", 0, 0 } }, // 30
	{ eDB_PARAM_ITEM_INSTANTREPLAYSEC,				{ 15,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_COUNTRY_ID,					{ eCountryID_GBR,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_PADDINGTIME_START, 			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_PADDINGTIME_STOP,				{ 0,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_TSR_ONOFF,						{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUTO_DELETE,					{ FALSE,					"", 0, 0 } }, //36
	{ eDB_PARAM_ITEM_2ND_INT_HDD_UUID,				{ FALSE,					DbParam_UNDEFINED, 0, 0 } },
	{ eDB_PARAM_ITEM_STEREO_SELECT,					{ eDxSTEREO_SELECT_STEREO,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_INVISIBLE_SEARCH, 				{ FALSE,					"", 0, 0 } },

	{ eDB_PARAM_ITEM_ANT_CONN_TYPE,					{ eDxANTCONN_MAX,			"", 0, 0 } }, // 40
	{ eDB_PARAM_ITEM_PINCODE,						{ 0,						"0000", 0, 0 } },
	{ eDB_PARAM_ITEM_CHANNEL_SETUP_PIN,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_GUIDANCE_POLICY, 				{ eDxGUIDANCEPOLICY_OFF,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_SUBTITLE_SETTING,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_CEC_ENABLE, 				{ FALSE,						"", 0, 0 } },//45
	{ eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING,			{ TRUE,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_CUR_SVC,						{ HANDLE_NULL,				"", 0, 0 } },
	{ eDB_PARAM_ITEM_LAST_VOLUME,					{50,						{0, }, 0, NULL } },
	{ eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN,			{ 14400000,					"", 0, 0 } },	//	4hours (60 * 60 * 4 * 1000)
	{ eDB_PARAM_ITEM_LAST_DECODER_VOLUME,			{100,						{0, }, 0, NULL } },
	{ eDB_PARAM_ITEM_PVR_DEFAULT_HDD_UUID,			{ 0,						DbParam_UNDEFINED, 0, 0} },

	{ eDB_PARAM_ITEM_GMT_OFFSET_MINUTES,			{ 0,						{0, }, 0, 0 } },	// UK+0 //50
	{ eDB_PARAM_ITEM_TIME_REGION_ID,				{ 0,						{0, }, 0, 0 } },	//## uk향은 default 0
	{ eDB_PARAM_ITEM_DESCRAMBLEONOFF,				{ FALSE,					"", 0, 0 } },

	/* power management */

	/* edit channel list */

	/* installation */
	{ eDB_PARAM_ITEM_MAN_TP_FREQUENCY,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_POLARIZATION, 			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_FEC,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_TRANSSPEC,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_MODULATION,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_PILOT,					{ 0,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_TER_SRCH_CHANNEL,				{ 21,						"", 0, 0 } },			// CH 21  //60
	{ eDB_PARAM_ITEM_TER_SRCH_FREQUENCY,			{ 470000,					"", 0, 0 } },			// default frequency
	{ eDB_PARAM_ITEM_TER_SRCH_BANDWIDTH,			{ eDxTER_BANDWIDTH_8M,		"", 0, 0 } },			// 7MHz
	{ eDB_PARAM_ITEM_TER_SRCH_NETSEARCH,			{ 0,						"", 0, 0 } },			//	HBOOL, 0:FALSE, 1:TRUE
	{ eDB_PARAM_ITEM_TER_SRCH_NETWORKID,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_TER_SRCH_POWER,				{ 0,						"", 0, 0 } },

	/* DisEqc1.3 MyPosition */
	{ eDB_PARAM_ITEM_MY_LONGITUDE,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MY_LATITUDE, 					{ 0,						"", 0, 0 } },

	/* uk dtt 사양 */
	{ eDB_PARAM_ITEM_ASO_NOTIFY_FLAG,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO,		{ 0,						"", 2100/*sizeof(XsvcSi_NcdInfoData_UkDtt_t)*/,&dbDefaultBuf } },
	{ eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO,			{ 0,						"", 512/*XsvcSi_UKDTT_MAX_NCD_MESSAGE_LEN*/, &dbDefaultBuf } }, //70
	{ eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_REGION_NAMEINFO_CNT,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_REGION_NAMEINFO,				{ 0,						"", 48*9, &dbDefaultBuf } },
	{ eDB_PARAM_ITEM_DISPLAY_CRID_SETTING,			{ 1,						"", 0, 0 } },			//HBOOL, 0:Disable, 1:Enable
	{ eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT,			{ TRUE,						"", 0, 0 } },

	/* live streaming */
	{ eDB_PARAM_ITEM_LIVESTREAM_PRIORITY,			{ eDxLsPriority_EQUAL_VIEW,	"", 0, 0 } },			// default: 시나리오 미적용시 view, 적용시 high

};

HINT32 xdb_default_GetSystemConfigItemNum_Fvp4000t(void)
{
	return sizeof(s_astHardCodingSystemItemList_Fvp4000t) / sizeof(dbParam_DefaultItem_t);
}

dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Fvp4000t(HUINT32 ulItemIdx)
{
	if (xdb_default_GetSystemConfigItemNum_Fvp4000t() <= ulItemIdx)
		return NULL;

	return (dbParam_DefaultItem_t*)&s_astHardCodingSystemItemList_Fvp4000t[ulItemIdx];
}

/*********************** End of File ******************************/

