// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Programme.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeProgramme.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "Exception.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"
#include "ParentalRating.h"


#ifdef OIPF
#include "Channel.h"
#include "PVR.h"
#endif // OIPF
#include "macros.h"

using namespace Ooif;

void Programme::init()
{
	classType = CLASS_PROGRAMME;
}

Programme::Programme(ProgrammeHandle i) : WrapperObject<Programme>(i)
{
	ENTRY;
	init();
}

Programme::Programme(const Programme &original) : WrapperObject<Programme>(original.identifier)
{
	ENTRY;
	init();
}

Programme::~Programme()
{
	ENTRY;
}

int Programme::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("ID_TVA_CRID", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("ID_DVB_EVENT", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_NUMBER("ID_TVA_CRID", PROGRAMME_ID_TVA_CRID, JSP_GET_VALUE_CACHE);
	GET_NUMBER("ID_DVB_EVENT", PROGRAMME_ID_DVB_EVENT, JSP_GET_VALUE_CACHE);
#ifdef OIPF
	GET_NUMBER("ID_TVA_GROUP_CRID", PROGRAMME_ID_TVA_GROUP_CRID, JSP_GET_VALUE_CACHE);
#endif // OIPF
#endif // OIPF || HBBTV_1_2_1

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("name", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_STRING("name", native_ProgrammeGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_STRING("longName", native_ProgrammeGetLongName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("description", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_STRING("description", native_ProgrammeGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("longDescription", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_STRING("longDescription", native_ProgrammeGetLongDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF || HBBTV_1_2_1

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("startTime", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("duration", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("channelID", CHECK_BROADCAST);

	//DAE spec ambiguity
	GET_GUARDED_SECURITY_CHECK("parentalRatings", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("parentalRating", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_NUMBER("startTime", native_ProgrammeGetStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_ProgrammeGetDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("channelID", native_ProgrammeGetChannelID(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	//DAE spec ambiguity
	NATIVE_GET_VOIDP_COLLECTION("parentalRatings", obj,
								native_ProgrammeGetParentalRating(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								getParentalRatings(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("parentalRating", obj,
								native_ProgrammeGetParentalRating(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								getParentalRatings(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);

#ifdef OIPF
	NATIVE_GET_NUMBER("episode", native_ProgrammeGetEpisode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalEpisodes", native_ProgrammeGetTotalEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("programmeID", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("programmeIDType", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getSIDescriptors", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_GET_STRING("programmeID", native_ProgrammeGetProgrammeID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("programmeIDType", native_ProgrammeGetProgrammeIDType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	GET_FUNCTION(getSIDescriptors, obj, "n-", JSP_GET_VALUE_CACHE);
#endif // OIPF || HBBTV_1_2_1

#ifdef OIPF
	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	NATIVE_GET_OBJECT("channel", obj, native_ProgrammeGetChannel(getIdentifier(), &RETVAL),
	                  new Channel(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("blocked", native_ProgrammeGetBlocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("showType", native_ProgrammeGetShowType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitles", native_ProgrammeGetSubtitles(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isHD", native_ProgrammeGetIsHD(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("audioType", native_ProgrammeGetAudioType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isMultilingual", native_ProgrammeGetIsMultilingual(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genre", obj,
	                             native_ProgrammeGetGenre(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hasRecording", native_ProgrammeGetHasRecording(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("audioLanguages", obj,
	                             native_ProgrammeGetAudioLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("subtitleLanguages", obj,
	                             native_ProgrammeGetSubtitleLanguages(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("locked", native_ProgrammeGetLocked(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("recording", obj,
	                  native_ProgrammeGetRecording(getIdentifier(), &RETVAL),
	                  new PVR(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("IMI", native_ProgrammeGetIMI(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("groupCRIDs", obj,
	                             native_ProgrammeGetGroupCRIDs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

int Programme::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("name", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_SET("name", native_ProgrammeSetName(getIdentifier(), STRING_VALUE(value[0])));

#ifdef OIPF
	NATIVE_SET("longName", native_ProgrammeSetLongName(getIdentifier(), STRING_VALUE(value[0])));
#endif // OIPF

#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("description", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_SET("description", native_ProgrammeSetDescription(getIdentifier(), STRING_VALUE(value[0])));

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("longDescription", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_SET("longDescription", native_ProgrammeSetLongDescription(getIdentifier(), STRING_VALUE(value[0])));
#endif // OIPF || HBBTV_1_2_1

#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("startTime", CHECK_BROADCAST);
	SET_GUARDED_SECURITY_CHECK("duration", CHECK_BROADCAST);
	SET_GUARDED_SECURITY_CHECK("channelID", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_SET("startTime", native_ProgrammeSetStartTime(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("duration", native_ProgrammeSetDuration(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("channelID", native_ProgrammeSetChannelID(getIdentifier(), STRING_VALUE(value[0])));

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifdef HBBTV_SECURITY
	SET_GUARDED_SECURITY_CHECK("programmeID", CHECK_BROADCAST);
	SET_GUARDED_SECURITY_CHECK("programmeIDType", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	NATIVE_SET("programmeID", native_ProgrammeSetProgrammeID(getIdentifier(), STRING_VALUE(value[0])));
	NATIVE_SET("programmeIDType", native_ProgrammeSetProgrammeIDType(getIdentifier(), NUMBER_VALUE(value[0])));
#endif // OIPF || HBBTV_1_2_1

	//DAE spec ambiguity
	SET_READONLY("parentalRatings");
	SET_READONLY("parentalRating");

#ifdef OIPF
	NATIVE_SET("episode", native_ProgrammeSetEpisode(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("totalEpisodes", native_ProgrammeSetTotalEpisodes(getIdentifier(), NUMBER_VALUE(value[0])));
	SET_READONLY("IMI");
	SET_READONLY("groupCRIDs");
	SET_READONLY("channel");
	SET_READONLY("blocked");
	SET_READONLY("locked");
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *Programme::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++)
	{
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

#if defined(OIPF) || defined(HBBTV_1_2_1)
//static
int Programme::getSIDescriptors(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 2);
	Programme *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Programme);
	OOIFNumber tagExtension = 0.0;
	OOIFBoolean has_tagExtension = false;
	int count;
	char **strings = NULL;
	int *string_lenghts = NULL;
	int descTag=0;

	if (argc == 2)
	{
		CHECK_ARGUMENT(2, JSP_TYPE_NUMBER);
		tagExtension = NUMBER_VALUE(argv[1]);
		has_tagExtension = true;
	}
	descTag = NUMBER_VALUE(argv[0]);

	NATIVE_CALL(native_ProgrammeGetSIDescriptors(instance->getIdentifier(),
	           descTag,
	            has_tagExtension,
	            tagExtension,
	            &count, &strings, &string_lenghts));
	
	if(count == 0)
	{
		if (strings) HLIB_STD_MemFree(strings);
		if(string_lenghts) HLIB_STD_MemFree(string_lenghts);
		RETURN_NULL(JSP_CALL_VALUE);
	}

	RETURN_OBJECT(this_obj, OOIFUtils::CreateStringWithLengthCollection(count, strings, string_lenghts),
	              result, JSP_CALL_VALUE);
}
#endif // OIPF || HBBTV_1_2_1
#ifdef OIPF
//static
int Programme::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
                        jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Programme *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, Programme);
	const char *retval;
	NATIVE_CALL(native_ProgrammeGetField(instance->getIdentifier(),
	                                     STRING_VALUE(argv[0]),
	                                     &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}
#endif // OIPF

void Programme::releaseNative(ProgrammeHandle id)
{
	NATIVE_WARN_NOK(native_ProgrammeReleaseHandle(id));
}