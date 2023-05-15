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
 * $brcm_Workfile: nexus_playpump.c $
 * $brcm_Revision: 136 $
 * $brcm_Date: 10/16/12 1:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_playpump.c $
 * 
 * 136   10/16/12 1:34p erickson
 * SW7435-375: transport does not depend on security
 * 
 * 135   10/15/12 12:01p erickson
 * SW7435-375: transport module should not call into security module
 * 
 * 134   10/11/12 12:06p vsilyaev
 * SW7425-3955: Use upper 8 bits as PES sub-id for MPEG-2 TS streams
 * 
 * 133   10/8/12 3:52p katrep
 * SW7445-1:first compile pass for 7445
 * 
 * 132   10/1/12 3:49p vsilyaev
 * SW7425-4013: Verify arguments of NEXUS_Playpump_WriteComplete call
 * 
 * 131   8/10/12 11:51a vsilyaev
 * SW7425-3701:  Improved spelling
 * 
 * 130   8/10/12 11:46a vsilyaev
 * SW7425-3701:  Use BXPT_Playback_IsAvailable to verify where playback
 *  channel is available
 * 
 * 129   8/9/12 4:57p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 128   7/30/12 11:02a erickson
 * SW7358-266: remove pidch ref_cnt from playpump/playback.
 *  NEXUS_Playpump_P_OpenPid will validate, which also allows
 *  pidChannelIndex options to be used.
 * 
 * 127   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 126   7/9/12 1:58p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 125   7/6/12 1:03p erickson
 * SW7340-296: use nexus_register_NEXUS_PidChannelHandle instead of attr
 * 
 * 124   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 123   6/15/12 11:51a erickson
 * SW7425-3081: add IsTransportTypeSupported
 *
 * 122   6/5/12 11:50a erickson
 * SW7340-296: call nexus_unregister only if
 *  NEXUS_Playpump_ClosePidChannel is called
 *
 * 121   6/4/12 4:32p vsilyaev
 * SW7420-2313: Use reference counter for the NEXUS_Dma objects
 *
 * 120   5/3/12 10:10a ahulse
 * SW7425-1886: merge to mainline
 *
 * SW7425-1886_2/1   5/1/12 4:58p ahulse
 * SW7425-1886: refactor to enable cc error with new xpt
 *
 * 119   3/21/12 2:28p erickson
 * SW7340-296: use nexus_unregister_NEXUS_PidChannelHandle instead of
 *  secondary_destructor
 *
 * 118   3/14/12 11:39a erickson
 * SW7401-4517: remove old code
 *
 * 117   3/9/12 12:20p erickson
 * SW7405-5604: improve DSS not supported error
 *
 * 116   1/3/12 12:29p erickson
 * SW7425-2050: revise so that non-audio pids can have substream id if pid
 *  >= 0x100. fix logic for audio pid default case.
 *
 * 115   1/2/12 4:18p erickson
 * SW7425-2050: allow substreams ID's for any audio codec when stream type
 *  is eVob
 *
 * 114   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 *
 * 113   12/19/11 10:06a erickson
 * SW7425-1795: validate driver-side CPU accessibility of playback fifo
 *
 * 112   12/9/11 5:22p bandrews
 * SW7550-772: merge to main
 *
 * SW7550-772/1   12/9/11 4:07p bandrews
 * SW7550-772: change playpump to always use the default timebase, instead
 *  of possibly using no timebase
 *
 * 111   12/5/11 10:43a erickson
 * SW7231-497: allow NEXUS_PlaypumpOpenSettings.fifoSize = 0 for
 *  scatter/gather-only usage
 *
 * 110   11/29/11 11:39a erickson
 * SW7420-2129: get current default heap using NEXUS_P_DefaultHeap
 *
 * 109   11/2/11 3:55p mward
 * SW7346-523: Fix compile error if !BXPT_HAS_32BIT_PB_TIMESTAMPS.
 *
 * 108   10/31/11 7:48p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/3   10/28/11 3:49p bandrews
 * SW7231-391: update timebase to a pointer
 *
 * SW7420-2078/2   10/27/11 8:07p bandrews
 * SW7231-391: merge from main
 *
 * 107   10/28/11 1:27p erickson
 * SW7346-523: extend NEXUS_TransportTimestamp to handle 32 bit timestamps
 *  and to be more explicit about 28/30 bit timestamps
 *
 * 106   10/26/11 9:30a erickson
 * SW7346-525: apply pacingOffsetAdjustDisable using XPT PI, not PB0-only
 *  register write
 *
 * 105   10/17/11 11:38a gmullen
 * SW7425-1383:  Merged to main
 *
 * SW7425-1383/2   10/14/11 4:09p gmullen
 * SW7425-1383: Updated per David's suggestions
 *
 * SW7425-1383/1   10/11/11 3:45p gmullen
 * SW7425-1383: Added API to return the chip-specific allPass PID channel
 *  number
 *
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 *
 * 104   9/14/11 12:11p jtna
 * SW7420-2054: enforce boundsHeap for scatter-gather
 *
 * 103   9/8/11 10:56a erickson
 * SW7408-284: add comment explaining 32/30_2U/28_4P timestamp modes for
 *  live, playback and record
 *
 * 102   9/2/11 12:40p erickson
 * SW7420-1995: unregister handles from objdb when doing automatic close
 *
 * 101   8/2/11 2:53p erickson
 * SW7420-1978: use BXPT_PB_SYNC_MPEG for TS for all silicon
 *
 * 100   7/21/11 7:06p jtna
 * SW7125-1015: assert pNumConsumed in
 *  NEXUS_Playpump_SubmitScatterGatherDescriptor()
 *
 * 99   7/21/11 2:28p jtna
 * SW7125-1015: added NEXUS_Playpump_SubmitScatterGatherDescriptor()
 *
 * 98   5/12/11 3:41p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 *
 * 97   3/8/11 6:08p vsilyaev
 * SW7422-101: Updated logic of handling continuityCountEnabled, so per
 *  band and per pid settings are combined with logical AND
 *
 * 96   2/28/11 12:00p hongtaoz
 * SW7425-93, SW7425-39: remove hardcoded PES pacing timebase setting
 *  which is set properly now by stream mux;
 *
 * 95   2/25/11 4:46p vsilyaev
 * SW7422-107, SW7425-39: Fixed typo
 *
 * 94   2/22/11 7:53p vsilyaev
 * SW7422-107: Added FLV mapping
 *
 * 93   1/28/11 8:55a erickson
 * SW7420-1440: add internal hooks so that NEXUS_PidChannel_Close can
 *  close playpump pidchannels (but not playback pidchannels)
 *
 * 92   1/17/11 9:31a erickson
 * SW7425-68: add NEXUS_TransportModuleSettings.mainHeapIndex
 *
 * 91   12/30/10 7:41p vsilyaev
 * SW7425-39: Removed special timestamp mode
 *
 * 90   12/30/10 6:42p vsilyaev
 * SW7425-39: Use special timestamp mode for mux
 *
 * 89   12/28/10 5:44p vsilyaev
 * SW7425-39: Added Mux specific interfaces
 *
 * 88   12/27/10 12:18p erickson
 * SW7425-39: fix warning
 *
 * 87   12/23/10 3:25p vsilyaev
 * SW7425-39: Start/stop playpump inside mux
 *
 * 86   12/16/10 6:12p vsilyaev
 * SW7425-39: Added playpump private API
 *
 * 85   12/16/10 5:05p erickson
 * SW7125-763: restore micropause after flush
 *
 * 84   12/13/10 6:59p vsilyaev
 * SW7425-39: Added support for MUX input
 *
 * 83   12/9/10 9:50a erickson
 * SW7420-1148: fix warning
 *
 * 82   12/7/10 5:46p erickson
 * SW7420-1148: added null_allowed, boundsHeap
 *
 * 81   11/5/10 12:28p gmohile
 * SW7422-20 : Use BXPT_HAS_PID_CHANNEL_PES_FILTERING instead of
 *  BXPT_P_MAX_PACKETIZERS_PER_PB
 *
 * 80   11/3/10 4:32p erickson
 * SW7422-20: use BXPT_P_MAX_PACKETIZERS_PER_PB instead of
 *  NEXUS_SW_VOB_SUPPORT
 *
 * 79   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 78   10/1/10 9:41a erickson
 * SW7420-1009: support NEXUS_ANY_ID
 *
 * 77   9/13/10 6:02p spothana
 *  SW7420-662: Increase the number of playback channels supported
 *
 * 76   8/18/10 12:26p vsilyaev
 * SW3556-1175: Added substream ID mapping for EAC3 audio
 *
 * 75   8/10/10 3:33p erickson
 * SW7420-934: rename NEXUS_Playpump_ReadComplete to
 *  NEXUS_Playpump_WriteComplete
 *
 * 74   6/11/10 6:58p mphillip
 * SW7125-463: Allow the user to configure DMA data format in playpump
 *
 * 73   6/7/10 6:23p mphillip
 * SW7550-398: Update examples and transport code to support non-DMA
 *  encrypted PVR
 *
 * 72   4/9/10 1:07p jgarrett
 * SW7405-4034: Moving MSDRM PD/ND binaries into nexus as required
 *
 * DrmMakefileUpdates/2   4/9/10 10:40a jgarrett
 * SW7405-4034: Removing stale MSDRM lines regarding ASF decryption
 *
 * DrmMakefileUpdates/1   4/8/10 4:46p jgarrett
 * SW7405-4034: Including binaries for MSDRM (PD/ND) in the nexus builds
 *  to avoid link issues
 *
 * 71   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 *
 * 70   12/29/09 12:51p erickson
 * SW7550-146: allow security module to be initialized after the transport
 *  module if HW requires it
 *
 * 69   11/3/09 9:56a erickson
 * SW7405-3308: add TEI error callback
 *
 * 68   10/22/09 4:14p erickson
 * SW7405-3245: fix playpump bprofile support
 *
 * 67   10/1/09 5:03p erickson
 * SW7405-3087: add playpump cc check, add cc error counting per pid
 *
 * 66   8/19/09 1:49p gmohile
 * PR 57814 : Add dvd vob support for 7401
 *
 * 65   6/11/09 5:05p jtna
 * PR55767: added NEXUS_PlaypumpSettings.timestamp.forceRestamping
 *
 * 64   6/11/09 4:31p jtna
 * PR55817: make check on duplicate channels backward-compatible
 *
 * 63   6/9/09 3:09p jtna
 * PR55767: refactor timestamp record
 *
 * 62   6/8/09 3:47p erickson
 * PR55817: relax the check on duplicate pid channels. they are allowed,
 *  as long as they are simultaneously enabled. we still prevent duplicate
 *  NEXUS_PidChannelHandle entries.
 *
 * 61   6/3/09 7:12p mward
 * PR 51821: Set the AlwaysResumeFromLastDescriptor = true in
 *  NEXUS_Playpump_Open() also, to avoid warnings from XPT.
 *
 * PR51821/1   6/1/09 12:23p vsilyaev
 * PR 51821: Always append chain to the last playback descriptor
 *
 * 59   6/1/09 8:45a erickson
 * PR55461: set PARSER_FORCE_RESTAMP for pcr-based pacing
 *
 * 58   5/28/09 2:41p erickson
 * PR55461: use BXPT_HAS_PCR_PACING
 *
 * 57   5/28/09 11:31a erickson
 * PR55461: add PCR-based pacing
 *
 * 56   5/19/09 3:02p vsilyaev
 * PR 55299: Fixed PES PID mapping for the case of multiple audio tracks
 *
 * 55   5/15/09 12:48p vsilyaev
 * PR 55193: Added function to return last parsed PTS by the media library
 *
 * 54   5/14/09 10:40a jtna
 * PR54515: workaround for playpump_open() dependency on security module
 *
 * 53   4/20/09 10:53a erickson
 * PR53662: rework XPT power management code to call BXPT_P_CanPowerDown
 *  before powering down core
 *
 * 52   4/17/09 2:35p katrep
 * PR50207: Reverting back previous changes
 *
 * 50   1/26/09 11:03a vsilyaev
 * PR 51579: Adding originalTransportType to the PlaypumpSettings
 *
 * 49   12/30/08 8:55a vsilyaev
 * PR 50606: Added hooks for hardware support of MPEG1 system streams
 *
 * 48   12/21/08 6:01p nickh
 * PR50605: Fix compile errors when running without Security
 *
 * 47   12/17/08 12:19a erickson
 * PR50231: clean up failed NEXUS_Playpump_Open
 *
 * 46   12/10/08 7:36p vsilyaev
 * PR 48908: Added control of power management
 *
 * 45   12/8/08 2:39p erickson
 * PR49993: added NEXUS_TaskCallback_Destroy
 *
 * 44   12/4/08 3:29p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 *  stream parsing
 *
 * 43   12/2/08 2:23p jgarrett
 * PR 47993: Fixing substream ID mapping to allow proper substream ID's to
 *  be passed from the application
 *
 * 42   11/21/08 8:54a erickson
 * PR49531: don't flush if not started
 *
 * 41   10/30/08 10:27p erickson
 * PR47132: added NEXUS_PlaypumpSettings.blindSync
 *
 * 40   10/29/08 1:58p erickson
 * PR47132: set BXPT_Playback_ParserConfig.ErrorInputIgnore and
 *  AcceptAdapt00 to true to allow raw data to pass through. no reason to
 *  exclude this data, like the current ContCountIgnore default.
 *
 * 39   10/27/08 11:20a erickson
 * PR47232: added pacingTsRangeError to NEXUS_PlaypumpStatus
 *
 * 38   10/24/08 4:41p erickson
 * PR47232: allow timebase to change on the fly
 *
 * 37   10/22/08 11:39a erickson
 * PR48216: allow transport to run with no dma or security modules
 *
 * 36   10/17/08 3:01p katrep
 * PR47951: Add index playpump status structure
 *
 * 35   10/13/08 9:41a erickson
 * PR47232: set BXPT_Playback_ParserConfig.TsMode
 *
 * 34   10/9/08 5:49p erickson
 * PR47232: more timestamp impl
 *
 * 33   10/9/08 10:06a erickson
 * PR47232: set additional timebase setting for
 *  NEXUS_PlaypumpSettings.timestamp.timebase
 *
 * 32   10/7/08 11:31p erickson
 * PR47232: extend NEXUS_PlaypumpSettings timestamp settings. reorganized
 *  into a nested structure.
 *
 * 31   10/6/08 1:59a erickson
 * PR47232: added NEXUS_Playpump_SetPause for IP STB throttling
 *
 * 30   10/6/08 1:30a erickson
 * PR47232: added NEXUS_PlaypumpSettings.pacingMaxError
 *
 * 29   9/22/08 10:03a erickson
 * PR47145: fix warning
 *
 * 28   9/4/08 12:46p vishk
 * PR 46315: Sample code for PID remapping
 *
 * 27   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 26   7/29/08 3:07p erickson
 * PR45199: added NEXUS_PlaypumpSettings.maxDataRate
 *
 * 25   7/28/08 3:33p erickson
 * PR45124: fix misspellings
 *
 * 24   7/24/08 7:55a gmullen
 * PR42365: Use MPEG blind sync mode for Directv playback.
 *
 * 23   6/25/08 5:50p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 22   6/17/08 12:39p vsilyaev
 * PR 42739: Fixed reference counter
 *
 * 21   6/17/08 11:26a vsilyaev
 * PR 42739: Fixed NO PVR build
 *
 * 20   6/17/08 10:53a vsilyaev
 * PR 42739: Added support for duplicate pids
 *
 * 19   6/13/08 6:43p erickson
 * PR43087: fix warnings if NEXUS_NUM_PLAYPUMPS=0
 *
 * 18   6/3/08 11:57a jgarrett
 * PR 43279: Correcting size argument
 *
 * 17   5/28/08 7:45p jrubio
 * PR43085: fix PLAYPUMP=0 compile
 *
 * 16   5/16/08 1:56p erickson
 * PR42758: add NEXUS_TransportType_eVob support
 *
 * 15   5/14/08 1:25p vsilyaev
 * PR 42119: Preserve PES->TS packetizer settings over
 *  Playpump_Stop/Playpump_Start calls. PR 41869:
 *
 * 14   5/9/08 11:34a erickson
 * PR42119: preserve originalTransportType for media-framework streams
 *
 * 13   5/7/08 10:24p vsilyaev
 * PR 41869: Added DMA into the playpump_crypto
 *
 * 12   5/7/08 2:00p vsilyaev
 * PR 41869: Added core for re-packetize MPEG-2 TS data for encrypted PVR
 *
 * 11   5/5/08 1:34p vsilyaev
 * PR 42355: Reset media playRate on stop
 *
 * 10   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 9   4/25/08 1:01p erickson
 * PR41951: NEXUS_Playpump_Stop should call flush before stopping
 *  PVRlib_Feed_Stop
 *
 * 8   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 7   4/17/08 9:31a erickson
 * PR39994: remove param
 *
 * 6   4/10/08 2:40p erickson
 * PR41557: enable ES packetization for all chips
 *
 * 5   4/9/08 1:09p jgarrett
 * PR 41557: Fixing GetDefaultOpenPidChannelSettings
 *
 * 4   2/28/08 9:44p vsilyaev
 * PR 40103: Used NEXUS_TaskCallback functions for API callbacks
 *
 * 3   2/26/08 10:53a erickson
 * PR34925: default playRate
 *
 * 2   1/21/08 10:42a gmullen
 * PR38854: Set BPVRlib channelNum to playpump instance.
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#include "nexus_playpump_impl.h"

#ifndef BXPT_HAS_MULTICHANNEL_PLAYBACK
#include "bchp_int_id_xpt_pb0.h"
#include "bchp_int_id_xpt_pb1.h"
#if NEXUS_NUM_PLAYPUMPS > 2
#include "bchp_int_id_xpt_pb2.h"
#endif
#if NEXUS_NUM_PLAYPUMPS > 3
#include "bchp_int_id_xpt_pb3.h"
#endif
#if NEXUS_NUM_PLAYPUMPS > 4
#include "bchp_int_id_xpt_pb4.h"
#endif
#if NEXUS_NUM_PLAYPUMPS > 5
#include "bchp_int_id_xpt_pb5.h"
#endif
#if NEXUS_NUM_PLAYPUMPS > 6
#include "bchp_int_id_xpt_pb6.h"
#endif
#if NEXUS_NUM_PLAYPUMPS > 7
#include "bchp_int_id_xpt_pb7.h"
#endif
#else /* BXPT_HAS_MULTICHANNEL_PLAYBACK */
#warning add mcpb for 7445
#endif

#include "priv/nexus_timebase_priv.h"
#include "nexus_class_verification.h"
#if NEXUS_NUM_DMA_CHANNELS
#include "nexus_dma.h"
#include "priv/nexus_dma_priv.h"
#endif
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_playpump);

#define NEXUS_P_PACKETIZER_BASE (0x100)

static BERR_Code NEXUS_Playpump_P_SetParserBand(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpSettings *pSettings);
static void NEXUS_Playpump_P_InstallRangeErrIntHandler(NEXUS_PlaypumpHandle p);
static void NEXUS_Playpump_P_UninstallRangeErrIntHandler(NEXUS_PlaypumpHandle p);
static NEXUS_Error NEXUS_Playpump_P_SetInterrupts(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpSettings *pSettings);

void NEXUS_Playpump_GetDefaultOpenSettings(NEXUS_PlaypumpOpenSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->streamMuxCompatible = false;
    pSettings->fifoSize = B_PVR_PLAYBACK_BUFFER;
    pSettings->alignment = 12; /* 2^12 = 4096  (I/O block size) alignment */
    pSettings->numDescriptors = NEXUS_NUM_PLAYBACK_DESC;
}
void
NEXUS_Playpump_GetDefaultSettings(NEXUS_PlaypumpSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#if NEXUS_NUM_DMA_CHANNELS
    pSettings->securityDmaDataFormat = NEXUS_DmaDataFormat_eMpeg;
#endif
    pSettings->transportType = NEXUS_TransportType_eTs;
    pSettings->originalTransportType = NEXUS_TransportType_eUnknown;
    pSettings->timestamp.type = NEXUS_TransportTimestampType_eNone;
    pSettings->timestamp.timebase = NEXUS_Timebase_eInvalid;
    pSettings->timestamp.pacingMaxError = 1024; /* HW reset value */
    pSettings->timestamp.parityCheckDisable = false; /* HW reset value */
    pSettings->mode = NEXUS_PlaypumpMode_eFifo;
    pSettings->playRate = NEXUS_NORMAL_PLAY_SPEED;
    pSettings->allPass = false;
    pSettings->acceptNullPackets = false;
    pSettings->maxDataRate = 108000000;
    NEXUS_CallbackDesc_Init(&pSettings->dataCallback);
    NEXUS_CallbackDesc_Init(&pSettings->errorCallback);
    return;
}

#if defined (HUMAX_PLATFORM_BASE)
void
NEXUS_Playpump_GetDcryptCtx(void *cntx, void *dcrypt_ctx)
{
	b_pump_get_demux_ctx(cntx, dcrypt_ctx);
}
#else
void
NEXUS_Playpump_GetDcryptCtx(void *cntx, void *dcrypt_ctx)
{
	return;
}
#endif

NEXUS_PlaypumpHandle
NEXUS_Playpump_Open(unsigned index, const NEXUS_PlaypumpOpenSettings *pSettings)
{
#if NEXUS_NUM_PLAYPUMPS
    NEXUS_PlaypumpHandle p;
    BXPT_Playback_ChannelSettings play_cfg;
    BERR_Code rc;
    BPVRlib_Feed_Settings feed_cfg;
    NEXUS_PlaypumpOpenSettings defaultSettings;
    NEXUS_HeapHandle heap;

    if (!pSettings) {
        NEXUS_Playpump_GetDefaultOpenSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    if (index == NEXUS_ANY_ID) {
        unsigned i;
        for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) {
            if(pTransport->playpump[i].playpump==NULL && BXPT_Playback_IsAvailable(pTransport->xpt, i)) {
                index = i;
                break;
            }
        }
        if (i == NEXUS_NUM_PLAYPUMPS) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            BDBG_ERR(("no playpump not available"));
            return NULL;
        }
    }

    if (index >= NEXUS_NUM_PLAYPUMPS) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ERR(("playpump[%d] not available", index));
        return NULL;
    }

    if (pTransport->playpump[index].playpump) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ERR(("playpump[%d] already open", index));
        return NULL;
    }
    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(playpump,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }
    
    
    pTransport->playpump[index].playpump = p = BKNI_Malloc(sizeof(*p));
    if (!p) {
        NEXUS_CLIENT_RESOURCES_RELEASE(playpump,Count,NEXUS_ANY_ID);
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Playpump, p);

    p->index = index;
    p->openSettings = *pSettings;
    p->consumerStarted = false;
    NEXUS_Playpump_GetDefaultSettings(&p->settings);
    p->settings.transportType = NEXUS_TransportType_eTs;
    p->settings.timestamp.type = NEXUS_TransportTimestampType_eNone;
    p->settings.timestamp.pacing = false;
    p->settings.playRate = NEXUS_NORMAL_PLAY_SPEED;
    p->dataCallback = NEXUS_TaskCallback_Create(p, NULL);
    if(!p->dataCallback) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    p->errorCallback = NEXUS_TaskCallback_Create(p, NULL);
    if(!p->errorCallback) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    p->ccErrorCallback = NEXUS_IsrCallback_Create(p, NULL);
    if(!p->ccErrorCallback) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    p->teiErrorCallback = NEXUS_IsrCallback_Create(p, NULL);
    if(!p->teiErrorCallback) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    NEXUS_CallbackDesc_Init(&p->settings.dataCallback);
    NEXUS_CallbackDesc_Init(&p->settings.errorCallback);
    b_pid_map_init(&p->packetizer_map, NEXUS_P_PACKETIZER_BASE);

    BLST_S_INIT(&p->pid_list);

    p->item_mem = BKNI_Malloc(sizeof(*p->item_mem)*pSettings->numDescriptors);
    if(p->item_mem==NULL) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }

    heap = NEXUS_P_DefaultHeap(pSettings->heap, NEXUS_DefaultHeapType_eFull);
    if (!heap) {
        heap = g_pCoreHandles->nexusHeap[pTransport->settings.mainHeapIndex];
    }
    /* some playpump operations require driver CPU accessible to the playback fifo */
    if (!NEXUS_P_CpuAccessibleHeap(heap)) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
    }
    p->heap = NEXUS_Heap_GetMemHandle(heap);

    /* fifoSize == 0 is valid for scatter/gather-only mode */
    if (pSettings->fifoSize) {
        p->buf_noncached = BMEM_AllocAligned(p->heap, pSettings->fifoSize, pSettings->alignment, 0);
        if (!p->buf_noncached) {
            rc = BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        rc = BMEM_Heap_ConvertAddressToCached(p->heap, p->buf_noncached, &p->buf); /* map memory to the cached region */
        if (rc) {rc=BERR_TRACE(rc); goto error;}
    }
    BDBG_MSG(("alloc buffer %p %d", p->buf, pSettings->fifoSize));

    heap = NEXUS_P_DefaultHeap(pSettings->boundsHeap, NEXUS_DefaultHeapType_eBounds);
    if (heap) {
        BMEM_HeapInfo heapInfo;
        BMEM_Heap_Handle boundsHeap = NEXUS_Heap_GetMemHandle(heap);
        BMEM_Heap_GetInfo(boundsHeap, &heapInfo);
        p->boundsHeapSize = heapInfo.zSize;
        rc = BMEM_Heap_ConvertAddressToCached(boundsHeap, heapInfo.pvAddress, &p->boundsHeapAddr);
        if (rc) {
            rc = BERR_TRACE(rc);
            p->boundsHeapAddr = 0;
            p->boundsHeapSize = 0;
        }
    }

    b_playpump_p_reset(p);

    rc = BXPT_Playback_GetChannelDefaultSettings(pTransport->xpt, index, &play_cfg);
    if (rc) {rc=BERR_TRACE(rc);goto error;}

#if NEXUS_HAS_LEGACY_XPT
    play_cfg.AlwaysResumeFromLastDescriptor = true;
#endif

    rc = BXPT_Playback_OpenChannel(pTransport->xpt, &p->xpt_play, index, &play_cfg);
    if (rc) {
        unsigned i;
        for (i=index;i<NEXUS_NUM_PLAYPUMPS;i++) {
            if (pTransport->playpump[i].playpump==NULL && BXPT_Playback_IsAvailable(pTransport->xpt, i)) {
                BDBG_ERR(("playback:%u is not available, try playback:%u", index, i));
                break;
            }
        }
        rc=BERR_TRACE(rc);
        goto error;
    }

    rc = NEXUS_Playpump_P_SetParserBand(p, &p->settings);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error;}

    BPVRlib_Feed_GetDefaultSettings(&feed_cfg);
    feed_cfg.xptHandle = pTransport->xpt;
    feed_cfg.xptPlayHandle = p->xpt_play;
    feed_cfg.heap = p->heap;
    feed_cfg.intHandle = g_pCoreHandles->bint;
    feed_cfg.numDesc = 32;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    if(p->openSettings.streamMuxCompatible) {
        /*
         * when used for StreamMux use number of descriptors as number of descriptors queued in the HW, otherwise numDesc is just number of SW descriptors,
         * SW->HW descriptors refilled by playpump (media conversion step run inline with refill)
         */
        feed_cfg.numDesc = p->openSettings.numDescriptors;
    }
#endif
    feed_cfg.applicationCnxt = p;
    feed_cfg.useExtndedDesc = pSettings->streamMuxCompatible;
    feed_cfg.descAvaliable_isr = NEXUS_P_Playpump_DescAvail_isr;
    rc = BPVRlib_Feed_Open(&p->play_feed, &feed_cfg);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error;}

    rc = BKNI_CreateEvent(&p->descEvent);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error;}

#if B_HAS_PLAYPUMP_IP
    {
        rc = b_playpump_ip_open(&p->ip, p, open_params);
        if (rc) { rc = BERR_TRACE(rc);goto error; }
    }
#endif

    /* init vob remap state */
    p->vob_remap_state.codec = NEXUS_AudioCodec_eMpeg;
    p->vob_remap_state.track = 0;

    p->crypto = b_pump_crypto_create(p);
#if 0 /* if security module not connected, NULL is normal */
    if(!p->crypto) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
#endif

#if B_HAS_MEDIA
    p->use_media = false;
    p->demux = b_pump_demux_create(p);
    if(!p->demux) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
#endif

    return p;
error:
    if (p) {
        if (p->crypto) {
            b_pump_crypto_destroy(p->crypto);
        }
        if (p->play_feed) {
            BPVRlib_Feed_Close(p->play_feed);
        }
        if (p->descEvent) {
            BKNI_DestroyEvent(p->descEvent);
        }
        if (p->xpt_play) {
            BXPT_Playback_CloseChannel(p->xpt_play);
        }
        if (p->buf_noncached) {
            BMEM_Free(p->heap, p->buf_noncached);
        }
        if(p->item_mem) {
            BKNI_Free(p->item_mem);
        }
        if(p->errorCallback) {
            NEXUS_TaskCallback_Destroy(p->errorCallback);
        }
        if(p->ccErrorCallback) {
            NEXUS_IsrCallback_Destroy(p->ccErrorCallback);
        }
        if(p->teiErrorCallback) {
            NEXUS_IsrCallback_Destroy(p->teiErrorCallback);
        }
        if(p->dataCallback) {
            NEXUS_TaskCallback_Destroy(p->dataCallback);
        }
        BKNI_Free(p);
        pTransport->playpump[index].playpump = NULL;
    }
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
#endif
    return NULL;
}

static void NEXUS_Playpump_P_Finalizer(NEXUS_PlaypumpHandle p)
{
    NEXUS_OBJECT_ASSERT(NEXUS_Playpump, p);

    if (p->state.running) {
        NEXUS_Playpump_Stop(p);
    }
    NEXUS_Playpump_CloseAllPidChannels(p);
#if B_HAS_PLAYPUMP_IP
    b_playpump_ip_close(&p->ip);
#endif
    if (p->crypto) {
        b_pump_crypto_destroy(p->crypto);
    }
#if B_HAS_MEDIA
    b_pump_demux_destroy(p->demux);
#endif
    p->state.packetizer = b_play_packetizer_none;

    /* force the destroying of the xpt interrupt */
    p->settings.ccError.callback = NULL;
    p->settings.teiError.callback = NULL;
    NEXUS_Playpump_P_SetInterrupts(p, &p->settings);

    BPVRlib_Feed_Close(p->play_feed);
    BKNI_DestroyEvent(p->descEvent);
    BXPT_Playback_CloseChannel(p->xpt_play);
    if (p->buf_noncached) {
        BMEM_Free(p->heap, p->buf_noncached);
    }
    BKNI_Free(p->item_mem);
    p->xpt_play = NULL;
    p->play_feed = NULL;
#if NEXUS_NUM_PLAYPUMPS
    pTransport->playpump[p->index].playpump = NULL;
#endif
#if NEXUS_NUM_DMA_CHANNELS
    if(p->settings.securityDma) {
        NEXUS_OBJECT_RELEASE(p, NEXUS_Dma, p->settings.securityDma);
    }
#endif

    NEXUS_IsrCallback_Destroy(p->ccErrorCallback);
    NEXUS_IsrCallback_Destroy(p->teiErrorCallback);
    NEXUS_TaskCallback_Destroy(p->errorCallback);
    NEXUS_TaskCallback_Destroy(p->dataCallback);
    NEXUS_CLIENT_RESOURCES_RELEASE(playpump,Count,NEXUS_ANY_ID);
    NEXUS_OBJECT_DESTROY(NEXUS_Playpump, p);
    BKNI_Free(p);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Playpump, NEXUS_Playpump_Close);

void NEXUS_Playpump_GetSettings(NEXUS_PlaypumpHandle p, NEXUS_PlaypumpSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BDBG_ASSERT(pSettings);
    *pSettings = p->settings;
    return;
}

static BERR_Code
NEXUS_Playpump_P_SetParserBand(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpSettings *pSettings)
{
    BXPT_Playback_ParserConfig parserConfig;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BDBG_ASSERT(p->xpt_play);

#if B_HAS_DSS
    rc = BXPT_DirecTvPlayback_SetParserBandMode( p->xpt_play, NEXUS_IS_DSS_MODE(pSettings->transportType)?
        BXPT_ParserMode_eDirecTv : BXPT_ParserMode_eMpeg); /* XXX this shall precced SetParserConfig, since it resets parser configuration */
    if (rc) return BERR_TRACE(rc);
#endif

    rc = BXPT_Playback_GetParserConfig(p->xpt_play, &parserConfig);
    if (rc) return BERR_TRACE(rc);
    /* for playback, we should just let all data through. if the stream is good, there's no harm.
    if the stream is bad, the decoders must handle this anyway. */
#if NEXUS_PARSER_BAND_CC_CHECK
    parserConfig.ContCountIgnore = !pSettings->continuityCountEnabled;
#else
    if(pSettings->continuityCountEnabled != p->settings.continuityCountEnabled && BLST_S_FIRST(&p->pid_list)!=NULL) {
        BDBG_WRN(("%#lx:continuityCountEnabled wouldn't get applied to aleady opened pids", (unsigned long)p));
    }
#endif
    parserConfig.ErrorInputIgnore = (pSettings->teiError.callback == NULL); /* if we want these errors, then transport cannot ignore */
    parserConfig.AcceptAdapt00 = true;
    parserConfig.AllPass = pSettings->allPass;
    parserConfig.AcceptNulls = pSettings->acceptNullPackets;

#if BXPT_HAS_PCR_PACING
    /* enable PARSER_FORCE_RESTAMP in pcr-pacing mode */
    parserConfig.ForceRestamping = (pSettings->timestamp.pacing && pSettings->timestamp.pcrPacingPid);
#endif
    if (pSettings->timestamp.forceRestamping==true) {
        parserConfig.ForceRestamping = true;
    }

    parserConfig.UsePcrTimeBase = true; /* eInvalid now means get the default timebase and track it */
    if (parserConfig.UsePcrTimeBase) {
        NEXUS_TimebaseHandle timebase = NEXUS_Timebase_Resolve_priv(pSettings->timestamp.timebase);
        if (timebase)
        {
            parserConfig.WhichPcrToUse = timebase->hwIndex;
        }
        else
        {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    rc = BXPT_Playback_SetParserConfig(p->xpt_play, &parserConfig);
    if (rc) return BERR_TRACE(rc);

    rc = BXPT_Playback_SetBitRate(p->xpt_play, pSettings->maxDataRate);
    if (rc) return BERR_TRACE(rc);

    return 0;
}


NEXUS_Error
NEXUS_Playpump_SetSettings(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpSettings *pSettings)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BDBG_ASSERT(pSettings);

    if (pSettings->playRate == 0) {
        /* playRate == 0 is not pause. It must be non-zero. */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if(p->state.running && (
                p->settings.transportType != pSettings->transportType ||
                p->settings.timestamp.type != pSettings->timestamp.type ||
                p->settings.timestamp.pacing != pSettings->timestamp.pacing ||
                p->settings.securityDma != pSettings->securityDma ||
                p->settings.securityContext != pSettings->securityContext ||
                p->settings.mode != pSettings->mode
                )) {
        BDBG_WRN(("NEXUS_Playpump_SetSettings: %lx can't change settings when started", (unsigned long)p));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_running;
    }

#if NEXUS_NUM_DMA_CHANNELS
    if (pSettings->securityDma && !pTransport->settings.dma) {
        BDBG_ERR(("Transport module does not have dma module handle."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
#endif

    if (pSettings->timestamp.pacingMaxError != p->settings.timestamp.pacingMaxError) {
        rc = BXPT_Playback_SetPacingErrorBound(p->xpt_play, pSettings->timestamp.pacingMaxError);
        if (rc) return BERR_TRACE(rc);
    }

    if (pSettings->timestamp.timebase != p->settings.timestamp.timebase) {
        BXPT_Playback_ChannelSettings cfg;

        rc = BXPT_Playback_GetChannelSettings(p->xpt_play, &cfg);
        if (rc) return BERR_TRACE(rc);

        cfg.UsePcrTimeBase = true; /* eInvalid now means get the default timebase and track it */
        if (cfg.UsePcrTimeBase) {
            NEXUS_TimebaseHandle timebase = NEXUS_Timebase_Resolve_priv(pSettings->timestamp.timebase);
            if (timebase)
            {
                cfg.WhichPcrToUse = timebase->hwIndex;
            }
            else
            {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }

        rc = BXPT_Playback_SetChannelSettings(p->xpt_play, &cfg);
        if (rc) return BERR_TRACE(rc);
    }

    if(pSettings->transportType != p->settings.transportType) {
        if(BLST_S_FIRST(&p->pid_list)!=NULL) {
            BDBG_WRN(("NEXUS_Playpump_SetSettings: %lx can't change settings when PIDs are attached", (unsigned long)p));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_attached;
        }
#if B_HAS_MEDIA
        rc = b_pump_demux_set_stream_type(p->demux, pSettings->transportType, &p->use_media);
        if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc); goto err_media;}
        BDBG_MSG(("NEXUS_Playpump_SetSettings: %s of stream type %u", p->use_media?"media preprocessing":"native processsing", pSettings->transportType));

        if(!p->use_media) {
            bool supported;

            if (p->crypto) {
                rc = b_pump_crypto_set_stream_type(p->crypto, pSettings->transportType, &supported);
                if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc); goto err_crypto;}
            }
        }
#endif
    }

    rc = NEXUS_Playpump_P_SetParserBand(p, pSettings);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_parser_band;}

#if B_HAS_MEDIA
    if(p->settings.playRate != pSettings->playRate) {
        if (p->state.packetizer==b_play_packetizer_media) {
            b_pump_demux_set_rate(p->demux, pSettings->playRate);
        }
    }
#endif

#if NEXUS_NUM_DMA_CHANNELS
    if (pSettings->securityDma != p->settings.securityDma) {
        if(p->settings.securityDma) {
            NEXUS_OBJECT_RELEASE(p, NEXUS_Dma, p->settings.securityDma);
        }
        if(pSettings->securityDma) {
            NEXUS_OBJECT_ACQUIRE(p, NEXUS_Dma, pSettings->securityDma);
        }
    }
#endif

    p->settings = *pSettings;
    NEXUS_TaskCallback_Set(p->dataCallback, &p->settings.dataCallback);
    NEXUS_TaskCallback_Set(p->errorCallback, &p->settings.errorCallback);
    NEXUS_Playpump_P_SetInterrupts(p, &p->settings);

#if B_HAS_MEDIA
err_crypto:
err_media:
#endif
err_parser_band:
err_running:
err_attached:
    return rc;
}

static NEXUS_Error NEXUS_Playpump_P_StartPid(NEXUS_PlaypumpHandle p, NEXUS_P_PlaypumpPidChannel *play_pid)
{
    NEXUS_Error rc;

    /* activate packetizer to all pids that needs packetizer */
    if(play_pid->packetizer.enable) {
        rc = BXPT_Playback_PacketizeStream(p->xpt_play, play_pid->packetizer.context, &play_pid->packetizer.cfg, true);
        if (rc) return BERR_TRACE(rc);
        play_pid->packetizer.active = true;
    }

    return 0;
}

static void NEXUS_Playpump_P_StopPid(NEXUS_PlaypumpHandle p, NEXUS_P_PlaypumpPidChannel *play_pid)
{
    NEXUS_Error rc;

    if(play_pid->packetizer.enable) {
        rc = BXPT_Playback_PacketizeStream(p->xpt_play, play_pid->packetizer.context, &play_pid->packetizer.cfg, false);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        play_pid->packetizer.active = false;
    }
}

static NEXUS_Error NEXUS_Playpump_Start_priv(NEXUS_PlaypumpHandle p, bool muxInput)
{
    BERR_Code rc;
    BXPT_Playback_ChannelSettings cfg;
    NEXUS_TransportType transportType;
    NEXUS_P_PlaypumpPidChannel *play_pid;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);

    if(NEXUS_GetEnv("profile_playpump")) {
        NEXUS_Profile_Start();
    }

    if (p->state.running) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_state; }

    /* If we're in allPass, verify that there is one, and only one, PID channel on the playback. Check that the PID
       channel used is correct too. */
    if (p->settings.allPass)
    {
        unsigned AllPassPidChannel;
        NEXUS_P_PlaypumpPidChannel *play_pid;

        unsigned ChannelCount = 0;
        bool FoundValidChannel = false;

        NEXUS_Playpump_GetAllPassPidChannelIndex( p, &AllPassPidChannel );
        for(play_pid=BLST_S_FIRST(&p->pid_list); play_pid; play_pid=BLST_S_NEXT(play_pid, link))
        {
            ChannelCount++;
            if(play_pid->pid_channel->status.pidChannelIndex == AllPassPidChannel)
            {
                FoundValidChannel = true;
            }
        }

#if defined(HUMAX_PLATFORM_BASE)
        /*Do not check channel count. AV pid channel shall be opened to get time information from decoder in scramble playback.*/
        if( !ChannelCount)
#else
        if( !ChannelCount || 1 < ChannelCount )
#endif
        {
            BDBG_ERR(( "Only 1 PID channel is supported in allPass mode." ));
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto error;
        }

        if( !FoundValidChannel )
        {
            BDBG_ERR(( "Incorrect PID channel used for allPass. See NEXUS_Playpump_GetAllPassPidChannelIndex()." ));
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto error;
        }
    }

    BKNI_Memset(&p->state, 0, sizeof(p->state)); /* wipe-out all temporary state */
    BKNI_Memset(&cfg, 0, sizeof(cfg));

    NEXUS_Transport_P_IncPowerDown(true);

    rc = NEXUS_Playpump_P_SetParserBand(p, &p->settings);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_parser_band;}

    transportType = p->settings.transportType;
#if B_HAS_MEDIA
    p->state.packetizer = p->use_media ? b_play_packetizer_media:b_play_packetizer_none;
    if (p->state.packetizer==b_play_packetizer_media) {
        transportType = NEXUS_TransportType_eMpeg2Pes;
    } else
#endif
#if NEXUS_NUM_DMA_CHANNELS
    if (p->settings.securityDma && p->crypto) {
        p->state.packetizer = b_play_packetizer_crypto;
    }
#endif

    rc = BXPT_Playback_GetChannelSettings(p->xpt_play, &cfg);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

#if NEXUS_HAS_LEGACY_XPT
    cfg.AlwaysResumeFromLastDescriptor = true;
#endif
#if B_HAS_NATIVE_MPEG1
    /* initialize to the MPEG2 Program Stream Mode */
    cfg.PsMode = BXPT_Playback_PS_Mode_MPEG2;
    cfg.PackHdrConfig = BXPT_Playback_PackHdr_Drop;
#endif

#if B_PACKETIZE_HSX
    if (!(transportType == NEXUS_TransportType_eTs || NEXUS_IS_DSS_MODE(transportType))) {
        cfg.SyncMode = BXPT_PB_SYNC_MPEG_BLIND; /* TS Blind */
        cfg.PacketLength = 184;
    } else
#endif
    switch(transportType) {
    case NEXUS_TransportType_eEs:
        cfg.SyncMode = BXPT_PB_SYNC_MPEG_BLIND; /* TS Blind */
        cfg.PacketLength = 184;
        break;
#if B_HAS_NATIVE_MPEG1
    case NEXUS_TransportType_eMpeg1Ps:
        cfg.PsMode = BXPT_Playback_PS_Mode_MPEG1;
        cfg.PackHdrConfig = BXPT_Playback_PackHdr_Drop;
        /* fallthrough */
#endif
    case NEXUS_TransportType_eVob:
    case NEXUS_TransportType_eMpeg2Pes:
        cfg.SyncMode = BXPT_PB_SYNC_PES; /* TS PES */
        break;

    case NEXUS_TransportType_eTs:
        cfg.PacketLength = 188;
        cfg.SyncMode = BXPT_PB_SYNC_MPEG; /* TS mode. This allows HW to search for the 0x47 sync byte. */
        break;
    case NEXUS_TransportType_eDssPes:
    case NEXUS_TransportType_eDssEs:
#if B_HAS_DSS
        cfg.PacketLength = 130;
        cfg.SyncMode = BXPT_PB_SYNC_MPEG_BLIND; /* direct TV mode. Use MPEG blind-sync, per DVT's suggestion. PR 42365 */
#else
        BDBG_ERR(("this chip does not support DSS"));
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto error;
#endif
        break;
    default:
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }

    if (p->settings.blindSync) {
        cfg.SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
    }

    switch (p->settings.timestamp.type) {
    case NEXUS_TransportTimestampType_eNone:
        cfg.TimestampEn = false;
        break;
    case NEXUS_TransportTimestampType_e30_2U_Mod300:
        cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Mod300;
        cfg.TimestampEn = true;
        #if BXPT_HAS_32BIT_PB_TIMESTAMPS
        cfg.Use32BitTimestamps = false;
        #endif
        break;
    case NEXUS_TransportTimestampType_e30_2U_Binary:
        cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
        cfg.TimestampEn = true;
        #if BXPT_HAS_32BIT_PB_TIMESTAMPS
        cfg.Use32BitTimestamps = false;
        #endif
        break;
    #if BXPT_HAS_32BIT_PB_TIMESTAMPS
    case NEXUS_TransportTimestampType_e32_Mod300:
        cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Mod300;
        cfg.TimestampEn = true;
        cfg.Use32BitTimestamps = true;
        break;
    case NEXUS_TransportTimestampType_e32_Binary:
        cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
        cfg.TimestampEn = true;
        cfg.Use32BitTimestamps = true;
/* TODO: enable this when needed
    case NEXUS_TransportTimestampType_e28_4P_Mod300:
        rec_cfg.TimestampMode = BXPT_TimestampMode_e28_4P_Mod300;
        rec_cfg.TimestampEn = true;
        rec_cfg.Use32BitTimestamps = true;
        break;
*/
        break;
    #endif
    default:
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto error;
    }

    if (p->settings.timestamp.pcrPacingPid && p->settings.timestamp.pacing) {
#if BXPT_HAS_PCR_PACING
        cfg.PcrPacingPid = p->settings.timestamp.pcrPacingPid;
        cfg.PcrBasedPacing = true;
#else
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto error;
#endif
    }

    cfg.ResetPacing = p->settings.timestamp.resetPacing;
    cfg.DisableTimestampParityCheck = p->settings.timestamp.parityCheckDisable;
    cfg.UsePcrTimeBase = true; /* eInvalid now means get the default timebase and track it */
    if (cfg.UsePcrTimeBase) {
        NEXUS_TimebaseHandle timebase = NEXUS_Timebase_Resolve_priv(p->settings.timestamp.timebase);
        if (timebase)
        {
            cfg.WhichPcrToUse = timebase->hwIndex;
        }
        else
        {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    cfg.PacingOffsetAdjustDisable = p->settings.timestamp.pacingOffsetAdjustDisable;

#if BXPT_HAS_TSMUX
    if(muxInput) {
        cfg.PesBasedPacing = true;
        cfg.Use8WordDesc = true;
        cfg.PesBasedPacing = true;
        cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
        cfg.Use32BitTimestamps = true;
        cfg.TimestampEn = false;
    } else {
        cfg.PesBasedPacing = false;
        cfg.Use8WordDesc = false;
    }
#else
    BSTD_UNUSED(muxInput);
#endif
    rc = BXPT_Playback_SetChannelSettings(p->xpt_play, &cfg);
    if (rc) {rc=BERR_TRACE(rc);goto error;}

#if B_HAS_VBI
    rc = bstream_p_vbi_open(&p->stream);
    if (rc) goto error;
#endif

    if (p->settings.timestamp.pacing) {
        rc = BXPT_Playback_ConfigPacing(p->xpt_play, BXPT_PacingControl_eStart);
        if (rc) goto error;
        p->state.pacing = true;

        NEXUS_Playpump_P_InstallRangeErrIntHandler(p);
    }

    for(play_pid=BLST_S_FIRST(&p->pid_list);play_pid!=NULL;play_pid=BLST_S_NEXT(play_pid, link)) {
        rc = NEXUS_Playpump_P_StartPid(p, play_pid);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    }

#if B_HAS_MEDIA
    if (p->state.packetizer==b_play_packetizer_media) {
        rc = b_pump_demux_start(p->demux);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    } else
#endif
    if (p->state.packetizer==b_play_packetizer_crypto) {
        rc = b_pump_crypto_start(p->crypto);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    } else {
        rc = BPVRlib_Feed_Start(p->play_feed);
        if (rc) {rc=BERR_TRACE(rc);goto error;}
    }

    /* pause playback until first consumer (decode or record) is started. this prevent data from being lost
    if any amount of time between playback start and rave configuration (which is done at decode start). */
    if(!p->consumerStarted) { /* 't pause on Start */
        rc = BXPT_Playback_Pause(p->xpt_play);
        if (rc) {rc=BERR_TRACE(rc);goto error_pause;}
    }

    /* We may have a left over event, and we want to ensure that 1 interrupt
    always means 1 free descriptor in b_playpump_p_xpt_event */
    BKNI_ResetEvent(p->descEvent);

    p->playEventHandle = NEXUS_RegisterEvent(p->descEvent, b_playpump_p_xpt_event, p);
    if (!p->playEventHandle) {rc=BERR_TRACE(NEXUS_UNKNOWN); goto error_started;}

    /* Set run state because calling first read */
    p->state.running = true;
    p->state.muxInput = muxInput;

    if(!muxInput) {
        /* Request the first async read, which kickstarts the read cycle. */
        b_playpump_p_do_read_callback(p);

        p->throttle_timer = NEXUS_ScheduleTimer(B_THROTTLE_TIMEOUT, b_playpump_p_throttle_timer, p);  /* schedulle timer after 30 ms */
    }

    return 0;

error_pause:
error_started:
#if B_HAS_MEDIA
    if (p->state.packetizer==b_play_packetizer_media) {
        b_pump_demux_stop(p->demux);
    } else
#endif
    if (p->state.packetizer==b_play_packetizer_crypto) {
        b_pump_crypto_stop(p->crypto);
    } else {
        BPVRlib_Feed_Stop(p->play_feed);
    }
    if (p->state.pacing) {
        NEXUS_Playpump_P_UninstallRangeErrIntHandler(p);

        rc = BXPT_Playback_ConfigPacing(p->xpt_play,BXPT_PacingControl_eStop);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    }
err_parser_band:
    NEXUS_Transport_P_IncPowerDown(false);
err_state:
error:
    BDBG_ASSERT(rc);
    return rc;
}

NEXUS_Error NEXUS_Playpump_Start(NEXUS_PlaypumpHandle p)
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    if(p->openSettings.streamMuxCompatible) {
        /* Can't use this playpump for as a regular playback */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif
    return NEXUS_Playpump_Start_priv(p, false);
}

NEXUS_Error NEXUS_Playpump_StartMuxInput_priv(NEXUS_PlaypumpHandle playpump)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(playpump, NEXUS_Playpump);
    return NEXUS_Playpump_Start_priv(playpump, true);
}


void NEXUS_Playpump_Stop(NEXUS_PlaypumpHandle p)
{
    BERR_Code rc;
    NEXUS_P_PlaypumpPidChannel *play_pid;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    if(!p->state.running) {
        BDBG_WRN(("NEXUS_Playpump_Stop: %#lx playpump already stopped"));
        return;
    }
    p->consumerStarted = false; /* don't pause on Start */

    NEXUS_UnregisterEvent(p->playEventHandle);
    if (p->state.pacing) {
        NEXUS_Playpump_P_UninstallRangeErrIntHandler(p);
        rc = BXPT_Playback_ConfigPacing(p->xpt_play, BXPT_PacingControl_eStop);
        if (rc!=BERR_SUCCESS) {
            BDBG_ERR(("ignored error %#x from BXPT_Playback_ConfigPacing", rc));
        }
        p->state.pacing = false;
    }
    rc = NEXUS_Playpump_Flush(p);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}

    /* after we've flushed, we can state we're not running */
    p->state.running = false;
    p->state.muxInput = false;

    p->settings.playRate = NEXUS_NORMAL_PLAY_SPEED; /* reset a play rate */
#if B_HAS_MEDIA
    if (p->state.packetizer==b_play_packetizer_media) {
        b_pump_demux_stop(p->demux);
    } else
#endif
    if (p->state.packetizer==b_play_packetizer_crypto) {
        b_pump_crypto_stop(p->crypto);
    } else {
        BPVRlib_Feed_Stop(p->play_feed);
    }

    for(play_pid=BLST_S_FIRST(&p->pid_list);play_pid!=NULL;play_pid=BLST_S_NEXT(play_pid, link)) {
        NEXUS_Playpump_P_StopPid(p, play_pid);
    }
    rc = BXPT_Playback_DisablePacketizers(p->xpt_play);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}

    if (p->throttle_timer) {
        NEXUS_CancelTimer(p->throttle_timer);
        p->throttle_timer = NULL;
    }

    NEXUS_Transport_P_IncPowerDown(false);

    if(NEXUS_GetEnv("profile_playpump")) {
        NEXUS_Profile_Stop("NEXUS_Playpump");
    }

    return;
}

NEXUS_Error NEXUS_Playpump_Flush(NEXUS_PlaypumpHandle p)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);

    if (!p->state.running) {
        /* no need to flush if not started. if it is stopped, BPVRlib_Feed_Stop/Start further down
        will have the unintentional effect of restarting playback. */
        return 0;
    }
    BDBG_MSG(("flushing playback buffers"));

#if B_HAS_MEDIA
    if (p->state.packetizer==b_play_packetizer_media) {
        b_pump_demux_flush(p->demux);
    } else
#endif
    if (p->state.packetizer==b_play_packetizer_crypto) {
        b_pump_crypto_flush(p->crypto);
    }
    b_playpump_p_reset(p);

    if (p->state.packetizer!=b_play_packetizer_media) {
        BPVRlib_Feed_Stop(p->play_feed);
        rc = BPVRlib_Feed_Start(p->play_feed);
        if (rc) return BERR_TRACE(rc);
    }

    p->state.last_addr = NULL;
    p->state.last_size = 0;

    /* PI will clear pause, so restore it */
    if (p->paused) {
        rc = NEXUS_Playpump_SetPause(p, true);
        if (rc) return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
}

#if defined(HUMAX_PLATFORM_BASE)
void *NEXUS_Playpump_GetBufferStatus(NEXUS_PlaypumpHandle p)
{
	return p->state.last_addr;
}


#else
void *NEXUS_Playpump_GetBufferStatus(NEXUS_PlaypumpHandle p)
{
	BSTD_UNUSED(p);
	return NULL;
}
#endif

NEXUS_Error
NEXUS_Playpump_GetBuffer(NEXUS_PlaypumpHandle p, void **buffer, size_t *size)
{
    uint8_t *addr;
    unsigned freeSize, freeDesc;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);

    if (!p->state.running) {
        /* don't print error message, because this is a normal exit from thread processing */
        return NEXUS_UNKNOWN;
    }

    freeSize = BFIFO_WRITE_PEEK(&p->fifo);
    if(freeSize==0) {
        BDBG_MSG_FLOW(("Playback buffer is full, keep waiting"));
        goto keep_waiting;
    }
    freeDesc = BFIFO_WRITE_LEFT(&p->activeFifo);
    if(freeDesc==0) {
        BDBG_MSG_FLOW(("no chunks available, waiting for any"));
        goto keep_waiting;
    }
    BDBG_ASSERT(BFIFO_WRITE_PEEK(&p->pendingFifo));
    BDBG_ASSERT(BFIFO_WRITE(&p->activeFifo)==BFIFO_WRITE(&p->pendingFifo));
    *size = freeSize;
    p->state.last_size = freeSize;
    if(p->settings.mode==NEXUS_PlaypumpMode_eScatterGather) {
       if(freeSize>freeDesc*sizeof(NEXUS_PlaypumpDesc)) {
            *size = freeDesc*sizeof(NEXUS_PlaypumpDesc);
            p->state.last_size = *size;
       }
    }
    addr = BFIFO_WRITE(&p->fifo);
    BDBG_ASSERT(addr);

    p->state.last_addr = addr;
    *buffer = p->state.last_addr;
    BDBG_MSG_FLOW(("get_buffer %#lx, %d", (unsigned long)*buffer, *size));
    return 0;

keep_waiting:
    *buffer = NULL;
    *size = 0;
    return 0;

}

NEXUS_Error NEXUS_Playpump_WriteComplete(NEXUS_PlaypumpHandle p, size_t skip, size_t amount_used)
{
    BERR_Code rc;
    unsigned amount_to_commit = skip + amount_used;
    unsigned amount_to_skip = skip;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);

    if (amount_to_commit > p->state.last_size) {
        BDBG_ERR(("GetBuffer returned %u, WriteComplete called with %u,%u(%u)",  p->state.last_size, skip, amount_used, amount_to_commit));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (!p->state.running) {
        /* don't print error message, because this is a normal exit from thread processing */
        return NEXUS_UNKNOWN;
    }

    if (!p->state.last_addr) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    if (skip==0 && amount_used==0) {
        BDBG_MSG(("%#lx loop detected", (unsigned long)p)); /* player sends an empty entry if it's about to loop a stream */
    }

    /* make sure it's in the physical memory so the chip can read it */
    BDBG_MSG_FLOW(("write_complete %#lx:%u %u %#lx", (unsigned long)p->state.last_addr, skip, amount_used, (unsigned long)BFIFO_WRITE(&p->activeFifo)));

    rc = b_playpump_p_add_request(p, amount_to_skip, amount_to_commit - amount_to_skip, NULL);
    if (rc) return BERR_TRACE(rc);

    p->state.last_addr = NULL;
    p->state.last_size = 0;

    return 0;
}

NEXUS_Error NEXUS_Playpump_SubmitScatterGatherDescriptor(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpScatterGatherDescriptor *pDesc, size_t numDescriptors, size_t *pNumConsumed)
{
    BERR_Code rc;
    unsigned nFree;
    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BDBG_ASSERT(pNumConsumed);

    if (!p->state.running) {
        /* don't print error message, because this is a normal exit from thread processing */
        return NEXUS_UNKNOWN;
    }

    *pNumConsumed = 0;

    nFree = BFIFO_WRITE_PEEK(&p->activeFifo);
    if (nFree==0 || numDescriptors==0) {
        return 0;
    }
    else if (nFree < numDescriptors) {
        numDescriptors = nFree;
    }

    BDBG_MSG_FLOW(("submit_sg %#lx: %#lx", (unsigned long)p->state.last_addr, (unsigned long)BFIFO_WRITE(&p->activeFifo)));

    rc = b_playpump_p_add_request(p, 0, numDescriptors*sizeof(NEXUS_PlaypumpScatterGatherDescriptor), pDesc);
    if (rc) return BERR_TRACE(rc);

    *pNumConsumed = numDescriptors;

    return 0;
}

NEXUS_Error NEXUS_Playpump_GetStatus(NEXUS_PlaypumpHandle p, NEXUS_PlaypumpStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->started = p->state.running;
    pStatus->fifoSize = p->openSettings.fifoSize;
    pStatus->descFifoSize = p->openSettings.numDescriptors;
    pStatus->bytesPlayed = p->state.bytes_played;
    pStatus->bufferBase = p->buf;
    pStatus->fifoDepth = BFIFO_READ_LEFT(&p->fifo);
    pStatus->descFifoDepth = BFIFO_READ_LEFT(&p->activeFifo);
    pStatus->index=p->index;
    pStatus->pacingTsRangeError = p->state.pacingTsRangeError;
    pStatus->syncErrors = 0;
    pStatus->resyncEvents = 0;
    pStatus->streamErrors = 0;
    pStatus->mediaPtsType = NEXUS_PtsType_eInterpolatedFromInvalidPTS;
    pStatus->mediaPts = 0;

#if B_HAS_MEDIA
    if(p->state.packetizer==b_play_packetizer_media) {
        b_pump_demux_status(p->demux, pStatus);
    } else
#endif
    if(p->state.packetizer==b_play_packetizer_crypto) {
        b_pump_crypto_status(p->crypto);
    }
    return NEXUS_SUCCESS;
}

void NEXUS_Playpump_P_ConsumerStarted(NEXUS_PlaypumpHandle p)
{
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    if(p->state.running && !p->consumerStarted) {
        rc = BXPT_Playback_Resume(p->xpt_play);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc);}
    }
    p->consumerStarted=true; /* don't pause on Start */
    return;
}

static void
NEXUS_Playpump_P_SetPacketizerCfg(BXPT_Playback_PacketizeConfig *pkt_cfg, NEXUS_TransportType type, uint16_t oldpid, uint16_t newpid,
    const NEXUS_PlaypumpOpenPidChannelSettings *pSettings)
{
    BDBG_MSG(("mapping stream id %#x to pid %#x", (unsigned)oldpid, (unsigned)newpid));

    pkt_cfg->Pid = newpid;
    pkt_cfg->ChannelNum = 0; /* fill this in below, after calling bsettop_p_transport_lock */
    if(type == NEXUS_TransportType_eEs) {
        pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Es;
    } else {
        pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_Sid;
        pkt_cfg->FilterConfig.StreamId = oldpid;
    }
    if (type == NEXUS_TransportType_eVob) {
        if (pSettings->pidType == NEXUS_PidType_eAudio) {
            switch (pSettings->pidTypeSettings.audio.codec) {
            case NEXUS_AudioCodec_eLpcmDvd:
            case NEXUS_AudioCodec_eLpcmHdDvd:
            case NEXUS_AudioCodec_eLpcmBluRay:
                pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId = 0xA0 | (oldpid>>8);
                break;
            case NEXUS_AudioCodec_eAc3Plus:
                pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId = 0xC0 | (oldpid>>8);
                break;
            case NEXUS_AudioCodec_eAc3:
                pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId = 0x80 | (oldpid>>8);
                break;
            case NEXUS_AudioCodec_eDts:
                pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
                pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId = 0x88 | (oldpid>>8);
                break;
            default:
                if (oldpid >= 0x100) {
                    pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
                    pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
                    pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId =(oldpid>>8);
                }
                break;
            }
        }
        else if (oldpid >= 0x100) {
            pkt_cfg->PacketizerMode = BXPT_Playback_PacketizerMode_Pes_SidSubSid;
            pkt_cfg->FilterConfig.StreamIdAndSubStreamId.Id = (oldpid & 0xFF);
            pkt_cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId =(oldpid>>8);
        }
    }
}

/* OpenPidChannel and activate packetization if needed */
static NEXUS_Error
NEXUS_Playpump_P_OpenPid(NEXUS_PlaypumpHandle p, NEXUS_P_PlaypumpPidChannel *play_pid, unsigned oldpid, NEXUS_TransportType type,
    NEXUS_TransportType originalType, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings, bool forcedPacketizerPid, uint16_t packetizerPid)
{
    uint16_t newpid = 0;
    BERR_Code rc;
    NEXUS_PidChannelHandle pidChannel=NULL;
    NEXUS_PidChannelStatus pid_status;

    play_pid->packetizer.enable = false;
    play_pid->packetizer.active = false;
    switch (type) {
    default:
        pidChannel = NEXUS_PidChannel_P_Open(NULL, p, oldpid, &pSettings->pidSettings, p->settings.continuityCountEnabled);
        if(!pidChannel) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_pid_channel_ts; }
        pidChannel->status.transportType = type;
        pidChannel->status.originalTransportType = originalType;
        goto opened;

    case NEXUS_TransportType_eEs:
    case NEXUS_TransportType_eVob:
    case NEXUS_TransportType_eMpeg2Pes:
    case NEXUS_TransportType_eMpeg1Ps:
        break;
    }
    newpid = b_pid_map_alloc(&p->packetizer_map);
    if(newpid==0) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_newpid;
    }

    /* the remainder of this function handles packetization */
    NEXUS_Playpump_P_SetPacketizerCfg(&play_pid->packetizer.cfg, type, oldpid, forcedPacketizerPid?packetizerPid:newpid, pSettings);

    pidChannel = NEXUS_PidChannel_P_Open(NULL, p, forcedPacketizerPid?packetizerPid:newpid, &pSettings->pidSettings, p->settings.continuityCountEnabled);
    if(!pidChannel) { rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_pid_channel; }

    pidChannel->status.transportType = NEXUS_TransportType_eTs;
    pidChannel->status.originalTransportType = originalType;
    pidChannel->status.remappedPid = oldpid;

    rc = NEXUS_PidChannel_GetStatus(pidChannel, &pid_status);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_pid_status; }
    play_pid->packetizer.context = newpid - NEXUS_P_PACKETIZER_BASE;
    play_pid->packetizer.cfg.ChannelNum = pid_status.pidChannelIndex;
    BDBG_ASSERT(newpid >= NEXUS_P_PACKETIZER_BASE);

    play_pid->packetizer.enable = true;

opened:
    play_pid->pid_channel = pidChannel;
    if (p->state.running) {
        rc = NEXUS_Playpump_P_StartPid(p, play_pid);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_packetize; }
    }

    return 0;

err_packetize:
err_pid_status:
    NEXUS_PidChannel_P_Close(pidChannel);
err_pid_channel:
    if (newpid) {
        b_pid_map_free(&p->packetizer_map, newpid);
    }
err_newpid:
err_pid_channel_ts:
    play_pid->pid_channel = NULL;
    BDBG_ASSERT(rc);
    return rc;
}

static void
NEXUS_Playpump_P_ClosePid(NEXUS_PlaypumpHandle p, NEXUS_P_PlaypumpPidChannel *play_pid)
{
#if B_HAS_MEDIA
    if(p->use_media) {
        if(p->state.running) {
            b_pump_demux_remove_pid(p->demux, play_pid);
        }
        b_pump_demux_close_pid(p->demux, play_pid);
    }
#endif
    if (p->state.running) {
        NEXUS_Playpump_P_StopPid(p, play_pid);
    }

    if(play_pid->packetizer.enable) {
        b_pid_map_free(&p->packetizer_map, play_pid->packetizer.context+NEXUS_P_PACKETIZER_BASE);
    }
    NEXUS_PidChannel_P_Close(play_pid->pid_channel);
    return;
}

void
NEXUS_Playpump_GetDefaultOpenPidChannelSettings(NEXUS_PlaypumpOpenPidChannelSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_PidChannel_GetDefaultSettings(&pSettings->pidSettings);
    pSettings->pidType = NEXUS_PidType_eUnknown;
    pSettings->allowTimestampReordering = true;
    return;
}



static NEXUS_PidChannelHandle
NEXUS_Playpump_P_OpenPidChannel_MuxImpl(NEXUS_PlaypumpHandle p, unsigned pid, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings, bool forcedPacketizerPid, uint16_t packetizerPid)
{
    BERR_Code rc;
    NEXUS_P_PlaypumpPidChannel *play_pid;
    NEXUS_PlaypumpOpenPidChannelSettings settings;
    NEXUS_TransportType transportType;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    if(!pSettings) {
        NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&settings);
        pSettings = &settings;
    }

    play_pid = BKNI_Malloc(sizeof(*play_pid));
    if(!play_pid) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_alloc;}
    transportType = p->settings.transportType;
    play_pid->pid = pid;
    play_pid->settings = *pSettings;
#if B_HAS_MEDIA
    if(p->use_media) {
        transportType = NEXUS_TransportType_eMpeg2Pes;
        rc  = b_pump_demux_open_pid(p->demux, play_pid, pid, pSettings);
        if(rc!=NEXUS_SUCCESS) {
            goto err_media_open;
        }
        pid = play_pid->media_pid;
        if (p->state.running) {
            rc = b_pump_demux_add_pid(p->demux, play_pid);
            if(rc!=NEXUS_SUCCESS) {
                goto err_media_add;
            }
        }
    }
#endif
    rc = NEXUS_Playpump_P_OpenPid(p, play_pid, pid, transportType,
            p->settings.originalTransportType==NEXUS_TransportType_eUnknown?p->settings.transportType:p->settings.originalTransportType,
            pSettings, forcedPacketizerPid, packetizerPid);
    if (rc) { rc = BERR_TRACE(rc); goto err_pid_channel;}
    BLST_S_DICT_ADD(&p->pid_list, play_pid, NEXUS_P_PlaypumpPidChannel, pid_channel, link, err_duplicate);

    return play_pid->pid_channel;

err_duplicate:
    BDBG_ERR(("NEXUS_Playpump_OpenPidChannel: %#lx detected duplicate pid %#lx", (unsigned long)p, (unsigned long)play_pid->pid_channel));
err_pid_channel:
#if B_HAS_MEDIA
    if(p->use_media && p->state.running) {
        b_pump_demux_remove_pid(p->demux, play_pid);
    }
err_media_add:
    if(p->use_media) {
        b_pump_demux_close_pid(p->demux, play_pid);
    }
err_media_open:
#endif
    BKNI_Free(play_pid);
err_alloc:
    return NULL;
}

NEXUS_PidChannelHandle
NEXUS_Playpump_OpenPidChannel(NEXUS_PlaypumpHandle p, unsigned pid, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings)
{
    NEXUS_PidChannelHandle pidChannel;
    pidChannel = NEXUS_Playpump_P_OpenPidChannel_MuxImpl(p, pid, pSettings, false, 0);
    if (pidChannel) {
        NEXUS_OBJECT_REGISTER(NEXUS_PidChannel, pidChannel, Open);
    }
    return pidChannel;
}

NEXUS_PidChannelHandle
NEXUS_Playpump_OpenPidChannel_priv( NEXUS_PlaypumpHandle p, uint16_t src_pid, uint16_t dst_pid, const NEXUS_PlaypumpOpenPidChannelSettings *pSettings )
{
    NEXUS_ASSERT_MODULE();
    return NEXUS_Playpump_P_OpenPidChannel_MuxImpl(p, src_pid, pSettings, true, dst_pid);
}

NEXUS_Error
NEXUS_Playpump_ClosePidChannel(NEXUS_PlaypumpHandle p, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_OBJECT_UNREGISTER(NEXUS_PidChannel, pidChannel, Close);
    return NEXUS_Playpump_P_ClosePidChannel(p, pidChannel);
}

NEXUS_Error
NEXUS_Playpump_P_ClosePidChannel(NEXUS_PlaypumpHandle p, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Error rc;
    NEXUS_P_PlaypumpPidChannel *play_pid;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    BLST_S_DICT_FIND(&p->pid_list, play_pid, pidChannel, pid_channel, link);
    if(play_pid==NULL) {
        BDBG_WRN(("NEXUS_Playpump_ClosePidChannel: %#lx can't find pid:%#lx", (unsigned long)p, (unsigned long)pidChannel));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_not_found;
    }
    BLST_S_DICT_REMOVE(&p->pid_list, play_pid, pidChannel, NEXUS_P_PlaypumpPidChannel, pid_channel, link);
    BDBG_ASSERT(play_pid);
    NEXUS_Playpump_P_ClosePid(p, play_pid);
    BKNI_Free(play_pid);
    return NEXUS_SUCCESS;

err_not_found:
    return rc;
}

void
NEXUS_Playpump_CloseAllPidChannels(NEXUS_PlaypumpHandle p)
{
    NEXUS_P_PlaypumpPidChannel *play_pid;
    BDBG_OBJECT_ASSERT(p, NEXUS_Playpump);
    while(NULL!=(play_pid=BLST_S_FIRST(&p->pid_list))) {
        NEXUS_Playpump_ClosePidChannel(p, play_pid->pid_channel);
    }
    return;
}

/* This function was added for low-level control required for IP STB flow control. */
NEXUS_Error NEXUS_Playpump_SetPause( NEXUS_PlaypumpHandle p, bool paused )
{
    BERR_Code rc;
    if (paused) {
        rc = BXPT_Playback_Pause(p->xpt_play);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        rc = BXPT_Playback_Resume(p->xpt_play);
        if (rc) return BERR_TRACE(rc);
    }
    p->paused = paused;
    return 0;
}

NEXUS_Error NEXUS_Playpump_SuspendPacing( NEXUS_PlaypumpHandle p, bool suspended )
{
    BERR_Code rc;
    rc = BXPT_Playback_ConfigPacing(p->xpt_play, suspended?BXPT_PacingControl_eStop:BXPT_PacingControl_eStart);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

static void NEXUS_Playpump_P_PacingErr_isr( void *playpump, int param2 )
{
    NEXUS_PlaypumpHandle p = (NEXUS_PlaypumpHandle)playpump;
    BSTD_UNUSED(param2);
    p->state.pacingTsRangeError++;
    return;
}

static void NEXUS_Playpump_P_InstallRangeErrIntHandler(NEXUS_PlaypumpHandle p)
{
#ifndef BXPT_HAS_MULTICHANNEL_PLAYBACK
	    BERR_Code rc;
    rc = BINT_CreateCallback(&p->pacingErrIntCallback, g_pCoreHandles->bint, BXPT_Playback_GetIntId(p->xpt_play, BXPT_PbInt_eTsRangeErr),
        NEXUS_Playpump_P_PacingErr_isr, ( void * ) p, 0 );
    if (!rc) {
        rc = BINT_EnableCallback(p->pacingErrIntCallback);
    }
    if (rc) {rc = BERR_TRACE(rc);}
#else
#warning mcpb update for 7445
	BSTD_UNUSED(p);
#endif
}

static void NEXUS_Playpump_P_UninstallRangeErrIntHandler(NEXUS_PlaypumpHandle p)
{
    (void)BINT_DestroyCallback(p->pacingErrIntCallback);
}

void NEXUS_Playpump_P_CCError_isr(void *context, int param)
{
    NEXUS_PlaypumpHandle p = context;
    BSTD_UNUSED(param);
    NEXUS_ParserBand_P_CountCcErrors_isr();
    NEXUS_IsrCallback_Fire_isr(p->ccErrorCallback);
}

void NEXUS_Playpump_P_TeiError_isr(void *context, int param)
{
    NEXUS_PlaypumpHandle p = context;
    BSTD_UNUSED(param);
    NEXUS_IsrCallback_Fire_isr(p->teiErrorCallback);
}

static NEXUS_Error NEXUS_Playpump_P_SetInterrupts(NEXUS_PlaypumpHandle p, const NEXUS_PlaypumpSettings *pSettings)
{
    BERR_Code rc;

    /* only enable certain interrupts when their callbacks are desired. this helps typical system performance. */
    if (pSettings->ccError.callback) {
        if (!p->ccErrorInt) {
            BDBG_MSG(("create playpump %d cc callback", p->index));
#if NEXUS_PARSER_BAND_CC_CHECK
            rc = BINT_CreateCallback(&p->ccErrorInt, g_pCoreHandles->bint,
                BXPT_Playback_GetIntId(p->xpt_play, BCHP_XPT_PB0_INTR_PARSER_CONTINUITY_ERROR_SHIFT),
                NEXUS_Playpump_P_CCError_isr, p, 0);
#else
            rc = BINT_CreateCallback(&p->ccErrorInt, g_pCoreHandles->bint,
                                     BXPT_INT_ID_PBP_CONTINUITY_ERROR( p->index ),
                                     NEXUS_Playpump_P_CCError_isr, p, 0);
#endif
            if (rc) return BERR_TRACE(rc);
            rc = BINT_EnableCallback(p->ccErrorInt);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (p->ccErrorInt) {
        (void)BINT_DestroyCallback(p->ccErrorInt);
        p->ccErrorInt = NULL;
    }

    /* only enable certain interrupts when their callbacks are desired. this helps typical system performance. */
    if (pSettings->teiError.callback) {
        if (!p->teiErrorInt) {
            BDBG_MSG(("create playpump %d tei callback", p->index));
#ifndef BXPT_HAS_MULTICHANNEL_PLAYBACK
            rc = BINT_CreateCallback(&p->teiErrorInt, g_pCoreHandles->bint,
                BXPT_Playback_GetIntId(p->xpt_play, BCHP_XPT_PB0_INTR_PARSER_TRANSPORT_ERROR_SHIFT),
                NEXUS_Playpump_P_TeiError_isr, p, 0);
            if (rc) return BERR_TRACE(rc);
            rc = BINT_EnableCallback(p->teiErrorInt);
            if (rc) return BERR_TRACE(rc);
#else
#warning need mcpb update for 7445
#endif
        }
    }
    else if (p->teiErrorInt) {
        (void)BINT_DestroyCallback(p->teiErrorInt);
        p->teiErrorInt = NULL;
    }

    NEXUS_IsrCallback_Set(p->ccErrorCallback, &pSettings->ccError);
    NEXUS_IsrCallback_Set(p->teiErrorCallback, &pSettings->teiError);

    return 0;
}

NEXUS_Error NEXUS_Playpump_GetAllPassPidChannelIndex(
    NEXUS_PlaypumpHandle playpump,
    unsigned *pHwPidChannel
    )
{
    BDBG_OBJECT_ASSERT(playpump, NEXUS_Playpump);
    BDBG_ASSERT(pHwPidChannel);

    *pHwPidChannel = BXPT_GET_PLAYBACK_ALLPASS_CHNL( playpump->index );
    return 0;
}

void NEXUS_Playpump_IsTransportTypeSupported( NEXUS_PlaypumpHandle playpump, NEXUS_TransportType transportType, bool *pIsSupported )
{
    BDBG_OBJECT_ASSERT(playpump, NEXUS_Playpump);
    switch (transportType) {
    case NEXUS_TransportType_eEs:
    case NEXUS_TransportType_eTs:
    case NEXUS_TransportType_eMpeg2Pes:
    case NEXUS_TransportType_eVob:
    case NEXUS_TransportType_eMpeg1Ps:
#if B_HAS_DSS
    case NEXUS_TransportType_eDssEs:
    case NEXUS_TransportType_eDssPes:
#endif
#if B_HAS_ASF
    case NEXUS_TransportType_eAsf:
#endif
#if B_HAS_AVI
    case NEXUS_TransportType_eAvi:
#endif
#if B_HAS_FLV
    case NEXUS_TransportType_eFlv:
#endif
#if B_HAS_RMFF
    case NEXUS_TransportType_eRmff:
#endif
    case NEXUS_TransportType_eWav:
    case NEXUS_TransportType_eMp4Fragment:
    case NEXUS_TransportType_eOgg:
    case NEXUS_TransportType_eFlac:
    case NEXUS_TransportType_eAiff:
    case NEXUS_TransportType_eApe:
        *pIsSupported = true;
        break;
    default:
        /* includes eMp4 and eMkv */
        *pIsSupported = false;
        break;
    }
}
