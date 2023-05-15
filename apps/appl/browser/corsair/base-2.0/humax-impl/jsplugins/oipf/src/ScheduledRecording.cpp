// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "ScheduledRecording.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeRecording.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "ParentalRating.h"
#include "Channel.h"

#include "macros.h"

using namespace Ooif;

void ScheduledRecording::init()
{
	classType = CLASS_SCHEDULEDRECORDING;
}

ScheduledRecording::ScheduledRecording(ScheduledRecordingHandle i) : WrapperObject<ScheduledRecording>(i)
{
	ENTRY;
	init();
}

ScheduledRecording::ScheduledRecording(const ScheduledRecording &original) : WrapperObject<ScheduledRecording>(original.identifier)
{
	ENTRY;
	init();
}

ScheduledRecording::~ScheduledRecording()
{
	ENTRY;
}

int ScheduledRecording::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
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

#ifdef JLABS
	GET_FUNCTION(setName, obj, "s", JSP_GET_VALUE);

	NATIVE_GET_NUMBER("factoryType", native_ScheduledRecordingGetFactoryType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("discId", native_ScheduledRecordingGetStorageId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recMode", native_ScheduledRecordingGetRecMode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("fromDap", native_ScheduledRecordingGetFromDap(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("groupIndex", native_ScheduledRecordingGetGroupIndex(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("userID", native_ScheduledRecordingGetUserID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("deviceHandle", native_ScheduledRecordingGetDeviceHandle(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("destinationId", native_ScheduledRecordingGetDestinationId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#elif HMX_WEBUI
	NATIVE_GET_NUMBER("factoryType", native_ScheduledRecordingGetFactoryType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("uid", native_ScheduledRecordingGetUID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif
	NATIVE_GET_NUMBER("startPadding", native_ScheduledRecordingGetStartPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("endPadding", native_ScheduledRecordingGetEndPadding(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("repeatDays", native_ScheduledRecordingGetRepeatDays(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_ScheduledRecordingGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longName", native_ScheduledRecordingGetLongName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_ScheduledRecordingGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longDescription", native_ScheduledRecordingGetLongDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("startTime", native_ScheduledRecordingGetStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_ScheduledRecordingGetDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_OBJECT("channel", obj,
					  native_ScheduledRecordingGetChannel(getIdentifier(), &RETVAL),
					  new Channel(RETVAL), JSP_GET_VALUE);

#if defined HMX_WEBUI
	NATIVE_GET_BOOLEAN("isScheduledAsSeries", native_ScheduledRecordingGetIsScheduledAsSeries(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("seriesID", native_ScheduledRecordingGetSeriesID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("groupCRIDs", obj,
	                             native_SchedulerGetGroupCRIDs(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                             JSP_GET_VALUE);
	NATIVE_GET_NUMBER("volume", native_ScheduledRecordingGetVolume(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("programmeCRID", native_ScheduledRecordingGetProgrammeCRID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("status", native_ScheduledRecordingGetStatus(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("eventbased", native_ScheduledRecordingIsEventBased(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_NUMBER("diseqcInput", native_ScheduledRecordingGetDiseqcInput(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("frequency", native_ScheduledRecordingGetFrequency(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("polarisation", native_ScheduledRecordingGetPolarisation(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("symbolRate", native_ScheduledRecordingGetSymbolRate(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("satType", native_ScheduledRecordingGetSatType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif /* HMX_WEBUI */
	NATIVE_GET_STRING("programmeID", native_ScheduledRecordingGetProgrammeID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_ENUM("programmeIDType", ScheduledRecordingProgrammeIDType, native_ScheduledRecordingGetProgrammeIDType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#if defined OIPF || defined OPT_HBBTV_SCHED_REC_SERIES
	NATIVE_GET_NUMBER("episode", native_ScheduledRecordingGetEpisode(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("totalEpisodes", native_ScheduledRecordingGetTotalEpisodes(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif /* OIPF || OPT_HBBTV_SCHED_REC_SERIES */
#ifdef OIPF
    NATIVE_GET_VOIDP_COLLECTION("parentalRatings", obj,
                                native_ScheduledRecordingGetParentalRating(getIdentifier(), &SIZE, &NATIVE_ARRAY),
                                getParentalRatings(SIZE, NATIVE_ARRAY),
                                JSP_GET_VALUE);
#else /* OIPF */
    NATIVE_GET_VOIDP_COLLECTION("parentalRating", obj,
                                native_ScheduledRecordingGetParentalRating(getIdentifier(), &SIZE, &NATIVE_ARRAY),
                                getParentalRatings(SIZE, NATIVE_ARRAY),
                                JSP_GET_VALUE);
#endif /* OIPF */
	return JSP_GET_NOTFOUND;
}

int ScheduledRecording::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	SET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	NATIVE_SET("startPadding", native_ScheduledRecordingSetStartPadding(getIdentifier(), NUMBER_VALUE(value[0])));
	NATIVE_SET("endPadding", native_ScheduledRecordingSetEndPadding(getIdentifier(), NUMBER_VALUE(value[0])));
#if defined HMX_WEBUI
	NATIVE_SET("duration", native_ScheduledRecordingSetDuration(getIdentifier(), NUMBER_VALUE(value[0])));
#endif
	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *ScheduledRecording::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++) {
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

void ScheduledRecording::releaseNative(ScheduledRecordingHandle id)
{
	NATIVE_WARN_NOK(native_ScheduledRecordingReleaseHandle(id));
}

#ifdef JLABS
int ScheduledRecording::setName(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	const char *name = STRING_VALUE(argv[0]);
	OOIFBoolean retval;

	ScheduledRecording *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, ScheduledRecording);

	NATIVE_CALL(native_SchedulerSetName(instance->getIdentifier(), name, &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // JLABS

#endif // HBBTV_PVR || OIPF

/* EOF */

