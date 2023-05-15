/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_impl.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 12/30/10 1:16p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_impl.h $
 * 
 * 11   12/30/10 1:16p vsilyaev
 * SWBLURAY-23899: Merged support to control min size of parsed data.
 * 
 * fix_bdvd_b3pvr_SWBLURAY-23899/1   12/21/10 12:08p ialauder
 * Allow the min prob amount to be configurable.
 * 
 * 10   6/21/10 12:50p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 9   4/28/10 11:49a vsilyaev
 * SWBLURAY-20104: Decreased size of the segment and increased number of
 * segments
 * 
 * 8   2/22/10 1:15p vsilyaev
 * SW3556-1051: Added function to initialize video track information
 * 
 * 7   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 6   10/1/09 12:08p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 * 
 * fix_bdvd_v1.5_pr15387/1   9/29/09 5:28p gooslin
 * Add index parse flag
 * 
 * 5   6/9/09 7:32p vsilyaev
 * PR 54342: Added code to probe index in the media files
 * 
 * 4   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 3   11/11/08 9:49p vsilyaev
 * PR 48182: Added code to detect stream duration, using parsed timestamps
 * 
 * 2   10/28/08 9:12a vsilyaev
 * PR 45520: Fixed warnings
 * 
 * 1   7/25/07 1:47p vsilyaev
 * PR 32813: internal API of bmedia_probe
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PROBE_IMPL_H__
#define _BMEDIA_PROBE_IMPL_H__

#include "bmedia_probe.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BMEDIA_PROBE_FEED_SIZE	(BIO_BLOCK_SIZE*2)

typedef struct bmedia_probe_parser_config {
    off_t parse_offset;
    bool probe_index; /* allows probing for presence of the index data, usually it causes the media probe to read data toward end of the file */
    bool parse_index; /* allows parsing of the index data, The entire contents of the index table may be read (perhaps more than once) and analyzed to provide more details about the layout of the file */
    bmedia_probe_stream_specific_cfg stream_specific;
    off_t min_parse_request; /* Minimum amount of data to parse to find all tracks */
} bmedia_probe_parser_config;


typedef struct bmedia_probe_format *bmedia_probe_base_t;
typedef char bmedia_probe_file_ext[6];
typedef struct bmedia_probe_format_desc {
	bstream_mpeg_type type;
	const bmedia_probe_file_ext *ext_list; /* list of extensions */
	size_t header_size;
	bool (*header_match)(batom_cursor *header);
	bmedia_probe_base_t (*create)(batom_factory_t factory);
	void (*destroy)(bmedia_probe_base_t probe);
	const bmedia_probe_stream *(*parse)(bmedia_probe_base_t probe, bfile_buffer_t buf, batom_pipe_t pipe, const bmedia_probe_parser_config *config);
	void (*stream_free)(bmedia_probe_base_t probe, bmedia_probe_stream *stream);
} bmedia_probe_format_desc;
	
void bmedia_probe_stream_init(bmedia_probe_stream *stream, bstream_mpeg_type type);
void bmedia_probe_track_init(bmedia_probe_track *track);
void bmedia_probe_basic_stream_free(bmedia_probe_base_t probe, bmedia_probe_stream *stream);
void bmedia_probe_add_track(bmedia_probe_stream *stream, bmedia_probe_track *track);
bool bmedia_probe_match_ext(const bmedia_probe_file_ext *ext_list, const char *ext);
void bmedia_probe_video_init(bmedia_probe_video *video);


typedef struct bmedia_timestamp_parser *bmedia_timestamp_parser_t;

typedef struct bmedia_timestamp {
	uint32_t timestamp;
	off_t offset;
} bmedia_timestamp;

typedef struct bmedia_timestamp_parser_methods {
	void (*seek)(bmedia_timestamp_parser_t parser, off_t offset);
	int (*parse)(bmedia_timestamp_parser_t parser, batom_cursor *cursor, bmedia_timestamp *timestamp);
	void (*destroy)(bmedia_timestamp_parser_t parser);
} bmedia_timestamp_parser_methods;

typedef struct bmedia_timestamp_parser {
	const bmedia_timestamp_parser_methods *methods;
} bmedia_timestamp_parser;





#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PROBE_IMPL_H__ */

