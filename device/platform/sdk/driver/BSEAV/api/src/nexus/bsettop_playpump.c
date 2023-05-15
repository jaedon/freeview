/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_playpump.c $
 * $brcm_Revision: 30 $
 * $brcm_Date: 5/12/11 3:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_playpump.c $
 * 
 * 30   5/12/11 3:45p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 * NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 * instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 29   8/10/10 4:21p erickson
 * SW7420-934: rename function
 * 
 * 28   6/10/10 2:45p mphillip
 * SW7550-380: Enable non-DMA PVR encryption for 7550 and similar
 * platforms
 * 
 * 27   2/11/10 2:54p erickson
 * SW7550-252: fix dma/security testes
 *
 * 26   6/15/09 4:59p jtna
 * PR43001: add support for TTS+encryption record/playback
 *
 * 25   6/12/09 11:10a jtna
 * PR43001: support TS/TTS record
 *
 * 24   12/24/08 12:42p mphillip
 * PR49607: Keyladder key derivation support added for encrypted PVR in
 * nexus shim
 *
 * 23   12/24/08 11:56a jgarrett
 * PR 50703: Allowing security module to be absent
 *
 * 22   10/31/08 12:30p jjordan
 * PR47230: Add TTS Pacing
 *
 * PR47230/1   10/28/08 10:37a jjordan
 * PR47230: Port TTS Pacing to Nexus
 *
 * 21   10/7/08 11:33p erickson
 * PR47232: NEXUS_PlaypumpSettings.timestamp api change
 *
 * 20   7/24/08 7:21p lwhite
 * PR45136: Use default settings in bplaypump_start
 *
 * 19   7/22/08 10:14a ssood
 * PR42739: moving the rtp header stats function to ip applib
 *
 * 18   7/21/08 5:22p katrep
 * PR42739: Fixed compilation errors in IP build
 *
 * 17   7/15/08 9:32a erickson
 * PR42739: fix non-ip builds
 *
 * 16   7/14/08 4:29p lwhite
 * PR42739: Added RTCP support
 *
 * 15   7/10/08 10:21p ssood
 * PR42739: Adding support to absorb high jitter for Live IP
 *
 * 14   7/10/08 3:23p katrep
 * PR43636: Add support MSDRM ND
 *
 * 13   6/27/08 3:36p mphillip
 * PR42901: Free PVR keyslot on close/stop
 *
 * 12   6/26/08 6:21p mphillip
 * PR42901: Encrypted PVR playback support
 *
 * 11   6/25/08 6:12p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 10   6/24/08 4:29p erickson
 * PR43222: add BDBG_CASSERT
 *
 * 9   6/12/08 9:07p katrep
 * PR43636: Add support for MSDRM
 *
 * 8   5/2/08 9:20a erickson
 * PR42339: fix pid channel alloc for PVR
 *
 * 7   4/28/08 11:54a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 6   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 5   4/3/08 5:41p jgarrett
 * PR 41312: Setting callback events
 *
 * 4   12/4/07 3:09p erickson
 * PR36068: remove packetize
 *
 * 3   11/14/07 1:29p erickson
 * PR36068: added record
 *
 * 2   11/13/07 11:58a erickson
 * PR36068: trick modes working
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 *******************************************************************************/
#include "bsettop_impl.h"

#if defined(B_HAS_IP)
#include "b_playback_ip_lib.h"
#endif
BDBG_MODULE(playpump);

BDBG_OBJECT_ID(bplaypump);

static void bplaypump_p_callback_handler(void *context);

void bplaypump_get_open_params(bobject_t id, bplaypump_open_params *open_params)
{
    BSTD_UNUSED(id);
    open_params->buffer_size = B_PVR_PLAYBACK_BUFFER;
    open_params->num_descriptors = B_PVR_N_PLAYBACK_DESC;
    open_params->alignment = 12; /* 4096 bytes (block size) aligment */
}

bplaypump_t bplaypump_open(bobject_t id, const bplaypump_open_params *open_params)
{
    bplaypump_t playpump;
    NEXUS_PlaypumpOpenSettings openSettings;
    unsigned index = B_ID_GET_INDEX(id);
    bplaypump_open_params default_open_params;

    if (!open_params) {
        bplaypump_get_open_params(id, &default_open_params);
        open_params = &default_open_params;
    }

    playpump = BKNI_Malloc(sizeof(*playpump));
    BKNI_Memset(playpump, 0, sizeof(*playpump));
    BDBG_OBJECT_SET(playpump, bplaypump);
    playpump->index = index;

    NEXUS_Playpump_GetDefaultOpenSettings(&openSettings);
    openSettings.fifoSize = open_params->buffer_size;
    openSettings.alignment = open_params->alignment;
    openSettings.numDescriptors = open_params->num_descriptors;
    playpump->nPlaypump = NEXUS_Playpump_Open(index, &openSettings);
    if (!playpump->nPlaypump) {
        BSETTOP_ERROR(berr_external_error);
        BDBG_OBJECT_DESTROY(playpump, bplaypump);
        BKNI_Free(playpump);
        return NULL;
    }

    playpump->event = B_Event_Create(NULL);
    if ( NULL == playpump->event )
    {
        BSETTOP_ERROR(berr_external_error);
        NEXUS_Playpump_Close(playpump->nPlaypump);
        BDBG_OBJECT_DESTROY(playpump, bplaypump);
        BKNI_Free(playpump);
        return NULL;
    }
    playpump->eventId = b_event_register(playpump->event, bplaypump_p_callback_handler, playpump);
    if ( NULL == playpump->eventId )
    {
        BSETTOP_ERROR(berr_external_error);
        B_Event_Destroy(playpump->event);
        NEXUS_Playpump_Close(playpump->nPlaypump);
        BDBG_OBJECT_DESTROY(playpump, bplaypump);
        BKNI_Free(playpump);
        return NULL;
    }

    return playpump;
}

void bplaypump_close(bplaypump_t playpump)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    b_event_unregister(playpump->eventId);
    B_Event_Destroy(playpump->event);
    NEXUS_Playpump_Close(playpump->nPlaypump);
    BDBG_OBJECT_DESTROY(playpump, bplaypump);
    BKNI_Free(playpump);
}

void bplaypump_params_init(bplaypump_params *params, bplaypump_t playpump)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    BKNI_Memset(params, 0, sizeof(*params));
    params->route_through_parser_band = true;
}

void bplaypump_p_read_callback(void *context, int param)
{
    bplaypump_t playpump = (bplaypump_t)context;
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    B_Event_Set(playpump->event);
    BSTD_UNUSED(param);
}

static void bplaypump_p_callback_handler(void *context)
{
    bplaypump_t playpump = (bplaypump_t)context;
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    if (playpump->params.read_callback) {
        b_unlock();
        (*playpump->params.read_callback)(playpump->params.callback_context);
        b_lock();
    }
}

bstream_t bplaypump_start(bplaypump_t playpump, const bstream_mpeg *mpeg, const bplaypump_params *params)
{
    NEXUS_Error rc;
    NEXUS_PlaypumpSettings nSettings;

    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    if (playpump->stream) {
        BDBG_ERR(("already playing"));
        return NULL;
    }

    NEXUS_Playpump_GetDefaultSettings(&nSettings);
    nSettings.transportType = b_mpegtype2nexus(mpeg->mpeg_type);
    nSettings.timestamp.pacing = params->timestamp_active;
    nSettings.timestamp.type = params->timestamp_enabled ?
        NEXUS_TransportTimestampType_eMod300 : /* TODO: DSS */
        NEXUS_TransportTimestampType_eNone;
#ifdef B_HAS_IP
	if(params->timestamp_enabled && params->timestamp_active) {
		nSettings.timestamp.pacingMaxError = params->pacing_max_error;
		nSettings.timestamp.pacingOffsetAdjustDisable = true;
		nSettings.timestamp.parityCheckDisable = true;
		nSettings.timestamp.resetPacing = true;
		nSettings.timestamp.type = NEXUS_TransportTimestampType_eBinary;
	}
#endif

    if (nSettings.timestamp.type==NEXUS_TransportTimestampType_eNone) {
        /* this affects playback TS => record TTS and should be harmless for all other cases.
           necessary here because playpump is not aware of its consumers */
        nSettings.timestamp.forceRestamping = true;
    }

    if (params->read_callback) {
        nSettings.dataCallback.callback = bplaypump_p_read_callback;
        nSettings.dataCallback.context = playpump;
    }
    else {
        nSettings.dataCallback.callback = NULL;
    }

    if (mpeg->mpeg_type == bstream_mpeg_type_ts && mpeg->encryption.type != bencryption_type_none) {
#if NEXUS_HAS_SECURITY
        if (!mpeg->encryption.key_ladder) {
            switch (mpeg->encryption.type) {
            case bencryption_type_des:
                if (mpeg->encryption.key_length != 64) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            case bencryption_type_3des:
                if (mpeg->encryption.key_length != 128) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            case bencryption_type_aes:
                if (mpeg->encryption.key_length != 128) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            default:
                BDBG_ERR(("Unsupported PVR encryption algorithm"));
                BSETTOP_ERROR(berr_invalid_parameter);
                return NULL;
            }
        } else if (mpeg->encryption.key_length != 8*sizeof(bcrypto_keyladder_data) || !mpeg->encryption.long_key) {
            BSETTOP_ERROR(berr_invalid_parameter); return NULL;
        }
        nSettings.securityContext = b_keyslot_m2m_allocate(&mpeg->encryption, false,
            nSettings.timestamp.type==NEXUS_TransportTimestampType_eNone?false:true);
        playpump->hKeySlot = nSettings.securityContext;
#if NEXUS_NUM_DMA_CHANNELS
        nSettings.securityDma = g_dma.hDma;
        BDBG_MSG(("Enabling decryption on playback: slot: %p, dma: %p",nSettings.securityContext,nSettings.securityDma));
#else
        BDBG_MSG(("Enabling decryption on playback: slot: %p",nSettings.securityContext));
#endif
        if (!nSettings.securityContext
#if NEXUS_NUM_DMA_CHANNELS
                || !nSettings.securityDma
#endif
                ) {
            BDBG_ERR(("Enabling encryption on playback FAILED!"));
#if NEXUS_NUM_DMA_CHANNELS
            BDBG_ERR(("slot: %p, dma: %p",nSettings.securityContext,nSettings.securityDma));
#else
            BDBG_ERR(("slot: %p",nSettings.securityContext));
#endif
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
#else
        BDBG_ERR(("PVR encryption is not supported"));
        BSETTOP_ERROR(berr_invalid_parameter);
        return NULL;
#endif
    }
#if B_HAS_MSDRM_PD || B_HAS_MSDRM_ND
    if(mpeg->mpeg_type == bstream_mpeg_type_asf) {
        nSettings.securityContext = (NEXUS_KeySlotHandle) (*(void **)mpeg->encryption.key);
    }
#endif

    rc = NEXUS_Playpump_SetSettings(playpump->nPlaypump, &nSettings);
    if (rc) {BSETTOP_ERROR(berr_external_error);return NULL;}

    rc = NEXUS_Playpump_Start(playpump->nPlaypump);
    if (rc) {BSETTOP_ERROR(berr_external_error);return NULL;}

    playpump->stream = bstream_p_open(NULL, playpump, NULL, 0, mpeg);

#ifdef B_HAS_IP
    playpump->stream->producer.is_playback_ip = params->is_playback_ip;
    playpump->stream->producer.use_live_playback_mode = params->use_live_playback_mode;
    playpump->stream->producer.timestamp_active = params->timestamp_active;
    playpump->stream->producer.timestamp_enabled = params->timestamp_enabled;
#endif

    playpump->params = *params;

    return playpump->stream;
}

bresult bplaypump_stop(bplaypump_t playpump)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    NEXUS_Playpump_Stop(playpump->nPlaypump);
    bstream_p_close(playpump->stream);
    playpump->stream = NULL;

    if (playpump->hKeySlot) {
        b_keyslot_m2m_free(playpump->hKeySlot);
        playpump->hKeySlot = NULL;
    }

    return 0;
}

bresult bplaypump_get_buffer(bplaypump_t playpump, void **buffer, size_t *size)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    rc = NEXUS_Playpump_GetBuffer(playpump->nPlaypump, buffer, size);
    if (rc) return BSETTOP_ERROR(berr_external_error);
    return 0;
}

bresult bplaypump_read_complete(bplaypump_t playpump, size_t skip, size_t amount)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    BDBG_CASSERT(sizeof(bplaypump_segment_desc) == sizeof(NEXUS_PlaypumpSegment));
    rc = NEXUS_Playpump_WriteComplete(playpump->nPlaypump, skip, amount);
    if (rc) return BSETTOP_ERROR(berr_external_error);
    return 0;
}

bresult bplaypump_set(bplaypump_t playpump, const bplaypump_settings *settings)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    playpump->settings = *settings;
    /* TODO */
    return 0;
}

void bplaypump_get(bplaypump_t playpump, bplaypump_settings *settings)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    *settings = playpump->settings;
}

bresult bplaypump_set_state(bplaypump_t playpump, bplaypump_state state, bool forward)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    playpump->state = state;
    playpump->forward = forward;
    /* TODO */
    return 0;
}

bresult bplaypump_set_decoder_rate(bplaypump_t playpump, unsigned rate)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    playpump->rate = rate;
    /* TODO */
    return 0;
}

bresult bplaypump_frame_advance(bplaypump_t playpump)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    if (playpump->stream->consumers.decode) {
        /* TODO */
    }
    return 0;
}

bresult bplaypump_flush(bplaypump_t playpump)
{
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    NEXUS_Playpump_Flush(playpump->nPlaypump);
    if (playpump->stream->consumers.decode) {
        /* TODO */
    }
    return 0;
}

bresult bplaypump_get_status(bplaypump_t p, bplaypump_status *status)
{
    NEXUS_PlaypumpStatus nStatus;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(p, bplaypump);
    BKNI_Memset(status, 0, sizeof(*status));

    rc = NEXUS_Playpump_GetStatus(p->nPlaypump, &nStatus);
    if (rc) return BSETTOP_ERROR(berr_external_error);
    status->fifo_depth = nStatus.fifoDepth;
    status->fifo_size = nStatus.fifoSize;
    status->buffer_base = nStatus.bufferBase;
    status->bytes_played = nStatus.bytesPlayed;
    status->running = nStatus.started;
/* TODO:     status->noverflows; */
    return 0;
}

bresult bplaypump_p_set_pwm_value(bplaypump_t p, int pwm_value)
{
    /* TODO */
    BDBG_OBJECT_ASSERT(p, bplaypump);
    BSTD_UNUSED(pwm_value);
    return 0;
}

void bplaypump_get_source(bplaypump_t playpump, bplaypump_source *source)
{
    /* TODO */
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    BSTD_UNUSED(source);
}

bresult bplaypump_set_source(bplaypump_t playpump, const bplaypump_source *source)
{
    /* TODO */
    BDBG_OBJECT_ASSERT(playpump, bplaypump);
    BSTD_UNUSED(source);
    return 0;
}

bresult bplaypump_get_rtp_hdr_data(bplaypump_t p, const bplaypump_rtp_hdr_data_t *rtp_header_data, unsigned *entry_cnt)
{
    /* This function now resides in IP Applib, so no need to implement it here */
    BSTD_UNUSED(p);
    BSTD_UNUSED(rtp_header_data);
    BSTD_UNUSED(entry_cnt);
    return BSETTOP_ERROR(berr_not_supported);
}

