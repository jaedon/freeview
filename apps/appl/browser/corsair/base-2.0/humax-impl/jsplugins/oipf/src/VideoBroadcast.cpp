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

#include "oplbroadcast.h"

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

#if defined(HMX_WEBUI)
#include "CasMessageNotifyBroadcastEvent.h"
#include "CasUiNotifyBroadcastEvent.h"
#include "RctNotifyBroadcastEvent.h"
#include "NCDNotifyBroadcastEvent.h"
#include "NetworkUriNotifyBroadcastEvent.h"
#include "RequestPincodeEvent.h"
#include "RequestMhegTuneEvent.h"
#include "RequestMhegInterruptEvent.h"
#include "MhegIcsStatusEvent.h"
#include "TsrStateChangedEvent.h"
#endif // HMX_WEBUI

#ifdef JLABS
#include "jlabsMultiViewComponent.h"
#include "jlabsDataContentComponent.h"
#include "EmmAutoDisplayMessageNotifyEvent.h"
#include "EmmNotifyNewMessageEvent.h"
#include "DrmErrorNotifyEvent.h"
#include "BmlErrorEvent.h"
#include "jlabsBSCATVChannel.h"
#include "jlabsDTTChannel.h"
#include "CAReplaceEvent.h"
#include "EventRelayEvent.h"
#include "EmergencyNotificationEvent.h"
#include "BmlStartEvent.h"
#include "BmlEndEvent.h"
#include "BSControlStartEvent.h"
#include "BmlVodEvent.h"
#endif // JLABS

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
	return (playstate == BROADCAST_PLAYSTATE_PRESENTING || playstate == BROADCAST_PLAYSTATE_STOPPED);
}

void VideoBroadcast::removeAllStreamEventListeners()
{
	NATIVE_WARN_NOK(native_BroadcastRemoveAllStreamEventListeners(getIdentifier()));
	removeAllListeners(StreamBroadcastEvent::STREAM_EVENT_NAME);
}

int VideoBroadcast::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	/* For the HbbTV Applications in TV Apps */
	char *host = OOIFUtils::getHost(obj);
	if (strncmp(host, "file", 4)) return JSP_GET_NOTFOUND;
	//NATIVE_SECURITY_CHECK(obj);
#endif

	// The video/broadcast embedded object
#ifdef HBBTV_1_2_1
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("fullScreen", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("data", CHECK_BROADCAST);
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
	INTRINSIC_EVENT_GET(Focus);
	INTRINSIC_EVENT_GET(Blur);
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
	 GET_FUNCTION(setChannelWithTuneInfo, obj, "on", JSP_GET_VALUE_CACHE);
#ifdef HMX_WEBUI
	GET_FUNCTION(setChannelFCC, obj, "on--", JSP_GET_VALUE_CACHE);
#endif
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

	GET_GUARDED_SECURITY_CHECK("RECORDING_UNREALIZED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_SCHEDULED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_REC_PRESTART", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_REC_ACQUIRING_RESOURCES", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_REC_STARTED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_REC_UPDATED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_REC_COMPLETED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_TS_ACQUIRING_RESOURCES", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_TS_STARTED", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("RECORDING_ERROR", CHECK_BROADCAST);

	GET_GUARDED_SECURITY_CHECK("playbackOffset", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("maxOffset", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("playSpeeds", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("recordNow", CHECK_BROADCAST);
#ifdef HMX_WEBUI
	GET_GUARDED_SECURITY_CHECK("recordDelayed", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("startOffset", CHECK_BROADCAST);
#endif
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
#ifdef HMX_WEBUI
	GET_NUMBER("RECORDING_UNREALIZED",				OPLRECORDER_REC_STATE_UNREALIZED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_SCHEDULED",				OPLRECORDER_REC_STATE_SCHEDULED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_PRESTART",			OPLRECORDER_REC_STATE_REC_PRESTART, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_ACQUIRING_RESOURCES",	OPLRECORDER_REC_STATE_REC_ACQUIRING_RESOURCES, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_STARTED",				OPLRECORDER_REC_STATE_REC_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_UPDATED",				OPLRECORDER_REC_STATE_REC_UPDATED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_COMPLETED",			OPLRECORDER_REC_STATE_REC_COMPLETED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_TS_ACQUIRING_RESOURCES",	OPLRECORDER_REC_STATE_TS_ACQUIRING_RESOURCES, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_TS_STARTED",				OPLRECORDER_REC_STATE_TS_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_TSR_BUFFER_COPYEND",		OPLRECORDER_REC_STATE_TSRBUFFER_COPYEND, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_ERROR",					OPLRECORDER_REC_STATE_ERROR, JSP_GET_VALUE_CACHE);
#endif
	INTRINSIC_EVENT_GET(PlaySpeedChanged);
#ifdef OIPF
    INTRINSIC_EVENT_GET(PlaySpeedsArrayChanged);
#endif /* OIPF */
	INTRINSIC_EVENT_GET(PlayPositionChanged);
	NATIVE_GET_NUMBER("playbackOffset", native_BroadcastGetPlaybackOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maxOffset", native_BroadcastGetMaxOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef HMX_WEBUI
	NATIVE_GET_NUMBER("recordingState", native_BroadcastGetRecordingState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(RecordingEvent);
	NATIVE_GET_NUMBER("playPosition", native_BroadcastGetPlayPosition(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playSpeed", native_BroadcastGetPlaySpeed(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("startOffset", native_BroadcastGetStartOffset(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("trickRestrictMode", native_BroadcastGetTrickRestrictMode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("tsrPaused", native_BroadcastGetTsrPaused(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("tsrPausedPoints", obj,
	                              native_BroadcastGetTsrPausedPoints(identifier, &SIZE, &NATIVE_ARRAY),
	                              getPausedPoints(SIZE, NATIVE_ARRAY),
	                              JSP_GET_VALUE);
    NATIVE_GET_NUMBER("storageRight", native_BroadcastGetStorageRight(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maxDuration", native_BroadcastGetMaxDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(TrickRestrictMode);
#endif
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
#ifdef HMX_WEBUI
	GET_FUNCTION(recordDelayed, obj, "nnn", JSP_GET_VALUE_CACHE);
#endif
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

#if !defined(JLABS)
	// Extensions to video/broadcast for synchronization
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("internalAddStreamEventListener", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("internalAddXMLStreamEventListener", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("removeStreamEventListener", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_FUNCTION(internalAddStreamEventListener, obj, "sso", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(internalAddXMLStreamEventListener, obj, "ssnno", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeStreamEventListener, obj, "sso", JSP_GET_VALUE_CACHE);
#endif

	// Extensions to video/broadcast for playback of selected components
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_VIDEO", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_AUDIO", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("COMPONENT_TYPE_SUBTITLE", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getComponents", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getCurrentActiveComponents", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("selectComponent", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("unselectComponent", CHECK_BROADCAST);
#ifdef HMX_WEBUI
	GET_GUARDED_SECURITY_CHECK("offComponent", CHECK_BROADCAST);
#endif

#endif // HBBTV_SECURITY
	GET_NUMBER("COMPONENT_TYPE_VIDEO", BROADCAST_COMPONENT_TYPE_VIDEO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_AUDIO", BROADCAST_COMPONENT_TYPE_AUDIO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_SUBTITLE", BROADCAST_COMPONENT_TYPE_SUBTITLE, JSP_GET_VALUE_CACHE);
#ifdef JLABS
	GET_NUMBER("COMPONENT_TYPE_SUPERIMPOSE", BROADCAST_COMPONENT_TYPE_SUPERIMPOSE, JSP_GET_VALUE);
	GET_NUMBER("COMPONENT_TYPE_DATA", BROADCAST_COMPONENT_TYPE_DATA, JSP_GET_VALUE);
	GET_FUNCTION(getMultiViewComponents, obj, "", JSP_GET_VALUE );
	GET_FUNCTION(showVideoMessage, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION(hideVideoMessage, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION(startBML, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(endBML, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(setBMLKeyControlMode, obj, "b", JSP_GET_VALUE);
#endif // JLABS

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

#ifdef HMX_WEBUI
	GET_FUNCTION(offComponent, obj, "nn", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(requestThumbnail, obj, "si", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(CheckAitSection, obj, "", JSP_GET_VALUE);
#endif

#ifdef JLABS
	INTRINSIC_EVENT_GET(EmmAutoDisplayMessageNotify);
	INTRINSIC_EVENT_GET(EmmNotifyNewMessage);
	INTRINSIC_EVENT_GET(DRMErrorNotify);
	INTRINSIC_EVENT_GET(BmlError);
	INTRINSIC_EVENT_GET(CAReplace);
	INTRINSIC_EVENT_GET(EventRelay);
	INTRINSIC_EVENT_GET(EmergencyNotification);
	INTRINSIC_EVENT_GET(BMLStart);
	INTRINSIC_EVENT_GET(BMLEnd);
	INTRINSIC_EVENT_GET(BSControlStart);
	INTRINSIC_EVENT_GET(BmlVod);
#endif // JLABS
#ifdef HMX_WEBUI
	GET_FUNCTION (checkChannelChangable, obj, "o---", JSP_GET_VALUE);
	GET_FUNCTION (checkChannelRecordable, obj, "ob--", JSP_GET_VALUE);
	GET_FUNCTION (getPlayingThumbnail, obj, "n", JSP_GET_VALUE);
	GET_FUNCTION (resultPincode, obj, "b", JSP_GET_VALUE);
#endif
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("addEventListener", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("removeEventListener", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
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
	INTRINSIC_EVENT(Focus);
	INTRINSIC_EVENT(Blur);

	// Extensions to video/broadcast for recording and time-shift
#if defined(HBBTV_PVR) || defined(OIPF)
	INTRINSIC_EVENT(PlaySpeedChanged);
	INTRINSIC_EVENT(PlayPositionChanged);
#ifdef OIPF
	INTRINSIC_EVENT(PlaySpeedsArrayChanged);
	NATIVE_SET("timeShiftMode", native_BroadcastSetTimeShiftMode(identifier, NUMBER_VALUE(value[0])));
#endif // OIPF
#ifdef HMX_WEBUI
	INTRINSIC_EVENT(PlayPositionChanged);
	INTRINSIC_EVENT(RecordingEvent);
#endif
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

#if defined(HMX_WEBUI)
	INTRINSIC_EVENT(CasMessageNotify);
	INTRINSIC_EVENT(CasUiNotify);
	INTRINSIC_EVENT(RctInfoChanged);
	INTRINSIC_EVENT(NCDChanged);	
	INTRINSIC_EVENT(NetworkUriChanged);
	INTRINSIC_EVENT(RequestPincode);
	INTRINSIC_EVENT(RequestMhegTune);
	INTRINSIC_EVENT(RequestMhegInterrupt);
	INTRINSIC_EVENT(MhegIcsStatus);
#endif // HMX_WEBUI

#ifdef JLABS
	INTRINSIC_EVENT(EmmAutoDisplayMessageNotify);
	INTRINSIC_EVENT(EmmNotifyNewMessage);
	INTRINSIC_EVENT(DRMErrorNotify);
	INTRINSIC_EVENT(BmlError);
	INTRINSIC_EVENT(CAReplace);
	INTRINSIC_EVENT(EventRelay);
	INTRINSIC_EVENT(EmergencyNotification);
	INTRINSIC_EVENT(BMLStart);
	INTRINSIC_EVENT(BMLEnd);
	INTRINSIC_EVENT(BSControlStart);
	INTRINSIC_EVENT(BmlVod);
#endif // JLABS

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
#if defined(HMX_WEBUI)
	CHECK_ARGUMENT_COUNT_MIN_MAX(3, 6);

	if (argc != 3 && argc != 6) {
		OOIF_LOG_WARNING("Wrong number of arguments: was %d, expected 3 or 6", argc);
	}
#else
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 6);
#endif

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	ChannelHandle retval = NULL;

	if (argc == 3) {
#if defined(HMX_WEBUI)
		if (TYPE_NUMBER(argv[1]) && TYPE_NUMBER(argv[2]))
		{
			NATIVE_CALL(native_BroadcastCreateChannelObject3lcn(instance->getIdentifier(),
															NUMBER_VALUE(argv[0]),
															NUMBER_VALUE(argv[1]),
															NUMBER_VALUE(argv[2]),
															&retval));
		}
		else
#endif
		{
			if (!TYPE_STRING(argv[1]) || !TYPE_NUMBER(argv[2]))
			{
				const char *dsd_utf8 = STRING_VALUE(argv[1]);
				int dsd_utf8_len = STRING_LENGTH(argv[1]);

				// dsd is wrapped inside UTF-8, so unpack it to its original binary data
				unsigned char *dsd_data = (unsigned char*)OOIFUtils::getTmpBuffer(dsd_utf8_len+1);
				if (!dsd_data)
					THROW_OOM_ERR(this_obj, result, JSP_CALL_EXCEPTION);
				int dsd_data_len;

				if (!OOIFUtils::unwrapBinaryDataFromUTF8(dsd_data, dsd_data_len, dsd_utf8, dsd_utf8_len))
					goto arg_error;

				// null terminate, just to be on the safe side
				dsd_data[dsd_data_len] = 0;

				NATIVE_CALL(native_BroadcastCreateChannelObject3(instance->getIdentifier(),
														 NUMBER_VALUE(argv[0]),
														 dsd_data,
														 dsd_data_len,
														 NUMBER_VALUE(argv[2]),
														 &retval));
			}
		}
	}
	else {
		int idType = 0, onid = 0, tsid = 0, sid = 0, sourceID = 0;
		const char* ipBroadcastID = NULL;

		// check all argument types for supplied parameters
		switch (argc) {
		default: // all cases fall through
		case 6: if (TYPE_STRING(argv[5])) ipBroadcastID = STRING_VALUE(argv[5]); else goto arg_error;
		case 5: if (TYPE_NUMBER(argv[4])) sourceID      = NUMBER_VALUE(argv[4]); else goto arg_error;
		case 4: if (TYPE_NUMBER(argv[3])) sid           = NUMBER_VALUE(argv[3]); else goto arg_error;
		case 3: if (TYPE_NUMBER(argv[2])) tsid          = NUMBER_VALUE(argv[2]); else goto arg_error;
		case 2: if (TYPE_NUMBER(argv[1])) onid          = NUMBER_VALUE(argv[1]); else goto arg_error;
		case 1: if (TYPE_NUMBER(argv[0])) idType        = NUMBER_VALUE(argv[0]); else goto arg_error;
			break;
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

//static
int VideoBroadcast::setChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int max_params = 3;
	
#if defined(HBBTV_PVR) || defined(OIPF)
	max_params++;
#endif // HBBTV_PVR || OIPF

	CHECK_ARGUMENT_COUNT_MIN_MAX(1,max_params);
	bool has_channel = (argv[0].type != JSP_TYPE_NULL);
	ChannelHandle channel;
	if (has_channel) {
#ifdef JLABS
		Channel *c = (OBJECT_VALUE(argv[0],jlabsDTTChannel) == NULL ?
					(OBJECT_VALUE(argv[0],jlabsBSCATVChannel) == NULL ? OBJECT_VALUE(argv[0],Channel) : OBJECT_VALUE(argv[0],jlabsBSCATVChannel))
					:OBJECT_VALUE(argv[0],jlabsDTTChannel));
		if(!c)
		{
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		}

#else
		Channel *c = OBJECT_VALUE_OR_THROW(c, argv[0], Channel);
#endif
		channel = c->getIdentifier();
	}
	else
	{
		channel = NULL;
	}

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFBoolean has_trickplay = (argc >= 2 && TYPE_BOOLEAN(argv[1]));
	OOIFBoolean trickplay = has_trickplay ? BOOLEAN_VALUE(argv[1]) : FALSE;
#ifdef OIPF
	const char *cad = (argc >= 3 && TYPE_STRING(argv[2])) ? STRING_VALUE(argv[2]) : NULL;
#else
	const char *cad = NULL;
#endif // !HBBTV
	OOIFNumber offset = ( argc >=4 && TYPE_NUMBER(argv[3])) ? NUMBER_VALUE(argv[3]) : 0;

	NATIVE_CALL(native_BroadcastSetChannel(instance->getIdentifier(), channel, \
										   has_trickplay, trickplay, cad, ( offset > 0 ) ? TRUE : FALSE, offset));

#ifdef HBBTV
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	NATIVE_WARN_NOK(native_AppManNotifySetChannelCalled(window, host, channel));
#endif //HBBTV
	return JSP_CALL_NO_VALUE;
}

int VideoBroadcast::setChannelWithTuneInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int max_params = 2;

#if defined(HBBTV_PVR) || defined(OIPF)
	max_params++;
#endif // HBBTV_PVR || OIPF

	CHECK_ARGUMENT_COUNT_MIN_MAX(1,max_params);

	bool has_channel = (argv[0].type != JSP_TYPE_NULL);
	ChannelHandle channel;
	if (has_channel)
	{
		Channel *c = OBJECT_VALUE_OR_THROW(c, argv[0], Channel);
		channel = c->getIdentifier();
	}
	else
	{
		channel = NULL;
	}

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastSetChannelWithTuneInfo(instance->getIdentifier(), channel,  NUMBER_VALUE(argv[1])));

#ifdef HBBTV
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	NATIVE_WARN_NOK(native_AppManNotifySetChannelCalled(window, host, channel));
#endif //HBBTV
	return JSP_CALL_NO_VALUE;
}


#ifdef HMX_WEBUI
//static - fast channel change
int VideoBroadcast::setChannelFCC(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
	int max_params = 3;

#if defined(HBBTV_PVR) || defined(OIPF)
	max_params++;
#endif // HBBTV_PVR || OIPF

	CHECK_ARGUMENT_COUNT_MIN_MAX(1,max_params);
	bool has_channel = (argv[0].type != JSP_TYPE_NULL);
	ChannelHandle channel;
	if (has_channel) {
#ifdef JLABS
		Channel *c = (OBJECT_VALUE(argv[0],jlabsDTTChannel) == NULL ?
					(OBJECT_VALUE(argv[0],jlabsBSCATVChannel) == NULL ? OBJECT_VALUE(argv[0],Channel) : OBJECT_VALUE(argv[0],jlabsBSCATVChannel))
					:OBJECT_VALUE(argv[0],jlabsDTTChannel));
		if(!c)
		{
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		}

#else
		Channel *c = OBJECT_VALUE_OR_THROW(c, argv[0], Channel);
#endif
		channel = c->getIdentifier();
	}
	else
	{
		channel = NULL;
	}

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFBoolean has_ViewID = (argc >= 2 && TYPE_NUMBER(argv[1]));
	OOIFNumber viewID = has_ViewID ? NUMBER_VALUE(argv[1]) : 1;
	NATIVE_CALL(native_BroadcastSetChannelFCC(instance->getIdentifier(), channel, viewID));

	return JSP_CALL_NO_VALUE;
}
#endif

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
	CHECK_ARGUMENT_COUNT_MIN_MAX(0, 1);
 	OOIFNumber viewID;
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	if (argc == 1)
	{
		viewID = NUMBER_VALUE(argv[0]);
		NATIVE_CALL(native_BroadcastStopByViewID(instance->getIdentifier(), viewID));
	}
	else
	{
	NATIVE_CALL(native_BroadcastStop(instance->getIdentifier()));
	}
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
#if defined(HMX_WEBUI)
	if(argc == 2)
		NATIVE_CALL(native_BroadcastRecordNowAt(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	else
#endif
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
	int componentType = argv[0].u.number;

	NATIVE_CALL(native_BroadcastGetComponents(instance->getIdentifier(),
											  (BroadcastComponentType)componentType,
											  &count, &handles));

	AVComponentCollection *components = new AVComponentCollection();

	if ((count > 0) && (NULL != handles))
	{
		for (int i=0; i<count; i++)
		{
#ifdef JLABS
			if( componentType == 0 )
			{ // VIDEO
				components->addItem(new VideoComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 1)
			{// AUDIO
				components->addItem(new AudioComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 2)
			{// SUBTITLE
				components->addItem(new SubtitleComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 4)
			{// DATA
				components->addItem(new jlabsDataContentComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else{ /* do nothing */ }
#else
			components->addItem(new AVComponent(handles[i], instance->classType));
#endif
		}
	}

	if(handles)
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

	OOIF_LOG_DEBUG("getCurrentActiveComponents called");

	int count = 0;
	AVComponentHandle *handles = NULL;
	int componentType = argv[0].u.number;

	NATIVE_CALL(native_BroadcastGetCurrentActiveComponents(instance->getIdentifier(),
											  (BroadcastComponentType)componentType,
											  &count, &handles));

	AVComponentCollection *components = new AVComponentCollection();

	if ((count > 0) && (NULL != handles))
	{
		for (int i=0; i<count; i++)
		{
#ifdef JLABS
			if( componentType == 0 )
			{ // VIDEO
				components->addItem(new VideoComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 1)
			{// AUDIO
				components->addItem(new AudioComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 2)
			{// SUBTITLE
				components->addItem(new SubtitleComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 4)
			{// DATA
				components->addItem(new jlabsDataContentComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else{ /* do nothing */ }
#else
			components->addItem(new AVComponent(handles[i], instance->classType));
#endif
		}
	}

	if(handles)
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

	if (argv[0].type == JSP_TYPE_NUMBER)
	{
		NATIVE_CALL(native_BroadcastSelectComponent2(instance->getIdentifier(), (BroadcastComponentType)argv[0].u.number));
	}
	else
	{
#if JLABS
		ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);

		switch (o->getType())
		{
			case CLASS_VIDEOCOMPONENT:
			{
				VideoComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], VideoComponent);
				NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_AUDIOCOMPONENT:
			{
				AudioComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AudioComponent);
				NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_SUBTITLECOMPONENT:
			{
				SubtitleComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], SubtitleComponent);
				NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_JLABS_DATA_CONTENT_COMPONENT:
			{
				jlabsDataContentComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], jlabsDataContentComponent);
				NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			default:
				THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
				break;
		}
#else
		AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);

    	NATIVE_CALL(native_BroadcastSelectComponent(instance->getIdentifier(), component->getIdentifier()));
#endif
	}

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

    if (argv[0].type == JSP_TYPE_NUMBER)
    {
        NATIVE_CALL(native_BroadcastUnselectComponent2(instance->getIdentifier(), (BroadcastComponentType)argv[0].u.number));
    }
    else
    {
#ifdef JLABS
		ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);

		switch (o->getType())
		{
			case CLASS_VIDEOCOMPONENT:
			{
				VideoComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], VideoComponent);
				NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_AUDIOCOMPONENT:
			{
				AudioComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AudioComponent);
				NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_SUBTITLECOMPONENT:
			{
				SubtitleComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], SubtitleComponent);
				NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			case CLASS_JLABS_DATA_CONTENT_COMPONENT:
			{
				jlabsDataContentComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], jlabsDataContentComponent);
				NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
			}
			break;

			default:
				THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
				break;
		}
#else
    	AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);
    	NATIVE_CALL(native_BroadcastUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
#endif
	}
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
	NATIVE_CALL(native_BroadcastRemoveStreamEventListener(STRING_VALUE(argv[0]),
	            event_name,
	            (void*)listener->opera_private));
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

#if defined(HMX_WEBUI)

BookmarkCollection *VideoBroadcast::getPausedPoints(int count, BookmarkHandle *handles)
{
	BookmarkCollection *collection = NULL;

	collection = new BookmarkCollection();
	if (collection == NULL)
	{
		if (handles != NULL)
		{
			for (int i=0; i<count; i++)
			{
				native_BookmarkReleaseHandle(handles[i]);
			}
			HLIB_STD_MemFree(handles);
		}
		return NULL;
	}

	if (handles != NULL)
	{
		for (int i=0; i<count; i++)
		{
			if (handles[i] != NULL)
			{
				collection->addItem(new Bookmark(handles[i]));
			}
		}
		HLIB_STD_MemFree(handles);
	}
	return collection;
}
#endif // HBBTV_PVR || OIPF

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
	OOIF_LOG_PRINT("[%d:%s] %d EVENT RECEVIED\n", __LINE__, __FUNCTION__, event);
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
	case BROADCAST_EVENT_FOCUS:
		OOIF_LOG_DEBUG("Focus event received");
		e = new EmptyEvent(context, "Focus");
		break;
	case BROADCAST_EVENT_BLUR:
		OOIF_LOG_DEBUG("BLUR event received");
		e = new EmptyEvent(context, "Blur");
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
		e = new SelectedComponentChangedEvent(context,
		                                      info->selected_component_changed_data.componentType
		                                      );
		break;
#endif // OIPF || HBBTV_1_2_1

#if defined(HMX_WEBUI)
	case BROADCAST_EVENT_CASMESSAGE_NOTIFY:
		OOIF_LOG_DEBUG("CasMessageNotify event received");
		e = new CasMessageNotifyBroadcastEvent(context,
		                                      info->cas_message_notify_data.module,
		                                      info->cas_message_notify_data.slot,
		                                      info->cas_message_notify_data.session,
		                                      info->cas_message_notify_data.message, 
		                                      info->cas_message_notify_data.message_length );
		break;

	case BROADCAST_EVENT_CASUI_NOTIFY:
		OOIF_LOG_DEBUG("CasUiNotify event received 0x%x %d 0x%x",
			info->cas_message_notify_data.module,
			info->cas_message_notify_data.slot,
			info->cas_message_notify_data.session);
		e = new CasUiNotifyBroadcastEvent(context,
											info->cas_message_notify_data.module,
											info->cas_message_notify_data.slot,
											info->cas_message_notify_data.session);
		break;

	case BROADCAST_EVENT_TRICK_RESTRICT_MODE_CHANGED:
		OOIF_LOG_DEBUG("TrickRestrictModeChanged event received");
		e = new EmptyEvent(context, "TrickRestrictModeChanged");
		break;

	case BROADCAST_EVENT_SI_RCT_CHANGED:
		OOIF_LOG_DEBUG("RCTChanged event received");
		e = new RctNotifyBroadcastEvent(context, info->rct_info.rctInfo);
		break;

	case BROADCAST_EVENT_SI_NCD_CHANGED:
		e = new NCDNotifyBroadcastEvent(context, info->ncd_changed_info.startTimeOfChange, info->ncd_changed_info.ASODuration,info->ncd_changed_info.ncdmessage);
		break;
		
	case BROADCAST_EVENT_SI_NETWORK_URI_CHANGED:
		e = new NetworkUriNotifyBroadcastEvent(context, info->network_uri_info.networkUri);
		break;

	case BROADCAST_EVENT_REQUEST_PINCODE:
		e = new RequestPincodeEvent(context, info->request_pincode_data.msg);
		break;

	case BROADCAST_EVENT_MHEG_REQUEST_TUNE:
		OOIF_LOG_DEBUG("BROADCAST_EVENT_MHEG_REQUEST_TUNE event received");
		e = new MhegTuneEvent(context, info->mheg_tune_info.channel, info->mheg_tune_info.svcUid, info->mheg_tune_info.tuneMode);
		break;

	case BROADCAST_EVENT_MHEG_INTERRUPT:
		OOIF_LOG_DEBUG("BROADCAST_EVENT_MHEG_INTERRUPT event received");
		e = new MhegInterruptEvent(context, info->mheg_interrupt_info.interruptFlag);
		break;

	case BROADCAST_EVENT_TSR_STATE_CHANGED:
		OOIF_LOG_DEBUG("BROADCAST_EVENT_TSR_STATE_CHANGED event received");
		e = new TsrStateChangedEvent(context, info->tsr_status.isPaused);
		break;
		
	case BROADCAST_EVENT_MHEG_ICS_STAUS:
		OOIF_LOG_DEBUG("BROADCAST_EVENT_MHEG_ICS_STAUS event received");
		e = new MhegIcsStatusEvent(context, info->mheg_ics_status.mhegIcsFlag);
		break;
#endif // HMX_WEBUI


#ifdef JLABS
	case BROADCAST_EVENT_EMM_AUTO_DISPLAY_MESSAGE_NOTIFY:
		OOIF_LOG_DEBUG("BROADCAST_EVENT_EMM_AUTO_DISPLAY_MESSAGE_NOTIFY event received");
      		e = new EmmAutoDisplayMessageNotifyEvent(context,
									info->emm_auto_display_message_notify_data.adm_info
									);
		break;
	case BROADCAST_EVENT_EMM_NOTIFY_NEW_MESSAGE:
		OOIF_LOG_DEBUG("EmmNotifyNewMessage event received");
        	e = new EmmNotifyNewMessageEvent(context,
									info->emm_notify_new_message_data.type,
									info->emm_notify_new_message_data.msg_id,
									info->emm_notify_new_message_data.new_msg
									);
		break;
	case BROADCAST_EVENT_DRM_ERROR_NOTIFY:
		OOIF_LOG_DEBUG("DRMErrorNotify event received");
		OOIF_LOG_DEBUG("err 0x%x, systemId %s",(unsigned int)(info->drm_error_notify_data.error_number),info->drm_error_notify_data.drm_system_id);
        	e = new DrmErrorNotifyEvent(context,
									info->drm_error_notify_data.error_number,
									info->drm_error_notify_data.drm_system_id
									);
		break;
	case BROADCAST_EVENT_BML_ERROR:
		OOIF_LOG_DEBUG("BmlError event received");
	        e = new BmlErrorEvent(context,
									info->bml_error_data.error_number
									);
		break;
	case BROADCAST_EVENT_CA_LINK_MSG_NOTIFY:
		OOIF_LOG_DEBUG("CA Link Message event received");
        	e = new CAReplaceEvent(context,
									info->ca_link_message_data.message,
									info->ca_link_message_data.channel
									);
		break;
	case BROADCAST_EVENT_BS_CONTROL_STARTED:
		OOIF_LOG_DEBUG("Bs Control Started event received");
	        e = new BSControlStartEvent(context);
		break;
	case BROADCAST_EVENT_EVENT_RELAY_NOTIFY:
		e = new EventRelayEvent( context, info->event_relay.channel );
		break;
	case BROADCAST_EVENT_EWS_NOTIFY:
		e = new EmergencyNotificationEvent( context,
											info->ews.channel,
											info->ews.tuner
											);
		break;
	case BROADCAST_EVENT_BML_START:
		e = new BMLStartEvent( context,
											info->bml_start.autoFlag
											);
		break;
	case BROADCAST_EVENT_BML_END:
		e = new BMLEndEvent( context,
											info->bml_end.cause,
											info->bml_end.param
											);
		break;
	case BROADCAST_EVENT_BML_VOD:
		OOIF_LOG_DEBUG("BmlVod event received");
	        e = new BmlVodEvent(context,
									info->bml_vod.appName,
									info->bml_vod.param
									);
		break;
#endif // JLABS
	default:
		OOIF_LOG_WARNING("Unknown event received");
		return;
	}
	OOIF_LOG_PRINT("%s end\n", __FUNCTION__);
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

#if defined(HMX_WEBUI)
int VideoBroadcast::checkChannelChangable (jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	VideoBroadcast		*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	bool				 has_channel = (argv[0].type != JSP_TYPE_NULL);
	OOIFBoolean			 retval = FALSE;
	ChannelHandle		 channel = NULL;

	if (has_channel)
	{
#ifdef JLABS
		Channel *c = (OBJECT_VALUE(argv[0],jlabsDTTChannel) == NULL ?
					(OBJECT_VALUE(argv[0],jlabsBSCATVChannel) == NULL ? OBJECT_VALUE(argv[0],Channel) : OBJECT_VALUE(argv[0],jlabsBSCATVChannel))
					:OBJECT_VALUE(argv[0],jlabsDTTChannel));
		if(!c)
		{
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		}
#else
		Channel *c = OBJECT_VALUE_OR_THROW (c, argv[0], Channel);
#endif
		channel = c->getIdentifier();
	}

	if (NULL == channel)
	{
		RETURN_BOOLEAN ((bool)FALSE, JSP_CALL_VALUE);
	}

	NATIVE_CALL (native_BroadcastCheckChannelChangable(instance->getIdentifier(), channel, &retval));
	RETURN_BOOLEAN ((bool)retval, JSP_CALL_VALUE);
}

int VideoBroadcast::checkChannelRecordable (jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	VideoBroadcast		*instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	bool				 has_channel = (argv[0].type != JSP_TYPE_NULL);
	OOIFBoolean			 retval = FALSE;
	ChannelHandle		 channel = NULL;

	if (has_channel)
	{
#ifdef JLABS
		Channel *c = (OBJECT_VALUE(argv[0],jlabsDTTChannel) == NULL ?
					(OBJECT_VALUE(argv[0],jlabsBSCATVChannel) == NULL ? OBJECT_VALUE(argv[0],Channel) : OBJECT_VALUE(argv[0],jlabsBSCATVChannel))
					:OBJECT_VALUE(argv[0],jlabsDTTChannel));
		if(!c)
		{
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		}
#else
		Channel *c = OBJECT_VALUE_OR_THROW (c, argv[0], Channel);
#endif
		channel = c->getIdentifier();
	}

	if (NULL == channel)
	{
		RETURN_BOOLEAN ((bool)FALSE, JSP_CALL_VALUE);
	}

	NATIVE_CALL (native_BroadcastCheckChannelRecordable(instance->getIdentifier(), channel, BOOLEAN_VALUE(argv[1]), &retval));
	RETURN_BOOLEAN ((bool)retval, JSP_CALL_VALUE);
}

int VideoBroadcast::getPlayingThumbnail (jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFNumber pos = NUMBER_VALUE(argv[0]);
	const char *retval;
	NATIVE_CALL(native_BroadcastGetPlayingThumbnail(instance->getIdentifier(), pos, &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::resultPincode(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFBoolean permit = argv[0].u.boolean;

	OOIFNumber retval;
	NATIVE_CALL(native_BroadcastResultPincode(instance->getIdentifier(), permit, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

#endif

#ifdef JLABS
//static
int VideoBroadcast::getMultiViewComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("getMultiViewComponents called");

	int count = 0;
	jlabsMultiViewComponentHandle *handles = NULL;

	NATIVE_CALL(native_BroadcastGetMultiViewComponents(instance->getIdentifier(),
											  &count, &handles));

	jlabsMultiViewComponentCollection *components = new jlabsMultiViewComponentCollection();

	if ((count > 0) && (NULL != handles))
	{
		for (int i=0; i<count; i++)
		{
			components->addItem(new jlabsMultiViewComponent((jlabsMultiViewComponentHandle)handles[i], instance->classType));
		}
	}

	if(handles)
		HLIB_STD_MemFree(handles);

	RETURN_OBJECT(this_obj, components, result, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::showVideoMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFNumber type = argv[0].u.number;

	OOIF_LOG_DEBUG("showVideoMessage called");

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastShowVideoMessage(instance->getIdentifier(), type, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoBroadcast::hideVideoMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFNumber type = argv[0].u.number;

	OOIF_LOG_DEBUG("hideVideoMessage called");

	OOIFBoolean retval;
	NATIVE_CALL(native_BroadcastHideVideoMessage(instance->getIdentifier(), type, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int VideoBroadcast::startBML(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("startBML called");

	NATIVE_CALL(native_BroadcastStartBML(instance->getIdentifier()));
	RETURN_NULL(JSP_CALL_VALUE);
}

//static
int VideoBroadcast::endBML(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("endBML called");

	NATIVE_CALL(native_BroadcastEndBML(instance->getIdentifier()));
	RETURN_NULL(JSP_CALL_VALUE);
}

//static
int VideoBroadcast::setBMLKeyControlMode(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);
	OOIFBoolean permit = argv[0].u.boolean;

	OOIF_LOG_DEBUG("setBMLKeyControlMode called");

	OOIFNumber retval;
	NATIVE_CALL(native_BroadcastSetBMLKeyControlMode(instance->getIdentifier(), permit, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

#endif // JLABS

#if defined(HMX_WEBUI)
int VideoBroadcast::recordDelayed(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(2,3);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("recordDelayed called");

	const char *retval;
	if(argc == 3)
		NATIVE_CALL(native_BroadcastRecordDelayedAt(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), &retval));
	else
		NATIVE_CALL(native_BroadcastRecordDelayed(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif

#if defined(HMX_WEBUI)
int VideoBroadcast::offComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(2,2);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("selectComponentOff called");

	NATIVE_CALL(native_BroadcastOffComponent(instance->getIdentifier(), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1])));

	return JSP_CALL_NO_VALUE;
}
int VideoBroadcast::requestThumbnail(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	const char *retval = NULL;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(1,2);
	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	OOIF_LOG_DEBUG("selectComponentOff called");
	if(argc == 2)
		NATIVE_CALL(native_BroadcastRequestThumbnail(instance->getIdentifier(), STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]),&retval));
	if(argc == 1)
		NATIVE_CALL(native_BroadcastRequestThumbnail(instance->getIdentifier(), STRING_VALUE(argv[0]), 0,&retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
	return JSP_CALL_NO_VALUE;
}

int VideoBroadcast::CheckAitSection(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							  int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	VideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoBroadcast);

	NATIVE_CALL(native_BroadcastCheckAitSection(instance->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}
#endif
