/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control_still_decode.cpp $
 * $brcm_Revision: 11 $
 * $brcm_Date: 1/21/10 5:44p $
 *
 * Module Description: Central control of Brutus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control_still_decode.cpp $
 * 
 * 11   1/21/10 5:44p mward
 * SW7400-2517: merge to main support for BUILD_SYSTEM=magnum.
 * 
 * SW7400-2517/1   9/14/09 3:10p mward
 * SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
 * 97400.
 * 
 * 10   8/20/09 11:03a erickson
 * PR55994: refactor api to support non-TS streams
 *
 * 9   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 8   6/24/08 4:29p erickson
 * PR43222: switch to settop api playpump segment type
 *
 * 7   6/24/08 10:04a erickson
 * PR43222: add segmented playpump support for thumbnails
 *
 * 6   4/15/08 4:09p erickson
 * PR40307: decrease still decode timeout
 *
 * 5   3/22/08 2:09a erickson
 * PR40307: thumbnail demo update
 *
 * 4   3/20/08 3:18a erickson
 * PR40307: reduce busy loop
 *
 * 3   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 2   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 1   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 ***************************************************************************/
#include "control.h"

BDBG_MODULE(brutus_control_still_decode);

/* process_still_decode() is called from Control::onTimeout for synchronized, incremental processing */
void Control::process_still_decode()
{
#ifdef NEXUS_BUILD
    if (_thumbnail.ready) {
        bsurface_t still_surface;

        BDBG_ASSERT(_thumbnail.current);
        still_surface = bdecode_acquire_capture_buffer(_still_decode, display()->graphics, NULL);
        BDBG_MSG(("got still %p", still_surface));
        if (still_surface) {
            /* Scale down for smaller long-term storage.
            128x72 is the max we can scale from a 1920x1080 (15x downscale max).
            Brutus wants a final 47x47, so I use 188x94. */
            bsurface_create_settings create_settings;
            bsurface_t smaller_still;

            bsurface_create_settings_init(&create_settings, display()->graphics);
            create_settings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8;
            create_settings.width = 188;
            create_settings.height = 94;
            smaller_still = bsurface_create(display()->graphics, &create_settings);
            bsurface_copy(smaller_still, NULL, still_surface, NULL);
            bsurface_sync(smaller_still);
            bsurface_destroy(still_surface);

            bthumbnail_manager_set_surface(thumbnail_manager(), _thumbnail.current, smaller_still,
                create_settings.width * create_settings.height * 2 + 200 /* size estimate */
                );
        }

        bdecode_stop(_still_decode);
        _thumbnail.current = NULL;
        _thumbnail.ready = false;
    }

    if (!_thumbnail.current && (MApplication::tickCount()-_thumbnail.emptyGetTime)>1000) {
        void *data;
        unsigned size;

        BDBG_ASSERT(!_thumbnail.ready);
        if (!bthumbnail_manager_get_next_request(thumbnail_manager(), &_thumbnail.current)) {
            BDBG_ASSERT(_thumbnail.current);
            decode_still();
            _thumbnail.startTime = MApplication::tickCount();
        }
        else {
            _thumbnail.current = NULL;
            _thumbnail.emptyGetTime = MApplication::tickCount();
            BDBG_MSG(("no still available at %d", _thumbnail.emptyGetTime));
        }
    }
    else {
        /* 3 second timeout */
        if (_thumbnail.current && MApplication::tickCount() - _thumbnail.startTime > 3000) {
            BDBG_WRN(("still decode timeout"));
            _thumbnail.current = NULL;
        }
    }
#endif /* NEXUS_BUILD */
}

#ifdef NEXUS_BUILD
/* Brutus implements thumbnail by going around the Settop API shim directly to Nexus.
This allows the bthumbnail_extractor library to be written with Nexus types, not Settop API types. */
#include "bsettop_types.h"
#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_impl.h"
}
#endif
int Control::decode_still()
{
#ifdef NEXUS_BUILD
    bplaypump_t playpump;
    bstream_t still_stream;
    bplaypump_params playpump_params;
    bthumbnail_data thumbnail_data;
    bthumbnail_stream_create_settings create_settings;
    bthumbnail_extractor_settings extractor_settings;
    PlayEngine *playEngine;
    NEXUS_PlaypumpHandle nexusPlaypump;
    Video *video;
    int rc;

    playEngine = checkoutPlayEngine();
    if (!playEngine) return BERR_TRACE(NEXUS_UNKNOWN);

    bthumbnail_get_data(_thumbnail.current, &thumbnail_data);
    bthumbnail_stream_get_create_settings(thumbnail_data.stream, &create_settings);
    video = (Video *)create_settings.user_context;

    playpump = playEngine->playpump();

    /* a single extractor is used for all streams, so we have to reset everything */
    bthumbnail_extractor_get_settings(_thumbnail.extractor, &extractor_settings);
    extractor_settings.playpump = playpump->nPlaypump; /* because of the checkout, the playpump handle may have changed */
    extractor_settings.videoCodec = b_videocodec2nexus(video->mpeg.video[0].format);
    extractor_settings.transportType = b_mpegtype2nexus(video->mpeg.mpeg_type);
    extractor_settings.videoPid = video->mpeg.video[0].pid;
    extractor_settings.datafile = video->thumbnail_data.data_bfile;
    extractor_settings.indexfile = video->thumbnail_data.index_bfile;
    rc = bthumbnail_extractor_set_settings(_thumbnail.extractor, &extractor_settings);
    if (rc) return BERR_TRACE(rc);

    rc = bthumbnail_extractor_start_playpump(_thumbnail.extractor);
    if (rc) return BERR_TRACE(rc);

    still_stream = bstream_p_open(NULL, playpump, NULL, 0, &video->mpeg);
    if (!still_stream) return BERR_TRACE(NEXUS_UNKNOWN);

    rc = bthumbnail_extractor_feed_picture(_thumbnail.extractor, thumbnail_data.time);
    if (rc) return BERR_TRACE(rc);

    BDBG_MSG(("decoding still at %d msec", thumbnail_data.time));
    rc = bdecode_start(_still_decode, still_stream, NULL);
    if (rc) return BERR_TRACE(rc);

    BKNI_Sleep(200); /* TODO: get event */

    bthumbnail_extractor_stop_playpump(_thumbnail.extractor);
    bstream_close(still_stream);
    checkinPlayEngine(playEngine);
#endif /* NEXUS_BUILD */

    return 0;
}

void Control::decoded_still_ready(void *context)
{
#ifdef NEXUS_BUILD
    /* we could set an event, but the Control::onTimeout is pretty quick and we get free, simple synchronization */
    ((Control*)context)->_thumbnail.ready = true;
#endif
}

