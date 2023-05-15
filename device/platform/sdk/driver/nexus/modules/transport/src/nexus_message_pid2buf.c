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
 * $brcm_Workfile: nexus_message_pid2buf.c $
 * $brcm_Revision: 48 $
 * $brcm_Date: 10/22/12 5:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_message_pid2buf.c $
 * 
 * 48   10/22/12 5:58p erickson
 * SW7435-440: fix bounds check for message capable pid channel
 * 
 * 47   8/30/12 12:20p erickson
 * SW7425-3418: add acquire/release for msg filter's pid channel
 * 
 * 46   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 45   2/1/12 5:00p hongtaoz
 * SW7425-2320: fixed PID remapping with message pid2buf and with
 *  duplicated PID channel;
 * 
 * 44   12/21/11 2:32p erickson
 * SW7425-1795: message buffers with maxContiguousMessageSize must be cpu
 *  accessible in the driver
 * 
 * 43   12/20/11 4:36p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * SW7425-1868/1   12/20/11 4:32p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * 42   12/15/11 2:32p gmullen
 * SW7425-1952: Merged changes to mainline
 * 
 * SW7425-1952/1   12/14/11 4:08p gmullen
 * SW7425-1952: Moved NEXUS_Message structure to nexus_transport_module.h
 * 
 * 41   11/7/11 3:13p erickson
 * SW7425-1688: clarify critical section in Close, delay assignment and
 *  add critical section in Open
 * 
 * 40   10/31/11 7:47p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 39   7/12/11 10:24a erickson
 * SW7346-316: fix NEXUS_Transport_P_SetInterrupts logic
 * 
 * 38   1/17/11 9:32a erickson
 * SW7425-68: add NEXUS_TransportModuleSettings.mainHeapIndex
 *
 * 37   12/16/10 5:12p erickson
 * SW7422-151: apply useRPipe to PES/TS capture
 *
 * 36   12/16/10 10:33a erickson
 * SW7422-151: add NEXUS_MessageStartSettings.useRPipe
 *
 * 35   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 34   9/8/10 4:41p erickson
 * SW3548-3073: NEXUS_Message_GetBuffer should not re-enable the interrupt
 *  if returned size is non-zero. This is correct backward compatibility
 *  with MSGlib. Also, moved definition of struct NEXUS_Message into each
 *  implementation.
 *
 * 33   9/3/10 11:56a erickson
 * SW35230-1172: add NEXUS_Message_GetBufferWithWrap
 *
 * 32   9/1/10 3:42p erickson
 * SW7420-996: add NEXUS_MessageStatus.moreDataAvailable
 *
 * 31   7/30/10 10:10a erickson
 * SW7400-2860: bypass maxContiguousMessageSize logic for DSS streams
 *
 * 30   6/29/10 1:34p erickson
 * SW7405-4525: if bank = -1, add filter to least used bank. add
 *  bank_refcnt to keep track.
 *
 * 29   5/10/10 5:25p jtna
 * SW7125-319: Coverity Defect ID:20370 FORWARD_NULL
 *
 * 28   4/19/10 2:52p erickson
 * SW7420-624: add NEXUS_Message_UpdateFilter
 *
 * 27   4/19/10 1:59p erickson
 * SW7420-624: add NEXUS_Message_AddFilter and NEXUS_Message_RemoveFilter
 *  to enable filter groups with pid2buf
 *
 * 26   4/16/10 12:08p erickson
 * SW7325-719: maxContiguousMessageSize only applies to PSI data
 *
 * 25   3/8/10 1:03p erickson
 * SW7405-4008: null pidChannel setting on Stop. it may no longer be
 *  valid.
 *
 * 24   2/26/10 2:15p erickson
 * SW7325-655: refactor NEXUS_Core_P_AddressToHeap
 *
 * 23   1/11/10 3:24p jhaberf
 * SW35230-1: Removed compiler warnings for 35230 build
 *
 * 22   11/23/09 10:00a erickson
 * SW7325-650: add sample of BXPT_PidPsiConfig.SaveLongPsiMsg
 *
 * 21   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 20   11/12/09 4:44p jhaberf
 * SW35230-1: Put #ifdef's around DSS functions so the drivers for chips
 *  which don't support DSS build.
 *
 * 19   11/6/09 1:57p erickson
 * SW7400-2559: added NEXUS_Message_SetDssCapPattern
 *
 * 18   11/3/09 4:17p erickson
 * SWDEPRECATED-3717: call NEXUS_Core_P_AddressToHeap only when necessary
 *
 * 17   10/20/09 3:56p erickson
 * SW7400-2559: fix non-DSS capable platforms
 *
 * 16   10/20/09 2:24p erickson
 * SW7400-2559: add DSS message support
 *
 * 15   10/15/09 1:53p erickson
 * SWDEPRECATED-3717: allow NEXUS_Message_GetDefaultStartSettings to be
 *  called without a NEXUS_MessageHandle
 *
 * 14   10/1/09 4:46p erickson
 * SWDEPRECATED-3995: turn off transport message interrupts before
 *  destroying NEXUS_IsrCallbacks.
 *
 * 13   9/29/09 3:03p erickson
 * SWDEPRECATED-3717: NEXUS_Message_Stop should clear more variables
 *
 * 12   9/25/09 11:31a erickson
 * SWDEPRECATED-3717: add debug
 *
 * 11   9/25/09 10:12a erickson
 * SWDEPRECATED-3717: use BXPT_GetDefaultPsiSettings
 *
 * 10   9/24/09 5:00p erickson
 * SWDEPRECATED-3717: support maxContiguousMessageSize
 *
 * 9   9/22/09 5:29p erickson
 * SWDEPRECATED-3717: move BXPT_Mesg_SetPid2Buff to system init time
 *
 * 8   9/18/09 4:31p erickson
 * SWDEPRECATED-3717: allow NEXUS_MessageSettings.bufferSize for pid2buf
 *
 * 7   9/2/09 3:23p jgarrett
 * SW7325-582: Destroying PidChannel to Buffer association in
 *  NEXUS_Message_Stop
 *
 * 6   8/26/09 11:05a erickson
 * SWDEPRECATED-3995: fixed MesgBufferNum assignment
 *
 * 5   8/26/09 8:07a erickson
 * SWDEPRECATED-3995: fixed last checkin
 *
 * 4   8/26/09 7:57a erickson
 * SWDEPRECATED-3995: fixed for pid2buf
 *
 * 3   8/20/09 4:18p erickson
 * PR53324: fix pad calculation
 *
 * 2   8/20/09 3:59p erickson
 * PR53324: fix structure assignment of pid, add 4 byte pad into GetBuffer
 *
 * 1   6/8/09 11:49a erickson
 * PR53324: add PID2BUF impl, default off
 *
 **************************************************************************/
#include "nexus_transport_module.h"

BDBG_MODULE(nexus_message);

#if NEXUS_NUM_MESSAGE_FILTERS

#include "bxpt_interrupt.h"

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

void NEXUS_Message_GetDefaultSettings(NEXUS_MessageSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->bufferSize = 4 * 1024;
    pSettings->maxContiguousMessageSize = 4 * 1024;
}

static void NEXUS_message_p_dataready_callback_isr(void *data, int unused)
{
    NEXUS_MessageHandle msg = (NEXUS_MessageHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    /* disable until the next GetBuffer call */
    BXPT_Interrupt_DisableMessageInt_isr(pTransport->xpt, msg->MesgBufferNum);

    BDBG_MSG(("dataready %p: %d", msg, msg->MesgBufferNum));
    NEXUS_IsrCallback_Fire_isr(msg->dataReady);
}

static void NEXUS_message_p_overflow_callback_isr(void *data, int unused)
{
    NEXUS_MessageHandle msg = (NEXUS_MessageHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    /* disable until the next GetBuffer call */
    BXPT_Interrupt_DisableMessageOverflowInt_isr(pTransport->xpt, msg->MesgBufferNum);

    NEXUS_IsrCallback_Fire_isr(msg->overflow);
}

static void NEXUS_Message_P_EnableInterrupt(NEXUS_MessageHandle msg, bool enabled)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    if (enabled) {
        rc = BXPT_Interrupt_EnableMessageInt(pTransport->xpt, msg->MesgBufferNum, NEXUS_message_p_dataready_callback_isr, msg, 0);
        if (rc) rc = BERR_TRACE(rc); /* fall through */
        rc = BXPT_Interrupt_EnableMessageOverflowInt(pTransport->xpt, msg->MesgBufferNum, NEXUS_message_p_overflow_callback_isr, msg, 0);
        if (rc) rc = BERR_TRACE(rc); /* fall through */
    }
    else {
        rc = BXPT_Interrupt_DisableMessageInt(pTransport->xpt, msg->MesgBufferNum);
        if (rc) rc = BERR_TRACE(rc); /* fall through */
        rc = BXPT_Interrupt_DisableMessageOverflowInt(pTransport->xpt, msg->MesgBufferNum);
        if (rc) rc = BERR_TRACE(rc); /* fall through */
    }
}

#if NEXUS_MESSAGE_USE_CHECK_TIMER
static void NEXUS_Message_P_CheckTimer(void *context)
{
    NEXUS_MessageHandle msg = context;
    msg->checkTimer = NULL;

    if (msg->noReadCompleteCount) {
        if (++msg->noReadCompleteCount >= 10) {
            BDBG_WRN(("You must call NEXUS_Message_ReadComplete to re-enable the dataReady callback."));
            msg->noReadCompleteCount = 1; /* allow it to repeat */
        }
    }

    msg->checkTimer = NEXUS_ScheduleTimer(1000, NEXUS_Message_P_CheckTimer, msg);
}
#endif

NEXUS_MessageHandle NEXUS_Message_Open(const NEXUS_MessageSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_MessageSettings settings;
    NEXUS_MessageHandle msg;
    NEXUS_MemoryAllocationSettings allocSettings;
    unsigned i;

    if (!pSettings) {
        NEXUS_Message_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    msg = BKNI_Malloc(sizeof(*msg));
    if(!msg) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);return NULL; }

    NEXUS_OBJECT_INIT(NEXUS_Message, msg);
    msg->settings = *pSettings;

    if (pSettings->bufferSize) {
        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        allocSettings.alignment = 1024;
        allocSettings.heap = g_pCoreHandles->nexusHeap[pTransport->settings.mainHeapIndex];
        rc = NEXUS_Memory_Allocate(pSettings->bufferSize, &allocSettings, &msg->allocatedBuffer);
        if (rc) {
            rc = BERR_TRACE(rc);
            goto error;
        }
        if (!NEXUS_Core_P_AddressToHeap(msg->allocatedBuffer, &msg->uncachedBuffer)) {
            BDBG_ERR(("A user-provided buffer must be allocated with NEXUS_Memory_Allocate in order to be properly translated for HW use."));
            goto error;
        }
    }

    if (pSettings->maxContiguousMessageSize) {
        NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
        allocSettings.heap = g_pCoreHandles->nexusHeap[pTransport->settings.mainHeapIndex];
        rc = NEXUS_Memory_Allocate(pSettings->maxContiguousMessageSize, &allocSettings, &msg->wrappedMessage.buffer);
        if (rc) {
            rc = BERR_TRACE(rc);
            goto error;
        }
    }

    /* must keep track of Message handles in order to route interrupts back out to Message handles */
    for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
        if (!pTransport->message.handle[i]) {
            break;
        }
    }
    if (i == NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES) {
        BDBG_ERR(("You must increase NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES"));
        goto error;
    }
    /* Use the index in the pTransport->message.handle[] array as the physical MesgBufferNum. */
    msg->MesgBufferNum = i;

    msg->dataReady = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->dataReady) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    msg->overflow = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->overflow) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    msg->psiLengthError = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->psiLengthError) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    msg->crcError = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->crcError) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    msg->pesLengthError = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->pesLengthError) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    msg->pesStartCodeError = NEXUS_IsrCallback_Create(msg, NULL);
    if(!msg->pesStartCodeError) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}

    BKNI_EnterCriticalSection();
    /* barrier to prevent this code from interrupting isr that is already running. don't assign until entire SW state is set. */
    pTransport->message.handle[i] = msg;
    BKNI_LeaveCriticalSection();

    /* set the interrupts */
    (void)NEXUS_Message_SetSettings(msg, pSettings);

#if NEXUS_MESSAGE_USE_CHECK_TIMER
    msg->checkTimer = NEXUS_ScheduleTimer(1000, NEXUS_Message_P_CheckTimer, msg);
#endif

    return msg;

error:
    NEXUS_Message_Close(msg);
    return NULL;
}

static void NEXUS_Message_P_Finalizer(NEXUS_MessageHandle msg)
{
    unsigned i;
    NEXUS_OBJECT_ASSERT(NEXUS_Message, msg);

#if NEXUS_MESSAGE_USE_CHECK_TIMER
    if (msg->checkTimer) {
        NEXUS_CancelTimer(msg->checkTimer);
        msg->checkTimer = NULL;
    }
#endif

    BDBG_MSG(("Close %p, %d", msg, msg->MesgBufferNum));
    if (msg->started) {
        NEXUS_Message_Stop(msg);
    }
    /* isr callbacks may still occur because isr is for all message filters, not this one only */

    for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
        if (pTransport->message.handle[i] == msg) {
            break;
        }
    }
    /* it's possible msg is not found if Open failed before completing */
    if (i < NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES) {
        BKNI_EnterCriticalSection();
        /* barrier to protect against isr which is already running from accessing this handle */
        pTransport->message.handle[i] = NULL;
        BKNI_LeaveCriticalSection();
    }
    /* call this after message.handle[] is cleared.
    TODO: unless we clear HW state, a stale interrupt may be waiting for new message filter that is immediately opened with same MesgBufferNum. */
    NEXUS_Transport_P_SetInterrupts();

    if (msg->dataReady) {
        NEXUS_IsrCallback_Destroy(msg->dataReady);
    }
    if (msg->overflow) {
        NEXUS_IsrCallback_Destroy(msg->overflow);
    }
    if (msg->psiLengthError) {
        NEXUS_IsrCallback_Destroy(msg->psiLengthError);
    }
    if (msg->crcError) {
        NEXUS_IsrCallback_Destroy(msg->crcError);
    }
    if (msg->pesLengthError) {
        NEXUS_IsrCallback_Destroy(msg->pesLengthError);
    }
    if (msg->pesStartCodeError) {
        NEXUS_IsrCallback_Destroy(msg->pesStartCodeError);
    }

    if (msg->wrappedMessage.buffer) {
        NEXUS_Memory_Free(msg->wrappedMessage.buffer);
    }
    if (msg->allocatedBuffer) {
        NEXUS_Memory_Free(msg->allocatedBuffer);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_Message, msg);
    BKNI_Free(msg);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Message, NEXUS_Message_Close);

void NEXUS_Message_GetSettings( NEXUS_MessageHandle msg, NEXUS_MessageSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    *pSettings = msg->settings;
}

NEXUS_Error NEXUS_Message_SetSettings( NEXUS_MessageHandle msg, const NEXUS_MessageSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    NEXUS_IsrCallback_Set(msg->dataReady, &pSettings->dataReady);
    NEXUS_IsrCallback_Set(msg->overflow, &pSettings->overflow);
    NEXUS_IsrCallback_Set(msg->psiLengthError, &pSettings->psiLengthError);
    NEXUS_IsrCallback_Set(msg->crcError, &pSettings->crcError);
    NEXUS_IsrCallback_Set(msg->pesLengthError, &pSettings->pesLengthError);
    NEXUS_IsrCallback_Set(msg->pesStartCodeError, &pSettings->pesStartCodeError);

    msg->settings = *pSettings;
    
    NEXUS_Transport_P_SetInterrupts();

    return 0;
}

void NEXUS_Message_GetDefaultStartSettings(NEXUS_MessageHandle msg, NEXUS_MessageStartSettings *pStartSettings)
{
    /* allow NULL msg handle to retrieve defaults w/o a handle */
    BSTD_UNUSED(msg);

    BKNI_Memset(pStartSettings, 0, sizeof(*pStartSettings));
    pStartSettings->format = NEXUS_MessageFormat_ePsi;
    pStartSettings->bufferMode = NEXUS_MessageBufferMode_eContinuous; /* deprecated */
    pStartSettings->bank = -1;
    NEXUS_Message_GetDefaultFilter(&pStartSettings->filter);
}

void NEXUS_Message_GetDefaultFilter(NEXUS_MessageFilter *pFilter)
{
    BKNI_Memset(pFilter, 0, sizeof(*pFilter));
    BKNI_Memset(pFilter->mask, 0xFF, sizeof(pFilter->mask));
    BKNI_Memset(pFilter->exclusion, 0xFF, sizeof(pFilter->exclusion));
}

NEXUS_Error NEXUS_Message_Start(NEXUS_MessageHandle msg, const NEXUS_MessageStartSettings *pStartSettings)
{
    unsigned short pid;
    BERR_Code rc;
    BXPT_MessageBufferSize bufferSizeEnum;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    if (msg->started) {
        BDBG_ERR(("already started"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (!pStartSettings || !pStartSettings->pidChannel) {
        BDBG_ERR(("pidChannel required"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if ((!pStartSettings->buffer || !pStartSettings->bufferSize) && !msg->allocatedBuffer) {
        BDBG_ERR(("NEXUS_Message_Start requires buffer and bufferSize"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (pStartSettings->buffer) {
        /* no other message filter should be using this buffer. if you were explicitly setting a shared buffer using NEXUS_MessageStartSettings.buffer, this
        will cause problems with pid2buf. Each filter needs its own buffer. */
        unsigned i;
        for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
            if (pTransport->message.handle[i] && pTransport->message.handle[i]->buffer == pStartSettings->buffer) {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
        if (msg->settings.maxContiguousMessageSize) {
            /* if this feature is enabled, the buffer must be cpu accessible inside nexus */
            if (!NEXUS_P_CpuAccessibleAddress(pStartSettings->buffer)) {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
    }

    msg->startSettings = *pStartSettings;

    if (pStartSettings->buffer) {
        msg->buffer = pStartSettings->buffer;
        msg->bufferSize = pStartSettings->bufferSize;
        /* convert uncached buffer for XPT PI use */
        if (!NEXUS_Core_P_AddressToHeap(msg->buffer, &msg->uncachedBuffer)) {
            BDBG_ERR(("A user-provided buffer must be allocated with NEXUS_Memory_Allocate in order to be properly translated for HW use."));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    else {
        msg->buffer = msg->allocatedBuffer;
        msg->bufferSize = msg->settings.bufferSize;
    }

    BDBG_ASSERT(msg->uncachedBuffer);

#define GET_BAND(PIDCHANNEL) ((PIDCHANNEL)->status.playback?BXPT_PB_PARSER((PIDCHANNEL)->status.playbackIndex):(PIDCHANNEL)->parserBand->hwIndex)

    msg->PidChannelNum = pStartSettings->pidChannel->status.pidChannelIndex;

    /* NOTE: the remapped PID in status should in sync with the remap setting */
    pid = pStartSettings->pidChannel->status.remappedPid;

    if (msg->PidChannelNum >= BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS) {
        BDBG_ERR(("Only pid channels 0..%d can be used for message filtering.", BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS-1));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    {
        unsigned bSize, temp;

        /* calculate actual bufferSize as a log of 2 */
        bufferSizeEnum = BXPT_MessageBufferSize_e1kB;
        bSize = 1024;
        temp = msg->bufferSize / 1024;
        while (temp > 1 && bSize < 512*1024)
        {
            temp /= 2; /* throw away remainder */
            bufferSizeEnum++;
            bSize *= 2;
        }
        BDBG_ASSERT(bufferSizeEnum <= BXPT_MessageBufferSize_e512kB);
        if (bSize < msg->settings.bufferSize) {
            BDBG_WRN(("only %d out of %d bytes of message buffer will be used.", bSize, msg->settings.bufferSize));
        }
    }

    rc = BXPT_Mesg_SetPidChannelBuffer(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, msg->uncachedBuffer, bufferSizeEnum);
    if (rc) {return BERR_TRACE(rc);}

    if (pStartSettings->format == NEXUS_MessageFormat_ePsi) {
        bool override;
        BXPT_PidPsiConfig psiConfig;

        rc = BXPT_Mesg_GetPidChannelPsiSettings(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, &override, &psiConfig);
        if (rc) return BERR_TRACE(rc);
        psiConfig.PsfCrcDis = pStartSettings->psfCrcDisabled;
#if 0
        psiConfig.SaveLongPsiMsg = true; /* set this to capture long PSI messages. */
#endif
        override = true;
        rc = BXPT_Mesg_SetPidChannelPsiSettings(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, override, &psiConfig);
        if (rc) return BERR_TRACE(rc);

        if (pStartSettings->bank != -1) {
            /* explicit filter selection */
            rc = BXPT_AllocPSIFilter( pTransport->xpt, pStartSettings->bank, &msg->FilterNum );
            if (rc) return BERR_TRACE(rc);

            msg->BankNum = (unsigned)pStartSettings->bank;
        }
        else {
            unsigned i;
            unsigned least_used_i = 0;
            unsigned least_used_amt = 0xFFFF;

            /* start with the least used bank */
            for (i=0; i<BXPT_NUM_FILTER_BANKS; i++){
                if (pTransport->message.bank_refcnt[i] < least_used_amt) {
                    least_used_i = i;
                    least_used_amt = pTransport->message.bank_refcnt[i];
                }
            }

            /* For easy application code, we search for an available bank. */
            for (i=least_used_i; i<BXPT_NUM_FILTER_BANKS; i++){
                rc = BXPT_AllocPSIFilter( pTransport->xpt, i, &msg->FilterNum );
                if (!rc) break;
            }
            if (i == BXPT_NUM_FILTER_BANKS) {
                for (i=0; i<least_used_i; i++){
                    rc = BXPT_AllocPSIFilter( pTransport->xpt, i, &msg->FilterNum );
                    if (!rc) break;
                }
                if (i == least_used_i) {
                    i = BXPT_NUM_FILTER_BANKS;
                }
            }
            if (i == BXPT_NUM_FILTER_BANKS) {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            msg->BankNum = i;
        }
        pTransport->message.bank_refcnt[msg->BankNum]++;

        BDBG_CASSERT(sizeof(pStartSettings->filter) == sizeof(BXPT_Filter));
        rc = BXPT_SetFilter( pTransport->xpt, msg->BankNum, msg->FilterNum, (const BXPT_Filter *)&pStartSettings->filter);
        if (rc) { rc = BERR_TRACE(rc); goto setfilter_error;}

        BXPT_GetDefaultPsiSettings(&msg->psiMessageSettings);
        msg->psiMessageSettings.Bank = msg->BankNum;
        msg->psiMessageSettings.FilterEnableMask = 1UL<<msg->FilterNum;
        msg->psiMessageSettings.CrcDisable = pStartSettings->psiCrcDisabled;
        msg->psiMessageSettings.Pid = pid;
        msg->psiMessageSettings.Band = GET_BAND(pStartSettings->pidChannel);
        msg->psiMessageSettings.StartingOffset = pStartSettings->filterOffset;
        msg->psiMessageSettings.SkipByte2 = pStartSettings->includeThirdFilterByte ? false : true;  /* BXPT preserves the hw's inverted logic. */
#if !NEXUS_HAS_LEGACY_XPT
        msg->psiMessageSettings.UseRPipe = pStartSettings->useRPipe;
#endif

        if (msg->startSettings.filterGroup) {
            rc = BXPT_Mesg_AddFilterToGroup(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, msg->FilterNum, &msg->psiMessageSettings);
            if (rc) { rc = BERR_TRACE(rc); goto addfiltertogroup_error;}
        }

#if B_HAS_DSS
        if (NEXUS_IS_DSS_MODE(pStartSettings->pidChannel->status.transportType)) {
            BDBG_MSG(("StartDirecTvMessageCapture %p: pidch=%d msgbuf=%d bank=%d", msg, msg->PidChannelNum, msg->MesgBufferNum, msg->BankNum));
            BDBG_CASSERT(NEXUS_DssMessageType_eMax-1 == BXPT_DirecTvMessageType_eRegular_CapFilter4);
            rc = BXPT_Mesg_StartDirecTvMessageCapture(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum,
                (BXPT_DirecTvMessageType)pStartSettings->dssMessageType,
                &msg->psiMessageSettings);
            if (rc) { rc = BERR_TRACE(rc); goto start_error;}
        }
        else
#endif
        {
            BDBG_MSG(("StartPsiMessageCapture %p: pidch=%d msgbuf=%d bank=%d", msg, msg->PidChannelNum, msg->MesgBufferNum, msg->BankNum));
            rc = BXPT_Mesg_StartPsiMessageCapture(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, &msg->psiMessageSettings);
            if (rc) { rc = BERR_TRACE(rc); goto start_error;}
        }
    }
    else {
        BXPT_PidChannelRecordSettings ChanSettings;

        BKNI_Memset(&ChanSettings, 0, sizeof(ChanSettings)); /* no XPT init func */
        ChanSettings.Pid = pid;
        ChanSettings.Band = GET_BAND(pStartSettings->pidChannel);
#if NEXUS_HAS_LEGACY_XPT
        ChanSettings.SaveAllCountType = 0;
        ChanSettings.SaveAllCount = 10;
#endif
        ChanSettings.ByteAlign = false;
        switch (pStartSettings->format) {
        case NEXUS_MessageFormat_eTs:
            ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePacketSaveAll;
            break;
        case NEXUS_MessageFormat_ePes:
            ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePes;
            break;
        default:
            ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePesSaveAll;
            break;
        }
#if !NEXUS_HAS_LEGACY_XPT
        ChanSettings.UseRPipe = pStartSettings->useRPipe;
#endif

        rc = BXPT_Mesg_StartPidChannelRecord(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, &ChanSettings);
        if (rc) return BERR_TRACE(rc);
    }

    NEXUS_Message_P_EnableInterrupt(msg, true);
    NEXUS_PidChannel_ConsumerStarted(pStartSettings->pidChannel);

    /* state is controlled by length integers */
    msg->wrappedMessage.length = 0;
    msg->wrappedMessage.amountConsumed = 0;
    msg->getBufferState.length = 0;
    msg->getBufferState.amountConsumed = 0;
    msg->lastGetBufferLength = 0;

    msg->started = true;
    NEXUS_OBJECT_ACQUIRE(msg, NEXUS_PidChannel, pStartSettings->pidChannel);
    return 0;

start_error:
    BXPT_RemoveFilterFromGroup(pTransport->xpt, msg->FilterNum, &msg->psiMessageSettings);
addfiltertogroup_error:
setfilter_error:
    BXPT_FreePSIFilter( pTransport->xpt, msg->BankNum, msg->FilterNum );
    pTransport->message.bank_refcnt[msg->BankNum]--;
    return rc;
}

void NEXUS_Message_Stop(NEXUS_MessageHandle msg)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (!msg->started) {
        return;
    }

#if NEXUS_MESSAGE_USE_CHECK_TIMER
    msg->noReadCompleteCount = 0;
#endif
    NEXUS_Message_P_EnableInterrupt(msg, false);

    if (msg->startSettings.format == NEXUS_MessageFormat_ePsi) {
        unsigned i;

#if B_HAS_DSS
        if (NEXUS_IS_DSS_MODE(msg->startSettings.pidChannel->status.transportType)) {
            BDBG_MSG(("StopDirecTvMessageCapture %p: %d %d", msg, msg->PidChannelNum, msg->MesgBufferNum));
            rc = BXPT_Mesg_StopDirecTvMessageCapture(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum );
            if (rc) rc = BERR_TRACE(rc); /* fall through */
        }
        else
#endif
        {
            BDBG_MSG(("StopPsiMessageCapture %p: %d %d", msg, msg->PidChannelNum, msg->MesgBufferNum));
            rc = BXPT_Mesg_StopPsiMessageCapture(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum );
            if (rc) rc = BERR_TRACE(rc); /* fall through */
        }
        rc = BXPT_Mesg_ClearPidChannelBuffer(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum );
        if (rc) rc = BERR_TRACE(rc); /* fall through */

        BXPT_FreePSIFilter(pTransport->xpt, msg->BankNum, msg->FilterNum);

        BDBG_ASSERT(pTransport->message.bank_refcnt[msg->BankNum]);
        pTransport->message.bank_refcnt[msg->BankNum]--;

        for (i=0;i<NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS;i++) {
            if (msg->additionalFilters[i].used) {
                NEXUS_Message_RemoveFilter(msg, msg->additionalFilters[i].filterNum);
            }
        }
    }
    else {
        rc = BXPT_Mesg_StopPidChannelRecord(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum );
        if (rc) rc = BERR_TRACE(rc); /* fall through */
    }

    NEXUS_OBJECT_RELEASE(msg, NEXUS_PidChannel, msg->startSettings.pidChannel);
    msg->startSettings.pidChannel = NULL; /* let go of this handle. it could be closed after Stop returns. */
    msg->started = false;
    msg->buffer = NULL;
    msg->bufferSize = 0;
}

NEXUS_Error NEXUS_Message_AddFilter( NEXUS_MessageHandle msg, const NEXUS_MessageFilter *pFilter, unsigned *pFilterNum )
{
    BERR_Code rc;
    unsigned i;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (!msg->started) {
        /* must be already started to add an additional filter */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (!msg->startSettings.filterGroup) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (msg->startSettings.format != NEXUS_MessageFormat_ePsi) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    for (i=0;i<NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS;i++) {
        if (!msg->additionalFilters[i].used)
            break;
    }
    if (i == NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    rc = BXPT_AllocPSIFilter( pTransport->xpt, msg->BankNum, &msg->additionalFilters[i].filterNum );
    if (rc) return BERR_TRACE(rc);

    pTransport->message.bank_refcnt[msg->BankNum]++;

    BDBG_CASSERT(sizeof(*pFilter) == sizeof(BXPT_Filter));
    rc = BXPT_SetFilter( pTransport->xpt, msg->BankNum, msg->additionalFilters[i].filterNum, (const BXPT_Filter *)pFilter);
    if (rc) {rc = BERR_TRACE(rc); goto setfilter_error;}

    rc = BXPT_Mesg_AddFilterToGroup(pTransport->xpt, msg->PidChannelNum, msg->MesgBufferNum, msg->additionalFilters[i].filterNum, &msg->psiMessageSettings);
    if (rc) {rc = BERR_TRACE(rc); goto addfilter_error;}

    msg->additionalFilters[i].used = true;
    *pFilterNum = msg->additionalFilters[i].filterNum;
    BDBG_MSG_TRACE(("NEXUS_Message_AddFilter(%p,%d) -> %d", msg, *pFilterNum, i));

    return 0;

setfilter_error:
addfilter_error:
    BXPT_FreePSIFilter( pTransport->xpt, msg->BankNum, msg->additionalFilters[i].filterNum );
    pTransport->message.bank_refcnt[msg->BankNum]--;
    return rc;
}

NEXUS_Error NEXUS_Message_RemoveFilter( NEXUS_MessageHandle msg, unsigned filterNum )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    for (i=0;i<NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS;i++) {
        if (msg->additionalFilters[i].used && msg->additionalFilters[i].filterNum == filterNum) {
            BDBG_MSG_TRACE(("NEXUS_Message_RemoveFilter(%p,%d) -> %d", msg, filterNum, i));
            BXPT_RemoveFilterFromGroup(pTransport->xpt, msg->additionalFilters[i].filterNum, &msg->psiMessageSettings);
            BXPT_FreePSIFilter( pTransport->xpt, msg->BankNum, msg->additionalFilters[i].filterNum );

            msg->additionalFilters[i].used = false;
            BDBG_ASSERT(pTransport->message.bank_refcnt[msg->BankNum]);
            pTransport->message.bank_refcnt[msg->BankNum]--;
            return 0;
        }
    }
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}

NEXUS_Error NEXUS_Message_UpdateFilter( NEXUS_MessageHandle msg, unsigned filterNum, const NEXUS_MessageFilter *pFilter )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (!msg->started) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* find or verify the filterNum for this msg handle */
    if (filterNum == NEXUS_MESSAGE_MAIN_FILTER_NUM) {
        filterNum = msg->FilterNum;
    }
    else {
        unsigned i;
        for (i=0;i<NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS;i++) {
            if (msg->additionalFilters[i].used && msg->additionalFilters[i].filterNum == filterNum) {
                break;
            }
        }
        if (i == NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    rc = BXPT_SetFilter( pTransport->xpt, msg->BankNum, filterNum, (const BXPT_Filter *)pFilter);
    if (rc) return BERR_TRACE(rc);

    return 0;
}


#define ADD_XPT_MESSAGE_PAD(message_length) \
    (((message_length) % 4) ? ((message_length) + 4 - ((message_length) % 4)) : (message_length))

NEXUS_Error NEXUS_Message_GetBuffer(NEXUS_MessageHandle msg, const void **buffer, size_t *length)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (!msg->started) {
        return NEXUS_UNKNOWN; /* fail silently */
    }

    if (msg->wrappedMessage.length) {
        *buffer = (uint8_t*)msg->wrappedMessage.buffer + msg->wrappedMessage.amountConsumed;
        BDBG_ASSERT(msg->wrappedMessage.length > msg->wrappedMessage.amountConsumed);
        *length = msg->wrappedMessage.length - msg->wrappedMessage.amountConsumed;
        goto done;
    }

    /* if the user's last ReadComplete did not finish off the GetBuffer, we return the amount left using cached pointers & sizes.  */
    if (msg->getBufferState.length) {
        *buffer = (uint8_t*)msg->getBufferState.buffer + msg->getBufferState.amountConsumed;
        BDBG_ASSERT(msg->getBufferState.length > msg->getBufferState.amountConsumed);
        *length = msg->getBufferState.length - msg->getBufferState.amountConsumed;
        goto done;
    }

    /* read from the HW */
    rc = BXPT_CheckBuffer( pTransport->xpt, msg->MesgBufferNum, (uint8_t **)buffer, length, &msg->status.moreDataAvailable);
    if (rc) {return BERR_TRACE(rc);}
    /* HW is configured to pad up to 4 bytes. report that back to the user and require them to consume it. */
    *length = ADD_XPT_MESSAGE_PAD(*length);

    if (!(*buffer)) {
        goto done;
    }

    /* converted back to cached address for internal and external use */
    *buffer = ((uint8_t*)*buffer - (uint8_t*)msg->uncachedBuffer) + (uint8_t*)msg->buffer;
    if (*length) {
        NEXUS_Memory_FlushCache((void*)*buffer, *length);
    }

    /* remember last read from HW */
    msg->getBufferState.buffer = *buffer;
    msg->getBufferState.length = *length;

#define TS_PSI_GET_SECTION_LENGTH( BUF ) (((((uint8_t*)(BUF))[1] << 8) | ((uint8_t*)(BUF))[2]) & 0x0FFF)

#if 0
    if (*length) {
        unsigned message_length;
        BDBG_ASSERT(*length >= 3);
        message_length = TS_PSI_GET_SECTION_LENGTH(*buffer) + 3;
        message_length = ADD_XPT_MESSAGE_PAD(message_length);
        BDBG_WRN(("%p: message %d, in getbuffer %d", msg, message_length, *length));
    }
#endif

    if (msg->wrappedMessage.buffer && msg->status.moreDataAvailable && msg->startSettings.format == NEXUS_MessageFormat_ePsi
#if B_HAS_DSS
        && !NEXUS_IS_DSS_MODE(msg->startSettings.pidChannel->status.transportType)
#endif
        )
    {
        uint8_t *buffer2;
        size_t length2;
        unsigned message_length;
        bool skip = false;
        bool MoreDataAvailable;

        /* the 4 byte pad ensures we can check the length before the wrap */
        BDBG_ASSERT(*length >= 3);

        /* we could have multiple messages at the end of the buffer. we only need to memcpy the one message that wrapped.
        so, parse message lengths until we get to the wrapped message. */
        message_length = TS_PSI_GET_SECTION_LENGTH(*buffer) + 3;
        message_length = ADD_XPT_MESSAGE_PAD(message_length);

        if (message_length <= *length) {
            /* if we have an unwrapped message preceding the wrapped message, change the length value to return only it for this call. this simplies our logic greatly.
            this assumes the XPT PI will return whatever portion was not consumed, including setting the MoreDataAvailable boolean again. */
            msg->getBufferState.length = *length = message_length;
            goto done;
        }

        /* now we have a single wrapped message */

        if (message_length > msg->settings.maxContiguousMessageSize) {
            BDBG_ERR(("Wrapped message of size %d bytes is larger than NEXUS_MessageSettings.maxContiguousMessageSize %d.", message_length, msg->settings.maxContiguousMessageSize));
            skip = true;
            /* keep going. still need to consume the message from HW. */
        }
        else {
            BKNI_Memcpy(msg->wrappedMessage.buffer, *buffer, *length);
        }

        /* consume the end of the buffer */
        rc = BXPT_UpdateReadOffset( pTransport->xpt, msg->MesgBufferNum, *length);
        if (rc) return BERR_TRACE(rc);

        /* get the beginning of the buffer, which could be more than just the remainder of the wrapped message.
        don't change msg->status.moreDataAvailable. */
        rc = BXPT_CheckBuffer( pTransport->xpt, msg->MesgBufferNum, &buffer2, &length2, &MoreDataAvailable);
        if (rc) {return BERR_TRACE(rc);}
        length2 = ADD_XPT_MESSAGE_PAD(length2);

        if (!skip) {
            if (length2 + *length < message_length) {
                BDBG_ERR(("Invalid wrapped message. Message is %d bytes, but two portions are %d and %d bytes.\n",
                    message_length, *length, length2));
                skip = true;
                /* keep going. still need to consume the message from HW. */
            }
            else {
                /* only process the remainder of the one message */
                unsigned remainder = message_length - *length;

                BKNI_Memcpy((uint8_t*)msg->wrappedMessage.buffer + *length, buffer2, remainder);
                msg->wrappedMessage.length = message_length;
                msg->wrappedMessage.amountConsumed = 0;
                BDBG_MSG(("%p copied wrapped message: %d = %d + %d", msg, msg->wrappedMessage.length, *length, remainder));

                /* the user gets a pointer & size to the data in the wrappedMessage buffer */
                *buffer = msg->wrappedMessage.buffer;
                *length = msg->wrappedMessage.length;

                /* we can consume the remainder from HW right now. we are operating out of the wrappedMessage.buffer */
                rc = BXPT_UpdateReadOffset( pTransport->xpt, msg->MesgBufferNum, remainder);
                if (rc) return BERR_TRACE(rc);

                msg->getBufferState.length = 0;
                goto done;
            }
        }

        if (skip) {
            /* just consume it all and return no data. hopefully we recover. */
            rc = BXPT_UpdateReadOffset( pTransport->xpt, msg->MesgBufferNum, length2);
            if (rc) return BERR_TRACE(rc);

            *buffer = NULL;
            *length = 0;
            msg->getBufferState.length = 0;
            goto done;
        }
    }

done:
    msg->lastGetBufferLength = *length;

    if (!msg->lastGetBufferLength) {
        /* reenable the callback if zero, otherwise a call to ReadComplete is required */
        NEXUS_Message_P_EnableInterrupt(msg, true);
    }
#if NEXUS_MESSAGE_USE_CHECK_TIMER
    else {
        /* leave disabled.  */
        msg->noReadCompleteCount = 1; /* kick start checkTimer */
    }
#endif

    if (*length) {
        BDBG_MSG(("GetBuffer %p: %p %d", msg, *buffer, *length));
    }

    return 0;
}

NEXUS_Error NEXUS_Message_GetBufferWithWrap( NEXUS_MessageHandle msg, const void **pBuffer, size_t *pLength, const void **pBuffer2, size_t *pLength2 )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (msg->started == false) {
        return BERR_UNKNOWN;
    }
    if (msg->settings.maxContiguousMessageSize) {
        BDBG_ERR(("NEXUS_Message_GetBufferWithWrap requires maxContiguousMessageSize == 0"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* read from the HW */
    rc = BXPT_CheckBufferWithWrap( pTransport->xpt, msg->MesgBufferNum, (uint8_t **)pBuffer, pLength, (uint8_t **)pBuffer2, pLength2);
    if (rc) {return BERR_TRACE(rc);}

    /* HW is configured to pad up to 4 bytes. report that back to the user and require them to consume it. */
    if (*pLength2) {
        *pLength2 = ADD_XPT_MESSAGE_PAD(*pLength2);
    }
    else {
        *pLength = ADD_XPT_MESSAGE_PAD(*pLength);
    }

    if (*pBuffer) {
        /* converted back to cached address for internal and external use */
        *pBuffer = ((uint8_t*)*pBuffer - (uint8_t*)msg->uncachedBuffer) + (uint8_t*)msg->buffer;
        if (*pLength) {
            NEXUS_Memory_FlushCache((void*)*pBuffer, *pLength);
        }
    }
    if (*pBuffer2) {
        /* converted back to cached address for internal and external use */
        *pBuffer2 = ((uint8_t*)*pBuffer2 - (uint8_t*)msg->uncachedBuffer) + (uint8_t*)msg->buffer;
        if (*pLength2) {
            NEXUS_Memory_FlushCache((void*)*pBuffer2, *pLength2);
        }
    }

    /* don't cache buffer & length with msg->getBufferState. NEXUS_Message_GetBufferWithWrap is only used if user is trying to peek ahead without completing what
    is already received, so we need the updated information. */
    msg->getBufferState.buffer = NULL;
    msg->getBufferState.length = 0;

    msg->lastGetBufferLength = *pLength + *pLength2;

    if (!msg->lastGetBufferLength) {
        /* reenable the callback if zero, otherwise a call to ReadComplete is required */
        NEXUS_Message_P_EnableInterrupt(msg, true);
    }
#if NEXUS_MESSAGE_USE_CHECK_TIMER
    else {
        /* leave disabled.  */
        msg->noReadCompleteCount = 1; /* kick start checkTimer */
    }
#endif

    if (msg->lastGetBufferLength) {
        BDBG_MSG(("GetBufferWithWrap %p: %p %d, %p %d", msg, *pBuffer, *pLength, *pBuffer2, *pLength2));
    }

    return 0;
}

NEXUS_Error NEXUS_Message_ReadComplete(NEXUS_MessageHandle msg, size_t amountConsumed)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (msg->started == false) {
        return BERR_UNKNOWN;
    }

    if (amountConsumed > msg->lastGetBufferLength) {
        /* you can never consume more than you were last given. a ReadComplete call must always be preceded by a GetBuffer call. */
        BDBG_ERR(("NEXUS_Message_ReadComplete called with %d, but last NEXUS_Message_GetBuffer only returned %d", amountConsumed, msg->lastGetBufferLength));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (msg->wrappedMessage.length) {
        msg->wrappedMessage.amountConsumed += amountConsumed;
        BDBG_MSG(("ReadComplete %p: %d, %d out of %d consumed (wrapped)", msg, amountConsumed, msg->wrappedMessage.amountConsumed, msg->wrappedMessage.length));
        if (msg->wrappedMessage.amountConsumed < msg->wrappedMessage.length) {
            goto done;
        }
        BDBG_ASSERT(msg->wrappedMessage.amountConsumed == msg->wrappedMessage.length);
        msg->wrappedMessage.length = 0;
        msg->wrappedMessage.amountConsumed = 0;
        goto done;
    }

    if (msg->getBufferState.length) {
        msg->getBufferState.amountConsumed += amountConsumed;
        BDBG_MSG(("ReadComplete %p: %d, %d out of %d consumsed", msg, amountConsumed, msg->getBufferState.amountConsumed, msg->getBufferState.length));

        if (msg->getBufferState.amountConsumed < msg->getBufferState.length) {
            /* we can't update HW until the full GetBuffer length is consumed. this is required so that we don't have to parse message
            lengths until we hit a wrap around. at that point, we must parse lengths so that we can copy only the one wrapped message into wrappedMessage.buffer. */
            goto done;
        }

        /* user has consumed it all. now tell HW. */
        BDBG_ASSERT(msg->getBufferState.amountConsumed == msg->getBufferState.length);

        rc = BXPT_UpdateReadOffset( pTransport->xpt, msg->MesgBufferNum, msg->getBufferState.length);
        if (rc) return BERR_TRACE(rc);

        msg->getBufferState.length = 0;
        msg->getBufferState.amountConsumed = 0;
    }
    else {
        rc = BXPT_UpdateReadOffset( pTransport->xpt, msg->MesgBufferNum, amountConsumed);
        if (rc) return BERR_TRACE(rc);
    }

done:
    /* reenable the callback */
    NEXUS_Message_P_EnableInterrupt(msg, true);
#if NEXUS_MESSAGE_USE_CHECK_TIMER
    msg->noReadCompleteCount = 0;
#endif
    msg->lastGetBufferLength = 0; /* require another GetBuffer before a ReadComplete, even if we have a remainder */
    return 0;
}

NEXUS_Error NEXUS_Message_GetStatus( NEXUS_MessageHandle msg, NEXUS_MessageStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    *pStatus = msg->status;
    return 0;
}

NEXUS_Error NEXUS_Message_SetDssCapPattern( unsigned capFilterIndex, uint32_t pattern)
{
    #if B_HAS_DSS
    return BXPT_DirecTv_SetCapPattern(pTransport->xpt, capFilterIndex, pattern);
    #else
    /* get of compiler warnings for the ! B_HAS_DSS case. */
    capFilterIndex = capFilterIndex; pattern = pattern;
    return BERR_SUCCESS;
    #endif
}

#else /* NEXUS_NUM_MESSAGE_FILTERS */

/* stubs */

struct NEXUS_Message {
    BDBG_OBJECT(NEXUS_Message)
};

void NEXUS_Message_GetDefaultSettings(NEXUS_MessageSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_MessageHandle NEXUS_Message_Open(const NEXUS_MessageSettings *pSettings)
{
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_Message_Close(NEXUS_MessageHandle msg)
{
    BSTD_UNUSED(msg);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

void NEXUS_Message_GetSettings( NEXUS_MessageHandle msg, NEXUS_MessageSettings *pSettings )
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_Message_SetSettings( NEXUS_MessageHandle msg, const NEXUS_MessageSettings *pSettings )
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Message_GetDefaultStartSettings(NEXUS_MessageHandle msg, NEXUS_MessageStartSettings *pStartSettings)
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(pStartSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_Message_Start(NEXUS_MessageHandle msg, const NEXUS_MessageStartSettings *pStartSettings)
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(pStartSettings);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Message_Stop(NEXUS_MessageHandle msg)
{
    BSTD_UNUSED(msg);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_Message_GetBuffer(NEXUS_MessageHandle msg, const void **buffer, size_t *length)
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(buffer);
    BSTD_UNUSED(length);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_Message_ReadComplete(NEXUS_MessageHandle msg, size_t amountConsumed)
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(amountConsumed);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_Message_GetStatus( NEXUS_MessageHandle msg, NEXUS_MessageStatus *pStatus )
{
    BSTD_UNUSED(msg);
    BSTD_UNUSED(pStatus);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_Message_SetDssCapPattern( unsigned capFilterIndex, uint32_t pattern)
{
    BSTD_UNUSED(capFilterIndex);
    BSTD_UNUSED(pattern);
    BDBG_WRN(("Message Filter not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Message_GetDefaultFilter(NEXUS_MessageFilter *pFilter)
{
    BKNI_Memset(pFilter, 0, sizeof(*pFilter));
    BKNI_Memset(pFilter->mask, 0xFF, sizeof(pFilter->mask));
    BKNI_Memset(pFilter->exclusion, 0xFF, sizeof(pFilter->exclusion));
}

NEXUS_Error NEXUS_Message_AddFilter( NEXUS_MessageHandle msg, const NEXUS_MessageFilter *pFilter, unsigned *pFilterNum )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    BSTD_UNUSED(pFilter);
    BSTD_UNUSED(pFilterNum);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Message_RemoveFilter( NEXUS_MessageHandle msg, unsigned filterNum )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    BSTD_UNUSED(filterNum);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Message_UpdateFilter( NEXUS_MessageHandle msg, unsigned filterNum, const NEXUS_MessageFilter *pFilter )
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    BSTD_UNUSED(filterNum);
    BSTD_UNUSED(pFilter);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

#endif /* NEXUS_NUM_MESSAGE_FILTERS */

/* private functions */

void NEXUS_Message_P_FireInterrupt_isr(NEXUS_MessageHandle msg, unsigned pidChannelIndex, NEXUS_XptDataInterrupt xptDataInterrupt)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

#if NEXUS_NUM_MESSAGE_FILTERS
    /* In this mode, the pidChannelIndex given is actually the message buffer number */
    if (msg->MesgBufferNum == (int)pidChannelIndex) {
        switch (xptDataInterrupt) {
        case NEXUS_XptDataInterrupt_ePsiLengthError:
            NEXUS_IsrCallback_Fire_isr(msg->psiLengthError);
            break;
        case NEXUS_XptDataInterrupt_eCrcError:
            NEXUS_IsrCallback_Fire_isr(msg->crcError);
            break;
        case NEXUS_XptDataInterrupt_ePesLengthError:
            NEXUS_IsrCallback_Fire_isr(msg->pesLengthError);
            break;
        case NEXUS_XptDataInterrupt_ePesStartCodeError:
            NEXUS_IsrCallback_Fire_isr(msg->pesStartCodeError);
            break;
        default:
            break;
        }
    }
#else
    BSTD_UNUSED(pidChannelIndex);
    BSTD_UNUSED(xptDataInterrupt);
#endif
}

bool NEXUS_Message_P_HasCallback(NEXUS_MessageHandle msg, NEXUS_XptDataInterrupt xptDataInterrupt)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
#if NEXUS_NUM_MESSAGE_FILTERS
    switch (xptDataInterrupt) {
    case NEXUS_XptDataInterrupt_ePsiLengthError: return (msg->settings.psiLengthError.callback != NULL);
    case NEXUS_XptDataInterrupt_eCrcError: return (msg->settings.crcError.callback != NULL);
    case NEXUS_XptDataInterrupt_ePesLengthError: return (msg->settings.pesLengthError.callback != NULL);
    case NEXUS_XptDataInterrupt_ePesStartCodeError: return (msg->settings.pesStartCodeError.callback != NULL);
    default: return false;
    }
#else
    BSTD_UNUSED(xptDataInterrupt);
    return false;
#endif
}
