/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)

#ifndef __NATIVE_DOWNLOAD
#define __NATIVE_DOWNLOAD

#include "NativeDrmAgent.h"
#include "NativeParentalControlManager.h"
#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup Download Content Download
  @{

   This is the native interface for the content download APIs as
   defined in DAE 7.4.
*/

/**
   Opaque reference to a Download object instance in the native
   platform as defined in DAE 7.4.4.
 */
typedef void *DownloadHandle;


/**
	Download event types.
*/
typedef enum {
	/** This event corresponds to the onDownloadStateChange event defined in DAE 7.4.3 */
	DOWNLOAD_EVENT_DOWNLOAD_STATE_CHANGE = 0
} DownloadEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#DownloadEvent.
*/
typedef union {
	/** used for DOWNLOAD_EVENT_DOWNLOAD_STATE_CHANGED as described in DAE 7.4.3.2 */
	struct {
		DownloadHandle item;
		OOIFNumber state;
		OOIFBoolean has_reason;
		OOIFNumber reason;
	} download_state_change_data;
} DownloadEventInfo;

/**
   The callback function that the native platform should call to
   dispatch a event. The OOIF plugin will propagate the event
   notification to the registered listeners on all existing
   DownloadManagers.
   \param event The event type
   \param info Event context info
   \see native_DownloadSetListener
*/
typedef void (downloadListener)(DownloadEvent event, DownloadEventInfo* info);

/**
   Registers new listener on the DownloadStateChange event.
   \param listener The listener
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_DownloadSetListener(downloadListener listener);

/*------------------------------------------
  DownloadTrigger
  ------------------------------------------*/
/** @name DownloadTrigger
	The  Download Trigger embedded object as defined in DAE 7.4.1.
	@{*/

/**
   Called when a new DownloadTrigger instance is created. The return
   value may be used for capability and security checks.
   \param window The identifier of the window the create call originates from
   \param host The host URI of the application the create call originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_DownloadTriggerOnCreate(long int window, const char *host);

/**
   This function corresponds to the OIPF function registerDownload() as
   defined in DAE 7.4.1.1.
   \param cad As defined in DAE 7.4.1.1
   \param time As defined in DAE 7.4.1.1
   \param window The identifier of the window the call originates from
   \param host The host URL of the current context
   \param retval Unique identifier as described in DAE 7.4.1.1
   \return OOIF return code
*/
OOIFReturnCode native_DownloadRegisterDownload(const char *cad, const char *time, long int window, const char *host, const char **retval);

/**
   This function corresponds to the OIPF function registerDownloadURL() as
   defined in DAE 7.4.1.1.

   \param url As defined in DAE 7.4.1.1
   \param contentType As defined in DAE 7.4.1.1
   \param downloadStart As defined in DAE 7.4.1.1
   \param window The identifier of the window the call originates from
   \param host The host URL of the current context
   \param retval Unique identifier as described in DAE 7.4.1.1
   \return OOIF return code
*/
OOIFReturnCode native_DownloadRegisterDownloadURL(const char *url, const char *contentType, const char *downloadStart, long int window, const char *host, const char **retval);

#ifdef OIPF
/**
   This function corresponds to the OIPF function checkDownloadPossible() as
   defined in DAE 7.4.1.1.
   \param sizeInBytes As defined in DAE 7.4.1.1
   \param retval Retval as defined in DAE 7.4.1.1
   \return OOIF return code
*/
OOIFReturnCode native_DownloadCheckDownloadPossible(OOIFNumber sizeInBytes, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function registerDownloadFromCRID() as
   defined in DAE 7.4.2.
   \param CRID As defined in DAE 7.4.1.2
   \param IMI As defined in DAE 7.4.1.2
   \param downloadStart As defined in DAE 7.4.1.2
   \param window The identifier of the window the call originates from
   \param host The host URL of the current context
   \param retval Retval as defined in DAE 7.4.1.2
   \return OOIF return code
*/
OOIFReturnCode native_DownloadRegisterDownloadFromCRID(const char *CRID, const char *IMI, const char *downloadStart, long int window, const char *host, const char **retval);
#endif /* OIPF */

/*@}*/
/*------------------------------------------
  Download Manager
  ------------------------------------------*/
/** @name DownloadManager
	The Download Manager embedded object as defined in DAE 7.4.3.
	@{*/

/**
   Called when a new DownloadManager instance is created. The return
   value may be used for capability and security checks.
   \param window The identifier of the window the create call originates from
   \param host The host URI of the application the create call originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_DownloadManagerOnCreate(long int window, const char *host);

/**
 * Called when an event listener is added, removed, or accessed from Javascript
 * on the DownloadManager Object.  OOIF will handle it automatically, so no
 * action is needed from the integration layer. Nonetheless, this callback
 * gives the oportunity to prevent the listener from being added, removed, or
 * accessed, by returning an error code. This may be used to implement security
 * checks, as defined in section HbbTV 11.1, or capability checks as defined in
 * DAE 7 and DAE 9.3
   \param window The identifier of the window the call originates from
   \param host The host URL (domain) of the current context
 * \param eventName the name of the event
 * \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_DownloadManagerOnListenerAccessed(long int window, const char *host, const char *eventName);
/**
   This function corresponds to the OIPF function pause() as
   defined in DAE 7.4.3.3.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.3.3
   \return OOIF return code
*/
OOIFReturnCode native_DownloadPause(DownloadHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function resume() as
   defined in DAE 7.4.3.3.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.3.3
   \return OOIF return code
*/
OOIFReturnCode native_DownloadResume(DownloadHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function remove() as
   defined in DAE 7.4.3.3.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.3.3
   \return OOIF return code
*/
OOIFReturnCode native_DownloadRemove(DownloadHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function getDownloads() as
   defined in DAE 7.4.3.3.
   \param id As defined in DAE 7.4.3.3
   \param window The identifier of the window the call originates from
   \param host The host URL (domain) of the current context
   \param count Number of handles in the returned array
   \param handles Return the array of handles to the downloads in the
   list here. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetDownloads(const char* id, long int window, const char *host, int *count, DownloadHandle **handles);

/**
   This function corresponds to the OIPF function createFilteredList() as
   defined in DAE 7.4.3.3.
   \param has_currentDomain is TRUE if currentDomain is defined, and FALSE
   if undefined
   \param currentDomain As defined in DAE 7.4.3.3.
   \param states As defined in DAE 7.4.3.3
   \param window The identifier of the window the call originates from
   \param host The host URL of the current context
   \param count Number of handles in the returned array
   \param handles Return the array of handles to the downloads in the
   list here. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_DownloadCreateFilteredList(OOIFBoolean has_currentDomain, OOIFBoolean currentDomain, OOIFNumber states, long int window, const char *host, int *count, DownloadHandle **handles);

#ifdef OIPF
/**
   This function corresponds to the OIPF function updateRegisteredDownload() as
   defined in DAE 7.4.3.3.
   \param id Download instance identifier
   \param newURL As defined in DAE 7.4.3.3.
   \param window The identifier of the window the call originates from
   \param host The host URL of the current context
   \param retval Retval as defined in DAE 7.4.3.3
   \return OOIF return code
*/
OOIFReturnCode native_DownloadUpdateRegisteredDownload(DownloadHandle id, const char *newURL, long int window, const char *host, OOIFBoolean *retval);
#endif /* OIPF */

/**
   Called when the Download instance is destroyed in the OOIF plugin.
   \param id Download instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_DownloadReleaseHandle(DownloadHandle id);

/*@}*/
/*------------------------------------------
  Download
  ------------------------------------------*/
/** @name Download
	The Download class as defined in DAE 7.4.4.
	@{*/

#ifdef OIPF
/**
	Download status constants as defined in DAE 7.4.4.
*/
typedef enum {
	/** The download has completed. */
	DOWNLOAD_COMPLETED = 1,
	/** The download is in progress. */
	DOWNLOAD_IN_PROGRESS = 2,
	/** The download has been paused (either by an application or automatically by the platform). */
	DOWNLOAD_PAUSED = 4,
	/** The download has failed. */
	DOWNLOAD_FAILED = 8,
	/** The download is queued but has not yet started. */
	DOWNLOAD_NOT_STARTED = 16,
	/** The download has stalled due to a transient failure and the Download Manager is attempting to recuperate and re-establish the download. */
	DOWNLOAD_STALLED = 32
} DownloadState;

/**
	Download failed reason constants as defined in DAE 7.4.4.
*/
typedef enum {
	/** The local storage is full. */
	DOWNLOAD_REASON_STORAGE_FULL = 0,
	/** The item cannot be downloaded (e.g. because it has not been purchased). */
	DOWNLOAD_REASON_CANNOT_DOWNLOAD = 1,
	/** The item is no longer available for download. */
	DOWNLOAD_REASON_NOT_AVAILABLE = 2,
	/** The item is invalid due to bad checksum or length. */
	DOWNLOAD_REASON_BAD_CHECKSUM = 3,
	/** Other reason. */
	DOWNLOAD_REASON_OTHER = 4
} DownloadFailedReason;
#endif /* OIPF */

/**
   Gets OIPF property totalSize as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetTotalSize(DownloadHandle id, OOIFNumber *retval);

/**
   Gets OIPF property state as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetState(DownloadHandle id, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets OIPF property reason as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetReason(DownloadHandle id, OOIFNumber *retval);
#endif /* OIPF */

/**
   Gets OIPF property amountDownloaded as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetAmountDownloaded(DownloadHandle id, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets OIPF property currentBitrate as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetCurrentBitrate(DownloadHandle id, OOIFNumber *retval);
#endif /* OIPF */

/**
   Gets OIPF property name as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetName(DownloadHandle id, const char **retval);

/**
   Gets OIPF property id as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetId(DownloadHandle id, const char **retval);

/**
   Gets OIPF property contentURL as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetContentURL(DownloadHandle id, const char **retval);

/**
   Gets OIPF property description as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetDescription(DownloadHandle id, const char **retval);

/**
   Gets OIPF property parentalRatings as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the parental ratings related to
   the download. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetParentalRatings(DownloadHandle id, int *count, ParentalRatingHandle **handles);

/**
   Gets OIPF property drmControl as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param count Number of handles in the returned array
   \param handles Use this to return the DRM control information of
   the downloaded content items. The OOIF plugin will take over
   ownership of the array and will later call free() to de-allocate
   the memory
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetDrmControl(DownloadHandle id, int *count, DRMControlInformationHandle **handles);

/**
   Gets OIPF property startTime as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4, represented as a
   date string of the format of (a string representation of) any
   value(s) acceptable as inparameter to the JavaScript Date()
   constructor, for example a dateString.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetStartTime(DownloadHandle id, const char **retval);

/**
   Gets OIPF property timeElapsed as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetTimeElapsed(DownloadHandle id, OOIFNumber *retval);

/**
   Gets OIPF property timeRemaining as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetTimeRemaining(DownloadHandle id, OOIFNumber *retval);

/**
   Gets OIPF property transferType as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetTransferType(DownloadHandle id, const char **retval);

/**
   Gets OIPF property originSite as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetOriginSite(DownloadHandle id, const char **retval);

/**
   Gets OIPF property originSiteName as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetOriginSiteName(DownloadHandle id, const char **retval);

/**
   Gets OIPF property contentID as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetContentID(DownloadHandle id, const char **retval);

/**
   Gets OIPF property iconURL as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param retval Retval as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadGetIconURL(DownloadHandle id, const char **retval);

#ifdef OIPF
/**
   Sets OIPF property description as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param value as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadSetDescription(DownloadHandle id, const char *value);

/**
   Sets OIPF property name as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param value as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadSetName(DownloadHandle id, const char *value);

/**
   Sets OIPF property contentID as defined in DAE 7.4.4.
   \param id Download instance identifier
   \param value as defined in DAE 7.4.4.
   \return OOIF return code
*/
OOIFReturnCode native_DownloadSetContentID(DownloadHandle id, const char *value);
#endif /* OIPF */
/*@}*/

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_DOWNLOAD */
#endif /* HBBTV_DOWNLOAD || OIPF */
