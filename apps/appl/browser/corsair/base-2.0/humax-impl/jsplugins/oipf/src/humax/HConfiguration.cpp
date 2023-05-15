// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "NativeHConfiguration.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "Channel.h"
#include "Programme.h"
#include "HConfiguration.h"

using namespace Ooif;

int HConfiguration::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif

#if defined(HMX_WEBUI)
	GET_NUMBER("TYPE_SUBTITLE_FONT_FIXED", SUBTITLE_FONT_TYPE_FIXED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_SUBTITLE_FONT_VARIABLE", SUBTITLE_FONT_TYPE_VARIABLE, JSP_GET_VALUE_CACHE);

	GET_NUMBER("TYPE_DB_RESTORE_FACTORY", DB_RESTORE_FACTORY, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_DB_RESTORE_LAST", DB_RESTORE_LAST, JSP_GET_VALUE_CACHE);

	GET_NUMBER("TYPE_STANDBY_NORMAL", STANDBY_NORMAL, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_STANDBY_POWERSAVING", STANDBY_POWER_SAVING, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_STANDBY_WAKEUPONLAN", STANDBY_WAKEUP_ON_LAN, JSP_GET_VALUE_CACHE);
#endif

	NATIVE_GET_STRING("preferredAudioLanguage", native_HConfigGetPreferredAudioLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("preferredSecondAudioLanguage", native_HConfigGetPreferredSecondAudioLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("preferredSubtitleLanguage", native_HConfigGetPreferredSubtitleLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("preferredSecondSubtitleLanguage", native_HConfigGetPreferredSecondSubtitleLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("countryId", native_HConfigGetCountryId(&RETVAL), JSP_GET_VALUE);
#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
	NATIVE_GET_NUMBER("regionId", native_HConfigGetRegionId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isPINEntryLocked", native_HConfigGetIsPINEntryLocked(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrPolicy", native_HConfigGetPvrPolicy(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSaveEpisodes", native_HConfigGetPvrSaveEpisodes(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrSaveDays", native_HConfigGetPvrSaveDays(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrStartPadding", native_HConfigGetPvrStartPadding(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("pvrEndPadding", native_HConfigGetPvrEndPadding(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("preferredTimeShiftMode", native_HConfigGetPreferredTimeShiftMode(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(setParentalControlPIN, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setParentalControlPINEnable, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getParentalControlPINEnable, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unlockWithParentalControlPIN, obj, "son", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(verifyParentalControlPIN, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBlockUnrated, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getBlockUnrated, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getText, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setText, obj, "ss", JSP_GET_VALUE_CACHE);
#endif // OIPF || OPT_HBBTV_EXT_CONFIG
#if defined(HMX_WEBUI)
	NATIVE_GET_NUMBER("operatorId", native_HConfigGetOperatorId(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("preferredMenuLanguage", native_HConfigGetPreferredMenuLanguage(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("standbyPowerMode", native_HConfigGetStandByPowerMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("standbySleepMode", native_HConfigGetStandBySleepMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("standbyBootMode", native_HConfigGetStandByBootMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("standbyWifiPower", native_HConfigGetStandByWifiPower(&RETVAL), JSP_GET_VALUE);	
	NATIVE_GET_BOOLEAN("automaticStandby", native_HConfigGetAutomaticStandby(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("autoDelete", native_HConfigGetAutoDelete(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("audioBleepEnabled", native_HConfigGetAudioBleep(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("subtitleFontType", native_HConfigGetSubtitleFontType(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("infoDisplayTimeout", native_HConfigGetInfoDisplayTimeout(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("timeShiftEnabled", native_HConfigGetTimeshiftEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("favChListNumberingMode", native_HConfigGetFavChListNumberingMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("descrambleOnOff", native_HConfigGetDescrambleOnOff(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("skipForwardTime", native_HConfigGetSkipForwardTime(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("instantReplayTime", native_HConfigGetInstantReplayTime(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("firstTimeBoot", native_HConfigGetFirstTimeBoot(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("channelsetupPin", native_HConfigGetChannelSetupPIN(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("guidancepolicy", native_HConfigGetGuidancePolicy(&RETVAL),JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hdmiCecMode", native_HConfigGetHdmiCecMode(&RETVAL),JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hdmiCecVolumeCtrlMode", native_HConfigGetHdmiCecVolumeCtrlMode(&RETVAL),JSP_GET_VALUE);
	NATIVE_GET_NUMBER("targetStorage", native_HConfigGetTargetStorage(&RETVAL),JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hbbTvEnbled", native_HConfigGetHbbTvEnabled(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("batupdatemode", native_HConfigGetBatUpdateMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("nitlcnupdatemode", native_HConfigGetNitLcnUpdateMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastsvcgroup", native_HConfigGetLastServiceGroup(&RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setField, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(doFactoryDefault, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(doDBRestore, obj, "sn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getCamNameBySlot, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setServerActivationTime, obj, "nnnb", JSP_GET_VALUE_CACHE);
//#if defined(CONFIG_OP_SES)
	GET_FUNCTION(getSatRecordConfirmCode, obj, "s", JSP_GET_VALUE_CACHE);
//#endif


//#endif
#endif

	return JSP_GET_NOTFOUND;
}

int HConfiguration::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
	NATIVE_SET("preferredAudioLanguage", native_HConfigSetPrefferedAudioLangue(STRING_VALUE(value[0])));
	NATIVE_SET("preferredSecondAudioLanguage", native_HConfigSetPrefferedSecondAudioLangue(STRING_VALUE(value[0])));
	NATIVE_SET("preferredSubtitleLanguage", native_HConfigSetPreferredSubtitleLanguage(STRING_VALUE(value[0])));
	NATIVE_SET("preferredSecondSubtitleLanguage", native_HConfigSetPreferredSecondSubtitleLanguage(STRING_VALUE(value[0])));
	NATIVE_SET("countryId", native_HConfigSetCountryId(STRING_VALUE(value[0])));
	NATIVE_SET("regionId", native_HConfigSetRegionId(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrPolicy", native_HConfigSetPvrPolicy(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrSaveEpisodes", native_HConfigSetPvrSaveEpisodes(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrSaveDays", native_HConfigSetPvrSaveDays(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrStartPadding", native_HConfigSetPvrStartPadding(NUMBER_VALUE(value[0])));
	NATIVE_SET("pvrEndPadding", native_HConfigSetPvrEndPadding(NUMBER_VALUE(value[0])));
	NATIVE_SET("prefeeredTimeShiftMode", native_HConfigSetPreferredTimeShiftMode(NUMBER_VALUE(value[0])));
#endif // OIPF || OPT_HBBTV_EXT_CONFIG

#if defined(HMX_WEBUI)
	NATIVE_SET("operatorId", native_HConfigSetOperatorId(NUMBER_VALUE(value[0])));
	NATIVE_SET("preferredMenuLanguage", native_HConfigSetPreferredMenuLanguage(STRING_VALUE(value[0])));
	NATIVE_SET("standbyPowerMode", native_HConfigSetStandByPowerMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("standbySleepMode", native_HConfigSetStandBySleepMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("standbyBootMode", native_HConfigSetStandByBootMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("standbyWifiPower", native_HConfigSetStandByWifiPower(NUMBER_VALUE(value[0])));
	NATIVE_SET("automaticStandby", native_HConfigSetAutomaticStandby(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("autoDelete", native_HConfigSetAutoDelete(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("audioBleepEnabled", native_HConfigSetAudioBleep(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("subtitleFontType", native_HConfigSetSubtitleFontType(NUMBER_VALUE(value[0])));
	NATIVE_SET("infoDisplayTimeout", native_HConfigSetInfoDisplayTimeout(NUMBER_VALUE(value[0])));
	NATIVE_SET("descrambleOnOff", native_HConfigSetDescrambleOnOff(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("timeShiftEnabled", native_HConfigSetTimeshiftEnabled(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("favChListNumberingMode", native_HConfigSetFavChListNumberingMode(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("skipForwardTime", native_HConfigSetSkipForwardTime(NUMBER_VALUE(value[0])));
	NATIVE_SET("instantReplayTime", native_HConfigSetInstantReplayTime(NUMBER_VALUE(value[0])));
	NATIVE_SET("firstTimeBoot", native_HConfigSetFirstTimeBoot(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("channelsetupPin", native_HConfigSetChannelSetupPIN(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("guidancepolicy", native_HConfigSetGuidancePolicy(NUMBER_VALUE(value[0])));
	NATIVE_SET("hdmiCecMode", native_HConfigSetHdmiCecMode(BOOLEAN_VALUE(value[0])));
        NATIVE_SET("hdmiCecVolumeCtrlMode", native_HConfigSetHdmiCecVolumeCtrlMode(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("targetStorage", native_HConfigSetTargetStorage(NUMBER_VALUE(value[0])));
	NATIVE_SET("hbbTvEnbled", native_HConfigSetHbbTvEnabled(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("batupdatemode", native_HConfigSetBatUpdateMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("nitlcnupdatemode", native_HConfigSetNitLcnUpdateMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("lastsvcgroup", native_HConfigSetLastServiceGroup(NUMBER_VALUE(value[0])));

#endif

#if defined(CONFIG_OP_SES)
	NATIVE_SET("SESSatrecordConfirmcode", native_HConfigSetSESSatrecordConfirmcode(&RETVAL), JSP_GET_VALUE);
#endif

	return JSP_PUT_NOTFOUND;
}

#if defined OIPF || defined OPT_HBBTV_EXT_CONFIG
//static
int HConfiguration::setParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_HConfigSetParentalControlPIN(STRING_VALUE(argv[0]), STRING_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::setParentalControlPINEnable(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_HConfigSetParentalControlPINEnable(STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::getParentalControlPINEnable(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_HConfigGetParentalControlPINEnable(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::unlockWithParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);

	ObjectInstance *targetObj = OBJECT_VALUE(argv[1], ObjectInstance);
	if(!targetObj ||
	   (targetObj->getType() != CLASS_CHANNEL && targetObj->getType() != CLASS_PROGRAMME)) {
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	void * target;
	if (targetObj->getType() == CLASS_CHANNEL)
		target = static_cast<Channel*>(targetObj)->getIdentifier();
	else // if (targetObj->getType() == CLASS_PROGRAMME)
		target = static_cast<Programme*>(targetObj)->getIdentifier();
	const char *pcPIN = STRING_VALUE(argv[0]);
	OOIFNumber duration = NUMBER_VALUE(argv[2]);
	OOIFNumber retval;
	NATIVE_CALL(native_HConfigUnlockWithParentalControlPIN(pcPIN, target, duration, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::verifyParentalControlPIN(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber retval;
	NATIVE_CALL(native_HConfigVerifyParentalControlPIN(STRING_VALUE(argv[0]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::setBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	OOIFNumber retval;
	NATIVE_CALL(native_HConfigSetBlockUnrated(STRING_VALUE(argv[0]), BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::getBlockUnrated(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean retval;
	NATIVE_CALL(native_HConfigGetBlockUnrated(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::getText(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_HConfigGetText(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::setText(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_HConfigSetText(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

#endif // OIPF || OPT_HBBTV_EXT_CONFIG

#if defined(HMX_WEBUI)
//static
int HConfiguration::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_HConfigGetField(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int HConfiguration::setField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_HConfigSetField(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}

int HConfiguration::doFactoryDefault(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
//	CHECK_ARGUMENT_COUNT(1);
//	CHECK_ARGUMENT_COUNT(0);

	if (argc == 0)
		NATIVE_CALL(native_HConfigDoFactoryDefault(NULL));
	else if (argc == 1)
		NATIVE_CALL(native_HConfigDoFactoryDefault(STRING_VALUE(argv[0])));

	return JSP_CALL_NO_VALUE;
}

//static
int HConfiguration::doDBRestore(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	OOIFNumber retval;

	NATIVE_CALL(native_HConfigDoDbRestore(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int HConfiguration::getCamNameBySlot(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	const char *retval;
	NATIVE_CALL(native_HConfigGetCamNameBySlot(NUMBER_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int HConfiguration::setServerActivationTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(4);

	NATIVE_CALL(native_HConfigSetServerActivationTime(NUMBER_VALUE(argv[0]),NUMBER_VALUE(argv[1]),NUMBER_VALUE(argv[2]),BOOLEAN_VALUE(argv[3])));

	return JSP_CALL_NO_VALUE;
}

int HConfiguration::getSatRecordConfirmCode(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);

	const char *retval;
	NATIVE_CALL(native_HConfigGetSatRecordConfirmCode(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


#endif
