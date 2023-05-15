/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_param_table.c
	@brief

	Description:  \n
	Module: db/param \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <dapi.h>
#include <hapi.h>

#include <db_param.h>
#include "_db_param.h"
#include "../local_include/_db_main.h"
#include <pdb_param.h>

//	the field key value is defined in oapi_scenario.h
//	we may change the key value defined file.
#include <oapi_scenario.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	DB_PARAM_ITEM_2_KEY_TABLE_SIZE()		(sizeof(s_astParam2DbKey) / sizeof(dbParam_ConvertTable))

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	DbParam_Item_e		eParamItem;
	DxDBSETUP_Item_e	eDxItem;
	HCHAR				*pszKey;
	dbStorageType_e		eStorageType;
	HCHAR				*pszSubKey;
} dbParam_ConvertTable;

/*******************************************************************/
/********************      Local Variables      ********************/
/*******************************************************************/

STATIC const dbParam_ConvertTable s_astParam2DbKey[] =
{
	{ eDB_PARAM_ITEM_OPERATOR_TYPE,						0,												(void *)OAPI_SCENARIO_OPERATORTYPE,			eDB_STORAGE_TYPE_REG_ENUM,	DxNAMEOF(DxOperator_e) },
	{ eDB_PARAM_ITEM_LANG_MAIN,							0,												(void *)OAPI_SCENARIO_MAINLANGUAGE,			eDB_STORAGE_TYPE_REG_STR,	NULL  },
#if 0
	{ eDB_PARAM_ITEM_LANG_EVENT,						0,												(void *)OAPI_SCENARIO_EVENTLANGUAGE,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
#endif
	{ eDB_PARAM_ITEM_LANG_AUDIO0,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE0,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO1,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE1,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO2,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE2,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO3,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE3,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO4,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE4,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO5,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE5,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO6,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE6,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO7,						0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE7,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED,			0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL,			0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT, 			0,												(void *)OAPI_SCENARIO_AUDIOLANGUAGE_DEFAULT, 	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE0, 					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE0, 	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE1,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE1, 	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE2,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE2,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE3,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE3,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE4,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE4,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE5,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE5,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE6,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE6,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE7,					0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE7,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED,			0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL,			0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT,				0,												(void *)OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_HARDOFHEARING,						0,												(void *)OAPI_SCENARIO_HARDOFHEARING,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_MATURITYRATING,					0,												(void *)OAPI_SCENARIO_MATURITYRATING,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_SKIPFORWARDSEC,					eDxSETUP_SYSTEM_SKIP_FORWARD_SEC, 				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_INSTANTREPLAYSEC,					eDxSETUP_SYSTEM_INSTANCE_REPLAY_SEC,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_COUNTRY_ID,						0,												(void *)OAPI_SCENARIO_COUNTRYCODE,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_AUDIO_DESCRIPTION,					eDxSETUP_SYSTEM_AUDIODESCRIPTION,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_PINCODE,							eDxSETUP_SECURE_SYSTEM_PINCODE,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_STR,	NULL  },
	{ eDB_PARAM_ITEM_PADDINGTIME_START,					0,												(void *)OAPI_SCENARIO_STARTPADDING,			eDB_STORAGE_TYPE_REG_INT,	NULL  },

	{ eDB_PARAM_ITEM_PADDINGTIME_STOP,					0,												(void *)OAPI_SCENARIO_ENDPADDING,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_TSR_ONOFF,							eDxSETUP_SYSTEM_TSR_ON_OFF,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_AUTO_DELETE,						0,												(void *)OAPI_SCENARIO_AUTODELETE,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_STEREO_SELECT,						eDxSETUP_SYSTEM_STEREO_SELECT,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxStereoSelect_e)  },
	{ eDB_PARAM_ITEM_DUALMONO_SELECT,					eDxSETUP_SYSTEM_DUAL_MONO,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxDualMonoSelect_e) },
	{ eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT,					eDxSETUP_SYSTEM_HDMI_AUDIO_FORMAT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxDigitalAudioFormat_e) },
	{ eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT,				eDxSETUP_SYSTEM_SPDIF_AUDIO_FORMAT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxDigitalAudioFormat_e) },
	{ eDB_PARAM_ITEM_LIPSYNC_DELAY,						eDxSETUP_SYSTEM_LIPSYNC_DELAY,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LAST_VOLUME,						eDxSETUP_USER_LAST_VOLUME,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LAST_DECODER_VOLUME,				eDxSETUP_USER_LAST_DECODER_VOLUME,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_INVISIBLE_SEARCH,					eDxSETUP_SYSTEM_INVISIBLE_SEARCH,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_POSTCODE,							0,												(void *)OAPI_SCENARIO_POSTCODE,				eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_ANT_CONN_TYPE,						eDxSETUP_SYSTEM_ANTENNA_CONNECTION,				(void *)OAPI_SCENARIO_ANTENNACONNECTION,	eDB_STORAGE_TYPE_REG_ENUM,	DxNAMEOF(DxAntennaConnection_e) },
	{ eDB_PARAM_ITEM_GUIDANCE_POLICY,					0,												(void *)OAPI_SCENARIO_GUIDANCEPOLICY,		eDB_STORAGE_TYPE_REG_ENUM,	DxNAMEOF(DxGuidancePolicy_e) },
	{ eDB_PARAM_ITEM_CHANNEL_SETUP_PIN,					0,												(void *)OAPI_SCENARIO_CHANNELSETUPPIN,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_ADULTCHANNELS,						0,												(void *)OAPI_SCENARIO_ADULTCHANNEL,			eDB_STORAGE_TYPE_REG_ENUM,	DxNAMEOF(DxAdultChannelType_e) },
	{ eDB_PARAM_ITEM_SUBTITLE_SETTING,					0,												(void *)OAPI_SCENARIO_SUBTITLESETTING,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_HDMI_CEC_ENABLE,					eDxSETUP_SYSTEM_HDMICEC_MODE,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
   	{ eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE,			eDxSETUP_SYSTEM_HDMICEC_VOLUME_ENABLE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_DAYLIGHT_SAVING,					0,												(void *)OAPI_SCENARIO_DAYLIGHTSAVING,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING,			eDxSETUP_SYSTEM_IR_VIRTUAL_SECURECW_SETTING,	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },		// CONFIG_MW_CAS_IRDETO && CONFIG_MW_CAS_IRDETO_SECURE
	{ eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP,		0,												(void *)OAPI_SCENARIO_TIMEUPDATE,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_2ND_INT_HDD_UUID,					eDxSETUP_SYSTEM_2ND_INT_HDD_UUID,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_STR,	NULL  },
	{ eDB_PARAM_ITEM_SWUPDATE_VERSION,					0,												(void *)OAPI_SCENARIO_SWUPDATEVERSION,		eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_IPSWUPDATE_VERSION,				0,												(void *)OAPI_SCENARIO_IPSWUPDATEVERSION,	eDB_STORAGE_TYPE_REG_STR,	NULL  },
	{ eDB_PARAM_ITEM_TVSCART_FORMAT,					eDxSETUP_STSTEM_TVSCART_FORMAT,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxScartFormat_e) },
	{ eDB_PARAM_ITEM_VCRSCART_FORMAT,					eDxSETUP_STSTEM_VCRSCART_FORMAT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxScartFormat_e) },
	{ eDB_PARAM_ITEM_SCART_PATH,						eDxSETUP_STSTEM_SCART_PATH,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxScartPath_e) },
	{ eDB_PARAM_ITEM_TV_ASPECTRATIO,					eDxSETUP_STSTEM_TV_ASPECTRATIO,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxAspectRatio_e) },
	{ eDB_PARAM_ITEM_DFC_SELECT,						eDxSETUP_STSTEM_DFC_SELECT,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxDfcSelect_e) },
	{ eDB_PARAM_ITEM_RESOLUTION_SELECT,					eDxSETUP_STSTEM_RESOLUTION_SELECT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxResolutionSelect_e) },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_R,				eDxSETUP_STSTEM_BACKGROUND_COLOR_R,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_G,				eDxSETUP_STSTEM_BACKGROUND_COLOR_G,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_BACKGROUND_COLOR_B,				eDxSETUP_STSTEM_BACKGROUND_COLOR_B,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_3D_SELECT,							eDxSETUP_STSTEM_3D_SELECT,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_ENUM,	DxNAMEOF(DxHdmi3DSelect_e)  },
	{ eDB_PARAM_ITEM_GMT_OFFSET_MINUTES,				0,												(void *)OAPI_SCENARIO_GMTOFFSET,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_TIME_REGION_ID,					eDxSETUP_SYSTEM_TIME_REGION_ID,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ANTSET_ANTENNATYPE,				eDxSETUP_SYSTEM_ANTSET_ANTENNATYPE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ANTSET_SAT_TUNER01,				eDxSETUP_SYSTEM_ANTSET_SAT_TUNER01,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ANTSET_SAT_TUNER02,				eDxSETUP_SYSTEM_ANTSET_SAT_TUNER02,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ANTSET_SAT_TUNER03,				eDxSETUP_SYSTEM_ANTSET_SAT_TUNER03,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SRCH_TPSELECTION,					eDxSETUP_SYSTEM_SRCH_TPSELECTION,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_MOTORSET_LNBFREQ,					eDxSETUP_SYSTEM_MOTORSET_LNBFREQ,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MOTORSET_LNBPOWER,					eDxSETUP_SYSTEM_MOTORSET_LNBPOWER,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MOTORSET_MOTORTYPE,				eDxSETUP_SYSTEM_MOTORSET_MOTORTYPE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_POSITIONSET_SATTYPE,				eDxSETUP_SYSTEM_POSITIONSET_SATTYPE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_POSITIONSET_MOTORDRIVE,			eDxSETUP_SYSTEM_POSITIONSET_MOTORDRIVE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_POSITIONSET_DRIVESIZE,				eDxSETUP_SYSTEM_POSITIONSET_DRIVESIZE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LIMITSET_MOTORLIMIT,				eDxSETUP_SYSTEM_LIMITSET_MOTORLIMIT,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LIMITSET_LIMITPOS,					eDxSETUP_SYSTEM_LIMITSET_LIMITPOS,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_TUNER_SELECTION,				eDxSETUP_SYSTEM_SAT_TUNER_SELECTION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_SRCH_ALL_ANT,					eDxSETUP_SYSTEM_SAT_SRCH_ALL_ANT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_TRANSPONDER,					eDxSETUP_SYSTEM_SAT_TRANSPONDER,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_NETWORK_SRCH,					eDxSETUP_SYSTEM_SAT_NETWORK_SRCH,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_CAFREE_TYPE,					eDxSETUP_SYSTEM_SAT_CAFREE_TYPE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_CHANNEL_TYPE,					eDxSETUP_SYSTEM_SAT_CHANNEL_TYPE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SAT_SEARCH_MODE,					eDxSETUP_SYSTEM_SAT_SEARCH_MODE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_CHANNEL,					eDxSETUP_SYSTEM_TER_SRCH_CHANNEL,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_FREQUENCY,				eDxSETUP_SYSTEM_TER_SRCH_FREQUENCY,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_BANDWIDTH,				eDxSETUP_SYSTEM_TER_SRCH_BANDWIDTH,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_NETSEARCH,				eDxSETUP_SYSTEM_TER_SRCH_NETSEARCH,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_NETWORKID,				eDxSETUP_SYSTEM_TER_SRCH_NETWORKID,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_POWER,					eDxSETUP_SYSTEM_TER_SRCH_POWER,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_TRANSMISSION,				eDxSETUP_SYSTEM_TER_SRCH_TRANSMISSION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TER_SRCH_AUTO_CH_NUMBERING,		eDxSETUP_SYSTEM_TER_SRCH_AUTO_CH_NUMBERING,		(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_TUNERID,					eDxSETUP_SYSTEM_CAB_SRCH_TUNERID,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_NETSEARCH,				eDxSETUP_SYSTEM_CAB_SRCH_NETSEARCH,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_CAFREE_TYPE,				eDxSETUP_SYSTEM_CAB_SRCH_CAFREE_TYPE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_CHANNEL_TYPE,				eDxSETUP_SYSTEM_CAB_SRCH_CHANNEL_TYPE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_FREQUENCY,				eDxSETUP_SYSTEM_CAB_SRCH_FREQUENCY,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_SYMBOL_RATE,				eDxSETUP_SYSTEM_CAB_SRCH_SYMBOL_RATE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_MODULATION,				eDxSETUP_SYSTEM_CAB_SRCH_MODULATION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_NETWORK_ID,				eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_ID,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STARTFREQUENCY,	eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STARTFREQUENCY,	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STOPFREQUENCY,		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STOPFREQUENCY,	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_LINEAR_SRCH_STEP,				eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_STEP,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_LINEAR_SRCH_MODULATION,		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_MODULATION,		(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_LINEAR_SRCH_SYMBOL_RATE,		eDxSETUP_SYSTEM_CAB_LINEAR_SRCH_SYMBOL_RATE,	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CAB_SRCH_NETWORK_SRCH_MODE,		eDxSETUP_SYSTEM_CAB_SRCH_NETWORK_SRCH_MODE,		(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_FREQUENCY,					eDxSETUP_SYSTEM_MAN_TP_FREQUENCY,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_SYMBOLRATE,					eDxSETUP_SYSTEM_MAN_TP_SYMBOLRATE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_POLARIZATION,				eDxSETUP_SYSTEM_MAN_TP_POLARIZATION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_FEC,						eDxSETUP_SYSTEM_MAN_TP_FEC,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_TRANSSPEC,					eDxSETUP_SYSTEM_MAN_TP_TRANSSPEC,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_MODULATION,					eDxSETUP_SYSTEM_MAN_TP_MODULATION,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MAN_TP_PILOT,						eDxSETUP_SYSTEM_MAN_TP_PILOT,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MOTOR_USER_LONGITUDE,				eDxSETUP_SYSTEM_MOTOR_USER_LONGITUDE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_TVTV_ENABLE,						0,												(void *)OAPI_SCENARIO_TVTV_ENABLE,			eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_TVTV_DOWNTYPE,						0,												(void *)OAPI_SCENARIO_TVTV_DOWNTYPE,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_MY_LONGITUDE,						eDxSETUP_USER_MY_LONGITUDE,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_MY_LATITUDE,						eDxSETUP_USER_MY_LATITUDE,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CUR_SVC,							eDxSETUP_USER_CUR_SVC,							(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_USER_PINCODE,						eDxSETUP_USER_PINCODE,							(void *)NULL,								eDB_STORAGE_TYPE_DAMA_STR,	NULL  },
	{ eDB_PARAM_ITEM_REGION_CODE,						eDxSETUP_USER_REGION_CODE,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_BOUQUET_ID,						eDxSETUP_USER_BOUQUET_ID,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_HOME_TP_INFO_1,					eDxSETUP_USER_HOME_TP_INFO_1,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_HOME_TP_INFO_2,					eDxSETUP_USER_HOME_TP_INFO_2,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_HOME_TP_INFO_3,					eDxSETUP_USER_HOME_TP_INFO_3,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_BAT_INFO,							eDxSETUP_USER_BAT_INFO,							(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_LAST_TV_SVC,						eDxSETUP_USER_LAST_TV_SVC,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_LAST_RADIO_SVC,					eDxSETUP_USER_LAST_RADIO_SVC,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_UPC_7DAY_TIMEDATA,					eDxSETUP_USER_UPC_7DAY_TIMEDATA,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_DETECT_INFO,					eDxSETUP_USER_OTA_DETECT_INFO,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_FLAG,							eDxSETUP_USER_OTA_FLAG,							(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_USERCANCELLED_VER,				eDxSETUP_USER_OTA_USERCANCELLED_VER,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_USERCANCELLED_TYPE,			eDxSETUP_USER_OTA_USERCANCELLED_TYPE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_TSINFO,						eDxSETUP_USER_OTA_TSINFO,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_OTAINFO,						eDxSETUP_USER_OTA_OTAINFO,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO,				eDxSETUP_USER_OTA_DOWNLOADEDINFO,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_EMERGENCYINFO,					eDxSETUP_USER_OTA_EMERGENCYINFO,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_SCHEDULEINFO,					eDxSETUP_USER_OTA_SCHEDULEINFO,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_OTA_RETRYCOUNT,					eDxSETUP_USER_OTA_RETRYCOUNT,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_RETRYVER,						eDxSETUP_USER_OTA_RETRYVER,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_RETRYTYPE,						eDxSETUP_USER_OTA_RETRYTYPE,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OTA_WAKEUP_FLAG,					eDxSETUP_USER_OTA_WAKEUP_FLAG,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_PVR_DEFAULT_HDD_UUID,				eDxSETUP_USER_PVR_DEFAULT_HDD_UUID,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_STR,	NULL  },
	{ eDB_PARAM_ITEM_HOME_CHANNEL,						eDxSETUP_USER_HOME_CHANNEL,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_ANTENNA_SORT_METHOD,				eDxSETUP_USER_ANTENNA_SORT_METHOD,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REGULAR_AUTO_UPDATE_SUCCESS,		eDxSETUP_SYSTEM_REGULAR_AUTO_UPDATE_SUCCESS,	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ADULT_CHANNEL,						eDxSETUP_SYSTEM_ADULT_CHANNEL,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_GUIDANCE_CHANNEL_GENRE,			eDxSETUP_SYSTEM_GUIDANCE_CHANNEL_GENRE,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_NIT_TS_LOOP_CRC32,					eDxSETUP_USER_NIT_TS_LOOP_CRC32,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_FAV_CHNUM_ORDER,					eDxSETUP_SYSTEM_FAV_CHNUM_ORDER,				(void *)OAPI_SCENARIO_FAVCHLISTNUMBERING,	eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING,			eDxSETUP_SYSTEM_DYNAMIC_UPDATE_SETTING,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ANTENNA_POWER,					eDxSETUP_SYSTEM_ANTENNA_POWER,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_CABLE_SEARCH_MODE,					eDxSETUP_SYSTEM_CABLE_SEARCH_MODE,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SVCLISTID,							eDxSETUP_SYSTEM_SES_SVCLISTID,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SVCLISTVER,						eDxSETUP_SYSTEM_SES_SVCLISTVER,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SVCLISTNAME, 						eDxSETUP_SYSTEM_SES_SVCLISTNAME,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_TVNUM,								eDxSETUP_SYSTEM_SES_TVNUM,						(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_RADIONUM,							eDxSETUP_SYSTEM_SES_RADIONUM,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LASTCHECKEDTIME, 					eDxSETUP_SYSTEM_SES_LASTCHECKEDTIME,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LASTUPDATEDTIME, 					eDxSETUP_SYSTEM_SES_LASTUPDATEDTIME,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REPLACESVC_NUM,					eDxSETUP_SYSTEM_SES_REPLACESVC_NUM,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REPLACESVC_ITEMS,					eDxSETUP_SYSTEM_SES_REPLACESVC_ITEMS,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_NEWSVC_NUM,						eDxSETUP_SYSTEM_SES_NEWSVC_NUM,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_NEWSVC_ITEMS,						eDxSETUP_SYSTEM_SES_NEWSVC_ITEMS,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_SCINSERTED,						eDxSETUP_SYSTEM_SES_SCINSERTED,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REPLACELATER_NUM,					eDxSETUP_SYSTEM_SES_REPLACELATER_NUM,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REPLACELATER_ITEMS,				eDxSETUP_SYSTEM_SES_REPLACELATER_ITEMS,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_COLDBOOT_RESULT,					eDxSETUP_SYSTEM_SES_COLDBOOT_RESULT,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SATREC_RECEIVERID,					0,												(void *)OAPI_SCENARIO_SATREC_RECEIVERID,	eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_SATREC_DETECTINFO,					eDxSETUP_SYSTEM_SES_SATREC_DETECTINFO,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_SATREC_FBDVERSION,					eDxSETUP_SYSTEM_SES_SATREC_FBDVERSION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SATREC_BDVERSION,					eDxSETUP_SYSTEM_SES_SATREC_BDVERSION,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SATREC_UPDATE_INTERVAL,			0,												(void *)OAPI_SCENARIO_SATREC_INTERVAL,		eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_OLDPREPAIDRIGHT,					eDxSETUP_SYSTEM_SES_OLDPREPAIDRIGHT,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_EM36_OPEN,							eDxSETUP_SYSTEM_SES_EM36_OPEN,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_SC_OLD_SN,							eDxSETUP_SYSTEM_SES_SC_OLD_SN,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_DESCRAMBLEONOFF,					0,												(void *)OAPI_SCENARIO_DESCRAMBLEONOFF,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_BATUPDATEONOFF,					0,												(void *)OAPI_SCENARIO_BATUPDATEONOFF,		eDB_STORAGE_TYPE_REG_INT,	NULL  },
   	{ eDB_PARAM_ITEM_NITLCNUPDATEONOFF,					0,												(void *)OAPI_SCENARIO_NITLCNUPDATEONOFF,	eDB_STORAGE_TYPE_REG_INT,	NULL  },
    { eDB_PARAM_ITEM_ASO_NOTIFY_FLAG,					eDxSETUP_USER_ASO_NOTIFY_FLAG, 					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_ASO_NETWORK_CHANGE_INFO, 			eDxSETUP_USER_ASO_NETWORK_CHANGE_INFO,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_ASO_NETWORK_MSG_INFO, 				eDxSETUP_USER_ASO_ASO_NETWORK_MSG_INFO,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_ASO_LAST_SCAN_TIME,				eDxSETUP_USER_ASO_LAST_SCAN_TIME,			 	(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REGION_NAMEINFO_CNT,				eDxSETUP_USER_REGION_NAMEINFO_CNT,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_REGION_NAMEINFO,					eDxSETUP_USER_REGION_NAMEINFO_PTR,				(void *)NULL,								eDB_STORAGE_TYPE_DAMA_BIN,	NULL  },
	{ eDB_PARAM_ITEM_DISPLAY_CRID_SETTING, 				eDxSETUP_SYSTEM_DISPLAY_CRID_SETTING,			(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LAST_MIC_VOLUME,					eDxSETUP_USER_LAST_MIC_VOLUME,					(void *)NULL,								eDB_STORAGE_TYPE_DAMA_INT,	NULL  },
	{ eDB_PARAM_ITEM_LIVESTREAM_PRIORITY,				eDxSETUP_USER_LIVE_STREAM_PRIORITY,				(void *)OAPI_SCENARIO_LIVESTREAM_PRIORITY,	eDB_STORAGE_TYPE_REG_ENUM, DxNAMEOF(DxLiveStreamPriority_e)  },
	{ eDB_PARAM_ITEM_AUTOMATIC_POWERDOWN,				eDxSETUP_SYSTEM_AUTOMATIC_POWERDOWN,			(void *)OAPI_SCENARIO_AUTOMATIC_POWERDOWN,	eDB_STORAGE_TYPE_REG_INT,	NULL  },
	{ eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT,				eDxSETUP_SYSTEM_PRIORITY_HDMIOUTPUT,			(void *)OAPI_SCENARIO_PRIORITY_HDMIOUTPUT,	eDB_STORAGE_TYPE_REG_INT,	NULL  },

	/* 위로 추가하세요... */
	{ eDB_PARAM_ITEM_MAX, 								0xFFFFFFFF,										(void *)NULL,								eDB_STORAGE_TYPE_MAX,		NULL  },
};

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

DbParam_Item_e db_param_TableGetParamItemByDxItem(const DxDBSETUP_Item_e eDxItem)
{
	HUINT32		i;
	HUINT32		ulNumItem = DB_PARAM_ITEM_2_KEY_TABLE_SIZE();

	for (i=0; i<ulNumItem; i++)
	{
		if (dbParam_IsRegistry(s_astParam2DbKey[i].eStorageType))
		{
		}
		else
		{
			if (s_astParam2DbKey[i].eDxItem == 0xFFFFFFFF)
				break;
			if (s_astParam2DbKey[i].eDxItem == eDxItem)
				return s_astParam2DbKey[i].eParamItem;
		}
	}

	return eDB_PARAM_ITEM_UNKNOWN;
};

DbParam_Item_e db_param_TableGetParamItemByRegKey(const HCHAR *pszKey)
{
	HUINT32		 i;
	HUINT32		 ulNumItem = DB_PARAM_ITEM_2_KEY_TABLE_SIZE();
	HUINT32		 ulLen = 0, ulStaticLen = 0;

	if (NULL == pszKey)
	{
		return eDB_PARAM_ITEM_UNKNOWN;
	}

	ulStaticLen = HxSTD_StrLen(pszKey);
	if (0 == ulStaticLen)
	{
		return eDB_PARAM_ITEM_UNKNOWN;
	}

	for (i=0; i<ulNumItem; i++)
	{
		if (s_astParam2DbKey[i].eParamItem >= eDB_PARAM_ITEM_MAX)
			break;

		if (TRUE == dbParam_IsRegistry(s_astParam2DbKey[i].eStorageType))
		{
			if (s_astParam2DbKey[i].pszKey)
			{
				ulLen = HxMACRO_MAX(HxSTD_StrLen(s_astParam2DbKey[i].pszKey), ulStaticLen);
				if (0 == HxSTD_StrNCmp(s_astParam2DbKey[i].pszKey, pszKey, ulLen))
					return s_astParam2DbKey[i].eParamItem;
			}
		}
	}

	return eDB_PARAM_ITEM_UNKNOWN;
};

STATIC HERROR db_param_TableGetDbKey(DbParam_Item_e eItem, dbStorageType_e *peStorageType, HUINT32 *pulKey, HCHAR **ppszSubKey)
{
	HUINT32		i;
	HUINT32		ulNumItem = DB_PARAM_ITEM_2_KEY_TABLE_SIZE();

	for (i=0; i<ulNumItem; i++)
	{
		if (s_astParam2DbKey[i].eParamItem >= eDB_PARAM_ITEM_MAX)
			break;

		if (s_astParam2DbKey[i].eParamItem != eItem)
			continue;

		if (peStorageType)	*peStorageType = s_astParam2DbKey[i].eStorageType;
		if (ppszSubKey)		*ppszSubKey = s_astParam2DbKey[i].pszSubKey;
		if (pulKey)			*pulKey = dbParam_IsRegistry(s_astParam2DbKey[i].eStorageType) ? (HUINT32)s_astParam2DbKey[i].pszKey : (HUINT32)s_astParam2DbKey[i].eDxItem;
		return ERR_OK;
	}

	return ERR_FAIL;
};

HERROR	db_param_TableGetDataType(DbParam_Item_e eItem, DbParam_Type_e *peDataType)
{
	HERROR					hErr = ERR_FAIL;
	dbStorageType_e			eStorageType;
	DbParam_Type_e			eType = eDB_PARAM_TYPE_Max;

	if (eDB_PARAM_ITEM_UNKNOWN == eItem || eDB_PARAM_ITEM_MAX <= eItem)
	{
		return ERR_FAIL;
	}

	if (NULL == peDataType)
	{
		return ERR_FAIL;
	}

	eStorageType = eDB_PARAM_TYPE_Max;
	hErr = db_param_TableGetDbKey(eItem, &eStorageType, NULL, NULL);
	if (ERR_OK == hErr)
	{
		hErr = ERR_FAIL;
		switch (eStorageType)
		{
		case eDB_STORAGE_TYPE_REG_INT:		eType = eDB_PARAM_TYPE_Int;		hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_REG_STR:		eType = eDB_PARAM_TYPE_String;	hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_REG_ENUM:		eType = eDB_PARAM_TYPE_Enum;	hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_DAMA_INT:		eType = eDB_PARAM_TYPE_Int;		hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_DAMA_STR:		eType = eDB_PARAM_TYPE_String;	hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_DAMA_ENUM:	eType = eDB_PARAM_TYPE_Enum;	hErr = ERR_OK;	break;
		case eDB_STORAGE_TYPE_DAMA_BIN:		eType = eDB_PARAM_TYPE_Bin;		hErr = ERR_OK;	break;
		default:
			break;
		}
	}

	if (ERR_OK == hErr)
	{
		*peDataType = eType;
	}

	return hErr;
}


HERROR db_param_TableConfigItem(dbParam_ItemConfig_t *pstConfig, HBOOL bSet, DbParam_Item_e eItem, HUINT32 ulData, HUINT32 ulParam)
{
	HERROR		 hErr = ERR_FAIL;
	HUINT32		 ulKey;
	HCHAR		*pszSubKey = NULL;

	if (NULL == pstConfig)
		return ERR_FAIL;

	HxSTD_MemSet(pstConfig, 0, sizeof(dbParam_ItemConfig_t));

	ulKey = 0;
	hErr = db_param_TableGetDbKey(eItem, &pstConfig->eType, &ulKey, &pszSubKey);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	switch (pstConfig->eType)
	{
	case eDB_STORAGE_TYPE_REG_INT:
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)ulData;
		}
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_REG_STR:
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegStr.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)ulData;
		}
		pstConfig->utParam.utRegStr.ulBufLen		= ulParam;
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_REG_ENUM:
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegEnum.pszKey			= (HCHAR *)ulKey;
		pstConfig->utParam.utRegEnum.pszEnumType	= pszSubKey;
		if (bSet)
		{
			pstConfig->utParam.utRegEnum.eEnumValue	= ulData;
		}
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_DAMA_INT:
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utDamaInt.eKey			= (DxDBSETUP_Item_e)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utDamaInt.nValue		= (HINT32)ulData;
		}
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_DAMA_STR:
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utDamaStr.eKey			= (DxDBSETUP_Item_e)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utDamaStr.pszString	= (HCHAR *)ulData;
		}
		pstConfig->utParam.utDamaStr.ulBufLen		= ulParam;
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_DAMA_ENUM:
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utDamaEnum.eKey			= (DxDBSETUP_Item_e)ulKey;
		pstConfig->utParam.utDamaEnum.pszEnumType	= pszSubKey;
		if (bSet)
		{
			pstConfig->utParam.utDamaEnum.eEnumValue	= (HINT32)ulData;
		}
		hErr = ERR_OK;
		break;
	case eDB_STORAGE_TYPE_DAMA_BIN:
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utDamaBin.eKey			= (DxDBSETUP_Item_e)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utDamaBin.pvData		= (void *)ulData;
		}
		pstConfig->utParam.utDamaBin.ulBufLen		= ulParam;
		pstConfig->utParam.utDamaBin.bCompare		= FALSE;
		hErr = ERR_OK;
		break;
	default:
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Warning("Invalid DB Type [%d]\n", pstConfig->eType);
		return ERR_FAIL;
	}

	switch (eItem)
	{
	case eDB_PARAM_ITEM_LANG_MAIN:					// OAPI_SCENARIO_MAINLANGUAGE;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegStr.pszKey 			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)ulData);
		}
		pstConfig->utParam.utRegStr.ulBufLen		= 4;
		hErr = ERR_OK;
		break;

#if 0
	case eDB_PARAM_ITEM_LANG_EVENT:					// OAPI_SCENARIO_EVENTLANGUAGE;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegStr.pszKey 			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)ulData);
		}
		pstConfig->utParam.utRegStr.ulBufLen		= 4;
		hErr = ERR_OK;
		break;
#endif

	case eDB_PARAM_ITEM_LANG_AUDIO0:				// OAPI_SCENARIO_AUDIOLANGUAGE0
	case eDB_PARAM_ITEM_LANG_AUDIO1:				// OAPI_SCENARIO_AUDIOLANGUAGE1
	case eDB_PARAM_ITEM_LANG_AUDIO2:				// OAPI_SCENARIO_AUDIOLANGUAGE2
	case eDB_PARAM_ITEM_LANG_AUDIO3:				// OAPI_SCENARIO_AUDIOLANGUAGE3
	case eDB_PARAM_ITEM_LANG_AUDIO4:				// OAPI_SCENARIO_AUDIOLANGUAGE4
	case eDB_PARAM_ITEM_LANG_AUDIO5:				// OAPI_SCENARIO_AUDIOLANGUAGE5
	case eDB_PARAM_ITEM_LANG_AUDIO6:				// OAPI_SCENARIO_AUDIOLANGUAGE6
	case eDB_PARAM_ITEM_LANG_AUDIO7:				// OAPI_SCENARIO_AUDIOLANGUAGE7
	case eDB_PARAM_ITEM_LANG_AUDIO_UNDEFINED:	// OAPI_SCENARIO_AUDIOLANGUAGE_UNDEFFINED
	case eDB_PARAM_ITEM_LANG_AUDIO_ORIGINAL: 		// OAPI_SCENARIO_AUDIOLANGUAGE_ORIGINAL
	case eDB_PARAM_ITEM_LANG_AUDIO_DEFAULT:		// OAPI_SCENARIO_ADUIOLANGUAGE_DEFAULT
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegStr.pszKey 			= (HCHAR *)ulKey;	//db_param_GetRegAudioLangKey(eItem);
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)ulData);
		}
		pstConfig->utParam.utRegStr.ulBufLen		= 4;
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_LANG_SUBTITLE0:				// OAPI_SCENARIO_SUBTITLELANGUAGE0
	case eDB_PARAM_ITEM_LANG_SUBTITLE1:				// OAPI_SCENARIO_SUBTITLELANGUAGE1
	case eDB_PARAM_ITEM_LANG_SUBTITLE2:				// OAPI_SCENARIO_SUBTITLELANGUAGE2
	case eDB_PARAM_ITEM_LANG_SUBTITLE3:				// OAPI_SCENARIO_SUBTITLELANGUAGE3
	case eDB_PARAM_ITEM_LANG_SUBTITLE4:				// OAPI_SCENARIO_SUBTITLELANGUAGE4
	case eDB_PARAM_ITEM_LANG_SUBTITLE5:				// OAPI_SCENARIO_SUBTITLELANGUAGE5
	case eDB_PARAM_ITEM_LANG_SUBTITLE6:				// OAPI_SCENARIO_SUBTITLELANGUAGE6
	case eDB_PARAM_ITEM_LANG_SUBTITLE7:				// OAPI_SCENARIO_SUBTITLELANGUAGE7
	case eDB_PARAM_ITEM_LANG_SUBTITLE_UNDEFINED:	// OAPI_SCENARIO_SUBTITLELANGUAGE_UNDEFFINED
	case eDB_PARAM_ITEM_LANG_SUBTITLE_ORIGINAL:		// OAPI_SCENARIO_SUBTITLELANGUAGE_ORIGINAL
	case eDB_PARAM_ITEM_LANG_SUBTITLE_DEFAULT:		// OAPI_SCENARIO_SUBTITLELANGUAGE_DEFAULT
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegStr.pszKey 			= (HCHAR *)ulKey;	//db_param_GetRegSubtitleLangKey(eItem);
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)HLIB_LANG_IdTo639((HxLANG_Id_e)ulData);
		}
		pstConfig->utParam.utRegStr.ulBufLen		= 4;
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_HARDOFHEARING:				// OAPI_SCENARIO_HARDOFHEARING;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= ((HBOOL)ulData) ? 1 : 0;
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_AUTO_DELETE:				// OAPI_SCENARIO_AUTODELETE;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg 							= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)((HBOOL)ulData ? 1 : 0);
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_CHANNEL_SETUP_PIN:			// OAPI_SCENARIO_CHANNELSETUPPIN
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)(((HBOOL)ulData) ? 1 : 0);
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_SUBTITLE_SETTING:			// OAPI_SCENARIO_SUBTITLESETTING
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)(((HBOOL)ulData) ? 1 : 0);
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_DAYLIGHT_SAVING:			// OAPI_SCENARIO_DAYLIGHTSAVING;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)(((HBOOL)ulData) ? 1 : 0);
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_UPDATE_TIME_ON_SPECIFIC_TP:		// OAPI_SCENARIO_TIMEUPDATE;
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_INT;
		pstConfig->nMsg								= eItem;
		pstConfig->utParam.utRegInt.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegInt.nValue		= (HINT32)(((HBOOL)ulData) ? 1 : 0);
		}
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_2ND_INT_HDD_UUID:			// eDxSETUP_SYSTEM_2ND_INT_HDD_UUID
		pstConfig->eType							= eDB_STORAGE_TYPE_DAMA_STR;
		pstConfig->nMsg								= eDB_PARAM_ITEM_UNKNOWN;
		pstConfig->utParam.utDamaStr.eKey			= (DxDBSETUP_Item_e)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utDamaStr.pszString	= (HCHAR *)ulData;
		}
		pstConfig->utParam.utDamaStr.ulBufLen		= dbParam_2ND_INT_HDD_UUID_MAX;
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_SWUPDATE_VERSION:			// OAPI_SCENARIO_SWUPDATEVERSION
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eDB_PARAM_ITEM_UNKNOWN;
		pstConfig->utParam.utRegStr.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)(((HBOOL)ulParam) ? (HCHAR *)ulData : "");
		}
		pstConfig->utParam.utRegStr.ulBufLen		= ulParam;
		hErr = ERR_OK;
		break;

	case eDB_PARAM_ITEM_IPSWUPDATE_VERSION:			// OAPI_SCENARIO_IPSWUPDATEVERSION
		pstConfig->eType							= eDB_STORAGE_TYPE_REG_STR;
		pstConfig->nMsg 							= eDB_PARAM_ITEM_UNKNOWN;
		pstConfig->utParam.utRegStr.pszKey			= (HCHAR *)ulKey;
		if (bSet)
		{
			pstConfig->utParam.utRegStr.pszValue	= (HCHAR *)(((HBOOL)ulParam) ? (HCHAR *)ulData : "");
		}
		pstConfig->utParam.utRegStr.ulBufLen		= ulParam;
		hErr = ERR_OK;
		break;


	default:
		if (ERR_OK != hErr)
		{
			if (dbParam_IsRegistry(pstConfig->eType))
				HxLOG_Warning("DB Type [%d] : Key [%s]\n", pstConfig->eType, pstConfig->utParam.pszKey);
			else
				HxLOG_Warning("DB Type [%d] : Key [%d]\n", pstConfig->eType, pstConfig->utParam.eKey);
			return ERR_FAIL;
		}
		break;
	}

	return hErr;
}

/* end of file */
