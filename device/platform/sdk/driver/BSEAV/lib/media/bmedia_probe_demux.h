/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_demux.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 4/20/12 2:27p $
 *
 * Module Description:
 *
 * Helper module to parse ES streams inside demux
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_demux.h $
 * 
 * 3   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 2   7/24/07 7:20p vsilyaev
 * PR 32813: Added MPEG-2 PES probe
 * 
 * 1   7/21/07 9:51p vsilyaev
 * PR 32813: Helper to write demux type of probes
 * 
 * 
 *******************************************************************************/
#ifndef __BMEDIA_PROBE_DEMUX_H__
#define __BMEDIA_PROBE_DEMUX_H__


#include "bmedia_probe_es.h"
#include "bhash.h"
#ifdef __cplusplus
extern "C"
{
#endif

struct bmedia_probe_demux_stream;

typedef struct bmedia_probe_demux {
	BHASH_HEAD(b_media_probe_hash, bmedia_probe_demux_stream, 8) hash;
	unsigned tracks;
	unsigned completed;
} bmedia_probe_demux;

void bmedia_probe_demux_init(bmedia_probe_demux *demux);
void bmedia_probe_demux_shutdown(bmedia_probe_demux *demux);
bmedia_probe_track *bmedia_probe_demux_data(bmedia_probe_demux *demux, batom_factory_t factory, unsigned track_id, bmedia_track_type track_type, batom_t data);
void bmedia_probe_demux_add_unknown(const bmedia_probe_demux *demux, bmedia_probe_stream *stream, bmedia_probe_track * (*allocate_track)(void));

#ifdef __cplusplus
}
#endif


#endif  /* __BMEDIA_PROBE_DEMUX_H__ */

