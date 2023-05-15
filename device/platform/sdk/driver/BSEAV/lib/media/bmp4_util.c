/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_util.c $
 * $brcm_Revision: 35 $
 * $brcm_Date: 3/5/12 12:32p $
 *
 * Module Description:
 *
 * MP4 container parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_util.c $
 * 
 * 35   3/5/12 12:32p vsilyaev
 * SW7425-2179: Added support for QT MJPEG types A and B
 * 
 * 34   2/21/12 5:14p vsilyaev
 * SW7425-2422: Added support for PCM (BE/twos) audio in MP4 container
 * 
 * 33   1/11/12 6:08p vsilyaev
 * SW7425-2112: Added handling of JPEG payloads
 * 
 * 32   6/20/11 7:57p vsilyaev
 * SWBLURAY-26273: Verify size of the Protection Scheme Information
 * 
 * 31   6/20/11 6:38p vsilyaev
 * SWBLURAY-26273: Communicate MP4 DRM information to application
 * 
 * 30   5/3/11 2:49p vsilyaev
 * SWBLURAY-22800: Use common code to parser H264 meta data
 * 
 * 29   2/1/11 2:23p vsilyaev
 * SWBLURAY-24413: Relaxed parsing of video track
 * 
 * fix_bdvd_v3.0_SWBLURAY-24413/1   2/1/11 11:05a gooslin
 * Skip boxes before the "esds" box for mp4v
 * 
 * 28   12/6/10 6:24p vsilyaev
 * SW35230-1953: Added ADPCM audio
 * 
 * 27   11/8/10 3:34p vsilyaev
 * SW35230-2096: Properly NULL terminate a language string
 * 
 * 26   11/4/10 11:29a vsilyaev
 * SW35230-2096: Added extraction of the language information
 * 
 * 25   10/18/10 1:46p vsilyaev
 * SWBLURAY-22500: Merged support for MPEG audio
 * 
 * fix_bdvd_v3.0_SWBLURAY-22500/1   10/14/10 3:45p ialauder
 * Add support for MPEG audio in MP4 container
 * 
 * 24   5/6/10 3:42p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 * container
 * 
 * 23   4/8/10 6:58p vsilyaev
 * SW7405-3773: Initial support for playback of MPEG4-Part12 fragmented
 * files
 * 
 * 22   3/9/10 9:55a vsilyaev
 * SW3548-2813:Parse only header of sample_size objects
 * 
 * 21   12/8/09 10:46a vsilyaev
 * SW7405-3556: Added flag to mark encrypted tracks
 * 
 * fix_bdvd_v3.0_issue16965/2   12/7/09 6:21p ialauder
 * Add DRMI and DRMS seperation for MP4 DRM
 * 
 * fix_bdvd_v3.0_issue16965/1   12/7/09 5:34p ialauder
 * AAC DRM detection
 * 
 * 20   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/2   11/30/09 1:38p ialauder
 * AAC-HE support
 * 
 * fix_bdvd_v3.0_issue17634/1   11/25/09 4:23p ialauder
 * Changes to detect AAC-HE
 * 
 * 19   9/1/09 4:06p vsilyaev
 * SW3548-2442: Added support for AMR audio
 * 
 * 18   7/23/09 9:53a vsilyaev
 * PR 57048: Added support for parsing AMR audio tracks
 * 
 * 17   7/9/09 1:12p vsilyaev
 * PR 55705: Added support for AC3/EAC3 audio
 * 
 * 16   3/16/09 5:47p vsilyaev
 * PR 53282: Added code to derive audio sampling rate
 * 
 * fix_bdvd_v2.0_pr13666/1   3/13/09 3:55p gooslin
 * Calculate sample rate when an invalid coded sample rate is detected
 * 
 * 15   3/6/09 11:32a vsilyaev
 * PR 52903: Added missing break
 * 
 * 14   2/10/09 3:22p gmohile
 * PR 51975 : support AAC Audio ISO IEC 13818-7
 * 
 * 13   1/14/09 6:12p vsilyaev
 * PR 45242: Relaxed parsing of MP4A box
 * 
 * 12   11/25/08 10:02a vsilyaev
 * PR 49654: Relaxed parsing of avc box
 * 
 * 11   10/31/08 9:26a gmohile
 * PR 48584 : Added 3gpp support
 * 
 * 10   10/16/08 6:53p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 * 
 * 9   10/16/08 3:27p vsilyaev
 * PR 48003: Exported function to parse MP4 avcC header
 * 
 * 8   2/22/08 3:06p vsilyaev
 * PR 39885: Relaxed parsing of strings
 * 
 * 7   5/30/07 5:18p vsilyaev
 * PR 31697: Added support for MPEG4Part2 video
 * 
 * 6   5/9/07 3:05p vsilyaev
 * PR 28631: Added parsing of audio stream header
 * 
 * 5   5/1/07 11:18a vsilyaev
 * PR 28631: Fixed handling of AVC video stream
 * 
 * 4   4/30/07 4:56p vsilyaev
 * PR 28631: Added sample scheduller
 * 
 * 3   4/29/07 1:39p vsilyaev
 * PR 28631: Promoted track navigation to dedicated file
 * 
 * 2   4/26/07 11:56p vsilyaev
 * PR 28631: Added parsing of sample entry
 * 
 * 1   4/26/07 2:19p vsilyaev
 * PR 28631: MP4 types and definitions
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmp4_util.h"
#include "bkni.h"
#include "biobits.h"


BDBG_MODULE(bmp4_util);

#if defined(HUMAX_PLATFORM_BASE)
/* MS Smoooth Streaming & PIFF */
static const uint8_t UUID_EXTENDTYPE[][16] = 
{
	{0xA2,0x39,0x4F,0x52,0x5A,0x9B,0x4F,0x14,0xA2,0x44,0x6C,0x42,0x7C,0x64,0x8D,0xF4}, /* SAMPLE ENCRYPTION */
	{0x6D,0x1D,0x9B,0x05,0x42,0xD5,0x44,0xE6,0x80,0xE2,0x14,0x1D,0xAF,0xF7,0x57,0xB2}, /* TfxdBox */
	{0xD4,0x80,0x7E,0xF2,0xCA,0x39,0x46,0x95,0x8E,0x54,0x26,0xCB,0x9E,0x46,0xA7,0x9F} /* TfrfBox */
};
#endif

/* ISO/IEC 14496-12:2005 MPEG-4 Part 12 - ISO Base Media File Format */
size_t
bmp4_parse_box(batom_cursor *cursor, bmp4_box *box)
{
	uint32_t size;
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(box);

	/* page 3 */
	size = batom_cursor_uint32_be(cursor);
	box->type = batom_cursor_uint32_be(cursor);
	if(!BATOM_IS_EOF(cursor)) {
		if(size!=1) {
			box->size = size;
			return sizeof(uint32_t)+sizeof(uint32_t); 
		} 
		box->size = batom_cursor_uint64_be(cursor);
		if(!BATOM_IS_EOF(cursor)) {
			return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint64_t); 
		}
	}
	return 0;
}

static size_t
b_mp4_find_box(batom_cursor *cursor, uint32_t type)
{
    for(;;) {
        bmp4_box box;
        size_t box_hdr_size = bmp4_parse_box(cursor, &box);

        if(box_hdr_size==0) {
            break;
        }
        if(box.type == type) {
            BDBG_MSG(("b_mp4_find_box: find " B_MP4_TYPE_FORMAT ":%u", B_MP4_TYPE_ARG(box.type), (unsigned)box.size));
            return box_hdr_size;
        }
        BDBG_MSG(("b_mp4_find_box: looking " B_MP4_TYPE_FORMAT " skipping " B_MP4_TYPE_FORMAT ":%u", B_MP4_TYPE_ARG(type), B_MP4_TYPE_ARG(box.type), (unsigned)box.size));
        if(box.size>box_hdr_size) {
            batom_cursor_skip(cursor, box.size - box_hdr_size);
            continue;
        }
    }
    return 0;
}

bool
bmp4_parse_box_extended(batom_cursor *cursor, bmp4_box_extended *box)
{
	size_t size;
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(box);

	/* page 3 */
	size = batom_cursor_copy(cursor, box->usertype, sizeof(box->usertype));
	return size==sizeof(box->usertype);
}

size_t
bmp4_parse_fullbox(batom_cursor *cursor, bmp4_fullbox *box)
{
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(box);

	/* page 4 */
	box->version = batom_cursor_byte(cursor);
    box->flags = batom_cursor_uint24_be(cursor);
	return !BATOM_IS_EOF(cursor);
}

bool
bmp4_parse_filetype(batom_t box, bmp4_filetypebox *filetype)
{
	batom_cursor cursor;
	BDBG_ASSERT(box);
	BDBG_ASSERT(filetype);

	batom_cursor_from_atom(&cursor, box);

	/* page 5 */
	filetype->major_brand = batom_cursor_uint32_be(&cursor);
	filetype->minor_version = batom_cursor_uint32_be(&cursor);
	if(!BATOM_IS_EOF(&cursor)) {
		unsigned i;
		for(i=0;i<sizeof(filetype->compatible_brands)/sizeof(filetype->compatible_brands[0]);i++) {
			filetype->compatible_brands[i] = batom_cursor_uint32_be(&cursor);
			if(BATOM_IS_EOF(&cursor)) {
				break;
			}
		}
		filetype->ncompatible_brands = i;
		BDBG_MSG(("bmp4_parse_filetype: major_brand:" B_MP4_TYPE_FORMAT " minor_version:%u ncompatible_brands:%u", B_MP4_TYPE_ARG(filetype->major_brand), (unsigned)filetype->minor_version, (unsigned)filetype->ncompatible_brands));
		return true;
	}
	return false;
}

bool
bmp4_parse_movieheader(batom_t box, bmp4_movieheaderbox *movieheader)
{
	batom_cursor cursor;
	bool valid=false;

	BDBG_ASSERT(box);
	BDBG_ASSERT(movieheader);

	batom_cursor_from_atom(&cursor, box);

	/* page 16 */
	if(bmp4_parse_fullbox(&cursor, &movieheader->fullbox)) {
		unsigned i;
		switch(movieheader->fullbox.version) {
		case 0:
			movieheader->creation_time = batom_cursor_uint32_be(&cursor);
			movieheader->modification_time = batom_cursor_uint32_be(&cursor);
			movieheader->timescale = batom_cursor_uint32_be(&cursor);
			movieheader->duration = batom_cursor_uint32_be(&cursor);
			break;
		case 1:
			movieheader->creation_time = batom_cursor_uint64_be(&cursor);
			movieheader->modification_time = batom_cursor_uint64_be(&cursor);
			movieheader->timescale = batom_cursor_uint32_be(&cursor);
			movieheader->duration = batom_cursor_uint64_be(&cursor);
			break;
		default:
			BDBG_WRN(("bmp4_parse_movieheader: unknown version %u", movieheader->fullbox.version));
			return false;
		}
		movieheader->rate = batom_cursor_uint32_be(&cursor);
		movieheader->volume = batom_cursor_uint16_be(&cursor);
		batom_cursor_skip(&cursor, 
				sizeof(uint16_t) +  /* reserved */
				2*sizeof(uint32_t)  /* reserved */
				);
		for(i=0;i<sizeof(movieheader->matrix)/sizeof(movieheader->matrix[0]);i++) {
			movieheader->matrix[i] = batom_cursor_uint32_be(&cursor);
		}
		batom_cursor_skip(&cursor, sizeof(uint32_t) /* pre_defined */ );
		movieheader->next_track_ID = batom_cursor_uint32_be(&cursor);
		valid = movieheader->timescale>0 && (!BATOM_IS_EOF(&cursor));
		if(valid) {
			BDBG_MSG(("bmp4_parse_movieheader: version:%u creation_time:%u modification_time:%u timescale:%u duration:%u(%u sec) rate:%u volume:%u next_track_ID:%u", (unsigned)movieheader->fullbox.version, (unsigned)movieheader->creation_time, (unsigned)movieheader->modification_time, (unsigned)movieheader->timescale, (unsigned)movieheader->duration, (unsigned)(movieheader->duration/movieheader->timescale), (unsigned)movieheader->rate, (unsigned)movieheader->volume, (unsigned)movieheader->next_track_ID));
		}
	}
	return valid;
}

bool
bmp4_parse_trackheader(batom_t box, bmp4_trackheaderbox *trackheader)
{
	batom_cursor cursor;
	bool valid=false;

	BDBG_ASSERT(box);
	BDBG_ASSERT(trackheader);

	batom_cursor_from_atom(&cursor, box);

	/* page 17 */
	if(bmp4_parse_fullbox(&cursor, &trackheader->fullbox)) {
		unsigned i;
		switch(trackheader->fullbox.version) {
		case 0:
			trackheader->creation_time = batom_cursor_uint32_be(&cursor);
			trackheader->modification_time = batom_cursor_uint32_be(&cursor);
			trackheader->track_ID = batom_cursor_uint32_be(&cursor);
			batom_cursor_skip(&cursor, sizeof(uint32_t));
			trackheader->duration = batom_cursor_uint32_be(&cursor);
			break;
		case 1:
			trackheader->creation_time = batom_cursor_uint64_be(&cursor);
			trackheader->modification_time = batom_cursor_uint64_be(&cursor);
			trackheader->track_ID = batom_cursor_uint32_be(&cursor);
			batom_cursor_skip(&cursor, sizeof(uint32_t));
			trackheader->duration = batom_cursor_uint64_be(&cursor);
			break;
		default:
			BDBG_WRN(("bmp4_parse_movieheader: unknown version %u", trackheader->fullbox.version));
			return false;
		}
		batom_cursor_skip(&cursor, 2*sizeof(uint32_t));
		trackheader->layer = batom_cursor_uint16_be(&cursor);
		trackheader->alternate_group = batom_cursor_uint16_be(&cursor);
		trackheader->volume = batom_cursor_uint16_be(&cursor);
		batom_cursor_skip(&cursor, sizeof(uint16_t));
		for(i=0;i<sizeof(trackheader->matrix)/sizeof(trackheader->matrix[0]);i++) {
			trackheader->matrix[i] = batom_cursor_uint32_be(&cursor);
		}
		trackheader->width = batom_cursor_uint32_be(&cursor);
		trackheader->height = batom_cursor_uint32_be(&cursor);
		valid = !BATOM_IS_EOF(&cursor);
		if(valid) {
			BDBG_MSG(("bmp4_parse_trackheader: version:%u track_ID:%u duration:%u layer:%u alternate_group:%u volume:%u width:%u height:%u", (unsigned)trackheader->fullbox.version, (unsigned)trackheader->track_ID, (unsigned)trackheader->duration, (unsigned)trackheader->layer, (unsigned)trackheader->alternate_group, (unsigned)trackheader->volume, (unsigned)trackheader->width>>16, (unsigned)trackheader->height>>16));
		}
	}
	return valid;
}

bool
bmp4_parse_mediaheader(batom_t box, bmp4_mediaheaderbox *mediaheader)
{
	batom_cursor cursor;
	bool valid=false;
    uint16_t language;

	BDBG_ASSERT(box);
	BDBG_ASSERT(mediaheader);

	batom_cursor_from_atom(&cursor, box);

	/* page 20 */
	if(bmp4_parse_fullbox(&cursor, &mediaheader->fullbox)) {
		switch(mediaheader->fullbox.version) {
		case 0:
			mediaheader->creation_time = batom_cursor_uint32_be(&cursor);
			mediaheader->modification_time = batom_cursor_uint32_be(&cursor);
			mediaheader->timescale = batom_cursor_uint32_be(&cursor);
			mediaheader->duration = batom_cursor_uint32_be(&cursor);
			break;

		case 1:
			mediaheader->creation_time = batom_cursor_uint64_be(&cursor);
			mediaheader->modification_time = batom_cursor_uint64_be(&cursor);
			mediaheader->timescale = batom_cursor_uint32_be(&cursor);
			mediaheader->duration = batom_cursor_uint64_be(&cursor);
			break;
		default:
			BDBG_WRN(("bmp4_parse_mediaheader: unknown version %u", mediaheader->fullbox.version));
			return false;
		}
        valid = mediaheader->timescale>0 && (!BATOM_IS_EOF(&cursor));
        mediaheader->language[0] = '\0';
        language = batom_cursor_uint16_be(&cursor);
        if(!BATOM_IS_EOF(&cursor)) {
            mediaheader->language[0] = 0x60 + B_GET_BITS(language, 14, 10);
            mediaheader->language[1] = 0x60 + B_GET_BITS(language,  9, 5);
            mediaheader->language[2] = 0x60 + B_GET_BITS(language,  4, 0);
            mediaheader->language[3] = '\0';
        }
        if(valid) {
            BDBG_MSG(("bmp4_parse_mediaheader: version:%u creation_time:%u modification_time:%u timescale:%u(%u sec) duration:%u language:%u", (unsigned)mediaheader->fullbox.version, (unsigned)mediaheader->creation_time, (unsigned)mediaheader->modification_time, (unsigned)mediaheader->timescale, (unsigned)(mediaheader->duration/mediaheader->timescale), (unsigned)mediaheader->duration, mediaheader->language));
        }
	}
	return valid;
}

bool 
bmp4_parse_string(batom_cursor *cursor, char *string, size_t strlen)
{
	unsigned i;

	BDBG_ASSERT(cursor);
	BDBG_ASSERT(strlen>0);
	BDBG_ASSERT(string);

	for(i=0;i<strlen;i++) {
		int ch = batom_cursor_next(cursor);
		if(ch==BATOM_EOF) {
			i++;
            break;
		}
		string[i]=ch;
		if(ch==0) {
			return true;
		}
	}
	if(i>0) {
		string[i-1] = '\0';
		return true;
	} else {
		return false;
	}
}

bool
bmp4_parse_handler(batom_t box, bmp4_handlerbox *handler)
{
	batom_cursor cursor;
	bool valid=false;
	BDBG_ASSERT(box);
	BDBG_ASSERT(handler);

	batom_cursor_from_atom(&cursor, box);

	/* page 20 */
	if(bmp4_parse_fullbox(&cursor, &handler->fullbox)) {
		batom_cursor_skip(&cursor, sizeof(uint32_t) /* pre defined */);
		handler->handler_type = batom_cursor_uint32_be(&cursor);
		batom_cursor_skip(&cursor, 3*sizeof(uint32_t) /* pre defined */);
		valid = bmp4_parse_string(&cursor, handler->name, sizeof(handler->name));
		if(valid) {
			BDBG_MSG(("bmp4_parse_handler: version:%u handler_type:" B_MP4_TYPE_FORMAT " name:'%s'", (unsigned)handler->fullbox.version, B_MP4_TYPE_ARG(handler->handler_type), handler->name));
		}
	}
	return valid;
}


bool 
bmp4_parse_visualsampleentry(batom_cursor *cursor, bmp4_visualsampleentry *entry)
{
	bool valid;

	batom_cursor_skip(cursor, sizeof(uint16_t) + sizeof(uint16_t) + 3*sizeof(uint32_t));
	entry->width = batom_cursor_uint16_be(cursor);
	entry->height = batom_cursor_uint16_be(cursor);
	entry->horizresolution = batom_cursor_uint32_be(cursor);
	entry->vertresolution = batom_cursor_uint32_be(cursor);
	batom_cursor_skip(cursor, sizeof(uint32_t));
	entry->frame_count = batom_cursor_uint16_be(cursor);
	batom_cursor_copy(cursor, entry->compressorname, sizeof(entry->compressorname));
	entry->compressorname[sizeof(entry->compressorname)-1] = '\0';
	entry->depth = batom_cursor_uint16_be(cursor);
	batom_cursor_skip(cursor, sizeof(int16_t));
	valid = !BATOM_IS_EOF(cursor);
	if(valid) {
		BDBG_MSG(("bmp4_parse_visualsampleentry: width:%u height:%u horizresolution:%u vertresolution:%u frame_count:%u compressorname:%s depth:%u", (unsigned)entry->width, (unsigned)entry->height, (unsigned)entry->horizresolution>>16, (unsigned)entry->vertresolution>>16, (unsigned)entry->frame_count, entry->compressorname, (unsigned)entry->depth));
	}
	return valid;
}

bool 
bmp4_parse_audiosampleentry(batom_cursor *cursor, bmp4_audiosampleentry *entry)
{
	bool valid;

	batom_cursor_skip(cursor, 2*sizeof(uint32_t));
	entry->channelcount = batom_cursor_uint16_be(cursor);
	entry->samplesize = batom_cursor_uint16_be(cursor);
	batom_cursor_skip(cursor, sizeof(uint16_t) + sizeof(uint16_t));
	entry->samplerate = batom_cursor_uint32_be(cursor);
	valid = !BATOM_IS_EOF(cursor);
	if(valid) {
		BDBG_MSG(("bmp4_parse_audiosampleentry: channelcount:%u samplesize:%u samplerate:%u", (unsigned)entry->channelcount, (unsigned)entry->samplesize, (unsigned)entry->samplerate>>16));
	}
	return valid;
}

bool 
bmp4_parse_sample_avcC(batom_cursor *cursor, bmp4_sample_avc *avc, size_t entry_data_size)
{
    BDBG_ASSERT(cursor);
	BDBG_ASSERT(avc);

    batom_cursor_copy(cursor, avc->data, entry_data_size);
    if(!bmedia_read_h264_meta(&avc->meta, avc->data, entry_data_size)) {
        return false;
    }
    BDBG_MSG(("b_mp4_parse_sample_avc: configurationVersion:%u AVCProfileIndication:%u profile_compatibility:%u AVCLevelIndication:%u lengthSize:%u nSequenceParameterSets:%u nPictureParameterSets:%u", (unsigned)avc->meta.configurationVersion, (unsigned)avc->meta.profileIndication, (unsigned)avc->meta.profileCompatibility, (unsigned)avc->meta.levelIndication, (unsigned)avc->meta.nalu_len, (unsigned)avc->meta.sps.no, (unsigned)avc->meta.pps.no));
    return true;
}

static bool 
b_mp4_parse_sample_avc(batom_cursor *cursor, bmp4_sample_avc *avc, size_t entry_data_size)
{
	BDBG_ASSERT(cursor);
	BDBG_ASSERT(avc);
	if(bmp4_parse_visualsampleentry(cursor, &avc->visual)) {
		bmp4_box box;

        for(;;) {
            size_t box_hdr_size = bmp4_parse_box(cursor, &box);
            if(box_hdr_size==0) {
                break;
            }
            if(box.type==BMP4_TYPE('a','v','c','C')) {
                return bmp4_parse_sample_avcC(cursor, avc, entry_data_size);
            }
            /* skip unknown boxes */
			BDBG_WRN(("b_mp4_parse_sample_avc: discarding unknown sample " B_MP4_TYPE_FORMAT " %u:%u", B_MP4_TYPE_ARG(box.type), (unsigned)box.size, entry_data_size));
            if(entry_data_size<box.size) {
                break;
            }
            entry_data_size -= box.size;
            batom_cursor_skip(cursor, box.size-box_hdr_size);
        }
	}
	return false;
}


static bool 
b_mp4_parse_sample_mp4a(batom_cursor *cursor, bmp4_sample_mp4a *mp4a, size_t entry_data_size)
{
    size_t last = batom_cursor_pos(cursor)+entry_data_size;

	if(bmp4_parse_audiosampleentry(cursor, &mp4a->audio)) {
        for(;;) {
            batom_checkpoint chk;
            bmp4_box box;
            bmp4_fullbox fullbox;
            size_t box_hdr_size;

            BATOM_SAVE(cursor, &chk);
            box_hdr_size = bmp4_parse_box(cursor, &box);

            if(box_hdr_size == 0 || box.type!=BMP4_TYPE('e','s','d','s')) {
                /* not "really" MPEG-4 Part12/Part14 streams have esds box at randdom position in the MP4A box, so skip unknown data and try over */
                if(BATOM_IS_EOF(cursor) || batom_cursor_pos(cursor)>=last) {
                    break;
                }
	            batom_cursor_rollback(cursor, &chk);
                batom_cursor_skip(cursor, 2);
                continue;
            }
            if(!bmp4_parse_fullbox(cursor, &fullbox)) {
                return false;
            }
            if(bmpeg4_parse_es_descriptor(cursor, &mp4a->mpeg4)) {
                if(mp4a->mpeg4.objectTypeIndication==BMPEG4_Audio_ISO_IEC_14496_3 || mp4a->mpeg4.objectTypeIndication==BMPEG4_Audio_ISO_IEC_13818_7) {
#if BDBG_DEBUG_BUILD
                    const char *codec;
                    unsigned sample_rate;
                    static const unsigned sample_rate_table[] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350};
                    switch(mp4a->mpeg4.decoder.iso_14496_3.audioObjectType) {
                    case 1:
                        codec = "AAC main";
                        break;
                    case 2:
                        codec = "AAC LC";
                        break;
                    case 3:
                        codec = "AAC SSR";
                        break;
                    case 4:
                        codec = "AAC LTP";
                        break;
                    case 5: 
                        codec = "AAC SBR";
                        break;
                    default:
                        codec = "";
                        break;
                    }
                    if(mp4a->mpeg4.decoder.iso_14496_3.samplingFrequencyIndex<sizeof(sample_rate_table)/sizeof(*sample_rate_table)) {
                        sample_rate = sample_rate_table[mp4a->mpeg4.decoder.iso_14496_3.samplingFrequencyIndex];
                    } else {
                        sample_rate = 0;
                    }
                    BDBG_MSG(("b_mp4_parse_sample_mp4a: codec:%s(%#x) sampling_rate:%u(%u) channelConfiguration:%u", codec, mp4a->mpeg4.decoder.iso_14496_3.audioObjectType, sample_rate, mp4a->mpeg4.decoder.iso_14496_3.samplingFrequencyIndex, mp4a->mpeg4.decoder.iso_14496_3.channelConfiguration));
#endif  /* BDBG_DEBUG_BUILD */
                    return true;
                } else if (mp4a->mpeg4.objectTypeIndication==BMPEG4_Audio_ISO_IEC_11172_3) {
                    return true;
                } else {
                    BDBG_WRN(("b_mp4_parse_sample_mp4a: unsupported objectTypeIndication %#x", mp4a->mpeg4.objectTypeIndication));
                }
            }
            break;
        }
	}
	return false;
}

static bool 
b_mp4_parse_sample_mp4v(batom_cursor *cursor, bmp4_sample_mp4v *mp4v)
{
	if(bmp4_parse_visualsampleentry(cursor, &mp4v->visual)) {
		bmp4_fullbox fullbox;

        if(!b_mp4_find_box(cursor, BMP4_TYPE('e','s','d','s'))) {
            return false;
        }
		if(!bmp4_parse_fullbox(cursor, &fullbox)) {
			return false;
		}
		if(bmpeg4_parse_es_descriptor(cursor, &mp4v->mpeg4)) {
			if(mp4v->mpeg4.objectTypeIndication==BMPEG4_Video_ISO_IEC_14496_2) {
				return true;
			} else {
			 	BDBG_WRN(("b_mp4_parse_sample_mp4v: unsupported objectTypeIndication %#x", mp4v->mpeg4.objectTypeIndication));
			}
		}
	}
	return false;
}

static bool 
b_mp4_parse_sample_s263(batom_cursor *cursor, bmp4_sample_s263 *s263)
{
	if(bmp4_parse_visualsampleentry(cursor, &s263->visual)) {
		bmp4_box box;	

		if(!bmp4_parse_box(cursor, &box)) {
			return false;
		}
		if(box.type!=BMP4_TYPE('d','2','6','3')) {
			return false;
		}

		s263->decspecinfo.vendor = batom_cursor_uint32_be(cursor);
		s263->decspecinfo.decoder_version = batom_cursor_byte(cursor);
		s263->decspecinfo.h263_level = batom_cursor_byte(cursor);
		s263->decspecinfo.h263_profile = batom_cursor_byte(cursor);

		if(cursor->left) {
			if(!bmp4_parse_box(cursor, &box)) {
				return false;
			}
			if(box.type!=BMP4_TYPE('b','i','t','r')) {
				return false;
			}
			
			s263->decspecinfo.decbitrate.avg_bitrate = batom_cursor_uint32_be(cursor);
			s263->decspecinfo.decbitrate.max_bitrate = batom_cursor_uint32_be(cursor);
		}
		
		return true;
	}

	return false;
}

static bool 
b_mp4_parse_sample_amr(batom_cursor *cursor, bmp4_sample_amr *amr, size_t entry_data_size)
{
    size_t last = batom_cursor_pos(cursor)+entry_data_size;
	if(bmp4_parse_audiosampleentry(cursor, &amr->audio)) {
        for(;;) {
            batom_checkpoint chk;
            bmp4_box box;
            size_t box_hdr_size;

            BATOM_SAVE(cursor, &chk);
            box_hdr_size = bmp4_parse_box(cursor, &box);

            if(box_hdr_size == 0 || box.type!=BMP4_TYPE('d','a','m','r')) {
                /* there is some junk stored priort to the AMRSpecificBox */
                if(BATOM_IS_EOF(cursor) || batom_cursor_pos(cursor)>=last) {
                    break;
                }
	            batom_cursor_rollback(cursor, &chk);
                batom_cursor_skip(cursor, 2);
                continue;
            }
            break;
        }
        amr->vendor = batom_cursor_uint32_be(cursor);
        amr->decoder_version = batom_cursor_byte(cursor);
        amr->mode_set = batom_cursor_uint16_be(cursor);
        amr->mode_change_period = batom_cursor_byte(cursor);
        amr->frames_per_sample = batom_cursor_byte(cursor);

        if(!BATOM_IS_EOF(cursor)) {
            BDBG_MSG(("b_mp4_parse_sample_amr: vendor(" B_MP4_TYPE_FORMAT ") decoder_version:%u mode_set:%u mode_change_period:%u frames_per_sample:%u", B_MP4_TYPE_ARG(amr->vendor), amr->decoder_version, amr->mode_set, amr->mode_change_period, amr->frames_per_sample));
            return true;
        }
	}
	return false;
}


static bool 
b_mp4_parse_sample_ms(batom_cursor *cursor, bmp4_sample_ms *ms)
{
    /* QuickTime File Format Specification , Sound Sample Description Extensions, p 136 */
	if(bmp4_parse_audiosampleentry(cursor, &ms->audio)) {
        uint32_t samplesPerPacket;
        uint32_t bytesPerPacket;
        uint32_t bytesPerFrame;
        uint32_t bytesPerSample;

        samplesPerPacket = batom_cursor_uint32_be(cursor);
        bytesPerPacket = batom_cursor_uint32_be(cursor);
        bytesPerFrame = batom_cursor_uint32_be(cursor);
        bytesPerSample = batom_cursor_uint32_be(cursor);
        if(b_mp4_find_box(cursor, BMP4_TYPE('w','a','v','e'))) {
            if(b_mp4_find_box(cursor, BMP4_TYPE('f','r','m','a'))) {
                uint32_t type = batom_cursor_uint32_be(cursor);
                if(b_mp4_find_box(cursor, type)) {
                    return bmedia_read_waveformatex(&ms->waveformat, cursor);
                }
            }
        }
    }
    return false;
}

static bool 
b_mp4_parse_sample_mjpeg(batom_cursor *cursor, bmp4_sample_mjpeg *mjpeg, uint32_t type)
{
    mjpeg->type = type;
	if(bmp4_parse_visualsampleentry(cursor, &mjpeg->visual)) {
		return true;
	}
	return false;
}

static bool 
b_mp4_parse_sample_twos(batom_cursor *cursor, bmp4_sample_twos *twos)
{
	if(bmp4_parse_audiosampleentry(cursor, &twos->audio)) {
        return true;
    }
    return false;
}

#define B_MP4_SAMPLE_ENTRY_SIZE        (BMEDIA_FIELD_LEN(SampleEntry:reserved, const uint8_t [6])+ BMEDIA_FIELD_LEN(SampleEntry:mdata_reference_index, uint16_t))
#define B_MP4_VISUAL_SAMPLE_ENTRY_SIZE (B_MP4_SAMPLE_ENTRY_SIZE+\
					                   BMEDIA_FIELD_LEN(VisualSampleEntry:pre_defined, uint16_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:reserved, const uint16_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:prefetched, uint32_t [3])+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:width, uint16_t)+BMEDIA_FIELD_LEN(VisualSampleEntry:height, uint16_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:horizresolution, uint32_t)+BMEDIA_FIELD_LEN(VisualSampleEntry:vertresolution, uint32_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:reserved, const uint32_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:frame_count, uint16_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:compressorname, uint8_t [32])+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:depth, uint16_t)+\
                                       BMEDIA_FIELD_LEN(VisualSampleEntry:predefined, int16_t))

#define B_MP4_AUDIO_SAMPLE_ENTRY_SIZE  (B_MP4_SAMPLE_ENTRY_SIZE+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:reserved, const uint32_t [2])+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:channelcount, int16_t)+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:samplesize , int16_t)+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:pre_defined, int16_t)+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:reserved, const int16_t)+\
                                       BMEDIA_FIELD_LEN(AudioSampleEntry:samplerate, int32_t))


bool 
bmp4_parse_sample_info(batom_t box, bmp4_sample_info *sample, uint32_t handler_type)
{
	batom_cursor cursor;
	unsigned i;

	BDBG_ASSERT(box);
	BDBG_ASSERT(sample);


	batom_cursor_from_atom(&cursor, box);

	sample->entry_count = 0;
	if(!bmp4_parse_fullbox(&cursor, &sample->fullbox)) {
		return false;
	}
	sample->entry_count = batom_cursor_uint32_be(&cursor);
	if(BATOM_IS_EOF(&cursor)) {
		return false;
	}
	if(sample->entry_count>B_MP4_MAX_SAMPLE_ENTRIES) {
		BDBG_WRN(("bmp4_parse_sample: truncating number of samples %u -> %u", sample->entry_count, B_MP4_MAX_SAMPLE_ENTRIES));
		sample->entry_count=B_MP4_MAX_SAMPLE_ENTRIES;
	}
	for(i=0;i<sample->entry_count;i++) {
		size_t box_hdr_size;
		size_t start_pos;
		size_t end_pos;
		size_t entry_data_size;
		bmp4_sample_type type;
		bmp4_sampleentry *entry;
		bmp4_box entry_box;
        bool encrypted = false;
        batom_cursor protection_scheme_information_cursor;
        size_t protection_scheme_information_size = 0;

		start_pos = batom_cursor_pos(&cursor);
		box_hdr_size = bmp4_parse_box(&cursor, &entry_box);
		if(box_hdr_size==0 || entry_box.size > batom_len(box)) {
			BDBG_WRN(("bmp4_parse_sample: invalid sample %u", i));
			goto error_sample;
		}
        if( (handler_type==BMP4_HANDLER_VISUAL && entry_box.type==BMP4_SAMPLE_ENCRYPTED_VIDEO) || 
            (handler_type==BMP4_HANDLER_AUDIO && entry_box.type==BMP4_SAMPLE_ENCRYPTED_AUDIO) ) {
            batom_cursor sinf_cursor;

            encrypted = true;
            BATOM_CLONE(&sinf_cursor, &cursor);
            batom_cursor_skip(&sinf_cursor, handler_type==BMP4_HANDLER_VISUAL?B_MP4_VISUAL_SAMPLE_ENTRY_SIZE:B_MP4_AUDIO_SAMPLE_ENTRY_SIZE);
            for(;;) {
		        size_t sinf_box_hdr_size;
		        bmp4_box sinf_box;

                BATOM_CLONE(&protection_scheme_information_cursor, &sinf_cursor);
                sinf_box_hdr_size = bmp4_parse_box(&sinf_cursor, &sinf_box);
                if(sinf_box_hdr_size==0 || sinf_box_hdr_size > sinf_box.size || batom_cursor_reserve(&protection_scheme_information_cursor, sinf_box.size)!=sinf_box.size) {
                    break;
                }
                if(sinf_box.type == BMP4_TYPE('s','i','n','f')) {
                    protection_scheme_information_size = sinf_box.size;
                    if(protection_scheme_information_size>sizeof(entry->protection_scheme_information)) {
                        break;
                    }
                    sinf_box_hdr_size = bmp4_parse_box(&sinf_cursor, &sinf_box);
                    if(sinf_box_hdr_size==0 || sinf_box_hdr_size > sinf_box.size) {
                        protection_scheme_information_size = 0;
                        break;
                    }
                    if(sinf_box.type == BMP4_TYPE('f','r','m','a')) {
                        uint32_t data_format = batom_cursor_uint32_be(&sinf_cursor);
                        if(!BATOM_IS_EOF(&sinf_cursor)) {
                            entry_box.type = data_format;
                        } else {
                            protection_scheme_information_size = 0;
                        }
                    }
                    break;
                }
                batom_cursor_skip(&sinf_cursor, sinf_box.size - sinf_box_hdr_size);
            }
            if(protection_scheme_information_size==0) {
                BDBG_WRN(("bmp4_parse_sample: can't find useful Protection Scheme Informatio Box ('sinf') for protected track"));
            }
        }

		entry_data_size = 0;
		type = bmp4_sample_type_unknown;
		switch(handler_type) {
		case BMP4_HANDLER_VISUAL:
			switch(entry_box.type) {
			case BMP4_SAMPLE_AVC:
				BDBG_MSG(("bmp4_parse_sample: avc video"));
				type = bmp4_sample_type_avc;
				entry_data_size = 
					box_hdr_size +
                    B_MP4_VISUAL_SAMPLE_ENTRY_SIZE +
					sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) /* AVCDecoderConfigurationRecord */;
				if(entry_data_size>entry_box.size)  {
					BDBG_WRN(("bmp4_parse_sample: invalid size of AVC box %u>%u", entry_data_size, (unsigned)entry_box.size));
					goto error_sample;
				}
				entry_data_size = entry_box.size - entry_data_size;
				break;
			case BMP4_SAMPLE_MP4V:
				BDBG_MSG(("bmp4_parse_sample: MPEG4-Part2 video"));
				type = bmp4_sample_type_mp4v;
				break;
			case BMP4_SAMPLE_S263:
				BDBG_MSG(("bmp4_parse_sample: H.263 video"));
				type = bmp4_sample_type_s263;
				break;
   			case BMP4_SAMPLE_JPEG:
   			case BMP4_SAMPLE_MJPA:
   			case BMP4_SAMPLE_MJPB:
				BDBG_MSG(("bmp4_parse_sample: MJPEG video " B_MP4_TYPE_FORMAT, B_MP4_TYPE_ARG(entry_box.type)));
				type = bmp4_sample_type_mjpeg;
				break;
            case BMP4_SAMPLE_DRMI:
                BDBG_MSG(("bmp4_parse_sample: DRM video"));
                type = bmp4_sample_type_drmi;
                break;
			default:
				break;
			}
            break;
		case BMP4_HANDLER_AUDIO:
			switch(entry_box.type) {
			case BMP4_SAMPLE_MP4A:
				BDBG_MSG(("bmp4_parse_sample: MP4A(AAC) audio"));
				type = bmp4_sample_type_mp4a;
				break;
            case BMP4_SAMPLE_AC3:
				BDBG_MSG(("bmp4_parse_sample: AC-3 audio"));
				type = bmp4_sample_type_ac3;
				break;
            case BMP4_SAMPLE_EAC3:
				BDBG_MSG(("bmp4_parse_sample: AC-3 audio"));
				type = bmp4_sample_type_eac3;
				break;
            case BMP4_SAMPLE_SAMR:
				BDBG_MSG(("bmp4_parse_sample: SAMR audio"));
				type = bmp4_sample_type_samr;
				break;
            case BMP4_SAMPLE_SAWB:
				BDBG_MSG(("bmp4_parse_sample: SAWB audio"));
				type = bmp4_sample_type_sawb;
				break;
            case BMP4_SAMPLE_SAWP:
				BDBG_MSG(("bmp4_parse_sample: SAWO audio"));
				type = bmp4_sample_type_sawp;
				break;
            case BMP4_SAMPLE_DRMS:
                BDBG_MSG(("bmp4_parse_sample: DRM audio"));
                type = bmp4_sample_type_drms;
                break;
            case BMP4_SAMPLE_QT_IMA_ADPCM:
                BDBG_MSG(("bmp4_parse_sample: IMA ADPCM audio"));
                type = bmp4_sample_type_qt_ima_adpcm;
                break;
   			case BMP4_SAMPLE_TWOS:
				BDBG_MSG(("bmp4_parse_sample: TWOS audio"));
				type = bmp4_sample_type_twos;
				break;
			default:
				break;
			}
		default:
			break;
		}
		entry = BKNI_Malloc(sizeof(*entry)+entry_data_size);
		if(!entry) {
			BDBG_ERR(("bmp4_parse_sample: can't allocate %u bytes", sizeof(*entry)+entry_data_size));
			goto error_sample;
		}
		entry->type = entry_box.type;
		entry->sample_type = type;
		batom_cursor_skip(&cursor, 6*sizeof(uint8_t));
		entry->data_reference_index = batom_cursor_uint16_be(&cursor);
        entry->encrypted = encrypted;
        entry->protection_scheme_information_size = protection_scheme_information_size;
        if(protection_scheme_information_size) {
            BDBG_ASSERT(protection_scheme_information_size <= sizeof(entry->protection_scheme_information));
            batom_cursor_copy(&protection_scheme_information_cursor, entry->protection_scheme_information, protection_scheme_information_size);
        }
		switch(type) {
		case bmp4_sample_type_avc:
			if(!b_mp4_parse_sample_avc(&cursor, &entry->codec.avc, entry_data_size)) {
				BDBG_WRN(("bmp4_parse_sample: error while parsing AVC sample"));
				entry->sample_type = bmp4_sample_type_unknown;
			}
			break;
		case bmp4_sample_type_mp4a:
            entry_data_size = batom_cursor_pos(&cursor);
            if(  entry_data_size >= start_pos && entry_box.size > (entry_data_size - start_pos) ) {
                entry_data_size = entry_box.size - (entry_data_size - start_pos);
            }
			if(!b_mp4_parse_sample_mp4a(&cursor, &entry->codec.mp4a, entry_data_size)) {
				BDBG_WRN(("bmp4_parse_sample: error while parsing MP4A sample"));
				entry->sample_type = bmp4_sample_type_unknown;
			}
            /* MPEG 1/2/3 audio is listed under the MP4A box */
            if (entry->codec.mp4a.mpeg4.objectTypeIndication == BMPEG4_Audio_ISO_IEC_11172_3) {
                entry->sample_type = bmp4_sample_type_mpg;
            }
			break;
		case bmp4_sample_type_mp4v:
			if(!b_mp4_parse_sample_mp4v(&cursor, &entry->codec.mp4v)) {
				entry->sample_type = bmp4_sample_type_unknown;
			}
			break;
		case bmp4_sample_type_s263:
			if(!b_mp4_parse_sample_s263(&cursor, &entry->codec.s263)) {
				entry->sample_type = bmp4_sample_type_unknown;
			}
			break;
        case bmp4_sample_type_ac3:
        case bmp4_sample_type_eac3:
	        if(!bmp4_parse_audiosampleentry(&cursor, &entry->codec.ac3.audio)) {
				entry->sample_type = bmp4_sample_type_unknown;
            }
            break;
        case bmp4_sample_type_samr:
        case bmp4_sample_type_sawb:
        case bmp4_sample_type_sawp:
            entry_data_size = batom_cursor_pos(&cursor);
            if(  entry_data_size >= start_pos && entry_box.size > (entry_data_size - start_pos) ) {
                entry_data_size = entry_box.size - (entry_data_size - start_pos);
            }
	        if(!b_mp4_parse_sample_amr(&cursor, &entry->codec.amr, entry_data_size)) {
				entry->sample_type = bmp4_sample_type_unknown;
            }
            break;
        case bmp4_sample_type_qt_ima_adpcm:
	        if(!b_mp4_parse_sample_ms(&cursor, &entry->codec.ms)) {
				entry->sample_type = bmp4_sample_type_unknown;
            }
            break;
		case bmp4_sample_type_mjpeg:
			if(!b_mp4_parse_sample_mjpeg(&cursor, &entry->codec.mjpeg, entry_box.type)) {
				entry->sample_type = bmp4_sample_type_unknown;
			}
			break;
		case bmp4_sample_type_twos:
			if(!b_mp4_parse_sample_twos(&cursor, &entry->codec.twos)) {
				entry->sample_type = bmp4_sample_type_unknown;
			}
			break;
		default:
			BDBG_WRN(("bmp4_parse_sample: unknown sample " B_MP4_TYPE_FORMAT " for handler " B_MP4_TYPE_FORMAT , B_MP4_TYPE_ARG(entry_box.type), B_MP4_TYPE_ARG(handler_type)));
			break;
		}
		end_pos = batom_cursor_pos(&cursor);
		BDBG_ASSERT(end_pos>=start_pos);
		end_pos -= start_pos;
		if(entry_box.size>=end_pos) {
			batom_cursor_skip(&cursor, entry_box.size - end_pos);
		} else {
			BDBG_WRN(("bmp4_parse_sample: error while parsing entry"));
			entry->sample_type = bmp4_sample_type_unknown;
		}
		sample->entries[i] = entry;
	}

error_sample:
	sample->entry_count = i;
	return true;
}


void 
bmp4_free_sample_info(bmp4_sample_info *sample)
{
	unsigned i;
	BDBG_ASSERT(sample);

	for(i=0;i<sample->entry_count;i++) {
		BKNI_Free(sample->entries[i]);
	}
	sample->entry_count = 0;
	return;
}


bool
bmp4_parse_sample_size_header(batom_cursor *cursor, bmp4_sample_size_header *header)
{
	if(!bmp4_parse_fullbox(cursor, &header->fullbox)) {
		return false;
	}
    /* Page 30 */
    header->default_sample_size = batom_cursor_uint32_be(cursor);
    header->sample_count = batom_cursor_uint32_be(cursor);
    return !BATOM_IS_EOF(cursor);
}

bool
bmp4_parse_compact_sample_size_header(batom_cursor *cursor, bmp4_compact_sample_size_header *header)
{
    uint32_t reserved;
	if(!bmp4_parse_fullbox(cursor, &header->fullbox)) {
		return false;
	}
    /* Page 30 */
    reserved = batom_cursor_uint24_be(cursor);
    header->field_size = batom_cursor_byte(cursor);
    header->sample_count = batom_cursor_uint32_be(cursor);
    return !BATOM_IS_EOF(cursor);
}

#if 0
/* unused functions */
bool 
bmp4_parse_sample_size(batom_t box, bmp4_sample_size *sample)
{
	batom_cursor cursor;
	bool valid;
    bmp4_sample_size_header header;

	BDBG_ASSERT(box);
	BDBG_ASSERT(sample);

	batom_cursor_from_atom(&cursor, box);

	if(!bmp4_parse_sample_size_header(&cursor, &header)) {
		return false;
	}
    sample->sample_count = header.sample_count;
    if (!header.default_sample_size) {
        /* We currently do not need the sample table */
        batom_cursor_skip(&cursor, sample->sample_count*sizeof(uint32_t));
    }
    valid = sample->sample_count>0 && (!BATOM_IS_EOF(&cursor));
	if(valid) {
		BDBG_MSG(("bmp4_parse_sample_size: default_sample_size=%u sample_count=%u", header.default_sample_size, sample->sample_count));
	}

	return valid;
}

bool 
bmp4_parse_compact_sample_size(batom_t box, bmp4_sample_size *sample)
{
	batom_cursor cursor;
	bool valid;
    bmp4_compact_sample_size_header header;

	BDBG_ASSERT(box);
	BDBG_ASSERT(sample);


	batom_cursor_from_atom(&cursor, box);
    if(!bmp4_parse_compact_sample_size_header(&cursor, &header)) {
        return false;
    }

    /* Page 30 */
    sample->sample_count = header.sample_count;

    /* Valid feild size values are 16, 8 and 4. If field size is 4 with an odd number of samples, then the last byte is padded. */
    batom_cursor_skip(&cursor, (sample->sample_count*header.field_size + (header.field_size & 0x4))/8);

    valid = sample->sample_count>0 && (!BATOM_IS_EOF(&cursor));
	if(valid) {
		BDBG_MSG(("bmp4_parse_sample_size: sample_count=%u", sample->sample_count));
	}

	return valid;
}
#endif
/* page 47 */
bool 
bmp4_parse_trackextends(batom_t box, bmp4_trackextendsbox *track_extends)
{
    batom_cursor cursor;
	bmp4_fullbox fullbox;

    batom_cursor_from_atom(&cursor, box);

	if(!bmp4_parse_fullbox(&cursor, &fullbox)) {
		return false;
	}
    track_extends->track_ID = batom_cursor_uint32_be(&cursor);
    track_extends->default_sample_description_index = batom_cursor_uint32_be(&cursor);
    track_extends->default_sample_duration = batom_cursor_uint32_be(&cursor);
    track_extends->default_sample_size = batom_cursor_uint32_be(&cursor);
    track_extends->default_sample_flags = batom_cursor_uint32_be(&cursor);
    return !BATOM_IS_EOF(&cursor);
}

bool 
bmp4_parse_movie_fragment_header(batom_cursor *cursor, bmp4_movie_fragment_header *header)
{
    /* page 39 */
	if(!bmp4_parse_fullbox(cursor, &header->fullbox)) {
		return false;
	}
    header->sequence_number = batom_cursor_uint32_be(cursor);
    return !BATOM_IS_EOF(cursor);
}

bool 
bmp4_parse_track_fragment_header(batom_cursor *cursor, bmp4_track_fragment_header *header)
{
    BKNI_Memset(header, 0, sizeof(*header));
    /* page 40 */
	if(!bmp4_parse_fullbox(cursor, &header->fullbox)) {
		return false;
	}
    header->track_ID = batom_cursor_uint32_be(cursor);
    if(header->fullbox.flags&0x0001) {
        header->validate.base_data_offset = true;
        header->base_data_offset = batom_cursor_uint64_be(cursor);
    }
    if(header->fullbox.flags&0x0002) {
        header->validate.sample_description_index = true;
        header->sample_description_index = batom_cursor_uint32_be(cursor);
    }
    if(header->fullbox.flags&0x0008) { 
        header->validate.default_sample_duration = true;
        header->default_sample_duration = batom_cursor_uint32_be(cursor);
    }
    if(header->fullbox.flags&0x0010) { 
        header->validate.default_sample_size = true;
        header->default_sample_size = batom_cursor_uint32_be(cursor);
    }
    if(header->fullbox.flags&0x0020) { 
        header->validate.default_sample_flags = true;
        header->default_sample_flags = batom_cursor_uint32_be(cursor);
    }
    return !BATOM_IS_EOF(cursor);
}

bool 
bmp4_parse_track_fragment_run_header(batom_cursor *cursor, bmp4_track_fragment_run_header  *run_header)
{
    BKNI_Memset(run_header, 0, sizeof(*run_header));
    /* pages 41,42 */
	if(!bmp4_parse_fullbox(cursor, &run_header->fullbox)) {
		return false;
	}
    run_header->sample_count = batom_cursor_uint32_be(cursor);
    if(run_header->fullbox.flags&0x001) {
        run_header->validate.data_offset = true;
        run_header->data_offset = batom_cursor_uint32_be(cursor);
    }
    if(run_header->fullbox.flags&0x004) {
        run_header->validate.first_sample_flags= true;
        run_header->first_sample_flags = batom_cursor_uint32_be(cursor);
    }
    BDBG_MSG(("bmp4_parse_track_fragment_run_header: %#lx %s fullbox.flags:%#x sample_count:%u", (unsigned long)cursor, BATOM_IS_EOF(cursor)?"EOF":"", run_header->fullbox.flags, run_header->sample_count));
    return !BATOM_IS_EOF(cursor);
}
#if defined(HUMAX_PLATFORM_BASE)
bmp4_uuid_extendedtype
bmp4_find_ext_extendedtype(batom_cursor *cursor)
{
	unsigned i = 0;
	unsigned j = 0;
	unsigned compare = 0;
	unsigned TYPELEN = 16;
	bmp4_uuid_extendedtype etype = BMP4_UUID_EXTENDEDTYPE_MAX;
	bmp4_box_extended extended_type;
	if(!bmp4_parse_box_extended(cursor, &extended_type)) {
		return BMP4_UUID_EXTENDEDTYPE_MAX;
	}
	/* TODO : Compare Extended Type */
	for(i = 0 ; i < BMP4_UUID_EXTENDEDTYPE_MAX ; i++)
	{
		compare = 0;
		for(j = 0; j < TYPELEN ; j++)
		{
			if(extended_type.usertype[j] == UUID_EXTENDTYPE[i][j])
			{
				compare++;
				if(compare == TYPELEN)
				{
					etype = i;
					break;
				}
			}
			else
				break;	
		}
		if(etype != BMP4_UUID_EXTENDEDTYPE_MAX)
		{
			break;
		}
	}
	return etype;
}

bool 
bmp4_parse_ext_sebox(batom_cursor *cursor, bmp4_ext_sample_encryption_box *sample_enc_box)
{
	unsigned i,j = 0;
	const uint8_t SAMPLE_ENCRYPTION_BOX_ID[16] = {0xA2,0x39,0x4F,0x52,0x5A,0x9B,0x4F,0x14,0xA2,0x44,0x6C,0x42,0x7C,0x64,0x8D,0xF4};
    BKNI_Memset(sample_enc_box, 0, sizeof(*sample_enc_box));   

    /* pages 41,42 */
	if(!bmp4_parse_fullbox(cursor, &sample_enc_box->fullbox)) {
		return false;
	}
	if(sample_enc_box->fullbox.flags & 0x0001)
	{
		sample_enc_box->algorithmID = batom_cursor_uint24_be(cursor);
		sample_enc_box->IV_Size = batom_cursor_byte(cursor);
		for(i = 0 ; i < 16; i++)
		{
			sample_enc_box->KID[i] = batom_cursor_byte(cursor);
		}
	}
	else
	{
		sample_enc_box->IV_Size = 8;
	}
	sample_enc_box->sample_count = batom_cursor_uint32_be(cursor);
	if(sample_enc_box->sample_count > MAX_NUM_OF_SAMPLES)
	{
		BDBG_ERR(("bmp4_parse_ext_sebox: MAX_NUM_OF_SAMPLES < sample_count"));
		BDBG_ASSERT(0);
		sample_enc_box->sample_count = MAX_NUM_OF_SAMPLES;
	}
	for(i=0; i<sample_enc_box->sample_count; i++)
	{
		batom_cursor_copy(cursor, &sample_enc_box->samples[i].InitializationVector_128[8], 8);
#if 0
		for(j = 0 ; j < 8; j++)
		{
			sample_enc_box->samples[i].InitializationVector_128[j] = batom_cursor_byte(cursor);
		}
		for(j = 8 ; j < 16; j++)
		{
			if(sample_enc_box->IV_Size == 16)
			{
				sample_enc_box->samples[i].InitializationVector_128[j] = batom_cursor_byte(cursor);
			}
			else
			{
				sample_enc_box->samples[i].InitializationVector_128[j] = 0;
			}
		}
#endif		
		if(sample_enc_box->fullbox.flags & 0x0002)
		{
			sample_enc_box->samples[i].NumberOfEntries = batom_cursor_uint16_be(cursor);
			
			for(j=0; j< sample_enc_box->samples[i].NumberOfEntries; j++)
			{
				sample_enc_box->samples[i].entries[j].BytesOfClearData = batom_cursor_uint16_be(cursor);
				sample_enc_box->samples[i].entries[j].BytesOfEncryptedData = batom_cursor_uint32_be(cursor);
			}
		}
		else
		{
			sample_enc_box->samples[i].NumberOfEntries = 0;
		}
	}
    return !BATOM_IS_EOF(cursor);
}

bool 
bmp4_parse_ext_sample_encryption_box(batom_cursor *cursor, bmp4_ext_sample_encryption_box *sample_enc_box)
{
	unsigned i = 0;
	const uint8_t SAMPLE_ENCRYPTION_BOX_ID[16] = {0xA2,0x39,0x4F,0x52,0x5A,0x9B,0x4F,0x14,0xA2,0x44,0x6C,0x42,0x7C,0x64,0x8D,0xF4};
    BKNI_Memset(sample_enc_box, 0, sizeof(*sample_enc_box));   
    #if 1 /* Old Logic */
	if(!bmp4_parse_box_extended(cursor, &sample_enc_box->extented_type)) {
		return false;
	}
	/* TODO : Compare Extended Type */
	for(i = 0 ; i < 16 ; i++)
	{
		if(sample_enc_box->extented_type.usertype[i] != SAMPLE_ENCRYPTION_BOX_ID[i])
		{
			return false;
		}
	}
	#endif
    /* pages 41,42 */
	if(!bmp4_parse_fullbox(cursor, &sample_enc_box->fullbox)) {
		return false;
	}
	if(sample_enc_box->fullbox.flags & 0x0001)
	{
		sample_enc_box->algorithmID = batom_cursor_uint24_be(cursor);
		sample_enc_box->IV_Size = batom_cursor_byte(cursor);
		for(i = 0 ; i < 16; i++)
		{
			sample_enc_box->KID[i] = batom_cursor_byte(cursor);
		}
	}
	else
	{
		sample_enc_box->IV_Size = 8;
	}
	sample_enc_box->sample_count = batom_cursor_uint32_be(cursor);
    return !BATOM_IS_EOF(cursor);
}


bool 
bmp4_parse_ext_sample_encryption(batom_cursor *cursor, const bmp4_ext_sample_encryption_box *sample_enc_box, bmp4_ext_sample_encryption *sample_encryption)
{
	unsigned i;
    BKNI_Memset(sample_encryption, 0, sizeof(*sample_encryption));   
	if(sample_enc_box->IV_Size == 16)
	{
		for(i = 0 ; i < 16; i++)
		{
			sample_encryption->InitializationVector_128[i] = batom_cursor_byte(cursor);
		}
	}
	else /* if(sample_enc_box->IV_Size == 8) */
	{
#if 0	
		for(i = 0 ; i < 8; i++)
		{
			sample_encryption->InitializationVector_64[i] = batom_cursor_byte(cursor);
		}
#else		

		for(i = 0 ; i < 8; i++)
		{
			sample_encryption->InitializationVector_128[i] = batom_cursor_byte(cursor);
		}
		for(i = 8 ; i < 16; i++)
		{
			sample_encryption->InitializationVector_128[i] = 0;
		}
#endif
	}
	if(sample_enc_box->fullbox.flags & 0x0002)
	{
		sample_encryption->NumberOfEntries = batom_cursor_uint16_be(cursor);
#if 0		
		for(i = 0 ; i < sample_encryption->NumberOfEntries; i++)
		{	
			sample_encryption->subsample.BytesOfClearData += batom_cursor_uint16_be(cursor);
			sample_encryption->subsample.BytesOfEncryptedData += batom_cursor_uint32_be(cursor);
		}
#endif		
	}
    return !BATOM_IS_EOF(cursor);
}

bool
bmp4_parse_ext_subsample_encryption(batom_cursor *cursor, bmp4_ext_subsample *subsample)
{
	subsample->BytesOfClearData = batom_cursor_uint16_be(cursor);
	subsample->BytesOfEncryptedData = batom_cursor_uint32_be(cursor);
    return !BATOM_IS_EOF(cursor);
}
#endif

void 
bmp4_init_track_fragment_run_state(bmp4_track_fragment_run_state *state)
{
    state->sample_no = 0;
    state->accumulated_size = 0;
    state->accumulated_time = 0;
    return;
}

bool bmp4_parse_track_fragment_run_sample(batom_cursor *cursor, const bmp4_track_fragment_header *fragment_header, const bmp4_track_fragment_run_header *run_header, const bmp4_trackextendsbox *track_extends, bmp4_track_fragment_run_state *state, bmp4_track_fragment_run_sample *sample)
{
    if(state->sample_no>=run_header->sample_count) {
        return false;
    }
    sample->time = state->accumulated_time;
    sample->offset = state->accumulated_size;
    if(run_header->fullbox.flags&0x0100) {
        sample->duration = batom_cursor_uint32_be(cursor);
    } else if (fragment_header->validate.default_sample_duration) {
        sample->duration = fragment_header->default_sample_duration;
    } else {
        sample->duration = track_extends->default_sample_duration;
    }

    if(run_header->fullbox.flags&0x0200) {
        sample->size = batom_cursor_uint32_be(cursor);
    } else if(fragment_header->validate.default_sample_size) {
        sample->size = fragment_header->default_sample_size;
    } else {
        sample->size = track_extends->default_sample_size;
    }

    if(run_header->fullbox.flags&0x0400) {
        sample->flags = batom_cursor_uint32_be(cursor);
    } else if(state->sample_no==0 && run_header->validate.first_sample_flags) {
        sample->flags = run_header->first_sample_flags;
    } else if(fragment_header->validate.default_sample_flags) {
        sample->flags = fragment_header->default_sample_flags;
    } else {
        sample->flags= track_extends->default_sample_flags;
    }

    if(run_header->fullbox.flags&0x0800) {
        sample->composition_time_offset = batom_cursor_uint32_be(cursor);
    } else {
        sample->composition_time_offset = 0;
    }
    if(!BATOM_IS_EOF(cursor)) {
        state->sample_no++;
        state->accumulated_size += sample->size;
        state->accumulated_time += sample->duration;
        return true;
    }
    return false;
}

