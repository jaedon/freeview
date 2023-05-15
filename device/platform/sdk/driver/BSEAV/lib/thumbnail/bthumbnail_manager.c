/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail_manager.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 8/14/09 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail_manager.c $
 * 
 * 10   8/14/09 3:13p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 9   4/4/08 5:35p jtna
 * PR41386: Coverity Defect ID:7971 USE_AFTER_FREE
 *
 * 8   4/4/08 5:32p jtna
 * PR41385: Coverity Defect ID:7972 USE_AFTER_FREE
 *
 * 7   4/4/08 12:14p jtna
 * PR41176: Coverity Defect ID:7938 NULL_RETURNS
 *
 * 6   3/24/08 10:01a erickson
 * PR40307: improved cleanup
 *
 * 5   3/22/08 2:08a erickson
 * PR40307: update
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

BDBG_OBJECT_ID(bthumbnail_manager);
BDBG_OBJECT_ID(bthumbnail_stream);
BDBG_OBJECT_ID(bthumbnail);

BDBG_MODULE(bthumbnail_manager);

void bthumbnail_manager_get_default_settings( bthumbnail_manager_settings *settings )
{
    BKNI_Memset(settings, 0, sizeof(*settings));
    settings->maximum_memory = 16 * 1024 * 1024;
    settings->compressed_buffer_size = 1 * 1024 * 1024;
}

bthumbnail_manager_t bthumbnail_manager_create( const bthumbnail_manager_settings *settings )
{
    bthumbnail_manager_t manager;

    if (settings->maximum_memory == 0) {
        return NULL;
    }

    manager = BKNI_Malloc(sizeof(*manager));
    if (manager == 0) {
        return NULL;
    }
    BKNI_Memset(manager, 0, sizeof(*manager));
    BDBG_OBJECT_SET(manager, bthumbnail_manager);

    BLST_D_INIT(&manager->list);
    BLST_D_INIT(&manager->dangling);
    manager->settings = *settings;
    manager->compressed_buffer = BKNI_Malloc(settings->compressed_buffer_size);

    return manager;
}

void bthumbnail_manager_destroy( bthumbnail_manager_t manager )
{
    bthumbnail_stream_t stream;
    bthumbnail_t thumbnail;

    BDBG_OBJECT_ASSERT(manager, bthumbnail_manager);

    /* coverity[alias] */
    /* coverity[use_after_free] */
    while ((stream = BLST_D_FIRST(&manager->list))) {
        bthumbnail_stream_destroy(stream);
    }
    /* coverity[alias] */
    /* coverity[use_after_free] */
    while ((thumbnail = BLST_D_FIRST(&manager->dangling))) {
        bthumbnail_p_destroy(thumbnail);
    }
    BKNI_Free(manager->compressed_buffer);
    BDBG_OBJECT_UNSET(manager, bthumbnail_manager);
    BKNI_Free(manager);
}

int bthumbnail_manager_get_next_request( bthumbnail_manager_t manager, bthumbnail_t *p_thumbnail )
{
    bthumbnail_stream_t stream;
    bthumbnail_t thumbnail = NULL;

    BDBG_OBJECT_ASSERT(manager, bthumbnail_manager);

    /* this algorithm needs to select the next most important thumbnail that needs to be decoded.
    1) missing thumbnails in a visible view_window
    2) missing thumbnails in a visible full_view
    3) missing thumbnails in an invisible full_view
    */
    for (stream = BLST_D_FIRST(&manager->list); stream; stream = BLST_D_NEXT(stream, link)) {
        if (stream->settings.view_window.visible) {
            thumbnail = bthumbnail_stream_p_create_next_thumbnail(stream);
            if (thumbnail) goto done;
        }
    }
    for (stream = BLST_D_FIRST(&manager->list); stream; stream = BLST_D_NEXT(stream, link)) {
        if (stream->settings.full_view.visible) {
            thumbnail = bthumbnail_stream_p_create_next_thumbnail(stream);
            if (thumbnail) goto done;
        }
    }
    for (stream = BLST_D_FIRST(&manager->list); stream; stream = BLST_D_NEXT(stream, link)) {
        thumbnail = bthumbnail_stream_p_create_next_thumbnail(stream);
        if (thumbnail) goto done;
    }

done:
    if (thumbnail) {
        *p_thumbnail = thumbnail;
        return 0;
    }
    else {
        return -1;
    }
}

int bthumbnail_manager_set_surface( bthumbnail_manager_t manager, bthumbnail_t thumbnail, void *surface, unsigned size )
{
    int result = 0;
    unsigned target_free_space;
    BDBG_OBJECT_ASSERT(manager, bthumbnail_manager);

    thumbnail->data.surface = surface;
    thumbnail->size = size;
    manager->total_size += size;
    manager->total_thumbnails++;

    if (thumbnail->manager) {
        bthumbnail_p_destroy(thumbnail);
    }
    else {
        BDBG_MSG(("setting thumbnail %p, surface %p, time %d, size %d, total_size %d", thumbnail, surface, thumbnail->data.time, size, manager->total_size));
    }

    /* based on last request, how many do we need to free up. also, double it to give some head room. */
    target_free_space = manager->amount_short * size * 2;
    manager->amount_short = 0; /* we've done our best. now clear it. */

    while (manager->total_size+target_free_space > manager->settings.maximum_memory) {
        bool deleted_some = false;
        bthumbnail_stream_t stream;
        BDBG_MSG(("reducing memory from %d to target %d", manager->total_size, manager->settings.maximum_memory));

        /* first, try streams that aren't visible */
        for (stream = BLST_D_FIRST(&manager->list); stream && !deleted_some; stream = BLST_D_NEXT(stream, link)) {
            if (!stream->settings.view_window.visible) {
                deleted_some = bthumbnail_stream_p_delete_thumbnails(stream, false);
            }
        }

        if (!deleted_some) {
            for (stream = BLST_D_FIRST(&manager->list); stream && !deleted_some; stream = BLST_D_NEXT(stream, link)) {
                if (!stream->settings.full_view.visible) {
                    deleted_some = bthumbnail_stream_p_delete_thumbnails(stream, false);
                }
            }
        }

        if (!deleted_some) {
            /* that didn't help. so now try visible streams. */
            for (stream = BLST_D_FIRST(&manager->list); stream && !deleted_some; stream = BLST_D_NEXT(stream, link)) {
                deleted_some = bthumbnail_stream_p_delete_thumbnails(stream, true);
            }

            if (!deleted_some) {
                BDBG_WRN(("bthumbnail_manager unable to reduce memory usage below maximum. please rework algorithm."));
                break;
            }
        }
        BDBG_MSG(("  reduced memory to %d", manager->total_size));
        manager->wait_for_change = true;
    }

    return result;
}

void bthumbnail_manager_clear_visible( bthumbnail_manager_t manager )
{
    bthumbnail_stream_t stream;
    BDBG_OBJECT_ASSERT(manager, bthumbnail_manager);
    for (stream = BLST_D_FIRST(&manager->list); stream; stream = BLST_D_NEXT(stream, link)) {
        stream->settings.view_window.visible = false;
        stream->settings.full_view.visible = false;
    }
    manager->wait_for_change = false;
}

void bthumbnail_manager_p_delete_surface(bthumbnail_manager_t manager, bthumbnail_t thumbnail)
{
    BDBG_OBJECT_ASSERT(manager, bthumbnail_manager);
    BDBG_ASSERT(manager == thumbnail->data.stream->manager);

    BDBG_MSG(("delete thumbnail %p, surface %p, time %d", thumbnail, thumbnail->data.surface, thumbnail->data.time));
    (*manager->settings.delete_surface)( manager->settings.callback_context, thumbnail->data.surface);
    manager->total_size -= thumbnail->size;
    manager->total_thumbnails--;
}
