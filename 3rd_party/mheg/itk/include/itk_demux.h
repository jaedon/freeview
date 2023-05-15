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
#if !defined ITK_DEMUX_H__
#define ITK_DEMUX_H__

/**
 * @file 
 * Demux Interface.
 */
#include "itk_types.h"
/**
 * @ingroup host
 * @defgroup demux Demux
 * @{
 */

/**
 * @defgroup demuxfilter Filter
 * @{
 */
/** 
 * The demux filter jump table.
 */
typedef struct itk_demux_filter_t
{
    /**
     * Enables/disables the specified demux filter
     *
     * Disabling a filter should flush any buffers associated with it to
     * ensure a clean startup if the filter is re-enabled.
     *
     * @param[in] thiz A pointer to the filter
     * @param[in] control ITK_ENABLE or ITK_DISABLE
     */
    void (*control)(struct itk_demux_filter_t *thiz, itk_control_t control);

    /**
     * Releases the filter resource
     *
     * @param[in] thiz A pointer to the filter
     */
    void (*release)(struct itk_demux_filter_t *thiz);
} itk_demux_filter_t;
/** @} */

/**
 * @defgroup demuxsession Demux Session
 * @{
 */

/**
 * The demux session jump table.
 */
typedef struct itk_demux_session_t
{
    /**
     * Requests a new demux filter from the session
     *
     * Filters are created disabled, to enable them call
     * itk_demux_filter_t::control().
     *
     * @param[in] thiz A pointer to the demux object
     * @param[in] pid The PID on which to set the filter
     * @param[in] match A pointer to an array of filter match bytes
     * @param[in] mask A pointer to an array of filter mask bytes
     * @param[in] depth The length of the match mask filter (up to 12 bytes)
     *
     * @return A valid handle to the filter if it was created
     * @return NULL if the filter was not created
     */
    itk_demux_filter_t *(*getFilter)(struct itk_demux_session_t *thiz, 
                                     itk_uint16_t pid,
                                     const itk_uint8_t *match,
                                     const itk_uint8_t *mask,
                                     itk_uint8_t depth);

    /**
     * Deletes the specified demux session
     *
     * All the filters created for the session should be released before
     * releasing the session.
     *
	 * @param[in] thiz A pointer to the demux object
     */
    void (*release)(struct itk_demux_session_t *thiz);
} itk_demux_session_t;

/**
 * Demux session callback
 *
 * The callback is invoked to deliver data from all filters in the session. It is
 * assumed that the callback may be called from a high priority task (even ISR).
 *
 * @note The data pointer must be valid until the callback returns. After this
 *       point it may become invalid.
 * 
 * @note It is possible (but not recommended) that filters each use their own
 *       thread. The callback provided by the client is re-entrant and can handle this.
 *
 * @param[in] context The context value given when the demux session was created.
 * @param[in] session The session being served.
 * @param[in] data A pointer to the data received.
 * @param[in] dataSize The number of bytes received.
 */
typedef void (*itk_demux_session_callback_t)(void *context,
                                             itk_demux_session_t *session,
                                             const itk_uint8_t *data,
                                             size_t dataSize);
/** 
 * @} 
 */

/**
 * @defgroup demuxsi SI Table Acquisition
 * @{
 */

/**
 * SI table call back
 *
 * The callback is invoked to deliver the SI table sections.  It is assumed that
 * the callback may be called from a high priority task (even ISR).
 *
 * @note The data pointer must be valid until the callback returns. After this
 *       point it may become invalid.
 *
 * @param[in] context The context given when the table was created.
 * @param[in] table The SI table filter object.
 * @param[in] data A pointer to the data received.
 * @param[in] dataSize The number of bytes received.
 */
typedef void (*itk_demux_si_callback_t)(void *context,
                                        itk_demux_filter_t *table,
                                        const itk_uint8_t *data,
                                        size_t dataSize);
/** 
 * @} 
 */

/**
 * The demux jump table
 */
typedef struct itk_demux_t
{
    /**
     * Creates a new demux session on a stream. 
     *
     * A demux session is used to create filters for collecting private data
     * sections.
     *
     * @param[in] thiz A pointer to the demux
     * @param[in] onid The Original Network ID of the stream
     * @param[in] tsid The Transport Stream ID of the stream
     * @param[in] callback A pointer to the callback function
     * @param[in] callbackContext A context to be passed to the callback
     *
     * @return A valid handle to the session if it was created
     * @return NULL if the session was not created
     */
    itk_demux_session_t *(*newSession)(struct itk_demux_t *thiz,
                                       itk_uint16_t onid,
                                       itk_uint16_t tsid,
                                       itk_demux_session_callback_t callback,
                                       void *callbackContext);

    /**
     * Creates a new SI table filter
     *
     * An SI table object provides a handle to an MPEG SI Table sections.  This can be 
     * used to access the PAT, PMT, NIT and SDT and potentially others.
     *
     * @param[in] thiz A pointer to the demux 
     * @param[in] onid The Original Network ID of the stream
     * @param[in] tsid The Transport Stream ID of the stream
     * @param[in] pid The PID on which to set the table filter
     * @param[in] tableId The table id, the first byte of the table
     * @param[in] tableIdExtn The table id extension, the 4th and 5th bytes of the table
     * @param[in] callback A pointer to the callback function
     * @param[in] callbackContext A context to be passed to the callback
     *
     * @return A valid handle to a filter if it was created
     * @return NULL if the filter was not created
     *
     * @remark The table_id_extension maps to the network id of a NIT, the transport stream 
     * id of a PAT or SDT and the service id of a PMT.
     */
    itk_demux_filter_t *(*newSITable)(struct itk_demux_t *thiz,
                                      itk_uint16_t onid,
                                      itk_uint16_t tsid,
                                      itk_uint16_t pid,
                                      itk_uint8_t tableId,
                                      itk_uint16_t tableIdExtn,
                                      itk_demux_si_callback_t callback,
                                      void *callbackContext);
} itk_demux_t;

/**
 * @}
 */
#endif
