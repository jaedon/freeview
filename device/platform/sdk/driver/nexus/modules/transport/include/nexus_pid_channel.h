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
 * $brcm_Workfile: nexus_pid_channel.h $
 * $brcm_Revision: 27 $
 * $brcm_Date: 10/4/12 5:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/nexus_pid_channel.h $
 * 
 * 27   10/4/12 5:47p erickson
 * SW7435-391: redo NEXUS_PidChannelStatus.parserBand to return HW parser
 *  band index
 * 
 * 26   10/31/11 7:05p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 25   9/6/11 10:59a jtna
 * SW7425-1215: add const to NEXUS_PidChannel_SetRemapSettings()
 * 
 * 24   9/2/11 2:54p jtna
 * SW7425-1215: add NEXUS_PidChannel_SetRemapSettings() and related
 *  changes
 * 
 * 23   8/31/11 12:02p erickson
 * SWDEPRECATED-2425: document audio VOB substream id
 * 
 * 22   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 21   1/3/11 1:56p erickson
 * SW7422-101: add NEXUS_PidChannelSettings.continuityCountEnabled
 *
 * 20   12/7/10 5:45p erickson
 * SW7420-1285: add null_allowed
 *
 * 19   6/16/10 12:22p erickson
 * SWDEPRECATED-2425: clarify how to reserve a
 *  NEXUS_PidChannelSettings.pidChannelIndex for specific pid channel
 *  index requirement
 *
 * 18   6/2/10 4:34p erickson
 * SW7335-739: remove unused field NEXUS_PidChannelSettings.secure
 *
 * 17   1/27/10 4:34p erickson
 * SW7400-2651: add NEXUS_PidChannelSettings.remap for SPID pid remapping
 *
 * 16   11/11/09 2:40p erickson
 * SW7400-2601: add NEXUS_PidChannelSettings.enabled, default to true
 *
 * 15   10/19/09 11:26a erickson
 * SW7400-2559: add NEXUS_PidChannelSettings.dssHdFilter
 *
 * 14   10/19/09 10:42a erickson
 * SW7400-2559: add NEXUS_PID_CHANNEL_OPEN macros for more
 *  NEXUS_PidChannel_Open options
 *
 * 13   10/1/09 5:02p erickson
 * SW7405-3087: count continuity count errors per pid, provide
 *  NEXUS_PidChannel_ResetStatus
 *
 * 12   6/12/09 3:31p erickson
 * PR55971: update comments related to remux usage
 *
 * 11   4/13/09 3:31p erickson
 * PR54058: deprecate NEXUS_PidChannelSettings.requireMessageBuffer. apps
 *  can use pidChannelIndex instead.
 *
 * 10   11/12/08 1:26p jgarrett
 * PR 48830: Adding NEXUS_PidChannel_SetEnabled
 *
 * 9   9/2/08 10:11a erickson
 * PR35457: update comments
 *
 * 8   7/21/08 1:27p erickson
 * PR45008: add pid splicing API
 *
 * 7   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 6   4/21/08 3:10p erickson
 * PR41987: add optional NEXUS_PidChannelSettings.pidChannelIndex for
 *  user-specified pid channel index
 *
 * 5   4/10/08 1:58p erickson
 * PR40079: added PidChannel ScramblingCheck
 *
 * 4   4/4/08 11:18a erickson
 * PR40079: impl NEXUS_PidChannel_GetScramblingStatus
 *
 * 3   2/25/08 9:06a jgarrett
 * PR 39435: Merging to main branch
 *
 * PR39435/1   2/22/08 5:30p shyam
 * PR39435 : Remove dependency on nexus_parser_band.h
 *
 * 2   2/1/08 5:05p vsilyaev
 * PR 38682: Added attributes to mark destructor and shutdown functions
 *
 * 1   1/18/08 2:15p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/24   1/8/08 9:27a erickson
 * PR34925: update comments
 *
 * Nexus_Devel/23   12/18/07 4:07p jgarrett
 * PR 38310: Adding original transport type to pid channel status
 *
 * Nexus_Devel/22   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 *
 **************************************************************************/
#ifndef NEXUS_PID_CHANNEL_H__
#define NEXUS_PID_CHANNEL_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle which represents a transport PID channel.

Description:
A NEXUS_PidChannel routes one PID from a parser band to a variety of clients (decoder, record, message filtering).

A NEXUS_PidChannel must be opened according to its source:

- NEXUS_PidChannel_Open is used to open a PID channel from a NEXUS_ParserBand. This is used for live streams.
- NEXUS_Playpump_OpenPidChannel is used to open a PID channel from a NEXUS_PlaypumpHandle. This is used for low-level PVR.
- NEXUS_Playback_OpenPidChannel is used to open a PID channel from a NEXUS_PlaybackHandle. This is used for high-level PVR.

PID channels are used by a variety of other Nexus interfaces:

- See NEXUS_VideoDecoderStartSettings for video decode
- See NEXUS_AudioDecoderStartSettings for audio decode
- See NEXUS_Recpump_AddPidChannel for low-level record
- See NEXUS_Record_AddPidChannel for high-level record
- See NEXUS_MessageStartSettings for message filtering
- See NEXUS_Remux_AddPidChannel for remux output
**/
typedef struct NEXUS_PidChannel *NEXUS_PidChannelHandle;

/**
Summary:
Special values for NEXUS_PidChannelSettings.pidChannelIndex
**/
#define NEXUS_PID_CHANNEL_OPEN_ANY (-1)
#define NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE (-2)
#define NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE (-3)

/**
Summary:
DSS HD filter modes for pid channels
**/
typedef enum NEXUS_PidChannelDssHdFilter {
    NEXUS_PidChannelDssHdFilter_eDisabled,
    NEXUS_PidChannelDssHdFilter_eAux, /* only AUX packets */
    NEXUS_PidChannelDssHdFilter_eNonAux, /* only non-AUX packets */
    NEXUS_PidChannelDssHdFilter_eMax
} NEXUS_PidChannelDssHdFilter;

/**
Summary:
Secondary PID (SPID) remapping
**/
typedef struct NEXUS_PidChannelRemapSettings
{
    bool enabled; /* enabled PID remapping */
    uint16_t pid; /* the new PID value */
    bool continuityCountEnabled; /* If true, transport will check for correct continuity counters and discard bad packets. Default is true. */
}NEXUS_PidChannelRemapSettings;

/**
Summary:
Settings for a PID channel passed into NEXUS_PidChannel_Open.
**/
typedef struct NEXUS_PidChannelSettings
{
    bool requireMessageBuffer; /* deprecated. use NEXUS_PidChannelSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE instead. */
    int pidChannelIndex;       /* Set the pid channel allocation scheme. This can be one of the special NEXUS_PID_CHANNEL_OPEN values, or an unsigned pidChannelIndex value.
                                  NEXUS_PID_CHANNEL_OPEN_ANY (default) will select any hardware PID channel index.
                                  NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE will select a hardware PID channel index which is capable of message filtering.
                                    Only the lower 128 pid channels are capable of message filtering.
                                  NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE will select a hardware PID channel index which is not capable of message filtering.
                                    This allows an application to reserve message-capable pid channels for message filtering.
                                  Otherwise, Nexus will use the pidChannelIndex if there is not a pid conflict on the same HW PID channel.

                                  If you need a specific pid channel index somewhere in your system, you may have to reserve that index by opening it
                                  immediately after system initialization. One case would be requiring an "all pass" pid channel. */
    NEXUS_PidChannelDssHdFilter dssHdFilter; /* DSS HD filter modes */
    bool enabled;              /* true by default. call NEXUS_PidChannel_SetEnabled to change after opening. */
    bool continuityCountEnabled;  /* If true, transport will check for correct continuity counters and discard bad packets. Default is true. */
    bool generateContinuityCount; /* If true, transport will generate a new continuity counters for the TS */

    NEXUS_PidChannelRemapSettings remap;
} NEXUS_PidChannelSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_PidChannel_GetDefaultSettings(
    NEXUS_PidChannelSettings *pSettings /* [out] */
    );

/**
Summary:
Open a new PID channel from a parser band.

Description:
Most platforms do not allow multiple HW PID channels for the same pid and parser band.
Therefore, nexus will reference count the multiple uses of the same HW PID channel, but return a
new NEXUS_PidChannelHandle value for each call to open.
Also, all duplicate pid channels must have the same NEXUS_PidChannelSettings.
**/
NEXUS_PidChannelHandle NEXUS_PidChannel_Open( /* attr{destructor=NEXUS_PidChannel_Close}  */
    NEXUS_ParserBand parserBand,              /* Data source for the PID channel */
    uint16_t pid,                             /* PID (packet ID) that will be selected from the parser band. If TS, this is a PID. If PES, this is a PES stream id.
                                                 If ES or NEXUS_ParserBandSettings.allPass is true, this param is ignored.
                                                 For audio VOB streams, the upper 8 bits of the PID specify the substream id. So, 0x00BD is AC3/DTS track 0,
                                                 0x01BD is AC3/DTS track 1, etc. */
    const NEXUS_PidChannelSettings *pSettings /* attr{null_allowed=y} Optional */
    );

/*
Summary:
Close a PID channel opened by NEXUS_PidChannel_Open.

Description:
This function should not close a PID channel opened by NEXUS_Playpump_OpenPidChannel or NEXUS_Playback_OpenPidChannel.
*/
void NEXUS_PidChannel_Close(
    NEXUS_PidChannelHandle pidChannel
    );

/**
Summary:
Close all PID channels opened on a parser band.

Description:
All associated PID channel handles will become invalid.
**/
void NEXUS_PidChannel_CloseAll(
    NEXUS_ParserBand parserBand
    );

/**
Summary:
Status for a PID channel retrieved from NEXUS_PidChannel_GetStatus
**/
typedef struct NEXUS_PidChannelStatus
{
    uint16_t pid;                      /* PID carried by this PID channel */
    unsigned pidChannelIndex;          /* HW PID channel index */
    NEXUS_TransportType transportType; /* If the parser band packetized the input, this described the output. If there is no
                                          packetization, this is the original transportType. */
    NEXUS_TransportType originalTransportType; /* If the parser band packetized the input, this describes the input. If there is no
                                          packetization, this is the same as transportType. */

    uint16_t remappedPid;              /* If the parser band packetized the input (for ES/PES playback) or
                                          if NEXUS_PidChannelSettings.remap.enabled is true (for live SPID remapping),
                                          this describes the PID of the output; otherwise, this is the original PID. */
    bool enabled;                      /* True if the PidChannel is enabled, false otherwise. */
    
    bool playback;                     /* true if pid channel is from a playback parser band. otherwise, it's from a live parser band. */
    unsigned playbackIndex;            /* HW playback index. Valid only if playback==true. */
    unsigned parserBand;               /* HW parser band index. Valid only if playback==false. */

    unsigned continuityCountErrors;    /* Rough count of cc errors for this pid channel since it was opened or since NEXUS_PidChannel_ResetStatus was called.
                                          The count is only performed if either NEXUS_ParserBandSettings.ccError and .continuityCountEnabled is set or
                                          NEXUS_PlaypumpSettings.ccError and .continuityCountEnabled is set for this pid.
                                          There is no ccError callback per pid.
                                          An exact count is not possible because more than one error could occur before the software can count and clear the hardware status register. */
} NEXUS_PidChannelStatus;

/**
Summary:
Get current status for a PID channel.
**/
NEXUS_Error NEXUS_PidChannel_GetStatus(
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PidChannelStatus *pStatus /* [out] */
    );

/**
Summary:
Inform a new PID channel that data can start flowing.

Description:
For some PID channels, notably those fed by playback, the flow of data may be paused until the first consumer is started.
This prevents lost data due to application delay between calls to Nexus.

A Nexus consumer (e.g. decoder, record, message filter, remux) will internally call this function when started
or when a pid channel is added to an already-started consumer.
This only needs to be called from the application for a non-Nexus consumer or some atypical configuration.
**/
void NEXUS_PidChannel_ConsumerStarted(
    NEXUS_PidChannelHandle pidChannel
    );

/**
Summary:
Add a PID channel for PID splicing

Description:
The transport hardware will monitor the transport stream for splice points.

This is only supported for PID channels which are currently being decoded.

Call NEXUS_PidChannel_RemoveSplicePidChannel or NEXUS_PidChannel_RemoveSplicePidChannels to remove the PID channel and disable PID splicing.
**/
NEXUS_Error NEXUS_PidChannel_AddSplicePidChannel(
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PidChannelHandle splicePidChannel
    );

/**
Summary:
Remove a PID channel that was added using NEXUS_PidChannel_AddSplicePidChannel
**/
NEXUS_Error NEXUS_PidChannel_RemoveSplicePidChannel(
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PidChannelHandle splicePidChannel
    );

/**
Summary:
Remove all PID channels that were added using NEXUS_PidChannel_AddSplicePidChannel
**/
void NEXUS_PidChannel_RemoveAllSplicePidChannels(
    NEXUS_PidChannelHandle pidChannel
    );

/**
Summary:
Enable or disable a PID channel on the fly.
**/
NEXUS_Error NEXUS_PidChannel_SetEnabled(
    NEXUS_PidChannelHandle pidChannel,
    bool enabled
    );

/**
Summary:
Set the remap settings for an already-opened PID channel.
**/
NEXUS_Error NEXUS_PidChannel_SetRemapSettings(
    NEXUS_PidChannelHandle pidChannel,
    const NEXUS_PidChannelRemapSettings *pSettings
    );

/**
Summary:
Reset any accumulated values in NEXUS_PidChannelStatus
**/
NEXUS_Error NEXUS_PidChannel_ResetStatus(
    NEXUS_PidChannelHandle pidChannel
    );

#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error NEXUS_CI_Access(
	unsigned long RegMem, 
	unsigned long RdWr, 
	unsigned long Offset, 
	unsigned long Value
	);

NEXUS_Error NEXUS_CI_Read(
	unsigned long RegMem, 
	unsigned long Offset, 
	unsigned long *Value /* [out] */
	);	

NEXUS_Error NEXUS_CI_Write(
	unsigned long RegMem, 
	unsigned long Offset, 
	unsigned long Value
	);	
unsigned int NEXUS_Get_PidChannel_Used_HUMAX(NEXUS_PidChannelHandle pidChannel);
#endif

#ifdef __cplusplus
}
#endif

#endif
