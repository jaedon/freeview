/***************************************************************************
 *     Copyright (c) 1998-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_utils.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 3/5/12 2:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/utils/ts_utils.h $
 * 
 * 6   3/5/12 2:41p erickson
 * SW7425-2404: improve PES filtering, especially bounded pes packets and
 * error handling
 * 
 * 5   11/4/11 4:49p erickson
 * SW7425-1657: add const
 * 
 * 4   10/28/09 1:30p erickson
 * SW7405-3287: add functions for getting and setting pes headers
 *
 * 3   4/30/07 1:31p erickson
 * PR30310: move BTP modes to public file
 *
 * 2   4/30/07 1:09p erickson
 * PR30310: added TIMING_MARKER, PICTURE_TAG. changed debug code to
 * function.
 *
 * Irvine_BSEAVSW_Devel/3   6/30/06 9:39a erickson
 * PR21941: fix start code detection algorithm, and eliminate duplication
 * by moving to ts_utils
 *
 * Irvine_BSEAVSW_Devel/2   9/14/05 2:24p erickson
 * PR17148: converted to BCHP_7411_REV
 *
 * Irvine_BSEAVSW_Devel/1   9/1/05 3:37p erickson
 * PR16964: moved common ts functions to shared locations
 *
 ****************************************************************/
#ifndef COMMONTS_H__
#define COMMONTS_H__

#include "bstd.h"
#include "playertypes.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *b_btp_mode_str(int mode);

/* Byteswap LE unsigned long to BE */
#define be(L) \
    ((((L) & 0xFF) << 24) | (((L) & 0xFF00) << 8) | (((L) & 0xFF0000) >> 8) | (((L) & 0xFF000000) >> 24))

unsigned short b_get_pid(
    const unsigned char *pkt    /* 188 byte transport packet */
    );

bool b_is_btp(
    const unsigned char *pkt    /* 188 byte transport packet */
    );

int b_get_btp_word(
    const unsigned char *pkt,   /* 188 byte transport packet */
    int command_word            /* index 0..9 into BTP command word array */
    );

/* advance the sccount, looking for start codes. when
sccount gets to 3, the next byte is a start code. */
int b_check_for_start_code(unsigned char data, int sccount);

bool b_is_pes_stream_id(unsigned char stream_id);

typedef enum b_pes_packet_type {
    b_pes_packet_type_invalid,
    b_pes_packet_type_data, /* program_stream_map, private_stream_2, ECM, EMM, program_stream_directory,
                             DSMCC_stream, ITU-T Rec. H.222.1 type E stream */
    b_pes_packet_type_pes,
    b_pes_packet_type_padding
} b_pes_packet_type;

b_pes_packet_type b_get_pes_type(unsigned char stream_id);

typedef struct b_pes_header
{
    b_pes_packet_type pes_type;
    unsigned packet_length;
    unsigned header_data_length;
    unsigned pts_dts_flags;
    uint32_t pts;
    uint32_t dts;
} b_pes_header;

int b_get_pes_header(const unsigned char *buf, b_pes_header *pes_header);

void b_set_pes_header(unsigned char *buf, const b_pes_header *pes_header);


#ifdef __cplusplus
}
#endif

#endif /* COMMONTS_H__ */
