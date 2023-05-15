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
 * $brcm_Workfile: nexus_recpump.c $
 * $brcm_Revision: 136 $
 * $brcm_Date: 10/16/12 1:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_recpump.c $
 * 
 * 136   10/16/12 1:34p erickson
 * SW7435-375: transport does not depend on security
 * 
 * 135   10/15/12 12:01p erickson
 * SW7435-375: transport module should not call into security module
 * 
 * 134   10/5/12 11:13a mphillip
 * SW7425-3963: Merge interrupt changes to main
 * 
 * SW7425-3963/1   9/20/12 4:16p mphillip
 * SW7425-3963: Disable rave interrupts while processing, to avoid
 *  interrupt flooding in records
 * 
 * 133   9/4/12 12:32p erickson
 * SW7346-728: only dealloc what we have allocated. don't reuse user
 *  buffer for nullifyVct.
 * 
 * 132   8/9/12 4:57p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 131   8/9/12 3:20p erickson
 * SW7425-3725: silence warning
 * 
 * 130   7/31/12 8:51a erickson
 * SW7420-2085: inc refcnt on pidchannel added to recpump
 * 
 * 129   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 128   6/4/12 4:32p vsilyaev
 * SW7420-2313: Use reference counter for the NEXUS_Dma objects
 * 
 * 127   5/9/12 11:42a ahulse
 * SW7425-2744: On record stop allow data less than atom size to fire
 *  dataReadyCallback
 * 
 * 126   5/8/12 7:05p hongtaoz
 * SW7425-2906: fixed segfault if adjustTimestampUsingPcrs is off;
 * 
 * 125   5/7/12 4:28p gmullen
 * SW7425-2906: Merged to main
 * 
 * 124   4/19/12 4:24p jtna
 * SW7425-2893: add NEXUS_RecpumpSettings.dropBtpPackets
 * 
 * SW7425-2906/2   4/30/12 10:03a gmullen
 * SW7425-2906: Changed PCR PID channel to Nexus handle
 * 
 * SW7425-2906/1   4/26/12 5:34p gmullen
 * SW7425-2906: Support adjusting record timestamps with PCR information
 * 
 * 123   4/6/12 2:02p vsilyaev
 * SW7425-2663: When bandHold enabled don't use dataReadyThreshold to set
 *  hi/lo rave thresholds
 * 
 * 122   4/4/12 4:31p ahulse
 * SW7425-2744: revert change
 * 
 * 121   4/3/12 4:07p ahulse
 * SW7425-2744: On record stop allow data less than atom size to fire
 *  dataReadyCallback
 * 
 * 120   3/15/12 1:38p erickson
 * SW7346-728: add user-allocated data/index buffer pointer in
 *  NEXUS_RecpumpOpenSettings
 * 
 * 119   2/27/12 4:38p erickson
 * SW7425-2487: fire correct ITB/CDB overflow callback, don't flush ITB to
 *  help app.
 * 
 * 118   2/23/12 3:16p mphillip
 * SW7425-2391: Revert previous change due to issues on 7335, 7125, and
 *  7208
 * 
 * 117   2/17/12 3:36p mphillip
 * SW7425-2391: Modify recpump Rave interrupt handling to be edge-based
 * 
 * 116   1/19/12 4:00p erickson
 * SW7425-2192: add NEXUS_TransportCapabilities.numTpitPids
 * 
 * 115   11/17/11 10:25a erickson
 * SW7420-2076: set NEXUS_RecpumpStatus.hasIndex
 * 
 * 114   11/4/11 4:56p erickson
 * SW7346-523: fix NEXUS_TransportTimestampType_e32_Mod300
 * 
 * 113   10/31/11 7:48p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/2   10/28/11 6:52p bandrews
 * SW7231-391: merge from main
 * 
 * 112   10/28/11 1:27p erickson
 * SW7346-523: extend NEXUS_TransportTimestamp to handle 32 bit timestamps
 *  and to be more explicit about 28/30 bit timestamps
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 111   10/6/11 6:05p jtna
 * SW7425-1218: use new BXPT_Rave_AllocCx() to alloc rave contexts
 * 
 * 110   9/8/11 12:17p jtna
 * SW7425-1218: add NEXUS_RecpumpOpenSettings.data/index.heap
 * 
 * 109   8/25/11 3:29p erickson
 * SW7346-454: add NEXUS_RecpumpStatus.rave.index
 * 
 * 108   7/7/11 4:45p erickson
 * SW7346-275: add NEXUS_RecpumpSettings.timestamp.parityCheckDisable
 * 
 * 107   6/16/11 2:22p erickson
 * SW7420-1148: cannot auto-cleanup pid channels in recpump because of
 *  object database auto-cleanup
 * 
 * 106   5/27/11 2:58p erickson
 * SW7346-203: allow configuration of 4 start code ranges for
 *  NEXUS_PidType_eOther indexing
 * 
 * 105   5/13/11 2:06p erickson
 * SW7335-957: add API for BXPT_Rave_SetTpitEcms
 * 
 * 104   5/12/11 3:42p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 103   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 102   2/7/11 3:36p erickson
 * SW7335-1036: fix platforms w/o crypto
 *
 * 101   2/7/11 2:23p erickson
 * SW7335-1036: add
 *  NEXUS_Recpump_GetDataBufferWithWrap/NEXUS_Recpump_GetIndexBufferWithWr
 *  ap
 *
 * 100   2/7/11 10:42a erickson
 * SW7342-333: add NEXUS_NUM_TPIT_PIDS
 *
 * 99   1/20/11 2:15p erickson
 * SW7420-1392: add NEXUS_RecpumpOpenSettings.useSecureHeap
 *
 * 98   1/18/11 5:25p vsilyaev
 * SW7420-1113: Improved handling of NEXUS_RecpumpFlowControl
 *
 * 97   1/18/11 3:47p vsilyaev
 * SW7422-115: Used BXPT_Rave_IndexerSettings.SvcMvcEn
 *
 * 96   1/10/11 6:45p randyjew
 * SW7420-1113: for NEXUS_RecpumpFlowControl_eAuto , change true to false.
 *  that should restore behavior.
 *
 * 95   1/5/11 1:43p vsilyaev
 * SW7420-1113: Added 'Auto' option for bandHoldEn, enables flow control
 *  if recpump is sourced by a playpump
 *
 * 94   1/5/11 12:33p vsilyaev
 * SW7422-115: Enable 12 byte record for H.264 SVC/MVC video
 *
 * 93   12/28/10 5:42p vsilyaev
 * SW7425-39: Expect that PidChannel's is internally packetized to MPEG-2
 *  TS
 *
 * 92   12/16/10 5:13p erickson
 * SW7422-151: add NEXUS_RecpumpAddPidChannelSettings.useRPipe
 *
 * 91   12/9/10 3:32p erickson
 * SW7420-1308: use unconditional nexus_dma_types.h in the public API
 *
 * 90   11/16/10 4:56p vsilyaev
 * SW7422-14: Fixed startcode detection for multiple pids
 *
 * 89   11/15/10 11:37a erickson
 * SW7335-936: I've eliminated the BDBG_ERR. it is normal behavior. I also
 *  eliminated the BDBG_MSG which made it look as if something was
 *  partially wrong. there is a simple and harmless race between XPT
 *  hitting the threshold and Nexus completing data.
 *
 * 88   11/2/10 11:31a erickson
 * SW7405-4982: fix dataReadyThreshold comparison. improve debug
 *  throughout.
 *
 * 87   10/28/10 5:17p erickson
 * SW7125-667: only request 9 bytes of payload if
 *  BXPT_HAS_STARTCODE_BUFFER_WORKAROUND is defined
 *
 * 86   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 85   10/11/10 5:03p erickson
 * SW7420-1113: added NEXUS_RecpumpSettings.bandHold
 *
 * 84   10/5/10 5:24p erickson
 * SW7420-1113: set lower thresholds to non-zero if rec_cfg.BandHoldEn set
 *
 * 83   10/1/10 9:51a erickson
 * SW7420-1009: support NEXUS_ANY_ID
 *
 * 82   9/24/10 1:23p erickson
 * SW7420-1113: check that all pid channels are or aren't playback. add
 *  band hold logic, but leave it disabled with a usage comment.
 *
 * 81   9/13/10 5:11p erickson
 * SW7125-632: add SCD 0x00 for MPEG if BXPT_HAS_AVS system
 *
 * 80   8/10/10 4:08p erickson
 * SW7420-934: rename NEXUS_Recpump_DataWriteComplete/IndexWriteComplete
 *  to NEXUS_Recpump_DataReadComplete/IndexReadComplete
 *
 * 79   8/6/10 9:58a erickson
 * SW7400-2858: added NEXUS_RecpumpSettings.data.useBufferSize
 *
 * 78   7/27/10 10:24a erickson
 * SW7342-234: fix NEXUS_HAS_DMA && NEXUS_HAS_SECURITY
 *
 * 77   6/21/10 4:32p erickson
 * SW7405-4249: added index option for NEXUS_PidType_eOther for indexing
 *  audio-only or scrambled streams
 *
 * 76   6/10/10 10:55p erickson
 * SW7550-398: fix warning
 *
 * 75   6/7/10 6:23p mphillip
 * SW7550-398: Update examples and transport code to support non-DMA
 *  encrypted PVR
 *
 * 74   6/3/10 1:55p erickson
 * SW3548-2939: fix warning if no security
 *
 * 73   5/28/10 2:06p vishk
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 72   5/27/10 6:22p VISHK
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 71   5/27/10 6:20p VISHK
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 70   5/27/10 3:09p VISHK
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 69   5/26/10 4:10p VISHK
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 68   5/18/10 3:39p mphillip
 * SW7550-398: Allow ES data to be read back as TS data
 *
 * 67   5/13/10 3:05p vishk
 * SW7405-3896: re-code nexus_recpump.c to use internal linked list
 *  instead of an array of size NEXUS_TOTAL_RECPUMP_PIDCHANNELS
 *
 * 66   4/30/10 11:46a erickson
 * SW7405-4105: RAVE can now deliver all 8 bytes of payload following SC.
 *  Set EsCount appropriately.
 *
 * 65   4/16/10 11:30a erickson
 * SW3548-2891: fix NEXUS_Recpump_Close logic
 *
 * 64   4/14/10 12:30p erickson
 * SW7550-372: if NEXUS_RAVE_MEMC_BLOCK_SIZE is large, we may need to
 *  decrement by more than one atomSize or packetSize to fit within bounds
 *
 * 63   2/15/10 1:19p rjain
 * SW7550-243: Increasing NEXUS_TOTAL_RECPUMP_PIDCHANNELS to 64
 *
 * 62   2/10/10 10:51a erickson
 * SW7335-671: use BXPT_RAVE_WRAP_THRESH
 *
 * 61   1/27/10 4:02p erickson
 * SW7400-2664: added NEXUS_RecpumpSettings.tpit options
 *
 * 60   1/20/10 5:15p erickson
 * SW7550-159: distinguish between NEXUS_RAVE_MEMC_BLOCK_SIZE of 256 bytes
 *  and NEXUS_RAVE_THRESHOLD_UNITS which may not be 256 bytes. neither are
 *  related to XC packet size (which is 200). allow a dataReadyThreshold
 *  below 512 bytes. this allows apps to get interrupts for one packet.
 *  temporarily force CdbUpperThreshold and ItbUpperThreshold to be at
 *  least 1. 0 should work.
 *
 * 59   1/8/10 10:11a erickson
 * SW7400-2645: refactored TPIT filter api for greater flexibility and
 *  better use of HW. one TPIT indexer used per recpump. filters can be
 *  set/reset to pid channels, whether recpump is started or stopped.
 *
 * 58   12/29/09 12:51p erickson
 * SW7550-146: allow security module to be initialized after the transport
 *  module if HW requires it
 *
 * 57   12/2/09 2:28p erickson
 * SW7400-2625: moved tpit indexer code to handle non-ts transport types
 *
 * 56   11/16/09 1:53p erickson
 * CDSTRMANA-294: allow capture of start codes that are not slices for
 *  MPEG streams
 *
 * 55   11/9/09 5:07p erickson
 * SW7405-3354: added NEXUS_RecpumpSettings.securityDmaDataFormat, default
 *  to NEXUS_DmaDataFormat_eMpeg
 *
 * 54   11/6/09 9:54a erickson
 * SW7550-60: fix logic for MpegMode and DSS
 *
 * 53   11/3/09 10:59a erickson
 * SW7550-60: add NEXUS_RecpumpSettings.outputTransportType for PES and ES
 *  record support
 *
 * 52   10/12/09 3:13p erickson
 * SW7335-587: allow NEXUS_Recpump_GetStatus to return all status fields,
 *  even if not started or no index
 *
 * 51   8/17/09 10:54a erickson
 * PR56657: move BXPT_Rave_NullifyVCT. fix no-index records.
 *
 * 50   7/16/09 12:43p erickson
 * PR56657: added NEXUS_RecpumpOpenSettings.nullifyVct
 *
 * 49   6/24/09 5:48p katrep
 * PR56058: Compiler warning
 *
 * 48   6/23/09 8:49a erickson
 * PR56058: test dataReadyThreshold before firing dataReady callback, with
 *  an exception for wrap-around.
 *
 * 47   6/9/09 3:09p jtna
 * PR55767: refactor timestamp record
 *
 * 46   6/9/09 10:54a erickson
 * PR55843: added NEXUS_RecpumpStatus.data.bufferBase and
 *  .index.bufferBase
 *
 * 45   5/29/09 3:05p erickson
 * PR54002: rework TPIT api to support simultaneous SCD indexing and TPIT
 *  filtering on the same pid
 *
 * 44   5/4/09 4:36p erickson
 * PR54770: fix warnings
 *
 * 43   4/14/09 2:30p erickson
 * PR54002: added tpit filtering mode
 *
 * 42   3/30/09 10:35a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 41   3/20/09 1:35p erickson
 * PR53466: add proper checks in NEXUS_Recpump_Close for possible failed
 *  NEXUS_Recpump_Open
 *
 * 40   2/23/09 10:41a erickson
 * PR52366: set NEXUS_RecpumpStatus.index.bytesRecorded
 *
 * 39   1/16/09 4:58p erickson
 * PR50099: backing out incorrect workaround
 *
 * 38   1/12/09 2:39p erickson
 * PR50099: re-enable the index upper thres isr as a workaround
 *
 * 37   1/8/09 12:48a erickson
 * PR50882: r->indexing must be set to true or false for each call to
 *  Start
 *
 * 36   12/18/08 3:05p jtna
 * PR50083: remove unnecessary code
 *
 * 35   12/18/08 2:44a erickson
 * PR50347: index-only record uses BXPT_RaveSoftMode_eIndexOnlyRecord mode
 *
 * 34   12/17/08 2:34p erickson
 * PR50461: modify atomSize restriction on the last GetBuffer
 *
 * 33   12/17/08 1:39p erickson
 * PR50461: lift atomSize restriction on the last GetBuffer. this allows
 *  all data to be recorded.
 *
 * 32   12/17/08 12:04a erickson
 * PR50347: added NEXUS_RecpumpOpenSettings.dummyRecpump for conserving
 *  memory on index-only record
 *
 * 31   12/1/08 12:48p erickson
 * PR49676: retrieve transportType and timestampType correctly for
 *  playback pid channels
 *
 * 30   10/29/08 11:59a erickson
 * PR48471: fix AVS ScRange assignment
 *
 * 29   10/22/08 11:39a erickson
 * PR48216: allow transport to run with no dma or security modules
 *
 * 28   10/17/08 3:23p katrep
 * PR48011: Add support to generate index using pid value during all pass
 *  record.
 *
 * 27   10/13/08 11:55a erickson
 * PR47572: ensure that CDB and ITB depth is always <= size. if not,
 *  assert on GetBuffer and cause GetStatus to fail.
 *
 * 26   8/27/08 11:55a katrep
 * PR45674: Fixed compile error
 *
 * 25   8/27/08 11:46a katrep
 * PR45674: Fixed the compiler error in 64MB mode.
 *
 * 24   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 23   7/18/08 10:20a erickson
 * PR44919: fix warnings
 *
 * 22   7/17/08 4:17p erickson
 * PR44919: allow NEXUS_NUM_RECPUMPS == 0
 *
 * 21   7/2/08 2:54p erickson
 * PR41619: NEXUS_Recpump_GetBuffer should limit reported size based on
 *  atomSize, not dataReadyThreshold
 *
 * 20   7/1/08 8:02p katrep
 * PR44478: Improper adjustment of data ready threshold caused the DSS
 *  recording to get stuck.
 *
 * 19   6/26/08 9:18p vsilyaev
 * PR 41869: Fixed data-flow for the PVR encryption
 *
 * 18   6/26/08 3:18p vsilyaev
 * PR 41869: Added encryption for DMA
 *
 * 17   6/25/08 5:50p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 16   5/28/08 11:00a erickson
 * PR34925: fix warning
 *
 * 15   5/16/08 11:24a vsilyaev
 * PR 41869: Removed unnecessary debug output
 *
 * 14   5/14/08 5:17p vsilyaev
 * PR 41869: Added DMA to support stream encryption
 *
 * 13   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 12   4/16/08 10:03a erickson
 * PR41865: allow NEXUS_RecpumpOpenSettings.data.atomSize of 0. explain
 *  +68 more in default bufferSize.
 *
 * 11   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 10   4/7/08 11:35a jgarrett
 * PR 41362: Resolving stop with no pid channels added
 *
 * 9   3/31/08 1:12p erickson
 * PR41075: added BDBG_OBJECT
 *
 * 8   3/12/08 2:31p katrep
 * PR40514:Add support for AVS record
 *
 * 7   3/6/08 3:08p erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 6   2/25/08 11:31a erickson
 * PR39750: remove bad assert
 *
 * 5   2/22/08 10:51a erickson
 * PR39862: add timestamp for buffer calc
 *
 * 4   2/8/08 12:02p vsilyaev
 * PR 38682: Fixed warning
 *
 * 3   2/7/08 10:20a erickson
 * PR39384: added recpump dataReadyThreshold and atomSize. clarified
 *  dataReadyCallback behavior.
 *
 * 2   1/22/08 9:49a erickson
 * PR34925: remove numDescriptors. re-add code to flush ITB on ITB
 * overflow.
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#include "priv/nexus_core.h"
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
#include "nexus_dma.h"
#include "priv/nexus_dma_priv.h"
#endif
#include "nexus_client_resources.h"

/*
security & dma options
1) NEXUS_ENCRYPTED_DVR_WITH_M2M - nexus uses M2M DMA to decrypt
2) otherwise - nexus uses keyslots to decrypt
There is no current option for NEXUS_HAS_DMA-only or internal hooks for SW-based decrypt (ala playpump external crypto)
*/

BDBG_MODULE(nexus_recpump);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

struct NEXUS_RecpumpFlow {
    bool pending; /* if true, then a callback has been sent and a complete is pending */
    bool irqEnabled; /* only accessed in the ISR context */
    NEXUS_RecpumpHandle recpump; /* link back to parent */
    BINT_CallbackHandle irq;
    BKNI_EventHandle event;
    NEXUS_EventCallbackHandle eventCallback;
    NEXUS_TaskCallbackHandle dataReadyCallback, overflowCallback;
    size_t bufferSize;
    void *bufferBase;
    unsigned lastGetBuffer; /* remember total size (size + size2) returned by last GetBuffer. this validates the WriteComplete call. */
    unsigned dataReadyThreshold; /* copy from Settings for use in flow */
    unsigned atomSize; /* copy from Settings for use in flow */
    uint64_t bytesRecorded;
};

#define NEXUS_P_FLOW_NAME(flow) ((flow)==&(flow)->recpump->data ? "CDB" : "ITB")
#define NEXUS_TOTAL_RECPUMP_PIDCHANNELS 64

/* XPT RAVE is configured with a fixed size for wrapping */
#define NEXUS_RAVE_MEMC_BLOCK_SIZE BXPT_RAVE_WRAP_THRESH

typedef enum NEXUS_Recpump_P_BufferState {
    NEXUS_Recpump_P_BufferState_eApplicationNotified,
    NEXUS_Recpump_P_BufferState_eWaitingApplication,
    NEXUS_Recpump_P_BufferState_eNoData,
    NEXUS_Recpump_P_BufferState_eError,
    NEXUS_Recpump_P_BufferState_eTerminated,
    NEXUS_Recpump_P_BufferState_eMax
} NEXUS_Recpump_P_BufferState;

typedef struct NEXUS_Recpump_P_PidChannel {
    struct {
        bool enabled;
        NEXUS_RecpumpTpitFilter filter;
        unsigned index;
    } tpit;
    NEXUS_RecpumpAddPidChannelSettings settings;
    NEXUS_PidChannelHandle  pidChn;
    BLST_S_ENTRY(NEXUS_Recpump_P_PidChannel) link;
    unsigned playback;
} NEXUS_Recpump_P_PidChannel;

struct NEXUS_Recpump {
    NEXUS_OBJECT(NEXUS_Recpump);
    unsigned tindex; /* index of pTransport->recpump[] */
    NEXUS_RecpumpOpenSettings openSettings;
    NEXUS_RecpumpSettings settings;
    BLST_S_HEAD(NEXUS_Recpump_P_PidChannels, NEXUS_Recpump_P_PidChannel) pid_list;
    unsigned pidChannelCnt; /* total number of pid channels added */
    BXPT_RaveIdx_Handle tpitIdx;
    unsigned tpitCount;
    BXPT_RaveIdx_Handle scdIdx;
    unsigned scdPidCount;
    bool indexing; /* was indexing started? */
    bool playback;

    NEXUS_TransportType inputTransportType;

    BXPT_RaveCx_Handle rave_rec;
    unsigned rave_rec_index;
    BXPT_RaveCx_Handle extra_rave_rec;
    struct { /* buffer pointers if recpump does its own alloc */
        void *data;
        void *index;
    } rave_rec_mem, extra_rave_rec_mem;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    NEXUS_PidChannelHandle indexPidChannel; /* if set, index this pid channel at Start */
    NEXUS_VideoCodec indexCodec; /* only valid if indexPidChannel set */
    uint16_t indexPid;			 /* if indexPid < 0x1fff then  set index using pid value,coming on indexPidChannel */
					 /* this is set with allpass record with index on a single pid */
    BXPT_RaveIdx_Handle rave_indx;
    BXPT_RaveIdx_Handle rave_indx_tpit;

    bool isTpitIndexType;	/* if index is tpit index type, then this value is true. the scd type is false */
#endif
    
    struct NEXUS_RecpumpFlow data, index;
    enum {
        Ready,
        Done,   /* besttop_recpump_stop has been recognized and we're exiting */
        Terminated  /* abnormal termination */
    } rave_state;
    bool started; /* true if Start was called */
    bool actuallyStarted; /* true if actually started - which requires Start to be called and at least one pid channel */
    bool dataStopped; /* true if StopData was called. sets to false on start */
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    bool cryptoActive; /* true if data encryption is required */
    /*
     * if crypto active there is also staging area in the FIFO - area where data is recored but not yet encrypted
     *      |---------|++++++++|==========|~~~~~~~~~~~~|-------------|
     *   FifoStart  CdbRead  CryptoTail CryptoHead CdbWrite    FifoEnd(Wrap)
     *   Legend:
     *     '-' - no data
     *     '+' - recorded and encrypted data (could be consumed by an application)
     *     '=' - recorded that currently being encrypted
     *     '~' - recorded that is not yet encrypted
     *
     *      Wrap of encrypted data
     *      |++++++++|==========|~~~~~~~~~~~~|-------------|+++++++++++++++|
     *   FifoStart CryptoTail CryptoHead CdbWrite        CdbRead      FifoEnd(Wrap)
     *
     *      Wrap of data to encrypt
     *      |========|~~~~~~~~~~~~|-------------|++++++++++++++|===========|
     *   FifoStart CryptoHead CdbWrite        CdbRead      CryptoTail   FifoEnd(Wrap)
     */
    struct {
        void *head, *tail; /* tail ... head -> distance of currently outstanding DMA transaction, it could wrap over the buffer boundary, tail==head  means no DMA outstanding */
        NEXUS_DmaJobBlockSettings blocks[2];
        NEXUS_DmaJobHandle job; /* job is only created when crypto started */
        size_t packetSize;
        BKNI_EventHandle event;
        NEXUS_EventCallbackHandle callback;
    } crypto;
#endif
};


static NEXUS_Error NEXUS_Recpump_P_Start(NEXUS_RecpumpHandle r);
static NEXUS_Error NEXUS_Recpump_P_StartFlow(struct NEXUS_RecpumpFlow *flow);
static void        NEXUS_Recpump_P_StopFlow(struct NEXUS_RecpumpFlow *flow);
static NEXUS_Recpump_P_BufferState NEXUS_Recpump_P_TestDataReadyCallback(struct NEXUS_RecpumpFlow *flow);

#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_RECPUMP_PID_IS_INDEX(PSETTINGS) \
    ( (((PSETTINGS)->pidType == NEXUS_PidType_eVideo) && ((PSETTINGS)->pidTypeSettings.video.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eAudio) && ((PSETTINGS)->pidTypeSettings.other.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eOther) && ((PSETTINGS)->pidTypeSettings.other.index)) \
    )
#else
#define NEXUS_RECPUMP_PID_IS_INDEX(PSETTINGS) \
    ( (((PSETTINGS)->pidType == NEXUS_PidType_eVideo) && ((PSETTINGS)->pidTypeSettings.video.index)) || \
      (((PSETTINGS)->pidType == NEXUS_PidType_eOther) && ((PSETTINGS)->pidTypeSettings.other.index)) \
    )
#endif

#define B_MAX(x,y) (((x)>=(y))?(x):(y))

void NEXUS_Recpump_GetDefaultOpenSettings(NEXUS_RecpumpOpenSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    /* The default bufferSize is based on the least common multiple of a 188 byte transport packet and 4096 atomSize (for O_DIRECT file i/o).
    Then, multiply by 12 to get 12 of these LCM's. That's the default buffer.
    Finally, add 68 in order to anticipate the NEXUS_RAVE_MEMC_BLOCK_SIZE replacement logic in NEXUS_Recpump_P_Start. 256-188 = 68. This is not a crucial step. We could have chosen
    to not add it, but it would have meant that the final buffer would be less than 12 LCM's after the NEXUS_RAVE_MEMC_BLOCK_SIZE replacement logic. */
    pSettings->data.bufferSize = ((188/4)*4096*12) + 68;
    pSettings->data.alignment = 12; /* 4K alignment */
    pSettings->data.dataReadyThreshold = (188/4)*4096; /* threshold is the least common multiple of a 188 byte transport packet and 4096 atomSize */
    pSettings->data.atomSize = 4096; /* typical value for disk direct I/O */

    /* 6*4 is a size of the single 6-word SCT entry. 16 SCT's per atom. index.bufferSize must be multiple of this. */
    pSettings->index.bufferSize = 6*4*16*48;
    pSettings->index.alignment = 7; /* 128-Byte alignment */
    pSettings->index.dataReadyThreshold = 1024;
    pSettings->index.atomSize = 0; /* typically not needed. index write benefits from stdio buffering. */
}

void NEXUS_Recpump_GetDefaultSettings( NEXUS_RecpumpSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_CallbackDesc_Init(&pSettings->data.dataReady);
    NEXUS_CallbackDesc_Init(&pSettings->data.overflow);
    NEXUS_CallbackDesc_Init(&pSettings->index.dataReady);
    NEXUS_CallbackDesc_Init(&pSettings->index.overflow);
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    pSettings->securityDmaDataFormat = NEXUS_DmaDataFormat_eMpeg;
#endif
    pSettings->outputTransportType = NEXUS_TransportType_eTs;
    pSettings->bandHold = NEXUS_RecpumpFlowControl_eAuto;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    pSettings->localTimestamp = false;
#endif
    return;
}

#if NEXUS_NUM_RECPUMPS
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
static void
NEXUS_Recpump_P_DmaCallback(void *context)
{
    NEXUS_RecpumpHandle pump=context;
    BDBG_OBJECT_ASSERT(pump, NEXUS_Recpump);
    if(pump->crypto.tail != pump->crypto.head) { /*  DMA was pending */
        bool pending = pump->data.pending;
        BDBG_MSG(("NEXUS_Recpump_P_DmaCallback:%#lx DMA completed (%#lx:%#lx) %s", (unsigned long)pump, (unsigned long)pump->crypto.tail, (unsigned long)pump->crypto.head, pending?"":"Waiting For Data"));
        pump->crypto.tail = pump->crypto.head;
        if(NEXUS_Recpump_P_TestDataReadyCallback(&pump->data)!=NEXUS_Recpump_P_BufferState_eApplicationNotified && !pending ) {
            BDBG_MSG(("dma complete but there is no data in the record buffer"));
        }
    }
    return;
}
#endif

static void
NEXUS_Recpump_isr(void *flow_, int parm2 )
{
    struct NEXUS_RecpumpFlow *flow = flow_;
    BSTD_UNUSED(parm2);
    BKNI_SetEvent_isr(flow->event);
    BDBG_ASSERT(flow->irqEnabled);
    BINT_DisableCallback_isr(flow->irq);
    flow->irqEnabled = false;
}
#endif

/* 
if the app specifics a buffer, use it.
if the app specifies a heap, then nexus does the alloc. 
if neither, xpt does the alloc. */
static NEXUS_Error
NEXUS_Recpump_P_AllocContextMem(BXPT_Rave_AllocCxSettings *allocSettings, const NEXUS_RecpumpOpenSettings *pSettings, bool allowUserAllocation,
    void **deallocCdbBuffer, void **deallocItbBuffer)
{
    NEXUS_Error rc;
    BMEM_Heap_Handle mem_heap;
  
    *deallocCdbBuffer = *deallocItbBuffer = NULL;
    if (allowUserAllocation && pSettings->data.buffer) {
        allocSettings->CdbBuffer = NEXUS_OffsetToUncachedAddr(NEXUS_AddrToOffset(pSettings->data.buffer));
        if (!allocSettings->CdbBuffer) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    } 
    else if (pSettings->data.heap && pSettings->data.bufferSize) {
        if (pSettings->useSecureHeap) { return BERR_TRACE(NEXUS_INVALID_PARAMETER); }
        
        mem_heap = NEXUS_Heap_GetMemHandle(pSettings->data.heap);
        allocSettings->CdbBuffer = BMEM_Heap_AllocAligned(mem_heap, pSettings->data.bufferSize, pSettings->data.alignment, 0);
        if (!allocSettings->CdbBuffer) { rc=BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY); return rc; }
        BDBG_MSG(("alloc cdb buffer %p", allocSettings->CdbBuffer));
        *deallocCdbBuffer = allocSettings->CdbBuffer;
    }
    if (allowUserAllocation && pSettings->index.buffer) {
        allocSettings->ItbBuffer = NEXUS_OffsetToUncachedAddr(NEXUS_AddrToOffset(pSettings->index.buffer));
        if (!allocSettings->ItbBuffer) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    } 
    else if (pSettings->index.heap && pSettings->index.bufferSize) {
        if (pSettings->useSecureHeap) { return BERR_TRACE(NEXUS_INVALID_PARAMETER); }

        mem_heap = NEXUS_Heap_GetMemHandle(pSettings->index.heap);
        allocSettings->ItbBuffer = BMEM_Heap_AllocAligned(mem_heap, pSettings->index.bufferSize, pSettings->index.alignment, 0);
        if (!allocSettings->ItbBuffer) { rc=BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY); return rc; }
        BDBG_MSG(("alloc itb buffer %p", allocSettings->ItbBuffer));
        *deallocItbBuffer = allocSettings->ItbBuffer;
    }

    return NEXUS_SUCCESS;
}

static void NEXUS_Recpump_P_ControlIrq(struct NEXUS_RecpumpFlow *flow, NEXUS_Recpump_P_BufferState  bufferState)
{
    BKNI_EnterCriticalSection();
    if(flow->irqEnabled) {
       if(bufferState !=  NEXUS_Recpump_P_BufferState_eNoData) {
            BINT_DisableCallback_isr(flow->irq);
            flow->irqEnabled = false;
       }
    } else {
        if(bufferState == NEXUS_Recpump_P_BufferState_eNoData) {
            BINT_EnableCallback_isr(flow->irq);
            flow->irqEnabled = true;
        }
    }
    BKNI_LeaveCriticalSection();
    return;
}

static void
NEXUS_Recpump_P_RaveEvent(void *flow_)
{
    struct NEXUS_RecpumpFlow *flow = flow_;
    NEXUS_Recpump_P_BufferState  bufferState;
    BDBG_MSG(("NEXUS_Recpump_P_RaveEvent:%#lx %s", (unsigned long)flow, NEXUS_P_FLOW_NAME(flow)));

    /* it's normal that we may get an event even though there's no dataready callback. there may be a rave event set
    while a WriteComplete finishes. no need to test here. */
    bufferState = NEXUS_Recpump_P_TestDataReadyCallback(flow);
    /* there is no promise of atomicity between  NEXUS_Recpump_isr and  NEXUS_Recpump_P_RaveEvent so interrupt could be wery well enabled by now */
    NEXUS_Recpump_P_ControlIrq(flow, bufferState);
    return;
}

NEXUS_RecpumpHandle NEXUS_Recpump_Open(unsigned index, const NEXUS_RecpumpOpenSettings *pSettings)
{
#if NEXUS_NUM_RECPUMPS
    NEXUS_Error rc = 0;
    BXPT_Rave_AllocCxSettings allocSettings;
    BINT_Id int_id;
    NEXUS_RecpumpHandle r;
    NEXUS_RecpumpOpenSettings settings;

    if (index == NEXUS_ANY_ID) {
        unsigned i;
        for (i=0;i<NEXUS_NUM_RECPUMPS;i++) {
            if (!pTransport->recpump[i]) {
                index = i;
                break;
            }
        }
        if (i == NEXUS_NUM_RECPUMPS) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            BDBG_ERR(("no recpump not available"));
            return NULL;
        }
    }

    if (index >= NEXUS_NUM_RECPUMPS) {
        BDBG_ERR(("recpump %d not supported", index));
        return NULL;
    }
    if (pTransport->recpump[index]) {
        BDBG_ERR(("recpump %d already open", index));
        return NULL;
    }

    if (!pSettings) {
        NEXUS_Recpump_GetDefaultOpenSettings(&settings);
        pSettings = &settings;
    }

    BDBG_MSG(("NEXUS_Recpump_Open %d: cdb=%d (%d threshold), itb=%d (%d threshold)",
        index,
        pSettings->data.bufferSize, pSettings->data.dataReadyThreshold,
        pSettings->index.bufferSize, pSettings->index.dataReadyThreshold));

    /* Ensure required numbers */
    if (pSettings->data.bufferSize < NEXUS_RAVE_MEMC_BLOCK_SIZE*2) {
        BDBG_ERR(("data.bufferSize %d invalid", pSettings->data.bufferSize));
        return NULL;
    }
    if (pSettings->data.dataReadyThreshold > pSettings->data.bufferSize) {
        BDBG_ERR(("data.dataReadyThreshold %d invalid", pSettings->data.dataReadyThreshold));
        return NULL;
    }
    /* index.bufferSize can be 0. Otherwise, it must be valid. */
    if (pSettings->index.bufferSize) {
        if (pSettings->index.bufferSize < NEXUS_RAVE_MEMC_BLOCK_SIZE*2) {
            BDBG_ERR(("index.bufferSize %d invalid", pSettings->index.bufferSize));
            return NULL;
        }
        if (pSettings->index.dataReadyThreshold > pSettings->index.bufferSize) {
            BDBG_ERR(("index.dataReadyThreshold %d invalid", pSettings->index.dataReadyThreshold));
            return NULL;
        }
    }
    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(recpump,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }

    /* Warn on dangerous numbers */
    if (pSettings->data.dataReadyThreshold * 100 / pSettings->data.bufferSize > 50) {
        BDBG_WRN(("data.dataReadyThreshold is very large. Overflow likely."));
    }
    if (pSettings->index.bufferSize && pSettings->index.dataReadyThreshold * 100 / pSettings->index.bufferSize > 50) {
        BDBG_WRN(("index.dataReadyThreshold is very large. Overflow likely."));
    }

    r = (NEXUS_RecpumpHandle)BKNI_Malloc(sizeof(*r));
    if (!r) {
        NEXUS_CLIENT_RESOURCES_RELEASE(recpump,Count,NEXUS_ANY_ID);
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    pTransport->recpump[index] = r;
    NEXUS_OBJECT_INIT(NEXUS_Recpump, r);
    r->tindex = index;
    r->openSettings = *pSettings;
    r->inputTransportType = NEXUS_TransportType_eTs;
    r->scdIdx = NULL;
    r->scdPidCount = 0;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    r->indexCodec = NEXUS_VideoCodec_eMpeg2;
    r->indexPid=0x1fff;
#endif
    NEXUS_Recpump_GetDefaultSettings(&r->settings);

    BXPT_Rave_GetDefaultAllocCxSettings(&allocSettings);
    allocSettings.CdbBuffer = NULL;
    allocSettings.ItbBuffer = NULL;
    allocSettings.BufferCfg.Cdb.Length = pSettings->data.bufferSize;
    allocSettings.BufferCfg.Itb.Length = pSettings->index.bufferSize;
    allocSettings.BufferCfg.Cdb.Alignment = pSettings->data.alignment;
    allocSettings.BufferCfg.Itb.Alignment = pSettings->index.alignment;

    /* Program ITB to big endian, CDB to host endianness.
    When reading from memory, either DMA or CPU, the memory controller will swap on a little endian system. So, on a little
    endian system, the end result will be ITB little, CDB big. On a big endian system, there is no memory controller swap,
    therefore the end result will be ITB big, CDB big. Therefore, the end result is: ITB host, CDB big.
    This is exactly what we want. It's just a little counter intuitive when programming transport here. */
    allocSettings.BufferCfg.Itb.LittleEndian = false;
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
    allocSettings.BufferCfg.Cdb.LittleEndian = true;
#else
    allocSettings.BufferCfg.Cdb.LittleEndian = false;
#endif

    if (pSettings->dummyRecpump) {
        /* alloc soft context for ITB-only record. CDB of dummyRecpump will be used as bit bucket. */
        allocSettings.SoftRaveMode = BXPT_RaveSoftMode_eIndexOnlyRecord;
        allocSettings.SoftRaveSrcCx = pSettings->dummyRecpump->rave_rec;
        if (pSettings->data.heap && pSettings->data.bufferSize != 0) {
            /* if CDB is bit bucket, user should not be allocating memory for it. */
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto error;
        }
        rc = NEXUS_Recpump_P_AllocContextMem(&allocSettings, pSettings, true, &r->rave_rec_mem.data, &r->rave_rec_mem.index);
        if (rc) { goto error; }
        
        rc = BXPT_Rave_AllocCx(pTransport->rave[0].channel, &allocSettings, &r->rave_rec);
        if (rc) { rc=BERR_TRACE(rc); goto error; }
    }
    else {
        allocSettings.RequestedType = pSettings->useSecureHeap?BXPT_RaveCx_eRecordR:BXPT_RaveCx_eRecord;
        allocSettings.SoftRaveSrcCx = NULL;
        rc = NEXUS_Recpump_P_AllocContextMem(&allocSettings, pSettings, true, &r->rave_rec_mem.data, &r->rave_rec_mem.index);
        if (rc) { goto error; }

        rc = BXPT_Rave_AllocCx(pTransport->rave[0].channel, &allocSettings, &r->rave_rec);
        if (rc) { rc=BERR_TRACE(rc); goto error; }
    }

    /* nullifyVct is only needed in limited uses. please ask for FAE for more usage information.
    at build time, you must export BXPT_VCT_SUPPORT=y.
    set nullifyVct = 0 to disable the feature.
    set nullifyVct = 1 for TVCT mode.
    set nullifyVct = 2 for CVCT mode. */
    if (pSettings->nullifyVct) {
#ifdef BXPT_VCT_SUPPORT
        /* an extra, unused RAVE context is allocated immediately after rave_rec so that RAVE FW has extra CXMEM to work with */
        allocSettings.RequestedType = BXPT_RaveCx_eRecord;
        allocSettings.SoftRaveSrcCx = NULL;
        rc = NEXUS_Recpump_P_AllocContextMem(&allocSettings, pSettings, false, &r->extra_rave_rec_mem.data, &r->extra_rave_rec_mem.index);
        if (rc) { goto error; }

        rc = BXPT_Rave_AllocCx(pTransport->rave[0].channel, &allocSettings, &r->rave_rec_extra);
        if (rc) { rc=BERR_TRACE(rc); goto error; }
#else
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
#endif
    }

    /* get bufferBase for data and index buffers */
    {
    BAVC_XptContextMap context;
    unsigned offset;
    void *temp;
    rc = BXPT_Rave_GetContextRegisters(r->rave_rec, &context);
    if (rc) { rc=BERR_TRACE(rc); goto error; }
    
    r->rave_rec_index = NEXUS_RAVE_INDEX(context.CDB_Read);

    offset = BREG_Read32(g_pCoreHandles->reg, context.CDB_Base);
    rc = BMEM_Heap_ConvertOffsetToAddress(g_pCoreHandles->heap[0], offset, &temp);
    if (rc) { rc=BERR_TRACE(rc); goto error; }
    rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], temp, (void **)&r->data.bufferBase);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    if (r->openSettings.index.bufferSize) {
        offset = BREG_Read32(g_pCoreHandles->reg, context.ITB_Base);
        rc = BMEM_Heap_ConvertOffsetToAddress(g_pCoreHandles->heap[0], offset, &temp);
        if (rc) { rc=BERR_TRACE(rc); goto error; }
        rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], temp, (void **)&r->index.bufferBase);
        if (rc) { rc=BERR_TRACE(rc); goto error; }
    }
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    /* Allocate the Indexer to go along with this Record Rave Context, Do only one indexer right now */
    rc = BXPT_Rave_AllocIndexer(pTransport->rave[0].channel, BXPT_RaveIdx_eScd, 1, r->rave_rec, &r->rave_indx);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    /* Add another indexer for TPIT */
    rc = BXPT_Rave_AllocIndexer(pTransport->rave[0].channel, BXPT_RaveIdx_eTpit, 1, r->rave_rec, &r->rave_indx_tpit);
    if (rc) { rc=BERR_TRACE(rc); goto error; }
#endif

    rc = BXPT_Rave_GetIntId(r->rave_rec, BXPT_RaveIntName_eCdbUpperThresh, &int_id);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    rc = BKNI_CreateEvent(&r->data.event);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    r->data.eventCallback = NEXUS_RegisterEvent(r->data.event, NEXUS_Recpump_P_RaveEvent, &r->data);
    if (!r->data.eventCallback) { rc=BERR_TRACE(NEXUS_UNKNOWN);goto error;}

    r->data.irqEnabled = false;
    rc = BINT_CreateCallback(&r->data.irq, g_pCoreHandles->bint, int_id, NEXUS_Recpump_isr, &r->data, 0);
    if (rc) { rc=BERR_TRACE(rc); goto error; }


    rc = BXPT_Rave_GetIntId(r->rave_rec, BXPT_RaveIntName_eItbUpperThresh, &int_id);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    rc = BKNI_CreateEvent(&r->index.event);
    if (rc) { rc=BERR_TRACE(rc); goto error; }

    r->index.eventCallback = NEXUS_RegisterEvent(r->index.event, NEXUS_Recpump_P_RaveEvent, &r->index);
    if (!r->index.eventCallback) { rc=BERR_TRACE(NEXUS_UNKNOWN);goto error;}

    r->index.irqEnabled = false;
    rc = BINT_CreateCallback(&r->index.irq, g_pCoreHandles->bint, int_id, NEXUS_Recpump_isr, &r->index, 0);
    if (rc) { rc=BERR_TRACE(rc); goto error; }


#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    rc = BKNI_CreateEvent(&r->crypto.event);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}

    r->crypto.callback = NEXUS_RegisterEvent(r->crypto.event, NEXUS_Recpump_P_DmaCallback, r);
    if(!r->crypto.callback) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}
#endif

    r->data.bufferSize = pSettings->data.bufferSize;
    r->data.dataReadyThreshold = pSettings->data.dataReadyThreshold;
    r->data.atomSize = pSettings->data.atomSize;
    /* r->data.atomSize = 0; */
    r->data.overflowCallback = NEXUS_TaskCallback_Create(r, NULL);
    r->data.dataReadyCallback = NEXUS_TaskCallback_Create(r, NULL);

    r->index.bufferSize = pSettings->index.bufferSize;
    r->index.dataReadyThreshold = pSettings->index.dataReadyThreshold;
    r->index.atomSize = pSettings->index.atomSize;
    r->index.overflowCallback = NEXUS_TaskCallback_Create(r, NULL);
    r->index.dataReadyCallback = NEXUS_TaskCallback_Create(r, NULL);

    /* init the callbacks */
    NEXUS_Recpump_SetSettings(r, &r->settings);

    r->dataStopped = false;
    r->data.recpump = r;
    r->index.recpump = r;

    BLST_S_INIT(&r->pid_list);
    return r;

error:
    NEXUS_Recpump_Close(r);
    return NULL;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
#endif
}

static void NEXUS_Recpump_P_Finalizer(NEXUS_RecpumpHandle r)
{
#if NEXUS_NUM_RECPUMPS
    NEXUS_OBJECT_ASSERT(NEXUS_Recpump, r);
    if (r->started) {
        NEXUS_Recpump_Stop(r);
    }
    NEXUS_Recpump_RemoveAllPidChannels(r);

    if (r->rave_rec_mem.data) {
        BDBG_ASSERT(r->openSettings.data.heap);
        BMEM_Heap_Free(NEXUS_Heap_GetMemHandle(r->openSettings.data.heap), r->rave_rec_mem.data);
    }
    if (r->rave_rec_mem.index) {
        BDBG_ASSERT(r->openSettings.index.heap);
        BMEM_Heap_Free(NEXUS_Heap_GetMemHandle(r->openSettings.index.heap), r->rave_rec_mem.index);
    }
    if (r->extra_rave_rec_mem.data) {
        BDBG_ASSERT(r->openSettings.data.heap);
        BMEM_Heap_Free(NEXUS_Heap_GetMemHandle(r->openSettings.data.heap), r->extra_rave_rec_mem.data);
    }
    if (r->extra_rave_rec_mem.index) {
        BDBG_ASSERT(r->openSettings.index.heap);
        BMEM_Heap_Free(NEXUS_Heap_GetMemHandle(r->openSettings.index.heap), r->extra_rave_rec_mem.index);
    }

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    if(r->crypto.callback) {
        NEXUS_UnregisterEvent(r->crypto.callback);
    }
    if(r->crypto.event) {
        BKNI_DestroyEvent(r->crypto.event);
    }
    if(r->settings.securityDma) {
        NEXUS_OBJECT_RELEASE(r, NEXUS_Dma, r->settings.securityDma);
    }
#endif
    if (r->index.irq) {
        BINT_DestroyCallback(r->index.irq);
    }
    if(r->index.eventCallback) {
        NEXUS_UnregisterEvent(r->index.eventCallback);
    }
    if (r->index.event) {
        BKNI_DestroyEvent(r->index.event);
    }
    if (r->data.irq) {
        BINT_DestroyCallback(r->data.irq);
    }
    if(r->data.eventCallback) {
        NEXUS_UnregisterEvent(r->data.eventCallback);
    }
    if (r->data.event) {
        BKNI_DestroyEvent(r->data.event);
    }
    if (r->tpitIdx) {
        BXPT_Rave_FreeIndexer(r->tpitIdx);
    }
    if (r->scdIdx) {
        BXPT_Rave_FreeIndexer(r->scdIdx);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    if (r->rave_indx) {
        BXPT_Rave_FreeIndexer(r->rave_indx);
    }
    if (r->rave_indx_tpit) {
        BXPT_Rave_FreeIndexer(r->rave_indx_tpit);
    }
#endif

    if (r->extra_rave_rec) {
        BXPT_Rave_FreeContext(r->extra_rave_rec);
    }
    if (r->rave_rec) {
        BXPT_Rave_FreeContext(r->rave_rec);
    }

    if (r->data.overflowCallback) {
        NEXUS_TaskCallback_Destroy(r->data.overflowCallback);
    }
    if (r->data.dataReadyCallback) {
        NEXUS_TaskCallback_Destroy(r->data.dataReadyCallback);
    }
    if (r->index.overflowCallback) {
        NEXUS_TaskCallback_Destroy(r->index.overflowCallback);
    }
    if (r->index.dataReadyCallback) {
        NEXUS_TaskCallback_Destroy(r->index.dataReadyCallback);
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    r->rave_indx = NULL;
    r->rave_indx_tpit = NULL;
    r->extra_rave_rec = NULL;
#endif

    r->rave_rec = NULL;
    pTransport->recpump[r->tindex] = NULL;
    NEXUS_CLIENT_RESOURCES_RELEASE(recpump,Count,NEXUS_ANY_ID);
    NEXUS_OBJECT_DESTROY(NEXUS_Recpump, r);
    BKNI_Free(r);
#else
    BSTD_UNUSED(r);
#endif
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Recpump, NEXUS_Recpump_Close);

void NEXUS_Recpump_GetSettings(NEXUS_RecpumpHandle r, NEXUS_RecpumpSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    *pSettings = r->settings;
}

#if defined(HUMAX_PLATFORM_BASE)
void NEXUS_Recpump_ChangeRecMode(NEXUS_RecpumpHandle r, int mode)
{
	if(mode == 0) /*AV*/
	{
		r->openSettings.data.dataReadyThreshold = r->openSettings.data.bufferSize / 5;
		r->data.dataReadyThreshold = r->openSettings.data.dataReadyThreshold;
	}
	else /*Audio only*/
	{
		r->openSettings.data.dataReadyThreshold = r->openSettings.data.bufferSize / 50;
		r->data.dataReadyThreshold = r->openSettings.data.dataReadyThreshold;
	}
}
#else
void NEXUS_Recpump_ChangeRecMode(NEXUS_RecpumpHandle r, int mode)
{
	BSTD_UNUSED(r);
	BSTD_UNUSED(mode);
}
#endif

NEXUS_Error NEXUS_Recpump_SetSettings(NEXUS_RecpumpHandle r, const NEXUS_RecpumpSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    if(r->started &&
        (r->settings.securityDma != pSettings->securityDma ||
        r->settings.securityContext != pSettings->securityContext)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if(pSettings->securityContext && !pSettings->securityDma) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if (pSettings->securityContext || pSettings->securityDma) {
        if (!pTransport->settings.dma) {
            BDBG_ERR(("Transport module does not have dma module handle."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
#endif
    if(r->started &&
        (r->settings.securityContext != pSettings->securityContext)) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if (r->settings.dropBtpPackets != pSettings->dropBtpPackets) {
        BXPT_Rave_ContextSettings cxSettings;
        BXPT_Rave_GetContextConfig(r->rave_rec, &cxSettings);
        cxSettings.EnableBppSearch = pSettings->dropBtpPackets;
        BXPT_Rave_SetContextConfig(r->rave_rec, &cxSettings);
    }

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    if (pSettings->securityDma != r->settings.securityDma) { 
        if(r->settings.securityDma) {
            NEXUS_OBJECT_RELEASE(r, NEXUS_Dma, r->settings.securityDma);
        }
        if(pSettings->securityDma) {
            NEXUS_OBJECT_ACQUIRE(r, NEXUS_Dma, pSettings->securityDma);
        }
    }
#endif

#if defined(HUMAX_PLATFORM_BASE) /* Recpump pause/resume */
		if(r->actuallyStarted)
		{
			BERR_Code rc;

			if(pSettings->recpumpSuspend)
			{
				rc = BXPT_Rave_DisableContext(r->rave_rec);
				if (rc) {
					rc=BERR_TRACE(rc);
					BDBG_ASSERT(rc);
					return rc;
				}
			}
			else
			{
				rc = BXPT_Rave_EnableContext(r->rave_rec);
				if (rc) {
					rc=BERR_TRACE(rc);
					BDBG_ASSERT(rc);
					return rc;
				}
			}
		}
#endif

    r->settings = *pSettings;
    NEXUS_TaskCallback_Set(r->data.overflowCallback, &r->settings.data.overflow);
    NEXUS_TaskCallback_Set(r->data.dataReadyCallback, &r->settings.data.dataReady);
    NEXUS_TaskCallback_Set(r->index.overflowCallback, &r->settings.index.overflow);
    NEXUS_TaskCallback_Set(r->index.dataReadyCallback, &r->settings.index.dataReady);
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Recpump_P_StartPid(NEXUS_RecpumpHandle r, NEXUS_Recpump_P_PidChannel *pid)
{
    NEXUS_Error rc = 0;
    BSTD_UNUSED(r);
    NEXUS_PidChannel_ConsumerStarted(pid->pidChn);
    return rc;
}

static void NEXUS_Recpump_P_StopPid(NEXUS_RecpumpHandle r, NEXUS_Recpump_P_PidChannel *pid)
{
    BSTD_UNUSED(r);
    BSTD_UNUSED(pid);
}

static void NEXUS_Recpump_P_GetThresholds(bool bandHold, unsigned dataReadyThreshold, unsigned bufferSize, uint16_t *upperThreshold, uint16_t *lowerThreshold)
{
    /* The XPT threshold values are in units of NEXUS_RAVE_THRESHOLD_UNITS bytes. */
    if(bandHold) {
        unsigned threshold = (bufferSize *3)/4;
        dataReadyThreshold = dataReadyThreshold + (NEXUS_RAVE_THRESHOLD_UNITS - 1);
        dataReadyThreshold -= dataReadyThreshold%NEXUS_RAVE_THRESHOLD_UNITS;
        if(threshold<dataReadyThreshold) {
            threshold = dataReadyThreshold;
        }
        *upperThreshold = threshold / NEXUS_RAVE_THRESHOLD_UNITS;
        *lowerThreshold = threshold / NEXUS_RAVE_THRESHOLD_UNITS;
        if(*upperThreshold==0) {
            /* TODO: 0 should work (i.e. an interrupt for any data), but it does not */
            *upperThreshold = 1;
        }
        if(*lowerThreshold>*upperThreshold) {
            *lowerThreshold = *upperThreshold - 1;
        }

    } else {
        *lowerThreshold = 0;
        *upperThreshold = dataReadyThreshold / NEXUS_RAVE_THRESHOLD_UNITS;
        if(*upperThreshold==0) {
            /* TODO: 0 should work (i.e. an interrupt for any data), but it does not */
            *upperThreshold = 1;
        }
    }
}
       

static NEXUS_Error NEXUS_Recpump_P_Start(NEXUS_RecpumpHandle r)
{
    BERR_Code rc;
    BXPT_Rave_RecordSettings rec_cfg;
    const NEXUS_RecpumpSettings *pSettings = &r->settings;
    unsigned packetSize, atom;
    bool startIndex = false;
    bool svcVideo = false;
    NEXUS_Recpump_P_PidChannel *pid;
    unsigned scdNo;
    bool bandHold;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    BDBG_ASSERT(r->started && !r->actuallyStarted);

    if (!pSettings->data.dataReady.callback) {
        BDBG_ERR(("dataReady.callback is required"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    r->rave_state = Ready;

    /* switching between 188 and 130 requires a Reset, not Flush */
    rc = BXPT_Rave_ResetContext(r->rave_rec);
    if (rc) {return BERR_TRACE(rc);}

#ifdef BXPT_VCT_SUPPORT
    if (r->openSettings.nullifyVct) {
        rc = BXPT_Rave_NullifyVCT(r->rave_rec, true, r->openSettings.nullifyVct == 1 ? BXPT_RaveVct_Tvct : BXPT_RaveVct_Cvct);
        if (rc) {return BERR_TRACE(rc);}
    }
#endif

    rc = BXPT_Rave_GetRecordConfig(r->rave_rec, &rec_cfg);
    if (rc) {return BERR_TRACE(rc);}

    switch (r->inputTransportType) {
    case NEXUS_TransportType_eTs:
    case NEXUS_TransportType_eDssEs:
    case NEXUS_TransportType_eDssPes:
        /* all outputs possible */
        break;
    case NEXUS_TransportType_eMpeg2Pes:
        if (r->settings.outputTransportType != NEXUS_TransportType_eMpeg2Pes &&
            r->settings.outputTransportType != NEXUS_TransportType_eEs)
        {
            BDBG_ERR(("cannot convert from transportType %d to %d", r->inputTransportType, r->settings.outputTransportType));
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        break;
    case NEXUS_TransportType_eEs:
        if (r->settings.outputTransportType != NEXUS_TransportType_eEs &&
            r->settings.outputTransportType != NEXUS_TransportType_eTs )
        {
            BDBG_ERR(("cannot convert from transportType %d to %d", r->inputTransportType, r->settings.outputTransportType));
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        break;
    default:
        BDBG_ERR(("transport input format %d not supported", r->inputTransportType));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    rec_cfg.MpegMode = !NEXUS_IS_DSS_MODE(r->inputTransportType);
    rec_cfg.CountRecordedPackets = pSettings->tpit.countRecordedPackets;

    /* if you turn on band hold based on playback, then the CDB_UPPER_THRESHOLD interrupt will hold playback.
    however, recpump already uses CDB_UPPER_THRESHOLD for the dataready interrupt. you cannot have the band hold and
    dataready on the same condition, otherwise you can't establish an I/O pipeline (i.e. new data coming in while you are
    writing out previous data) . what's needed is a second upper threshold interrupt: one for data ready and another
    for band hold. that said, if you want to set band hold, enable this code, set the dataReadyThreshold to be a much higher
    percentage (like 80%), then have your app poll recpump so that you can have an I/O pipeline. */
    switch(pSettings->bandHold) {
    case NEXUS_RecpumpFlowControl_eEnable:
        bandHold = true;
        break;
    case NEXUS_RecpumpFlowControl_eDisable:
        bandHold = false;
        break;
    case NEXUS_RecpumpFlowControl_eAuto:
        bandHold = false;
        for(pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
            BDBG_ASSERT(pid->pidChn);
            if (pid->pidChn->status.playback) {
                bandHold = true;
                break;
            }
        }
        bandHold = false; /* Can't use band hold, revert it back */
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    rec_cfg.BandHoldEn = bandHold;

    switch (r->settings.outputTransportType) {
    case NEXUS_TransportType_eTs:
    case NEXUS_TransportType_eDssEs:
    case NEXUS_TransportType_eDssPes:
        /* if input is DSS, a default output type of eTs is understood to be DSS. */
        rec_cfg.OutputFormat = BAVC_StreamType_eTsMpeg; /* this applies to DSS too */
        packetSize = NEXUS_IS_DSS_MODE(r->inputTransportType)?130:188;
        if (pSettings->timestampType != NEXUS_TransportTimestampType_eNone) {
            packetSize += 4;
        }
        break;
    case NEXUS_TransportType_eMpeg2Pes:
        /* the current impl does not use STREAM_ID_HI/LO for PES id filtering */
        rec_cfg.OutputFormat = BAVC_StreamType_ePes;
        packetSize = 0; /* there is no packet alignment for PES record. app can use M2M DMA to assemble byte aligned data into I/O sized (e.g. 4K) chunks. */
        if (pSettings->timestampType != NEXUS_TransportTimestampType_eNone) {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        break;
    case NEXUS_TransportType_eEs:
        rec_cfg.OutputFormat = BAVC_StreamType_eEs;
        packetSize = 0; /* there is no packet alignment for ES record */
        if (pSettings->timestampType != NEXUS_TransportTimestampType_eNone) {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        break;
    default:
        BDBG_ERR(("transport output format not supported"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }


    if (r->openSettings.data.atomSize % 64 != 0) {
        BDBG_ERR(("data.atomSize not supported. Recpump can be extended to support other atomSizes if required."));
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SATIP)
        if(r->openSettings.data.bCheckAlign)
        {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
#else
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
    }

    /* find "atom" which is a multiple of data.atomSize and packetSize */
    switch (packetSize) {
    case 188: atom = r->openSettings.data.atomSize * packetSize / 4; break;
    case 130: atom = r->openSettings.data.atomSize * packetSize / 2; break;
    case 192: atom = r->openSettings.data.atomSize * packetSize / 64; break;
    case 134: atom = r->openSettings.data.atomSize * packetSize / 2; break;
    default:  atom = 0; break; /* no alignment. should only occur for packetSize = 0. */
    }

    /* Remove the last transport packet and replace with one whole XC packet. */
    if (pSettings->data.useBufferSize) {
        rec_cfg.UseCdbSize = pSettings->data.useBufferSize;
    } else {
        rec_cfg.UseCdbSize = r->openSettings.data.bufferSize;
    }
    if (atom) {
        rec_cfg.UseCdbSize -= rec_cfg.UseCdbSize % atom;
    }
    if (packetSize) {
        /* Remove the last transport packet and replace it with a whole NEXUS_RAVE_MEMC_BLOCK_SIZE, then remove an atom or packetSize in order
        to get be under the actual bufferSize. This is required for correct RAVE wraparound logic.
        Note that this logic allows a single recpump buffer to be used for any packet size and any atom size. It's just that some combinations
        result in more truncation at the end of the buffer. The only way to avoid any truncation is to only use one packet size, one atom size,
        and to anticipate the NEXUS_RAVE_MEMC_BLOCK_SIZE replacement in your application (e.g. bufferSize = bufferSize - 188 + 256). */
        rec_cfg.UseCdbSize -= packetSize;
        rec_cfg.UseCdbSize += NEXUS_RAVE_MEMC_BLOCK_SIZE;
        /* be sure we stick within the memory we're given */
        while (rec_cfg.UseCdbSize > r->openSettings.data.bufferSize) {
            rec_cfg.UseCdbSize -= atom?atom:packetSize;
        }
        BDBG_ASSERT(rec_cfg.UseCdbSize <= r->openSettings.data.bufferSize);
    }

    NEXUS_Recpump_P_GetThresholds(bandHold, r->openSettings.data.dataReadyThreshold, rec_cfg.UseCdbSize, &rec_cfg.CdbUpperThreshold, &rec_cfg.CdbLowerThreshold);
    NEXUS_Recpump_P_GetThresholds(bandHold, r->openSettings.index.dataReadyThreshold, r->openSettings.index.bufferSize, &rec_cfg.ItbUpperThreshold, &rec_cfg.ItbLowerThreshold);

    BDBG_MSG(("Start: CDB alloc=%d use=%d, threshold=%d, ITB alloc=%d threshold=%d",
        r->openSettings.data.bufferSize, rec_cfg.UseCdbSize, r->openSettings.data.dataReadyThreshold,
        r->openSettings.index.bufferSize, r->openSettings.index.dataReadyThreshold));

    switch (pSettings->timestampType) {
    case NEXUS_TransportTimestampType_eNone:
        rec_cfg.UseTimeStamps = false;
        break;
    case NEXUS_TransportTimestampType_e30_2U_Mod300:
        rec_cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Mod300;
        rec_cfg.UseTimeStamps = true;
        rec_cfg.DisableTimestampParityCheck = false;
        break;
    case NEXUS_TransportTimestampType_e30_2U_Binary:
        rec_cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
        rec_cfg.UseTimeStamps = true;
        rec_cfg.DisableTimestampParityCheck = false;
        break;
    case NEXUS_TransportTimestampType_e32_Mod300:    
        rec_cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Mod300;
        rec_cfg.UseTimeStamps = true;
        rec_cfg.DisableTimestampParityCheck = true;
        break;
    case NEXUS_TransportTimestampType_e32_Binary:    
        rec_cfg.TimestampMode = BXPT_TimestampMode_e30_2U_Binary;
        rec_cfg.UseTimeStamps = true;
        rec_cfg.DisableTimestampParityCheck = true;
        break;
/* TODO: enable this when needed
    case NEXUS_TransportTimestampType_e28_4P_Mod300:    
        rec_cfg.TimestampMode = BXPT_TimestampMode_e28_4P_Mod300;
        rec_cfg.UseTimeStamps = true;
        rec_cfg.DisableTimestampParityCheck = true;
        break;
*/        
    default:
        BDBG_ERR(("Invalid timestamp mode"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    /* 1. Count number of pids that require indexing */
    for(scdNo=0,pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        if (NEXUS_RECPUMP_PID_IS_INDEX(&pid->settings)) {
            scdNo++;
            if( pid->settings.pidType == NEXUS_PidType_eVideo &&
               (pid->settings.pidTypeSettings.video.codec == NEXUS_VideoCodec_eH264_Svc || pid->settings.pidTypeSettings.video.codec == NEXUS_VideoCodec_eH264_Mvc)) {
                svcVideo = true;
            }
        }
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_ATS
	/* Adjust recorded timestamps to stay locked to the stream's PCRs. In other words,
	the delta between PTSs on PCR carrying packets will match the delta between the
	PCRs. */
    if(r->settings.adjustTimestampUsingPcrs)
    {
        if( NULL == r->settings.pcrPidChannel )
        {
            BDBG_ERR(("NULL pcrPidChannel for timestamp adjustments."));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        rec_cfg.PcrPidChannelIndex = r->settings.pcrPidChannel->status.pidChannelIndex;
    }
    rec_cfg.LockTimestampsToPcrs = r->settings.adjustTimestampUsingPcrs;
#else /* #if BXPT_HAS_ATS */
    if(r->settings.adjustTimestampUsingPcrs) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif /* #if BXPT_HAS_ATS */
#if BXPT_HAS_LOCAL_ATS
    if(r->settings.localTimestamp) {
        rec_cfg.GenerateLocalAts = true;
        rec_cfg.LocalAtsFormat = 0;
    } else {
        rec_cfg.GenerateLocalAts = false;
    }
#else /* #if BXPT_HAS_LOCAL_ATS */
    if(r->settings.localTimestamp) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
#endif /* #if BXPT_HAS_LOCAL_ATS */
#else
#if BXPT_HAS_ATS
	/* Adjust recorded timestamps to stay locked to the stream's PCRs. In other words,
	the delta between PTSs on PCR carrying packets will match the delta between the
	PCRs. */
    if(r->settings.adjustTimestampUsingPcrs)
    {
        if( NULL == r->settings.pcrPidChannel )
        {
            BDBG_ERR(("NULL pcrPidChannel for timestamp adjustments."));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        rec_cfg.PcrPidChannelIndex = r->settings.pcrPidChannel->status.pidChannelIndex;
    }
    rec_cfg.LockTimestampsToPcrs = r->settings.adjustTimestampUsingPcrs;
#endif
#endif

    rc = BXPT_Rave_SetRecordConfig(r->rave_rec, &rec_cfg);
    if (rc) {return BERR_TRACE(rc);}

    /* 2. Allocate SCD indexer for desired number of pids (we only increase  number of SCD) */
    if(scdNo>r->scdPidCount) {
        if(r->scdIdx) {
            BXPT_Rave_FreeIndexer(r->scdIdx);
            r->scdPidCount = 0;
            r->scdIdx = NULL;
        }
        rc = BXPT_Rave_AllocIndexer(pTransport->rave[0].channel, BXPT_RaveIdx_eScd, scdNo, r->rave_rec, &r->scdIdx);
        if (rc) { return BERR_TRACE(rc); }
        r->scdPidCount = scdNo;
    }

    for(scdNo=0,pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        BXPT_Rave_IndexerSettings indx_cfg;
        BXPT_Rave_ScdEntry ScdConfig;
        const NEXUS_RecpumpAddPidChannelSettings *pSettings = &pid->settings;

        if (!pid->pidChn) continue;
        if (!NEXUS_RECPUMP_PID_IS_INDEX(&pid->settings)) {
            continue;
        }

        BKNI_Memset(&ScdConfig, 0, sizeof(ScdConfig));

        if (pSettings->pidType == NEXUS_PidType_eVideo && pSettings->pidTypeSettings.video.index) {

            if (!r->openSettings.index.bufferSize) {
                BDBG_ERR(("No index buffer allocated"));
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }

            /* Set Index Configuration */
            BDBG_ASSERT(r->scdIdx);
            if(scdNo==0) {
                /* We are only capturing Index Data on the main video PID*/
                rc = BXPT_Rave_GetIndexerConfig( r->scdIdx, &indx_cfg);
                if (rc) {return BERR_TRACE(rc);}


                switch (pSettings->pidTypeSettings.video.codec) {
                case NEXUS_VideoCodec_eH264:
                case NEXUS_VideoCodec_eH264_Mvc:
                case NEXUS_VideoCodec_eH264_Svc:
                case NEXUS_VideoCodec_eH263:
                case NEXUS_VideoCodec_eVc1:
                case NEXUS_VideoCodec_eVc1SimpleMain:
                    /* 8 bytes of post-SC data is the max that can be captured.
                    AVC requires 59 bits, so we need them all. */
                    indx_cfg.Cfg.Scd.ScRange[0].RangeHi = 0x7F;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeLo = 0x00;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeIsASlice = false;
                    break;
                case NEXUS_VideoCodec_eMpeg2:
                    indx_cfg.Cfg.Scd.ScRange[0].RangeHi = 0xB7;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeLo = 0xB3;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeIsASlice = false;
#if BXPT_HAS_AVS
    /* for AVS-capable chips, we must explicitly enable SC 0x00 for MPEG */
                    indx_cfg.Cfg.Scd.ScRange[1].RangeHi = 0x00;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeLo = 0x00;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeIsASlice = false;
#endif
                    break;
                case NEXUS_VideoCodec_eAvs:
                    /* sequence start and end */
                    indx_cfg.Cfg.Scd.ScRange[0].RangeHi = 0xB1;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeLo = 0xB0;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[0].RangeIsASlice = false;
                    /* I picture */
                    indx_cfg.Cfg.Scd.ScRange[1].RangeHi = 0xB3;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeLo = 0xB3;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[1].RangeIsASlice = false;
                    /* P and B pictures */
                    indx_cfg.Cfg.Scd.ScRange[2].RangeHi = 0xB6;
                    indx_cfg.Cfg.Scd.ScRange[2].RangeLo = 0xB6;
                    indx_cfg.Cfg.Scd.ScRange[2].RangeEnable = true;
                    indx_cfg.Cfg.Scd.ScRange[2].RangeIsASlice = false;
                    break;
                default:
                    BDBG_ERR(("Video Format not supported"));
                    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
                }
#if NEXUS_SVC_MVC_SUPPORT
                indx_cfg.Cfg.Scd.SvcMvcMode = svcVideo;
#endif

                rc = BXPT_Rave_SetIndexerConfig(r->scdIdx, &indx_cfg);
                if (rc) {return BERR_TRACE(rc);}
            }

            ScdConfig.PidChannel = pid->pidChn->status.pidChannelIndex;

#if BXPT_HAS_STARTCODE_BUFFER_WORKAROUND
            ScdConfig.EsCount = 9; /* EsCount 9 means 1 start code + 8 bytes of payload after the start code, which is the maximum allowed by 6 word SCT */
#else
            /* Wait for RAVE FW to fix so we can request all 8 bytes of Payload*/
            ScdConfig.EsCount = 7;
#endif
            ScdConfig.ExtractPts = true;
            if (pSettings->pidTypeSettings.video.pid != 0x1fff)
            {
                BDBG_WRN(("Generating index using pid 0x%x (%u), %u", pSettings->pidTypeSettings.video.pid, pSettings->pidTypeSettings.video.pid, scdNo));
                rc = BXPT_Rave_SetScdUsingPid(r->scdIdx, scdNo, pSettings->pidTypeSettings.video.pid, &ScdConfig);
                if (rc) {return BERR_TRACE(rc);}
            }
            else
            {
                /* TODO: verify not allPass */
                BDBG_MSG(("Generating index using pid channel %d",ScdConfig.PidChannel));
                rc = BXPT_Rave_SetScdEntry(r->scdIdx, scdNo, &ScdConfig);
                if (rc) {return BERR_TRACE(rc);}
            }
        }
#if defined(HUMAX_PLATFORM_BASE)
        else if ((pSettings->pidType == NEXUS_PidType_eAudio || pSettings->pidType == NEXUS_PidType_eOther) && pSettings->pidTypeSettings.other.index) {
#else
        else if (pSettings->pidType == NEXUS_PidType_eOther && pSettings->pidTypeSettings.other.index) {
#endif

            if (!r->openSettings.index.bufferSize) {
                BDBG_ERR(("No index buffer allocated"));
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }

            if(scdNo==0) {
                unsigned i;
                /* Set Index Configuration */
                (void)BXPT_Rave_GetIndexerConfig( r->scdIdx, &indx_cfg);
                BDBG_CASSERT(NEXUS_NUM_STARTCODE_RANGES <= BXPT_MAX_STARTCODE_RANGES);
                for (i=0;i<NEXUS_NUM_STARTCODE_RANGES;i++) {
                    indx_cfg.Cfg.Scd.ScRange[i].RangeHi = pSettings->pidTypeSettings.other.startCodeRange[i].high;
                    indx_cfg.Cfg.Scd.ScRange[i].RangeLo = pSettings->pidTypeSettings.other.startCodeRange[i].low;
                    indx_cfg.Cfg.Scd.ScRange[i].RangeEnable = (pSettings->pidTypeSettings.other.startCodeRange[i].high >= pSettings->pidTypeSettings.other.startCodeRange[i].low);
                    indx_cfg.Cfg.Scd.ScRange[i].RangeIsASlice = false;
                }
                rc = BXPT_Rave_SetIndexerConfig(r->scdIdx, &indx_cfg);
                if (rc) {return BERR_TRACE(rc);}
            }

            ScdConfig.PidChannel = pid->pidChn->status.pidChannelIndex;
            ScdConfig.EsCount = 2; /* no payload needed, min 2 required */
            ScdConfig.ExtractPts = true;
            rc = BXPT_Rave_SetScdEntry(r->scdIdx, scdNo, &ScdConfig);
            if (rc) {return BERR_TRACE(rc);}
        }
        scdNo++;
        startIndex = true;

        if (pid->tpit.enabled) {
            startIndex = true;
        }
    }

    if (r->tpitIdx) {
        unsigned i;
        
        BXPT_Rave_IndexerSettings indx_cfg;
        (void)BXPT_Rave_GetIndexerConfig(r->tpitIdx, &indx_cfg);
        indx_cfg.Cfg.Tpit.FirstPacketEnable = pSettings->tpit.firstPacketEnable;
        indx_cfg.Cfg.Tpit.StorePcrMsb = pSettings->tpit.storePcrMsb;
        indx_cfg.Cfg.Tpit.IdleEventEnable = pSettings->tpit.idleEventEnable;
        indx_cfg.Cfg.Tpit.RecordEventEnable = pSettings->tpit.recordEventEnable;
        rc = BXPT_Rave_SetIndexerConfig(r->tpitIdx, &indx_cfg);
        if (rc) {return BERR_TRACE(rc);}
        
        for (i=0;i<NEXUS_NUM_ECM_TIDS;i++) {
            if (pSettings->tpit.ecmPair[i].enabled) {
                rc = BXPT_Rave_SetTpitEcms(r->tpitIdx, i+1 /* PI is one-based */, pSettings->tpit.ecmPair[i].evenTid, pSettings->tpit.ecmPair[i].oddTid);
                if (rc) {return BERR_TRACE(rc);}
            }
            /* TODO: no disable */
        }
    }
#if defined(HUMAX_PLATFORM_BASE) /*recpump pause/resume*/
	if(pSettings->recpumpSuspend)
	{
		rc = BXPT_Rave_DisableContext(r->rave_rec);
		if (rc) {rc=BERR_TRACE(rc);goto err_rave;}
	}
	else
	{
		rc = BXPT_Rave_EnableContext(r->rave_rec);
		if (rc) {rc=BERR_TRACE(rc);goto err_rave;}
	}
#else	 /*  Original*/
    rc = BXPT_Rave_EnableContext(r->rave_rec);
    if (rc) {rc=BERR_TRACE(rc);goto err_rave;}
#endif

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
    if(!r->settings.securityContext) {
        r->cryptoActive = false;
    } else {
        NEXUS_DmaJobSettings jobSettings;
        unsigned i;

        if (packetSize) {
            r->data.dataReadyThreshold -= r->data.dataReadyThreshold%packetSize; /* convert dataReadyThreshold to multiplier of transport packet size */
        }
        if(!g_NEXUS_Transport_P_State.settings.dma) { rc = BERR_TRACE(BERR_NOT_SUPPORTED);goto err_dma;}

        r->crypto.head = r->crypto.tail = NULL;
        NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
        jobSettings.numBlocks = sizeof(r->crypto.blocks)/sizeof(*r->crypto.blocks);
        jobSettings.dataFormat = r->settings.securityDmaDataFormat;
        jobSettings.keySlot = r->settings.securityContext;
        r->crypto.job = NEXUS_DmaJob_Create(r->settings.securityDma, &jobSettings);
        if(!r->crypto.job) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_dma;}
        for(i=0;i<sizeof(r->crypto.blocks)/sizeof(*r->crypto.blocks);i++) { /* initialize dma blocks */
            NEXUS_DmaJob_GetDefaultBlockSettings(&r->crypto.blocks[i]);
            r->crypto.blocks[i].cached = false;
        }
        r->crypto.blocks[0].resetCrypto = true;
        r->crypto.blocks[0].scatterGatherCryptoStart = true;
        r->crypto.blocks[1].scatterGatherCryptoEnd = true;
        r->crypto.packetSize = packetSize;
        r->cryptoActive = true;
    }
#endif

    for(pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        if (pid->pidChn) {
            NEXUS_Recpump_P_StartPid(r, pid);
        }
    }

    /* start MPEG data flow */
    BDBG_MSG(("Starting data flow"));
    rc = NEXUS_Recpump_P_StartFlow(&r->data);
    if (rc) {rc=BERR_TRACE(rc);goto err_record_data; }

    if (startIndex) {
        /* start Index data flow */
        BDBG_MSG(("Starting Index flow"));
        rc = NEXUS_Recpump_P_StartFlow(&r->index);
        if (rc) {rc=BERR_TRACE(rc);goto err_record_scd; }

        /* keep track of indexing with a separate variable. the indexPidChannel might be removed before we stop the flow. */
        r->indexing = true;
    }
    else {
        r->indexing = false;
    }

    r->actuallyStarted = true;
    return 0;

#if NEXUS_ENCRYPTED_DVR_WITH_M2M
err_dma:
    NEXUS_Recpump_P_StopFlow(&r->index);
#endif
err_record_scd:
    NEXUS_Recpump_P_StopFlow(&r->data);
err_record_data:
    BXPT_Rave_DisableContext(r->rave_rec);
err_rave:
    BDBG_ASSERT(rc);
    return rc;
}

NEXUS_Error NEXUS_Recpump_Start(NEXUS_RecpumpHandle r)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    if (r->started) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    BDBG_ASSERT(!r->started);
    BDBG_ASSERT(!r->actuallyStarted);

    r->started = true;
    if (r->pidChannelCnt) {
        rc = NEXUS_Recpump_P_Start(r);
    }
    else {
        /* defer */
        rc = 0;
    }

    /* mark started only if successful */
    if (rc) {
        r->started = false;
    } else {
        r->dataStopped = false;
    }
    return rc;
}

void NEXUS_Recpump_StopData(NEXUS_RecpumpHandle r)
{
    BERR_Code rc;
    NEXUS_Recpump_P_PidChannel *pid;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    if (!r->started)  {
        BDBG_ERR(("Recpump %d not started", r->tindex));
        return;
    }
    if(r->dataStopped) {
        BDBG_ERR(("Recpump %d already stopped", r->tindex));
        return;
    }
    r->dataStopped = true;

    if (r->rave_state != Terminated) {
        r->rave_state = Done;
    }

    if(r->actuallyStarted)
    {

        rc = BXPT_Rave_DisableContext(r->rave_rec);
        if (rc) { BDBG_ERR(("Error from BXPT_Rave_DisableContext, ignored")); }

        NEXUS_Recpump_P_StopFlow(&r->data);
        if (r->indexing) {
            NEXUS_Recpump_P_StopFlow(&r->index);
        }
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
        if(r->cryptoActive) {
            unsigned i;
            for(i=0;i<100;i++) {
                NEXUS_DmaJobStatus jobStatus;
                if(r->crypto.tail == r->crypto.head) { /*  DMA idle */
                    break;
                }
                rc = NEXUS_DmaJob_GetStatus(r->crypto.job, &jobStatus);
                if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);break;}
                if(jobStatus.currentState == NEXUS_DmaJobState_eComplete) {
                    break;
                }
                BDBG_MSG(("NEXUS_Recpump_StopData: %#lx waiting for DMA to complete %u", (unsigned long)r, i));
                BKNI_Sleep(1);
            }
            NEXUS_DmaJob_Destroy(r->crypto.job);
            r->crypto.job = NULL;
            r->cryptoActive = false;
        }
#endif
        for(pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
            if (pid->pidChn) {
                NEXUS_Recpump_P_StopPid(r, pid);
            }
        }
    }

    return;
}

void NEXUS_Recpump_Stop(NEXUS_RecpumpHandle r)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    r->rave_state = Terminated;
    if(!r->dataStopped) {
        NEXUS_Recpump_StopData(r);
    }
    r->actuallyStarted = false;
    r->started = false;
    return;
}

void NEXUS_Recpump_GetDefaultAddPidChannelSettings(NEXUS_RecpumpAddPidChannelSettings *pSettings)
{
    unsigned i;
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->pidType = NEXUS_PidType_eUnknown;
    pSettings->pidTypeSettings.video.pid = 0x1fff;
    for (i=0;i<NEXUS_NUM_STARTCODE_RANGES;i++) {
        if (i == 0) {
            /* all start codes */
            pSettings->pidTypeSettings.other.startCodeRange[i].low  = 0x00;
            pSettings->pidTypeSettings.other.startCodeRange[i].high = 0xFF;
        }
        else {
            /* disabled (if low > high) */
            pSettings->pidTypeSettings.other.startCodeRange[i].low  = 0x01;
            pSettings->pidTypeSettings.other.startCodeRange[i].high = 0x00;
        }
    }
}



NEXUS_Error NEXUS_Recpump_AddPidChannel(NEXUS_RecpumpHandle r, NEXUS_PidChannelHandle pidChannel, const NEXUS_RecpumpAddPidChannelSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_RecpumpAddPidChannelSettings settings;
    NEXUS_TransportType transportType;
    NEXUS_Recpump_P_PidChannel *pid;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
    NEXUS_RecpumpTpitFilter mytpitFilter;
#endif

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    if (!pSettings) {
        NEXUS_Recpump_GetDefaultAddPidChannelSettings(&settings);
        pSettings = &settings;
    }

    if (BLST_S_FIRST(&r->pid_list)) {
        /* check if already playback or not */
        if (pidChannel->status.playback != r->playback) {
            BDBG_ERR(("You cannot mix playback and non-playback pid channels to the same recpump"));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    else {
        r->playback = pidChannel->status.playback;
    }

    pid = BKNI_Malloc(sizeof(*pid));
    if ( NULL == pid )
    {
       return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset((void*)pid, 0x0, sizeof(*pid));
    pid->pidChn = pidChannel;
    BLST_S_DICT_ADD(&r->pid_list, pid, NEXUS_Recpump_P_PidChannel, pidChn, link, err_duplicate);
    pid->settings = *pSettings;
    NEXUS_OBJECT_ACQUIRE(r, NEXUS_PidChannel, pidChannel);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SCRAMBLE_RECORDING)
		if(pSettings->pidType == NEXUS_PidType_eVideo && pSettings->pidTypeSettings.video.index)
		{
			NEXUS_Recpump_GetDefaultTpitFilter(&mytpitFilter);
			mytpitFilter.pid = pSettings->pidTypeSettings.video.pid;
			mytpitFilter.mpegMode = true;
			mytpitFilter.config.mpeg.payloadUnitStartEnable = true;
			mytpitFilter.config.mpeg.payloadUnitStartCompValue = true;
			(void)NEXUS_Recpump_SetTpitFilter(r, pidChannel, &mytpitFilter);
		}
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_PID_CHANGE_DURING_RECORD)
    if (pSettings->pidType == NEXUS_PidType_eVideo && pSettings->pidTypeSettings.video.index) {
    	if (r->actuallyStarted) {
    		if(r->indexCodec != pSettings->pidTypeSettings.video.codec)
    		{
    			BDBG_WRN(("You must set open the index pid channel before starting recpump. This pid channels index setting will be ignored."));
    			BDBG_ERR(("\n\n####index Codec = 0x%x,	 pSettings->pidTypeSettings.video.codec = 0x%x\n", r->indexCodec, pSettings->pidTypeSettings.video.codec));
    		}
    		else
    		{
    			r->indexPidChannel = pidChannel;
    			r->indexPid = pSettings->pidTypeSettings.video.pid;
    			if (r->indexPidChannel) {
    				BXPT_Rave_ScdEntry ScdConfig;

    				ScdConfig.PidChannel = r->indexPidChannel->status.pidChannelIndex;

    				/* Wait for RAVE FW to fix so we can request all 8 bytes of Payload*/
    				ScdConfig.EsCount = 7;
    				ScdConfig.ExtractPts = true;

    				if(r->isTpitIndexType == false)
    				{
#if defined(HUMAX_PLATFORM_BASE) && defined(RECPUMP_DEBUG)
    					BDBG_ERR(("USE SCD entry"));
    					BDBG_ERR(("ScdConfig.Pid = %#x",r->indexPid));
#endif
    					if(r->indexPid != 0x1fff)
    					{
    						BDBG_WRN(("Generating index using pid 0x%x (%d)",r->indexPid,r->indexPid));
    						rc= BXPT_Rave_SetScdUsingPid(r->rave_indx, 0,r->indexPid,&ScdConfig);
    					}
    					else
    					{
    						/* TODO: verify not allPass */
    						BDBG_MSG(("Generating index using pid channel %d",ScdConfig.PidChannel));
    						rc = BXPT_Rave_SetScdEntry(r->rave_indx, 0, &ScdConfig);
    					}
    				}
    				else
    				{
#if defined(HUMAX_PLATFORM_BASE) && defined(RECPUMP_DEBUG)
    					BDBG_ERR(("USE TPIT entry"));
#endif
    				}
    				if (rc) {return BERR_TRACE(rc);}
    			}
    		}
    	}
    	else {
    		r->indexPidChannel = pidChannel;
    		r->indexCodec = pSettings->pidTypeSettings.video.codec;
    		r->indexPid = pSettings->pidTypeSettings.video.pid;
    	}
    }

#else

    if (NEXUS_RECPUMP_PID_IS_INDEX(pSettings)) {
        if (r->actuallyStarted) {
            BDBG_WRN(("You must add the index pid channel before starting recpump. This pid channel's index setting will be ignored."));
        }
    }
#endif

    if (pidChannel->status.playback) {
        NEXUS_PlaypumpSettings playpumpSettings;
        NEXUS_Playpump_GetSettings(pTransport->playpump[pidChannel->status.playbackIndex].playpump, &playpumpSettings);
        transportType = playpumpSettings.transportType;
        switch(transportType) {
        case NEXUS_TransportType_eMpeg2Pes:
            transportType = NEXUS_TransportType_eTs;
            break;
        default:
            break;
        }
    }
    else {
        NEXUS_ParserBandSettings parserBandSettings;
        NEXUS_ParserBand_P_GetSettings(pidChannel->parserBand, &parserBandSettings);
        transportType = parserBandSettings.transportType;
    }
    if (r->pidChannelCnt == 0) {
        r->inputTransportType = transportType;
    }
    else if (r->inputTransportType != transportType) {
        BDBG_ERR(("transport type for all pid channels must match"));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
    }

    rc = BXPT_Rave_AddPidChannel(r->rave_rec, pidChannel->status.pidChannelIndex, pSettings->useRPipe);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    /* now that we're up, we can do the bookkeeping. if we fail between here and the end, we must unwind. */
    r->pidChannelCnt++;

    if (r->started) {
        if (!r->actuallyStarted) {
            BDBG_ASSERT(r->pidChannelCnt == 1);
            rc = NEXUS_Recpump_P_Start(r);
        }
        else {
            /* already started, so just start this pid */
            (void)NEXUS_Recpump_P_StartPid(r, pid);
        }
    }

    /* if the last step failed, we have to undo the bookkeeping */
    if (rc) {
        BXPT_Rave_RemovePidChannel(r->rave_rec, pidChannel->status.pidChannelIndex);
        r->pidChannelCnt--;
        goto error;
    }

    return 0;

error:
    NEXUS_OBJECT_RELEASE(r, NEXUS_PidChannel, pidChannel);
    BLST_S_DICT_REMOVE(&r->pid_list, pid, pidChannel, NEXUS_Recpump_P_PidChannel, pidChn, link);
    pid->pidChn = NULL;
    BKNI_Free(pid);
    BDBG_ASSERT(rc);
    return rc;
err_duplicate:
    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
    BKNI_Free(pid);
    return rc;
}

NEXUS_Error NEXUS_Recpump_RemovePidChannel(NEXUS_RecpumpHandle r, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Error rc;
    NEXUS_Recpump_P_PidChannel *pid;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    BLST_S_DICT_FIND(&r->pid_list, pid, pidChannel, pidChn, link);
    if(pid==NULL) {
        BDBG_WRN(("NEXUS_Recpump_RemovePidChannel: %#lx can't find pid:%#lx", (unsigned long)r, (unsigned long)pidChannel));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_pid;
    }

    if (r->actuallyStarted) {
        NEXUS_Recpump_P_StopPid(r, pid);
    }

    (void)NEXUS_Recpump_SetTpitFilter(r, pidChannel, NULL);

    rc = BXPT_Rave_RemovePidChannel(r->rave_rec, pidChannel->status.pidChannelIndex);
    if (rc) return BERR_TRACE(rc);
    
    /* must finish using pidChannel before releasing */
    NEXUS_OBJECT_RELEASE(r, NEXUS_PidChannel, pidChannel);
    BLST_S_DICT_REMOVE(&r->pid_list, pid, pidChannel, NEXUS_Recpump_P_PidChannel, pidChn, link);
    if(!pid) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_pid;}

    BDBG_ASSERT(r->pidChannelCnt);

    pid->pidChn = NULL;
    r->pidChannelCnt--;
    if (r->scdIdx && r->pidChannelCnt==0) { /* when all pids were removed free indexer */
        BXPT_Rave_FreeIndexer(r->scdIdx);
        r->scdIdx=NULL;
        r->scdPidCount = 0;
    }

    BKNI_Free(pid);

    return 0;
err_pid:
    return rc;
}

void NEXUS_Recpump_RemoveAllPidChannels(NEXUS_RecpumpHandle r)
{
    NEXUS_Recpump_P_PidChannel *pid;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);

    while(NULL!=(pid=BLST_S_FIRST(&r->pid_list))) {
        NEXUS_Recpump_RemovePidChannel(r, pid->pidChn);
    }

    BDBG_ASSERT(r->pidChannelCnt == 0);
}

static NEXUS_Error
NEXUS_Recpump_P_GetBuffer(struct NEXUS_RecpumpFlow *flow, const void **buffer, size_t *bufferSize, const void **buffer2, size_t *bufferSize2, bool *pBypassThresholdTest)
{
    BERR_Code rc;
    BXPT_Rave_ContextPtrs ptrs;
    const BXPT_Rave_DataPtrs *ptr;
    unsigned size;
    bool bypassThresholdTest = false;
    NEXUS_RecpumpHandle pump;
    uint8_t *dataPtr;

    pump = flow->recpump;

    if (!pump->actuallyStarted)  {
        /* don't print error message, because this is a normal exit from thread processing */
        return NEXUS_UNKNOWN;
    }
    rc = BXPT_Rave_CheckBuffer(pump->rave_rec, &ptrs);
    if (rc) return BERR_TRACE(rc);

    if (flow==&pump->data) {
        ptr = &ptrs.Cdb;
    }
    else {
        ptr = &ptrs.Itb;
    }
    BDBG_MSG_TRACE(("RAVE %s depth %u %u, size %u", NEXUS_P_FLOW_NAME(flow),
            (unsigned long)(ptr->ByteCount), (unsigned long)ptr->WrapByteCount,
            flow->bufferSize));

    /* if the depth is greater than the size, then underlying FW or SW must be in a bad state.
    the chances that upper level software can survive this are very low. better to assert now and fix the problem. */
    if ((unsigned long)(ptr->ByteCount + ptr->WrapByteCount) > flow->bufferSize) {
        BDBG_ERR(("Invalid RAVE %s depth %u, size %u", NEXUS_P_FLOW_NAME(flow),
            (unsigned long)(ptr->ByteCount + ptr->WrapByteCount),
            flow->bufferSize));
        BKNI_Fail();
    }

    if (flow==&pump->data) {
        size = ptr->ByteCount;
        dataPtr = ptr->DataPtr;
        if(size<=0) {
            goto done;
        } else {
            bypassThresholdTest = ptr->WrapByteCount>0;
        }
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
        if(pump->cryptoActive) {
            uint8_t *tail = pump->crypto.tail;
            if(pump->crypto.head == tail) { /* DMA was completed, check if there is new data */
                unsigned nvecs;
                if(tail==NULL) { /* after start we don't know start of CDB buffer */
                    tail = pump->crypto.head=pump->crypto.tail = dataPtr;
                }
                if(ptr->WrapByteCount==0) {
                    if(((uint8_t *)dataPtr + size) >= tail) {
                        /*      |---------|++++++++|~~~~~~~~~~~~|-------------|
                         *   FifoStart  dataPtr  tail     (dataPtr+size) FifoEnd(Wrap) */
                        size  = (dataPtr + size) - tail;
                    } else { /* we just passed through FIFO wrap pointer, reset head */
                        /*      |~~~~~~~~~~~~~~~~~|---------------------------|
                         *   dataPtr/FifoStart (dataPtr+size)         tail/FifoEnd(Wrap) */
                        tail = pump->crypto.tail = dataPtr;
                    }
                    nvecs = 1;
                    if (pump->crypto.packetSize) {
                        size -= size%pump->crypto.packetSize;
                    }
                    pump->crypto.head = tail + size;
                } else {
                    unsigned partialPacket;

                    if(dataPtr > tail)  {
                        /* all data prior to wrap is encrypted */
                        if( ((uint8_t *)ptr->WrapDataPtr + ptr->WrapByteCount) >= tail) {
                            /*     |++++++++++++++++++++|~~~~~~~~~~~~|----------------|+++++++++++++++|
                             * FifoStart/WrapDataPtr  tail WrapDataPtr+WrapByteCount dataPtr   FifoEnd(Wrap)/dataPtr+size */
                            nvecs = 1;
                            size  = ((uint8_t *)ptr->WrapDataPtr + ptr->WrapByteCount) - tail; /* number of bytes prior to wrap */
                            if (pump->crypto.packetSize) {
                                size -= size%pump->crypto.packetSize; /* if there is no wrap, then don't cross packet boundary */
                            }
                            pump->crypto.head = tail + size;
                        } else {
                            return BERR_TRACE(NEXUS_UNKNOWN); /* somehow rave pointer raced ahead of DMA pointer*/
                        }
                    } else if((dataPtr + size) >= tail) {
                        /*     |~~~~~~~~~~~~~~~~~~~~~~~|-------------------|++++++++++|~~~~~~~~|
                        * FifoStart/WrapDataPtr  WrapDataPtr+WrapByteCount dataPtr  tail  FifoEnd(Wrap)/dataPtr+size */
                        size = (dataPtr + size) - tail; /* number of bytes prior to wrap */
                        if(size>0) { /* if there is unencrypted data prior to wrap */
                            if (pump->crypto.packetSize) {
                                partialPacket = (ptr->WrapByteCount + size)%pump->crypto.packetSize; /* size of partial packet */
                            }
                            else {
                                partialPacket = 0;
                            }
                            if(ptr->WrapByteCount > partialPacket) { /* if after write there is complete packet after wrap */
                                nvecs = 2;
                                pump->crypto.head = (uint8_t*)ptr->WrapDataPtr + (ptr->WrapByteCount - partialPacket);
                                pump->crypto.blocks[1].blockSize = ptr->WrapByteCount - partialPacket;
                                pump->crypto.blocks[1].pSrcAddr = ptr->WrapDataPtr;
                                pump->crypto.blocks[1].pDestAddr = ptr->WrapDataPtr;
                            } else { /* Can't use data after wrap yet */
                                nvecs = 1;
                                if (pump->crypto.packetSize) {
                                    size -= size%pump->crypto.packetSize;
                                }
                                pump->crypto.head = tail + size;
                            }
                        } else {
                            /* there was wrap and only unencrypted data after wrap */
                            /*     |~~~~~~~~~~~~~~~~~~~~~~~|-------------------|++++++++++|
                            * FifoStart/WrapDataPtr  WrapDataPtr+WrapByteCount dataPtr  tail/FifoEnd(Wrap)/dataPtr+size */
                            nvecs = 1;
                            size = ptr->WrapByteCount;
                            if (pump->crypto.packetSize) {
                                size -= size%pump->crypto.packetSize;
                            }
                            tail = pump->crypto.tail = ptr->WrapDataPtr;
                            pump->crypto.head = tail + size;
                        }
                    } else {
                        return BERR_TRACE(NEXUS_UNKNOWN); /* somehow rave pointer raced ahead of DMA pointer*/
                    }
                }
                /* BDBG_MSG(("DMA: %u %u %u (%u %u %u(%p:%p:%u)", tail != pump->crypto.head,nvecs, size>flow->dataReadyThreshold, !flow->pending, tail<(dataPtr+flow->dataReadyThreshold), (tail+size)>=(dataPtr+flow->dataReadyThreshold), (tail+size), (dataPtr+flow->dataReadyThreshold), size)); */

                if(tail != pump->crypto.head && /* there is new data available */
                   (nvecs>1 ||  /* wrap */
                    size>flow->dataReadyThreshold || /* or DMA block large enough */
                    (!flow->pending && tail<(dataPtr+flow->dataReadyThreshold) && (tail+size)>=(dataPtr+flow->dataReadyThreshold))) /* application waits for data and completed DMA would make threshold test to pass */
                        ) {
                    pump->crypto.blocks[0].blockSize = size;
                    pump->crypto.blocks[0].pSrcAddr = tail;
                    pump->crypto.blocks[0].pDestAddr = tail;
                    pump->crypto.blocks[0].scatterGatherCryptoEnd = nvecs==2?false:true;

                    NEXUS_Module_Lock(g_NEXUS_Transport_P_State.settings.dma);
                    rc = NEXUS_DmaJob_ProcessBlocks_priv(pump->crypto.job, pump->crypto.blocks, nvecs, pump->crypto.event);
                    NEXUS_Module_Unlock(g_NEXUS_Transport_P_State.settings.dma);
                    BDBG_MSG(("NEXUS_Recpump_P_GetBuffer:%#lx DMA(%#lx:%u) %u->%u", (unsigned long)pump, (unsigned long)pump->crypto.tail, nvecs, size, rc));
                    if(rc==NEXUS_SUCCESS) {
                        tail = pump->crypto.tail = pump->crypto.head;
                    } else if (rc!=NEXUS_DMA_QUEUED) {
                        pump->crypto.head = tail; /* revert changes */
                        return BERR_TRACE(rc);
                    }
                } else {
                    pump->crypto.head = tail;
                }
            }

            if(tail >= dataPtr) {
                size = tail - dataPtr;
                bypassThresholdTest = false;
            } else {
                size = ptr->ByteCount;
                bypassThresholdTest = true;
            }
        }
#endif
    } else {
        size = ptr->ByteCount;
        dataPtr = ptr->DataPtr;
        if (size<=0) {
            goto done;
        } else {
            /* on a wrap around, we must always return whatever data is available */
            bypassThresholdTest = ptr->WrapByteCount>0;
        }
    }

done:
    if (buffer2) {
        /* if you want to peek around the wrap around, don't apply any atomSize truncation. just give
        both buffer and buffer2 exactly as is.
        if crypto is active, don't peek around. */
#if NEXUS_ENCRYPTED_DVR_WITH_M2M
        if (!pump->cryptoActive && ptr->WrapByteCount>0) {
#else
        if (ptr->WrapByteCount>0) {
#endif
            *buffer2 = ptr->WrapDataPtr;
            *bufferSize2 = ptr->WrapByteCount;
        }
        else {
            /* the user is requesting peek around, but there is none or it is wrapped with
            dma logic and can't be provided. */
            *buffer2 = NULL;
            *bufferSize2 = 0;
        }
    }
    else {
        /* apply atomSize limitations before setting buffer/bufferSize */
        if (pump->rave_state != Done) {
            if (size >= flow->atomSize || bypassThresholdTest) {
                /* don't modify size & dataPtr */
            } else {
                /* If we're not at wrap around, and we're not done, then GetBuffer shouldn't report any data which is
                less than the threshold. Let the app wait for the interrupt. */
                size = 0;
                dataPtr = NULL;
            }
        } else {
            /* If we're done, we send everything out. The last piece of data can be < atomSize. */
            if (flow->atomSize && size > flow->atomSize) {
                size -= (size % flow->atomSize);
            }
        }
    }
    if ( pump->rave_state == Done ) {
        /* if writeAllTimeout is set ALL data in buffer will be written - even data < 4k */
        bypassThresholdTest = true;
    }
    *bufferSize = size;
    *buffer = dataPtr;
    *pBypassThresholdTest = bypassThresholdTest;

    BDBG_MSG(("GetBuffer[%s] %u (actual %u, threshold %u)", NEXUS_P_FLOW_NAME(flow), *bufferSize, size, flow->dataReadyThreshold));

    return NEXUS_SUCCESS;
}


static NEXUS_Error
NEXUS_Recpump_P_WriteComplete(struct NEXUS_RecpumpFlow *flow, size_t amount_written)
{
    BERR_Code rc;
    BXPT_RaveCx_Status status;
    NEXUS_Recpump_P_BufferState  bufferState;

    if (!flow->recpump->actuallyStarted)  {
        /* don't print error message, because this is a normal exit from thread processing */
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    if (amount_written > flow->lastGetBuffer) {
        BDBG_WRN(("Cannot WriteComplete %u when last GetBuffer was %u", amount_written, flow->lastGetBuffer));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    BDBG_MSG(("WriteComplete[%s] %u", NEXUS_P_FLOW_NAME(flow), amount_written));
    flow->bytesRecorded += amount_written;

    if (flow == &flow->recpump->index) {
        /* Itb read pointer is updated here */
        rc = BXPT_Rave_UpdateReadOffset(flow->recpump->rave_rec, 0 /* CDB count */, amount_written);
    }
    else {
        rc = BXPT_Rave_UpdateReadOffset(flow->recpump->rave_rec, amount_written, 0 /* ITB count */);
    }
    if (rc) {return BERR_TRACE(rc);}

    rc = BXPT_Rave_GetContextStatus(flow->recpump->rave_rec, &status);
    if (rc) {return BERR_TRACE(rc);}

    if (flow == &flow->recpump->index) {
        if (status.ItbOverflow) {
            BDBG_ERR(("[%#x]ITB Overflow detected in record", flow));
            NEXUS_TaskCallback_Fire(flow->overflowCallback);
        }
    }
    else {
        if (status.CdbOverflow) {
            BDBG_ERR(("[%#x]CDB Overflow detected in record", flow));
            NEXUS_TaskCallback_Fire(flow->overflowCallback);
        }
    }

    flow->pending = false;
    flow->lastGetBuffer = 0;

    bufferState = NEXUS_Recpump_P_TestDataReadyCallback(flow);
    NEXUS_Recpump_P_ControlIrq(flow, bufferState);

    return NEXUS_SUCCESS;
}

/* this function handles calls from application, it checks rave buffer, saves last returned size and flushes cache */
static NEXUS_Error
NEXUS_Recpump_P_GetBufferApi(struct NEXUS_RecpumpFlow *flow, const void **buffer, size_t *bufferSize, const void **buffer2, size_t *bufferSize2)
{
    NEXUS_Error rc;
    bool wrapped;
    rc = NEXUS_Recpump_P_GetBuffer(flow, buffer, bufferSize, buffer2, bufferSize2, &wrapped);
    if(rc==NEXUS_SUCCESS) {
        flow->lastGetBuffer = *bufferSize;

#if defined(HUMAX_PLATFORM_BASE)
    {
        NEXUS_RecpumpHandle pump;
        
        pump = flow->recpump;
        if( pump->cryptoActive && (pump->crypto.head != pump->crypto.tail) )  /* DMA was pending */
        {
            /* clean-up DMA pending status */
            if( pump->crypto.job != NULL )
            {
                NEXUS_DmaJobStatus jobStatus;
                NEXUS_DmaJob_GetStatus(pump->crypto.job, &jobStatus);
            }
        }
    }
#endif

        if(*bufferSize>0) {
            rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], (void *)*buffer, (void **)buffer);
            if (rc) return BERR_TRACE(rc);
            NEXUS_FlushCache(*buffer, *bufferSize);
        } else {
            *buffer = NULL;
        }

        if (bufferSize2) {
            flow->lastGetBuffer += *bufferSize2;
            if(*bufferSize2>0) {
                rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], (void *)*buffer2, (void **)buffer2);
                if (rc) return BERR_TRACE(rc);
                NEXUS_FlushCache(*buffer2, *bufferSize2);
            } else {
                *buffer2 = NULL;
            }
        }
    }
    return rc;
}


NEXUS_Error NEXUS_Recpump_GetDataBuffer(NEXUS_RecpumpHandle r, const void **pBuffer, size_t *pAmount)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_GetBufferApi(&r->data, pBuffer, pAmount, NULL, NULL);
}

NEXUS_Error NEXUS_Recpump_GetDataBufferWithWrap( NEXUS_RecpumpHandle r, const void **pBuffer, size_t *pAmount, const void **pBuffer2, size_t *pAmount2 )
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_GetBufferApi(&r->data, pBuffer, pAmount, pBuffer2, pAmount2);
}

NEXUS_Error NEXUS_Recpump_DataReadComplete(NEXUS_RecpumpHandle r, size_t amount)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_WriteComplete(&r->data, amount);
}

NEXUS_Error NEXUS_Recpump_GetIndexBuffer(NEXUS_RecpumpHandle r, const void **pBuffer, size_t *pAmount)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_GetBufferApi(&r->index, pBuffer, pAmount, NULL, NULL);
}

NEXUS_Error NEXUS_Recpump_GetIndexBufferWithWrap( NEXUS_RecpumpHandle r, const void **pBuffer, size_t *pAmount, const void **pBuffer2, size_t *pAmount2)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_GetBufferApi(&r->index, pBuffer, pAmount, pBuffer2, pAmount2);
}

NEXUS_Error NEXUS_Recpump_IndexReadComplete(NEXUS_RecpumpHandle r, size_t amount)
{
    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    return NEXUS_Recpump_P_WriteComplete(&r->index, amount);
}

NEXUS_Error NEXUS_Recpump_GetStatus(NEXUS_RecpumpHandle r, NEXUS_RecpumpStatus *pStatus)
{
    BERR_Code rc;
    BXPT_Rave_ContextPtrs ptrs;
    BXPT_Rave_DataPtrs *ptr;
    BXPT_Rave_RecordStats stats;
    NEXUS_Recpump_P_PidChannel *pid;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    pStatus->openSettings = r->openSettings;
    pStatus->rave.index = r->rave_rec_index;

    rc = BXPT_Rave_CheckBuffer(r->rave_rec, &ptrs);
    if (rc) return BERR_TRACE(rc);
    rc = BXPT_Rave_GetRecordStats(r->rave_rec, &stats);
    if (rc) return BERR_TRACE(rc);

    ptr = &ptrs.Cdb;
    pStatus->data.bytesRecorded = (unsigned long)stats.ByteCount;
    pStatus->data.bufferBase = r->data.bufferBase;
    pStatus->data.fifoDepth = (unsigned long)(ptr->ByteCount + ptr->WrapByteCount);
    pStatus->data.fifoSize =  r->data.bufferSize;
    if (pStatus->data.fifoDepth > pStatus->data.fifoSize) {
        BDBG_ERR(("Invalid RAVE CDB depth %d, size %d", pStatus->data.fifoDepth, pStatus->data.fifoSize));
        pStatus->data.fifoDepth = 0;
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    ptr = &ptrs.Itb;
    pStatus->index.bytesRecorded = r->index.bytesRecorded;
    pStatus->index.bufferBase = r->index.bufferBase;
    pStatus->index.fifoDepth = (unsigned long)(ptr->ByteCount + ptr->WrapByteCount);
    pStatus->index.fifoSize = r->index.bufferSize;
    if (pStatus->index.fifoDepth > pStatus->index.fifoSize) {
        BDBG_ERR(("Invalid RAVE ITB depth %d, size %d", pStatus->index.fifoDepth, pStatus->index.fifoSize));
        pStatus->index.fifoDepth = 0;
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    
    for(pid=BLST_S_FIRST(&r->pid_list);pid;pid=BLST_S_NEXT(pid, link)) {
        if (NEXUS_RECPUMP_PID_IS_INDEX(&pid->settings)) {
            pStatus->hasIndex = true;
            break;
        }
    }
    
    return 0;
}

static NEXUS_Recpump_P_BufferState
NEXUS_Recpump_P_TestDataReadyCallback(struct NEXUS_RecpumpFlow *flow)
{
    const void *buffer, *buffer2;
    bool bypassThresholdTest = false;
    size_t size = 0, size2 = 0;

    if (flow->recpump->rave_state == Terminated) {
        return NEXUS_Recpump_P_BufferState_eTerminated;
    }

    if (NEXUS_Recpump_P_GetBuffer(flow, &buffer, &size, &buffer2, &size2, &bypassThresholdTest)!=NEXUS_SUCCESS) {
        return NEXUS_Recpump_P_BufferState_eError;
    }

    /* If we haven't exceeded the threshold, we should not fire the dataReady callback. Excessive callbacks lead to poor performance.
    However, we must always fire the callback at the wraparound so the application doesn't get stuck if relying on this callback.
    Must apply NEXUS_RAVE_THRESHOLD_UNITS adjustment before comparing with flow->dataReadyThreshold because XPT is one-based, not zero-based. */
    if (size+NEXUS_RAVE_THRESHOLD_UNITS < flow->dataReadyThreshold && !bypassThresholdTest) {
        return NEXUS_Recpump_P_BufferState_eNoData;
    }

    if (flow->pending) {
        return NEXUS_Recpump_P_BufferState_eWaitingApplication;
    }

    flow->pending = true;
    BDBG_MSG(("dataReady[%s] size=%d", NEXUS_P_FLOW_NAME(flow), size));

    NEXUS_TaskCallback_Fire(flow->dataReadyCallback);
    return NEXUS_Recpump_P_BufferState_eApplicationNotified;
}


static NEXUS_Error
NEXUS_Recpump_P_StartFlow(struct NEXUS_RecpumpFlow *flow)
{
    BERR_Code rc;
    BDBG_ASSERT(!flow->irqEnabled);
    BKNI_EnterCriticalSection();
    rc = BINT_EnableCallback_isr(flow->irq);
    flow->irqEnabled = rc==BERR_SUCCESS;
    BKNI_LeaveCriticalSection();
    if (rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto error; }
    flow->pending = false;
    flow->bytesRecorded = 0;
    return NEXUS_SUCCESS;
error:
    return rc;
}

static void
NEXUS_Recpump_P_StopFlow(struct NEXUS_RecpumpFlow *flow)
{
    BKNI_EnterCriticalSection();
    if(flow->irqEnabled) {
        BINT_DisableCallback_isr(flow->irq);
        flow->irqEnabled = false;
    }
    BKNI_LeaveCriticalSection();
    /* check that there is enought data in the buffer and issue callback */
    NEXUS_Recpump_P_TestDataReadyCallback(flow);
}

void NEXUS_Recpump_GetDefaultTpitFilter( NEXUS_RecpumpTpitFilter *pFilter )
{
    BKNI_Memset(pFilter, 0, sizeof(*pFilter));
    pFilter->mpegMode = true;
}

NEXUS_Error NEXUS_Recpump_SetTpitFilter( NEXUS_RecpumpHandle r, NEXUS_PidChannelHandle pidChannel, const NEXUS_RecpumpTpitFilter *pFilter )
{
    BERR_Code rc;
    NEXUS_Recpump_P_PidChannel *pid;
    NEXUS_Recpump_P_PidChannel *pid1;

    BDBG_OBJECT_ASSERT(r, NEXUS_Recpump);
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    BDBG_CASSERT(sizeof(BXPT_Rave_TpitEntry) == sizeof(NEXUS_RecpumpTpitFilter));

    BLST_S_DICT_FIND(&r->pid_list, pid, pidChannel, pidChn, link);
    if(pid==NULL) {
        BDBG_WRN(("NEXUS_Recpump_SetTpitFilter: %#lx can't find pid:%#lx", (unsigned long)r, (unsigned long)pidChannel));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_pid;
    }


    if (pFilter) {
        /* validation */
        if (pFilter->mpegMode != !NEXUS_IS_DSS_MODE(r->inputTransportType)) {
            BDBG_ERR(("NEXUS_RecpumpTpitFilter.mpegMode does not match parser band's transportType"));
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* recpump init */
        if (r->tpitCount == 0) {
            BDBG_ASSERT(!r->tpitIdx);
            rc = BXPT_Rave_AllocIndexer(pTransport->rave[0].channel, BXPT_RaveIdx_eTpit, 1, r->rave_rec, &r->tpitIdx);
            if (rc) return BERR_TRACE(rc);
        }

        if (!pid->tpit.enabled) {
            /* enable a new filter. this requires finding an available tpit filter index.
            this search doesn't assume BXPT_P_MAX_TPIT_PIDS is > or < than NEXUS_TOTAL_RECPUMP_PIDCHANNELS. */
            unsigned availableIndex;
            for (availableIndex=0;availableIndex<BXPT_NUM_TPIT_PIDS;availableIndex++) {
                bool taken = false;

                for(pid1=NULL, pid1=BLST_S_FIRST(&r->pid_list);pid1;pid1=BLST_S_NEXT(pid1, link)) {
                     if (pid1->pidChn != pidChannel && pid1->tpit.enabled) {
                        if (pid1->tpit.index == availableIndex) {
                            BDBG_ASSERT(availableIndex <= BXPT_NUM_TPIT_PIDS);
                            taken = true;
                            break;
                        }
                    }
                }
                if (!taken) break;
            }
            if (availableIndex == BXPT_NUM_TPIT_PIDS) {
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }

            pid->tpit.index = availableIndex;
        }

        pid->tpit.filter = *pFilter;
        pid->tpit.filter.pid = pidChannel->status.pid; /* force the pid */

        /*BDBG_MSG(("setting tpit filter slot %d: tpit index %d", i, pid->tpit.index));*/
        rc = BXPT_Rave_SetTpitFilter(r->tpitIdx, pid->tpit.index, (const BXPT_Rave_TpitEntry *)&pid->tpit.filter);
        if (rc) return BERR_TRACE(rc);

        /* only do bookkeeping after complete success */
        if (!pid->tpit.enabled) {
            pid->tpit.enabled = true;
            r->tpitCount++;
            BDBG_ASSERT(r->tpitCount);
        }
    }
    else {
        /* disable */
        if (pid->tpit.enabled) {
            BKNI_Memset(&pid->tpit.filter, 0, sizeof(pid->tpit.filter));

            /*BDBG_MSG(("unsetting tpit filter slot %d: tpit index %d", i, pid->tpit.index));*/
            (void)BXPT_Rave_SetTpitFilter(r->tpitIdx, pid->tpit.index, (const BXPT_Rave_TpitEntry *)&pid->tpit.filter);
            /* don't fail on the disable. we can't mess up the bookkeeping. */

            pid->tpit.enabled = false;
            pid->tpit.index = 0xFFFFFFFF; /* don't care */
            BDBG_ASSERT(r->tpitCount);
            r->tpitCount--;
        }

        /* recpump uninit */
        if (r->tpitCount == 0 && r->tpitIdx) {
            BXPT_Rave_FreeIndexer(r->tpitIdx);
            r->tpitIdx = NULL;
        }
    }

    return 0;
err_pid:
    return rc;
}
