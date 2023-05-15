/***************************************************************************
 *     (c)2003-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_packetsub.c $
 * $brcm_Revision: 26 $
 * $brcm_Date: 8/9/12 11:01a $
 *
 * Porting interface code for the packet substitution section of the
 * data transport core.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_packetsub.c $
 * 
 * 26   8/9/12 11:01a gmohile
 * SW7420-2085 : Fix platforms without packetsub
 * 
 * 25   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 24   4/5/12 4:37p erickson
 * SW7425-2147: removed NEXUS_PacketSubSettings.forcedOutput. clarified
 *  priority of other output rate settings.
 * 
 * 23   3/7/12 2:54p gmullen
 * SW7325-879: Added BXPT_HAS_PACKETSUB_FORCED_INSERTION around code
 * 
 * 22   2/27/12 10:45a gmullen
 * SW7425-2147: Merged changes to main
 * 
 * SW7425-2147/2   1/16/12 4:16p gmullen
 * SW7425-2147: Fixed incorrect band number
 * 
 * SW7425-2147/1   1/13/12 11:07a gmullen
 * SW7425-2147: Added forcedInsertion and playback support
 * 
 * 21   2/3/12 11:55a erickson
 * SW7405-5598: remove NEXUS_NUM_PACKET_SUB
 * 
 * 20   1/30/12 1:18p erickson
 * SW7405-5325: flush fifo on stop as well
 * 
 * 19   1/30/12 11:46a erickson
 * SW7405-5325: fix NEXUS_PacketSub_Flush
 * 
 * 18   12/16/11 1:14p erickson
 * SW7405-5325: fix 40nm packetsub
 * 
 * 17   12/16/11 11:43a erickson
 * SW7405-5325: rework packetsub to use array of descriptors, allow for
 *  skip in the fifo, add dataCallback
 *
 * 16   12/15/11 11:29a gmullen
 * SW7425-1589: Merged to main
 * 
 * SW7425-1589/1   12/12/11 4:35p gmullen
 * SW7425-1589: Added BXPT_PacketSub_GetEobIntId()
 * 
 * 15   12/9/11 5:52p erickson
 * SW7405-5325: remove BCHP_CHIP list
 *
 * 14   10/31/11 7:48p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 *
 * 13   7/8/11 10:59a jtna
 * SW7405-5382: revert back to no playback support. the PSUB HW does not
 *  support it.
 *
 * 12   7/7/11 4:24p jtna
 * SW7405-5382: initial playback packetsub work
 *
 * 11   5/16/11 9:36a erickson
 * SW7405-5323: add NEXUS_PacketSubStatus.busy
 *
 * 10   5/13/11 1:11p erickson
 * SW7405-5322: add NEXUS_FlushCache
 *
 * 9   5/13/11 12:18p erickson
 * SW7420-1873: remove shutdown attribute
 *
 * 8   1/25/11 3:38p rjlewis
 * SW7420-879: use default provided by PI for output rate.
 *
 * 6   1/17/11 9:31a erickson
 * SW7425-68: add NEXUS_TransportModuleSettings.mainHeapIndex
 *
 * 5   9/16/10 5:23p jgarrett
 * SW7125-621: Adding stub for NEXUS_PacketSub_GetStatus
 *
 * 4   9/15/10 4:24p erickson
 * SW7125-621: add NEXUS_PacketSub_GetStatus
 *
 * 3   8/10/10 3:41p erickson
 * SW7420-934: rename NEXUS_PacketSub_ReadComplete to
 *  NEXUS_PacketSub_WriteComplete
 *
 * 2   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 1   5/26/09 11:39a erickson
 * PR55105: add packet sub api
 *
 ***************************************************************************/
#include "nexus_transport_module.h"

BDBG_MODULE(nexus_packetsub);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

#if BXPT_HAS_PACKETSUB

#include "bxpt_packetsub.h"
#include "bchp_int_id_xpt_bus_if.h"

struct NEXUS_PacketSub
{
    NEXUS_OBJECT(NEXUS_PacketSub);
    NEXUS_PacketSubOpenSettings openSettings;
    NEXUS_PacketSubSettings settings;
    BXPT_PacketSub_Handle xptPacketSub;
    bool started;
    uint8_t *buffer; /* cached ptr */
    uint8_t *uncachedBuffer; /* uncached ptr for buffer */
    unsigned offsetBuffer; /* offset for buffer */
    unsigned wptr, rptr, size; /* fifo of buffer space */

    struct {
        BXPT_PacketSub_Descriptor *ptr; /* pointer to array of descriptors */
        unsigned wptr, rptr, size; /* fifo of descriptors */
    } desc;

    BMEM_Heap_Handle mem;
    unsigned lastGetBufferSize;
    NEXUS_IsrCallbackHandle finished, dataCallback;
    BINT_CallbackHandle psubInt;
};

static void
NEXUS_PacketSub_isr(void *context, int param)
{
    NEXUS_PacketSubHandle packetSub = context;
    BXPT_PacketSub_Descriptor *pDesc;
    BXPT_PacketSub_ChannelStatus status;

    BSTD_UNUSED(param);

    /* TODO: need isr functions */
    (void)BXPT_PacketSub_GetChannelStatus(packetSub->xptPacketSub, &status);
    (void)BXPT_PacketSub_GetCurrentDescriptorAddress(packetSub->xptPacketSub, &pDesc);
    BDBG_MSG_TRACE(("%p: isr cur desc %p, busy=%d finished=%d", packetSub, pDesc, status.Busy, status.Finished));

#if 0
    /* fifo should not be empty when we get an interrupt */
    BDBG_ASSERT(packetSub->desc.rptr != packetSub->desc.wptr);
#else
    /* TODO: why does this happen? */
    if (packetSub->desc.rptr == packetSub->desc.wptr) {
        return;
    }
#endif

    /* if not finished, then back up one desc to find what's been completed so far */
    if (!status.Finished) {
        if (pDesc == &packetSub->desc.ptr[packetSub->desc.rptr]) {
            /* none completed */
            return;
        }
        if (pDesc == packetSub->desc.ptr) {
            pDesc = &packetSub->desc.ptr[packetSub->desc.size-1];
        }
        else {
            pDesc--;
        }
    }

    /* update buffer and desc rptrs */
    packetSub->desc.rptr = (pDesc - packetSub->desc.ptr) + 1;
    if (packetSub->desc.rptr == packetSub->desc.size) {
        packetSub->desc.rptr = 0;
    }
    
    BDBG_ASSERT(pDesc->BufferStartAddr >= packetSub->offsetBuffer && pDesc->BufferStartAddr < packetSub->offsetBuffer + packetSub->size);
    packetSub->rptr = (pDesc->BufferStartAddr - packetSub->offsetBuffer) + pDesc->BufferLength;
    BDBG_ASSERT(packetSub->rptr <= packetSub->size);
    if (packetSub->rptr == packetSub->size) {
        packetSub->rptr = 0;
    }
    BDBG_MSG(("isr(%p) fifo=%d,%d; desc=%d,%d", packetSub, packetSub->rptr, packetSub->wptr, packetSub->desc.rptr, packetSub->desc.wptr));

    NEXUS_IsrCallback_Fire_isr(packetSub->dataCallback);
    if (packetSub->rptr == packetSub->wptr) {
        BDBG_ASSERT(packetSub->desc.rptr == packetSub->desc.wptr);
        NEXUS_IsrCallback_Fire_isr(packetSub->finished);
    }
}

void NEXUS_PacketSub_GetDefaultOpenSettings( NEXUS_PacketSubOpenSettings *pOpenSettings )
{
    BKNI_Memset(pOpenSettings, 0, sizeof(*pOpenSettings));
    pOpenSettings->fifoSize = 32 * 1024;
    pOpenSettings->numDescriptors = 4; /* default to a small number */
}

NEXUS_PacketSubHandle NEXUS_PacketSub_Open( unsigned index, const NEXUS_PacketSubOpenSettings *pOpenSettings )
{
    NEXUS_PacketSubHandle packetSub;
    BERR_Code rc;
    BXPT_PacketSub_ChannelSettings channelSettings;
    void *temp;
    unsigned int_id;
    NEXUS_PacketSubOpenSettings defaultOpenSettings;

    if (!pOpenSettings) {
        NEXUS_PacketSub_GetDefaultOpenSettings(&defaultOpenSettings);
        pOpenSettings = &defaultOpenSettings;
    }
    
    if (index >= BXPT_NUM_PACKETSUBS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    if (pOpenSettings->fifoSize < 4) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    if (pOpenSettings->numDescriptors == 0) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    packetSub = BKNI_Malloc(sizeof(*packetSub));
    if (!packetSub) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_PacketSub, packetSub);

    packetSub->finished = NEXUS_IsrCallback_Create(packetSub, NULL);
    if (!packetSub->finished) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    packetSub->dataCallback = NEXUS_IsrCallback_Create(packetSub, NULL);
    if (!packetSub->dataCallback) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }

    rc = BXPT_PacketSub_GetChannelDefaultSettings(pTransport->xpt, index, &channelSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    /* set default settings to match HW/PI default */
    packetSub->settings.outputRate = channelSettings.OutputRate;

    rc = BXPT_PacketSub_OpenChannel(pTransport->xpt, &packetSub->xptPacketSub, index, &channelSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    rc = BXPT_PacketSub_GetEobIntId( packetSub->xptPacketSub, &int_id );
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    packetSub->mem = pOpenSettings->heap ? NEXUS_Heap_GetMemHandle(pOpenSettings->heap) : g_pCoreHandles->heap[pTransport->settings.mainHeapIndex];

    /* alloc fifo */
    packetSub->size = pOpenSettings->fifoSize;
    packetSub->uncachedBuffer = BMEM_AllocAligned(packetSub->mem, packetSub->size, 2 /* 4 byte aligned */, 0);
    if (!packetSub->uncachedBuffer) {
        rc = BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
        goto error;
    }
    rc = BMEM_Heap_ConvertAddressToCached(packetSub->mem, packetSub->uncachedBuffer, &temp);
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    rc = BMEM_Heap_ConvertAddressToOffset(packetSub->mem, packetSub->uncachedBuffer, &packetSub->offsetBuffer);
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    packetSub->buffer = temp;

    /* alloc descriptors */
    packetSub->desc.size = pOpenSettings->numDescriptors + 1; /* alloc one extra for simple fifo logic */
    packetSub->desc.ptr = BMEM_AllocAligned(packetSub->mem, packetSub->desc.size * sizeof(BXPT_PacketSub_Descriptor), 4 /* 16 byte aligned */, 0);
    if (!packetSub->desc.ptr) {
        rc = BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
        goto error;
    }
    BDBG_MSG_TRACE(("%p: desc fifo %#x, size %#x * %d", packetSub, packetSub->desc.ptr, sizeof(BXPT_PacketSub_Descriptor), packetSub->desc.size));

    packetSub->openSettings = *pOpenSettings;

    rc = BINT_CreateCallback(&packetSub->psubInt, g_pCoreHandles->bint, int_id, NEXUS_PacketSub_isr, packetSub, 0);
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    rc = BINT_EnableCallback(packetSub->psubInt);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    return packetSub;

error:
    NEXUS_PacketSub_Close(packetSub);
    return NULL;
}

static void NEXUS_PacketSub_P_Finalizer( NEXUS_PacketSubHandle packetSub )
{
    NEXUS_OBJECT_ASSERT(NEXUS_PacketSub, packetSub);

    if (packetSub->started) {
        NEXUS_PacketSub_Stop(packetSub);
    }
    if (packetSub->psubInt) {
        BINT_DestroyCallback(packetSub->psubInt);
    }
    if (packetSub->xptPacketSub) {
        BXPT_PacketSub_CloseChannel(packetSub->xptPacketSub);
    }
    if (packetSub->desc.ptr) {
        BMEM_Heap_Free(packetSub->mem, packetSub->desc.ptr);
    }
    if (packetSub->uncachedBuffer) {
        BMEM_Heap_Free(packetSub->mem, packetSub->uncachedBuffer);
    }
    if (packetSub->finished) {
        NEXUS_IsrCallback_Destroy(packetSub->finished);
    }
    if (packetSub->dataCallback) {
        NEXUS_IsrCallback_Destroy(packetSub->dataCallback);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_PacketSub, packetSub);
    BKNI_Free(packetSub);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_PacketSub, NEXUS_PacketSub_Close);

NEXUS_Error NEXUS_PacketSub_Start( NEXUS_PacketSubHandle packetSub )
{
    BERR_Code rc;
    NEXUS_PidChannelHandle pidChannel;

    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    if (packetSub->started) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    pidChannel = packetSub->settings.pidChannel;
    if (!pidChannel) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    #if !BXPT_HAS_PB_PACKETSUB
    if (pidChannel->status.playback) {
        /* PSUB HW does not support playback */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    #endif

    rc = BXPT_PacketSub_SetPidChanNum(packetSub->xptPacketSub, pidChannel->status.pidChannelIndex, 
        pidChannel->status.playback ? BXPT_PB_PARSER(pidChannel->status.playbackIndex) : pidChannel->parserBand->hwIndex );
    if (rc) return BERR_TRACE(rc);

    rc = BXPT_PacketSub_StartChannel(packetSub->xptPacketSub);
    if (rc) return BERR_TRACE(rc);

    packetSub->started = true;

    return 0;
}

static void nexus_p_packetsub_flush_fifo(NEXUS_PacketSubHandle packetSub)
{
    BKNI_EnterCriticalSection();
    packetSub->rptr = packetSub->wptr = 0;
    packetSub->desc.rptr = packetSub->desc.wptr = 0;
    BKNI_LeaveCriticalSection();
    packetSub->lastGetBufferSize = 0;
}

void NEXUS_PacketSub_Stop( NEXUS_PacketSubHandle packetSub )
{
    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    if (packetSub->started) {
        (void)BXPT_PacketSub_StopChannel(packetSub->xptPacketSub);
        packetSub->started = false;
        
        nexus_p_packetsub_flush_fifo(packetSub);
    }
}

NEXUS_Error NEXUS_PacketSub_SetSettings( NEXUS_PacketSubHandle packetSub, const NEXUS_PacketSubSettings *pSettings )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    NEXUS_IsrCallback_Set(packetSub->finished, &pSettings->finished);
    NEXUS_IsrCallback_Set(packetSub->dataCallback, &pSettings->dataCallback);

    if (packetSub->started) {
        /* some settings can't be changed after start */
        if (pSettings->pidChannel != packetSub->settings.pidChannel) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        if (pSettings->loop != packetSub->settings.loop) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    rc = BXPT_PacketSub_SetFullRateOutput(packetSub->xptPacketSub, pSettings->fullRateOutput);
    if (rc) return BERR_TRACE(rc);

    rc = BXPT_PacketSub_SetOutputRate(packetSub->xptPacketSub, pSettings->outputRate);
    if (rc) return BERR_TRACE(rc);

#if BXPT_HAS_PACKETSUB_FORCED_INSERTION
    rc = BXPT_PacketSub_SetForcedInsertion( packetSub->xptPacketSub, pSettings->forcedInsertion );
    if (rc ) return BERR_TRACE( rc );
#endif

    packetSub->settings = *pSettings;
    return 0;
}

void NEXUS_PacketSub_GetSettings( NEXUS_PacketSubHandle packetSub, NEXUS_PacketSubSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);
    *pSettings = packetSub->settings;
}

NEXUS_Error NEXUS_PacketSub_SetPause( NEXUS_PacketSubHandle packetSub, bool paused )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    rc = BXPT_PacketSub_PauseChannel(packetSub->xptPacketSub, paused);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

/* === data; --- available
   |===W---R===|
   |---R===W---|
   |R======W---|
*/
/* continuous space available at wptr */
#define BFIFO_WRITE_AVAILABLE(wptr, rptr, size, min) ((wptr)<(rptr)?(rptr)-(wptr)-(min):(rptr)?(size)-(wptr):(size)-(wptr)-(min))

NEXUS_Error NEXUS_PacketSub_GetBuffer( NEXUS_PacketSubHandle packetSub, void **pBuffer, unsigned *pSize )
{
    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    /* there must be buffer space and a spare desciptor */
    BKNI_EnterCriticalSection();
    if (!BFIFO_WRITE_AVAILABLE(packetSub->desc.wptr, packetSub->desc.rptr, packetSub->desc.size, 1) ||
        !BFIFO_WRITE_AVAILABLE(packetSub->wptr, packetSub->rptr, packetSub->size, 4)) {
        *pBuffer = NULL;
        *pSize = 0;
    }
    else {
        *pBuffer = &packetSub->buffer[packetSub->wptr];
        *pSize = BFIFO_WRITE_AVAILABLE(packetSub->wptr, packetSub->rptr, packetSub->size, 4);
    }
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("GetBuffer(%p) fifo=%d,%d; desc=%d,%d; size=%d", packetSub, packetSub->rptr, packetSub->wptr, packetSub->desc.rptr, packetSub->desc.wptr, *pSize));

    packetSub->lastGetBufferSize = *pSize;

    return 0;
}

NEXUS_Error NEXUS_PacketSub_WriteCompleteWithSkip( NEXUS_PacketSubHandle packetSub, unsigned skip, unsigned amount )
{
    NEXUS_Error rc;
    BXPT_PacketSub_Descriptor *pDesc;

    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);

    if (skip + amount > packetSub->lastGetBufferSize) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    if (skip) {
        BKNI_EnterCriticalSection();
        packetSub->wptr += skip;
        if (packetSub->wptr == packetSub->size) {
            packetSub->wptr = 0;
        }
        BKNI_LeaveCriticalSection();
    }
    
    if (!amount) {
        goto done;
    }
    
    /* flush data in cached fifo before submitting to HW */
    NEXUS_FlushCache(&packetSub->buffer[packetSub->wptr], amount);

    /* prepare descriptor for this submisssion */
    pDesc = &packetSub->desc.ptr[packetSub->desc.wptr];
    rc = BXPT_PacketSub_CreateDesc(pTransport->xpt, pDesc, &packetSub->uncachedBuffer[packetSub->wptr], amount, true,
        packetSub->settings.loop ? pDesc : NULL);
    if (rc) return BERR_TRACE(rc);

    BKNI_EnterCriticalSection();
    if (++packetSub->desc.wptr == packetSub->desc.size) {
        packetSub->desc.wptr = 0;
    }
    packetSub->wptr += amount;
    if (packetSub->wptr == packetSub->size) {
        packetSub->wptr = 0;
    }
    BKNI_LeaveCriticalSection();

    /* must add to HW after incrementing wptr */
    rc = BXPT_PacketSub_AddDescriptors(packetSub->xptPacketSub, pDesc, pDesc);
    if (rc) return BERR_TRACE(rc);
    BDBG_MSG_TRACE(("%p: submit desc %p", packetSub, pDesc));

done:
    BDBG_MSG(("WriteComplete(%p) fifo=%d,%d; desc=%d,%d; skip=%d, amount=%d", packetSub, packetSub->rptr, packetSub->wptr, packetSub->desc.rptr, packetSub->desc.wptr, skip, amount));

    packetSub->lastGetBufferSize = 0;

    return 0;
}

void NEXUS_PacketSub_Flush( NEXUS_PacketSubHandle packetSub )
{
    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);
    if (packetSub->started) {
        NEXUS_Error rc;
        
        (void)BXPT_PacketSub_StopChannel(packetSub->xptPacketSub);
        rc = BXPT_PacketSub_StartChannel(packetSub->xptPacketSub);
        if (rc) {rc = BERR_TRACE(rc);}
        
        nexus_p_packetsub_flush_fifo(packetSub);
    }
}

NEXUS_Error NEXUS_PacketSub_GetStatus( NEXUS_PacketSubHandle packetSub, NEXUS_PacketSubStatus *pStatus )
{
    BXPT_PacketSub_ChannelStatus chStatus;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(packetSub, NEXUS_PacketSub);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    rc = BXPT_PacketSub_GetChannelStatus(packetSub->xptPacketSub, &chStatus);
    if (rc) return BERR_TRACE(rc);
    pStatus->finished = chStatus.Finished;
    pStatus->busy = chStatus.Busy;

    return 0;
}

#else /* BXPT_HAS_PACKETSUB */
struct NEXUS_PacketSub
{
    NEXUS_OBJECT(NEXUS_PacketSub);    
};

void NEXUS_PacketSub_GetDefaultOpenSettings( NEXUS_PacketSubOpenSettings *pOpenSettings )
{
    BSTD_UNUSED(pOpenSettings);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_PacketSubHandle NEXUS_PacketSub_Open( unsigned index, const NEXUS_PacketSubOpenSettings *pOpenSettings )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pOpenSettings);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

static void NEXUS_PacketSub_P_Finalizer( NEXUS_PacketSubHandle packetSub )
{
    BSTD_UNUSED(packetSub);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_PacketSub, NEXUS_PacketSub_Close);

NEXUS_Error NEXUS_PacketSub_Start( NEXUS_PacketSubHandle packetSub )
{
    BSTD_UNUSED(packetSub);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_PacketSub_Stop( NEXUS_PacketSubHandle packetSub )
{
    BSTD_UNUSED(packetSub);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_PacketSub_SetSettings( NEXUS_PacketSubHandle packetSub, const NEXUS_PacketSubSettings *pSettings )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_PacketSub_GetSettings( NEXUS_PacketSubHandle packetSub, NEXUS_PacketSubSettings *pSettings )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_PacketSub_SetPause( NEXUS_PacketSubHandle packetSub, bool paused )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(paused);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_PacketSub_GetBuffer( NEXUS_PacketSubHandle packetSub, void **pBuffer, unsigned *pSize )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(pBuffer);
    BSTD_UNUSED(pSize);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_PacketSub_WriteCompleteWithSkip( NEXUS_PacketSubHandle packetSub, unsigned skip, unsigned amount )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(skip);
    BSTD_UNUSED(amount);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_PacketSub_Flush( NEXUS_PacketSubHandle packetSub )
{
    BSTD_UNUSED(packetSub);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_PacketSub_GetStatus( NEXUS_PacketSubHandle packetSub, NEXUS_PacketSubStatus *pStatus )
{
    BSTD_UNUSED(packetSub);
    BSTD_UNUSED(pStatus);
    BDBG_WRN(("Packetsub not enabled on this chipset"));
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
#endif /* BXPT_HAS_PACKETSUB */

NEXUS_Error NEXUS_PacketSub_WriteComplete( NEXUS_PacketSubHandle packetSub, unsigned amount )
{
    return NEXUS_PacketSub_WriteCompleteWithSkip(packetSub, 0, amount);
}
