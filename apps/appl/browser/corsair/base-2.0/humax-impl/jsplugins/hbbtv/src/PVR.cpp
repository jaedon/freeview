// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(HBBTV_PVR) || defined(OIPF)
#include <string.h>

#include "macros.h"
#include "PVR.h"

#include "NativeRecording.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY
#include "Channel.h"
#include "BookmarkCollection.h"

using namespace Ooif;

#ifdef OIPF

PVRData PVRDataWrapper::data;

void PVRDataWrapper::init(void *id)
{
	static bool initialized = false;
	if (!initialized)
	{
		memset(&data, 0, sizeof(PVRData));
		NATIVE_WARN_NOK(native_PVRInitialize(&data));
		initialized = true;
	}
}
#endif // OIPF

#ifdef OIPF
PVR::PVR(PVRHandle i) : DataObject<PVR, PVRDataWrapper>(i)
#else
PVR::PVR(PVRHandle i) : WrapperObject<PVR>(i)
#endif // OIPF
{
	ENTRY;
	init();
}

#ifdef OIPF
PVR::PVR(const PVR &original) : DataObject<PVR, PVRDataWrapper>(original.identifier)
#else
PVR::PVR(const PVR &original) : WrapperObject<PVR>(original.identifier)
#endif // OIPF
{
	ENTRY;
	init();
}

PVR::~PVR()
{
	ENTRY;
}

void PVR::init()
{
	classType = CLASS_PVR;
}

int PVR::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	//getters common for ScheduledRecording and Recording
#ifdef OIPF
	GET_NUMBER("ID_TVA_CRID", SCHEDULED_RECORDING_ID_TVA_CRID, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_TVA_GROUP_CRID", SCHEDULED_RECORDING_ID_TVA_GROUP_CRID, JSP_GET_VALUE_CACHE);
#endif // OIPF

#ifndef HBBTV_1_2_1
	GET_NUMBER("ID_DVB_EVENT", SCHEDULED_RECORDING_ID_DVB_EVENT, JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("repeatDays", native_PVRGetRepeatDays(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longName", native_PVRGetLongName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longDescription", native_PVRGetLongDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // !HBBTV_1_2_1

	NATIVE_GET_NUMBER("startPadding", native_PVRGetStartPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("endPadding", native_PVRGetEndPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_PVRGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_PVRGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("startTime", native_PVRGetStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_PVRGetDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("channel", obj, native_PVRGetChannel(getIdentifier(), &RETVAL), new Channel(RETVAL), JSP_GET_VALUE);

#if defined(HBBTV) && !defined(HBBTV_1_2_1)
	NATIVE_GET_BOOLEAN("isSeries", native_PVRGetIsSeries(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // HBBTV && !HBBTV_1_2_1

#ifndef HBBTV_1_2_1
	NATIVE_GET_STRING("programmeID", native_PVRGetProgrammeID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_ENUM("programmeIDType", ScheduledRecordingProgrammeIDType, native_PVRGetProgrammeIDType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("episode", native_PVRGetEpisode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalEpisodes", native_PVRGetTotalEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // !HBBTV_1_2_1

	//DAE spec ambiguity
	NATIVE_GET_VOIDP_COLLECTION("parentalRating", obj, native_PVRGetParentalRatings(getIdentifier(), &SIZE, &NATIVE_ARRAY), getParentalRatings(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("parentalRatings", obj, native_PVRGetParentalRatings(getIdentifier(), &SIZE, &NATIVE_ARRAY), getParentalRatings(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);


	//getters for Recording (Recording is a sub-interface of ScheduledRecording)
#ifdef OIPF
	DATA_GET_NUMBER(RECORDING_UNREALIZED);
	DATA_GET_NUMBER(RECORDING_SCHEDULED);
	DATA_GET_NUMBER(RECORDING_DEL_SCHEDULED);
	DATA_GET_NUMBER(RECORDING_REC_PRESTART);
	DATA_GET_NUMBER(RECORDING_REC_ACQUIRING_RESOURCES);
	DATA_GET_NUMBER(RECORDING_REC_STARTED);
	DATA_GET_NUMBER(RECORDING_REC_UPDATED);
	DATA_GET_NUMBER(RECORDING_REC_COMPLETED);
	DATA_GET_NUMBER(RECORDING_REC_PARTIALLY_COMPLETED);
	DATA_GET_NUMBER(RECORDING_TS_ACQUIRING_RESOURCES);
	DATA_GET_NUMBER(RECORDING_TS_STARTED);
	DATA_GET_NUMBER(RECORDING_ERROR);

	DATA_GET_NUMBER(ERROR_REC_RESOURCE_LIMITATION);
	DATA_GET_NUMBER(ERROR_INSUFFICIENT_STORAGE);
	DATA_GET_NUMBER(ERROR_TUNER_CONFLICT);
	DATA_GET_NUMBER(ERROR_REC_DRM_RESTRICTION);
	DATA_GET_NUMBER(ERROR_REC_UNKNOWN);
	DATA_GET_NUMBER(ERROR_TS_RESOURCE_LIMITATION);
	DATA_GET_NUMBER(ERROR_TS_DRM_RESTRICTION);
	DATA_GET_NUMBER(ERROR_TS_UNKNOWN);
#endif // OIPF
	NATIVE_GET_NUMBER("state", native_PVRGetState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("id", native_PVRGetID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBER("error", native_PVRGetError(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isManual", native_PVRGetIsManual(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("doNotDelete", native_PVRGetDoNotDelete(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("saveDays", native_PVRGetSaveDays(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("saveEpisodes", native_PVRGetSaveEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("blocked", native_PVRGetBlocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("showType", native_PVRGetShowType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitles", native_PVRGetSubtitles(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("subtitleLanguages", obj,
	                             native_PVRGetSubtitleLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isHD", native_PVRGetIsHD(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isWidescreen", native_PVRGetIsWidescreen(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("audioType", native_PVRGetAudioType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isMultilingual", native_PVRGetIsMultilingual(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("audioLanguages", obj,
	                             native_PVRGetAudioLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genres", obj,
	                             native_PVRGetGenres(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
#endif // OIPF
	NATIVE_GET_NUMBER("recordingStartTime", native_PVRGetRecordingStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recordingDuration", native_PVRGetRecordingDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_VOIDP_COLLECTION("bookmarks", obj,
	                            native_PVRGetBookmarks(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                            new BookmarkCollection(getIdentifier(), SIZE, NATIVE_ARRAY),
	                            JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("locked", native_PVRGetLocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

int PVR::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	SET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	//setters common for ScheduledRecording and Recording
#ifndef HBBTV_1_2_1
	SET_READONLY("repeatDays");
	SET_READONLY("longName");
	SET_READONLY("longDescription");
#endif // HBBTV_1_2_1

	NATIVE_SET("startPadding", native_PVRSetStartPadding(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("endPadding", native_PVRSetEndPadding(getIdentifier(), NUMBER_VALUE(value[0])));
	SET_READONLY("name");
	SET_READONLY("description");
	SET_READONLY("startTime");
	SET_READONLY("duration");
	SET_READONLY("channel");

#if defined(HBBTV) && !defined(HBBTV_1_2_1)
	SET_READONLY("isSeries");
#endif // HBBTV && !HBBTV_1_2_1

#ifndef HBBTV_1_2_1
	SET_READONLY("programmeID");
	SET_READONLY("programmeIDType");
	SET_READONLY("episode");
	SET_READONLY("totalEpisodes");
#endif // !HBBTV_1_2_1

	//DAE spec ambiguity
	SET_READONLY("parentalRating");
	SET_READONLY("parentalRatings");


	//setters for Recording
	SET_READONLY("state");
	SET_READONLY("id");
#ifdef OIPF
	SET_READONLY("error");
	SET_READONLY("isManual");
	NATIVE_SET("doNotDelete", native_PVRSetDoNotDelete(getIdentifier(), BOOLEAN_VALUE(value[0])));
	NATIVE_SET("saveDays", native_PVRSetSaveDays(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("saveEpisodes", native_PVRSetSaveEpisodes(getIdentifier(), NUMBER_VALUE(value[0])));
	SET_READONLY("blocked");
	SET_READONLY("showType");
	SET_READONLY("subtitles");
	SET_READONLY("subtitleLanguages");
	SET_READONLY("isHD");
	SET_READONLY("isWidescreen");
	SET_READONLY("audioType");
	SET_READONLY("isMultilingual");
	SET_READONLY("audioLanguages");
	SET_READONLY("genres");
#endif // OIPF
	SET_READONLY("recordingStartTime");
	SET_READONLY("recordingDuration");
#ifdef OIPF
	SET_READONLY("bookmarks");
	SET_READONLY("locked");
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *PVR::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++)
	{
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

//static
void PVR::releaseNative(PVRHandle id)
{
	NATIVE_WARN_NOK(native_PVRReleaseHandle(id));
}

#endif // HBBTV_PVR || OIPF
