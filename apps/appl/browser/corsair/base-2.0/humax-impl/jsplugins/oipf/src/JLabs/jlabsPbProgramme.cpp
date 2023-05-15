// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "jlabsPbProgramme.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeJlabsPbProgramme.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY
#include "AVComponent.h"

#include "Exception.h"
#include "Collection.h"
#include "ParentalRatingCollection.h"
#include "ParentalRating.h"


#ifdef OIPF
#include "Channel.h"
#include "ScheduledRecording.h"
#include "Recording.h"
#endif // OIPF

#include "macros.h"
#ifdef JLABS
#include "jlabsDataContentComponent.h"
#endif // JLABS

using namespace Ooif;

void jlabsPbProgramme::init()
{
	classType = CLASS_JLABS_PBPROGRAMME;
}

jlabsPbProgramme::jlabsPbProgramme(jlabsPbProgrammeHandle i) : WrapperObject<jlabsPbProgramme>(i)
{
	ENTRY;
	init();
}

jlabsPbProgramme::jlabsPbProgramme(const jlabsPbProgramme &original) : WrapperObject<jlabsPbProgramme>(original.identifier)
{
	ENTRY;
	init();
}

jlabsPbProgramme::~jlabsPbProgramme()
{
	ENTRY;
}

int jlabsPbProgramme::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_VOIDP_COLLECTION("parentalRating", obj,
								native_jlabsPbProgrammeGetParentalRating(getIdentifier(), &SIZE, &NATIVE_ARRAY),
								getParentalRatings(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
	NATIVE_GET_STRING("name", native_jlabsPbProgrammeGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_jlabsPbProgrammeGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("longDescription", native_jlabsPbProgrammeGetLongDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("startTime", native_jlabsPbProgrammeGetStartTime(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_jlabsPbProgrammeGetDuration(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("channelID", native_jlabsPbProgrammeGetChannelID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("programmeID", native_jlabsPbProgrammeGetProgrammeID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("programmeIDType", native_jlabsPbProgrammeGetProgrammeIDType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_OBJECT("channel", obj, native_jlabsPbProgrammeGetChannel(getIdentifier(), &RETVAL), new Channel(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("hasRecording", native_jlabsPbProgrammeGetHasRecording(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("subtitles", native_jlabsPbProgrammeGetSubtitles(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isHD", native_jlabsPbProgrammeGetIsHD(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING_COLLECTION("genre", obj, native_jlabsPbProgrammeGetGenre(getIdentifier(), &SIZE, &NATIVE_ARRAY), JSP_GET_VALUE);
	GET_FUNCTION(getField, obj, "s", JSP_GET_VALUE_CACHE);
	NATIVE_GET_NUMBER("digitalCopyControl", native_jlabsPbProgrammeGetDigitalCopyControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("analogCopyControl", native_jlabsPbProgrammeGetAnalogCopyControl(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("is3D", native_jlabsPbProgrammeGetIs3D(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_BOOLEAN("isCharged", native_jlabsPbProgrammeGetIsCharged(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("contractVertificationInfo", native_jlabsPbProgrammeGetContractVertificationInfo(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("recordingStatus", native_jlabsPbProgrammeGetRecordingStatus(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	GET_FUNCTION(getComponents, obj, "n", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int jlabsPbProgramme::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *jlabsPbProgramme::getParentalRatings(int count, ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++)
	{
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

int jlabsPbProgramme::getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
								jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	jlabsPbProgramme *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsPbProgramme);
	const char *retval;

	NATIVE_CALL(native_jlabsPbProgrammeGetField(instance->getIdentifier(), STRING_VALUE(argv[0]), &retval));
	RETURN_STRING(retval, JSP_CALL_VALUE);
}

int jlabsPbProgramme::getComponents(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	jlabsPbProgramme *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsPbProgramme);
	AVComponentCollection *retval = new AVComponentCollection();
	int i, count = 0;
	AVComponentHandle *handles;
	int componentType = NUMBER_VALUE(argv[0]);

	NATIVE_CALL(native_jlabsPbProgrammeGetComponents(instance->getIdentifier(),	componentType, &count, &handles));
	if ((count > 0) && (NULL != handles))
	{
		for (i=0; i<count; i++)
		{
			if( componentType == 0 )
			{ // VIDEO
				retval->addItem(new VideoComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 1)
			{// AUDIO
				retval->addItem(new AudioComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 2)
			{// SUBTITLE
				retval->addItem(new SubtitleComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else if( componentType == 4)
			{// DATA
				retval->addItem(new jlabsDataContentComponent((AVComponentHandle)handles[i], instance->classType));
			}
			else
			{ /* do nothing */ }
		}
	}
	if (handles)
	{
		HLIB_STD_MemFree(handles);
	}

	RETURN_OBJECT(this_obj, retval, result, JSP_CALL_VALUE);
}

void jlabsPbProgramme::releaseNative(jlabsPbProgrammeHandle id)
{
	OOIF_LOG_DUMMY("[%s:%d] called\n",__PRETTY_FUNCTION__,__LINE__);

	NATIVE_WARN_NOK(native_jlabsPbProgrammeReleaseHandle(id));
}


