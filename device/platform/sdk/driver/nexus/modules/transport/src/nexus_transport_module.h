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
 * $brcm_Workfile: nexus_transport_module.h $
 * $brcm_Revision: 109 $
 * $brcm_Date: 10/11/12 12:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_transport_module.h $ 
 * 
 * 109   10/11/12 12:06p vsilyaev
 * SW7425-3955: Use upper 8 bits as PES sub-id for MPEG-2 TS streams
 * 
 * 108   9/5/12 6:10p jtna
 * SW7425-3782: rework host and demod TSMF impl. both cases now go through
 *  the host API
 * 
 * 107   6/27/12 6:25p bandrews
 * SW7346-826: merge to main
 * 
 * SW7346-826/3   6/20/12 8:59p bandrews
 * SW7346-826: merge from main
 * 
 * 106   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7346-826/2   6/20/12 8:39p bandrews
 * SW7346-826: clean up FIFO watchdog code
 * 
 * SW7346-826/1   6/14/12 8:50p bandrews
 * SW7346-826: add video non-consumption condition detection and flush
 *  resolution
 * 
 * 105   6/5/12 11:48a erickson
 * SW7340-296: call nexus_unregister only if
 *  NEXUS_Playpump_ClosePidChannel is called
 * 
 * 104   6/1/12 4:18p vsilyaev
 * SW7420-2312: Added reference counting of the StcChannel
 * 
 * 103   5/31/12 4:29p vsilyaev
 * SW7420-2311: Added reference counting for pid channel
 * 
 * 102   5/14/12 7:41p bandrews
 * SW7425-2264: merge to main
 * 
 * SW7425-2264/1   5/4/12 9:50p bandrews
 * SW7425-2264: first stab at low latency TSM impl
 * 
 * 101   3/27/12 11:24a erickson
 * SW7435-77: add #if for minimal resource build
 * 
 * 100   3/2/12 4:56p bandrews
 * SW7425-2276: merge to main
 * 
 * SW7425-2276/2   3/1/12 7:06p bandrews
 * SW7425-2276: improve gap detection; use correct placement for BXVD_PVR_
 *  function call; ensure all counters are 64 bit
 * 
 * SW7425-2276/1   2/23/12 9:08p bandrews
 * SW7425-2276: initial impl of gap detection
 * 
 * 99   1/24/12 10:19a gmullen
 * SW7425-1886: Merged to main
 * 
 * SW7425-1886/1   1/19/12 5:44p gmullen
 * SW7425-1886: CC interrupt now goes to app
 * 
 * 98   1/12/12 12:25p jtna
 * SW7425-1863: correctly conditionally-compile standby-related code
 * 
 * 97   1/11/12 10:59a jtna
 * SW7425-1863: add code that removes app requirement to close/re-open pid
 *  channels
 * 
 * 96   12/23/11 8:54a erickson
 * SW7425-1952: fix 7550
 * 
 * 95   12/16/11 1:14p erickson
 * SW7405-5325: fix 40nm packetsub
 * 
 * 94   12/15/11 2:32p gmullen
 * SW7425-1952: Merged changes to mainline
 * 
 * SW7425-1952/1   12/14/11 4:08p gmullen
 * SW7425-1952: Moved NEXUS_Message structure to nexus_transport_module.h
 * 
 * 93   12/13/11 9:45p bandrews
 * SW7425-1772: merge to main
 * 
 * SW7425-1772/1   12/12/11 10:01p bandrews
 * SW7425-1772: add priority queue for decoder stc management
 * 
 * 92   12/9/11 5:22p bandrews
 * SW7550-772: merge to main
 * 
 * SW7550-772/1   12/8/11 8:58p bandrews
 * SW7550-772: implement default timebase usage
 * 
 * 91   12/2/11 11:20a gmohile
 * SW7425-1183 : Add transport wake up
 * 
 * 90   11/14/11 2:28p erickson
 * SW7425-1448: add NEXUS_Timebase_SetVdecFrameRate and
 *  NEXUS_Timebase_SetHdDviFrameRate
 * 
 * 89   10/31/11 7:49p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/3   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * SW7420-2078/2   10/14/11 8:36p bandrews
 * SW7231-391: update stc channel to work with client timebase
 * 
 * SW7420-2078/1   10/11/11 8:26p bandrews
 * SW7231-391: add support for parser band and timebase protected client
 *  usage
 * 
 * 88   10/5/11 12:24p jtna
 * SW7425-1189: add transport and frontend TSMF support
 * 
 * 87   8/25/11 3:29p erickson
 * SW7346-454: add NEXUS_RecpumpStatus.rave.index
 * 
 * 86   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 85   6/27/11 4:44p bandrews
 * SW7425-654: merge to main
 * 
 * SW7425-654/3   6/27/11 4:10p bandrews
 * SW7425-654: Guard STC reprogram with validity
 * 
 * SW7425-654/2   6/23/11 11:38p bandrews
 * SW7425-654: program STC to zero when realtime set; fix calls to
 *  decoders for STC offset; add nonRealTime flag to STC channel and
 *  pcrlib
 * 
 * SW7425-654/1   6/21/11 9:33p bandrews
 * SW7425-654: Support use of SW STC offset and transmission of offset
 *  between paired STC channels
 * 
 * 84   6/6/11 4:08p jtna
 * SW7425-644: add transport SAM support
 * 
 * 83   5/12/11 3:40p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 82   4/18/11 10:19a erickson
 * SW7335-1168: add NEXUS_RaveStatus.enabled
 *
 * 81   4/6/11 2:56p erickson
 * SW7342-340: add HW rave indices to NEXUS_VideoDecoderExtendedStatus
 *
 * 80   3/8/11 6:08p vsilyaev
 * SW7422-101: Updated logic of handling continuityCountEnabled, so per
 *  band and per pid settings are combined with logical AND
 *
 * 79   3/1/11 5:38p gmohile
 * SW7408-210 : Add standby support
 *
 * 78   3/1/11 10:20a erickson
 * SW7335-1168: internalize #define NEXUS_SW_RAVE_SUPPORT
 *
 * 77   2/28/11 11:06a erickson
 * SW7342-331: add template SW RAVE extension
 *
 * 76   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 75   2/9/11 2:18p erickson
 * SW7420-1009: add NEXUS_ParserBand_Acquire and Release
 *
 * 74   1/28/11 8:55a erickson
 * SW7420-1440: add internal hooks so that NEXUS_PidChannel_Close can
 *  close playpump pidchannels (but not playback pidchannels)
 *
 * 73   1/20/11 2:35p erickson
 * SW7420-1237: support live and playback input to remux, allow per-parser
 *  band input maxDataRate
 *
 * 72   1/5/11 11:06a erickson
 * SW7420-1357: adapt to 7422 XPT PI
 *
 * 71   12/28/10 5:44p vsilyaev
 * SW7425-39: Added Mux specific interfaces
 *
 * 70   12/16/10 6:07p vsilyaev
 * SW7425-39: Added playpump private interface
 *
 * 69   12/13/10 4:06p erickson
 * SW35230-179: add NEXUS_RaveStatus.numOutputBytes
 *
 * 68   11/3/10 4:32p erickson
 * SW7422-20: use BXPT_P_MAX_PACKETIZERS_PER_PB instead of
 *  NEXUS_SW_VOB_SUPPORT
 *
 * 67   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 66   9/8/10 4:41p erickson
 * SW3548-3073: moved definition of struct NEXUS_Message into each
 *  implementation
 *
 * 65   9/1/10 3:42p erickson
 * SW7420-996: add NEXUS_MessageStatus.moreDataAvailable
 *
 * 64   7/14/10 5:37p erickson
 * SW3556-1145: add NEXUS_TimebaseTrackRange to all source type settings
 *
 * 63   6/30/10 10:26a erickson
 * SW7405-4525: fix NEXUS_USE_SW_FILTER chips
 *
 * 62   6/29/10 1:34p erickson
 * SW7405-4525: if bank = -1, add filter to least used bank. add
 *  bank_refcnt to keep track.
 *
 * 61   6/7/10 3:07p erickson
 * SW7550-448: add tsmf extension option
 *
 * 60   5/10/10 8:41p vishk
 * SW7405-3940: re-code nexus_remux.c to use internal linked list instead
 *  of an array of size NEXUS_MAX_REMUX_PID_CHANNELS
 *
 * 59   4/19/10 1:59p erickson
 * SW7420-624: add NEXUS_Message_AddFilter and NEXUS_Message_RemoveFilter
 *  to enable filter groups with pid2buf
 *
 * 58   3/8/10 1:03p erickson
 * SW7405-4008: add BDBG_OBJECT_ASSERT's for message error interrupts
 *
 * 57   2/10/10 11:49a erickson
 * SW7550-159: reversing NEXUS_RAVE_THRESHOLD_UNITS back to 256. 128
 *  causes corruption in the audio RAVE registers. no reason this won't
 *  affect 7550 too.
 *
 * 56   1/20/10 5:13p erickson
 * SW7550-159: allow RAVE to be configured with a non-default
 *  ThresholdGranularityInBytes. switching default to 128, which allows
 *  for an interrupt on one packet.
 *
 * 55   12/11/09 3:32p erickson
 * SW7550-112: merge 7550 code. add support for sw message filtering.
 *  allow for no HW message filtering using standard XPT define.
 *
 * 54   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 53   11/6/09 11:32a erickson
 * SW7405-3287: add swPcrOffset value and enable flag to StcChannel so
 *  that each StcChannel instance can manage a distinct STC value, even
 *  with a shared stcIndex.
 *
 * 52   10/26/09 12:27p erickson
 * SW7405-3287: add support for sw-based pcr offset for mosaic playback
 *  TSM support
 *
 * 51   10/20/09 5:34p jhaberf
 * SW35130-1: Removed DirectTV support for 35230
 *
 * 50   10/19/09 11:26a erickson
 * SW7400-2559: add NEXUS_PidChannelSettings.dssHdFilter
 *
 * 49   10/16/09 4:34p erickson
 * SWDEPRECATED-2636: allow NEXUS_MAX_REMUX_PID_CHANNELS to be overridden
 *  in nexus_platform_features.h
 *
 * 48   10/1/09 5:03p erickson
 * SW7405-3087: add playpump cc check, add cc error counting per pid
 *
 * 47   9/24/09 5:00p erickson
 * SWDEPRECATED-3717: support maxContiguousMessageSize
 *
 * 46   9/18/09 4:31p erickson
 * SWDEPRECATED-3717: allow NEXUS_MessageSettings.bufferSize for pid2buf
 *
 * 45   6/12/09 3:05p jtna
 * PR55971: added NEXUS_Remux.started
 *
 * 44   6/9/09 3:52p erickson
 * PR55611: back out previous change
 *
 * 43   6/8/09 11:49a erickson
 * PR53324: add PID2BUF impl, default off
 *
 * 42   6/2/09 11:45a erickson
 * PR55611: allow timebase to change in NEXUS_StcChannel_SetSettings
 *
 * 41   5/26/09 11:39a erickson
 * PR55105: add packet sub api
 *
 * 40   5/22/09 8:53a erickson
 * PR55402: fix playback parser band support for remux
 *
 * 39   5/18/09 4:24p erickson
 * PR54880: allow StcChannels with different PCR_OFFSET contexts and STC's
 *
 * 38   4/28/09 10:29a erickson
 * PR54656: call BXPT_PCR_DirecTv_GetLastPcr in DSS mode
 *
 * 37   4/24/09 11:40a agadre
 * PR48702:
 * Pull in the NEXUS SVP extension include files under the NEXUS_HAS_SVP
 *  extension.
 *
 * 36   4/20/09 10:53a erickson
 * PR53662: rework XPT power management code to call BXPT_P_CanPowerDown
 *  before powering down core
 *
 * 35   4/17/09 12:31p erickson
 * PR53728: added NEXUS_RemuxSettings.maxDataRate
 *
 * 34   4/14/09 1:44p agadre
 * PR48702:
 * Add SVP includes when ICAM support is defined.
 *
 * 33   4/8/09 4:31p vsilyaev
 * PR 54016: Added support for precompiled header
 *
 * 32   1/19/09 5:24p bandrews
 * PR51021: merge
 *
 * PR51021/2   1/16/09 4:13p bandrews
 * PR51021: Updated to separate permission from enable
 *
 * PR51021/1   1/16/09 12:18a bandrews
 * PR51021: First stab at cleaning up ASTM playback blockage
 *
 * 31   12/17/08 2:31p cnovak
 * PR48702: Merge to main branch: Add icam include, conditional on
 *  NEXUS_HAS_ICAM
 *
 * 30   12/17/08 1:48p katrep
 * PR49875: Add support for 7466
 *
 * 29   12/15/08 5:41p jtna
 * PR49927: add NEXUS_ParserBandSettings.lengthError
 *
 * 28   12/11/08 8:05a vsilyaev
 * PR 48908: Changed algorithm to power up/down blocks
 *
 * 27   12/3/08 1:18p erickson
 * PR45405: rename rave member to pidChannelIndex to avoid
 *  misunderstanding
 *
 * 26   11/24/08 10:28a erickson
 * PR48846: added pcrError callback
 *
 * 25   11/18/08 12:57p erickson
 * PR48155: extend tsm deadlock algo to check more specific case
 *
 * 24   11/18/08 11:54a erickson
 * PR48155: add decoder fifo watchdog logic to recover from TSM deadlocks
 *
 * 23   11/11/08 2:55a erickson
 * PR 48847: added NEXUS_Message_SetSettings
 *
 * 22   9/11/08 3:13p erickson
 * PR46646: added crcError, pesLengthError, pesStartCodeError
 *
 * 20   9/9/08 10:04a erickson
 * PR45940: added NEXUS_Rave_GetCdbValidPointer_isr for better detection
 *  if source fifo goes dry
 *
 * 19   9/3/08 7:34p vishk
 * PR 46315: Sample code for PID remapping
 *
 * 18   5/30/08 10:54a erickson
 * PR43123: add USE_SETTOP_API_SW_RAVE build option, but default it off.
 *
 * 17   5/20/08 7:46p shyam
 * PR 42103 : Add transport MPOD support
 *
 * 16   5/1/08 2:55p erickson
 * PR42391: remove duplicate and unused storage to prevent future bugs
 *
 * 15   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 14   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 13   4/10/08 2:40p erickson
 * PR41557: enable ES packetization for all chips
 *
 * 12   4/10/08 1:58p erickson
 * PR40079: added PidChannel ScramblingCheck
 *
 * 11   4/7/08 10:15a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 10   3/27/08 10:37a erickson
 * PR40851: if parser band is DSS, it should override some input band
 *  settings. if not DSS, don't override.
 *
 * 9   3/25/08 5:25p bandrews
 * PR40090: Added compile flag for ASTM
 *
 * 8   3/24/08 3:22p bandrews
 * PR40090: API changes
 *
 * 7   3/20/08 10:22a erickson
 * PR40716: configure DPCR for analog video sources using frameRate, just
 *  like HdDvi.
 *
 * 6   3/5/08 5:21p erickson
 * PR39435: reorder includes
 *
 * 5   3/4/08 5:09p jrubio
 * PR40169: add 7325
 *
 * 4   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 3   2/26/08 10:01a erickson
 * PR39934: added SW RAVE support
 *
 * 2   2/22/08 5:46p shyam
 * PR39435 : Add remux capability to Nexus
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/30   1/14/08 5:47p jgarrett
 * PR 38708: Adding PCR callback to application
 *
 * Nexus_Devel/29   12/28/07 5:19p erickson
 * PR38470: move conversion functions to core
 *
 * Nexus_Devel/28   11/30/07 1:18p erickson
 * PR34925: rename to nexus_stc_channel.h
 *
 * Nexus_Devel/27   11/19/07 4:33p erickson
 * PR34925: added NEXUS_TimebaseStatus
 *
 * Nexus_Devel/26   11/19/07 1:19p erickson
 * PR34925: split demux.h into separate interface files
 *
 * Nexus_Devel/25   11/16/07 9:35a erickson
 * PR34925: moved SET to Open. moved pcrlibData from global to handle
 * property. fixed SetStc for eHost.
 *
 * Nexus_Devel/24   11/9/07 3:18p erickson
 * PR36814: impl HdDviIn
 *
 * Nexus_Devel/23   11/6/07 3:44p erickson
 * PR34925: added BDBG_OBJECT checking
 *
 * Nexus_Devel/22   10/31/07 9:07a erickson
 * PR36570: added 3563 support
 *
 * Nexus_Devel/21   10/17/07 5:36p vsilyaev
 * PR 35824: Fixed feedback from decoder to playpump
 *
 * Nexus_Devel/20   10/9/07 3:21p vsilyaev
 * PR 35824: Demoted Playpump_Pause to private function
 *
 * Nexus_Devel/19   10/9/07 2:04p erickson
 * PR34925: recpump working
 *
 * Nexus_Devel/18   10/9/07 11:04a vsilyaev
 * PR 34662: Rearranged inclusion order of header files
 *
 * Nexus_Devel/17   10/1/07 1:45p erickson
 * PR34925: make Rave a private API
 *
 * Nexus_Devel/16   9/28/07 10:00a erickson
 * PR34925: 0.1 prerel review
 *
 * Nexus_Devel/15   9/27/07 7:53p jgarrett
 * PR 34954: Added headers
 *
 * Nexus_Devel/14   9/27/07 2:00p erickson
 * PR34925: added syncthunk
 *
 * Nexus_Devel/13   9/26/07 4:17p erickson
 * PR34925: playpump updates
 *
 * Nexus_Devel/12   9/26/07 3:11p erickson
 * PR34925: initial playpump checkin
 *
 * Nexus_Devel/11   9/25/07 3:07p erickson
 * PR34925: update
 *
 * Nexus_Devel/10   9/25/07 1:46p erickson
 * PR34925: fix warnings
 *
 * Nexus_Devel/9   9/24/07 2:36p erickson
 * PR34925: added Message
 *
 * Nexus_Devel/8   9/24/07 10:09a erickson
 * PR34925: update
 *
 * Nexus_Devel/7   9/21/07 4:19p erickson
 * PR32925: update
 *
 * Nexus_Devel/6   9/21/07 11:16a erickson
 * PR34925: update
 *
 * Nexus_Devel/5   9/17/07 4:12p erickson
 * PR34925: added timebase and stcchannel
 *
 * Nexus_Devel/4   9/14/07 3:27p erickson
 * PR34925: initial compilation
 *
 * Nexus_Devel/3   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#ifndef NEXUS_TRANSPORT_MODULE_H__
#define NEXUS_TRANSPORT_MODULE_H__

#include "nexus_base.h"
#include "nexus_platform_features.h"
#include "nexus_transport_thunks.h"
#include "nexus_types.h"
#include "nexus_video_types.h"
#include "nexus_transport_init.h"
#include "priv/nexus_transport_standby_priv.h"
#include "nexus_pid_channel.h"
#include "nexus_pid_channel_scrambling.h"
#include "nexus_timebase.h"
#include "nexus_stc_channel.h"
#include "nexus_message.h"
#include "nexus_message_sam.h"
#include "nexus_remux.h"
#include "nexus_input_band.h"
#include "nexus_parser_band.h"
#include "nexus_packetsub.h"
#include "nexus_playpump.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_timebase_priv.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_playpump_priv.h"
#include "priv/nexus_pid_channel_priv.h"
#include "priv/nexus_core.h"
#include "nexus_transport_capabilities.h"
#include "nexus_recpump.h"
#include "nexus_core_utils.h"
#include "priv/nexus_core.h"
#include "nexus_mpod.h"
#include "nexus_tsmf.h"
#include "nexus_transport_wakeup.h"
#if NEXUS_HAS_ICAM
#include "nexus_icam.h"
#endif
#if NEXUS_HAS_SVP
#include "nexus_svp.h"
#endif

#include "blst_queue.h"

#include "bxpt.h"
#if BXPT_HAS_DIRECTV_SUPPORT
/* convert #define */
#define B_HAS_DSS 1
#endif
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "bxpt_rave.h"
#if NEXUS_NUM_REMUX
#include "bxpt_remux.h"
#endif
#if B_HAS_DSS
#include "bxpt_directv.h"
#include "bxpt_directv_pcr.h"
#include "bxpt_directv_pcroffset.h"
#endif
#include "bpcrlib.h"
#include "bchp_xpt_rave.h"

#if NEXUS_USE_SW_FILTER
#include "nexus_message_swfilter_priv.h"
#endif
#if NEXUS_USE_MSGLIB
#include "bmsglib.h"
#endif

#if BXPT_HAS_TSMUX
#include "priv/nexus_tsmux_priv.h"
#endif

#if BXPT_HAS_WAKEUP_PKT_SUPPORT
#include "bchp_xpt_wakeup.h"
#include "bxpt_wakeup.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME transport
#define NEXUS_MODULE_SELF g_NEXUS_Transport_P_State.transportModule

BDBG_OBJECT_ID_DECLARE(NEXUS_Rave);
BDBG_OBJECT_ID_DECLARE(NEXUS_PidChannel);
BDBG_OBJECT_ID_DECLARE(NEXUS_StcChannel);
BDBG_OBJECT_ID_DECLARE(NEXUS_Remux);
BDBG_OBJECT_ID_DECLARE(NEXUS_Message);
BDBG_OBJECT_ID_DECLARE(NEXUS_ParserBand);
BDBG_OBJECT_ID_DECLARE(NEXUS_Timebase);

#ifndef NEXUS_RAVE_THRESHOLD_UNITS
/* XPT HW has a programmable units for CDB/ITB threshold interrupts. Nexus uses a fixed value. */
#define NEXUS_RAVE_THRESHOLD_UNITS 256
#endif

#ifndef BXPT_NUM_FILTER_BANKS
/*
XPT legacy (pre-7422) mapping
if NEXUS_HAS_LEGACY_XPT == 0, then it's latest XPT. today that's 7422, 7425.
if NEXUS_HAS_LEGACY_XPT == 1, then it's pre-7422.
NEXUS_HAS_LEGACY_XPT could have other non-zero values as XPT evolves.
*/
#define NEXUS_HAS_LEGACY_XPT 1
#define BXPT_NUM_FILTER_BANKS   BXPT_P_MAX_FILTER_BANKS
#define BXPT_NUM_TPIT_PIDS      BXPT_P_MAX_TPIT_PIDS
#define BXPT_NUM_PID_PARSERS    BXPT_P_MAX_PID_PARSERS
#define BXPT_NUM_PLAYBACKS      BXPT_P_MAX_PLAYBACKS
#define BXPT_NUM_REMULTIPLEXORS BXPT_P_MAX_REMULTIPLEXORS
#define BXPT_NUM_PCR_OFFSET_CHANNELS BXPT_P_MAX_PCR_OFFSET_CHANNELS
#define BXPT_NUM_PCRS           BXPT_P_MAX_PCRS
#define BXPT_NUM_STCS           BXPT_P_MAX_STCS
#define BXPT_NUM_PACKETSUBS     BXPT_P_MAX_PACKETSUBS
#define NEXUS_PARSER_BAND_CC_CHECK 1
#endif

/* SW RAVE is supported by default on all chips */
#ifndef NEXUS_SW_RAVE_SUPPORT
#define NEXUS_SW_RAVE_SUPPORT 1
#endif

/* playpump/recpump with crypto is only supported with MEM_DMA HW, not SHARF_DMA HW.
   we have to use NEXUS_NUM_DMA_CHANNELS, not NEXUS_HAS_DMA, because the latter is 
   defined even if the platform only has SHARF_DMA */
#if NEXUS_NUM_DMA_CHANNELS && NEXUS_HAS_SECURITY
#define NEXUS_ENCRYPTED_DVR_WITH_M2M 1
#endif

typedef struct NEXUS_ParserBand * NEXUS_ParserBandHandle;

typedef struct NEXUS_Remux_P_PidChannel {
    BLST_S_ENTRY(NEXUS_Remux_P_PidChannel) link;
    NEXUS_PidChannelHandle  pidChn;
} NEXUS_Remux_P_PidChannel;

struct NEXUS_Remux {
    NEXUS_OBJECT(NEXUS_Remux);
#if NEXUS_NUM_REMUX
    BXPT_Remux_Handle xptRmx ;
#endif
    NEXUS_RemuxSettings settings;
    unsigned index;      /* This channels index. */
    unsigned parserBandCount;
#if defined(NEXUS_NUM_PARSER_BANDS) && defined(NEXUS_NUM_PLAYPUMPS)
#define NEXUS_NUM_REMUX_PARSER_INPUTS (NEXUS_NUM_PARSER_BANDS + NEXUS_NUM_PLAYPUMPS)
#elif defined(NEXUS_NUM_PARSER_BANDS)
#define NEXUS_NUM_REMUX_PARSER_INPUTS (NEXUS_NUM_PARSER_BANDS)
#else
#define NEXUS_NUM_REMUX_PARSER_INPUTS (NEXUS_NUM_PLAYPUMPS)
#endif
    NEXUS_RemuxParserBandwidth remuxParserBandwidth[NEXUS_NUM_REMUX_PARSER_INPUTS];
    unsigned pidChannelCount[NEXUS_NUM_REMUX_PARSER_INPUTS];
    bool started;
    BLST_S_HEAD(NEXUS_Remux_P_PidChannels, NEXUS_Remux_P_PidChannel) pid_list;
};

/* this is the implementation of NEXUS_RaveHandle */
struct NEXUS_Rave {
    BDBG_OBJECT(NEXUS_Rave)
    BXPT_RaveCx_Handle raveHandle; /* This is the RAVE channel, aka device. */
    NEXUS_RaveSettings settings;
    BAVC_XptContextMap xptContextMap;
    unsigned index;
    bool enabled;
    unsigned pidChannelIndex;
    NEXUS_TimerHandle timer;
    uint32_t lastValid; /* CDB_VALID monitoring */
    uint64_t numOutputBytes;
#if NEXUS_SW_RAVE_SUPPORT
    struct {
        int index; /* HW index, -1 if unused */
        BXPT_RaveCx_Handle raveHandle;
        BAVC_XptContextMap xptContextMap;
        bool enabled;
        void *extensionData;
    } swRave;
#endif
};

struct NEXUS_PcrlibData {
    BLST_Q_ENTRY(NEXUS_PcrlibData) link;
    NEXUS_StcChannelDecoderType type;
    unsigned priority;
    bool connected;
    BPCRlib_StcDecIface iface; /* interface expected by pcrlib */
    NEXUS_StcChannelCallbacks callbacks;
    bool isHung;
    unsigned percentFull;
    bool requiresFlush;
};

/* this is the implementation of NEXUS_StcChannelHandle */
struct NEXUS_StcChannel {
    NEXUS_OBJECT(NEXUS_StcChannel);
    bool isStcTrick;
    bool frozen;
    unsigned increment;
    unsigned prescale;
    bool stcValid;
    bool nonRealTime;
    BXPT_PcrOffset_Handle pcrOffset;
    unsigned stcIndex; /* STC HW index */
    NEXUS_StcChannelSettings settings;
    NEXUS_TimebaseHandle timebase;
    BPCRlib_Channel pcrlibChannel;
    struct NEXUS_PcrlibData pcrlibData[NEXUS_StcChannelDecoderType_eMax];
    BLST_Q_HEAD(NEXUS_StcChannel_DecoderPriority_Head, NEXUS_PcrlibData) decoderPriorities;
    bool swPcrOffsetEnabled;
    unsigned swPcrOffset; /* set if swPcrOffsetEnabled = true */
#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
        NEXUS_StcChannelAstmSettings settings;
    } astm;
#endif
    NEXUS_StcChannelDecoderType lowLatencyDecoder;
    NEXUS_StcChannelHandle pairedChannel;
    NEXUS_StcChannelHandle lastStallingStcChannel;
    bool lastCounterValueValid;
    uint64_t lastCounterValue;
};

/* this is the implementation of NEXUS_PidChannelHandle */
struct NEXUS_PidChannel {
    NEXUS_OBJECT(NEXUS_PidChannel);
    BLST_S_ENTRY(NEXUS_PidChannel) link;
    NEXUS_ParserBandHandle parserBand;
    NEXUS_PidChannelStatus status;
    unsigned combinedPid;
    NEXUS_PidChannelSettings settings;
    NEXUS_PlaypumpHandle playpump; /* set if this is a playpump pidchannel */
    struct {
        NEXUS_RaveHandle rave;
        NEXUS_PidChannelScramblingSettings settings;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        NEXUS_TimerHandle dummyreadtimer;
#endif
    } scramblingCheck;
};

#ifndef NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES
#if NEXUS_USE_MSGLIB
#define NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES 256
#else
/* The PID2BUF table is limited to 128 */
#define NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES 128
#endif
#endif

/* this is the impl of NEXUS_MessageHandle */
#ifndef NEXUS_USE_SW_FILTER /* SW message filter uses seperate impl maintained in nexus_message_swfilter.c */
struct NEXUS_Message {
    NEXUS_OBJECT(NEXUS_Message);
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_MessageStatus status;
    unsigned FilterNum;
    unsigned BankNum;
    int MesgBufferNum; /* -1 means the buffer num has not been assigned */
    unsigned PidChannelNum;
    BXPT_PsiMessageSettings psiMessageSettings;

    void *buffer; /* actual buffer in use. cached addr. */
    void *uncachedBuffer; /* uncached addr for buffer. */
    unsigned bufferSize;

    void *allocatedBuffer; /* buffer allocated by Open if bufferSize is set. can be overridden by Start buffer. */

    unsigned lastGetBufferLength; /* last length returned by NEXUS_Message_GetBuffer. this could be from the main
                                     buffer or wrappedMessage. */

    struct {
        void *buffer; /* size is settings.maxContiguousMessageSize */
        unsigned length; /* size of message in the buffer */
        unsigned amountConsumed;
    } wrappedMessage;

    struct {
        const void *buffer; /* point to data last retrieved from XPT PI */
        unsigned length; /* length of data last retrieved from XPT PI */
        unsigned amountConsumed; /* total amount consumed from the buffer/length last reported by XPT PI */
    } getBufferState;

#define NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS 32
    struct {
        bool used;
        unsigned filterNum;
    } additionalFilters[NEXUS_MESSAGE_P_MAX_ADDITIONAL_FILTERS];

    bool started;

#if NEXUS_MESSAGE_USE_CHECK_TIMER || NEXUS_USE_MSGLIB
/* if you stop getting dataReady callbacks, it could be because you aren't calling ReadComplete. this was a fix after
nexus_message_pid2buf.c was released for a while, so I'm adding this debug option. */

/* Also needed to support the older non-PID2BUF message code, hence the check for NEXUS_USE_MSGLIB. */
    unsigned noReadCompleteCount;
    NEXUS_TimerHandle checkTimer;
#endif

    NEXUS_IsrCallbackHandle dataReady, overflow, psiLengthError, crcError, pesLengthError, pesStartCodeError;

#if NEXUS_USE_MSGLIB
    /* Need to support both PID2BUF and non-PID2BUF software builds with this common struct. */
    BMSGlib_Session_Handle session;
#endif
};
#endif /* NEXUS_USE_SW_FILTER */

struct NEXUS_ParserBand
{
    NEXUS_OBJECT(NEXUS_ParserBand);
    NEXUS_ParserBand enumBand; /* enum variant */
    unsigned hwIndex;
    NEXUS_ParserBandSettings settings;
    int refcnt;
    int pidChannels;
    bool enabled;
    bool acquired;
    BINT_CallbackHandle ccErrorInt;
    NEXUS_IsrCallbackHandle ccErrorCallback;
    BINT_CallbackHandle teiErrorInt;
    NEXUS_IsrCallbackHandle teiErrorCallback;
    BINT_CallbackHandle lengthErrorInt;
    NEXUS_IsrCallbackHandle lengthErrorCallback;
};

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_ParserBand);

struct NEXUS_Timebase 
{
    NEXUS_OBJECT(NEXUS_Timebase);
    NEXUS_Timebase enumTimebase; /* enum variant */
    unsigned hwIndex;
    BXPT_PCR_Handle pcr;
    bool isDss;
    bool acquired;
    NEXUS_TimebaseSettings settings;
    NEXUS_TimebaseStatus status;
    BINT_CallbackHandle intMonitorCallback;
    NEXUS_IsrCallbackHandle monitorCallback;
    BINT_CallbackHandle intPcrErrorCallback;
    NEXUS_IsrCallbackHandle pcrErrorCallback;
    NEXUS_VideoFrameRate hdDviFrameRate;
    NEXUS_VideoFrameRate vdecFrameRate;
#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
        NEXUS_TimebaseAstmSettings settings;
        bool sourceTypeSaved;
        NEXUS_TimebaseSourceType savedSourceType;
    } astm;
#endif /* NEXUS_HAS_ASTM */
};

typedef enum NEXUS_XptDataInterrupt {
    NEXUS_XptDataInterrupt_ePsiLengthError,
    NEXUS_XptDataInterrupt_eCrcError,
    NEXUS_XptDataInterrupt_ePesLengthError,
    NEXUS_XptDataInterrupt_ePesStartCodeError,
    NEXUS_XptDataInterrupt_eMax
} NEXUS_XptDataInterrupt;

struct NEXUS_Transport_P_State {
    NEXUS_ModuleHandle transportModule;
    BXPT_Handle xpt;
    BPCRlib_Handle pcrlib;
#if NEXUS_USE_MSGLIB
    BMSGlib_Handle msglib;
#endif
    unsigned powerCount;

#if NEXUS_NUM_INPUT_BANDS
    struct {
        NEXUS_InputBandSettings settings;
        NEXUS_TransportType transportType;
        bool enabled;
    } inputBand[NEXUS_NUM_INPUT_BANDS];
#endif
#if NEXUS_NUM_PARSER_BANDS
    struct NEXUS_ParserBand parserBand[NEXUS_NUM_PARSER_BANDS];
#endif
#if NEXUS_NUM_PLAYPUMPS
    struct {
        NEXUS_PlaypumpHandle playpump; /* dynamically allocated by NEXUS_Playpump_Open */
    } playpump[NEXUS_NUM_PLAYPUMPS];
#endif
#if NEXUS_NUM_RECPUMPS
    NEXUS_RecpumpHandle recpump[NEXUS_NUM_RECPUMPS];
#endif

#if NEXUS_NUM_PID_CHANNELS
    BLST_S_HEAD(NEXUS_Transport_P_PidChannels, NEXUS_PidChannel) pidChannels;
    unsigned hwPidChannelRefCnt[NEXUS_NUM_PID_CHANNELS];
#endif

#if NEXUS_NUM_TIMEBASES
    struct NEXUS_Timebase timebase[NEXUS_NUM_TIMEBASES];
#endif

#if NEXUS_NUM_STC_CHANNELS
    struct NEXUS_StcChannel stcChannel[NEXUS_NUM_STC_CHANNELS];
#endif

#if NEXUS_NUM_RAVE_CHANNELS
    struct {
        BXPT_Rave_Handle channel;
        struct NEXUS_Rave context[NEXUS_NUM_RAVE_CONTEXTS];
    } rave[NEXUS_NUM_RAVE_CHANNELS];
#endif

    struct {
        NEXUS_MessageHandle handle[NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES];
        BINT_CallbackHandle xptDataInterrupt[NEXUS_XptDataInterrupt_eMax];
#if !NEXUS_USE_SW_FILTER
        unsigned bank_refcnt[BXPT_NUM_FILTER_BANKS];
#endif
    } message;
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    struct {
        BINT_CallbackHandle intPacketFoundCallback;
        /* BKNI_EventHandle event; */
        /* NEXUS_EventCallbackHandle eventHandle; */
        NEXUS_IsrCallbackHandle wakeupCallback;
        NEXUS_TransportWakeupSettings settings;
    } wakeup;
#endif
#if (!NEXUS_HAS_LEGACY_XPT) && NEXUS_NUM_PARSER_BANDS
    struct {
        bool parserEnabled[NEXUS_NUM_PARSER_BANDS];
    } standby;
#endif

    NEXUS_TransportModuleSettings settings;
};

/* global module handle & data */
extern struct NEXUS_Transport_P_State g_NEXUS_Transport_P_State;
#define pTransport (&g_NEXUS_Transport_P_State)

#if NEXUS_HAS_TSMF
typedef struct NEXUS_Tsmf_P_State
{
    BLST_S_HEAD(NEXUS_TsmfContexts, NEXUS_Tsmf) handles;
} NEXUS_Tsmf_P_State;

extern NEXUS_Tsmf_P_State g_NEXUS_Tsmf_P_State;
#endif


void NEXUS_Playpump_P_ConsumerStarted(NEXUS_PlaypumpHandle p);

NEXUS_Error NEXUS_Timebase_P_StartMonitor(NEXUS_TimebaseHandle timebase);
void NEXUS_Timebase_P_StopMonitor(NEXUS_TimebaseHandle timebase);
void NEXUS_Timebase_GetDefaultSettings_priv(NEXUS_TimebaseSettings *pSettings);
NEXUS_Error NEXUS_Timebase_P_SetSettings(NEXUS_TimebaseHandle timebase, const NEXUS_TimebaseSettings *pSettings);
void NEXUS_Timebase_P_GetSettings(NEXUS_TimebaseHandle timebase, NEXUS_TimebaseSettings *pSettings);

void NEXUS_ParserBand_P_SetEnable(NEXUS_ParserBandHandle parserBand);
void NEXUS_ParserBand_P_CountCcErrors_isr(void);
void NEXUS_ParserBand_P_GetSettings(NEXUS_ParserBandHandle band, NEXUS_ParserBandSettings *pSettings);
NEXUS_Error NEXUS_ParserBand_P_SetSettings(NEXUS_ParserBandHandle band, const NEXUS_ParserBandSettings *pSettings);
NEXUS_ParserBandHandle NEXUS_ParserBand_Resolve_priv(NEXUS_ParserBand band);

NEXUS_Error NEXUS_InputBand_P_SetTransportType(NEXUS_InputBand inputBand, NEXUS_TransportType transportType);

NEXUS_PidChannelHandle NEXUS_PidChannel_P_Open(NEXUS_ParserBandHandle parserBand, NEXUS_PlaypumpHandle playpump, unsigned pid,
    const NEXUS_PidChannelSettings *pSettings, bool bandContinuityCountEnabled);
void NEXUS_PidChannel_P_Close(NEXUS_PidChannelHandle handle);
NEXUS_Error NEXUS_Playpump_P_ClosePidChannel(NEXUS_PlaypumpHandle p, NEXUS_PidChannelHandle pidChannel);


void NEXUS_Message_P_FireInterrupt_isr(NEXUS_MessageHandle msg, unsigned pidChannelIndex, NEXUS_XptDataInterrupt xptDataInterrupt);
bool NEXUS_Message_P_HasCallback(NEXUS_MessageHandle msg, NEXUS_XptDataInterrupt xptDataInterrupt);

void NEXUS_Transport_P_SetInterrupts(void);
void NEXUS_Transport_P_IncPowerDown(bool powered);

/* calculate rave index from CDB_READ register address */
#define NEXUS_RAVE_INDEX(CDB_READ)    (((CDB_READ) - BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR) / (BCHP_XPT_RAVE_CX1_AV_CDB_READ_PTR - BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR))

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Playpump);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Recpump);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_PacketSub);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Message);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Tsmf);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Remux);

#ifdef __cplusplus
}
#endif

#endif
