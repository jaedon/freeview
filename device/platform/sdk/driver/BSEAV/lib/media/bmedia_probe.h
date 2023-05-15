/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe.h $
 * $brcm_Revision: 31 $
 * $brcm_Date: 12/30/10 1:16p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe.h $
 * 
 * 31   12/30/10 1:16p vsilyaev
 * SWBLURAY-23899: Merged support to control min size of parsed data.
 * 
 * 30   11/4/10 11:58a vsilyaev
 * SW35230-2036: Clarified conditions when bmpeg2ts_psi_probe_track could
 * be used
 * 
 * 29   8/27/10 5:28p vsilyaev
 * SWBLURAY-21085: Improved detection time duration of MPEG-2 TS  streams
 * 
 * 28   6/21/10 12:50p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 27   3/3/10 11:26a vsilyaev
 * SW7405-3959: Use 32-bit field for video bitrate
 * 
 * 26   2/11/10 7:25p vsilyaev
 * SW3556-1051: Added enum to specify timestamp order in the stream
 * 
 * 25   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 24   10/1/09 12:08p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 * 
 * fix_bdvd_v1.5_pr15387/3   10/1/09 2:08p gooslin
 * Move max_av_byte_delta to the proper probe struct
 * 
 * fix_bdvd_v1.5_pr15387/2   9/29/09 5:32p gooslin
 * Merge latest
 * 
 * fix_bdvd_v1.5_pr15387/1   9/29/09 5:27p gooslin
 * Add index parse flag
 * 
 * 23   9/10/09 12:15p vsilyaev
 * SW7335-529: Use bmedia_types for container, video and audio types
 * 
 * 22   7/13/09 1:42p vsilyaev
 * PR 55989: Mark proper MPEG-2 TS files as self-indexable
 * 
 * 21   6/9/09 7:32p vsilyaev
 * PR 54342: Added code to probe index in the media files
 * 
 * 20   1/22/09 3:51p gmohile
 * PR 51373 : Probe bitrate for mpeg video es streams
 * 
 * 19   12/8/08 4:13p vsilyaev
 * PR 47650: Added code to parse ID3 meta-data
 * 
 * 18   12/1/08 5:56p vsilyaev
 * PR 49646: Fixed size of spare data
 * 
 * 17   11/25/08 1:12a vsilyaev
 * PR 49646: Added code to parse and print H264 profile/level information
 * 
 * 16   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 15   11/11/08 6:07p vsilyaev
 * PR 47926: Handle ES files with wrong extension
 * 
 * 14   10/15/08 7:18p vsilyaev
 * PR 47926: Added probe mode to handle files with wrong extension
 * 
 * 13   9/18/08 3:27p vsilyaev
 * PR 47105: Added support for layered buffer cache
 * 
 * 12   9/25/07 2:45p gmohile
 * PR 34640 : Remove private date from track structure
 * 
 * 11   9/6/07 4:58p gmohile
 * PR 34640 : Add Divx Drm support
 * 
 * 10   7/25/07 1:51p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 9   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 8   7/21/07 9:53p vsilyaev
 * PR 32813: Added MPEG-1 System probe
 * 
 * 7   7/20/07 11:57p vsilyaev
 * PR 32813: Added MPEG video probe
 * 
 * 6   7/20/07 1:36p vsilyaev
 * PR 32813: Added support for ES files
 * 
 * 5   7/19/07 12:17p vsilyaev
 * PR 32813: Renamed bmedia_track to bmedia_track_type
 * 
 * 4   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 3   7/10/07 5:22p vsilyaev
 * PR 32813: Added stream duration field
 * 
 * 2   7/10/07 12:42p erickson
 * PR32813: fix issues while integrating with Brutus
 * 
 * 1   7/9/07 4:06p vsilyaev
 * PR 32813: Stream format probe module
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PROBE_H__
#define _BMEDIA_PROBE_H__

#include "bfile_buffer.h"
#include "bmedia_types.h"
#include "blst_squeue.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*================== Module Overview =====================================
This module is used to extract (probe) program information from the multimedia container.
Information that could be extracted from the multimedia container depends on the
format, however this type of data would be extracted for all formats:
    o type of container
	o number of tracks 
	o number of programs
	o for each track, track number (also known as PID or stream id), track type (audio or video) and codec type, would be extracted as well

At this type library supports following container formats:
    o MPEG-2 TS
	o MPEG-2 PES (also known as MPEG-2 Program, DVD VOB)
	o MPEG-1 System
	o AVI (also know as DivX)
	o ASF
	o MP4

Sample code:
	const char *file = "test.mpg"
	bmedia_probe_t probe = bmedia_probe_create();
	FILE *fin = fopen(file,"rb");
	bfile_io_read_t  fd=bfile_stdio_read_attach(fin);
	const bmedia_probe_stream *stream;
	const bmedia_probe_track *track;
	bmedia_probe_config probe_config;
	char stream_info[512];

	bmedia_probe_default_cfg(&probe_config);
	probe_config.file_name = file;
	stream = bmedia_probe_parse(probe, fd, &probe_config);
	if(stream) {
		bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
		printf("%s: %s", file, stream_info);
		for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
			if(track->type==bmedia_track_type_video) {
			    printf("video track %u codec:%u\n", track->number, track->info.video.codec);
			}
		}
		bmedia_probe_stream_free(stream);
	}
	bfile_stdio_read_detach(fd);
	fclose(fin);
	bmedia_probe_destroy(probe);

========================================================================*/


/*
Summary:
  bmedia_probe_t is the descriptror for media probe library
Description:
  Multiple instances of bmedia_probe_t could exist at any given time, 
  however single instance of bmedia_probe_t couls parse(probe) only single
  file at the time 
*/
typedef struct bmedia_probe *bmedia_probe_t;

/*
Summary:
   This enum is used to indicate timestamp ordering in the stream
Description:
   Historically dIfferent formats use different ordering of timestamps. Timestamp could be either linked
   to the particular frame, and reordered together with frames (this is most often used configuration)
   or timestamps could be not associated with the frames and just stored in the sequentail order.
*/
typedef enum bmedia_timestamp_order {
    bmedia_timestamp_order_display, /* stream timestamps are in the display order (PTS), timestamp linked to each frame, and if stream has B frames timestamps saved in the stream aren't in sequential order */
    bmedia_timestamp_order_decode  /* stream timestamps are in the decode order (DTS),  timestamps aren't linked to each frame and even if stream has B frames, timestamps saved in the stream are still sequential */
} bmedia_timestamp_order;

/*
Summary:
   This structure defines common properties of video track
*/
typedef struct bmedia_probe_video {
	bvideo_codec codec; /* type  of video codec */
    bmedia_timestamp_order timestamp_order; /* order of timestamps in the stream */
    uint16_t width; /* coded video width, or 0 if unknown */
	uint16_t height; /* coded video height, or 0 if unknown  */
	unsigned bitrate; /* video bitrate in Kbps, or 0 if unknown */
    struct {
        uint8_t spare[32];
    } codec_specific; /* codec specific data */
} bmedia_probe_video;

/*
Summary:
   This structure defines common properties of audio track
*/
typedef struct bmedia_probe_audio {
	baudio_format codec; /* type of audio codec */
	uint8_t channel_count;  /* number of channels, or 0 if unknown  */
	uint8_t sample_size; /* number of bits in the each sample, or 0 if unknown */
	uint16_t bitrate; /* audio bitrate in Kbps, or 0 if unknown */
	unsigned sample_rate; /* audio sampling rate in Hz, or 0 if unknown */
	
/* #if defined(HUMAX_PLATFORM_BASE) *//* Add language-info to the probe-result of Audio track  by JGKIM[Retail-MC] */	
	uint8_t languageCode[4]; /* 3 ascii codes, add one more for "\0" */
/* #endif //defined(HUMAX_PLATFORM_BASE) */

    struct {
        uint8_t spare[32];
    } codec_specific; /* codec specific data */
} bmedia_probe_audio;


/* #if defined(HUMAX_PLATFORM_BASE) *//* Add Dvb-subtitle to the probe-result  by JGKIM[Retail-MC] */
typedef struct bmedia_probe_dvb_subtitle {
    uint8_t type;
    uint8_t compositionPageId;
    uint8_t ancillaryPageId;
	uint8_t languageCode[4]; /* 3 ascii codes, add one more for "\0" */
} bmedia_probe_dvb_subtitle;
/* #endif //defined(HUMAX_PLATFORM_BASE) */


/*
Summary:
   This enum is used to indicate track type
*/
typedef	enum bmedia_track_type {
	bmedia_track_type_video, /* video track */
	bmedia_track_type_audio, /* audio track */
	bmedia_track_type_pcr,   /* track with PCR information */

/* #if defined(HUMAX_PLATFORM_BASE) *//* Add Dvb-subtitle to the probe-result  by JGKIM[Retail-MC] */
	bmedia_track_type_dvb_subtitle,
/* #endif //defined(HUMAX_PLATFORM_BASE) */

	bmedia_track_type_other /* track type other than listed above, it could be video or audio track with unknown codec type */
} bmedia_track_type;

#define BMEDIA_PROBE_INVALID_PROGRAM    0xFFFFFFFF
/*
Summary:
   This structure is used to describe single track 
*/
typedef struct bmedia_probe_track {
    BLST_SQ_ENTRY(bmedia_probe_track) link; /* this field is used to link tracks together */
	bmedia_track_type type; /* type of track */
	unsigned number; /* unique track ID */
	unsigned program; /* program number that track belongs to */
	union {
		bmedia_probe_audio audio; /* information for audio track */
		bmedia_probe_video video; /* information for video track */
		
/* #if defined(HUMAX_PLATFORM_BASE) *//* Add Dvb-subtitle to the probe-result  by JGKIM[Retail-MC] */
		bmedia_probe_dvb_subtitle dvb_subtitle;  /* information for subtitle track */
/* #endif //defined(HUMAX_PLATFORM_BASE) */

	} info;        
} bmedia_probe_track;

/*
Summary:
   Type of the index avaliable in the media file
*/
typedef enum bmedia_probe_index {
    bmedia_probe_index_unknown, /* information about index file is unknown */
    bmedia_probe_index_available, /* there is an index object and media file could be used with or without an index file */
    bmedia_probe_index_missing, /* there is no index object, and media file could be used without an index */
    bmedia_probe_index_required, /* there is an index object, and media file should be used with the index */
    bmedia_probe_index_self /* content of the file could be uses as an index */
} bmedia_probe_index;

/*
Summary:
   This structure is used to describe entire multimedia stream
*/
typedef struct bmedia_probe_stream {
	BLST_SQ_HEAD(bmedia_probe_track_list, bmedia_probe_track) tracks; /* linked list of tracks */
	bstream_mpeg_type type; /* type of multimedia stream */
    bmedia_probe_index index; /* type of the index data */
	unsigned max_bitrate; /* maximum stream bitreate, bps or 0 if unknown  */
	unsigned duration; /* duration of stream, in milliseconds or 0 if unknown */
	unsigned nprograms; /* total number of programs */
	unsigned ntracks; /* total number of tracks */
	unsigned probe_id; /* unique id of the probe */
} bmedia_probe_stream;

typedef struct bmedia_probe_stream_specific_cfg {
    struct {
        bool probe_attachments; /* allowing probing of extra metadata attached to the container */
        bool probe_next_volume; /* causes read of top-level objects until found either end-of-file or next volume */
    } mkv; /* MKV (Matroska) specific options */
} bmedia_probe_stream_specific_cfg;

/* 
Summary:
    This structure is used to configure the duration calcuation settings
*/
typedef struct bmedia_probe_duration_config {
    int32_t max_bitrate_probe_depth; /* number of times to retry binary probe */
    int32_t max_bitrate_percentage_difference; /* how close the two halves of the bitrate should be */
    int32_t max_bitrate_n_segments; /* number of segments to break a section into */
    uint32_t min_bitrate_segment_size; /* smallest size of a section before we break it into segments */
} bmedia_probe_duration_config;

/*
Summary:
   This structure is used to instruce module on how to parse a stream
*/
typedef struct bmedia_probe_config {
	const char *file_name; /* name of the file, it's used as hint in what stream type is, e.g. if file has extension of .mp4, then this is MP4 container and other parsers wouldn't be used for this file. If this member is NULL, then heuristic based on file name wouldn't be used */
	bstream_mpeg_type type; /* type of the stream, of bstream_mpeg_type_unknown if stream type is unknown. Limits probing of the file to specified type of container. Knowing stream type could speed-up parsing and remove a lot of guess work. */
	bool probe_es; /* if stream wasn't detected as any valid containter, setting this member to true, would cause stream to be tested as any type of elementary streams */
    bool probe_all_formats; /* if heuristics based on extension didn't work then probe stream using all avaliable probes */
    bool probe_payload; /* allows probing payload for some container formats, it allows to extract more track specific information */
    bool probe_duration; /* alllows payload scanning and random access to detect more accurate stream duration */
    bool probe_index; /* allows probing for presence of the index data, usually it causes the media probe to read data toward end of the file */
    bool parse_index; /* allows parsing of the index data, The entire contents of the index table may be read (perhaps more than once) and analyzed to provide more details about the layout of the file */
    bmedia_probe_stream_specific_cfg stream_specific;
    bfile_buffer_t  buffer; /* optional buffer that shall be used to access file data */
    off_t probe_offset; /* first position in the file that should be use for probing, for certain type of files in might be beneficial to start parsing from the middle of file */
    bmedia_probe_duration_config duration_config; /* settings to configure how accurate the duration calcuation should be */
    off_t min_probe_request; /* Minimum amount of data to probe to find all tracks */
} bmedia_probe_config;

/*
Summary:
   This function is used to create new instance of bmedia_probe_t 
Result:
   o NULL - if there isn't enough system resources
   o new instance of bmedia_probe_t
*/
bmedia_probe_t bmedia_probe_create(void);

/*
Summary:
   This function is used to release all resources acquired by bmedia_probe_t
*/
void bmedia_probe_destroy(
		bmedia_probe_t probe /* instance of bmedia_probe_t */
		);

/*
Summary:
   This function is used to initializes bmedia_probe_config structure with default values. 
See also:
    bmedia_probe_parse
*/
void bmedia_probe_default_cfg(
		bmedia_probe_config *config /* pointer to bmedia_probe_config */
		);

/*
Summary:
   This function is used to parse multimedia container
Description:
   The bmedia_probe_parser function parsed multimedia container and returns discovered information.
   Prior calling this function user must provide means of reading (accessing media data) and initialize
   bmedia_probe_config structure.
See also:
	bmedia_probe_default_cfg
	bmedia_probe_stream_free
Result:
   o NULL - unknown or unsupported stream format
   o read-only pointer to stream descriptor
*/
const bmedia_probe_stream *
bmedia_probe_parse(
		bmedia_probe_t probe,  /* instance of bmedia_probe_t */
		bfile_io_read_t fd,    /* interface to access (read) data from multimedia container */
		const bmedia_probe_config *config /* pointer to bmedia_probe_config */
		);

/*
Summary:
   This function is used to convert stream descriptor into textual form
Result:
   o number of characters printed
*/
int 
bmedia_stream_to_string(
		const bmedia_probe_stream *stream,  /* read-only pointer to stream descriptor */
		char *buf,  /* pointer to the character buffer */
		size_t size /* size of character buffer */
		);

/*
Summary:
   This function is used to free resources associated with tbe stream descriptor
*/
void bmedia_probe_stream_free(
		bmedia_probe_t probe,  /* instance of bmedia_probe_t */
		const bmedia_probe_stream *stream /* pointer to the stream descriptor */
		);

#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PROBE_H__ */

