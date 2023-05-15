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
 * $brcm_Workfile: nexus_playpump_impl.h $
 * $brcm_Revision: 31 $
 * $brcm_Date: 10/1/12 3:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_playpump_impl.h $
 * 
 * 31   10/1/12 3:49p vsilyaev
 * SW7425-4013: Verify arguments of NEXUS_Playpump_WriteComplete call
 * 
 * 30   7/30/12 11:02a erickson
 * SW7358-266: remove pidch ref_cnt from playpump/playback.
 *  NEXUS_Playpump_P_OpenPid will validate, which also allows
 *  pidChannelIndex options to be used.
 * 
 * 29   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 28   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 * 
 * 27   9/14/11 12:11p jtna
 * SW7420-2054: enforce boundsHeap for scatter-gather
 * 
 * 26   7/21/11 2:28p jtna
 * SW7125-1015: added NEXUS_Playpump_SubmitScatterGatherDescriptor()
 * 
 * 25   12/28/10 5:44p vsilyaev
 * SW7425-39: Added Mux specific interfaces
 * 
 * 24   12/16/10 6:11p vsilyaev
 * SW7425-39: Added playpump private API
 * 
 * 23   12/16/10 5:05p erickson
 * SW7125-763: restore micropause after flush
 * 
 * 22   12/7/10 5:46p erickson
 * SW7420-1148: added null_allowed, boundsHeap
 * 
 * 21   8/25/10 6:31p vsilyaev
 * SWGIGGS2S-1166: Don't consume new data when flush is queued
 * 
 * 20   6/18/10 2:04p VISHK
 * SW7335-717: Pid Mapping is an Array Max 16. We need to increase this
 * 
 * 18   4/9/10 1:07p jgarrett
 * SW7405-4034: Moving MSDRM PD/ND binaries into nexus as required
 * 
 * DrmMakefileUpdates/1   4/8/10 4:46p jgarrett
 * SW7405-4034: Including binaries for MSDRM (PD/ND) in the nexus builds
 *  to avoid link issues
 * 
 * 17   12/11/09 3:31p erickson
 * SW7550-112: convert to standard XPT define
 *
 * 16   11/3/09 9:56a erickson
 * SW7405-3308: add TEI error callback
 *
 * 15   10/1/09 5:03p erickson
 * SW7405-3087: add playpump cc check, add cc error counting per pid
 *
 * 14   1/23/09 12:40p gmohile
 * PR 51374 : Added congestion detection in throttle timer
 *
 * 13   12/30/08 8:55a vsilyaev
 * PR 50606: Added hooks for hardware support of MPEG1 system streams
 *
 * 12   12/4/08 3:29p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 *  stream parsing
 *
 * 11   10/28/08 2:01p vsilyaev
 * PR 48003: Merge up small chunks to reduce use of descriptors
 *
 * 10   10/27/08 11:20a erickson
 * PR47232: added pacingTsRangeError to NEXUS_PlaypumpStatus
 *
 * 9   10/20/08 6:34p vsilyaev
 * PR 47330: When doing native playback (MPEG-2 TS/PES/ES) then queue
 *  multiple S/G entries in the playback HW
 *
 * 8   6/17/08 11:26a vsilyaev
 * PR 42739: Fixed NO PVR build
 *
 * 7   6/17/08 10:53a vsilyaev
 * PR 42739: Added support for duplicate pids
 *
 * 6   5/14/08 1:24p vsilyaev
 * PR 42119: Preserve PES->TS packetizer settings over
 *  Playpump_Stop/Playpump_Start calls
 *
 * 5   5/7/08 2:00p vsilyaev
 * PR 41869: Added core for re-packetize MPEG-2 TS data for encrypted PVR
 *
 * 4   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 3   3/31/08 4:41p erickson
 * PR40817: compile with PVR_SUPPORT=n
 *
 * 2   2/28/08 9:44p vsilyaev
 * PR 40103: Used NEXUS_TaskCallback functions for API callbacks
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/18   12/20/07 2:07p vsilyaev
 * PR 37574: Added playRate for playpump
 *
 * Nexus_Devel/17   12/6/07 5:23p vsilyaev
 * PR 37574: Added handling of segmented feed
 *
 * Nexus_Devel/16   12/5/07 3:51p vsilyaev
 * PR 37574: User better pid allocation scheme
 *
 * Nexus_Devel/15   12/5/07 12:44p vsilyaev
 * PR 37574: Enabled media support
 *
 * Nexus_Devel/14   12/4/07 6:19p vsilyaev
 * PR 37574: Added more bmedia support
 *
 * Nexus_Devel/13   11/26/07 4:46p vsilyaev
 * PR 37574: Integrated bmedia support
 *
 * Nexus_Devel/12   10/31/07 9:07a erickson
 * PR36570: added 3563 support
 *
 * Nexus_Devel/11   10/23/07 4:17p vsilyaev
 * PR 36199: Use PVR_Feed inseted of PVR_Play
 *
 * Nexus_Devel/10   10/19/07 7:21p vsilyaev
 * PR 36291: Replaced linked list management with FIFO
 *
 * Nexus_Devel/9   10/19/07 4:05p vsilyaev
 * PR 36291: Added datatype for S/G playpump
 *
 * Nexus_Devel/8   10/17/07 5:36p vsilyaev
 * PR 35824: Fixed feedback from decoder to playpump
 *
 * Nexus_Devel/7   10/9/07 2:10p vsilyaev
 * PR 35824: Reworked playpump API
 *
 * Nexus_Devel/6   10/9/07 11:04a vsilyaev
 * PR 34662: Rearranged inclusion order of header files
 *
 * Nexus_Devel/5   10/8/07 6:14p vsilyaev
 * PR 35824: Coded packetizer support
 *
 * Nexus_Devel/4   10/8/07 1:42p vsilyaev
 * PR 35824: Implemented NEXUS_Playpump_Open/ClosePidChannel
 *
 * Nexus_Devel/3   9/26/07 4:57p erickson
 * PR34926: internal function cleanup
 *
 * Nexus_Devel/2   9/26/07 4:17p erickson
 * PR34925: playpump updates
 *
 * Nexus_Devel/1   9/26/07 3:11p erickson
 * PR34925: initial playpump checkin
 *
 **************************************************************************/
#ifndef NEXUS_PLAYPUMP_IMPL_H__
#define NEXUS_PLAYPUMP_IMPL_H__

#include "nexus_transport_module.h"

#ifndef B_HAS_MEDIA
#define B_HAS_MEDIA 1
#endif

#if B_HAS_MEDIA
#include "bmedia_filter.h"
#endif

#include "bpvrlib_feed.h"
#if BXPT_HAS_DIRECTV_SUPPORT
#include "bxpt_directv_playback.h"
#endif
#include "bfifo.h"
#if 0 && B_HAS_DRM
#include "drmdecryptor.h"
#endif
#if 0 && B_HAS_DRM_ND
#include "bdrmnd_decryptor.h"
#endif

/* setting this to 1 will save data that goes to the playpump */
#define B_SAVE_PLAYPUMP 0

/* We have a separate dbg macro for data flow. When you want to see it, uncomment
the following */
#define BDBG_MSG_FLOW(x) /* BDBG_MSG(x) */

#define B_THROTTLE_TIMEOUT  30

/* we use private1 id */
#define B_PES_ID    0xBD
#define B_PES_SIZE_MAX  (16384-6)

/* PVR atom mutlilpier of both transport packet size, filesyste block size  and filesystem block size */
#define B_PVR_ATOM_SIZE ((188/4)*4096)
#define B_PVR_N_ATOMS  12

/* SCD atom is enough to hold about 3 frames worth of data */
#define B_PVR_SCD_SIZE  (3*4*32)

/* This is the size of the buffer we'll allocate for playback operations */
#define B_PVR_PLAYBACK_BUFFER (B_PVR_ATOM_SIZE*8) /* = 1540096 or 1504K or 1.5MB */

/* Number of descriptors for playback,  this is should correspond to number of frames in the
   playback buffer, plus multiplier for 1 frame rewind.
   For Broadcom trick modes, we need one for every BTP and two for every frame,
   so that's why we increased it dramatically. */
#define NEXUS_NUM_PLAYBACK_DESC 100

/* all new chips have native support for MPEG1 System streams */
/* #define B_HAS_NATIVE_MPEG1  1 */

struct bpvr_queue_item {
    NEXUS_PlaypumpDesc desc;
    size_t skip;
    unsigned ref_cnt;
    bool sg; /* app submitted scatter gather descriptor via NEXUS_PlaypumpMode_eScatterGather or NEXUS_Playpump_SubmitScatterGatherDescriptor() */
};

/* current might have beed '#define'ed to something else, clear it */
#undef current

BFIFO_HEAD(bpvr_queue, struct bpvr_queue_item);

enum b_play_packetizer {b_play_packetizer_none=0, b_play_packetizer_media, b_play_packetizer_crypto};


typedef struct b_pid_map {
    unsigned base_pid;
    uint8_t map[32];
} b_pid_map;

void b_pid_map_init(b_pid_map *map, uint16_t base);
uint16_t b_pid_map_alloc(b_pid_map *map);
void b_pid_map_free(b_pid_map *map, uint16_t pid);

typedef struct NEXUS_P_PlaypumpPidChannel {
    BLST_S_ENTRY(NEXUS_P_PlaypumpPidChannel) link;
    NEXUS_PidChannelHandle pid_channel;
#if B_HAS_MEDIA
    bmedia_stream_t media_stream;
    unsigned stream_id; /* stream id */
    uint16_t media_pid; /* mapped pes id */
#endif
    NEXUS_PlaypumpOpenPidChannelSettings settings;
    struct {
        bool enable;
        bool active;
        BXPT_Playback_PacketizeConfig cfg;
        unsigned context;
    } packetizer;
    unsigned pid;
} NEXUS_P_PlaypumpPidChannel;

BDBG_OBJECT_ID_DECLARE(NEXUS_Playpump);

typedef struct b_pump_demux *b_pump_demux_t;
typedef struct b_pump_crypto *b_pump_crypto_t;

struct NEXUS_Playpump {
    NEXUS_OBJECT(NEXUS_Playpump);
    unsigned index;
    BFIFO_HEAD(NEXUS_Playpump_P_Fifo, uint8_t) fifo;
    struct bpvr_queue activeFifo;  /* FIFO of descriptors with data */
    struct bpvr_queue pendingFifo; /* FIFO of descriptors with data not submitted to hardware */
    NEXUS_TaskCallbackHandle dataCallback;
    NEXUS_TaskCallbackHandle errorCallback;
    NEXUS_IsrCallbackHandle ccErrorCallback;
    NEXUS_IsrCallbackHandle teiErrorCallback;

    BLST_S_HEAD(NEXUS_P_PlaypumpPidList, NEXUS_P_PlaypumpPidChannel) pid_list;

    NEXUS_PlaypumpSettings settings;
    NEXUS_PlaypumpOpenSettings openSettings;

    void *buf; /* cached address used through */
    void *buf_noncached; /* non-cached address returned by MEM */

    void *boundsHeapAddr;
    size_t boundsHeapSize;

    bool consumerStarted;
    bool paused;

    BXPT_Playback_Handle xpt_play;
    BPVRlib_Feed_Handle play_feed;
    BKNI_EventHandle descEvent;
    NEXUS_EventCallbackHandle playEventHandle;
    BINT_CallbackHandle pacingErrIntCallback;
    BINT_CallbackHandle ccErrorInt;
    BINT_CallbackHandle teiErrorInt;
    BMEM_Heap_Handle heap;

    NEXUS_TimerHandle throttle_timer;

#if B_HAS_MEDIA
    b_pump_demux_t demux;
    bool use_media;
#endif
    b_pump_crypto_t crypto;

    struct {
        bool running;
        bool pacing; /* pacing was enabled and needed to be stopped */
        bool muxInput; /* used as mux input */
        void *last_addr; /* needed for flush and verification of user arguments in NEXUS_Playpump_WriteComplete */
        size_t last_size; /* needed for verification of user arguments in NEXUS_Playpump_WriteComplete */
        struct bpvr_queue_item *last_item; /* last item added into the playpump, but not send to hardware, it's used for merging data into the last chunk */
        enum b_play_packetizer packetizer; /* PES packetizer is active */
        NEXUS_PlaypumpDesc active; /* active transaction */
        NEXUS_PlaypumpDesc current;
        size_t segment_left;
        unsigned queued_in_hw; /* number of elements queued in the HW */
        unsigned pacingTsRangeError;
        uint64_t bytes_played;
    } state;

    b_pid_map packetizer_map;

    struct {
        NEXUS_AudioCodec codec;
        unsigned track;
    } vob_remap_state;

#if B_HAS_PLAYPUMP_IP
    struct b_playpump_ip_impl ip;
#endif
    struct bpvr_queue_item *item_mem;
};


void b_playpump_p_xpt_event(void *playback_);
void b_playpump_p_do_read_callback(NEXUS_PlaypumpHandle p);
void b_playpump_p_stop(NEXUS_PlaypumpHandle p);
void b_playpump_p_reset(NEXUS_PlaypumpHandle p);
void b_playpump_p_throttle_timer(void *play);
bool b_play_next(NEXUS_PlaypumpHandle p);

BERR_Code b_playpump_p_add_request(NEXUS_PlaypumpHandle p, size_t skip, size_t amount_used, const NEXUS_PlaypumpScatterGatherDescriptor *pSgDesc);

void NEXUS_P_Playpump_DescAvail_isr(void *p, int unused);


#if B_SAVE_PLAYPUMP
void b_playpump_p_data(const void *data, size_t len);
void b_playpump_p_data_flush(void);
#endif

void b_pump_crypto_enqueue(b_pump_crypto_t crypto, struct bpvr_queue_item *item);
bool b_pump_crypto_feed(b_pump_crypto_t crypto, struct bpvr_queue_item *item, const void *data, size_t len);
bool b_pump_crypto_is_empty(b_pump_crypto_t crypto);
NEXUS_Error b_pump_crypto_start(b_pump_crypto_t crypto);
NEXUS_Error b_pump_crypto_set_stream_type(b_pump_crypto_t crypto, NEXUS_TransportType transportType, bool *supported);
void b_pump_crypto_flush(b_pump_crypto_t crypto);
void b_pump_crypto_reclaim(b_pump_crypto_t crypto);
void b_pump_crypto_stop(b_pump_crypto_t crypto);
void b_pump_crypto_status(b_pump_crypto_t crypto);
void b_pump_crypto_destroy(b_pump_crypto_t crypto);
b_pump_crypto_t b_pump_crypto_create(NEXUS_PlaypumpHandle pump);

#if B_HAS_MEDIA
void b_pump_demux_reclaim(b_pump_demux_t demux);
void b_pump_demux_enqueue(b_pump_demux_t demux, struct bpvr_queue_item *item);
bool b_pump_demux_feed(b_pump_demux_t demux, struct bpvr_queue_item *item, const void *data, size_t len);
void b_pump_demux_flush(b_pump_demux_t demux);
void b_pump_demux_stop(b_pump_demux_t demux);
b_pump_demux_t b_pump_demux_create(NEXUS_PlaypumpHandle pump);
void b_pump_demux_destroy(b_pump_demux_t demux);
void b_pump_demux_status(b_pump_demux_t demux, NEXUS_PlaypumpStatus *pStatus);
NEXUS_Error b_pump_demux_set_stream_type(b_pump_demux_t demux, NEXUS_TransportType transportType, bool *supported);
NEXUS_Error b_pump_demux_start(b_pump_demux_t demux);
NEXUS_Error b_pump_demux_open_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid, unsigned stream_id, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings);
bool b_pump_demux_is_empty(b_pump_demux_t demux);
void b_pump_demux_close_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid);
NEXUS_Error b_pump_demux_add_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid);
void b_pump_demux_remove_pid(b_pump_demux_t demux, NEXUS_P_PlaypumpPidChannel *pid);
void b_pump_demux_descriptor(b_pump_demux_t demux, const NEXUS_PlaypumpSegment *desc);
void b_pump_demux_set_rate(b_pump_demux_t demux, int rate);
void b_pump_demux_advance(b_pump_demux_t demux);
bool b_pump_demux_is_congested(b_pump_demux_t);
#endif /* B_HAS_MEDIA */


#endif /* NEXUS_PLAYPUMP_IMPL_H__ */

