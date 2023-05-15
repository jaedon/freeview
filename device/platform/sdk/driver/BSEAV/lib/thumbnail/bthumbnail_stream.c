/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_stream.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 8/14/09 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_stream.c $
 * 
 * 12   8/14/09 3:13p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 11   6/17/08 7:01a erickson
 * PR43222: add divx 311 support
 *
 * 10   4/15/08 4:11p erickson
 * PR40307: rework display algo to remove holes for still decode timeouts
 *
 * 9   4/4/08 5:30p jtna
 * PR41387: Coverity Defect ID:7970 USE_AFTER_FREE
 *
 * 8   4/4/08 12:19p jtna
 * PR41175: Coverity Defect ID:7939 NULL_RETURNS
 *
 * 7   3/24/08 10:01a erickson
 * PR40307: improved cleanup
 *
 * 6   3/22/08 2:08a erickson
 * PR40307: update
 *
 * 5   3/20/08 2:25p erickson
 * PR40307: remove warning
 *
 * 4   3/20/08 3:17a erickson
 * PR40307: clean up surfaces if we exceed the maximum_memory
 *
 * 3   3/20/08 1:47a erickson
 * PR40307: update
 *
 * 2   3/18/08 3:55p erickson
 * PR40307: update
 *
 * 1   3/17/08 9:22a erickson
 * PR40307: add thumbnail library
 *
 ******************************************************************/
#include "bthumbnail.h"
#include "bthumbnail_priv.h"

BDBG_MODULE(bthumbnail_stream);

void bthumbnail_stream_get_default_create_settings( bthumbnail_stream_create_settings *create_settings )
{
    BKNI_Memset(create_settings, 0, sizeof(*create_settings));
}

bthumbnail_stream_t bthumbnail_stream_create( bthumbnail_manager_t manager, const bthumbnail_stream_create_settings *create_settings )
{
    bthumbnail_stream_t stream;

    stream = BKNI_Malloc(sizeof(*stream));
    if (stream == 0) {
        return NULL;
    }
    BKNI_Memset(stream, 0, sizeof(*stream));
    BDBG_OBJECT_SET(stream, bthumbnail_stream);

    stream->create_settings = *create_settings;
    BLST_D_INIT(&stream->list);
    BLST_D_INSERT_HEAD(&manager->list, stream, link);

    stream->manager = manager;
    stream->settings.view_window.spacing = 30; /* 30 seconds */
    stream->settings.view_window.first_time = 0;
    stream->settings.view_window.last_time = 5 * 60; /* 5 minutes */
    stream->settings.full_view.number = 5;
    stream->settings.full_view.min_spacing = 15; /* seconds */
    manager->wait_for_change = false;

    return stream;
}

void bthumbnail_stream_destroy( bthumbnail_stream_t stream )
{
    bthumbnail_t thumbnail;

    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);

    /* coverity[alias] */
    /* coverity[use_after_free] */
    while ((thumbnail = BLST_D_FIRST(&stream->list))) {
        bthumbnail_p_destroy(thumbnail);
    }

    BLST_D_REMOVE(&stream->manager->list, stream, link);
    BDBG_OBJECT_UNSET(stream, bthumbnail_stream);
    BKNI_Free(stream);
}

void bthumbnail_stream_get_settings( bthumbnail_stream_t stream, bthumbnail_stream_settings *settings )
{
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);
    *settings = stream->settings;
}

int bthumbnail_stream_set_settings( bthumbnail_stream_t stream, const bthumbnail_stream_settings *settings )
{
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);
    stream->settings = *settings;
    stream->manager->wait_for_change = false;
    /* TODO: flush thumbnails that don't line up with new values */
    return 0;
}

#define ALIGN(num,spacing) \
    do { \
        if (spacing && num%spacing) { \
            if (num < spacing) { \
                num = spacing; \
            } \
            else { \
                num -= num%spacing; \
            } \
       } \
    } while (0)

int bthumbnail_p_stream_get_thumbnails( bthumbnail_stream_t stream, unsigned first_time, unsigned last_time,
    bthumbnail_t *thumbnail_array, unsigned total, unsigned *total_read, unsigned spacing )
{
    unsigned target_time;
    bthumbnail_t thumbnail;

    BSTD_UNUSED(last_time);
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);

    *total_read = 0;
    target_time = first_time;

    ALIGN(target_time, spacing);

    BDBG_MSG(("get_thumbnails starting at %d", target_time));
#if 0
/* TODO: this code has accurate spacing, but shows holes if a thumbnail is missing. */
    for (thumbnail = BLST_D_FIRST(&stream->list); thumbnail; ) {
        if (*total_read == total) {
            break;
        }
        if (spacing && thumbnail->data.time % spacing) {
            BDBG_MSG(("  skipping %d", thumbnail->data.time));
            thumbnail = BLST_D_NEXT(thumbnail, link);
            continue;
        }

        if (thumbnail->data.time > target_time) {
            BDBG_MSG(("  no thumbnail at %d", target_time));
            thumbnail_array[*total_read] = NULL;
            (*total_read)++;
            target_time += spacing;
        }
        else if (thumbnail->data.time == target_time) {
            if (thumbnail->data.surface && !thumbnail->manager) {
                BDBG_MSG(("  thumbnail at %d", target_time));
                thumbnail_array[*total_read] = thumbnail;
            }
            else {
                BDBG_MSG(("  NULL thumbnail at %d", target_time));
                thumbnail_array[*total_read] = NULL;
            }
            (*total_read)++;
            target_time += spacing;
            thumbnail = BLST_D_NEXT(thumbnail, link);
        }
        else {
            BDBG_MSG(("  skipping %d", thumbnail->data.time));
            thumbnail = BLST_D_NEXT(thumbnail, link);
        }
    }
#else
    for (thumbnail = BLST_D_FIRST(&stream->list); thumbnail; ) {
        if (*total_read == total) {
            break;
        }
        if (spacing && thumbnail->data.time % spacing) {
            BDBG_MSG(("  skipping %d", thumbnail->data.time));
            thumbnail = BLST_D_NEXT(thumbnail, link);
            continue;
        }

        if (thumbnail->data.time > target_time) {
            target_time += spacing;
        }
        else if (thumbnail->data.time == target_time && thumbnail->data.surface && !thumbnail->manager) {
            BDBG_MSG(("  thumbnail at %d", target_time));
            thumbnail_array[*total_read] = thumbnail;
            (*total_read)++;
            target_time += spacing;
            thumbnail = BLST_D_NEXT(thumbnail, link);
        }
        else {
            BDBG_MSG(("  skipping %d", thumbnail->data.time));
            thumbnail = BLST_D_NEXT(thumbnail, link);
        }
    }
#endif

    stream->manager->amount_short = total - *total_read;

    return 0;
}

int bthumbnail_stream_get_thumbnails( bthumbnail_stream_t stream, unsigned first_time, unsigned last_time,
    bthumbnail_t *thumbnail_array, unsigned total, unsigned *total_read )
{
    return bthumbnail_p_stream_get_thumbnails(stream, first_time, last_time,
        thumbnail_array, total, total_read, stream->settings.view_window.spacing);
}

int bthumbnail_stream_get_full_view_thumbnails( bthumbnail_stream_t stream,
    bthumbnail_t *thumbnail_array, unsigned total, unsigned *total_read )
{
    return bthumbnail_p_stream_get_thumbnails(stream, stream->settings.first_time, 0xFFFFFFFF,
        thumbnail_array, total, total_read, stream->actual_full_view_spacing);
}

#define BDBG_MSG_SEARCH(X) /* BDBG_MSG */

bthumbnail_t bthumbnail_stream_p_create_next_thumbnail(bthumbnail_stream_t stream)
{
    bthumbnail_t thumbnail;
    unsigned spacing;
    unsigned target_first_time, target_last_time;

    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);

    if (stream->settings.view_window.visible) {
        spacing = stream->settings.view_window.spacing;
        target_first_time = stream->settings.view_window.first_time;
        target_last_time = stream->settings.view_window.last_time;

        if (target_first_time > stream->settings.last_time || target_last_time < stream->settings.first_time) {
            BDBG_MSG_SEARCH(("search stream %p: out of view_window", stream));
            return NULL;
        }

        if (target_first_time < stream->settings.first_time)
            target_first_time = stream->settings.first_time;
        if (target_last_time > stream->settings.last_time)
            target_last_time = stream->settings.last_time;
    }
    else if (stream->settings.full_view.visible) {
        spacing = stream->settings.full_view.number ? (stream->settings.last_time - stream->settings.first_time) / stream->settings.full_view.number : stream->settings.full_view.min_spacing;
        if (spacing < stream->settings.full_view.min_spacing) {
            spacing = stream->settings.full_view.min_spacing;
        }
        target_first_time = stream->settings.first_time;
        target_last_time = stream->settings.last_time;
        stream->actual_full_view_spacing = spacing; /* remember this for our get */
    }
    else {
        BDBG_MSG_SEARCH(("search stream %p: invisible", stream));
        return NULL;
    }

    ALIGN(target_first_time, spacing);

    BDBG_MSG_SEARCH(("search stream %p: range %d..%d, %d spacing", stream, target_first_time, target_last_time, spacing));
    /* create view_window thumbnail */
    for (thumbnail = BLST_D_FIRST(&stream->list); thumbnail && target_first_time <= target_last_time; thumbnail = BLST_D_NEXT(thumbnail, link)) {
        if (thumbnail->data.time < target_first_time) {
            continue;
        }
        else if (thumbnail->data.time == target_first_time) {
            /* we already have it. try the next. */
            target_first_time += spacing;
        }
        else {
            /* we don't have it, so create one. */
            return bthumbnail_p_create_thumbnail(stream, target_first_time);
        }
    }
    if (!thumbnail && target_first_time <= target_last_time) {
        return bthumbnail_p_create_thumbnail(stream, target_first_time);
    }

    /* this stream has all the thumbnails it wants */
    return NULL;
}

void bthumbnail_stream_get_data( bthumbnail_stream_t stream, bthumbnail_stream_data *data )
{
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);
    data->total_thumbnails = 0; /* TODO */
}

void bthumbnail_stream_get_create_settings( bthumbnail_stream_t stream, bthumbnail_stream_create_settings *create_settings )
{
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);
    *create_settings = stream->create_settings;
}

bool bthumbnail_stream_p_delete_thumbnails(bthumbnail_stream_t stream, bool aggressive)
{
    bthumbnail_t thumbnail;
    bthumbnail_t furthest = NULL;
    unsigned max_distance = 0;

    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);
    /* if neither view_window or full_view is visible, delete thumbnails. also, if only full view is visible,
    delete non-full view thumbnails */
    if (!stream->settings.view_window.visible) {
        for (thumbnail = BLST_D_FIRST(&stream->list); thumbnail; thumbnail = BLST_D_NEXT(thumbnail, link)) {
            if (!stream->settings.full_view.visible || !stream->actual_full_view_spacing || thumbnail->data.time % stream->actual_full_view_spacing) {
                BDBG_MSG(("delete from %d %d %d", stream->settings.full_view.visible, thumbnail->data.time, stream->actual_full_view_spacing));
                bthumbnail_p_destroy(thumbnail);
                return true;
            }
        }
    }
    if (!aggressive) return false;

    /* we have to delete thumbnails from this stream. delete the thumbnail which is furthest from that view window. */
    for (thumbnail = BLST_D_FIRST(&stream->list); thumbnail; thumbnail = BLST_D_NEXT(thumbnail, link)) {
        unsigned d;
        if (thumbnail->data.time < stream->settings.view_window.first_time) {
            d = stream->settings.view_window.first_time - thumbnail->data.time;
        }
        else {
            d = thumbnail->data.time - stream->settings.view_window.first_time;
        }
        if (d >= max_distance) {
            max_distance = d;
            furthest = thumbnail;
        }
    }
    if (furthest) {
        BDBG_MSG(("delete furthest %d %d", max_distance));
        bthumbnail_p_destroy(furthest);
        return true;
    }

    return false;
}
