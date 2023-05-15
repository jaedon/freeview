/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_probe.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/8/12 5:24p $
 *
 * Module Description:
 *
 * BMedia library, ape stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bape_probe.c $
 * 
 * 2   5/8/12 5:24p vsilyaev
 * SW7425-2378: Added seek support
 * 
 * 1   2/21/12 5:09p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bape_probe.h"
#include "bape_util.h"
#include "bkni.h"

BDBG_MODULE(bape_probe);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

typedef struct bape_probe  *bape_probe_t; 

struct bape_probe {
    BDBG_OBJECT(bape_probe_t)
    unsigned unused;
};

BDBG_OBJECT_ID(bape_probe_t);

static const bmedia_probe_file_ext b_ape_ext[] =  {
    {"ape"},
    {""}
};

static bool
b_ape_probe_header_match(batom_cursor *header)
{
    bape_file_descriptor file_descriptor;

    return bape_parse_file_descriptor(header, &file_descriptor);
}


static bmedia_probe_base_t 
b_ape_probe_create(batom_factory_t factory)
{
    bape_probe_t    probe;

    BSTD_UNUSED(factory);

    BDBG_MSG_TRACE(("b_ape_probe_create"));

    probe = BKNI_Malloc(sizeof(*probe));
    if(!probe) {
        BDBG_ERR(("b_ape_probe_create: can't allocate %u bytes", sizeof(*probe)));
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(probe, bape_probe_t);

    return (bmedia_probe_base_t)probe;

err_alloc:
    return NULL;
}

static void 
b_ape_probe_destroy(bmedia_probe_base_t probe_)
{
    bape_probe_t probe = (bape_probe_t)probe_;
    BDBG_OBJECT_ASSERT(probe, bape_probe_t);

    BDBG_OBJECT_DESTROY(probe, bape_probe_t);
    BKNI_Free(probe);
    return;
}


const bmedia_probe_stream *
b_ape_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
    bape_probe_t probe = (bape_probe_t)probe_;
    bfile_buffer_result result;
    bmedia_probe_stream *stream=NULL;
    bmedia_probe_track *track;
    batom_t atom;
    batom_cursor cursor;
    bape_file_descriptor file_descriptor;
    bape_frame_header frame_header;

    BDBG_OBJECT_ASSERT(probe, bape_probe_t);
    BSTD_UNUSED(pipe);

    BDBG_MSG_TRACE(("b_ape_probe_parse:%#lx", (unsigned long)probe));

    
    atom = bfile_buffer_read(buf, config->parse_offset+0, BAPE_FILE_DESCRIPTOR_LENGTH+BAPE_FRAME_HEADER_LENGTH, &result);
    if(!atom) {
        goto error;
    }
    batom_cursor_from_atom(&cursor, atom);
    if(!bape_parse_file_descriptor(&cursor, &file_descriptor)) {
        goto error;
    }
    if(!bape_parse_frame_header(&cursor, &frame_header) || frame_header.sample_rate==0 ) {
        goto error;
    }
    batom_release(atom);
	stream = BKNI_Malloc(sizeof(*stream));
    if(!stream) {
        goto error;
    }
    bmedia_probe_stream_init(stream, bstream_mpeg_type_ape);
    track = BKNI_Malloc(sizeof(*track));
    if(!track) {
        goto error;
    }
    bmedia_probe_track_init(track);
    stream->index = bmedia_probe_index_required;
    stream->duration = ((frame_header.total_frames * frame_header.blocks_per_frame + frame_header.final_frame_blocks) * (uint64_t)1000)/frame_header.sample_rate;
    track->type = bmedia_track_type_audio;
    track->info.audio.codec = baudio_format_ape;
    track->number = 1;
    track->info.audio.channel_count = frame_header.channels;
    track->info.audio.sample_size = frame_header.bits_per_sample;
    track->info.audio.sample_rate = frame_header.sample_rate;
    track->info.audio.bitrate = 0;
    bmedia_probe_add_track(stream, track); 
    return stream;

error:
    if(stream) {
        BKNI_Free(stream);
    }
    if(atom) {
        batom_release(atom);
    }
    return NULL;
}

const bmedia_probe_format_desc bape_probe = {
    bstream_mpeg_type_es,
    b_ape_ext, /* ext_list */
	BAPE_FILE_DESCRIPTOR_LENGTH,
    b_ape_probe_header_match, /* header_match */
    b_ape_probe_create, /* create */
    b_ape_probe_destroy, /* destroy */
    b_ape_probe_parse, /* parse */
    bmedia_probe_basic_stream_free /* stream free */
};



