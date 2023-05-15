/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmsglib.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 7/8/09 2:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/msglib/7401/bmsglib.h $
 * 
 * Hydra_Software_Devel/11   7/8/09 2:57p erickson
 * PR53768: removed BMSGlib_BufferMode. it was never implemented and has
 * no use.
 *
 * Hydra_Software_Devel/10   3/24/09 10:01a erickson
 * PR53516: added BMSGlib_GetStatus
 *
 * Hydra_Software_Devel/9   6/18/08 1:08p erickson
 * PR43730: fix warnings by moving param to size_t
 *
 * Hydra_Software_Devel/8   5/9/08 1:29p erickson
 * PR42456: added BMSGlib_Format_ePES_SAVE_ALL. clarified that
 * maxContiguousMessageSize only applies to PSI messages.
 *
 * Hydra_Software_Devel/7   4/14/08 1:16p erickson
 * PR41730: move user buffer override from Open time to Start time to give
 * maximum flexibility
 *
 * Hydra_Software_Devel/6   3/21/08 11:04a erickson
 * PR40813: added BMSGlib_SessionParams.bank. Removed unimplemented
 * BMSGlib_Format's. Clarified mem alloc design.
 *
 * Hydra_Software_Devel/5   3/10/08 1:27p erickson
 * PR39836: enforce memory allocation rules for group masters. If the
 * group master Closes, all other sessions in the group must be Stopped.
 * Don't allow the group master to restart if there are other sessions
 * still active in the group.
 *
 * Hydra_Software_Devel/4   2/26/08 10:29p erickson
 * PR39781: allow maxContiguousMessageSize == 0 for no data copy on wrap
 * around
 *
 * Hydra_Software_Devel/3   9/13/07 1:42p gmullen
 * PR32868: Changed bmsglib to accept PID channel number, rather than PID
 * and band combo.
 *
 * Hydra_Software_Devel/2   7/26/07 11:11p erickson
 * PR32868: added more debug and verification
 *
 * Hydra_Software_Devel/1   7/26/07 12:35p erickson
 * PR32868: initial checkin of BMSGlib
 *
 ************************************************************************/
#ifndef BMSGLIB_H__
#define BMSGLIB_H__

/*=******************************
Provide library for message filtering, especially for an efficient, reliable implementation of filter groups.

740x platforms have a HW restriction which limits a pid/band to be routed to the message filter block only once.
This creates the requirement to use filter groups if you want to apply more than one filter on a pid.
This HW restriction does not exist in all chips. For those chips which supports duplicate pid channels, BMSGlib_SessionParams.filterGroup
defaults to false.

Filter groups allow multiple filters to be applied on a single pid channel, but places all filtered messages into a
single ring buffer. This requires the application to demux the messages per filter.

BMSGlib supports high-bandwidth message capture. Therfore it minimizes any data copy.

Sample Usage:
void dataReadyCallback_isr(void *context)
{
    BKNI_SetEvent((BKNI_EventHandle)context);
}

main()
{
    uint16_t pids[]; // given
    BKNI_EventHandle event;
    BMSGlib_Settings settings;
    BMSGlib_Handle msglib;
    BMSGlib_Session_Handle msglib_session;

    BMSGlib_GetDefaultSettings(&settings);
    settings.xpt = xpt;
    settings.mem = mem;
    BMSGlib_Open(&msglib, settings);

    BKNI_CreateEvent(&event);

    BMSGlib_GetDefaultSessionSetting(&session_settings);
    session_settings.bufferSize = 4*1024; // let MSGlib alloc
    session_settings.dataReadyCallback_isr = dataReadyCallback_isr;
    session_settings.callbackContext = event;
    BMSGlib_OpenSession(&msglib_session, &session_settings);

    for (i=0;i<total_pids;i++) {
        BMSGlib_SessionParams params;

        BMSGlib_GetDefaultSessionParams(msglib_session, &params);
        params.format = BMSGlib_Format_ePSI;
        params.pid = pids[i];
        params.parserBand = X;
        params.filterGroup = false; // don't need filter groups because this is synchronous
        BMSGlib_StartSession(msglib_session, &params);

        // event will fire when data is available. this app assumes it will.
        BKNI_WaitForEvent(event, BKNI_INFINITE);
        rc = BMSGlib_GetBuffer(msglib_session, &buffer, &length);
        if (rc) continue; // error

        // buffer is guaranteed to contain at least one whole message
        BDBG_ASSERT(length);

        process_message(buffer, length);

        BMSGlib_StopSession(msglib_session);
    }
    BMSGlib_CloseSession(msglib_session);
    BMSGlib_Close(msglib);
    BKNI_DestroyEvent(event);
}
*********************************/

#include "bstd.h"
#include "bmem.h"
#include "bxpt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A global handle is needed for resource allocation. */
typedef struct BMSGlib_Impl *BMSGlib_Handle;

/* A session is one filter for a band/pid combination. */
typedef struct BMSGlib_Session_Impl *BMSGlib_Session_Handle;

/*
Summary:
The settings needed for opening the BMSGlib singleton.
*/
typedef struct BMSGlib_Settings {
    BMEM_Handle mem;
    BXPT_Handle xpt;
} BMSGlib_Settings;

/*
Summary:
Calltype typedef used by the session to notify the caller of state changes or other information.
*/
typedef void (*BMSGlib_Callback)(void *context);

/*
Summary:
The settings needed for opening a session.

Description:
If you use filter groups, only the memory from the first filter in the group will be used.
See BMSGlib_OpenSession for more details on buffer allocation options.
*/
typedef struct BMSGlib_SessionSettings {
    unsigned bufferSize; /* Size in bytes. Only certain buffer sizes may be supported by HW. This size will be rounded down to the nearest supported value.
                            If you want to control the allocation and use of memory, set this to zero; then set BMSGlib_SessionParams.buffer. */

    void *callbackContext; /* app context pointer passed to all BMSGlib_Callbacks. */
    BMSGlib_Callback dataReadyCallback_isr; /* Called whenever data is available.
        App should schedule future call to BMSGlib_GetBuffer, but cannot call BMSGlib_GetBuffer in this isr callback.
        This is required for filter groups.
        If a dataReadyCallback is sent, it is possible that GetBuffer may still return no data. */
    BMSGlib_Callback overflowCallback_isr; /* called if overflow occurs. app should return very quickly. */

    unsigned maxContiguousMessageSize; /* The maximum size PSI message that is guaranteed to be returned as a whole message from BMSGlib_GetBuffer.
                                          For high bitrate message capture, set this to zero. This will ensure no data copy and GetBuffer will not
                                          guarantee to return whole messages.
                                          This does not apply to PES/TS capture. */
} BMSGlib_SessionSettings;

/*
The type of data being captured in a session.
*/
typedef enum BMSGlib_Format {
    BMSGlib_Format_ePSI,            /* capture PSI sections */
    BMSGlib_Format_ePES,            /* capture entire PES packets, respecting PES packet length */
    BMSGlib_Format_ePES_SAVE_ALL,   /* capture entire TS payload, without reference to PES packet length */
    BMSGlib_Format_eTS              /* capture entire transport stream */
} BMSGlib_Format;

/*
Summary:
The parameters needed for starting a session.
*/
typedef struct BMSGlib_SessionParams {
    BMSGlib_Format format;

    int bank; /* Select filter bank 0..3. If -1, MSGlib will automatically select the bank. Default is -1.
                 For filter groups, HW requires that all filters in a group be on the same bank. MSGlib will
                 attempt to do this. However, depending on the pattern of application filter usage, automatic filter selection may
                 not be possible. If you hit this case, your application will need to use explicit filter bank selection. */

    /* Number of the PID channel that carries the data. NOTE: Caller must configure the channel before starting the session. */
    unsigned PidChannel;

    BXPT_Filter filter; /* See bxpt.h for documentation. Be aware that byte 2 is skipped. */
    bool filterGroup; /* If true, BMSGlib will use filter groups. This value defaults to true only on those platforms
        where filter groups are required for multiple filters on the same pid. */
    bool psiCrcDisabled; /* Disable CRC checks on PSI information */
    bool psfCrcDisabled; /* Disable CRC checks on short form private sections */

    void *buffer; /* Optional user-supplied uncached-memory buffer which is bufferSize in length.
                     If NULL, MSGlib will use the buffer allocated with BMSGlib_SessionSettings.bufferSize.
                     Buffers must be physically contiguous, 1K aligned and BMSGlib_Settings.mem should be able to convert between physical address and cached memory. */
    unsigned bufferSize; /* Size in bytes. Only certain buffer sizes may be supported by HW. The actual size used will be rounded down to the nearest supported value. */
} BMSGlib_SessionParams;

/*
Summary:
Get default settings for BMSGlib_Open.
*/
void BMSGlib_GetDefaultSettings(
    BMSGlib_Settings *settings /* [out] */
    );

/*
Summary:
Open the BMSGlib handle singleton.

Description:
This provides a global resource manager for BMSGlib_Sessions.
*/
BERR_Code BMSGlib_Open(
    BMSGlib_Handle *handle,
    const BMSGlib_Settings *settings
    );

/*
Summary:
Close the BMSGlib handle singleton.
*/
void BMSGlib_Close(
    BMSGlib_Handle handle
    );

/*
Summary:
Get default settings for BMSGlib_OpenSession
*/
void BMSGlib_GetDefaultSessionSettings(
    BMSGlib_Handle handle,
    BMSGlib_SessionSettings *settings /* [out] */
    );

/*
Summary:
Open a session for capturing messages. Each session can have one filter applied to a parser_band/pid pair at a time.

Description:
Each session provides control over memory allocation and callback management.
Memory will be allocated from BMEM if BMSGlib_SessionSettings.bufferSize is non-zero.
You can restart sessions multiple times with different SessionParams without reallocating memory.

If you are using filter groups, the first session which is started for the PID is considered the group master.
The buffer memory of the group master is used for message capture. The buffer memory for the other sessions in the group is not used.
The group master's buffer memory remains in use by the group until all sessions in the group are stopped.
If the group master is closed before the other sessions in the group are stopped, the group master must stop any other sessions in the group.
If the group master is started on another pid before the other sessions in the group are stopped, the group master start will fail.

If you want the flexibility to switch any filter in and out of a group, without stopping the other filters, you should allocate your
own memory with BMEM and give it to BMSGlib using BMSGlib_SessionParams.buffer.
If you do, MSGlib will not automatically stop filters, regardless of any Close or pid switch.
The user must ensure the following:
1) that all message filters that use a buffer are stoppped before the buffer is deallocated.
2) that all message filters that use a buffer are filtering on the same pid.
*/
BERR_Code BMSGlib_OpenSession(
    BMSGlib_Handle msglib,
    BMSGlib_Session_Handle *session,
    const BMSGlib_SessionSettings *settings
    );

/*
Summary:
Close this session

Description:
If this is the group master, this will cause all other sessions in the group to be stopped.
See BMSGlib_OpenSession for details.
*/
void BMSGlib_CloseSession(
    BMSGlib_Session_Handle session
    );

/*
Summary:
Get default session parameters for BMSGlib_StartSession.
*/
void BMSGlib_GetDefaultSessionParams(
    BMSGlib_Session_Handle session,
    BMSGlib_SessionParams *params /* [out] */
    );

/*
Summary:
Start capturing data with this session.

Description:
If this is the group master, you cannot start on another PID before all other sessions in the group are stopped.
See BMSGlib_OpenSession for details.
*/
BERR_Code BMSGlib_StartSession(
    BMSGlib_Session_Handle session,
    const BMSGlib_SessionParams *params
    );

/*
Summary:
Stop capturing messages with this session.

Description:
You can restart this session with different SessionParams.
*/
void BMSGlib_StopSession(
    BMSGlib_Session_Handle session
    );

/**
Summary:
Get a pointer to a message captured for this session.

Description:
Guaranteed to return whole PSI messages if maxContiguousMessageSize is non-zero. Wrap around is handled internally.
You can call BMSGlib_GetBuffer multiple times without changing BMSGlib state. It is not destructive.
You cannot call BMSGlib_GetBuffer after calling BMSGlib_StopSession.
**/
BERR_Code BMSGlib_GetBuffer(
    BMSGlib_Session_Handle handle,
    const void **buffer, /* [out] cached memory.
        may not come from SessionSettings.buffer. may come from internal buffer if maxContiguousMessageSize > 0,
        or may come from a group buffer (another session in the same filter group).
        Returns NULL if no data is available.
        This is rightly marked as const because the user should not modify the data. If the user does, another BMSGlib_SessionHandle might
        get invalid results. */
    size_t *length /* [out] number of bytes pointed to by buffer. returns 0 if no data is available. */
    );

/*
Summary:
Notify MSGlib of how much data was consumed from the last BMSGlib_GetBuffer call.

Description:
It must be <= the length returned by BMSGlib_GetBuffer.
If < length, you must call BMSGlib_GetBuffer again to reobtain the pointer to the buffer.
That is, you cannot call BMSGlib_ReadComplete more than once after each BMSGlib_GetBuffer.
You cannot call BMSGlib_ReadComplete after calling BMSGlib_StopSession.
*/
BERR_Code BMSGlib_ReadComplete(
    BMSGlib_Session_Handle handle,
    unsigned amount_consumed /* number of bytes the caller has processed. Must be <= the length returned by last call to BMSGlib_GetBuffer. */
    );

/*
Summary:
Status information returned by BMSGlib_GetStatus
*/
typedef struct BMSGlib_Status
{
    unsigned groupMembers; /* total number of filters in the filter group, including this session */
    bool isGroupMaster; /* true if this session is the group master */
} BMSGlib_Status;

/*
Summary:
Get status information about the session
*/
BERR_Code BMSGlib_GetStatus(
    BMSGlib_Session_Handle handle,
    BMSGlib_Status *pStatus /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* BMSGLIB_H__ */
