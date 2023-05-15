/*
 * Copyright (c) 2011 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */

#ifndef ITK_DSMCC_H__
#define ITK_DSMCC_H__

/**
 * @file 
 * DSMCC Interface.
 */
#include "itk_os.h"

struct itk_dsmcc_object_t;
struct itk_dsmcc_session_t;

/**Object type. 
 */
typedef enum itk_dsmcc_object_type_t
{
    ITK_DSMCC_DIRECTORY=1,              /**< Directory object */
    ITK_DSMCC_FILE,                     /**< File object */
    ITK_DSMCC_STREAM                    /**< Stream object */
} itk_dsmcc_object_type_t;

/**Service context list, as defined in table B.19 of ETSI TS 102809 B.2.3.5
 */
typedef struct itk_dsmcc_sclist_t
{
    itk_uint16_t count;                 /**< Service context lists count */
    void *data;                         /**< Pointer to the service context list data */
} itk_dsmcc_sclist_t;

/**Directory entry
 */
typedef struct itk_dsmcc_dir_entry_t
{
    char *name;                         /**< entry name relative to parent directory */
    itk_dsmcc_object_type_t type;       /**< entry type */
} itk_dsmcc_dir_entry_t;

/**Directory object
 */
typedef struct itk_dsmcc_dir_t
{
    itk_uint16_t numEntries;            /**< number of entries in the dir */
    itk_dsmcc_dir_entry_t *entries;     /**< array of size numEntries */
    itk_dsmcc_sclist_t svcCtxtList;     /**< service context list of directory  (e.g. srg info) */
} itk_dsmcc_dir_t;

/**File object
 */
typedef struct itk_dsmcc_fil_t
{
    itk_uint32_t size;                  /**< number of total bytes in the file */
    itk_uint32_t offset;                /**< number of bytes offset into file */
    itk_uint32_t length;                /**< number of bytes of data */
    void *data;                         /**< the file data */
} itk_dsmcc_fil_t;    

/**Stream event
 */
typedef struct itk_dsmcc_str_event_t
{
    char *name;                         /**< name/data associtaed with event */
    itk_uint16_t id;                    /**< event id */
} itk_dsmcc_str_event_t;
    
/**Stream object
 */
typedef struct itk_dsmcc_str_t
{
    itk_uint16_t onid;                  /**< org network id of stream reference */
    itk_uint16_t tsid;                  /**< transport stream id of stream reference */
    itk_uint16_t sid;                   /**< stream id stream reference */
    itk_uint16_t numEvents;             /**< number of stream events associated with object */
    itk_dsmcc_str_event_t *events;      /**< the stream events */
} itk_dsmcc_str_t;

/**Generic object data
 */
typedef struct itk_dsmcc_object_data_t
{
    itk_errcode_t errcode;              /**< result */
    itk_dsmcc_object_type_t type;       /**< type of payload below */
    union
    {
        itk_dsmcc_fil_t file;             
        itk_dsmcc_dir_t directory;
        itk_dsmcc_str_t stream;
    }object;
} itk_dsmcc_object_data_t;

/**Object load callback
 */
typedef void (*itk_dsmcc_object_callback_t)(void *context, struct itk_dsmcc_object_t *object, itk_dsmcc_object_data_t *objdata);

/**Stream event callback 
 */
typedef void (*itk_dsmcc_ste_callback_t)(void *context, struct itk_dsmcc_object_t *object,
                                         itk_uint16_t eventId, itk_uint16_t version, itk_uint32_t privateDataLen, void const *privateData);

/**Object jump table
 */
typedef struct itk_dsmcc_object_t
{
    /**Returns the object's name.
     * @param[in] thiz      The object interface.
     */
    char const * (*name)(struct itk_dsmcc_object_t *thiz);
    
    /**Returns the object's type.
     * @param[in] thiz      The object interface.
     */
    itk_dsmcc_object_type_t (*type)(struct itk_dsmcc_object_t *thiz);

    /**Requests the object begins loading.
     * @param[in] thiz      The object interface.
     */
    itk_errcode_t (*load)(struct itk_dsmcc_object_t *thiz /*TBD load options*/);

    /**Registers a callback function to receive subscribed event notifications.
     * @param[in] thiz      The object interface.
     * @param[in] fn        A function to invoke when event is received.
     * @param[in] context   A context to return in the callback function.
     */
    itk_errcode_t (*registerStreamEventCallback)(struct itk_dsmcc_object_t *thiz, itk_dsmcc_ste_callback_t fn, void *context);

    /**Subscribes to events assoicated with a stream object. 
     * @param[in] thiz      The object interface.
     * @param[in] id        The id of the event.
     */
    itk_errcode_t (*subscribeStreamEvent)(struct itk_dsmcc_object_t *thiz, itk_uint16_t id);

    /**Unsubscribes from events associated with a stream object. 
     * @param[in] thiz      The object interface.
     * @param[in] thiz      The id of the event.
     */
    itk_errcode_t (*unsubscribeStreamEvent)(struct itk_dsmcc_object_t *thiz, itk_uint16_t id);

    /**Releases the object interface 
     * @param[in] thiz      The object interface.
     */
    void (*release)(struct itk_dsmcc_object_t *thiz);
} itk_dsmcc_object_t;

/**Session events
 */
typedef enum itk_dsmcc_session_event_t 
{
    ITK_DSMCC_SESSION_ATTACH,           /**< A session attach has completed */
    ITK_DSMCC_SESSION_DETACH,         	/**< A session detach has completed */
    ITK_DSMCC_SESSION_SERVICE_BOOT      /**< A service boot event has occured */
} itk_dsmcc_session_event_t;

/**Container for dsmcc section
 */
typedef struct itk_dsmcc_section_t
{
    itk_uint32_t size;                  /**< length of section buffer */
    void const *data;                         /**< pointer to dsmcc section data */
} itk_dsmcc_section_t;

typedef struct itk_dsmcc_session_data_t
{
    itk_dsmcc_session_event_t type;     /**< type of session data */
    itk_errcode_t result;               /**< result */
    union
    {
        itk_dsmcc_section_t attach;
        itk_errcode_t detach;
        itk_errcode_t service;
    }session;
} itk_dsmcc_session_data_t;

/** DSM-CC NSAP
 */
typedef struct itk_dsmcc_nsap_t
{
    itk_uint32_t carouselId;            /**< carousel id as specified in carousel id descriptor of PMT ES loop */
    itk_uint16_t networkId;             /**< dvb network id of service carrying carousel */
    itk_uint16_t originalNetworkId;     /**< dvb original network id of service carrying carousel */
    itk_uint16_t transportStreamId;     /**< dvb transport stream id of service carrying carousel */
    itk_uint16_t serviceId;             /**< dvb service id of service carrying carousel */
} itk_dsmcc_nsap_t;

/**Session callback
 */
typedef void (*itk_dsmcc_session_callback_t)(void *context, struct itk_dsmcc_session_t *session, itk_dsmcc_session_data_t *sesdata);

/**Service boot callback
 */
typedef void (*itk_dsmcc_service_boot_callback_t)(void *context, struct itk_dsmcc_session_t *session);
/**Session object jump table
 */
typedef struct itk_dsmcc_session_t
{
    /**Attempts to mount a DSM-CC object carousel
     * @param[in] thiz      The session interface.
     * @param[in] nsap      The identifier for the carousel.
     */
    itk_errcode_t (*attach)(struct itk_dsmcc_session_t *thiz, itk_dsmcc_nsap_t nsap);

    /**Detches from the currently mounted object carousel and unsubscribes the service boot
     * @param[in] thiz      The session interface.
     */
    itk_errcode_t (*detach)(struct itk_dsmcc_session_t *thiz);

    /**Subscribes to the service boot event 
     * @param[in] thiz      The session interface.
     * @param[in] tag       The stream id tag of the service boot ES.
     */
    itk_errcode_t (*subscribeServiceBoot)(struct itk_dsmcc_session_t *thiz, itk_uint16_t tag, 
        itk_dsmcc_service_boot_callback_t fn, void *context);


    /**Creates a new itk_dsmcc_object_t
     * @param[in] thiz      The session interface.
     * @param[in] name      Fully qualified object name.
     * @param[in] type      The object type.
     * @param[in] fn        A callback function invoked when the object loads.
     * @param[in] context   A context to return in the callback function.
     */
    itk_dsmcc_object_t *(*newObject)(struct itk_dsmcc_session_t *thiz, const char *name, itk_dsmcc_object_type_t type, 
        itk_dsmcc_object_callback_t fn, void *context);
    
    /**Releases session
     * @param[in] thiz      The session interface.
     */
    void (*release)(struct itk_dsmcc_session_t *thiz);
} itk_dsmcc_session_t;

/**Create a new session.
 * @param[in] heap      Heap for the download session. See User Guide for size recommendation.
 * @param[in] fn        Session callback function.
 * @param[in] context   Session context that will be passed to callback function.
 */
ITK_EXPORT itk_dsmcc_session_t *ITK_newDsmccSession(itk_heap_t *heap, /*TBD TYPE?*/ itk_dsmcc_session_callback_t fn, void *context);

/**Initialise the DSM-CC
 * @param[in] heap   The heap to use. See User Guide for size recommendation.
 */
ITK_EXPORT itk_errcode_t ITK_initDsmcc(itk_heap_t *heap);

/**Terminate the DSM-CC
 */
ITK_EXPORT void ITK_termDsmcc(void);

#endif /*ITK_DSMCC_H__*/
