/***************************************************************************
 *     Copyright (c) 2011 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bogg_util.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/7/11 6:35p $
 *
 * Module Description:
 *
 * OGG parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bogg_util.h $
 * 
 * 2   12/7/11 6:35p vsilyaev
 * SW7425-1917: Removed unused macro
 * 
 * 1   8/25/11 6:17p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 *******************************************************************************/

#ifndef _BOGG_UTIL_H__
#define _BOGG_UTIL_H__

#include "bioatom.h"
#include "bmedia_util.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define bogg_read_uint64 batom_cursor_uint64_le 
#define bogg_read_uint32 batom_cursor_uint32_le 
#define bogg_read_uint8 batom_cursor_byte

#define BOGG_PAGE_MIN_HEADER_LENGTH (27+1)
#define BOGG_PAGE_MAX_HEADER_LENGTH (27+255)


#define BOGG_HEADER_TYPE_CONTINUED_PACKET   0x01
#define BOGG_HEADER_TYPE_FIRST_PAGE         0x02
#define BOGG_HEADER_TYPE_LAST_PAGE          0x04

#define BOGG_PAGE_MAX_SEGMENTS  255
typedef struct bogg_page_header {
    uint8_t header_type;
    uint8_t page_segments;
    uint32_t bitstream_serial_number;
    uint32_t page_sequence_number;
    uint32_t crc;
    uint64_t granule_position;
} bogg_page_header;


bool bogg_parse_page_header(batom_cursor *cursor, bogg_page_header *header);

typedef struct bogg_page_payload_parser {
    unsigned payload;
    batom_cursor segment;
} bogg_page_payload_parser;

bool bogg_page_payload_parser_init(bogg_page_payload_parser *parser, batom_cursor *cursor, const bogg_page_header *header);
int bogg_page_payload_parser_next(bogg_page_payload_parser *parser, const bogg_page_header *header, bool *spanning);
int bogg_page_get_size(const bogg_page_header *header, const batom_cursor *cursor);

#define  BVORBIS_IDENTIFICATION_HEADER  1
#define  BVORBIS_COMMENT_HEADER         3
#define  BVORBIS_SETUO_HEADER           5


typedef struct bvorbis_frame_header {
    uint8_t packet_type;
} bvorbis_frame_header;

bool bvorbis_parse_frame_header(batom_cursor *cursor, bvorbis_frame_header *header);

typedef struct bvorbis_indentification_header {
    uint16_t vorbis_version;
    uint8_t audio_channels;
    uint32_t audio_sample_rate;
    uint32_t bitrate_maximum;
    uint32_t bitrate_nominal;
    uint32_t bitrate_minimum;
} bvorbis_indentification_header;

bool bvorbis_parse_indentification_header(batom_cursor *cursor, bvorbis_indentification_header *header);



#ifdef __cplusplus
}
#endif

#endif /* _BOGG_UTIL_H__ */

