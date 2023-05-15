/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_stream.c $
 * $brcm_Revision: 51 $
 * $brcm_Date: 10/23/12 12:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_stream.c $
 * 
 * 51   10/23/12 12:21p erickson
 * SW7435-440: use NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE for
 * bstream_pid_type_other
 * 
 * 50   10/9/12 5:17p mward
 * SW7425-3782: use NEXUS_FrontendConnector if NEXUS_HAS_FRONTEND
 * 
 * 49   10/8/12 2:20p erickson
 * SW7425-3782: use NEXUS_FrontendConnector
 * 
 * 48   8/20/12 5:28p mward
 * SW7346-119:  maxDataRate is still set using parserBandSettings for
 * "legacy" XPT.
 * 
 * 47   8/20/12 4:47p mward
 * SW7346-119:  maxDataRate can only be changed via
 * NEXUS_TransportModuleSettings.
 * 
 * 46   7/26/12 5:20p jtna
 * SW7346-943: do upto two fake frontend<->PB mappings in Brutus to handle
 * some PIP usage modes
 * 
 * 45   7/2/12 10:01a katrep
 * SW7425-3350:fixed build error for the platforms with no frontends
 * 
 * 44   6/27/12 2:59p jtna
 * SW7425-3350: rework Brutus MTSIF frontend handling
 * 
 * 43   11/3/11 4:28p erickson
 * SW7425-1383: use GetAllPassPidChannelIndex functions
 * 
 * 42   11/3/11 1:32p erickson
 * SW7231-391: use NEXUS_ParserBand_e0 as proper base
 * 
 * 41   9/28/11 11:13a randyjew
 * SW7344-134:Fix error, rearrange sequence
 * 
 * 40   9/26/11 12:46p mward
 * SW7125-1048:  Set parser band maxDataRate to max_data_rate Mbps instead
 * of using an unconditional increase.
 * 
 * 39   9/19/11 3:58p randyjew
 * SW7344-134: Fix for pcr pid similiar to video/audio pid, open as video
 * or audio pic to avoid duplicate pid channel.
 * 
 * 38   8/5/11 4:28p mward
 * SW7125-1048:  Increase parser band maxDataRate to 30 Mbps to accomodate
 * a particular lipsync test stream.
 * 
 * 37   7/7/11 4:13p erickson
 * SW7425-828: when using PCR in playback, we must consider that it's the
 * same as the index pid, which is handled differently.
 * 
 * 36   12/15/10 10:39a erickson
 * SW7420-941: delete unused and incorrect BLST_Q logic. BLST_Q_REMOVE was
 * being called for a node that wasn't in a list.
 * 
 * 35   3/16/10 10:48a jtna
 * SW3556-1051: revert back to host-reordering
 * 
 * 34   2/16/10 5:42p jtna
 * SW3556-1051: use video decoder timestamp reordering
 * 
 * 33   12/9/09 12:04p gmohile
 * SW7408-1 : Add 7408 support
 * 
 * 32   5/12/09 7:39p bandrews
 * PR54955: qualify timestamp flag with B_HAS_IP
 * 
 * 31   5/11/09 4:34p ssood
 * PR54955: Record TTS streams over HTTP: set flag in stream status to
 * indicate a TTS stream
 * 
 * 30   10/16/08 6:20p katrep
 * PR47690: Adding allpass record from playback parser
 * 
 * 29   9/25/08 1:06p katrep
 * PR47154: Add support for recording with null packets and without null
 * packets
 * 
 * 28   9/22/08 1:59p katrep
 * PR47154: Add support for allpass record
 * 
 * 27   9/19/08 5:06p vishk
 * PR 47150: Coverity Issues.
 * 
 * 26   9/15/08 5:40p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 * 
 * 25   8/14/08 11:24a vishk
 * PR 4537: bstream_vbi_settings structure call-back is not being called
 * when data is ready
 * 
 * 24   7/23/08 1:52p erickson
 * PR44874: fix bstream_close
 *
 * 23   7/22/08 2:59p erickson
 * PR44874: allow static allocation of bstream for playback. this is
 * required to support bstream_close after a bplayback_stop.
 *
 * 22   7/18/08 4:21p erickson
 * PR44919: improve DBG
 *
 * 21   7/2/08 4:59p vishk
 * PR 40020: bsettop_stream (crypto, network decryption): Develop
 * SettopAPI-to-Nexus shim layer
 *
 * 20   5/2/08 9:20a erickson
 * PR42339: fix pid channel alloc for PVR
 *
 * 19   5/1/08 4:08p erickson
 * PR42339: remove unnecessary stream.dynamic
 *
 * 18   4/28/08 11:54a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 17   4/25/08 1:01p erickson
 * PR41951: don't close the nIndexPid if it was reused for video
 *
 * 16   4/15/08 12:09p erickson
 * PR36068: fix analog
 *
 * 15   4/10/08 10:02a erickson
 * PR36068: coverity fix
 *
 * 14   4/9/08 5:54p jgarrett
 * PR 41567: Adding playback video pid management for indexing
 *
 * 13   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 12   3/25/08 10:10a erickson
 * PR36068: added analog tuner
 *
 * 11   3/20/08 12:04p katrep
 * PR40699: Use a new parser band if stream is opened for same input band
 * (eg for PIP )
 *
 * 10   3/11/08 2:05p jrubio
 * PR40019: add pcr functions
 *
 * 9   3/10/08 4:55p katrep
 * PR40019: Move the header files to bsettop_impl.h
 *
 * 8   3/10/08 4:49p katrep
 * PR40019: Inputs bands need to configured correctly for the stream types
 * , mpeg vs dss
 *
 * 7   1/24/08 3:13p jgarrett
 * PR 38919: Renaming NEXUS_Platform_GetStreamerInputBand
 *
 * 6   11/13/07 11:58a erickson
 * PR36068: trick modes working
 *
 * 5   11/12/07 5:26p erickson
 * PR36802: call NEXUS_Platform_GetStreamerInputBand for streamer input
 * band conversion
 *
 * 4   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 3   10/16/07 2:30p erickson
 * PR36068: added refcnt to parser_band for message filtering
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(stream);

#if NEXUS_NUM_INPUT_BANDS
struct bband g_bands[B_MAX_INPUTS];
#endif
#if NEXUS_NUM_PARSER_BANDS
struct bparser_band g_parserBands[B_MAX_PARSERS];
#endif
struct bparser_band g_playbackParserBands[B_PVR_N_PLAYBACKS];
BDBG_OBJECT_ID(bstream);

static void bstream_p_close_pid_priv(bstream_t stream, NEXUS_PidChannelHandle pidChannel, bool force);

unsigned b_get_parser_band(bband_t band)
{
#if NEXUS_NUM_PARSER_BANDS
    unsigned i;
    for (i=0;i<B_MAX_PARSERS;i++) {
        if (g_parserBands[i].band == band) {
            return i;
        }
    }
    BDBG_ERR(("unmapped input band"));
#else
    BSTD_UNUSED(band);
#endif

    return 0;
}

void bstream_p_init()
{
#if NEXUS_NUM_INPUT_BANDS
    unsigned i;
    BKNI_Memset(g_bands, 0, sizeof(g_bands));
    for (i=0;i<B_MAX_INPUTS;i++) {
        bband_t band = &g_bands[i];
        band->nInputBand = (NEXUS_InputBand)i;
        band->mpeg_type = bstream_mpeg_type_ts;
    }
#endif
#if NEXUS_NUM_PARSER_BANDS
    BKNI_Memset(g_parserBands, 0, sizeof(g_parserBands));
    for (i=0;i<B_MAX_PARSERS;i++) {
        struct bparser_band *parser_band = &g_parserBands[i];
        parser_band->nParserBand = NEXUS_ParserBand_e0 + i;
    }
#endif
    BKNI_Memset(g_playbackParserBands, 0, sizeof(g_playbackParserBands));
}

void bstream_mpeg_init(bstream_mpeg *mpeg)
{
    BKNI_Memset(mpeg, 0, sizeof(*mpeg));
    mpeg->video[0].format = bvideo_codec_mpeg2;
    mpeg->audio[0].format = baudio_format_mpeg;
    mpeg->mpeg_type = bstream_mpeg_type_ts;
    bencryption_params_init(&mpeg->encryption);
}

bband_t bstreamer_attach(bobject_t id, bstream_mpeg_type mpeg_type)
{
#if NEXUS_NUM_INPUT_BANDS
    unsigned bandnum = B_ID_GET_INDEX(id);
    bband_t band = NULL;

    if (bandnum < 100) {
        NEXUS_Platform_GetStreamerInputBand(bandnum, &bandnum);
        if (bandnum < B_MAX_INPUTS) {
            band = &g_bands[bandnum];
        }
    }
    else if (bandnum-100 < B_MAX_INPUTS) {
        band = &g_bands[bandnum - 100];
    }

    if (band) {
        /* TODO: config input band */
        band->mpeg_type = mpeg_type;
    }
    else {
        BDBG_ERR(("bstreamer_attach failed for %d (max %d)", bandnum, B_MAX_INPUTS));
        band = NULL;
        BSETTOP_ERROR(berr_not_available);
    }

    band->nFrontend = NULL;
    return band;
#else 
    BSTD_UNUSED(id);
    BSTD_UNUSED(mpeg_type);
    return NULL;
#endif
}

bstream_t bstream_p_open_message(bband_t band, const bstream_mpeg *mpeg)
{
#if NEXUS_NUM_PARSER_BANDS
    unsigned i;

    /* try to use a parser band already in use */
    for (i=0;i<B_MAX_PARSERS;i++) {
        if (g_parserBands[i].band == band) {
            return bstream_p_open(band, NULL, NULL, i, mpeg);
        }
    }

    /* otherwise, alloc a new parser band */
    return bstream_open(band, mpeg);
#else
    BSTD_UNUSED(band);
    BSTD_UNUSED(mpeg);
    return NULL;
#endif
}

bstream_t bstream_open(bband_t band, const bstream_mpeg *mpeg)
{
#if NEXUS_NUM_PARSER_BANDS
    unsigned parser_band, i;

    BDBG_MSG(("bstream_open %d", band->nInputBand));
    /* assigned parser band, preferring straight mapping first */
    parser_band = band->nInputBand;
    /* look for available parser band if this parser band is taken */
    if (g_parserBands[parser_band].band){
        for (i=0;i<B_MAX_PARSERS;i++) {
            if (!g_parserBands[i].band) break;
        }
        if (i == B_MAX_PARSERS) {
            BDBG_ERR(("no more parser bands"));
            return NULL;
        }
        else {
            BDBG_MSG(("Using parser band %d with input band %d",i,band->nInputBand));
            parser_band =i;
        }

    }
    return bstream_p_open(band, NULL, NULL, parser_band, mpeg);
#else
    BSTD_UNUSED(band);
    BSTD_UNUSED(mpeg);
    return NULL;
#endif
}

bstream_t bstream_p_open(bband_t band, bplaypump_t playpump, bplayback_t playback,
    unsigned parser_band, const bstream_mpeg *mpeg)
{
    bstream_t stream;
    bresult result;

    stream = BKNI_Malloc(sizeof(*stream));
    BKNI_Memset(stream, 0, sizeof(*stream));
    BDBG_OBJECT_SET(stream, bstream);

    if (!band) {
        if (playpump) {
            stream->producer.playpump = playpump;
        }
        else {
            BDBG_ASSERT(playback);
            stream->producer.playback = playback;
        }
    }
#if NEXUS_NUM_PARSER_BANDS
    else {
        BDBG_ASSERT(!playpump && !playback);
        BDBG_MSG(("map input band %d to live parser band %d", band->nInputBand, parser_band));
        stream->producer.band = band;
        stream->parser_band = &g_parserBands[parser_band];
        stream->parser_band->band = band;
        stream->parser_band->refcnt++;
        BDBG_MSG(("  refcnt %d", stream->parser_band->refcnt));
    }
#else
    BSTD_UNUSED(parser_band);
#endif
    result = bstream_p_set(stream, mpeg);
    if (result) {result = BERR_TRACE(result); goto err_set;}

    return stream;

err_set:
    (void)bstream_p_set(stream, NULL);
    BDBG_OBJECT_DESTROY(stream, bstream);
    BKNI_Free(stream);
    return NULL;
}

bresult bstream_p_set(bstream_t stream, const bstream_mpeg *mpeg)
{
    bresult result;

    BDBG_OBJECT_ASSERT(stream, bstream);

#if NEXUS_NUM_PARSER_BANDS
    if (stream->parser_band) {
            NEXUS_ParserBandSettings parserBandSettings;
            NEXUS_ParserBand_GetSettings(stream->parser_band->nParserBand, &parserBandSettings);
        if (mpeg) {
            /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
#if NEXUS_HAS_FRONTEND
            if (stream->producer.band->nFrontend!=NULL) {
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(stream->producer.band->nFrontend);
            }
            else 
#endif
			{
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				parserBandSettings.sourceTypeSettings.inputBand = stream->parser_band->band->nInputBand;
            }
            switch(mpeg->mpeg_type)
            {
            case bstream_mpeg_type_ts:
                parserBandSettings.transportType = NEXUS_TransportType_eTs;
                break;
            case bstream_mpeg_type_dss_es:
                parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
                break;
            case bstream_mpeg_type_dss_pes:
                parserBandSettings.transportType = NEXUS_TransportType_eDssPes;
                break;
            default:
                BDBG_ERR(("Unsupported mpeg_type %d",mpeg->mpeg_type));
            }

            if (NEXUS_GetEnv("max_data_rate")) {
                parserBandSettings.maxDataRate = NEXUS_atoi(NEXUS_GetEnv("max_data_rate"));
                BDBG_MSG(("max_data_rate %d Mbps",parserBandSettings.maxDataRate));
                parserBandSettings.maxDataRate = parserBandSettings.maxDataRate * 1024 * 1024;
            }
        }
        else {
            /* we need parser band get default settings ???? */
            parserBandSettings.acceptNullPackets=false;
            parserBandSettings.allPass=false;
        }
        NEXUS_ParserBand_SetSettings(stream->parser_band->nParserBand, &parserBandSettings);

        if (stream->producer.band->nFrontend) {
            NEXUS_ParserBand dummy = NEXUS_NUM_PARSER_BANDS-1;
            /* undo the fake mapping */
            NEXUS_ParserBand_GetSettings(dummy, &parserBandSettings);
            parserBandSettings.sourceTypeSettings.mtsif = NULL;
            NEXUS_ParserBand_SetSettings(dummy, &parserBandSettings);

            dummy = NEXUS_NUM_PARSER_BANDS-2;
            NEXUS_ParserBand_GetSettings(dummy, &parserBandSettings);
            if (parserBandSettings.sourceTypeSettings.mtsif!=NULL) {
                parserBandSettings.sourceTypeSettings.mtsif = NULL;
                NEXUS_ParserBand_SetSettings(dummy, &parserBandSettings);
            }
#if NEXUS_HAS_FRONTEND
            /* reapply both the fake and real mapping */
            NEXUS_Frontend_ReapplyTransportSettings(stream->producer.band->nFrontend);
#endif
        }
    }
#endif
#if NEXUS_HAS_PLAYBACK
    if ( stream->producer.playback )
    {
        if (mpeg)
        {
            if ( mpeg->video[0].pid > 0 && mpeg->video[0].pid < 0x1fff )
            {
                NEXUS_PlaybackPidChannelSettings pidChannelSettings;
                NEXUS_Playback_GetDefaultPidChannelSettings(&pidChannelSettings);
#if 0 /* TODO: decoder feature is being reworked. use host-reordering in the meantime */
                pidChannelSettings.pidSettings.allowTimestampReordering = false; /* turn it off at host and enable it at decoder. this is the default now */
#endif
                pidChannelSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
                pidChannelSettings.pidTypeSettings.video.codec = b_videocodec2nexus(mpeg->video[0].format);
                pidChannelSettings.pidTypeSettings.video.decoder = NULL;        /* Decode will set this later */
                pidChannelSettings.pidTypeSettings.video.index = true;
                BDBG_ASSERT(!stream->nIndexPid); /* if static_alloc is used correctly in the shim, we will never realloc this */
                BDBG_MSG(("Opening playback video pid channel for pid %#x",mpeg->video[0].pid));
                stream->nIndexPid = NEXUS_Playback_OpenPidChannel(stream->producer.playback->nPlayback, mpeg->video[0].pid, &pidChannelSettings);
                if ( NULL == stream->nIndexPid )
                {
                    BDBG_ERR(("Unable to open video index pid"));
                    return BSETTOP_ERROR(berr_external_error);
                }
            }
        }
        else
        {
            if ( stream->nIndexPid )
            {
                BDBG_ASSERT(NULL != stream->producer.playback);
                NEXUS_Playback_ClosePidChannel(stream->producer.playback->nPlayback, stream->nIndexPid);
                if (stream->pcrPidChannel == stream->nIndexPid) {
                    stream->pcrPidChannel = NULL;
                }
                stream->nIndexPid = NULL;
            }
        }
    }
#endif

    /* TODO: should we call bstream_set_mpeg_parameters if mpeg is NULL or not? */
        if (mpeg) {
            /* Set settings */
            result = bstream_set_mpeg_parameters(stream, mpeg);
            if (result)
            {
                BSETTOP_ERROR(result);
                goto err_set;
            }
        }

    /* open PCR pid for digital streams. */
    if (!stream->producer.tuner && mpeg && mpeg->pcr_pid != 0 && mpeg->pcr_pid < 0x1fff) {
        /* if PCR pid is same as video or audio, open as that type. avoids duplicate pid channel open */
        if (mpeg->pcr_pid == mpeg->video[0].pid) {
            if (stream->nIndexPid) {
                if (stream->pcrPidChannel && stream->pcrPidChannel != stream->nIndexPid) {
                    bstream_p_close_pid_priv(stream, stream->pcrPidChannel, true);
                }
                stream->pcrPidChannel = stream->nIndexPid;
            }
            else {
                stream->pcrPidChannel = bstream_p_open_pid(stream, mpeg->pcr_pid, bstream_pid_type_video);
            }
        }
        else if (mpeg->pcr_pid == mpeg->audio[0].pid) {
            stream->pcrPidChannel = bstream_p_open_pid(stream, mpeg->pcr_pid, bstream_pid_type_audio);
        }
        else {
            stream->pcrPidChannel = bstream_p_open_pid(stream, mpeg->pcr_pid, bstream_pid_type_pcr);
        }
        if (!stream->pcrPidChannel) BDBG_ERR(("unable to open PCR pid %04x", mpeg->pcr_pid)); /* fall through */
    }
    else {
        if (stream->pcrPidChannel) {
            bstream_p_close_pid_priv(stream, stream->pcrPidChannel, true);
            stream->pcrPidChannel = NULL;
        }
    }

    

    return 0;

err_set:
    if (stream->pcrPidChannel) {
        if (stream->pcrPidChannel != stream->nIndexPid) {
            bstream_p_close_pid_priv(stream, stream->pcrPidChannel, false);
        }
        stream->pcrPidChannel = NULL;
    }
#if NEXUS_HAS_PLAYBACK
    if ( stream->nIndexPid )
    {
        NEXUS_Playback_ClosePidChannel(stream->producer.playback->nPlayback, stream->nIndexPid);
        stream->nIndexPid = NULL;
    }
#endif
    return result;
}

void bstream_close(bstream_t stream)
{
    BDBG_OBJECT_ASSERT(stream, bstream);
    /* NOTE: the _p_close adds no value now, but was useful for previous features. it's harmless to keep it. */
    bstream_p_close(stream);
}

void bstream_p_close(bstream_t stream)
{
    BDBG_OBJECT_ASSERT(stream, bstream);
    
    bstream_p_stop_consumers(stream);

    if (stream->cc_data_ready_timer) {
        stream->vbi_settings.cc_data_ready_callback = NULL;
        stream->vbi_settings.callback_context = NULL;
        b_timer_cancel(stream->cc_data_ready_timer);
        stream->cc_data_ready_timer = NULL;
    }
    
    /* this frees resources allocated by previous sets */
    (void)bstream_p_set(stream, NULL);

    if (!stream->static_alloc) {
        if ( stream->parser_band && 0 == --stream->parser_band->refcnt )
        {
            stream->parser_band->band = NULL;
        }

        BDBG_OBJECT_DESTROY(stream, bstream);
        BKNI_Free(stream);
    }
}

bresult bstream_get_mpeg_parameters(bstream_t stream, bstream_status *status)
{
    BDBG_OBJECT_ASSERT(stream, bstream);
    BKNI_Memset(status, 0, sizeof(*status));

    if (stream->parser_band) {
        status->band = stream->producer.band;
        status->parser_band = stream->parser_band->nParserBand - NEXUS_ParserBand_e0;
    }
    status->mpeg = stream->mpeg;
    status->decode = stream->consumers.decode;
#if B_HAS_IP
    status->timestamp_enabled = stream->producer.timestamp_enabled;
#endif
    return 0;
}

bresult
bstream_set_mpeg_parameters(bstream_t stream, const bstream_mpeg *mpeg)
{
    BDBG_OBJECT_ASSERT(stream, bstream);
    BDBG_ASSERT(NULL != mpeg);

    /* Notify consumers of change before storing settings */
    if ( stream->consumers.decode )
    {
        bdecode_p_mpeg_change(stream->consumers.decode, mpeg);
    }
    if ( stream->consumers.recpump )
    {
        brecpump_p_mpeg_change(stream->consumers.recpump, mpeg);
    }
    if ( stream->consumers.record )
    {
        brecord_p_mpeg_change(stream->consumers.record, mpeg);
    }

    stream->mpeg = *mpeg;

    return b_ok;
}

void bband_get(bband_t band, bband_settings *settings)
{
    settings->bandnum = band->nInputBand;
    settings->mpeg_type = band->mpeg_type;
    settings->tuner = band->tuner;
}

void bstream_join(bstream_t video_source, bstream_t audio_source)
{
    BSTD_UNUSED(video_source);
    BSTD_UNUSED(audio_source);
    BSETTOP_ERROR(berr_not_supported);
}

bstream_t bstream_open_child(bstream_t parent, const bstream_mpeg *mpegparams)
{
    BSTD_UNUSED(parent);
    BSTD_UNUSED(mpegparams);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

bresult bstream_start_pcr_monitor(bstream_t stream)
{
    bresult berr = b_ok;

    BDBG_OBJECT_ASSERT(stream, bstream);

    if (stream->consumers.decode) {
        NEXUS_Error err = NEXUS_SUCCESS;
        NEXUS_TimebaseStatus pStatus;
        NEXUS_StcChannelSettings pSettings;
        NEXUS_StcChannelHandle handle = stream->consumers.decode->stcChannel;

        NEXUS_StcChannel_GetSettings( handle, &pSettings);

        err = NEXUS_Timebase_GetStatus(pSettings.timebase, &pStatus);
        if (err != NEXUS_SUCCESS)
            berr = berr_external_error;

        stream->pcr_status.monitor_started = true;
        stream->pcr_status.pcr_count = pStatus.pcrCount;
        stream->pcr_status.pcr_valid = pStatus.pcrValid;
    }
    else {
        BKNI_Memset(&stream->pcr_status, 0, sizeof(stream->pcr_status));
    }

    return berr;
}

void bstream_stop_pcr_monitor(bstream_t stream)
{
    /* Stop the monitoring */
    stream->pcr_status.monitor_started = false;
}

bresult bstream_get_pcr_status(bstream_t stream, bstream_pcr_status *status)
{
    NEXUS_Error err = NEXUS_SUCCESS;
    bresult berr = b_ok;

    BDBG_OBJECT_ASSERT(stream, bstream);
    BKNI_Memset(status, 0, sizeof(*status));

    if (stream->consumers.decode) {
        NEXUS_TimebaseStatus pStatus;
        NEXUS_StcChannelSettings pSettings;
        NEXUS_StcChannelHandle handle;

        handle = stream->consumers.decode->stcChannel;
        NEXUS_StcChannel_GetSettings( handle, &pSettings);

        err = NEXUS_Timebase_GetStatus(pSettings.timebase, &pStatus);
        if (err != NEXUS_SUCCESS)
            berr = berr_external_error;

        if( stream->pcr_status.monitor_started)
        {
            status->monitor_started = stream->pcr_status.monitor_started;
            status->pcr_count = pStatus.pcrCount - stream->pcr_status.pcr_count;
            status->pcr_valid  = pStatus.pcrValid;
        }
    }
    else {
        BDBG_WRN(("pcr_status requires current decode"));
    }


    return berr;
}

NEXUS_PidChannelHandle bstream_p_open_pid(bstream_t stream, uint16_t pid, bstream_pid_type type)
{
    NEXUS_PidChannelHandle pidChannel=NULL;
    NEXUS_PidType nexusType;
    int index=0;
    NEXUS_AudioCodec audioCodec=NEXUS_AudioCodec_eUnknown;
    NEXUS_VideoCodec videoCodec=NEXUS_VideoCodec_eUnknown;

    BDBG_OBJECT_ASSERT(stream, bstream);
    
    if (pid == stream->mpeg.pcr_pid && stream->pcrPidChannel) {
        return stream->pcrPidChannel;
    }
    else if (pid == stream->mpeg.video[0].pid && stream->nIndexPid) {
        return stream->nIndexPid;
    }

    switch ( type )
    {
    case bstream_pid_type_audio:
        nexusType = NEXUS_PidType_eAudio;
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if ( stream->mpeg.audio[index].pid == pid )
            {
                audioCodec = b_audiocodec2nexus(stream->mpeg.audio[index].format);
                break;
            }            
        }
        if ( index >= BSETTOP_MAX_PROGRAMS )
        {
            BDBG_ERR(("Audio program not in bstream_mpeg"));
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
        break;
    case bstream_pid_type_video:
        nexusType = NEXUS_PidType_eVideo;
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if ( stream->mpeg.video[index].pid == pid )
            {
                videoCodec = b_videocodec2nexus(stream->mpeg.video[index].format);
                break;
            }            
        }
        if ( index >= BSETTOP_MAX_PROGRAMS )
        {
            BDBG_ERR(("Video program not in bstream_mpeg"));
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
        break;
    default:
        /* use eOther for PCR */
        nexusType = NEXUS_PidType_eOther;
        break;
    }

    if ( stream->producer.band )
    {
        NEXUS_PidChannelSettings settings;

        NEXUS_PidChannel_GetDefaultSettings(&settings);
        BDBG_MSG(("Opening live pid channel for PID 0x%x, type %d", pid, type));
        BDBG_ASSERT(stream->parser_band);
        /* bstream_pid_type_other and 0x1fff,0xffff pid has special meaning, for all pass record */
        if(type == bstream_pid_type_other && (pid==0x1FFF || pid==0xFFFF))
        {
            unsigned temp;
            BDBG_MSG(("Opening pid channnel for allpass mode"));
            NEXUS_ParserBand_GetAllPassPidChannelIndex(stream->parser_band->nParserBand, &temp);
            settings.pidChannelIndex = temp;
            pid=0x1fff; /* xpt doesnt all pid number 0xffff */
        }
        else if (type == bstream_pid_type_other) {
            /* could be called by bmessage_start, so request only msg-capable pid channels */
            settings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
        }
        pidChannel = NEXUS_PidChannel_Open(stream->parser_band->nParserBand, pid, &settings);
    }
    else if ( stream->producer.playpump )
    {
        NEXUS_PlaypumpOpenPidChannelSettings settings;

        BDBG_ASSERT(!stream->parser_band);
        NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);
        settings.pidType = nexusType;
        if ( nexusType == NEXUS_PidType_eAudio )
        {
            settings.pidTypeSettings.audio.codec = audioCodec;
        }
        else if(type == bstream_pid_type_other && (pid==0x1FFF || pid==0xFFFF))
        {
            NEXUS_PlaypumpStatus playpumpStatus;
            unsigned temp;
            NEXUS_Playpump_GetStatus(stream->producer.playpump->nPlaypump,&playpumpStatus);
            BDBG_MSG(("Opening pid channnel for allpass mode"));
            NEXUS_Playpump_GetAllPassPidChannelIndex(stream->producer.playpump->nPlaypump, &temp);
            settings.pidSettings.pidChannelIndex = temp;
            pid=0x1fff; /* xpt doesnt all pid number 0xffff */
        }
        BDBG_MSG(("Opening playpump pid channel for PID 0x%x, type %d", pid, pid, type));
        pidChannel = NEXUS_Playpump_OpenPidChannel(stream->producer.playpump->nPlaypump, pid, &settings);
    }
#if NEXUS_HAS_PLAYBACK
    else if (stream->producer.playback)
    {
        NEXUS_PlaybackPidChannelSettings settings;

        BDBG_ASSERT(!stream->parser_band);
        NEXUS_Playback_GetDefaultPidChannelSettings(&settings);
        settings.pidSettings.pidType = nexusType;
        settings.pidTypeSettings.video.decoder = NULL; /* this gets set in bsettop_decode */

        if ( nexusType == NEXUS_PidType_eAudio )
        {
            settings.pidTypeSettings.audio.primary = NULL; /* this gets set in bsettop_decode */
            settings.pidTypeSettings.audio.secondary = NULL; /* this gets set in bsettop_decode */
            settings.pidSettings.pidTypeSettings.audio.codec = audioCodec;
        }
        else if ( nexusType == NEXUS_PidType_eVideo )
        {
            settings.pidTypeSettings.video.codec = videoCodec;
            settings.pidTypeSettings.video.index = false;           /* We only index the first video pid by convention */
        }
        else if ( nexusType == NEXUS_PidType_eOther && !(pid==0x1FFF || pid==0xFFFF))
        {
        }
        else
        {
            NEXUS_PlaybackSettings playbackSettings;
            NEXUS_PlaypumpStatus playpumpStatus;
            unsigned temp;
            NEXUS_Playback_GetSettings(stream->producer.playback->nPlayback, &playbackSettings);
            NEXUS_Playpump_GetStatus(playbackSettings.playpump,&playpumpStatus);
            BDBG_MSG(("Opening pid channnel for allpass mode"));
            /* pid channel index need to be same as parser  band */
            NEXUS_Playpump_GetAllPassPidChannelIndex(playbackSettings.playpump, &temp);
            settings.pidSettings.pidSettings.pidChannelIndex = temp; 
            pid=0x1fff; /* xpt doesnt all pid number 0xffff */
        }
        BDBG_MSG(("Opening playback pid channel for PID 0x%x, type %d", pid, type));
        pidChannel = NEXUS_Playback_OpenPidChannel(stream->producer.playback->nPlayback, pid, &settings);
    }
#endif
    else
    {
        BDBG_ERR(("Invalid stream configuration"));
        BSETTOP_ERROR(berr_external_error);
        return NULL;
    }

    if ( NULL == pidChannel )
    {
        BDBG_ERR(("Unable to open pid 0x%x (%d)", pid, pid));
        BSETTOP_ERROR(berr_external_error);
    }

    return pidChannel;
}

static void bstream_p_close_pid_priv(bstream_t stream, NEXUS_PidChannelHandle pidChannel, bool force)
{
    BDBG_OBJECT_ASSERT(stream, bstream);
    BDBG_ASSERT(NULL != pidChannel);
    
    if (!force) {
        /* the index pid and pcr pid is closed with the stream */
        if (pidChannel == stream->nIndexPid || pidChannel == stream->pcrPidChannel) {
            return;
        }
    }

    if ( stream->producer.band )
    {
        NEXUS_PidChannel_Close(pidChannel);
    }
    else if ( stream->producer.playpump )
    {
        NEXUS_Playpump_ClosePidChannel(stream->producer.playpump->nPlaypump, pidChannel);
    }
#if NEXUS_HAS_PLAYBACK
    else if (stream->producer.playback)
    {
        NEXUS_Playback_ClosePidChannel(stream->producer.playback->nPlayback, pidChannel);
    }
#endif
    else
    {
        BDBG_ERR(("Invalid stream configuration.  Stream must have a producer while pid channels are open."));
        BDBG_ASSERT(false);
    }
}

void bstream_p_close_pid(bstream_t stream, NEXUS_PidChannelHandle pidChannel)
{
    bstream_p_close_pid_priv(stream, pidChannel, false);
}

void bstream_p_stop_consumers(bstream_t stream)
{
    BDBG_OBJECT_ASSERT(stream, bstream);

    BDBG_MSG(("Stopping all consumers"));

    if (stream->consumers.decode)
    {
        bdecode_stop(stream->consumers.decode);
        BDBG_ASSERT(stream->consumers.decode == NULL);
    }
    if ( stream->consumers.still )
    {
        bdecode_stop(stream->consumers.still);
        BDBG_ASSERT(stream->consumers.still == NULL);
    }
    if ( stream->consumers.recpump )
    {
        brecpump_stop(stream->consumers.recpump);
        BDBG_ASSERT(stream->consumers.recpump == NULL);
    }
    if ( stream->consumers.record )
    {
        brecord_stop(stream->consumers.record);
        BDBG_ASSERT(stream->consumers.record == NULL);
    }
}

