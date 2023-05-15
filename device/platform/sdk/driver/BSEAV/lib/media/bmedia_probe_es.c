/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_es.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 12/10/10 1:53p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_es.c $
 * 
 * 17   12/10/10 1:53p gmohile
 * SW35230-2517 : Initial version for AVS probe
 * 
 * 16   12/22/09 1:20p vsilyaev
 * SW7405-3646: replaced bmedia_probe_es_scan_scode with
 * bmedia_video_scan_scode
 * 
 * 15   9/29/09 2:46p vsilyaev
 * SWDEPRECATED-3624: Added AAC LOAS probe
 * 
 * 14   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 13   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/1   2/20/09 5:49p gooslin
 * Add probe for a start code
 * 
 * 12   1/29/09 7:23p vsilyaev
 * PR 32813: Fixed free of NULL pointer
 * 
 * 11   1/22/09 1:34p vsilyaev
 * PR 51351: Added more debug output
 * 
 * 10   11/19/08 9:56a gmohile
 * PR 48852 : Add DTS probe for ES layer
 * 
 * 9   10/10/08 7:21p jtna
 * PR45520: BMedia support for H.264 (AVC) video
 * 
 * 8   3/17/08 2:13p jtna
 * PR 37079: Coverity Defect ID:5041 USE_AFTER_FREE
 * 
 * 7   3/4/08 12:23p vsilyaev
 * PR 39818: Fixed use of unitialized variable
 * 
 * 6   10/17/07 5:48p vishk
 * PR 35957: Coverity Defect ID:3960 DEADCODE
 * 
 * 5   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 4   7/23/07 7:14p vsilyaev
 * PR 32813: Added AAC ADTS probe
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
#include "bstd.h"
#include "bmedia_probe_es.h"
#include "bac3_probe.h"
#include "bdts_probe.h"
#include "bmpeg_audio_probe.h"
#include "bmpeg_video_probe.h"
#include "bavc_video_probe.h"
#include "bavs_video_probe.h"
#include "baac_adts_probe.h"
#include "baac_loas_probe.h"
#include "bkni.h"

BDBG_MODULE(bmedia_probe_es);

BDBG_OBJECT_ID(bmedia_probe_es_t);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

static const bmedia_probe_es_desc *b_media_probe_es[] = {
	&bac3_probe, 
	&bdts_probe,
	&baac_adts_probe, 
	&baac_loas_probe, 
	&bmpeg_audio_probe,
	&bmpeg_video_probe,
	&bavc_video_probe,
	&bavs_video_probe
};

#define B_MEDIA_N_ES_PROBES ((sizeof(b_media_probe_es)/sizeof(*b_media_probe_es)))

struct bmedia_probe_es {
	BDBG_OBJECT(bmedia_probe_es_t)
	batom_factory_t factory;
	size_t data_len;
	bmedia_probe_base_es_t probes[B_MEDIA_N_ES_PROBES];
	bool mask[B_MEDIA_N_ES_PROBES];
};

bmedia_probe_es_t 
bmedia_probe_es_create(batom_factory_t factory)
{
	bmedia_probe_es_t  probe;
	unsigned i;

	probe = BKNI_Malloc(sizeof(*probe));
    BDBG_MSG_TRACE(("bmedia_probe_es_create: %#lx", (unsigned long)probe));
	if(!probe) {
		BDBG_ERR(("bmedia_probe_es_create: can't allocate %u bytes", sizeof(*probe)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(probe, bmedia_probe_es_t);
	probe->factory = factory;
	probe->data_len = 0;
	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		probe->probes[i] = NULL;
		probe->mask[i] = true;
	}
	return probe;
err_alloc:
	return NULL;
}

void
bmedia_probe_es_destroy(bmedia_probe_es_t probe)
{
	unsigned i;

    BDBG_MSG_TRACE(("bmedia_probe_es_destroy: %#lx", (unsigned long)probe));

	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);

	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		if(probe->probes[i]) {
			b_media_probe_es[i]->destroy(probe->probes[i]);
		}
	}
	BDBG_OBJECT_DESTROY(probe, bmedia_probe_es_t);
	BKNI_Free(probe);
	return;
}

void
bmedia_probe_es_reset(bmedia_probe_es_t probe)
{
	unsigned i;

    BDBG_MSG_TRACE(("bmedia_probe_es_reset: %#lx", (unsigned long)probe));
	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);
	probe->data_len = 0;
	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		if(probe->probes[i]) {
			b_media_probe_es[i]->reset(probe->probes[i]);
		}
		probe->mask[i] = true;
	}
	return;
}

void
bmedia_probe_es_filter_ext(bmedia_probe_es_t probe, const char *ext)
{
	unsigned i;

    BDBG_MSG_TRACE(("bmedia_probe_es_filter_ext: %#lx", (unsigned long)probe));
	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);
	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		if(!probe->mask[i]) {
			continue;
		}
		probe->mask[i] = bmedia_probe_match_ext(b_media_probe_es[i]->ext_list, ext);
	}
	return;
}

void 
bmedia_probe_es_filter_type(bmedia_probe_es_t probe, bmedia_track_type track_type)
{
	unsigned i;

    BDBG_MSG_TRACE(("bmedia_probe_es_filter_type: %#lx", (unsigned long)probe));
	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);
	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		if(!probe->mask[i]) {
			continue;
		}
		probe->mask[i] = (b_media_probe_es[i]->type==track_type);
	}
	return;
}

bmedia_probe_track *
bmedia_probe_es_feed(bmedia_probe_es_t probe, batom_t atom, unsigned *nactive)
{
	unsigned i, active;
	bmedia_probe_track *track=NULL;

    BDBG_MSG_TRACE(("bmedia_probe_es_feed: %#lx", (unsigned long)probe));
	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);
	probe->data_len += batom_len(atom);
	for(active=0,i=0;i<B_MEDIA_N_ES_PROBES;i++) {
		bool done;

		if(!probe->mask[i]) {
			continue;
		}
		if(!probe->probes[i]) {
			/* create  elementary probes on demand */
			probe->probes[i]=b_media_probe_es[i]->create(probe->factory);
			if(!probe->probes[i]) {
				probe->mask[i] = false;
				continue;
			}
		}
		active++;
		batom_lock(atom);
		done = false;
		track = b_media_probe_es[i]->feed(probe->probes[i], atom, &done);
		batom_release(atom);
		if(track == NULL && !done && probe->data_len>b_media_probe_es[i]->max_parsing_size) {
			unsigned probability=0;
			done = true;
			track = b_media_probe_es[i]->last(probe->probes[i], &probability);
			if(track && probability<50) {
				BKNI_Free(track);
				track = NULL;
			}
		}
		if(track) {
			probe->mask[i] = false;
			break;
		}
		if(done) {
			b_media_probe_es[i]->reset(probe->probes[i]);
			probe->mask[i] = false;
		}
	}
	batom_release(atom);
	*nactive = active;
	return track;
}

bmedia_probe_track *
bmedia_probe_es_last(bmedia_probe_es_t probe)
{
	unsigned i;
	unsigned probability;
	unsigned last_probability=0;
	bmedia_probe_track *last_track=NULL;
	bmedia_probe_track *track = NULL;

    BDBG_MSG_TRACE(("bmedia_probe_es_last: %#lx", (unsigned long)probe));

	BDBG_OBJECT_ASSERT(probe, bmedia_probe_es_t);
	for(i=0;i<B_MEDIA_N_ES_PROBES;i++) {

		if(!probe->probes[i]) {
			continue;
		}
		track = b_media_probe_es[i]->last(probe->probes[i], &probability);
		if(!track) {
			continue;
		}
		else if(last_track == NULL) {
			last_track = track;
			last_probability=probability;
		}
		else {
			if(probability<last_probability) {
				BKNI_Free(track);
			} else {
				BKNI_Free(last_track);
				last_track = track;
				last_probability = probability;
			}
		}
	}
	return last_track;
}



bmedia_probe_track *
bmedia_probe_es_nolast(bmedia_probe_base_es_t probe, unsigned *probability)
{
	BSTD_UNUSED(probe);
    BDBG_MSG_TRACE(("bmedia_probe_es_nolast: %#lx", (unsigned long)probe));
	*probability = 0;
	return NULL;
}

