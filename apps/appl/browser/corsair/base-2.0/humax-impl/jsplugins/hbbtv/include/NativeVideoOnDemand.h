/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_VIDEO_ON_DEMAND
#define __NATIVE_VIDEO_ON_DEMAND

#include "NativeAVComponent.h"
#include "NativeOOIFTypes.h"
#include "NativeParentalControlManager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
   \defgroup VOD Media Playback (VideoOnDemand)
   @{

   This is the VideoOnDemand (VOD) native interface for the Media
   Playback API and streamed A/V content as defined in DAE 7.14
   and CEA-2014-A 5.7.1.
*/

/**
   Opaque reference to a VideoOnDemand instance in the native platform.
 */
typedef void* VodHandle;

/**
	VideoOnDemand event types.
*/
typedef enum {
	/** This event corresponds to the onPlaySpeedChanged event defined in CEA-2014-A req 5.7.1.f */
	VOD_EVENT_PLAY_STATE_CHANGED = 0,
	/** This event corresponds to the onFullScreenChanged event defined in CEA-2014-A req 5.7.1.g */
	VOD_EVENT_FULL_SCREEN_CHANGED = 1,
#ifndef HBBTV_1_2_1
	/** This event corresponds to the onPlaySpeedChanged event defined in DAE 7.14.3 */
	VOD_EVENT_PLAY_SPEED_CHANGED = 2,
#endif // !HBBTV_1_2_1
	/** This event corresponds to the onPlayPositionChanged event defined in DAE 7.14.3 */
	VOD_EVENT_PLAY_POSITION_CHANGED = 3,
	/** This event corresponds to the onDRMRightsError event defined in DAE 7.14.6 */
	VOD_EVENT_DRM_RIGHTS_ERROR = 4,
	/** This event corresponds to the onParentalRating event defined in DAE 7.14.5 */
	VOD_EVENT_PARENTAL_RATING_CHANGE = 5,
	/** This event corresponds to the onParentalRatingError event defined in DAE 7.14.5 */
	VOD_EVENT_PARENTAL_RATING_ERROR = 6
#ifdef OIPF
	,
	/** This event corresponds to the onReadyToPlay event defined in DAE 7.14.8.1 **/
	VOD_EVENT_READY_TO_PLAY = 7,
	/** This event corresponds to the onRepresentationChange event defined in DAE 7.14.8.1 **/
	VOD_EVENT_REPRESENTATION_CHANGE = 8,
	/** This event corresponds to the onPeriodChange event defined in DAE 7.14.8.1 **/
	VOD_EVENT_PERIOD_CHANGE = 9,
	/** This event corresponds to the onPlaySpeedsArrayChanged event defined in DAE 7.14.3.2 **/
	VOD_EVENT_PLAY_SPEEDS_ARRAY_CHANGED = 10
#endif /* OIPF */
#if defined(OIPF) || defined(HBBTV_1_2_1)
	,
	/** This event corresponds to the onSelectedComponentChanged event defined in DAE 7.16.5.1.4 **/
	VOD_EVENT_SELECTED_COMPONENT_CHANGED = 11
#endif /* OIPF || HBBTV_1_2_1 */
#if defined(HMX_VOD_PDL)
	/** This event corresponds to the onReadyStateChange event */
	,VOD_EVENT_READY_STATE_CHANGED = 12
#endif
} VodEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#VodEvent.
*/
typedef union {
	/** used for VOD_EVENT_PLAY_SPEED_CHANGED as described in DAE 7.14.3.1 */
	struct {
		OOIFNumber speed;
	} play_speed_changed_data;
	/** used for VOD_EVENT_PLAY_POSITION_CHANGED as described in DAE 7.14.3.1 */
	struct {
		OOIFBoolean has_position;
		OOIFNumber position;
	} play_position_changed_data;
	/** used for VOD_EVENT_DRM_RIGHTS_ERROR as described in DAE 7.14.6 */
	struct {
		OOIFNumber error_state;
		const char* content_id;
		const char* drm_system_id;
		const char* rights_issuer_url;
	} drm_rights_error_data;
	/** used for VOD_EVENT_PARENTAL_RATING_CHANGE as described in DAE 7.14.5 */
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
#endif /* OIPF || HBBTV_1_2_1*/
	} parental_rating_change_data;
	/** used for VOD_EVENT_PARENTAL_RATING_ERROR as described in DAE 7.14.5 */
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
#ifdef OIPF
	/** used for VOD_EVENT_REPRESENTATION_CHANGE as described in DAE 7.14.8 */
	struct {
		OOIFNumber bandwidth;
		OOIFNumber position;
	} representation_change_data;
	/** used for VOD_EVENT_PERIOD_CHANGE as described in DAE 7.14.8 */
	struct {
		OOIFNumber bandwidths[];
		OOIFNumber position;
		int bandwidths_count;
	} period_change_data;
#endif /* OIPF */
#if defined(OIPF) || defined(HBBTV_1_2_1)
	/** used for BROADCAST_EVENT_SELECTED_COMPONENT_CHANGED as described in DAE 7.16.5.1.4 */
	struct {
		OOIFNumber componentType;
	} selected_component_changed_data;
#endif /* OIPF || HBBTV_1_2_1 */
} VodEventInfo;

/**
  The following constants define type of selected AV component
  as described in the DAE1.1 7.14.4.1 / DAE 7.16.5.1
*/
typedef enum {
   VOD_COMPONENT_TYPE_VIDEO = 0,
   VOD_COMPONENT_TYPE_AUDIO = 1,
   VOD_COMPONENT_TYPE_SUBTITLE = 2,
} VodComponentType;

/**
   The callback function that the native platform should call to
   dispatch a VideoOnDemand related event. The OOIF plugin will call the
   registered listeners on the VideoOnDemand instance that the
   reference point out.
   \param event The event type
   \param info Event context info, or NULL if no context info exists
   \param id VOD instance identifier
   @see native_VodSetListener
 */
typedef void (vodListener)(VodEvent event, VodEventInfo *info, VodHandle id);

/**
   Registers new listener on VideoOnDemand related events.
   \param listener The listener callback function
   \return OOIF_RETURN_OK
   @see vodListener
 */
OOIFReturnCode native_VodSetListener(vodListener listener);

/**
   Determines whether the given mime-type is supported by VideoOnDemand object.
   NOTE: The OOIF framework takes ownership of the returned mime_types array
   and the strings it contains, and will free the memory when appropriate.
   malloc(), strdup(), or similar should be used in the native code to allocate
   memory for them.
   @param mime_types The array of supported mime-types.
   @param count Number of mime-types supported
   @return OOIF_RETURN_OK
 */
OOIFReturnCode native_VodGetSupportedMimetypes(char ***mime_types, int* count);

/**
   Creates a new VideoOnDemand instance in the native platform. The
   returned handle will be used as unique identifier for the
   VideoOnDemand instance in the OOIF plugin.
   \param id Reference to the new instance
   \param window_identifier The identifier of the window the create
   call originates from
   \param host_uri The host URI of the application the create call
   originates from
   \param mime_type The mime type of the VOD instance
   \param data The data of the VOD instance, or NULL if no data parameter was
   found in the object tag
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_VodCreate(VodHandle *id, long int window_identifier, const char *host_uri, const char *mime_type, const char *data);

/**
   Called when a VideoOnDemand instance is destroyed in the OOIF plugin.
   \param id VOD instance identifier
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_VodReleaseHandle(VodHandle id);

/**
   Called when an event listener is added, removed, or accessed from Javascript
   on a VideoOnDemand Object.  OOIF will handle it automatically, so no action
   is needed from the integration layer. Nonetheless, this callback gives the
   opportunity to prevent the listener from being added, removed, or accessed,
   by returning an error code. This may be used to implement security checks,
   as defined in section HbbTV 11.1, or capability checks as defined in
   DAE 7 and DAE 9.3
   \param id Reference to the VideoOnDemand instance to which a event listener
   is being added, removed, or accessed.
   \param eventName the name of the event
   \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_VodOnListenerAccessed(VodHandle id, const char *eventName);

/**
   Gets the streamed A/V content property data as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetData(VodHandle id, const char **retval);

/**
   Gets the streamed A/V content property "type"
   See the diagram in DAE1.1 7.14.1 / DAE2.1 7.14.1.1 of OIPF/DAE.
   \param id VOD instance identifier
   \param retval Retval
   \return OOIF return code
*/
OOIFReturnCode native_VodGetType(VodHandle id, const char **retval);

/**
   Sets the streamed A/V content property data as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param data As defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodSetData(VodHandle id, const char *data);

/**
   Sets the streamed A/V content property "type"
   See the diagram in DAE1.1 7.14.1 / DAE2.1 7.14.1.1.
   \param id VOD instance identifier
   \param type Mime type set.
   \return OOIF return code. OOIF_RETURN_NOT_SUPPORTED if mime type is not supported.
*/
OOIFReturnCode native_VodSetType(VodHandle id, const char *type);

/**
   Gets the streamed A/V content property playPosition as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetPlayPosition(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property playTime as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetPlayTime(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property playState as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetPlayState(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property error as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetError(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property speed as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetSpeed(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property nrTacks as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetNrTracks(VodHandle id, OOIFNumber *retval);

/**
   Gets the streamed A/V content property currentTrackIndex as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetCurrentTrackIndex(VodHandle id, OOIFNumber *retval);

/**
   Sets the streamed A/V content property currentTrackIndex as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param index As defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodSetCurrentTrackIndex(VodHandle id, OOIFNumber index);

/**
   Gets the streamed A/V content property persist as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetPersist(VodHandle id, OOIFBoolean *retval);

/**
   Sets the streamed A/V content property persist as defined in
   CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param persist As defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodSetPersist(VodHandle id, OOIFBoolean persist);

#ifdef OIPF
/**
   Gets a comma-separated list (CSV list) of information on protocols the underlying mechanism supports for ?쐓inking?
   AV content in its current state as defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetSinkProtocolInfo(VodHandle id, const char **retval);

/**
   Gets a collection of UPnP AVTransport state variable names and their respective values that are associated
   with this AV object as defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param stateVariableList State variable list as defined in CEA-2014-A req 5.7.1.f
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodGetTransportStateVariables(VodHandle id, const char *stateVariableList, const char **retval);

/**
   Extracts the values from the stateVariableValuePairs argument and copies these values to the corresponding
   UPnP AVTransport state variables associated with this AV object as defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param stateVariableValuePairs State variable value pairs as defined in CEA-2014-A req 5.7.1.f
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodSetTransportStateVariables(VodHandle id, const char *stateVariableValuePairs, const char **retval);

#endif // OIPF

/**
   This function corresponds to the streamed A/V content function play() as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param speed As defined in CEA-2014-A req 5.7.1.f
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodPlay(VodHandle id, OOIFNumber speed, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function stop() as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
*/
OOIFReturnCode native_VodStop(VodHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function seek() as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param pos As defined in CEA-2014-A req 5.7.1.f
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
 */
OOIFReturnCode native_VodSeek(VodHandle id, OOIFNumber pos, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function setVolume()
   as defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param volume As defined in CEA-2014-A req 5.7.1.f
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
 */
OOIFReturnCode native_VodSetVolume(VodHandle id, OOIFNumber volume, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function getVolume()
   as defined in CEA-2014-A req 5.7.1.
   \param id VOD instance identifier
   \param volume As defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
 */
OOIFReturnCode native_VodGetVolume(VodHandle id, OOIFNumber *volume);

/**
   This function corresponds to the streamed A/V content function next() as
   defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
 */
OOIFReturnCode native_VodNext(VodHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function previous()
   as defined in CEA-2014-A req 5.7.1.f.
   \param id VOD instance identifier
   \param retval Retval as defined in CEA-2014-A req 5.7.1.f
   \return OOIF return code
 */
OOIFReturnCode native_VodPrevious(VodHandle id, OOIFBoolean *retval);

/**
   This function corresponds to the streamed A/V content function
   setFullScreen() as defined in CEA-2014-A req 5.7.1.g.
   \param id VOD instance identifier
   \param fullscreen As defined in CEA-2014-A req 5.7.1.g
   \return OOIF_RETURN_OK if the call is successful
*/
OOIFReturnCode native_VodSetFullScreen(VodHandle id, OOIFBoolean fullscreen);

/**
   This function corresponds to the OIPF function getComponents() as
   defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param componentType As defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3
   \param count Use this to return number of handles in the composed array
   \param handles Use this to return the composed component
   array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_VodGetComponents(VodHandle id, OOIFNumber componentType, int *count, AVComponentHandle **handles);

/**
   This function corresponds to the OIPF function getCurrentActiveComponents() as
   defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param componentType As defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3
   \param count Use this to return number of handles in the composed array
   \param handles Use this to return the composed component
   array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_VodGetCurrentActiveComponents(VodHandle id, OOIFNumber componentType, int *count, AVComponentHandle **handles);

/**
   This function corresponds to the OIPF function selectComponent() as
   defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param component As defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_VodSelectComponent(VodHandle id, AVComponentHandle component);

/**
   This function corresponds to the OIPF function unselectComponent() as
   defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param component As defined in DAE1.1 7.14.4.1.2 / DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_VodUnselectComponent(VodHandle id, AVComponentHandle component);

#if defined(OIPF) || defined(HBBTV_1_2_1)
/**
   This function corresponds to the OIPF function selectComponent() as
   defined in DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param component As defined in DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_VodSelectComponent2(VodHandle id, OOIFNumber component);

/**
   This function corresponds to the OIPF function unselectComponent() as
   defined in DAE 7.16.5.1.3.
   \param id VOD instance identifier
   \param component As defined in DAE 7.16.5.1.3
   \return OOIF return code
 */
OOIFReturnCode native_VodUnselectComponent2(VodHandle id, OOIFNumber component);
#endif /* OIPF || HBBTV_1_2_1 */

/**
   This function corresponds to the HbbTV function queue() as
   defined in HBBTV A.2.5 / DAE2.1 7.14.11.
   \param id VOD instance identifier
   \param url As defined in HBBTV A.2.5
   \param retval Retval as defined in HBBTV A.2.5
   \return OOIF return code
 */
OOIFReturnCode native_VodQueue(VodHandle id, const char* url, OOIFBoolean *retval);

#if defined(HBBTV_PVR) || defined(HBBTV_DOWNLOAD) || defined(OIPF)
/**
   This function corresponds to the OIPF function setSource() as
   defined in DAE 7.14.7.
   \param identifier VOD instance identifier
   \param id As defined in  DAE 7.14.7
   \param retval Retval as defined in  DAE 7.14.7
   \return OOIF return code
 */
OOIFReturnCode native_VodSetSource(VodHandle identifier, const char* id, OOIFBoolean *retval);
#endif /* HBBTV_PVR || HBBTV_DOWNLOAD || OIPF */

#ifdef OIPF
/**
   Gets OIPF property playSpeeds as defined in DAE 7.14.3.1.
   \param id Vod instance identifier
   \param count Use this to return number of handles in the composed array
   \param playSpeeds Use this to return the composed array of
   playSpeeds if supported playback speeds are known. The OOIF plugin
   will take over ownership of the array and will later call free() to
   de-allocate the memory
   \return OOIF return code
 */
OOIFReturnCode native_VodGetPlaySpeeds(VodHandle id, int *count, OOIFNumber **playSpeeds);

/** This function corresponds to the OIPF property oitfSourceIPAddress as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param retval Return value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodGetOitfSourceIPAddress(VodHandle id, const char **retval);

/** This function corresponds to the OIPF property oitfSourcePortAddress as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param retval Return value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodGetOitfSourcePortAddress(VodHandle id, const char **retval);

/** This function gets the OIPF property oitfNoRTSPSessionControl as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param retval Return value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodGetOitfNoRTSPSessionControl(VodHandle id, OOIFBoolean *retval);

/** This function sets the OIPF property oitfNoRTSPSessionControl as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param value Property value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodSetOitfNoRTSPSessionControl(VodHandle id, OOIFBoolean value);

/** This function gets the OIPF property oitfRTSPSessionId as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param retval Return value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodGetOitfRTSPSessionId(VodHandle id, const char **retval);

/** This function sets the OIPF property oitfRTSPSessionId as defined
    in DAE 7.14.3.1
    \param id Vod instance identifier
    \param value Property value as defined in OIPF 7.14.3.1
    \return OOIF return code
*/
OOIFReturnCode native_VodSetOitfRTSPSessionId(VodHandle id, const char *value);

/** This function corresponds to the OIPF property readyToPlay as defined
    in DAE 7.14.8.1
    \param id Vod instance identifier
    \param retval Return value as defined in OIPF 7.14.8.1
    \return OOIF return code
*/
OOIFReturnCode native_VodGetReadyToPlay(VodHandle id, OOIFBoolean *retval);

/**
   Gets OIPF property availableRepresentationsBandwidth as defined in DAE 7.14.8.1.
   \param id Vod instance identifier
   \param count Use this to return number of handles in the composed array
   \param bandwidths Use this to return the composed array. The OOIF plugin will take over ownership of the array
   and will later call free() to de-allocate the memory.
   \return OOIF return code
 */
OOIFReturnCode native_VodGetAvailableRepresentationsBandwidth(VodHandle id, int *count, OOIFNumber **bandwidths);

/**
   Gets OIPF property currentRepresentation as defined in DAE 7.14.8.1.
   \param id Vod instance identifier
   \param retval Return value as defined in OIPF 7.14.8.1
   \return OOIF return code
 */
OOIFReturnCode native_VodGetCurrentRepresentation(VodHandle id, OOIFNumber *retval);

/**
   Gets OIPF property maxRepresentation as defined in DAE 7.14.8.1.
   \param id Vod instance identifier
   \param retval Return value as defined in OIPF 7.14.8.1
   \return OOIF return code
 */
OOIFReturnCode native_VodGetMaxRepresentation(VodHandle id, OOIFNumber *retval);

/**
   Gets OIPF property minRepresentation as defined in DAE 7.14.8.1.
   \param id Vod instance identifier
   \param retval REturn value as defined in OIPF 7.14.8.1
   \return OOIF return code
 */
OOIFReturnCode native_VodGetMinRepresentation(VodHandle id, OOIFNumber *retval);

/**
   Gets OIPF property supportedStrategies as defined in DAE 7.14.8.1.
   \param id Vod instance identifier
   \param count Use this to return number of handles in the composed array
   \param strategies Use this to return the array.
   The OOIF framework takes ownership of the returned array
   and the strings it contains, and will free the memory when appropriate.
   malloc(), strdup(), or similar should be used in the native code to allocate
   memory for them.
   \return OOIF return code
 */
OOIFReturnCode native_VodGetSupportedStrategies(VodHandle id, int *count, char ***strategies);

/** This function corresponds to the OIPF function getAvailablePlayTime()
    as defined in DAE 7.14.8.2
    \param id Vod instance identifier
    \param fromPlayPosition as defined in DAE 7.14.8.2
    \param retval Return value as defined in OIPF 7.14.8.2
    \return OOIF return code
*/
OOIFReturnCode native_VodGetAvailablePlayTime(VodHandle id, OOIFBoolean fromPlayPosition, OOIFNumber *retval);

/** This function corresponds to the OIPF function setBufferingStrategy()
    as defined in DAE 7.14.8.2
    \param id Vod instance identifier
    \param name as defined in DAE 7.14.8.2
    \param retval Return value as defined in DAE 7.14.8.2
    \return OOIF return code
*/
OOIFReturnCode native_VodSetBufferingStrategy(VodHandle id, const char *name, OOIFBoolean *retval);

/** This function corresponds to the OIPF function setRepresentationStrategy()
    as defined in DAE 7.14.8.2
    \param id Vod instance identifier
    \param maxBandwidth as defined in DAE 7.14.8.2. In case JavaScript value was undefined NaN will be used.
    \param minBandwidth as defined in DAE 7.14.8.2. In case JavaScript value was undefined NaN will be used.
    \param position as defined in DAE 7.14.8.2. In case JavaScript value was not provided NaN will be used.
    \param retval Return value as defined in DAE 7.14.8.2
    \return OOIF return code
*/
OOIFReturnCode native_VodSetRepresentationStrategy(VodHandle id, OOIFNumber maxBandwidth, OOIFNumber minBandwidth, OOIFNumber position, OOIFBoolean* retval);
#endif /* OIPF */

/**
   Updates visibility of the video by start/stop drawing the video on the screen.
   \param id VOD instance identifier
   \param visibility TRUE if video should be drawn, FALSE if not
   \param unloaded TRUE if the video object is being removed from the
   display due to the document being unloaded (browser navigating to
   another page). Relevant only if visibility is FALSE.

   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_VodSetVisibility(VodHandle id, OOIFBoolean visibility, OOIFBoolean unloaded);

/**
   Update video position.
   \param id VOD instance identifier
   \param x X-position of the video window
   \param y Y-position of the video window
   \param width width of the video window
   \param height height of the video window
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_VodSetVideoWindow(VodHandle id, int x, int y, int width, int height);

/**
   This function corresponds to the loop parameter of the &lt;param&gt; element, as
   defined in DAE 7.14.10.1.

   \param id Vod instance identifier
   \param value the received value of the loop parameter. Can be a
   string representation of the allowed values according to DAE1.1
   7.14.10.1, or NULL if the param with name="loop" removed or the
   name attribute was updated
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_VodSetLoop(VodHandle id, const char* value);

/**
   This function corresponds to the cache parameter of the &lt;param&gt; element, as
   defined in DAE 7.14.10.1.

   \param id Vod instance identifier
   \param value the received value of the loop parameter. Can be a
   string representation of the allowed values according to DAE1.1
   7.14.10.1, or NULL if the param with name="cache" removed or the
   name attribute was updated
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_VodSetCache(VodHandle id, const char* value);


#if defined(HMX_VOD_PDL)
/**
	Contains the percentage of buffering completed
 */
OOIFReturnCode native_VodGetBufferingProgress(VodHandle id, OOIFNumber *retval);

/**
	Retrieves the buffering time, which is the time allocated
	for buffering incoming data before playback begins
 */
OOIFReturnCode native_VodGetBufferingTime(VodHandle id, OOIFNumber *retval);

/**
	Returns a list of chunks, i.e.  parts of the content that
	have already been downloaded to local storage, as a string in JSON notation.

	example.
	The PdlChunks property contains the following string:

	"{pdlActive: true, chunks: [ {start: 0, end: 20}, {start: 32, end: 38},
	{start: 2932, end: 3466} ]}"

	When parsed through eval() in JavaScript, the resulting object.
	s chunks property contains an array of three objects.
	These objects indicate that the content section from 0 seconds to 20 seconds,
	another section from 32 seconds to 38 seconds and a third section from 2932 seconds
	to 3466 seconds have already been downloaded.
 */
OOIFReturnCode native_VodGetPdlChunks(VodHandle id, const char **retval);

/**
	Contains the state of readiness of the player control.

	ReadyState 31 = the player is loading a file
	ReadyState 33 = the player has downloaded enough data to play the file,
	but has not yet received all data
	ReadyState 34 = All data has been downloaded
 */
OOIFReturnCode native_VodGetReadyState(VodHandle id, OOIFNumber *retval);

void native_send_videoondemand_readystate_change_event(VodHandle id);
#endif

/**
   \return OOIF return code
 */
OOIFReturnCode native_VodSeekSegment(VodHandle id, OOIFNumber pos_segment, OOIFBoolean *retval);

#if defined(MAXDOME)
/**
   Gets the streamed A/V content property errorDetails as defined in
   Maxdome_TVClient_Integration_Specification_v.1.00
   \param id VOD instance identifier
   \param retval Retval as defined in Maxdome_TVClient_Integration_Specification_v.1.00
   \return OOIF return code
*/
OOIFReturnCode native_VodGetErrorDetails(VodHandle id, OOIFNumber *retval);
#endif

/*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_VIDEO_ON_DEMAND */
