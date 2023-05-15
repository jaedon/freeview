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
 * $brcm_Workfile: nexus_message.h $
 * $brcm_Revision: 39 $
 * $brcm_Date: 12/20/11 4:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/include/nexus_message.h $
 * 
 * 39   12/20/11 4:36p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * SW7425-1868/1   12/20/11 4:32p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * 38   6/2/11 4:22p erickson
 * SWDEPRECATED-2425: add comment about filtering on integer ranges
 * 
 * 37   12/16/10 10:33a erickson
 * SW7422-151: add NEXUS_MessageStartSettings.useRPipe
 * 
 * 36   12/14/10 4:37p erickson
 * SW7420-1285: add null_allowed attribute
 * 
 * 35   10/15/10 1:38p erickson
 * SWDEPRECATED-2425: clarify TS and PES capture
 * 
 * 34   9/8/10 4:42p erickson
 * SW3548-3073: NEXUS_Message_GetBuffer should not re-enable the interrupt
 *  if returned size is non-zero. This is correct backward compatibility
 *  with MSGlib.
 *
 * 33   9/3/10 11:56a erickson
 * SW35230-1172: add NEXUS_Message_GetBufferWithWrap
 *
 * 32   9/1/10 3:42p erickson
 * SW7420-996: add NEXUS_MessageStatus.moreDataAvailable
 *
 * 31   7/14/10 1:43p erickson
 * SW7405-4561: clarify NEXUS_MessageFilter comments
 *
 * 30   7/12/10 4:49p erickson
 * SW7405-4561: add support for 32 byte filters using export
 *  BXPT_FILTER_32=y
 *
 * 29   5/12/10 5:35p erickson
 * SW7420-624: update comments on NEXUS_MessageStartSettings.filterGroup
 *
 * 28   4/19/10 2:52p erickson
 * SW7420-624: add NEXUS_Message_UpdateFilter
 *
 * 27   4/19/10 1:59p erickson
 * SW7420-624: add NEXUS_Message_AddFilter and NEXUS_Message_RemoveFilter
 *  to enable filter groups with pid2buf
 *
 * 26   2/11/10 2:20p erickson
 * SWDEPRECATED-2425: update comments
 *
 * 25   12/11/09 3:32p erickson
 * SW7550-112: merge 7550 code. add support for sw message filtering.
 *  allow for no HW message filtering using standard XPT define.
 *
 * 24   11/6/09 1:57p erickson
 * SW7400-2559: added NEXUS_Message_SetDssCapPattern
 *
 * 23   10/20/09 2:24p erickson
 * SW7400-2559: add DSS message support
 *
 * 22   10/8/09 1:23p erickson
 * SWDEPRECATED-3717: revise comments based on switch to pid2buf
 *
 * 21   7/8/09 2:56p erickson
 * PR53768: deprecated NEXUS_MessageBufferMode. it was never implemented
 *  and has no use.
 *
 * 20   6/12/09 12:38p erickson
 * PR35457: update comments on NEXUS_MessageSettings.dataReady callback
 *
 * 19   3/24/09 10:01a erickson
 * PR53516: added NEXUS_Message_GetStatus
 *
 * 18   11/26/08 3:06p erickson
 * PR35457: update comments
 *
 * 17   11/11/08 2:54a erickson
 * PR 48847: added NEXUS_Message_SetSettings
 *
 * 16   9/11/08 3:13p erickson
 * PR46646: added crcError, pesLengthError, pesStartCodeError
 *
 * 14   8/6/08 3:01p erickson
 * PR45472: update comments
 *
 * 13   5/9/08 1:29p erickson
 * PR42456: added NEXUS_MessageFormat_ePesSaveAll. clarified that
 *  maxContiguousMessageSize only applies for PSI messages.
 *
 * 12   5/6/08 4:43p erickson
 * PR34925: clarify comment
 *
 * 11   4/14/08 1:17p erickson
 * PR41730: move user buffer override from Open time to Start time to give
 *  maximum flexibility
 *
 * 10   3/24/08 10:19a erickson
 * PR40813: added bank param
 *
 * 9   3/11/08 4:26p erickson
 * PR39836: clarify comment
 *
 * 8   3/10/08 1:28p erickson
 * PR39836: enforce memory allocation rules for group masters. If the
 *  group master Closes, all other sessions in the group must be Stopped.
 *  Don't allow the group master to restart if there are other sessions
 *  still active in the group.
 *
 * 7   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 6   2/26/08 10:30p erickson
 * PR39781: allow maxContiguousMessageSize == 0 for no data copy on wrap
 *  around
 *
 * 5   2/26/08 9:53p erickson
 * PR39836: update comments
 *
 * 4   2/4/08 10:07a erickson
 * PR34925: remove unsupported enums
 *
 * 3   2/1/08 5:05p vsilyaev
 * PR 38682: Added attributes to mark destructor and shutdown functions
 *
 * 2   1/30/08 6:16p vsilyaev
 * PR 38682: Added attributes for memory translation
 *
 * 1   1/18/08 2:15p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/12   1/8/08 9:27a erickson
 * PR34925: update comments
 *
 * Nexus_Devel/11   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 *
 **************************************************************************/
#ifndef NEXUS_MESSAGE_H__
#define NEXUS_MESSAGE_H__

#include "nexus_types.h"
#include "nexus_pid_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Summary:
Handle which represents one message filter on a PID.

Description:
The transport message filter HW is able to filter for PSI messages as well as capture (not filter) PES and TS data.
NEXUS_Message cannot use a filter when capturing PES and TS packets. It captures all data on a pid. You can also use NEXUS_Recpump to do the same type of PES and TS capture.

See nexus/examples/transport/message.c for an example application.
*/
typedef struct NEXUS_Message *NEXUS_MessageHandle;

/*
Summary:
Settings needed for opening a message filter.

Description:
Note that some settings cannot be changed after NEXUS_Message_Open.
*/
typedef struct NEXUS_MessageSettings
{
    /* The following settings can only be set at NEXUS_Message_Open */
    unsigned bufferSize; /* Size in bytes. Only certain buffer sizes may be supported by HW. This size will be rounded down to the nearest supported value.
                            If you want to control the allocation and use of memory, set this to zero; then set NEXUS_MessageStartSettings.buffer. */
    unsigned maxContiguousMessageSize; /* The maximum size PSI message that is guaranteed to be returned as a whole message from NEXUS_Message_GetBuffer.
                                          For high bitrate message capture, set this to zero. This will ensure no data copy and GetBuffer will not
                                          guarantee to return whole messages.
                                          This does not apply to PES/TS capture. */
   unsigned recpumpIndex; /* Only required for RAVE-based software message filtering. Enabled with NEXUS_USE_SW_FILTER=y compilation option.
       Indicates which recpump to use for the RAVE record. Recpump is opened internally. Application needs to make sure not to re-use this recpump for anything else.
       Filters on the same stream, of the same format, can share a recpump. But filters of different formats need to use different recpumps.
       ie use a different recpump for each format ie PSI, TS & PES.  */


    /* The following settings can be changed with NEXUS_Message_SetSettings */
    NEXUS_CallbackDesc dataReady; /* Callback fires when data is in the buffer. After you receive a callback, call NEXUS_Message_GetBuffer to determine how much data is present.
                                     If NEXUS_Message_GetBuffer returns non-zero, you must call NEXUS_Message_ReadComplete to re-enable the callback.
                                     Even if you don't want to consume any data, you can call NEXUS_Message_ReadComplete with 0, but you will likely receive another callback immediately.
                                     If NEXUS_Message_GetBuffer returns zero, dataReady will be re-enabled immediately by NEXUS_Message_GetBuffer. */
    NEXUS_CallbackDesc overflow;  /* Callback fires when a buffer overflow occurs. No response required. */

    NEXUS_CallbackDesc psiLengthError; /* Callback fires if there is an unexpected start of a PSI section in the middle of a current PSI section. The message will be dropped. */
    NEXUS_CallbackDesc crcError;       /* Callback fires if there is a CRC error in the PSI section. The message will be dropped. */
    NEXUS_CallbackDesc pesLengthError; /* Callback fires if a PUSI bit is detected in the middle of a PES packet. */
    NEXUS_CallbackDesc pesStartCodeError; /* Callback fires if a packet with the PUSI set does not begin with 00 00 01. */
} NEXUS_MessageSettings;

/*
Summary:
The type of data being captured by the filter.
*/
typedef enum NEXUS_MessageFormat
{
    NEXUS_MessageFormat_ePsi,       /* Filter for Program Specific Information (PSI) from an MPEG2 Transport stream */
    NEXUS_MessageFormat_ePes,       /* PES data capture. Captures PES packets with respect to PES packet length. */
    NEXUS_MessageFormat_ePesSaveAll,/* PES data capture. Captures entire payload without respect to PES packet length. */
    NEXUS_MessageFormat_eTs,        /* MPEG2 Transport data capture */
    NEXUS_MessageFormat_eMax
} NEXUS_MessageFormat;

/*
Summary:
Deprecated: The number of messages being captured in the buffer.
*/
typedef enum NEXUS_MessageBufferMode
{
    NEXUS_MessageBufferMode_eOneMessage, /* captures only one message then stops */
    NEXUS_MessageBufferMode_eOneBuffer,  /* captures as many messages as possible in one buffer, without wrapping, then stops */
    NEXUS_MessageBufferMode_eContinuous, /* captures messages with wrapping and does not stop until requested by the user */
    NEXUS_MessageBufferMode_eMax
} NEXUS_MessageBufferMode;

/*
Summary:
DSS message types
*/
typedef enum NEXUS_DssMessageType
{
    NEXUS_DssMessageType_eAuxOnlyPackets,
    NEXUS_DssMessageType_eMpt,
    NEXUS_DssMessageType_eRegular,

    /* Regular messages with CAP filtering. Set pattern using NEXUS_Message_SetDssCapPattern. */
    NEXUS_DssMessageType_eRegularCapFilter0,
    NEXUS_DssMessageType_eRegularCapFilter1,
    NEXUS_DssMessageType_eRegularCapFilter2,
    NEXUS_DssMessageType_eRegularCapFilter3,
    NEXUS_DssMessageType_eRegularCapFilter4,

    NEXUS_DssMessageType_eMax
} NEXUS_DssMessageType;


/**
Summary:
Number of bytes in each filter.

Description:
This can be reconfigured to 32 bytes by building with export BXPT_FILTER_32=y.

The message filter hardware block is composed of 512 filters, 4 bytes each.
They are arranged as 16 columns (i.e. banks) and 32 rows (i.e. # of filters in a bank).
The banks can be combined to support larger filters and fewer banks.

The Magnum XPT software support two configurations:

a) 16 byte filters in 4 banks, 32 filters per bank (verify: 16/4 * 4 * 32 = 512)
b) 32 byte filters in 2 banks, 32 filters per bank (verify: 32/4 * 2 * 32 = 512)

Magnum XPT software does not support 4, 8 or 64 byte filter configurations.
**/
#ifndef NEXUS_MESSAGE_FILTER_SIZE
#define NEXUS_MESSAGE_FILTER_SIZE 16
#endif

/**
Summary:
Filter used to capture specific PSI messages muxed together on the same PID

Description:
This does not apply to PES or TS capture.

All filtering is done using bit by bit comparisons.
There are two types of masks that are used in conjuntion with the coefficient[] array.

inclusion mask (mask) - for all inclusion mask bits that are unmasked
    (set to 0) ALL of the corresponding bits in the
    message must match the bits specified by the coefficient in order to
    be included. If one bit differs, it will be excluded.

exclusion mask (exclusion) - for all exclusion mask bits that are unmasked
    (set to 0) at least one of the corresponding bits
    in the message must NOT match the bits specified
    by the coefficient in order to be included. If all of the bits
    match, it will be excluded.

Results from both comparisons are AND'd together to form the final pass/fail decision.

Setting all bits in both inclusion mask and exclusion mask to 1's will result in the
coefficent bits being ignored.

Special note: By default, the filter skips over the 3rd byte 
    (i.e. the message length field). So:
    NEXUS_MessageFilter.mask[0] filters on message_byte[0]
    NEXUS_MessageFilter.mask[1] filters on message_byte[1]
    NEXUS_MessageFilter.mask[2] filters on message_byte[3]
    NEXUS_MessageFilter.mask[3] filters on message_byte[4] etc.
 
    The 3rd byte will be included if includeThirdFilterByte in
the NEXUS_MessageStartSettings struct is set to true. The 
default value is false.  
 
The hardware allows the filtering to start at bytes other than 
the first byte in the message data. The filterOffset member of 
the NEXUS_MessageStartSettings struct sets a zero-based offset 
into the message data. This offset is applied after 
skipping/including the third byte (above). For example, if the 
filterOffset is 3 and the 3rd byte is not included, the above 
comparison becomes: 
 
    NEXUS_MessageFilter.mask[0] filters on message_byte[4]
    NEXUS_MessageFilter.mask[1] filters on message_byte[5]
    NEXUS_MessageFilter.mask[2] filters on message_byte[6]
    NEXUS_MessageFilter.mask[3] filters on message_byte[7] etc.
 
Although message filtering only does bitwise comparisons, it is 
possible to filter on certain integer ranges. You can test for 
byte values <= an upper bound, > a lower bound, or both. The 
bounds values must be 2^N-1. That is, the only bounds are FF, 
    7F, 3F, 1F, 0F, 07, 03, 01 and 00. The formulas are: data[x]
    <= UPPER         : coef[x]=00; mask[x]=UPPER;
    exclusion[x]=FF data[x] > LOWER          : coef[x]=00;
    mask[x]=FF;    exclusion[x]=LOWER UPPER >= data[x] > LOWER :
    coef[x]=00; mask[x]=UPPER; exclusion[x]=LOWER
If you want to filter on ranges other than 2^N-1, you must first do HW filtering on a wider 2^N-1 range,
then do more narrow SW filtering within those results.
**/
typedef struct NEXUS_MessageFilter
{
    uint8_t mask[NEXUS_MESSAGE_FILTER_SIZE];        /* inclusion mask for coefficient[]. See Description. */
    uint8_t coefficient[NEXUS_MESSAGE_FILTER_SIZE]; /* bits to be included or excluded, depending on mask[] and exclusion[]. See Description. */
    uint8_t exclusion[NEXUS_MESSAGE_FILTER_SIZE];   /* exclusion mask for coefficient[]. See Description. */
} NEXUS_MessageFilter;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
*/
void NEXUS_Message_GetDefaultSettings(
    NEXUS_MessageSettings *pSettings /* [out] */
    );

/*
Summary:
Open a filter for capturing messages.

Description:
By default, a message buffer will be allocated per filter.
This buffer can be reused for multiple Start/Stop sessions without having to be reallocated. This reduces memory fragmentation in the system.
*/
NEXUS_MessageHandle NEXUS_Message_Open( /* attr{destructor=NEXUS_Message_Close} */
    const NEXUS_MessageSettings *pSettings /* attr{null_allowed=y} */
    );

/*
Summary:
Close the filter.
*/
void NEXUS_Message_Close(
    NEXUS_MessageHandle handle
    );

/*
Summary:
Get settings which were set at NEXUS_Message_Open or the last call to NEXUS_Message_SetSettings.
*/
void NEXUS_Message_GetSettings(
    NEXUS_MessageHandle handle,
    NEXUS_MessageSettings *pSettings /* [out] */
    );

/*
Summary:
Set new settings

Description:
Read the comments in NEXUS_MessageSettings to see which individual settings will be updated by this function.
*/
NEXUS_Error NEXUS_Message_SetSettings(
    NEXUS_MessageHandle handle,
    const NEXUS_MessageSettings *pSettings
    );

/*
Summary:
Settings needed to start a message filter.
*/

typedef struct NEXUS_MessageStartSettings
{
    NEXUS_PidChannelHandle pidChannel;  /* Input to message filter */

    NEXUS_MessageFormat format;         /* Type of data to capture. Defaults to ePsi. */
    NEXUS_MessageBufferMode bufferMode; /* Deprecated */
    int bank; /* Select filter bank 0..3. If -1, Nexus will automatically select the bank. Default is -1. */

    NEXUS_MessageFilter filter;         /* See structure for documentation. */
    bool filterGroup;                   /* Filter group support allows multiple filters to multiplex their messages into a single buffer.
                                           For older silicon, this was required for multiple filters on the same pid. See NEXUS_USE_MSGLIB in transport.inc
                                           and Nexus_Usage.pdf for this legacy support.
                                           For newer silicon, it can optionally be turned on, but must be used with NEXUS_Message_AddFilter and Nexus will not
                                           do any automatic demuxing of the messages. The application will need to demux the messages. */
    bool psiCrcDisabled;                /* Disable CRC checks on PSI information */
    bool psfCrcDisabled;                /* Disable CRC checks on short form private sections */

    void *buffer;        /* attr{memory=cached} Optional user-supplied memory buffer which is bufferSize in length.
                            You must allocate this buffer with NEXUS_Memory_Allocate. Must be 1K aligned.
                            If NULL, Nexus will use the buffer allocated with NEXUS_MessageSettings.bufferSize.
                            See NEXUS_Message_Open for usage modes. */
    unsigned bufferSize; /* Size in bytes. Only certain buffer sizes may be supported by HW. The actual size used will be rounded down to the nearest supported value. */

    NEXUS_DssMessageType dssMessageType;
    bool useRPipe;

    unsigned filterOffset;       /* offset all filters by this number of bytes */
    bool includeThirdFilterByte; /* default to false. if filterOffset is non-zero, you will likely want to set this to true. */
} NEXUS_MessageStartSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.

Default settings will result in all PSI data being returned for a given PID.
*/
void NEXUS_Message_GetDefaultStartSettings(
    NEXUS_MessageHandle handle,
    NEXUS_MessageStartSettings *pStartSettings /* [out] */
    );

/*
Summary:
Start capturing data with this filter.
*/
NEXUS_Error NEXUS_Message_Start(
    NEXUS_MessageHandle handle,
    const NEXUS_MessageStartSettings *pStartSettings
    );

/*
Summary:
Get a default filter before calling NEXUS_Message_AddFilter.
*/
void NEXUS_Message_GetDefaultFilter(
    NEXUS_MessageFilter *pFilter
    );

/*
Summary:
Add an additional filter to an already-started message filter session

Description:
Any messages captured by this additional filter will be multiplexed into the
NEXUS_MessageHandle's single buffer. It is the application's responsibility to
demux those messages.

NEXUS_Message_AddFilter must be called after NEXUS_Message_Start.
The additional filter will be applied on the same pid channel, bank, etc.
*/
NEXUS_Error NEXUS_Message_AddFilter(
    NEXUS_MessageHandle handle,
    const NEXUS_MessageFilter *pFilter,
    unsigned *pFilterNum /* [out] */
    );

/*
Summary:
Remove a filter added with NEXUS_Message_AddFilter

Description:
If you call NEXUS_Message_Stop, all filters added with NEXUS_Message_AddFilter are automatically
removed. NEXUS_Message_RemoveFilter is only needed when you want to remove a single filter
from a message filter session which must continue running.
*/
NEXUS_Error NEXUS_Message_RemoveFilter(
    NEXUS_MessageHandle handle,
    unsigned filterNum
    );

#define NEXUS_MESSAGE_MAIN_FILTER_NUM ((unsigned)-1)

/*
Summary:
Update a filter that's already been started
*/
NEXUS_Error NEXUS_Message_UpdateFilter(
    NEXUS_MessageHandle handle,
    unsigned filterNum, /* use NEXUS_MESSAGE_MAIN_FILTER_NUM to update the filter set with NEXUS_MessageStartSettings.filter.
                           else, use the filterNum returned by NEXUS_Message_AddFilter. */
    const NEXUS_MessageFilter *pFilter
    );

/*
Summary:
Stop capturing messages with this filter.

Description:
You can restart this filter with different MessageStartSettings.
You cannot call NEXUS_Message_GetBuffer or NEXUS_Message_ReadComplete after calling NEXUS_Message_Stop.
*/
void NEXUS_Message_Stop(
    NEXUS_MessageHandle handle
    );

/**
Summary:
Get a pointer to a message captured for this filter.

Description:
Nexus will always return whole PSI messages if maxContiguousMessageSize is >= the largest PSI message and if the application always
calls NEXUS_Message_ReadComplete on whole messages (including the pad). If you call NEXUS_Message_ReadComplete with partial message length, then NEXUS_Message_GetBuffer will return the partial amount remaining in the buffer.

You can call NEXUS_Message_GetBuffer multiple times without changing Message state. It is not destructive.
You cannot call NEXUS_Message_GetBuffer after calling NEXUS_Message_Stop.

One call to NEXUS_Message_GetBuffer may retrieve zero, one or more messages.
**/
NEXUS_Error NEXUS_Message_GetBuffer(
    NEXUS_MessageHandle handle,
    const void **pBuffer, /* [out] attr{memory=cached} Pointer to cached memory where the next message to be processed by the caller is stored.
        Note that this may not come from MessageSettings.buffer. It may come from an internal buffer if maxContiguousMessageSize > 0,
        or may come from a group buffer (another filter in the same filter group).
        Returns NULL if no data is available. */
    size_t *pLength /* [out] Number of bytes pointed to by pBuffer. Returns 0 if no data is available. */
    );

/**
Summary:
Get two pointers to a message captured for this filter, both before and after the wrap around.

Description:
See NEXUS_Message_GetBuffer for general usage.

NEXUS_Message_GetBufferWithWrap requires maxContiguousMessageSize=0.

If NEXUS_Message_GetBufferWithWrap returns non-zero sizes for pLength and pLength2, you may call NEXUS_Message_ReadComplete with any number <= their sum.
**/
NEXUS_Error NEXUS_Message_GetBufferWithWrap(
    NEXUS_MessageHandle handle,
    const void **pBuffer, /* [out] attr{memory=cached} data before wrap. Returns NULL if no data is available. */
    size_t *pLength, /* [out] Number of bytes pointed to by pBuffer. Returns 0 if no data is available. */
    const void **pBuffer2, /* [out] attr{memory=cached} data after wrap. Returns NULL if no data is available. */
    size_t *pLength2 /* [out] Number of bytes pointed to by pBuffer2. Returns 0 if no data is available. */
    );

/*
Summary:
Report back how much data was consumed from the last NEXUS_Message_GetBuffer call.

Description:
It must be <= the length returned by NEXUS_Message_GetBuffer.
If < length, you must call NEXUS_Message_GetBuffer again to reobtain the pointer to the buffer.
That is, you cannot call NEXUS_Message_ReadComplete more than once after each NEXUS_Message_GetBuffer.
You cannot call NEXUS_Message_ReadComplete after calling NEXUS_Message_Stop.

You can call NEXUS_Message_ReadComplete with amountConsumed == 0.

NEXUS_Message_ReadComplete must be called to re-enable the dataReady callback if you received a callback and NEXUS_Message_GetBuffer returns a non-zero value.

The transport message filtering hardware ensure 4 byte alignment by adding up to 3 padding bytes as needed.
The length returned by NEXUS_Message_GetBuffer includes this pad.
However, when you parse the length field from the PSI section data, it will not include this pad.
If your message length is not 4 byte aligned and if you call ReadComplete based on message size, then
you need to factor in this pad to your amountConsumed. For example:

    if (message_size % 4) {
        message_size = message_size - (message_size % 4) + 4;
    }
    NEXUS_Message_ReadComplete(msg, message_size)

*/
NEXUS_Error NEXUS_Message_ReadComplete(
    NEXUS_MessageHandle handle,
    size_t amountConsumed /* number of bytes the caller has processed. Must be <= the length returned by last call to NEXUS_Message_GetBuffer. */
    );

/*
Summary:
Status information
*/
typedef struct NEXUS_MessageStatus
{
    unsigned groupMembers; /* total number of filters in the filter group, including this session */
    bool isGroupMaster; /* true if this session is the group master */
    bool moreDataAvailable; /* set to true if the last NEXUS_Message_GetBuffer call detected that more data is available on a wrap around */
} NEXUS_MessageStatus;

/*
Summary:
Get status information about the session
*/
NEXUS_Error NEXUS_Message_GetStatus(
    NEXUS_MessageHandle handle,
    NEXUS_MessageStatus *pStatus /* [out] */
    );

/*
Summary:
Set CAP pattern corresponding to each NEXUS_DssMessageType_eRegularCapFilter type
*/
NEXUS_Error NEXUS_Message_SetDssCapPattern(
    unsigned capFilterIndex, /* 0 corresponds to NEXUS_DssMessageType_eRegularCapFilter0, etc. */
    uint32_t pattern /* 32-bit CAP pattern */
    );

#ifdef __cplusplus
}
#endif

#endif
