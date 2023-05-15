/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_DRMAGENT
#define __NATIVE_DRMAGENT

#include "NativeOOIFTypes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup DRM Content Service Protection (DRM)
   @{

   This file defines the native interface for the DRM agent.
   See DAE 7.4 for more details.
*/

/**
   Opaque pointer to a DrmAgent instance in the native platform
*/
typedef void* DrmAgentHandle;

#ifdef OIPF
/**
	DRM System Status values
*/
typedef enum {
	READY = 0,
	UNKNOWN = 1,
	INITIALISING = 2,
	ERROR = 3
} DrmSystemStatus;
#endif /* OIPF */

/**
	DRM event types.
*/
typedef enum {
	/** This event corresponds to the onDRMMessageResult event defined in DAE 7.6.1 */
	DRM_EVENT_DRM_MESSAGE_RESULT = 0
#ifdef OIPF
	/** This event corresponds to the onDRMSystemStatusChange event defined in DAE 7.6.1 */
	, DRM_EVENT_DRM_SYSTEM_STATUS_CHANGE = 1
#endif /* OIPF */
#if defined(OIPF) || defined(HBBTV_1_2_1)
	/** This event corresponds to the onDRMSystemMessage event defined in DAE 7.6.1 */
	, DRM_EVENT_DRM_SYSTEM_MESSAGE = 2
#endif /* OIPF || HBBTV_1_2_1 */
} DrmEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#DrmEvent.
*/
typedef union {
	/** used for DRM_EVENT_DRM_MESSAGE_RESULT as described in DAE 7.6.1.1 */
	struct {
		const char *msgID;
		const char *resultMsg;
		OOIFNumber resultCode;
	} drm_message_result_data;
#ifdef OIPF
	/** used for DRM_EVENT_DRM_SYSTEM_STATUS_CHANGE as described in DAE 7.6.1.1 */
	struct {
		const char *DRMSystemID;
	} drm_system_status_change;
#endif /* OIPF */
#if defined(HBBTV_1_2_1) || defined(OIPF)
	/** used for DRM_EVENT_DRM_SYSTEM_MESSAGE as described in DAE 7.6.1.1 */
	struct {
		const char *msg;
		const char *DRMSystemID;
	} drm_system_message;
#endif /* HBBTV_1_2_1 || OIPF */
} DrmEventInfo;

/**
   An OOIF internal callback function which the native code
   should call when it wishes to dispatch a DRM related event.

   \param DrmEvent The event type
   \param DrmEventInfo Event context info
   \param id The instance identifier of the DrmAgent the event occurred on
   @see native_DrmAgentSetListener
 */
typedef void (drmListener)(DrmEvent event, DrmEventInfo *info, DrmAgentHandle id);

/**
   The OOIF will call this function to register the function that the
   native code should call when a DRM event has occurred.

   \param listener The listener callback function
   \return OOIF_RETURN_OK
   \see drmListener
 */
OOIFReturnCode native_DrmAgentSetListener(drmListener listener);

/**
   Creates a new DrmAgent instance in the native platform. The
   returned handle will be used as unique identifier for the
   DrmAgent instance in the OOIF plugin.
   \param id Reference to the new instance
   \param window_identifier The identifier of the window the create
   call originates from
   \param host The host URI of the application the create call
   originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_DrmAgentCreate(DrmAgentHandle *id, long int window_identifier, const char *host);

/**
   Called when the DrmAgent instance is destroyed in the OOIF plugin.
   \param id Download instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_DrmAgentReleaseHandle(DrmAgentHandle id);

/**
 * Called when an event listener is added, removed, or accessed from Javascript
 * on a DrmAgent Object.  OOIF will handle it automatically, so no action
 * is needed from the integration layer. Nonetheless, this callback gives the
 * oportunity to prevent the listener from being added, removed, or accessed,
 * by returning an error code. This may be used to implement security checks,
 * as defined in section HbbTV 11.1, or capability checks as defined in DAE 7
 * and DAE 9.3
 * \param id Reference to the DrmAgent instance to which a event listener
 * is being added, removed, or accessed.
 * \param eventName the name of the event
 * \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_DrmAgentOnListenerAccessed(DrmAgentHandle id, const char *eventName);

/**
   This function corresponds to sendDRMMessage() as defined in DAE
   7.6.1.2. The function should return a unique ID to identify the
   message.
   \param id DrmAgent instance identifier
   \param msgType As defined in DAE 7.6.1.2
   \param msg As defined in DAE 7.6.1.2
   \param DRMSystemID As defined in DAE 7.6.1.2
   \param retval Return value as defined in DAE 7.6.1.2
   \return OOIF return code
*/
OOIFReturnCode native_DrmAgentSendDRMMessage(DrmAgentHandle id, const char *msgType, const char *msg, const char *DRMSystemID, const char **retval);

#ifdef OIPF
/**
   This function corresponds to DRMSystemStatus() as defined in DAE 7.6.1.2.
   \param id DrmAgent instance identifier
   \param DRMSystemID As defined in DAE 7.6.1.2
   \param retval Return value as defined in DAE 7.6.1.2
   \return OOIF return code
*/
OOIFReturnCode native_DrmAgentDRMSystemStatus(DrmAgentHandle id, const char *DRMSystemID, DrmSystemStatus *retval);
#endif /* OIPF */

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
/**
   Opaque reference to a DRMControlInfo instance in the native platform
*/
typedef void* DRMControlInformationHandle;

/** @name DRMControlInfo
	The DRMControlInfo class as defined in DAE 7.4.6.
	@{*/
/**
   Struct for describing a DRMControlInformation object properties as defined
   in DAE 7.4.6. The OOIF plugin will take over ownership of all
   strings in the data struct, HLIB_STD_StrDup() should be used in the native
   code to allocate memory for them since the plugin will later use
   free() to de-allocate the memory.
 */
typedef struct {
	char *drmType;
	char *drmContentID;
	char *rightsIssuerURL;
	char *silentRightsURL;
	char *previewRightsURL;
	char *drmPrivateData;
	OOIFBoolean doNotRecord;
	OOIFBoolean doNotTimeShift;
} DRMControlInformationData;

/**
   Called when the DRMControlInfo instance is destroyed in the OOIF plugin.
   \param id DRMControlInfo instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_DRMControlInformationReleaseHandle(DRMControlInformationHandle id);

/**
   Populate the data struct fields with the property values of the
   referenced DRMControlInfo instance as defined in DAE 7.4.6. The
   OOIF Plugin will handle all getter calls.

   \param identifier DRMControlInfo instance identifier
   \param data The data struct to be initialized
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_DRMControlInformationInitialize(DRMControlInformationHandle identifier,
											   DRMControlInformationData *data);

/*@}*/
#endif // HBBTV_DOWNLOAD || OIPF
/*@}*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_DRMAGENT */
