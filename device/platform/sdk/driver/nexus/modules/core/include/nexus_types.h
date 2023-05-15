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
 * $brcm_Workfile: nexus_types.h $
 * $brcm_Revision: 79 $
 * $brcm_Date: 10/8/12 1:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/core/include/nexus_types.h $
 * 
 * 79   10/8/12 1:03p erickson
 * SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
 *  connections
 * 
 * 78   10/3/12 4:59p jtna
 * SW7425-3782: refactor nexus tsmf impl
 * 
 * 77   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 76   5/23/12 6:12p jgarrett
 * SW7425-3070: Adding NEXUS_AudioOutputPll Settings to control VCXO
 *  source
 * 
 * 75   5/10/12 6:16p jtna
 * SW7425-2752: rework MTSIF-frontend <-> host interface
 * 
 * 74   4/12/12 4:00p jgarrett
 * SW7425-2378: Adding Monkey's Audio and MLP
 * 
 * 73   4/9/12 3:50p jgarrett
 * SW7231-662: Adding AIFF support
 * 
 * 72   2/24/12 5:15p erickson
 * SW7425-1969: public API cannot #include priv header file
 * 
 * 71   2/7/12 7:34p jgarrett
 * SW7425-2136: Adding FLAC support
 * 
 * 70   2/1/12 2:22p erickson
 * SW7435-19: merge
 * 
 * SW7435-19/3   11/30/11 11:24a mward
 * SW7435-19:  Merge from main.
 * 
 * SW7435-19/2   11/21/11 4:00p mward
 * SW7435-19:  Merge from main.
 * 
 * SW7435-19/1   11/3/11 5:47p mward
 * SW7435-7:  Add NEXUS_Timebase_e8, NEXUS_Timebase_e9 for 7435.
 * 
 * 69   11/29/11 11:32a erickson
 * SW7420-2129: move definition of NEXUS_HeapHandle
 * 
 * 68   11/21/11 2:48p jgarrett
 * SW7425-1406: Adding Vorbis audio support
 * 
 * VORBIS_DEVEL/3   11/16/11 3:40p jgarrett
 * SW7425-1406: Merging latest main branch changes
 * 
 * VORBIS_DEVEL/2   10/6/11 5:51p jgarrett
 * SW7425-1406: Merge TSMF changes
 * 
 * 67   11/16/11 1:21p erickson
 * SW7125-1121: add NEXUS_MEMORY_TYPE_RESERVED for placeholder heaps that
 *  prevent allocations from certain regions
 * 
 * 66   11/7/11 3:59p bandrews
 * SW7231-446: merge to main
 * 
 * SW7231-446/1   11/3/11 7:55p bandrews
 * SW7231-446: implemented
 * 
 * 65   11/1/11 9:19a erickson
 * SW7231-391: extend NEXUS_Timebase to e7
 * 
 * 64   10/31/11 7:03p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/4   10/28/11 6:55p bandrews
 * SW7231-391: merge from main
 * 
 * 63   10/28/11 1:27p erickson
 * SW7346-523: extend NEXUS_TransportTimestamp to handle 32 bit timestamps
 *  and to be more explicit about 28/30 bit timestamps
 * 
 * SW7420-2078/3   10/27/11 7:47p bandrews
 * SW7231-391: merge from main
 * 
 * 62   10/25/11 11:36a vsilyaev
 * SWDTV-8388: Added NEXUS_TransportType_eOgg type
 * 
 * SW7420-2078/2   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * SW7420-2078/1   10/14/11 8:36p bandrews
 * SW7231-391: update stc channel to work with client timebase
 * 
 * 61   10/14/11 2:52p jtna
 * SW7425-1189: added NEXUS_TsmfSettings.frontendTsmfIndex. allow change
 *  between auto and semi-auto mode without enable/disable.
 * 
 * 60   10/5/11 12:22p jtna
 * SW7425-1189: add transport and frontend TSMF support
 * 
 * VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
 * SW7425-1406: Adding Vorbis support
 * 
 * 59   9/7/11 3:33p erickson
 * SW7346-419: add more NEXUS_InputBand values
 * 
 * 58   9/2/11 4:08p erickson
 * SWDEPRECATED-2425: update comments
 * 
 * 57   7/12/11 5:02p erickson
 * SW7425-881: add up to 16 parser bands
 * 
 * 56   5/11/11 10:52a erickson
 * SW7401-4501: add NEXUS_PixelFormat_eL8
 * 
 * 55   4/25/11 1:52p erickson
 * SW7630-104: Added support for A8_Y8, Cb8 and Cr8 formats.
 * 
 * 54   4/20/11 6:41p jhaberf
 * SWDTV-6702: Added an identifier for total input bands of 9
 * 
 * 53   3/10/11 4:09p erickson
 * SWDEPRECATED-2425: clarify comment
 *
 * 52   3/2/11 4:20p erickson
 * SW7405-3671: add three pixel formats
 *
 * 51   2/14/11 12:29p jhaberf
 * SW35230-2942: Added macros for checking nexus pixel format type
 *
 * 50   2/8/11 5:15p vsilyaev
 * SW7425-30: Removed unused types
 *
 * 49   1/27/11 10:26a erickson
 * SW7422-2: clarify NEXUS_MemoryType macros
 *
 * 48   1/21/11 3:11p erickson
 * SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
 *  communication
 *
 * 47   12/30/10 7:40p vsilyaev
 * SW7425-39: Removed special timestamp mode
 *
 * 46   12/30/10 6:42p vsilyaev
 * SW7425-39: Use special timestamp mode for mux
 *
 * 45   12/20/10 2:11p erickson
 * SW7422-2: redefine NEXUS_MemoryType to be bitmask for different memory
 *  mapping requirements per heap, remove deprecated api's
 *
 * 44   12/13/10 11:59a vsilyaev
 * SW7425-40: Added new types for the video encoder
 *
 * 43   12/9/10 12:00p erickson
 * SW35230-2492: move NEXUS_ClipRect to nexus_types.h
 *
 * 42   12/7/10 5:43p erickson
 * SW7420-1148: add NEXUS_Certificate
 *
 * 41   11/15/10 3:51p erickson
 * SW7342-276: added NEXUS_VbiDataType, extended NEXUS_ClosedCaptionData
 *  to support PTS and DSS subtitles
 *
 * 40   11/12/10 10:08a erickson
 * SW7422-20: added NEXUS_ParserBand_e6
 *
 * 39   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 38   9/8/10 11:44a vsilyaev
 * SW7468-129: Added RMFF type
 *
 * 37   8/19/10 9:39a erickson
 * SW7405-3671: fix NEXUS_PIXEL_FORMAT_IS_PALETTE
 *
 * 36   8/16/10 11:02a erickson
 * SW7405-3671: expand and rearrange NEXUS_PixelFormat for compatibility
 *  with M2MC packet blit
 *
 * 35   8/4/10 3:19p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 *
 * SW7405-4325/1   7/2/10 7:23p mphillip
 * SW7405-4325: Move keyslot allocation/deallocation to base to break DMA
 *  dependency
 *
 * 34   5/6/10 3:45p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 *  container
 *
 * 33   3/3/10 10:03a erickson
 * SW35230-109: add BGR pixel formats
 *
 * 32   2/10/10 2:36p erickson
 * SW3548-2778: remove unused NEXUS_PixelFormat
 *
 * 31   1/6/10 11:35a erickson
 * SW3556-958: clarify usage of NEXUS_TransportTimestampType_eMod300 and
 *  eBinary
 *
 * 30   12/29/09 2:47p erickson
 * SW7420-515: remove NEXUS_PixelFormat_eA0 and
 *  NEXUS_PixelFormat_eVideoTunnel
 *
 * 29   12/14/09 11:44a mphillip
 * SW7405-3471: Merge additional pixel format support to main
 *
 * DirectFB_1_4_Port/2   12/14/09 3:02p robertwm
 * SW7405-3471: Nexus needs to support additional pixel formats for
 *  DirectFB.
 *
 * DirectFB_1_4_Port/1   11/24/09 7:11p robertwm
 * SW7405-3471: Nexus needs to support additional pixel formats for
 *  DirectFB.
 *
 * 27   8/17/09 10:33a erickson
 * PR57745: add NEXUS_ParserBand_e6
 *
 * 26   8/14/09 7:16p jrubio
 * PR55232: Input Band Max 7 for 7342
 *
 * 25   7/15/09 7:24p vsilyaev
 * PR 55653: Added WAV format
 *
 * 24   5/8/09 3:45p erickson
 * PR35457: added NEXUS_PidType_eOther for clarity of usage
 *
 * 23   4/17/09 9:30a erickson
 * PR54304: add NEXUS_PixelFormat_eR8_G8_B8_A8
 *
 * 22   3/16/09 11:06a erickson
 * PR53248: add NEXUS_ClosedCaptionData.parityError
 *
 * 21   3/9/09 2:31p erickson
 * PR50748: added NEXUS_MemoryType
 *
 * 20   3/5/09 2:49p erickson
 * PR52674: added comment re: NEXUS_ClosedCaptionData.noData
 *
 * 19   2/17/09 4:27p erickson
 * PR52115: added NEXUS_ClosedCaptionData.noData boolean
 *
 * 18   1/26/09 12:46p vsilyaev
 * PR 51579: Added NEXUS_TransportType_eUnknown type
 *
 * 17   10/31/08 12:00p erickson
 * PR47030: readd additionalLines
 *
 * 16   10/30/08 8:48p erickson
 * PR47030: added NEXUS_HeapHandle and related settings
 *
 * 15   7/14/08 4:10p vsilyaev
 * PR 44795: Removed duplicate types
 *
 * 14   6/17/08 7:19p vsilyaev
 * PR 40870: Fixed YCrCb test
 *
 * 13   6/17/08 5:21p vsilyaev
 * PR 40870: Added YUV 4:4:4 pixel formats
 *
 * 12   6/17/08 12:30a mphillip
 * PR40027: Global keyslot handle type
 *
 * 11   5/2/08 12:34p erickson
 * PR35457: fixed NEXUS_PIXEL_FORMAT_IS_YCRCB
 *
 * 10   4/30/08 10:52a erickson
 * PR35457: improve Surface comments. move general items to nexus_types.h.
 *
 * 9   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 8   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 7   4/1/08 3:04p erickson
 * PR40567: update and merge
 *
 * PR40567/2   4/1/08 12:06p xhuang
 * PR40567:add comments for YCbCr format define
 *
 * PR40567/1   3/26/08 6:22p dyzhang
 * PR40567: move NEXUS_PixelFormat from nexus_surface.h to nexus_types.h
 *
 * 5   3/6/08 10:46a erickson
 * PR40351: moved NEXUS_NORMAL_DECODE_RATE from audio/video to base
 *
 * 4   3/3/08 2:03p vsilyaev
 * PR 39818: Support of MKV format
 *
 * 3   1/30/08 4:59p erickson
 * PR36808: added NEXUS_Pixel
 *
 * 2   1/28/08 9:41a erickson
 * PR35457: comment update
 *
 * 1   1/18/08 2:12p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/15   11/28/07 10:37a erickson
 * PR35457: doc update
 *
 * Nexus_Devel/14   11/19/07 4:53p erickson
 * PR34925: added more parser bands
 *
 * Nexus_Devel/13   11/1/07 9:04a erickson
 * PR36633: rework enums to naming convention
 *
 * Nexus_Devel/12   10/9/07 3:23p vsilyaev
 * PR 35824: Added VOB stream type
 *
 * Nexus_Devel/11   10/9/07 12:32p vsilyaev
 * PR 35824: Fixed naming conventions
 *
 * Nexus_Devel/10   10/9/07 12:16p vsilyaev
 * PR 35824: Added NEXUS_PidType
 *
 * Nexus_Devel/9   10/8/07 5:39p vsilyaev
 * PR 35824: Renamed MPEG1 PES to MPEG1 PS (Program Stream)
 *
 * Nexus_Devel/8   9/28/07 2:04p erickson
 * PR34925: remove NEXUS_InputBand_ePlayback. added comment on
 * ParserBand_ePlayback.
 *
 * Nexus_Devel/7   9/26/07 10:59a erickson
 * PR34926: added NEXUS_PtsType, other comments
 *
 * Nexus_Devel/6   9/21/07 11:17a erickson
 * PR34925: add temp CableCardType
 *
 * Nexus_Devel/5   9/14/07 4:16p erickson
 * PR34925: transport update
 *
 * Nexus_Devel/4   9/12/07 1:56p vsilyaev
 * PR 34419: Base audio and video types
 *
 * Nexus_Devel/3   8/24/07 11:17a erickson
 * PR34254: added types
 *
 * Nexus_Devel/2   8/23/07 5:50p jgarrett
 * PR 34254: Adding Timebase
 *
 * Nexus_Devel/1   8/23/07 5:12p erickson
 * PR34254: initial checkin
 *
 **************************************************************************/
#ifndef NEXUS_TYPES_H__
#define NEXUS_TYPES_H__

#include "nexus_base_types.h"
#include "nexus_audio_types.h"
#include "nexus_video_types.h"
#include "nexus_security_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
An InputBand is a digital input to the transport block.
It takes data from outside the chip and routes it to a parser band.

Description:
See Also:
NEXUS_InputBand_SetSettings
**/
typedef unsigned NEXUS_InputBand;
#define NEXUS_InputBand_e0 (0)
#define NEXUS_InputBand_e1 (1)
#define NEXUS_InputBand_e2 (2)
#define NEXUS_InputBand_e3 (3)
#define NEXUS_InputBand_e4 (4)
#define NEXUS_InputBand_e5 (5)
#define NEXUS_InputBand_e6 (6)
#define NEXUS_InputBand_e7 (7)
#define NEXUS_InputBand_e8 (8)
#define NEXUS_InputBand_e9 (9)
#define NEXUS_InputBand_e10 (10)
#define NEXUS_InputBand_e11 (11)
#define NEXUS_InputBand_e12 (12)
#define NEXUS_InputBand_e13 (13)
#define NEXUS_InputBand_e14 (14)
#define NEXUS_InputBand_e15 (15)
#define NEXUS_InputBand_eMax NEXUS_NUM_INPUT_BANDS

/**
Summary:
An ParserBand is a digital input to the transport block
which takes data from an input band and routes it to various PidChannels.

Description:
Nexus supports playback parser bands through the Playpump and Playback interfaces.

See Also:
NEXUS_ParserBand_SetSettings
**/
typedef enum {
    NEXUS_ParserBand_eInvalid = -1, 
    NEXUS_ParserBand_e0 = 0,
    NEXUS_ParserBand_e1,
    NEXUS_ParserBand_e2,
    NEXUS_ParserBand_e3,
    NEXUS_ParserBand_e4,
    NEXUS_ParserBand_e5,
    NEXUS_ParserBand_e6,
    NEXUS_ParserBand_e7,
    NEXUS_ParserBand_e8,
    NEXUS_ParserBand_e9,
    NEXUS_ParserBand_e10,
    NEXUS_ParserBand_e11,
    NEXUS_ParserBand_e12,
    NEXUS_ParserBand_e13,
    NEXUS_ParserBand_e14,
    NEXUS_ParserBand_e15,
    NEXUS_ParserBand_eMax
} NEXUS_ParserBand;

/**
Summary:
The transport format, or stream type, of digital data.

Description:
This includes a variety of container classes and standard stream muxes.

See Also:
NEXUS_PidChannelSettings
NEXUS_StcChannelSettings
**/
typedef enum NEXUS_TransportType {
    NEXUS_TransportType_eUnknown,   /* Unknown stream format */
    NEXUS_TransportType_eEs,        /* Elementary stream. No container or muxing. */
    NEXUS_TransportType_eTs,        /* MPEG2 transport stream */
    NEXUS_TransportType_eMpeg2Pes,  /* MPEG2 packetized elementary stream, this includes MPEG2 Program Stream  streams */
    NEXUS_TransportType_eVob,       /* DVD VOB, this is subset of MPEG2 Program Stream, special processing is applied for VOB streams */
    NEXUS_TransportType_eMpeg1Ps,   /* MPEG1 program stream */
    NEXUS_TransportType_eDssEs,     /* DSS with ES payload (used for SD) */
    NEXUS_TransportType_eDssPes,    /* DSS with PES payload (used for HD) */
    NEXUS_TransportType_eAsf,       /* Advanced Systems Format */
    NEXUS_TransportType_eAvi,       /* Audio Video Interleave */
    NEXUS_TransportType_eMp4,       /* MP4 (MPEG-4 Part12) container */
    NEXUS_TransportType_eFlv,       /* Flash video container */
    NEXUS_TransportType_eMkv,       /* Matroska container */
    NEXUS_TransportType_eWav,       /* WAVE audio container */
    NEXUS_TransportType_eMp4Fragment,  /* separate 'moof' boxes from the MP4 (MPEG-4 Part12) container */
    NEXUS_TransportType_eRmff,       /* RMFF container */
    NEXUS_TransportType_eOgg,        /* OGG container */
    NEXUS_TransportType_eFlac,       /* FLAC encapsulation */
    NEXUS_TransportType_eAiff,       /* AIFF (Audio Interchange File Format) container */
    NEXUS_TransportType_eApe,        /* Monkey's Audio container */
    NEXUS_TransportType_eMax
} NEXUS_TransportType;

/**
Summary:
A transport timestamp is a value prepended to every transport packet for pacing control.

Description:
See Also:
NEXUS_ParserBandSettings
NEXUS_PlaypumpSettings
**/
typedef enum NEXUS_TransportTimestampType {
    NEXUS_TransportTimestampType_eNone,   /* No timestamp is prepended to the transport packets. */

    NEXUS_TransportTimestampType_e30_2U_Mod300, /* 30 bit timestamp, mod-300 value, 2 user bits. no parity bits.
                                             A 4-byte 27MHz timestamp is prepended to every transport packet.
                                             Used for MPEG2TS streams (188 byte packets).
                                             The 27Mhz timestamp is divided by 300 to convert to a 90Hz timestamp before use 
                                             because MPEG2TS PTS/PCR units are 90Khz. */
    NEXUS_TransportTimestampType_eMod300 = NEXUS_TransportTimestampType_e30_2U_Mod300, /* alias */

    NEXUS_TransportTimestampType_e30_2U_Binary, /* 30 bit timestamp, binary value, 2 user bits. no parity bits.
                                             A 4-byte 27MHz timestamp is prepended to every transport packet.
                                             Used for DSS streams (130 byte packets).
                                             The 27Mz timestamp is used unmodified because DSS PTS/PCR units are 27Mhz. */
    NEXUS_TransportTimestampType_eBinary = NEXUS_TransportTimestampType_e30_2U_Binary, /* alias */

    NEXUS_TransportTimestampType_e32_Mod300, /* 32 bit timestamp, mod-300 value, no user bits. no parity bits. */
    NEXUS_TransportTimestampType_e32_Binary, /* 32 bit timestamp, binary value, no user bits. no parity bits. */
    /* e28_4P_Mod300 is not used. if needed, it would be 28 bit timestamp, mod-300 value, 4 parity bits. */
    /* e28_4P_Binary is not supported */
    NEXUS_TransportTimestampType_eMax
} NEXUS_TransportTimestampType;

/**
#if defined(HUMAX_PLATFORM_BASE) merged from URSR14.1
Summary:
Maximum number of MTSIF channels, for either backend or frontend.

Description:
This is the larger of the number of MTSIF channels on either the backend or frontend chip.

It was moved here because it is used both by the transport module and the frontend module.
**/
#define NEXUS_MAX_MTSIF        4

/**
Summary:
A Timebase provides a clock for various cores like decoders, display and mixers.

Description:
See Also:
NEXUS_Timebase_SetSettings
**/
typedef enum 
{
    NEXUS_Timebase_eInvalid=-1, 
    NEXUS_Timebase_e0 = 0,
    NEXUS_Timebase_e1,
    NEXUS_Timebase_e2,
    NEXUS_Timebase_e3,
    NEXUS_Timebase_e4,
    NEXUS_Timebase_e5,
    NEXUS_Timebase_e6,
    NEXUS_Timebase_e7,
    NEXUS_Timebase_e8,
    NEXUS_Timebase_e9,
    NEXUS_Timebase_eMax
} NEXUS_Timebase;

/**
Summary:
Handle for a frontend connection
**/
typedef struct NEXUS_FrontendConnector *NEXUS_FrontendConnectorHandle;

/*
Summary:
Handle which represents a remux channel.
*/
typedef struct NEXUS_Remux *NEXUS_RemuxHandle;

/*
Summary:
Handle which represents a TSMF channel.
*/
typedef struct NEXUS_Tsmf *NEXUS_TsmfHandle;

/**
Summary:
Cable card type
**/
typedef enum NEXUS_CableCardType
{
    NEXUS_CableCardType_eNone,
    NEXUS_CableCardType_eSCard,
    NEXUS_CableCardType_eMCard
} NEXUS_CableCardType;

/**
Summary:
Describes the origin of a PTS value.

Description:
See Also:
NEXUS_VideoDecoderStatus
NEXUS_AudioDecoderStatus
**/
typedef enum NEXUS_PtsType
{
    NEXUS_PtsType_eCoded,                     /* PTS value came from the stream. */
    NEXUS_PtsType_eInterpolatedFromValidPTS,  /* PTS has been interpolated over time from a valid (i.e. coded) PTS from the stream. */
    NEXUS_PtsType_eHostProgrammedPTS,         /* PTS was set by the application and did not come from the stream. */
    NEXUS_PtsType_eInterpolatedFromInvalidPTS /* PTS has been interpolated over time from an invalid value. */
} NEXUS_PtsType;

/**
Summary:
Describes type of data in a PID.

Description:
See Also:
NEXUS_RecpumpAddPidChannelSettings
NEXUS_PlaypumpAddPidChannelSettings
**/
typedef enum NEXUS_PidType {
    NEXUS_PidType_eVideo,            /* Used to indicate video stream */
    NEXUS_PidType_eAudio,            /* Used to indicate audio stream */
    NEXUS_PidType_eOther,            /* Used to indicate stream type different from above. Useful for PCR, PSI or other non-audio/video data. */
    NEXUS_PidType_eUnknown = NEXUS_PidType_eOther /* Unknown pid type */
} NEXUS_PidType;

/*
Summary:
For non-palettized surfaces, NEXUS_Pixel is an ARGB8888 pixel.
When used, it will be converted into the desired pixel for a surface.
For palettized surfaces, this is the palette index plus possible alpha value.

Description:
For palette indexes, values should be packed into the least significant bits
of NEXUS_Pixel. For instance, a8_palette8 uses byte[0] for the palette8 index,
byte[1] for alpha8 value and ignores byte[2] and byte[3].
palette2 uses bits 0 and 1 of byte[0] and ignores the rest.

Used in NEXUS_SurfaceMemory, NEXUS_SurfaceSettings, NEXUS_DisplaySettings, and NEXUS_GraphicsSettings.
*/
typedef uint32_t NEXUS_Pixel;

/**
Summary:
Base rate for normal decode used to specify trick mode rates.

Description:
This is used for both audio and video trick modes.
See NEXUS_VideoDecoderTrickState and NEXUS_AudioDecoderTrickState.
See comments in NEXUS_VideoDecoderTrickState.rate for how to use a multiple or fraction of this value to specify trick mode rates.
**/
#define NEXUS_NORMAL_DECODE_RATE 1000

/*
Summary:
Pixel format of a graphics surface or video buffer

Description:
The enum name describes the pixel format:

o A = alpha channel
o X = block out alpha channel
o R = red component of color
o G = green component of color
o B = blue component of color

YCbCr444 means each sample has a Y, a Cb, and a Cr value, so Y:Cb:Cr = 1:1:1 in stream.
YCbCr422 means for every two horizontal Y samples, there is one Cb and Cr sample, so Y:Cb:Cr = 2:1:1 in stream.
10-bit means each Y/Cb/Cr value is 10 bits, 8-bit means each Y/Cb/Cr value is 8 bits.
For example,
YCbCr444 10bit, each sample has Y(10bit)+Cb(10bit)+Cr(10bit) = 30 bits
YCbCr422 8bit, each sample has either Y(8bit)+Cb(8bit)=16bit or Y(8bit)+Cr(8bit) = 16 bits

R8 G8 B8 means 8 bits of red component (R), 8 bits of green component (G) and 8 bits of blue component (B) that are 24-bit packed.
X8 R8 G8 B8 means 8 bits of nothing (X), 8 bits of red component (R), 8 bits of green component (G) and 8 bits of blue component (B)
that occupy a 32-bit word.

These values match BM2MC_PACKET_PixelFormat. They do not match BPXL_Format.

See Also:
NEXUS_SurfaceCreateSettings
*/
typedef enum NEXUS_PixelFormat
{
    NEXUS_PixelFormat_eUnknown = 0,

    NEXUS_PixelFormat_eR5_G6_B5,         /* 16-bit, no per-pixel alpha */
    NEXUS_PixelFormat_eB5_G6_R5,         /* 16-bit, no per-pixel alpha */

    NEXUS_PixelFormat_eA1_R5_G5_B5,      /* 16-bit */
    NEXUS_PixelFormat_eX1_R5_G5_B5,      /* 16-bit */
    NEXUS_PixelFormat_eA1_B5_G5_R5,      /* 16-bit */
    NEXUS_PixelFormat_eX1_B5_G5_R5,      /* 16-bit */
    NEXUS_PixelFormat_eR5_G5_B5_A1,      /* 16-bit */
    NEXUS_PixelFormat_eR5_G5_B5_X1,      /* 16-bit */
    NEXUS_PixelFormat_eB5_G5_R5_A1,      /* 16-bit */
    NEXUS_PixelFormat_eB5_G5_R5_X1,      /* 16-bit */

    NEXUS_PixelFormat_eA4_R4_G4_B4,      /* 16-bit */
    NEXUS_PixelFormat_eX4_R4_G4_B4,      /* 16-bit */
    NEXUS_PixelFormat_eA4_B4_G4_R4,      /* 16-bit */
    NEXUS_PixelFormat_eX4_B4_G4_R4,      /* 16-bit */
    NEXUS_PixelFormat_eR4_G4_B4_A4,      /* 16-bit */
    NEXUS_PixelFormat_eR4_G4_B4_X4,      /* 16-bit */
    NEXUS_PixelFormat_eB4_G4_R4_A4,      /* 16-bit */
    NEXUS_PixelFormat_eB4_G4_R4_X4,      /* 16-bit */

    NEXUS_PixelFormat_eA8_R8_G8_B8,      /* 32-bit */
    NEXUS_PixelFormat_eX8_R8_G8_B8,      /* 32-bit */
    NEXUS_PixelFormat_eA8_B8_G8_R8,      /* 32-bit */
    NEXUS_PixelFormat_eX8_B8_G8_R8,      /* 32-bit */
    NEXUS_PixelFormat_eR8_G8_B8_A8,      /* 32-bit */
    NEXUS_PixelFormat_eR8_G8_B8_X8,      /* 32-bit */
    NEXUS_PixelFormat_eB8_G8_R8_A8,      /* 32-bit */
    NEXUS_PixelFormat_eB8_G8_R8_X8,      /* 32-bit */

    NEXUS_PixelFormat_eA8,               /* 8-bit alpha-only surface (constant color) */
    NEXUS_PixelFormat_eA4,               /* 4-bit alpha-only surface (constant color) */
    NEXUS_PixelFormat_eA2,               /* 2-bit alpha-only surface (constant color) */
    NEXUS_PixelFormat_eA1,               /* 1-bit alpha-only surface (constant color) */

    NEXUS_PixelFormat_eW1,

    NEXUS_PixelFormat_eA8_Palette8,      /* A8 and 8-bit Palette index */
    NEXUS_PixelFormat_ePalette8,         /* 8-bit Palette index */
    NEXUS_PixelFormat_ePalette4,         /* 4-bit palette index */
    NEXUS_PixelFormat_ePalette2,         /* 2-bit palette index */
    NEXUS_PixelFormat_ePalette1,         /* 1-bit palette index */

    NEXUS_PixelFormat_eY8_Palette8,      /* Y8 and 8-bit Palette index */
    NEXUS_PixelFormat_eY8,               /* Y8 */
    NEXUS_PixelFormat_eA8_Y8,            /* 8-bit alpha and 8-bit luma, for YCbCr420 */

    NEXUS_PixelFormat_eCb8_Cr8,
    NEXUS_PixelFormat_eCr8_Cb8,
    NEXUS_PixelFormat_eCb8,                   /* 8-bit chroma-only (Cb) */
    NEXUS_PixelFormat_eCr8,                   /* 8-bit chroma-only (Cr) */

    NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eY08_Cr8_Y18_Cb8,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eCb8_Y08_Cr8_Y18,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eCb8_Y18_Cr8_Y08,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
    NEXUS_PixelFormat_eCr8_Y08_Cb8_Y18,  /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */

    NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10, /* 32-bit for 1 pixel,  YCbCr444 w/ 10-bit values */

    NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8, /* YCbCr444 w/ 8-bit values */
    NEXUS_PixelFormat_eA8_Cr8_Cb8_Y8, /* YCbCr444 w/ 8-bit values */
    NEXUS_PixelFormat_eCr8_Cb8_Y8_A8, /* YCbCr444 w/ 8-bit values */
    NEXUS_PixelFormat_eY8_Cb8_Cr8_A8, /* YCbCr444 w/ 8-bit values */

    NEXUS_PixelFormat_eY010_Cb10_Y110_Cr10,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eY010_Cr10_Y110_Cb10,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eY110_Cb10_Y010_Cr10,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eY110_Cr10_Y010_Cb10,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eCb10_Y010_Cr10_Y110,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eCb10_Y110_Cr10_Y010,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eCr10_Y110_Cb10_Y010,   /* YCbCr422 w/ 10-bit values */
    NEXUS_PixelFormat_eCr10_Y010_Cb10_Y110,   /* YCbCr422 w/ 10-bit values */

    NEXUS_PixelFormat_eL8,                    /* 8-bit luma */
    NEXUS_PixelFormat_eL4_A4,                 /* 4-bit luma and 4-bit alpha */
    NEXUS_PixelFormat_eL8_A8,                 /* 8-bit luma and 8-bit alpha */
    NEXUS_PixelFormat_eL15_L05_A6,            /* 5-bit luma1, 5-bit luma0, 6-bit alpha */

    /* The following pixel formats are not supported by NEXUS_Graphics2D */
    NEXUS_PixelFormat_eR8_G8_B8,         /* 24-bit packet */
    NEXUS_PixelFormat_eYCbCr422_10bit,

    NEXUS_PixelFormat_eMax
} NEXUS_PixelFormat;

/**
Summary:
Palette test macro for NEXUS_PixelFormat type
**/
#define NEXUS_PIXEL_FORMAT_IS_PALETTE(pixelformat) \
     ((pixelformat) >= NEXUS_PixelFormat_eA8_Palette8 && (pixelformat) <= NEXUS_PixelFormat_eY8_Palette8)

/**
Summary:
YCrCb test macro for NEXUS_PixelFormat type
**/
#define NEXUS_PIXEL_FORMAT_IS_YCRCB(pixelformat) \
    (((pixelformat) >= NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8 && (pixelformat) <= NEXUS_PixelFormat_eCr10_Y010_Cb10_Y110) || \
     ((pixelformat)== NEXUS_PixelFormat_eYCbCr422_10bit))

/**
Summary:
422 format test macro for NEXUS_PixelFormat type
**/
#define NEXUS_PIXEL_FORMAT_IS_422(pixelFormat) \
    ((NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8 <= pixelFormat && \
      NEXUS_PixelFormat_eCr8_Y08_Cb8_Y18 >= pixelFormat) || \
     (NEXUS_PixelFormat_eY010_Cb10_Y110_Cr10 <= pixelFormat && \
      NEXUS_PixelFormat_eCr10_Y010_Cb10_Y110 >= pixelFormat) || \
     (NEXUS_PixelFormat_eYCbCr422_10bit == pixelFormat))

/**
Summary:
444 format test macro for NEXUS_PixelFormat type
**/
#define NEXUS_PIXEL_FORMAT_IS_444(pixelFormat) \
    (NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10 <= pixelFormat && \
     NEXUS_PixelFormat_eY8_Cb8_Cr8_A8    >= pixelFormat)

/***************************************************************************
Summary:
Buffer count and pixel types per buffer type (fullHd, hd or sd) used in NEXUS_DisplayHeapSettings and NEXUS_DisplayModuleSettings
***************************************************************************/
typedef struct NEXUS_DisplayBufferTypeSettings
{
    unsigned count;                 /* Number of full-sized buffers to allocate */
    unsigned pipCount;              /* Number of PIP-sized buffers to allocate */
    NEXUS_PixelFormat pixelFormat;  /* Pixel format of the buffers */
    NEXUS_VideoFormat format;       /* Max-sized format of the buffer (e.g. 1080i or 1080p? 480 or 576?) */
    unsigned additionalLines;
} NEXUS_DisplayBufferTypeSettings;

/***************************************************************************
Summary:
Additional properties for Display module heap allocation

Description:
When the Display module does video buffer allocation, it needs to manage those allocations
in fullHD (i.e. 2HD), HD and SD sizes. This is needed to prevent fragmentation for highly dynamic
buffer allocation schemes.

If a heap is not used by the Display module, these settings are not used.

If a heap is used by the Display module in either NEXUS_VideoWindowSettings.heapIndex or NEXUS_VideoInputSettings.heapIndex,
the heap cannot also be used by any other part of Nexus.
The Display module will overlay its internal display heap over the whole heap.
***************************************************************************/
typedef struct NEXUS_DisplayHeapSettings
{
    NEXUS_DisplayBufferTypeSettings fullHdBuffers; /* double-sized HD buffers used for 1080p output */
    NEXUS_DisplayBufferTypeSettings hdBuffers;     /* HD buffers used for 1080i or 720p output */
    NEXUS_DisplayBufferTypeSettings sdBuffers;     /* SD buffers used for 480i/576i output */
} NEXUS_DisplayHeapSettings;

/**
Summary:
General purpose rectangle
**/
typedef struct NEXUS_Rect
{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} NEXUS_Rect;

/**
Summary:
Relative clip rectangle

Description:
Values are relative, specified in units of 1/100%.  For example, if you want 5%
clipping on all sides, use the value 500 in all fields.

Structure can be used with other units (e.g. pixels), but it should be explicitly
commented so where used and not here.
**/
typedef struct NEXUS_ClipRect
{
    unsigned left;
    unsigned right;
    unsigned top;
    unsigned bottom;
} NEXUS_ClipRect;

/**
Summary:
VBI data type enum

Description:
NEXUS_VbiDataType is general purpose and could be used in a variety of ways in different Nexus. It should not be modified for chip-specific use.

See NEXUS_ClosedCaptionData in this file as well as nexus_video_input_vbi.h and nexus_display_vbi.h for
other VBI-related API's.
**/
typedef enum NEXUS_VbiDataType {
    NEXUS_VbiDataType_eClosedCaption,
    NEXUS_VbiDataType_eSubtitle,
    NEXUS_VbiDataType_eTeletext,
    NEXUS_VbiDataType_eVps,
    NEXUS_VbiDataType_eGemStar,
    NEXUS_VbiDataType_eWss,
    NEXUS_VbiDataType_eCgms,
    NEXUS_VbiDataType_eMax
} NEXUS_VbiDataType;

/**
Summary:
One field of EIA-608 or EIA-708 closed caption data

Description:
See NEXUS_VideoInput_ReadClosedCaption and NEXUS_Display_WriteClosedCaption
**/
typedef struct NEXUS_ClosedCaptionData
{
    NEXUS_VbiDataType type; /* Can be eClosedCaption or eSubtitle. eSubtitle is only supported for DSS subtitles.
                               If eSubtitle, then 'field' is 'languageType'. */
    uint32_t pts;   /* only set if data comes from a digital source */
    uint16_t field; /* If 0/1, this is EIA-608. 0 = top field, 1 = bottom field.
                       If 2/3, this is EIA-708, 2 = DTVCC_PACKET_DATA cc_type, 3 = DTVCC_PACKET_START cc_type */
    bool noData;    /* If true, data[] contains no valid data.
                       For 608 sources, this is reported by the VBI decoder.
                       For 708 sources, noData is the negation of userdata's cc_valid field. Therefore, do not discard an entry
                         if noData=true. Instead, set your parser's cc_valid=!noData and pass the data along. */
    bool parityError;  /* If true, data[] contains a parity error. Parity is the 8th bit of each data[] element and can be computed using the lower 7 bits of data. */
    uint8_t data[2];
} NEXUS_ClosedCaptionData;


/* NEXUS_MEMORY_TYPE macros are used to control memory mapping.
Memory mapping requirements depend on the type of access in the driver and application (for kernel mode,
the driver is in the kernel and the application is in user space). */
#define NEXUS_MEMORY_TYPE_DRIVER_UNCACHED       0x0001
#define NEXUS_MEMORY_TYPE_DRIVER_CACHED         0x0002
#define NEXUS_MEMORY_TYPE_APPLICATION_CACHED    0x0004
#define NEXUS_MEMORY_TYPE_SECURE                0x0008

/* RESERVED = nexus heap is placeholder. no underlying BMEM heap created, so no allocations allowed. */
#define NEXUS_MEMORY_TYPE_RESERVED              0x0010 

/* NEXUS_MemoryType macros are used to assemble commonly used NEXUS_MEMORY_TYPE combinations */

/* cached and uncached CPU access from the driver only.
note that some driver code may only require cached or only required uncached.
the minimal memory mapping is preferred if it is known. */
#define NEXUS_MemoryType_eDriver        (NEXUS_MEMORY_TYPE_DRIVER_UNCACHED|NEXUS_MEMORY_TYPE_DRIVER_CACHED)

/* cached CPU access in application (user space). no CPU access in the driver. no uncached access in application.
can be used for graphics surfaces, record buffers, etc.
HW access is still allowed with an eApplication heap. for instance, you can allocate a graphics framebuffer from
an eApplication heap because only the app needs CPU access; the driver simply needs to program the offset to the GFD. */
#define NEXUS_MemoryType_eApplication   (NEXUS_MEMORY_TYPE_APPLICATION_CACHED)

/* full CPU access from driver and application */
#define NEXUS_MemoryType_eFull          (NEXUS_MEMORY_TYPE_DRIVER_UNCACHED|NEXUS_MEMORY_TYPE_DRIVER_CACHED|NEXUS_MEMORY_TYPE_APPLICATION_CACHED)

/* accessible by HW devices only. no CPU access in driver or application; therefore no memory
mapping required. */
#define NEXUS_MemoryType_eDeviceOnly    (0x0)

/* restricted access heap for secure processor. limited device access and no CPU access. */
#define NEXUS_MemoryType_eSecure        (NEXUS_MEMORY_TYPE_SECURE)

/* NEXUS_MemoryType_eDevice is deprecated */
#define NEXUS_MemoryType_eDevice NEXUS_MemoryType_eFull

typedef unsigned NEXUS_MemoryType;

/***************************************************************************
Summary:
Autheticated ID for client/server communication.

Description:
For non-secure environments, the certificate could be a simple ID.
For secure environments, this should use encrypted using standard public-key cryptography.
***************************************************************************/
typedef struct NEXUS_Certificate
{
    unsigned char data[256];
    unsigned length;
} NEXUS_Certificate;

/***************************************************************************
Summary:
Modes supported for accepting TSMF packets.

Description:
This enum represents the modes that TSMF supports when there is a change for
the version number field inside a frame header.
***************************************************************************/
typedef enum NEXUS_TsmfVersionChangeMode
{
    NEXUS_TsmfVersionChangeMode_eAllFrame,       /* Accept all Frame Header packets */
    NEXUS_TsmfVersionChangeMode_eFrameChangeVer, /* Accept only Frame Header packets that has a change in version number field */
    NEXUS_TsmfVersionChangeMode_eMax
} NEXUS_TsmfVersionChangeMode;

/***************************************************************************
Summary:
Structure for field verification configuration of a TSMF module

Description:
This structure represents configurations for enabling/disabling of certain
field verification of a TSMF module.
***************************************************************************/
typedef struct NEXUS_TsmfFieldVerifyConfig
{
    bool crcCheckDisable;         /* TSMF Frame Header CRC check */
    bool relTsStatusCheckDisable; /* TSMF Frame Header Rel_TS_Status check */
    bool frameTypeCheckDisable;   /* TSMF Frame Header Frame_Type check */
    bool relTsModeCheckDisable;   /* TSMF Frame Header REL_TS_MODE check */
    bool syncCheckDisable;        /* TSMF Frame Header SYNC check */
    bool ccCheckDisable;          /* TSMF Frame Header CC check */
    bool adapCheckDisable;        /* TSMF Frame Header ADAP check */
    bool scCheckDisable;          /* TSMF Frame Header SC check */
    bool tsPriorCheckDisable;     /* TSMF Frame Header TS_PRIOR check */
    bool pusiCheckDisable;        /* TSMF Frame Header PUSI check */
    bool teiCheckDisable;         /* TSMF Frame Header TEI check */
    NEXUS_TsmfVersionChangeMode versionChangeMode;
} NEXUS_TsmfFieldVerifyConfig;

/***************************************************************************
Description:
Used in NEXUS_TsmfSettings to specify the type of input to the TSMF.
***************************************************************************/
typedef enum NEXUS_TsmfSourceType
{
    NEXUS_TsmfSourceType_eInputBand,
    NEXUS_TsmfSourceType_eRemux,
    NEXUS_TsmfSourceType_eMtsif, /* MTSIF-based frontend */
    NEXUS_TsmfSourceType_eMax
} NEXUS_TsmfSourceType;

/***************************************************************************
Description:
Settings to control TSMF module in transport and frontend blocks.
***************************************************************************/
typedef struct NEXUS_TsmfSettings
{
    NEXUS_TsmfSourceType sourceType;
    struct {
        NEXUS_InputBand inputBand;
        NEXUS_RemuxHandle remux;
        NEXUS_FrontendConnectorHandle mtsif;
    } sourceTypeSettings;

    bool enabled;
#if (defined(CONFIG_DEMOD_TSMF) || (defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)))
	bool parserforcesetting;
#endif    
    NEXUS_TsmfFieldVerifyConfig fieldVerifyConfig;
    unsigned relativeTsNum; /* TSMF Relative TS Number */
    bool semiAutomaticMode; /* Set true for semi-automatic mode.
                               If false, it is automatic mode and the following settings params are ignored */

    /* semi-automatic mode settings */
    uint32_t slotMapLo; /* Lower 32 bits (LSBs) of the 52-bit TSMF Slot Map vector */
    uint32_t slotMapHi; /* Upper 21 bits of the 53-bit Slot Map vector for TSMF Demultiplex */
} NEXUS_TsmfSettings;

/***************************************************************************
Summary:
Callback function prototype used in NEXUS_CoreInterruptInterface.pConnectInterrupt
****************************************************************************/
typedef void (*NEXUS_Core_InterruptFunction)(void *, int);

/***************************************************************************
Summary:
VCXO-PLL Index 
 
Description: 
VCXO-PLLs connect to a NEXUS_Timebase and provide a reference clock to 
NEXUS_AudioOutputPll objects as well as CCIR-656 outputs. 
****************************************************************************/
typedef enum NEXUS_Vcxo
{
    NEXUS_Vcxo_e0,
    NEXUS_Vcxo_e1,
    NEXUS_Vcxo_e2,
    NEXUS_Vcxo_e3,
    NEXUS_Vcxo_eMax
} NEXUS_Vcxo;

#ifdef __cplusplus
}
#endif

#endif

