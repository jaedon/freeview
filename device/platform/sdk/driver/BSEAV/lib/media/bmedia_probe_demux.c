/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_demux.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 4/20/12 2:27p $
 *
 * Module Description:
 *
 * Helper module to parse ES streams inside demux
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_demux.c $
 * 
 * 7   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 6   10/18/07 2:03p jtna
 * PR35955: Coverity Defect ID:4005 RESOURCE_LEAK
 * 
 * 5   10/18/07 1:53p jtna
 * PR35954: Coverity Defect ID:4009 REVERSE_INULL
 * 
 * 4   7/25/07 6:28p vsilyaev
 * PR 32813: Added code to handle partially matches ES streams
 * 
 * 3   7/24/07 7:20p vsilyaev
 * PR 32813: Added MPEG-2 PES probe
 * 
 * 2   7/23/07 11:36a vsilyaev
 * PR 32813: Added MPEG-2 TS payload probe
 * 
 * 1   7/21/07 9:51p vsilyaev
 * PR 32813: Helper to write demux type of probes
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmedia_probe_demux.h"
#include "bkni.h"

BDBG_MODULE(bmedia_probe_demux);

typedef struct bmedia_probe_demux_stream {
	BHASH_ENTRY(bmedia_probe_demux_stream) hash_link;
	unsigned track_id;
	bmedia_probe_es_t probe;
	bool	done;
} bmedia_probe_demux_stream;

#define B_MEDIA_PROBE_KEY_COMPARE(id,substream) ((id)==(substream)->track_id)
#define B_MEDIA_PROBE_HASH(id) ((id)^((id)>>4))

BHASH_GENERATE(b_media_probe_hash, unsigned, bmedia_probe_demux_stream, hash_link, B_MEDIA_PROBE_HASH, B_MEDIA_PROBE_KEY_COMPARE)

void 
bmedia_probe_demux_init(bmedia_probe_demux *demux)
{
	BHASH_INIT(b_media_probe_hash, &demux->hash);
	demux->tracks = 0;
	demux->completed = 0;
	return;
}

void 
bmedia_probe_demux_shutdown(bmedia_probe_demux *demux)
{
	bmedia_probe_demux_stream *substream;
	BHASH_FOREACH_BEGIN(b_media_probe_hash,&demux->hash,bmedia_probe_demux_stream,substream,hash_link)
		if(substream->probe) {
			BDBG_MSG(("bmedia_probe_demux_shutdown: %#lx destroing es probe %#lx for id:%#x", (unsigned long)demux, substream->probe, substream->track_id));
			bmedia_probe_es_destroy(substream->probe);
		}
		BHASH_REMOVE(b_media_probe_hash, &demux->hash, substream->track_id);
		BKNI_Free(substream);
	BHASH_FOREACH_END()
	return;
}

bmedia_probe_track *
bmedia_probe_demux_data(bmedia_probe_demux *demux, batom_factory_t factory, unsigned track_id, bmedia_track_type track_type, batom_t data)
{
	bmedia_probe_demux_stream *substream;
	bmedia_probe_track *track=NULL;

	substream = BHASH_FIND(b_media_probe_hash, &demux->hash, track_id);
	if(!substream) {
		substream = BKNI_Malloc(sizeof(*substream));
		if(!substream) {
			BDBG_ERR(("bmedia_probe_demux_data: %#lx can't allocate %u bytes", (unsigned long)demux, sizeof(*substream)));
			goto done;
		}
		substream->track_id = track_id;
		substream->done = false;
		substream->probe = bmedia_probe_es_create(factory);
		BDBG_MSG(("bmedia_probe_demux_data: %#lx creating es probe %#lx for id:%#x", (unsigned long)demux, substream->probe, track_id));
		if(!substream->probe) {
			BKNI_Free(substream);
			goto done;
		}
		if(track_type!=bmedia_track_type_other) {
			bmedia_probe_es_filter_type(substream->probe, track_type);
		}
		BHASH_ADD(b_media_probe_hash,&demux->hash, track_id, substream);
		demux->tracks++;
	}
	BDBG_ASSERT(substream); BDBG_ASSERT(substream->probe);
	if(!substream->done) {
		unsigned nactive;
		track = bmedia_probe_es_feed(substream->probe, data, &nactive);
		if(nactive==0 && !track) {
			track = BKNI_Malloc(sizeof(*track));
			if(!track) {
				BDBG_ERR(("bmedia_probe_demux_data: %#lx can't allocate %u bytes", (unsigned long)demux, sizeof(*track)));
				goto done;
			}
			bmedia_probe_track_init(track);
			track->type = bmedia_track_type_other;
		}
		if(track) {
			track->number = track_id;
			substream->done = true;
			demux->completed++;
			BDBG_MSG(("bmedia_probe_demux_data: %#lx finished parsing for id:%#x track %#lx (%u:%u)", (unsigned long)demux, track_id, (unsigned long)track, demux->completed, demux->tracks));
		}
	} else {
		batom_release(data);
	}

done:
	return track;
}

void 
bmedia_probe_demux_add_unknown(const bmedia_probe_demux *demux, bmedia_probe_stream *stream, bmedia_probe_track * (*allocate_track)(void))
{
	bmedia_probe_demux_stream *substream;
	bmedia_probe_track *track;

	BHASH_FOREACH_BEGIN(b_media_probe_hash,&demux->hash,bmedia_probe_demux_stream,substream,hash_link) 
		if(substream->done) {
			continue;
		}
		track = bmedia_probe_es_last(substream->probe);
		if(track==NULL) {
            if(allocate_track) {
                track = allocate_track();
            }  else {
			    track = BKNI_Malloc(sizeof(*track));
            }
			if(!track) {
				BDBG_ERR(("bmedia_probe_demux_add_unknown: %#lx can't allocate %u bytes", (unsigned long)demux, sizeof(*track)));
				break;
			}
			bmedia_probe_track_init(track);
			track->type = bmedia_track_type_other;
		}
		track->number = substream->track_id;
		bmedia_probe_add_track(stream, track);
	BHASH_FOREACH_END()
	return;
}

