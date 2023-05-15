// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "VideoOnDemand.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"

#include "NativeVideoOnDemand.h"
#include "NativeCapabilities.h"
#include "EmptyEvent.h"
#include "ParentalRatingEvent.h"
#ifndef HBBTV_1_2_1
#include "PlaySpeedChangedEvent.h"
#endif // !HBBTV_1_2_1
#include "PlayPositionChangedEvent.h"
#include "DRMRightsErrorVodEvent.h"
#include "Collection.h"
#include "AVComponent.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY
#include "ParentalRatingChangeEvent.h"
#include "ParentalRatingErrorEvent.h"
#ifdef OIPF
#include "RepresentationChangeEvent.h"
#include "PeriodChangeEvent.h"
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SelectedComponentChangedEvent.h"
#endif // OIPF || HBBTV_1_2_1

using namespace Ooif;


//static
list<VideoOnDemand*> VideoOnDemand::instances;

VideoOnDemand::VideoOnDemand(VodHandle i) : WrapperObject<VideoOnDemand>(i)
{
	ENTRY;
	classType = CLASS_VIDEO_ON_DEMAND;
	VideoOnDemand::instances.push_back(this);
}

VideoOnDemand::~VideoOnDemand()
{
	ENTRY;
	VideoOnDemand::instances.remove(this);
}

int VideoOnDemand::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	//CEA2014A requirements
	NATIVE_GET_STRING("type", native_VodGetType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("data", native_VodGetData(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playPosition", native_VodGetPlayPosition(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playTime", native_VodGetPlayTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playState", native_VodGetPlayState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("error", native_VodGetError(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("speed", native_VodGetSpeed(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("nrTracks", native_VodGetNrTracks(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("currentTrackIndex", native_VodGetCurrentTrackIndex(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(PlayStateChange);
	INTRINSIC_EVENT_GET(FullScreenChange);
	NATIVE_GET_BOOLEAN("persist", native_VodGetPersist(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(play, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stop, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(seek, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setVolume, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(next, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(previous, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(seekSegment, obj, "n", JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("fullScreen", isFullScreen(), JSP_GET_VALUE);
	GET_FUNCTION(setFullScreen, obj, "b", JSP_GET_VALUE_CACHE);

#ifdef MAXDOME
    NATIVE_GET_NUMBER("errorDetails", native_VodGetErrorDetails(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif

	//Extensions to A/V object for trickmodes
#ifndef HBBTV_1_2_1
	INTRINSIC_EVENT_GET(PlaySpeedChanged);
#endif // !HBBTV_1_2_1
	INTRINSIC_EVENT_GET(PlayPositionChanged);
#ifdef OIPF
	NATIVE_GET_NUMBERS_COLLECTION("playSpeeds", obj, native_VodGetPlaySpeeds(getIdentifier(), &SIZE, &NATIVE_ARRAY), getNumberCollection(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_STRING("oitfSourceIPAddress", native_VodGetOitfSourceIPAddress(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("oitfSourcePortAddress", native_VodGetOitfSourcePortAddress(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("oitfNoRTSPSessionControl", native_VodGetOitfNoRTSPSessionControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("oitfRTSPSessionId", native_VodGetOitfRTSPSessionId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF

	//Extensions to A/V object for parental rating error
	INTRINSIC_EVENT_GET(ParentalRatingChange);
	INTRINSIC_EVENT_GET(ParentalRatingError);

	//Extensions to A/V object for DRM rights errors
	INTRINSIC_EVENT_GET(DRMRightsError);

	//Extensions to A/V object for playing media objects
#if defined(HBBTV_PVR) || defined(HBBTV_DOWNLOAD) || defined(OIPF)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("setSource", CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	GET_FUNCTION(setSource, obj, "s", JSP_GET_VALUE_CACHE);
#endif //HBBTV_PVR || HBBTV_DOWNLOAD || OIPF

	//Extensions to A/V object for UI feedback of buffering A/V content
#ifdef OIPF
	NATIVE_GET_BOOLEAN("readyToPlay", native_VodGetReadyToPlay(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(ReadyToPlay);
	GET_FUNCTION(getAvailablePlayTime, obj, "b", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setBufferingStrategy, obj, "s", JSP_GET_VALUE_CACHE);
#endif // OIPF

	//Extensions to A/V object for playback of selected componentes
	GET_NUMBER("COMPONENT_TYPE_VIDEO", VOD_COMPONENT_TYPE_VIDEO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_AUDIO", VOD_COMPONENT_TYPE_AUDIO, JSP_GET_VALUE_CACHE);
	GET_NUMBER("COMPONENT_TYPE_SUBTITLE", VOD_COMPONENT_TYPE_SUBTITLE, JSP_GET_VALUE_CACHE);
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

	// DAE 1.1 / 2.1 specific properties and methods
	GET_FUNCTION(getVolume, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(queue, obj, "s", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	NATIVE_GET_NUMBER("currentRepresentation", native_VodGetCurrentRepresentation(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("maxRepresentation", native_VodGetMaxRepresentation(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("minRepresentation", native_VodGetMinRepresentation(getIdentifier(), &RETVAL), JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setRepresentationStrategy, obj, "nnn", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(PlaySpeedsArrayChanged);
	INTRINSIC_EVENT_GET(RepresentationChange);
	INTRINSIC_EVENT_GET(PeriodChange);
	NATIVE_GET_NUMBERS_COLLECTION("availableRepresentationsBandwidth", obj,
	                              native_VodGetAvailableRepresentationsBandwidth(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                              getNumberCollection(SIZE, NATIVE_ARRAY),
	                              JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("supportedStrategies", obj,
	                             native_VodGetSupportedStrategies(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
#endif //OIPF
#if defined(HMX_VOD_PDL)
	NATIVE_GET_NUMBER("BufferingProgress", native_VodGetBufferingProgress(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("BufferingTime", native_VodGetBufferingTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("PdlChunks", native_VodGetPdlChunks(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("ReadyState", native_VodGetReadyState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(ReadyStateChange);
#endif
	return EventTarget::getter(obj, name, result);
}

int VideoOnDemand::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	//CEA2014A requirements
	NATIVE_SET("type", native_VodSetType(getIdentifier(), STRING_VALUE(value[0])));
	NATIVE_SET("data", native_VodSetData(getIdentifier(), TYPE_STRING(value[0]) ? STRING_VALUE(value[0]) : NULL));
	SET_READONLY("playPosition");
	SET_READONLY("playTime");
	SET_READONLY("playState");
	SET_READONLY("error");
	SET_READONLY("speed");
	SET_READONLY("nrTracks");
	NATIVE_SET("currentTrackIndex", native_VodSetCurrentTrackIndex(getIdentifier(), NUMBER_VALUE(value[0])));
	INTRINSIC_EVENT(PlayStateChange);
	INTRINSIC_EVENT(FullScreenChange);
	NATIVE_SET("persist", native_VodSetPersist(getIdentifier(), BOOLEAN_VALUE(value[0])));
	SET_READONLY("fullScreen");

	//Extensions to A/V object for trickmodes
#ifndef HBBTV_1_2_1
	INTRINSIC_EVENT(PlaySpeedChanged);
#endif // !HBBTV_1_2_1
	INTRINSIC_EVENT(PlayPositionChanged);
#ifdef OIPF
	SET_READONLY("playSpeeds");
	SET_READONLY("oitfSourceIPAddress");
	SET_READONLY("oitfSourcePortAddress");
	NATIVE_SET("oitfRTSPSessionId", native_VodSetOitfRTSPSessionId(getIdentifier(), STRING_VALUE(value[0])));
	NATIVE_SET("oitfNoRTSPSessionControl", native_VodSetOitfNoRTSPSessionControl(getIdentifier(), BOOLEAN_VALUE(value[0])));
#endif // OIPF

	//Extensions to A/V object for parental rating error
	INTRINSIC_EVENT(ParentalRatingChange);
	INTRINSIC_EVENT(ParentalRatingError);

	//Extensions to A/V object for DRM rights errors
	INTRINSIC_EVENT(DRMRightsError);

	//Extensions to A/V object for UI feedback of buffering A/V content
#ifdef OIPF
	SET_READONLY("readyToPlay");
	INTRINSIC_EVENT(ReadyToPlay);
#endif // OIPF

	//Extensions to A/V object for playback of selected componentes
#if defined(OIPF) || defined(HBBTV_1_2_1)
	INTRINSIC_EVENT(SelectedComponentChanged);
#endif // OIPF || HBBTV_1_2_1

	// DAE 1.1 / 2.1 specific properties and methods
#ifdef OIPF
	INTRINSIC_EVENT(PlaySpeedsArrayChanged);
	INTRINSIC_EVENT(RepresentationChange);
	INTRINSIC_EVENT(PeriodChange);
	SET_READONLY("availableRepresentationsBandwidth");
	SET_READONLY("currentRepresentation");
	SET_READONLY("maxRepresentation");
	SET_READONLY("minRepresentation");
	SET_READONLY("supportedStrategies");
#endif //OIPF
#if defined(HMX_VOD_PDL)
	INTRINSIC_EVENT(ReadyStateChange);
#endif

	return JSP_PUT_NOTFOUND;
}


void VideoOnDemand::attr_change(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
	if (strcmp(name, "data") == 0)
	{
		NATIVE_WARN_NOK(native_VodSetData(getIdentifier(), value));
	}

	INTRINSIC_EVENT_ATTR(ParentalRatingChange);
	INTRINSIC_EVENT_ATTR(ParentalRatingError);
	INTRINSIC_EVENT_ATTR(PlayStateChange);
	INTRINSIC_EVENT_ATTR(FullScreenChange);
	INTRINSIC_EVENT_ATTR(PlaySpeedChanged);
	INTRINSIC_EVENT_ATTR(PlayPositionChanged);
	INTRINSIC_EVENT_ATTR(DRMRightsError);
#if defined OIPF || defined READY_TO_PLAY
	INTRINSIC_EVENT_ATTR(ReadyToPlay);
#endif //OIPF
}


void VideoOnDemand::param_set(jsplugin_obj *obj, const char *name, const char *value)
{
	ENTRY;
	OOIF_LOG_DEBUG("name: %s value: %s", name, value ? value : "NULL");
	if (strcmp(name, "cache") == 0)
	{
		NATIVE_WARN_NOK(native_VodSetCache(getIdentifier(), value));	
	}
	if (strcmp(name, "loop") == 0)
	{
		NATIVE_WARN_NOK(native_VodSetLoop(getIdentifier(), value));
	}
	if (strcmp(name, "data") == 0)
	{
		NATIVE_WARN_NOK(native_VodSetData(getIdentifier(), value));
	}
}


//static
int VideoOnDemand::play(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT_MIN_MAX(0,1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	OOIFNumber speed = 1.0;
	if (argc > 0 && TYPE_NUMBER(argv[0]))
	{
		speed = NUMBER_VALUE(argv[0]);
	}

	OOIFBoolean retval;
	NATIVE_CALL(native_VodPlay(instance->getIdentifier(), speed, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodStop(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::seek(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFNumber pos = NUMBER_VALUE(argv[0]);
	OOIFBoolean retval;
	NATIVE_CALL(native_VodSeek(instance->getIdentifier(), pos, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::seekSegment(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFNumber pos = NUMBER_VALUE(argv[0]);
	OOIFBoolean retval;
	NATIVE_CALL(native_VodSeekSegment(instance->getIdentifier(), pos, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::setVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	OOIFNumber volume = NUMBER_VALUE(argv[0]);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodSetVolume(instance->getIdentifier(), volume, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::getVolume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFNumber retval;
	NATIVE_CALL(native_VodGetVolume(instance->getIdentifier(), &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::next(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodNext(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::previous(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodPrevious(instance->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

OOIFReturnCode VideoOnDemand::updateNativeWindowState(WindowState requested_state)
{
	return native_VodSetFullScreen(getIdentifier(), requested_state == FULLSCREEN);
}

//static
int VideoOnDemand::getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	AVComponentHandle *handles = NULL;
	int count = 0;

	unsigned int componentType = NUMBER_VALUE(argv[0]);

	NATIVE_CALL(native_VodGetComponents(instance->getIdentifier(),
	                                    (OOIFNumber)componentType,//NUMBER_VALUE(argv[0]),
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
int VideoOnDemand::getCurrentActiveComponents(jsplugin_obj *this_obj,
        jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv,
        jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	AVComponentHandle *handles = NULL;
	int count = 0;
	unsigned int componentType = NUMBER_VALUE(argv[0]);

	NATIVE_CALL(native_VodGetCurrentActiveComponents(instance->getIdentifier(),
	            (OOIFNumber)componentType,//NUMBER_VALUE(argv[0]),
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
int VideoOnDemand::selectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                   int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (TYPE_NUMBER(argv[0]))
	{
		NATIVE_CALL(native_VodSelectComponent2(instance->getIdentifier(), NUMBER_VALUE(argv[0])));
		return JSP_CALL_NO_VALUE;
	}
#endif // OIPF || HBBTV_1_2_1
	AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);
	NATIVE_CALL(native_VodSelectComponent(instance->getIdentifier(), component->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoOnDemand::unselectComponent(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                     int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

#if defined(OIPF) || defined (HBBTV_1_2_1)
	if (TYPE_NUMBER(argv[0]))
	{
		NATIVE_CALL(native_VodUnselectComponent2(instance->getIdentifier(), NUMBER_VALUE(argv[0])));
		return JSP_CALL_NO_VALUE;
	}
#endif // OIPF || HBBTV_1_2_1
	AVComponent *component = OBJECT_VALUE_OR_THROW(component, argv[0], AVComponent);
	NATIVE_CALL(native_VodUnselectComponent(instance->getIdentifier(), component->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int VideoOnDemand::queue(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	const char *url;
	if (argc > 0 && TYPE_STRING(argv[0]))
	{
		url = STRING_VALUE(argv[0]);
	}
	else
	{
		url = "";
	}

	OOIFBoolean retval;
	NATIVE_CALL(native_VodQueue(instance->getIdentifier(), url, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

#if defined(HBBTV_PVR) || defined(HBBTV_DOWNLOAD) || defined(OIPF)
//static
int VideoOnDemand::setSource(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	const char* id = STRING_VALUE(argv[0]);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodSetSource(instance->getIdentifier(), id, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // HBBTV_PVR || HBBTV_DOWNLOAD || OIPF

#ifdef OIPF
//static
int VideoOnDemand::getAvailablePlayTime(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	OOIFBoolean fromPlayPosition = BOOLEAN_VALUE(argv[0]);

	OOIFNumber retval;
	NATIVE_CALL(native_VodGetAvailablePlayTime(instance->getIdentifier(), fromPlayPosition, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::setBufferingStrategy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);
	const char* name = STRING_VALUE(argv[0]);

	OOIFBoolean retval;
	NATIVE_CALL(native_VodSetBufferingStrategy(instance->getIdentifier(), name, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int VideoOnDemand::setRepresentationStrategy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT_MIN_MAX(2,3);
	VideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VideoOnDemand);

	OOIFNumber maxBandwidth = NAN, minBandwidth = NAN, position = NAN;
	if (TYPE_NUMBER(argv[0]))
	{
		maxBandwidth = NUMBER_VALUE(argv[0]);
	}
	if (TYPE_NUMBER(argv[1]))
	{
		minBandwidth = NUMBER_VALUE(argv[1]);
	}
	if ((argc == 3) && TYPE_NUMBER(argv[2]))
	{
		position = NUMBER_VALUE(argv[2]);
	}

	OOIFBoolean retval;
	NATIVE_CALL(native_VodSetRepresentationStrategy(instance->getIdentifier(), maxBandwidth, minBandwidth, position, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // OIPF

void VideoOnDemand::videoVisible(int visibility, int unloading)
{
	if (toggleVisibility(visibility))
	{
		NATIVE_WARN_NOK(native_VodSetVisibility(getIdentifier(), visibility, unloading));
	}
}

void VideoOnDemand::videoPosition(int x, int y, int w, int h)
{
	if (resize(x, y, w, h))
	{
		NATIVE_WARN_NOK(native_VodSetVideoWindow(getIdentifier(), x, y, w, h));
	}
}

//static
VideoOnDemand* VideoOnDemand::GetInstance(VodHandle id)
{
	for (list<VideoOnDemand*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		if ((*it)->getIdentifier() == id)
			return *it;
	}
	return NULL;
}

//static
NumberCollection* VideoOnDemand::getNumberCollection(int count, OOIFNumber *numbers)
{
	NumberCollection *collection = new NumberCollection();
	for (int i=0; i<count; i++)
	{
		collection->addItem(numbers[i]);
	}
	HLIB_STD_MemFree(numbers);
	return collection;
}

OOIFReturnCode VideoOnDemand::checkListenerAccessAllowed(const char *eventName)
{
	return native_VodOnListenerAccessed(identifier, eventName);
}

//static
void VideoOnDemand::onVideoOnDemandEvent(VodEvent event,
        VodEventInfo *info,
        VodHandle id)
{
	VideoOnDemand* vod = GetInstance(id);
	if (!vod)
	{
		OOIF_LOG_WARNING("Unknown VodHandle received");
		return;
	}

	Event *e = NULL;
	jsplugin_obj *context = vod->getHost();
	switch (event)
	{
	case VOD_EVENT_PLAY_STATE_CHANGED:
		OOIF_LOG_DEBUG("PlayStateChange event received");
		e = new EmptyEvent(context, "PlayStateChange");
		break;
	case VOD_EVENT_FULL_SCREEN_CHANGED:
		OOIF_LOG_DEBUG("FullScreenChange event received");
		e = new EmptyEvent(context, "FullScreenChange");
		break;
#ifndef HBBTV_1_2_1
	case VOD_EVENT_PLAY_SPEED_CHANGED:
		OOIF_LOG_DEBUG("PlaySpeedChanged event received");
		e = new PlaySpeedChangedEvent(context, info->play_speed_changed_data.speed);
		break;
#endif // !HBBTV_1_2_1
	case VOD_EVENT_PLAY_POSITION_CHANGED:
		OOIF_LOG_DEBUG("PlayPositionChanged event received");
		e = new PlayPositionChangedEvent(context,
		                                 info->play_position_changed_data.has_position,
		                                 info->play_position_changed_data.position);
		break;
	case VOD_EVENT_DRM_RIGHTS_ERROR:
	{
		OOIF_LOG_DEBUG("DRMRightsError event received");
		e = new DRMRightsErrorVodEvent(context,
		                               info->drm_rights_error_data.error_state,
		                               info->drm_rights_error_data.content_id,
		                               info->drm_rights_error_data.drm_system_id,
		                               info->drm_rights_error_data.rights_issuer_url);
		break;
	}
	case VOD_EVENT_PARENTAL_RATING_CHANGE:
		OOIF_LOG_DEBUG("ParentalRatingChange event received");
		e = new ParentalRatingChangeEvent(context,
		                                  info->parental_rating_change_data.contentID,
#if defined(OIPF) || defined (HBBTV_1_2_1)
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
	case VOD_EVENT_PARENTAL_RATING_ERROR:
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
#ifdef OIPF
	case VOD_EVENT_READY_TO_PLAY:
	{
		OOIF_LOG_DEBUG("ReadyToPlay event received");
		e = new EmptyEvent(context, "ReadyToPlay");
		break;
	}
	case VOD_EVENT_REPRESENTATION_CHANGE:
	{
		OOIF_LOG_DEBUG("RepresentationChange event received");
		e = new RepresentationChangeEvent(context,
		                                  "RepresentationChange",
		                                  info->representation_change_data.bandwidth,
		                                  info->representation_change_data.position);
		break;
	}
	case VOD_EVENT_PERIOD_CHANGE:
	{
		OOIF_LOG_DEBUG("PeriodChange event received");
		NumberCollection* bandwidths = getNumberCollection(info->period_change_data.bandwidths_count,
		                               info->period_change_data.bandwidths);
		e = new PeriodChangeEvent(context,
		                          "PeriodChange",
		                          bandwidths,
		                          info->period_change_data.position);
		break;
	}
	case VOD_EVENT_PLAY_SPEEDS_ARRAY_CHANGED:
	{
		OOIF_LOG_DEBUG("PlaySpeedsArrayChanged event received");
		e = new EmptyEvent(context, "PlaySpeedsArrayChanged");
		break;
	}
#endif // OIPF
#if defined(OIPF) || defined(HBBTV_1_2_1)
	case VOD_EVENT_SELECTED_COMPONENT_CHANGED:
	{
		OOIF_LOG_DEBUG("SelectedComponentChanged event recieved");
				// OIPF DAE 7.16.5.1.2 Properties
		e = new SelectedComponentChangedEvent(context,
		                                      info->selected_component_changed_data.componentType, 
		                                      "SelectedComponentChanged"
		                                      );
		vod->dispatchEvent(e);
		//OIPF DAE 7.16.5.1.4 Events
		e = new SelectedComponentChangedEvent(context,
		                                      info->selected_component_changed_data.componentType, 
		                                      "SelectedComponentChange"
		                                     );
		break;
	}
#endif // OIPF || HBBTV_1_2_1
#if defined(HMX_VOD_PDL)
	case VOD_EVENT_READY_STATE_CHANGED:
		OOIF_LOG_DEBUG("ReadyStateChange event received\n");
		e = new EmptyEvent(context, "ReadyStateChange");
		break;
#endif
	default:
		OOIF_LOG_WARNING("Unknown event received");
		return;
	}
	vod->dispatchEvent(e);
}

void VideoOnDemand::releaseNative(VodHandle id)
{
	ENTRY;

	NATIVE_WARN_NOK(native_VodReleaseHandle(id));
}
