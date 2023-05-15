/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwav_probe.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 2/21/12 5:14p $
 *
 * Module Description:
 *
 * BMedia library, wav stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bwav_probe.c $
 * 
 * 12   2/21/12 5:14p vsilyaev
 * SW7425-2422: Added support for PCM (BE/twos) audio in MP4 container
 * 
 * 11   8/8/11 11:45a vsilyaev
 * SWDTV-7845: Improved handling of extra-low bitrates
 * 
 * 10   7/11/11 4:57p vsilyaev
 * SWDTV-7845: Added WAV player
 * 
 * 9   3/3/11 4:39p vsilyaev
 * SWBLURAY-24399: Merged into mainline
 * 
 * 8   3/2/11 1:03p vsilyaev
 * SWBLURAY-24399: Simplified parsing of DST-CD data
 * 
 * 7   3/2/11 12:32p vsilyaev
 * SWBLURAY-24399: Merged DTS-CD support
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/2   3/3/11 12:36p ialauder
 * If we determine that this is a DTS-CD track in a WAV, then make the
 * stream type as ES.
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:43p ialauder
 * DTS-CD wav files appear like PCM files, but contain DTS data.  We need
 * to parse the data to ensure there is no DTS sync words.
 * 
 * 6   10/8/10 12:56p vsilyaev
 * SW3556-1201: Enabled detecting of dts/mpeg/ac3 audio
 * 
 * 5   6/1/10 5:52p vsilyaev
 * SW7405-4408: Moved code that could potentially dereference 'freed'
 * pointer
 * 
 * 4   4/15/10 12:27p vsilyaev
 * SWBLURAY-20165: Added support to extract multichannel PCM audio
 * 
 * 3   4/8/10 3:46p jtna
 * SW7125-292: Coverity Defect ID:20566 USE_AFTER_FREE
 * 
 * 2   1/25/10 12:39p vsilyaev
 * SWBLURAY-18710: Fixed warnings
 * 
 * 1   7/15/09 7:20p vsilyaev
 * PR 55653: Support for WAV files
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bwav_probe.h"
#include "bkni.h"
#include "briff_parser.h"

BDBG_MODULE(bwav_probe);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

typedef struct bwav_probe  *bwav_probe_t; 

typedef struct b_wav_probe_object_handler {
	briff_parser_handler handler; /* must be first */
	bwav_probe_t probe; /* pointer to the probe */ 
} b_wav_probe_object_handler;

struct bwav_probe {
    BDBG_OBJECT(bwav_probe_t)
    briff_parser_t riff_parser;
    bmedia_probe_track *track;
    bmedia_probe_stream *stream;
    b_wav_probe_object_handler fmt_handler;
    b_wav_probe_object_handler data_handler;
};

BDBG_OBJECT_ID(bwav_probe_t);

static const bmedia_probe_file_ext b_wav_ext[] =  {
    {"wav"},
    {""}
};

static bool
b_wav_probe_header_match(batom_cursor *header)
{
	briff_chunk_id riff;
	briff_chunk_id wav;
	
	riff = batom_cursor_uint32_le(header);
	batom_cursor_skip(header, sizeof(briff_atom) /* size */);
	wav = batom_cursor_uint32_le(header);
	if(BATOM_IS_EOF(header)) {
		return false;
	}
	return riff==B_RIFF_RIFF && BMEDIA_FOURCC_WAVE(wav);
}

static briff_parser_action 
b_wav_probe_file_type(void *context, briff_chunk_id file_type)
{
    bwav_probe_t    probe  = context;
    BSTD_UNUSED(probe);
    BDBG_OBJECT_ASSERT(probe, bwav_probe_t);

    return BMEDIA_FOURCC_WAVE(file_type) ? briff_parser_action_none : briff_parser_action_return;
}



static briff_parser_action 
b_wav_probe_fmt(briff_parser_handler *handler, briff_chunk_id chunk_id, unsigned object_size, unsigned object_offset, batom_t object )
{
    bwav_probe_t    probe = ((b_wav_probe_object_handler *)handler)->probe;
    briff_parser_action  action = briff_parser_action_return;

    BDBG_OBJECT_ASSERT(probe, bwav_probe_t);
    BDBG_MSG(("b_wav_probe_fmt:%#lx probe " BMEDIA_FOURCC_FORMAT " %u:%u:%#lx(%u)",  (unsigned long)probe, BMEDIA_FOURCC_ARG(chunk_id), object_size, object_offset, (unsigned long)object, object?batom_len(object):0));
    if(probe->track) {
        /* duplicated FMT record */
        goto done;
    }
    if(chunk_id==BMEDIA_FOURCC('f','m','t',' ') && object && object_offset==0 && object_size==batom_len(object)) {
	    bmedia_waveformatex fmt;
        batom_cursor cursor;
		bmedia_probe_track *track;

        batom_cursor_from_atom(&cursor, object);
        if(!bmedia_read_waveformatex(&fmt, &cursor)) {
            goto done;
        }
		track = BKNI_Malloc(sizeof(*track));
        if(!track) {
            goto done;
        }
	    bmedia_probe_track_init(track);
        track->type = bmedia_track_type_audio;

        if(BMEDIA_WAVFMTEX_AUDIO_PCM(&fmt)) {
            track->info.audio.codec = baudio_format_pcm;
        } else {
            track->info.audio.codec = baudio_format_unknown;
            switch(fmt.wFormatTag) {
            default:
                break;
            /* these should carry just raw es payload, and it's handled in b_wav_filter_generic */
            case 0x0050:
                track->info.audio.codec = baudio_format_mpeg;
                break;
            case 0x0055:
                track->info.audio.codec = baudio_format_mp3;
                break;
            case 0x2000:
                track->info.audio.codec = baudio_format_ac3;
                break;
            case 0x2001:
                track->info.audio.codec = baudio_format_dts;
                break;
            }
        }
        track->number = 1;
        track->info.audio.channel_count = fmt.nChannels;
	    track->info.audio.sample_size = fmt.wBitsPerSample;
	    track->info.audio.sample_rate = fmt.nSamplesPerSec;
        track->info.audio.bitrate = (fmt.nAvgBytesPerSec*8)/1000;
        probe->track = track;
        action = briff_parser_action_none;
    }
done:
    if(object) {
        batom_release(object);
    }
    return action;
}

static briff_parser_action 
b_wav_probe_data(briff_parser_handler *handler, briff_chunk_id chunk_id, unsigned object_size, unsigned object_offset, batom_t object )
{
    bwav_probe_t probe = ((b_wav_probe_object_handler *)handler)->probe;
    briff_parser_action  action = briff_parser_action_return;
	bmedia_probe_stream *stream;

    BSTD_UNUSED(chunk_id);
    BSTD_UNUSED(object_offset);
    BDBG_OBJECT_ASSERT(probe, bwav_probe_t);
    BDBG_MSG(("b_wav_probe_data:%#lx probe " BMEDIA_FOURCC_FORMAT " %u:%u:%#lx(%u)",  (unsigned long)probe, BMEDIA_FOURCC_ARG(chunk_id), object_size, object_offset, (unsigned long)object, object?batom_len(object):0));
    if(!probe->track) {
        /* no FMT record */
        goto done;
    }
	stream = BKNI_Malloc(sizeof(*stream));
    if(!stream) {
        goto done;
    }
    bmedia_probe_stream_init(stream, bstream_mpeg_type_wav);
    if(probe->track && probe->track->info.audio.bitrate>0) {
        stream->index = bmedia_probe_index_self;
        stream->max_bitrate = probe->track->info.audio.bitrate*1000;
        stream->duration = (object_size*(uint64_t)8)/probe->track->info.audio.bitrate;

        /* Need to verify it is not a DTS-CD track */
        if (probe->track->info.audio.codec == baudio_format_pcm){
            batom_cursor cursor;
            batom_cursor_from_atom(&cursor, object);
            if (bmedia_is_dts_cd(&cursor)) {
                probe->track->info.audio.channel_count = 6;
                probe->track->info.audio.codec = baudio_format_dts_cd;
                stream->type = bstream_mpeg_type_es;
            }
        }
    }
    bmedia_probe_add_track(stream, probe->track); 
    probe->track = NULL;
    probe->stream = stream;

done:
    if(object) {
        batom_release(object);
    }
    return action;
}


static bmedia_probe_base_t 
b_wav_probe_create(batom_factory_t factory)
{
    bwav_probe_t    probe;
    briff_parser_cfg riff_cfg;

    BDBG_MSG_TRACE(("b_wav_probe_create"));

    probe = BKNI_Malloc(sizeof(*probe));
    if(!probe) {
        BDBG_ERR(("b_wav_probe_create: can't allocate %u bytes", sizeof(*probe)));
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(probe, bwav_probe_t);
    probe->track = NULL;
    probe->stream = NULL;
    briff_parser_default_cfg(&riff_cfg);
    riff_cfg.user_cntx = probe;
    riff_cfg.file_type = b_wav_probe_file_type;
	probe->riff_parser = briff_parser_create(factory, &riff_cfg);
	if(!probe->riff_parser) {
		goto err_create_wav_probe;
	}
	probe->fmt_handler.probe = probe;
	probe->data_handler.probe = probe;
	briff_parser_install_handler(probe->riff_parser, &probe->fmt_handler.handler, BMEDIA_FOURCC('f','m','t',' '), b_wav_probe_fmt); 
	briff_parser_install_handler(probe->riff_parser, &probe->data_handler.handler, BMEDIA_FOURCC('d','a','t','a'), b_wav_probe_data); 

    return (bmedia_probe_base_t)probe;

err_create_wav_probe:
    BKNI_Free(probe);
err_alloc:
    return NULL;
}

static void 
b_wav_probe_destroy(bmedia_probe_base_t probe_)
{
    bwav_probe_t probe = (bwav_probe_t)probe_;
    BDBG_OBJECT_ASSERT(probe, bwav_probe_t);

	briff_parser_remove_handler(probe->riff_parser, &probe->fmt_handler.handler);
	briff_parser_remove_handler(probe->riff_parser, &probe->data_handler.handler);

    briff_parser_destroy(probe->riff_parser);
    BDBG_OBJECT_DESTROY(probe, bwav_probe_t);
    BKNI_Free(probe);
    return;
}


const bmedia_probe_stream *
b_wav_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
    bwav_probe_t probe = (bwav_probe_t)probe_;
    size_t read_len = BMEDIA_PROBE_FEED_SIZE;
    bfile_buffer_result result;
    bmedia_probe_stream *stream=NULL;
    batom_t atom;


    BDBG_OBJECT_ASSERT(probe, bwav_probe_t);

    BDBG_MSG_TRACE(("b_wav_probe_parse:%#lx", (unsigned long)probe));

    if (config->min_parse_request != 0) {
        read_len = config->min_parse_request;
    }
    
    atom = bfile_buffer_read(buf, config->parse_offset+0, read_len, &result);
    if(!atom) {
        goto error;
    }
    batom_pipe_push(pipe, atom);
    briff_parser_feed(probe->riff_parser, pipe);
    stream = probe->stream;
    if(probe->track) {
        BKNI_Free(probe->track);
        probe->track = NULL;
    }

error:
    briff_parser_reset(probe->riff_parser);
    batom_pipe_flush(pipe);
    return stream;
}

const bmedia_probe_format_desc bwav_probe = {
    bstream_mpeg_type_es,
    b_wav_ext, /* ext_list */
	/* RIFF 			            size				WAV */
	sizeof(briff_chunk_id)+sizeof(briff_atom)+sizeof(briff_chunk_id), /* header_size */
    b_wav_probe_header_match, /* header_match */
    b_wav_probe_create, /* create */
    b_wav_probe_destroy, /* destroy */
    b_wav_probe_parse, /* parse */
    bmedia_probe_basic_stream_free /* stream free */
};



