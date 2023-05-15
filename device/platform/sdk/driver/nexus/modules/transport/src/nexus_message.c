/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_message.c $
 * $brcm_Revision: 32 $
 * $brcm_Date: 12/15/11 2:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/src/nexus_message.c $
 * 
 * 32   12/15/11 2:32p gmullen
 * SW7425-1952: Merged changes to mainline
 * 
 * SW7425-1952/1   12/14/11 4:08p gmullen
 * SW7425-1952: Moved NEXUS_Message structure to nexus_transport_module.h
 * 
 * 31   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 30   9/8/10 4:41p erickson
 * SW3548-3073: moved definition of struct NEXUS_Message into each
 *  implementation
 *
 * 29   9/3/10 11:56a erickson
 * SW35230-1172: add NEXUS_Message_GetBufferWithWrap
 *
 * 28   4/19/10 2:52p erickson
 * SW7420-624: add NEXUS_Message_UpdateFilter
 *
 * 27   4/19/10 2:02p erickson
 * SW7420-624: add NEXUS_Message_GetDefaultFilter and
 *  AddFilter/RemoveFilter stubs
 *
 * 26   3/8/10 1:03p erickson
 * SW7405-4008: null pidChannel setting on Stop. it may no longer be
 *  valid.
 *
 * 25   2/26/10 2:15p erickson
 * SW7325-655: refactor NEXUS_Core_P_AddressToHeap
 *
 * 24   11/12/09 4:44p jhaberf
 * SW35230-1: Put #ifdef's around DSS functions so the drivers for chips
 *  which don't support DSS build.
 *
 * 23   11/6/09 1:57p erickson
 * SW7400-2559: added NEXUS_Message_SetDssCapPattern
 *
 * 22   10/15/09 1:53p erickson
 * SWDEPRECATED-3717: allow NEXUS_Message_GetDefaultStartSettings to be
 *  called without a NEXUS_MessageHandle
 *
 * 21   8/19/09 2:53p erickson
 * PR57800: turn off transport message interrupts before destroying
 *  NEXUS_IsrCallbacks.
 *
 * 20   6/11/09 4:23p erickson
 * PR55511: XPT will automatically enable a pid channel used for message
 *  filtering. Nexus status should reflect this.
 *
 * 19   3/24/09 10:01a erickson
 * PR53516: added NEXUS_Message_GetStatus
 *
 * 18   11/20/08 5:01p erickson
 * PR48848: asserts
 *
 * 17   11/11/08 2:55a erickson
 * PR 48847: added NEXUS_Message_SetSettings
 *
 * 16   9/11/08 3:13p erickson
 * PR46646: added crcError, pesLengthError, pesStartCodeError
 *
 * 14   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 13   5/14/08 12:30p erickson
 * PR41730: translate the cached address to an uncached adderss before
 *  handing to MSGlib
 *
 * 12   5/9/08 1:29p erickson
 * PR42456: added NEXUS_MessageFormat_ePesSaveAll. clarified that
 *  maxContiguousMessageSize only applies for PSI messages.
 *
 * 11   4/14/08 1:17p erickson
 * PR41730: move user buffer override from Open time to Start time to give
 *  maximum flexibility
 *
 * 10   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 9   3/24/08 10:19a erickson
 * PR40813: added bank param
 *
 * 8   3/10/08 11:32a erickson
 * PR39836: set up Nexus callbacks before calling BMSGlib_OpenSession
 *
 * 7   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 6   2/28/08 10:42p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 5   2/26/08 10:30p erickson
 * PR39781: allow maxContiguousMessageSize == 0 for no data copy on wrap
 *  around
 *
 * 4   2/26/08 9:54p erickson
 * PR39836: added asserts
 *
 * 3   2/19/08 2:57p erickson
 * PR39735: assert malloc
 *
 * 2   1/31/08 12:55p erickson
 * PR34925: clarify comments, remove unsupported enums
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/10   11/6/07 3:43p erickson
 * PR34925: added BDBG_OBJECT checking
 *
 * Nexus_Devel/9   11/2/07 10:04a erickson
 * PR36633: fix coding convention
 *
 * Nexus_Devel/8   10/17/07 11:06a erickson
 * PR36068: close MSGlib before the IsrCallbacks
 *
 * Nexus_Devel/7   10/9/07 3:49p erickson
 * PR34925: change Params to StartSettings
 *
 * Nexus_Devel/6   10/4/07 12:54p erickson
 * PR34925: use standard NEXUS_Error values
 *
 * Nexus_Devel/5   10/3/07 1:41p erickson
 * PR34925: converted to more efficient NEXUS_IsrCallback_Fire_isr
 *
 * Nexus_Devel/4   9/25/07 3:07p erickson
 * PR34925: update
 *
 * Nexus_Devel/3   9/25/07 2:01p erickson
 * PR34925: convert to task before QueueCallback
 *
 * Nexus_Devel/2   9/25/07 1:46p erickson
 * PR34925: fix warnings
 *
 * Nexus_Devel/1   9/24/07 2:36p erickson
 * PR34925: added Message
 *
 **************************************************************************/
#include "nexus_transport_module.h"

BDBG_MODULE(nexus_message);

BDBG_OBJECT_ID(NEXUS_Message);

#if 0
/* Moved to nexus_transport_module.h */
/* this is the impl of NEXUS_MessageHandle */
struct NEXUS_Message {
    BDBG_OBJECT(NEXUS_Message)
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_MessageStatus status;
    BMSGlib_Session_Handle session;
    bool started;
    unsigned noReadCompleteCount;
    NEXUS_TimerHandle checkTimer;
    NEXUS_IsrCallbackHandle dataReady, overflow, psiLengthError, crcError, pesLengthError, pesStartCodeError;

};
#endif

void NEXUS_Message_GetDefaultSettings(NEXUS_MessageSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->bufferSize = 4 * 1024;
    pSettings->maxContiguousMessageSize = 4 * 1024;
}

static void NEXUS_message_p_dataready_callback_isr(void *data)
{
    NEXUS_MessageHandle msg = (NEXUS_MessageHandle)data;
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    NEXUS_IsrCallback_Fire_isr(msg->dataReady);
}

static void NEXUS_message_p_overflow_callback_isr(void *data)
{
    NEXUS_MessageHandle msg = (NEXUS_MessageHandle)data;
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    NEXUS_IsrCallback_Fire_isr(msg->overflow);
}

NEXUS_MessageHandle NEXUS_Message_Open(const NEXUS_MessageSettings *pSettings)
{
    BMSGlib_SessionSettings sessionSettings;
    BERR_Code rc;
    NEXUS_MessageSettings settings;
    NEXUS_MessageHandle msg;
    unsigned i;

    if (!pSettings) {
        NEXUS_Message_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    msg = BKNI_Malloc(sizeof(*msg));
    if(!msg) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);return NULL; }

    BKNI_Memset(msg, 0, sizeof(*msg));
    BDBG_OBJECT_SET(msg, NEXUS_Message);
    msg->settings = *pSettings;

    /* must keep track of Message handles in order to route interrupts back out to Message handles */
    for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
        if (!pTransport->message.handle[i]) {
            pTransport->message.handle[i] = msg;
            break;
        }
    }
    if (i == NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES) {
        BDBG_ERR(("You must increase NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES"));
        goto error;
    }

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

    /* set the interrupts */
    (void)NEXUS_Message_SetSettings(msg, pSettings);

    /* open MSGlib after setting up Nexus callbacks */
    BMSGlib_GetDefaultSessionSettings(pTransport->msglib, &sessionSettings);
    sessionSettings.bufferSize = pSettings->bufferSize;
    sessionSettings.maxContiguousMessageSize = pSettings->maxContiguousMessageSize;
    sessionSettings.callbackContext = msg;
    sessionSettings.dataReadyCallback_isr = NEXUS_message_p_dataready_callback_isr;
    sessionSettings.overflowCallback_isr = pSettings->overflow.callback ? NEXUS_message_p_overflow_callback_isr : NULL;
    rc = BMSGlib_OpenSession(pTransport->msglib, &msg->session, &sessionSettings);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    return msg;

error:
    NEXUS_Message_Close(msg);
    return NULL;
}

void NEXUS_Message_Close(NEXUS_MessageHandle msg)
{
    unsigned i;
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    /* Must close MSGlib before the IsrCallbacks */
    if (msg->session) {
        BMSGlib_CloseSession(msg->session);
    }
    /* Guaranteed no callbacks now */

    BKNI_EnterCriticalSection();
    for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
        if (pTransport->message.handle[i] == msg) {
            pTransport->message.handle[i] = NULL;
            break;
        }
    }
    BDBG_ASSERT(i < NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES);
    BKNI_LeaveCriticalSection();
    /* call this after message.handle[] is cleared */
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

    BDBG_OBJECT_DESTROY(msg, NEXUS_Message);
    BKNI_Free(msg);
}

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
    NEXUS_Transport_P_SetInterrupts();

    msg->settings = *pSettings;

    return 0;
}


void NEXUS_Message_GetDefaultStartSettings(NEXUS_MessageHandle msg, NEXUS_MessageStartSettings *pStartSettings)
{
    /* allow NULL msg handle to retrieve defaults w/o a handle */
    BSTD_UNUSED(msg);

    BKNI_Memset(pStartSettings, 0, sizeof(*pStartSettings));
    pStartSettings->format = NEXUS_MessageFormat_ePsi;
    pStartSettings->bufferMode = NEXUS_MessageBufferMode_eContinuous;
    pStartSettings->bank = -1;
    NEXUS_Message_GetDefaultFilter(&pStartSettings->filter);
#if NEXUS_NUM_RAVE_CONTEXTS
    pStartSettings->filterGroup = true;
#endif
}

NEXUS_Error NEXUS_Message_Start(NEXUS_MessageHandle msg, const NEXUS_MessageStartSettings *pStartSettings)
{
    BERR_Code rc;
    BMSGlib_SessionParams sessionParams;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    if (msg->started) {
        BDBG_ERR(("already started"));
        return NEXUS_NOT_SUPPORTED;
    }
    if (!pStartSettings || !pStartSettings->pidChannel) {
        BDBG_ERR(("pidChannel required"));
        return NEXUS_INVALID_PARAMETER;
    }

    msg->startSettings = *pStartSettings;

    BMSGlib_GetDefaultSessionParams(msg->session, &sessionParams);
    switch (pStartSettings->format) {
    case NEXUS_MessageFormat_eTs: sessionParams.format = BMSGlib_Format_eTS; break;
    case NEXUS_MessageFormat_ePes: sessionParams.format = BMSGlib_Format_ePES; break;
    case NEXUS_MessageFormat_ePesSaveAll: sessionParams.format = BMSGlib_Format_ePES_SAVE_ALL; break;
    case NEXUS_MessageFormat_ePsi: sessionParams.format = BMSGlib_Format_ePSI; break;
    default: BDBG_ERR(("invalid format")); return NEXUS_INVALID_PARAMETER;
    }
    sessionParams.PidChannel = pStartSettings->pidChannel->status.pidChannelIndex;
    sessionParams.filterGroup = pStartSettings->filterGroup;
    sessionParams.bank = pStartSettings->bank;
    sessionParams.psiCrcDisabled = pStartSettings->psiCrcDisabled;
    sessionParams.psfCrcDisabled = pStartSettings->psfCrcDisabled;
    if (pStartSettings->buffer) {
        if (!NEXUS_Core_P_AddressToHeap(pStartSettings->buffer, &sessionParams.buffer)) {
            BDBG_ERR(("A user-provided buffer must be allocated with NEXUS_Memory_Allocate in order to be properly translated for HW use."));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        BDBG_ASSERT(sessionParams.buffer);
    }
    sessionParams.bufferSize = pStartSettings->bufferSize;
    BDBG_CASSERT(sizeof(sessionParams.filter) == sizeof(pStartSettings->filter));
    BKNI_Memcpy(&sessionParams.filter, &pStartSettings->filter, sizeof(sessionParams.filter));

    /* NOTE: XPT PI will automatically enable the pid channel inside BXPT_StartPsiMessageCapture or BXPT_StartPidChannelRecord. */
    pStartSettings->pidChannel->status.enabled = true;

    rc = BMSGlib_StartSession(msg->session, &sessionParams);
    if (rc) {return BERR_TRACE(rc);}

    NEXUS_PidChannel_ConsumerStarted(pStartSettings->pidChannel);

    msg->started = true;
    return 0;
}

void NEXUS_Message_Stop(NEXUS_MessageHandle msg)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    if (!msg->started) {
        return;
    }
    BMSGlib_StopSession(msg->session);
    msg->startSettings.pidChannel = NULL; /* let go of this handle. it could be closed after Stop returns. */
    msg->started = false;
}

NEXUS_Error NEXUS_Message_GetBuffer(NEXUS_MessageHandle msg, const void **buffer, size_t *length)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    return BMSGlib_GetBuffer(msg->session, buffer, length);
}

NEXUS_Error NEXUS_Message_GetBufferWithWrap( NEXUS_MessageHandle msg, const void **pBuffer, size_t *pLength, const void **pBuffer2, size_t *pLength2 )
{
    /* not supported, but we can help app compatibility by just mapping to NEXUS_Message_GetBuffer */
    *pBuffer2 = NULL;
    *pLength2 = 0;
    return NEXUS_Message_GetBuffer(msg, pBuffer, pLength);
}

NEXUS_Error NEXUS_Message_ReadComplete(NEXUS_MessageHandle msg, size_t amount_consumed)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    return BMSGlib_ReadComplete(msg->session, amount_consumed);
}

NEXUS_Error NEXUS_Message_GetStatus( NEXUS_MessageHandle msg, NEXUS_MessageStatus *pStatus )
{
    BMSGlib_Status status;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    rc = BMSGlib_GetStatus(msg->session, &status);
    if (rc) return BERR_TRACE(rc);

    pStatus->groupMembers = status.groupMembers;
    pStatus->isGroupMaster = status.isGroupMaster;

    return 0;
}

NEXUS_Error NEXUS_Message_SetDssCapPattern( unsigned capFilterIndex, uint32_t pattern)
{
    #if B_HAS_DSS
    return BXPT_DirecTv_SetCapPattern(pTransport->xpt, capFilterIndex, pattern);
    #else
    return BERR_SUCCESS;
    #endif
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

void NEXUS_Message_P_FireInterrupt_isr(NEXUS_MessageHandle msg, unsigned pidChannelIndex, NEXUS_XptDataInterrupt xptDataInterrupt)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);

    if (msg->startSettings.pidChannel->status.pidChannelIndex == pidChannelIndex) {
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
}

bool NEXUS_Message_P_HasCallback(NEXUS_MessageHandle msg, NEXUS_XptDataInterrupt xptDataInterrupt)
{
    BDBG_OBJECT_ASSERT(msg, NEXUS_Message);
    switch (xptDataInterrupt) {
    case NEXUS_XptDataInterrupt_ePsiLengthError: return (msg->settings.psiLengthError.callback != NULL);
    case NEXUS_XptDataInterrupt_eCrcError: return (msg->settings.crcError.callback != NULL);
    case NEXUS_XptDataInterrupt_ePesLengthError: return (msg->settings.pesLengthError.callback != NULL);
    case NEXUS_XptDataInterrupt_ePesStartCodeError: return (msg->settings.pesStartCodeError.callback != NULL);
    default: return false;
    }
}
