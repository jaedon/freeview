/*
 * Copyright (c) 2007 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_SERVICE_H__
#define ITK_SERVICE_H__

#include "itk_types.h"
#include "itk_eits.h"

/**
 * @file 
 * Service Manager
 */

/**
 * @ingroup host
 * @defgroup service Service Manager
 * @{
 */

/**
 * The maximum length for a string in this API
 */
#define ITK_SERVICE_INFO_MAX_STR 256

struct itk_service_t;


/**
 * Flags for configuring the behaviour of service changes
 * 
 * Note: The flags below define options that can be combined by a logical OR operation.
 */
typedef enum itk_service_tune_flag_t
{
    ITK_TUNE_KEEP_CONTEXT = 1 << 0, /**< Tune without changing the viewer's perceived context (i.e. current channel) */
    ITK_TUNE_KEEP_DISPLAY = 1 << 1, /**< Do not clear the display during the retune */
    ITK_TUNE_NO_BANNER    = 1 << 2, /**< Do not display a retune banner when the new channel is selected */
    ITK_TUNE_KEEP_BROADCAST_INTERRUPT = 1 << 3 /**< Do not reset the broadcast interruption settings */
} itk_service_tune_flag_t;

/**
 * Switch for locator format
 */
typedef enum itk_service_locator_type_t
{
    ITK_LOCATOR_BCAST = 0, /**< channel is a broadcast service */
    ITK_LOCATOR_HOST       /**< channel is a host defined type */
} itk_service_locator_type_t;

/**
 * Switch for event information format
 */
typedef enum itk_service_info_type_t
{
    ITK_SERVICE_EV_EIT = 0, /**< event info is a DVB EIT event */
    ITK_SERVICE_EV_HOST     /**< event info is host-defined event */
} itk_service_info_type_t;

/**
 * Service location structure
 *
 * This structure is returned from getServiceLocator.
 * The format depends on the service type. For broadcast
 * services the onid,nid,tsid and sid are returned. Note
 * that all of these values are required to be returned.
 */
typedef struct itk_service_locator_t
{
    /**
     * Service type
     * Specifies the format of the following locator
     */
    itk_service_locator_type_t type;
    /**
     * Service locator
     */
    union locator
    {
        /**
         * Broadcast stream info
         * For type = ITK_SERVICE_BCAST
         */
        itk_nsap_t bcast;
        /* others TBD */
    } locator;
} itk_service_locator_t;

/**
 * Service information structure
 *
 * This structure is returned from getServiceInfo.
 */
typedef struct itk_service_info_t
{
    /**
     * Service name (for DVB this is from the service_descriptor)
     * See @ref strfmt for details of the format of the string
     */
    char name[ITK_SERVICE_INFO_MAX_STR];
    /**
     * Service provider from the service_descriptor
     * See @ref strfmt for details of the format of the string
     */
    char provider[ITK_SERVICE_INFO_MAX_STR];
    /**
     * Service type (from a DVB service_descriptor) 
     */
    itk_uint8_t type;       
    /**
     * Service information from the PMT, 
     * ITK_PRESENT if the service is signalled with subtitles
     */
    itk_present_t subtitles;
    /**
     * Channel number.
     * The number assigned to a service for display purposes. Note this
     * may be the LCN or alternate LCN or some other device defined values.
     * Only required for ZA profile.
     */
    itk_uint32_t channel_number;
    /**
     * Service running status
     * The running status of the service as indicated by the running_status field 
     * in the SDT. See EN300468, 5.2.3 Table 6.
     * Only required for ZA profile.
     */
    itk_uint8_t running_status;   

} itk_service_info_t;


/**
 * Service event (programme) information
 */
typedef struct itk_service_event_t
{
    /**
     * Indicates the version of the event data that this event was derived from.
     * For EIT events this is obtained from the EIT section. 
     */
    itk_uint32_t version;
    /**
     * Event type
     * Specifies the format of event_info
     */
    itk_service_info_type_t type;
    /**
     * Service event information
     */
    union event_info
    {
        /**
         * DVB EIT-derived data for type == SERVICE_EV_EIT
         */
        struct eit
        {
            /**
             * Name from short_event_descriptor 
             * The string is empty if the short_event_descriptor is not present
             * See @ref strfmt for details of the format of the string
             */
            char event_name[ITK_SERVICE_INFO_MAX_STR];
            /**
             * Short description from short_event_descriptor 
             * The string is empty if the short_event_descriptor is not present
             * See @ref strfmt for details of the format of the string
             */
            char short_desc[ITK_SERVICE_INFO_MAX_STR];
            /**
             * Available Audio languages from component_descriptor
             * The string is empty if the component_descriptor is not present.
             * The string can contain any number of language codes that are seperated 
             * by a space and must be terminated by a '\0'.
             */
            char audio_langs[ITK_SERVICE_INFO_MAX_STR];
            /**
             * Available subtitle languages from component_descriptor
             * The string is empty if the component_descriptor is not present.
             * The string can contain any number of language codes that are seperated 
             * by a space and must be terminated by a '\0'.
             */
            char subttl_langs[ITK_SERVICE_INFO_MAX_STR];
            /**
             * Content type from a content_descriptor. Up to 4 categories may be stored,
             * with only the DVB (non-broadcaster) nibbles considered. The categories are
             * packed from LSB to MSB  - pad with zeros 
             * e.g. 0x00000010, 0x00001011
             */
            itk_uint32_t content_type; 
            /**
             * Start date in MJD 
             */
            itk_int32_t date;
            /**
             * Start time in seconds from midnight (UTC + local offset)
             */
            itk_int32_t start;
            /**
             * Duration in seconds 
             */
            itk_int32_t duration;
            /** 
             * ITK_PRESENT if the free_CA_mode flag for the EIT event is 1
             */
            itk_present_t encrypted;   
            /**
             * Age rating from the parental_rating_descriptor, 0 if not present 
             */
            itk_uint8_t rating;         
        } eit;
        /* other types */
    } event_info;
} itk_service_event_t;

/**
 * Service change event types
 */
typedef enum itk_service_tune_event_t
{
    ITK_SERVICE_TUNE_START = 0,             /**< Service has started */
    ITK_SERVICE_TUNE_STOP_HOST,             /**< Service is ending by non-iTK request */
    ITK_SERVICE_TUNE_STOP_ITK,              /**< Service is ending by iTK-initiated request */
    ITK_SERVICE_TUNE_STOP_HOST_KEEP_ALIVE   /**< Host-triggered NDT */
} itk_service_tune_event_t;

/**
 * Service event listener prototype
 */
typedef void (*itk_service_event_callback_t)(void *context, struct itk_service_t *service, itk_service_tune_event_t ev);

/**
 * The service manager jump table
 */
typedef struct itk_service_t
{
    /**
     * Adds a callback function to the service manager
     *
     * @note There is at most one callback required
     *
     * @param[in] thiz The service manager
     * @param[in] callback A pointer to the callback function
     * @param[in] context Context data to provide to the callback when it is invoked
     *
     * @return ITKE_OK if the callback is registered
     * @return ITKE_NO_RESOURCE if there is already a callback registered
     */
    itk_errcode_t (*addEventCallback)(struct itk_service_t *thiz, 
                                      itk_service_event_callback_t callback, 
                                      void *context);

    /**
     * Removes an event callback function from the service manager
     *
     * @param[in] thiz The service manager
     */
    void (*removeEventCallback)(struct itk_service_t *thiz);

    /**
     * Looks up a channel reference in the host's channel table
     *
     * The reference format may depend on the service type. The returned
     * value is unique for the service and is 0 or greater. If the service
     * is not visible then the value -1 is returned. For a list of locator
     * formats see @ref reference_fmt
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] service service reference string.
     *
     * @return A positive index (including 0) for the service
     * @return -1 if the service reference can not be seen in the database
     */
    itk_int32_t (*findService)(struct itk_service_t *thiz, const char *service);
    
    /**
     * Returns locator information for the service
     *
     * The service is referenced by an index value obtained from a call to findService.
     * The format of the returned data depends on the type of service.
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The service index
     * @param[out] loc A pointer to a itk_service_locator_t structure to fill
     *
     * @return ITKE_OK
     * @return ITKE_NOT_FOUND if the index is not valid
     */
    itk_errcode_t (*getServiceLocator)(struct itk_service_t *thiz, itk_int32_t index, itk_service_locator_t *loc);

    /**
     * Returns service-level information
     *
     * Service level information includes the service name, provider name etc.
     * For DVB this is obtained from a service_descriptor
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The service index
     * @param[out] info A pointer to a itk_service_info_t structure to fill
     *
     * @return ITKE_OK
     * @return ITKE_NOT_FOUND if the index is not valid
     */
    itk_errcode_t (*getServiceInfo)(struct itk_service_t *thiz, itk_int32_t index, itk_service_info_t *info);

    /**
     * Obtains information on events available on the service.
     *
     * The event in question is defined by an eventIndex. Index = 0 indicates the current
     * event, index = 1 indicates the following event, and so on. The call is synchronous and
     * shall return an ITKE_NOT_FOUND value if the requested event is not yet available.
     *
     * @note For DVB EIT-based services it is expected that only EITp/f is monitored - i.e. the
     * maximum value of eventIndex required is 1. Support for additional values is optional.
     *
     * @note There is no requirement to ensure that event info is obtained from a coherent event
     * database. If the event data increments between two calls then the version returned must
     * be changed in line with the version of the data. If coherence is required by the client
     * then this will be managed above the API.
     *
     * @param[in]  thiz A pointer to a service database
     * @param[in]  index The service index
     * @param[in]  eventIndex A 0 based index for the event
     * @param[out] info A pointer to a itk_service_event_info_t structure to fill
     *
     * @return ITKE_OK if the event was found and populated
     * @return ITKE_NOT_FOUND if the index is not valid
     */
    itk_errcode_t (*getServiceEventInfo)(struct itk_service_t *thiz, 
                                         itk_int32_t index,
                                         itk_uint16_t eventIndex,
                                         itk_service_event_t *info);

    /**
     * Requests that the host retune to a new service
     *
     * The call is asynchronous. The call must return to allow the client 
     * to process any commands from the host as a result of the service change.
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The service index
     * @param[in] flags A set of flags to indicate how to perform the retune
     *
     * @return ITKE_OK
     * @return ITKE_NOT_FOUND if the index is not valid
     */
    itk_errcode_t (*startRetune)(struct itk_service_t *thiz, itk_int32_t index, itk_service_tune_flag_t flags);
   
    /**
     * Checks if non-destructive tune is allowed between two services.
     *
     * The means of determining if there is a policy to allow non-destructive tune
     * usage is not defined and depends on the target platform.
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index1 The initial service 
     * @param[in] index2 The target service
     *
     * @return ITKE_OK if non-destructive tune is allowed.
     * @return ITKE_NOT_SUPPORTED if non-destructive tune is not allowed.
     */
    itk_errcode_t (*checkNonDestructiveTuneAllowed)(struct itk_service_t *thiz, itk_int32_t index1, itk_int32_t index2);

    /**
     * Checks if a service has an Interactive Channel policy enabled
     *
     * The means of determining if there is a policy to allow Interaction Channel
     * usage is not defined and depends on the target platform.
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The index of the service in question
     *
     * @return ITKE_OK if the requested service has an Interaction Channel policy enabled
     * @return ITKE_NOT_SUPPORTED if Interaction Channel features are not enabled for the service
     */
    itk_errcode_t (*checkMhegICEnabled)(struct itk_service_t *thiz, itk_int32_t index);

    /**
     * Checks if a service has MHEG functionality enabled
     *
     * The means of determining if there is mechanism to disable use of the
     * MHEG functionality features is not defined and depends on the target
     * platform. Basic MHEG functionality may not imply that all features are enabled,
     * only that an application can be executed from the service.
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The index of the service in question
     *
     * @return ITKE_OK if the requested service has MHEG interactive features enabled
     * @return ITKE_NOT_SUPPORTED if MHEG interactive features are not enabled for the service
     */
    itk_errcode_t (*checkMhegEnabled)(struct itk_service_t *thiz, itk_int32_t index);

    /**
     * Obtain the Freesat service identifier for the current service.
     * @param[in] thiz A pointer to a service database.
     * @param[in] service_index
     * @return The 15 bit freesat service identifier, on success, -1 otherwise.
     * @remark This identifier is carried in the
     * Regionalised logical channel descriptor, which itself is in the BAT.
     */
    itk_int32_t (*getFreeSatServiceIdentifier)(struct itk_service_t * thiz,
                                               itk_int32_t service_index);

    /**
     * Checks if a Freesat service is a member of a service group.
     * @param[in] thiz A pointer to a service database.
     * @param[in] freesat_service_index
     * @param[in] freesat_service_service_group
     * @return ITKE_OK if the service is a member of the service group,
     * ITKE_FAIL otherwise.
     * @remark The Free satellite servicegroup descriptor, carried in BAT,
     * defines the Freesat service groups.
     */
    itk_errcode_t (*checkGroupMembership)(struct itk_service_t *thiz,
                                          itk_uint16_t freesat_service_index,
                                          itk_uint16_t freesat_service_group);

    /**
     * Obtain the non-volatile storage allocation, in bytes, for a Freesat
     * service.
     * @param[in] thiz A pointer to a service database.
     * @param[in] freesat_service_index
     * @return The storage allocation (which is zero if no storage has been
     * allocated for this service).
     * @remark The Freesat interactive storage descriptor in the BAT defines
     * storage allocations for Freesat services and service groups.
     */
    size_t (*getServiceNvStorageAllocation)(struct itk_service_t * thiz,
                                            itk_uint16_t freesat_service_index);

    /**
     * Obtain the non-volatile storage allocation, in bytes, for a Freesat
     * service group.
     * @param[in] thiz A pointer to a service database.
     * @param[in] freesat_service_service_group
     * @return The storage allocation (which is zero if no storage has been
     * allocated for this service group).
     * @remark The Freesat interactive storage descriptor in the BAT defines
     * storage allocations for Freesat services and service groups.
     */
    size_t (*getGroupNvStorageAllocation)(struct itk_service_t * thiz,
                                          itk_uint16_t freesat_service_group);

    /**
     * Enable/Disable the presentation of the Broadcast Triggered Native Applications 
     * (i.e. Native UI applications showing related contents avilable for recording)
     * while the MHEG application is running. This setting can persist across application 
     * initiated service tune if ITK_TUNE_KEEP_BROADCAST_INTERRUPT flag is set in 
     * itk_service::startRetune().
     *
     * @param[in] thiz The host
     * @param[in] control Enable/Disable the presentation of broadcast triggered native applications.
     */
    void (*setBroadcastInterruption)(struct itk_service_t *thiz, const itk_control_t control);

    /**
     * Obtain the location string identifying the broadcast location for Australia's VAST profile.
     *
     * @param[in]  thiz The host
     * @param[out] loc  A pointer to a buffer to fill with the location string, or NULL bytes.
     * @param[in]  loc_size The size of the loc buffer, in bytes.
     * @remark The location string is expected to be a 3 character string equivalent to the 
     * country_code in the country_availability_descriptor defined by [EN300468] of the 
     * BAT carrying services for the location. Note the country_code in the BAT is coded into 
     * 24-bits in accordance to ISO/IEC 8859-1. For example, "NSW" is coded as 
     * "10011100101001101010111" or 5133143.
     */
    void (*getBroadcastLocation)(struct itk_service_t *thiz, itk_uint8_t *loc, itk_uint32_t loc_size);

    /**
     * Obtain the number of services.
     *
     * @param[in] thiz A pointer to a service database.
     *
     * @return The total number of services in the database.
     */
    itk_int32_t (*getServiceCount)(struct itk_service_t * thiz);

    /**
     * Returns the next visible channel reference in the host's channel table
     *
     * @param[in] thiz A pointer to a service database
     * @param[in] index The index of the service to offset from.
     * @param[in] updown The direction to offset the next service index.  
     *
     * @return A positive index (including 0) for the service.
     * @return -1 When reaching the beginning or end of the hosts channel table. 
     * 
     * @remark Valid values of updown parameter include 1 for up and -1 for down. All other 
     * values are undefined.
     */
    itk_int32_t (*getNextServiceIndex)(struct itk_service_t *thiz, itk_int32_t index, itk_int8_t updown);

    /**
     * Creates a scheduled event info iterator and sets it to the service date and time.
     *
     * @param[in]  thiz The host
     * @param[in]  index The service carrying the event.
     * @param[in]  date The date date in MJD on which the event occurs.
     * @param[in]  time The time in seconds from midnight (UTC + local offset) at which the event is scheduled.
     *
     * The scheduled event info iterator allows the client to query EITs. When the iterator is created
     * it is given a service, date and time. The iterator shall be initialised to reference event 
     * occuring on the given service and at the given date and time. Subsequent calls to the iterator 
     * return information about that event until the iterator is released or advanced with a call to 
     * itk_eits_iterator_t::nextEvent().
     *
     * @remark The time given may not be the start time of the event, rather it will be a time at which
     * the event is scheduled according to the start_time and duration in the EITs.
     *
     */
    itk_eits_iterator_t *(*newEitsIterator)(struct itk_service_t *thiz, itk_int32_t index, itk_int32_t date, itk_int32_t start);

    /** Returns the event id of the present or following event on a given service index.
     *  The event in question is defined by an eventIndex. Index = 0 indicates the current
     *  event, index = 1 indicates the following event.
     *  @param[in]  thiz The host.
     *  @param[in]  index The service index.
     *  @param[in]  eventIndex Identifies present or following
     *  @param[out] eventId Reference to variable to store the eventId.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     */
    itk_validation_t (*getEventId)( struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_uint16_t *eventId );
    
    /** Returns the number of component descriptors signalled in the present or following event for a given service.
     *  The event in question is defined by an eventIndex. Index = 0 indicates the current
     *  event, index = 1 indicates the following event.
     *  @param[in]  thiz The host.
     *  @param[in]  index The service index.
     *  @param[in]  eventIndex Identifies present or following
     *  @return     The number of descriptors.
     *
     *  @remark See EN300468. The data returned is extracted from Component Descriptors as defined in clause 6.2.8.
     */
    itk_uint16_t (*getEitpfNumComponentDescs)( struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex);

    /** Returns space separated lists of char hex values in ascii representing the values of stream_content 
     *  and component_type for each component signalled in the present or following event for a given service.
     *  The event in question is defined by an eventIndex. Index = 0 indicates the current
     *  event, index = 1 indicates the following event.
     *  @param[in]  thiz The host.
     *  @param[in]  index The service index.
     *  @param[in]  eventIndex Identifies present or following.
     *  @param[out] stream_content A pointer to store the stream_content list.
     *  @param[in]  scLength The length of stream content buffer.
     *  @param[out] component_type A pointer to store the component type list.
     *  @param[in]  ctLength The length of component type buffer.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     *
     *  The function returns two lists extracted from Component Descriptors as defined in EN300468 clause 6.2.8. 
     *  The stream content list contains a space separated list of 1 char hex values in ascii representing the 
     *  values of stream_content for each component of the event. The component_type list contains a space 
     *  separated list of 2 char hex values in ascii representing the values of component_type for each 
     *  component of the event.
     *
     *  @remark See EN300468. The data returned is extracted from Component Descriptors as defined in clause 6.2.8.
     */
    itk_validation_t (*getEitpfComponentDescs)( struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, 
                                char *stream_content, itk_uint16_t scLength,
                                char *component_type, itk_uint16_t ctLength);

    /** Returns if a given component is signalled for the present or following event for a given service.
     *  The event in question is defined by an eventIndex. Index = 0 indicates the current
     *  event, index = 1 indicates the following event.
     *  @param[in]  thiz The host.
     *  @param[in]  index The service index.
     *  @param[in]  eventIndex Identifies present or following.
     *  @return     The result of the operation. ITK_PRESENT if the component is signalled, ITK_NOT_PRESENT if not.
     *
     *  @remark See EN300468, Clause 6.2.8 Component Descriptors.
     */
    itk_present_t (*getEitpfComponentInfo)( struct itk_service_t *thiz, itk_int32_t index,
        itk_uint16_t eventIndex, itk_component_t component);

} itk_service_t;

/** 
 * @}
 */
#endif
