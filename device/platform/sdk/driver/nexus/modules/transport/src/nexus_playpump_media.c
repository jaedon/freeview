/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_playpump_media.c $
 * $brcm_Revision: 56 $
 * $brcm_Date: 10/16/12 2:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_playpump_media.c $
 * 
 * 56   10/16/12 2:26p gmohile
 * SW7435-375: transport module should not call into security module
 * 
 * 55   7/27/12 2:14p jgarrett
 * SW7425-3281: Merge to main branch
 * 
 * SW7425-3281/2   7/10/12 2:19p piyushg
 * SW7425-3281: Nexus DRM support
 * 
 * SW7425-3281/1   7/8/12 4:39p piyushg
 * SW7425-3281: Add NEXUS_Playpump_GetAsfHandle_priv to get ASF handle
 *  from playpump.
 * 
 * 54   4/16/12 11:23a vsilyaev
 * SW7425-2378: Fixed handling of APE container
 * 
 * 53   4/12/12 4:00p jgarrett
 * SW7425-2378: Adding Monkey's Audio and MLP
 * 
 * 52   4/9/12 3:50p jgarrett
 * SW7231-662: Adding AIFF support
 * 
 * 51   2/7/12 7:34p jgarrett
 * SW7425-2136: Adding FLAC support
 * 
 * 50   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 * 
 * 49   11/21/11 2:48p jgarrett
 * SW7425-1406: Adding Vorbis audio support
 * 
 * VORBIS_DEVEL/2   11/16/11 1:51p jgarrett
 * SW7425-1406: Merging latest main branch changes
 * 
 * 48   11/2/11 11:19a vsilyaev
 * SW7425-1654: Merged decryption for stream of MP4 Fragments
 * 
 * SW7425-1654/1   11/2/11 1:50p hugost
 * Added playready hooks for fragmented MP4 decryption.
 * 
 * 47   10/25/11 11:36a vsilyaev
 * SWDTV-8388: Added NEXUS_TransportType_eOgg type
 * 
 * 46   10/11/11 10:59a vsilyaev
 * SW7420-2086: Don't advance data FIFO for S/G payloads
 * 
 * VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
 * SW7425-1406: Adding Vorbis support
 * 
 * 45   9/13/11 2:06p vsilyaev
 * SW7425-1219: Consume more of old data, prior to advancing to a new
 *  block
 * 
 * 44   5/12/11 3:41p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 43   2/22/11 7:53p vsilyaev
 * SW7422-107: Added FLV mapping
 * 
 * 42   12/16/10 6:07p vsilyaev
 * SW7425-39: Added playpump private interface
 * 
 * 41   12/7/10 7:53p piyushg
 * SW7405-4386: Initial checkin for playready intigration with mainline
 *  brutus
 * 
 * 40   9/8/10 12:05p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * 39   8/25/10 6:31p vsilyaev
 * SWGIGGS2S-1166: Don't consume new data when flush is queued
 * 
 * 38   7/19/10 4:24p gmohile
 * SW3548-2994 : Add support for encrypted mkv tracks
 * 
 * 37   5/6/10 3:45p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 *  container
 * 
 * 36   3/9/10 7:42p vsilyaev
 * SW7405-3975: Verify that PES queue if empty before flushing at EOS
 * 
 * 35   3/2/10 4:01p gmohile
 * SW7408-79 : Fix compile error
 * 
 * 34   2/26/10 6:13p vsilyaev
 * SW7405-3975: Advance demux priort to flushing it
 * 
 * 33   2/26/10 4:37p gmohile
 * SW7408-79 : Move subtitle parser functionality to app
 * 
 * 32   2/1/10 10:14a maivu
 * SW3548-2750: Fixed mismatch struct name change for divx_drm build from
 *  SW7335-661 checkin
 * 
 * 31   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 30   7/15/09 7:24p vsilyaev
 * PR 55653: Added WAV format
 * 
 * 29   5/20/09 11:15a vsilyaev
 * PR 37574: Removed unused/missleading code
 * 
 * 28   5/15/09 12:48p vsilyaev
 * PR 55193: Added function to return last parsed PTS by the media library
 * 
 * 27   1/23/09 12:40p gmohile
 * PR 51374 : Added congestion detection in throttle timer
 * 
 * 26   12/30/08 8:55a vsilyaev
 * PR 50606: Added hooks for hardware support of MPEG1 system streams
 * 
 * 25   12/19/08 7:05p vsilyaev
 * PR 49464: Convert queued-up data prior to feching new data
 * 
 * 24   12/4/08 3:29p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 *  stream parsing
 * 
 * 23   12/4/08 1:47p gmohile
 * PR 49988 : Fix return code for DivX DRM failure
 * 
 * 22   10/22/08 11:39a erickson
 * PR48216: allow transport to run with no dma or security modules
 *
 * 21   10/21/08 3:04p vsilyaev
 * PR 48176: Improved error recovery
 *
 * 20   9/12/08 12:39p vsilyaev
 * PR 46175: Added code to handle case when AVI frame is larger then PVR
 *  FIFO size
 *
 * 19   8/20/08 10:45a erickson
 * PR45521: set bmedia_filter_set_keyframeonly for non-normal play rates
 *
 * 18   8/7/08 4:47p erickson
 * PR45422: DivX DRM fix
 *
 * 17   8/5/08 9:05a erickson
 * PR45422: add DivX DRM support
 *
 * 16   7/23/08 2:47p erickson
 * PR45016: add SUBTITLE_SUPPORT
 *
 * 15   6/30/08 10:03a erickson
 * PR44322: fix warning
 *
 * 14   6/25/08 5:51p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 13   6/24/08 11:50a vsilyaev
 * PR 38347: Use separate timestamps for audio and video streams
 *
 * 12   6/17/08 11:26a vsilyaev
 * PR 42739: Fixed NO PVR build
 *
 * 11   6/16/08 6:29p vsilyaev
 * PR 43636: Fixed key passing
 *
 * 10   6/12/08 9:10p katrep
 * PR43636: Add code to validate the key passed
 *
 * 9   5/14/08 1:19p vsilyaev
 * PR 42119: Fixed crash in debug output
 *
 * 8   5/7/08 2:00p vsilyaev
 * PR 41869: Added core for re-packetize MPEG-2 TS data for encrypted PVR
 *
 * 7   5/5/08 1:34p vsilyaev
 * PR 42355: Reset media playRate on stop
 *
 * 6   4/18/08 5:41p vsilyaev
 * PR 41868: Connected playpump security to MS DRM
 *
 * 5   3/25/08 12:13p erickson
 * PR36068: make bmem_alloc private
 *
 * 4   3/6/08 1:50p vsilyaev
 * PR 38347: Allow to set timestamp for all sub-streams
 *
 * 3   3/3/08 3:37p vsilyaev
 * PR 39818: Support of MKV format
 *
 * 2   3/3/08 3:11p erickson
 * PR40194: handle ReadComplete(0,0) for EOF flushing of media framework
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/7   12/20/07 2:07p vsilyaev
 * PR 37574: Added playRate for playpump
 *
 * Nexus_Devel/6   12/6/07 5:23p vsilyaev
 * PR 37574: Added handling of segmented feed
 *
 * Nexus_Devel/5   12/5/07 12:44p vsilyaev
 * PR 37574: Enabled media support
 *
 * Nexus_Devel/4   12/4/07 6:19p vsilyaev
 * PR 37574: Added more bmedia support
 *
 * Nexus_Devel/3   11/26/07 4:46p vsilyaev
 * PR 37574: Integrated bmedia support
 *
 * Nexus_Devel/2   10/9/07 11:04a vsilyaev
 * PR 34662: Rearranged inclusion order of header files
 *
 * Nexus_Devel/1   9/26/07 3:11p erickson
 * PR34925: initial playpump checkin
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#include "nexus_playpump_impl.h"
#include "bsink_playback.h"
#include "blst_squeue.h"

#if B_HAS_MSDRM_PD
#include "priv/nexus_core_security.h"
#if  B_HAS_MSDRM_PRDY
#include "prdy_core.h"
#include "prdy_decryptor.h"
#include "prdy_mp4.h"
#else
#include "drmcore.h"
#include "drmdecryptor.h"
#endif
#endif
#if B_HAS_MSDRM_ND
#include "bdrmnd_decryptor.h"
#endif
#if B_HAS_DIVX_DRM
#include "bdrm_decrypt.h"
#endif


#if B_HAS_MEDIA
/* saving PES dara to file is controlled in the  BSEAV/lib/utils/bsink_playback.c */

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */
BDBG_MODULE(nexus_playpump_media);


BDBG_OBJECT_ID(b_pump_demux_t);
/* Must be 10K for proper WMA audio flushing */
#define B_PUMP_DEMUX_FILLER_SIZE    10240
struct b_pump_demux {
    BDBG_OBJECT(b_pump_demux_t)
    NEXUS_PlaypumpHandle pump;
    struct bpvr_queue pending; /* entries extracted from the playpump main queue and fed into the pump_packtizer */
    batom_factory_t factory;
    batom_pipe_t pipe_demux; /* input to the filter */
    bsink_playback_t sink;
    struct {
        bool  feed_stall; /* true if feed stalled (i.e. due to lack to resources) */
        bool  flush_queued;  /* if received entry to flush filter (e.g. end of stream mark), actuall flush would happen after completing processing of all queued data */
        unsigned forced_clear_events; /* number calls to bmedia_filter_clear */
        size_t pes_bytes; /* number of bytes consumed by bsink_playback */
        size_t recycle_count; /* number of recycled playpump segments */
        batom_pipe_t pipe_in; /* pipe from the bmedia_filter */
    } pes_feed;
    bmedia_filter_t filter;
    void *eos_filler;
    void *dcrypt_ctx;      /* drm context, may be used by asf and others */
    void *drm_ctx;
    b_pid_map video_map;
    b_pid_map audio_map;
    b_pid_map other_map;
};

#if defined(B_HAS_ASF)
extern void* b_media_asf_get_handle(bmedia_filter_t filter);
void* 
NEXUS_Playpump_GetAsfHandle_priv(NEXUS_PlaypumpHandle handle)
{
	void* asf = NULL;
	NEXUS_LockModule();
	if(handle)
		asf = b_media_asf_get_handle(handle->demux->filter);
	NEXUS_UnlockModule();
	return asf;
}
#endif


static void *
b_mem_alloc(balloc_iface_t alloc, size_t size)
{
    void *buf, *buf_cached;
    BERR_Code rc;
    BSTD_UNUSED(alloc);
    buf = BMEM_Alloc(g_pCoreHandles->heap[0], size);
    if(!buf) {rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);goto err_alloc;}
    rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], buf, &buf_cached); /* map memory to the cached region */
    if (rc) {rc = BERR_TRACE(rc); goto err_cached;}
    return buf_cached;

err_cached:
    BMEM_Free(g_pCoreHandles->heap[0], buf);
err_alloc:
    return NULL;
}

static void
b_mem_free(balloc_iface_t alloc, void *ptr)
{
    uint32_t offset;
    void *buf;

    BSTD_UNUSED(alloc);
    offset = NEXUS_AddrToOffset(ptr);
    buf = NEXUS_OffsetToUncachedAddr(offset);
    BMEM_Free(g_pCoreHandles->heap[0], buf);
    return;
}

static const struct balloc_iface bmem_alloc[] = {
    {b_mem_alloc, b_mem_free}
};

#define B_PUMP_DEMUX_FACTORY_ATOMS  128
#define B_PUMP_DEMUX_POOL_BLOCKS    64

static void
b_pump_reset_pes_feed(b_pump_demux_t demux)
{
    demux->pes_feed.pes_bytes = 0;
    demux->pes_feed.feed_stall = false;
    demux->pes_feed.flush_queued = false;
    demux->pes_feed.recycle_count = 0;
    demux->pes_feed.forced_clear_events = 0;
    return;
}

#if defined (B_HAS_MSDRM_PD) || defined (B_HAS_MSDRM_ND)
static void 
b_pump_update_asf_stream_cfg(void *cntx, unsigned stream_id, basf_stream_cfg *cfg)
{
    b_pump_demux_t demux = cntx;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
#if defined (B_HAS_MSDRM_PD)
    bdrm_update_asf_stream_cfg(&demux->dcrypt_ctx, stream_id, cfg);
#elif defined (B_HAS_MSDRM_ND)
    bdrmnd_update_asf_stream_cfg(&demux->dcrypt_ctx, stream_id, cfg);
#endif
    return;
}
#endif

#if defined (B_HAS_MSDRM_PRDY)

void b_pump_parse_bmp4_box(void *cntx, const batom_cursor *data, size_t payload_len)
{
    b_pump_demux_t demux = cntx;

    if( demux->pump->settings.securityContext != NULL){
        NEXUS_KeySlotHandle key = (NEXUS_KeySlotHandle )demux->pump->settings.securityContext;
        bdrm_parse_mp4_fragment_context( (void *)key->drmContext, data, payload_len);
    }
}

void b_pump_mp4_sample(void *cntx, uint32_t track_ID, unsigned sample_no, const batom_cursor *data, size_t payload_len)
{
    b_pump_demux_t demux = cntx;

    if ( demux->pump->settings.securityContext != NULL){
        NEXUS_KeySlotHandle key = (NEXUS_KeySlotHandle )demux->pump->settings.securityContext;
        bdrm_decrypt_mp4_sample( (void *)key->drmContext, track_ID, sample_no, data, payload_len);
    }
}

static void 
b_pump_update_mp4_fragment_cfg(void *cntx, bmp4_fragment_demux_cfg *cfg)
{
    b_pump_demux_t demux = cntx;
    cfg->user_context = cntx;
    cfg->traf_box = b_pump_parse_bmp4_box;
    cfg->sample = b_pump_mp4_sample;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);

    return;
}
#endif


#if defined(B_HAS_AVI) && B_HAS_DIVX_DRM
static void 
b_pump_activate_avi_stream(void *cntx, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream)
{
    b_pump_demux_t demux = cntx;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    bdrm_activate(&demux->dcrypt_ctx, stream_id, parser, stream);
    return;
}

static void 
b_pump_deactivate_avi_stream(void *cntx, unsigned stream_id, bavi_parser_t parser, bavi_stream_t stream)
{
    b_pump_demux_t demux = cntx;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    bdrm_deactivate(&demux->dcrypt_ctx, stream_id, parser, stream);
    return;
}
#endif

static void 
b_pump_demux_stream_error(void *cntx)
{
    b_pump_demux_t demux = cntx;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    NEXUS_TaskCallback_Fire(demux->pump->errorCallback);
    return;
}

#if defined (HUMAX_PLATFORM_BASE)
void
b_pump_get_demux_ctx(void* cntx, void* dcrypt_ctx)
{
	b_pump_demux_t demux = cntx;
	dcrypt_ctx = demux->dcrypt_ctx;
}
#else
void b_pump_get_demux_ctx(void* cntx, void* dcrypt_ctx)
{
	return;
}
#endif

b_pump_demux_t
b_pump_demux_create(NEXUS_PlaypumpHandle pump)
{
    b_pump_demux_t demux;
    bmedia_filter_cfg filter_cfg;
    bsink_playback_cfg sink_cfg;
    BERR_Code rc;
    void *addr;

    BDBG_MSG_TRACE(("b_pump_demux_create>:"));
    demux = BKNI_Malloc(sizeof(*demux));
    if(!demux) {
        BDBG_ERR(("b_pump_demux_init: can't allocate %u bytes", sizeof(*demux)));
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BDBG_OBJECT_INIT(demux, b_pump_demux_t);
    demux->pump = pump;

    b_pump_reset_pes_feed(demux);
    demux->pes_feed.pipe_in = NULL;
    BFIFO_INIT(&demux->pending, pump->item_mem, pump->openSettings.numDescriptors);
    b_pid_map_init(&demux->video_map, 0xE0);
    b_pid_map_init(&demux->audio_map, 0xC0);
    b_pid_map_init(&demux->other_map, 0xD0); /* Hijack  top 16 audio stream_id's  ITU-T Rec. H.222.0 (ISO/IEC 13818-1) Table 2-22 . Stream_id assignments */
    demux->factory = batom_factory_create(bkni_alloc, B_PUMP_DEMUX_FACTORY_ATOMS);
    if(!demux->factory) {
        goto err_factory;
    }
    bsink_playback_default_cfg(&sink_cfg);
    sink_cfg.feed = demux->pump->play_feed;
    sink_cfg.cache_flush = NEXUS_FlushCache;
    sink_cfg.addr_to_offset = NEXUS_AddrToOffset;
    demux->sink = bsink_playback_create(demux->factory, &sink_cfg);
    if(!demux->sink) {
        goto err_sink;
    }

    demux->pipe_demux = batom_pipe_create(demux->factory);
    if(!demux->pipe_demux) {
        goto err_pipe_demux;
    }
    demux->eos_filler = BMEM_Alloc(g_pCoreHandles->heap[0], B_PUMP_DEMUX_FILLER_SIZE);
    if(!demux->eos_filler) {
        goto err_eos_filler;
    }

    bmedia_filter_default_cfg(&filter_cfg);
    rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], demux->eos_filler, &addr);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_cached_addr;}
    filter_cfg.eos_data = addr;
    filter_cfg.eos_len = B_PUMP_DEMUX_FILLER_SIZE;
    filter_cfg.application_cnxt = demux;
    filter_cfg.stream_error = b_pump_demux_stream_error;

#if defined(B_HAS_ASF)
#if defined (B_HAS_MSDRM_PD) ||  defined (B_HAS_MSDRM_ND)
    filter_cfg.update_asf_stream_cfg = b_pump_update_asf_stream_cfg;
#endif
#endif /* asf drm support */

#if defined (B_HAS_MSDRM_PRDY)
    filter_cfg.update_mp4_fragment_cfg = b_pump_update_mp4_fragment_cfg;
#endif

#if defined(B_HAS_AVI)
#if B_HAS_DIVX_DRM
    BDBG_MSG(("ACTIVATE DIVX DRM"));
    filter_cfg.avi[0].activate_avi_stream = b_pump_activate_avi_stream;
    filter_cfg.avi[0].deactivate_avi_stream = b_pump_deactivate_avi_stream;
#endif
#endif

    BKNI_Memset((void *)filter_cfg.eos_data, 0, filter_cfg.eos_len);
    demux->filter = bmedia_filter_create(demux->factory, bmem_alloc, &filter_cfg);

    if(!demux->filter) {
        goto err_filter;
    }

    BDBG_MSG_TRACE(("b_pump_demux_create>: %#lx", (unsigned long)demux));
    return demux;

err_eos_filler:
    BMEM_Free(g_pCoreHandles->heap[0], demux->eos_filler);
err_cached_addr:
err_filter:
    batom_pipe_destroy(demux->pipe_demux);
err_pipe_demux:
    bsink_playback_destroy(demux->sink);
err_sink:
    batom_factory_destroy(demux->factory);
err_factory:
    BKNI_Free(demux);
err_alloc:
    return NULL;
}

void
b_pump_demux_destroy(b_pump_demux_t demux)
{
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_destroy>: %#lx", (unsigned long)demux));
    bmedia_filter_destroy(demux->filter);
    bsink_playback_destroy(demux->sink);
    batom_pipe_destroy(demux->pipe_demux);
    batom_factory_destroy(demux->factory);
    BMEM_Free(g_pCoreHandles->heap[0], demux->eos_filler);
    BDBG_OBJECT_DESTROY(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_destroy<: %#lx", (unsigned long)demux));
    BKNI_Free(demux);
    return;
}


void 
b_pump_demux_status(b_pump_demux_t demux, NEXUS_PlaypumpStatus *pStatus)
{
    batom_factory_stats factory_stats;
    bmedia_filter_status filter_status;


    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);

    BDBG_MSG(("demux_status: pending:%#lx:%u  pipe_in:%#lx pes_bytes:%u %s", (unsigned long)BFIFO_READ(&demux->pending), BFIFO_READ_PEEK(&demux->pending)?BFIFO_READ(&demux->pending)->ref_cnt:0, (unsigned long)demux->pes_feed.pipe_in?batom_pipe_peek(demux->pes_feed.pipe_in):0, demux->pes_feed.pes_bytes, demux->pes_feed.feed_stall?"STALL":""));

    batom_factory_get_stats(demux->factory, &factory_stats);
    BDBG_MSG(("demux_status: atoms[live:%u allocated:%u freed:%u] alloc[pool:%u/%u/%u arena:%u/%u/%u alloc:%u/%u]", factory_stats.atom_live, factory_stats.atom_allocated, factory_stats.atom_freed, factory_stats.alloc_pool, factory_stats.free_pool, factory_stats.full_pool, factory_stats.alloc_arena, factory_stats.free_arena, factory_stats.full_arena, factory_stats.alloc_alloc, factory_stats.free_alloc));

    bmedia_filter_get_status(demux->filter, &filter_status);
    BDBG_MSG(("demux_status: offset:%llu acc_length:%u obj_length:%u state:%s errors:%u,%u,%u", filter_status.offset, filter_status.acc_length, filter_status.obj_length, filter_status.state, filter_status.errors.sync_errors, filter_status.errors.resync_events, filter_status.errors.format_errors));

    pStatus->syncErrors += filter_status.errors.sync_errors;
    pStatus->resyncEvents += filter_status.errors.resync_events;
    pStatus->streamErrors += filter_status.errors.format_errors + demux->pes_feed.forced_clear_events;
    pStatus->mediaPtsType = filter_status.last_pts_valid?NEXUS_PtsType_eCoded:NEXUS_PtsType_eInterpolatedFromInvalidPTS;
    pStatus->mediaPts = filter_status.last_pts;

    return;
}


void
b_pump_demux_stop(b_pump_demux_t demux)
{
    NEXUS_P_PlaypumpPidChannel *pid;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_stop>: %#lx", (unsigned long)demux));
    BDBG_ASSERT(demux->pump);

#if B_HAS_DIVX_DRM
    if (demux->dcrypt_ctx && (demux->pump->settings.transportType == NEXUS_TransportType_eAvi || demux->pump->settings.transportType == NEXUS_TransportType_eMkv)){
        bdrm_stop(demux->dcrypt_ctx);
    }
#endif

    for(pid=BLST_S_FIRST(&demux->pump->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        b_pump_demux_remove_pid(demux, pid);
    }
    bmedia_filter_stop(demux->filter);
    bsink_playback_stop(demux->sink);
    b_pump_reset_pes_feed(demux);
    demux->pes_feed.pipe_in = NULL;
    b_pump_demux_set_rate(demux, demux->pump->settings.playRate); /* reset rate on stop */
    BFIFO_INIT(&demux->pending, demux->pump->item_mem, demux->pump->openSettings.numDescriptors);
    BDBG_MSG_TRACE(("b_pump_demux_stop<: %#lx", (unsigned long)demux));
    return;
}

static void
b_pump_demux_advance_recycle(b_pump_demux_t demux)
{
    struct bpvr_queue_item *item;
    unsigned count;

    BDBG_MSG_TRACE(("b_pump_demux_advance_recycle>: %#lx", (unsigned long)demux));
    for(count=0;BFIFO_READ_PEEK(&demux->pending)>0;count++) {
        item = BFIFO_READ(&demux->pending);
        BDBG_MSG_TRACE(("b_pump_demux_advance_recycle: %#lx %#lx:%u", (unsigned long)demux, (unsigned long)item, item->ref_cnt));
        if (item->ref_cnt) { break; }
        BDBG_MSG_TRACE(("b_pump_demux_advance_recycle: %#lx %#lx recycling %u:%u %#lx:%#lx", (unsigned long)demux, (unsigned long)item, item->skip, item->desc.length, (unsigned long)BFIFO_READ(&demux->pump->fifo), (unsigned long)item->desc.addr - item->skip));
        if(!item->sg) {
        BDBG_ASSERT(BFIFO_READ(&demux->pump->fifo) == ((uint8_t *)item->desc.addr - item->skip));
        BFIFO_READ_COMMIT(&demux->pump->fifo, item->skip+item->desc.length);
        }
        BFIFO_READ_COMMIT(&demux->pending, 1);
        BFIFO_READ_COMMIT(&demux->pump->activeFifo, 1); /* recycle chunk */
        item->skip = 0;
        item->desc.length = 0;
        item->desc.addr = 0;
    }
    demux->pes_feed.recycle_count += count;
    BDBG_MSG_TRACE(("b_pump_demux_advance_recycle<: %#lx %u:%u", (unsigned long)demux, demux->pes_feed.recycle_count, count));
    return;
}

/* should be the same as in playback, basisally if buffer less than B_IO_BLOCK_LIMIT, then playback would wait for playpump to consume it, but playpump(media_filter) could wait for playback to feed more data in order to satisfy its needs */
#define B_IO_BLOCK_SIZE 4096
#define B_IO_BLOCK_LIMIT (4*B_IO_BLOCK_SIZE)

static bool /* this function return true if there any usefull empty space in the playpump FIFO */
b_pump_demux_check_buffer( b_pump_demux_t demux)
{
    size_t data_left;
    uint8_t *addr;
    NEXUS_PlaypumpHandle pump = demux->pump;
    if(BFIFO_WRITE_LEFT(&pump->activeFifo)==0) {
        return false;
    }
    data_left = BFIFO_WRITE_LEFT(&pump->fifo);
    if(data_left > B_IO_BLOCK_LIMIT) {
        return true;
    }
    addr = BFIFO_WRITE(&pump->fifo);
    if((addr+B_IO_BLOCK_LIMIT) >= ((uint8_t *)pump->buf + pump->openSettings.fifoSize)) { /* if data is close to wrap, then application would discard it (and there is might be more data after wrap */
        return true;
    }
    return false;
}

void
b_pump_demux_advance(b_pump_demux_t demux)
{
    BDBG_MSG_TRACE(("b_pump_demux_advance>: %#lx %s %s", (unsigned long)demux, demux->pes_feed.feed_stall?"stall":"", batom_pipe_peek(demux->pipe_demux)==NULL?"idle":""));
    /* 1. recycle packets in the playpump fifo */
    b_pump_demux_advance_recycle(demux);
    if(demux->pes_feed.feed_stall ) {
        /* 2. some data was recycled, push demux further */
        demux->pes_feed.feed_stall = !(bmedia_filter_feed(demux->filter, demux->pipe_demux));
    }
    /* 2. Check if filter was stalled */
    BDBG_MSG_TRACE(("b_pump_demux_advance: %#lx %u %u %u %u %u", (unsigned long)demux, demux->pes_feed.pes_bytes, BFIFO_READ_PEEK(&demux->pump->pendingFifo), BFIFO_WRITE_PEEK(&demux->pump->fifo), BFIFO_WRITE_LEFT(&demux->pump->activeFifo), batom_pipe_peek(demux->pes_feed.pipe_in)));
    if (demux->pes_feed.pes_bytes==0 && BFIFO_READ_PEEK(&demux->pump->pendingFifo)==0 && !b_pump_demux_check_buffer(demux) && batom_pipe_peek(demux->pes_feed.pipe_in)==NULL ) {
        /* if there is no queued PES bytes, no elements in the pending fifo, but application FIFO full, then clogged media filter (e.g. got a frame that is larger then  playback buffer) */
        BDBG_WRN(("b_pump_demux_advance: %#lx congestion detected in the media filter, clearing[%u]", (unsigned long)demux, demux->pes_feed.forced_clear_events));
        bmedia_filter_clear(demux->filter);
        demux->pes_feed.forced_clear_events ++;
        b_pump_demux_stream_error(demux);
    }
    /* 3. feed data to the playback hardware */
    b_pump_demux_advance_recycle(demux);
    demux->pes_feed.pes_bytes += bsink_playback_feed(demux->sink, demux->pes_feed.pipe_in);
    /* 4. verify if flush is queued */
    if(demux->pes_feed.flush_queued) {
        if(!demux->pes_feed.feed_stall && batom_pipe_peek(demux->pes_feed.pipe_in)==NULL && batom_pipe_peek(demux->pipe_demux)==NULL) {
            BDBG_MSG(("b_pump_demux_advance: %#lx loop flush", (unsigned long)demux)); /* player sends an empty entry if it's about to loop a stream */
            demux->pes_feed.flush_queued = false;
            bmedia_filter_flush(demux->filter);
            b_pump_demux_advance_recycle(demux);
        }
    }
    /* 5. do a callback to application */
    if (demux->pes_feed.recycle_count) {
        BDBG_MSG_TRACE(("b_pump_demux_advance_feed: %#lx read_callback %u", (unsigned long)demux, demux->pes_feed.recycle_count));
        demux->pes_feed.recycle_count = 0;
        b_playpump_p_do_read_callback(demux->pump);
    }
    BDBG_MSG_TRACE(("b_pump_demux_advance<: %#lx", (unsigned long)demux));
}

void
b_pump_demux_reclaim(b_pump_demux_t demux)
{
    bsink_playback_status sink_status;
    size_t pes_bytes;

    BDBG_MSG_TRACE(("b_pump_demux_reclaim>: %#lx", (unsigned long)demux));
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);

    if(!demux->pes_feed.pipe_in) {
        /* run-away event */
        return;
    }

    pes_bytes = bsink_playback_feed(demux->sink, demux->pes_feed.pipe_in);
    BDBG_MSG_TRACE(("b_pump_demux_reclaim: %#lx pes_bytes:%u(%u)", (unsigned long)demux, pes_bytes, demux->pes_feed.pes_bytes));
    demux->pes_feed.pes_bytes += pes_bytes;
    b_pump_demux_advance_recycle(demux);
    bsink_playback_get_status(demux->sink, &sink_status);
    if(sink_status.idle) {
        batom_pipe_t pipe = demux->pipe_demux;
        demux->pes_feed.pes_bytes = 0;
        /* we have played all queued up PES entries, try to push accumulated stream data */
        demux->pes_feed.feed_stall = !(bmedia_filter_feed(demux->filter, pipe));
        b_pump_demux_advance(demux);
        if(demux->pes_feed.pes_bytes==0) {
            unsigned i;
            for(i=0;i<16;i++) {
                if(batom_pipe_peek(pipe)==NULL) { 
                    /* no accumulated data, get new data from FIFO  */
                    b_play_next(demux->pump);
                    break;
                } else {
                    /* push accumulated data */
                    BDBG_MSG(("b_pump_demux_reclaim: %#lx feed accumulated data %u", (unsigned long)demux, i));
                    demux->pes_feed.feed_stall = !(bmedia_filter_feed(demux->filter, pipe));
                    b_pump_demux_advance(demux);
                    if(demux->pes_feed.pes_bytes>0) {
                        /* exit if constructed PES data */
                        break;
                    }
                }
            }
        }
    }
    BDBG_MSG_TRACE(("b_pump_demux_reclaim<: %#lx %s pes_bytes:%u", (unsigned long)demux, sink_status.idle?"IDLE":"", demux->pes_feed.pes_bytes));
}


void
b_pump_demux_enqueue(b_pump_demux_t demux, struct bpvr_queue_item *item)
{
    BDBG_MSG_TRACE(("b_pump_demux_enqueue>: %#lx %#lx (%#lx:%u)", (unsigned long)demux, (unsigned long)item));
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    if (item->desc.length==0 && item->skip==0) {
        BDBG_MSG(("b_pump_demux_enqueue: %#lx  loop", (unsigned long)demux)); /* player sends an empty entry if it's about to loop a stream */
        demux->pes_feed.flush_queued = true;
    }
    BDBG_ASSERT(BFIFO_WRITE(&demux->pending)==item);
    BFIFO_WRITE_COMMIT(&demux->pending, 1);
    BDBG_MSG_TRACE(("b_pump_demux_enqueue<: %#lx %#lx (%#lx:%u)", (unsigned long)demux, (unsigned long)item));
    return;
}

static void
b_pump_demux_atom_free(batom_t atom, void *user)
{
    struct bpvr_queue_item *item = *(struct bpvr_queue_item **)user;
    BDBG_MSG_TRACE(("b_pump_demux_atom_free>: %#lx %#lx:%u", (unsigned long)atom, (unsigned long)item, item->ref_cnt));
    BSTD_UNUSED(atom);
    BDBG_ASSERT(item->ref_cnt>0);
    item->ref_cnt--;
    BDBG_MSG_TRACE(("b_pump_demux_atom_free>: %#lx %#lx:%u", (unsigned long)atom, (unsigned long)item, item->ref_cnt));
    return;
}

static const batom_user b_pump_demux_atom = {
    b_pump_demux_atom_free,
    sizeof(struct bpvr_queue_item **)
};

bool
b_pump_demux_feed(b_pump_demux_t demux, struct bpvr_queue_item *item, const void *data, size_t len)
{
    batom_t atom;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_feed>: %#lx %#lx (%#lx:%u) %02X %02X %02X %02X", (unsigned long)demux, (unsigned long)item, (unsigned long)data, len, (unsigned)((uint8_t*)data)[0], (unsigned)((uint8_t*)data)[1], (unsigned)((uint8_t*)data)[2], (unsigned)((uint8_t*)data)[3]));

    atom = batom_from_range(demux->factory, data, len, &b_pump_demux_atom, &item);
    if(atom) {
        batom_pipe_t pipe = demux->pipe_demux;

        item->ref_cnt++;
        demux->pes_feed.pes_bytes = 0;
        batom_pipe_push(pipe, atom);
        demux->pes_feed.feed_stall = !(bmedia_filter_feed(demux->filter, pipe));
        b_pump_demux_advance(demux);
    }
    BDBG_MSG_TRACE(("b_pump_demux_feed<: %#lx %#lx (%#lx:%u)", (unsigned long)demux, (unsigned long)item, (unsigned long)data, len));
    return true;
}

void
b_pump_demux_flush(b_pump_demux_t demux)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_flush>: %#lx", (unsigned long)demux));
    b_pump_reset_pes_feed(demux);
    bmedia_filter_flush(demux->filter);
    batom_pipe_flush(demux->pipe_demux);
    bsink_playback_stop(demux->sink);
    b_pump_demux_advance_recycle(demux);
    rc = bsink_playback_start(demux->sink);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    BFIFO_INIT(&demux->pending, demux->pump->item_mem, demux->pump->openSettings.numDescriptors);
    BDBG_MSG_TRACE(("b_pump_demux_flush<: %#lx", (unsigned long)demux));
    return;
}

NEXUS_Error
b_pump_demux_open_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid, unsigned stream_id, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings)
{
    uint16_t pes_pid;
    NEXUS_Error rc;
    b_pid_map *map;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pid);
    switch(pSettings->pidType) {
    case NEXUS_PidType_eAudio:
        map = &demux->audio_map;
        break;
    case NEXUS_PidType_eVideo:
        map = &demux->video_map;
        break;
    default:
        map = &demux->other_map;
        break;
    }
    pes_pid = b_pid_map_alloc(map);
    if(pes_pid==0) {rc=BERR_TRACE(BERR_NOT_SUPPORTED);goto err_pes_pid;}
    BDBG_MSG(("b_pump_demux_open_pid:%#lx mapping media stream:%u to PES ID:%#x", (unsigned long)demux, (unsigned)stream_id, (unsigned)pes_pid));

    pid->media_stream = NULL;
    pid->media_pid = pes_pid;
    pid->stream_id = stream_id;
    return NEXUS_SUCCESS;

err_pes_pid:
    return  rc;
}

NEXUS_Error
b_pump_demux_add_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid)
{
    NEXUS_Error rc;
    bmedia_stream_cfg stream_cfg;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_ASSERT(pid);
    BDBG_MSG(("b_pump_demux_add_pid: %#lx mapping stream %u to pid %#x", (unsigned long)demux, pid->stream_id, pid->media_pid));
    bmedia_stream_default_cfg(&stream_cfg);
    stream_cfg.reorder_timestamps = pid->settings.allowTimestampReordering;
    pid->media_stream = bmedia_filter_stream_open(demux->filter, pid->stream_id, pid->media_pid, &stream_cfg);
    if(!pid->media_stream) {rc=BERR_TRACE(BERR_NOT_SUPPORTED);goto err_media_stream;}
    return NEXUS_SUCCESS;

err_media_stream:
    return rc;
}

void
b_pump_demux_close_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid)
{
    unsigned media_pid;
    b_pid_map *map;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_ASSERT(pid);

    media_pid = pid->media_pid;
    if(media_pid>=demux->video_map.base_pid) {
        map = &demux->video_map;
    } else if (media_pid>=demux->other_map.base_pid) {
        map = &demux->other_map;
    } else {
        map = &demux->audio_map;
    }
    b_pid_map_free(map, media_pid);
    pid->media_pid = 0;
    pid->stream_id = 0;
    return;
}

void
b_pump_demux_remove_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid)
{
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_ASSERT(pid);

    if(!pid->media_stream) {
        return;
    }
    bmedia_filter_stream_close(demux->filter, pid->media_stream);
    pid->media_stream = NULL;
    return;
}

NEXUS_Error
b_pump_demux_set_stream_type(b_pump_demux_t demux, NEXUS_TransportType transportType, bool *supported)
{
    bstream_mpeg_type mediaType;
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);

    BDBG_ASSERT(supported);
    *supported = false;
    if(demux->pes_feed.pipe_in) {
        bmedia_filter_stop(demux->filter);
        demux->pes_feed.pipe_in = NULL;
    }
    switch(transportType) {
    case NEXUS_TransportType_eMkv:
    case NEXUS_TransportType_eMp4:
    case NEXUS_TransportType_eApe:
        /* player does conversion of those to PES  */
        mediaType = bstream_mpeg_type_pes;
        break;
#if !B_HAS_NATIVE_MPEG1
    case NEXUS_TransportType_eMpeg1Ps:
        mediaType = bstream_mpeg_type_mpeg1;
        break;
#endif
    case NEXUS_TransportType_eAvi:
        mediaType = bstream_mpeg_type_avi;
        break;
    case NEXUS_TransportType_eAsf:
        mediaType = bstream_mpeg_type_asf;
        break;
    case NEXUS_TransportType_eWav:
        mediaType = bstream_mpeg_type_wav;
        break;
    case NEXUS_TransportType_eAiff:
        mediaType = bstream_mpeg_type_aiff;
        break;
    case NEXUS_TransportType_eMp4Fragment:
        mediaType = bstream_mpeg_type_mp4_fragment;
        break;
    case NEXUS_TransportType_eRmff:
        mediaType = bstream_mpeg_type_rmff;
        break;
    case NEXUS_TransportType_eFlv:
        mediaType = bstream_mpeg_type_flv;
        break;
    case NEXUS_TransportType_eOgg:
        mediaType = bstream_mpeg_type_ogg;
        break;
    case NEXUS_TransportType_eFlac:
        mediaType = bstream_mpeg_type_flac;
        break;
    default:
        return NEXUS_SUCCESS;
    }
    demux->pes_feed.pipe_in = bmedia_filter_start(demux->filter, mediaType);
    if(!demux->pes_feed.pipe_in) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    *supported = true;
    return NEXUS_SUCCESS;
}


NEXUS_Error
b_pump_demux_start(b_pump_demux_t demux)
{
    NEXUS_Error rc = 0;
    NEXUS_P_PlaypumpPidChannel *pid;
    bool supported;

    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_MSG_TRACE(("b_pump_demux_start>: %#lx", (unsigned long)demux));

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    if(demux->pump->settings.securityContext==NULL) {
       demux->dcrypt_ctx = NULL;
    }
#else
    if (0) { }
#endif
#if B_HAS_DIVX_DRM
    else if (demux->pump->settings.transportType == NEXUS_TransportType_eAvi || demux->pump->settings.transportType == NEXUS_TransportType_eMkv){        
        BDBG_CASSERT( sizeof(demux->dcrypt_ctx) == sizeof(demux->pump->settings.securityContext));
        demux->dcrypt_ctx = (void *)demux->pump->settings.securityContext; /* copy the key */

        if (bdrm_start(demux->dcrypt_ctx, NULL)){
            bdrm_stop(demux->dcrypt_ctx);
            demux->dcrypt_ctx = NULL;
	    rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err_stream;
        }
    }
#endif
    else {
#if B_HAS_MSDRM_PD
        NEXUS_KeySlotHandle key = (NEXUS_KeySlotHandle )demux->pump->settings.securityContext;
        BDBG_CASSERT( sizeof(demux->dcrypt_ctx) == sizeof(demux->pump->settings.securityContext));
        demux->dcrypt_ctx = (void *)key->drmContext;

#endif
    }

    if(!demux->pes_feed.pipe_in) {
        rc = b_pump_demux_set_stream_type(demux, demux->pump->settings.transportType, &supported);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_stream;}
        BDBG_ASSERT(supported);
    }
    for(pid=BLST_S_FIRST(&demux->pump->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        BDBG_ASSERT(pid->media_stream == NULL);
        rc = b_pump_demux_add_pid(demux, pid);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_pid;}
    }
    rc = bsink_playback_start(demux->sink);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_sink_start; }

    BDBG_MSG_TRACE(("b_pump_demux_start<: %#lx", (unsigned long)demux));
    return NEXUS_SUCCESS;

err_sink_start:
err_pid:
    for(pid=BLST_S_FIRST(&demux->pump->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        if(pid->media_stream) {
            b_pump_demux_remove_pid(demux, pid);
        }
    }
err_stream:
    return rc;
}

bool
b_pump_demux_is_empty(b_pump_demux_t demux)
{
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);

    return demux->pes_feed.pes_bytes == 0;
}


void
b_pump_demux_descriptor(b_pump_demux_t demux, const NEXUS_PlaypumpSegment *desc)
{
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_ASSERT(desc);
    bmedia_filter_seek(demux->filter, desc->offset);
    if(desc->timestamp!=0) {
        unsigned i;
        for(i=0; i<sizeof(desc->timestamp_delta)/sizeof(desc->timestamp_delta[0]); i++){
            if(desc->timestamp_delta[i].stream_id) {
                bmedia_filter_set_offset(demux->filter, desc->timestamp + desc->timestamp_delta[i].timestamp_delta, desc->timestamp_delta[i].stream_id);
            }
        }
    }
    return;
}

void
b_pump_demux_set_rate(b_pump_demux_t demux, int rate)
{
    BDBG_OBJECT_ASSERT(demux, b_pump_demux_t);
    BDBG_CASSERT( (256 / (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE) == (256 * BMEDIA_TIME_SCALE_BASE)/NEXUS_NORMAL_PLAY_SPEED));
    bmedia_filter_set_timescale(demux->filter, rate/(NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE));
    bmedia_filter_set_keyframeonly(demux->filter, rate!=NEXUS_NORMAL_PLAY_SPEED);
}

bool 
b_pump_demux_is_congested(b_pump_demux_t demux)
{
    return demux->pes_feed.flush_queued;
}

#endif /* B_HAS_MEDIA */

