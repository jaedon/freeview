/**
	@file     xdb_param_base.c
	@brief

	Description: default db 생성을 위한 default value 모음
	Module: DB/param \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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
#include "../local_include/_xdb_param.h"
#include <_db_param_migration.h>

//	the field key value is defined in oapi_scenario.h
//	we may change the key value defined file.
#include <oapi_scenario.h>

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

HERROR xdb_param_MigrateItems_Base(void)
{
	HERROR	hErr;

	//	TODO: Move old DBSetup to Registry if needs
	hErr = db_param_MigrateEnum(eDxSETUP_SYSTEM_OPERATOR,				OAPI_SCENARIO_OPERATORTYPE, DxNAMEOF(DxOperator_e));

	hErr |= db_param_MigrateLang(eDxSETUP_SYSTEM_MAIN_LANGUAGE,			OAPI_SCENARIO_MAINLANGUAGE);
	hErr |= db_param_MigrateLang(eDxSETUP_SYSTEM_AUDIO_LANGUAGE+0,		OAPI_SCENARIO_AUDIOLANGUAGE0);
	hErr |= db_param_MigrateLang(eDxSETUP_SYSTEM_AUDIO_LANGUAGE+1,		OAPI_SCENARIO_AUDIOLANGUAGE1);
	hErr |= db_param_MigrateLang(eDxSETUP_SYSTEM_SUBTITLE_LANGUAGE+0,	OAPI_SCENARIO_SUBTITLELANGUAGE0);
	hErr |= db_param_MigrateLang(eDxSETUP_SYSTEM_SUBTITLE_LANGUAGE+1,	OAPI_SCENARIO_SUBTITLELANGUAGE1);

	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_HARDOFHEARING,			OAPI_SCENARIO_HARDOFHEARING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_MATURITYRATING,			OAPI_SCENARIO_MATURITYRATING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_COUNTRY_INDEX,			OAPI_SCENARIO_COUNTRYCODE);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_PADDING_START_TIME,		OAPI_SCENARIO_STARTPADDING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_PADDING_END_TIME,		OAPI_SCENARIO_ENDPADDING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_AUTO_DELETE,			OAPI_SCENARIO_AUTODELETE);

	hErr |= db_param_MigrateEnum(eDxSETUP_SYSTEM_ANTENNA_CONNECTION,	OAPI_SCENARIO_ANTENNACONNECTION, DxNAMEOF(DxAntennaConnection_e));
	hErr |= db_param_MigrateEnum(eDxSETUP_SYSTEM_GUIDANCE_POLICY,		OAPI_SCENARIO_GUIDANCEPOLICY, DxNAMEOF(DxGuidancePolicy_e));

	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_CHANNEL_SETUP_PIN,		OAPI_SCENARIO_CHANNELSETUPPIN);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_SUBTITLE_SETTING,		OAPI_SCENARIO_SUBTITLESETTING);
	hErr |= db_param_MigrateInt(eDxSETUP_HIDDEN_UPDATE_TIME_ON_ALL_TP,	OAPI_SCENARIO_TIMEUPDATE);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_GMT_OFFSET_MINUTES,		OAPI_SCENARIO_GMTOFFSET);

	hErr |= db_param_MigrateStr(eDxSETUP_SYSTEM_POSTCODE,				OAPI_SCENARIO_POSTCODE);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_ADULTCHANNEL,			OAPI_SCENARIO_ADULTCHANNEL);

	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_DAYLIGHT_SAVING,		OAPI_SCENARIO_DAYLIGHTSAVING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_DESCRAMBLEONOFF,		OAPI_SCENARIO_DESCRAMBLEONOFF);

	hErr |= db_param_MigrateInt(eDxSETUP_USER_BAT_UPGRADE_ONOFF,		OAPI_SCENARIO_BATUPDATEONOFF);
	hErr |= db_param_MigrateInt(eDxSETUP_USER_NIT_LCN_UPGRADE_ONOFF,	OAPI_SCENARIO_NITLCNUPDATEONOFF);

	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_FAV_CHNUM_ORDER,		OAPI_SCENARIO_FAVCHLISTNUMBERING);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_AUTOMATIC_POWERDOWN,	OAPI_SCENARIO_AUTOMATIC_POWERDOWN);
	hErr |= db_param_MigrateInt(eDxSETUP_SYSTEM_PRIORITY_HDMIOUTPUT,	OAPI_SCENARIO_PRIORITY_HDMIOUTPUT);

	return hErr;
}



/*********************** End of File ******************************/

