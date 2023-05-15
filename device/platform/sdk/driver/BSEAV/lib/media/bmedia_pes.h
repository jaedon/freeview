/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_pes.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 6/10/09 12:44p $
 *
 * Module Description:
 *
 * BMedia library, PES producer
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_pes.h $
 * 
 * 12   6/10/09 12:44p vsilyaev
 * PR 55896: Merged changes for extended ASF codec support
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/1   6/5/09 4:44p gooslin
 * Use meta_header flag to indicate a meta data packet
 * 
 * 11   5/15/09 12:48p vsilyaev
 * PR 55193: Added function to return last parsed PTS by the media library
 * 
 * 10   3/16/09 6:29p vsilyaev
 * PR 53281: Make option to sent PTS once per stream and stream type
 * agnostic
 * 
 * 9   3/5/09 7:02p vsilyaev
 * PR 52434: Unify location of frame start codes
 * 
 * 8   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * fix_bdvd_v1.42_pr11402/1   11/1/08 7:46p gooslin
 * Cleanup A/V sync modifications
 * 
 * 7   3/3/08 12:18p vsilyaev
 * PR 39818: Added utility functions to construct PES header and AAC ADTS
 * header
 * 
 * 6   12/4/07 9:43a gmohile
 * PR 30491: Send DTS entry with value 0xffffffff to signal decoder of EOS
 * entry
 * 
 * 5   7/9/07 3:46p vsilyaev
 * PR 32846: Added ASF support
 * 
 * 4   5/25/07 10:03a vsilyaev
 * PR 29815: Added missing extern keyword
 * 
 * 3   5/21/07 6:03p vsilyaev
 * PR 29815: Use common code to handle insertion of EOS and filling data
 * 
 * 2   5/1/07 3:23p vsilyaev
 * PR 28631: Integrated support for MP4 container
 * 
 * 1   3/27/07 6:21p vsilyaev
 * PR 29125: PES producer
 * 
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PES_H__
#define _BMEDIA_PES_H__

#include "balloc.h"
#include "bioatom.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define B_MEDIA_PACKET_NO_HEADER	(0x7F)
#define B_MEDIA_PACKET_EOS			(0xFE)
#define B_MEDIA_PACKET_FLAG_EOS		(0x80)
#define BMEDIA_PACKET_HEADER_INIT(hdr) { (hdr)->pts_valid=false;(hdr)->key_frame=false;(hdr)->meta_header=false;(hdr)->header_type=B_MEDIA_PACKET_NO_HEADER;(hdr)->header_len=0;(hdr)->header_off=0;} while(0)

typedef struct bmedia_packet_header {
	uint32_t pts; /* PTS of the packet, in 45KHz */
	uint8_t header_type; /* static header that added before payload */
	uint8_t header_len; /* length of the variable length header, that stored past bmedia_packet_header */
	uint8_t header_off; /* offset into the static header, where variable header shall be placed */
	bool pts_valid; /* validator for PTS value */
	bool key_frame; /* set to true if this is a key frame */
    bool meta_header; /* set to true if this packet is required for decoding of following frames */
} bmedia_packet_header;

typedef struct bmedia_pes *bmedia_pes_t;
typedef struct bmedia_pes_stream_cfg {
	size_t nvecs;
	const batom_vec *const*vecs;
    bool single_pts;
} bmedia_pes_stream_cfg;

typedef struct bmedia_pes_cfg {
	size_t pes_hdr_size;
	size_t eos_len;
	const void *eos_data;
} bmedia_pes_cfg;

typedef struct bmedia_pes_status {
    bool last_pts_valid;
    uint32_t last_pts;
} bmedia_pes_status;

void bmedia_pes_default_cfg(bmedia_pes_cfg *cfg);
void bmedia_pes_default_stream_cfg(bmedia_pes_stream_cfg *cfg);
bmedia_pes_t bmedia_pes_create(batom_factory_t factory, balloc_iface_t alloc, const bmedia_pes_cfg *cfg);
void bmedia_pes_destroy(bmedia_pes_t  stream);
bool bmedia_pes_feed(bmedia_pes_t stream, batom_pipe_t pipe);
void bmedia_pes_start(bmedia_pes_t stream, batom_pipe_t pipe, const bmedia_pes_stream_cfg *cfg, uint8_t pes_id);
void bmedia_pes_get_stream_cfg(bmedia_pes_t stream, bmedia_pes_stream_cfg *cfg);
void bmedia_pes_set_stream_cfg(bmedia_pes_t stream, const bmedia_pes_stream_cfg *cfg);
void bmedia_pes_stop(bmedia_pes_t stream);
void bmedia_pes_reset(bmedia_pes_t stream);
void bmedia_pes_get_status(bmedia_pes_t stream, bmedia_pes_status *status);

extern const batom_user bmedia_atom[1];


#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PES_H__ */

