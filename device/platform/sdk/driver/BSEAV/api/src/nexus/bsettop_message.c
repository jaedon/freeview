/***************************************************************************
 *  Copyright (c) 2003-2010, Broadcom Corporation
 *  All Rights Reserved
 *  Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_message.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 12/15/10 10:39a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_message.c $
 * 
 * 11   12/15/10 10:39a erickson
 * SW7420-941: delete unused and incorrect BLST_Q logic. BLST_Q_REMOVE was
 * being called for a node that wasn't in a list.
 * 
 * 10   10/8/10 5:06p erickson
 * SW7420-1148: don't call NEXUS_Message_GetDefaultStartSettings with a
 * bad handle
 *
 * 9   10/15/09 1:54p erickson
 * SWDEPRECATED-3717: have settop api shim default filter_group setting
 * match nexus default filterGroup setting
 *
 * 8   8/18/09 6:38p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app.
 *
 * 7   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 6   4/3/08 5:41p jgarrett
 * PR 41312: Setting callback events
 *
 * 5   12/4/07 3:09p erickson
 * PR36068: call NEXUS_PidChannel_Close
 *
 * 4   11/2/07 1:25p erickson
 * PR36633: update nexus enums
 *
 * 3   10/16/07 2:28p erickson
 * PR36068: pass through getbuffer/readcomplete rc's. stop when closing to
 * get bstream_close called.
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(message);

BDBG_OBJECT_ID(bmessage);

static void bmessage_p_dataready_handler(void *context,int param);

bmessage_stream_t bmessage_open(bmessage_format format)
{
    bmessage_stream_t msg;

    msg = BKNI_Malloc(sizeof(*msg));
    BKNI_Memset(msg, 0, sizeof(*msg));
    BDBG_OBJECT_SET(msg, bmessage);
    msg->format = format;
#if 0
    msg->dataEvent = B_Event_Create(NULL);
    if ( NULL == msg->dataEvent )
    {
        BKNI_Free(msg);
        BSETTOP_ERROR(berr_external_error);
        return NULL;
    }
    msg->dataEventId = b_event_register(msg->dataEvent, bmessage_p_dataready_handler, msg);
    if ( NULL == msg->dataEventId )
    {
        B_Event_Destroy(msg->dataEvent);
        BKNI_Free(msg);
        BSETTOP_ERROR(berr_external_error);
        return NULL;
    }
#endif
    msg->dataCallback=b_callback_create(msg,bmessage_p_dataready_handler,msg,0);
    if(msg->dataCallback==NULL)
    {
        BSETTOP_ERROR(berr_external_error);
        return NULL;
    }

    return msg;
}

void bmessage_close(bmessage_stream_t msg)
{
    BDBG_OBJECT_ASSERT(msg, bmessage);
    if (msg->stream) {
        bmessage_stop(msg);
    }
    if (msg->nMessage) {
        NEXUS_Message_Close(msg->nMessage);
    }
#if 0
    b_event_unregister(msg->dataEventId);
    B_Event_Destroy(msg->dataEvent);
#endif
    if(msg->dataCallback)
    {
        b_callback_destroy(msg->dataCallback);
    }
    BDBG_OBJECT_DESTROY(msg, bmessage);
    BKNI_Free(msg);
}

void bmessage_stream_params_init(bmessage_stream_params *params, bmessage_stream_t msg)
{
    NEXUS_MessageStartSettings startSettings;

    BKNI_Memset(params, 0, sizeof(*params));
    params->buffer_size = 4; /* 4K */
    BKNI_Memset(params->filter.mask, 0xFF, sizeof(params->filter.mask));
    BKNI_Memset(params->filter.excl, 0xFF, sizeof(params->filter.excl));

    /* shim default should match the nexus default */
    if (!msg->nMessage) {
        /* unforunately, NEXUS_Message_GetDefaultStartSettings takes a handle */
        NEXUS_MessageHandle temp = NEXUS_Message_Open(NULL);
        if (temp) {
            NEXUS_Message_GetDefaultStartSettings(temp, &startSettings);
            NEXUS_Message_Close(temp);
        }
        else {
            /* just couldn't get a default from nexus, so come up with one */
            BKNI_Memset(&startSettings, 0, sizeof(startSettings));
        }
    }
    else {
        NEXUS_Message_GetDefaultStartSettings(msg->nMessage, &startSettings);
    }
    params->filter_group = startSettings.filterGroup;
}

/* Skip byte two (since the hardware doesn't look at that byte) */
#define COPY_FILTER_BYTES( a, b ) \
    do { \
    a[0] = b[0]; \
    a[1] = b[1]; \
    BKNI_Memmove( &a[2], &b[3], 13 ); \
    a[15] = 0xFF; \
    } while(0)

void bmessage_p_dataready(void *context, int param)
{
    bmessage_stream_t msg = (bmessage_stream_t)context;
    BDBG_OBJECT_ASSERT(msg, bmessage);
    BSTD_UNUSED(param);
    /*B_Event_Set(msg->dataEvent);*/
    b_callback_fire(msg->dataCallback);
}

static void bmessage_p_dataready_handler(void *context,int param)
{
    bmessage_stream_t msg = (bmessage_stream_t)context;
    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(msg, bmessage);
    if (msg->params.data_ready_callback) {
        /*b_unlock();*/
        (*msg->params.data_ready_callback)(msg->params.callback_context);
        /*b_lock();*/
    }
}

void bmessage_p_overflow(void *context, int param)
{
    bmessage_stream_t msg = (bmessage_stream_t)context;
    BSTD_UNUSED(param);
    if (msg->params.overflow) {
        (*msg->params.overflow)(msg->params.callback_context);
    }
}

bresult bmessage_start(bmessage_stream_t msg, const bmessage_stream_params *params)
{
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(msg, bmessage);
    BDBG_ASSERT(NULL != params);

    if ( msg->stream )
    {
        bmessage_stop(msg);
    }

    if (msg->nMessage) {
        NEXUS_Message_Close(msg->nMessage);
    }

    msg->params = *params;

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.bufferSize = params->buffer_size * 1024;
    if (params->data_ready_callback) {
        settings.dataReady.callback = bmessage_p_dataready;
        settings.dataReady.context = msg;
    }
    if (params->overflow) {
        settings.overflow.callback = bmessage_p_overflow;
        settings.overflow.context = msg;
    }
    msg->nMessage = NEXUS_Message_Open(&settings);
    if (!msg->nMessage) {
        return BSETTOP_ERROR(berr_external_error);
    }

    NEXUS_Message_GetDefaultStartSettings(msg->nMessage, &startSettings);
    if (params->band) {
        bstream_mpeg mpeg;
        /* don't need pid channels, but must alloc parser band */
        bstream_mpeg_init(&mpeg);
        msg->stream = bstream_p_open_message(params->band, &mpeg);
        if (!msg->stream) {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
    else {
        msg->stream = params->stream;
        if (!msg->stream) {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
    msg->nPidChannel = startSettings.pidChannel = bstream_p_open_pid(msg->stream, params->pid, bstream_pid_type_other);
    if ( NULL == msg->nPidChannel )
    {
        BDBG_ERR(("Unable to open PID channel for message filtering"));
        bmessage_stop(msg);
        return BSETTOP_ERROR(berr_external_error);
    }

    startSettings.format = b_messageformat2nexus(msg->format);
    startSettings.bufferMode = NEXUS_MessageBufferMode_eContinuous;
    COPY_FILTER_BYTES(startSettings.filter.coefficient, params->filter.coef);
    COPY_FILTER_BYTES(startSettings.filter.mask, params->filter.mask);
    COPY_FILTER_BYTES(startSettings.filter.exclusion, params->filter.excl);
    startSettings.filterGroup = params->filter_group;
    startSettings.psiCrcDisabled = params->psi_crc_disabled;
    startSettings.psfCrcDisabled = params->psf_crc_disabled;

    rc = NEXUS_Message_Start(msg->nMessage, &startSettings);
    if (rc)
    {
        BDBG_ERR(("Unable to start message filtering"));
        bmessage_stop(msg);
        return BSETTOP_ERROR(berr_external_error);
    }
    return 0;
}

bresult bmessage_get_buffer(bmessage_stream_t msg, const void ** p_bfr, size_t *length)
{
    BDBG_OBJECT_ASSERT(msg, bmessage);
    return NEXUS_Message_GetBuffer(msg->nMessage, p_bfr, length);
}

bresult bmessage_read_complete(bmessage_stream_t msg, size_t amount_consumed)
{
    BDBG_OBJECT_ASSERT(msg, bmessage);
    return NEXUS_Message_ReadComplete(msg->nMessage, amount_consumed);
}

void bmessage_stop(bmessage_stream_t msg)
{
    BDBG_OBJECT_ASSERT(msg, bmessage);
    NEXUS_Message_Stop(msg->nMessage);
    if ( msg->stream )
    {
        if (msg->nPidChannel)
        {
            bstream_p_close_pid(msg->stream, msg->nPidChannel);
            msg->nPidChannel = NULL;
        }
        if ( msg->params.band )
        {
            bstream_close(msg->stream);
        }
        msg->stream = NULL;
    }
}

