/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_probe.c $
 * $brcm_Revision: 57 $
 * $brcm_Date: 3/5/12 12:32p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_probe.c $
 * 
 * 57   3/5/12 12:32p vsilyaev
 * SW7425-2179: Added support for QT MJPEG types A and B
 * 
 * 56   2/21/12 5:14p vsilyaev
 * SW7425-2422: Added support for PCM (BE/twos) audio in MP4 container
 * 
 * 55   1/20/12 1:38p vsilyaev
 * SW7425-2178: Relaxed probing of MP4 containers
 * 
 * 54   1/20/12 1:00p vsilyaev
 * SW7425-2178: Added MJPEG video codec
 * 
 * 53   1/11/12 6:08p vsilyaev
 * SW7425-2112: Added handling of JPEG payloads
 * 
 * 52   6/21/11 1:29p vsilyaev
 * SWDTV-7625: Skip over 'subs' box
 * 
 * 51   6/20/11 6:38p vsilyaev
 * SWBLURAY-26273: Communicate MP4 DRM information to application
 * 
 * 50   5/3/11 2:49p vsilyaev
 * SWBLURAY-22800: Use common code to parser H264 meta data
 * 
 * 49   12/30/10 1:07p vsilyaev
 * SW35230-2287: Updated parsing at end-of-file
 * 
 * 48   12/6/10 6:24p vsilyaev
 * SW35230-1953: Added ADPCM audio
 * 
 * 47   11/22/10 11:54a vsilyaev
 * SW35230-2287: Keep on sending dummy data until parsed all accumulated
 * data
 * 
 * 46   11/4/10 11:29a vsilyaev
 * SW35230-2096: Added extraction of the language information
 * 
 * 45   10/18/10 1:46p vsilyaev
 * SWBLURAY-22500: Merged support for MPEG audio
 * 
 * 44   8/26/10 5:35p vsilyaev
 * SW7420-876: Return location of the rmra box
 * 
 * 43   8/26/10 5:01p vsilyaev
 * SW7420-876: Fixed parsing of small MP4 files
 * 
 * fix_bdvd_v3.0_SWBLURAY-22500/2   10/15/10 9:20a ialauder
 * Use MP4 audio info
 * 
 * fix_bdvd_v3.0_SWBLURAY-22500/1   10/14/10 3:45p ialauder
 * Add support for MPEG audio in MP4 container
 * 
 * 42   5/4/10 6:03p vsilyaev
 * SWBLURAY-20481, SW3548-2813: Keep on parsing cached data after file
 * have reached EOF
 * 
 * 41   4/14/10 12:44p jtna
 * SW7125-289: Coverity Defect ID:20326 BAD_FREE (Coverity comment must
 * precede line in question)
 * 
 * 40   4/8/10 6:58p vsilyaev
 * SW7405-3773: Initial support for playback of MPEG4-Part12 fragmented
 * files
 * 
 * 39   3/19/10 5:56p vsilyaev
 * SW7405-3773: Added detection of fragmented MP4 files
 * 
 * 38   3/9/10 9:55a vsilyaev
 * SW3548-2813:Parse only header of sample_size objects
 * 
 * 37   3/5/10 4:01p vsilyaev
 * SW3548-2813: Skip and read only portion of compactsamplesize and
 * samplesize boxes
 * 
 * 36   1/26/10 2:23p vsilyaev
 * SWBLURAY-18771: Added AAC profile for MP4 and MKV containers
 * 
 * fix_bdvd_v3.0_issue18771/1   1/26/10 4:49p ialauder
 * Add baac_probe to store profile of AAC track
 * 
 * 35   1/14/10 12:50p vsilyaev
 * SWBLURAY-14718: Updated comments in regard to track duration and number
 * of samples, this information is sufficient to extract frame rate for
 * the video tracks
 * 
 * 34   1/14/10 12:40p vsilyaev
 * SWBLURAY-14718: Merged frame rate detection
 * 
 * fix_bdvd_v3.0_issue18060/1   1/14/10 1:30p gooslin
 * Return video frame rate on probe operation
 * 
 * 33   12/8/09 10:50a vsilyaev
 * SW7405-3556: Just in case clear out 'encrypted' flag
 * 
 * 32   12/8/09 10:46a vsilyaev
 * SW7405-3556: Added flag to mark encrypted tracks
 * 
 * fix_bdvd_v3.0_issue16965/3   12/8/09 9:16a ialauder
 * Rename encryption flag
 * 
 * fix_bdvd_v3.0_issue16965/1   12/7/09 5:35p ialauder
 * AAC DRM detection
 * 
 * 31   12/1/09 3:29p vsilyaev
 * SW7405-3513: Return AAC+ codec if stream has SBR information
 * 
 * 30   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/3   12/1/09 5:55p ialauder
 * Use true/false for has_ext bool
 * 
 * fix_bdvd_v3.0_issue17634/2   11/30/09 1:41p ialauder
 * AAC-HE support
 * 
 * fix_bdvd_v3.0_issue17634/1   11/25/09 4:24p ialauder
 * Changes to detect AAC-HE
 * 
 * 29   9/30/09 12:20p vsilyaev
 * SW7405-2566: Properly indentify AC3 and AC3+ codecs
 * 
 * 28   9/1/09 4:06p vsilyaev
 * SW3548-2442: Added support for AMR audio
 * 
 * 27   7/23/09 9:53a vsilyaev
 * PR 57048: Added support for parsing AMR audio tracks
 * 
 * 26   7/9/09 1:12p vsilyaev
 * PR 55705: Added support for AC3/EAC3 audio
 * 
 * 25   6/9/09 7:32p vsilyaev
 * PR 54342: Added code to probe index in the media files
 * 
 * 24   6/5/09 4:27p vsilyaev
 * PR 55757: Added code to guesstimate bitrate based on the size of the
 * data object
 * 
 * 23   6/5/09 1:31p vsilyaev
 * PR 55417: Fixed memory leak
 * 
 * 22   6/3/09 7:55p vsilyaev
 * PR 55417: Updated code for new semantics of the bfile_buffer_read
 * 
 * 21   4/17/09 12:22p vsilyaev
 * PR 54149: Fixed warning from the static analyze tool
 * 
 * 20   4/7/09 10:19a vsilyaev
 * PR 50118: Added address_free tag
 * 
 * 19   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 * 
 * 18   3/26/09 11:33a vsilyaev
 * PR 53282: Fix typo from change to calculate the sample rate
 * 
 * fix_bdvd_v2.0_pr13769/1   3/19/09 4:51p gooslin
 * Fix typo from change to calculate the sample rate
 * 
 * 17   3/16/09 5:47p vsilyaev
 * PR 53282: Added code to derive audio sampling rate
 * 
 * fix_bdvd_v2.0_pr13666/1   3/13/09 3:55p gooslin
 * Calculate sample rate when an invalid coded sample rate is detected
 * 
 * 16   12/10/08 4:03p vsilyaev
 * PR 50118: Covered warning
 * 
 * 15   12/4/08 5:46p vsilyaev
 * PR 49899: Detect all brands of Mpeg4Part12 containers, and pass
 * compability test to application
 * 
 * 14   11/25/08 1:12a vsilyaev
 * PR 49646: Added code to parse and print H264 profile/level information
 * 
 * 13   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 12   11/11/08 9:43a gmohile
 * PR 25109 : Fix mp4v probe.
 * 
 * 11   11/11/08 9:38a gmohile
 * PR 48584 : Added 3gpp support
 * 
 * 10   10/2/08 10:47a vsilyaev
 * PR 32813: Use proper api to add detected track
 * 
 * 9   9/17/08 6:25p vsilyaev
 * PR 45431: Added segments for moov and mdat objects
 * 
 * 8   9/3/08 6:15p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 7   7/20/07 11:57p vsilyaev
 * PR 32813: Added MPEG video probe
 * 
 * 6   7/19/07 12:17p vsilyaev
 * PR 32813: Renamed bmedia_track to bmedia_track_type
 * 
 * 5   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 4   7/12/07 12:38p vsilyaev
 * PR 32813: Included MP41 MP42 stream brands
 * 
 * 3   7/10/07 5:54p vsilyaev
 * PR 32813: Fixed extraction of duration
 * 
 * 2   7/10/07 5:22p vsilyaev
 * PR 32813: Added stream duration field
 * 
 * 1   7/9/07 4:05p vsilyaev
 * PR 32813: MP4 probe module
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmp4_probe.h"
#include "bmp4_parser.h"
#include "bkni.h"
#include "bmedia_util.h"
#include "bavc_video_probe.h"
#include "baac_probe.h"
#include "bfile_cache.h"

BDBG_MODULE(bmp4_probe);

typedef struct bmp4_probe  *bmp4_probe_t; 

typedef struct b_mp4_probe_handler {
	bmp4_parser_handler handler; /* must be first */
	bmp4_probe_t probe; /* pointer to probe */ 
} b_mp4_probe_handler;

struct bmp4_probe {
	BDBG_OBJECT(bmp4_probe_t)
	bmp4_parser_t parser;
	bmp4_probe_stream *stream;
	bmp4_probe_track *track;
	off_t next_seek;
	bool movieheader_valid;
	bool movie_valid;
	bool stream_error;
	bool trackheader_valid;
	bool sample_valid;
	bool sample_size_valid;
	bool mediaheader_valid;
	bool handler_valid;
    bool mdat_valid;
    bfile_segment samplesize;
    bfile_segment compactsamplesize;
    batom_factory_t factory;
    uint32_t handler_type;
	b_mp4_probe_handler filetype_handler;
	b_mp4_probe_handler movie_handler;
	b_mp4_probe_handler track_handler;
	b_mp4_probe_handler media_handler;
	b_mp4_probe_handler mediainfo_handler;
	b_mp4_probe_handler sampletable_handler;
};

BDBG_OBJECT_ID(bmp4_probe_t);

static bool 
b_mp4_probe_header_match(batom_cursor *header)
{
	bmp4_box box;
	size_t header_size;

	header_size = bmp4_parse_box(header, &box);
	if(header_size==0) {
		return false;
	}
	/* ISO/IEC 14496-12:2005 MPEG-4 Part 12 - ISO Base Media File Format */
	/* page 5 */
    switch(box.type) {
    default:
		return false;
    case BMP4_FILETYPEBOX:
    case BMP4_MOVIE:
    case BMP4_MOVIE_DATA:
        return box.size > 0;
    }
}

static const bmedia_probe_file_ext b_mp4_ext[] =  {
	{"mp4"},
	{""}
};

static bmp4_probe_stream *
b_mp4_get_stream(bmp4_probe_t probe)
{
    bmp4_probe_stream *stream;

    stream = probe->stream;
    if(!stream) {
        stream = BKNI_Malloc(sizeof(*stream));
        if(stream) {
            bmedia_probe_stream_init(&stream->media, bstream_mpeg_type_mp4);
            stream->media.index = bmedia_probe_index_required;
            stream->mdat.offset = 0;
            stream->mdat.size = 0;
            stream->moov.offset = 0;
            stream->moov.size = 0;
            stream->rmra.offset = 0;
            stream->rmra.size = 0;
            stream->uuid.offset = 0;
            stream->uuid.size = 0;
            stream->compatible = false;
            stream->fragmented = false;
            stream->ftyp[0] = '\0';
            probe->stream = stream;
        } else {
            BDBG_ERR(("b_mp4_probe_hdrl: %#lx can't allocate %u bytes", (unsigned long)probe, sizeof(*stream)));
        }
    }
    return stream;
}

static bmp4_parser_action  
b_mp4_probe_filetype(bmp4_parser_handler *handler, uint32_t type, batom_t box)
{
	bmp4_probe_t probe = ((b_mp4_probe_handler *)handler)->probe;
    static const uint32_t 
    b_mp4_compatible_brands[] = 
    {
        BMP4_TYPE('q','t',' ',' '),
        BMP4_TYPE('3','g','2','a'),
        BMP4_TYPE('i','s','o','m'),
        BMP4_TYPE('i','s','o','2'),
        BMP4_TYPE('m','p','4','1'),
        BMP4_TYPE('m','p','4','2')
    };


	BDBG_MSG(("b_mp4_player_filetype:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	if(type == BMP4_FILETYPEBOX) {
        bmp4_filetypebox filetype;
		BDBG_ASSERT(box);
		if(bmp4_parse_filetype(box, &filetype)) {
            bmp4_probe_stream *stream = b_mp4_get_stream(probe);
            if(stream) {
                unsigned n;
                uint32_t brand = filetype.major_brand;
                stream->ftyp[0] = (brand>>24)&0xFF;
                stream->ftyp[1] = (brand>>16)&0xFF;
                stream->ftyp[2] = (brand>>8)&0xFF;
                stream->ftyp[3] = brand&0xFF;
                stream->ftyp[4] = '\0';
                for(n=0;;n++) {
                    unsigned i;
        		    for(i=0;i<sizeof(b_mp4_compatible_brands)/sizeof(*b_mp4_compatible_brands);i++) {
                        if(brand == b_mp4_compatible_brands[i]) {
                            stream->compatible = true;
                            goto done;
                        }
                    }
                    if(n>=filetype.ncompatible_brands) {
                        break;
                    }
                    brand = filetype.compatible_brands[n];
                }
done:
                ;
			}
        }
	}
	if(box) {
		batom_release(box);
	}
	return bmp4_parser_action_none;
}


#if 0
	brand = batom_cursor_uint32_be(header);
	batom_cursor_skip(header, sizeof(uint32_t) /* version */);
	box_left = box.size - (header_size+sizeof(uint32_t)+sizeof(uint32_t));
	for(;;) {
		unsigned i;

		if(BATOM_IS_EOF(header)) {
			return false;
		}
		BDBG_MSG(("b_mp4_probe_header_match: brand " B_MP4_TYPE_FORMAT , B_MP4_TYPE_ARG(brand)));
		for(i=0;i<sizeof(b_mp4_compatible_brands)/sizeof(*b_mp4_compatible_brands);i++) {
			if(brand == b_mp4_compatible_brands[i]) {
				return true;
			}
		}
		/* read minor brands */
		brand = batom_cursor_uint32_be(header);
		if(box_left<sizeof(uint32_t)) {
			return false;
		}
		box_left -= sizeof(uint32_t);
	} 
#endif

static void 
b_mp4_probe_set_segment(bmp4_probe_t probe, bmp4_probe_segment *segment, uint64_t size, uint64_t offset)
{
    bmp4_probe_stream *stream;
    stream = b_mp4_get_stream(probe);
    if(stream) {
        segment = (void *)((uint8_t *)segment + (unsigned long)stream); /* original 'segment' is just used to send offset of the 'segment' in the stream object , now get right pointer */
        segment->offset = offset;
        segment->size = size;
    }
    return;
}

static bmp4_parser_action 
b_mp4_probe_object_begin(void *cntx, uint32_t type, uint64_t size, uint64_t offset, size_t header_size)
{
    bmp4_probe_t probe = cntx;
    bmp4_probe_stream *stream=NULL;
    bmp4_parser_action result = bmp4_parser_action_none;
    unsigned i;
    static const uint32_t skip_boxes [] = {
        BMP4_DECODETIMETOSAMPLE,
        BMP4_COMPOSITIONTIMETOSAMPLE,
        BMP4_SAMPLESIZE,
        BMP4_COMPACTSAMPLESIZE,
        BMP4_SAMPLETOCHINK,
        BMP4_CHUNKOFFSET,
        BMP4_CHUNKLARGEOFFSET,
        BMP4_SYNCSAMPLE,
        BMP4_TYPE('s','t','s','h'),
        BMP4_TYPE('s','t','d','p'),
        BMP4_TYPE('p','a','d','b'),
        BMP4_TYPE('s','d','t','p'),
        BMP4_TYPE('s','b','g','p'),
        BMP4_TYPE('u','u','i','d'),
        BMP4_TYPE('r','m','r','a'),
        BMP4_TYPE('s','u','b','s'),
        BMP4_MOVIE_DATA
    };

    BDBG_MSG(("b_mp4_probe_object_begin:%#lx " B_MP4_TYPE_FORMAT " %u bytes at %#lx", (unsigned long)cntx, B_MP4_TYPE_ARG(type), size, (unsigned long)offset));
    BSTD_UNUSED(header_size);
    BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
    switch(type) {
    case BMP4_MOVIE_DATA:
        probe->mdat_valid = true;
        b_mp4_probe_set_segment(probe, &stream->mdat, size, offset);
        break;
    case BMP4_MOVIE:
        b_mp4_probe_set_segment(probe, &stream->moov, size, offset);
        break;
    case BMP4_TYPE('r','m','r','a'):
        b_mp4_probe_set_segment(probe, &stream->rmra, size, offset);
        break;
    case BMP4_TYPE('u','u','i','d'):
        b_mp4_probe_set_segment(probe, &stream->uuid, size, offset);
        break;
    case BMP4_COMPACTSAMPLESIZE:
        bfile_segment_set(&probe->compactsamplesize, offset, size);
        result = bmp4_parser_action_return;
        break;
    case BMP4_SAMPLESIZE:
        bfile_segment_set(&probe->samplesize, offset, size);
        result = bmp4_parser_action_return;
        break;
    case BMP4_MOVIE_FRAGMENT:
    case BMP4_MOVIE_EXTENDS:
        stream = b_mp4_get_stream(probe);
        if(stream) {
            stream->fragmented = true;
        }
        result = bmp4_parser_action_return;
        break;
    default:
        break;
    }

    if(size>1024) { /* don't bother with small boxes */
        for(i=0;i<sizeof(skip_boxes)/sizeof(*skip_boxes);i++) {
            if(type==skip_boxes[i]) {
                probe->next_seek = offset+size; /* skip over large boxes */
                return bmp4_parser_action_return;
            }
        }
    }
    return result;
}

static bmp4_parser_action  
b_mp4_probe_movie(bmp4_parser_handler *handler, uint32_t type, batom_t box)
{
	bmp4_probe_t probe = ((b_mp4_probe_handler *)handler)->probe;
	bmp4_parser_action action = bmp4_parser_action_none;
	bmp4_movieheaderbox movieheader;

	BDBG_MSG(("b_mp4_probe_movie:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	switch(type) {
	case BMP4_TYPE_BEGIN:
		b_mp4_get_stream(probe);
		break;
	case BMP4_MOVIEHEADER:
		BDBG_ASSERT(box);
		probe->movieheader_valid = bmp4_parse_movieheader(box, &movieheader);
		if(!probe->movieheader_valid) {
			BDBG_MSG(("b_mp4_probe_movie:%#lx error in the movie header", (unsigned long)probe));
			goto error;
		}
		if(probe->stream) {
			probe->stream->media.duration = (1000*movieheader.duration)/movieheader.timescale;
		}
		break;
	case BMP4_TYPE_END:
		BDBG_MSG(("b_mp4_probe_movie:%#lx parsing completed", (unsigned long) probe));
		probe->movie_valid = true;
		action = bmp4_parser_action_return;
		break;
	default:
		break;
	}
done:
	if(box) {
		batom_release(box);
	}
	return action;
error:
	probe->stream_error = true;
	action = bmp4_parser_action_return;
	goto done;
}

static bool
b_mp4_probe_validate_sample_rate(bmp4_probe_t probe, bmp4_probe_track *track)
{
    bmedia_info_aac aac_info;

    /* Verify we have a valid sample rate as coded by the tream */
    if (!bmedia_info_aac_set_sampling_frequency_index(&aac_info, track->media.info.audio.sample_rate)) {
        unsigned calc_sample_rate;

        if ( !(probe->sample_size_valid && probe->mediaheader_valid && track->duration && track->sample_count) ) {
    		BDBG_ERR(("%s: Information necessary to calculate the samplerate not available, using an unsupported sample rate %u", "b_mp4_probe_validate_sample_rate", track->media.info.audio.sample_rate));
            return false;
        }

        /* The coded sample rate is not known by our decoder.  Try to calculate it... */
        calc_sample_rate = (1000 /*convert to seconds*/ * 1024 /*convert to Kbytes*/ * (uint64_t)track->sample_count) / track->duration;
        if (bmedia_info_aac_set_sampling_frequency_index(&aac_info, calc_sample_rate)) {
            unsigned sample_rate = bmedia_info_aac_sampling_frequency_from_index(aac_info.sampling_frequency_index);
            /* The calculated sample rate matches our list */
    		BDBG_WRN(("%s: Track sample rate %u is unrecognized. Calculating the rate to be %u", "b_mp4_probe_validate_sample_rate", track->media.info.audio.sample_rate, sample_rate));
            track->media.info.audio.sample_rate = sample_rate;
        } else {
    		BDBG_ERR(("%s: Track using an unsupported sample rate %u", "b_mp4_probe_validate_sample_rate", track->media.info.audio.sample_rate));
            return false;
        }
    } else {
        /* The sample rate was detected in our psupported list. Now, just ensure the value is normalized to be compatible with our decoder (e.g. 48002 will become 48000) */
        track->media.info.audio.sample_rate = bmedia_info_aac_sampling_frequency_from_index(aac_info.sampling_frequency_index);
    }

    /* track->media.info.audio.sample_rate has been verified to contain a known sample rate */
    return true;
}

static bmp4_parser_action  
b_mp4_probe_track(bmp4_parser_handler *handler, uint32_t type, batom_t box)
{
	bmp4_probe_t probe = ((b_mp4_probe_handler *)handler)->probe;
	bmp4_parser_action action = bmp4_parser_action_none;
	bmp4_probe_track *track;
	bmp4_trackheaderbox trackheader;

	BDBG_MSG(("b_mp4_probe_track:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	track = probe->track;
	switch(type) {
	case BMP4_TRACKHEADER:
		BDBG_ASSERT(box);
		if(!track) {
			break;
		}
		probe->trackheader_valid = bmp4_parse_trackheader(box, &trackheader);
		if(!probe->trackheader_valid) {
			break;
		}
		track->media.number = trackheader.track_ID;
		break;
	case BMP4_TYPE_BEGIN:
		BDBG_MSG(("b_mp4_probe_track:%#lx creating new track", (unsigned long)probe));
		track = BKNI_Malloc(sizeof(*track));
		if(!track) {
			BDBG_ERR(("b_mp4_probe_track: %#lx can't allocate %u bytes", (unsigned long)track, sizeof(*track)));
			break;
		}
		bmedia_probe_track_init(&track->media);
        track->duration = 0;
        track->sample_count = 0;
        track->encrypted = false;
		probe->trackheader_valid = false;
		probe->sample_valid = false;
		probe->sample_size_valid = false;
		probe->mediaheader_valid = false;
		probe->handler_valid = false;
        bfile_segment_clear(&probe->compactsamplesize);
        bfile_segment_clear(&probe->samplesize);
		probe->track = track;
		break;
	case BMP4_TYPE_END:
		if(!track) {
			break;	
		}
		if(probe->trackheader_valid && probe->sample_valid && probe->mediaheader_valid && probe->handler_valid) {
			BDBG_ASSERT(probe->stream);

            if (track->media.type == bmedia_track_type_audio &&
                track->media.info.audio.codec != baudio_format_unknown &&
                track->media.info.audio.sample_rate != 0 ) {
                b_mp4_probe_validate_sample_rate(probe, track);
            }
            /* coverity[address_free] */
            bmedia_probe_add_track(&probe->stream->media, &track->media); /* free is not bad since &track->media == track */
		} else {
			BKNI_Free(track);
		}

        probe->track = NULL;
		break;
	default:
		break;
	}
	if(box) {
		batom_release(box);
	}
	return action;
}

static bmp4_parser_action  
b_mp4_probe_media(bmp4_parser_handler *handler_, uint32_t type, batom_t box)
{
	bmp4_probe_t probe = ((b_mp4_probe_handler *)handler_)->probe;
	bmp4_parser_action action = bmp4_parser_action_none;
	bmp4_probe_track *track;
	bmp4_mediaheaderbox mediaheader;
	bmp4_handlerbox handler;

	BDBG_MSG(("b_mp4_probe_media:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler_->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	track = probe->track;
	if(track) {
		switch(type) {
		case BMP4_MEDIAHEADER:
			BDBG_ASSERT(box);
			probe->mediaheader_valid = bmp4_parse_mediaheader(box, &mediaheader);
			if(probe->mediaheader_valid) {
               track->duration = (1000*mediaheader.duration)/mediaheader.timescale;
               BDBG_CASSERT(sizeof(track->language)==sizeof(mediaheader.language));
               BKNI_Memcpy(track->language,mediaheader.language, sizeof(track->language));
            } else {
				BDBG_WRN(("b_mp4_probe_media: %#lx track:%u error in parsing Media Header", (unsigned long)probe, track->media.number));
			}
			break;
		case BMP4_HANDLER:
			BDBG_ASSERT(box);
			probe->handler_valid = bmp4_parse_handler(box, &handler);
			if(!probe->handler_valid) {
				BDBG_WRN(("b_mp4_probe_media: %#lx track:%u error in parsing Handler Reference", (unsigned long)probe, track->media.number));
			} 
			probe->handler_type = handler.handler_type;
			break;
		default:
			break;
		}
	}
	if(box) {
		batom_release(box);
	}
	return action;
}

static bmp4_parser_action  
b_mp4_probe_sampletable(bmp4_parser_handler *handler, uint32_t type, batom_t box)
{
    bmp4_probe_t probe = ((b_mp4_probe_handler *)handler)->probe;
    bmp4_parser_action action = bmp4_parser_action_none;
    bmp4_sample_info sample_info;
    bmp4_probe_track *track;

    BDBG_MSG(("b_mp4_probe_sampletable:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
    BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
    track = probe->track;
    if(track) {
        switch(type) {
        case BMP4_SAMPLEDESCRIPTION:
            if(probe->handler_valid) {
                probe->sample_valid = bmp4_parse_sample_info(box, &sample_info, probe->handler_type);
                track->encrypted = false;
                if(probe->sample_valid) {
                    if(sample_info.entry_count>0) {
                        unsigned i;
                        const bmp4_audiosampleentry *audio=NULL;
                        const bmp4_visualsampleentry *visual=NULL;
                        bmp4_sampleentry *sample = sample_info.entries[0];
                        track->encrypted = sample->encrypted;
                        track->protection_scheme_information_size = sample->protection_scheme_information_size;
                        BDBG_CASSERT(sizeof(track->protection_scheme_information)==sizeof(sample->protection_scheme_information));
                        BKNI_Memcpy(track->protection_scheme_information, sample->protection_scheme_information, sizeof(track->protection_scheme_information));
                        switch(sample->sample_type) {
                        case bmp4_sample_type_avc:
                            visual = &sample->codec.avc.visual;
                            track->media.info.video.codec = bvideo_codec_h264;
                            for(i=0;i<sample->codec.avc.meta.sps.no;i++) {
                                bmedia_probe_h264_video *h264_info = (bmedia_probe_h264_video *)&track->media.info.video.codec_specific;
                                size_t data_len;
                                const uint8_t *data;

                                data = bmedia_seek_h264_meta_data(&sample->codec.avc.meta.sps, i, &data_len);
                                if(data==NULL) {
                                    break;
                                }
                                if(data_len==0) { continue;}
                                b_h264_video_sps_parse(&h264_info->sps, data+1, data_len-1);
                                if(h264_info->sps.valid) {
                                    break;
                                }
                            }
                            break;
                        case bmp4_sample_type_mp4v:
                            visual = &sample->codec.mp4v.visual;
                            track->media.info.video.codec = bvideo_codec_mpeg4_part2;
                            break;
                        case bmp4_sample_type_s263:
                            visual = &sample->codec.s263.visual;
                            track->media.info.video.codec = bvideo_codec_h263;
                            break;
                        case bmp4_sample_type_mp4a:
                            track->media.type = bmedia_track_type_audio;
                            track->media.info.audio.channel_count = sample->codec.mp4a.audio.channelcount;
                            track->media.info.audio.sample_size = sample->codec.mp4a.audio.samplesize;
                            track->media.info.audio.sample_rate = bmedia_info_aac_sampling_frequency_from_index(sample->codec.mp4a.mpeg4.decoder.iso_14496_3.samplingFrequencyIndex);
                            track->media.info.audio.codec = (sample->codec.mp4a.mpeg4.decoder.iso_14496_3.audioObjectType == 5 ? baudio_format_aac_plus_adts: baudio_format_aac);
                            BDBG_CASSERT(sizeof(bmedia_probe_aac_audio) <= sizeof(track->media.info.audio.codec_specific));
                            if (sample->codec.mp4a.mpeg4.decoder.iso_14496_3.audioObjectType <= 5)
                            {
                                ((bmedia_probe_aac_audio*)&track->media.info.audio.codec_specific)->profile = 
                                    (bmedia_probe_aac_profile)sample->codec.mp4a.mpeg4.decoder.iso_14496_3.audioObjectType;
                            }
                            else
                            {
                                ((bmedia_probe_aac_audio*)&track->media.info.audio.codec_specific)->profile = 
                                    bmedia_probe_aac_profile_unknown;
                            }
                            break;                        
                        case bmp4_sample_type_mpg:
                            track->media.info.audio.codec = baudio_format_mpeg;
                            audio = &sample->codec.mp4a.audio;
                            break;
                        case bmp4_sample_type_ac3:
                        case bmp4_sample_type_eac3:
                            track->media.info.audio.codec = sample->sample_type==bmp4_sample_type_ac3? baudio_format_ac3 : baudio_format_ac3_plus;
                            audio = &sample->codec.ac3.audio;
                            break;                        
                        case bmp4_sample_type_samr:
                        case bmp4_sample_type_sawb:
                        case bmp4_sample_type_sawp:
                            audio = &sample->codec.amr.audio;
                            track->media.info.audio.codec = baudio_format_amr; /* not supported by the audio decoder */
                            break;                        
                        case bmp4_sample_type_drms:
                            track->media.type = bmedia_track_type_audio;
                            track->media.info.audio.codec = baudio_format_unknown;
                            track->encrypted = true;
                            break;
                        case bmp4_sample_type_drmi:
                            track->media.type = bmedia_track_type_video;
                            track->media.info.video.codec = bvideo_codec_unknown;
                            track->encrypted = true;
                            break;
                        case bmp4_sample_type_qt_ima_adpcm:
                            track->media.info.audio.codec = baudio_format_dvi_adpcm;
                            audio = &sample->codec.ms.audio;
                            break;
                        case bmp4_sample_type_mjpeg:
                            visual = &sample->codec.mjpeg.visual;
                            track->media.info.video.codec = bvideo_codec_mjpeg;
                            break;
                        case bmp4_sample_type_twos:
                            track->media.info.audio.codec = baudio_format_pcm;
                            audio = &sample->codec.twos.audio;
                            break;
                        case bmp4_sample_type_unknown:
                            track->media.type = bmedia_track_type_other;
                            break;
                        }
                        if(visual) {
                            track->media.type = bmedia_track_type_video;
                            track->media.info.video.width = visual->width;
                            track->media.info.video.height = visual->height;
                        } else if(audio) {
                            track->media.type = bmedia_track_type_audio;
                            track->media.info.audio.channel_count = audio->channelcount;
                            track->media.info.audio.sample_size = audio->samplesize;
                            track->media.info.audio.sample_rate = audio->samplerate>>16;
                        }

                    } else {
                        BDBG_WRN(("b_mp4_probe_media: %#lx track:%u Sample Description invalid number of entries", (unsigned long)probe, track->media.number));
                        probe->sample_valid = false;
                    }
                    bmp4_free_sample_info(&sample_info);
                } else {
                    BDBG_WRN(("b_mp4_probe_media: %#lx track:%u error in parsing Sample Description", (unsigned long)probe, track->media.number));
                }
            } else {
                BDBG_WRN(("b_mp4_probe_media: %#lx track:%u Handler Reference invalid, can't parse Sample Description", (unsigned long)probe, track->media.number));
            }
            break;
        default:
            break;
        }
    }
    if(box) {
        batom_release(box);
    }
    return action;
}

static bmp4_parser_action  
b_mp4_probe_mediainfo(bmp4_parser_handler *handler, uint32_t type, batom_t box)
{
	bmp4_probe_t probe = ((b_mp4_probe_handler *)handler)->probe;
	bmp4_parser_action action = bmp4_parser_action_none;

	BSTD_UNUSED(type);

	BDBG_MSG(("b_mp4_probe_mediainfo:%#lx " B_MP4_TYPE_FORMAT "[" B_MP4_TYPE_FORMAT "] %u bytes", (unsigned long)probe, B_MP4_TYPE_ARG(handler->type), B_MP4_TYPE_ARG(type), box?batom_len(box):0));
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	if(box) {
		batom_release(box);
	}
	return action;
}


static bmedia_probe_base_t 
b_mp4_probe_create(batom_factory_t factory)
{
	bmp4_probe_t	probe;
	bmp4_parser_cfg cfg;

	probe = BKNI_Malloc(sizeof(*probe));
	if(!probe) {
		BDBG_ERR(("b_mp4_probe_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, bmp4_probe_t);
	probe->stream = NULL;
	probe->filetype_handler.probe = probe;
	probe->movie_handler.probe = probe;
	probe->track_handler.probe = probe;
	probe->media_handler.probe = probe;
	probe->mediainfo_handler.probe = probe;
	probe->sampletable_handler.probe = probe;
    probe->factory = factory;

	bmp4_parser_default_cfg(&cfg);
	cfg.user_cntx = probe;
	cfg.box_begin = b_mp4_probe_object_begin;
	probe->parser = bmp4_parser_create(factory, &cfg);
	if(!probe->parser) {
		goto err_parser;
	}
	bmp4_parser_install_handler(probe->parser, &probe->filetype_handler.handler, BMP4_FILETYPEBOX, b_mp4_probe_filetype); 
	bmp4_parser_install_handler(probe->parser, &probe->movie_handler.handler, BMP4_MOVIE, b_mp4_probe_movie); 
	bmp4_parser_install_handler(probe->parser, &probe->track_handler.handler, BMP4_TRACK, b_mp4_probe_track); 
	bmp4_parser_install_handler(probe->parser, &probe->media_handler.handler, BMP4_MEDIA, b_mp4_probe_media); 
	bmp4_parser_install_handler(probe->parser, &probe->mediainfo_handler.handler, BMP4_MEDIAINFORMATION, b_mp4_probe_mediainfo); 
	bmp4_parser_install_handler(probe->parser, &probe->sampletable_handler.handler, BMP4_SAMPLETABLE, b_mp4_probe_sampletable); 
	return (bmedia_probe_base_t)probe;
err_parser:
	BKNI_Free(probe);
err_alloc:
	return NULL;
}

static void 
b_mp4_probe_destroy(bmedia_probe_base_t probe_)
{
	bmp4_probe_t probe = (bmp4_probe_t)probe_;

	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	bmp4_parser_remove_handler(probe->parser, &probe->sampletable_handler.handler);
	bmp4_parser_remove_handler(probe->parser, &probe->mediainfo_handler.handler);
	bmp4_parser_remove_handler(probe->parser, &probe->media_handler.handler);
	bmp4_parser_remove_handler(probe->parser, &probe->track_handler.handler);
	bmp4_parser_remove_handler(probe->parser, &probe->movie_handler.handler);
	bmp4_parser_remove_handler(probe->parser, &probe->filetype_handler.handler);
	bmp4_parser_destroy(probe->parser);
	BDBG_OBJECT_DESTROY(probe, bmp4_probe_t);
	BKNI_Free(probe);
	return;
}

const bmedia_probe_stream *
b_mp4_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
	bmp4_probe_t probe = (bmp4_probe_t)probe_;
	off_t off;
	const size_t read_len = BMEDIA_PROBE_FEED_SIZE;
	bmp4_probe_stream *stream;

	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	BDBG_ASSERT(probe->stream==NULL);
	probe->track = NULL;
	probe->stream_error = false;
	probe->next_seek = 0;
	probe->movieheader_valid = false;
	probe->movie_valid = false;
    probe->mdat_valid = false;
    probe->sample_size_valid = false;
    bfile_segment_clear(&probe->compactsamplesize);
    bfile_segment_clear(&probe->samplesize);

	for(off=0;;) {
		batom_t atom;
		bfile_buffer_result result;
		size_t feed_len;
		size_t atom_len;
        bmp4_parser_status parser_status;


		if(probe->next_seek!=0) {
			off = probe->next_seek;
			bmp4_parser_seek(probe->parser, off); /* skip over large box */
			probe->next_seek = 0;
		}
		BDBG_MSG(("b_mp4_probe_parse: %#lx reading %u:%u", (unsigned long)probe, (unsigned)(config->parse_offset + off), read_len));
		atom = bfile_buffer_read(buf, config->parse_offset+off, read_len, &result);
		if(!atom) {
            bmp4_parser_get_status(probe->parser, &parser_status);
            BDBG_MSG(("b_mp4_probe_parse: %#lx box_length:%u acc_length:%u", (unsigned long)probe, parser_status.box_length, parser_status.acc_length));
            if(result!=bfile_buffer_result_eof || parser_status.box_length>parser_status.acc_length || parser_status.acc_length<=BMP4_BOX_MAX_SIZE) {
                break;
            }
            atom_len = 0;
		}  else {
            atom_len = batom_len(atom);
            BDBG_MSG(("b_mp4_probe_parse: %#lx read %u:%u -> %#lx", (unsigned long)probe, (unsigned)(config->parse_offset+off), atom_len, (unsigned long)atom));
            off += atom_len;
            batom_pipe_push(pipe, atom);
        }
		feed_len = bmp4_parser_feed(probe->parser, pipe);
		if( (feed_len!=atom_len && probe->next_seek==0) || probe->stream_error) {
			break;
		}
		if(probe->movie_valid && (probe->mdat_valid || (probe->stream && probe->stream->fragmented))) {
			break;
		}
        if(!probe->sample_size_valid && probe->track) {
            bmp4_probe_track *track = probe->track;
            batom_cursor cursor;

            if(bfile_segment_test(&probe->samplesize)) {
	            bmp4_sample_size_header header;

                atom = bfile_buffer_read(buf, probe->samplesize.start, 128, &result);
                if(!atom) {
                    break;
                }
	            batom_cursor_from_atom(&cursor, atom);
                /* Only one of BMP4_SAMPLESIZE or BMP4_COMPACTSAMPLESIZE may be present */
                probe->sample_size_valid = bmp4_parse_sample_size_header(&cursor, &header);
                batom_release(atom);
                if (probe->sample_size_valid) {
                    track->sample_count = header.sample_count;
                }
                bfile_segment_clear(&probe->samplesize);
            } else if(bfile_segment_test(&probe->samplesize)) {
	            bmp4_compact_sample_size_header header;

                atom = bfile_buffer_read(buf, probe->compactsamplesize.start, 128, &result);
                if(!atom) {
                    break;
                }
	            batom_cursor_from_atom(&cursor, atom);
                probe->sample_size_valid = bmp4_parse_compact_sample_size_header(&cursor, &header);
                batom_release(atom);
                if (probe->sample_size_valid) {
                    track->sample_count = header.sample_count;
                }
            }
        }
        if(result==bfile_buffer_result_eof) {
            bmp4_parser_get_status(probe->parser, &parser_status);
            if(parser_status.box_length>parser_status.acc_length) {
                break;
            } /* else keep on sending dummy data */
        }
	}
    if(probe->track) {
        BKNI_Free(probe->track);
        probe->track=NULL;
    }
	bmp4_parser_reset(probe->parser);
	/* return result of parsing */
	stream = probe->stream;
    if(stream) {
        if(stream->media.duration) {
            stream->media.max_bitrate = ((1000*8)*((stream->mdat.size+stream->moov.size)))/stream->media.duration;
        }
		BDBG_MSG(("b_mp4_probe_parse: %#lx moov: %u:%u mdat:%u:%u %uKBps", (unsigned long)probe, (unsigned)stream->moov.offset, (unsigned)stream->moov.size, (unsigned)stream->mdat.offset, (unsigned)stream->mdat.size, stream->media.max_bitrate/1024));
    }
	probe->stream = NULL;
	return &stream->media;
}

static void
b_mp4_probe_stream_free(bmedia_probe_base_t probe_, bmedia_probe_stream *stream)
{
	bmp4_probe_t probe = (bmp4_probe_t)probe_;
	BDBG_OBJECT_ASSERT(probe, bmp4_probe_t);
	BSTD_UNUSED(probe);

	bmedia_probe_basic_stream_free(probe_, stream);
	return;
}


const bmedia_probe_format_desc bmp4_probe = {
	bstream_mpeg_type_mp4,
	b_mp4_ext, /* ext_list */
	256, /* ftyp box shall be smaller then that */
	b_mp4_probe_header_match, /* header_match */
	b_mp4_probe_create, /* create */
	b_mp4_probe_destroy, /* destroy */
	b_mp4_probe_parse, /* parse */
	b_mp4_probe_stream_free /* stream free */
};

