// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "DownloadManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeDownload.h"
#include "Download.h"
#include "Collection.h"
#include "DownloadStateChangeEvent.h"
#include "DiscInfo.h"
#include "Exception.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "macros.h"
#include "OOIFUtils.h"

using namespace Ooif;


//static
list<DownloadManager*> DownloadManager::instances;

DownloadManager::DownloadManager()
{
	ENTRY;
	classType = CLASS_DOWNLOADMANAGER;
	DownloadManager::instances.push_back(this);
}

DownloadManager::~DownloadManager()
{
	DownloadManager::instances.remove(this);
}

int DownloadManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY

	INTRINSIC_EVENT_GET(DownloadStateChange);
#ifdef OIPF
	GET_OBJECT("discInfo", obj, new DiscInfo(), JSP_GET_VALUE);
#endif // OIPF
	GET_FUNCTION(pause, obj, "o", JSP_GET_VALUE);
	GET_FUNCTION(resume, obj, "o", JSP_GET_VALUE);
	GET_FUNCTION(remove, obj, "o", JSP_GET_VALUE);
	GET_FUNCTION(getDownloads, obj, "-", JSP_GET_VALUE);
	GET_FUNCTION(createFilteredList, obj, "-n", JSP_GET_VALUE);
#ifdef OIPF
	GET_FUNCTION(updateRegisteredDownload, obj, "o-", JSP_GET_VALUE);
#endif // OIPF

	return EventTarget::getter(obj, name, result);
}

int DownloadManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(DownloadStateChange);

	return JSP_PUT_NOTFOUND;
}

//static
int DownloadManager::pause(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	Download *d = OBJECT_VALUE_OR_THROW(d, argv[0], Download);
	OOIFBoolean retval;
	NATIVE_CALL(native_DownloadPause(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int DownloadManager::resume(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	Download *d = OBJECT_VALUE_OR_THROW(d, argv[0], Download);
	OOIFBoolean retval;
	NATIVE_CALL(native_DownloadResume(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int DownloadManager::remove(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);

	Download *d = OBJECT_VALUE_OR_THROW(d, argv[0], Download);
	OOIFBoolean retval;
	NATIVE_CALL(native_DownloadRemove(d->getIdentifier(), &retval));
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int DownloadManager::getDownloads(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                  int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT_MIN_MAX(0, 1);

	const char *id;
	if (argc > 0 && TYPE_STRING(argv[0]))
		id = STRING_VALUE(argv[0]);
	else
		id = NULL;

	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	DownloadCollection *collection = new DownloadCollection();
	// Now query native download layer to get a list of downloads
	DownloadHandle *handles = NULL;
	int downloadCount;
	NATIVE_CALL(native_DownloadGetDownloads(id, window, host, &downloadCount, &handles));
	for (int i=0; i<downloadCount; i++)
	{
		collection->addItem(new Download(handles[i]));
	}
	HLIB_STD_MemFree(handles);

	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

//static
int DownloadManager::createFilteredList(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                        int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	CHECK_ARGUMENT_COUNT(2);

	OOIFBoolean has_currentDomain;
	OOIFBoolean currentDomain;
	if (TYPE_BOOLEAN(argv[0]))
	{
		has_currentDomain = TRUE;
		currentDomain = BOOLEAN_VALUE(argv[0]);
	}
	else
	{
		has_currentDomain = FALSE;
		currentDomain = 0;
	}

	OOIFNumber states = NUMBER_VALUE(argv[1]);
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	DownloadCollection *collection = new DownloadCollection();
	DownloadHandle *handles = NULL;
	int downloadCount;
	NATIVE_CALL(native_DownloadCreateFilteredList(has_currentDomain, currentDomain, states,
	            window, host, &downloadCount, &handles));
	for (int i=0; i<downloadCount; i++)
	{
		collection->addItem(new Download(handles[i]));
	}
	HLIB_STD_MemFree(handles);

	RETURN_OBJECT(this_obj, collection, result, JSP_CALL_VALUE);
}

#ifdef OIPF
//static
int DownloadManager::updateRegisteredDownload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(2);

	Download *d = OBJECT_VALUE_OR_THROW(d, argv[0], Download);

	const char *newURL;
	if (TYPE_STRING(argv[1]))
	{
		newURL = STRING_VALUE(argv[1]);
	}
	else
	{
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
	}

	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	OOIFBoolean retval = FALSE;
	NATIVE_CALL(native_DownloadUpdateRegisteredDownload(d->getIdentifier(), newURL, window, host, &retval));

	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}
#endif // OIPF

//static
void DownloadManager::onDownloadEvent(DownloadEvent event, DownloadEventInfo* info)
{
	// Iterate over all instances and dispatch the event
	for (list<DownloadManager*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		Event *e = NULL;
		jsplugin_obj *context = (*it)->getHost();
		switch (event)
		{
		case DOWNLOAD_EVENT_DOWNLOAD_STATE_CHANGE:
		{
			OOIF_LOG_DEBUG("DownloadStateChange event received");
			DownloadHandle handle = info->download_state_change_data.item;
			OOIFNumber state = info->download_state_change_data.state;
			OOIFBoolean has_reason = info->download_state_change_data.has_reason;
			OOIFNumber reason = info->download_state_change_data.reason;
			e = new DownloadStateChangeEvent(context, handle, state, has_reason, reason);
			break;
		}
		default:
			OOIF_LOG_WARNING("Unknown event received");
			return;
		}
		if (e)
			(*it)->dispatchEvent(e);
	}
}

OOIFReturnCode DownloadManager::checkListenerAccessAllowed(const char *eventName)
{
#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	jsplugin_obj *this_obj = getHost();
	char *host = OOIFUtils::getHost(this_obj);
	long int window = OOIFUtils::getWindow(this_obj);
	OOIFReturnCode ret = native_DownloadManagerOnListenerAccessed(window, host, eventName);
	return ret;
}
#endif // HBBTV_DOWNLOAD || OIPF
