/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_es.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 12/22/09 1:20p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_es.h $
 * 
 * 9   12/22/09 1:20p vsilyaev
 * SW7405-3646: replaced bmedia_probe_es_scan_scode with
 * bmedia_video_scan_scode
 * 
 * 8   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 7   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/1   2/20/09 5:49p gooslin
 * Add probe for a start code
 * 
 * 6   10/28/08 9:12a vsilyaev
 * PR 45520: Fixed warnings
 * 
 * 5   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 4   7/25/07 1:51p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 3   7/21/07 9:53p vsilyaev
 * PR 32813: Added MPEG-1 System probe
 * 
 * 2   7/20/07 11:57p vsilyaev
 * PR 32813: Added MPEG video probe
 * 
 * 1   7/20/07 12:52p vsilyaev
 * PR 32813: Added elementary stream probes
 * 
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PROBE_ES_H__
#define _BMEDIA_PROBE_ES_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmedia_probe_es *bmedia_probe_es_t;

typedef struct bmedia_probe_base_es *bmedia_probe_base_es_t;

typedef struct bmedia_probe_es_desc {
	bmedia_track_type type;
	union {
		baudio_format audio;
		bvideo_codec video;
	} codec;
	const bmedia_probe_file_ext *ext_list; /* list of extensions */
	size_t max_parsing_size;
	bmedia_probe_base_es_t (*create)(batom_factory_t factory);
	void (*destroy)(bmedia_probe_base_es_t probe);
	bmedia_probe_track *(*feed)(bmedia_probe_base_es_t probe, batom_t atom, bool *done);
	void (*reset)(bmedia_probe_base_es_t probe);
	bmedia_probe_track *(*last)(bmedia_probe_base_es_t probe, unsigned *probability);
} bmedia_probe_es_desc;

bmedia_probe_es_t bmedia_probe_es_create(batom_factory_t factory);
void bmedia_probe_es_destroy(bmedia_probe_es_t probe);
bmedia_probe_track * bmedia_probe_es_feed(bmedia_probe_es_t probe, batom_t atom, unsigned *nactive);
bmedia_probe_track * bmedia_probe_es_last(bmedia_probe_es_t probe);
void bmedia_probe_es_filter_ext(bmedia_probe_es_t probe, const char *ext);
void bmedia_probe_es_filter_type(bmedia_probe_es_t probe, bmedia_track_type track_type);
void bmedia_probe_es_reset(bmedia_probe_es_t probe);
bmedia_probe_track *bmedia_probe_es_nolast(bmedia_probe_base_es_t probe, unsigned *probability);
#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PROBE_ES_H__ */

