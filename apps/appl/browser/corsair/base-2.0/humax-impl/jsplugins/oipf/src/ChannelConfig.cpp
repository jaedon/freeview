// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "ChannelConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "NativeChannel.h"
#include "Channel.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#ifdef OIPF
#include "FavouriteList.h"
#include "FavouriteListCollection.h"
#include "humax/ChannelScanParameters.h"
#include "ChannelScanEvent.h"
#include "ChannelScanHumaxEvent.h"
#include "ChannelScanOptions.h"
#include "ChannelListUpdateEvent.h"
#include "ChannelListUpdateHumaxEvent.h"
#include "EmptyEvent.h"
#endif // OIPF

#ifdef JLABS
#include "jlabsChannelListCollection.h"
#include "NativeJlabsChannel.h"
#include "jlabsDTTChannelList.h"
#include "jlabsBSCATVChannelList.h"
#include "ChannelList.h"
#endif // JLABS

#ifdef JLABS_JCOM
#include "ResultCreateMaskEvent.h"
#endif
#include "Exception.h"

#include "macros.h"

#include "oplbuilder.h"

using namespace Ooif;

#ifdef OIPF
list<ChannelConfig*> ChannelConfig::instances;
#endif // OIPF

ChannelConfig::ChannelConfig()
{
	ENTRY;
#ifdef OIPF
	classType = class_name;
	ChannelConfig::instances.push_back(this);
#endif // OIPF
}

ChannelConfig::~ChannelConfig()
{
	ENTRY;
#ifdef OIPF
	ChannelConfig::instances.remove(this);
#endif // OIPF
    if ( channels != NULL )
		delete channels;
}

ChannelConfig::ChannelConfig(ChannelConfigSelection selection)
{
	ENTRY;
	switch (selection)
	{
	case DEFAULT:
		channels = new ChannelList(ALL);
		break;
	case RECORDING:
		channels = new ChannelList(RECORDABLE);
		break;
	default:
		OOIF_LOG_ERROR("Unknown Channel Config Type");
	}
#ifdef OIPF
	classType = class_name;
	ChannelConfig::instances.push_back(this);
#endif // OIPF
}

int ChannelConfig::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	ENTRY;

	// FavouriteListCollection favouriteLists (sic)

	// We cannot return our ChannelList because it will be deleted
	// when the host jsplugin object is garbage collected (or
	// more likely unloaded), so just copy it
	if (strcmp(name, "channelList") == 0)
	{
		OOIFBoolean managed = FALSE;
		NATIVE_WARN_NOK(native_ChannelConfigIsChannelListManaged(&managed));
		if (managed)
			RETURN_OBJECT(obj, new ChannelList(channels), result, JSP_GET_VALUE);
		else
			RETURN_NULL(JSP_GET_VALUE);
	}

#ifdef OIPF
	if (strcmp(name, "currentFavouriteList") == 0)
	{
		OOIFBoolean managed = FALSE;
		NATIVE_WARN_NOK(native_ChannelConfigIsFavouriteListManaged(&managed));
		if (managed)
		{
			FavouriteListHandle favlist = NULL;
			NATIVE_WARN_NOK(native_ChannelConfigGetCurrentFavouriteList(&favlist));
			if (favlist)
				RETURN_OBJECT(obj, new FavouriteList(favlist), result, JSP_GET_VALUE);
			else
				RETURN_NULL(JSP_GET_VALUE);
		}
		else
			RETURN_NULL(JSP_GET_VALUE);
	}

	if (strcmp(name, "favouriteLists") == 0)
	{
		OOIFBoolean managed = FALSE;
		NATIVE_WARN_NOK(native_ChannelConfigIsFavouriteListManaged(&managed));
		if (managed)
		{
			RETURN_OBJECT(obj, new FavouriteListCollection(), result, JSP_GET_VALUE);
		}
		else
			RETURN_NULL(JSP_GET_VALUE);
	}

	if (strcmp(name, "currentChannel") == 0)
	{
		ChannelHandle channel = NULL;
		NATIVE_WARN_NOK(native_ChannelConfigGetCurrentChannel(&channel));
		if(channel)
			RETURN_OBJECT(obj, new Channel(channel), result, JSP_GET_VALUE);
		else
			RETURN_NULL(JSP_GET_VALUE);
	}

	GET_FUNCTION(createFilteredList, obj, "bbbs", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startScan, obj, "o-s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelList, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelObject, obj, "nnnnns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelScanParametersObject, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelScanOptionsObject, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelSearchedInfoObject, obj, "n", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ChannelScan);
	INTRINSIC_EVENT_GET(ChannelScanHumax); 
	INTRINSIC_EVENT_GET(ChannelListUpdate);
	INTRINSIC_EVENT_GET(ChannelListUpdateHumax);
#endif // OIPF

#if defined(HMX_WEBUI)
	NATIVE_GET_STRING_COLLECTION("genre", obj,
								 native_ChannelConfigGetFreesatGenres(&SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genreExt", obj,
								 native_ChannelConfigGetFreesatGenresExt(&SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	GET_FUNCTION(removeChannel, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeChannelList, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(controlScan, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setSelectedConflictLcn, obj, "on", JSP_GET_VALUE_CACHE);
#endif // #if defined(HMX_WEBUI)

#ifdef JLABS
	GET_NUMBER("JLABS_DTT_NETWORK", JLABS_DTT_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_BS_NETWORK", JLABS_BS_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CATV_NETWORK", JLABS_CATV_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_ALL_NETWORK", JLABS_ALL_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS1_NETWORK", JLABS_CS1_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_CS2_NETWORK", JLABS_CS2_NETWORK, JSP_GET_VALUE_CACHE);
	GET_NUMBER("JLABS_JCHITS_NETWORK", JLABS_JCHITS_NETWORK, JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelListCollection, obj, "n", JSP_GET_VALUE_CACHE);
#endif // JLABS
#ifdef JLABS_JCOM
	GET_FUNCTION(createChannelMask, obj, "", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(ResultCreateMask);
#endif //JLABS_JCOM
	return EventTarget::getter(obj, name, result);

//	return JSP_GET_NOTFOUND;
}

int ChannelConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#if defined(HMX_WEBUI)
	// dynamic update on/off based on onChannelListUpdate listener.
	if (!strcmp(name, "onChannelListUpdate") || !strcmp(name, "onChannelListUpdateHumax")) {
		if (value->type == JSP_TYPE_NULL) {
			if (ChannelConfig::countListener(name + 2) == 1) {
				// turn off 'dynamic update'
				NATIVE_WARN_NOK(native_ChannelConfigSetChannelListUpdate(FALSE));
			}
		}
		else if (value->type == JSP_TYPE_NATIVE_OBJECT) {
			if (ChannelConfig::countListener(name + 2) == 0) {
				// turn on 'dynamic update'
				NATIVE_WARN_NOK(native_ChannelConfigSetChannelListUpdate(TRUE));
			}
		}
	}

	INTRINSIC_EVENT(ChannelScan);
	INTRINSIC_EVENT(ChannelScanHumax);
	INTRINSIC_EVENT(ChannelListUpdate);
	INTRINSIC_EVENT(ChannelListUpdateHumax);
#endif // OIPF

#ifdef JLABS_JCOM
	INTRINSIC_EVENT(ResultCreateMask);
#endif
	return JSP_PUT_NOTFOUND;
}

#ifdef OIPF
//static
int ChannelConfig::createFilteredList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                      int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(4);

	// bool blocked, bool favourite, bool hidden, string favouriteListID
	// Let's simply do this filtering ourselves
	ChannelList *base = NULL;

	if (strlen(STRING_VALUE(argv[3])) > 0 &&  TYPE_BOOLEAN(argv[1]) && BOOLEAN_VALUE(argv[1]))
	{
		FavouriteListHandle handle = NULL;
		FavouriteListHandle *handles = NULL;
		int count = 0;
		NATIVE_CALL(native_ChannelConfigGetFavouriteLists(&count, &handles));
		if ( handles != NULL )
		{
			for (int i=0; i<count; i++)
			{
				const char *id=NULL;
				NATIVE_WARN_NOK(native_FavouriteListGetFavID(handles[i], &id));
				if (!strcmp(id, STRING_VALUE(argv[3])))
				{
					handle = handles[i];
					break;
				}
			}

			if (handle)
			{
				base = new FavouriteList(handle);
			} 
			else 
			{
				base = NULL;
			}
			HLIB_STD_MemFree(handles);
		}
	}
	else
	{
		base = new ChannelList(ALL);
	}

	if (base)
	{
		ChannelList *answer = new ChannelList(base,
		                                      TYPE_BOOLEAN(argv[0]), BOOLEAN_VALUE(argv[0]),
		                                      TYPE_BOOLEAN(argv[1]), BOOLEAN_VALUE(argv[1]),
		                                      TYPE_BOOLEAN(argv[2]), BOOLEAN_VALUE(argv[2]));
		delete base;

		RETURN_OBJECT(this_obj, answer, result, JSP_CALL_VALUE);
	}
	else
	{
		RETURN_NULL(JSP_CALL_VALUE);
	}
}

//static
int ChannelConfig::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int		count = 0;

	OPL_SCANOPTIONS_t	stScanOptions;
	OPL_SCANPARAMS_t	*pstScanParams;
	const ScanOptions		*pstChScanOpt;

	ChannelScanOptions		*scanOptions;
	const char				*scanMode = NULL;

	scanMode = OPLSCANTYPE_NORMAL;
	if (argc == 3)
		scanMode = STRING_VALUE(argv[2]);

	ENTRY;

	convertOPLParams(&argv[1], &count, (void**)&pstScanParams);
	if (count == 0) {
		if (pstScanParams) {
			HLIB_STD_MemFree(pstScanParams);
		}
		return JSP_CALL_ERROR;
	}

	scanOptions = OBJECT_VALUE(argv[0], ChannelScanOptions);
	if (scanOptions) {
		pstChScanOpt = scanOptions->getOpts();

		stScanOptions.channelType		= pstChScanOpt->channelType;
		stScanOptions.casType			= pstChScanOpt->casType;
		stScanOptions.isReplaceChannel	= pstChScanOpt->replaceExisting;
		stScanOptions.enableNetworkScan	= pstChScanOpt->enableNetworkScan;
	} else {
		memset(&stScanOptions, 0, sizeof(OPL_SCANOPTIONS_t));
	}

	OPL_Builder_RegisterListener(onChannelConfigEventOPL);
	if (OPL_Builder_StartScan((HCHAR *)scanMode, &stScanOptions, count, pstScanParams) == BPL_STATUS_OK)
	{
		HLIB_STD_MemFree(pstScanParams);
		return JSP_CALL_NO_VALUE;
	}

	HLIB_STD_MemFree(pstScanParams);

	return JSP_CALL_ERROR;
}

//static
int ChannelConfig::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	ENTRY;

	//	TODO: Remove OPL Event Listener
	OPL_Builder_UnRegisterListener();

	if (OPL_Builder_StopScan() == BPL_STATUS_OK)
		return JSP_CALL_NO_VALUE;

	return JSP_CALL_ERROR;
}

//static
int ChannelConfig::createChannelList(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	if (strlen(STRING_VALUE(argv[0])) > 0)
	{
		OOIFBoolean retval = FALSE;
		NATIVE_CALL(native_ChannelConfigIsValidBDR(STRING_VALUE(argv[0]), &retval));
		if(retval)
			RETURN_OBJECT(this_obj, new ChannelList(STRING_VALUE(argv[0])), result, JSP_CALL_VALUE);
	}
	RETURN_NULL(JSP_CALL_VALUE);
}

//static
int ChannelConfig::createChannelObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int idType = 0, onid = 0, tsid = 0, sid = 0, sourceID = 0;
	const char* ipBroadcastID = NULL;
	ChannelHandle retval = NULL;

	// check all argument types for supplied parameters
	switch (argc)
	{
	default: // all cases fall through
	case 6:
		if (TYPE_STRING(argv[5])) ipBroadcastID = STRING_VALUE(argv[5]);
		else goto arg_error;
	case 5:
		if (TYPE_NUMBER(argv[4])) sourceID      = NUMBER_VALUE(argv[4]);
		else goto arg_error;
	case 4:
		if (TYPE_NUMBER(argv[3])) sid           = NUMBER_VALUE(argv[3]);
		else goto arg_error;
	case 3:
		if (TYPE_NUMBER(argv[2])) tsid          = NUMBER_VALUE(argv[2]);
		else goto arg_error;
	case 2:
		if (TYPE_NUMBER(argv[1])) onid          = NUMBER_VALUE(argv[1]);
		else goto arg_error;
	case 1:
		if (TYPE_NUMBER(argv[0])) idType        = NUMBER_VALUE(argv[0]);
		else goto arg_error;
		break;
	}

	NATIVE_CALL(native_ChannelConfigCreateChannelObject(idType,
	            argc >= 2, onid,
	            argc >= 3, tsid,
	            argc >= 4, sid,
	            argc >= 5, sourceID,
	            ipBroadcastID,
	            &retval));

	RETURN_OBJECT(this_obj, new Channel(retval), result, JSP_CALL_VALUE);

arg_error:
	THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
}

//static
int ChannelConfig::createChannelScanOptionsObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new ChannelScanOptions(), result, JSP_CALL_VALUE);
}

//static
int ChannelConfig::createChannelScanParametersObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;

	CHECK_ARGUMENT_COUNT_MIN_MAX(0, 1);

	if (argc == 0)
	{
		//	ChannelScanParameterCollection
		ChannelScanParametersCollection	*base;

		OOIF_LOG_DEBUG("createChannelScanParameterCollection\n");
		base = new ChannelScanParametersCollection();
		RETURN_OBJECT(this_obj, base, result, JSP_CALL_VALUE);
	} else
//	if (argc == 1)
	{
		//	ChannelScanParameterObject
		ChannelScanParameters	*base;

		OOIF_LOG_DEBUG("createChannelScanParameterObject\n");
		base = new ChannelScanParameters(NUMBER_VALUE(argv[0]));
		RETURN_OBJECT(this_obj, base, result, JSP_CALL_VALUE);
	}
}

int ChannelConfig::createChannelSearchedInfoObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	int count = NUMBER_VALUE(argv[0]);

	ChannelConfig	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelConfig);

	if (count > 0)
	{
		instance->searchedInfos = (humax::SearchedInfo_Data*)HLIB_STD_MemAlloc(sizeof(humax::SearchedInfo_Data) * count);
		RETURN_OBJECT(this_obj, new CSearchedInfo(count, instance->searchedInfos), result, JSP_CALL_VALUE);
	}

	return JSP_CALL_NO_VALUE;
}


//static
void ChannelConfig::onChannelConfigEvent(ChannelConfigEvent event, ChannelConfigEventInfo *info)
{
	jsplugin_obj	*context;

	for (list<ChannelConfig*>::iterator it=instances.begin(); it != instances.end(); it++)
	{
		ChannelConfig* ch_config = *it;
	    Event* e = NULL;
            Event *new_e = NULL;   // new event

		context = ch_config->getHost();

		switch(event)
		{
#ifndef	HMX_WEBUI
			case CHANNEL_CONFIG_EVENT_CHANNEL_SCAN:
			{
				e = new ChannelScanEvent(ch_config->getHost(),
					info->channel_scan_data.type,
					info->channel_scan_data.progress,
					info->channel_scan_data.frequency,
					info->channel_scan_data.signalStrength,
					info->channel_scan_data.channelNumber,
					info->channel_scan_data.channelType,
					info->channel_scan_data.channelCount,
					info->channel_scan_data.transponderCount);
					
				ch_config->dispatchEvent(e);
				break;
			}
#endif
			case CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE:
			{
				// re-load channel list
				delete ch_config->channels;

				if (ch_config->selection == RECORDING) {
					ch_config->channels = new ChannelList(RECORDABLE);
				} else {
					ch_config->channels = new ChannelList(ALL);
				}

				e = new EmptyEvent(ch_config->getHost(), "ChannelListUpdate");
				ch_config->dispatchEvent(e);
				break;
			}
			default:
			{
				OOIF_LOG_WARNING("Unknown event received");
			}
		}
	}
}


//static
void ChannelConfig::onChannelConfigEvent(humax::ChannelConfigEvent event, humax::ChannelConfigEventInfo *info)
{
	Event	*e = NULL;
        Event	*new_e = NULL;
	jsplugin_obj	*context;

	for (list<ChannelConfig*>::iterator it=instances.begin(); it!=instances.end(); it++) {
		ChannelConfig *instance = *it;

		context = instance->getHost();

		switch (event)
		{
			case	humax::ChannelConfigEvent_ChannelScan:
				{
					humax::ChannelScan_Data	*params = &info->uChannelScanData;


					new_e = new ChannelScanHumaxEvent(context, params->type, params->progress, params->frequency,
																params->signalStrength, params->channelNumber,
																params->channelType, params->channelCount, params->transponderCount,
																params->infoCount, params->searchedInfos, params->tunedInfo);
																 // opera 4.6
					e = new ChannelScanEvent(context, params->type, params->progress, params->frequency,
																params->signalStrength, params->channelNumber,
																params->channelType, params->channelCount, params->transponderCount,
																params->infoCount, params->searchedInfos, params->tunedInfo);
				}
				break;
			case	humax::ChannelConfigEvent_Connection:
				{
					humax::ConnectionTest_Data	*params = &info->uConnectionData;


					new_e = new ChannelScanHumaxEvent(context, params->type, params->progress, 0,
																0, 0,
																0, params->connType, 0,
																0, NULL, NULL);

					e = new ChannelScanEvent(context, params->type, params->progress, 0,
																0, 0,
																0, params->connType, 0, 
																0, NULL, NULL);
				}
				break;
			case	humax::ChannelConfigEvent_ListUpdated:
				new_e = new ChannelListUpdateHumaxEvent(context); // opera 4.6
				e = new EmptyEvent(context, "ChannelListUpdate");
				break;
		}
		instance->dispatchEvent(e);
                instance->dispatchEvent(new_e);
	}
}

#define	__STRNCPY(b, a, c)		{ if (a == NULL) strncpy(b, "", c); else strncpy(b, a, c);}

void ChannelConfig::convertScanParameters(ChannelScanParameters *scanParameters, void *pvParams)
{
	OPL_SCANPARAMS_t	*pstScanParams = (OPL_SCANPARAMS_t*)pvParams;

	switch (scanParameters->idType)
	{
		case	CHANNEL_ID_DVB_S:
		case	CHANNEL_ID_DVB_S2:
			pstScanParams->tunerType = OPL_TUNER_SAT;
			{
				DVBS_SCANPARAMS_t	*scanParams;
				DVBSChannelScanParameters	*oopParameters = scanParameters->dvbs_scan_parameters;

				scanParams = &pstScanParams->data.stDVBSScanParams;
				scanParams->startFreq		= oopParameters->startFrequency;
				scanParams->endFreq			= oopParameters->endFrequency;
				scanParams->modulationModes	= oopParameters->modulationModes;
				scanParams->polarisation	= oopParameters->polarisation;
				scanParams->orbitalPosition	= oopParameters->orbitalPosition;
				scanParams->networkId		= oopParameters->networkId;

				__STRNCPY(scanParams->symbolRate, oopParameters->symbolRate, (MAX_SYMBOLRATE_LEN - 1));
				__STRNCPY(scanParams->codeRate, oopParameters->codeRate, (MAX_PARAMS_LEN - 1));

				scanParams->antennaId = oopParameters->antennaId;
				__STRNCPY(scanParams->antennaName, oopParameters->antennaName, (MAX_PARAMS_LEN - 1));
			}
			break;
		case	CHANNEL_ID_DVB_C:
		case	CHANNEL_ID_DVB_C2:
			pstScanParams->tunerType = OPL_TUNER_CAB;
			{
				DVBC_SCANPARAMS_t	*scanParams;
				DVBCChannelScanParameters	*oopParameters = scanParameters->dvbc_scan_parameters;

				scanParams = &pstScanParams->data.stDVBCScanParams;
				scanParams->startFreq		= oopParameters->startFrequency;
				scanParams->endFreq			= oopParameters->endFrequency;
				scanParams->modulationModes	= (OPL_QAM_MODULATION_MODE_e)oopParameters->modulationModes;
				scanParams->raster			= oopParameters->raster;
				scanParams->networkId		= oopParameters->networkId;
				scanParams->startNoetworkScanOnNIT	= oopParameters->startNoetworkScanOnNIT;

				__STRNCPY(scanParams->symbolRate, oopParameters->symbolRate, (MAX_SYMBOLRATE_LEN-1));
			}
			break;
		case	CHANNEL_ID_DVB_T:
		case	CHANNEL_ID_DVB_T2:
			pstScanParams->tunerType = OPL_TUNER_TER;
			{
				DVBT_SCANPARAMS_t	*scanParams;
				DVBTChannelScanParameters	*oopParameters = scanParameters->dvbt_scan_parameters;

				scanParams = &pstScanParams->data.stDVBTScanParams;
				scanParams->startFreq		= oopParameters->startFrequency;
				scanParams->endFreq			= oopParameters->endFrequency;
				scanParams->raster			= oopParameters->raster;
				scanParams->modulationModes	= oopParameters->modulationModes;

				__STRNCPY(scanParams->transmission, oopParameters->transmission, (MAX_PARAMS_LEN-1));
				__STRNCPY(scanParams->ofdm, oopParameters->ofdm, (MAX_PARAMS_LEN-1));
				__STRNCPY(scanParams->bandwidth, oopParameters->bandwidth, (MAX_PARAMS_LEN-1));
			}
			break;
	}
}

int	ChannelConfig::convertOPLParams(jsplugin_value *argv, int *pnCount, void **pvData)
{
	int		count;

	OPL_SCANPARAMS_t	*pstScanParams;

	ChannelScanParameters	*scanParameters;
	ChannelScanParametersCollection	*scanParameterCollection;

	scanParameters = OBJECT_VALUE(*argv, ChannelScanParameters);
	if (scanParameters == NULL)
		scanParameterCollection = OBJECT_VALUE(*argv, ChannelScanParametersCollection);

	if (scanParameters == NULL && scanParameterCollection == NULL)
	{
		OOIF_LOG_ERROR("startTune : parameter is not valid!!!\n");
		return JSP_CALL_ERROR;
	}

	count = 0;
	if (scanParameters) {
		count = 1;
		pstScanParams = (OPL_SCANPARAMS_t*)HLIB_STD_MemAlloc(sizeof(OPL_SCANPARAMS_t) * count);

		convertScanParameters(scanParameters, (void*)pstScanParams);
	} else if (scanParameterCollection) {
		count = scanParameterCollection->getSize();
		OOIF_LOG_ERROR("ChannelParameter is Collection!!!! [%d]\n", count);

		pstScanParams = (OPL_SCANPARAMS_t*)HLIB_STD_MemAlloc(sizeof(OPL_SCANPARAMS_t) * count);

		for (int i = 0; i < count; i++)
		{
			ChannelScanParameters	*paramV;

			paramV = scanParameterCollection->internalGetItem(i);
			convertScanParameters(paramV, (void*)&pstScanParams[i]);
		}
	}

	*pvData = pstScanParams;
	*pnCount = count;

	return 0;
}

int	ChannelConfig::onChannelConfigEventOPL(void *pvData)
{
	int		i;
	OPL_BUILDEREVENT_t		*pstEvent = (OPL_BUILDEREVENT_t*)pvData;
	humax::ChannelConfigEventInfo	scanEventInfo;

	memset(&scanEventInfo, 0, sizeof(humax::ChannelConfigEventInfo));
	switch (pstEvent->eventType)
	{
		case	eBUILDEREVENT_SCAN:
			scanEventInfo.uChannelScanData.type = pstEvent->u.scanEvent.type;
			if (pstEvent->u.scanEvent.totalTpNum)
				scanEventInfo.uChannelScanData.progress = ((pstEvent->u.scanEvent.completeTpNum * 100) / pstEvent->u.scanEvent.totalTpNum);
			else
				scanEventInfo.uChannelScanData.progress = -1;

			scanEventInfo.uChannelScanData.frequency = pstEvent->u.scanEvent.frequency;
			scanEventInfo.uChannelScanData.signalStrength = pstEvent->u.scanEvent.signalStrength;
			scanEventInfo.uChannelScanData.channelNumber = 0;
			scanEventInfo.uChannelScanData.channelType = 0;
			scanEventInfo.uChannelScanData.channelCount = pstEvent->u.scanEvent.searchedSvcNum;
			scanEventInfo.uChannelScanData.transponderCount = pstEvent->u.scanEvent.completeTpNum;

			scanEventInfo.uChannelScanData.infoCount = pstEvent->u.scanEvent.infoCount;
			if (scanEventInfo.uChannelScanData.infoCount > 0)
			{
				scanEventInfo.uChannelScanData.searchedInfos = (humax::SearchedInfo_Data*)HLIB_STD_MemAlloc(sizeof(humax::SearchedInfo_Data) * scanEventInfo.uChannelScanData.infoCount);
				memset(scanEventInfo.uChannelScanData.searchedInfos, 0, sizeof(humax::SearchedInfo_Data));
				for (i = 0; i < scanEventInfo.uChannelScanData.infoCount; i++)
				{
					humax::SearchedInfo_Data	*info = &scanEventInfo.uChannelScanData.searchedInfos[i];
					info->onId			= pstEvent->u.scanEvent.searchedInfos[i].onId;
					info->tsId			= pstEvent->u.scanEvent.searchedInfos[i].tsId;
					info->svcId			= pstEvent->u.scanEvent.searchedInfos[i].svcId;
					info->channelType	= pstEvent->u.scanEvent.searchedInfos[i].channelType;
					info->casType		= pstEvent->u.scanEvent.searchedInfos[i].casType;
					info->lcn			= pstEvent->u.scanEvent.searchedInfos[i].lcn;
					strncpy(info->name, pstEvent->u.scanEvent.searchedInfos[i].name, (MAX_SEARCHEDINFO_NAME_LEN - 1));
				}
			}

			if (NULL != pstEvent->u.scanEvent.pstTunedParam) {
				humax::ChannelTuned_Data *pTmpInfo;
				pTmpInfo = (humax::ChannelTuned_Data*)HLIB_STD_MemAlloc(sizeof(humax::ChannelTuned_Data));
				if (pTmpInfo) {
					memset(pTmpInfo, 0, sizeof(humax::ChannelTuned_Data));
					pTmpInfo->tunerType = pstEvent->u.scanEvent.pstTunedParam->tunerType;

					if (0x01 == pstEvent->u.scanEvent.pstTunedParam->tunerType) 				// SAT
					{
						pTmpInfo->data.stDVBSTunedData.antennaId		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.antennaId;
						pTmpInfo->data.stDVBSTunedData.startFreq		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.startFreq;
						pTmpInfo->data.stDVBSTunedData.endFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.endFreq;
						pTmpInfo->data.stDVBSTunedData.modulationModes	= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.modulationModes;
						pTmpInfo->data.stDVBSTunedData.polarisation		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.polarisation;
						pTmpInfo->data.stDVBSTunedData.orbitalPosition	= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.orbitalPosition;
						pTmpInfo->data.stDVBSTunedData.networkId		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.networkId;

						strncpy(pTmpInfo->data.stDVBSTunedData.antennaName, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.antennaName, (MAX_DATAS_LEN-1));
						strncpy(pTmpInfo->data.stDVBSTunedData.symbolRate, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.symbolRate, (MAX_SYMBOL_LEN-1));
						strncpy(pTmpInfo->data.stDVBSTunedData.codeRate, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.codeRate, (MAX_DATAS_LEN-1));
					}
					else if (0x02 == pstEvent->u.scanEvent.pstTunedParam->tunerType)			// TER
					{
						pTmpInfo->data.stDVBTTunedData.startFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.startFreq;
						pTmpInfo->data.stDVBTTunedData.endFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.endFreq;
						pTmpInfo->data.stDVBTTunedData.raster				= pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.raster;
						pTmpInfo->data.stDVBTTunedData.modulationModes		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.modulationModes;

						strncpy(pTmpInfo->data.stDVBTTunedData.ofdm, pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.ofdm, (MAX_DATAS_LEN-1));
						strncpy(pTmpInfo->data.stDVBTTunedData.bandwidth, pstEvent->u.scanEvent.pstTunedParam->data.stDVBTScanParams.bandwidth, (MAX_DATAS_LEN-1));
					}
					else if (0x04 == pstEvent->u.scanEvent.pstTunedParam->tunerType)			// CAB
					{
						pTmpInfo->data.stDVBCTunedData.startFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.startFreq;
						pTmpInfo->data.stDVBCTunedData.endFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.endFreq;
						pTmpInfo->data.stDVBCTunedData.raster				= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.raster;
						pTmpInfo->data.stDVBCTunedData.startNoetworkScanOnNIT				= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.startNoetworkScanOnNIT;
						pTmpInfo->data.stDVBCTunedData.modulationModes		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.modulationModes;

						strncpy(pTmpInfo->data.stDVBCTunedData.symbolRate, pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.symbolRate, (MAX_SYMBOL_LEN-1));
						pTmpInfo->data.stDVBCTunedData.networkId		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBCScanParams.networkId;
					}
					else
					{
						// TODO: Implement Ter(eChDeliveryType_TER) & CAB(eChDeliveryType_CAB)
						HLIB_STD_MemFree(pTmpInfo);
						pTmpInfo = NULL;
					}
				}
				scanEventInfo.uChannelScanData.tunedInfo = pTmpInfo;
			} else {
				scanEventInfo.uChannelScanData.tunedInfo = NULL;
			}

			onChannelConfigEvent(humax::ChannelConfigEvent_ChannelScan, &scanEventInfo);

			if (scanEventInfo.uChannelScanData.infoCount > 0)
				HLIB_STD_MemFree(scanEventInfo.uChannelScanData.searchedInfos);
			if (NULL != scanEventInfo.uChannelScanData.tunedInfo)
				HLIB_STD_MemFree(scanEventInfo.uChannelScanData.tunedInfo);

			// FREE Allocated memory in OPL_Builder (_OPL_BUILDER_Notifier() & _OPL_BUILDER_ConvertTunedParam())
			if (NULL != pstEvent->u.scanEvent.pstTunedParam)
			{
				HLIB_STD_MemFree(pstEvent->u.scanEvent.pstTunedParam);
				pstEvent->u.scanEvent.pstTunedParam = NULL;
			}
			if (NULL != pstEvent->u.scanEvent.searchedInfos)
			{
				HLIB_STD_MemFree(pstEvent->u.scanEvent.searchedInfos);
				pstEvent->u.scanEvent.searchedInfos = NULL;
			}
			break;
		case	eBUILDEREVENT_CONNECTION:
			scanEventInfo.uConnectionData.type = pstEvent->u.connectionEvent.type;
			scanEventInfo.uConnectionData.progress = pstEvent->u.connectionEvent.progress;
			scanEventInfo.uConnectionData.connType = pstEvent->u.connectionEvent.connType;

			onChannelConfigEvent(humax::ChannelConfigEvent_Connection, &scanEventInfo);
			break;
		default:
			break;
	}

	return 0;
}

CSearchedInfo::CSearchedInfo(int _infoCount, humax::SearchedInfo_Data *_searchedInfos)
{
	classType = CLASS_SEARCHEDINFO;

	infoCount = _infoCount;
	searchedInfos = NULL;
	if (infoCount > 0)
	{
		searchedInfos = (humax::SearchedInfo_Data*)HLIB_STD_MemAlloc(sizeof(humax::SearchedInfo_Data) * _infoCount);
		memcpy(searchedInfos, _searchedInfos, _infoCount * sizeof(humax::SearchedInfo_Data));
	}
}

CSearchedInfo::~CSearchedInfo()
{
	if (infoCount > 0)
		HLIB_STD_MemFree(searchedInfos);
}

int CSearchedInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_NUMBER("length", infoCount, JSP_GET_VALUE);
	GET_FUNCTION(getInfo, obj, "ns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getStringInfo, obj, "ns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setInfo, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setStringInfo, obj, "", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int	CSearchedInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

int	CSearchedInfo::getInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(2);

	CSearchedInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CSearchedInfo);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->infoCount)
	{
		humax::SearchedInfo_Data	*info = &(instance->searchedInfos[index]);

		if (strstr(type, "onId")) {
			RETURN_NUMBER(info->onId, JSP_CALL_VALUE);
		} else
		if (strstr(type, "tsId")) {
			RETURN_NUMBER(info->tsId, JSP_CALL_VALUE);
		} else
		if (strstr(type, "svcId")) {
			RETURN_NUMBER(info->svcId, JSP_CALL_VALUE);
		} else
		if (strstr(type, "channelType")) {
			RETURN_NUMBER(info->channelType, JSP_CALL_VALUE);
		} else
		if (strstr(type, "casType")) {
			RETURN_NUMBER(info->casType, JSP_CALL_VALUE);
		} else
		if (strstr(type, "lcn")) {
			RETURN_NUMBER(info->lcn, JSP_CALL_VALUE);
		} else
		if (strstr(type, "name")) {
			RETURN_STRING(info->name, JSP_CALL_VALUE);
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}

int	CSearchedInfo::getStringInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(2);

	CSearchedInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CSearchedInfo);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->infoCount)
	{
		humax::SearchedInfo_Data	*info = &(instance->searchedInfos[index]);

		if (strstr(type, "name")) {
			RETURN_STRING(info->name, JSP_CALL_VALUE);
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}

humax::SearchedInfo_Data CSearchedInfo::getInfo(int index)
{
	humax::SearchedInfo_Data info = searchedInfos[index];
	return info;
}

int	CSearchedInfo::setInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(3);

	CSearchedInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CSearchedInfo);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->infoCount)
	{
		humax::SearchedInfo_Data	*info = &(instance->searchedInfos[index]);

		if (strstr(type, "onId")) {
			info->onId = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "tsId")) {
			info->tsId = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "svcId")) {
			info->svcId = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "channelType")) {
			info->channelType = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "casType")) {
			info->casType = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "lcn")) {
			info->lcn = NUMBER_VALUE(argv[2]);
		} else
		if (strstr(type, "name")) {
			strncpy(info->name, STRING_VALUE(argv[2]), MAX_SEARCHEDINFO_NAME_LEN-1); info->name[MAX_SEARCHEDINFO_NAME_LEN-1] = '\0';
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}

int	CSearchedInfo::setStringInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int		index;
	const	char	*type;

	CHECK_ARGUMENT_COUNT(3);

	CSearchedInfo	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, CSearchedInfo);

	index = NUMBER_VALUE(argv[0]);
	type = STRING_VALUE(argv[1]);

	if (index < instance->infoCount)
	{
		humax::SearchedInfo_Data	*info = &(instance->searchedInfos[index]);

		if (strstr(type, "name")) {
			strncpy(info->name, STRING_VALUE(argv[2]), MAX_SEARCHEDINFO_NAME_LEN-1); info->name[MAX_SEARCHEDINFO_NAME_LEN-1] = '\0';
		}
	}

	RETURN_UNDEFINED(JSP_CALL_VALUE);
}
#endif // OIPF

#ifdef JLABS
int ChannelConfig::createChannelListCollection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	if (NUMBER_VALUE(argv[0]) >= JLABS_DTT_NETWORK && NUMBER_VALUE(argv[0]) <= JLABS_JCHITS_NETWORK)
	{
		jlabsDTTChannelList *dttList = NULL;
		jlabsBSCATVChannelList *bsCatvList = NULL;
		jlabsChannelListCollection *listCollection = NULL;
		jlabsChannelListNetwork network = (jlabsChannelListNetwork)NUMBER_VALUE(argv[0]);
		jlabsChannelListHandle *handles = NULL;
		int	*pNumOfChannelsByNetwork = NULL;
		int	count = 0;
		int	i, j;
		jlabsChannelListNetwork	aeCreateAllChannelListCollectionType[JLABS_END_NETWORK] = {
																JLABS_DTT_NETWORK
																,JLABS_BS_NETWORK
																,JLABS_CATV_NETWORK
																,JLABS_CS1_NETWORK
																,JLABS_CS2_NETWORK
																,JLABS_JCHITS_NETWORK
																};

		if(network == JLABS_ALL_NETWORK)
		{
			listCollection = new jlabsChannelListCollection(network);
			if(listCollection == NULL)
			{
				OOIF_LOG_ERROR("listCollection is null [%s : %d]\n", __FUNCTION__,__LINE__);
				goto funcExit;
			}
			for(i = 0; i < (JLABS_END_NETWORK); i++)
			{
				if(aeCreateAllChannelListCollectionType[i] == JLABS_DTT_NETWORK)
				{
					/* DTT channel list 0~N */
					native_jlabsChannelConfigGetChannelListsByNetType(aeCreateAllChannelListCollectionType[i], &count, &pNumOfChannelsByNetwork, &handles);
					if((pNumOfChannelsByNetwork != NULL) || (handles != NULL))
					{
						for(j = 0; j < count ; j++)
						{
							dttList = new jlabsDTTChannelList(pNumOfChannelsByNetwork[j], handles[j]);
							listCollection->addItem((ChannelList*)dttList);
						}
						NATIVE_CALL(native_jlabsChannelConfigFreeChList(handles));
						NATIVE_CALL(native_jlabsChannelConfigFreeNumOfChannel(pNumOfChannelsByNetwork));
					}
					handles = NULL;
					pNumOfChannelsByNetwork = NULL;
					dttList = NULL;
					count = 0;
				}
				else
				{
					native_jlabsChannelConfigGetChannelListsByNetType(aeCreateAllChannelListCollectionType[i], &count, &pNumOfChannelsByNetwork, &handles);
					if((pNumOfChannelsByNetwork != NULL) || (handles != NULL))
					{
						for(j = 0; j < count ; j++)
						{
							bsCatvList = new jlabsBSCATVChannelList(pNumOfChannelsByNetwork[j], handles[j]);
							listCollection->addItem((ChannelList*)bsCatvList);
						}
						NATIVE_CALL(native_jlabsChannelConfigFreeChList(handles));
						NATIVE_CALL(native_jlabsChannelConfigFreeNumOfChannel(pNumOfChannelsByNetwork));
					}
					handles = NULL;
					pNumOfChannelsByNetwork = NULL;
					bsCatvList = NULL;
					count = 0;
				}
			}
		}
		else
		{
			if(network == JLABS_DTT_NETWORK)
			{
				/* DTT channel list 0~N */
				NATIVE_CALL(native_jlabsChannelConfigGetChannelListsByNetType(JLABS_DTT_NETWORK, &count, &pNumOfChannelsByNetwork, &handles));
				if((pNumOfChannelsByNetwork != NULL) || (handles != NULL))
				{
					listCollection = new jlabsChannelListCollection(network);
					if(listCollection == NULL)
					{
						OOIF_LOG_ERROR("listCollection is null [%s : %d]\n", __FUNCTION__,__LINE__);
						goto funcExit;
					}
					for(i = 0; i < count ; i++)
					{
						dttList = new jlabsDTTChannelList(pNumOfChannelsByNetwork[i], handles[i]);
						listCollection->addItem((ChannelList*)dttList);
					}
					NATIVE_CALL(native_jlabsChannelConfigFreeChList(handles));
					NATIVE_CALL(native_jlabsChannelConfigFreeNumOfChannel(pNumOfChannelsByNetwork));
				}
				handles = NULL;
				pNumOfChannelsByNetwork = NULL;
				dttList = NULL;
				count = 0;
			}
			else if((network == JLABS_BS_NETWORK)
			|| (network == JLABS_CATV_NETWORK)
			|| (network == JLABS_CS1_NETWORK)
			|| (network == JLABS_CS2_NETWORK)
			|| (network == JLABS_JCHITS_NETWORK))
			{
				NATIVE_CALL(native_jlabsChannelConfigGetChannelListsByNetType(network, &count, &pNumOfChannelsByNetwork, &handles));
				if((pNumOfChannelsByNetwork != NULL) || (handles != NULL))
				{
					listCollection = new jlabsChannelListCollection(network);
					if(listCollection == NULL)
					{
						OOIF_LOG_ERROR("listCollection is null [%s : %d]\n", __FUNCTION__,__LINE__);
						goto funcExit;
					}
					for(i = 0; i < count ; i++)
					{
						bsCatvList = new jlabsBSCATVChannelList(pNumOfChannelsByNetwork[i], handles[i]);
						listCollection->addItem(bsCatvList);
					}
					NATIVE_CALL(native_jlabsChannelConfigFreeChList(handles));
					NATIVE_CALL(native_jlabsChannelConfigFreeNumOfChannel(pNumOfChannelsByNetwork));
				}
				handles = NULL;
				pNumOfChannelsByNetwork = NULL;
				bsCatvList = NULL;
				count = 0;
			}
			else
			{
				OOIF_LOG_ERROR("wrong network (%d)[%s : %d]\n", network, __FUNCTION__,__LINE__);
				goto funcExit;
			}
		}

		RETURN_OBJECT(this_obj, listCollection, result, JSP_CALL_VALUE);

	funcExit:
		NATIVE_CALL(native_jlabsChannelConfigFreeChList(handles));
		NATIVE_CALL(native_jlabsChannelConfigFreeNumOfChannel(pNumOfChannelsByNetwork));
		RETURN_NULL(JSP_CALL_VALUE);
	}

	return JSP_CALL_NO_VALUE;
}

#endif // JLABS

#ifdef JLABS_JCOM
int ChannelConfig::createChannelMask(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIFBoolean retval;

	ENTRY;
	NATIVE_CALL(native_jlabsChannelConfigCreateChannelMask(&retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

void ChannelConfig::onResultCreateMaskEvent(int result)
{
	for (list<ChannelConfig*>::iterator it=instances.begin(); it != instances.end(); it++)
	{
		ChannelConfig* ch_config = *it;
		ch_config->dispatchEvent(new ResultCreateMaskEvent(ch_config->getHost(), result));
	}
}

#endif

#if defined(HMX_WEBUI)
int	ChannelConfig::controlScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	ENTRY;

	if (OPL_Builder_ControlScan((HCHAR*)STRING_VALUE(argv[0])) == BPL_STATUS_OK)
		return JSP_CALL_NO_VALUE;

	return JSP_CALL_ERROR;
}

// dynamic update on/off based on onChannelListUpdate listener.
//static
int ChannelConfig::countListener (const char *name)
{
	int	count = 0;
	jsplugin_obj *temp_obj;

	for (list<ChannelConfig*>::iterator it=ChannelConfig::instances.begin()
		; it != ChannelConfig::instances.end()
		; it++) {
		ChannelConfig* self = *it;

		if (self->findIntrinsicListener(name, &temp_obj) == OOIF_RETURN_OK)
			count++;
	}
	return count;
}

//static
int ChannelConfig::removeChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
									  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	ObjectInstance *obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ObjectInstance);
	if (obj->getType() == CLASS_CHANNEL) {
		// Add channel
		Channel *c = static_cast<Channel*>(obj);

		for (list<ChannelConfig*>::iterator it=instances.begin(); it!=instances.end(); it++) {
			ChannelConfig *instance = *it;
			instance->channels->removeChannel(c);
		}
	}
	else if (obj->getType() == CLASS_CHANNELLIST) {
		// Add all these channels
		ChannelList *cl = static_cast<ChannelList*>(obj);
		for (int i=0; i<cl->getSize(); i++) {
			Channel *c = cl->internalGetItem(i);

			for (list<ChannelConfig*>::iterator it=instances.begin(); it!=instances.end(); it++) {
				ChannelConfig *instance = *it;
				instance->channels->removeChannel(c);
			}
		}
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	return JSP_CALL_NO_VALUE;
}

int ChannelConfig::removeChannelList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
									  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	ObjectInstance *obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ObjectInstance);
	if (obj->getType() == CLASS_CHANNELLIST) {
		for (list<ChannelConfig*>::iterator it=instances.begin(); it!=instances.end(); it++) {
			ChannelConfig *instance = *it;
			instance->channels->removeChannelList();
		}
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	return JSP_CALL_NO_VALUE;
}

int ChannelConfig::setSelectedConflictLcn(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
									  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);

	ChannelConfig	*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ChannelConfig);
	ObjectInstance *obj = OBJECT_VALUE_OR_THROW(obj, argv[0], ObjectInstance);
	CSearchedInfo *searchedInfObj = static_cast<CSearchedInfo*>(obj);

	int count = NUMBER_VALUE(argv[1]);

	OOIF_LOG_INFO("conflict lcn (%d)\n", count);

	if (obj->getType() == CLASS_SEARCHEDINFO)
	{
		OPL_SEARCHEDINFO_t *selectedInfos = (OPL_SEARCHEDINFO_t*)HLIB_STD_MemAlloc(sizeof(OPL_SEARCHEDINFO_t) * count);

		if (selectedInfos)
		{
			for (int i = 0; i < count; i++)
			{
				humax::SearchedInfo_Data info = searchedInfObj->getInfo(i);
				selectedInfos[i].onId = info.onId;
				selectedInfos[i].tsId = info.tsId;
				selectedInfos[i].svcId = info.svcId;
				selectedInfos[i].channelType = info.channelType;
				selectedInfos[i].casType = info.casType;
				selectedInfos[i].lcn = info.lcn;
				strncpy(selectedInfos[i].name, info.name, MAX_INFO_LEN-1); selectedInfos[i].name[MAX_INFO_LEN-1] = '\0';


				OOIF_LOG_INFO("Selected conflict lcn info (%d, %d, %d, %d, %d, %d, %s)\n",
								selectedInfos[i].onId, selectedInfos[i].tsId, selectedInfos[i].svcId,
								selectedInfos[i].channelType, selectedInfos[i].casType, selectedInfos[i].lcn, selectedInfos[i].name);
			}

			OPL_Builder_SetSelectedConflictLcn(selectedInfos, count);

			HLIB_STD_MemFree(selectedInfos);
		}

	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

	HLIB_STD_MemFree(instance->searchedInfos);
	return JSP_CALL_NO_VALUE;
}

#endif
