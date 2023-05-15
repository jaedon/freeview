/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_pvr_util.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/23/08 10:20a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_pvr_util.c $
 * 
 * 3   9/23/08 10:20a jrubio
 * PR46925: fix typo
 * 
 * 2   9/15/08 5:36p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 * 
 * 1   6/11/08 7:20p ssood
 * PR42739: added virtual interface back to the playback module to allow
 * ip implementation in addition to the regular playback implementation
 * 
 * File is copied over from legacy Settop API implementation (BSEAV/api/src/pvr/bsettop_pvr_util.c).
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(pvr_util);

void 
bplayback_params_init(bplayback_params *params, bplayback_t playback) 
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(playback);
	BDBG_ASSERT(playback->iface.params_init);
	playback->iface.params_init(params, playback);
#else
    BSTD_UNUSED(params);
    BSTD_UNUSED(playback);
#endif
                                                 
	return;
}

#if NEXUS_HAS_PLAYBACK
static void 
bplayback_params_init_dummy(bplayback_params *params, bplayback_t playback) 
{
	BSTD_UNUSED(playback);
	BKNI_Memset(params, 0, sizeof(*params));
	return ;
}
#endif

bstream_t 
bplayback_start(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source, const bplayback_params *params)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(playback);
	BDBG_ASSERT(playback->iface.start);
	return playback->iface.start(playback, playpump, mpeg, source, params);
#else
    BSTD_UNUSED(playpump);
    BSTD_UNUSED(playback);
    BSTD_UNUSED(mpeg);
    BSTD_UNUSED(source);
    BSTD_UNUSED(params);
    return NULL;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bstream_t 
bplayback_start_dummy(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source, const bplayback_params *params)
{
	BSTD_UNUSED(playback);
	BSTD_UNUSED(playpump);
	BSTD_UNUSED(mpeg);
	BSTD_UNUSED(source);
	BSTD_UNUSED(params);

	BSETTOP_ERROR(berr_not_supported);
	return NULL;
}
#endif

bstream_t 
bplayback_start_timeshifting(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source,  const bplayback_params *params, brecord_t record )
{
#if NEXUS_HAS_PLAYBACK
	return playback->iface.start_timeshifting(playback, playpump, mpeg, source, params, record);
#else
    BSTD_UNUSED(playpump);
    BSTD_UNUSED(playback);
    BSTD_UNUSED(source);
    BSTD_UNUSED(params);
    BSTD_UNUSED(record);
    BSTD_UNUSED(mpeg);
    return NULL;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bstream_t 
bplayback_start_timeshifting_dummy(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source,  const bplayback_params *params, brecord_t record )
{
	BSTD_UNUSED(playback);
	BSTD_UNUSED(playpump);
	BSTD_UNUSED(mpeg);
	BSTD_UNUSED(source);
	BSTD_UNUSED(params);
	BSTD_UNUSED(record);

	BSETTOP_ERROR(berr_not_supported);
	return NULL;
}
#endif

bresult 
bplayback_stop_timeshifting(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(playback);
	BDBG_ASSERT(playback->iface.stop_timeshifting);
	return playback->iface.stop_timeshifting(playback);
#else
    BSTD_UNUSED(playback);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_stop_timeshifting_dummy(bplayback_t playback)
{
	BSTD_UNUSED(playback);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_stop(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(playback);
	BDBG_ASSERT(playback->iface.stop);
	return playback->iface.stop(playback);
#else
    BSTD_UNUSED(playback);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_stop_dummy(bplayback_t playback)
{
	BSTD_UNUSED(playback);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_get_status(bplayback_t p, bplayback_status *status)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.get_status);
	return p->iface.get_status(p, status);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(status);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_get_status_dummy(bplayback_t p, bplayback_status *status)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(status);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_pause(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.pause);
	return p->iface.pause(p);
#else
    BSTD_UNUSED(p);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_pause_dummy(bplayback_t p)
{
	BSTD_UNUSED(p);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_catchup_record(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.catchup_record);
	return p->iface.catchup_record(p);
#else
    BSTD_UNUSED(p);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_catchup_record_dummy(bplayback_t p)
{
	BSTD_UNUSED(p);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_play(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.play);
	return p->iface.play(p);
#else
    BSTD_UNUSED(p);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_play_dummy(bplayback_t p)
{
	BSTD_UNUSED(p);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_frameadvance(bplayback_t p, bplayback_player_mode playermode,	bool forward)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.frameadvance);
	return p->iface.frameadvance(p, playermode, forward);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(playermode);
    BSTD_UNUSED(forward);
    return berr_not_available;
#endif
}

bresult 
bplayback_frameadvance_dummy(bplayback_t p, bplayback_player_mode playermode,	bool forward)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(playermode);
	BSTD_UNUSED(forward);
	return BSETTOP_ERROR(berr_not_supported);
}

bresult 
bplayback_trickmode(bplayback_t p, const bplayback_trickmode_params *trickmode)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.trickmode);
	return p->iface.trickmode(p, trickmode);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(trickmode);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_trickmode_dummy(bplayback_t p, const bplayback_trickmode_params *trickmode)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(trickmode);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

bresult 
bplayback_get_trickmode_by_rate(bplayback_t p, int rate, bplayback_trickmode_params *trickmode)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.get_trickmode_by_rate);
	return p->iface.get_trickmode_by_rate(p, rate, trickmode);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(rate);
    BSTD_UNUSED(trickmode);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static bresult 
bplayback_get_trickmode_by_rate_dummy(bplayback_t p, int rate, bplayback_trickmode_params *trickmode)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(trickmode);
	BSTD_UNUSED(rate);
	return BSETTOP_ERROR(berr_not_supported);
}
#endif

long 
bplayback_goto_index(bplayback_t p, long index_offset, int whence)
{
#if NEXUS_HAS_PLAYBACK
    BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.goto_index);
	return p->iface.goto_index(p, index_offset, whence);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(index_offset);
    BSTD_UNUSED(whence);
    return 0;
#endif
}

#if NEXUS_HAS_PLAYBACK
static long 
bplayback_goto_index_dummy(bplayback_t p, long index_offset, int whence)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(index_offset);
	BSTD_UNUSED(whence);
	BSETTOP_ERROR(berr_not_supported);
	return -1;
}
#endif

long 
bplayback_goto_timestamp(bplayback_t p, long timestamp, int whence)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.goto_timestamp);
	return p->iface.goto_timestamp(p, timestamp, whence);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(timestamp);
    BSTD_UNUSED(whence);
    return berr_not_available;
#endif
}

#if NEXUS_HAS_PLAYBACK
static long 
bplayback_goto_timestamp_dummy(bplayback_t p, long timestamp, int whence)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(timestamp);
	BSTD_UNUSED(whence);
	BSETTOP_ERROR(berr_not_supported);
	return -1;
}
#endif

off_t 
bplayback_goto_mpeg_file_offset(bplayback_t p, off_t mpeg_file_offset, int whence)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(p);
	BDBG_ASSERT(p->iface.goto_mpeg_file_offset);
	return p->iface.goto_mpeg_file_offset(p, mpeg_file_offset, whence);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(mpeg_file_offset);
    BSTD_UNUSED(whence);
    return 0;
#endif
}

#if NEXUS_HAS_PLAYBACK
static off_t 
bplayback_goto_mpeg_file_offset_dummy(bplayback_t p, off_t mpeg_file_offset, int whence)
{
	BSTD_UNUSED(p);
	BSTD_UNUSED(mpeg_file_offset);
	BSTD_UNUSED(whence);
	BSETTOP_ERROR(berr_not_supported);
	return -1;
}
#endif

void
bplayback_close(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
	BDBG_ASSERT(playback);
	BDBG_ASSERT(playback->iface.close);
	playback->iface.close(playback);
	return;
#else
    BSTD_UNUSED(playback);
    return;
#endif
}

void
bplayback_close_dummy(bplayback_t playback)
{
	BSTD_UNUSED(playback);
	return;
}

#if NEXUS_HAS_PLAYBACK
static const bplayback_iface dummy_iface = {
	bplayback_params_init_dummy,
	bplayback_start_dummy,
	bplayback_start_timeshifting_dummy,
	bplayback_stop_timeshifting_dummy,
	bplayback_stop_dummy,
	bplayback_get_status_dummy,
	bplayback_pause_dummy,
	bplayback_catchup_record_dummy,
	bplayback_play_dummy,
	bplayback_frameadvance_dummy,
	bplayback_trickmode_dummy,
	bplayback_get_trickmode_by_rate_dummy,
	bplayback_goto_index_dummy,
	bplayback_goto_timestamp_dummy,
	bplayback_goto_mpeg_file_offset_dummy,
	bplayback_close_dummy
};

void 
bplayback_iface_init(bplayback_iface *iface)
{
	*iface = dummy_iface;
    return;

}
#endif

#if 0
void 
brecord_params_init(brecord_params *params, brecord_t record) 
{
	BDBG_ASSERT(record);
	BDBG_ASSERT(record->params_init);
	record->params_init(params, record);
	return;
}

static void 
brecord_params_init_dummy(brecord_params *params, brecord_t record) 
{
	BSTD_UNUSED(record);
	BKNI_Memset(params, 0, sizeof(*params));
	return ;
}

bresult 
brecord_start(brecord_t record, brecpump_t recpump, bstream_t stream, brecord_file_t source, const brecord_params *params)
{
	BDBG_ASSERT(record);
	BDBG_ASSERT(record->start);
	return record->start(record, recpump, stream, source, params);
}

static bresult 
brecord_start_dummy(brecord_t record, brecpump_t recpump, bstream_t stream, brecord_file_t source, const brecord_params *params)
{
	BSTD_UNUSED(record);
	BSTD_UNUSED(recpump);
	BSTD_UNUSED(stream);
	BSTD_UNUSED(source);
	BSTD_UNUSED(params);
	return BSETTOP_ERROR(berr_not_supported);
}

bresult 
brecord_stop(brecord_t record)
{
	BDBG_ASSERT(record);
	BDBG_ASSERT(record->stop);
	return record->stop(record);
}

static bresult 
brecord_stop_dummy(brecord_t record)
{
	BSTD_UNUSED(record);
	return BSETTOP_ERROR(berr_not_supported);
}

void 
brecord_get_status(brecord_t r, brecord_status *status)
{
	BDBG_ASSERT(r);
	BDBG_ASSERT(r->get_status);
	r->get_status(r, status);
	return;
}

static void 
brecord_get_status_dummy(brecord_t r, brecord_status *status)
{
	BSTD_UNUSED(r);
	BSTD_UNUSED(status);
	return;
}

void
brecord_close(brecord_t record)
{
	BDBG_ASSERT(record);
	BDBG_ASSERT(record->close);
	record->close(record);
	return;
}

void
brecord_close_dummy(brecord_t record)
{
	BSTD_UNUSED(record);
	return;
}

static const brecord_iface record_dummy_iface = {
	brecord_params_init_dummy,
	brecord_close_dummy,
	brecord_start_dummy,
	brecord_stop_dummy,
	brecord_get_status_dummy,

};

void 
brecord_iface_init(brecord_iface *iface)
{
	*iface = record_dummy_iface;
	return;
}

static const char b_playback_file[] = "b_playback_file";

static ssize_t 
b_no_read(bfile_io_read_t fd, void *buf, size_t length)
{
	BSTD_UNUSED(fd);
	BSTD_UNUSED(buf);
	BSTD_UNUSED(length);
	return -1;
}

static off_t 
b_no_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	BSTD_UNUSED(fd);
	BSTD_UNUSED(offset);
	BSTD_UNUSED(whence);
	return -1;
}

static int 
b_no_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	BSTD_UNUSED(fd);
	*first = *last = 0;
	return -1;
}

static void
b_no_close(bplayback_file_t file)
{
	BSTD_UNUSED(file);
	return;
}

static const struct bfile_io_read b_no_io_read = {
	b_no_read,
	b_no_seek,
	b_no_bounds,
	{
		bio_default_priority,
		NULL
	}
};

void
bplayback_file_init(struct bplayback_file *file)
{
	BKNI_Memset(file, 0, sizeof(*file));
	file->file.data = (void *)&b_no_io_read;
	file->file.index = NULL;
	file->file.close = b_no_close;
	file->file_kind = b_playback_file;
	return;
}

#endif
