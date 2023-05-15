/**
	@file     xdb_param_default_icordpro.c
	@brief

	Description: default db 생성을 위한 default value 모음
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

//#include <_db_main.h>
#include <_db_param_default.h>

#include "../local_include/_xdb_param.h"


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

STATIC const dbParam_DefaultItem_t	s_astHardCodingSystemItemList_Icordpro[] =
{
	{ eDB_PARAM_ITEM_OPERATOR_TYPE,					{ eDxOPERATOR_Ses,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_MAIN,						{ eLangID_German,			"", 0, 0 } }, // 추후 german으로 변경
	{ eDB_PARAM_ITEM_LANG_EVENT,					{ eLangID_German,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO0,					{ eLangID_German,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO1,					{ eLangID_German,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL, 			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT,				{ eLangID_German,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE0,				{ eLangID_NONE,				"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE1, 				{ eLangID_NONE, 			"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED,			{ eLangID_Undefined,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL,			{ eLangID_OriginalAudio,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT,				{ eLangID_German,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_MATURITYRATING,				{ eDxRATING_VIEW_ALL,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_HARDOFHEARING,					{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUDIO_DESCRIPTION,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_STEREO_SELECT,					{ 0,						DxSTR_STEREO_SELECT_STEREO, 0, 0 } },
	{ eDB_PARAM_ITEM_DUALMONO_SELECT,				{ 0,						DxSTR_DUALMONO_SELECT_BOTH, 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT,				{ 0,						DxSTR_DIGITALAUDIO_FORMAT_PCM, 0, 0 } },
	{ eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT,			{ 0,						DxSTR_DIGITALAUDIO_FORMAT_PCM, 0, 0 } },
	{ eDB_PARAM_ITEM_SKIPFORWARDSEC,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_INSTANTREPLAYSEC,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_COUNTRY_ID,					{ eCountryID_DEU,			"", 0, 0 } },		// country code는 Germany로 변경
	{ eDB_PARAM_ITEM_PADDINGTIME_START, 			{ 180,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_PADDINGTIME_STOP,				{ 180,						"", 0, 0 } },

	{ eDB_PARAM_ITEM_TVSCART_FORMAT, 				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_VCRSCART_FORMAT,				{ 0,						DxSTR_SCART_FORMAT_CVBS, 0, 0 } },
	{ eDB_PARAM_ITEM_SCART_PATH,					{ 0,						DxSTR_SCART_PATH_STB2TV, 0, 0 } },
	{ eDB_PARAM_ITEM_TV_ASPECTRATIO,				{ 0,						DxSTR_ASPECTRATIO_16X9, 0, 0 } },
	{ eDB_PARAM_ITEM_DFC_SELECT,					{ 0,						DxSTR_DFC_SELECT_DEFAULT, 0, 0 } },
	{ eDB_PARAM_ITEM_RESOLUTION_SELECT,				{ 0,						DxSTR_RESOLUTION_SELECT_1080I, 0, 0 } },
	{ eDB_PARAM_ITEM_3D_SELECT,						{ 0,						DxSTR_HDMI_3DSELECT_OFF, 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_R,			{ 0, 						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_G,			{ 0, 						"", 0, 0 } },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_B,			{ 0, 						"", 0, 0 } },

	{ eDB_PARAM_ITEM_TSR_ONOFF,						{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_AUTO_DELETE,					{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_2ND_INT_HDD_UUID,				{ FALSE,					DbParam_UNDEFINED, 0, 0 } },
	{ eDB_PARAM_ITEM_STEREO_SELECT,					{ eDxSTEREO_SELECT_STEREO,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_INVISIBLE_SEARCH, 				{ FALSE,					"", 0, 0 } },

	{ eDB_PARAM_ITEM_ANT_CONN_TYPE,					{ eDxANTCONN_MAX,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_PINCODE,						{ 0,						"0000", 0, 0 } },
	{ eDB_PARAM_ITEM_CHANNEL_SETUP_PIN,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_GUIDANCE_POLICY, 				{ eDxGUIDANCEPOLICY_OFF,	"", 0, 0 } },
	{ eDB_PARAM_ITEM_SUBTITLE_SETTING,				{ FALSE,					"", 0, 0 } },
	{ eDB_PARAM_ITEM_HDMI_CEC_ENABLE, 				{ TRUE,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, 		{ FALSE,					"", 0, 0 } },

	{ eDB_PARAM_ITEM_CUR_SVC,						{ HANDLE_NULL,				"", 0, 0 } },
	{ eDB_PARAM_ITEM_LAST_VOLUME,					{ 50, 						{0, }, 0, NULL } },
	{ eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN,			{ 10800000,					"", 0, 0 } },	//	3hours (60 * 60 * 3 * 1000)
	{ eDB_PARAM_ITEM_PVR_DEFAULT_HDD_UUID,			{ 0,						DbParam_UNDEFINED, 0, 0} },

	{ eDB_PARAM_ITEM_GMT_OFFSET_MINUTES,			{ 60,						"", 0, 0 } },
	/* power management */
#if defined(CONFIG_MW_EPG_TVTV)
	{ eDB_PARAM_ITEM_TVTV_ENABLE, 					{ TRUE,						"", 0, 0 } }, // enable
	{ eDB_PARAM_ITEM_TVTV_DOWNTYPE,					{ FALSE,					"", 0, 0 } }, // at active standby
#endif

	/* edit channel list */

	/* installation */
	{ eDB_PARAM_ITEM_ANTSET_SAT_TUNER01,			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_FREQUENCY,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_POLARIZATION, 			{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_FEC,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_TRANSSPEC,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_MODULATION,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MAN_TP_PILOT,					{ 0,						"", 0, 0 } },
//	no one get value from SEARCH_ACN
//	{ eDB_PARAM_ITEM_SEARCH_ACN,					{ eWord_Disable,			"", 0, 0 } },
	{ eDB_PARAM_ITEM_ANTSET_ANTENNATYPE,			{ eUiAntType_LNB_Only,		"", 0, 0 } },
	{ eDB_PARAM_ITEM_SAT_NETWORK_SRCH,				{ 1,						"", 0, 0 } },


	/* SCD Setting */
	/* nothing */

	/* DisEqc1.3 MyPosition */
	{ eDB_PARAM_ITEM_MY_LONGITUDE,					{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_MY_LATITUDE, 					{ 0,						"", 0, 0 } },

#if defined(CONFIG_MW_SATRECORD)
	/* Satlite Record */
	{ eDB_PARAM_ITEM_SATREC_FBDVERSION,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_SATREC_BDVERSION,				{ 0,						"", 0, 0 } },
	{ eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL,		{ eDxSATREC_INTERVAL_AUTO,	"", 0, 0 } },
#endif
};

HINT32 xdb_default_GetSystemConfigItemNum_Icordpro(void)
{
	return sizeof(s_astHardCodingSystemItemList_Icordpro) / sizeof(dbParam_DefaultItem_t);
}

dbParam_DefaultItem_t	*xdb_default_GetSystemConfigItem_Icordpro(HUINT32 ulItemIdx)
{
	if (xdb_default_GetSystemConfigItemNum_Icordpro() <= ulItemIdx)
		return NULL;

	return (dbParam_DefaultItem_t *)&s_astHardCodingSystemItemList_Icordpro[ulItemIdx];
}

STATIC void xdb_param_InitDefaultKeepingItem(dbParam_KeepingItem_t *pstItem)
{
	HxSTD_MemSet(pstItem, 0, sizeof(dbParam_KeepingItem_t));

	pstItem->stInfo.szItemText[0] = '\0';
}

#if defined(CONFIG_MW_SATRECORD)
STATIC	const DbParam_Item_e	 s_astKeepingItemList[] =
{
	eDB_PARAM_ITEM_SATREC_RECEIVERID,
	//add keeping DB item
};
#endif

HERROR xdb_param_GetKeepingConfigItem_Icordpro(dbParam_KeepingItem_t **pastItem, HINT32 *pnItemNum)
{
	HERROR							 hErr = ERR_FAIL;
#if defined(CONFIG_MW_SATRECORD)
	HINT32							 nItemNum = 0;

	// 1. Get Item Num
	nItemNum = sizeof(s_astKeepingItemList) / sizeof(DbParam_Item_e);

	HxLOG_Debug("nItemNum [%d]\n", nItemNum);

	hErr = xdb_param_KeepingItemCreateList(s_astKeepingItemList, (const HINT32)nItemNum, pastItem, pnItemNum);

	HxLOG_Debug("xdb_param_KeepingItemCreateList() return [%d]\n", hErr);
#endif

	return hErr;
}

HERROR xdb_param_FreeKeepingConfigItem_Icordpro(dbParam_KeepingItem_t *astItem, HINT32 nItemNum)
{
	return xdb_param_KeepingItemFree(astItem, nItemNum);
}

HBOOL xdb_param_FindKeepingConfigItem_Icordpro(DbParam_Item_e eItem)
{
#if defined(CONFIG_MW_SATRECORD)
	HINT32 i, nItemNum = sizeof(s_astKeepingItemList) / sizeof(DbParam_Item_e);

	for (i=0; i<nItemNum; i++)
	{
		if (s_astKeepingItemList[i] == eItem)
			return TRUE;
	}
#endif

	return FALSE;
}

/*********************** End of File ******************************/

