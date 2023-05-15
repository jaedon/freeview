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
 * $brcm_Workfile: nexus_parser_band.h $
 * $brcm_Revision: 33 $
 * $brcm_Date: 10/8/12 1:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/nexus_parser_band.h $
 * 
 * 33   10/8/12 1:03p erickson
 * SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
 *  connections
 * 
 * 32   10/3/12 5:00p jtna
 * SW7425-3782: refactor nexus tsmf impl
 * 
 * 31   5/10/12 6:17p jtna
 * SW7425-2752: rework MTSIF-frontend <-> host interface
 * 
 * 30   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 29   10/31/11 7:05p bandrews
 * SW7231-391: merge to main
 * 
 * 28   10/17/11 12:07p jtna
 * SW7425-1383: merge NEXUS_ParserBand_GetAllPassPidChannelIndex
 * 
 * SW7420-2078/3   10/11/11 8:26p bandrews
 * SW7231-391: add support for parser band and timebase protected client
 *  usage
 * 
 * SW7420-2078/2   10/6/11 9:14p bandrews
 * SW7420-2078: merge from main
 * 
 * 27   10/6/11 2:04p erickson
 * SW7346-502: add NEXUS_ParserBandSettings.acceptAdapt00
 * 
 * SW7420-2078/1   10/6/11 8:14p bandrews
 * SW7231-391: change parser band source type settings from a union to a
 *  struct for verification ease
 * 
 * 26   5/2/11 3:07p jtna
 * SW7408-1: fix warning for platforms without parser bands
 * 
 * 25   4/29/11 10:44a erickson
 * SW7346-163: add required #include
 * 
 * 24   4/15/11 10:35a erickson
 * SW7425-313: add initial MTSIF support
 *
 * 23   4/1/11 4:24p jtna
 * SW7346-119: add 40nm allpass support
 *
 * 22   3/10/11 9:23a erickson
 * SWDEPRECATED-2425: clarify maxDataRate
 *
 * 21   2/9/11 2:18p erickson
 * SW7420-1009: add NEXUS_ParserBand_Acquire and Release
 *
 * 20   12/9/10 3:32p erickson
 * SW7420-1308: use unconditional nexus_dma_types.h in the public API
 *
 * 19   10/21/10 10:44a erickson
 * SW35230-1563: add NEXUS_ParserBandSettings.teiIgnoreEnabled
 *
 * 18   10/1/09 4:45p erickson
 * SW7405-3087: fix comment
 *
 * 17   9/8/09 1:25p erickson
 * SWDEPRECATED-2425: improve comments on maxDataRate
 *
 * 16   6/9/09 3:05p erickson
 * PR55767: move timestampType from NEXUS_ParserBandSettings to
 *  NEXUS_RecpumpSettings
 *
 * 15   12/12/08 11:00a erickson
 * PR49927: add NEXUS_ParserBandSettings.lengthError
 *
 * 14   12/1/08 12:53p erickson
 * PR49676: add comments on pidChannelIndex requirements for allPass pid
 *  channels
 *
 * 13   7/23/08 10:29a erickson
 * PR42197: remove NEXUS_ParserBandSettings.pacing because it only applies
 *  to playback, which is not supported with this interface
 *
 * 12   6/25/08 4:30p katrep
 * PR44085: Program the block XPT RS/XC buffer blockout counters and
 *  according to packet size.
 *
 * 11   6/13/08 7:19p erickson
 * PR42973: merge all pass changes
 *
 * PR42973/1   5/22/08 3:34p shyam
 * PR 42973 : Add allPass and Null PAcket support to Nexus
 *
 * 10   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 9   3/5/08 2:10p jrubio
 * PR40169: add REMUX var
 *
 *
 * 7   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 4   2/22/08 7:18p shyam
 * PR39435: PR39435 : Add remux capability - remove duplicate definition
 *  of Remux Handle
 *
 * PR39435/1   2/22/08 5:29p shyam
 * PR39435 : Add remux capability - remove duplicate definition of Remux
 *  Handle
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/3   1/8/08 9:27a erickson
 * PR34925: update comments
 *
 * Nexus_Devel/2   11/30/07 11:13a erickson
 * PR35457: api update
 *
 * Nexus_Devel/1   11/19/07 4:34p erickson
 * PR34925: added InputBandSettings
 *
 **************************************************************************/
#ifndef NEXUS_PARSER_BAND_H__
#define NEXUS_PARSER_BAND_H__

#include "nexus_types.h"
#include "nexus_transport_capabilities.h"
#include "nexus_remux.h"
#include "nexus_pid_channel.h"
#include "nexus_input_band.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=********************
A NEXUS_ParserBand routes data from an input band or remux input to a variety of pid channels.

Use NEXUS_PidChannel to route individual pids to various consumers.
*********************/

/*
Summary:
Used in NEXUS_ParserBandSettings to specify the type of input to the parser band.
*/
typedef enum NEXUS_ParserBandSourceType
{
    NEXUS_ParserBandSourceType_eInputBand,
    NEXUS_ParserBandSourceType_eRemux,
    NEXUS_ParserBandSourceType_eMtsif,
    NEXUS_ParserBandSourceType_eTsmf,
    NEXUS_ParserBandSourceType_eMax
} NEXUS_ParserBandSourceType;

/**
Summary:
Settings for an parser band. Retrieved with NEXUS_ParserBand_GetSettings.
**/
typedef struct NEXUS_ParserBandSettings
{
    NEXUS_ParserBandSourceType sourceType; /* Specifies from what type of source the data is coming into the parser band. See sourceTypeSettings for
                                              additional settings for each enum. */
    struct
    {
        NEXUS_InputBand inputBand;     /* Input band that is mapped to this parserBand. You can route an input band
                                          to multiple parser bands. */
        NEXUS_RemuxHandle remux;       /* A loopback route from a remux output back into a parser band */
        NEXUS_FrontendConnectorHandle mtsif; /* Frontend connector whose input band is mapped to this parserBand, via MTSIF.
                                          Use NEXUS_Frontend_GetConnector(frontend) to get this handle.
                                          The MTSIF connection is established when the frontend is tuned, not when NEXUS_ParserBand_SetSettings is called.
                                          Likewise, the connection is destroyed when frontend is untuned. */
        NEXUS_TsmfHandle tsmf;
    } sourceTypeSettings; /* See sourceType to determine which member of the union to use. */

    NEXUS_TransportType transportType;              /* The format of data on the parser band.
                                                       If the stream has timestamps, set NEXUS_InputBandSettings.packetLength to strip them.
                                                       If you want to record with new timestamps, see NEXUS_RecpumpSettings.timestampType. */
    bool continuityCountEnabled;                    /* If true, transport will check for correct continuity counters and discard bad packets. Default is true. */
    bool teiIgnoreEnabled;                          /* If true, transport will ignore TEI error bits and error input signals. This will allow bad packets to enter transport. Default is false. */
    NEXUS_CableCardType cableCard;                  /* Routing out to cable card */
    bool allPass;                                   /* If true, NEXUS_ParserBand_OpenPidChannel's pid param is ignored and the resulting pid channel can be used for all-pass record.
                                                       Also set acceptNullPackets to true if you want to capture the entire stream.
                                                       When opening the allPass pid channel, set NEXUS_PidChannelSettings.pidChannelIndex to the HwPidChannel obtained 
                                                       from NEXUS_ParserBand_GetAllPassPidChannelIndex(). */
    bool acceptNullPackets;                         /* If true and allPass is true, then all-pass record will include null packets. */

    unsigned maxDataRate;                           /* Maximum data rate for this parser band in units of bits per second. Default is typically 25000000 (i.e. 25 Mbps).
                                                       This is the maximum data rate of the sum of each pid being filtered (i.e. for each opened pid channel).
                                                       It is not the data rate of the original, unfiltered transport stream.
                                                       If you increase this max rate, you need to analyze total transport bandwidth and overall system bandwidth.
                                                       For 40nm platforms and beyond, this setting has no effect and is controlled via
                                                       NEXUS_TransportModuleSettings.maxDataRate.parserBand instead. */

    NEXUS_CallbackDesc ccError;                     /* Continuity Count Error (CC) - raised when continuity counter of next packet does not have the next counter value */
    NEXUS_CallbackDesc teiError;                    /* Transport Error Indicator (TEI) - error status from a demodulator */
    NEXUS_CallbackDesc lengthError;                 /* Transport Length Error - raised when a short transport packet is received.
                                                       Long transport packets are automatically truncated and do not raise this error. */
    bool acceptAdapt00;                             /* Packets with an adaptation field of 00 are accepted if true */
} NEXUS_ParserBandSettings;

/**
Summary:
Get current NEXUS_ParserBand settings.
**/
void NEXUS_ParserBand_GetSettings(
    NEXUS_ParserBand parserBand,
    NEXUS_ParserBandSettings *pSettings /* [out] */
    );

/**
Summary:
Set updated NEXUS_ParserBand settings.
**/
NEXUS_Error NEXUS_ParserBand_SetSettings(
    NEXUS_ParserBand parserBand,
    const NEXUS_ParserBandSettings *pSettings
    );

/**
Summary:
Open a parser band for exclusive use

Description:
NEXUS_ParserBand_Open is required for protected clients to exclusively use oa parser band.
**/
NEXUS_ParserBand NEXUS_ParserBand_Open( /* attr{destructor=NEXUS_ParserBand_Close}  */
    unsigned index /* Use a specific index or NEXUS_ANY_ID */
    );

/**
Summary:
Close a parser band opened with NEXUS_ParserBand_Open
**/
void NEXUS_ParserBand_Close(
    NEXUS_ParserBand parserBand
    );

/****************************************

parser band remapping support

****************************************/

/**
Summary:
One parser band remapping, used in NEXUS_ParserBandMapping.
**/
typedef struct NEXUS_ParserBandMap
{
    unsigned virtualParserBandNum;
    bool virtualParserIsPlayback;
} NEXUS_ParserBandMap;

/**
Summary:
System wide parser band remapping.
The index to frontEnd[] and playback[] is always the physical band number, even
after a remapping is done.
Any use of NEXUS_ParserBand in the rest of nexus will follow the virtual number, if set.
**/
typedef struct NEXUS_ParserBandMapping
{
    NEXUS_ParserBandMap frontend[NEXUS_MAX_PARSER_BANDS];
    NEXUS_ParserBandMap playback[NEXUS_MAX_PLAYPUMPS];
} NEXUS_ParserBandMapping;

/**
Summary:
Get current parser band remapping
**/
void NEXUS_GetParserBandMapping(
    NEXUS_ParserBandMapping *pParserMap /* [out] */
    );

/**
Summary:
Set new parser band remapping

Description:
Parser band remapping allowed parser band numbering (both frontend and playback parser bands)
to be virtualized.
**/
NEXUS_Error NEXUS_SetParserBandMapping(
    const NEXUS_ParserBandMapping *pParserMap
    );

/**
Summary:
Determine the PID channel that will be used in allPass mode. 
 
Description: 
Each parser will use a different PID channel when operating in 
allPass mode. This API provides the channel number for the given 
parser band. 
**/
NEXUS_Error NEXUS_ParserBand_GetAllPassPidChannelIndex(
    NEXUS_ParserBand parserBand, 
    unsigned *pHwPidChannel
    );

#ifdef __cplusplus
}
#endif

#endif
