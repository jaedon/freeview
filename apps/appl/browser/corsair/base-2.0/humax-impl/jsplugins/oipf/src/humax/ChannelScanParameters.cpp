#ifdef OIPF
#include "humax/HDataTypes.h"
#include "humax/ChannelScanParameters.h"
#include "NativeChannel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

#if defined JLABS
#include "NativeJlabsLocalSystem.h"
#endif

using namespace Ooif;

#define	__STRDUP(b, a)		{if (b) HLIB_STD_MemFree(b); if (a) b = HLIB_STD_StrDup(a); else b = NULL;}
__inline char *	__STRNDUP (const char *str, int size)
{
	int	len;
	char *	dup;

	if (str)
	{
		len = strlen(str);
        if ( len > size)
            len = size;
        dup = (char *)HLIB_STD_MemDup(str, len+1);
		return dup;
	}

	return NULL;
}
DVBTChannelScanParameters::DVBTChannelScanParameters()
{
	ENTRY;

	startFrequency = 0;
	endFrequency = 0;
	raster = 0;
	modulationModes = 0;
	transmission = (char *)HLIB_STD_MemCalloc(2); // 2 byte null intialization
	ofdm = (char *)HLIB_STD_MemCalloc(2); // 2 byte null intialization
	bandwidth = (char *)HLIB_STD_MemCalloc(2); // 2 byte null intialization
}

DVBTChannelScanParameters::DVBTChannelScanParameters(const DVBTChannelScanParameters &original)
{
	ENTRY;

	startFrequency = original.startFrequency;
	endFrequency   = original.endFrequency;
	modulationModes= original.modulationModes;
	raster   = original.raster;

	if (original.transmission == NULL)
		transmission = NULL;
	else
		transmission	 = HLIB_STD_StrDup(original.transmission);
	if (original.ofdm == NULL)
		ofdm = NULL;
	else
		ofdm     = HLIB_STD_StrDup(original.ofdm);
	if (original.bandwidth == NULL)
		bandwidth = NULL;
	else
		bandwidth       = HLIB_STD_StrDup(original.bandwidth);

}

DVBTChannelScanParameters::DVBTChannelScanParameters(humax::DVBT_TunedData *_DVBTtunedData)
{
	startFrequency = _DVBTtunedData->startFreq;
	endFrequency = _DVBTtunedData->endFreq;
	raster = _DVBTtunedData->raster;
	modulationModes = _DVBTtunedData->modulationModes;

	if (_DVBTtunedData->transmission == NULL) {
		transmission = NULL;
	} else {
		transmission = __STRNDUP(_DVBTtunedData->transmission,(MAX_DATAS_LEN-1));
	}
	
	if (_DVBTtunedData->ofdm == NULL) {
		ofdm = NULL;
	} else {
		ofdm = __STRNDUP(_DVBTtunedData->ofdm,(MAX_DATAS_LEN-1));
	}

	if (_DVBTtunedData->bandwidth == NULL)
	{
		bandwidth = NULL;
	} else {
		bandwidth = __STRNDUP(_DVBTtunedData->bandwidth,(MAX_DATAS_LEN-1));
	}
}

DVBTChannelScanParameters::~DVBTChannelScanParameters()
{
	ENTRY;

	if (transmission)
		HLIB_STD_MemFree(transmission);
	if (ofdm)
		HLIB_STD_MemFree(ofdm);
	if (bandwidth)
		HLIB_STD_MemFree(bandwidth);
}

int	DVBTChannelScanParameters::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER ("startFrequency",	startFrequency,		JSP_GET_VALUE);
	GET_NUMBER ("endFrequency",		endFrequency,		JSP_GET_VALUE);
	GET_NUMBER ("raster",			raster,				JSP_GET_VALUE);
	GET_NUMBER ("modulationModes",	modulationModes,	JSP_GET_VALUE);

	GET_STRING ("transmission",		transmission,		JSP_GET_VALUE);
	GET_STRING ("ofdm",				ofdm,				JSP_GET_VALUE);
	GET_STRING ("bandwidth",		bandwidth,			JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int	DVBTChannelScanParameters::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_WITH_ACTION("startFrequency",	(startFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("endFrequency",		(endFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("raster",			(raster = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("modulationModes",	(modulationModes = NUMBER_VALUE(value[0])));

	SET_WITH_ACTION("transmission",		(__STRDUP(transmission, STRING_VALUE(value[0]))));
	SET_WITH_ACTION("ofdm",				(__STRDUP(ofdm, STRING_VALUE(value[0]))));
	SET_WITH_ACTION("bandwidth",		(__STRDUP(bandwidth, STRING_VALUE(value[0]))));

	return JSP_PUT_NOTFOUND;
}

DVBCChannelScanParameters::DVBCChannelScanParameters()
{
	ENTRY;

	startFrequency = 0;
	endFrequency = 0;
	raster = 0;
	networkId = 0;
	modulationModes = 0;
	startNoetworkScanOnNIT = FALSE;
	symbolRate = NULL;
}

DVBCChannelScanParameters::DVBCChannelScanParameters(const DVBCChannelScanParameters &original)
{
	ENTRY;

	startFrequency = original.startFrequency;
	endFrequency   = original.endFrequency;
	modulationModes= original.modulationModes;
	networkId      = original.networkId;
	raster = original.raster;
	startNoetworkScanOnNIT = original.startNoetworkScanOnNIT;

	if (original.symbolRate == NULL)
		symbolRate = NULL;
	else
		symbolRate     = HLIB_STD_StrDup(original.symbolRate);
}

DVBCChannelScanParameters::DVBCChannelScanParameters(humax::DVBC_TunedData *_DVBCtunedData)
{
	startFrequency = _DVBCtunedData->startFreq;
	endFrequency = _DVBCtunedData->endFreq;
	raster = _DVBCtunedData->raster;
	networkId = _DVBCtunedData->networkId;
	modulationModes = _DVBCtunedData->modulationModes;
	startNoetworkScanOnNIT = _DVBCtunedData->startNoetworkScanOnNIT;
	symbolRate = __STRNDUP(_DVBCtunedData->symbolRate,(MAX_SYMBOL_LEN-1));
}

DVBCChannelScanParameters::~DVBCChannelScanParameters()
{
	ENTRY;

	if (symbolRate) {
		HLIB_STD_MemFree(symbolRate);
	}

}

int	DVBCChannelScanParameters::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER ("startFrequency",		startFrequency,			JSP_GET_VALUE);
	GET_NUMBER ("endFrequency",			endFrequency,			JSP_GET_VALUE);
	GET_NUMBER ("raster",				raster,					JSP_GET_VALUE);
	GET_NUMBER ("networkId",			networkId,				JSP_GET_VALUE);
	GET_NUMBER ("modulationModes",	modulationModes,			JSP_GET_VALUE);
	GET_BOOLEAN("startNoetworkScanOnNIT",startNoetworkScanOnNIT,	JSP_GET_VALUE);
	GET_STRING ("symbolRate",			symbolRate,				JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int	DVBCChannelScanParameters::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_WITH_ACTION("startFrequency",			(startFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("endFrequency",				(endFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("raster",					(raster = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("networkId",				(networkId = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("modulationModes",			(modulationModes = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("startNoetworkScanOnNIT",	(startNoetworkScanOnNIT = BOOLEAN_VALUE(value[0])));
	SET_WITH_ACTION("symbolRate",				(__STRDUP(symbolRate, STRING_VALUE(value[0]))));

	return JSP_PUT_NOTFOUND;
}

DVBSChannelScanParameters::DVBSChannelScanParameters()
{
	ENTRY;

	antennaId = 0;
	antennaName = NULL;

	startFrequency = 0;
	endFrequency = 0;
	modulationModes = 0;
	polarisation = 0;
	networkId = 0;
	orbitalPosition = 0.0;
	symbolRate = NULL;
	codeRate = NULL;
}

DVBSChannelScanParameters::DVBSChannelScanParameters(const DVBSChannelScanParameters &original)
{
	ENTRY;

	antennaId = original.antennaId;

	if (original.antennaName == NULL)
		antennaName = NULL;
	else
		antennaName = HLIB_STD_StrDup(original.antennaName);

	startFrequency = original.startFrequency;
	endFrequency   = original.endFrequency;
	modulationModes= original.modulationModes;
	polarisation   = original.polarisation;
	networkId      = original.networkId;

	if (original.symbolRate == NULL)
		symbolRate = NULL;
	else
		symbolRate     = HLIB_STD_StrDup(original.symbolRate);
	if (original.codeRate == NULL)
		codeRate = NULL;
	else
		codeRate       = HLIB_STD_StrDup(original.codeRate);
	orbitalPosition= original.orbitalPosition;
}

DVBSChannelScanParameters::DVBSChannelScanParameters(humax::DVBS_TunedData *_DVBStunedData)
{
	ENTRY;

	antennaId = _DVBStunedData->antennaId;

	if (_DVBStunedData->antennaName == NULL)
		antennaName = NULL;
	else
		antennaName = HLIB_STD_StrDup(_DVBStunedData->antennaName);

	startFrequency = _DVBStunedData->startFreq;
	endFrequency   = _DVBStunedData->endFreq;
	modulationModes= _DVBStunedData->modulationModes;
	polarisation   = _DVBStunedData->polarisation;
	networkId      = _DVBStunedData->networkId;

	if (_DVBStunedData->symbolRate == NULL)
		symbolRate = NULL;
	else
		symbolRate     = HLIB_STD_StrDup(_DVBStunedData->symbolRate);
	if (_DVBStunedData->codeRate == NULL)
		codeRate = NULL;
	else
		codeRate       = HLIB_STD_StrDup(_DVBStunedData->codeRate);
	orbitalPosition= _DVBStunedData->orbitalPosition;
}

DVBSChannelScanParameters::~DVBSChannelScanParameters()
{
	ENTRY;
	if (antennaName) {
		HLIB_STD_MemFree(antennaName);
	}
	if (symbolRate) {
		HLIB_STD_MemFree(symbolRate);
	}
	if (codeRate) {
		HLIB_STD_MemFree(codeRate);
	}
}

int	DVBSChannelScanParameters::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER ("antennaId",			antennaId,				JSP_GET_VALUE);
	GET_STRING ("antennaName",			antennaName,			JSP_GET_VALUE);

	GET_NUMBER ("startFrequency",		startFrequency,			JSP_GET_VALUE);
	GET_NUMBER ("endFrequency",			endFrequency,			JSP_GET_VALUE);
	GET_NUMBER ("modulationModes",		modulationModes,		JSP_GET_VALUE);
	GET_NUMBER ("polarisation",			polarisation,			JSP_GET_VALUE);
	GET_NUMBER ("networkId",			networkId,				JSP_GET_VALUE);

	GET_STRING ("symbolRate",			symbolRate,				JSP_GET_VALUE);
	GET_STRING ("codeRate",				codeRate,				JSP_GET_VALUE);

	GET_NUMBER ("orbitalPosition",		orbitalPosition,		JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int	DVBSChannelScanParameters::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_WITH_ACTION("antennaId",				(antennaId = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("antennaName",				(__STRDUP(antennaName, STRING_VALUE(value[0]))));

	SET_WITH_ACTION("startFrequency",			(startFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("endFrequency",				(endFrequency = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("modulationModes",			(modulationModes = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("polarisation",				(polarisation = NUMBER_VALUE(value[0])));
	SET_WITH_ACTION("networkId",				(networkId = NUMBER_VALUE(value[0])));

	SET_WITH_ACTION("symbolRate",				(__STRDUP(symbolRate, STRING_VALUE(value[0]))));
	SET_WITH_ACTION("codeRate",					(__STRDUP(codeRate, STRING_VALUE(value[0]))));

	SET_WITH_ACTION("orbitalPosition",			(orbitalPosition = NUMBER_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

ChannelScanParameters::ChannelScanParameters(humax::ChannelTuned_Data *_tunedData) : WrapperObject<ChannelScanParameters>(_tunedData)
{
	ENTRY;

	dvbs_scan_parameters = NULL;
	dvbc_scan_parameters = NULL;
	dvbt_scan_parameters = NULL;

	if (_tunedData)
	{
		switch (_tunedData->tunerType)
		{
			case	0x01: // eChDeliveryType_SAT
				idType = CHANNEL_ID_DVB_S;
				dvbs_scan_parameters = new DVBSChannelScanParameters(&_tunedData->data.stDVBSTunedData);
				break;
			case	0x02: // eChDeliveryType_TER
				idType = CHANNEL_ID_DVB_T;
				dvbt_scan_parameters = new DVBTChannelScanParameters(&_tunedData->data.stDVBTTunedData);
				break;
			case	0x04: // eChDeliveryType_CAB
				idType = CHANNEL_ID_DVB_C;
				dvbc_scan_parameters = new DVBCChannelScanParameters(&_tunedData->data.stDVBCTunedData);
				break;
		}
	} else {
		OOIF_LOG_DEBUG("_tunedData is NULL!");
	}

	classType = CLASS_CHANNELSCANPARAMS;
}

ChannelScanParameters::ChannelScanParameters(int id_type) : WrapperObject<ChannelScanParameters>((void*)idType)
{
	ENTRY;

	dvbs_scan_parameters = NULL;
	dvbc_scan_parameters = NULL;
	dvbt_scan_parameters = NULL;

	switch (id_type)
	{
		case	CHANNEL_ID_DVB_S:
		case	CHANNEL_ID_DVB_S2:
			dvbs_scan_parameters = new DVBSChannelScanParameters();
			break;
		case	CHANNEL_ID_DVB_C:
		case	CHANNEL_ID_DVB_C2:
			dvbc_scan_parameters = new DVBCChannelScanParameters();
			break;
		case	CHANNEL_ID_DVB_T:
		case	CHANNEL_ID_DVB_T2:
			dvbt_scan_parameters = new DVBTChannelScanParameters();
			break;
	}

	idType = id_type;

	classType = CLASS_CHANNELSCANPARAMS;
}

ChannelScanParameters::ChannelScanParameters(const ChannelScanParameters &original) : WrapperObject<ChannelScanParameters>(original.identifier)
{
	ENTRY;

	dvbs_scan_parameters = NULL;
	dvbc_scan_parameters = NULL;
	dvbt_scan_parameters = NULL;

	idType = original.idType;
	switch (original.idType)
	{
		case	CHANNEL_ID_DVB_S:
		case	CHANNEL_ID_DVB_S2:
			dvbs_scan_parameters = new DVBSChannelScanParameters(*original.dvbs_scan_parameters);
			break;
		case	CHANNEL_ID_DVB_C:
		case	CHANNEL_ID_DVB_C2:
			dvbc_scan_parameters = new DVBCChannelScanParameters(*original.dvbc_scan_parameters);
			break;
		case	CHANNEL_ID_DVB_T:
		case	CHANNEL_ID_DVB_T2:
			dvbt_scan_parameters = new DVBTChannelScanParameters(*original.dvbt_scan_parameters);
			break;
	}

}

#if defined(JLABS)
ChannelScanParameters::ChannelScanParameters(jlabsScanningHandle handle) : WrapperObject<ChannelScanParameters>(handle)
{
	ENTRY;

	stNativeJlabsLocalSystem_ScanSetting_t 	*pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)handle;

	dvbs_scan_parameters = NULL;
	dvbc_scan_parameters = NULL;
	dvbt_scan_parameters = NULL;

	idType = 10;
	dvbc_scan_parameters = new DVBCChannelScanParameters();
	dvbc_scan_parameters->startFrequency 			= pstSetting->nStartFrequency;
	dvbc_scan_parameters->endFrequency 			= pstSetting->nEndFrequency;
	dvbc_scan_parameters->raster 					= pstSetting->nRaster;
	dvbc_scan_parameters->modulationModes 		= pstSetting->nModulationModes;
	dvbc_scan_parameters->networkId 				= pstSetting->nNetworkId;
	dvbc_scan_parameters->startNoetworkScanOnNIT 	= pstSetting->bstartNoetworkScanOnNIT;
	dvbc_scan_parameters->symbolRate 				= (char *)&pstSetting->aucSymbolRate[0];

	classType = class_name;
}

#endif

ChannelScanParameters::~ChannelScanParameters()
{
	ENTRY;

	switch (idType)
	{
		case	CHANNEL_ID_DVB_S:
		case	CHANNEL_ID_DVB_S2:
			if (dvbs_scan_parameters)
				delete dvbs_scan_parameters;
			break;
		case	CHANNEL_ID_DVB_C:
		case	CHANNEL_ID_DVB_C2:
			if (dvbc_scan_parameters)
				delete dvbc_scan_parameters;
			break;
		case	CHANNEL_ID_DVB_T:
		case	CHANNEL_ID_DVB_T2:
			if (dvbt_scan_parameters)
				delete dvbt_scan_parameters;
			break;
	}
}

ChannelScanParametersCollection::ChannelScanParametersCollection()
{
	ENTRY;
	classType = CLASS_CHANNELSCANPARAMSCOLLECTION;
}

ChannelScanParametersCollection::~ChannelScanParametersCollection()
{
}

int	ChannelScanParametersCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	int ret = Collection<ChannelScanParameters>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;

	GET_FUNCTION(addChannelScanParam, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeChannelScanParam, obj, "-", JSP_GET_VALUE_CACHE);

	return	JSP_GET_NOTFOUND;
}

int	ChannelScanParametersCollection::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return	JSP_PUT_NOTFOUND;
}

int	ChannelScanParametersCollection::addChannelScanParam(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ChannelScanParameters	*obj;
	ChannelScanParametersCollection	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelScanParametersCollection);

	CHECK_ARGUMENT_COUNT(1);

	obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ChannelScanParameters);

	instance->addItem(new ChannelScanParameters(*obj));

	return JSP_CALL_NO_VALUE;
}

int	ChannelScanParametersCollection::removeChannelScanParam(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
    ChannelScanParameters	*obj;
    ChannelScanParametersCollection	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelScanParametersCollection);

    CHECK_ARGUMENT_COUNT(1);

    obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ChannelScanParameters);

    for (int i = 0; i< instance->getSize(); i++) {
    	if (instance->internalGetItem(i)->getIdentifier() == obj->getIdentifier()) {
    		instance->internalRemoveIndex(i);
    		break;
    	}
    }
    return JSP_CALL_NO_VALUE;
}

int	ChannelScanParameters::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER ("idType",		idType,			JSP_GET_VALUE);
	switch (idType)
	{
		case	CHANNEL_ID_DVB_S:
		case	CHANNEL_ID_DVB_S2:
			if (dvbs_scan_parameters)
			{
				return dvbs_scan_parameters->getter(obj, name, result);
			}
			break;
		case	CHANNEL_ID_DVB_C:
		case	CHANNEL_ID_DVB_C2:
			if (dvbc_scan_parameters)
			{
				return dvbc_scan_parameters->getter(obj, name, result);
			}
			break;
		case	CHANNEL_ID_DVB_T:
		case	CHANNEL_ID_DVB_T2:
			if (dvbt_scan_parameters)
			{
				return dvbt_scan_parameters->getter(obj, name, result);
			}
			break;
		default:
			OOIF_LOG_ERROR("Error : not supported idType [%d]\n", idType);
			break;
	}

	return JSP_GET_NOTFOUND;
}

#endif // OIPF

