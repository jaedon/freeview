// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "Download.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"
#include "NativeDownload.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#include "DRMControlInformation.h"
#include "ParentalRating.h"

#include "macros.h"

using namespace Ooif;

void Download::init()
{
	classType = CLASS_DOWNLOAD;
}

Download::Download(DownloadHandle i) : WrapperObject<Download>(i)
{
	ENTRY;
	init();
}

Download::Download(const Download &original) : WrapperObject<Download>(original.identifier)
{
	ENTRY;
	init();
}

Download::~Download()
{
	ENTRY;
}

int Download::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	NATIVE_GET_NUMBER("totalSize", native_DownloadGetTotalSize(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("state", native_DownloadGetState(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("amountDownloaded", native_DownloadGetAmountDownloaded(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	NATIVE_GET_NUMBER("reason",  native_DownloadGetReason(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("currentBitrate",  native_DownloadGetCurrentBitrate(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#endif // OIPF
	NATIVE_GET_STRING("name", native_DownloadGetName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("id", native_DownloadGetId(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("contentURL", native_DownloadGetContentURL(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("description", native_DownloadGetDescription(getIdentifier(), &RETVAL), JSP_GET_VALUE);

	NATIVE_GET_VOIDP_COLLECTION("parentalRatings", obj,
	                            native_DownloadGetParentalRatings(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                            getParentalRatings(SIZE, NATIVE_ARRAY),
	                            JSP_GET_VALUE);
	NATIVE_GET_VOIDP_COLLECTION("drmControl", obj,
	                            native_DownloadGetDrmControl(getIdentifier(), &SIZE, &NATIVE_ARRAY),
	                            getDRMControl(SIZE, NATIVE_ARRAY),
	                            JSP_GET_VALUE);

	if (!strcmp(name, "startTime"))
	{
		return returnStartTime(obj, result);
	}

	NATIVE_GET_NUMBER("timeElapsed", native_DownloadGetTimeElapsed(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("timeRemaining", native_DownloadGetTimeRemaining(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("transferType", native_DownloadGetTransferType(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("originSite", native_DownloadGetOriginSite(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("originSiteName", native_DownloadGetOriginSiteName(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("contentID", native_DownloadGetContentID(getIdentifier(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("iconURL", native_DownloadGetIconURL(getIdentifier(), &RETVAL), JSP_GET_VALUE);
#ifdef OIPF
	GET_NUMBER("DOWNLOAD_COMPLETED", DOWNLOAD_COMPLETED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("DOWNLOAD_IN_PROGRESS", DOWNLOAD_IN_PROGRESS, JSP_GET_VALUE_CACHE);
	GET_NUMBER("DOWNLOAD_PAUSED", DOWNLOAD_PAUSED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("DOWNLOAD_FAILED", DOWNLOAD_FAILED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("DOWNLOAD_NOT_STARTED", DOWNLOAD_NOT_STARTED, JSP_GET_VALUE_CACHE);
	GET_NUMBER("DOWNLOAD_STALLED", DOWNLOAD_STALLED, JSP_GET_VALUE_CACHE);
#endif // OIPF

	return JSP_GET_NOTFOUND;
}

int Download::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
#ifdef OIPF
	NATIVE_SET("description", native_DownloadSetDescription(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("name", native_DownloadSetName(identifier, STRING_VALUE(value[0])));
	NATIVE_SET("contentID", native_DownloadSetContentID(identifier, STRING_VALUE(value[0])));
#endif // OIPF

	return JSP_PUT_NOTFOUND;
}

ParentalRatingCollection *Download::getParentalRatings(int count,
        ParentalRatingHandle *handles)
{
	ParentalRatingCollection *collection = new ParentalRatingCollection(getIdentifier());
	for (int i=0; i<count; i++)
	{
		collection->addItem(new ParentalRating(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

DRMControlInfoCollection *Download::getDRMControl(int count,
        DRMControlInformationHandle *handles)
{
	DRMControlInfoCollection *collection = new DRMControlInfoCollection;
	for (int i=0; i<count; i++)
	{
		collection->addItem(new DRMControlInformation(handles[i]));
	}
	HLIB_STD_MemFree(handles);
	return collection;
}

int Download::returnStartTime(jsplugin_obj *obj, jsplugin_value *result)
{
	const char *time;
	NATIVE_GET(native_DownloadGetStartTime(getIdentifier(), &time));
	const char *before = "new Date('";
	const char *after = "')";
	int len = strlen(before)+strlen(time)+strlen(after) + 1;
	char *final = OOIFUtils::getTmpBuffer(strlen(before)+strlen(time)+strlen(after) + 1);
	if (!final)
		return JSP_GET_NOMEM;
	snprintf(final, len, "%s%s%s", before, time, after);

	result->u.string = final;
	result->type = JSP_TYPE_EXPRESSION;
	return JSP_GET_VALUE;
}

void Download::releaseNative(DownloadHandle id)
{
	NATIVE_WARN_NOK(native_DownloadReleaseHandle(id));
}

#endif // HBBTV_DOWNLOAD || OIPF
