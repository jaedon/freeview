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
 * $brcm_Workfile: nexus_record.c $
 * $brcm_Revision: 44 $
 * $brcm_Date: 6/11/12 12:33p $
 *
 * Module Description:
 *  PVR record module
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/record/src/nexus_record.c $
 * 
 * 44   6/11/12 12:33p erickson
 * SW7358-325: default navVersion to BNAV_Version_TimestampOnly if there
 *  is no video codec
 * 
 * 43   5/9/12 11:41a ahulse
 * SW7425-2744: Use actual rave fifodepth to detect empty buffer
 * 
 * 42   2/24/12 10:59a erickson
 * SW7425-2458: NEXUS_Record_RemoveAllPidChannels should be clean if no
 *  pid channels
 * 
 * 41   2/22/12 4:27p erickson
 * SW7425-2458: call Stop from NEXUS_Record_Close
 * 
 * 40   2/22/12 3:58p erickson
 * SW7425-2458: call NEXUS_Record_RemoveAllPidChannels in Close
 * 
 * 39   2/21/12 3:42p erickson
 * SW7425-2439: add comment about code duplication
 * 
 * 38   2/6/12 1:25p erickson
 * SW7425-2325: add NEXUS_RecordSettings.pollingTimer
 * 
 * 37   1/5/12 12:47p erickson
 * SW7231-578: add NEXUS_RecordStatus.indexParsingErrors
 *
 * 36   9/15/11 10:34a erickson
 * SW7405-5489: initialize flow->state, but also add asserts so we aren't
 *  covering up a reentrancy bug.
 *
 * 35   8/18/11 4:33p hongtaoz
 * SW7425-1144: continue to consume data when band hold enabled and single
 *  data write size is limited to support faster NRT transcode;
 *
 * 34   7/21/11 2:48p jtna
 * SWDTV-7995: Coverity Warning BAD_SIZEOF
 *
 * 33   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 *
 * 32   3/4/11 5:55p vsilyaev
 * SW7422-14: Added mapping of SVC/MVC video types
 *
 * 31   11/11/10 5:49p vsilyaev
 * SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
 *  GetDefaultSettings
 *
 * 30   8/10/10 4:08p erickson
 * SW7420-934: rename NEXUS_Recpump_DataWriteComplete/IndexWriteComplete
 *  to NEXUS_Recpump_DataReadComplete/IndexReadComplete
 *
 * 29   6/24/10 9:51a erickson
 * SW7335-782: fix bad NEXUS_UnlockModule for index writes in
 *  NEXUS_Record_Stop
 *
 * 28   6/21/10 4:32p erickson
 * SW7405-4249: added indexing support for NEXUS_PidType_eOther
 *
 * 27   6/2/10 5:31p erickson
 * SW7400-2789: add NEXUS_RecordSettings.allowLargeTimeGaps
 *
 * 26   5/18/10 11:57a erickson
 * SW7405-4193: support timestamp TS record
 *
 * 25   4/2/10 2:37p erickson
 * SW7405-3833: nexus_file_pvr.h is now part of the public API
 *
 * 24   3/29/10 1:18p gmohile
 * SW7405-4128 : Check GetPosition return code before updating timestamp
 *
 * 23   3/26/10 10:00a katrep
 * SW7405-355:Add AVS record support
 *
 * 22   10/9/09 11:43a vsilyaev
 * SW7405-3126: Scale down reported priority if last write wasn't comple
 *  write
 *
 * 21   10/9/09 11:28a vsilyaev
 * SW7405-3126: Continue write operatrion until entire block is written
 *
 * 20   9/17/09 10:11a erickson
 * SW7405-3026: allow Record module to build and run without Playback
 *  module
 *
 * 19   7/31/09 12:46p erickson
 * PR57268: remove dead flow->info.data.throwAway code from nexus_record,
 *  clean up leftover Settop API comments
 *
 * 18   6/4/09 3:23p erickson
 * PR54129: don't align to 4K block if not O_DIRECT
 *
 * 17   4/21/09 3:32p erickson
 * PR45902: playback module is optional. only required for timeshifting.
 *  add NEXUS_RecordModule_SetPlaybackModule for dynamic changes. add
 *  runtime check.
 *
 * 16   2/23/09 11:10a erickson
 * PR52034: added NEXUS_RecordStatus.picturesIndexed
 *
 * 15   1/7/09 9:49p erickson
 * PR49501: added NEXUS_Record_StartAppend
 *
 * 14   12/17/08 2:33p erickson
 * PR50461: add memset to NEXUS_Record_GetDefaultSettings
 *
 * 13   12/17/08 1:41p erickson
 * PR50461: implement NEXUS_RecordSettings.writeAllTimeout, allow for a
 *  non-atomSize aligned last read when stopping
 *
 * 12   12/12/08 5:11p erickson
 * PR50347: allow index-only recording
 *
 * 11   9/16/08 11:34a erickson
 * PR46783: protect NEXUS_Record_GetStatus if there is no recpump
 *
 * 10   9/12/08 4:32p mphillip
 * PR46711: Set recpump settings on every call, not just the first call.
 *
 * 9   7/29/08 2:35p erickson
 * PR45238: DataReadyCallbacks must check if Record is waiting on File
 *  i/o. This fixes a bug introduced by the Timer, but also protects us
 *  from extra XPT isr callbacks.
 *
 * 8   7/2/08 2:55p erickson
 * PR41619: add timer to NEXUS_Record to improve timeshifting for low
 *  bitrate streams
 *
 * 7   6/25/08 6:09p vsilyaev
 * PR 41869: Added user provided recpumpSettings to allow set secutity
 *  context
 *
 * 6   5/28/08 2:31p erickson
 * PR42329: NEXUS_Record_P_GetPriority may be called after Recpump has
 *  stopped. Handle this case without an assert.
 *
 * 5   5/22/08 3:27p jgarrett
 * PR 42329: Re-enabling callbacks on start, dropping stale callbacks.
 *
 * 4   5/7/08 4:21p erickson
 * PR42329: use NEXUS_StopCallbacks instead of sleeps and loops inside
 *  NEXUS_Record_Stop
 *
 * 3   3/8/08 7:45a erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 2   2/7/08 10:20a erickson
 * PR39384: added recpump dataReadyThreshold and atomSize. clarified
 *  dataReadyCallback behavior.
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/

#include "nexus_record_module.h"
#include "blst_slist.h"
#include "bfile_io.h"
#include "bcmindexer.h"
#include "nexus_file_pvr.h"
#if NEXUS_HAS_PLAYBACK
#include "priv/nexus_playback_notify.h"
#endif
#include "nexus_core_utils.h"

BDBG_MODULE(nexus_record);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

typedef struct NEXUS_Record_P_PidChannel {
    BLST_S_ENTRY(NEXUS_Record_P_PidChannel) link;
    NEXUS_PidChannelHandle  pidChn;
    NEXUS_RecordPidChannelSettings cfg;
} NEXUS_Record_P_PidChannel;

typedef struct NEXUS_Record_P_Playback {
    BLST_S_ENTRY(NEXUS_Record_P_Playback) link;
    NEXUS_PlaybackHandle playback;
} NEXUS_Record_P_Playback;

BDBG_OBJECT_ID(NEXUS_Record);
BDBG_OBJECT_ID(NEXUS_Record_P_Flow);

typedef struct NEXUS_Record_P_Flow {
    BDBG_OBJECT(NEXUS_Record_P_Flow)
    NEXUS_FileWriteHandle file;
    const void *buf;
    enum {
        Waiting,
        Writing, /* waiting for async File callback */
        Done,   /* brecord_stop has been recognized and we're exiting */
        Terminated, /* abnormal termination */
        Idle /* used for index write to recycle CPU cycles */
    } state;
    bool stopping;  /* inside NEXUS_Record_Stop, we call NEXUS_Recpump_StopData and process callbacks for remaining data. */
    bool partialWrite; /* set to true if last write was partial, this is used to lower reported priority */
    size_t size;
    union {
        struct {
            NEXUS_TimerHandle timer; /* timer used in the indexer write */
            BNAV_Indexer_Handle indexer; /* this field only used for scd record and only if we were told to use bcmindexer */
        } index;
        struct {
            size_t blockSize; /* size of the data */
            size_t blockOffset; /* current offset */
        } data;
    } info;
    NEXUS_RecordHandle record; /* link back to parent */
    NEXUS_CallbackHandler dataReady;
    NEXUS_CallbackHandler overflow;
    bool emptyBuffer;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
    bool bRecordWriteSizeLimit;
#endif
} NEXUS_Record_P_Flow;


struct NEXUS_Record {
    BDBG_OBJECT(NEXUS_Record)
    BLST_S_HEAD(NEXUS_Record_P_PidChannels, NEXUS_Record_P_PidChannel) pid_list;
    BLST_S_HEAD(NEXUS_Record_P_Playbacks, NEXUS_Record_P_Playback) playback_list;
    NEXUS_RecordSettings cfg;
    bool started;
    NEXUS_Record_P_Flow data, index;
    NEXUS_RecordErrorType errorType;
    size_t entry_size;
    unsigned lastTimestamp;
    NEXUS_TaskCallbackHandle errorCallback, overflowCallback;
    NEXUS_TimerHandle processDataTimer;
    unsigned picturesIndexed;
};

static void NEXUS_Record_P_DataReadyCallback(void *context);
static void NEXUS_Record_P_IndexReadyCallback(void *context);
static void NEXUS_Record_P_Overflow(void *context);
static void NEXUS_Record_P_ProcessDataTimer(void *context);

static void
NEXUS_Record_P_InitFlow(NEXUS_RecordHandle record, NEXUS_Record_P_Flow *flow)
{
    flow->state = Idle;
    flow->stopping = false;
    flow->partialWrite = false;
    flow->info.index.timer = NULL;
    flow->info.index.indexer = NULL;
    flow->record = record;
    return;
}

void
NEXUS_Record_GetDefaultSettings(NEXUS_RecordSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->indexFormat = NEXUS_RecordIndexType_eBcm;
    NEXUS_CallbackDesc_Init(&pSettings->overflowCallback);
    NEXUS_CallbackDesc_Init(&pSettings->errorCallback);
    NEXUS_Recpump_GetDefaultSettings(&pSettings->recpumpSettings);
    pSettings->pollingTimer = 250; /* milliseconds */
    return;
}


NEXUS_RecordHandle
NEXUS_Record_Create(void)
{
    NEXUS_RecordHandle record;
    NEXUS_Error rc;

    record = BKNI_Malloc(sizeof(*record));
    if(!record) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BDBG_OBJECT_INIT(record, NEXUS_Record);
#if NEXUS_HAS_PLAYBACK
    BLST_S_INIT(&record->playback_list);
#endif
    BLST_S_INIT(&record->pid_list);
    record->started = false;
    BDBG_OBJECT_INIT(&record->data, NEXUS_Record_P_Flow);
    BDBG_OBJECT_INIT(&record->index, NEXUS_Record_P_Flow);
    record->data.record = record;
    record->index.record = record;
    record->errorType = NEXUS_RecordErrorType_eNone;
    record->entry_size = 0;
    record->lastTimestamp = 0;
    record->errorCallback = NEXUS_TaskCallback_Create(record, NULL);
    record->overflowCallback = NEXUS_TaskCallback_Create(record, NULL);
    record->processDataTimer = NULL;
    NEXUS_CallbackHandler_Init(record->data.dataReady, NEXUS_Record_P_DataReadyCallback, &record->data);
    NEXUS_CallbackHandler_Init(record->index.dataReady, NEXUS_Record_P_IndexReadyCallback, &record->index);
    NEXUS_CallbackHandler_Init(record->data.overflow, NEXUS_Record_P_Overflow, record);
    NEXUS_CallbackHandler_Init(record->index.overflow, NEXUS_Record_P_Overflow, record);
    NEXUS_Record_GetDefaultSettings(&record->cfg);
    return record;
err_alloc:
    return NULL;
}

void
NEXUS_Record_Destroy(NEXUS_RecordHandle record)
{
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    NEXUS_Record_Stop(record);
    NEXUS_Record_RemoveAllPidChannels(record);
    NEXUS_TaskCallback_Destroy(record->errorCallback);
    NEXUS_TaskCallback_Destroy(record->overflowCallback);
    NEXUS_CallbackHandler_Shutdown(record->data.dataReady);
    NEXUS_CallbackHandler_Shutdown(record->index.dataReady);
    NEXUS_CallbackHandler_Shutdown(record->data.overflow);
    NEXUS_CallbackHandler_Shutdown(record->index.overflow);
    BDBG_OBJECT_DESTROY(record, NEXUS_Record);
    BKNI_Free(record);
    return;
}

/* B_RECORD_ATOM_SIZE allows Record to divide up its writes into small enough pieces so that
it doesn't create a latency problem.
It must be a multiple of BIO_BLOCK_SIZE. It does not need to be a multiple of packet size (e.g. 188 bytes). */
#define B_RECORD_ATOM_SIZE  (47*BIO_BLOCK_SIZE)
/* We limit record size to prevent overflow. If a file transaction takes a long time, we
may overflow while we wait. Better to split it up into slightly smaller transactions so that we free
up space sooner. */
#define B_RECORD_WRITE_SIZE_LIMIT (3*B_RECORD_ATOM_SIZE)

/* XXX OS dependent, only used to get last disk/file error code */
#include <errno.h>

static void
NEXUS_Record_P_WriteDone(void *flow_, ssize_t size)
{
    NEXUS_Record_P_Flow *flow = flow_;
    NEXUS_RecordHandle record;
    NEXUS_Error rc;
    NEXUS_Record_P_Playback *play_item;

    if (flow->state != Writing) {
        BDBG_ERR(("Received bad callback from File module"));
        return;
    }

    BDBG_OBJECT_ASSERT(flow, NEXUS_Record_P_Flow);
    record = flow->record;
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(record->cfg.recpump);
    BDBG_MSG_TRACE(("(%#x) completed writing %d(%u) bytes of data from %#x", (unsigned)flow->record, (int)size, flow->info.data.blockSize-flow->info.data.blockOffset, (unsigned)flow->buf+flow->info.data.blockOffset));

    if (size<0) {
        BDBG_ERR(("Can't write data to the file, terminating record: %d", errno));
        switch(errno) {
        case ENOSPC: record->errorType = NEXUS_RecordErrorType_eDiskFull;break;
        case EFBIG:  record->errorType = NEXUS_RecordErrorType_eMaxFileSize;break;
        default:  record->errorType = NEXUS_RecordErrorType_eUnknown;break;
        }
        flow->state = Terminated;
        /* Give recpump signal to stop feeding you data more data otherwise you will
           an overflow real quick. You loose data so its pretty much
           same as overflow, but the system will crash and you can recover from this*/
        NEXUS_Recpump_Stop(record->cfg.recpump);
        NEXUS_TaskCallback_Fire(record->errorCallback);
        return;
    }

#if NEXUS_HAS_PLAYBACK
    play_item=BLST_S_FIRST(&record->playback_list);
    if (play_item) {
        if (!g_NEXUS_Record_P_ModuleState.cfg.modules.playback) {
            BDBG_ERR(("Record module requires Playback module handle for timeshifting."));
        }
        else if (NEXUS_Module_TryLock(g_NEXUS_Record_P_ModuleState.cfg.modules.playback)) {
            for(play_item=BLST_S_FIRST(&record->playback_list);play_item;play_item=BLST_S_NEXT(play_item,link)) {
                NEXUS_Playback_RecordProgress_priv(play_item->playback);
            }
            NEXUS_Module_Unlock(g_NEXUS_Record_P_ModuleState.cfg.modules.playback);
        }
    }
#else
    BSTD_UNUSED(play_item);
#endif

    if (!flow->file) {
        BDBG_WRN(("Detected stop condition, record done"));
        flow->state = Done;
        return;
    }
    if(flow->info.data.blockOffset+size > flow->info.data.blockSize) {
        BDBG_ERR(("Wrote more data then expected: %d %d", flow->info.data.blockSize - flow->info.data.blockOffset, size));
    }
    flow->info.data.blockOffset += size;
    if(flow->info.data.blockOffset >= flow->info.data.blockSize) {
        flow->state = Waiting;
        rc = NEXUS_Recpump_DataReadComplete(record->cfg.recpump, flow->info.data.blockSize); /* completed write */
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
        /* if band hold enabled, call again in case write size was limited, to reduce buffer level away from bandhold threshold.
         * NOTE: we could keep consuming but we also like to reduce data ready callbacks to be efficient. */
        if(record->cfg.recpumpSettings.bandHold && (size == B_RECORD_WRITE_SIZE_LIMIT)) {
            NEXUS_Record_P_DataReadyCallback(flow);
        }
        return;
    } else {
        const uint8_t *buf = (const uint8_t *)flow->buf + flow->info.data.blockOffset;
        size = flow->info.data.blockSize - flow->info.data.blockOffset;
        flow->partialWrite = true;
        BDBG_MSG_TRACE(("(%#x) continue writing %u bytes of data from %#x", (unsigned)flow->record, (unsigned)size, (unsigned)buf));
        NEXUS_File_AsyncWrite(flow->file, buf, size, NEXUS_RecordModule, NEXUS_Record_P_WriteDone, flow);
        return;
    }
}

static void
NEXUS_Record_P_DataReadyCallback(void *context)
{
    NEXUS_Record_P_Flow *flow = context;
    const void *buffer;
    size_t size;
    NEXUS_Error rc;

    if (flow->state == Writing) {
        return;
    }

    BDBG_OBJECT_ASSERT(flow, NEXUS_Record_P_Flow);
    BDBG_OBJECT_ASSERT(flow->record, NEXUS_Record);
    BDBG_ASSERT(flow->record->cfg.recpump);

    if(!flow->record->started) { goto done; }

    rc = NEXUS_Recpump_GetDataBuffer(flow->record->cfg.recpump, &buffer, &size);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto done;}

    flow->emptyBuffer = (size == 0);

    if ( size == 0 )
    {
        /* Possible with a stale callback */
        goto done;
    }

    if (!flow->file) {
        /* we're not writing this data to file. just complete all of it and wait for more. */
        rc = NEXUS_Recpump_DataReadComplete(flow->record->cfg.recpump, size);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
        flow->state = Waiting;
        return;
    }

#ifdef DIRECT_IO_SUPPORT
    if (!flow->stopping) {
        unsigned truncAmount;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SATIP)
		if(size >= BIO_BLOCK_SIZE)
		{
			/* Direct I/O requires multiples of BIO_BLOCK_SIZE */
			truncAmount = size % BIO_BLOCK_SIZE;
			size -= truncAmount;
		}
#else
		/* If Recpump's data.atomSize is working, we will never get less that BIO_BLOCK_SIZE, even on wrap around. */
		BDBG_ASSERT(size >= BIO_BLOCK_SIZE);

        /* Direct I/O requires multiples of BIO_BLOCK_SIZE */
        truncAmount = size % BIO_BLOCK_SIZE;
        size -= truncAmount;
#endif
    }
#endif

    /* We limit record size to prevent overflow. If a file transaction takes a long time, we
    may overflow while we wait. Better to split it up into slightly smaller transactions so that we free
    up space sooner. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
    /* If writing data to HDD media, limit once writing data size as B_RECORD_WRITE_SIZE_LIMIT  
     * Media without Cache memory such as USB Flash Memory, just write data size, not having the limitation.
    */
    if (size>B_RECORD_WRITE_SIZE_LIMIT) {
        if(flow->bRecordWriteSizeLimit == true) {
            BDBG_MSG(("Limiting write size from %d to %d", (int)size, (int)B_RECORD_WRITE_SIZE_LIMIT));
            size = B_RECORD_WRITE_SIZE_LIMIT;
        }
    }
#else
    if (size>B_RECORD_WRITE_SIZE_LIMIT) {
        BDBG_MSG(("Limiting write size from %d to %d", (int)size, (int)B_RECORD_WRITE_SIZE_LIMIT));
        size = B_RECORD_WRITE_SIZE_LIMIT;
    }
#endif

    flow->buf = buffer;
    flow->partialWrite = false;
    flow->info.data.blockOffset = 0;
    flow->info.data.blockSize = size;
    BDBG_MSG_TRACE(("(%#x) writing %u bytes of data from %#x", (unsigned)flow->record, size, (unsigned)flow->buf));
    NEXUS_File_AsyncWrite(flow->file, flow->buf, size, NEXUS_RecordModule, NEXUS_Record_P_WriteDone, flow);
    flow->state = Writing;

done:
    return;
}

static void
NEXUS_Record_P_IdleTimer(void *context)
{
    NEXUS_Record_P_Flow *flow = context;
    NEXUS_RecordHandle record;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(flow, NEXUS_Record_P_Flow);

    BDBG_OBJECT_ASSERT(flow->record, NEXUS_Record);
    record = flow->record;
    BDBG_ASSERT(record->cfg.recpump);

    flow->info.index.timer = NULL;
    flow->state = Waiting;
    rc = NEXUS_Recpump_IndexReadComplete(record->cfg.recpump, flow->size);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    return;
}

static void
NEXUS_Record_P_IndexReadyCallback(void *context)
{
    NEXUS_Record_P_Flow *flow = context;
    const void *buffer;
    size_t size;
    NEXUS_Error rc;
    NEXUS_RecordHandle record;

    BDBG_OBJECT_ASSERT(flow, NEXUS_Record_P_Flow);
    BDBG_OBJECT_ASSERT(flow->record, NEXUS_Record);
    record = flow->record;
    BDBG_ASSERT(record->cfg.recpump);

    if(!record->started) { goto done; }
    rc = NEXUS_Recpump_GetIndexBuffer(record->cfg.recpump, &buffer, &size);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto done;}

    flow->emptyBuffer = (size == 0);

    if ( size == 0 )
    {
        /* Possible with a stale callback */
        goto done;
    }

    BDBG_MSG_TRACE(("(%#x) got %d bytes of index from %#lx", (unsigned long)flow->record, (int)size, (unsigned long)buffer));
    if(flow->info.index.indexer) {
        unsigned sct_size = (record->entry_size == 6)?
            sizeof(BSCT_SixWord_Entry):sizeof(BSCT_Entry);
        unsigned sct_limit;
        size_t old_size;

        sct_limit = 6*4*16/sct_size ; /* we only record one SCT descriptor at the time */

        /* NAV data */
        if (size < sct_size) {
            BDBG_ERR(("Got buffer from record less then size of SCT table, ignore it"));
            goto done;
        }
        size -= size % sct_size;
        old_size = size;
        /* unless we are stopping, limit number of sct entries recordd at the time */
        if (size>sct_limit*sct_size && !flow->stopping) {
            size = sct_limit*sct_size;
        }

#ifdef SCT_WRITE
        BDBG_MSG(("sct: addr %p size %d", buffer, size));
        fwrite(buffer, 1, size, sctfile);
#endif

        BDBG_MSG(("nav: addr %p size %d(%d)", buffer, (int)size, (int)old_size));
        /* nothing */
        BNAV_Indexer_Feed(flow->info.index.indexer, (void*)buffer, size/sct_size);
        flow->size = size;
        if(size<=old_size) {
            flow->state = Idle;
            rc = NEXUS_Recpump_IndexReadComplete(record->cfg.recpump, size);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
        } else {
            flow->state = Waiting;
            flow->info.index.timer = NEXUS_ScheduleTimer(10, NEXUS_Record_P_IdleTimer, flow);
        }
    } else {
        flow->state = Idle;
        rc = NEXUS_Recpump_IndexReadComplete(record->cfg.recpump, size);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    }

done:
    return;
}


static int
NEXUS_Record_P_GetPriority(void *cntx)
{
    NEXUS_RecordHandle record = cntx;
    NEXUS_RecpumpStatus status;
    unsigned factor;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(record->cfg.recpump);

    rc = NEXUS_Recpump_GetStatus(record->cfg.recpump, &status);
    if (rc || status.data.fifoSize == 0) {
        /* If recpump has been stopped, we report back lowest priority */
        return 0;
    }

    /* priority grows as quadrat of the fifo level */
    factor = status.data.fifoDepth/(status.data.fifoSize/256); /* 0 .. 256 - where 0 empty and 256 full */
    factor = (factor * factor) >> 6; /* priority is 0 ... 1024 */
    if(record->data.partialWrite) {
        factor = factor/16; /* scale down factor if  writer wasn't able to take all data (this prevents livelock case, where buffer is full, and writer can't take data, since it'd block all other writers */
    }
    BDBG_MSG_TRACE(("[%#lx] level %d%% priority %d", (unsigned long)record,
              status.data.fifoDepth*100/status.data.fifoSize, factor));
    /* when bandhold=true, no need to test it */
    if (status.data.fifoDepth*100/status.data.fifoSize >= 50 && !record->cfg.recpumpSettings.bandHold) {
        BDBG_WRN(("[%#lx] CDB level is %d%%. Overflow possible.", (unsigned long)record,
            status.data.fifoDepth*100/status.data.fifoSize));
    }
    if (status.index.fifoSize && status.index.fifoDepth*100/status.index.fifoSize >= 50 && !record->cfg.recpumpSettings.bandHold) {
        BDBG_WRN(("[%#lx] ITB level is %d%%. Overflow possible.", (unsigned long)record,
            status.index.fifoDepth*100/status.index.fifoSize));
    }
    return (int)factor;
}

static void
NEXUS_Record_P_Overflow(void *context)
{
    NEXUS_RecordHandle record = context;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);

    /* we have to record the case where we abort abnormally */
    if( record->index.state == Terminated || record->data.state == Terminated)
    {
        BDBG_WRN(("Terminating and ignoring overflow!"));
        return;
    }
    BDBG_WRN(("%#lx overflow event", (unsigned long)record));
#if B_VALIDATE_NAV_ENTRIES
    /* when debugging record, it's best to die on the first overflow. */
    BKNI_Fail();
#endif
    NEXUS_TaskCallback_Fire(record->overflowCallback);
    return;
}

/**
This is an aggressive debug mode for record.
It should not be used in a real system.
#define B_VALIDATE_NAV_ENTRIES 1
**/
#if B_VALIDATE_NAV_ENTRIES
#include "../src/bcmindexerpriv.h"
static bool validate_entry(BNAV_Entry *entry)
{
    /* some of these ranges are excessive, but we're mainly looking for corruptions which should have wildly
    different values */
    if (BNAV_get_frameType(entry) > 3) { /* I,B,P */
        BDBG_ERR(("invalid BNAV_get_frameType: %d", BNAV_get_frameType(entry)));
        return false;
    }
    if (BNAV_get_seqHdrStartOffset(entry) > 10 * 1000000) { /* GOP byte size */
        BDBG_ERR(("invalid BNAV_get_seqHdrStartOffset: %d", BNAV_get_seqHdrStartOffset(entry)));
        return false;
    }
    if (BNAV_get_refFrameOffset(entry) > 500) { /* # of pictures in a GOP */
        BDBG_ERR(("invalid BNAV_get_refFrameOffset: %d", BNAV_get_refFrameOffset(entry)));
        return false;
    }
    if (BNAV_get_version(entry) > 3) { /* could be 2 for MPEG, 3 for AVC */
        BDBG_ERR(("invalid BNAV_get_version: %d", BNAV_get_version(entry)));
        return false;
    }
    /* NOTE: we're not checking file offset. to do this right for N records, we would need a data structure.
    checking for frameSize should be sufficient. */
    if (BNAV_get_frameSize(entry) > 1000000) { /* largest I frame would be MPEG HD */
        BDBG_ERR(("invalid BNAV_get_frameSize: %d", BNAV_get_frameSize(entry)));
        return false;
    }
    return true;
}
#endif

/* synchronous write of index data, either SCT or NAV */
static unsigned long
NEXUS_Record_P_WriteNav(const void *p_bfr, unsigned long numEntries, unsigned long entrySize, void *fp )
{
    NEXUS_RecordHandle record = fp;
    NEXUS_FileWriteHandle index = record->index.file;
    BNAV_Indexer_Position position;
    ssize_t rc;
#if B_VALIDATE_NAV_ENTRIES
    bool fail = false;
    unsigned i;
    for (i=0;i<numEntries;i++) {
        if (!validate_entry(&((BNAV_Entry*)p_bfr)[i]))
            fail = true;
    }
#endif

    BDBG_MSG(("bnav_write %p, %d", p_bfr, entrySize*numEntries));
    record->picturesIndexed += numEntries;
    rc = index->write(index, p_bfr, entrySize*numEntries);

    if(BNAV_Indexer_GetPosition(record->index.info.index.indexer, &position)>=0){
        record->lastTimestamp = position.timestamp;
    }

#if B_VALIDATE_NAV_ENTRIES
    /* fail after the NAV entry is written. it should be on disk. */
    if (fail) BKNI_Fail();
#endif

    if (rc<0) {
        return 0;
    } else {
        return (unsigned long)rc;
    }
}

void
NEXUS_Record_GetSettings(NEXUS_RecordHandle record, NEXUS_RecordSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(pSettings);
    *pSettings = record->cfg;
    return;
}

NEXUS_Error
NEXUS_Record_SetSettings(NEXUS_RecordHandle record, const NEXUS_RecordSettings *settings)
{
    NEXUS_RecpumpSettings cfg;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(settings);

    if(record->started && (settings->recpump != record->cfg.recpump || settings->indexFormat != record->cfg.indexFormat) ) {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_settings;
    }
    if(settings->recpump==NULL && BLST_S_FIRST(&record->pid_list)) {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_settings;
    }
    if(settings->recpump) {
        NEXUS_RecpumpStatus status;

        cfg = settings->recpumpSettings;
        NEXUS_Recpump_GetStatus(settings->recpump, &status);
        if (!status.openSettings.data.atomSize || status.openSettings.data.atomSize % BIO_BLOCK_SIZE != 0) {
            BDBG_ERR(("NEXUS_Record requires that NEXUS_RecpumpOpenSettings.data.atomSize be a multiple of %d", BIO_BLOCK_SIZE));
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SATIP)
            if(status.openSettings.data.bCheckAlign)
            {
		rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_settings;
            }
#else
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_settings;
#endif
        }
        if (status.openSettings.data.dataReadyThreshold < B_RECORD_ATOM_SIZE) {
            BDBG_WRN(("NEXUS_RecpumpOpenSettings.data.dataReadyThreshold should be at least %d for optimal NEXUS_Record performance", B_RECORD_ATOM_SIZE));
        }
        NEXUS_CallbackHandler_PrepareCallback(record->data.dataReady, cfg.data.dataReady);
        NEXUS_CallbackHandler_PrepareCallback(record->index.dataReady, cfg.index.dataReady);
        NEXUS_CallbackHandler_PrepareCallback(record->data.overflow, cfg.data.overflow);
        NEXUS_CallbackHandler_PrepareCallback(record->index.overflow, cfg.index.overflow);
        rc = NEXUS_Recpump_SetSettings(settings->recpump, &cfg);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc);goto err_recpump_settings;}
    } else {
        NEXUS_Recpump_GetDefaultSettings(&cfg);
        rc = NEXUS_Recpump_SetSettings(record->cfg.recpump, &cfg); /* wipe out previous recpump settings */
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc);goto err_recpump_settings;}
    }

    NEXUS_TaskCallback_Set(record->errorCallback, &settings->errorCallback);
    NEXUS_TaskCallback_Set(record->overflowCallback, &settings->overflowCallback);

    record->cfg = *settings;
    
    if (record->cfg.pollingTimer && !record->processDataTimer && record->started) {
        record->processDataTimer = NEXUS_ScheduleTimer(record->cfg.pollingTimer, NEXUS_Record_P_ProcessDataTimer, record);
    }
    
    return NEXUS_SUCCESS;

err_recpump_settings:
err_settings:
    return rc;
}

void
NEXUS_Record_GetDefaultPidChannelSettings( NEXUS_RecordPidChannelSettings *pSettings )
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&pSettings->recpumpSettings);
    return;
}

#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_RECPUMP_PID_IS_INDEX(PSETTINGS) \
    ( (((PSETTINGS)->pidType == NEXUS_PidType_eVideo) && ((PSETTINGS)->pidTypeSettings.video.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eAudio) && ((PSETTINGS)->pidTypeSettings.other.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eOther) && ((PSETTINGS)->pidTypeSettings.other.index)) \
    )
#else
#define NEXUS_RECPUMP_PID_IS_INDEX(PSETTINGS) \
    ( (((PSETTINGS)->pidType == NEXUS_PidType_eVideo) && ((PSETTINGS)->pidTypeSettings.video.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eOther) && ((PSETTINGS)->pidTypeSettings.other.index)) \
    )
#endif

NEXUS_Error
NEXUS_Record_AddPidChannel(NEXUS_RecordHandle record, NEXUS_PidChannelHandle pidChannel, const NEXUS_RecordPidChannelSettings *pSettings )
{
    NEXUS_Error rc;
    NEXUS_Record_P_PidChannel *pid;
    NEXUS_RecordPidChannelSettings settings;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(pidChannel);
    if(pSettings==NULL) {
        NEXUS_Record_GetDefaultPidChannelSettings(&settings);
        pSettings = &settings;
    }    
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    if( pSettings->recpumpSettings.pidType == NEXUS_PidType_eVideo && pSettings->recpumpSettings.pidTypeSettings.video.index)
    {
    	if(record->started && (record->index.info.index.indexer == NULL))
    	{
    		rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;
    	}
    /*	i think below code is not needed.
    	else if(record->started && (record->indexCodec != pSettings->recpumpSettings.pidTypeSettings.video.codec))
    	{
    		rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;
    	}
    	else
    	{
    		record->indexCodec = pSettings->recpumpSettings.pidTypeSettings.video.codec;
    	}
    */
    }
    else if(pSettings->recpumpSettings.pidType == NEXUS_PidType_eAudio && pSettings->recpumpSettings.pidTypeSettings.other.index)
    {
    	if(record->started && (record->index.info.index.indexer == NULL))
    	{
    		rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;
    	}
    }
    else if(pSettings->recpumpSettings.pidType == NEXUS_PidType_eOther && pSettings->recpumpSettings.pidTypeSettings.other.index)
    {
    	if(record->started && (record->index.info.index.indexer == NULL))
    	{
    		rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;
    	}
    }
#else	/* original code */
    if (record->started && NEXUS_RECPUMP_PID_IS_INDEX(&pSettings->recpumpSettings)) {
        rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err_settings;
    }
#endif
    if(!record->cfg.recpump) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_recpump;}
    rc = NEXUS_Recpump_AddPidChannel(record->cfg.recpump, pidChannel, &pSettings->recpumpSettings);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_pidchn;}
    pid = BKNI_Malloc(sizeof(*pid));
    if(!pid) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}
    pid->pidChn = pidChannel;
    pid->cfg = *pSettings;
    BLST_S_DICT_ADD(&record->pid_list, pid, NEXUS_Record_P_PidChannel, pidChn, link, err_duplicate);
    return NEXUS_SUCCESS;

err_duplicate:
    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
    BKNI_Free(pid);
err_alloc:
    {
        NEXUS_Error rc = NEXUS_Recpump_RemovePidChannel(record->cfg.recpump, pidChannel);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    }
err_pidchn:
err_recpump:
err_settings:
    return rc;
}

NEXUS_Error
NEXUS_Record_RemovePidChannel(NEXUS_RecordHandle record, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Record_P_PidChannel *pid;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BDBG_ASSERT(pidChannel);
    if(!record->cfg.recpump) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_recpump;}
    BLST_S_DICT_REMOVE(&record->pid_list, pid, pidChannel, NEXUS_Record_P_PidChannel, pidChn, link);
    if(!pid) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_pid;}
    BKNI_Free(pid);
    rc = NEXUS_Recpump_RemovePidChannel(record->cfg.recpump, pidChannel);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_remove;}
    return NEXUS_SUCCESS;

err_remove:
err_recpump:
err_pid:
    return rc;
}

void
NEXUS_Record_RemoveAllPidChannels(NEXUS_RecordHandle record)
{
    NEXUS_Record_P_PidChannel *pid;
    NEXUS_Error rc;

    if (!BLST_S_FIRST(&record->pid_list)) {
        return;
    }
    if(!record->cfg.recpump) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    while(NULL!=(pid=BLST_S_FIRST(&record->pid_list))) {
        BLST_S_REMOVE_HEAD(&record->pid_list, link);
        if(record->cfg.recpump) {
            rc=NEXUS_Recpump_RemovePidChannel(record->cfg.recpump, pid->pidChn);
            if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc);}
        }
        BKNI_Free(pid);
    }
    return;
}

/**
When timeshifting with low bit rate streams, we need to poll recpump and not wait for the dataReadyThreshold to be met.
**/
static void NEXUS_Record_P_ProcessDataTimer(void *context)
{
    NEXUS_RecordHandle record = context;
    record->processDataTimer = NULL;

    if (record->started) {
        NEXUS_Record_P_DataReadyCallback(&record->data);
        NEXUS_Record_P_IndexReadyCallback(&record->index);
        if (record->cfg.pollingTimer) {
            record->processDataTimer = NEXUS_ScheduleTimer(record->cfg.pollingTimer, NEXUS_Record_P_ProcessDataTimer, record);
        }
    }
}

void NEXUS_Record_GetDefaultAppendSettings( NEXUS_RecordAppendSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error
NEXUS_Record_Start(NEXUS_RecordHandle record, NEXUS_FileRecordHandle file)
{
    return NEXUS_Record_StartAppend(record, file, NULL);
}

NEXUS_Error
NEXUS_Record_StartAppend(NEXUS_RecordHandle record, NEXUS_FileRecordHandle file, const NEXUS_RecordAppendSettings *pAppendSettings)
{
    bool hasIndex;
    NEXUS_Record_P_PidChannel *pid, *indexPid;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    if (!record->cfg.recpump) { return BERR_TRACE(NEXUS_INVALID_PARAMETER); }

    /* it's possible to do data-only or index-only record */
    if (!file->data && !file->index) {return BERR_TRACE(NEXUS_INVALID_PARAMETER); }

    record->picturesIndexed = 0;

    for(indexPid=NULL, pid=BLST_S_FIRST(&record->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        if (NEXUS_RECPUMP_PID_IS_INDEX(&pid->cfg.recpumpSettings)) {
            indexPid = pid;
            if(pid->cfg.recpumpSettings.pidTypeSettings.video.codec != NEXUS_VideoCodec_eH264) {
                break; /* stop here, unless H264 video, in which case there could be MVC/SVC pids */
            }
        }
    }
    hasIndex = indexPid && record->cfg.indexFormat != NEXUS_RecordIndexType_eNone && file->index;
    if(hasIndex) {
        NEXUS_PidChannelStatus  status;
        BDBG_ASSERT(indexPid && indexPid->pidChn);
        rc = NEXUS_PidChannel_GetStatus(indexPid->pidChn, &status);
        if(rc==NEXUS_SUCCESS) {
            hasIndex = status.transportType == NEXUS_TransportType_eTs;
        } else {
            rc = BERR_TRACE(rc);
            hasIndex = false;
        }
    }

    if (file->data) {
        bio_write_attach_priority(file->data, NEXUS_Record_P_GetPriority, record);
    }

    NEXUS_Record_P_InitFlow(record, &record->index);
    NEXUS_Record_P_InitFlow(record, &record->data);
    record->index.info.index.indexer = NULL;

#ifdef SCT_WRITE
    sctfile = fopen("stream.sct", "w+");
#endif

    if (hasIndex && record->cfg.indexFormat == NEXUS_RecordIndexType_eBcm) {
        BNAV_Indexer_Settings cfg;

        BNAV_Indexer_GetDefaultSettings(&cfg);

        cfg.writeCallback = NEXUS_Record_P_WriteNav;
        cfg.filePointer = record;
        if (record->cfg.recpumpSettings.timestampType == NEXUS_TransportTimestampType_eNone) {
            cfg.transportTimestampEnabled = false;
        }
        else{
            cfg.transportTimestampEnabled = true;
        }
        cfg.allowLargeTimeGaps = record->cfg.allowLargeTimeGaps;

        BDBG_ASSERT(indexPid);

        if (indexPid->cfg.recpumpSettings.pidType == NEXUS_PidType_eVideo) {
#if defined(HUMAX_PLATFORM_BASE)
			NEXUS_Recpump_ChangeRecMode(record->cfg.recpump, 0);
#endif
            /* This logic is duplicated in bmedia_player_nav.c's bmedia_player_nav_create. It is required for bcmindexer instead of passing the codec type. */
            switch (indexPid->cfg.recpumpSettings.pidTypeSettings.video.codec) {
            case NEXUS_VideoCodec_eH264_Svc:
            case NEXUS_VideoCodec_eH264_Mvc:
                record->entry_size = 6;
                cfg.videoFormat = BNAV_Indexer_VideoFormat_AVC_SVC;
                cfg.sctVersion = BSCT_Version6wordEntry;
                cfg.navVersion = BNAV_Version_AVC_Extended;
                break;
            case NEXUS_VideoCodec_eH264:
                record->entry_size = 6;
                cfg.videoFormat = BNAV_Indexer_VideoFormat_AVC;
                cfg.sctVersion = BSCT_Version6wordEntry;
                cfg.navVersion = BNAV_Version_AVC;
                break;
            case NEXUS_VideoCodec_eMpeg2:
                cfg.videoFormat = BNAV_Indexer_VideoFormat_MPEG2;
                cfg.sctVersion = BSCT_Version6wordEntry;
                record->entry_size = 6;
                break;
            case NEXUS_VideoCodec_eVc1:
                record->entry_size = 6;
                cfg.videoFormat = BNAV_Indexer_VideoFormat_VC1;
                cfg.sctVersion = BSCT_Version6wordEntry;
                cfg.navVersion = BNAV_Version_VC1_PES;
                break;
            case NEXUS_VideoCodec_eAvs:
                cfg.videoFormat = BNAV_Indexer_VideoFormat_AVS;
                cfg.sctVersion = BSCT_Version6wordEntry;
                cfg.navVersion = BNAV_Version_AVS;
                record->entry_size = 6;
                break;
            case NEXUS_VideoCodec_eUnknown:
                /* must be audio index, which is TimestampOnly */
                cfg.sctVersion = BSCT_Version6wordEntry;
                cfg.navVersion = BNAV_Version_TimestampOnly;
                record->entry_size = 6;
                break;
            default:
                BDBG_ERR(("Video Format not supported %#x", indexPid->cfg.recpumpSettings.pidTypeSettings.video.codec));
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }
        }
#if defined(HUMAX_PLATFORM_BASE)
        else if ((indexPid->cfg.recpumpSettings.pidType == NEXUS_PidType_eAudio) || (indexPid->cfg.recpumpSettings.pidType == NEXUS_PidType_eOther)) {
#else
        else if (indexPid->cfg.recpumpSettings.pidType == NEXUS_PidType_eOther) {
#endif
            record->entry_size = 6;
            cfg.sctVersion = BSCT_Version6wordEntry;
            cfg.navVersion = BNAV_Version_TimestampOnly;
#if defined(HUMAX_PLATFORM_BASE)			
			NEXUS_Recpump_ChangeRecMode(record->cfg.recpump, 1);
#endif
        }

        if (pAppendSettings) {
            cfg.append.offsetHi = pAppendSettings->startingOffset >> 32;
            cfg.append.offsetLo = pAppendSettings->startingOffset & 0xFFFFFFFF;
            cfg.append.timestamp = pAppendSettings->timestamp;
        }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
         cfg.fifoRecord = record->cfg.fifoRecord ? true : false;
#endif

        BNAV_Indexer_Open(&record->index.info.index.indexer, &cfg);
    }
    record->index.file = file->index;
    record->data.file = file->data;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_USB_FLASH_RECORD)
    record->data.bRecordWriteSizeLimit = file->bRecordWriteSizeLimit;
#endif

    BDBG_ASSERT(record->cfg.recpump);
    BDBG_MSG(("Starting recpump..."));
    NEXUS_StartCallbacks(record->cfg.recpump);
    rc = NEXUS_Recpump_Start(record->cfg.recpump);
    if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_start;}
#if defined(HUMAX_PLATFORM_BASE)
    record->lastTimestamp = 0;
#endif
    record->started = true;

    BDBG_ASSERT(!record->processDataTimer);
    if (record->cfg.pollingTimer) {
        record->processDataTimer = NEXUS_ScheduleTimer(record->cfg.pollingTimer, NEXUS_Record_P_ProcessDataTimer, record);
    }

    return NEXUS_SUCCESS;

err_start:
    return rc;
}

void
NEXUS_Record_Stop(NEXUS_RecordHandle record)
{
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);

    if(!record->started) {
        return;
    }

    if (record->processDataTimer) {
        NEXUS_CancelTimer(record->processDataTimer);
        record->processDataTimer = NULL;
    }

    /* this may generate callbacks to save last recorded data */
    record->data.stopping = true;
    record->index.stopping = true;
    BDBG_ASSERT(record->cfg.recpump);
    NEXUS_Recpump_StopData(record->cfg.recpump);

    if (record->cfg.writeAllTimeout) {
        unsigned timeout;
        NEXUS_RecpumpStatus status;
        for(timeout=0; timeout < record->cfg.writeAllTimeout; timeout+=10) {
           BDBG_ASSERT(NEXUS_Recpump_GetStatus(record->cfg.recpump, &status) == NEXUS_SUCCESS);
           if (status.data.fifoDepth==0 && status.index.fifoDepth==0) {
                 break;
           }
           NEXUS_UnlockModule();
           BKNI_Sleep(10);
           NEXUS_LockModule();
        }
    }

    NEXUS_StopCallbacks(record->cfg.recpump);
    NEXUS_CallbackHandler_Stop(record->data.dataReady);
    NEXUS_CallbackHandler_Stop(record->index.dataReady);
    NEXUS_CallbackHandler_Stop(record->data.overflow);
    NEXUS_CallbackHandler_Stop(record->index.overflow);

    record->started = false;
    record->data.file = NULL; /* this is the signal to exit. all recpump callbacks will already have been processed */
    record->index.file = NULL; /* this is the signal to exit. all recpump callbacks will already have been processed */

    while (record->data.state == Writing) {
        BDBG_WRN(("Record writing something to disk, waiting for it: %d", record->data.state));
        NEXUS_UnlockModule();
        BKNI_Sleep(50); /* sleep 50 ms */
        NEXUS_LockModule();
    }
    while (record->index.state == Writing) {
        BDBG_WRN(("Record writing something to disk, waiting for it: %d", record->index.state));
        NEXUS_UnlockModule();
        BKNI_Sleep(50); /* sleep 50 ms */
        NEXUS_LockModule();
    }

    NEXUS_Recpump_Stop(record->cfg.recpump);

    if (record->index.info.index.indexer) {
        BNAV_Indexer_Close(record->index.info.index.indexer);
        record->index.info.index.indexer = NULL;
    }
    record->data.stopping = false;
    record->index.stopping = false;
    if(record->index.info.index.timer) {
        NEXUS_CancelTimer(record->index.info.index.timer);
        record->index.info.index.timer = NULL;
    }
#if NEXUS_HAS_PLAYBACK
    if(BLST_S_FIRST(&record->playback_list)) {
        BDBG_WRN(("NEXUS_Recpump_Stop: %#lx stopping with playback attached", record));
        NEXUS_Record_RemoveAllPlaybacks(record);
    }
#endif

    /* test that no callback reentrancy happened during this Stop call. NEXUS_CallbackHandler_Stop ensures that. */
    BDBG_ASSERT(record->data.state != Writing);
    BDBG_ASSERT(record->index.state != Writing);

    return;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
NEXUS_Error
NEXUS_Record_Switch2Linear(NEXUS_RecordHandle record)
{
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    if (!record->cfg.recpump || !record->index.info.index.indexer || !record->cfg.fifoRecord) {
		return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    record->cfg.fifoRecord = false;
    BNAV_Indexer_SetFifoRecord(record->index.info.index.indexer, false);

    return NEXUS_SUCCESS;
}
#else
NEXUS_Error
NEXUS_Record_Switch2Linear(NEXUS_RecordHandle record)
{
    BSTD_UNUSED(record);

    return NEXUS_SUCCESS;
}
#endif

NEXUS_Error
NEXUS_Record_AddPlayback(NEXUS_RecordHandle record, NEXUS_PlaybackHandle playback)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;
    NEXUS_Record_P_Playback *play_item;
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    play_item = BKNI_Malloc(sizeof(*play_item));
    if(!play_item) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    play_item->playback = playback;
    BLST_S_DICT_ADD(&record->playback_list, play_item, NEXUS_Record_P_Playback, playback, link, err_duplicate);

    return NEXUS_SUCCESS;
err_duplicate:
    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
    BKNI_Free(play_item);
err_alloc:
    return rc;
#else
    BSTD_UNUSED(record);
    BSTD_UNUSED(playback);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error
NEXUS_Record_RemovePlayback( NEXUS_RecordHandle record, NEXUS_PlaybackHandle playback)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Record_P_Playback *play_item;
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);
    BLST_S_DICT_REMOVE(&record->playback_list, play_item, playback, NEXUS_Record_P_Playback, playback, link);
    if(play_item==NULL) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
    BKNI_Free(play_item);
    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(record);
    BSTD_UNUSED(playback);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

void
NEXUS_Record_RemoveAllPlaybacks( NEXUS_RecordHandle record)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Record_P_Playback *play_item;
    BDBG_OBJECT_ASSERT(record, NEXUS_Record);

    while(NULL!=(play_item=BLST_S_FIRST(&record->playback_list))) {
        BLST_S_REMOVE_HEAD(&record->playback_list, link);
        BKNI_Free(play_item);
    }
#else
    BSTD_UNUSED(record);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
    return;
}

NEXUS_Error NEXUS_Record_GetStatus(NEXUS_RecordHandle record, NEXUS_RecordStatus *pStatus)
{
    NEXUS_Error rc;
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    if (record->cfg.recpump) {
        rc = NEXUS_Recpump_GetStatus(record->cfg.recpump, &pStatus->recpumpStatus);
        if (rc) return BERR_TRACE(rc);
    }

    pStatus->error = record->errorType;
    pStatus->lastTimestamp = record->lastTimestamp;
    pStatus->picturesIndexed = record->picturesIndexed;

    if (record->index.info.index.indexer) {
        BNAV_Indexer_Status status;
        BNAV_Indexer_GetStatus(record->index.info.index.indexer, &status);
        pStatus->indexParsingErrors = status.parsingErrors;
    }

    return 0;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
NEXUS_Error NEXUS_Record_TpitIndexerEnable(NEXUS_RecordHandle record, bool tpitEnable)
{
    BNAV_Indexer_Settings cfg;

    BNAV_Indexer_GetSettings(record->index.info.index.indexer, &cfg);
    if (cfg.videoFormat == BNAV_Indexer_VideoFormat_MPEG2)
    {
    	BNAV_Indexer_SetNavVersion(record->index.info.index.indexer, tpitEnable?BNAV_Version_TPIT : BNAV_VersionLatest);
    }
    else if (cfg.videoFormat == BNAV_Indexer_VideoFormat_AVC)
    {
    	BNAV_Indexer_SetNavVersion(record->index.info.index.indexer, tpitEnable?BNAV_Version_TPIT_AVC : BNAV_Version_AVC);
    }
    else
    {
    	BDBG_ERR(("Video Format not supported %#x", cfg.videoFormat));
    	return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#if 0 /*hykim: obsolete*/
    if (record->cfg.recpump) {
        rc = NEXUS_Recpump_TpitIndexerEnable(record->cfg.recpump, tpitEnable);
        if (rc) return BERR_TRACE(rc);
    }
#endif
    return NEXUS_SUCCESS;
}
#else
NEXUS_Error NEXUS_Record_TpitIndexerEnable(NEXUS_RecordHandle record, bool tpitEnable)
{
    BSTD_UNUSED(record);
    BSTD_UNUSED(tpitEnable);

	return NEXUS_SUCCESS;
}
#endif
