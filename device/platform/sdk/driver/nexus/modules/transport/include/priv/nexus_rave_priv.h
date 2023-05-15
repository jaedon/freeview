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
 * $brcm_Workfile: nexus_rave_priv.h $
 * $brcm_Revision: 21 $
 * $brcm_Date: 8/29/12 5:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/priv/nexus_rave_priv.h $
 * 
 * 21   8/29/12 5:06p bandrews
 * SW7231-975: add ITB status accessor
 * 
 * 20   5/30/12 4:50p vsilyaev
 * SW7425-3028: Added function to look for specific PTS in the rave buffer
 * 
 * 19   2/22/12 5:13p erickson
 * SW7425-2459: OTFPVR requires heap used by RAVE, verify CPU
 *  accessibility
 * 
 * 18   4/18/11 10:19a erickson
 * SW7335-1168: add NEXUS_RaveStatus.enabled
 *
 * 17   4/6/11 2:56p erickson
 * SW7342-340: add HW rave indices to NEXUS_VideoDecoderExtendedStatus
 *
 * 16   3/1/11 10:21a erickson
 * SW7335-1168: make NEXUS_RaveHandle public
 *
 * 15   12/13/10 4:06p erickson
 * SW35230-179: add NEXUS_RaveStatus.numOutputBytes
 *
 * 14   2/8/10 5:02p vsilyaev
 * SW3556-1003: Added flag to disable reordering of video frames
 *
 * 13   4/2/09 1:51p erickson
 * PR53817: nexus_base.h should not be included in any public api
 *
 * 12   2/4/09 7:09p vsilyaev
 * PR 50887: Added bit to enable OTF PVR mode
 *
 * 11   1/16/09 2:02p erickson
 * PR50905: added NEXUS_Rave_GetMostRecentPts_priv
 *
 * 10   12/8/08 12:22p erickson
 * PR48155: check cdb valid and read to determine if fifo is hung
 *
 * 9   9/9/08 10:04a erickson
 * PR45940: added NEXUS_Rave_GetCdbValidPointer_isr for better detection
 *  if source fifo goes dry
 *
 * 8   7/23/08 3:01p jgarrett
 * PR 39452: Merging to main branch
 *
 * 7   7/17/08 5:24p jgarrett
 * PR 42642: Adding Get/Set thresholds
 *
 * Nexus_Audio_Descriptors/1   5/15/08 5:35p jgarrett
 * PR 42360: Adding audio descriptors
 *
 * 6   5/13/08 7:16p jgarrett
 * PR 42221: Adding frame depth for audio
 *
 * 5   4/7/08 10:16a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 4   4/4/08 11:18a erickson
 * PR40079: impl NEXUS_PidChannel_GetScramblingStatus
 *
 * 3   2/26/08 10:01a erickson
 * PR39934: added SW RAVE support
 *
 * 2   2/20/08 4:59p erickson
 * PR39786: add NEXUS_Rave_FindVideoStartCode_priv
 *
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   11/19/07 1:20p erickson
 * PR34925: split demux.h into separate interface files
 *
 * Nexus_Devel/4   10/3/07 9:50a erickson
 * PR34925: coding convention
 *
 * Nexus_Devel/3   10/1/07 1:45p erickson
 * PR34925: make Rave a private API
 *
 * Nexus_Devel/2   9/24/07 11:16a erickson
 * PR34925: update
 *
 * Nexus_Devel/1   9/21/07 5:03p erickson
 * PR34926: added calls to StcChannel from VideoDecoder
 *
 * Nexus_Devel/3   9/21/07 11:15a erickson
 * PR34925: update
 *
 * Nexus_Devel/2   9/14/07 3:27p erickson
 * PR34925: initial compilation
 *
 * Nexus_Devel/1   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#ifndef NEXUS_RAVE_PRIV_H__
#define NEXUS_RAVE_PRIV_H__

#include "nexus_types.h"
#include "nexus_rave.h"
#include "nexus_pid_channel.h"
#include "bavc_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_RaveOpenSettings
{
    bool record;
    bool restrictedRegion;
    BAVC_CdbItbConfig config;
    bool allocSoftRave; /* set true if codec will requires SW rave processing (true for some video codecs) */
} NEXUS_RaveOpenSettings;

void NEXUS_Rave_GetDefaultOpenSettings_priv(
    NEXUS_RaveOpenSettings *pSettings /* [out] */
    );

NEXUS_RaveHandle NEXUS_Rave_Open_priv(
    const NEXUS_RaveOpenSettings *pOpenSettings
    );

void NEXUS_Rave_Close_priv(
    NEXUS_RaveHandle handle
    );

typedef struct NEXUS_RaveSettings
{
    NEXUS_PidChannelHandle pidChannel;
    bool bandHold;
    bool continuityCountEnabled;
    bool audioDescriptor;           /* If true, this context will mark audio dsecriptors */
    bool otfPvr;    /* If true, this context is used as input for the OTF PVR */
    bool disableReordering; /* if true, this context would not reorder video frames, where otherwise reordering is required */
    bool numOutputBytesEnabled; /* if true, count numOutputBytes for this RAVE context.
                             this requires a SW poll, so it's not an always-on feature. */
} NEXUS_RaveSettings;

void NEXUS_Rave_GetDefaultSettings_priv(
    NEXUS_RaveSettings *pSettings /* [out] */
    );

NEXUS_Error NEXUS_Rave_ConfigureVideo_priv(
    NEXUS_RaveHandle handle,
    NEXUS_VideoCodec codec,
    const NEXUS_RaveSettings *pSettings
    );

NEXUS_Error NEXUS_Rave_ConfigureAudio_priv(
    NEXUS_RaveHandle handle,
    NEXUS_AudioCodec codec,
    const NEXUS_RaveSettings *pSettings
    );

/* allow all data to pass */
NEXUS_Error NEXUS_Rave_ConfigureAll_priv(
    NEXUS_RaveHandle handle,
    const NEXUS_RaveSettings *pSettings
    );

typedef struct NEXUS_RaveStatus
{
    unsigned index; /* HW index of RAVE */
    int swRaveIndex; /* HW index of SW RAVE. -1 if unused */
    BAVC_XptContextMap xptContextMap; /* register information required for decoders to make connection.
                                         if SW RAVE is enabled, this will be the linked context */
    uint64_t numOutputBytes; /* requires numOutputBytesEnabled = true */
    bool enabled;
    NEXUS_HeapHandle heap; /* heap used for rave allocations */
} NEXUS_RaveStatus;

NEXUS_Error NEXUS_Rave_GetStatus_priv(
    NEXUS_RaveHandle handle,
    NEXUS_RaveStatus *pStatus /* [out] */
    );

void NEXUS_Rave_Enable_priv(
    NEXUS_RaveHandle handle
    );

void NEXUS_Rave_Disable_priv(
    NEXUS_RaveHandle handle
    );

void NEXUS_Rave_Flush_priv(
    NEXUS_RaveHandle handle
    );

void NEXUS_Rave_RemovePidChannel_priv(
    NEXUS_RaveHandle handle
    );

void NEXUS_Rave_GetCdbBufferInfo_isr(
    NEXUS_RaveHandle handle,
    unsigned *depth, /* [out] */
    unsigned *size   /* [out] */
    );

void NEXUS_Rave_GetItbBufferInfo_isr(
    NEXUS_RaveHandle rave, 
    unsigned *depth, /* [out] */
    unsigned *size   /* [out] */
    );

bool NEXUS_Rave_FindPts_priv(
    NEXUS_RaveHandle rave,
    uint32_t pts);

bool NEXUS_Rave_FindVideoStartCode_priv(
    NEXUS_RaveHandle rave,
    uint8_t startCode
    );

void NEXUS_Rave_GetAudioFrameCount_priv(
    NEXUS_RaveHandle rave,
    unsigned *pFrameCount     /* [out] */
    );

NEXUS_Error NEXUS_Rave_SetCdbThreshold_priv(
    NEXUS_RaveHandle rave,
    unsigned cdbDepth       /* CDB threshold in bytes (0 is default) */
    );

/**
Summary:
This function is used to determine if data is entering the CDB.
If the VALID and READ pointers become static over a period of time, then
either data has stopped flowing or the buffer is full and the RAVE band hold
is set.
**/
void NEXUS_Rave_GetCdbPointers_isr(
    NEXUS_RaveHandle rave,
    uint32_t *validPointer, /* [out] */
    uint32_t *readPointer /* [out] */
    );

/**
Summary:
Added to support NEXUS_VideoDecoder_GetMostRecentPts
**/
NEXUS_Error NEXUS_Rave_GetMostRecentPts_priv(
    NEXUS_RaveHandle rave,
    uint32_t *pPts /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif
