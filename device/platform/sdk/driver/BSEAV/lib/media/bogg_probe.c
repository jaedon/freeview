/***************************************************************************
 *     Copyright (c) 2011-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bogg_probe.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 1:00p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bogg_probe.c $
 * 
 * 5   9/6/12 1:00p vsilyaev
 * SW7552-340: Fixed memory leak in the error handling path
 * 
 * 4   9/6/12 12:57p vsilyaev
 * SW7552-340: Fixed memory leak in the error handling path
 * 
 * 3   7/24/12 12:01p vsilyaev
 * SW7425-3575,SW7425-3576: Updated code for better compability with a
 * static analyzer tools
 * 
 * 2   11/11/11 5:24p vsilyaev
 * SW7425-1752: Added FLAC format support
 * 
 * 1   8/25/11 6:17p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bogg_probe.h"
#include "bogg_parser.h"
#include "bkni.h"
#include "biobits.h"
#include "bfile_cache.h"


BDBG_MODULE(bogg_probe);

typedef struct bogg_probe  *bogg_probe_t; 

struct bogg_probe {
    BDBG_OBJECT(bogg_probe)
    batom_factory_t factory;
};

BDBG_OBJECT_ID(bogg_probe);

static bmedia_probe_base_t 
b_ogg_probe_create(batom_factory_t factory)
{
    bogg_probe_t    probe;

    probe = BKNI_Malloc(sizeof(*probe));
    if(!probe) {
        BDBG_ERR(("b_ogg_probe_create: can't allocate %u bytes", sizeof(*probe)));
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(probe, bogg_probe);
    probe->factory = factory;
    return (bmedia_probe_base_t)probe;
err_alloc:
    return NULL;
}

static void 
b_ogg_probe_destroy(bmedia_probe_base_t probe_)
{
    bogg_probe_t probe = (bogg_probe_t) probe_;

    BDBG_OBJECT_ASSERT(probe, bogg_probe);
    BDBG_OBJECT_DESTROY(probe, bogg_probe);
    BKNI_Free(probe);
    return;
}


static const bmedia_probe_stream *
b_ogg_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
    bogg_probe_t probe = (bogg_probe_t)probe_;
    int rc;
    bfile_cached_segment segment;
    bogg_probe_stream *stream;
    bogg_probe_track *track;
    bogg_page_header header;
    int payload_size;
    bool spanning;
    bvorbis_frame_header vorbis_frame_header;
    bvorbis_indentification_header vorbis_indentification_header;
    batom_cursor payload;
    bogg_page_payload_parser parser;

    BSTD_UNUSED(config);
    BSTD_UNUSED(pipe);

    BDBG_OBJECT_ASSERT(probe, bogg_probe);
    rc = bfile_cached_segment_init(&segment, buf, probe->factory, 4);
    if(rc<0) {goto err_segment;}
    bfile_cached_segment_set(&segment, 0, 1*1024*1024);
    if(!bfile_cached_segment_reserve(&segment, BOGG_PAGE_MAX_HEADER_LENGTH)) { goto err_no_data; }
    if(!bogg_parse_page_header(&segment.cursor, &header)) { goto err_no_data;}
    if(header.header_type!=BOGG_HEADER_TYPE_FIRST_PAGE) { goto err_no_data;}
    if(header.page_segments!=1) { goto err_no_data; }
    if(!bfile_cached_segment_reserve(&segment, header.page_segments)) { goto err_no_data; }
    if(!bogg_page_payload_parser_init(&parser, &segment.cursor, &header)) {goto err_no_data;}
    payload_size = bogg_page_payload_parser_next(&parser, &header, &spanning);
    BDBG_MSG(("b_ogg_probe_parse:%#lx first_packet:%d", (unsigned long)probe, payload_size));
    if(payload_size<=0 || spanning) { goto err_no_data;}
    if(!bfile_cached_segment_reserve(&segment, payload_size)) { goto err_no_data; }

    stream = BKNI_Malloc(sizeof(*stream));
    if(!stream) { goto err_stream; }
    bmedia_probe_stream_init(&stream->media, bstream_mpeg_type_ogg);

    BATOM_CLONE(&payload, &segment.cursor);
    if(batom_cursor_skip(&segment.cursor, payload_size)!=(size_t)payload_size) {goto err_stream_no_data;}
    if(!bvorbis_parse_frame_header(&payload, &vorbis_frame_header)) {goto err_stream_no_data;}
    if(vorbis_frame_header.packet_type!=BVORBIS_IDENTIFICATION_HEADER) {goto err_stream_no_data;}
    if(!bvorbis_parse_indentification_header(&payload, &vorbis_indentification_header)) {goto err_stream_no_data;}

    /* return result of parsing */
    track = BKNI_Malloc(sizeof(*track));
    if(!track) { goto err_track_audio; }
    bmedia_probe_track_init(&track->media);
	BLST_SQ_INSERT_TAIL(&stream->media.tracks, &track->media, link);
    track->media.type = bmedia_track_type_audio;
    track->media.number = header.bitstream_serial_number;
    track->media.info.audio.channel_count = vorbis_indentification_header.audio_channels;
    track->media.info.audio.sample_size = 16;
    track->media.info.audio.sample_rate = vorbis_indentification_header.audio_sample_rate;
    track->media.info.audio.bitrate = vorbis_indentification_header.bitrate_nominal/1000;
    track->media.info.audio.codec = baudio_format_vorbis;

    bfile_cached_segment_shutdown(&segment);

    return &stream->media;

err_track_audio:
err_stream_no_data:
    BKNI_Free(stream);
err_stream:
err_no_data:
    bfile_cached_segment_shutdown(&segment);
err_segment:
    return NULL;
}

static bool 
b_ogg_probe_header_match(batom_cursor *cursor)
{
    bogg_page_header header;

    if(!bogg_parse_page_header(cursor, &header)) {
        return false;
    }
    if(header.header_type!=BOGG_HEADER_TYPE_FIRST_PAGE) {
        return false;
    }
    return true;
}

static const bmedia_probe_file_ext b_ogg_ext[] =  {
    {"ogg"},
    {""}
};

const bmedia_probe_format_desc bogg_probe = {
    bstream_mpeg_type_ogg,
    b_ogg_ext, /* ext_list */
    BOGG_PAGE_MAX_HEADER_LENGTH,
    b_ogg_probe_header_match, /* header_match */
    b_ogg_probe_create, /* create */
    b_ogg_probe_destroy, /* destroy */
    b_ogg_probe_parse, /* parse */
    bmedia_probe_basic_stream_free /* stream free */
};

