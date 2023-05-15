// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "VideoBroadcast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "OOIFUtils.h"

#include "macros.h"

#include "NativeVideoBroadcast.h"
#include "NativeCapabilities.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "Channel.h"
#include "Exception.h"
#include "ChannelConfig.h"
#include "AVComponent.h"
#include "Programme.h"

#include "EmptyEvent.h"
#include "PlayStateChangeBroadcastEvent.h"
#include "ChannelChangeSucceedBroadcastEvent.h"
#include "ChannelChangeErrorBroadcastEvent.h"
#include "ParentalRatingChangeEvent.h"
#include "ParentalRatingErrorEvent.h"
#if defined(HBBTV_PVR) || defined(OIPF)
#include "PlaySpeedChangedEvent.h"
#include "PlayPositionChangedEvent.h"
#endif // HBBTV_PVR || OIPF
#include "RecordingEventBroadcastEvent.h"
#include "DRMRightsErrorEventBroadcastEvent.h"
#include "NativeApplicationManager.h"
#include "StreamBroadcastEvent.h"
#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SelectedComponentChangedEvent.h"
#endif // OIPF || HBBTV_1_2_1

//HBBTV_2_0
#include "ComponentChangedEvent.h"

using namespace Ooif;

//static
list<VideoBroadcast*> VideoBroadcast::instances;

VideoBroadcast::VideoBroadcast(VideoBroadcastHandle i) :
	WrapperObject<VideoBroadcast>(i),
	playstate(BROADCAST_PLAYSTATE_UNREALIZED)
{
	ENTRY;
	classType = CLASS_VIDEO_BROADCAST;
	instances.push_back(this);
	native_BroadcastSetPlayState(getIdentifier(), 0);
}

VideoBroadcast::~VideoBroadcast()
{
	instances.remove(this);
}

void VideoBroadcast::addStreamEventListener(const char *streamEventName, jsplugin_obj *listener)
{
	StreamEventListener *new_listener = new StreamEventListener(listener, streamEventName);
	addNewListener(new_listener);
}

void VideoBroadcast::internalRemoveStreamEventListener(const char *streamEventName, jsplugin_obj *listener)
{
	StreamEventListener tmp_listener(listener, streamEventName);
	EventListener *matching_listener = getMatchingListener(tmp_listener);
	if (matching_listener)
	{
		OOIF_LOG_DEBUG("removing stream event listener on %s", streamEventName);
		removeListener(matching_listener);
	}
}

bool VideoBroadcast::isAddStreamEventListenerAllowed()
{
#ifdef HBBTV_1_2_1
	return true;//(playstate == BROADCAST_PLAYSTATE_PRESENTING || playstate == BROADCAST_PLAYSTATE_STOPPED);
#else
	return true;
#endif
}

void VideoBroadcast::removeAllStreamEventListeners()
{
	NATIVE_WARN_NOK(native_BroadcastRemoveAllStreamEventListeners(getIdentifier()));
	removeAllListeners(StreamBroadcastEvent::STREAM_EVENT_NAME);
}

int VideoBroadcast::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	// The video/broadcast embedded object
#ifdef HBBTV_1_2_1
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("fullScreen", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
#endif // HBBTV_1_2_1

	GET_BOOLEAN("fullScreen", isFullScreen(), JSP_GET_VALUE);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	NATIVE_GET_STRING("data", native_BroadcastGetData(identifier, &RETVAL), JSP_GET_VALUE);
#endif // OIPF || HBBTV_1_2_1
	NATIVE_GET_NUMBER("playState", native_BroadcastGetPlayState(identifier, &RETVAL), JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(ChannelChangeError);
	INTRINSIC_EVENT_GET(PlayStateChange);
	INTRINSIC_EVENT_GET(ChannelChangeSucceeded);
	INTRINSIC_EVENT_GET(FullScreenChange);

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("getChannelConfig", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("bindToCurrentChannel", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("prevChannel", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("nextChannel", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_FUNCTION(getChannelConfig, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(bindToCurrentChannel, obj, "", JSP_GET_VALUE_CACHE);
	// Note that this encompasses both instances of the function
	GET_FUNCTION(createChannelObject, obj, "n-----", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setChannel, obj, "o---", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(prevChannel, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(nextChannel, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setFullScreen, obj, "b", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	GET_FUNCTION(setVolume, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVolume, obj, "", JSP_GET_VALUE_CACHE);
#endif // OIPF
	GET_FUNCTION(release, obj, "", JSP_GET_VALUE_CACHE);

	// Extensions to video/broadcast for recording and time-shift
#if defined(HBBTV_PVR) || defined(OIPF)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("POSITION_START", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("POSITION_CURRENT", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("POSITION_END", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("playbackOffset", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("maxOffset", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("playSpeeds", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("recordNow", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("stopRecording", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("pause", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("resume", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("setSpeed", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("seek", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("stopTimeshift", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_NUMBER("POSITION_START", BROADCAST_POSITION_START, JSP_GET_VALUE_CACHE);
	GET_NUMBER("POSITION_CURRENT", BROADCAST_POSITION_CURRENT, JSP_GET_VALUE_CACHE);
	GET_NUMBER("POSITION_END", BROADCAST_POSITION_END, JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(PlaySpeedChanged);
	INTRINSIC_EVENT_GET(PlayPositionChanged);
	NATIVE_GET_NUMBER("playbackOffset", native_BroadcastGetPlaybackOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maxOffset", native_BroadcastGetMaxOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBER("timeShiftMode", native_BroadcastGetTimeShiftMode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("currentTimeShiftMode", native_BroadcastGetCurrentTimeShiftMode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(PlaySpeedsArrayChanged);
#endif // OIPF
	NATIVE_GET_NUMBERS_COLLECTION("playSpeeds", obj,
	                              native_BroadcastGetPlaySpeeds(identifier, &SIZE, &NATIVE_ARRAY),
	                              getPlaySpeeds(SIZE, NATIVE_ARRAY),
	                              JSP_GET_VALUE);
	GET_FUNCTION(recordNow, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopRecording, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(pause, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(resume, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setSpeed, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(seek, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopTimeshift, obj, "", JSP_GET_VALUE_CACHE);
#endif // HBBTV_PVR || OIPF

	// Extensions to video/broadcast for recording and time-shift (if not full PVR option is supported)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("playPosition", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("playSpeed", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("recordingState", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_NUMBER("playPosition", native_BroadcastGetPlayPosition(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playSpeed", native_BroadcastGetPlaySpeed(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recordingState", native_BroadcastGetRecordingState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(RecordingEvent);

	// Extensions to video/broadcast for access to EIT p/f
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("programmes", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_VOIDP_COLLECTION("programmes", obj,
	                            native_BroadcastGetProgrammes(identifier, &SIZE, &NATIVE_ARRAY),
	                            getProgrammes(SIZE, NATIVE_ARRAY),
	                            JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(ProgrammesChanged);

	// Extensions to video/broadcast for parental ratings errors
	INTRINSIC_EVENT_GET(ParentalRatingChange);
	INTRINSIC_EVENT_GET(ParentalRatingError);

	// Extensions to video/broadcast for DRM rights errors
	INTRINSIC_EVENT_GET(DRMRightsError);

	// Extensions to video/broadcast for current channel information
#ifdef HBBTV_SECURITY
	if (!strcmp("currentChannel", name))
	{
		SECURITY_CHECK(obj, CHECK_BROADCAST, RETURN_NULL(JSP_GET_VALUE));
	}
#endif // HBBTV_SECURITY
	NATIVE_GET_OBJECT("currentChannel", obj,
	                  native_BroadcastGetCurrentChannel(identifier, &RETVAL),
	                  new Channel(RETVAL),
	                  JSP_GET_VALUE);

	// Extensions to video/broadcast for synchronization
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("internalAddStreamEventListener", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("internalAddXMLStreamEventListener", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("removeStreamEventListener", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_FUNCTION(internalAddStreamEventListener, obj, "sso", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(internalAddXMLStreamEventListener, obj, "ssnno", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeStreamEventListener, obj, "sso", JSP_GET_VALUE_CACHE);

	// Extensions to video/broadcast for playback of selected components
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_VIDEO", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_AUDIO", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_SUBTITLE", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getComponents", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getCurrentActiveComponents", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("selectComponent", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("unselectComponent", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_NUMBER("COMPONENT_TYPE_VIDEO", BROADCAST_COMPONENT_TYPE_VIDEO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_AUDIO", BROADCAST_COMPONENT_TYPE_AUDIO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_SUBTITLE", BROADCAST_COMPONENT_TYPE_SUBTITLE, JSP_GET_VALUE_CACHE);

	GET_FUNCTION(getComponents, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getCurrentActiveComponents, obj, "n", JSP_GET_VALUE_CACHE);
#if defined(OIPF) || defined(HBBTV_1_2_1)
	GET_FUNCTION(selectComponent, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unselectComponent, obj, "-", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(SelectedComponentChanged);
#else
	GET_FUNCTION(selectComponent, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(unselectComponent, obj, "o", JSP_GET_VALUE_CACHE);
#endif

	return EventTarget::getter(obj, name, result);
}

int VideoBroadcast::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	SET_READONLY("fullScreen");
#ifdef HBBTV_1_2_1
#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("data", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
#endif // HBBTV_1_2_1
#if defined(OIPF) || defined(HBBTV_1_2_1)
	NATIVE_SET("data", native_BroadcastSetData(identifier, STRING_VALUE(value[0])));
#endif // OIPF || HBBTV_1_2_1
	SET_READONLY("playState");

	INTRINSIC_EVENT(ChannelChangeError);
	INTRINSIC_EVENT(PlayStateChange);
	INTRINSIC_EVENT(ChannelChangeSucceeded);
	INTRINSIC_EVENT(FullScreenChange);

	// Extensions to video/broadcast for recording and time-shift
#if defined(HBBTV_PVR) || defined(OIPF)
	INTRINSIC_EVENT(PlaySpeedChanged);
	INTRINSIC_EVENT(PlayPositionChanged);
#ifdef OIPF
	INTRINSIC_EVENT(PlaySpeedsArrayChanged);
	NATIVE_SET("timeShiftMode", native_BroadcastSetTimeShiftMode(identifier, NUMBER_VALUE(value[0])));
#endif // OIPF
#endif // HBBTV_PVR || OIPF

	// Extension for recording and time shift, see errata 4.8.15.
	INTRINSIC_EVENT(RecordingEvent);

	// Extensions to video/broadcast for access to EIT p/f
	INTRINSIC_EVENT(ProgrammesChanged);

	// Extensions to video/broadcast for parental ratings errors
	INTRINSIC_EVENT(ParentalRatingChange);
	INTRINSIC_EVENT(ParentalRatingError);

	// Extensions to video/broadcast for playback of selected components
#if defined(OIPF) || defined(HBBTV_1_2_1)
	INTRINSIC_EVENT(SelectedComponentChanged);
#endif // OIPF || HBBTV_1_2_1

	// Extensions to video/broadcast for DRM rights errors
	INTRINSIC_EVENT(DRMRightsError);

	return JSP_PUT_NOTFOUND;
}

void VideoBroadcast::attr_change(jsplugin_obj* obj, const char* name, const char* value)
{
	ENTRY;
	INTRINSIC_EVENT_ATTR(ChannelChangeError);
	INTRINSIC_EVENT_ATTR(PlayStateChange);
	INTRINSIC_EVENT_ATTR(ChannelChangeSucceeded);
	INTRINSIC_EVENT_ATTR(FullScreenChange);
	INTRINSIC_EVENT_ATTR(ParentalRatingChange);
	INTRINSIC_EVENT_ATTR(ParentalRatingError);

#ifdef PVR_SUPPORT
	// Extensions to video/broadcast for recording and time-shift
	INTRINSIC_EVENT_ATTR(PlaySpeedChanged);
	INTRINSIC_EVENT_ATTR(PlayPositionChanged);
#endif // PVR_SUPPORT

	INTRINSIC_EVENT_ATTR(RecordingEvent);

	// Extensions to video/broadcast for access to EIT p/f
	INTRINSIC_EVENT_ATTR(ProgrammesChanged);

	INTRINSIC_EVENT_ATTR(DRMRightsError);
}

void VideoBroadcast::videoVisible(int visibility, int unloading)
{
	if (toggleVisibility(visibility))
	{
		NATIVE_WARN_NOK(native_BroadcastSetVisibility(getIdentifier(), visibility, unloading));
	}
}

void VideoBroadcast::videoPosition(int x, int y, int w, int h)
{
	if (resize(x, y, w, h))
	{
		NATIVE_WARN_NOK(native_BroadcastSetVideoWindow(getIdentifier(), x, y, w, h));
	}
}

//static
int VideoBroadcast::createChannelObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 6);

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	ChannelHandle retval = NULL;

	// first check for 3-argument overload
	if (argc == 3 && TYPE_STRING(argv[1]) && TYPE_NUMBER(argv[2]))
	{
		const char *dsd_utf8 = STRING_VALUE(argv[1]);
		int dsd_utf8_len = STRING_LENGTH(argv[1]);

		// dsd is wrapped inside UTF-8, so unpack it to its original binary data
		unsigned char *dsd_data = (unsigned char*)OOIFUtils::getTmpBuffer(dsd_utf8_len+1);
		if (!dsd_data)
			THROW_OOM_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		int dsd_data_len;

		if (!OOIFUtils::unwrapBinaryDataFromUTF8(dsd_data, dsd_data_len, dsd_utf8, dsd_utf8_len))
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

		// null terminate, just to be on the safe side
		dsd_data[dsd_data_len] = 0;

		NATIVE_CALL(native_BroadcastCreateChannelObject3(instance->getIdentifier(),
		            NUMBER_VALUE(argv[0]),
		            dsd_data,
		            dsd_data_len,
		            NUMBER_VALUE(argv[2]),
		            &retval));
	}
	else
	{
		int idType = 0, onid = 0, tsid = 0, sid = 0, sourceID = 0;
		const char* ipBroadcastID = NULL;

		// check all argument types for supplied parameters
		// see 7.13.1.3 in Volume 5 of OIPF spec
		//The type of channel, as indicated by one of the ID_* constants defined in Section 7.13.12.1.
		if (argc > 0 && TYPE_NUMBER(argv[0]))
		{
			idType = NUMBER_VALUE(argv[0]);
		}
		else
		{
			goto arg_error;
		}

		//The original network ID. Optional argument that SHALL be specified when the idType specifies a channel of type ID_DVB_* or ID_ISDB_*.
 		if (argc > 1 && TYPE_NUMBER(argv[1]))
 		{
			onid = NUMBER_VALUE(argv[1]);
 		}
		else if (CHANNEL_ID_DVB_C <= idType && idType <= CHANNEL_ID_ISDB_T)
		{
			goto arg_error;
		}
		//The transport stream ID. Optional argument that MAY be specified when the idType specifies a channel of type ID_DVB_* or ID_ISDB_*.
		if (argc > 2 && TYPE_NUMBER(argv[2]))
		{
			tsid = NUMBER_VALUE(argv[2]);
		}
		else if (CHANNEL_ID_DVB_C <= idType && idType <= CHANNEL_ID_ISDB_T)
		{
			goto arg_error;
		}
		//The service ID. Optional argument that SHALL be specified when the idType specifies a channel of type ID_DVB_* or ID_ISDB_*.
		if (argc > 3 && TYPE_NUMBER(argv[3]))
		{
			sid = NUMBER_VALUE(argv[3]);
		}
		else if (CHANNEL_ID_DVB_C <= idType && idType <= CHANNEL_ID_ISDB_T)
		{
			goto arg_error;
		}
		//The source_ID. Optional argument that SHALL be specified when the idType specifies a channel of type ID_ATSC_T.
		if (argc > 4 && TYPE_NUMBER(argv[4]))
		{
			sourceID = NUMBER_VALUE(argv[4]);
		}
		else if (idType == CHANNEL_ID_ATSC_T)
		{
			goto arg_error;
		}
		//The DVB textual service identifier of the IP broadcast service, specified in the format ��ServiceName.DomainName��, or the URI of the IP broadcast service.
		//Optional argument that SHALL be specified when the idType specifies a channel of type ID_IPTV_SDS or ID_IPTV_URI.
		if (argc > 5 && TYPE_STRING(argv[5]))
		{
			ipBroadcastID = STRING_VALUE(argv[5]);
		}
		else if (idType == CHANNEL_ID_IPTV_SDS || idType == CHANNEL_ID_IPTV_URI)
		{
			goto arg_error;
		}

		NATIVE_CALL(native_BroadcastCreateChannelObject6(instance->getIdentifier(),
														 idType,
														 argc >= 2, onid,
														 argc >= 3, tsid,
														 argc >= 4, sid,
														 argc >= 5, sourceID,
														 ipBroadcastID,
														 &retval));
	}

	RETURN_OBJECT(this_obj, new Channel(retval), result, JSP_CALL_VALUE);
	arg_error:
	THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
}

//static
int VideoBroadcast::getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean managed;
	NATIVE_WARN_NOK(native_ChannelConfigIsChannelListManaged(&managed));
	if (managed)
	{
		RETURN_OBJECT(this_obj, new ChannelConfig(DEFAULT), result, JSP_CALL_VALUE);
	}
	else
	{
		RETURN_NULL(JSP_CALL_VALUE);
	}
}

//static
int VideoBroadcast::bindToCurrentChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
#ifdef OIPF
	ChannelHandle channel;
	NATIVE_CALL(native_BroadcastBindToCurrentChannel(instance->getIdentifier(), &channel));
	RETURN_OBJECT(this_obj, new Channel(channel), result, JSP_CALL_VALUE);
#elif defined(HBBTV)
	NATIVE_CALL(native_BroadcastBindToCurrentChannel(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
#endif // HBBTV
}

typedef enum {
	PROP_GET_SID = 1,
	PROP_GET_ONID = 2,
	PROP_GET_TSID = 3,
} prop_cmd;

typedef struct _channel_userdata_t {
    jsplugin_obj 	*this_obj;
    jsplugin_obj 	*arg_obj;
    jsplugin_obj 	*global_context;
	void 			*plugin_private;

	prop_cmd 	cmd;
	double 		sid;
	double 		onid;
	double 		tsid;
} channel_userdata_t;

static void __get_data(int status, jsplugin_value *return_value, void *user_data)
{
	if( status == JSP_CALLBACK_OK && return_value->type == JSP_TYPE_NUMBER){
		jsplugin_obj *obj = (jsplugin_obj *)user_data;
		channel_userdata_t *userdata= (channel_userdata_t *) obj->plugin_private;

		switch (userdata->cmd){
			case PROP_GET_SID :
				userdata->sid  = return_value->u.number;
				break;
			case PROP_GET_ONID :
				userdata->onid = return_value->u.number;
				break;
			case PROP_GET_TSID :
				userdata->tsid  = return_value->u.number;
				break;

			default: break;
		}
	}
	g_opera_callbacks->resume((jsplugin_obj *)user_data);
}

//static
int VideoBroadcast::setChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	/*  resume  */
	if (argc == -1) {
		if(this_obj->plugin_private)
		{
			channel_userdata_t *userdata=  (channel_userdata_t *) this_obj->plugin_private;

			if( userdata->onid < 0 ){
				userdata->cmd = PROP_GET_ONID;
	    		g_opera_callbacks->get_property(userdata->global_context, userdata->arg_obj, "onid", this_obj, &__get_data);
				return JSP_CALL_DELAYED;
			}

			if( userdata->tsid < 0 ){
				userdata->cmd = PROP_GET_TSID;
	    		g_opera_callbacks->get_property(userdata->global_context, userdata->arg_obj, "tsid", this_obj, &__get_data);
				return JSP_CALL_DELAYED;
			}

		 	ChannelHandle ch = NULL;

			this_obj->plugin_private = userdata->plugin_private;
			VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

			native_BroadcastFindChannelByTriplet(CHANNEL_TYPE_TV, userdata->onid, userdata->tsid, userdata->sid, &ch);
			HLIB_STD_MemFree(userdata);
			if(ch) 	NATIVE_CALL(native_BroadcastSetChannel(instance->getIdentifier(), ch, FALSE, FALSE, NULL, FALSE, 0));
		}

		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

#if defined(HBBTV_PVR) || defined(OIPF)
	CHECK_ARGUMENT_COUNT_MIN_MAX(1,4);
#else
	CHECK_ARGUMENT_COUNT_MIN_MAX(1,3);
#endif // HBBTV_PVR || OIPF


	if (argv[0].type == JSP_TYPE_NATIVE_OBJECT)
	{
 		channel_userdata_t *userdata = (channel_userdata_t *)HLIB_STD_MemAlloc(sizeof(channel_userdata_t));
    	memset(userdata, 0, sizeof(channel_userdata_t));

		long int win_id = OOIFUtils::getWindow(this_obj);

		userdata->sid = userdata->onid = userdata->tsid = -1;
  		userdata->plugin_private = this_obj->plugin_private;
  		userdata->global_context = OOIFUtils::getGlobalContext(win_id);
    	userdata->this_obj = this_obj;
    	userdata->arg_obj = argv[0].u.object;

		this_obj->plugin_private = userdata;

		userdata->cmd = PROP_GET_SID;
	    g_opera_callbacks->get_property(userdata->global_context, argv[0].u.object, "sid", this_obj, &__get_data);

		return JSP_CALL_DELAYED;
	}


	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	ChannelHandle channel_handle = 0;
	if (argv[0].type != JSP_TYPE_NULL)
	{
		Channel *channel = OBJECT_VALUE_OR_THROW(channel, argv[0],Channel);
		channel_handle = channel->getIdentifier();
	}
	OOIFBoolean has_trickplay = (argc >= 2 && TYPE_BOOLEAN(argv[1]));
	OOIFBoolean trickplay = has_trickplay ? BOOLEAN_VALUE(argv[1]) : FALSE;
	const char *cad = (argc >= 3 && TYPE_STRING(argv[2])) ? STRING_VALUE(argv[2]) : NULL;

#if defined(HBBTV_PVR) || defined(OIPF)
	OOIFNumber has_offset = (argc >= 4 && TYPE_NUMBER(argv[3]));
	OOIFNumber offset = has_offset ? NUMBER_VALUE(argv[3]) : 0;
	NATIVE_CALL(native_BroadcastSetChannel(instance->getIdentifier(), channel_handle,
	                                       has_trickplay, trickplay, cad, has_offset, offset));
#else
	NATIVE_CALL(native_BroadcastSetChannel(instance->getIdentifier(), channel_handle,
	                                       has_trickplay, trickplay, cad, FALSE, 0));
#endif // HBBTV_PVR || OIPF

#ifdef HBBTV
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	NATIVE_WARN_NOK(native_AppManNotifySetChannelCalled(window, host, channel_handle));
#endif //HBBTV
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::prevChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastPrevChannel(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::nextChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastNextChannel(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                         int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastStop(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::setFullScreen(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	SECURITY_CHECK( this_obj, CHECK_BROADCAST, return JSP_CALL_NO_VALUE);
#endif // HBBTV_SECURITY
	return VisualObject::setFullScreen(this_obj, function_obj, argc, argv, result);
}

OOIFReturnCode VideoBroadcast::updateNativeWindowState(WindowState requested_state)
{
	return native_BroadcastSetFullScreen(getIdentifier(), requested_state == FULLSCREEN);
}

#ifdef OIPF
//static
int VideoBroadcast::setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                              int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastSetVolume(instance->getIdentifier(), NUMBER_VALUE(argv[0]), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                              int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFNumber retval;
	NATIVE_CALL(native_BroadcastGetVolume(instance->getIdentifier(), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}
#endif // OIPF

//static
int VideoBroadcast::release(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                            int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	SECURITY_CHECK( this_obj, CHECK_BROADCAST, return JSP_CALL_NO_VALUE);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	NATIVE_CALL(native_BroadcastRelease(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

#if defined(HBBTV_PVR) || defined(OIPF)
//static
int VideoBroadcast::recordNow(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                              int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	const char *retval;
	NATIVE_CALL(native_BroadcastRecordNow(instance->getIdentifier(), NUMBER_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::stopRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastStopRecording(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                          int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastPause(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::resume(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                           int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastResume(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::setSpeed(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                             int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastSetSpeed(instance->getIdentifier(),
	                                     NUMBER_VALUE(argv[0]),
	                                     &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                         int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(2);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastSeek(instance->getIdentifier(), NUMBER_VALUE(argv[0]),
	                                 (BroadcastPosition)argv[1].u.number, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::stopTimeshift(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastStopTimeshift(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // HBBTV_PVR || OIPF

//static
int VideoBroadcast::getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	int count = 0;
	AVComponentHandle *handles = NULL;
	NATIVE_CALL(native_BroadcastGetComponents(instance->getIdentifier(),
	            (BroadcastComponentType)argv[0].u.number,
	            &count, &handles));
	AVComponentCollection *components = new AVComponentCollection();
	for (int i=0; i<count; i++)
	{
		components->addItem(new AVComponent(handles[i], instance->classType));
	}
	HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, components, result, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::getCurrentActiveComponents(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	int count = 0;
	AVComponentHandle *handles = NULL;
	NATIVE_CALL(native_BroadcastGetCurrentActiveComponents(instance->getIdentifier(),
	            (BroadcastComponentType)argv[0].u.number,
	            &count, &handles));
	AVComponentCollection *components = new AVComponentCollection();
	for (int i=0; i<count; i++)
	{
		components->addItem(new AVComponent(handles[i], instance->classType));
	}
	HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, components, result, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::selectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                    int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (TYPE_NUMBER(argv[0]))
	{
		NATIVE_CALL(native_BroadcastSelectComponent2(instance->getIdentifier(), NUMBER_VALUE(argv[0])));
		return JSP_CALL_NO_VALUE;
	}
#endif // OIPF || HBBTV_1_2_1
	AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);
	NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::unselectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                      int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (TYPE_NUMBER(argv[0]))
	{
		NATIVE_CALL(native_BroadcastUnselectComponent2(instance->getIdentifier(), NUMBER_VALUE(argv[0])));
		return JSP_CALL_NO_VALUE;
	}
#endif // OIPF || HBBTV_1_2_1
	AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);
	NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::internalAddStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);
	CHECK_ARGUMENT(2, JSP_TYPE_NATIVE_OBJECT);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	jsplugin_obj *listener = argv[2].u.object;

	if (!instance->isAddStreamEventListenerAllowed())
	{
		OOIF_LOG_DEBUG("Ignoring call to addStreamEvent listener due to playstate");
		return JSP_CALL_NO_VALUE;
	}

	long int window_identifier = OOIFUtils::getWindow(this_obj);
	const char *event_name = STRING_VALUE(argv[1]);
	NATIVE_CALL(native_BroadcastAddStreamEventListener(STRING_VALUE(argv[0]),
	            event_name,
	            (void*)listener->opera_private,
	            VideoBroadcast::onVideoBroadcastStreamEvent,
	            instance->getIdentifier(),
	            window_identifier));
	instance->addStreamEventListener(event_name, listener);
	return JSP_CALL_NO_VALUE;
}

//static
int VideoBroadcast::internalAddXMLStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(5);
	CHECK_ARGUMENT(4, JSP_TYPE_NATIVE_OBJECT);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	jsplugin_obj *listener = argv[4].u.object;

	if (!instance->isAddStreamEventListenerAllowed())
	{
		OOIF_LOG_DEBUG("Ignoring call to addStreamEvent listener due to playstate");
		return JSP_CALL_NO_VALUE;
	}

	long int window_identifier = OOIFUtils::getWindow(this_obj);
	const char *event_name = STRING_VALUE(argv[1]);
	NATIVE_CALL(native_BroadcastAddXMLStreamEventListener(STRING_VALUE(argv[0]),
	            event_name,
	            NUMBER_VALUE(argv[2]),
	            NUMBER_VALUE(argv[3]),
	            (void*)listener->opera_private,
	            VideoBroadcast::onVideoBroadcastStreamEvent,
	            instance->getIdentifier(),
	            window_identifier));
	instance->addStreamEventListener(event_name, listener);
	return JSP_CALL_NO_VALUE;
}


//static
int VideoBroadcast::removeStreamEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);
	CHECK_ARGUMENT(2, JSP_TYPE_NATIVE_OBJECT);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	jsplugin_obj *listener = argv[2].u.object;

	const char *event_name = STRING_VALUE(argv[1]);
	OOIFReturnCode ret = native_BroadcastRemoveStreamEventListener(STRING_VALUE(argv[0]),
	            event_name,
	                     											(void*)listener->opera_private);
	if (ret == OOIF_RETURN_OK)
	instance->internalRemoveStreamEventListener(event_name, listener);
	
	return JSP_CALL_NO_VALUE;
}

//static
VideoBroadcast* VideoBroadcast::GetInstance(VideoBroadcastHandle id)
{
	for (list<VideoBroadcast*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		if ((*it)->getIdentifier() == id)
			return *it;
	}
	return NULL;
}

// Help functions for collectins
ProgrammeCollection *VideoBroadcast::getProgrammes(int count, ProgrammeHandle *handles)
{
	ProgrammeCollection *pc = new ProgrammeCollection();
	for (int i=0; i<count; i++)
	{
		pc->addItem(new Programme(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return pc;
}

#if defined(HBBTV_PVR) || defined(OIPF)
NumberCollection *VideoBroadcast::getPlaySpeeds(int count, OOIFNumber *speeds)
{
	NumberCollection *collection = new NumberCollection();
	for (int i=0; i<count; i++)
	{
		collection->addItem(speeds[i]);
	}
	HLIB_STD_MemFree(speeds);
	return collection;
}

#endif // HBBTV_PVR || OIPF

OOIFReturnCode VideoBroadcast::checkListenerAccessAllowed(const char *eventName)
{
#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("ParentalRatingChange", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("ParentalRatingError", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("PlaySpeedChange", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("PlayPositionChange", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("RecordingEvent", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("ProgrammesChanged", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("FullScreenChange", CHECK_BROADCAST);
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("SelectedComponentChanged", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	return native_BroadcastOnListenerAccessed(identifier, eventName);
}

//static
void VideoBroadcast::onVideoBroadcastEvent(BroadcastEvent event, BroadcastEventInfo *info, VideoBroadcastHandle id)
{
	VideoBroadcast* vb = GetInstance(id);

	if (!vb)
	{
		OOIF_LOG_WARNING("Unknown VideoBroadcastdHandle received");
		return;
	}

	Event *e = NULL;
	jsplugin_obj *context = vb->getHost();

	switch (event)
	{
	case BROADCAST_EVENT_CHANNEL_CHANGE_ERROR:
		OOIF_LOG_DEBUG("ChannelChangeError event received");
		e = new ChannelChangeErrorBroadcastEvent(context,
		        info->channel_change_error_data.channel,
		        info->channel_change_error_data.error_state);
		break;
	case BROADCAST_EVENT_PLAYSTATE_CHANGE:
	{
		OOIF_LOG_DEBUG("PlayStateChange event received");
#ifdef HBBTV
		if (info->playstate_change_data.state == BROADCAST_PLAYSTATE_UNREALIZED ||
		        ((!info->playstate_change_data.has_error) &&
		         (info->playstate_change_data.state == BROADCAST_PLAYSTATE_CONNECTING) &&
		         (vb->playstate != BROADCAST_PLAYSTATE_UNREALIZED)))
		{
			OOIF_LOG_DEBUG("Removing all StreamEvent listeners on video broadcast object due to playstate changed");
			vb->removeAllStreamEventListeners();
		}
#endif //HBBTV
		vb->playstate = info->playstate_change_data.state;
		e = new PlayStateChangeBroadcastEvent(context,
		                                      info->playstate_change_data.state,
		                                      info->playstate_change_data.has_error,
		                                      info->playstate_change_data.error
		                                     );
		break;
	}
	case BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED:
		OOIF_LOG_DEBUG("ChannelChangeSucceed event received");
		e = new ChannelChangeSucceedBroadcastEvent(context, info->channel_change_succeed_data.channel);
		break;
	case BROADCAST_EVENT_FULLSCREEN_CHANGE:
		OOIF_LOG_DEBUG("FullScreenChange event received");
		e = new EmptyEvent(context, "FullScreenChange");
		break;
	case BROADCAST_EVENT_PARENTAL_RATING_CHANGE:
		OOIF_LOG_DEBUG("ParentalRatingChange event received");
		e = new ParentalRatingChangeEvent(context,
		                                  info->parental_rating_change_data.contentID,
#if defined(OIPF) || defined(HBBTV_1_2_1)
		                                  info->parental_rating_change_data.ratings,
		                                  info->parental_rating_change_data.ratings_count,
#else
		                                  info->parental_rating_change_data.rating,
#endif
		                                  info->parental_rating_change_data.DRMSystemID
#if defined(OIPF) || defined(HBBTV_1_2_1)
		                                  ,
		                                  info->parental_rating_change_data.blocked
#endif // OIPF || HBBTV_1_2_1
		                                 );
		break;
	case BROADCAST_EVENT_PARENTAL_RATING_ERROR:
		OOIF_LOG_DEBUG("ParentalRatingError event received");
		e = new ParentalRatingErrorEvent(context,
		                                 info->parental_rating_error_data.contentID,
#if defined(OIPF) || defined(HBBTV_1_2_1)
		                                 info->parental_rating_error_data.ratings,
		                                 info->parental_rating_error_data.ratings_count,
#else
		                                 info->parental_rating_error_data.rating,
#endif
		                                 info->parental_rating_error_data.DRMSystemID
		                                );
		break;
	case BROADCAST_EVENT_PROGRAMMES_CHANGED:
		OOIF_LOG_DEBUG("ProgrammesChanged event received");
		e = new EmptyEvent(context, "ProgrammesChanged");
		break;
	case BROADCAST_EVENT_RECORDING_EVENT:
		OOIF_LOG_DEBUG("RecordingEvent event received");
		e = new RecordingEventBroadcastEvent(context,
		                                     info->recording_event_data.state,
		                                     info->recording_event_data.has_error,
		                                     info->recording_event_data.error,
		                                     info->recording_event_data.recordingId
		                                    );
		break;
#if defined(HBBTV_PVR) || defined(OIPF)
	case BROADCAST_EVENT_PLAY_SPEED_CHANGED:
		OOIF_LOG_DEBUG("PlaySpeedChanged event received");
		e = new PlaySpeedChangedEvent(context, info->play_speed_changed_data.speed);
		break;
	case BROADCAST_EVENT_PLAY_POSITION_CHANGED:
		OOIF_LOG_DEBUG("PlayPositionChanged event received");
		e = new PlayPositionChangedEvent(context,
		                                 info->play_position_changed_data.has_position,
		                                 info->play_position_changed_data.position
		                                );
		break;
#ifdef OIPF
	case BROADCAST_EVENT_PLAY_SPEEDS_ARRAY_CHANGED:
		OOIF_LOG_DEBUG("PlaySpeedsArrayChanged event received");
		e = new EmptyEvent(context, "PlaySpeedsArrayChanged");
		break;
#endif // OIPF
#endif // HBBTV_PVR || OIPF
	case BROADCAST_EVENT_DRM_RIGHTS_ERROR_EVENT:
		OOIF_LOG_DEBUG("DRMRightsError event received");
		e = new DRMRightsErrorEventBroadcastEvent(context,
		        info->drm_rights_error_data.error_state,
		        info->drm_rights_error_data.content_id,
		        info->drm_rights_error_data.drm_system_id,
		        info->drm_rights_error_data.rights_issuer_url
		                                         );
		break;
#if defined(OIPF) || defined(HBBTV_1_2_1)
	case BROADCAST_EVENT_SELECTED_COMPONENT_CHANGED:
		OOIF_LOG_DEBUG("SelectedComponentChanged event received");
		// OIPF DAE 7.16.5.1.2 Properties
		e = new SelectedComponentChangedEvent(context,
		                                      info->selected_component_changed_data.componentType,
		                                      "SelectedComponentChanged"
		                                      );
		vb->dispatchEvent(e);
		//OIPF DAE 7.16.5.1.4 Events
		e = new SelectedComponentChangedEvent(context,
		                                      info->selected_component_changed_data.componentType,
		                                      "SelectedComponentChange"
		                                      );
		break;
#endif // OIPF || HBBTV_1_2_1

	// HBBTV_2_0
	case BROADCAST_EVENT_COMPONENT_CHANGED:
		OOIF_LOG_DEBUG("ComponentChanged event received");
		e = new ComponentChangedEvent(context,
                                      info->component_changed_data.componentType
                                      );
		break;

	default:
		OOIF_LOG_WARNING("Unknown event received");
		return;
	}
	vb->dispatchEvent(e);
}

//static
void VideoBroadcast::onVideoBroadcastStreamEvent(const char *name,
        void* listener_id,
        const char *data,
        const char *text,
        const char *status,
        VideoBroadcastHandle id)
{
	VideoBroadcast* vb = GetInstance(id);
	if (!vb)
	{
		OOIF_LOG_WARNING("Unknown VideoBroadcastdHandle received");
		return;
	}

	jsplugin_obj *context = vb->getHost();
	Event *e = new StreamBroadcastEvent(context, name, listener_id, data, text, status);
	vb->dispatchEvent(e);
}

void VideoBroadcast::releaseNative(VideoBroadcastHandle id)
{
	ENTRY;

#ifdef HBBTV
	NATIVE_WARN_NOK(native_BroadcastRemoveAllStreamEventListeners(id));
#endif // HBBTV

	NATIVE_WARN_NOK(native_BroadcastReleaseHandle(id));
}
