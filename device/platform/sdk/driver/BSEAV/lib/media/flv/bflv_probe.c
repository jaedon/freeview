/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bflv_probe.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 9/15/11 5:53p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/flv/bflv_probe.c $
 * 
 * 10   9/15/11 5:53p jtna
 * SWDTV-8677: add check for invalid last_tag location
 * 
 * 9   8/25/11 6:21p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 8   8/16/11 11:50a vsilyaev
 * SW7425-938: Added mapping for SH263 and VP8 video codecs
 * 
 * 7   8/8/11 11:51a vsilyaev
 * SWBLURAY-22800: Improved error handling in
 * b_media_probe_aquire_flv_duration
 * 
 * 6   6/29/11 8:55p vsilyaev
 * SW7231-268: Added support for AAC audio and improved timestamp handling
 * for AVC video
 * 
 * 5   6/6/11 4:30p vsilyaev
 * SW7422-454: Added S263 support
 * 
 * 4   5/3/11 5:02p vsilyaev
 * SWBLURAY-22800: Refactored support of H.264 in FLV
 * 
 * 3   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 2   9/3/08 6:14p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 1   7/11/08 3:57p vsilyaev
 * PR 31188: Added probe module for the FLV files
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bflv_probe.h"
#include "bflv_parser.h"
#include "bkni.h"
#include "biobits.h"


BDBG_MODULE(bflv_probe);

typedef struct bflv_probe  *bflv_probe_t; 

typedef struct b_flv_probe_object_handler {
	bflv_parser_handler handler; /* must be first */
	bflv_probe_t probe; /* pointer to demux */ 
    bool found;
    uint8_t meta;
} b_flv_probe_object_handler;

struct bflv_probe {
	BDBG_OBJECT(bflv_probe_t)
	bflv_parser_t parser;
    b_flv_probe_object_handler video;
    b_flv_probe_object_handler audio;
};

BDBG_OBJECT_ID(bflv_probe_t);

static bflv_parser_action 
b_flv_probe_data(bflv_parser_handler *handler_, batom_t object, uint8_t meta )
{
    b_flv_probe_object_handler *handler = ( b_flv_probe_object_handler *)handler_;

	BDBG_OBJECT_ASSERT(handler->probe, bflv_probe_t);
    batom_release(object);
    handler->found = true;
    handler->meta = meta;
    return bflv_parser_action_none;
}


static bmedia_probe_base_t 
b_flv_probe_create(batom_factory_t factory)
{
	bflv_probe_t	probe;
	bflv_parser_cfg cfg;

	probe = BKNI_Malloc(sizeof(*probe));
	if(!probe) {
		BDBG_ERR(("b_flv_probe_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, bflv_probe_t);
	probe->video.probe = probe;
	probe->audio.probe = probe;
	bflv_parser_default_cfg(&cfg);
	probe->parser = bflv_parser_create(factory, &cfg);
	if(!probe->parser) {
		goto err_parser;
	}
	bflv_parser_install_handler(probe->parser, &probe->audio.handler, 8, b_flv_probe_data); 
	bflv_parser_install_handler(probe->parser, &probe->video.handler, 9, b_flv_probe_data); 
	return (bmedia_probe_base_t)probe;
err_parser:
	BKNI_Free(probe);
err_alloc:
	return NULL;
}

static void 
b_flv_probe_destroy(bmedia_probe_base_t probe_)
{
	bflv_probe_t probe = (bflv_probe_t) probe_;

	BDBG_OBJECT_ASSERT(probe, bflv_probe_t);
	bflv_parser_remove_handler(probe->parser, &probe->video.handler);
	bflv_parser_remove_handler(probe->parser, &probe->audio.handler);
	bflv_parser_destroy(probe->parser);
	BDBG_OBJECT_DESTROY(probe, bflv_probe_t);
	BKNI_Free(probe);
	return;
}

static unsigned
b_media_probe_aquire_flv_duration(bfile_buffer_t buf)
{
    int rc;
    off_t off_first, off_last;
    batom_t atom;
    bfile_buffer_result result;
    batom_cursor cursor;
    uint32_t last_tag;
    uint32_t timestamp = 0;

    rc = bfile_buffer_get_bounds(buf, &off_first, &off_last);
    if(rc!=0) { goto error;}

    /* The last 4 bytes of an FLV file specify the size of the previous tag,
        We can use this to locate the last tag in the file and determine the timestamp */
    atom = bfile_buffer_read(buf, off_last-4, 4, &result);
    if(atom==NULL) { goto error;}
    
    batom_cursor_from_atom(&cursor, atom);
    last_tag = batom_cursor_uint32_be(&cursor);
    batom_release(atom);
    if(BATOM_IS_EOF(&cursor)) {goto error;}
    if(last_tag+4 >= off_last) {goto error;}

    atom = bfile_buffer_read(buf, off_last-(last_tag+4), last_tag, &result);
    if(atom==NULL) { goto error;}
    batom_cursor_from_atom(&cursor, atom);
    
    /* TagType(1byte) + DataSize(3bytes) */
    batom_cursor_skip(&cursor, 4);
	timestamp = batom_cursor_uint24_be(&cursor);
	timestamp |= ((uint32_t)batom_cursor_byte(&cursor))<<24;
    batom_release(atom);
    if(BATOM_IS_EOF(&cursor)) {goto error;}
	
    return timestamp;
error:
    return 0;
}

static const bmedia_probe_stream *
b_flv_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
	bflv_probe_t probe = (bflv_probe_t)probe_;
	off_t off;
	size_t read_len = 16384;
	bflv_probe_stream *stream;
    bflv_probe_track *track;

	BDBG_OBJECT_ASSERT(probe, bflv_probe_t);
    probe->audio.found = false;
    probe->video.found = false;
	for(off=0;off<1*1024*1024;) {
		batom_t atom;
		bfile_buffer_result result;
		size_t feed_len;
		size_t atom_len;

		BDBG_MSG(("b_flv_probe_parse: %#lx reading %u:%u", (unsigned long)probe, (unsigned)off, read_len));
		atom = bfile_buffer_read(buf, off+config->parse_offset, read_len, &result);
		if(result!=bfile_buffer_result_ok) {
			break;
		}
		if(!atom) {
			break;
		}
		atom_len = batom_len(atom);
		BDBG_MSG(("b_flv_probe_parse: %#lx read %u:%u -> %#lx", (unsigned long)probe, (unsigned)off, atom_len, (unsigned long)atom));
		off += atom_len;
		batom_pipe_push(pipe, atom);
		feed_len = bflv_parser_feed(probe->parser, pipe);
		if(feed_len!=atom_len) {
			break;
		}
        if(probe->video.found && probe->audio.found) {
            break;
        }
	}
	bflv_parser_reset(probe->parser);
    if(!probe->video.found && !probe->audio.found) { goto err_no_data;}

	/* return result of parsing */
	stream = BKNI_Malloc(sizeof(*stream));
    if(!stream) { goto err_stream; }
    bmedia_probe_stream_init(&stream->media, bstream_mpeg_type_flv);
    if(probe->video.found) {
        track = BKNI_Malloc(sizeof(*track));
        if(!track) { goto err_track_video; }
        bmedia_probe_track_init(&track->media);
        track->media.type = bmedia_track_type_video;
        track->media.number = probe->video.handler.tag_type;
        switch(B_GET_BITS(probe->video.meta,3,0)) {
        case B_FLV_CODECID_S263: track->media.info.video.codec = bvideo_codec_spark; break;
        case B_FLV_CODECID_ON2_VP6_ALPHA: /* VP6 with alpha channel */
        case B_FLV_CODECID_ON2_VP6: track->media.info.video.codec = bvideo_codec_vp6;  break;
        case B_FLV_CODECID_H264: track->media.info.video.codec = bvideo_codec_h264;  break;

        case 3: /* Screen video */
        case 6: /* Screen video V2 */
        default:
            track->media.info.video.codec = bvideo_codec_unknown;  break;
        }
        track->media.info.video.width = 0;
	    track->media.info.video.height = 0;
		BLST_SQ_INSERT_TAIL(&stream->media.tracks, &track->media, link);
    }
    if(probe->audio.found) {
        static const uint16_t sound_rate[4]={5513, 11025, 22050, 44100};
        track = BKNI_Malloc(sizeof(*track));
        if(!track) { goto err_track_audio; }
        bmedia_probe_track_init(&track->media);
        track->media.type = bmedia_track_type_audio;
        track->media.number = probe->audio.handler.tag_type;
        track->media.info.audio.channel_count = B_GET_BIT(probe->audio.meta,0)?2:1;
        track->media.info.audio.sample_size = B_GET_BIT(probe->audio.meta,1)?16:8;
        track->media.info.audio.sample_rate = sound_rate[B_GET_BITS(probe->audio.meta,3,2)];
        switch(B_GET_BITS(probe->audio.meta,7,4)) {
        case B_FLV_SOUNDFORMAT_MP3: 
        case B_FLV_SOUNDFORMAT_MP3_8KHZ: track->media.info.audio.codec = baudio_format_mp3; break;
        case B_FLV_SOUNDFORMAT_AAC: track->media.info.audio.codec = baudio_format_aac; break;
        case 0: /* Linear PCM */
        case 1: /* ADPCM */
        case 3: /* Linear PCM, little endian */
        case 4: /* Nellymoser 16kHz mono */
        case 5: /* "Nellymoser 8kHz mono */
        case 6: /* Nellymoser */
        case 7: /* G.711 A-law */
        case 8: /* G.711 mu-law */
        default:
            track->media.info.audio.codec = baudio_format_unknown; break;
        }
		BLST_SQ_INSERT_TAIL(&stream->media.tracks, &track->media, link);
    }

    stream->media.duration = b_media_probe_aquire_flv_duration(buf);
	return &stream->media;

err_track_audio:
err_track_video:
    bmedia_probe_basic_stream_free(probe_, &stream->media);
err_stream:
err_no_data:
    return NULL;
}

static bool 
b_flv_probe_header_match(batom_cursor *header)
{
    uint8_t byte0, byte1, byte2;
	
	byte0 = batom_cursor_byte(header);
	byte1 = batom_cursor_byte(header);
	byte2 = batom_cursor_byte(header);
    if(byte0 != 'F' || byte1 != 'L' || byte2 != 'V') {
        return false;
    }

	byte0 = batom_cursor_byte(header);
    if(byte0 != 1) {
        return false;
    }

	byte0 = batom_cursor_byte(header);
    if(B_GET_BITS(byte0,7,4) != 0) {
        return false;
    }

	if(BATOM_IS_EOF(header)) {
		return false;
	}
	return true;
}

static const bmedia_probe_file_ext b_flv_ext[] =  {
	{"flv"},
	{""}
};

const bmedia_probe_format_desc bflv_probe = {
	bstream_mpeg_type_flv,
	b_flv_ext, /* ext_list */
	/* FLV version type */
	    3 +  1  +   1 , /* header_size */
	b_flv_probe_header_match, /* header_match */
	b_flv_probe_create, /* create */
	b_flv_probe_destroy, /* destroy */
	b_flv_probe_parse, /* parse */
	bmedia_probe_basic_stream_free /* stream free */
};

