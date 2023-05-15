/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_probe.c $
 * $brcm_Revision: 52 $
 * $brcm_Date: 9/28/12 12:10p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_probe.c $
 * 
 * 52   9/28/12 12:10p vsilyaev
 * SW7435-384: Addes support for EAC3 audio in ASF container
 * 
 * 51   7/24/12 12:05p vsilyaev
 * SW7425-3574: Fixed potentianl memory leak on some corrupted streams
 * 
 * 50   5/30/12 2:12p vsilyaev
 * SW7425-3144: Improved handling of concatenated streams
 * 
 * 49   9/22/11 1:15p vsilyaev
 * SW7425-1306: Don't advertize as self-indexable filea that are non-
 * seekable or without duration
 * 
 * 48   4/12/11 10:50a vsilyaev
 * SWDTV-6571: Fixed typo
 * 
 * 47   4/11/11 3:12p vsilyaev
 * SWDTV-6571: Added parsing of deviceConformanceTemplate
 * 
 * 46   11/4/10 10:37a vsilyaev
 * SW35230-2093: Added parsing of language information
 * 
 * 45   9/28/10 11:04a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 44   9/24/10 4:00p vsilyaev
 * SWGERRARD-557: Better indicate index type if navigation supported
 * without index
 * 
 * 43   8/25/10 3:33p vsilyaev
 * SW3548-3068: Added stereoscopic fields
 * 
 * 42   7/27/10 8:09p vsilyaev
 * SWBLURAY-21061: Fixed warning
 * 
 * 41   7/22/10 4:59p vsilyaev
 * SWBLURAY-21061: Better detect DRM files
 * 
 * dev_bdvd_v3.0_issue21061/2   7/21/10 3:12p ialauder
 * remove unused variable
 * 
 * 40   6/28/10 3:03p vsilyaev
 * SWBLURAY-21061: Refactored code to allow separation bettwen no-DRM and
 * parsing error
 * 
 * 39   6/28/10 12:19p vsilyaev
 * SWBLURAY-21061: Added code to look/parse the Content Encryption Object.
 * 
 * fix_bdvd_v3.0_issue21061/1   6/22/10 8:31a ialauder
 * Add support to parse Content Encryption object for DRM
 * 
 * 38   4/27/10 12:35p vsilyaev
 * SWBLURAY-19928: Clear index structure
 * 
 * fix_bdvd_v3.0_issue19928/1   4/27/10 11:59a ialauder
 * We need to initialize the index structure
 * 
 * 37   4/23/10 6:54p vsilyaev
 * SW3548-2908: Skip over various objects when looking for the simple
 * index
 * 
 * 36   4/19/10 12:16p vsilyaev
 * SW3548-2890: Populate video bitrate
 * 
 * 35   4/14/10 11:37a jtna
 * SW7125-280: Coverity Defect ID:20324 BAD_FREE (comment must precede
 * line in question)
 * 
 * 34   4/1/10 12:19p vsilyaev
 * SW3548-2867: Skip over the index object when looking for the simple-
 * index object
 * 
 * 33   2/22/10 2:36p vsilyaev
 * SWBLURAY-18922: Added audio->video and video->audio difference.
 * 
 * fix_bdvd_v3.0_issue18922/3   2/22/10 12:12p gooslin
 * Return both the maximum distance an audio frame is ahead of a video
 * frame at a given PTS and the maximum distance a video frame is ahead
 * of an audio frame at a given PTS
 * 
 * fix_bdvd_v3.0_issue18922/2   2/22/10 10:45a gooslin
 * Merge latest back to branch
 * 
 * 32   2/19/10 6:38p vsilyaev
 * SWBLURAY-18922: Fixed warnings and cleaned code
 * 
 * 31   2/19/10 6:17p vsilyaev
 * SWBLURAY-18922: Added parsing of the index object to return distance
 * between video and audio streams.
 * 
 * fix_bdvd_v3.0_issue18922/1   2/19/10 3:54p gooslin
 * Detect the maximum byte difference between a audio and video frame at a
 * give PTS
 * 
 * 29   1/13/10 7:05p vsilyaev
 * SW3556-986: Returned averageTimePerFrame from the ASF Extended Stream
 * Properties Object
 * 
 * 28   12/29/09 4:25p vsilyaev
 * SW3556-959: Added ranges for the header, data and simple_index objects
 * 
 * 27   12/18/09 12:58p vsilyaev
 * SW3556-946: Added fields from the ASF FileObject
 * 
 * 26   10/28/09 12:44p vsilyaev
 * SW7405-3278: Apply preroll into the duration
 * 
 * 25   8/19/09 11:58a vsilyaev
 * PR 57639: Added dynamic range control information
 * 
 * 24   6/12/09 12:29p vsilyaev
 * PR 55896 PR 55873: Also added AAC audio support
 * 
 * 23   6/10/09 12:43p vsilyaev
 * PR 55896: Merged changes for extended ASF codec support
 * 
 * 22   6/9/09 7:32p vsilyaev
 * PR 54342: Added code to probe index in the media files
 * 
 * 21   6/5/09 4:27p vsilyaev
 * PR 55757: Added code to guesstimate bitrate based on the size of the
 * data object
 * 
 * 20   6/3/09 7:55p vsilyaev
 * PR 55417: Updated code for new semantics of the bfile_buffer_read
 * 
 * 19   6/3/09 7:35p vsilyaev
 * PR 55617: Added code to extract Pixel Aspect Ration
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/2   6/4/09 3:49p gooslin
 * Add H.264 support to ASF
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/1   6/2/09 5:51p gooslin
 * Extend ASF codec support
 * 
 * 18   5/15/09 11:37a vsilyaev
 * PR 55142: Removed test for WMV1 and WMV2 codecs
 * 
 * 17   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 16   5/15/09 10:56a vsilyaev
 * PR 55142: Added MP43 video support for ASF files
 * 
 * dev_bdvd_v1.5_add_mp4v3_support_to_asf/1   5/13/09 5:54p gooslin
 * Add MP4v3 support
 * 
 * 15   5/6/09 1:51p vsilyaev
 * PR 54741: Calculating audio bitrate in 1000 bits per second
 * 
 * 14   5/4/09 6:08p vsilyaev
 * PR 52434: Use common routines to parse Wave and Bitmap headers
 * 
 * 13   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 12   4/7/09 10:18a vsilyaev
 * PR 50120: Added address_free tag
 * 
 * 11   12/10/08 4:02p vsilyaev
 * PR 50120: Covered warning
 * 
 * 10   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 9   10/2/08 10:47a vsilyaev
 * PR 32813: Use proper api to add detected track
 * 
 * 8   9/3/08 6:15p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 7   10/18/07 1:43p jtna
 * PR35951: Coverity Defect ID:4003 RESOURCE_LEAK
 * 
 * 6   7/23/07 5:51p vsilyaev
 * PR 32813: Fixed comments and unnecessary code
 * 
 * 5   7/19/07 12:17p vsilyaev
 * PR 32813: Renamed bmedia_track to bmedia_track_type
 * 
 * 4   7/17/07 4:30p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 3   7/11/07 5:27p vsilyaev
 * PR 32846: Fixed some typos
 * 
 * 2   7/10/07 5:23p vsilyaev
 * PR 32813: Added stream duration field
 * 
 * 1   7/9/07 4:00p vsilyaev
 * PR 32813: ASF probe module
 * 
 *******************************************************************************/
#include "bstd.h"
#include "basf_probe.h"
#include "basf_parser.h"
#include "blst_slist.h"
#include "bkni.h"

BDBG_MODULE(basf_probe);


/* 4.7  Metadata Object (optional, 0 or 1) */
typedef struct basf_description_record {
    basf_word streamNumber;
    basf_word nameLength;
    basf_word dataType;
    basf_dword dataLength;
} basf_description_record;

static const basf_guid
  basf_extended_content_descryption_guid = 
    {{0xD2, 0xD0, 0xA4, 0x40, 0xE3, 0x07, 0x11, 0xD2, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50}},
  basf_language_list_guid = 
    {{0x7C, 0x43, 0x46, 0xA9, 0xEF, 0xE0, 0x4B, 0xFC, 0xB2, 0x29, 0x39, 0x3E, 0xDE, 0x41, 0x5C, 0x85}},
  basf_metadata_guid = 
    {{0xC5, 0xF8, 0xCB, 0xEA, 0x5B, 0xAF, 0x48, 0x77, 0x84, 0x67, 0xAA, 0x8C, 0x44, 0xFA, 0x4C, 0xCA}};

static bool
basf_read_description_record(batom_cursor *cursor, basf_description_record *record)
{
    batom_cursor_skip(cursor, sizeof(basf_word));
    record->streamNumber = basf_read_word(cursor);
    record->nameLength = basf_read_word(cursor);
    record->dataType = basf_read_word(cursor);
    record->dataLength = basf_read_dword(cursor);
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }
    BDBG_MSG(("basf_read_description_record: streamNumber:%u nameLength:%u dataType:%u dataLength:%u", (unsigned)record->streamNumber, (unsigned)record->nameLength, (unsigned)record->dataType, (unsigned)record->dataLength));
    return true;
}

static const char * const b_asf_meta_tags[] = {
#define B_ASF_META_ASPECT_RATIO_X 0
    "AspectRatioX",
#define B_ASF_META_ASPECT_RATIO_Y 1
    "AspectRatioY",
#define B_ASF_META_WMA_PEAK_REFERENCE   2
    "WM/WMADRCPeakReference",
#define B_ASF_META_WMA_PEAK_TARGET  3
    "WM/WMADRCPeakTarget",
#define B_ASF_META_WMA_AVERAGE_REFERENCE 4
    "WM/WMADRCAverageReference",
#define B_ASF_META_WMA_AVERAGE_TARGET 5
    "WM/WMADRCAverageTarget",
#define B_ASF_META_DEVICE_CONFORMANCE_TEMPLATE  6
    "DeviceConformanceTemplate"
};
#define B_ASF_N_META_TAGS (sizeof(b_asf_meta_tags)/sizeof(*b_asf_meta_tags))

#define B_ASF_METADATA_ENTRY_STRING 0x0000
#define B_ASF_METADATA_ENTRY_BINARY 0x0001
#define B_ASF_METADATA_ENTRY_BOOL   0x0002
#define B_ASF_METADATA_ENTRY_DWORD  0x0003
#define B_ASF_METADATA_ENTRY_QWORD  0x0004
#define B_ASF_METADATA_ENTRY_WORD   0x0005
#define B_ASF_METADATA_ENTRY_GUID   0x0006

static int 
b_asf_meta_match_record(batom_cursor *cursor, basf_word nameLength, unsigned ntags, const char *const tags[], bool *temp)
{
    bool *valid = temp;
    unsigned tag;
    int i;
    int match=-1;
    unsigned pos;

    for(tag=0;tag<ntags;tag++) {
        valid[tag] = true;
    }
    for(pos=0,i=nameLength;i>0;i-=2,pos++) {
        int ch = batom_cursor_next(cursor);
        int next = batom_cursor_next(cursor);
        match = -1;
        if(next==BATOM_EOF) { /* reached EOF */
            return -1;
        }
        if(next!=0 ) { /* we only support ASCII tags */
            continue;
        }
        for(tag=0;tag<ntags;tag++) {
            if(!valid[tag]) { continue;}
            if(ch==tags[tag][pos] ) { 
                match = tag;
                if(ch!='\0') {
                    continue;
                }
                /* EOF should should clear valid bit */
            }
            valid[tag] = false; /* clear valid table */
        }
    }
    return match;
}

static bool
b_asf_metadata_read_data_uint32(batom_cursor *cursor, const basf_description_record *record, uint32_t *data)
{
    switch(record->dataType) {
    case B_ASF_METADATA_ENTRY_BOOL: /* BOOL */
    case B_ASF_METADATA_ENTRY_WORD: /* WORD */
        if(record->dataLength==2) {
            *data = basf_read_word(cursor);
            return !BATOM_IS_EOF(cursor);
        }
        break;
    case B_ASF_METADATA_ENTRY_DWORD: /* DWORD */
        if(record->dataLength==4) {
            *data = basf_read_dword(cursor);
            return !BATOM_IS_EOF(cursor);
        }
        break;
    case B_ASF_METADATA_ENTRY_QWORD: /* QWORD */
        if(record->dataLength==8) {
            *data = basf_read_qword(cursor);
            return !BATOM_IS_EOF(cursor);
        }
        break;
    default:
        break;
    }
    batom_cursor_skip(cursor, record->dataLength);
    return false;
}


typedef struct b_asf_metadata_element {
    BLST_S_ENTRY(b_asf_metadata_element) link;
    basf_word streamNumber;
    uint16_t tag;
    unsigned value;
} b_asf_metadata_element;

typedef BLST_S_HEAD(b_asf_metadata_list, b_asf_metadata_element) b_asf_metadata_list;

static const b_asf_metadata_element *
b_asf_metadata_find(b_asf_metadata_list *list, unsigned streamNumber, unsigned tag)
{
    b_asf_metadata_element *e;

    for(e=BLST_S_FIRST(list);e;e=BLST_S_NEXT(e,link)) {
        if(e->tag==tag && e->streamNumber==streamNumber) {
            return e;
        }
    }
    return NULL;
}

static bool 
b_asf_probe_header_match(batom_cursor *header)
{
	basf_guid guid;
	basf_qword size;
	
	basf_read_guid(header, &guid);
	size = basf_read_qword(header);
	if(BATOM_IS_EOF(header)) {
		return false;
	}
	return basf_compare_guid(&guid, &basf_header_guid)==0 &&  (size >=  sizeof(guid)+sizeof(size));
}

BDBG_OBJECT_ID(basf_probe_t);

typedef struct basf_probe  *basf_probe_t; 

typedef struct b_asf_probe_object_handler {
	basf_parser_object_handler handler; /* must be first */
	basf_probe_t probe; /* pointer to demux */ 
} b_asf_probe_object_handler;

typedef struct b_asf_extended_stream_properties_element {
    BLST_S_ENTRY(b_asf_extended_stream_properties_element) link;
    basf_extended_stream_properties extended_stream_properties;
} b_asf_extended_stream_properties_element;

typedef BLST_S_HEAD(b_asf_extended_stream_properties_list, b_asf_extended_stream_properties_element) b_asf_extended_stream_properties_list;

#define B_ASF_PROBE_MAX_LANGUAGE_ENTRIES    16

typedef struct b_asf_probe_language_entry {
    uint16_t length;
    uint16_t data[1]; /* variable length */
}b_asf_probe_language_entry;

struct basf_probe {
	BDBG_OBJECT(basf_probe_t)
	basf_parser_t parser;
	basf_probe_stream *stream;
    b_asf_metadata_list metadata_parsed;
    b_asf_metadata_list metadata_free;
    b_asf_extended_stream_properties_list extended_stream_properties_list;
	b_asf_probe_object_handler extended_stream_properties;
	b_asf_probe_object_handler stream_properties;
	b_asf_probe_object_handler file_properties;
	b_asf_probe_object_handler metadata;
    b_asf_probe_object_handler content_encryption;
    b_asf_probe_object_handler extended_content_encryption;
    b_asf_probe_object_handler extended_content_description;
    b_asf_probe_object_handler language_list;
    b_asf_metadata_element metadata_array[16];
    struct {
        unsigned offset;
        unsigned count;
        uint16_t data[BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER]; /* sequence of UCS-2 NULL termiated strings */
    } deviceConformanceTemplate;
    basf_index index;
    basf_qword next_seek;
    basf_probe_stereoscopic_info stereoscopic_info;
    bool content_encrypted;
    basf_encryption_type encryption_type;
    size_t language_entries_count;
    b_asf_probe_language_entry *language_entries[B_ASF_PROBE_MAX_LANGUAGE_ENTRIES];
};

static const bmedia_probe_file_ext b_asf_ext[] =  {
	{"wmv"},{"asf"},{"wma"},
	{""}
};

/* information extracted from header of the ASF Index Object */
typedef struct basf_index_specifiers {
	basf_word 	stream_number;
    enum {
        index_type_invalid = 0,
        index_type_nearest_packet = 1,     /* The Nearest Past Data Packet indexes point to the data packet whose presentation time is closest to the index entry time. */ 
        index_type_nearest_media = 2,      /* The Nearest Past Object indexes point to the closest data packet containing an entire object or first fragment of an object. */
        index_type_nearest_cleanpoint = 3  /* The Nearest Past Cleanpoint indexes point to the closest data packet containing an entire object (or first fragment of an object) that has the Cleanpoint Flag set. */
    } index_type;

    basf_qword block_position;
    off_t prev_stream_offset;
    off_t cur_stream_offset;
    bool offset_valid;
} basf_index_specifiers;

/* Invalid index offset as defined by ASF_SPECIFICATION page 59 */
#define ASF_INVALID_INDEX_OFFSET 0xFFFFFFFF

static void
b_asf_probe_parse_index(basf_probe_t probe, bfile_buffer_t buf)
{
    unsigned i, j, k;
    batom_t atom;
    bfile_buffer_result result;
    batom_cursor cursor;
    basf_index_specifiers *stream_specifiers;
    basf_index_specifiers *video_specifier = NULL;
    bmedia_probe_track *video_track = NULL;
    off_t gap;


    BDBG_ASSERT(probe->stream);
    /* Find the video track */
    for(video_track=BLST_SQ_FIRST(&probe->stream->media.tracks); video_track; video_track=BLST_SQ_NEXT(video_track, link)) {
        if (video_track->type == bmedia_track_type_video) {
            break;
        }
    }

    if (!probe->index.object_offset || !probe->index.object_size || probe->index.index_specifiers_count<=1 || !video_track) {
        return;
    }

    atom = bfile_buffer_read(buf, probe->index.object_offset, probe->index.object_size, &result);
    if (!atom) {
        BDBG_ERR(("b_asf_probe_parse_index: %#lx can't read entire contents of index (offset:%u size:%u) in a single chunk. Multiple reads not yet implimented.", (unsigned long)probe, probe->index.object_offset, probe->index.object_size));
        goto err_atom;
    }

    /* Get a cursor to the data and skip the section which has already been parsed into the basf_index struct */
    batom_cursor_from_atom(&cursor, atom);
    batom_cursor_skip(&cursor, probe->index.offset-probe->index.object_offset );

    stream_specifiers = BKNI_Malloc(sizeof(*stream_specifiers)*probe->index.index_specifiers_count);
    if (!stream_specifiers) {
        BDBG_ERR(("b_asf_probe_parse_index: %#lx Failed to allocate stream specifiers struct", (unsigned long)probe));
        goto err_specifiers;
    }

    /* In addition to reading the index type and stream id for each specifer, initialize values used for finding the max byte difference */
    for(i=0; i!=probe->index.index_specifiers_count; i++){
	    stream_specifiers[i].stream_number = basf_read_word(&cursor);
	    stream_specifiers[i].index_type = basf_read_word(&cursor);
        if(BATOM_IS_EOF(&cursor)) {
            goto err_end_of_data;
        }

        switch (stream_specifiers[i].index_type) {
        case index_type_nearest_packet:
        case index_type_nearest_media:
        case index_type_nearest_cleanpoint:
            break;
        default:
            stream_specifiers[i].index_type = index_type_invalid;
        }
        stream_specifiers[i].prev_stream_offset = -1;
        stream_specifiers[i].cur_stream_offset = -1;
        stream_specifiers[i].offset_valid = false;
        stream_specifiers[i].block_position = 0;
        if (stream_specifiers[i].stream_number == video_track->number && stream_specifiers[i].index_type != index_type_invalid) {
            /* If we have not yet found the video specifier yet, use this one. If there are multiple specifiers for the same video track, try to avoid 
               the type index_type_nearest_cleanpoint if possibile, it provides the least accurate resolution */
            if (!video_specifier || (video_specifier->index_type == index_type_nearest_cleanpoint)) {
                video_specifier = &stream_specifiers[i];
            }
        }
    }

    if (!video_specifier) {
        BDBG_ERR(("b_asf_probe_parse_index: %#lx Failed find a specifier for the video stream", (unsigned long)probe));
        goto err_video_specifier;
    }

    /* The index can be broken up into multiple blocks (to support large files), most files will have a single block */
    for(i=0; i!=probe->index.index_blocks_count; i++){
        /* Get the number of entries in this block as well as the base offset for each stream in this block */
        basf_dword entry_count = basf_read_dword(&cursor);
        if(BATOM_IS_EOF(&cursor)) {
            goto err_end_of_data;
        }

        for(j=0; j!=probe->index.index_specifiers_count; j++){
            stream_specifiers[j].block_position = basf_read_qword(&cursor);
        }
        if(BATOM_IS_EOF(&cursor)) {
            goto err_end_of_data;
        }
        for(j=0; j!=entry_count; j++){
            /* For each entry in this block, there will be one offset for each specifier. Read the offset for each specifier and mark if it is valid */
            for(k=0; k<probe->index.index_specifiers_count; k++){
                stream_specifiers[k].prev_stream_offset = stream_specifiers[k].cur_stream_offset; /* save previous offset */
                stream_specifiers[k].cur_stream_offset = basf_read_dword(&cursor); /* read the offset for this specifier */
                if(BATOM_IS_EOF(&cursor)) {
                    goto err_end_of_data;
                }
                /* The index can contin invalid entries marked by offset ASF_INVALID_INDEX_OFFSET or it might repeat the same frame multiple times. 
                   If this offset is invalid or it's a duplicate, don't use it */
                stream_specifiers[k].offset_valid = (stream_specifiers[k].cur_stream_offset != ASF_INVALID_INDEX_OFFSET) && (stream_specifiers[k].cur_stream_offset != stream_specifiers[k].prev_stream_offset);
                stream_specifiers[k].cur_stream_offset += stream_specifiers[k].block_position; /*  The actual frame offset is calculated by adding the current block's base address */
            }

            /* Now go through each specifier's offset and calculate the byte difference from the video specifier's offset */
            for(k=0; k<probe->index.index_specifiers_count; k++){
                if ((k+1) == video_specifier->stream_number || !video_specifier->offset_valid || !stream_specifiers[k].offset_valid) {
                    continue;
                }

                gap = video_specifier->cur_stream_offset - stream_specifiers[k].cur_stream_offset;
                /* If the byte gap is the largest we've found, mark it as such */
                if (gap > probe->stream->max_video_to_audio_delta) {
                    probe->stream->max_video_to_audio_delta = gap;
                    BDBG_MSG(("b_asf_probe_parse_index: Video at time ~%ums was found to %lld bytes ahead of audio at the same time", j*probe->index.index_entry_time_interval, gap));
                }
                if (-gap > probe->stream->max_audio_to_video_delta) {
                    probe->stream->max_audio_to_video_delta = -gap;
                    BDBG_MSG(("b_asf_probe_parse_index: Audio at time ~%ums was found to %lld bytes ahead of video at the same time", j*probe->index.index_entry_time_interval, -gap));
                }
            }
        }
    }

    /* Check if video is always ahead of audio (or visa versa) and indicate the delta is known */
    if (probe->stream->max_video_to_audio_delta > 0 && probe->stream->max_audio_to_video_delta == -1) {
        probe->stream->max_audio_to_video_delta = 0;
    }
    if (probe->stream->max_audio_to_video_delta > 0 && probe->stream->max_video_to_audio_delta == -1) {
        probe->stream->max_video_to_audio_delta = 0;
    }

err_end_of_data:
err_video_specifier:
    BKNI_Free(stream_specifiers);
err_specifiers:
    batom_release(atom);
err_atom:
    return;
}

static bool
b_asf_probe_video_stream(basf_probe_t probe, basf_probe_track *track, batom_cursor *cursor)
{
    basf_video_stream_properties asf_video;
    bool pr;

    if(!basf_read_video_stream_properties(cursor, &asf_video, &pr)) {
        return false;	
    }
    if(pr) {
        probe->encryption_type = basf_encryption_version_pr;
        track->encryptedContentFlag = true;
    }

	track->media.info.video.width = asf_video.bi.biWidth;
	track->media.info.video.height = asf_video.bi.biHeight;
    track->media.info.video.timestamp_order = bmedia_timestamp_order_decode;
	if(BMEDIA_FOURCC_VC1AP_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_vc1;
    } else if(BMEDIA_FOURCC_VC1SM_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_vc1_sm;
    } else if(BMEDIA_FOURCC_DIVX3_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_divx_311;
	} else if(BMEDIA_FOURCC_DIVX5_CODEC(asf_video.bi.biCompression) || BMEDIA_FOURCC_XVID_CODEC(asf_video.bi.biCompression) || BMEDIA_FOURCC_3IVX_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_mpeg4_part2;
	} else if(BMEDIA_FOURCC_H264_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_h264;
	} else if(BMEDIA_FOURCC_MPEG2_CODEC(asf_video.bi.biCompression)) {
		track->media.info.video.codec = bvideo_codec_mpeg2;
	} else {
		track->media.info.video.codec = bvideo_codec_unknown;
	}
	return true;
}

static bool
b_asf_probe_audio_stream(basf_probe_t probe, basf_probe_track *track, batom_cursor *cursor)
{
    basf_audio_stream_properties asf_audio;
    bool pr;

    if(!basf_read_audio_stream_properties(cursor, &asf_audio, &pr)) {
        return false;	
    }
    if(pr) {
        probe->encryption_type = basf_encryption_version_pr;
        track->encryptedContentFlag = true;
    }
	track->media.info.audio.channel_count  = asf_audio.nChannels;
	track->media.info.audio.sample_size = asf_audio.wBitsPerSample;
	track->media.info.audio.sample_rate =  asf_audio.nSamplesPerSec;
    track->media.info.audio.bitrate = (asf_audio.nAvgBytesPerSec * 8)/1000;
	if(BMEDIA_WAVFMTEX_AUDIO_WMA(&asf_audio)){
		track->media.info.audio.codec = baudio_format_wma_std;
	} else if (BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_wma_pro;
	} else if (BMEDIA_WAVFMTEX_AUDIO_PCM(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_pcm;
	} else if (BMEDIA_WAVFMTEX_AUDIO_MP3(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_mp3;
	} else if (BMEDIA_WAVFMTEX_AUDIO_MPEG(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_mpeg;
	} else if (BMEDIA_WAVFMTEX_AUDIO_AC3(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_ac3;
	} else if (BMEDIA_WAVFMTEX_AUDIO_DTS(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_dts;
	} else if (BMEDIA_WAVFMTEX_AUDIO_AAC(&asf_audio) || BMEDIA_WAVFMTEX_AUDIO_RAW_AAC(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_aac;
	} else if (basf_waveformatex_is_eac3(&asf_audio)) {
		track->media.info.audio.codec = baudio_format_ac3_plus;
	} else {
        BDBG_MSG(("not supported format:%#x", asf_audio.wFormatTag));
		track->media.info.audio.codec = baudio_format_unknown;
	}
	return true;
}

static basf_parser_handler_action
b_asf_probe_content_encryption(basf_parser_object_handler *handler,  batom_t header)
{
    batom_cursor cursor;
    basf_probe_t probe;
    basf_content_encryption_properties properties;

    probe = ((b_asf_probe_object_handler *)handler)->probe;
    BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    batom_cursor_from_atom(&cursor, header);
    basf_read_content_encryption_properties(&cursor, &properties);
    probe->content_encrypted =  properties.drm;
    if (probe->content_encrypted && probe->encryption_type != basf_encryption_version_2) {
        probe->encryption_type = basf_encryption_version_1;
    }
    return basf_parser_handler_action_none;
}

static basf_parser_handler_action
b_asf_probe_extended_content_encryption(basf_parser_object_handler *handler,  batom_t header)
{
    basf_probe_t probe;

    BSTD_UNUSED(header);

    probe = ((b_asf_probe_object_handler *)handler)->probe;
    BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    probe->encryption_type = basf_encryption_version_2;
    return basf_parser_handler_action_none;
}

static basf_parser_handler_action
b_asf_probe_language_list(basf_parser_object_handler *handler,  batom_t header)
{
    basf_probe_t probe;
    batom_cursor cursor;
    basf_word language_id_records_counts;
    unsigned i;

    BSTD_UNUSED(header);

    probe = ((b_asf_probe_object_handler *)handler)->probe;
    BDBG_OBJECT_ASSERT(probe, basf_probe_t);

    batom_cursor_from_atom(&cursor, header);
    language_id_records_counts = basf_read_word(&cursor);
    BDBG_MSG(("b_asf_probe_language_list: %#lx count:%u", (unsigned long)probe, language_id_records_counts));
    if(BATOM_IS_EOF(&cursor)) {
        goto done;
    }
    if(language_id_records_counts>B_ASF_PROBE_MAX_LANGUAGE_ENTRIES) {
        BDBG_WRN(("limiting number of language ID's from %u to %u",  language_id_records_counts, B_ASF_PROBE_MAX_LANGUAGE_ENTRIES));
        language_id_records_counts=B_ASF_PROBE_MAX_LANGUAGE_ENTRIES;
    }
    for(i=0;i<language_id_records_counts;i++) {
        basf_byte length;
        unsigned j;
        b_asf_probe_language_entry *entry;
        length = basf_read_byte(&cursor);
        if(BATOM_IS_EOF(&cursor)) {
            goto done;
        }
        entry = BKNI_Malloc(sizeof(*entry)+(length*sizeof(basf_word)));
        if(!entry) {
            goto done;
        }
        entry->length = length + 1; /* account for NULL termination */
        entry->data[length] = 0; /* add NULL termination */
        for(j=0;j<length/2;j++) {
            entry->data[j] = basf_read_word(&cursor);
        }
        if(BATOM_IS_EOF(&cursor)) {
            BKNI_Free(entry);
            goto done;
        }
        probe->language_entries_count = i+1;
        probe->language_entries[i] = entry;
    }
done:
    return basf_parser_handler_action_none;
}

static const char * const b_asf_content_tags[] = {
#define B_ASF_CONTENT_STEREOSCOPIC  0
    "Stereoscopic",
#define B_ASF_CONTENT_STEREOSCOPIC_LAYOUT   1
    "StereoscopicLayout",
#define B_ASF_CONTENT_STEREOSCOPIC_SKIP 2
    "StereoscopicSkip"
#if 0
#define B_ASF_CONTENT_STEREOSCOPIC_HORIZONTAL_PARALLAX  3
    ,"StereoscopicHorizontalParallax"
#endif
};
#define B_ASF_N_CONTENT_TAGS (sizeof(b_asf_content_tags)/sizeof(*b_asf_content_tags))


struct b_asf_key_value_pair {
    const char *key;
    unsigned value;
};

#if 0
static const struct b_asf_key_value_pair b_asf_stereoscopic_types[] = {
    {"SideBySideRF", basf_stereoscopic_SideBySideRF},
    {"SideBySideLF", basf_stereoscopic_SideBySideLF},
    {"OverUnderRT", basf_stereoscopic_OverUnderRT},
    {"OverUnderLT", basf_stereoscopic_OverUnderLT},
    {"DualStream", basf_stereoscopic_DualStream},
    {"MultiStream", basf_stereoscopic_MultiStream}
};
#endif
static const char * const b_asf_stereoscopic_types[] = {
    "SideBySideRF", 
    "SideBySideLF", 
    "OverUnderRT", 
    "OverUnderLT",
    "DualStream", 
    "MultiStream"
};
#define B_ASF_N_STEREOSCOPIC_TYPES (sizeof(b_asf_stereoscopic_types)/sizeof(*b_asf_stereoscopic_types))

static basf_parser_handler_action
b_asf_probe_extended_content_description(basf_parser_object_handler *handler,  batom_t header)
{
    basf_probe_t probe;
    basf_word content_descriptors_count;
    unsigned i;
	batom_cursor cursor;

    BSTD_UNUSED(header);

    probe = ((b_asf_probe_object_handler *)handler)->probe;
    BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    BDBG_MSG(("b_asf_probe_extended_content_description: %#lx", (unsigned long)probe));

    batom_cursor_from_atom(&cursor, header);
    content_descriptors_count = basf_read_word(&cursor);
    if(BATOM_IS_EOF(&cursor)) {
        goto done;
    }
    for(i=0;i<content_descriptors_count;i++) {
        bool temp[B_ASF_N_CONTENT_TAGS+B_ASF_N_STEREOSCOPIC_TYPES];
        basf_word length;
        basf_word type;
        int key;
        
        length = basf_read_word(&cursor);
        if(BATOM_IS_EOF(&cursor)) {
            goto done;
        }
        key = b_asf_meta_match_record(&cursor, length, B_ASF_N_CONTENT_TAGS, b_asf_content_tags, temp);
        type = basf_read_word(&cursor);
        length = basf_read_word(&cursor);
        if(BATOM_IS_EOF(&cursor)) {
            goto done;
        }
        switch(key) {
        case B_ASF_CONTENT_STEREOSCOPIC:
            key = batom_cursor_vword_le(&cursor, length);
            if(!BATOM_IS_EOF(&cursor)) {
                probe->stereoscopic_info.stereoscopic = key!=0;
            }
            break;
        case B_ASF_CONTENT_STEREOSCOPIC_SKIP:
            probe->stereoscopic_info.skip = basf_read_qword(&cursor);
            probe->stereoscopic_info.skip_valid = !BATOM_IS_EOF(&cursor);
            break;
        case B_ASF_CONTENT_STEREOSCOPIC_LAYOUT:
            key = b_asf_meta_match_record(&cursor, length, B_ASF_N_STEREOSCOPIC_TYPES, b_asf_stereoscopic_types, temp);
            if(key>=0) {
                probe->stereoscopic_info.layout = key;
            }
            break;
        default:
            batom_cursor_skip(&cursor, length);
            break;
        }
    }
    BDBG_MSG(("b_asf_probe_extended_content_description: %#lx stereoscopic:%u layout:%u skip:%u", (unsigned long)probe, (unsigned)probe->stereoscopic_info.stereoscopic, (unsigned)probe->stereoscopic_info.layout, (unsigned)probe->stereoscopic_info.skip));
done:
    return basf_parser_handler_action_none;
}


static basf_parser_handler_action
b_asf_probe_file_properties(basf_parser_object_handler *handler,  batom_t header)
{
	batom_cursor cursor;
	basf_probe_stream *stream;
	basf_file_properties file;
	basf_probe_t probe;
    basf_qword duration;

	probe = ((b_asf_probe_object_handler *)handler)->probe;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);


	if(probe->stream != NULL) {
	    return basf_parser_handler_action_return;
    }
	batom_cursor_from_atom(&cursor, header);
	if(!basf_read_file_properties(&cursor, &file)) {
		BDBG_WRN(("b_asf_stream_properties_object: %#lx error in the stream header", (unsigned long)probe));
		goto done;
	}

	stream = BKNI_Malloc(sizeof(*stream));
	if(!stream) {
		BDBG_ERR(("b_asf_probe_stream_properties: %#lx can't allocate %u bytes", (unsigned long)probe, sizeof(*stream)));
		goto done;
	}
    BKNI_Memset(stream, 0, sizeof(*stream));
	bmedia_probe_stream_init(&stream->media, bstream_mpeg_type_asf);
	stream->media.max_bitrate =  file.maximumBitrate;
	duration = file.playDuration/(10*1000); /* convert from 100-nanosecond units to 1ms units */
    if(duration>=file.preroll) { /* preroll is in 1ms units */
        duration -= file.preroll;
    }
	stream->media.duration = duration;
    stream->media.index = bmedia_probe_index_unknown;
    stream->seekableFlag = file.seekableFlag;
    stream->broadcastFlag = file.broadcastFlag;
    stream->liveFlag = file.liveFlag;
    stream->fileSize = file.fileSize;
    stream->max_audio_to_video_delta = -1;
    stream->max_video_to_audio_delta = -1;
    stream->encryption_type = basf_encryption_none;
    stream->deviceConformanceTemplateCount = 0;
	probe->stream = stream;

done:
	return basf_parser_handler_action_none;
}


static void
b_asf_parse_stream_properties(basf_probe_t probe, batom_cursor *cursor, bool hidden)
{
	basf_stream_properties asf_stream;
	basf_probe_track *track;
	basf_probe_stream *stream;
	bool track_valid;

	if(!basf_read_stream_properties(cursor, &asf_stream)) {
		BDBG_WRN(("%s: %#lx error in the stream header", "b_asf_parse_stream_properties", (unsigned long)probe));
		goto done;
	}
	stream = probe->stream;
	if(!stream) {
		BDBG_WRN(("%s: %#lx error stream without file properties", "b_asf_probe_stream_properties", (unsigned long)probe));
		goto done;
	}
	track = BKNI_Malloc(sizeof(*track));
	if(!track) {
		BDBG_ERR(("%s: %#lx can't allocate %u bytes", "b_asf_probe_stream_properties", (unsigned long)probe, sizeof(*track)));
		goto done;
	}
    BKNI_Memset(track, 0, sizeof(*track));
	bmedia_probe_track_init(&track->media);

    track->language = NULL;
	track->media.number = asf_stream.streamNumber;
	track->encryptedContentFlag = asf_stream.encryptedContentFlag;
    track->hidden = hidden;
    track->aspectRatioValid = false;
    track->dynamicRangeControlValid = false;
    track->aspectRatio.x = 0;
    track->aspectRatio.y = 0;
    track->dynamicRangeControl.peakReference = 0;
    track->dynamicRangeControl.peakTarget = 0;
    track->dynamicRangeControl.averageReference = 0;
    track->dynamicRangeControl.averageTarget = 0;
    track->averageTimePerFrame = 0;

	if(BASF_STREAM_VIDEO(&asf_stream)) {
		track->media.type = bmedia_track_type_video;
		track_valid = b_asf_probe_video_stream(probe, track, cursor);
	} else if (BASF_STREAM_AUDIO(&asf_stream)) {
		track->media.type = bmedia_track_type_audio;
		track_valid = b_asf_probe_audio_stream(probe, track, cursor);
	} else {
		track->media.type = bmedia_track_type_other;
		track_valid = true;
	}
	if(track_valid) {
		/*  coverity[address_free] */
		bmedia_probe_add_track(&stream->media, &track->media); /* free is not bad since &track->media == track */
	} else {
		BKNI_Free(track);
	}

done:
    return;
}

static basf_parser_handler_action
b_asf_probe_stream_properties(basf_parser_object_handler *handler,  batom_t header)
{
	basf_probe_t probe;
	batom_cursor cursor;

	batom_cursor_from_atom(&cursor, header);

	probe = ((b_asf_probe_object_handler *)handler)->probe;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    b_asf_parse_stream_properties(probe, &cursor, false);
	return basf_parser_handler_action_none;
}


static basf_parser_handler_action
b_asf_probe_extended_stream_properties(basf_parser_object_handler *handler,  batom_t header)
{
	basf_probe_t probe;
	batom_cursor cursor;
    b_asf_extended_stream_properties_element *properties;

	batom_cursor_from_atom(&cursor, header);

	probe = ((b_asf_probe_object_handler *)handler)->probe;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);

    properties = BKNI_Malloc(sizeof(*properties));
    if(!properties) { (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto done; }

    if(!basf_read_extended_stream_properties(&cursor, &properties->extended_stream_properties)) {
		BDBG_WRN(("b_asf_probe_extended_stream_properties: %#lx error in the stream header", (unsigned long)probe));
        BKNI_Free(properties);
        goto done;
    }
    if(properties->extended_stream_properties.hasStreamPropertiesObject) {
        b_asf_parse_stream_properties(probe, &cursor, true);
    }
    BLST_S_INSERT_HEAD(&probe->extended_stream_properties_list, properties, link);
done:
	return basf_parser_handler_action_none;
}

static basf_parser_handler_action
b_asf_probe_metadata(basf_parser_object_handler *handler,  batom_t header)
{
	batom_cursor cursor;
	basf_probe_t probe;
    basf_word count;
    unsigned i;

	probe = ((b_asf_probe_object_handler *)handler)->probe;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);

	batom_cursor_from_atom(&cursor, header);
    count = basf_read_word(&cursor);
    if(BATOM_IS_EOF(&cursor)) {goto done;}
    for(i=0;i<count;i++) {
        basf_description_record record;
        int tag;
        bool temp[B_ASF_N_META_TAGS];
        if(!basf_read_description_record(&cursor, &record)) {
            goto done;
        }
        tag = b_asf_meta_match_record(&cursor, record.nameLength, B_ASF_N_META_TAGS, b_asf_meta_tags, temp);
        if(tag==B_ASF_META_DEVICE_CONFORMANCE_TEMPLATE) {
            bool parsed=false;
            unsigned len = record.dataLength/2;
            if(record.dataType==B_ASF_METADATA_ENTRY_STRING && record.dataLength%2==0 && (len + 1 < BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER - probe->deviceConformanceTemplate.offset)) {
                unsigned j;
                for(j=0;j<len;j++) {
                    basf_word ch;
                    ch = basf_read_word(&cursor);
                    if(ch=='\0') {
                        break;
                    }
                    probe->deviceConformanceTemplate.data[probe->deviceConformanceTemplate.offset+j] = ch;
                }
                len = j;
                if(!BATOM_IS_EOF(&cursor)) {
                    probe->deviceConformanceTemplate.data[probe->deviceConformanceTemplate.offset+len]=0;
                    probe->deviceConformanceTemplate.offset += len+1;
                    probe->deviceConformanceTemplate.count ++;
                    parsed = true;
                }
            } else {
                batom_cursor_skip(&cursor, record.dataLength);
            }
            if(!parsed) {
                BDBG_WRN(("%#lx: skipped DeviceConformanceTemplate:%u:%u at %u", (unsigned long)probe, record.dataType, record.dataLength, probe->deviceConformanceTemplate.count));
            }
        } else if(tag>=0) {
            uint32_t data;
            if(b_asf_metadata_read_data_uint32(&cursor, &record, &data)) {
                b_asf_metadata_element *e = BLST_S_FIRST(&probe->metadata_free);
                BDBG_MSG(("b_asf_probe_metadata: %#lx found tag %s:%u for stream %u", (unsigned long)probe, b_asf_meta_tags[tag], (unsigned)data, (unsigned)record.streamNumber));
                if(e) {
                    e->streamNumber = record.streamNumber;
                    e->tag = tag;
                    e->value = data;
                    BLST_S_REMOVE_HEAD(&probe->metadata_free, link);
                    BLST_S_INSERT_HEAD(&probe->metadata_parsed, e, link);
                }
            }
        } else {
            batom_cursor_skip(&cursor, record.dataLength);
        }
    }
    
done:
	return basf_parser_handler_action_none;
}

static void 
b_asf_probe_simple_index(void *application_cnxt, const basf_simple_index *index)
{
	basf_probe_t probe = application_cnxt;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    BSTD_UNUSED(index);
    BDBG_MSG(("b_asf_probe_simple_index: %#lx", (unsigned long)application_cnxt));
    if(probe->stream) {
        probe->stream->media.index = bmedia_probe_index_available;
        probe->stream->simple_index.offset = index->object_offset;
        probe->stream->simple_index.size = index->object_size;
    }
	return;
}

static void 
b_asf_probe_index(void *application_cnxt, const basf_index *index)
{
	basf_probe_t probe = application_cnxt;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    BSTD_UNUSED(index);
    BDBG_MSG(("b_asf_probe_index: %#lx", (unsigned long)application_cnxt));
    if(probe->stream) {
        probe->stream->index.offset = index->object_offset;
        probe->stream->index.size = index->object_size;
        probe->index = *index;
    }
	return;
}

static void
b_asf_probe_object(void *application_cnxt, const basf_guid *guid, basf_qword object_offset,basf_qword  object_size)
{
	basf_probe_t probe = application_cnxt;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    BSTD_UNUSED(guid);
    if(basf_compare_guid(guid, &basf_simple_index_guid)!=0) {
        probe->next_seek = object_offset+object_size;
    }
    return;
}

static bmedia_probe_base_t
b_asf_probe_create(batom_factory_t factory)
{
	basf_probe_t	probe;
	basf_parser_cfg cfg;

	probe = BKNI_Malloc(sizeof(*probe));
	if(!probe) {
		BDBG_ERR(("b_asf_probe_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, basf_probe_t);
	probe->stream = NULL;
	basf_parser_default_cfg(&cfg);
    BLST_S_INIT(&probe->extended_stream_properties_list);
    BLST_S_INIT(&probe->metadata_parsed);
    BLST_S_INIT(&probe->metadata_free);
    cfg.simple_index = b_asf_probe_simple_index;
    cfg.index = b_asf_probe_index;
    cfg.object = b_asf_probe_object;
    cfg.application_cnxt = probe;
	probe->parser = basf_parser_create(factory, &cfg);
	if(!probe->parser) {
		goto err_parser;
	}
	probe->stream_properties.probe = probe;
	probe->extended_stream_properties.probe = probe;
	probe->file_properties.probe = probe;
	probe->metadata.probe = probe;
    probe->content_encryption.probe = probe;
    probe->extended_content_encryption.probe = probe;
    probe->extended_content_description.probe = probe;
    probe->language_list.probe = probe;
    probe->content_encrypted = false;
    probe->encryption_type = basf_encryption_none;
    probe->language_entries_count = 0;
    BKNI_Memset(&probe->index, 0, sizeof(basf_index));
	basf_parser_install_object_handler(probe->parser, &probe->extended_stream_properties.handler, &basf_extended_stream_properties_guid, b_asf_probe_extended_stream_properties);
	basf_parser_install_object_handler(probe->parser, &probe->stream_properties.handler, &basf_stream_properties_guid, b_asf_probe_stream_properties);
	basf_parser_install_object_handler(probe->parser, &probe->file_properties.handler, &basf_file_properties_guid, b_asf_probe_file_properties);
	basf_parser_install_object_handler(probe->parser, &probe->metadata.handler, &basf_metadata_guid, b_asf_probe_metadata);
    basf_parser_install_object_handler(probe->parser, &probe->content_encryption.handler, &basf_content_encryption_guid, b_asf_probe_content_encryption);
    basf_parser_install_object_handler(probe->parser, &probe->extended_content_encryption.handler, &basf_extended_content_encryption_guid, b_asf_probe_extended_content_encryption);
    basf_parser_install_object_handler(probe->parser, &probe->extended_content_description.handler, &basf_extended_content_descryption_guid, b_asf_probe_extended_content_description);
    basf_parser_install_object_handler(probe->parser, &probe->language_list.handler, &basf_language_list_guid, b_asf_probe_language_list);
	return (bmedia_probe_base_t)probe;
err_parser:
	BKNI_Free(probe);
err_alloc:
	return NULL;
}

static void 
b_asf_probe_destroy(bmedia_probe_base_t probe_)
{
	basf_probe_t probe = (basf_probe_t)probe_;

	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
    basf_parser_remove_object_handler(probe->parser, &probe->language_list.handler);
    basf_parser_remove_object_handler(probe->parser, &probe->extended_content_description.handler); 
    basf_parser_remove_object_handler(probe->parser, &probe->extended_content_encryption.handler); 
    basf_parser_remove_object_handler(probe->parser, &probe->content_encryption.handler); 
	basf_parser_remove_object_handler(probe->parser, &probe->metadata.handler); 
	basf_parser_remove_object_handler(probe->parser, &probe->extended_stream_properties.handler); 
	basf_parser_remove_object_handler(probe->parser, &probe->stream_properties.handler);
	basf_parser_remove_object_handler(probe->parser, &probe->file_properties.handler);
	basf_parser_destroy(probe->parser);
	BDBG_OBJECT_DESTROY(probe, basf_probe_t);
	BKNI_Free(probe);
	return;
}


const bmedia_probe_stream *
b_asf_probe_parse(bmedia_probe_base_t probe_, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config)
{
	basf_probe_t probe = (basf_probe_t)probe_;
	off_t off;
	const size_t read_len = BMEDIA_PROBE_FEED_SIZE;
	basf_parser_info info;
	basf_probe_stream *stream;
    unsigned i;
    batom_t atom;
    bfile_buffer_result result;
    size_t feed_len;
    size_t atom_len;
    b_asf_extended_stream_properties_element *extended_stream_properties;

	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
	BDBG_ASSERT(probe->stream==NULL);
    BLST_S_INIT(&probe->extended_stream_properties_list);
    BLST_S_INIT(&probe->metadata_parsed);
    BLST_S_INIT(&probe->metadata_free);

    BKNI_Memset(&probe->stereoscopic_info, 0, sizeof(probe->stereoscopic_info));
    probe->stereoscopic_info.stereoscopic = false;
    probe->stereoscopic_info.layout = basf_stereoscopic_unknown;
    probe->deviceConformanceTemplate.offset = 0;
    probe->deviceConformanceTemplate.count = 0;

    for(i=0;i<sizeof(probe->metadata_array)/sizeof(probe->metadata_array[0]);i++) {
        BLST_S_INSERT_HEAD(&probe->metadata_free, &probe->metadata_array[i], link);
    }

	for(off=config->parse_offset;;) {

		BDBG_MSG(("b_asf_probe_parse: %#lx reading %u:%u", (unsigned long)probe, (unsigned)off, read_len));
		atom = bfile_buffer_read(buf, off, read_len, &result);
		if(!atom) {
			break;
		}
		atom_len = batom_len(atom);
		BDBG_MSG(("b_asf_probe_parse: %#lx read %u:%u -> %#lx", (unsigned long)probe, (unsigned)off, atom_len, (unsigned long)atom));
		off += atom_len;
		batom_pipe_push(pipe, atom);
		feed_len = basf_parser_feed(probe->parser, pipe);
		if(feed_len!=atom_len) {
			break;
		}
		basf_parser_get_info(probe->parser, &info);
		if(info.state==basf_parser_state_data_object) {
			break;
		}
	}
    basf_parser_get_info(probe->parser, &info);
    probe->stream->header.offset = info.header.offset;
    probe->stream->header.size = info.header.size;
    probe->stream->data.offset = info.data.object_offset;
    probe->stream->data.size = info.data.object_size;
    if(config->probe_index && probe->stream && probe->stream->media.index == bmedia_probe_index_unknown) {
        if(info.state==basf_parser_state_data_object && info.data.total_data_packets > 0) {
            unsigned i;
            batom_pipe_flush(pipe);
            off = info.data.data_offset + info.file.pkt_size*(info.data.total_data_packets);
            basf_parser_seek(probe->parser, off);
            off += config->parse_offset;
            if(info.file.seekable && info.file.duration) {
                probe->stream->media.index = bmedia_probe_index_self;
            } else {
                probe->stream->media.index = bmedia_probe_index_missing;
            }
            for(i=0;probe->stream->media.index != bmedia_probe_index_available && i<(16 * 4096);) {
                uint64_t index_end = probe->stream->index.offset + probe->stream->index.size;
                BDBG_MSG(("b_asf_probe_parse: %#lx reading index %u:%u", (unsigned long)probe, (unsigned)off, read_len));
                atom = bfile_buffer_read(buf, off, read_len, &result);
                if(!atom) {
                    break;
                }
                atom_len = batom_len(atom);
                BDBG_MSG(("b_asf_probe_parse: %#lx read index %u:%u -> %#lx", (unsigned long)probe, (unsigned)off, atom_len, (unsigned long)atom));
                off += atom_len;
                i += atom_len;
                batom_pipe_push(pipe, atom);
                probe->next_seek = 0;
                feed_len = basf_parser_feed(probe->parser, pipe);
                if(feed_len!=atom_len) {
                    break;
                }
                index_end = probe->stream->index.offset + probe->stream->index.size;
                if(off<(off_t)index_end) {
                    BDBG_MSG(("b_asf_probe_parse: %#lx skping over index index %lu -> %lu", (unsigned long)probe, (unsigned long)off, (unsigned long)index_end));
                    off = index_end;
                    basf_parser_seek(probe->parser, off);
                } else if(off<(off_t)probe->next_seek) {
                    BDBG_MSG(("b_asf_probe_parse: %#lx skping over other object %lu -> %lu", (unsigned long)probe, (unsigned long)off, (unsigned long)probe->next_seek));
                    off = probe->next_seek;
                    basf_parser_seek(probe->parser, off);
                }
            }
        }
    }

    if (probe->stream && config->parse_index) {
        b_asf_probe_parse_index(probe, buf);
    }

	/* return result of parsing */
	stream = probe->stream;
	probe->stream = NULL;
    if(stream) {
	    basf_probe_track *track;

        stream->stereoscopic_info = probe->stereoscopic_info;
        if(stream->media.max_bitrate==0) {
            if(info.data.total_data_packets!=(size_t)(-1) && stream->media.duration!=0) {
                stream->media.max_bitrate = ((1000*8)*((info.data.total_data_packets*info.file.pkt_size+info.data.data_offset)))/stream->media.duration;
                BDBG_MSG(("b_asf_probe_parse: %#lx max_bitrate %uKbps", (unsigned long)probe, stream->media.max_bitrate/1024));
            }
        }
        
        stream->encryption_type = probe->encryption_type;

        stream->deviceConformanceTemplateCount = probe->deviceConformanceTemplate.count;
        BKNI_Memcpy(stream->deviceConformanceTemplates, probe->deviceConformanceTemplate.data, probe->deviceConformanceTemplate.offset*sizeof(stream->deviceConformanceTemplates[0]) );

        for(track=(basf_probe_track*)BLST_SQ_FIRST(&stream->media.tracks); track; track=(basf_probe_track*)BLST_SQ_NEXT(&track->media, link) ) {
            const b_asf_metadata_element *aspectX = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_ASPECT_RATIO_X);
            const b_asf_metadata_element *aspectY = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_ASPECT_RATIO_Y);
            const b_asf_metadata_element *peakReference = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_WMA_PEAK_REFERENCE);
            const b_asf_metadata_element *peakTarget = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_WMA_PEAK_TARGET);
            const b_asf_metadata_element *averageReference = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_WMA_AVERAGE_REFERENCE);
            const b_asf_metadata_element *averageTarget = b_asf_metadata_find(&probe->metadata_parsed, track->media.number, B_ASF_META_WMA_AVERAGE_TARGET);
            if(aspectX && aspectY) {
                track->aspectRatio.x = aspectX->value;
                track->aspectRatio.y = aspectY->value;
                track->aspectRatioValid = true;
                BDBG_MSG(("b_asf_probe_parse: %#lx track %u pixelAspectRatio: %u:%u", (unsigned long)probe, (unsigned)track->media.number, track->aspectRatio.x, track->aspectRatio.y));
            }
            if(peakReference && peakTarget && averageReference && averageTarget) {
                track->dynamicRangeControl.peakReference = peakReference->value;
                track->dynamicRangeControl.peakTarget = peakTarget->value;
                track->dynamicRangeControl.averageReference = averageReference->value;
                track->dynamicRangeControl.averageTarget = averageTarget->value;
                BDBG_MSG(("b_asf_probe_parse: %#lx track %u dynamicRangeControl: %u:%u:%u:%u", (unsigned long)probe, (unsigned)track->media.number, track->dynamicRangeControl.peakReference, track->dynamicRangeControl.peakTarget, track->dynamicRangeControl.averageReference, track->dynamicRangeControl.averageTarget));
            }
            for(extended_stream_properties=BLST_S_FIRST(&probe->extended_stream_properties_list);extended_stream_properties!=NULL;extended_stream_properties = BLST_S_NEXT(extended_stream_properties, link)) {
                if(extended_stream_properties->extended_stream_properties.streamNumber == track->media.number) {
                    track->averageTimePerFrame = extended_stream_properties->extended_stream_properties.averageTimePerFrame;
                    if(extended_stream_properties->extended_stream_properties.streamLanguageIDIndex < probe->language_entries_count) {
                        const b_asf_probe_language_entry *entry = probe->language_entries[extended_stream_properties->extended_stream_properties.streamLanguageIDIndex];
                        size_t length;
                        BDBG_ASSERT(entry);
                        length = entry->length*sizeof(basf_word);
                        track->language = BKNI_Malloc(length);
                        if(track->language) {
                            BKNI_Memcpy(track->language, entry->data, length);
                        }
                    } else {
                        BDBG_WRN(("b_asf_probe_parse:%#lx track %u uses invalid language ID %u(%u)", (unsigned long)probe, track->media.number, extended_stream_properties->extended_stream_properties.streamLanguageIDIndex, probe->language_entries_count));
                    }
                }
                if(track->media.type == bmedia_track_type_video) {
                    track->media.info.video.bitrate = extended_stream_properties->extended_stream_properties.dataBitrate/1024;
                }
            }
            if (probe->content_encrypted) {
                track->encryptedContentFlag = true;
            }
        }
    }
    while(NULL!=(extended_stream_properties=BLST_S_FIRST(&probe->extended_stream_properties_list))) {
        BLST_S_REMOVE_HEAD(&probe->extended_stream_properties_list, link);
        BKNI_Free(extended_stream_properties);
    }
    for(i=0;i<probe->language_entries_count;i++) {
        BKNI_Free(probe->language_entries[i]);
        probe->language_entries[i] = NULL;
    }
    probe->language_entries_count=0;
	basf_parser_reset(probe->parser);
	return &stream->media;
}

static void
b_asf_probe_stream_free(bmedia_probe_base_t probe_, bmedia_probe_stream *stream)
{
	basf_probe_t probe = (basf_probe_t)probe_;
	basf_probe_track *track;
	BDBG_OBJECT_ASSERT(probe, basf_probe_t);
	BSTD_UNUSED(probe);

	BDBG_ASSERT(stream);

	while(NULL!=(track=(basf_probe_track*)BLST_SQ_FIRST(&stream->tracks))) {
		BLST_SQ_REMOVE_HEAD(&stream->tracks, link);
        if(track->language) {
            BKNI_Free(track->language);
        }
		BKNI_Free(track);
	}
	BKNI_Free(stream);
	return;
}


const bmedia_probe_format_desc basf_probe = {
	bstream_mpeg_type_asf,
	b_asf_ext, /* ext_list */
	sizeof(basf_guid)+sizeof(basf_qword), /* header_size */
	b_asf_probe_header_match, /* header_match */
	b_asf_probe_create, /* create */
	b_asf_probe_destroy, /* destroy */
	b_asf_probe_parse, /* parse */
	b_asf_probe_stream_free /* stream free */
};

