// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(OIPF) && defined(HMX_WEBUI)

#include "PVREvent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"


using namespace Ooif;

namespace humax
{

PVREvent::~PVREvent()
{
	ENTRY;

	if (schedule)
		delete schedule;
	if (recording)
		delete recording;
	if (conflicts)
		delete conflicts;
	if (conflictRecs)
		delete conflictRecs;
	if (alternatives)
		delete alternatives;
	if (altevents)
		delete altevents;
}

PVREvent::PVREvent(int state, ScheduledRecordingHandle recordingHandle, int _error, ScheduledRecordingHandle *conflictsHandles, int count, ScheduledRecordingHandle *alternativesHandles, int alt_count, ProgrammeHandle *alteventHandles,int alt_evt_count, jsplugin_obj *c)
	: Ooif::PVREvent(NULL, state, c), error(_error)
{
    ENTRY;

    schedule  = NULL;
    recording = NULL;
#if 0
    OOIF_LOG_PRINT(
    	"\t*********************************************************\n"
    	"\t-     recordingHandle: 0x%X\n"
    	"\t- conflictsHandles[0]: 0x%X\n"
    	"\t- conflictsHandles[1]: 0x%X\n"
    	"\t- conflictsHandles[2]: 0x%X\n"
    	"\t- conflictsHandles[3]: 0x%X\n"
    	"\t- conflictsHandles[4]: 0x%X\n"
    	"\t*********************************************************\n"
    	, (int)recordingHandle
    	, (conflictsHandles && count > 0) ? conflictsHandles[0] : 0
    	, (conflictsHandles && count > 1) ? conflictsHandles[1] : 0
    	, (conflictsHandles && count > 2) ? conflictsHandles[2] : 0
    	, (conflictsHandles && count > 3) ? conflictsHandles[3] : 0
    	, (conflictsHandles && count > 4) ? conflictsHandles[4] : 0
    );
#endif
    OOIFBoolean	recorded;

    NATIVE_WARN_NOK(native_ScheduledRecordingIsRecorded(recordingHandle, &recorded));
    if ( recordingHandle )
    {
    	if (!recorded)
    		schedule  = new ScheduledRecording(recordingHandle);
    	else
    		recording = new Recording((RecordingHandle)recordingHandle);
    }

    conflicts    = NULL;
    conflictRecs = NULL;
    alternatives = NULL;
    altevents = NULL;
    conflictGroupInfo = NULL;

    if(conflictsHandles && count > 0)
    {
    	conflicts	 = new ScheduledRecordingCollection();
    	conflictRecs = new RecordingCollection();
    	for(int i = 0; i < count; i++)
    	{
            // Collection을 하나로 관리하면 좋지만, ScheduledRecording과 Recording이 상속관계가 아니라서 일단 둘로 나눈다.

            NATIVE_WARN_NOK(native_ScheduledRecordingIsRecorded(conflictsHandles[i], &recorded));
            if ( conflictsHandles[i] )
            {
            	if (!recorded)
            		conflicts->addItem(new ScheduledRecording(conflictsHandles[i]));
            	else
            		conflictRecs->addItem(new Recording((RecordingHandle)conflictsHandles[i]));
            }
    	}
    }

    if (alternativesHandles && alt_count > 0)
    {
        alternatives    = new ScheduledRecordingCollection();

        for (int i = 0 ; i < alt_count ; i++)
        {
            if ( alternativesHandles[i] )
            {
                alternatives->addItem(new ScheduledRecording(alternativesHandles[i]));
            }
        }
    }

    if(alteventHandles && alt_evt_count > 0)
    {
        altevents		= new ProgrammeCollection();

        for(int i= 0; i< alt_evt_count; i++)
        {
            if ( alteventHandles[i] )
            {
                altevents->addItem(new Programme(alteventHandles[i]));
            }
        }
    }
}

int PVREvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);

	// 0
	GET_NUMBER("state", state, JSP_GET_VALUE_CACHE);

	// 1
	if (schedule)
	{
		GET_OBJECT("recording", obj, new ScheduledRecording(schedule->getIdentifier()), JSP_GET_VALUE_CACHE);
	}
	else// if (recording)
	{
		GET_OBJECT("recording", obj, new Recording(recording->getIdentifier()), JSP_GET_VALUE_CACHE);
	}

	// 2
	GET_NUMBER("error", error, JSP_GET_VALUE_CACHE);

	// 3
	if (strcmp(name, "scheduledRecordingConflicts") == 0)
	{
		ScheduledRecordingCollection	*collection = getScheduledRecordingConflicts();
		if (collection == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		RETURN_OBJECT(obj, collection, result, JSP_GET_VALUE_CACHE);
	}

	// 4
	if (strcmp(name, "recordingConflicts") == 0)
	{
		RecordingCollection	*collection = getRecordingConflicts();
		if (collection == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		RETURN_OBJECT(obj, collection, result, JSP_GET_VALUE_CACHE);
	}

	// 5
	if (strcmp(name, "scheduledRecordingAlternatives") == 0)
	{
		ScheduledRecordingCollection	*collection = getScheduledRecordingAlternatives();
		if (collection == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		RETURN_OBJECT(obj, collection, result, JSP_GET_VALUE_CACHE);
	}

	// 6
	if (strcmp(name, "scheduledRecordingAltEvents") == 0)
	{
		ProgrammeCollection	*collection = getScheduledRecordingAltEvents();
		if (collection == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		RETURN_OBJECT(obj, collection, result, JSP_GET_VALUE_CACHE);
	}

	// 7
	if (strcmp(name, "conflictGroupInfo") == 0)
	{
		NumberCollection	*collection = getScheduledRecordingGroupInfo();
		if (collection == NULL)
		{
			RETURN_NULL(JSP_GET_VALUE);
		}
		RETURN_OBJECT(obj, collection, result, JSP_GET_VALUE_CACHE);
	}

	return Ooif::PVREvent::getter(obj, name, result);
}

int PVREvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return Ooif::PVREvent::setter(obj, name, value);
}

//virtual
int PVREvent::serialize(jsplugin_value **argv)
{
#ifdef JLABS
	*argv = new jsplugin_value[7];
#else
	*argv = new jsplugin_value[8];
#endif
	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = state;

	result[1].type = JSP_TYPE_NULL;
	if (schedule)
	{
		SET_SERIALIZED_OBJECT(new ScheduledRecording(schedule->getIdentifier()), 1);
	}
	else// if (recording)
	{
		SET_SERIALIZED_OBJECT(new Recording(recording->getIdentifier()), 1);
	}

	result[2].type = JSP_TYPE_NUMBER;
	result[2].u.number = error;

	result[3].type = JSP_TYPE_NULL;
	result[4].type = JSP_TYPE_NULL;
	result[5].type = JSP_TYPE_NULL;
	result[6].type = JSP_TYPE_NULL;

	ScheduledRecordingCollection *sConflict = getScheduledRecordingConflicts();
	if (sConflict)
	{
		SET_SERIALIZED_OBJECT(sConflict, 3);
	}

	RecordingCollection *rConflict = getRecordingConflicts();
	if (rConflict)
	{
		SET_SERIALIZED_OBJECT(rConflict, 4);
	}

	ScheduledRecordingCollection *sAlternative = getScheduledRecordingAlternatives();
	if (sAlternative)
	{
		SET_SERIALIZED_OBJECT(sAlternative, 5);
	}

	ProgrammeCollection *sAltEvent = getScheduledRecordingAltEvents();
	if (sAltEvent)
	{
		SET_SERIALIZED_OBJECT(sAltEvent, 6);
	}

#ifdef JLABS
	return 7;
#else
	result[7].type = JSP_TYPE_NULL;
	NumberCollection *sConflictGroupInfo = getScheduledRecordingGroupInfo();
	if (sConflictGroupInfo)
	{
		SET_SERIALIZED_OBJECT(sConflictGroupInfo, 7);
	}

	return 8;
#endif
}

ScheduledRecordingCollection*	PVREvent::getScheduledRecordingConflicts ()
{
	ENTRY;

	if (conflicts == NULL || conflicts->getSize() <= 0)
		return NULL;

	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();

	for (int i = 0 ; i < conflicts->getSize() ; i++)
	{
		collection->addItem(new ScheduledRecording(*(conflicts->internalGetItem(i))));
	}
	return collection;
}

ScheduledRecordingCollection*	PVREvent::getScheduledRecordingAlternatives ()
{
	ENTRY;

	if (alternatives == NULL || alternatives->getSize() <= 0)
		return NULL;

	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();

	for (int i = 0 ; i < alternatives->getSize() ; i++)
	{
		collection->addItem(new ScheduledRecording(*(alternatives->internalGetItem(i))));
	}
	return collection;
}

ProgrammeCollection*	PVREvent::getScheduledRecordingAltEvents ()
{
	ENTRY;

	if (altevents == NULL || altevents->getSize() <= 0)
		return NULL;

	ProgrammeCollection *collection = new ProgrammeCollection();

	for (int i = 0 ; i < altevents->getSize() ; i++)
	{
		collection->addItem(new Programme(*(altevents->internalGetItem(i))));
	}
	return collection;
}

NumberCollection*	PVREvent::getScheduledRecordingGroupInfo ()
{
	ENTRY;

	if (conflictGroupInfo == NULL || conflictGroupInfo->getSize() <= 0)
		return NULL;

	NumberCollection *collection = new NumberCollection();

	for (int i = 0 ; i < conflictGroupInfo->getSize() ; i++)
	{
		collection->addItem(conflictGroupInfo->internalGetItem(i));
	}
	return collection;
//	return conflictGroupInfo;
}

RecordingCollection*			PVREvent::getRecordingConflicts ()
{
	ENTRY;

	if (conflictRecs == NULL || conflictRecs->getSize() <= 0)
		return NULL;

	RecordingCollection *collection = new RecordingCollection();

	for (int i = 0 ; i < conflictRecs->getSize() ; i++)
	{
		collection->addItem(new Recording(*(conflictRecs->internalGetItem(i))));
	}
	return collection;
}

}//namespace humax

#endif // OIPF
