/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_VIDEO_BROADCAST
#define __NATIVE_VIDEO_BROADCAST

#include "NativeOOIFTypes.h"
#include "NativeAVComponent.h"
#include "NativeChannel.h"
#include "NativeParentalControlManager.h"
#include "NativeProgramme.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup Broadcast Scheduled content and hybrid tuner (VideoBroadcast)
   @{

   This is the VideoBroadcast native interface for the Scheduled
   content and hybrid tuner APIs as defined in DAE 7.13.
*/

/**
   Opaque reference to a VideoBroadcast instance in the native platform.
 */
typedef void* VideoBroadcastHandle;


/**
  Play state values specified in HbbTV Table A.4
*/
typedef enum {
   BROADCAST_PLAYSTATE_UNREALIZED = 0,
   BROADCAST_PLAYSTATE_CONNECTING = 1,
   BROADCAST_PLAYSTATE_PRESENTING = 2,
   BROADCAST_PLAYSTATE_STOPPED = 3
} BroadcastPlaystate;

/**
   VideoBroadcast event types.
*/
typedef enum {
	/** This event corresponds to the onChannelChangeError event defined in DAE 7.13.1 */
	BROADCAST_EVENT_CHANNEL_CHANGE_ERROR = 0,
	/** This event corresponds to the onPlayStateChange event defined in DAE 7.13.1 */
	BROADCAST_EVENT_PLAYSTATE_CHANGE = 1,
	/** This event corresponds to the onChannelChangeSucceeded event defined in DAE 7.13.1 */
	BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED = 2,
	/** This event corresponds to the onFullScreenChange event defined in DAE 7.13.1 */
	BROADCAST_EVENT_FULLSCREEN_CHANGE = 3,
	/** This event corresponds to the onParentalRatingChange event defined in DAE 7.13.5 */
	BROADCAST_EVENT_PARENTAL_RATING_CHANGE = 4,
	/** This event corresponds to the onParentalRatingError event defined in DAE 7.13.5 */
	BROADCAST_EVENT_PARENTAL_RATING_ERROR = 5,
	/** This event corresponds to the onProgrammeChanged event defined in DAE 7.13.3 */
	BROADCAST_EVENT_PROGRAMMES_CHANGED = 6,
	/** This event corresponds to the onDRMRightsErrorEvent event defined in DAE 7.13.6 */
	BROADCAST_EVENT_DRM_RIGHTS_ERROR_EVENT = 7,
	/** This event corresponds to the onRecordingEvent event defined in DAE 7.13.2 */
	BROADCAST_EVENT_RECORDING_EVENT = 8
#if defined(HBBTV_PVR) || defined(OIPF)
	,
	/** This event corresponds to the onPlaySpeedChanged event defined in DAE 7.13.2 */
	BROADCAST_EVENT_PLAY_SPEED_CHANGED = 9,
	/** This event corresponds to the onPlayPositionChanged event defined in DAE 7.13.2 */
	BROADCAST_EVENT_PLAY_POSITION_CHANGED = 10
#endif /* HBBTV_PVR || OIPF */
#ifdef OIPF
	,
	/** This event corresponds to the onPlaySpeedsArrayChanged event defined in DAE 7.13.2 */
	BROADCAST_EVENT_PLAY_SPEEDS_ARRAY_CHANGED = 11
#endif /* OIPF */
#if defined(OIPF) || defined(HBBTV_1_2_1)
	,
	/** This event corresponds to the onSelectedComponentChanged event defined in DAE 7.16.5.1.4 **/
	BROADCAST_EVENT_SELECTED_COMPONENT_CHANGED = 12
#endif /* OIPF || HBBTV_1_2_1*/

	// HBBTV_2_0
	,BROADCAST_EVENT_COMPONENT_CHANGED = 13
} BroadcastEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#BroadcastEvent.
*/
typedef union {
	/** used for BROADCAST_EVENT_CHANNEL_CHANGE_ERROR as described in DAE 7.13.1.2. Use channel=0 for 'NULL' */
	struct {
		ChannelHandle channel;
		OOIFNumber error_state;
	} channel_change_error_data;
	/** used for BROADCAST_EVENT_PLAYSTATE_CHANGE as described in DAE 7.13.1.2 */
	struct {
		BroadcastPlaystate state;
		OOIFBoolean has_error;
		OOIFNumber error;
	} playstate_change_data;
	/** used for BROADCAST_EVENT_CHANNEL_CHANGE_SUCCEED as described in DAE 7.13.1.2 Use channel=0 for 'NULL'  */
	struct {
		ChannelHandle channel;
	} channel_change_succeed_data;
	/** used for BROADCAST_EVENT_PARENTAL_RATING_CHANGE as described in DAE 7.13.5 */
	struct {
		const char* contentID;
#if defined(OIPF) || defined(HBBTV_1_2_1)
		ParentalRatingHandle* ratings; /*!< Opera takes ownership of memory allocated in this array */
		int ratings_count;
#else
		ParentalRatingHandle rating;
#endif
		const char* DRMSystemID; /*!< use NULL for JavaScripts's null */
#if defined(OIPF) || defined(HBBTV_1_2_1)
		OOIFBoolean blocked;
#endif /* OIPF || HBBTV_1_2_1 */
	} parental_rating_change_data;
	/** used for BROADCAST_EVENT_PARENTAL_RATING_ERROR as described in DAE 7.13.5 */
	struct {
		const char* contentID;
#if defined(OIPF) || defined(HBBTV_1_2_1)
		ParentalRatingHandle* ratings; /*!< Opera takes ownership of memory allocated in this array */
		int ratings_count;
#else
		ParentalRatingHandle rating;
#endif
		const char* DRMSystemID; /*!< use NULL for JavaScripts's null */
	} parental_rating_error_data;
#if defined(HBBTV_PVR) || defined(OIPF)
	/** used for BROADCAST_EVENT_PLAY_SPEED_CHANGED as described in DAE 7.13.2 */
	struct {
		OOIFNumber speed;
	} play_speed_changed_data;
	/** used for BROADCAST_EVENT_PLAY_POSITION_CHANGED as described in DAE 7.13.2 */
	struct {
		OOIFBoolean has_position;
		OOIFNumber position;
	} play_position_changed_data;
#endif /* HBBTV_PVR || OIPF */
	/** used for BROADCAST_EVENT_RECORDING_EVENT as described in DAE 7.13.2 */
	struct {
		OOIFNumber state;
		OOIFBoolean has_error;
		OOIFNumber error;
		const char *recordingId;
	} recording_event_data;
	/** used for BROADCAST_EVENT_DRM_RIGHTS_ERROR_EVENT as described in DAE 7.13.6 */
	struct {
		OOIFNumber error_state;
		const char* content_id;
		const char* drm_system_id;
		const char* rights_issuer_url;
	} drm_rights_error_data;
#if defined(OIPF) || defined(HBBTV_1_2_1)
	/** used for BROADCAST_EVENT_SELECTED_COMPONENT_CHANGED as described in DAE 7.16.5.1.4 */
	struct {
		OOIFNumber componentType;
	} selected_component_changed_data;
#endif /* OIPF || HBBTV_1_2_1 */

	// HBBTV_2_0
	struct {
		OOIFNumber componentType;
	} component_changed_data;

} BroadcastEventInfo;

/**
  The following constants define type of selected AV component
  as described in the DAE1.1 7.13.4.1 / DAE 7.16.5.1
*/
typedef enum {
	BROADCAST_COMPONENT_TYPE_VIDEO = 0,
	BROADCAST_COMPONENT_TYPE_AUDIO = 1,
	BROADCAST_COMPONENT_TYPE_SUBTITLE = 2,
} BroadcastComponentType;

#if defined(HBBTV_PVR) || defined(OIPF)
/**
  The following constants define type of playback position
  as described in the DAE 7.13.2.
*/
typedef enum {
	BROADCAST_POSITION_START = 0,
	BROADCAST_POSITION_CURRENT = 1,
	BROADCAST_POSITION_END = 2
} BroadcastPosition;
#endif /* HBBTV_PVR || OIPF */

/**
   The callback function that the native platform should call to
   dispatch a VideoBroadcast related event. The OOIF plugin will call the
   registered listeners on the VideoBroadcast instance that the
   reference point out.
   \param event The event type
   \param info Event context info, or NULL if no context info exists
   \param id VideoBroadcast instance identifier
   @see native_BroadcastSetListener
 */
typedef void (videoBroadcastListener)(BroadcastEvent event, BroadcastEventInfo *info, VideoBroadcastHandle id);

/**
   Registers new listener on VideoBroadcast related events.
   \param listener The ligstener
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_BroadcastSetListener(videoBroadcastListener listener);

/**
   The callback function that the native platform should call to
   dispatch a VideoBroadcast stream event as defined in HBBTV 8.2.1.2 / DAE2.1 7.13.21.
   \param id The VideoBroadcast instance identifier received when the
   listener was added.
   @see native_BroadcastAddStreamEventListener
 */
typedef void (videoBroadcastStreamEventListener)(const char *name, void* listener_id, const char *data, const char *text, const char *status, VideoBroadcastHandle id);

/**
   Adds a new stream event listener as defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21.
   \param targetURL As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param eventName As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param listenerId Identifier for this listener instance, will be
   used as identifier if remove is called
   \param listener Listener function to call when event occur
   \param id The videoBroadcast instance id to send as reference at
   event callbacks
   \param windowIdentifier Identifier of the window in which the
   VideoBroadcast object can be found
   \return OOIF return code
   @see videoBroadcastStreamEventListener
   @see native_BroadcastRemoveStreamEventListener
*/
OOIFReturnCode native_BroadcastAddStreamEventListener(const char *targetURL, const char *eventName, void *listenerId, videoBroadcastStreamEventListener listener, VideoBroadcastHandle id, long int windowIdentifier);

/**
   Adds a new stream event listener as defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21 based
   on the content of an XML file describing the Stream Event DSM-CC
   object
   \param targetURL As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param eventName As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param eventId Event ID specified in XML file
   \param componentTag Component tag specified in XML file where the
   event will be sent out
   \param listenerId Identifier for this listener instance, will be
   used as identifier if remove is called
   \param listener Listener function to call when event occur
   \param id The videoBroadcast instance id to send as reference at
   event callbacks
   \param windowIdentifier Identifier of the window in which the
   VideoBroadcast object can be found
   \return OOIF return code
   @see videoBroadcastStreamEventListener
   @see native_BroadcastRemoveStreamEventListener
*/
OOIFReturnCode native_BroadcastAddXMLStreamEventListener(const char *targetURL, const char *eventName, OOIFNumber eventId, OOIFNumber componentTag, void *listenerId, videoBroadcastStreamEventListener listener, VideoBroadcastHandle id, long int windowIdentifier);

/**
   Removes a stream event listener as defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21.
   Note that the specification does not specify a use for the
   targetURL parameter. Listener should be removed matched on
   eventName and listenerId only, and will be so internally in OOIF.
   \param targetURL As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param eventName As defined in HBBTV 8.2.1.1 / DAE2.1 7.13.21
   \param listenerId Identifier for this listener instance, received
   when listener was added
   \return OOIF return code
   @see native_BroadcastAddStreamEventListener
*/
OOIFReturnCode native_BroadcastRemoveStreamEventListener(const char *targetURL, const char *eventName, void *listenerId);

/**
   Removes all stream listeners associated with the indicated
   VideoBroadcast instance.
   \param id The videoBroadcast instance id that was passed when the
   subscriptions were added.
   \return OOIF_RETURN_OK
   @see native_BroadcastAddStreamEventListener
*/
OOIFReturnCode native_BroadcastRemoveAllStreamEventListeners(VideoBroadcastHandle id);

/**
   Creates a new VideoBroadcast instance in the native platform. The
   returned handle will be used as unique identifier for the
   VideoBroadcast instance in the OOIF plugin.
   \param id Reference to the new instance
   \param window_identifier The identifier of the window the create
   call originates from
   \param host_uri The host URI of the application the create call
   originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_BroadcastCreate(VideoBroadcastHandle *id, long int window_identifier, const char *host_uri);

/**
   Called when an event listener is added, removed, or accessed from Javascript
   on a VideoBroadcast Object.  OOIF will handle it automatically, so no action
   is needed from the integration layer. Nonetheless, this callback gives the
   opportunity to prevent the listener from being added, removed, or accessed,
   by returning an error code. This may be used to implement security checks,
   as defined in section HbbTV 11.1, or capability checks as defined in DAE 7
   and DAE 9.3
   \param id Reference to the VideoBroadcast instance to which a event listener
   is being added, removed, or accessed.
   \param eventName the name of the event
   \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
*/
OOIFReturnCode native_BroadcastOnListenerAccessed(VideoBroadcastHandle id, const char *eventName);

/**
   Called when the VideoBroadcast instance is destroyed in the OOIF plugin.
   \param id VideoBroadcast instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_BroadcastReleaseHandle(VideoBroadcastHandle id);

/**
   Updates visibility of the video by start/stop drawing the video on the screen.
   \param id VideoBroadcast instance identifier
   \param visibility TRUE if video should be drawn, FALSE if drawing should stop
   \param unloading TRUE if the video object is being removed from the
   display due to the document being unloaded (browser navigating to
   another page). Relevant only if \ref visibility is FALSE.
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_BroadcastSetVisibility( VideoBroadcastHandle id, OOIFBoolean visibility, OOIFBoolean unloading);

/**
   Update video position.
   \param id VideoBroadcast instance identifier
   \param x X-position of the video window
   \param y Y-position of the video window
   \param width width of the video window
   \param height height of the video window
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_BroadcastSetVideoWindow(VideoBroadcastHandle id, int x, int y, int width, int height);

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   Gets OIPF property data as defined in DAE 7.13.1.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.1.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetData(VideoBroadcastHandle id, const char **retval);

/**
   Sets OIPF property data as defined in DAE 7.13.1.2.
   \param id VideoBroadcast instance identifier
   \param data As defined in DAE 7.13.1.2.
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSetData(VideoBroadcastHandle id, const char *data);
#endif /* OIPF || HBBTV_1_2_1 */

/**
   Gets OIPF property playState as defined in DAE 7.13.1.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.1.2
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastGetPlayState(VideoBroadcastHandle id, OOIFNumber *retval);

/**
   Gets OIPF property playState as defined in DAE 7.13.1.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.1.2
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastSetPlayState(VideoBroadcastHandle id, OOIFNumber playState);


/**
   Gets OIPF property currentChannel as defined in DAE1.1 7.13.7.1 / DAE 7.13.8.1.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE1.1 7.13.7.1 / DAE 7.13.8.1
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastGetCurrentChannel(VideoBroadcastHandle id, ChannelHandle *retval);

#ifdef OIPF
/**
   This function corresponds to the OIPF function bindToCurrentChannel() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.1.3.
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastBindToCurrentChannel(VideoBroadcastHandle id, ChannelHandle *retval);
#elif defined(HBBTV)
/**
   This function corresponds to the OIPF function bindToCurrentChannel() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastBindToCurrentChannel(VideoBroadcastHandle id);
#endif /* HBBTV */

/**
   This function corresponds to the OIPF function
   createChannelObject(Integer idType, String dsd, Integer sid) as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param idType As defined in DAE 7.13.1.3
   \param dsd As defined in DAE 7.13.1.3, as raw byte data
   \param dsd_len The length in bytes of the dsd parameter
   \param sid As defined in DAE 7.13.1.3
   \param retval Retval as defined in DAE 7.13.1.3
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastCreateChannelObject3(VideoBroadcastHandle id, OOIFNumber idType, const unsigned char *dsd, int dsd_len, OOIFNumber sid, ChannelHandle *retval);

/**
   This function corresponds to the OIPF function
   createChannelObject(Integer idType, Integer onid, Integer tsid, Integer sid,
   Integer sourceID, String ipBroadcastID) as defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param idType As defined in DAE 7.13.1.3
   \param has_onid is FALSE when the optional onid parameter isn't set.
   \param onid As defined in DAE 7.13.1.3
   \param has_tsid is FALSE when the optional tsid parameter isn't set.
   \param tsid As defined in DAE 7.13.1.3
   \param has_sid is FALSE when the optional sid parameter isn't set.
   \param sid As defined in DAE 7.13.1.3
   \param has_sourceID is FALSE when the optional sourceID parameter isn't set.
   \param sourceID As defined in DAE 7.13.1.3
   \param ipBroadcastID As defined in DAE 7.13.1.3, or NULL when not set.
   \param retval Retval as defined in DAE 7.13.1.3
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastCreateChannelObject6(VideoBroadcastHandle id, OOIFNumber idType, OOIFBoolean has_onid, OOIFNumber onid, OOIFBoolean has_tsid, OOIFNumber tsid, OOIFBoolean has_sid, OOIFNumber sid, OOIFBoolean has_sourceID, OOIFNumber sourceID, const char *ipBroadcastID, ChannelHandle *retval);

/**
   This function corresponds to the OIPF function setChannel() as
   defined in DAE 7.13.1.3, and DAE 7.13.2.3 for the offset parameter.
   \param id VideoBroadcast instance identifier
   \param channel As defined in DAE 7.13.1.3. channel=0 is used for 'NULL'
   \param has_trickplay set to TRUE if trickplay is defined, or to FALSE if it is undefined.
   \param trickplay As defined in DAE 7.13.1.3
   \param contentAccessDescriptorURL As defined in DAE 7.13.1.3
   \param has_offset is FALSE if either PVR or broadcast over IP is not supported, or if undefined
   \param offset As defined in DAE 7.13.2.3
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastSetChannel(VideoBroadcastHandle id, ChannelHandle channel, OOIFBoolean has_trickplay, OOIFBoolean trickplay, const char *contentAccessDescriptorURL, OOIFBoolean has_offset, OOIFNumber offset);

/**
   This function corresponds to the OIPF function prevChannel() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastPrevChannel(VideoBroadcastHandle id);

/**
   This function corresponds to the OIPF function nextChannel() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastNextChannel(VideoBroadcastHandle id);

/**
   This function corresponds to the OIPF function setFullScreen() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param fullscreen As defined in DAE 7.13.1.3
   \return OOIF_RETURN_OK if the call is successful
*/
OOIFReturnCode native_BroadcastSetFullScreen(VideoBroadcastHandle id, OOIFBoolean fullscreen);

#ifdef OIPF
/**
   This function corresponds to the OIPF function setVolume() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param volume As defined in DAE 7.13.1.2
   \param retval Retval as defined in DAE 7.13.1.2
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastSetVolume(VideoBroadcastHandle id, OOIFNumber volume, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function getVolume() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.1.3
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastGetVolume(VideoBroadcastHandle id, OOIFNumber *retval);
#endif /* OIPF */

/**
   This function corresponds to the OIPF function release() as
   defined in DAE 7.13.1.3.
   \param id VideoBroadcast instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastRelease(VideoBroadcastHandle id);

/**
   This function corresponds to the HbbTV / OIPF function stop() as
   defined in HBBTV A.2.4.1 / DAE 7.13.1.3
   \param id VideoBroadcast instance identifier
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastStop(VideoBroadcastHandle id);

/**
   Gets OIPF property programmes as defined in DAE 7.13.3.
   \param id VideoBroadcast instance identifier
   \param count Use this to return number of handles in the composed array
   \param handles Use this to return the composed array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetProgrammes(VideoBroadcastHandle id, int *count, ProgrammeHandle **handles);

/**
   This function corresponds to the OIPF function getComponents() as
   defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param componentType As defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3
   \param count Use this to return number of handles in the composed array
   \param handles Use this to return the composed component
   array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetComponents(VideoBroadcastHandle id, BroadcastComponentType componentType, int *count, AVComponentHandle **handles);

/**
   This function corresponds to the OIPF function getCurrentActiveComponents() as
   defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param componentType As defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3
   \param count Use this to return number of handles in the composed array
   \param handles Use this to return the composed component
   array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return Number of components in the returned array
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastGetCurrentActiveComponents(VideoBroadcastHandle id, BroadcastComponentType componentType, int *count, AVComponentHandle **handles);

/**
   This function corresponds to the OIPF function selectComponent() as
   defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param component As defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSelectComponent(VideoBroadcastHandle id, AVComponentHandle component);

/**
   This function corresponds to the OIPF function unselectComponent() as
   defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param component As defined in DAE1.1 7.13.4.1.2 / DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastUnselectComponent(VideoBroadcastHandle id, AVComponentHandle component);

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   This function corresponds to the OIPF function selectComponent() as
   defined in DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param component As defined in DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSelectComponent2(VideoBroadcastHandle id, OOIFNumber component);

/**
   This function corresponds to the OIPF function unselectComponent() as
   defined in DAE 7.16.5.1.3.
   \param id VideoBroadcast instance identifier
   \param component As defined in DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastUnselectComponent2(VideoBroadcastHandle id, OOIFNumber component);
#endif /* OIPF || HBBTV_1_2_1 */

#if defined(HBBTV_PVR) || defined(OIPF)
/**
   Gets OIPF property playbackOffset as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetPlaybackOffset(VideoBroadcastHandle id, OOIFNumber *retval);

/**
   Gets OIPF property maxOffset as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
*/
OOIFReturnCode native_BroadcastGetMaxOffset(VideoBroadcastHandle id, OOIFNumber *retval);
#endif /* HBBTV_PVR || OIPF */

/**
   Gets OIPF property recordingState as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetRecordingState(VideoBroadcastHandle id, OOIFNumber *retval);

/**
   Gets OIPF property playPosition as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetPlayPosition(VideoBroadcastHandle id, OOIFNumber *retval);

/**
   Gets OIPF property playSpeed as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetPlaySpeed(VideoBroadcastHandle id, OOIFNumber *retval);

#ifdef OIPF
/**
   Gets OIPF property timeShiftMode as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetTimeShiftMode(VideoBroadcastHandle id, OOIFNumber *retval);

/**
   Sets OIPF property timeShiftMode as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param timeShiftMode Value of new timeShiftMode as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSetTimeShiftMode(VideoBroadcastHandle id, OOIFNumber timeShiftMode);

/**
   Gets OIPF property currentTimeShiftMode as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.2
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetCurrentTimeShiftMode(VideoBroadcastHandle id, OOIFNumber *retval);
#endif /* OIPF */

#if defined(HBBTV_PVR) || defined(OIPF)
/**
   Gets OIPF property playSpeeds as defined in DAE 7.13.2.2.
   \param id VideoBroadcast instance identifier
   \param count Use this to return number of handles in the composed array
   \param playSpeeds Use this to return the composed array of
   playSpeeds if supported playback speeds are known. The OOIF plugin
   will take over ownership of the array and will later call free() to
   de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastGetPlaySpeeds(VideoBroadcastHandle id, int *count, OOIFNumber **playSpeeds);

/**
   This function corresponds to the OIPF function recordNow() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param duration As defined in DAE 7.13.2.3
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastRecordNow(VideoBroadcastHandle id, OOIFNumber duration, const char **retval);

/**
   This function corresponds to the OIPF function stopRecording() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastStopRecording(VideoBroadcastHandle id);

/**
   This function corresponds to the OIPF function pause() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastPause(VideoBroadcastHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function resume() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastResume(VideoBroadcastHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function setSpeed() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param speed As defined in DAE 7.13.2.3
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSetSpeed(VideoBroadcastHandle id, OOIFNumber speed, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function seek() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param offset As defined in DAE 7.13.2.3
   \param reference As defined in DAE 7.13.2.3
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastSeek(VideoBroadcastHandle id, OOIFNumber offset, BroadcastPosition reference, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function stopTimeshift() as
   defined in DAE 7.13.2.3.
   \param id VideoBroadcast instance identifier
   \param retval Retval as defined in DAE 7.13.2.3
   \return OOIF return code
 */
OOIFReturnCode native_BroadcastStopTimeshift(VideoBroadcastHandle id, OOIFBoolean *retval);
#endif /* HBBTV_PVR || OIPF */

/*@}*/
OOIFReturnCode native_BroadcastFindChannelByTriplet(OOIFNumber idType, OOIFNumber onid, OOIFNumber tsid, OOIFNumber sid,  ChannelHandle *retval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_VIDEO_BROADCAST */
