/*
 * Copyright (c) 2008 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */

#if !defined ITK_APPMMI_H__
#define ITK_APPMMI_H__
/**
 * @file
 * CI AppMMI (Common Interface Application MMI) integration
 */
#include "itk_types.h"

typedef enum itk_appmmi_status_t
{
    ITK_APPMMI_STATUS_OK = 1,       /**< The operation was handled */
    ITK_APPMMI_STATUS_DOMAIN,       /**< The domain is not supported */
    ITK_APPMMI_STATUS_BUSY,         /**< The receiver is busy */
    ITK_APPMMI_STATUS_NOT_AVAILABLE /**< No AppMMI session is avalable */
} itk_appmmi_status_t;


/** iTK Application MMI session messages */
typedef enum itk_appmmi_message_t
{
    ITK_APPMMI_START,
    ITK_APPMMI_ABORT,
    ITK_APPMMI_FILE_REQUEST,
    ITK_APPMMI_FILE_ACK
} itk_appmmi_message_t;

/** Start data */
typedef struct itk_appmmi_req_start_t
{
    size_t                      appDomainIdentifierLength;
    const itk_uint8_t           *appDomainIdentifier;
    size_t                      initialObjectLength;
    const itk_uint8_t           *initialObject;
} itk_appmmi_req_start_t;

/** File requests and file acknowledge */
typedef struct itk_appmmi_file_req_ack_t
{
    itk_uint8_t                 requestType;        /**< 0x00 for File, 0x01 for Data, 0x02 for FileHash */
    itk_uint32_t                length;             /**< FileName Length or Data Length depending on type */
    const itk_uint8_t           *byte;              /**< FileName Bytes or Data Bytes depending on type */
} itk_appmmi_file_req_ack_t;

/** File Acknowledge */
typedef struct itk_appmmi_file_ack_t
{
    itk_uint8_t                 fileOK;
    itk_uint8_t                 requestOK;          /**< AppMMI v2 only */
    itk_appmmi_file_req_ack_t   data;
} itk_appmmi_file_ack_t;

/** App MMI Message */
typedef union itk_appmmi_msg_t
{
    itk_appmmi_req_start_t      reqStart;
    itk_appmmi_file_req_ack_t   fileReq;
    itk_appmmi_file_ack_t       fileAck;
} itk_appmmi_msg_t;

/**
 *
 * @param[in] context   The callback context passed to itk_appmmi_t::newAppMMISession()
 * @param[in] type      The message type ITK_APPMMI_START, ITK_APPMMI_ABORT or ITK_APPMMI_FILE_ACK
 * @param[in] msg       The message
 * @return ITK_APPMMI_STATUS_OK     If the message sent ok.
 * @return ITK_APPMMI_STATUS_DOMAIN If the app domain identifier in a start message is unsupported.
 */
typedef itk_appmmi_status_t (*itk_appmmi_callback_t)(void *context, itk_appmmi_message_t type, itk_appmmi_msg_t msg);

/**
 * AppMMI session interface.
 * The interface is provided by the host when the client requests a new session.
 */
typedef struct itk_appmmi_session_t
{
    /** 
     * Release session 
     */
    void (*release)(struct itk_appmmi_session_t *thiz);

    /**
     * Sends a file request or a CI message.
     *
     * The call creates either a FileRequest or CI message. The request provides
     * either the FileNameByte data (and optionally the hash) or the CI message data.
     * AppMMI is not able to handle multiple simultaneous requests.
     * If a request is made before the previous one has been handled, the
     * call returns ITK_APPMMI_STATUS_BUSY.
     *
     * @param[in] thiz The appmmi session
     * @param[in] type The message type, ITK_APPMMI_FILE_REQUEST or ITK_APPMMI_ABORT
     * @param[in] msg  The file request.
     *
     * @return ITK_APPMMI_STATUS_OK if the request is sent
     * @return ITK_APPMMI_STATUS_BUSY if there is a pending request
     * @return ITK_APPMMI_STATUS_NOT_AVAILABLE if there is no session to
     * handle the request, i.e. no CAM inserted.
     */
    itk_appmmi_status_t (*sendMessage)(struct itk_appmmi_session_t *thiz,
        itk_appmmi_message_t type, itk_appmmi_msg_t msg);
} itk_appmmi_session_t;

/**
 * Application MMI interface.
 * Registered with itk_manager_t type ITK_APPMMI.
 */
typedef struct itk_appmmi_t
{
    /**
     * Create a new Application MMI Session.
     * @return A session interface or NULL if cannot create.
     */
    itk_appmmi_session_t *(*newAppMMISession)(struct itk_appmmi_t *thiz, 
                itk_appmmi_callback_t callback, void *callbackContext);

    /**
     * Returns the API version that the session supports.
     *
     * The call queries the version of the AppMMI Resource that is
     * supported. CI+ specification 1.3 introduces AppMMI Resource v2,
     * which includes the facility to check if cached content is up to
     * date using a new RequestType to verify the MD5 hash of a file's
     * content.
     * If AppMMI Resource v2 is supported, the client may use
     * ITK_APPMMI_FILE_REQUEST messages with requestType value of 0x02 (FileHash). 
     * The corresponding ITK_APPMMI_FILE_REQUEST_ACK shall set the RequestOK
     * bit appropriately.
     *
     * @param[in] thiz The appmmi session
     *
     * @return 1 if AppMMI Resource is supported (CI+ Spec 1.2).
     * @return 2 if AppMMI Resource v2 is supported (CI+ Spec 1.3).
     */
    itk_uint32_t (*resourceVersion)(struct itk_appmmi_t *thiz);

} itk_appmmi_t;
/**
 * @}
 */
#endif
