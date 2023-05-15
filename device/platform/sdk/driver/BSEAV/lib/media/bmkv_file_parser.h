/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmkv_file_parser.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 7/1/11 12:59p $
 *
 * Module Description:
 *
 * MKV container file_parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmkv_file_parser.h $
 * 
 * 6   7/1/11 12:59p vsilyaev
 * SWDTV-7732: Added webm DocType
 * 
 * 5   10/1/10 5:49p vsilyaev
 * SW35230-1433: Added handling of 'Header Stripping' payload compression
 * 
 * 4   6/21/10 12:49p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 3   6/11/10 10:31a gmohile
 * SW3548-2978 : Add support to parse mkv chapters
 * 
 * 2   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 1   3/4/08 12:20p vsilyaev
 * PR 39818: File based MKV parser
 * 
 *******************************************************************************/
#ifndef _BMKV_FILE_PARSER_H__
#define _BMKV_FILE_PARSER_H__

#include "bmkv_file_parser.h"
#include "bmkv_parser.h"
#include "bfile_buffer.h"
#include "bfile_cache.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmkv_file_parser *bmkv_file_parser_t;

typedef struct b_mkv_file_parser_handler {
	bmkv_parser_handler handler; /* must be first member */
	bmkv_file_parser_t file_parser;
} b_mkv_file_parser_handler;

struct bmkv_file_parser {
	BDBG_OBJECT(bmkv_file_parser_t)
	bmkv_parser_t  mkv;
	bmkv_Tracks trackData;
	bmkv_SeekHead seekhead;
	bmkv_SegmentInformation segment_info;

	struct {
		bool ebml;
		bool tracks;
		bool seekhead;
		bool segment_info;
	} validate;
	struct {
		bool tracks;
		bool segment_info;
		bool attachment;
		bool chapters;
	} seek_to;

	bfile_segment segment;
	bfile_segment cluster_seekhead;
	bfile_segment attachment;
	bfile_segment chapters;

	uint64_t next_seek;
	bmkv_id seek_to_element;
	bool stream_error;
    bool next_volume;
	bool segment_end;
    char docType[16];

	b_mkv_file_parser_handler ebml_handler;
	b_mkv_file_parser_handler seekhead_handler;
	b_mkv_file_parser_handler tracks_handler;
	b_mkv_file_parser_handler segment_info_handler;
	struct {
		bool attachment;
		bool chapters;
	} config;
};

typedef struct bmkv_file_encoding_info {
    bool present;
    bool supported;
    bool ContentCompAlgo_valid;
    unsigned ContentCompAlgo;
    bmkv_data ContentCompSettings;
} bmkv_file_encoding_info;

void bmkv_file_parser_release(bmkv_file_parser_t file_parser);
int bmkv_file_parser_init(bmkv_file_parser_t file_parser, batom_factory_t factory);
void bmkv_file_parser_shutdown(bmkv_file_parser_t file_parser);
int bmkv_file_parser_parse(bmkv_file_parser_t file_parser, bfile_buffer_t file_buf, batom_pipe_t pipe_mkv);
void bmkv_file_parser_process_encoding_info(const bmkv_TrackEntry *track, bmkv_file_encoding_info *info);

#ifdef __cplusplus
}
#endif


#endif /* _BMKV_FILE_PARSER_H__ */

