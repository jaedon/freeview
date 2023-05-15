/***************************************************************************
 *  Copyright (c) 2004-2012, Broadcom Corporation
 *  All Rights Reserved
 *  Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_recpump.c $
 * $brcm_Revision: 29 $
 * $brcm_Date: 6/18/12 12:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_recpump.c $
 * 
 * 29   6/18/12 12:50p jessem
 * SW7425-3264: Fixed coverity defect.
 *
 * 28   8/16/11 5:12p dlwin
 * SW7346-457: Resolved Coverity:35200 issue.
 *
 * 27   5/12/11 3:46p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 * NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 * instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 *
 * 26   8/10/10 4:22p erickson
 * SW7420-934: rename function
 *
 * 25   6/22/10 4:34p mphillip
 * SW7550-463: Clear keyslot when there is security but no DMA
 *
 * 24   6/10/10 2:45p mphillip
 * SW7550-380: Enable non-DMA PVR encryption for 7550 and similar
 * platforms
 *
 * 23   2/11/10 2:54p erickson
 * SW7550-252: fix dma/security testes
 *
 * 22   6/15/09 4:59p jtna
 * PR43001: add support for TTS+encryption record/playback
 *
 * 21   3/9/09 7:00p nickh
 * PR52996: Fix compilation errors when NEXUS_HAS_SECURITY is disabled
 *
 * 20   12/24/08 12:42p mphillip
 * PR49607: Keyladder key derivation support added for encrypted PVR in
 * nexus shim
 *
 * 19   12/24/08 11:56a jgarrett
 * PR 50703: Allowing security module to be absent
 *
 * 18   10/29/08 4:56p jrubio
 * PR47690: make sure Playback is taking out if NEXUS_HAS_PLAYBACK is
 * false
 *
 * 17   10/17/08 4:02p katrep
 * PR47690: Add option to generate index index on video pid during allpass
 * record, disabled by default.To enable define ALLPASS_RECORD_WITH_INDEX
 *
 * 16   10/16/08 6:20p katrep
 * PR47690: Adding allpass record from playback parser
 *
 * 15   10/15/08 2:39p mphillip
 * PR45211: Disable encryption on clear records
 *
 * 14   9/25/08 1:06p katrep
 * PR47154: Add support for recording with null packets and without null
 * packets
 *
 * 13   9/22/08 1:53p katrep
 * PR47154: More fine tuning of allpass record
 *
 * 12   9/19/08 7:49p katrep
 * PR47154: Add support for allpass record
 *
 * 11   6/27/08 3:36p mphillip
 * PR42901: Free PVR keyslot on close/stop
 *
 * 10   6/26/08 6:21p mphillip
 * PR42901: Encrypted PVR playback support
 *
 * 9   4/10/08 9:56a erickson
 * PR36068: coverity fixes
 *
 * 8   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 7   4/3/08 5:41p jgarrett
 * PR 41312: Setting callback events
 *
 * 6   3/12/08 6:37p katrep
 * PR40033: Implemented setttop recpump using nexus
 *
 * 5   2/7/08 10:20a erickson
 * PR39384: added recpump dataReadyThreshold
 *
 * 4   1/22/08 9:51a erickson
 * PR34925: removed numDescriptors
 *
 * 3   11/14/07 1:29p erickson
 * PR36068: added record
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 *******************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(recpump);

BDBG_OBJECT_ID(brecpump);

/* #define  ALLPASS_RECORD_WITH_INDEX 1 */

static void brecpump_p_data_write_callback(void *context, int param)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    BSTD_UNUSED(param);
    B_Event_Set(recpump->dataEvent);
}

static void brecpump_p_data_write_handler(void *context)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(recpump->params.data_write_callback)
    {
        b_unlock();
        (*recpump->params.data_write_callback)(recpump->params.callback_context);
        b_lock();
    }
}

static void brecpump_p_index_write_callback(void *context, int param)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    BSTD_UNUSED(param);
    B_Event_Set(recpump->indexEvent);
}

static void brecpump_p_index_write_handler(void *context)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(recpump->params.index_write_callback)
    {
        b_unlock();
        (*recpump->params.index_write_callback)(recpump->params.callback_context);
        b_lock();
    }
}

static void brecpump_p_overflow_callback(void *context, int param)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    BSTD_UNUSED(param);
    B_Event_Set(recpump->overflowEvent);
}

static void brecpump_p_overflow_handler(void *context)
{
    brecpump_t recpump = context;
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(recpump->params.overflow)
    {
        b_unlock();
        (*recpump->params.overflow)(recpump->params.callback_context);
        b_lock();
    }
}

void brecpump_get_open_params(bobject_t id, brecpump_open_params *open_params)
{
    BSTD_UNUSED(id);
    BKNI_Memset(open_params, 0, sizeof(*open_params));
    /* a small pad is needed for optimal sizing of a 188/4096 byte aligned buffer. any
    number will work, but the amount unused at the end of the buffer may be undesired. */
    open_params->data.buffer_size = B_PVR_RECORD_BUFFER + 68;
    open_params->data.num_descriptors = B_PVR_N_RECORD_DESC;
    open_params->data.alignment = 12; /* 4K alignment */
    open_params->scode.buffer_size = (6*4*16)*48; /* 6*4 is a size of the single 6-word SCT entry, 16 is a number of SCT entries per descriptor */
    open_params->scode.num_descriptors = 48;
    open_params->scode.alignment = 0; /* unused */
}

brecpump_t brecpump_open(bobject_t id, const brecpump_open_params *open_params)
{
    brecpump_t recpump;
    unsigned index;
    brecpump_open_params default_open_params;
    NEXUS_RecpumpOpenSettings openSettings;

    recpump = BKNI_Malloc(sizeof(*recpump));
    if ( NULL == recpump )
    {
        BSETTOP_ERROR(berr_external_error);
        return NULL;
    }

    BKNI_Memset(recpump, 0, sizeof(*recpump));
    BDBG_OBJECT_SET(recpump, brecpump);

    index = B_ID_GET_INDEX(id);

    if (!open_params) {
        brecpump_get_open_params(id, &default_open_params);
        open_params = &default_open_params;
    }

    NEXUS_Recpump_GetDefaultOpenSettings(&openSettings);
    openSettings.data.bufferSize = open_params->data.buffer_size;
    openSettings.data.dataReadyThreshold = openSettings.data.bufferSize / 5; /* 20% */
    openSettings.data.alignment = open_params->data.alignment;
    openSettings.index.bufferSize = open_params->scode.buffer_size;
    openSettings.index.alignment = open_params->scode.alignment;
    openSettings.index.dataReadyThreshold = openSettings.index.bufferSize / 5; /* 20% */

    recpump->nRecpump = NEXUS_Recpump_Open(index, &openSettings);
    if (!recpump->nRecpump) {
        BDBG_OBJECT_DESTROY(recpump, brecpump);
        BKNI_Free(recpump);
        return NULL;
    }

    recpump->dataEvent = B_Event_Create(NULL);
    if ( NULL == recpump->dataEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }
    recpump->indexEvent = B_Event_Create(NULL);
    if ( NULL == recpump->indexEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }
    recpump->overflowEvent = B_Event_Create(NULL);
    if ( NULL == recpump->overflowEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }

    recpump->dataEventId = b_event_register(recpump->dataEvent, brecpump_p_data_write_handler, recpump);
    if ( NULL == recpump->dataEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }

    recpump->indexEventId = b_event_register(recpump->indexEvent, brecpump_p_index_write_handler, recpump);
    if ( NULL == recpump->indexEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }

    recpump->overflowEventId = b_event_register(recpump->overflowEvent, brecpump_p_overflow_handler, recpump);
    if ( NULL == recpump->overflowEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto err_event;
    }

    return recpump;

err_event:
    brecpump_close(recpump);
    return NULL;
}

void brecpump_close(brecpump_t recpump)
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);

    if ( NULL != recpump->overflowEventId )
        b_event_unregister(recpump->overflowEventId);
    if ( NULL != recpump->overflowEventId )
        b_event_unregister(recpump->indexEventId);
    if ( NULL != recpump->indexEvent )
        b_event_unregister(recpump->dataEventId);

    if ( NULL != recpump->dataEventId )
        B_Event_Destroy(recpump->overflowEvent);
    if ( NULL != recpump->indexEvent )
        B_Event_Destroy(recpump->indexEvent);
    if ( NULL != recpump->dataEvent )
        B_Event_Destroy(recpump->dataEvent);

    NEXUS_Recpump_Close(recpump->nRecpump);
    BDBG_OBJECT_DESTROY(recpump, brecpump);
    BKNI_Free(recpump);
}

void brecpump_params_init(brecpump_params *params,  brecpump_t recpump)
{
    BSTD_UNUSED(recpump);
    BKNI_Memset(params, 0, sizeof(*params));
    bencryption_params_init(&params->encryption);
}

bresult brecpump_start(brecpump_t recpump, bstream_t stream, const brecpump_params *params)
{

    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_Error rc=NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(recpump, brecpump);

    if (!stream || !stream->parser_band || !params || !params->data_write_callback)
    {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    NEXUS_Recpump_GetSettings(recpump->nRecpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = brecpump_p_data_write_callback;
    recpumpSettings.data.dataReady.context = recpump;
    recpumpSettings.index.dataReady.callback = brecpump_p_index_write_callback;
    recpumpSettings.index.dataReady.context = recpump;
    recpumpSettings.data.overflow.callback = brecpump_p_overflow_callback;
    recpumpSettings.data.overflow.context = recpump;
    recpumpSettings.index.overflow.callback = brecpump_p_overflow_callback;
    recpumpSettings.index.overflow.context = (void*)recpump;
    if (params->encryption.type != bencryption_type_none) {
#if NEXUS_HAS_SECURITY
        if (!params->encryption.key_ladder) {
            switch (params->encryption.type) {
            case bencryption_type_des:
                if (params->encryption.key_length != 64)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            case bencryption_type_3des:
                if (params->encryption.key_length != 128)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            case bencryption_type_aes:
                if (params->encryption.key_length != 128)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            default:
                BDBG_ERR(("Unsupported PVR encryption algorithm"));
                return BSETTOP_ERROR(berr_invalid_parameter);
            }
        } else if (params->encryption.key_length != 8*sizeof(bcrypto_keyladder_data) || !params->encryption.long_key) {
            return BSETTOP_ERROR(berr_invalid_parameter);
        }
        recpumpSettings.securityContext = b_keyslot_m2m_allocate(&params->encryption,true,
            recpumpSettings.timestampType==NEXUS_TransportTimestampType_eNone?false:true);
        recpump->hKeySlot = recpumpSettings.securityContext;
#if NEXUS_NUM_DMA_CHANNELS
        recpumpSettings.securityDma = g_dma.hDma;
        BDBG_MSG(("Enabling encryption on record: slot: %p, dma: %p",recpumpSettings.securityContext,recpumpSettings.securityDma));
#else
        BDBG_MSG(("Enabling encryption on record: slot: %p",recpumpSettings.securityContext));
#endif
        if (!recpumpSettings.securityContext
#if NEXUS_NUM_DMA_CHANNELS
                || !recpumpSettings.securityDma
#endif
                ) {
            BDBG_ERR(("Enabling encryption on record FAILED!"));
#if NEXUS_NUM_DMA_CHANNELS
            BDBG_ERR(("slot: %p, dma: %p",recpumpSettings.securityContext,recpumpSettings.securityDma));
#else
            BDBG_ERR(("slot: %p",recpumpSettings.securityContext));
#endif
            return BSETTOP_ERROR(berr_external_error);
        }
#else
        BDBG_ERR(("PVR encryption is not supported"));
        return BSETTOP_ERROR(berr_invalid_parameter);
#endif
    } else {
#if NEXUS_HAS_SECURITY
        recpumpSettings.securityContext = NULL;
#if NEXUS_NUM_DMA_CHANNELS
        recpumpSettings.securityDma = NULL;
#endif
#endif
    }
    rc = NEXUS_Recpump_SetSettings(recpump->nRecpump, &recpumpSettings);
	if (rc!=NEXUS_SUCCESS)
	{
		return BSETTOP_ERROR(berr_external_error);
	}

    if(stream->mpeg.video[0].pid == 0 && (stream->mpeg.audio[0].pid==0 || stream->mpeg.audio[0].pid >= 0x1fff))
    {
        BDBG_WRN(("todo add suppport for all pass record"));
        return BSETTOP_ERROR(berr_not_supported);
    }

    recpump->params = *params;
    recpump->stream = stream;

    /* relying on the mpeg structure to give us the pids, starting from video pids */
    brecpump_p_mpeg_change(recpump, &stream->mpeg);

    rc = NEXUS_Recpump_Start(recpump->nRecpump);
    if ( rc )
    {
        recpump->stream = NULL;
        brecpump_p_mpeg_change(recpump, NULL);
        return BSETTOP_ERROR(berr_external_error);
    }

    return b_ok;
}

bresult brecpump_stop(brecpump_t recpump)
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    /* Remove all pid channels */
    brecpump_p_mpeg_change(recpump, NULL);
    NEXUS_Recpump_StopData(recpump->nRecpump);
    recpump->stream->consumers.recpump = NULL;
    recpump->stream = NULL;

    if (recpump->hKeySlot) {
        b_keyslot_m2m_free(recpump->hKeySlot);
        recpump->hKeySlot = NULL;
    }

    return b_ok;
}

void brecpump_immediate_stop(brecpump_t recpump)
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    /* Remove all pid channels */
    brecpump_p_mpeg_change(recpump, NULL);
    NEXUS_Recpump_Stop(recpump->nRecpump);
    recpump->stream->consumers.recpump = NULL;
    recpump->stream = NULL;
}

bresult brecpump_data_get_buffer(brecpump_t recpump, const void **buffer,  size_t *buffer_size )
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(NEXUS_Recpump_GetDataBuffer(recpump->nRecpump, buffer, buffer_size))
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    return b_ok;
}

bresult brecpump_index_get_buffer(brecpump_t recpump, const void **buffer,  size_t *buffer_size )
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(NEXUS_Recpump_GetIndexBuffer(recpump->nRecpump, buffer, buffer_size))
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    return b_ok;
}

bresult brecpump_data_write_complete(brecpump_t recpump, size_t amount_written)
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(NEXUS_Recpump_DataReadComplete(recpump->nRecpump, amount_written))
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    return b_ok;
}

bresult brecpump_index_write_complete(brecpump_t recpump, size_t amount_written)
{
    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(NEXUS_Recpump_IndexReadComplete(recpump->nRecpump, amount_written))
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    return b_ok;
}

void brecpump_get_status(brecpump_t recpump, brecpump_status *status )
{
    NEXUS_RecpumpStatus recpumpStatus;

    BDBG_OBJECT_ASSERT(recpump, brecpump);
    if(NEXUS_Recpump_GetStatus(recpump->nRecpump,&recpumpStatus))
    {
        BSETTOP_ERROR(berr_external_error);
        return;
    }
    status->has_index = recpumpStatus.hasIndex;
    status->index_bytes_recorded = recpumpStatus.index.bytesRecorded;
    status->index_fifo_depth = recpumpStatus.index.fifoDepth;
    status->index_fifo_size = recpumpStatus.index.fifoSize;
    status->last_timestamp = 0; /* not supported in Nexus recpump */
    status->mpeg_bytes_recorded = recpumpStatus.data.bytesRecorded;
    status->mpeg_fifo_depth = recpumpStatus.data.fifoDepth;
    status->mpeg_fifo_size = recpumpStatus.data.fifoSize;
}


static void brecpump_p_mpeg_change_allpass(brecpump_t recpump , const bstream_mpeg *new_settings)
{
    uint16_t pid=0;
    int index=0;
    NEXUS_Error errCode;
    bstream_t stream = recpump->stream;
    NEXUS_RecpumpAddPidChannelSettings *pRecpumpPidChannelSettings=NULL;

#if NEXUS_HAS_PLAYBACK
    if (stream->producer.playback)
    {
        NEXUS_PlaybackSettings playbackSettings;
        NEXUS_PlaypumpStatus playpumpStatus;
        NEXUS_Playback_GetSettings(stream->producer.playback->nPlayback, &playbackSettings);
        NEXUS_Playpump_GetStatus(playbackSettings.playpump,&playpumpStatus);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            playbackSettings.playpumpSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            playbackSettings.playpumpSettings.acceptNullPackets=true;
        }
        playbackSettings.playpumpSettings.allPass=true;
        errCode = NEXUS_Playback_SetSettings(stream->producer.playback->nPlayback, &playbackSettings);
        BDBG_WRN(("Setting playback %d to allpass mode %s null packets",playpumpStatus.index,
                  playbackSettings.playpumpSettings.acceptNullPackets?"with":"without"));
        if ( errCode )
        {
            BDBG_ERR(("Unable to set Playback_SetSettings"));
            BSETTOP_ERROR(berr_external_error);
        }
    }
    else
#endif /* Does not have playback support from NEXUS */
    if (stream->producer.playpump)
        {
        NEXUS_PlaypumpSettings playpumpSettings;
        NEXUS_PlaypumpStatus playpumpStatus;
        NEXUS_Playpump_GetSettings(stream->producer.playpump->nPlaypump, &playpumpSettings);
        NEXUS_Playpump_GetStatus(stream->producer.playpump->nPlaypump,&playpumpStatus);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            playpumpSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            playpumpSettings.acceptNullPackets=true;
        }
        playpumpSettings.allPass=true;
        errCode = NEXUS_Playpump_SetSettings(stream->producer.playpump->nPlaypump, &playpumpSettings);
        BDBG_WRN(("Setting playpump %d to allpass mode %s null packets",playpumpStatus.index,
                  playpumpSettings.acceptNullPackets?"with":"without"));
        if ( errCode )
        {
            BDBG_ERR(("Unable to set Playpump_SetSettings"));
            BSETTOP_ERROR(berr_external_error);
        }
    }
    else if (stream->producer.band)
    {
        NEXUS_ParserBandSettings parserBandSettings;
        NEXUS_ParserBand_GetSettings(stream->parser_band->nParserBand, &parserBandSettings);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            parserBandSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            parserBandSettings.acceptNullPackets=true;
        }

        parserBandSettings.allPass=true;
        errCode = NEXUS_ParserBand_SetSettings(stream->parser_band->nParserBand, &parserBandSettings);

        BDBG_WRN(("Setting parser band %d to allpass mode %s null packets",stream->parser_band->nParserBand,
                  parserBandSettings.acceptNullPackets?"with":"without"));
        if ( errCode )
        {
            BDBG_ERR(("Unable to set ParserBand_SetSettings"));
            BSETTOP_ERROR(berr_external_error);
        }
    }
    else
    {
        BDBG_ERR(("Unknown producer"));
        return;
    }

    /* remove and close pid record pid channels */
    for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
    {
        if ( recpump->videoPid[index] )
        {
            errCode = NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->videoPid[index]);
            if ( errCode )
            {
                BDBG_ERR(("Unable to RemovePidChannel(video)"));
                BSETTOP_ERROR(berr_external_error);
            }
            bstream_p_close_pid(recpump->stream, recpump->videoPid[index]);
            recpump->videoPid[index] = NULL;
        }
        if ( recpump->audioPid[index] )
        {
            errCode = NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->audioPid[index]);
            if ( errCode )
            {
                BDBG_ERR(("Unable to RemovePidChannel(audio)"));
                BSETTOP_ERROR(berr_external_error);
            }
            bstream_p_close_pid(recpump->stream, recpump->audioPid[index]);
            recpump->audioPid[index] = NULL;
        }
        if ( recpump->ancillaryPid[index] )
        {
            errCode = NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->ancillaryPid[index]);
            if ( errCode )
            {
                BDBG_ERR(("Unable to RemovePidChannel(ancillaryPid)"));
                BSETTOP_ERROR(berr_external_error);
            }
            bstream_p_close_pid(recpump->stream, recpump->ancillaryPid[index]);
            recpump->ancillaryPid[index] = NULL;
        }
    }
    /* Add pid channnel to record */
    recpump->videoPid[0] = bstream_p_open_pid(recpump->stream,pid, bstream_pid_type_other);
    if ( NULL == recpump->videoPid[0] )
    {
        BDBG_WRN(("Unable to allocate record pid channel for all pass record"));
        BSETTOP_ERROR(berr_external_error);
    }

#if ALLPASS_RECORD_WITH_INDEX
    {
        NEXUS_RecpumpAddPidChannelSettings pidCfg;
        NEXUS_Recpump_GetDefaultAddPidChannelSettings(&pidCfg);
        pidCfg.pidType = NEXUS_PidType_eVideo;
        pidCfg.pidTypeSettings.video.index = true;
        pidCfg.pidTypeSettings.video.codec = b_videocodec2nexus(new_settings->video[1].format);
        /* generate index using pid number instead of pid chanel number */
        pidCfg.pidTypeSettings.video.pid = new_settings->video[1].pid;
        pRecpumpPidChannelSettings= &pidCfg;
    }
#endif

    errCode = NEXUS_Recpump_AddPidChannel(recpump->nRecpump, recpump->videoPid[0], pRecpumpPidChannelSettings);
    if ( errCode )
    {
        BDBG_ERR(("Unable to add record pid channel"));
        BSETTOP_ERROR(berr_external_error);
    }


    return;
}

void brecpump_p_mpeg_change(brecpump_t recpump, const bstream_mpeg *new_settings)
{
    int index;
    NEXUS_Error errCode;
#if 0
    bstream_mpeg empty_settings;
#endif

    BDBG_OBJECT_ASSERT(recpump, brecpump);
    BDBG_ASSERT(NULL != recpump->stream);


    /* check to see if we all pass record is requested */
    if(new_settings)
    {
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if(new_settings->ancillary[index].pid)
                break;
            if(new_settings->audio[index].pid && new_settings->audio[index].pid != 0xFFFF &&  new_settings->audio[index].pid != 0x1FFF  )
                break;
#if ALLPASS_RECORD_WITH_INDEX
            /* Ignore the video PID */
            if(index==1)
                continue;
#endif
            if(new_settings->video[index].pid)
                break;
        }
        if(index == BSETTOP_MAX_PROGRAMS)
        {
            brecpump_p_mpeg_change_allpass(recpump,new_settings);
            return;
        }
    }
    else
    {
        /* remove and close pid record pid channels */
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if ( recpump->videoPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->videoPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->videoPid[index]);
                recpump->videoPid[index] = NULL;
            }
            if ( recpump->audioPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->audioPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->audioPid[index]);
                recpump->audioPid[index] = NULL;
            }
            if ( recpump->ancillaryPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->ancillaryPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->ancillaryPid[index]);
                recpump->ancillaryPid[index] = NULL;
            }
        }
        return;
    }
#if 0
    /* Free all PID channels with NULL settings */
    if ( NULL == new_settings )
    {
        bstream_mpeg_init(&empty_settings);
        new_settings = &empty_settings;
    }
#endif

    for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
    {
        if ( (recpump->stream->mpeg.video[index].pid != new_settings->video[index].pid) ||
             (new_settings->video[index].pid != 0 && new_settings->video[index].pid < 0x1fff &&
              NULL == recpump->videoPid[index]) )
        {
            if ( recpump->videoPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->videoPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->videoPid[index]);
                recpump->videoPid[index] = NULL;
            }
            if ( new_settings->video[index].pid != 0 &&
                 new_settings->video[index].pid < 0x1fff )
            {
                recpump->videoPid[index] = bstream_p_open_pid(recpump->stream, new_settings->video[index].pid, bstream_pid_type_video);
                if ( NULL == recpump->videoPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", new_settings->video[index].pid, new_settings->video[index].pid));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
                    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
                    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
                    addPidChannelSettings.pidTypeSettings.video.codec = b_videocodec2nexus(new_settings->video[index].format);
                    if ( 0 == index && recpump->params.index_write_callback )
                        addPidChannelSettings.pidTypeSettings.video.index = true;
                    else
                        addPidChannelSettings.pidTypeSettings.video.index = false;

                    errCode = NEXUS_Recpump_AddPidChannel(recpump->nRecpump, recpump->videoPid[index], &addPidChannelSettings);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
        }
        if ( (recpump->stream->mpeg.audio[index].pid != new_settings->audio[index].pid) ||
             (new_settings->audio[index].pid != 0 && new_settings->audio[index].pid < 0x1fff &&
              NULL == recpump->audioPid[index]) )
        {
            if ( recpump->audioPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->audioPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->audioPid[index]);
                recpump->audioPid[index] = NULL;
            }
            if ( new_settings->audio[index].pid != 0 &&
                 new_settings->audio[index].pid < 0x1fff )
            {
                recpump->audioPid[index] = bstream_p_open_pid(recpump->stream, new_settings->audio[index].pid, bstream_pid_type_other);
                if ( NULL == recpump->audioPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", new_settings->audio[index].pid, new_settings->audio[index].pid));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    errCode = NEXUS_Recpump_AddPidChannel(recpump->nRecpump, recpump->audioPid[index], NULL);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
        }
        if ( (recpump->stream->mpeg.ancillary[index].pid != new_settings->ancillary[index].pid) ||
             (new_settings->ancillary[index].pid != 0 && new_settings->ancillary[index].pid < BSETTOP_PAT_PID &&
              NULL == recpump->ancillaryPid[index]) )
        {
            if ( recpump->ancillaryPid[index] )
            {
                NEXUS_Recpump_RemovePidChannel(recpump->nRecpump, recpump->ancillaryPid[index]);
                bstream_p_close_pid(recpump->stream, recpump->ancillaryPid[index]);
                recpump->ancillaryPid[index] = NULL;
            }
            if ( new_settings->ancillary[index].pid != 0 &&
                 new_settings->ancillary[index].pid < 0x1fff )
            {
                uint16_t pid;
                pid = new_settings->ancillary[index].pid;
                if ( pid == BSETTOP_PAT_PID )
                {
                    pid = 0;
                }
                recpump->ancillaryPid[index] = bstream_p_open_pid(recpump->stream, pid, bstream_pid_type_other);
                if ( NULL == recpump->ancillaryPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", new_settings->ancillary[index].pid, new_settings->ancillary[index].pid));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    errCode = NEXUS_Recpump_AddPidChannel(recpump->nRecpump, recpump->ancillaryPid[index], NULL);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
        }
    }
}

