/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2ts_probe.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 4/23/10 2:12p $
 *
 * Module Description:
 *
 * MPEG-2 TS Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2ts_probe.h $
 * 
 * 6   4/23/10 2:12p vsilyaev
 * SWBLURAY-19982: Improved resyncing in the MPEG-2 TS stream
 * 
 * fix_bdvd_v3.0_issue19982/1   4/23/10 4:42p ialauder
 * Improve code to find TS sync bytes
 * 
 * 5   11/11/08 9:49p vsilyaev
 * PR 48182: Added code to detect stream duration, using parsed timestamps
 * 
 * 4   9/3/08 6:14p vsilyaev
 * PR 44493: Extended list of extension, use 'const' modifier for the
 * extension list
 * 
 * 3   7/21/08 1:18p vsilyaev
 * PR44493: Add support for 192 byte TS packets
 * 
 * PR44493/1   7/18/08 4:53p jtna
 * PR44493: Add support for 192 byte TS packets
 * 
 * 2   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 1   7/23/07 11:33a vsilyaev
 * PR 32813: MPEG2-TS payload probe module
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG2TS_PROBE_H__
#define __BMPEG2TS_PROBE_H__


#include "bmedia_probe_impl.h"
#ifdef __cplusplus
extern "C"
{
#endif

bool bmpeg2ts_probe_header_match(batom_cursor *cursor);
bool bmpeg2ts192_probe_header_match(batom_cursor *cursor);

extern const bmedia_probe_file_ext bmpeg2ts_probe_ext[];
extern const bmedia_probe_file_ext bmpeg2ts192_probe_ext[];
extern const bmedia_probe_format_desc bmpeg2ts_probe;
extern const bmedia_probe_format_desc bmpeg2ts192_probe;

typedef struct bmpeg2ts_probe_stream {
    bmedia_probe_stream media;
    unsigned pkt_len;
} bmpeg2ts_probe_stream;

bmedia_timestamp_parser_t bmpeg2ts_pcr_parser_create(uint16_t pid, size_t packet_len);

#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG2TS_PROBE_H__ */

