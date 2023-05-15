// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "Recording.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeRecording.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "Collection.h"
#include "BookmarkCollection.h"
#include "ParentalRating.h"
#include "Channel.h"

#include "macros.h"

using namespace Ooif;

void Recording::init()
{
	classType = CLASS_RECORDING;
}

Recording::Recording(RecordingHandle i) : ScheduledRecording(i)
{
	ENTRY;
	init();
}

Recording::Recording(const Recording &original) : ScheduledRecording(original.identifier)
{
	ENTRY;
	init();
}

Recording::~Recording()
{
	ENTRY;
}

int Recording::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

#ifdef OIPF
    GET_NUMBER("ID_TVA_CRID", SCHEDULED_RECORDING_ID_TVA_CRID, JSP_GET_VALUE_CACHE);
#endif // OIPF
    GET_NUMBER("ID_DVB_EVENT", SCHEDULED_RECORDING_ID_DVB_EVENT, JSP_GET_VALUE_CACHE);
#ifdef OIPF
    GET_NUMBER("ID_TVA_GROUP_CRID", SCHEDULED_RECORDING_ID_TVA_GROUP_CRID, JSP_GET_VALUE_CACHE);
#endif /* OIPF */

#ifdef OIPF
// Part inherited from the ScheduledRecording class.
// Have to double it to make the integration layer able to differ a ScheduledRecording Object from a Recording Object
#ifdef JLABS

	NATIVE_GET_NUMBER("factoryType", native_RecordingGetFactoryType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recMode", native_RecordingGetRecMode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("fromDap", native_RecordingGetFromDap(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("groupIndex", native_RecordingGetGroupIndex(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("userID", native_RecordingGetUserID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceHandle", native_RecordingGetDeviceHandle(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("destinationId", native_RecordingGetDestinationId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif
	NATIVE_GET_NUMBER("startPadding", native_RecordingGetStartPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("endPadding", native_RecordingGetEndPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("repeatDays", native_RecordingGetRepeatDays(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_RecordingGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longName", native_RecordingGetLongName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_RecordingGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longDescription", native_RecordingGetLongDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("startTime", native_RecordingGetStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_RecordingGetDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_OBJECT("channel", obj,
	                  native_RecordingGetChannel(getIdentifier(), &RETVAL),
	                  new Channel(RETVAL), JSP_GET_VALUE);
#ifdef HMX_WEBUI
	NATIVE_GET_BOOLEAN("isScheduledAsSeries", native_ScheduledRecordingGetIsScheduledAsSeries(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("seriesID", native_ScheduledRecordingGetSeriesID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(setName, obj, "s", JSP_GET_VALUE);
	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
#endif // HMX_WEBUI

#if (defined(JLABS) || defined(HMX_WEBUI))
	NATIVE_GET_STRING("discId", native_RecordingGetStorageId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif //(defined(JLABS) || defined(HMX_WEBUI))

	NATIVE_GET_STRING("programmeID", native_RecordingGetProgrammeID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_ENUM("programmeIDType", ScheduledRecordingProgrammeIDType, native_RecordingGetProgrammeIDType(getIdentifier(), &RETVAL), JSP_GET_VALUE);

#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
	NATIVE_GET_NUMBER("episode", native_RecordingGetEpisode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalEpisodes", native_RecordingGetTotalEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef HMX_WEBUI
	NATIVE_GET_NUMBER("season", native_RecordingGetSeason(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif
#endif // OOIF || OPT_HBBTV_SCHED_REC_SERIES
//-------------------- end of the interface inherited from the ScheduledRecording class

    GET_NUMBER("RECORDING_UNREALIZED", RECORDING_UNREALIZED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_SCHEDULED", RECORDING_SCHEDULED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_DEL_SCHEDULED", RECORDING_DEL_SCHEDULED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_PRESTART", RECORDING_REC_PRESTART, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_ACQUIRING_RESOURCES", RECORDING_REC_ACQUIRING_RESOURCES, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_STARTED", RECORDING_REC_STARTED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_UPDATED", RECORDING_REC_UPDATED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_COMPLETED", RECORDING_REC_COMPLETED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_PARTIALLY_COMPLETED", RECORDING_REC_PARTIALLY_COMPLETED, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_REC_START_ERROR", RECORDING_REC_START_ERROR, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_TS_ACQUIRING_RESOURCES", RECORDING_TS_ACQUIRING_RESOURCES, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_TS_STARTED", RECORDING_TS_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("RECORDING_REC_LOAD", RECORDING_REC_LOAD, JSP_GET_VALUE_CACHE);
    GET_NUMBER("RECORDING_ERROR", RECORDING_ERROR, JSP_GET_VALUE_CACHE);

    GET_NUMBER("ERROR_REC_RESOURCE_LIMITATION", RECORDING_ERROR_REC_RESOURCE_LIMITATION, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_INSUFFICIENT_STORAGE", RECORDING_ERROR_INSUFFICIENT_STORAGE, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_TUNER_CONFLICT", RECORDING_ERROR_TUNER_CONFLICT, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_REC_DRM_RESTRICTION", RECORDING_ERROR_REC_DRM_RESTRICTION, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_REC_UNKNOWN", RECORDING_ERROR_REC_UNKNOWN, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_TS_RESOURCE_LIMITATION", RECORDING_ERROR_TS_RESOURCE_LIMITATION, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_TS_DRM_RESTRICTION", RECORDING_ERROR_TS_DRM_RESTRICTION, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_TS_UNKNOWN", RECORDING_ERROR_TS_UNKNOWN, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_NO_AV", RECORDING_ERROR_NO_AV, JSP_GET_VALUE_CACHE);
    GET_NUMBER("ERROR_POWER_OFF", RECORDING_ERROR_POWER_OFF, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ERROR_REC_CAS", RECORDING_ERROR_REC_CAS, JSP_GET_VALUE_CACHE);
#endif // OIPF
	NATIVE_GET_NUMBER("state", native_RecordingGetState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBER("error", native_RecordingGetError(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF
	NATIVE_GET_STRING("id", native_RecordingGetID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_BOOLEAN("isManual", native_RecordingGetIsManual(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("doNotDelete", native_RecordingGetDoNotDelete(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("saveDays", native_RecordingGetSaveDays(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("saveEpisodes", native_RecordingGetSaveEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("blocked", native_RecordingGetBlocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF
	NATIVE_GET_VOIDP_COLLECTION("parentalRatings", obj,
								native_RecordingGetParentalRatings(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								getParentalRatings(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBER("showType", native_RecordingGetShowType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitles", native_RecordingGetSubtitles(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("subtitleLanguages", obj,
								 native_RecordingGetSubtitleLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isHD", native_RecordingGetIsHD(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isWidescreen", native_RecordingGetIsWidescreen(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("audioType", native_RecordingGetAudioType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isMultilingual", native_RecordingGetIsMultilingual(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("audioLanguages", obj,
								 native_RecordingGetAudioLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genres", obj,
								 native_RecordingGetGenres(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
#endif // OIPF
	NATIVE_GET_NUMBER("recordingStartTime", native_RecordingGetRecordingStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recordingDuration", native_RecordingGetRecordingDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_VOIDP_COLLECTION("bookmarks", obj,
								native_RecordingGetBookmarks(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								new BookmarkCollection(getIdentifier(), SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("locked", native_RecordingGetLocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef HMX_WEBUI
	NATIVE_GET_BOOLEAN("willBeDeleted", native_RecordingGetWillBeDeleted(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("lastPlayPosition", native_RecordingGetLastPlayPosition(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbnail", native_RecordingGetThumbnail(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isManualLocked", native_RecordingGetIsManualLocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isScrambled", native_RecordingGetIsScrambled(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isEncrypted", native_RecordingGetIsEncrypted(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("copyControl", native_RecordingGetCopyControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBERS_COLLECTION("casIDs", obj,
								 native_RecordingGetCasIDs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 getCasIDs(SIZE, NATIVE_ARRAY),
								 JSP_GET_VALUE);
	
	NATIVE_GET_STRING_COLLECTION("groupCRIDs", obj,
								 native_RecordingGetGroupCRIDs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								 JSP_GET_VALUE);
	NATIVE_GET_STRING("programmeCRID", native_RecordingGetProgrammeCRID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("seriesTitle", native_RecordingGetSeriesTitle(getIdentifier(), &RETVAL), JSP_GET_VALUE);


	NATIVE_GET_STRING_COLLECTION("subtitleTypes", obj,
						 native_RecordingGetSubtitleTypes(getIdentifier(), &SIZE, &NATIVE_ARRAY),
						 JSP_GET_VALUE);
	NATIVE_GET_NUMBER("doNotDeleteSetTime", native_RecordingGetDoNotDeleteSetTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("expiryDate", native_RecordingGetExpiryDate(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("pausedPoints", obj,
								native_RecordingGetPausedPoints(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								new BookmarkCollection(getIdentifier(), SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
#endif // HMX_WEBUI
#endif // OIPF

#ifdef JLABS
	NATIVE_GET_NUMBER("digitalCopyControl", native_RecordingGetDigitalCopyControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("analogCopyControl", native_RecordingGetAnalogCopyControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isTranscoded", native_RecordingGetTranscoded(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("digitalCopyCount", native_RecordingGetDigitalCopyCount(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbnailURL", native_RecordingGetThumbnailURL(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isWatched", native_RecordingGetTranscoded(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("chapters", obj,
								native_RecordingGetChapters(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								new BookmarkCollection(getIdentifier(), SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);

#endif // JLABS

	return JSP_GET_NOTFOUND;
}

int Recording::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#ifdef OIPF
	//NATIVE_SET("doNotDelete", native_RecordingSetDoNotDelete(getIdentifier(), BOOLEAN_VALUE(value[0])));
	NATIVE_SET("saveDays", native_RecordingSetSaveDays(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("saveEpisodes", native_RecordingSetSaveEpisodes(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("endPadding", native_RecordingSetEndPadding(getIdentifier(), NUMBER_VALUE(value[0])));
#ifdef HMX_WEBUI
	NATIVE_SET("isManualLocked", native_RecordingSetIsManualLocked(getIdentifier(), BOOLEAN_VALUE(value[0])));
#endif
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *Recording::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++) {
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

void Recording::releaseNative(RecordingHandle id)
{
	NATIVE_WARN_NOK(native_RecordingReleaseHandle(id));
}

#ifdef HMX_WEBUI
//static
NumberCollection *Recording::getCasIDs(int count, OOIFNumber *casIDs)
{
	NumberCollection *collection = new NumberCollection();
	
	for (int i = 0 ; casIDs && i < count ; i++) 
	{
		collection->addItem(casIDs[i]);
	}
	
	if (casIDs)
	{
		HLIB_STD_MemFree(casIDs);
	}
	return collection;
}

int Recording::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
						jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Recording *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Recording);
	const char *retval;
	NATIVE_CALL(native_RecordingGetField(instance->getIdentifier(),
										 STRING_VALUE(argv[0]),
										 &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int Recording::setName(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	const char *name = STRING_VALUE(argv[0]);
	OOIFBoolean retval;

	Recording *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Recording);

	NATIVE_CALL(native_RecordingSetName(instance->getIdentifier(), name, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif

#endif // HBBTV_PVR || OIPF

/* EOF */

