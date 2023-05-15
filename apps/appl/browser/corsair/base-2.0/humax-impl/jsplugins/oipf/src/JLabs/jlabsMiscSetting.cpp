#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "jlabsMiscSetting.h"
#include "NativeJlabsMiscSetting.h"
#include "NativeJlabsChannel.h"
using namespace Ooif;

int jlabsMiscSetting::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("JLABS_DTT_NETWORK", JLABS_DTT_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_BS_NETWORK", JLABS_BS_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CATV_NETWORK", JLABS_CATV_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_ALL_NETWORK", JLABS_ALL_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS1_NETWORK", JLABS_CS1_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS2_NETWORK", JLABS_CS2_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_JCHITS_NETWORK", JLABS_JCHITS_NETWORK, JSP_GET_VALUE_CACHE);

	NATIVE_GET_STRING("postCode", native_jlabsMiscSettingGetPostCode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("localArea", native_jlabsMiscSettingGetLocalArea(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("tvAspectRatio", native_jlabsMiscSettingGetTvAspectRatio(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("dualMono", native_jlabsMiscSettingGetSoundMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("spdifMode", native_jlabsMiscSettingGetSpdifMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("subtitleMode", native_jlabsMiscSettingGetSubtitleMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("superimposeMode", native_jlabsMiscSettingGetSuperImposeMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("setupConfigurationLaunchMode", native_jlabsMiscSettingGetSetupConfigurationLaunchMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("numberOfTuners", native_jlabsMiscSettingGetNumberOfTuners(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("displayMode3D", native_jlabsMiscSettingGetDisplayMode3D(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("ThreeDMode", native_jlabsMiscSettingGetThreeDMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("autoSoftwareDownloadEnable", native_jlabsMiscSettingGetAutoSoftwareDownloadEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("channelMaskEnable", native_jlabsMiscSettingGetChannelMaskEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("seamlessEnable", native_jlabsMiscSettingGetSeamlessEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("autoPPVPurchaseEnable", native_jlabsMiscSettingGetAutoPPVPurchaseEnable(&RETVAL), JSP_GET_VALUE);
	/* read only */
	NATIVE_GET_NUMBER("lastChannelNetworkTypeName", native_jlabsMiscSettingGetLastChannelNetworkTypeName(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastChannelThreeDigitNumber", native_jlabsMiscSettingGetLastChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	/* function */
	GET_FUNCTION(get, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(set, obj, "ss", JSP_GET_VALUE_CACHE);
#if defined(JLABS_JCOM)
	/* string */
	NATIVE_GET_STRING("limitPerPPV", native_jlabsMiscSettingGetLimitPerPPV(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("limitPayPerMonth", native_jlabsMiscSettingGetLimitPerPPM(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("hdmiAudioMode", native_jlabsMiscSettingGetHdmiAudioMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("friendlyName", native_jlabsMiscSettingGetFriendlyName(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("naviURL", native_jlabsMiscSettingGetNaviUrl(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("interacTVURL", native_jlabsMiscSettingGetInteracTVURL(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("internetBrowserURL", native_jlabsMiscSettingGetInternetBrowserURL(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("vodHomeURL", native_jlabsMiscSettingGetVodHomeURL(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("acsURL", native_jlabsMiscSettingGetAcsUrl(&RETVAL), JSP_GET_VALUE);
	/* number  */
	NATIVE_GET_NUMBER("pinUnlockExpiration", native_jlabsMiscSettingGetPinUnlockExpiration(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("tuningRange", native_jlabsMiscSettingGetTuningRange(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("timebarDisplayMode", native_jlabsMiscSettingGetTimebarDisplayMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("hdmiCECMode", native_jlabsMiscSettingGetHdmiCecMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("amPermissionLevel", native_jlabsMiscSettingGetAMPermissionLevel(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("hdmiResolutionMode", native_jlabsMiscSettingGetHdmiResolutionMode(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("standbyMode", native_jlabsMiscSettingGetStandbyMode(&RETVAL), JSP_GET_VALUE);

	/* boolean */
	NATIVE_GET_BOOLEAN("bmlInfoDisplayEnable", native_jlabsMiscSettingGetBmlInfoDisplayEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("iPlateDisplayEnable", native_jlabsMiscSettingGetIPlateDisplayEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("frontDisplayEnable", native_jlabsMiscSettingGetFrontDisplayEnable(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isDMSPublic", native_jlabsMiscSettingGetIsDMSPublic(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isDMSDeliver", native_jlabsMiscSettingGetIsDMSDeliver(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isIPDubbing", native_jlabsMiscSettingGetIsIPDubbing(&RETVAL), JSP_GET_VALUE);
	/* read only */
	NATIVE_GET_NUMBER("lastDttChannelThreeDigitNumber", native_jlabsMiscSettingGetLastDTTChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastDttChannelBranchNumber", native_jlabsMiscSettingGetLastDttChannelBranchNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastBsChannelThreeDigitNumber", native_jlabsMiscSettingGetLastBSChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastCatvChannelThreeDigitNumber", native_jlabsMiscSettingGetLastCATVChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastCs1ChannelThreeDigitNumber", native_jlabsMiscSettingGetLastCS1ChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastCs2ChannelThreeDigitNumber", native_jlabsMiscSettingGetLastCS2ChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastJchitsChannelThreeDigitNumber", native_jlabsMiscSettingGetLastJCHITSChannelThreeDigitNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastChannelBranchNumber", native_jlabsMiscSettingGetLastChannelBranchNumber(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("channelMaskPattern", native_jlabsMiscSettingGetChannelMaskPattern(&RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isOutputHD", native_jlabsMiscSettingGetIsOutputHD(&RETVAL), JSP_GET_VALUE);
#endif
	return JSP_GET_NOTFOUND;
}

int jlabsMiscSetting::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	NATIVE_SET("postCode", native_jlabsMiscSettingSetPostCode(STRING_VALUE(value[0])));
	NATIVE_SET("localArea", native_jlabsMiscSettingSetLocalArea(STRING_VALUE(value[0])));
	NATIVE_SET("tvAspectRatio", native_jlabsMiscSettingSetTvAspectRatio(STRING_VALUE(value[0])));
	NATIVE_SET("dualMono", native_jlabsMiscSettingSetSoundMode(STRING_VALUE(value[0])));
	NATIVE_SET("spdifMode", native_jlabsMiscSettingSetSpdifMode(STRING_VALUE(value[0])));

	NATIVE_SET("subtitleMode", native_jlabsMiscSettingSetSubtitleMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("superimposeMode", native_jlabsMiscSettingSetSuperImposeMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("setupConfigurationLaunchMode", native_jlabsMiscSettingSetSetupConfigurationLaunchMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("numberOfTuners", native_jlabsMiscSettingSetNumberOfTuners(NUMBER_VALUE(value[0])));
	NATIVE_SET("displayMode3D", native_jlabsMiscSettingSetDisplyMode3D(NUMBER_VALUE(value[0])));

	NATIVE_SET("ThreeDMode", native_jlabsMiscSettingSetThreeDMode(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("autoSoftwareDownloadEnable", native_jlabsMiscSettingSetAutoSoftwareDownloadEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("channelMaskEnable", native_jlabsMiscSettingSetChannelMaskEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("seamlessEnable", native_jlabsMiscSettingSetSeamlessEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("autoPPVPurchaseEnable", native_jlabsMiscSettingSetAutoPPVPurchaseEnable(BOOLEAN_VALUE(value[0])));
#if defined(JLABS_JCOM)
	/* string */
	NATIVE_SET("limitPerPPV", native_jlabsMiscSettingSetLimitPerPPV(STRING_VALUE(value[0])));
	NATIVE_SET("limitPayPerMonth", native_jlabsMiscSettingSetLimitPerPPM(STRING_VALUE(value[0])));
	NATIVE_SET("hdmiAudioMode", native_jlabsMiscSettingSetHdmiAudioMode(STRING_VALUE(value[0])));
	NATIVE_SET("friendlyName", native_jlabsMiscSettingSetFriendlyName(STRING_VALUE(value[0])));
	NATIVE_SET("naviUrl", native_jlabsMiscSettingSetNaviUrl(STRING_VALUE(value[0])));
	NATIVE_SET("interacTVURL", native_jlabsMiscSettingSetInteracTVUrl(STRING_VALUE(value[0])));
	NATIVE_SET("internetBrowserURL", native_jlabsMiscSettingSetInternetBrowserUrl(STRING_VALUE(value[0])));
	NATIVE_SET("vodHomeURL", native_jlabsMiscSettingSetVodHomeUrl(STRING_VALUE(value[0])));
	NATIVE_SET("acsURL", native_jlabsMiscSettingSetAcsUrl(STRING_VALUE(value[0])));
	/* number  */
	NATIVE_SET("pinUnlockExpiration", native_jlabsMiscSettingSetPinUnlockExpiration(NUMBER_VALUE(value[0])));
	NATIVE_SET("tuningRange", native_jlabsMiscSettingSetTuningRange(NUMBER_VALUE(value[0])));
	NATIVE_SET("hdmiCECMode", native_jlabsMiscSettingSetHdmiCecMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("amPermissionLevel", native_jlabsMiscSettingSetAmPermissionLevel(NUMBER_VALUE(value[0])));
	NATIVE_SET("timebarDisplayMode", native_jlabsMiscSettingSetTimebarDisplayMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("hdmiResolutionMode", native_jlabsMiscSettingSetHdmiResolutionMode(NUMBER_VALUE(value[0])));
	NATIVE_SET("standbyMode", native_jlabsMiscSettingSetStandbyMode(NUMBER_VALUE(value[0])));

	/* boolean */
	NATIVE_SET("bmlInfoDisplayEnable", native_jlabsMiscSettingSetBmlInfoDisplayEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("iPlateDisplayEnable", native_jlabsMiscSettingSetIPlateDisplayEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("frontDisplayEnable", native_jlabsMiscSettingSetFrontDisplayEnable(BOOLEAN_VALUE(value[0])));
	NATIVE_SET("isDMSPublic", native_jlabsMiscSettingSetIsDMSPublic(BOOLEAN_VALUE(value[0])));
#endif
	return JSP_PUT_NOTFOUND;
}

//static
int jlabsMiscSetting::get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(1);

	const char *retval;
	NATIVE_CALL(native_jlabsMiscSettingGet(STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int jlabsMiscSetting::set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(2);

	NATIVE_CALL(native_jlabsMiscSettingSet(STRING_VALUE(argv[0]), STRING_VALUE(argv[1])));
	return JSP_CALL_NO_VALUE;
}


#endif // #if defined OIPF && defined JLABS
