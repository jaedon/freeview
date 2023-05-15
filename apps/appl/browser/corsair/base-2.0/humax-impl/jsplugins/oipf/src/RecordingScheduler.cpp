// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_PVR) || defined(OIPF)

#include "RecordingScheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "OOIFUtils.h"

#include "NativeRecording.h"
#include "NativeProgramme.h"
#include "NativeCapabilities.h"
#include "NativeDisc.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "ScheduledRecording.h"
#include "Collection.h"
#include "Recording.h"
#include "ChannelConfig.h"
#ifdef JLABS
#include "HDiscInfo.h"
#else
#include "DiscInfo.h"
#endif
#include "PVREvent.h"
#include "Programme.h"
#include "Exception.h"
#ifdef JLABS
#include "HNReservationFinishedEvent.h"
#include "HNRemoveResultEvent.h"
#include "jlabsCDSRecording.h"
#endif // JLABS

using namespace Ooif;

list<RecordingScheduler*> RecordingScheduler::recordingSchedulers;

RecordingScheduler::RecordingScheduler()
{
	ENTRY;
	classType = CLASS_RECORDINGSCHEDULER;
	recordingSchedulers.push_back(this);

#if !defined(JLABS) && defined(HMX_WEBUI)
	native_SchedulerRegRecFileOpEventListener(RecordingScheduler::onRecordingFileOperationEvent);
#endif /* !defined(JLABS) && defined(HMX_WEBUI) */
}

RecordingScheduler::~RecordingScheduler()
{
	ENTRY;
	RecordingScheduler::recordingSchedulers.remove(this);
}

int RecordingScheduler::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

#if !defined(JLABS) && !defined(HMX_WEBUI)
	GET_FUNCTION(record, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordAt, obj, "iiis", JSP_GET_VALUE_CACHE);
#endif
#if defined(HMX_WEBUI)
	GET_FUNCTION(getCountScheduleList, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getCountRecordingList, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getScheduleList, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getRecordingList, obj, "", JSP_GET_VALUE_CACHE);
#endif
	GET_FUNCTION(getScheduledRecordings, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelConfig, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(remove, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(createProgrammeObject, obj, "", JSP_GET_VALUE_CACHE);
	NATIVE_GET_VOIDP_COLLECTION("recordings", obj,
								native_SchedulerGetRecordings(&SIZE, &NATIVE_ARRAY),
								getAllRecordings(SIZE, NATIVE_ARRAY),
								JSP_GET_VALUE);
	GET_FUNCTION(removeSeries, obj, "-", JSP_GET_VALUE_CACHE);
#ifdef OIPF
#ifndef JLABS
	NATIVE_GET_OBJECT("discInfo", obj, native_DiscInfoOnCreate(), new DiscInfo(), JSP_GET_VALUE);
#endif
	GET_FUNCTION(stop, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(refresh, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getRecording, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(startSatRecord, obj, "", JSP_GET_VALUE_CACHE);
	INTRINSIC_EVENT_GET(PVREvent);
#endif // OIPF

#if !defined(JLABS) && defined(HMX_WEBUI)
	GET_FUNCTION(record, obj, "oii", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordAt, obj, "iiisii", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordCrid, obj, "isss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordConfirm, obj, "-", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(update, obj, "oiiisii", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(copyRecording, obj, "oi", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(moveRecording, obj, "oi", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeRecording, obj, "o", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(stopFileOp, obj, "i", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setSeriesLock, obj, "sb", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setDoNotDelete, obj, "oi", JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(FileOpEvent);
#endif //!defined(JLABS) && defined(HMX_WEBUI)

#ifdef JLABS
	GET_FUNCTION(record, obj, "io--", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(recordAt, obj, "iiiis--", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(copy, obj, "ioss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getRecordings, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getCurrentRecordings, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(setDefaultDMS, obj, "ss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getDefaultDMSDeviceHandle, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getDefaultDMSRecordDestinationId, obj, "", JSP_GET_VALUE_CACHE);

	NATIVE_GET_OBJECT("discInfo", obj,
					  native_RecordingGetDiscInfo(&RETVAL),
					  new HDiscInfo(RETVAL), JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("discs", obj,
								native_RecordingGetStorages(&SIZE, &NATIVE_ARRAY),
								getDiscs(SIZE, NATIVE_ARRAY), JSP_GET_VALUE);

	INTRINSIC_EVENT_GET(HNRemoveResult);
	INTRINSIC_EVENT_GET(HNReservationFinished);
	GET_NUMBER("USB_RECORDING", (int)(RECORDING_SCHEDULER_USB_RECORDING), JSP_GET_VALUE_CACHE);
	GET_NUMBER("VIEWING_RESERVATION", (int)(RECORDING_SCHEDULER_VIEWING_RESERVATION), JSP_GET_VALUE_CACHE);
	GET_NUMBER("DLNA_UPLOAD", (int)(RECORDING_SCHEDULER_DLNA_UPLOAD), JSP_GET_VALUE_CACHE);
	GET_NUMBER("DLNA_COPY_RESERVATION", (int)(RECORDING_SCHEDULER_DLNA_COPY), JSP_GET_VALUE_CACHE);
#endif // JLABS

#ifdef OIPF
	return EventTarget::getter(obj, name, result);
#else
	return JSP_GET_NOTFOUND;
#endif // OIPF
}

int RecordingScheduler::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#ifdef OIPF
	INTRINSIC_EVENT(PVREvent);
#endif  // OIPF

#if !defined(JLABS) && defined(HMX_WEBUI)
	INTRINSIC_EVENT(FileOpEvent);
#endif /* #if !defined(JLABS) && defined(HMX_WEBUI) */

#ifdef JLABS
	INTRINSIC_EVENT(HNRemoveResult);
	INTRINSIC_EVENT(HNReservationFinished);
#endif	// JLABS

	return JSP_PUT_NOTFOUND;
}

#ifdef JLABS
//static
int RecordingScheduler::record(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
    CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
    //CHECK_ARGUMENT_COUNT(3);
    OOIFNumber	factory = NUMBER_VALUE(argv[0]);
    Programme	*p = OBJECT_VALUE_OR_THROW(p, argv[1], Programme);
    ScheduledRecordingHandle sri = NULL;

    int nfac = (int)factory;

    switch(nfac)
    {
    	case 0:	// USB_RECORDING
    		NATIVE_CALL(native_SchedulerRecord(p->getIdentifier(), factory, NUMBER_VALUE(argv[3]), STRING_VALUE(argv[2]), NULL,
    									OOIFUtils::getWindow(this_obj),
                                        OOIFUtils::getHost(this_obj), &sri));
    		break;
    	case 1:	// VIEW_RESERVATION
    		NATIVE_CALL(native_SchedulerRecord(p->getIdentifier(), factory, 0, NULL, NULL,
    									OOIFUtils::getWindow(this_obj),
                                        OOIFUtils::getHost(this_obj), &sri));
    		break;
    	case 2:	// DLNA
    		NATIVE_CALL(native_SchedulerRecord(p->getIdentifier(), factory, 0, STRING_VALUE(argv[2]), STRING_VALUE(argv[3]),
    									OOIFUtils::getWindow(this_obj),
                                        OOIFUtils::getHost(this_obj), &sri));
    		break;
        default:
            RETURN_NULL(JSP_CALL_VALUE);
            break;
    }


    RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}

int RecordingScheduler::recordAt(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
    CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
    //CHECK_ARGUMENT_COUNT(6);

    OOIFNumber	factory = NUMBER_VALUE(argv[0]);
    ScheduledRecordingHandle sri = NULL;

    int nfac = (int)factory;

    switch(nfac)
    {
    	case 0:	// USB_RECORDING
    		NATIVE_CALL(native_SchedulerRecordAt( NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), NUMBER_VALUE(argv[3]),
    									 STRING_VALUE(argv[4]), NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[6]), STRING_VALUE(argv[5]), NULL,
    									 OOIFUtils::getHost(this_obj), &sri));
    		break;
    	case 1:	// VIEW_RESERVATION
    		NATIVE_CALL(native_SchedulerRecordAt(NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), NUMBER_VALUE(argv[3]),
    									 STRING_VALUE(argv[4]), NUMBER_VALUE(argv[0]), 0, NULL, NULL,
    									 OOIFUtils::getHost(this_obj), &sri));
    		break;
    	case 2:	// DLNA
    		NATIVE_CALL(native_SchedulerRecordAt(NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), NUMBER_VALUE(argv[3]),
    									 STRING_VALUE(argv[4]), NUMBER_VALUE(argv[0]), 0, STRING_VALUE(argv[5]), STRING_VALUE(argv[6]),
    									 OOIFUtils::getHost(this_obj), &sri));
    		break;
        default:
            RETURN_NULL(JSP_CALL_VALUE);
            break;     
    }

    RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}

int RecordingScheduler::copy(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	//CHECK_ARGUMENT_COUNT(6);
	Recording	*p = OBJECT_VALUE_OR_THROW(p, argv[1], Recording);

	ScheduledRecordingHandle sri = NULL;

	NATIVE_CALL(native_SchedulerCopy(NUMBER_VALUE(argv[0]), p->getIdentifier(), STRING_VALUE(argv[2]), STRING_VALUE(argv[3]), &sri));

	RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}

int RecordingScheduler::getRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	const char *discId = STRING_VALUE(argv[0]);

	RecordingHandle *handles = NULL;
	int count	= 0;
	NATIVE_CALL(native_Jlabs_SchedulerGetRecordings(discId, &count, &handles));
	RecordingCollection *collection = new RecordingCollection();

	for (int i=0; i<count; i++)
	{
		collection->addItem(new Recording(handles[i]));
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	handles = NULL;
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::setDefaultDMS(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(2);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	CHECK_ARGUMENT(1, JSP_TYPE_STRING);

	OOIFNumber retval;

	const char *device_handle 	= STRING_VALUE(argv[0]);
	const char *destination_id	= STRING_VALUE(argv[1]);

	NATIVE_CALL(native_SchedulerSetDefaultDMS(device_handle, destination_id, &retval));
	RETURN_NUMBER(retval, JSP_CALL_VALUE);
}

int RecordingScheduler::getDefaultDMSDeviceHandle(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	unsigned char *retval;

	NATIVE_CALL(native_SchedulerGetDefaultDMSDeviceHandle(&retval));
	RETURN_STRING((const char*)retval, JSP_CALL_VALUE);
}

int RecordingScheduler::getDefaultDMSRecordDestinationId(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);

	unsigned char *retval;

	NATIVE_CALL(native_SchedulerGetDefaultDMSRecordDestinationId(&retval));
	RETURN_STRING((const char*)retval, JSP_CALL_VALUE);
}


HDiscInfoCollection	*RecordingScheduler::getDiscs(int count, HDiscInfoHandle *handles)
{
	ENTRY;
	HDiscInfoCollection	*collection = new HDiscInfoCollection;

	for (int i=0; i<count; i++)
	{
		collection->addItem(new HDiscInfo(handles[i]));
	}
	HLIB_STD_MemFree(handles);

	return collection;
}

#else	// JLABS
//static
int RecordingScheduler::recordAt(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

#ifdef HMX_WEBUI
	CHECK_ARGUMENT_COUNT_MIN_MAX(4, 7);
	ScheduledRecordingHandle sri = NULL;
	OOIFNumber	factory, target;

	if (argc == 4 || (argc == 5 && argv[4].type == JSP_TYPE_NULL))
		factory = SCHEDULED_RECORDING_RECORDING;
	else
		factory = NUMBER_VALUE(argv[4]);

	if(argc == 4 || argc == 5 || (argc == 6 && argv[5].type == JSP_TYPE_NULL))
		target = SCHEDULED_RECTARGET_INTHDD;
	else
		target= NUMBER_VALUE(argv[5]);

	NATIVE_CALL(native_SchedulerRecordAt(NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]),
										 NUMBER_VALUE(argv[2]), STRING_VALUE(argv[3]),
										 factory, target,NUMBER_VALUE(argv[6]),
										 OOIFUtils::getHost(this_obj), &sri));
#else
	CHECK_ARGUMENT_COUNT(4);

	ScheduledRecordingHandle sri;
	NATIVE_CALL(native_SchedulerRecordAt(NUMBER_VALUE(argv[0]), NUMBER_VALUE(argv[1]),
										 NUMBER_VALUE(argv[2]), STRING_VALUE(argv[3]),
										 OOIFUtils::getHost(this_obj), &sri));
#endif
	RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::record(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

#ifdef HMX_WEBUI
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 3);
	Programme *p = OBJECT_VALUE_OR_THROW(p, argv[0], Programme);
	ScheduledRecordingHandle sri = NULL;
	OOIFNumber	factory, target;

	if (argc == 1 || (argc == 2 && argv[1].type == JSP_TYPE_NULL))
		factory = SCHEDULED_RECORDING_RECORDING;
	else
		factory = NUMBER_VALUE(argv[1]);

	if(argc == 1 || argc == 2 || (argc == 3 && argv[2].type == JSP_TYPE_NULL))
		target = SCHEDULED_RECTARGET_INTHDD;
	else
		target= NUMBER_VALUE(argv[2]);

	NATIVE_CALL(native_SchedulerRecord(
					p->getIdentifier()
					, factory
					, target
					, OOIFUtils::getWindow(this_obj)
					, OOIFUtils::getHost(this_obj), &sri
				));
#else
	CHECK_ARGUMENT_COUNT(1);
	Programme *p = OBJECT_VALUE_OR_THROW(p, argv[0], Programme);

	ScheduledRecordingHandle sri;
	NATIVE_CALL(native_SchedulerRecord(p->getIdentifier(), OOIFUtils::getWindow(this_obj),
                                       OOIFUtils::getHost(this_obj), &sri));
#endif
	RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}

#ifdef HMX_WEBUI
int RecordingScheduler::recordCrid(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(4, 4);
	ScheduledRecordingHandle sri = NULL;

	NATIVE_CALL(native_SchedulerRecordCrid(
					  NUMBER_VALUE(argv[0])
					, STRING_VALUE(argv[1])
					, STRING_VALUE(argv[2])
					, STRING_VALUE(argv[3])
					, OOIFUtils::getWindow(this_obj)
					, OOIFUtils::getHost(this_obj), &sri
				));
	RETURN_OBJECT(this_obj, new ScheduledRecording(sri), result, JSP_CALL_VALUE);
}
#endif



#ifdef HMX_WEBUI
//static
int RecordingScheduler::update(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(5, 7);

	ScheduledRecordingHandle sri = NULL;
	RecordingHandle rec = NULL;
	OOIFNumber	factory = 0, target = 0;
	ObjectInstance *inObj = OBJECT_VALUE_OR_THROW(inObj, argv[0], ObjectInstance);
	ObjectInstance *outObj = NULL;

	switch(argc){
		case 7://obj, startTime, duration, repeat, ccid, factoryType, storage target
			factory = NUMBER_VALUE(argv[5]);
			if(argv[6].type == JSP_TYPE_NULL){
				target = SCHEDULED_RECTARGET_INTHDD;
			} else {
				target= NUMBER_VALUE(argv[6]);
			}
			break;
		case 6://obj, startTime, duration, repeat, ccid, factoryType, undefined
			if(argv[5].type == JSP_TYPE_NULL){
				factory = SCHEDULED_RECORDING_RECORDING;
			} else {
				factory = NUMBER_VALUE(argv[5]);
			}
			target = SCHEDULED_RECTARGET_INTHDD;
			break;
		case 5://obj, startTime, duration, repeat, ccid, undefined, undefined
			factory = SCHEDULED_RECORDING_RECORDING;
			target = SCHEDULED_RECTARGET_INTHDD;
			break;
		default:
			break;
	}

	switch (inObj->getType()) {
#ifdef OIPF
	case CLASS_RECORDING:
		NATIVE_CALL(native_SchedulerUpdate((static_cast<Recording*>(inObj))->getIdentifier(),
			NUMBER_VALUE(argv[1]),
			NUMBER_VALUE(argv[2]),
			NUMBER_VALUE(argv[3]),
			STRING_VALUE(argv[4]),
			factory,
			target,
			OOIFUtils::getHost(this_obj),
			&rec));
		outObj = new Recording(rec);
		break;
#endif // OIPF
	case CLASS_SCHEDULEDRECORDING:
		NATIVE_CALL(native_SchedulerUpdateScheduled(static_cast<ScheduledRecording*>(inObj)->getIdentifier(),
			NUMBER_VALUE(argv[1]),
			NUMBER_VALUE(argv[2]),
			NUMBER_VALUE(argv[3]),
			STRING_VALUE(argv[4]),
			factory,
			target,
			OOIFUtils::getHost(this_obj),
			&sri));
		outObj = new ScheduledRecording(sri);
		break;
	default:
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		break;
	}

	RETURN_OBJECT(this_obj, outObj, result, JSP_CALL_VALUE);
}

int RecordingScheduler::setDoNotDelete(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(2);
	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);

	OOIFBoolean doNotDelete = BOOLEAN_VALUE(argv[1]);

	switch (o->getType()) {
	case CLASS_RECORDING:
		NATIVE_CALL(native_RecordingSetDoNotDelete((static_cast<Recording*>(o))->getIdentifier(), doNotDelete));
		break;
	default:
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::recordConfirm(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							   int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);
	OOIFNumber	type;
	OOIFNumber	check;

	type = NUMBER_VALUE(argv[1]);
	check = NUMBER_VALUE(argv[2]);

	switch (o->getType()) {
#ifdef OIPF
	case CLASS_RECORDING:
		NATIVE_CALL(native_SchedulerConfirmScheduled((static_cast<Recording*>(o))->getIdentifier(), type, check));
		break;
#endif // OIPF
	case CLASS_SCHEDULEDRECORDING:
		NATIVE_CALL(native_SchedulerConfirmScheduled(static_cast<ScheduledRecording*>(o)->getIdentifier(), type, check));
		break;
	default:
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::copyRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 2);

	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);
	OOIFNumber targetStorage = NUMBER_VALUE(argv[1]);
	OOIFNumber ticket = -1;
#ifdef OIPF
	if(o->getType() == CLASS_RECORDING)
	{
		NATIVE_CALL(native_SchedulerCopyRecording((static_cast<Recording*>(o))->getIdentifier(), targetStorage, &ticket));
	}
#endif // OIPF
	RETURN_NUMBER(ticket, JSP_CALL_VALUE);

}


//static
int RecordingScheduler::moveRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 2);

	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);
	OOIFNumber targetStorage = NUMBER_VALUE(argv[1]);
	OOIFNumber ticket = -1;
#ifdef OIPF
	if(o->getType() == CLASS_RECORDING)
	{
		NATIVE_CALL(native_SchedulerMoveRecording((static_cast<Recording*>(o))->getIdentifier(), targetStorage, &ticket));
	}
#endif // OIPF
	RETURN_NUMBER(ticket, JSP_CALL_VALUE);

}

//static
int RecordingScheduler::removeRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 1);

	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);
	OOIFNumber ticket = -1;
#ifdef OIPF
	if(o->getType() == CLASS_RECORDING)
	{
		NATIVE_CALL(native_SchedulerRemoveRecording((static_cast<Recording*>(o))->getIdentifier(), &ticket));
	}
#endif // OIPF
	RETURN_NUMBER(ticket, JSP_CALL_VALUE);

}

//static
int RecordingScheduler::stopFileOp(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 1);

	OOIFNumber ticket = NUMBER_VALUE(argv[0]), retTicket = -1;

	NATIVE_CALL(native_SchedulerStopFileOp(ticket, &retTicket));

	RETURN_NUMBER(retTicket, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::setSeriesLock(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT_MIN_MAX(2, 2);

	const char *seriesID = STRING_VALUE(argv[0]);
	OOIFBoolean lockState = BOOLEAN_VALUE(argv[1]);

	NATIVE_CALL(native_SchedulerSetManualLock(seriesID, lockState));

	return JSP_CALL_NO_VALUE;
}

#include "humax/RecordingFileOpEvent.h"
//static
void RecordingScheduler::onRecordingFileOperationEvent(int ticket, int result)
{
	// All known RecordingSchedule objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;
		rs->dispatchEvent(new RecordingFileOpEvent(ticket, result, rs->getHost()));
	}
}

#endif //HMX_WEBUI

#endif // JLABS

//static
int RecordingScheduler::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

#ifdef HMX_WEBUI
	CHECK_ARGUMENT_COUNT_MIN_MAX(1, 2);

	OOIFBoolean bRemoveFirstItem = FALSE;
	if(argc == 2 && argv[1].type != JSP_TYPE_NULL)
		bRemoveFirstItem = BOOLEAN_VALUE(argv[1]);
#else
	CHECK_ARGUMENT_COUNT(1);
#endif

	ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);

	switch (o->getType()) {
#ifdef OIPF
	case CLASS_RECORDING:
		NATIVE_CALL(native_SchedulerRemove((static_cast<Recording*>(o))->getIdentifier()));
		return JSP_CALL_NO_VALUE;

#endif // OIPF
	case CLASS_SCHEDULEDRECORDING:
#ifdef HMX_WEBUI
		// TODO: add native_SchedulerRemoveWholeSeries
		if(bRemoveFirstItem)
			NATIVE_CALL(native_SchedulerRemoveFirstScheduled(static_cast<ScheduledRecording*>(o)->getIdentifier()));
		else
#endif
			NATIVE_CALL(native_SchedulerRemoveScheduled(static_cast<ScheduledRecording*>(o)->getIdentifier()));
		return JSP_CALL_NO_VALUE;

#ifdef JLABS
	case CLASS_JLABS_CDSRECORDING:
		char *retval;
		NATIVE_CALL(native_SchedulerRemoveCDSRecording(static_cast<jlabsCDSRecording*>(o)->getIdentifier(), &retval));
		RETURN_STRING((const char*)retval, JSP_CALL_VALUE);
#endif
	default:
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::removeSeries(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                               int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
		CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

		CHECK_ARGUMENT_COUNT(1);

		ObjectInstance *o = OBJECT_VALUE_OR_THROW(o, argv[0], ObjectInstance);

		switch (o->getType()) {
		case CLASS_SCHEDULEDRECORDING:
				NATIVE_CALL(native_SchedulerRemoveWholeSeries(static_cast<ScheduledRecording*>(o)->getIdentifier()));
			return JSP_CALL_NO_VALUE;

		default:
			THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
		}

		return JSP_CALL_NO_VALUE;
}

//static
#ifdef JLABS
int RecordingScheduler::getScheduledRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	OOIFBoolean isRecorded = FALSE;
	ScheduledRecordingHandle *handles = NULL;
	int recordingCount;
	NATIVE_CALL(native_SchedulerGetScheduledRecordings(OOIFUtils::getHost(this_obj),
	                                                   &recordingCount, &handles));
	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();
	for (int i=0; i<recordingCount; i++) {
		native_ScheduledRecordingIsRecorded(handles[i], &isRecorded);
		if (isRecorded)
		{
			collection->addItem(new Recording(handles[i]));
		}
		else
		{
			collection->addItem(new ScheduledRecording(handles[i]));
		}
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}
#else
int RecordingScheduler::getScheduledRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	ScheduledRecordingHandle *handles = NULL;
	int recordingCount;
	NATIVE_CALL(native_SchedulerGetScheduledRecordings(OOIFUtils::getHost(this_obj),
	                                                   &recordingCount, &handles));
	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();
	for (int i=0; i<recordingCount; i++) {
		collection->addItem(new ScheduledRecording(handles[i]));
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}
#endif

int RecordingScheduler::getCurrentRecordings(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	RecordingHandle *handles = NULL;
	int count	= 0;
	NATIVE_CALL(native_getCurrentRecordings(&count, &handles));
	RecordingCollection *collection = new RecordingCollection();

	for (int i=0; i<count; i++)
	{
		collection->addItem(new Recording(handles[i]));
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	handles = NULL;
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}


//static
int RecordingScheduler::getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc,
        jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);
	RETURN_OBJECT(this_obj, new ChannelConfig(RECORDING), result, JSP_CALL_VALUE);
}

//
int RecordingScheduler::createProgrammeObject(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
        int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	ProgrammeHandle id;
	NATIVE_CALL(native_ProgrammeCreate(&id, OOIFUtils::getWindow(this_obj),
	                                   OOIFUtils::getHost(this_obj)));
	RETURN_OBJECT(this_obj, new Programme(id), result, JSP_CALL_VALUE);
}

#ifdef OIPF
//static
ScheduledRecordingCollection *RecordingScheduler::getAllRecordings(int recordingsCount, ScheduledRecordingHandle *handles)
{
    ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();
    OOIFBoolean isRecorded = FALSE;

    for (int i=0; i<recordingsCount; i++)
    {
        native_ScheduledRecordingIsRecorded(handles[i], &isRecorded);
        if (isRecorded)
        {
            collection->addItem(new Recording(handles[i]));
        }
        else
        {
            collection->addItem(new ScheduledRecording(handles[i]));
        }
    }
    HLIB_STD_MemFree(handles);
    return collection;
}
#else /* OIPF */
//static
RecordingCollection *RecordingScheduler::getAllRecordings(int recordingsCount, RecordingHandle *handles)
{
	RecordingCollection *collection = new RecordingCollection();
	for (int i=0; i<recordingsCount; i++) {
		collection->addItem(new Recording(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}
#endif /* OIPF */

#ifdef OIPF
OOIFReturnCode RecordingScheduler::checkListenerAccessAllowed(const char *eventName)
{
	return native_SchedulerOnListenerAccessed(eventName);
}

//static
int RecordingScheduler::stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	Recording *sr = OBJECT_VALUE_OR_THROW(sr, argv[0], Recording);
	NATIVE_CALL(native_SchedulerStop(sr->getIdentifier()));
	return JSP_CALL_NO_VALUE;
}

//static
int RecordingScheduler::getRecording(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
							 int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_STRING);
	const char *recording_id = STRING_VALUE(argv[0]);

	RecordingHandle retval;
	NATIVE_CALL(native_SchedulerGetRecording(recording_id, &retval));
	RETURN_OBJECT(this_obj, new Recording(retval), result, JSP_CALL_VALUE);
}

//static
int RecordingScheduler::refresh(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
								int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL(native_SchedulerRefresh());
	return JSP_CALL_NO_VALUE;
}

//static
void RecordingScheduler::onRecordingEvent(RecordingHandle handle, OOIFNumber state)
{
	// All known RecordingSchedule objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;
		rs->dispatchEvent(new PVREvent(handle, state, rs->getHost()));
	}
}

#ifdef	HMX_WEBUI
#include "humax/PVREvent.h"
//static
void RecordingScheduler::onScheduleEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error, ScheduledRecordingHandle *conflicts, int count, ScheduledRecordingHandle *alternatives, int alt_count, ProgrammeHandle *altevents, int alt_evt_count)
{
	// All known RecordingSchedule objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;

		rs->dispatchEvent(new humax::PVREvent(state, handle, error, conflicts, count, alternatives, alt_count, altevents, alt_evt_count, rs->getHost()));
	}
}

void RecordingScheduler::onRecordEvent(OOIFNumber state, ScheduledRecordingHandle handle, OOIFNumber error)
{
	// All known RecordingSchedule objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;

		rs->dispatchEvent(new humax::PVREvent(state, handle, error, NULL, 0, NULL,0, NULL,0, rs->getHost()));
	}
}



int RecordingScheduler::startSatRecord(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	OOIF_LOG_DEBUG(" Start Sat Record Proc!!! ");

	CHECK_ARGUMENT_COUNT(0);
	NATIVE_CALL(native_RecordingStartSatRecord());
	return JSP_CALL_NO_VALUE;
}

#endif // HMX_WEBUI


#endif // OIPF

#ifdef JLABS
//static
void RecordingScheduler::onHNReservationFinishedEvent(ScheduledRecordingHandle recording, OOIFNumber errorCode)
{
	// All known CodManager objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;

		rs->dispatchEvent(new HNReservationFinished(recording, errorCode, rs->getHost()));
	}

}

void RecordingScheduler::onHNRemoveResultEvent(char *strRemove, OOIFNumber errorCode)
{
	// All known CodManager objects should dispatch this event
	for (list<RecordingScheduler*>::iterator it=recordingSchedulers.begin(); it!=recordingSchedulers.end(); it++) {
		RecordingScheduler *rs = *it;

		rs->dispatchEvent(new HNRemoveResult(strRemove, errorCode, rs->getHost()));
	}

}

#endif // JLABS

#if defined(HMX_WEBUI)
int RecordingScheduler::getCountScheduleList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	int scheduleCount;
	NATIVE_CALL(native_SchedulerGetCountScheduleList(&scheduleCount));
	RETURN_NUMBER(scheduleCount, JSP_CALL_VALUE);
}

int RecordingScheduler::getCountRecordingList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	int recordingCount;
	NATIVE_CALL(native_SchedulerGetCountRecordingList(&recordingCount));
	RETURN_NUMBER(recordingCount, JSP_CALL_VALUE);
}

int RecordingScheduler::getScheduleList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	ScheduledRecordingHandle *handles = NULL;
	int scheduleCount;
	NATIVE_CALL(native_SchedulerGetScheduleList(&scheduleCount, &handles));
	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();
	for (int i=0; i<scheduleCount; i++) {
		collection->addItem(new ScheduledRecording(handles[i]));
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

int RecordingScheduler::getRecordingList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
		int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	ScheduledRecordingHandle *handles = NULL;
	int recordingCount;
	NATIVE_CALL(native_SchedulerGetRecordingList(&recordingCount, &handles));
	ScheduledRecordingCollection *collection = new ScheduledRecordingCollection();
	for (int i=0; i<recordingCount; i++) {
		collection->addItem(new Recording(handles[i]));
	}
	if( handles )
		HLIB_STD_MemFree(handles);
	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

#endif


#endif // HBBTV_PVR || OIPF

/* EOF */

