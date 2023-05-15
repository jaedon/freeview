/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_stream.h $
 * $brcm_Revision: 18 $
 * $brcm_Date: 1/21/10 5:56p $
 *
 * Module Description:
 *
 * AVI parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_stream.h $
 * 
 * 18   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 17   10/16/09 1:10p vsilyaev
 * SW7405-3217: VC1 support in AVI streams
 * 
 * fix_bdvd_v2.0_pr16643/2   10/15/09 4:30p gooslin
 * Lower memory requirment for vc1 sequence header
 * 
 * fix_bdvd_v2.0_pr16643/1   10/14/09 3:41p gooslin
 * Add VC1 support to AVI
 * 
 * 16   3/16/09 6:29p vsilyaev
 * PR 53281: Make option to sent PTS once per stream and stream type
 * agnostic
 * 
 * 15   3/16/09 5:44p vsilyaev
 * PR 53281: Added option to send PTS only once
 * 
 * fix_bdvd_v2.0_pr13666/1   3/15/09 3:17p gooslin
 * Allow sending only the first PTS for AVC streams
 * 
 * 14   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:42a gooslin
 * Move FourCC defines to bmedia scope
 * 
 * 13   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:44p gooslin
 * Cleanup A/V sync modifications
 * 
 * 12   3/6/08 11:23a gmohile
 * PR 38347 : Set seperate timestamp offset for audio and video
 * 
 * 11   5/21/07 6:04p vsilyaev
 * PR 29815: Use common code to handle insertion of EOS and filling data
 * 
 * 10   4/25/07 6:31p vsilyaev
 * PR 29815: Added code to insert EOS sequence at end of the PES stream
 * 
 * 9   4/24/07 4:42p vsilyaev
 * PR 29921: Added timestamp scaler
 * 
 * 8   3/1/07 7:08p vsilyaev
 * PR 25701: Added flush function
 * 
 * 7   2/15/07 3:11p vsilyaev
 * PR 25701: Added code to handle parsing during resource starvation
 * 
 * 6   2/13/07 7:02p vsilyaev
 * PR 25701: Added support for DivX3.11
 * 
 * 5   2/12/07 6:14p vsilyaev
 * PR 25701: Fixed timestamps for audio
 * 
 * 4   2/12/07 2:52p vsilyaev
 * PR 25701: Added audio support
 * 
 * 3   2/9/07 1:10p vsilyaev
 * PR 25701: Added stream and format specific hanlder
 * 
 * 2   2/9/07 11:16a vsilyaev
 * PR 25701: Renamed bavi_t to bavi_parser_t
 * 
 * 1   2/8/07 11:44p vsilyaev
 * PR 25701: bavi_stream parser
 * 
 * 
 *******************************************************************************/

#ifndef _BAVI_STREAM_H___
#define _BAVI_STREAM_H___

#include "bavi_parser.h"
#include "bmedia_util.h"
#include "bmedia_pes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum bavi_stream_type {
	bavi_stream_type_unknown=0,
	bavi_stream_type_video, 
	bavi_stream_type_audio, 
	bavi_stream_type_invalid
} bavi_stream_type;

typedef struct bavi_stream_config {
    bool reorder_timestamps;
} bavi_stream_config;

typedef struct bavi_stream_status {
	bavi_streamheader header;
	bavi_stream_type stream_type;
	union {
		bmedia_waveformatex audio;
		bmedia_bitmapinfo video;
	} stream_info;

	uint8_t seq_header[256];
	unsigned seq_header_len;
	union {
		bmedia_vc1sm_info vc1_sm;
		bmedia_vc1ap_info vc1_ap;
	} seq_hdr;
} bavi_stream_status;

typedef struct bavi_file_status {
	bool header_valid;
	bavi_mainheader mainheader;
} bavi_file_status;

typedef struct bavi_stream *bavi_stream_t;
typedef struct bavi_demux  *bavi_demux_t;

#define BAVI_STREAMD_ID_RAW		1
#define BAVI_STREAMD_ID_ES		0

typedef struct bavi_demux_cfg {
	void *user_cntx;
	unsigned pes_hdr_size;
	unsigned max_streams;
	unsigned preallocated_streams;
} bavi_demux_cfg;

void bavi_demux_default_cfg(bavi_demux_cfg *cfg);
bavi_demux_t bavi_demux_create(bavi_parser_t parser, batom_factory_t factory, balloc_iface_t alloc, const bavi_demux_cfg *cfg);
void bavi_demux_reset(bavi_demux_t demux);
void bavi_demux_flush(bavi_demux_t demux);
void bavi_demux_destroy(bavi_demux_t demux);
bool bavi_demux_step(bavi_demux_t demux);
void bavi_demux_set_offset(bavi_stream_t stream, uint32_t timestamp);
void bavi_demux_set_timescale(bavi_demux_t demux, bmedia_time_scale time_scale);
void bavi_demux_movi_end(bavi_demux_t demux);
void bavi_demux_default_stream_cfg(bavi_stream_config *stream_config);

bavi_stream_t bavi_demux_get_stream(bavi_demux_t demux, unsigned n);
int bavi_stream_activate(bavi_stream_t stream, batom_pipe_t pipe, const bavi_stream_config *stream_config);
void bavi_stream_get_stream_cfg(bavi_stream_t stream, bmedia_pes_stream_cfg *cfg);
void bavi_stream_deactivate(bavi_stream_t stream);
void bavi_stream_get_status(bavi_stream_t stream, bavi_stream_status *status);

#ifdef __cplusplus
}
#endif

#endif /* _BAVI_STREAM_H__ */

