/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmsglib.c $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 4/1/10 2:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/msglib/7401/bmsglib.c $
 * 
 * Hydra_Software_Devel/37   4/1/10 2:39p erickson
 * SW7405-4152: set group->psi_settings.Bank correctly
 * 
 * Hydra_Software_Devel/36   11/10/09 8:53a gmullen
 * SW7405-3365: NULL'd p_session pointer if buffer alloc failed
 * 
 * Hydra_Software_Devel/35   8/19/09 1:01p erickson
 * PR57768: remove unnecessary BMEM_Heap_FlushCache
 *
 * Hydra_Software_Devel/34   7/8/09 2:57p erickson
 * PR53768: removed BMSGlib_BufferMode. it was never implemented and has
 * no use.
 *
 * Hydra_Software_Devel/33   6/5/09 2:30p erickson
 * PR55761: limit bufferSize to 512KB
 *
 * Hydra_Software_Devel/32   5/8/09 2:36p erickson
 * PR53390: add BMSGlib_Flush. flush XPT and MSGlib when XPT message error
 * occurs. this avoids filter group parsing errors due to corruption in
 * the buffers.
 *
 * Hydra_Software_Devel/31   5/6/09 8:22a erickson
 * PR54867: fix BMSGlib_StopSession for non-PSI filters
 *
 * Hydra_Software_Devel/30   4/9/09 1:40p erickson
 * PR54038: improved ERR message. have BMSGlib_ReadComplete still consume
 * data if amount_consumed is wrong.
 *
 * Hydra_Software_Devel/29   3/24/09 10:01a erickson
 * PR53516: added BMSGlib_GetStatus
 *
 * Hydra_Software_Devel/28   3/18/09 5:13p erickson
 * PR53214: convert BMSGlib_GetBuffer recursion into goto
 *
 * Hydra_Software_Devel/27   3/18/09 4:57p erickson
 * PR53214: have BMSGlib_ReadComplete automatically handle 4-byte padding.
 * include a BDBG_WRN so no one's surprised.
 *
 * Hydra_Software_Devel/26   3/12/09 5:17p erickson
 * PR52900: relax check for internally allocated message buffers
 *
 * Hydra_Software_Devel/25   3/9/09 11:00a erickson
 * PR52900: verify that all message filters in a group are using the same
 * buffer
 *
 * Hydra_Software_Devel/24   2/25/09 5:00p erickson
 * PR52511: remove non-const global data, add proper error checking for
 * BKNI_Malloc
 *
 * Hydra_Software_Devel/23   1/27/09 9:04a erickson
 * PR51468: make variable static
 *
 * Hydra_Software_Devel/22   11/21/08 3:48p erickson
 * PR48848: dynamically switch message filter group mastership to a
 * running slave if master stops
 *
 * Hydra_Software_Devel/21   6/18/08 1:07p erickson
 * PR43730: fix warnings by moving param to size_t
 *
 * Hydra_Software_Devel/20   5/30/08 6:40p vishk
 * PR 42684: bsettop_message does not handle PES correctly
 *
 * Hydra_Software_Devel/19   5/9/08 1:29p erickson
 * PR42456: added BMSGlib_Format_ePES_SAVE_ALL. clarified that
 * maxContiguousMessageSize only applies to PSI messages.
 *
 * Hydra_Software_Devel/18   4/14/08 1:16p erickson
 * PR41730: move user buffer override from Open time to Start time to give
 * maximum flexibility
 *
 * Hydra_Software_Devel/17   3/21/08 11:05a erickson
 * PR40813: added BMSGlib_SessionParams.bank.
 *
 * Hydra_Software_Devel/16   3/10/08 1:27p erickson
 * PR39836: enforce memory allocation rules for group masters. If the
 * group master Closes, all other sessions in the group must be Stopped.
 * Don't allow the group master to restart if there are other sessions
 * still active in the group.
 *
 * Hydra_Software_Devel/15   3/10/08 11:33a erickson
 * PR39836: added BMSGlib_StopSession to CloseSession
 *
 * Hydra_Software_Devel/14   3/10/08 10:08a erickson
 * PR40321: improve error message if maxContiguousMessageSize is too small
 *
 * Hydra_Software_Devel/13   2/26/08 10:29p erickson
 * PR39781: allow maxContiguousMessageSize == 0 for no data copy on wrap
 * around
 *
 * Hydra_Software_Devel/12   12/5/07 10:33a katrep
 * PR37217: Added support for 7335.
 *
 * Hydra_Software_Devel/11   11/1/07 1:31p erickson
 * PR36637: use latest transport calls
 *
 * Hydra_Software_Devel/10   11/1/07 9:35a erickson
 * PR36570: added 3563
 *
 * Hydra_Software_Devel/9   10/18/07 12:31p erickson
 * PR35906: rework BMSGlib_StartSession to set rc and dealloc PSI filter
 * in all failure cases
 *
 * Hydra_Software_Devel/8   10/16/07 10:29a jtna
 * PR35993: Coverity Defect ID:3995 OVERRUN_DYNAMIC
 *
 * Hydra_Software_Devel/7   10/15/07 4:11p jtna
 * PR35993: Coverity Defect ID:3995 OVERRUN_DYNAMIC
 *
 * Hydra_Software_Devel/6   9/13/07 1:42p gmullen
 * PR32868: Changed bmsglib to accept PID channel number, rather than PID
 * and band combo.
 *
 * Hydra_Software_Devel/5   8/7/07 5:22p katrep
 * PR32868: Fixed message parsing for playback parser.
 *
 * Hydra_Software_Devel/4   8/2/07 11:50p vsilyaev
 * PR 32868: Fixed warnings in release build by improving use of
 * BERR_TRACE
 *
 * Hydra_Software_Devel/3   7/30/07 4:24p erickson
 * PR32868: remove dependency on tspsi_validate.h
 *
 * Hydra_Software_Devel/2   7/26/07 11:11p erickson
 * PR32868: added more debug and verification
 *
 * Hydra_Software_Devel/1   7/26/07 12:35p erickson
 * PR32868: initial checkin of BMSGlib
 *
 ************************************************************************/
#include "bmsglib.h"
#include "bkni.h"
#include "bxpt_interrupt.h"

BDBG_MODULE(msglib);

#if (BCHP_CHIP==7038 && BCHP_VER>=BCHP_VER_C0)
#define MAX_GROUPS 100
#define MAX_SESSIONS_PER_GROUP 64
#else
#define MAX_GROUPS 50
#define MAX_SESSIONS_PER_GROUP 32
#endif
#define FILTER_SIZE 16

#if BCHP_CHIP != 7038
#define B_HAS_RAVE 1
#endif

static BERR_Code BMSGlib_Flush(BMSGlib_Session_Handle session);

BDBG_OBJECT_ID(BMSGlib);
BDBG_OBJECT_ID(BMSGlib_Session);

struct BMSGlib_Session_Impl {
    BDBG_OBJECT(BMSGlib_Session)
    BMSGlib_Handle msglib;
    BMSGlib_SessionSettings settings;
    BMSGlib_SessionParams params;

    bool started;
    unsigned bufferSize; /* actual buffer size in bytes */
    void *buffer; /* this it the buffer that will be used, whether internally allocated or not */

    struct {
        void *buffer; /* if not null, MSGlib allocated this and must free it. */
        unsigned bufferSize;
    } alloc;

    unsigned PidChannelNum;
    unsigned FilterNum;
    unsigned BankNum;

    struct BMSGlib_Group *group; /* this is needed to look up group on api calls */
    struct BMSGlib_Group *dependent_group; /* this session provided its nexus-allocated buffer for the group.
        it cannot be closed or switch pids until all dependent sessions are stopped. */

    int group_read_length; /* if > 0, this msgstream's filter has tested true
        for this msg, so we need to read_complete this total down to 0. This
        value is not guaranteed to be the msg length because of wrap around. */
};

/* although params->filterGroup is an optional, to keep the internal impl simple, all BMSGlib_Sessions use BMSGlib_Groups.  */
struct BMSGlib_Group {
    BXPT_PsiMessageSettings psi_settings;
    int refcnt;
    unsigned PidChannelNum;
    BMSGlib_Handle msglib;

    /* Each session is a separate Coeff/Mask/Exclusion set */
    BMSGlib_Session_Handle sessions[MAX_SESSIONS_PER_GROUP]; /* this is needed to look up each stream at isr time */
    BMSGlib_Session_Handle master_session;

    bool allow_group; /* if true, this BMSGlib_Group can support >1 session */
    bool just_started;

    int group_read_cnt; /* if > 0, there's a group read in process */

    const void *current_msg; /* pointer to current msg in buffer */
    size_t length; /* remaining length in buffer */
    unsigned current_msg_length; /* this is the length of the next message pointed
        to by group->current_msg, even if the message wraps. */

    unsigned char *contiguous_buffer; /* buffer used for one-message memcpy to guarantee contiguous PSI messages up to a max size */
    bool use_contiguous_buffer; /* if true, contiguous_buffer is in use and has already been completed with XPT */
};

struct BMSGlib_Impl {
    BDBG_OBJECT(BMSGlib)
    BMSGlib_Settings settings;
    struct BMSGlib_Group groups[MAX_GROUPS];
};


void BMSGlib_GetDefaultSettings(BMSGlib_Settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(*settings));
}

BERR_Code BMSGlib_Open(BMSGlib_Handle *handle, const BMSGlib_Settings *settings)
{
    if (!settings->mem || !settings->xpt) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    *handle = (BMSGlib_Handle)BKNI_Malloc(sizeof(struct BMSGlib_Impl));
    if (!*handle) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(*handle, 0, sizeof(struct BMSGlib_Impl));
    (*handle)->settings = *settings;
    BDBG_OBJECT_SET((*handle), BMSGlib);

    return BERR_SUCCESS;
}

void BMSGlib_Close(BMSGlib_Handle handle)
{
    BDBG_OBJECT_DESTROY(handle, BMSGlib);
    BKNI_Free(handle);
}

void BMSGlib_GetDefaultSessionSettings(BMSGlib_Handle handle, BMSGlib_SessionSettings *settings)
{
    BDBG_OBJECT_ASSERT(handle, BMSGlib);
    BKNI_Memset(settings, 0, sizeof(*settings));
    settings->bufferSize = 4*1024;
    settings->maxContiguousMessageSize = 4*1024;
}

BERR_Code BMSGlib_OpenSession(BMSGlib_Handle msglib, BMSGlib_Session_Handle *p_session, const BMSGlib_SessionSettings *settings)
{
    BMSGlib_Session_Handle session;

    BDBG_OBJECT_ASSERT(msglib, BMSGlib);

    /* callbacks are not required. the app could decide to poll. */
    /* bufferSize is not required. the app could pass a buffer in at Start time. check then. */

    session = *p_session = (BMSGlib_Session_Handle)BKNI_Malloc(sizeof(*session));
    if (!session) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(session, 0, sizeof(*session));
    BDBG_OBJECT_SET(session, BMSGlib_Session);
    session->msglib = msglib;
    session->settings = *settings;

    if (settings->bufferSize) {
        session->alloc.bufferSize = settings->bufferSize;
        session->alloc.buffer = BMEM_AllocAligned(session->msglib->settings.mem, session->alloc.bufferSize, 10, 0);
        BDBG_MSG(("Allocated %#x, %d bytes", session->alloc.buffer, session->alloc.bufferSize));
        if (!session->alloc.buffer) {
            BKNI_Free(session);
            *p_session = NULL;
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
    }

    return BERR_SUCCESS;
}

void BMSGlib_CloseSession(BMSGlib_Session_Handle session)
{
    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    BMSGlib_StopSession(session);

    if (session->dependent_group) {
        int i;
        struct BMSGlib_Group *group = session->dependent_group;
        /* We have to stop the others to avoid inevitable and unrecoverable memory corruption. */
        BDBG_WRN(("You are closing the group's master session (%p). All other sessions in the group must be stopped.", session));

        for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
            if (group->sessions[i]) {
                BMSGlib_StopSession(group->sessions[i]);
            }
        }
        BDBG_ASSERT(!session->dependent_group);
    }

    if (session->alloc.buffer) {
        BMEM_Free(session->msglib->settings.mem, session->alloc.buffer);
    }
    BDBG_OBJECT_DESTROY(session, BMSGlib_Session);
    BKNI_Free(session);
}

void BMSGlib_GetDefaultSessionParams(BMSGlib_Session_Handle session, BMSGlib_SessionParams *params)
{
    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    BKNI_Memset(params, 0, sizeof(*params));
    BKNI_Memset(params->filter.Mask, 0xFF, sizeof(params->filter.Mask));
    BKNI_Memset(params->filter.Exclusion, 0xFF, sizeof(params->filter.Exclusion));
/* for RAVE platforms, multiple pid channels on the same band and pid are not
allowed. an easy way to avoid this is to use filter groups. */
#if B_HAS_RAVE
    params->filterGroup = true;
#endif
    params->bank = -1; /* automatic selection */
}

static void BMSGlib_p_dataReady_isr(void *param1, int param2)
{
    int i;
    struct BMSGlib_Group *group = (struct BMSGlib_Group *)param1;
    BSTD_UNUSED(param2);
    /* disable until the next BXPT_GetBuffer call */
    BXPT_Interrupt_DisableMessageInt_isr(group->msglib->settings.xpt, group->PidChannelNum);

    /* everybody gets a callback. we can't know if the first message in
    the buffer is for a given session at isr time. */
    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        BMSGlib_Session_Handle session = group->sessions[i];
        if (session && session->settings.dataReadyCallback_isr) {
            (*session->settings.dataReadyCallback_isr)(session->settings.callbackContext);
        }
    }
}

static void BMSGlib_p_overflow_callback_isr(void *param1, int param2)
{
    int i;
    struct BMSGlib_Group *group = (struct BMSGlib_Group *)param1;
    BSTD_UNUSED(param2);
    /* disable until the next BXPT_GetBuffer call */
    BXPT_Interrupt_DisableMessageOverflowInt_isr(group->msglib->settings.xpt, group->PidChannelNum);

    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        BMSGlib_Session_Handle session = group->sessions[i];
        if (session && session->settings.overflowCallback_isr) {
            (*session->settings.overflowCallback_isr)(session->settings.callbackContext);
        }
    }
}

static BERR_Code BMSGlib_p_enable_interrupt(BMSGlib_Session_Handle session, bool enabled)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    if (session->settings.dataReadyCallback_isr) {
        if (enabled) {
            rc = BXPT_Interrupt_EnableMessageInt(session->msglib->settings.xpt, session->PidChannelNum,
                BMSGlib_p_dataReady_isr, session->group, 0);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            rc = BXPT_Interrupt_DisableMessageInt(session->msglib->settings.xpt, session->PidChannelNum);
            if (rc) return BERR_TRACE(rc);
        }
    }
    if (session->settings.overflowCallback_isr) {
        if (enabled) {
            rc = BXPT_Interrupt_EnableMessageOverflowInt(session->msglib->settings.xpt, session->PidChannelNum,
                BMSGlib_p_overflow_callback_isr, session->group, 0);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            rc = BXPT_Interrupt_DisableMessageOverflowInt(session->msglib->settings.xpt, session->PidChannelNum);
            if (rc) return BERR_TRACE(rc);
        }
    }
    return BERR_SUCCESS;
}

/**
Returns used or unused group. Caller should distinguish using refcnt.
If refcnt is 0, caller is responsible for populating psi_settings.
Caller is responsible for inc/dec of refcnt.
**/

static struct BMSGlib_Group *BMSGlib_p_find_group(BMSGlib_Handle msglib, bool allow_group, unsigned PidChannel )
{
    int i;
    struct BMSGlib_Group *unused_group = NULL;

    /* first, try to find one that's already used */
    for (i=0;i<MAX_GROUPS;i++) {
        struct BMSGlib_Group *group = &msglib->groups[i];
        if (group->refcnt && group->allow_group && allow_group) {
            /* this group is already in use. check if we can join their party. */
            if (group->PidChannelNum == PidChannel )
            {
                return group;
            }
        }
        else if (!group->refcnt && !unused_group) {
            /* find one unused group in case we don't find one we can join */
            unused_group = group;
        }
    }
    if (!unused_group) {
        BDBG_ERR(("No more message filter groups available"));
    }
    else {
        unused_group->allow_group = allow_group;
    }
    return unused_group;
}

static BERR_Code BMSGlib_p_add_stream(struct BMSGlib_Group *group, BMSGlib_Session_Handle session)
{
    int i;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    /* No critical section needed here. The danger is that we add a stream to a group
    but don't get a callback because of a race condition. But we always request
    a callback after adding subsequent group members at the end of BMSGlib_StartSession.
    See there for explanation. */
    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        if (!group->sessions[i]) {
            if (group->refcnt == 0) {
                BDBG_ASSERT(!group->contiguous_buffer);
                if (session->settings.maxContiguousMessageSize) {
                    group->contiguous_buffer = BMEM_AllocAligned(session->msglib->settings.mem, session->settings.maxContiguousMessageSize, 10, 0 );
                    BDBG_MSG(("Allocated maxContiguousMessage buffer %#x, %d bytes", group->contiguous_buffer, session->settings.maxContiguousMessageSize));
                    if (!group->contiguous_buffer) {
                        rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto done;
                    }
                }
                BDBG_ASSERT(!group->master_session);
                group->master_session = session;
            }
            /* hook it up */
            group->msglib = session->msglib;
            group->sessions[i] = session;
            group->refcnt++;
            session->group = group;
            BDBG_MSG(("add to group %p (refcnt %d)", group, group->refcnt));
            goto done;
        }
    }
    BDBG_ERR(("No more filter groups available"));
    rc = BERR_TRACE(BERR_UNKNOWN);
done:
    return rc;
}

static void BMSGlib_p_remove_stream(struct BMSGlib_Group *group, BMSGlib_Session_Handle session)
{
    int i;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        if (group->sessions[i] == session) {
            group->sessions[i] = NULL;
            group->refcnt--;

            if (group->refcnt == 0) {
                /* the grouping can be undone */
                if (group->contiguous_buffer) {
                    BMEM_Free(session->msglib->settings.mem, group->contiguous_buffer);
                    group->contiguous_buffer = NULL;
                }

                /* disconnect the master session. it can be safely closed now. */
                BDBG_ASSERT(group->master_session);
                group->master_session->dependent_group = NULL;
                group->master_session = NULL;
                group->current_msg = NULL;
            }
            else {
                /* throw the mastership to another session in the group if the session doesn't own the buffer
                   and the new master doesn't own the buffer. if any session in the group owns the buffer,
                   the whole system is too inflexible. */
                if (group->master_session == session && !session->alloc.buffer) {
                    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
                        if (group->sessions[i] && !group->sessions[i]->alloc.buffer) {
                            BDBG_MSG(("switching group master from %p to %p", group->master_session, group->sessions[i]));
                            group->master_session = group->sessions[i];
                            break;
                        }
                    }
                }
            }

            session->group = NULL;
            BDBG_MSG(("remove from group %p (refcnt %d)", group, group->refcnt));
            return;
        }
    }
    BDBG_ASSERT(0);
}

static bool BMSGlib_P_TS_Filter_Compare(const uint8_t *msg, const uint8_t *inclMask, const uint8_t *exclMask,
    const uint8_t *coef, size_t filterSize )
{
    bool inclResult = true;
    bool exclResult = true;
    bool exclEnabled = false;
    size_t i;
    unsigned message_index;

    for( i = 0, message_index = 0; message_index < filterSize; i++, message_index++ )
    {
        /* BXPT_Filter skips the 2nd byte */
        if (message_index == 2) {
            message_index++;
        }
        if( (inclMask[i] | coef[i]) != (inclMask[i] | msg[message_index]) )
        {
            inclResult = false;
            break;
        }

        if( exclEnabled == false && exclMask[i] != 0xFF )
        {
            exclEnabled = true;
            exclResult = false;
        }

        if( (~exclMask[i] & coef[i]) != (~exclMask[i] & msg[message_index]) )
        {
            exclResult = true;
        }
    }

#if 0
/* for debug to make sure the logic is right */
    if( !(inclResult && exclResult) )
    {
        BKNI_Printf("Incl: ");
        for( i = 0; i < filterSize; i++ )
        {
            BKNI_Printf("%02x ", inclMask[i]);
        }
        BKNI_Printf("\nExcl: ");
        for( i = 0; i < filterSize; i++ )
        {
            BKNI_Printf("%02x ", exclMask[i]);
        }
        BKNI_Printf("\nCoef: ");
        for( i = 0; i < filterSize; i++ )
        {
            BKNI_Printf("%02x ", coef[i]);
        }
        BKNI_Printf("\n Msg: ");
        for( i = 0; i < filterSize; i++ )
        {
            BKNI_Printf("%02x ", msg[i]);
        }
        BKNI_Printf("\n");
    }
#endif

    return inclResult && exclResult;
}

#define TS_READ_16( buf ) ((uint16_t)((buf)[0]<<8|(buf)[1]))
#define TS_PSI_SECTION_LENGTH_OFFSET                1
#define TS_PSI_GET_SECTION_LENGTH( buf )    (uint16_t)(TS_READ_16( &(buf)[TS_PSI_SECTION_LENGTH_OFFSET] ) & 0x0FFF)

static int TS_Get_Message_Size(const uint8_t *msg)
{
    size_t message_size = TS_PSI_GET_SECTION_LENGTH(msg) + 3;
    /*
    The transport hardware automatically pads messages to be 32 bit aligned.
    Therefore, the message size must be increased to take this into account.
    */
    if( message_size%4 ) {
        message_size += 4-(message_size%4);
    }
    return message_size;
}

static bool TS_Validate_Size( const uint8_t *msg, size_t size)
{
    size_t current_size = 0;
    while (current_size + TS_PSI_SECTION_LENGTH_OFFSET + 1 < size) {
        current_size += TS_Get_Message_Size(&msg[current_size]);
    }
    /* In the end, it must be exactly equal */
    return current_size == size;
}

/**
NOTE: There's another way we could have done this. There's an XPT option to append
every message with a filter bitmask. This is a simpler and more explicit way of checking
a filter match. However, the above could would have to read the whole message
in order to perform the test. Plus, we'd have to pass these filter bitmask's out
to the user (breaking existing apps), or have every BMSGlib_GetBuffer skip over
them (which won't work well at high bitrates), or have a flag to indicate that
a BMSGlib_stream will be a group before the first BMSGlib_start (which changes the
public API and probably leads to user errors). So, it seems the best option is
to use this soft filter test, which should work.
**/

/* Examine the next message in the buffer for group processing */
static BERR_Code BMSGlib_process_next_message(BMSGlib_Session_Handle session)
{
    struct BMSGlib_Group *group = session->group;
    size_t remainder = 0;
    int i,filterSize;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);

    /* we should have data read to be processed */
    BDBG_ASSERT(group->current_msg && group->length);
    /* we should not be in the middle of a group read */
    BDBG_ASSERT(group->group_read_cnt == 0);

    /* We know group->length is large enough to at least read the size
    because of XPT's 4 byte alignment */
    BDBG_ASSERT(group->length >= 4);
    group->current_msg_length = TS_Get_Message_Size(group->current_msg);

    /* determine if the message wraps */
    if (group->length < group->current_msg_length) {
        bool MoreDataAvailable;
        BERR_Code rc;
        uint8_t *read_ahead_pointer;
        size_t read_ahead_length;

        BDBG_MSG(("group message wraps around end of buffer, %d bytes, msglen=%d", group->length, group->current_msg_length));

        if (group->current_msg_length > session->settings.maxContiguousMessageSize) {
            /* options for fixing this: increase maxContiguousMessageSize to accomodate max, or set maxContiguousMessageSize to 0
            to bypass this copy and require app to piece together messages. */
            BDBG_ERR(("%d byte message received which is larger than %d maxContiguousMessageSize. It must be discarded.",
                group->current_msg_length, session->settings.maxContiguousMessageSize));
            return -1;
        }
        if (!group->contiguous_buffer) {
            /* this should only be NULL if maxContiguousMessageSize is zero */
            return -1;
        }

        remainder = group->current_msg_length - group->length;

        /* Copy the first part of the message into the buffer */
        BKNI_Memcpy(group->contiguous_buffer, group->current_msg, group->length);

        /* This is the trick part, we have to complete the transaction now in order to get more from XPT. */
        rc = BXPT_UpdateReadOffset( session->msglib->settings.xpt, session->PidChannelNum, group->length);
        if (rc) {
            /* flush for error recovery, but still pass on the error code */
            BMSGlib_Flush(session);
            return BERR_TRACE(rc);
        }

        /* The early BXPT_CheckBuffer is no problem because it's non-destructive. */
        rc = BXPT_CheckBuffer( session->msglib->settings.xpt, session->PidChannelNum,
            (uint8_t **)&read_ahead_pointer, &read_ahead_length, &MoreDataAvailable);
        if (rc) return BERR_TRACE(rc);

        /* XPT should only return whole messages, so we should not come up short */
        if (read_ahead_length < remainder){
            /* PR 53390 - could be HW buffer corruption. absorb the error. */
            BDBG_ERR(("read bad message length (got %d, expecting %d) at wrap around", read_ahead_length, remainder));
            BMSGlib_Flush(session);
            return 0;
        }

        BDBG_ASSERT(read_ahead_pointer);

        /* Copy the second part of the message to the end of the buffer. Don't copy more than the message. */
        BKNI_Memcpy(&group->contiguous_buffer[group->length], read_ahead_pointer, remainder);

        /* Complete this part of the read too */
        rc = BXPT_UpdateReadOffset( session->msglib->settings.xpt, session->PidChannelNum, remainder);
        if (rc) {
            /* flush for error recovery, but still pass on the error code */
            BMSGlib_Flush(session);
            return BERR_TRACE(rc);
        }

        group->use_contiguous_buffer = true;
        group->current_msg = group->contiguous_buffer;
    }

    if( group->current_msg_length < FILTER_SIZE ) filterSize = group->current_msg_length;
    else filterSize = FILTER_SIZE;

    /* compare message with each session's filter. */
    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        BMSGlib_Session_Handle session = group->sessions[i];
        if (session) {
            if (BMSGlib_P_TS_Filter_Compare((const uint8_t *)group->current_msg,
                session->params.filter.Mask, session->params.filter.Exclusion,
                session->params.filter.Coeficient, filterSize))
            {
                session->group_read_length = group->current_msg_length;
                group->group_read_cnt++;
                BDBG_MSG(("found message for %p, group_read_length=%d, first_byte=%02x",
                    session, session->group_read_length, *(const uint8_t *)group->current_msg));
            }
            else {
                session->group_read_length = 0;
            }
        }
    }

    return BERR_SUCCESS;
}

static void BMSGlib_p_advance_group_buffer(struct BMSGlib_Group *group)
{
    /* If buffer is NULL, then we're waiting on XPT
    If group_read_cnt != 0, then we're still waiting on read_complete's
    Otherwise, we can advance and send callbacks */
    if (!group->current_msg || group->group_read_cnt)
        return;

    /* This assumes group complete when the whole message is processed */
    group->length -= group->current_msg_length;

    if (!group->length)
        group->current_msg = NULL;
    else
        group->current_msg = (const char *)group->current_msg + group->current_msg_length;

    /* Fire the callbacks. If we have more messages to process
    in the buffer, this keeps the sessions coming back for more.
    However, even if we're done with the buffer, we may have bypassed the
    BXPT_GetBuffer and so we have to do this. This means the group reads
    will always end with callbacks that result in no reads. That's the way it is. */
    BKNI_EnterCriticalSection();
    BMSGlib_p_dataReady_isr(group, 0);
    BKNI_LeaveCriticalSection();

    /* TODO: performance optimization:
    if (group->current_msg), we could call BMSGlib_GetBuffer internally,
    and then only callback those who will receive the next message. The current
    impl won't hurt. */
}

BERR_Code BMSGlib_StartSession(BMSGlib_Session_Handle session, const BMSGlib_SessionParams *params)
{
    int i;
    struct BMSGlib_Group *group;
    BERR_Code rc = BERR_SUCCESS;
    BXPT_Handle xpt = session->msglib->settings.xpt;
    bool first_session;
    bool allocFilter = false;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    if (session->started) {
        BMSGlib_StopSession(session);
    }

    if (session->dependent_group) {
        BDBG_ERR(("This session is the master of filter group which is current active. You must stop all other members of the group before restarting this session."));
        return BERR_TRACE(-1);
    }

    session->params = *params;

    if (params->buffer) {
        session->buffer = params->buffer;
        session->bufferSize = params->bufferSize;
    }
    else {
        session->buffer = session->alloc.buffer;
        session->bufferSize = session->alloc.bufferSize;
    }
    /* verify we have a buffer that's usable */
    if (!session->buffer || session->bufferSize<1024 || ((unsigned)session->buffer % 1024)) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_MSG(( "BMSGlib_StartSession %p, PID channel %u", session, params->PidChannel ));

    group = BMSGlib_p_find_group( session->msglib, params->filterGroup, params->PidChannel );
    if (!group) return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

    first_session = (group->refcnt == 0);

    if (first_session) {
        BXPT_MessageBufferSize bufferSizeEnum;
        unsigned bufferSize, temp;

        /* calculate actual bufferSize as a log of 2, up to a max of 512KB */
        bufferSizeEnum = BXPT_MessageBufferSize_e1kB;
        bufferSize = 1024;
        temp = session->bufferSize / 1024;
        while (temp > 1 && bufferSize < 512*1024) {
            temp /= 2; /* throw away remainder */
            bufferSizeEnum++;
            bufferSize *= 2;
        }
        BDBG_ASSERT(bufferSizeEnum <= BXPT_MessageBufferSize_e512kB);
        if (bufferSize < session->bufferSize) {
            BDBG_WRN(("only %d out of %d bytes of BMSGlib buffer will be used.", bufferSize, session->bufferSize));
        }

        BKNI_Memset(group->sessions, 0, sizeof(group->sessions));

        /* If Nexus allocated the buffer, then Nexus must ensure that all uses of the buffer are correct. */
        if (session->alloc.buffer) {
            session->dependent_group = group;
        }

        session->PidChannelNum = params->PidChannel;
        rc = BXPT_SetPidChannelBuffer( xpt, session->PidChannelNum, session->buffer, bufferSizeEnum);
        if (rc) { rc=BERR_TRACE(rc); goto error;}

        group->PidChannelNum = session->PidChannelNum;
    }
    else {
        /* TODO: we don't use the buffers on secondary filters. we can't avoid the alloc (done at Open time), but
        maybe we can WRN the user that the memory is not used so they can alloc less. */

        if( params->format == BMSGlib_Format_ePSI ) {
            BDBG_ASSERT(session->params.filterGroup);
            session->PidChannelNum = group->PidChannelNum;

            if (!session->settings.dataReadyCallback_isr) {
                BDBG_ERR(("dataReadyCallback_isr is required for filtergroups"));
                rc = BERR_TRACE(BERR_INVALID_PARAMETER);
                goto error;
            }
        }
        else {
            BDBG_ERR(("Don't allow message groups for non-PSI filtering"));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto error;
        }

        if (params->buffer && params->buffer != group->master_session->buffer) {
            BDBG_ERR(("All message filters in a group must use the same buffer."));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto error;
        }
    }

    /* We need to add to group before enabling it to prevent race
    conditions. We've already capture refcnt==0 before this. */
    rc = BMSGlib_p_add_stream(group, session);
    if (rc) {rc=BERR_TRACE(rc);goto error;}
    BDBG_MSG(("add session to group %p, refcnt %d", group, group->refcnt));

    if (session->params.format == BMSGlib_Format_ePSI) {
        if (session->params.bank != -1) {
            /* explicit filter selection */
            rc = BXPT_AllocPSIFilter( xpt, session->params.bank, &session->FilterNum );
            if (rc == BERR_SUCCESS){
                session->BankNum = session->params.bank;
                allocFilter = true;
            }
        }
        else if (!first_session) {
            /* HW requires  all filters in a group to be on the same bank. */
            BDBG_ASSERT(group->refcnt && group->master_session);
            rc = BXPT_AllocPSIFilter( xpt, group->master_session->BankNum, &session->FilterNum );
            if (rc == BERR_SUCCESS){
                session->BankNum = group->master_session->BankNum;
                allocFilter = true;
            }
        }
        else {
            /* For easy application code, we search for an available bank. */
            for(i=0; i<BXPT_P_MAX_FILTER_BANKS; i++){
                rc = BXPT_AllocPSIFilter( xpt, i, &session->FilterNum );
                if (rc == BERR_SUCCESS){
                    session->BankNum = i;
                    allocFilter = true;
                    break;
                }
            }
        }
        if (rc) {rc=BERR_TRACE(rc);goto error;}

        rc = BXPT_SetFilter( xpt, session->BankNum, session->FilterNum, &session->params.filter);
        if (rc) {rc=BERR_TRACE(rc);goto error;}

        if (session->params.filterGroup) {
#if BCHP_CHIP == 7038
            rc = BXPT_AddFilterToGroup(xpt, session->FilterNum, &group->psi_settings);
#else
            rc = BXPT_AddFilterToGroup(xpt, group->PidChannelNum, session->FilterNum, &group->psi_settings);
#endif
            if (rc) {rc=BERR_TRACE(rc);goto error;}
        }
    }

    if (first_session) {
        unsigned Pid, ParserBand;
        bool IsParserAPlayback;

        group->psi_settings.Bank = session->BankNum;
        group->psi_settings.FilterEnableMask = 1UL<<session->FilterNum;
        group->psi_settings.CrcDisable = params->psiCrcDisabled;
        group->just_started = true;

        rc = BMSGlib_p_enable_interrupt(session, true);
        if (rc) {rc=BERR_TRACE(rc);goto error;}

        rc = BXPT_GetPidChannelConfig( xpt, session->PidChannelNum, &Pid, &ParserBand, &IsParserAPlayback );
        if (rc) {rc=BERR_TRACE(rc);goto error;}

        switch(session->params.format) {
        default:
        case BMSGlib_Format_ePES:
        case BMSGlib_Format_eTS:
        {
            BXPT_PidChannelRecordSettings ChanSettings;

            BKNI_Memset(&ChanSettings, 0, sizeof(ChanSettings)); /* no XPT init func */
            ChanSettings.Pid  = Pid;
            ChanSettings.Band = IsParserAPlayback ? BXPT_PB_PARSER( ParserBand ) : ParserBand;
            ChanSettings.SaveAllCountType = 0;
            ChanSettings.SaveAllCount = 10;
            ChanSettings.ByteAlign = false;
            switch (session->params.format) {
            case BMSGlib_Format_eTS:
                ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePacketSaveAll;
                break;
            case BMSGlib_Format_ePES:
                ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePes;
                break;
            default:
                ChanSettings.RecordType = BXPT_SingleChannelRecordType_ePesSaveAll;
                break;
            }

            rc = BXPT_StartPidChannelRecord( xpt, session->PidChannelNum, &ChanSettings );
            if (rc) {rc=BERR_TRACE(rc); goto error; }
            break;
        }
        case BMSGlib_Format_ePSI:
            {
#if B_HAS_RAVE
                bool override=false;
                BXPT_PidPsiConfig psiConfig;

                rc = BXPT_GetPidChannelPsiSettings( xpt, session->PidChannelNum, &override, &psiConfig );
                if (rc) {rc=BERR_TRACE(rc);goto error;}

                psiConfig.PsfCrcDis = params->psfCrcDisabled;
                override = true;

                rc = BXPT_SetPidChannelPsiSettings( xpt, session->PidChannelNum, override, &psiConfig );
                if (rc) {rc=BERR_TRACE(rc);goto error;}
#else
                if( params->psfCrcDisabled ) {
                    BDBG_WRN(("Setting psfCrcDisabled is not supported yet" ));
                }
#endif
                group->psi_settings.Pid = Pid;
                group->psi_settings.Band = IsParserAPlayback ? BXPT_PB_PARSER( ParserBand ) : ParserBand;
                rc = BXPT_StartPsiMessageCapture( xpt, session->PidChannelNum, &group->psi_settings );
                if (rc) {rc=BERR_TRACE(rc);goto error;}
            }
            break;
        }
    }

    session->started = true;

    /* If this session is part of a group, and if this group already has
    data pending, we need to make sure this session gets a callback. Otherwise
    we'll be waiting for this session's read_complete, and it may be waiting
    for a callback. */
    if (session->group->current_msg && session->settings.dataReadyCallback_isr) {
        BKNI_EnterCriticalSection();
        (*session->settings.dataReadyCallback_isr)(session->settings.callbackContext);
        BKNI_LeaveCriticalSection();
    }

    return BERR_SUCCESS;

error:
    if (allocFilter)  {
        BXPT_FreePSIFilter( xpt, session->BankNum, session->FilterNum );
    }
    BDBG_ASSERT(rc); /* if the code gets here, it must be a failure */
    return rc;
}

void BMSGlib_StopSession(BMSGlib_Session_Handle session)
{
    struct BMSGlib_Group *group = session->group;
    BXPT_Handle xpt = session->msglib->settings.xpt;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    if (session->started == false) {
        return;
    }
    BDBG_ASSERT(group);

    if (session->group_read_length) {
        group->group_read_cnt--;
        session->group_read_length = 0;
        /* if this was the last one we're waiting on, we need to advance */
        BMSGlib_p_advance_group_buffer(group);
    }

    BMSGlib_p_remove_stream(group, session);

    if (group->refcnt == 0) {
        BMSGlib_p_enable_interrupt(session, false);
        if (session->params.format == BMSGlib_Format_ePSI) {
            BXPT_StopPsiMessageCapture(xpt, session->PidChannelNum );
        }
        else {
            BXPT_StopPidChannelRecord(xpt, session->PidChannelNum );
        }
    }

    if (session->params.format == BMSGlib_Format_ePSI) {
        if (session->params.filterGroup) {
            BXPT_RemoveFilterFromGroup(xpt, session->FilterNum, &group->psi_settings);
        }

        BXPT_FreePSIFilter(xpt, session->BankNum, session->FilterNum);
    }

    session->started = false;
}

/* flush everything that's in the buffer, regardless of group. everything starts over. */
static BERR_Code BMSGlib_Flush(BMSGlib_Session_Handle session)
{
    bool MoreDataAvailable;
    unsigned length;
    unsigned i;

    BDBG_MSG(("BMSGlib_Flush %p", session));
    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);

    do {
        uint8_t *buffer;
        BERR_Code rc;

        rc = BXPT_CheckBuffer( session->msglib->settings.xpt, session->PidChannelNum, &buffer, &length, &MoreDataAvailable);
        if (rc) return BERR_TRACE(rc);

        /* unconditionally consume everything */
        if (length) {
            rc = BXPT_UpdateReadOffset( session->msglib->settings.xpt, session->PidChannelNum, length);
            if (rc) return BERR_TRACE(rc);
        }
    } while (MoreDataAvailable || length);

    session->group->group_read_cnt = 0;
    session->group->current_msg = NULL;
    session->group->length = 0;
    session->group->current_msg_length = 0;

    for (i=0;i<MAX_SESSIONS_PER_GROUP;i++) {
        if (session->group->sessions[i]) {
            session->group->sessions[i]->group_read_length = 0;
        }
    }

    return 0;
}

BERR_Code BMSGlib_GetBuffer(BMSGlib_Session_Handle session, const void **buffer, size_t *length)
{
    bool MoreDataAvailable = false;
    int retryCount = 0;
    struct BMSGlib_Group *group = session->group;
    void *buf_cached;
    BERR_Code rc;
    BMEM_Heap_Handle mem;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    BDBG_ASSERT(buffer);
    BDBG_ASSERT(length);
    mem = session->msglib->settings.mem;

    if (session->started == false) {
        return BERR_UNKNOWN;
    }
    BDBG_ASSERT(group);

    if(session->params.format != BMSGlib_Format_ePSI) {
        /* shortcut for PES/TS record */
        rc = BXPT_CheckBuffer( session->msglib->settings.xpt, session->PidChannelNum, (uint8_t **)buffer, length, &MoreDataAvailable);
        if (rc) {return BERR_TRACE(rc);}

        if (*buffer) {
            rc = BMEM_Heap_ConvertAddressToCached(mem, (void*)*buffer, &buf_cached);
            if (rc) {return BERR_TRACE(rc);}
            *buffer = buf_cached;
            if (*length) {
                BMEM_Heap_FlushCache(mem, buf_cached, *length);
            }
        }
        /* reenable the callback */
        BMSGlib_p_enable_interrupt(session, true);

        return BERR_SUCCESS;
    }

try_next_message:
    /* If we're in a group read, don't expand the buffer. We'll have to finish
    the read */
    if (!group->current_msg) {
        rc = BXPT_CheckBuffer( session->msglib->settings.xpt, session->PidChannelNum, (uint8_t **)buffer,
            length, &MoreDataAvailable );
        if (rc) {return BERR_TRACE(rc);}

        if (*buffer) {
            rc = BMEM_Heap_ConvertAddressToCached(mem, (void*)*buffer, &buf_cached);
            if (rc) {return BERR_TRACE(rc);}
            *buffer = buf_cached;
            if (*length) {
                BMEM_Heap_FlushCache(mem, buf_cached, *length);
            }
        }

        /* Because of potentially bad filtering, we have to validate the first message */
        if (*buffer && *length) {
            /* If we're the only entry in this group, then apply the soft filter to the first one.
            Otherwise, we'll apply a soft filter every time, so there's no need. */
            if (group->just_started && group->refcnt == 1) {
                if (!BMSGlib_P_TS_Filter_Compare(*buffer, session->params.filter.Mask,
                        session->params.filter.Exclusion, session->params.filter.Coeficient,
                        sizeof(session->params.filter.Mask))
                    ||
                    !TS_Validate_Size(*buffer, *length))
                {
                    /* clear the buffer */
                    BDBG_WRN(("Bad message discarded len=%d", *length));
                    BMSGlib_ReadComplete(session, *length);
                    *length = 0;
                }
            }
            group->just_started = false;
        }
        /* reenable the callback */
        BMSGlib_p_enable_interrupt(session, true);
        if (!(*buffer && *length)) {
            goto done;
        }
    }

    /* if maxContiguousMessageSize is set, then we must call BMSGlib_process_next_message in order to send
    whole messages on wrap around
    */
    if (session->settings.maxContiguousMessageSize) {
        /* this is the first pass in the group read for the entire buffer */
        if (!group->current_msg) {
            BDBG_MSG(("first pass for group buffer, len %d, pos 0x%x",
                *length, *buffer));
            group->current_msg = *buffer;
            group->length = *length;
            BDBG_ASSERT(group->group_read_cnt == 0);
        }

        /* first pass for this message, so figure out all
        the session's that must read the message before we can advance */
        if (!group->group_read_cnt) {
            BERR_Code rc;

            rc = BMSGlib_process_next_message(session);
            if (rc) return rc;

            /* Because of an internal flush (for error recovery), it's possible that there is no data after a successful process_next_message */
            if (!group->current_msg_length) {
                *buffer = NULL;
                *length = 0;
                goto done;
            }

            BDBG_MSG(("new group message group_read_cnt=%d, msglen=%d", group->group_read_cnt,
                group->current_msg_length));
            BDBG_ASSERT(group->refcnt >= group->group_read_cnt);

            if (!group->group_read_cnt) {
                /* no one wants this message. this happens when we close a session
                which originally filtered the data but stopped before reading it all out. */
                BDBG_MSG(("unused message %d at %p", group->current_msg_length, group->current_msg));

                rc = BMSGlib_ReadComplete(session, group->current_msg_length);
                if ( rc ) {
                  BDBG_ERR(("GetBuffer/ReadComplete got an error %d",rc));
                  return rc;
                }

                /* The following code was found to be needed in one case. It is highly unlikely that an infinite
                loop will happen here, especially with the BMSGlib_Flush error recovery. But we're left it in. */
                retryCount++;
                if ( retryCount > 50 ) {
                    BDBG_ERR(("Aborting apparent infinite loop after %d retries", retryCount));
                    BMSGlib_Flush(session);
                    *buffer = NULL;
                    *length = 0;
                    return 0;
                }

                /* at this point, we should try the next message. we will
                exit when we find something, or run out of data. */
                goto try_next_message;
            }
        }

        /* check if this session is a part of this current group read */
        if (session->group_read_length) {
            /* remember that this can be called multiple times, so don't
            unmark here */
            *buffer = group->current_msg;
            *length = session->group_read_length;
        }
        else {
            /* there may be data for this session later in the buffer,
            but not this message. the key is that every session will be interrupted
            again when this message is done */
            *buffer = NULL;
            *length = 0;
        }
    }

done:
    BDBG_MSG(("GetBuffer %p (group %p) buffer=%p length=%d", session, session->group, *buffer, *length));
    return BERR_SUCCESS;
}

BERR_Code BMSGlib_ReadComplete(BMSGlib_Session_Handle session, unsigned amount_consumed)
{
    struct BMSGlib_Group *group = session->group;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    if (session->started == false) {
        return BERR_UNKNOWN;
    }
    BDBG_ASSERT(group);

    BDBG_MSG(("ReadComplete %p (group %p): %d out of %d", session, session->group, amount_consumed, session->group_read_length));

    if (session->params.format!=BMSGlib_Format_ePSI) {
        /* shortcut for PES/TS record */
        rc = BXPT_UpdateReadOffset( session->msglib->settings.xpt, session->PidChannelNum, amount_consumed);
        if (rc) return BERR_TRACE(rc);
        return BERR_SUCCESS;
    }

    /* In more recent silicon, this 4 byte pad is programmable. But for now the XPT PI defaults it on. */
    if (amount_consumed % 4) {
        BDBG_WRN(("XPT HW pads messages to 4 byte alignment. Increasing BMSGlib_ReadComplete(%d) to consume that pad.", amount_consumed));
        amount_consumed += 4 - (amount_consumed % 4);
    }

    if (group->group_read_cnt) {
        /* make sure we're not completing more than we expected */
        if (amount_consumed > (size_t)session->group_read_length) {
            BDBG_ERR(("app trying to consume %d, but only %d is available. reducing amount.", amount_consumed, session->group_read_length));
            amount_consumed = (size_t)session->group_read_length;
        }
        if (!session->group_read_length) {
            /* this session was not part of the group read, so it's not part of the group_read_cnt. */
            return BERR_SUCCESS;
        }
        session->group_read_length -= amount_consumed;
        if (session->group_read_length) {
            /* this session is not done */
            return BERR_SUCCESS;
        }
        if (--group->group_read_cnt > 0) {
            /* this session is done, but there are more reads of the current
            message pending before we're done */
            return BERR_SUCCESS;
        }
    }

    /* we're done with the message. */

    if (group->use_contiguous_buffer) {
        /* in this case, we've already updated XPT, so just clear the state and reread on next get_buffer. */
        group->use_contiguous_buffer = false;
        group->current_msg_length = 0;
        group->length = 0;
    }
    else {
        rc = BXPT_UpdateReadOffset( session->msglib->settings.xpt, session->PidChannelNum, amount_consumed);
        if (rc) {
            /* flush for error recovery, but still pass on the error code */
            BMSGlib_Flush(session);
            return BERR_TRACE(rc);
        }
    }
    BMSGlib_p_advance_group_buffer(group);

    return BERR_SUCCESS;
}

BERR_Code BMSGlib_GetStatus( BMSGlib_Session_Handle session, BMSGlib_Status *pStatus )
{
    BDBG_OBJECT_ASSERT(session, BMSGlib_Session);
    if (session->group) {
        pStatus->groupMembers = session->group->refcnt;
        pStatus->isGroupMaster = (session == session->group->master_session);
    }
    else {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    }
    return 0;
}
