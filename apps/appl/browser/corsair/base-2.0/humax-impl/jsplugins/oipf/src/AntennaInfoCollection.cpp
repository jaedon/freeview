#ifdef OIPF

#include "AntennaInfoCollection.h"

#include "jsplugin.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NativeConfiguration.h"

#include "oplantinfo.h"
#include "oplbuilder.h"

using namespace Ooif;

#define	__STRDUP(b, a)		{if (b) HLIB_STD_MemFree(b); if (a) b = HLIB_STD_StrDup(a); else b = NULL;}
#define	__STRCPY(b, a, len)		{strncpy(b, a, len); b[len] = 0;}
void	AntennaInfo::init(AntennaInfoHandle id)
{
	OPL_AntennaInfo_t	stAntInfo;

	classType = CLASS_ANTENNAINFO;

	if (OPL_ANTINFO_GetInfo((int)id, &stAntInfo) == ERR_OK)
	{
		memcpy(&antennaInfo, &stAntInfo, sizeof(OPL_AntennaInfo_t));
		strncpy(antennaInfo.satelliteType, stAntInfo.satelliteType, OPL_ANTINFO_NAME_LEN - 1);
		strncpy(antennaInfo.satelliteName, stAntInfo.satelliteName, OPL_ANTINFO_NAME_LEN - 1);
		antennaId = (int)id;
	}
}

AntennaInfo::AntennaInfo(AntennaInfoHandle i) : WrapperObject<AntennaInfo>(i)
{
	ENTRY;

	init(i);
}

AntennaInfo::AntennaInfo(const AntennaInfo &original) : WrapperObject<AntennaInfo>(original.identifier)
{
	ENTRY;

	init(original.identifier);
}

AntennaInfo::~AntennaInfo()
{
	ENTRY;

//1    !!!!! 절대로 Array를 Free하지 말도록 !!!!!!

/*
	if (antennaInfo.satelliteType)
		HLIB_STD_MemFree(antennaInfo.satelliteType);
	if (antennaInfo.satelliteName)
		HLIB_STD_MemFree(antennaInfo.satelliteName);
*/
}

const char	*satelliteTypeList = "[AGILA_2,AMOS_1,ANIK_F1,APSTAR_1,APSTAR_1A,APSTAR_2R,ARABSAT_2A3A,ARABSAT_2B,ASIASAT_2,ASIASAT_3S,ASTRA_1,ASTRA_2,ASTRA_3,ATLANTA_BIRD_1,BRASILSAT_B1,BRASILSAT_B2,BRASILSAT_B3,BRASILSAT_B4,EURASIASAT_1,EUTELSAT_II_F2,EUTELSAT_W1,EUTELSAT_W2,EUTELSAT_W3,EXPRESS_3A,EXPRESS_6A,GORIZONT_26,GORIZONT_33,HISPASAT,HOTBIRD,INSAT_2DT,INSAT_2E3B,INSAT_3C,INTELSAT_605,INTELSAT_704,INTELSAT_706,INTELSAT_707,INTELSAT_709,INTELSAT_805,INTELSAT_901,INTELSAT_902,INTELSAT_904,INTELSAT_906,JCSAT_3,KOREASAT_3,LMI_1,MEASAT_1,MEASAT_2,NAHUEL_1,NILESAT,NSS_703,NSS_803,NSS_806,OPTUS_B1,OPTUS_B3,PALAPA_C2,PAS_1R,PAS_2,PAS_3R_6_6B,PAS_7_10,PAS_8,PAS_9,SATMEX_5,SINOSAT_1,SIRIUS,ST_1,TELECOM_2A,TELECOM_2C,TELECOM_2D,TELKOM_1,TELSTAR_12,THAICOM_1A,THAICOM_2_3,THOR,TURKSAT_1B,TURKSAT_1C,YAMAL_102,AMC_1,AMC_3,AMC_4,AMC_7,ANIK_E2,GALAXY_10R,GALAXY_11,GALAXY_1R,GALAXY_3C,GALAXY_4R,GALAXY_9,SATCOM_C3,SATCOM_C4,SOLIDARIDAD_2,TELSTAR_4,TELSTAR_5,TELSTAR_7,EUROBIRD_1,EUROBIRD_3,EUROBIRD_9,EUTELSAT_W4,EUTELSAT_W6,EXPRESS_AM1,EXPRESS_AM22,HELLAS_SAT2,INTELSAT_10_02,INTELSAT_12,INTELSAT_7_10,INTELSAT_802,INTELSAT_907,NSS_7,ATLANTA_BIRD_2,ATLANTA_BIRD_3,THOR_3_5,TURKSAT_2A,USERANT_1,USERANT_2,USERANT_3,USERANT_4]";

int AntennaInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	//	대부분은 필요없으나 특정 모델에서는 SatelliteType을 보고 OCTO가 특별한 행동을 해야 할 경우도 있다.
	//	이름이 다르면 안되니 SATTYPE_LIST로 OCTO가 지원하는 SATTYPE을 알려준다.
	GET_STRING("SATTYPE_LIST",	satelliteTypeList,	JSP_GET_VALUE_CACHE);

	GET_NUMBER("TYPE_LNB",		eAntType_LNB, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_DISEQC",	eAntType_Diseqc, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_SCD",		eAntType_SCD, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_SMATV",	eAntType_SMATV, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TYPE_MONOBLOCK",eAntType_MonoBlock, JSP_GET_VALUE_CACHE);

	GET_NUMBER("LNBFREQ_UNIVERSAL",	-1,				JSP_GET_VALUE_CACHE);

	GET_NUMBER("LNBVOLT_STD",	0,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("LNBVOLT_HIGH",	1,					JSP_GET_VALUE_CACHE);

	GET_NUMBER("DISEQC_1_0",	0,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("DISEQC_1_1",	1,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("DISEQC_1_2",	2,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("DISEQC_1_3",	3,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("DISEQC_2_0",	4,					JSP_GET_VALUE_CACHE);

	GET_NUMBER("SW_N",			0,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("SW_A",			1,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("SW_B",			2,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("SW_C",			3,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("SW_D",			4,					JSP_GET_VALUE_CACHE);

	GET_NUMBER("TONEBURST_A",	1,					JSP_GET_VALUE_CACHE);
	GET_NUMBER("TONEBURST_B",	2,					JSP_GET_VALUE_CACHE);

	GET_NUMBER("id",				antennaId,					JSP_GET_VALUE);
	GET_NUMBER("antennaType",		antennaInfo.eAntennaType,		JSP_GET_VALUE);
	GET_STRING("satelliteType",		antennaInfo.satelliteType,		JSP_GET_VALUE);
	GET_STRING("satelliteName",		antennaInfo.satelliteName,		JSP_GET_VALUE);
	GET_NUMBER("lnbFrequency",		antennaInfo.lnb_freq,			JSP_GET_VALUE);
	GET_NUMBER("lnbVoltage",		antennaInfo.lnb_voltage,		JSP_GET_VALUE);
	GET_NUMBER("diseqcVersion",		antennaInfo.diseqc_version,		JSP_GET_VALUE);
	GET_NUMBER("switchInput",		antennaInfo.switch_input,		JSP_GET_VALUE);
	GET_BOOLEAN("enable22KHzTone",	antennaInfo.enable_22khztone,	JSP_GET_VALUE);
	GET_NUMBER("toneBurst",			antennaInfo.tone_burst,			JSP_GET_VALUE);
	GET_NUMBER("position",			antennaInfo.position,			JSP_GET_VALUE);
	GET_NUMBER("longitude",			antennaInfo.longitude,			JSP_GET_VALUE);

	GET_NUMBER("scdBandNo0",		antennaInfo.scd_band_num[0],	JSP_GET_VALUE);
	GET_NUMBER("scdBandNo1",		antennaInfo.scd_band_num[1],	JSP_GET_VALUE);
	GET_NUMBER("scdBandNo2",		antennaInfo.scd_band_num[2],	JSP_GET_VALUE);
	GET_NUMBER("scdBandNo3",		antennaInfo.scd_band_num[3],	JSP_GET_VALUE);
	GET_NUMBER("scdBandFreq0",		antennaInfo.scd_band_freq[0],	JSP_GET_VALUE);
	GET_NUMBER("scdBandFreq1",		antennaInfo.scd_band_freq[1],	JSP_GET_VALUE);
	GET_NUMBER("scdBandFreq2",		antennaInfo.scd_band_freq[2],	JSP_GET_VALUE);
	GET_NUMBER("scdBandFreq3",		antennaInfo.scd_band_freq[3],	JSP_GET_VALUE);

	GET_FUNCTION(commit, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(refresh, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int AntennaInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	SET_WITH_ACTION("antennaType",				(antennaInfo.eAntennaType = (OPL_AntennaType_e)NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("satelliteType",			(__STRCPY(antennaInfo.satelliteType, STRING_VALUE(value[0]), OPL_ANTINFO_NAME_LEN - 1)));
	SET_WITH_ACTION("satelliteName",			(__STRCPY(antennaInfo.satelliteName, STRING_VALUE(value[0]), OPL_ANTINFO_NAME_LEN - 1)));

	SET_WITH_ACTION("lnbFrequency",				(antennaInfo.lnb_freq = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("lnbVoltage",				(antennaInfo.lnb_voltage = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("diseqcVersion",			(antennaInfo.diseqc_version = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("switchInput",				(antennaInfo.switch_input = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("enable22KHzTone",			(antennaInfo.enable_22khztone = BOOLEAN_VALUE(value[0])));
	SET_WITH_ACTION("toneBurst",				(antennaInfo.tone_burst = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("position",					(antennaInfo.position = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("longitude",				(antennaInfo.longitude = NUMBER_VALUE(value[0])));

	SET_WITH_ACTION("scdBandNo0",				(antennaInfo.scd_band_num[0] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandNo1",				(antennaInfo.scd_band_num[1] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandNo2",				(antennaInfo.scd_band_num[2] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandNo3",				(antennaInfo.scd_band_num[3] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandFreq0",				(antennaInfo.scd_band_freq[0] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandFreq1",				(antennaInfo.scd_band_freq[1] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandFreq2",				(antennaInfo.scd_band_freq[2] = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("scdBandFreq3",				(antennaInfo.scd_band_freq[3] = NUMBER_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

int	AntennaInfo::commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	AntennaInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, AntennaInfo);

	if (OPL_ANTINFO_SetInfo((int)instance->getIdentifier(), &instance->antennaInfo) == ERR_OK)
	{
		RETURN_BOOLEAN(1, JSP_CALL_VALUE);
	}

	RETURN_BOOLEAN(0, JSP_CALL_VALUE);
}

int	AntennaInfo::refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	AntennaInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, AntennaInfo);
	OPL_AntennaInfo_t	stAntInfo;

	if (OPL_ANTINFO_GetInfo((int)instance->getIdentifier(), &stAntInfo) == ERR_OK)
	{
		memcpy(&instance->antennaInfo, &stAntInfo, sizeof(OPL_AntennaInfo_t));

		//1    !!!!! 절대로 Array를 Free하지 말도록 !!!!!!
		/*
		if (instance->antennaInfo.satelliteName)
			HLIB_STD_MemFree(instance->antennaInfo.satelliteName);
		if (instance->antennaInfo.satelliteType)
			HLIB_STD_MemFree(instance->antennaInfo.satelliteType);
		*/
		__STRCPY(instance->antennaInfo.satelliteType, stAntInfo.satelliteType, OPL_ANTINFO_NAME_LEN - 1);
		__STRCPY(instance->antennaInfo.satelliteName, stAntInfo.satelliteName, OPL_ANTINFO_NAME_LEN - 1);
//		instance->antennaInfo.satelliteType = HLIB_STD_StrDup(stAntInfo.satelliteType);
//		instance->antennaInfo.satelliteName = HLIB_STD_StrDup(stAntInfo.satelliteName);

		RETURN_BOOLEAN(1, JSP_CALL_VALUE);
	}

	RETURN_BOOLEAN(0, JSP_CALL_VALUE);
}

void AntennaInfo::releaseNative(AntennaInfoHandle id)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
AntennaInfoCollection::AntennaInfoCollection()
{
	ENTRY;
	classType = CLASS_ANTENNAINFOCOLLECTION;

	int	count;
	void *id;

	count = OPL_ANTINFO_Count();
	for (int i = 0; i < count; i++)
	{
		id = (void *)OPL_ANTINFO_GetId(i);
		if (id == NULL)
			continue;
		addItem(new AntennaInfo(id));
	}
}

AntennaInfoCollection::~AntennaInfoCollection()
{
	ENTRY;
}

int AntennaInfoCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif
	int ret = Collection<AntennaInfo>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(commit, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(refresh, obj, "", JSP_GET_VALUE_CACHE);
 	GET_FUNCTION(createAntennaInfo, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getAntennaInfo, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeAll, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getConnectionType, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setConnectionType, obj, "n", JSP_GET_VALUE_CACHE);

	//	Antenna Collection에서 DiSEqC 1.2, 1.3 command가 동작하게??
	GET_NUMBER("UNDEFINED",	0, JSP_GET_VALUE_CACHE);
	GET_NUMBER("EAST",		1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("WEST",		2, JSP_GET_VALUE_CACHE);

	GET_NUMBER("STEP",		1, JSP_GET_VALUE_CACHE);
	GET_NUMBER("TIMEOUT",	2, JSP_GET_VALUE_CACHE);
	GET_NUMBER("CONTINUOUS",3, JSP_GET_VALUE_CACHE);

	GET_FUNCTION(gotoPosition, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(storePosition, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recalculate, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setLimit, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(enableLimit, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(disableLimit, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(gotoDegree, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(gotoSatellite, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(drive, obj, "nnn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getMyLatitude, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getMyLongitude, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setMyLatitude, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setMyLongitude, obj, "n", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

//static
int AntennaInfoCollection::commit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OPL_ANTINFO_Sync();

	RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

//static
int AntennaInfoCollection::createAntennaInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	AntennaInfoCollection	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, AntennaInfoCollection);
	AntennaInfoHandle		handle;

	handle = (AntennaInfoHandle)OPL_ANTINFO_New();
#if	1
	if (handle)
	{
		instance->addItem(new AntennaInfo(handle));
	}
	RETURN_NUMBER((int)handle, JSP_CALL_VALUE);
#else
	if (handle)
	{
		instance->addItem(new AntennaInfo(handle));
		RETURN_OBJECT(this_obj, new AntennaInfo(handle), result, JSP_CALL_VALUE);
	}

	return JSP_CALL_ERROR;
#endif
}

int	AntennaInfoCollection::getAntennaInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		antId;
	AntennaInfoHandle	handle;

	CHECK_ARGUMENT_COUNT(1);
	antId = NUMBER_VALUE(argv[0]);
	handle = (AntennaInfoHandle)(antId);

	RETURN_OBJECT(this_obj, new AntennaInfo(handle), result, JSP_CALL_VALUE);
}

int	AntennaInfoCollection::getConnectionType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		connectionType;

	CHECK_ARGUMENT_COUNT(0);

	connectionType = OPL_ANTINFO_GetConnectionType();

	RETURN_NUMBER(connectionType, JSP_CALL_VALUE);
}

int	AntennaInfoCollection::setConnectionType(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);
	n = NUMBER_VALUE(argv[0]);

	OPL_ANTINFO_SetConnectionType(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    int		n;

    CHECK_ARGUMENT_COUNT(0);
    AntennaInfoCollection	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, AntennaInfoCollection);


    n = instance->getSize();
    while (n > 0)
    {
    	instance->internalRemoveIndex(n - 1);
    	n = instance->getSize();
    }
    OPL_ANTINFO_RemoveAll();
    OPL_ANTINFO_Refresh();

    int	count;
    void *id;
    count = OPL_ANTINFO_Count();
    for (int i = 0; i < count; i++)
    {
    	id = (void*)OPL_ANTINFO_GetId(i);
    	if (id == NULL)
    		continue;
    	instance->addItem(new AntennaInfo(id));
    }

#if	0
    n = instance->getSize();
    while (n > 0)
    {
    	OPL_AntennaInfo_t	stAntInfo;

    	antInfo = instance->internalGetItem(n - 1);

    	OPL_ANTINFO_GetInfo((int)antInfo->getIdentifier(), &stAntInfo);

    	if (antInfo->antennaInfo.satelliteName)
    		HLIB_STD_MemFree(antInfo->antennaInfo.satelliteName);
    	if (antInfo->antennaInfo.satelliteType)
    		HLIB_STD_MemFree(antInfo->antennaInfo.satelliteType);

    	memcpy(&antInfo->antennaInfo, &stAntInfo, sizeof(OPL_AntennaInfo_t));
    	antInfo->antennaInfo.satelliteType = HLIB_STD_StrDup(stAntInfo.satelliteType);
    	antInfo->antennaInfo.satelliteName = HLIB_STD_StrDup(stAntInfo.satelliteName);

    	n--;
    }
#endif

    RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

int	AntennaInfoCollection::removeAll(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    int		n;

    CHECK_ARGUMENT_COUNT(0);
    AntennaInfoCollection	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, AntennaInfoCollection);

    n = instance->getSize();
    while (n > 0)
    {
    	instance->internalRemoveIndex(n - 1);
    	n = instance->getSize();
    }
    OPL_ANTINFO_RemoveAll();

    RETURN_BOOLEAN(1, JSP_CALL_VALUE);
}

int	AntennaInfoCollection::gotoPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_GotoPosition(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::storePosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_StorePosition(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::recalculate(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_Recalculate(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OPL_Builder_Motor_Stop();

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::setLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_SetLimit(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::enableLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OPL_Builder_Motor_EnableLimit();

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::disableLimit(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	OPL_Builder_Motor_DisableLimit();

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::gotoDegree(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_GotoDegree(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::gotoSatellite(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		n;

	CHECK_ARGUMENT_COUNT(1);

	n = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_GotoSatellite(n);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::drive(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		direction, mode, step;

	CHECK_ARGUMENT_COUNT(3);

	direction	= NUMBER_VALUE(argv[0]);
	mode		= NUMBER_VALUE(argv[1]);
	step		= NUMBER_VALUE(argv[2]);

	OPL_Builder_Motor_Move(direction, mode, step);

	return JSP_CALL_NO_VALUE;
}

int	AntennaInfoCollection::getMyLatitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		mylatitude;

	CHECK_ARGUMENT_COUNT(0);

	mylatitude = OPL_Builder_Motor_GetMyLatitude();

	RETURN_NUMBER(mylatitude, JSP_CALL_VALUE);
}


 int AntennaInfoCollection::getMyLongitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
 {
	 int	 	mylongitude;

	 CHECK_ARGUMENT_COUNT(0);

	 mylongitude = OPL_Builder_Motor_GetMyLongitude();

	 RETURN_NUMBER(mylongitude, JSP_CALL_VALUE);
 }

int	AntennaInfoCollection::setMyLatitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		mylatitude;

	CHECK_ARGUMENT_COUNT(1);

	mylatitude = NUMBER_VALUE(argv[0]);

	OPL_Builder_Motor_SetMyLatitude(mylatitude);

	return JSP_CALL_NO_VALUE;
}

 int AntennaInfoCollection::setMyLongitude(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
 {
	 int		mylongitude;

	 CHECK_ARGUMENT_COUNT(1);

	 mylongitude = NUMBER_VALUE(argv[0]);

	 OPL_Builder_Motor_SetMyLongitude(mylongitude);

	 return JSP_CALL_NO_VALUE;
 }

 #endif // OIPF
