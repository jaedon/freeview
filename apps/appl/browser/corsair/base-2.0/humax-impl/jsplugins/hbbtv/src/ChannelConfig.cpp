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
#include "ChannelScanOptions.h"
#include "ChannelScanParameters.h"
#include "ChannelScanEvent.h"
#endif // OIPF

#include "Exception.h"

#include "macros.h"

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
			RETURN_OBJECT(obj, new ChannelList(channels), result, JSP_GET_VALUE_CACHE);
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
			RETURN_OBJECT(obj, new FavouriteList(favlist), result, JSP_GET_VALUE);
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
			RETURN_OBJECT(obj, new FavouriteListCollection(), result, JSP_GET_VALUE_CACHE);
		}
		else
			RETURN_NULL(JSP_GET_VALUE);
	}

	NATIVE_GET_OBJECT("currentChannel", obj,
	                  native_ChannelConfigGetCurrentChannel(&RETVAL),
	                  new Channel(RETVAL),
	                  JSP_GET_VALUE);

	GET_FUNCTION(createFilteredList, obj, "---s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startScan, obj, "oo", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopScan, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelList, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelObject, obj, "nnnnns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelScanParametersObject, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createChannelScanOptionsObject, obj, "", JSP_GET_VALUE_CACHE);

	INTRINSIC_EVENT_GET(ChannelScan);
	INTRINSIC_EVENT_GET(ChannelListUpdate);

	return EventTarget::getter(obj, name, result);
#else
	return JSP_GET_NOTFOUND;
#endif // OIPF
}

int ChannelConfig::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef OIPF
	INTRINSIC_EVENT(ChannelScan);
	INTRINSIC_EVENT(ChannelListUpdate);
#endif // OIPF
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
	ChannelList *base;

	if (strlen(STRING_VALUE(argv[3])) > 0 &&  TYPE_BOOLEAN(argv[1]) && BOOLEAN_VALUE(argv[1]))
	{
		FavouriteListHandle handle = NULL;
		FavouriteListHandle *handles = NULL;
		int count;
		NATIVE_CALL(native_ChannelConfigGetFavouriteLists(&count, &handles));
		for (int i=0; i<count; i++)
		{
			const char *id;
			NATIVE_WARN_NOK(native_FavouriteListGetFavID(handles[i], &id));
			if (!strcmp(id, STRING_VALUE(argv[3])))
			{
				handle = handles[i];
				break;
			}
		}
		base = new FavouriteList(handle);
		HLIB_STD_MemFree(handles);
	}
	else
	{
		base = new ChannelList(ALL);
	}

	ChannelList *answer = new ChannelList(base,
	                                      TYPE_BOOLEAN(argv[0]), BOOLEAN_VALUE(argv[0]),
	                                      TYPE_BOOLEAN(argv[1]), BOOLEAN_VALUE(argv[1]),
	                                      TYPE_BOOLEAN(argv[2]), BOOLEAN_VALUE(argv[2]));
	delete base;

	RETURN_OBJECT(this_obj, answer, result, JSP_CALL_VALUE);
}

//static
int ChannelConfig::startScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	ChannelScanOptions* ch_scan_opts = OBJECT_VALUE_OR_THROW(ch_scan_opts, argv[0], ChannelScanOptions);
	ChannelScanParameters* ch_scan_params = OBJECT_VALUE_OR_THROW(ch_scan_params, argv[1], ChannelScanParameters);
	OOIFNumber retval = 0;
	ScanParameters scan_params;
	ScanOptions scan_options;

	scan_options.channelType = ch_scan_opts->getNumber("channelType");
	scan_options.replaceExisting = ch_scan_opts->getBoolean("replaceExisting");
	switch (ch_scan_params->type())
	{
	case ChannelScanParameters::CSP_DVBT:
	{
		DVBTChannelScanParameters* dvbt_ch_scan_params = reinterpret_cast<DVBTChannelScanParameters*>(ch_scan_params);
		scan_params.scan_parameters_dvbt.startFrequency = dvbt_ch_scan_params->getNumber("startFrequency");
		scan_params.scan_parameters_dvbt.endFrequency = dvbt_ch_scan_params->getNumber("endFrequency");
		scan_params.scan_parameters_dvbt.raster = dvbt_ch_scan_params->getNumber("raster");
		scan_params.scan_parameters_dvbt.ofdm = dvbt_ch_scan_params->getString("ofdm");
		scan_params.scan_parameters_dvbt.modulationModes = dvbt_ch_scan_params->getNumber("modulationModes");
		scan_params.scan_parameters_dvbt.bandwidth = dvbt_ch_scan_params->getString("bandwidth");
		NATIVE_CALL(native_ChannelConfigStartScan(&scan_options, SCAN_PARAMETERS_DVBT, &scan_params, &retval));
		break;
	}
	case ChannelScanParameters::CSP_DVBS:
	{
		DVBSChannelScanParameters* dvbs_ch_scan_params = reinterpret_cast<DVBSChannelScanParameters*>(ch_scan_params);
		scan_params.scan_parameters_dvbs.startFrequency = dvbs_ch_scan_params->getNumber("startFrequency");
		scan_params.scan_parameters_dvbs.endFrequency = dvbs_ch_scan_params->getNumber("endFrequency");
		scan_params.scan_parameters_dvbs.modulationModes = dvbs_ch_scan_params->getNumber("modulationModes");
		scan_params.scan_parameters_dvbs.symbolRate = dvbs_ch_scan_params->getString("symbolRate");
		scan_params.scan_parameters_dvbs.polarisation = dvbs_ch_scan_params->getNumber("polarisation");
		scan_params.scan_parameters_dvbs.codeRate = dvbs_ch_scan_params->getString("codeRate");
		scan_params.scan_parameters_dvbs.orbitalPosition = dvbs_ch_scan_params->getNumber("orbitalPosition");
		if (dvbs_ch_scan_params->property_type("networkId") == JSP_TYPE_NUMBER)
		{
			scan_params.scan_parameters_dvbs.networkId = TRUE;
			scan_params.scan_parameters_dvbs.networkId = dvbs_ch_scan_params->getNumber("networkId");
		}
		else
		{
			scan_params.scan_parameters_dvbs.networkId = FALSE;
			scan_params.scan_parameters_dvbs.networkId = 0;
		}
		NATIVE_CALL(native_ChannelConfigStartScan(&scan_options, SCAN_PARAMETERS_DVBS, &scan_params, &retval));
		break;
	}
	case ChannelScanParameters::CSP_DVBC:
	{
		DVBCChannelScanParameters* dvbc_ch_scan_params = reinterpret_cast<DVBCChannelScanParameters*>(ch_scan_params);
		scan_params.scan_parameters_dvbc.startFrequency = dvbc_ch_scan_params->getNumber("startFrequency");
		scan_params.scan_parameters_dvbc.endFrequency = dvbc_ch_scan_params->getNumber("endFrequency");
		scan_params.scan_parameters_dvbc.raster = dvbc_ch_scan_params->getNumber("raster");
		scan_params.scan_parameters_dvbc.startNetworkScanOnNIT = dvbc_ch_scan_params->getBoolean("startNetworkScanOnNIT");
		scan_params.scan_parameters_dvbc.modulationModes = dvbc_ch_scan_params->getNumber("modulationModes");
		scan_params.scan_parameters_dvbc.symbolRate = dvbc_ch_scan_params->getString("symbolRate");
		if (dvbc_ch_scan_params->property_type("networkId") == JSP_TYPE_NUMBER)
		{
			scan_params.scan_parameters_dvbc.networkId = TRUE;
			scan_params.scan_parameters_dvbc.networkId = dvbc_ch_scan_params->getNumber("networkId");
		}
		else
		{
			scan_params.scan_parameters_dvbc.networkId = FALSE;
			scan_params.scan_parameters_dvbc.networkId = 0;
		}
		NATIVE_CALL(native_ChannelConfigStartScan(&scan_options, SCAN_PARAMETERS_DVBC, &scan_params, &retval));
		break;
	}
	}
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int ChannelConfig::stopScan(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL(native_ChannelConfigStopScan());
	return JSP_CALL_NO_VALUE;
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

	CHECK_ARGUMENT_COUNT(1);
	OOIFNumber id = NUMBER_VALUE(argv[0]);

	if(id == CHANNEL_ID_DVB_C || id == CHANNEL_ID_DVB_C2)
	{
		RETURN_OBJECT(this_obj, new DVBCChannelScanParameters(), result, JSP_CALL_VALUE);
	}

	if(id == CHANNEL_ID_DVB_S || id == CHANNEL_ID_DVB_S2)
	{
		RETURN_OBJECT(this_obj, new DVBSChannelScanParameters(), result, JSP_CALL_VALUE);
	}

	if(id == CHANNEL_ID_DVB_T || id == CHANNEL_ID_DVB_T2)
	{
		RETURN_OBJECT(this_obj, new DVBTChannelScanParameters(), result, JSP_CALL_VALUE);
	}

	RETURN_NULL(JSP_CALL_VALUE);
}

//static
void ChannelConfig::onChannelConfigEvent(ChannelConfigEvent event, ChannelConfigEventInfo *info)
{
	for (list<ChannelConfig*>::iterator it=instances.begin(); it != instances.end(); it++)
	{
		ChannelConfig* ch_config = *it;
		Event* e = NULL;
		switch(event)
		{
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
		case CHANNEL_CONFIG_EVENT_CHANNEL_LIST_UPDATE:
		{
			e = new EmptyEvent(ch_config->getHost(), "ChannelListUpdateEvent");
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
#endif // OIPF
