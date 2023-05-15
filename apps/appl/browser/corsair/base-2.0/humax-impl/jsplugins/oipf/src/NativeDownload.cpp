// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#include "NativeDownload.h"
#include "oplpcmanager.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

downloadListener *global_downloadListener = 0;

OOIFReturnCode native_DownloadSetListener(downloadListener listener)
{
	global_downloadListener = listener;
	return OOIF_RETURN_OK;
}

/*------------------------------------------
  DownloadTrigger
  ------------------------------------------*/
OOIFReturnCode native_DownloadTriggerOnCreate(long int window, const char *host)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadRegisterDownload(const char *cad, const char *time, long int window, const char *host, const char **retval)
{
	OOIF_LOG_DEBUG("registerDownload called with cad: %s, time: %s, window: %ld, host: %s", cad, time, window, host);

	*retval = "CAD Download Identifier";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadRegisterDownloadURL(const char *url, const char *contentType, const char *downloadStart, long int window, const char *host, const char **retval)
{
	OOIF_LOG_DEBUG("registerDownloadURL called with url: %s, contentType: %s, "
	               "downloadStart: %s, window: %ld, host: %s",
	               url, contentType, downloadStart, window, host);
	*retval = "URL Download Identifier";
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DownloadRegisterDownloadFromCRID(const char *CRID, const char *IMI, const char *downloadStart, long int window, const char *host, const char **retval)
{
	OOIF_LOG_DEBUG("registerDownloadFromCRID called with CRID: %s, IMI: %s, downloadStart: %s, window: %ld, host: %s",
	               CRID ? CRID : "NULL",
	               IMI ? IMI : "NULL",
	               downloadStart ? downloadStart : "NULL",
	               window, host);
	*retval = "CRID Download Identifier";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadCheckDownloadPossible(OOIFNumber sizeInBytes, OOIFNumber *retval)
{
	OOIF_LOG_DEBUG("checkDownloadPossible called with sizeInBytes: %f", (float)sizeInBytes);
	*retval = 1;
	return OOIF_RETURN_OK;
}
#endif // OIPF

/*------------------------------------------
  Download Manager
  ------------------------------------------*/

OOIFReturnCode native_DownloadManagerOnCreate(long int window, const char *host)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadManagerOnListenerAccessed(long int window, const char *host, const char *eventName)
{
	return OOIF_RETURN_OK;
}

void sendDownloadStateChangedEvent(DownloadHandle identifier, OOIFNumber state, OOIFBoolean send_reason, OOIFNumber reason = 0)
{
	if (global_downloadListener)
	{
		DownloadEventInfo info;
		info.download_state_change_data.item = identifier;
		info.download_state_change_data.state = state;
		info.download_state_change_data.has_reason = send_reason;
		if (send_reason)
			info.download_state_change_data.reason = reason;
		global_downloadListener(DOWNLOAD_EVENT_DOWNLOAD_STATE_CHANGE, &info);
	}
}

OOIFReturnCode native_DownloadPause(DownloadHandle identifier, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Pausing download: %ld", reinterpret_cast<long>(identifier));
	sendDownloadStateChangedEvent(identifier, 4, false);
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadResume(DownloadHandle identifier, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Resuming download: %ld", reinterpret_cast<long>(identifier));
	sendDownloadStateChangedEvent(identifier, 2, false);
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadRemove(DownloadHandle identifier, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("Removing download: %ld", reinterpret_cast<long>(identifier));
	sendDownloadStateChangedEvent(identifier, 8, true, 4);
	*retval = 1;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetDownloads(const char* id, long int window, const char *host, int *count, DownloadHandle **handles)
{
	OOIF_LOG_DEBUG("getDownloads called with id: %s, window: %ld, host: %s",
	               id ? id : "NULL", window, host);
	if (!id || !strcmp(id, "Dummy Identifier") )
	{
		DownloadHandle *myHandles = (DownloadHandle*)HLIB_STD_MemAlloc(sizeof(DownloadHandle)*4);
		myHandles[0] = (void*)11;
		myHandles[1] = (void*)22;
		myHandles[2] = (void*)33;
		myHandles[3] = (void*)44;
		*handles = myHandles;
		*count = 4;
	}
	else
		*count = 0;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadCreateFilteredList(OOIFBoolean has_currentDomain, OOIFBoolean currentDomain, OOIFNumber states, long int window, const char *host, int *count, DownloadHandle **handles)
{
	OOIF_LOG_DEBUG("createFilteredList called with has_currentDomain: %d, currentDomain: %d, "
	               "states: %f, window: %ld, host: %s",
	               has_currentDomain, currentDomain, states, window, host);

	if (!has_currentDomain)
		*count = 0;
	else
	{
		DownloadHandle *myHandles = (DownloadHandle*)HLIB_STD_MemAlloc(sizeof(DownloadHandle)*3);
		myHandles[0] = (void*)11;
		myHandles[1] = (void*)22;
		myHandles[2] = (void*)33;
		*handles = myHandles;
		*count = 3;
	}
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DownloadUpdateRegisteredDownload(DownloadHandle id, const char *newURL, long int window, const char *host, OOIFBoolean *retval)
{
	OOIF_LOG_DEBUG("updateRegisteredDownload called with id: %ld, newURL: %s, window: %ld, host: %s",
	               reinterpret_cast<long>(id), newURL ? newURL : "NULL", window, host);
	*retval = 1;
	return OOIF_RETURN_OK;
}
#endif // OIPF

/*------------------------------------------
  Download
  ------------------------------------------*/

OOIFReturnCode native_DownloadGetTotalSize(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = static_cast<OOIFNumber>(reinterpret_cast<long>(identifier) * 2);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetState(DownloadHandle identifier, OOIFNumber *retval)
{
#ifdef OIPF
	*retval = (int)(DOWNLOAD_COMPLETED);
#else
	*retval = 0;
#endif // OIPF
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DownloadGetReason(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = (int)(DOWNLOAD_REASON_STORAGE_FULL);
	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_DownloadGetAmountDownloaded(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = static_cast<OOIFNumber>(reinterpret_cast<long>(identifier));
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DownloadGetCurrentBitrate(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = static_cast<OOIFNumber>(reinterpret_cast<long>(identifier));
	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_DownloadGetName(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy name";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetId(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Identifier";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetContentURL(DownloadHandle identifier, const char **retval)
{
	*retval = "http://content.url";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetDescription(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Download Description";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetParentalRatings(DownloadHandle identifier, int *count, ParentalRatingHandle **handles)
{
	// This implementation is temporary work around, fix crash in JLAB DAE Test Application.
	ParentalRatingHandle *myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle));
	int						value = 0;
	OPL_ParentalRating_t	*parentalRating;

	parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, value, 0, NULL);
	myHandles[0] = (ParentalRatingHandle)parentalRating;

	*handles = myHandles;
	*count = 1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetDrmControl(DownloadHandle identifier, int *count, DRMControlInformationHandle **handles)
{
	DRMControlInformationHandle *myHandles = (DRMControlInformationHandle*)HLIB_STD_MemAlloc(sizeof(DRMControlInformationHandle)*2);
	myHandles[0] = (void*)11;
	myHandles[1] = (void*)22;
	*handles = myHandles;
	*count = 2;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetStartTime(DownloadHandle identifier, const char **retval)
{
	*retval = "Tue, 25 Aug 2009 08:20:00 GMT";
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_DownloadGetTimeElapsed(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = 1234;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetTimeRemaining(DownloadHandle identifier, OOIFNumber *retval)
{
	*retval = 44;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetTransferType(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Transfer Type";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetOriginSite(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Origin Site";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetOriginSiteName(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Origin Site Name";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetContentID(DownloadHandle identifier, const char **retval)
{
	*retval = "Dummy Content ID";
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadGetIconURL(DownloadHandle identifier, const char **retval)
{
	*retval = "http://icon.uri";
	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_DownloadSetDescription(DownloadHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting Download.description to %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadSetName(DownloadHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting Download.name to %s", value);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_DownloadSetContentID(DownloadHandle identifier, const char *value)
{
	OOIF_LOG_DEBUG("Setting Download.contentID to %s", value);
	return OOIF_RETURN_OK;
}
#endif // OIPF

OOIFReturnCode native_DownloadReleaseHandle(DownloadHandle id)
{
	return OOIF_RETURN_OK;
}
#endif // HBBTV_DOWNLOAD || OIPF
