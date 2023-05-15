 /***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthumbnail.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 8/14/09 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail.c $
 * 
 * 14   8/14/09 3:12p erickson
 * PR55994: refactor so that bthumbnail_extractor can be used standlone
 * with nexus
 *
 * 13   7/6/09 8:49a vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes
 * key frame extraction for AVI 2.0 (OpenDML) streams
 *
 * 12   4/22/09 2:12p erickson
 * PR54418: added variety of codec support for mkv/mp4
 *
 * 11   8/1/08 5:05p erickson
 * PR45307: removed workaround
 *
 * 10   7/31/08 10:35a erickson
 * PR45037: add MKV support
 *
 * 9   6/24/08 4:29p erickson
 * PR43222: update def of NEXUS_PlaypumpSegment
 *
 * 8   6/20/08 12:33p erickson
 * PR43222: impl asf/avi thumbnails
 *
 * 7   4/4/08 12:16p jtna
 * PR41177: Coverity Defect ID:7937 NULL_RETURNS
 *
 * 6   3/24/08 10:01a erickson
 * PR40307: added assert
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

BDBG_MODULE(bthumbnail);

int bthumbnail_get_data( bthumbnail_t thumbnail, bthumbnail_data *data )
{
    BDBG_OBJECT_ASSERT(thumbnail, bthumbnail);
    *data = thumbnail->data;
    return 0;
}

bthumbnail_t bthumbnail_p_create_thumbnail( bthumbnail_stream_t stream, unsigned time )
{
    bthumbnail_t thumbnail, cur, prev = NULL;

    BDBG_MSG(("create_thumbnail for %s at %d", stream->create_settings.name, time));
    BDBG_OBJECT_ASSERT(stream, bthumbnail_stream);

    thumbnail = BKNI_Malloc(sizeof(*thumbnail));
    if (thumbnail == 0) {
        return NULL;
    }
    BKNI_Memset(thumbnail, 0, sizeof(*thumbnail));
    BDBG_OBJECT_SET(thumbnail, bthumbnail);

    thumbnail->data.stream = stream;
    thumbnail->data.time = time;

    /* must insert in ascending order by time */
    for (cur = BLST_D_FIRST(&stream->list); cur; cur = BLST_D_NEXT(cur, link)) {
        if (cur->data.time > time)
            break;
        prev = cur;
    }
    if (prev) {
        BLST_D_INSERT_AFTER(&stream->list, prev, thumbnail, link);
    }
    else {
        BLST_D_INSERT_HEAD(&stream->list, thumbnail, link);
    }

#if BDBG_DEBUG_BUILD
    /* verify correct order */
    prev = NULL;
    for (cur = BLST_D_FIRST(&stream->list); cur; cur = BLST_D_NEXT(cur, link)) {
        if (prev) BDBG_ASSERT(cur->data.time > prev->data.time);
        prev = cur;
    }
#endif

    return thumbnail;
}

void bthumbnail_p_destroy( bthumbnail_t thumbnail )
{
    BDBG_OBJECT_ASSERT(thumbnail, bthumbnail);

    if (thumbnail->manager) {
        /* if it was deleted while decoding, it was put into the dangling list. */
        BLST_D_REMOVE(&thumbnail->manager->dangling, thumbnail, link);
        if (thumbnail->data.surface) {
            bthumbnail_manager_p_delete_surface(thumbnail->manager, thumbnail);
        }
    }
    else {
        BLST_D_REMOVE(&thumbnail->data.stream->list, thumbnail, link);

        if (thumbnail->data.surface) {
            bthumbnail_manager_p_delete_surface(thumbnail->data.stream->manager, thumbnail);
        }
    }

    BDBG_OBJECT_UNSET(thumbnail, bthumbnail);
    BKNI_Free(thumbnail);
}

