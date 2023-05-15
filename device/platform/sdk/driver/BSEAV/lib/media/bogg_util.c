/***************************************************************************
 *     Copyright (c) 2011 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bogg_util.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/25/11 7:20p $
 *
 * Module Description:
 *
 * OGG parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bogg_util.c $
 * 
 * 2   8/25/11 7:20p vsilyaev
 * SWDTV-8388: Fixed warning
 * 
 * 1   8/25/11 6:17p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bogg_util.h"
#include "bkni.h"

BDBG_MODULE(bogg_util);

bool bogg_parse_page_header(batom_cursor *cursor, bogg_page_header *header)
{
    uint32_t sync;
    int data;

    sync = bogg_read_uint32(cursor);
    if(BATOM_IS_EOF(cursor) || sync!=BMEDIA_FOURCC('O','g','g','S')) {
        return false;
    }
    data = batom_cursor_next(cursor); /* version */
    if(data==BATOM_EOF) {
        return false;
    }
    if(data!=0) {
        BDBG_WRN(("unsupported OGG version:%d", data));
        return false;
    }
    header->header_type = bogg_read_uint8(cursor);
    header->granule_position = bogg_read_uint64(cursor);
    header->bitstream_serial_number = bogg_read_uint32(cursor);
    header->page_sequence_number = bogg_read_uint32(cursor);
    header->crc = bogg_read_uint32(cursor);
    header->page_segments = bogg_read_uint8(cursor);
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }
    BDBG_MSG(("page_header: header_type:%#x granule_position:%#lu bitstream_serial_number:%u page_sequence_number:%u page_segments:%u", (unsigned)header->header_type, (unsigned long)header->granule_position, (unsigned)header->bitstream_serial_number, (unsigned)header->page_sequence_number, (unsigned)header->page_segments));
    return true;
}


#define B_OGG_LACING_VALUE   255

bool bogg_page_payload_parser_init(bogg_page_payload_parser *parser, batom_cursor *cursor, const bogg_page_header *header)
{
    BATOM_CLONE(&parser->segment, cursor);
    parser->payload = 0;
    return batom_cursor_skip(cursor, header->page_segments)==header->page_segments;
}

int bogg_page_payload_parser_next(bogg_page_payload_parser *parser, const bogg_page_header *header, bool *spanning)
{
    int size;
    unsigned i;

    *spanning = false;
    if(parser->payload>=header->page_segments) {
        return -1;
    }
    for(size=0,i=parser->payload;;) {
        int data = batom_cursor_next(&parser->segment); 
        if(data==BATOM_EOF) {
            size = -1;
        }
        i++;
        size += data;
        if(data!=B_OGG_LACING_VALUE) {
            break;
        }
        if(i>=header->page_segments) {
            *spanning = (data == B_OGG_LACING_VALUE);
            break;
        }
    }
    parser->payload = i;
    BDBG_MSG(("bogg_page_payload_parser_next: %d", size));
    return size;
}

int bogg_page_get_size(const bogg_page_header *header, const batom_cursor *cursor)
{
    batom_cursor data;
    int size;
    bogg_page_payload_parser parser;

    BATOM_CLONE(&data, cursor);
    if(!bogg_page_payload_parser_init(&parser, &data, header)) {
        return -1;
    }
    for(size=header->page_segments;;) {
        bool spanning;
        int rc = bogg_page_payload_parser_next(&parser, header, &spanning);
        if(rc<0) {
            break;
        }
        size += rc;
    }
    BDBG_MSG(("bogg_page_payload_parser_next: %d", size));
    return size;
}

bool bvorbis_parse_frame_header(batom_cursor *cursor, bvorbis_frame_header *header)
{
    static const char vorbis[]={'v','o','r','b','i','s'};
    unsigned i;
    header->packet_type = batom_cursor_byte(cursor);
    for(i=0;i<sizeof(vorbis)/sizeof(*vorbis);i++) {
        int data = batom_cursor_next(cursor); /* version */
        if(data==BATOM_EOF) {
            return false;
        }
        if(data!=vorbis[i]) {
            return false;
        }
    }
    BDBG_MSG(("bvorbis_parse_frame_header: type %u", header->packet_type));
    return true;
}

bool bvorbis_parse_indentification_header(batom_cursor *cursor, bvorbis_indentification_header *header)
{
    header->vorbis_version = batom_cursor_uint32_le(cursor);
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }
    if(header->vorbis_version!=0) {
        BDBG_WRN(("bvorbis_parse_indentification_header: unsupported version %#x", header->vorbis_version));
        return false;
    }

    header->audio_channels = batom_cursor_byte(cursor);
    header->audio_sample_rate = batom_cursor_uint32_le(cursor);
    header->bitrate_maximum = batom_cursor_uint32_le(cursor);
    header->bitrate_nominal = batom_cursor_uint32_le(cursor);
    header->bitrate_minimum = batom_cursor_uint32_le(cursor);
    batom_cursor_byte(cursor); /* skip blocksize_0/1 */
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }
    BDBG_MSG(("bvorbis_parse_indentification_header: audio_channels:%u audio_sample_rate:%u bitrate(%u/%u/%u)", header->audio_channels, header->audio_sample_rate, header->bitrate_maximum, header->bitrate_nominal, header->bitrate_minimum));
    return  true;
}

