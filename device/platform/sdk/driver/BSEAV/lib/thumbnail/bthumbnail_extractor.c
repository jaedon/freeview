/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_extractor.c $
 * $brcm_Revision: 13 $
 * $brcm_Date: 10/7/11 11:11a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_extractor.c $
 * 
 * 13   10/7/11 11:11a erickson
 * SW7346-503: add bthumbnail_extractor_settings.timestampType
 * 
 * 12   9/14/10 4:44p erickson
 * SWDEPRECATED-3408: add missing #include
 * 
 * 11   8/16/10 4:29p jtna
 * SW35230-649: added bthumbnail_extractor_get_status
 * 
 * 10   8/16/10 3:43p jtna
 * SW35230-649: merge
 * 
 * SW35230-649/1   8/13/10 12:02p jtna
 * SW3520-649: added avi-specific handling to match new avi player
 * behavior
 * 
 * 9   8/10/10 4:22p erickson
 * SW7420-934: rename function
 * 
 * 8   6/24/10 9:49a erickson
 * SW3548-2959: fix potentially uninitialized variable
 *
 * 7   6/24/10 9:46a erickson
 * SW3548-2959: malloc io_buffer instead of blowing out the stack
 *
 * 6   6/23/10 2:55p katrep
 * SW7405-4502:Compiler warning in DEBUG=n build
 *
 * 5   6/18/10 2:06p erickson
 * SW3548-2959: fix warning
 *
 * 4   6/15/10 4:45p mphillip
 * SW7405-4502: Fix warnings with DEBUG=n
 *
 * 3   5/13/10 11:02a jtna
 * SW7125-293: Coverity Defect ID:20492 STACK_USE
 *
 * 2   8/20/09 10:59a erickson
 * PR55994: change api to support non-TS streams
 *
 * 1   8/14/09 3:12p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 ******************************************************************/
#include "bthumbnail_extractor.h"
#include "bmedia_player.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdio.h>

BDBG_MODULE(bthumbnail_extractor);

BDBG_OBJECT_ID(bthumbnail_extractor);

struct bthumbnail_extractor {
    BDBG_OBJECT(bthumbnail_extractor)
    bthumbnail_extractor_create_settings create_settings;
    bthumbnail_extractor_settings settings;
    bthumbnail_extractor_status status;
    bmedia_player_t player;
    batom_factory_t factory;
    bfile_buffer_t buffer;
    bfile_buffer_cfg buffer_cfg;
};

static int bthumbnail_extractor_p_send(bthumbnail_extractor_t handle, unsigned offset, const void *buffer, unsigned size, bmedia_player_pos timestamp);
static void bthumbnail_extractor_p_destroy_resources(bthumbnail_extractor_t handle);
static unsigned char bthumbnail_get_endcode(NEXUS_VideoCodec videoCodec);
static int bthumbnail_stream_p_generate_startcode_packet(bthumbnail_extractor_t handle, uint8_t *buffer, unsigned size);

void bthumbnail_extractor_get_default_create_settings( bthumbnail_extractor_create_settings *p_settings )
{
    BKNI_Memset(p_settings, 0, sizeof(*p_settings));
    p_settings->buffer_size = 200 * 1024;
}

bthumbnail_extractor_t bthumbnail_extractor_create( const bthumbnail_extractor_create_settings *p_settings )
{
    bthumbnail_extractor_t handle;

    handle = BKNI_Malloc(sizeof(*handle));
    if (!handle) {
        BERR_Code rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        BSTD_UNUSED(rc);
        return NULL;
    }
    BKNI_Memset(handle, 0, sizeof(*handle));
    BDBG_OBJECT_SET(handle, bthumbnail_extractor);

    handle->create_settings = *p_settings;

    /* default settings */
    handle->settings.videoCodec = NEXUS_VideoCodec_eMpeg2;
    handle->settings.transportType = NEXUS_TransportType_eTs;

    return handle;
}

void bthumbnail_extractor_destroy( bthumbnail_extractor_t handle )
{
    BDBG_OBJECT_ASSERT(handle, bthumbnail_extractor);
    bthumbnail_extractor_p_destroy_resources(handle);
    BDBG_OBJECT_DESTROY(handle, bthumbnail_extractor);
    BKNI_Free(handle);
}

void bthumbnail_extractor_get_settings( bthumbnail_extractor_t handle, bthumbnail_extractor_settings *p_settings )
{
    BDBG_OBJECT_ASSERT(handle, bthumbnail_extractor);
    *p_settings = handle->settings;
}

static void bthumbnail_extractor_p_destroy_resources(bthumbnail_extractor_t handle)
{
    if (handle->player) {
        bmedia_player_destroy(handle->player);
        handle->player = NULL;
    }
    if (handle->buffer) {
        bfile_buffer_destroy(handle->buffer);
        handle->buffer = NULL;
    }
    if (handle->buffer_cfg.buf) {
        BKNI_Free(handle->buffer_cfg.buf);
        handle->buffer_cfg.buf = NULL;
    }
    if (handle->factory) {
        batom_factory_destroy(handle->factory);
        handle->factory = NULL;
    }
}

int bthumbnail_extractor_set_settings( bthumbnail_extractor_t handle, const bthumbnail_extractor_settings *p_settings )
{
    bmedia_player_config config;
    bmedia_player_decoder_mode decode_mode;
    bmedia_player_stream stream;
    int rc;

    BDBG_OBJECT_ASSERT(handle, bthumbnail_extractor);
    handle->settings = *p_settings;

    bthumbnail_extractor_p_destroy_resources(handle);

    if (!p_settings->indexfile && !p_settings->datafile) {
        /* this just frees resources */
        return 0;
    }

    /* needed for MP4 and MKV */
    handle->factory = batom_factory_create(bkni_alloc, 256);
    if (!handle->factory) return BERR_TRACE(NEXUS_UNKNOWN);

    bfile_buffer_default_cfg(&handle->buffer_cfg);
    handle->buffer_cfg.buf = BKNI_Malloc(handle->buffer_cfg.buf_len);
    if (!handle->buffer_cfg.buf) return BERR_TRACE(NEXUS_UNKNOWN);
    handle->buffer_cfg.async = false;
    handle->buffer_cfg.fd = p_settings->indexfile;
    handle->buffer = bfile_buffer_create(handle->factory, &handle->buffer_cfg);
    if (!handle->buffer) return BERR_TRACE(NEXUS_UNKNOWN);

    bmedia_player_init_config(&config);
    config.buffer = handle->buffer;
    config.decoder_features.brcm = false;
    config.decoder_features.dqt = false;

    bmedia_player_init_stream(&stream);
    stream.stream.mpeg2ts.packet_size = 188;
    if (p_settings->timestampType != NEXUS_TransportTimestampType_eNone) {
        stream.stream.mpeg2ts.packet_size += 4;
    }
    switch (p_settings->transportType) {
    case NEXUS_TransportType_eAsf: stream.format = bstream_mpeg_type_asf; break;
    case NEXUS_TransportType_eAvi: stream.format = bstream_mpeg_type_avi; break;
    case NEXUS_TransportType_eMkv: stream.format = bstream_mpeg_type_mkv; break;
    case NEXUS_TransportType_eMp4: stream.format = bstream_mpeg_type_mp4; break;
    default: stream.format = bstream_mpeg_type_ts; break;
    }
    stream.master = p_settings->videoPid;
    stream.without_index = !p_settings->indexfile;

    handle->player = bmedia_player_create(p_settings->indexfile ? p_settings->indexfile : p_settings->datafile, &config, &stream);
    if (!handle->player) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* non-dqt, non-brcm reverse will always be I-frame only. 
       note, setting to forward mode will result in seek returning other non-I-frames after the I-frame. 
       this trips up AVD when doing still decode */
    rc = bmedia_player_set_direction(handle->player, -1, -BMEDIA_TIME_SCALE_BASE, &decode_mode);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

int bthumbnail_extractor_feed_picture( bthumbnail_extractor_t handle, unsigned timestamp )
{
    off_t last_start = 0;
    int stage = 0;
    int rc;
#if defined(HUMAX_PLATFORM_BASE)	
    int loop=0;
#endif
#define PLAYPUMP_TRAILING_PAD 512
    unsigned char pad[PLAYPUMP_TRAILING_PAD];
/* TODO: IO_BUFFER_SIZE should be smaller. right now the PlaypumpSegment logic requries that the picture be read in in one shot */
#define IO_BUFFER_SIZE (256*1024)
    unsigned char *io_buffer = NULL;
    bmedia_player_entry entry;
#if defined(HUMAX_PLATFORM_BASE)
    unsigned dummy=0;
    unsigned loopingLimiter=0;
    unsigned numhittingEOF=0;
#endif

    BDBG_OBJECT_ASSERT(handle, bthumbnail_extractor);
    BKNI_Memset(&handle->status, 0, sizeof(handle->status));

    /* read the still */
    BDBG_MSG(("bthumbnail_extractor_get_thumbnail:%#lx time:%u", handle, timestamp));
    rc = bmedia_player_seek(handle->player, timestamp);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto done;
    }

    io_buffer = BKNI_Malloc(IO_BUFFER_SIZE);
    if (!io_buffer) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }

    while (1) {
#if defined(HUMAX_PLATFORM_BASE)
        loopingLimiter++;
		if(loopingLimiter > 200)
		{
			BDBG_ERR(("Unable to get valid picture data from this file."));
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            break;		
		}
#endif	
        if (bmedia_player_next(handle->player, &entry)) {
            /* may have hit BOF */
            break;
        }
        BDBG_MSG(("bmedia_player_next %u, %u, %u, %d, %u:%u", (unsigned)entry.start, (unsigned)entry.length, entry.embedded, entry.timestamp, entry.type, entry.content));
        if (!entry.embedded && entry.start < last_start) {
            /* this works before bmedia is set in reverse mode */
            break;
        }

#if defined(HUMAX_PLATFORM_BASE)
        if(entry.type == bmedia_player_entry_type_end_of_stream)
        {
            numhittingEOF++;
            if(numhittingEOF > 5)
            {
				BDBG_MSG(("It seems not a valid SCD-type index file."));
                rc = BERR_TRACE(NEXUS_UNKNOWN);
                break;
            }
        }
#endif
        if (entry.embedded) {
            bthumbnail_extractor_p_send(handle, entry.start, entry.embedded, entry.length, 0);
        }
        else if (entry.atom) {
            batom_cursor cursor;
            ssize_t n;
            unsigned total_read = 0;

            batom_cursor_from_atom(&cursor, entry.atom);
            while (total_read < entry.length) {
                n = entry.length - total_read;
                if (n > IO_BUFFER_SIZE) {
                    n = IO_BUFFER_SIZE;
                }
                /* TODO: avoid the memcpy */
                n = batom_cursor_copy(&cursor, io_buffer, n);
                bthumbnail_extractor_p_send(handle, entry.start, io_buffer, n, 0);
                total_read += n;
            }
            batom_release(entry.atom);

            /* one atom = one picture */
            stage = 2;
        }
        else {
            ssize_t n;
            unsigned total_read = 0;

            if (handle->settings.transportType == NEXUS_TransportType_eAsf || handle->settings.transportType == NEXUS_TransportType_eAvi) {
                if (handle->settings.transportType != NEXUS_TransportType_eAvi) {
                    if (entry.content != bmedia_player_entry_content_header) {
                        stage = 2;
                    }
                }
                else { /* avi */
                    if (entry.content != bmedia_player_entry_content_header) {
                        stage++; 
                        /* we need two payloads from avi player after the header. 
                           the first payload will contain the first sequence header and first I-frame in the stream. 
                           timestamp==-1 must be fed back into avi player in order to extract just the sequence header. 
                           the next payload will contain the I-frame we seeked to */
                    }
                    if (entry.timestamp==0) {
                        /* in the corner case where the timestamp of the I-frame we want is actually at 0, we must set 
                           the timestamp to some nonzero value in order to get the I-frame. 
                           see nexus_playpump_media.c:b_pump_demux_descriptor */
                        entry.timestamp = 1;
                    }
                }
            }
            else {
                /* transport */
#if defined(HUMAX_PLATFORM_BASE)
                loop++;
                if(handle->settings.timestampType != NEXUS_TransportTimestampType_eNone)
                {
                    dummy=4;
                }
                if (entry.length > (188+dummy) && stage == 0 && loop > 1)
#else
                if (entry.length > 188 && stage == 0)
#endif
                    stage = 1;
                else if (stage == 1)
                    stage = 2;
            }

            handle->settings.datafile->seek(handle->settings.datafile, entry.start, SEEK_SET);

            while (total_read < entry.length) {
                n = entry.length - total_read;
                if (n > IO_BUFFER_SIZE) {
                    n = IO_BUFFER_SIZE;
                }
                n = handle->settings.datafile->read(handle->settings.datafile, io_buffer, n);
                if (n < 0) {
                    rc = BERR_TRACE(NEXUS_UNKNOWN);
                    goto done;
                }
#if defined(HUMAX_PLATFORM_BASE)
                rc = bthumbnail_extractor_p_send(handle, entry.start, io_buffer, n, entry.timestamp);
                if(rc != 0) goto done;
#else
                bthumbnail_extractor_p_send(handle, entry.start, io_buffer, n, entry.timestamp);
#endif
                total_read += n;
            }
            last_start = entry.start;
        }
        if (stage == 2) break;
    };

    handle->status.timestamp = entry.timestamp;

    if (handle->settings.transportType == NEXUS_TransportType_eMkv || handle->settings.transportType == NEXUS_TransportType_eMp4) {
        bmedia_pes_info info;
#define MAX_PACKET_SIZE 192
        unsigned char packet[MAX_PACKET_SIZE];
        unsigned n;

        /* create EOS PES packet */
        bmedia_pes_info_init(&info, 0xe0);
        n = bmedia_pes_header_init(packet, MAX_PACKET_SIZE, &info);

        /* add the terminating startcode */
        packet[n++] = 0x00;
        packet[n++] = 0x00;
        packet[n++] = 0x01;
        packet[n++] = bthumbnail_get_endcode(handle->settings.videoCodec);
        while (n < 36) {
            packet[n++] = 0;
        }
        BDBG_ASSERT(n == 36);
        bthumbnail_extractor_p_send(handle, 0, packet, n, 0);

        BKNI_Memset(pad, 0, sizeof(pad));
        bthumbnail_extractor_p_send(handle, 0, pad, sizeof(pad), 0);
    }
    else if (handle->settings.transportType == NEXUS_TransportType_eAsf || handle->settings.transportType == NEXUS_TransportType_eAvi) {
        unsigned char fill = 0;

        /* now add a one byte payload segment so that mediaframe generates an end-of-sequence */
        bthumbnail_extractor_p_send(handle, 0, &fill, sizeof(fill), 0);
    }
    else {
        unsigned char packet[MAX_PACKET_SIZE];
        unsigned n;

        /* now copy the endcode for the codec */
        n = bthumbnail_stream_p_generate_startcode_packet(handle, packet, MAX_PACKET_SIZE);
        bthumbnail_extractor_p_send(handle, 0, packet, n, 0);

        /* now add 256 bytes of zero to flush data through transport */
        BKNI_Memset(pad, 0, sizeof(pad));
        bthumbnail_extractor_p_send(handle, 0, pad, sizeof(pad), 0);
    }

    rc = 0;

done:
    if (io_buffer) BKNI_Free(io_buffer);
    return rc;
}

void bthumbnail_extractor_get_status(bthumbnail_extractor_t handle, bthumbnail_extractor_status *status)
{
    BKNI_Memcpy(status, &handle->status, sizeof(handle->status));
}

int bthumbnail_extractor_start_playpump(bthumbnail_extractor_t handle)
{
    NEXUS_PlaypumpSettings playpumpSettings;
    int rc;

    if (!handle->settings.playpump) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    NEXUS_Playpump_GetSettings(handle->settings.playpump, &playpumpSettings);
    switch (handle->settings.transportType) {
    case NEXUS_TransportType_eMkv:
    case NEXUS_TransportType_eMp4:
        playpumpSettings.transportType = NEXUS_TransportType_eMpeg2Pes;
        break;
    default:
        playpumpSettings.transportType = handle->settings.transportType;
        break;
    }
    playpumpSettings.mode = NEXUS_PlaypumpMode_eSegment;
    playpumpSettings.timestamp.type = handle->settings.timestampType;
    rc = NEXUS_Playpump_SetSettings(handle->settings.playpump, &playpumpSettings);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Playpump_Start(handle->settings.playpump);
    if (rc) return BERR_TRACE(rc);

    /* We must set the playRate after Start for it to be properly applied.
    A non-normal playrate is needed to force media framework into key-frame only mode. Some ASF/AVI files have indexes which
    point to more than just a single key-frame, so code to filter down to a single key-frame has to be handled inside Playpump. */
    NEXUS_Playpump_GetSettings(handle->settings.playpump, &playpumpSettings);
    playpumpSettings.playRate = NEXUS_NORMAL_PLAY_SPEED+1;
    rc = NEXUS_Playpump_SetSettings(handle->settings.playpump, &playpumpSettings);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

void bthumbnail_extractor_stop_playpump(bthumbnail_extractor_t handle)
{
    NEXUS_Playpump_Stop(handle->settings.playpump);
}

static int bthumbnail_extractor_p_send_raw(bthumbnail_extractor_t handle, const void *buffer, unsigned size)
{
#if defined(HUMAX_PLATFORM_BASE)
    int retryCount=0;
#endif
    while (size) {
        size_t playback_size;
        void *playback_buffer;
        int rc;

        rc = NEXUS_Playpump_GetBuffer(handle->settings.playpump, &playback_buffer, &playback_size);
        if (rc) return BERR_TRACE(rc);

        if (!playback_size) {
#if defined(HUMAX_PLATFORM_BASE)
            retryCount++;
            if(retryCount > 300) /*no progress over 3 seconds*/
            {
                BDBG_ERR(("Playpump halted. maybe source is corrupted."));
                return -1;
            }
#endif
            BKNI_Sleep(10); /* blocking function */
            continue;
        }

        if (playback_size > size) {
            playback_size = size;
        }
        BKNI_Memcpy(playback_buffer, buffer, playback_size);

        rc = NEXUS_Playpump_WriteComplete(handle->settings.playpump, 0, playback_size);
        if (rc) return BERR_TRACE(rc);

        buffer = (uint8_t*)buffer + playback_size;
        size -= playback_size;
    }

    return 0;
}

static int bthumbnail_extractor_p_send(bthumbnail_extractor_t handle, unsigned offset, const void *buffer, unsigned size, bmedia_player_pos timestamp)
{
    NEXUS_PlaypumpSegment segment;
    int rc;

    BKNI_Memset(&segment, 0, sizeof(segment));
    segment.signature = NEXUS_PLAYPUMP_SEGMENT_SIGNATURE;
    segment.length = sizeof(NEXUS_PlaypumpSegment) + size;
    segment.offset = offset;

    segment.timestamp = timestamp;
    segment.timestamp_delta[0].stream_id = handle->settings.videoPid;
    segment.timestamp_delta[0].timestamp_delta = 0;

    rc = bthumbnail_extractor_p_send_raw(handle, &segment, sizeof(segment));
    if (rc) return BERR_TRACE(rc);

    rc = bthumbnail_extractor_p_send_raw(handle, buffer, size);
    if (rc) return BERR_TRACE(rc);

    BDBG_MSG(("send size %u, offset %u, timestamp %d", size, offset, timestamp));
    return 0;
}

static unsigned char bthumbnail_get_endcode(NEXUS_VideoCodec videoCodec)
{
    switch (videoCodec){
    case NEXUS_VideoCodec_eMpeg2:
        return 0xB7;
    case NEXUS_VideoCodec_eH264:
        return 0x0A;
    case NEXUS_VideoCodec_eVc1:
    case NEXUS_VideoCodec_eVc1SimpleMain:
        return 0x0A;
    case NEXUS_VideoCodec_eMpeg4Part2:
    case NEXUS_VideoCodec_eDivx311:
        return 0xB1;
    default:
        BDBG_ERR(("unsupported video codec %d", videoCodec));
        return 0x0A;
    }
}

static int bthumbnail_stream_p_generate_startcode_packet(bthumbnail_extractor_t handle, uint8_t *buffer, unsigned size)
{
    unsigned offset = 0;

    BSTD_UNUSED(size);

    if (handle->settings.timestampType != NEXUS_TransportTimestampType_eNone) {
        offset = 4;
    }
    buffer[0+offset] = 0x47;   /* SYNC BYTE */
    buffer[1+offset] = (handle->settings.videoPid >> 8) & 0x1f;
    buffer[2+offset] = handle->settings.videoPid & 0xff;  /* PID */
    buffer[3+offset] = 0x30; /* not scrambled, adaptation_field then payload, 0 continuity counter */
    buffer[4+offset] = 188-5-4; /* leave only 4 bytes of payload */
    BKNI_Memset(&buffer[5+offset], 0, 188-5-4); /* zero out adaptation field */
    buffer[184+offset] = 0x00;
    buffer[185+offset] = 0x00;
    buffer[186+offset] = 0x01;
    buffer[187+offset] = bthumbnail_get_endcode(handle->settings.videoCodec);

    return 188+offset;
}
