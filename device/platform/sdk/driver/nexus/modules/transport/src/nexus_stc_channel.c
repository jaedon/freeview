/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_stc_channel.c $
* $brcm_Revision: 109 $
* $brcm_Date: 10/2/12 4:53p $
*
* API Description:
*   Management of STC Channels that deliver timebases to decoders.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/src/nexus_stc_channel.c $
* 
* 109   10/2/12 4:53p hongtaoz
* SW7425-4022: added HW stc value debug message in NRT mode;
* 
* 108   8/31/12 4:53p erickson
* SWDEPRECATED-2917: don't allow NULL pcr pidChannel. switching to
*  freerun doesn't work because decoders are waiting for a PCR.
* 
* 107   8/16/12 3:42p bandrews
* SW7552-316: fix lack of type setting after ClearCallbacks
* 
* 106   8/15/12 5:40p bandrews
* SW7552-316: fixing bad message format
* 
* 105   8/9/12 4:57p erickson
* SW7425-2734: allow server to limit resources for untrusted clients
* 
* 104   7/31/12 5:40p bandrews
* SW7552-313: guard stc request regen for audio, in case there are no
*  audios added
* 
* 103   7/5/12 5:16p bandrews
* SW7231-876: really fix warnings
* 
* 102   7/5/12 5:15p bandrews
* SW7231-876: fixed warnings
* 
* 101   6/27/12 6:25p bandrews
* SW7346-826: merge to main
* 
* SW7346-826/5   6/20/12 9:19p bandrews
* SW7346-826: fix warning
* 
* SW7346-826/4   6/20/12 9:16p bandrews
* SW7346-826: fix IsFrozen priv api
* 
* SW7346-826/3   6/20/12 9:02p bandrews
* SW7346-826: merge from main
* 
* 100   6/19/12 12:44p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* SW7346-826/2   6/20/12 8:39p bandrews
* SW7346-826: clean up FIFO watchdog code
* 
* SW7346-826/1   6/14/12 8:50p bandrews
* SW7346-826: add video non-consumption condition detection and flush
*  resolution
* 
* 99   6/1/12 4:18p vsilyaev
* SW7420-2312: Added reference counting of the StcChannel
* 
* 98   5/14/12 7:41p bandrews
* SW7425-2264: merge to main
* 
* SW7425-2264/2   5/7/12 7:31p bandrews
* SW7425-2264: fix build errors
* 
* SW7425-2264/1   5/4/12 9:50p bandrews
* SW7425-2264: first stab at low latency TSM impl
* 
* 97   4/12/12 4:00p jgarrett
* SW7425-2378: Adding Monkey's Audio and MLP
* 
* 96   4/9/12 3:50p jgarrett
* SW7231-662: Adding AIFF support
* 
* 95   3/5/12 4:01p bandrews
* SW7425-2276: guard NRT with BXPT_HAS_TSMUX
* 
* 94   3/2/12 6:35p bandrews
* SW7425-2276: update messages to include stc channel index and pointer;
*  fix erroneous deadlock detect while gap detect is waiting for a cycle
* 
* 93   3/2/12 4:56p bandrews
* SW7425-2276: merge to main
* 
* 92   2/22/12 4:32p erickson
* SW7425-2458: add BDBG_OBJECT_ASSERT for pidChannel
* 
* SW7425-2276/3   3/1/12 7:06p bandrews
* SW7425-2276: improve gap detection; use correct placement for BXVD_PVR_
*  function call; ensure all counters are 64 bit
* 
* SW7425-2276/2   2/27/12 5:12p bandrews
* SW7425-2276: call XDM directly, use pcrOffset handle properly
* 
* SW7425-2276/1   2/23/12 9:08p bandrews
* SW7425-2276: initial impl of gap detection
* 
* 91   2/21/12 2:19p erickson
* SW7425-2130: fix debug warnings
* 
* 90   2/7/12 7:34p jgarrett
* SW7425-2136: Adding FLAC support
* 
* 89   12/16/11 6:04p bandrews
* SW7125-1187: change error returned to NOT_AVAILABLE
* 
* 88   12/16/11 6:02p bandrews
* SW7125-1187: if stcChannel->timebase is NULL, return error
* 
* 87   12/13/11 9:45p bandrews
* SW7425-1772: merge to main
* 
* SW7425-1772/9   12/13/11 5:58p bandrews
* SW7425-1772: condition decoder queue debug; clear pcrlib config before
*  filling it out
* 
* SW7425-1772/8   12/13/11 5:37p bandrews
* SW7425-1772: remove a message
* 
* SW7425-1772/7   12/13/11 5:30p bandrews
* SW7425-1772: fixed warning
* 
* SW7425-1772/6   12/13/11 5:29p bandrews
* SW7425-1772: debug support for 6 ch audio
* 
* SW7425-1772/5   12/12/11 10:09p bandrews
* SW7425-1772: merge from main
* 
* 86   12/9/11 5:22p bandrews
* SW7550-772: merge to main
* 
* SW7550-772/2   12/9/11 4:06p bandrews
* SW7550-772: fix compile error
* 
* SW7550-772/1   12/8/11 8:58p bandrews
* SW7550-772: implement default timebase usage
* 
* 85   11/21/11 2:48p jgarrett
* SW7425-1406: Adding Vorbis audio support
* 
* VORBIS_DEVEL/2   11/16/11 1:51p jgarrett
* SW7425-1406: Merging latest main branch changes
* 
* SW7425-1772/4   12/12/11 10:01p bandrews
* SW7425-1772: add priority queue for decoder stc management
* 
* SW7425-1772/3   12/1/11 8:18p bandrews
* SW7425-1772: allow decoders 3-5 request stc to renotify if any other
*  decoder already validated
* 
* SW7425-1772/2   11/17/11 9:18p bandrews
* SW7425-1772: don't allow decoders 3-5 to error or request
* 
* SW7425-1772/1   11/17/11 6:31p bandrews
* SW7425-1772: 6 channel support hack
* 
* 84   10/31/11 7:48p bandrews
* SW7231-391: merge to main
* 
* 83   10/21/11 12:58p hongtaoz
* SW7425-1547: added stc index in the debug messages;
* 
* 82   10/19/11 2:03p hongtaoz
* SW7425-1546: fixed MOD300 math for NRT stc increment;
* 
* SW7420-2078/2   10/25/11 5:22p bandrews
* SW7231-391: update parser band and timebase implementations to use
*  handles everywhere, even for legacy enum usage
* 
* SW7420-2078/1   10/14/11 8:36p bandrews
* SW7231-391: update stc channel to work with client timebase
* 
* 81   10/10/11 4:16p erickson
* SW7420-1009: fix NEXUS_ANY_ID impl
* 
* VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
* SW7425-1406: Adding Vorbis support
* 
* 80   9/27/11 7:36p bandrews
* SW7425-1044: stc channel will now respond to request stc by renotifying
*  decoder of stc validity if it was already valid
* 
* 79   8/19/11 7:10p hongtaoz
* SW7425-767: add 2 to STC_OFFSET to tolerate the jitter caused by 1/1001
*  frame interval rounding error;
* 
* 78   8/12/11 4:17p jtna
* SW7425-1070: merge video primer ptsStcDiffCorrection feature
* 
* 77   8/2/11 2:14p erickson
* SW7420-1009: support NEXUS_ANY_ID
* 
* 76   7/28/11 3:53p bandrews
* SW7425-654: only set STC to zero on entering NRT mode, not on every NRT
*  config
* 
* 75   6/27/11 4:47p bandrews
* SW7425-654: merge to main
* 
* SW7425-654/7   6/27/11 4:14p bandrews
* SW7425-654: provide suitable open defaults
* 
* SW7425-654/6   6/27/11 4:10p bandrews
* SW7425-654: Guard STC reprogram with validity
* 
* SW7425-654/5   6/24/11 7:17p bandrews
* SW7425-654: fix NRT vs. RT message so that it correctly reflects the
*  state requested
* 
* SW7425-654/4   6/24/11 12:32a bandrews
* SW7425-654: update to tell pcrlib about pairing
* 
* SW7425-654/3   6/23/11 11:38p bandrews
* SW7425-654: program STC to zero when realtime set; fix calls to
*  decoders for STC offset; add nonRealTime flag to STC channel and
*  pcrlib
* 
* SW7425-654/2   6/22/11 3:59p bandrews
* SW7425-654: use connected state to set paired channel handles
* 
* SW7425-654/1   6/21/11 9:33p bandrews
* SW7425-654: Support use of SW STC offset and transmission of offset
*  between paired STC channels
* 
* 74   6/14/11 4:59p hongtaoz
* SW7425-654: compute stcIncrement in MOD300 or binary format according
*  to transport type;
* 
* 73   6/10/11 3:54p vsilyaev
* SW7425-654: Added triggerNumber to the
*  NEXUS_StcChannelNonRealtimeSettings
* 
* 72   6/10/11 11:05a vsilyaev
* SW7425-654: Fixed typo
* 
* 71   6/9/11 6:54p vsilyaev
* SW7425-653: Override ExternalTriggerNum with 0
* 
* 70   6/9/11 6:44p vsilyaev
* SW7425-654: Added support for non realtime video decode
* 
* 69   6/8/11 2:21p vsilyaev
* SW7425-654: Fixed warning
* 
* 68   6/8/11 2:20p vsilyaev
* SW7425-654: Updated to match new PI
* 
* 67   6/7/11 3:46p vsilyaev
* SW7425-654: Added API to support non-realtime transcode
* 
* 66   5/18/11 7:25p jgarrett
* SW7231-148: Adding RMFF and Cook support
* 
* 65   5/18/11 12:53p erickson
* SW7420-1886: default new stc channel to normal rat
* 
* 64   4/20/11 6:10p jtna
* SW7425-365: fix build warnings for 2.6.37 kernel
* 
* 63   3/17/11 4:42p erickson
* SW7346-120: add NEXUS_StcChannel_VerifyPidChannel
*
* 62   2/28/11 8:48a hongtaoz
* SW7425-84,SW7425-85,SW7425-102: fixed compile error for non-7425
*  platforms;
*
* 61   2/27/11 12:09p hongtaoz
* SW7425-84, SW7425-85, SW7425-102: set encode STC to binary mode;
*
* 60   1/5/11 11:06a erickson
* SW7420-1357: adapt to 7422 XPT PI
*
* 59   1/5/11 10:21a erickson
* SW7420-1357: add param validation, macro validation
*
* 58   12/30/10 4:18p vsilyaev
* SW7425-39: Fixed typo
*
* 57   12/30/10 11:34a vsilyaev
* SW7425-39: Added control over PCR bits
*
* 56   11/15/10 10:59a erickson
* SW7408-144: autoConfigTimebase should use
*  NEXUS_StcChannelSettings.modeSettings.pcr.maxPcrError if appropriate
*
* 55   11/4/10 6:53p bandrews
* SW7405-4876: merge to main
*
* 54   11/2/10 6:39p bandrews
* SW3548-1159: added non-TSM mode recovery methods and some options for
*  controlling ASTM better
*
* 53   10/21/10 8:28p bandrews
* SW3548-1282: need to fake request STC in STC master mode, using both
*  audio and video
*
* SW7405-4876/4   10/22/10 6:55p bandrews
* SW7405-4876: move complicated code into separate function
*
* SW7405-4876/3   10/21/10 8:23p bandrews
* SW3548-1282: actually, make both audio and video request when using STC
*  master mode, to simulate real systems
*
* SW7405-4876/2   10/21/10 8:07p bandrews
* SW3548-1282: need to fake request for STC when using STC master mode,
*  too -> using audio
*
* SW7405-4876/1   9/15/10 7:57p bandrews
* SW7405-4876: update pcrlib audio channel usage
*
* 52   3/26/10 1:00p vsilyaev
* SW7550-349: Use offsetThreshold to guarantee STC update
*
* 51   11/6/09 11:32a erickson
* SW7405-3287: add swPcrOffset value and enable flag to StcChannel so
*  that each StcChannel instance can manage a distinct STC value, even
*  with a shared stcIndex.
*
* 50   10/28/09 3:21p erickson
* SW7405-3287: rework SetStc (internal and external) to support shared
*  stcIndex and swPcrOffset
*
* 49   10/26/09 12:26p erickson
* SW7405-3287: add support for sw-based pcr offset for mosaic playback
*  TSM support
*
* 48   8/11/09 2:22p erickson
* PR56965: switch to #ifdef BXPT_HAS_MOSAIC_SUPPORT
*
* 47   7/31/09 2:42p gmohile
* PR 56512 : Add 7403 support
*
* 46   7/30/09 4:47p erickson
* PR54880: add NEXUS_StcChannel_GetSerialStc_priv
*
* 45   7/17/09 7:13p bandrews
* PR49215: astm dvr support
*
* 44   7/15/09 7:24p vsilyaev
* PR 55653: Added WAV format
*
* 43   6/9/09 3:50p erickson
* PR55611: back out previous changes. allow timebase changes by using new
*  XPT PI api.
*
* 42   6/2/09 11:45a erickson
* PR55611: allow timebase to change in NEXUS_StcChannel_SetSettings
*
* 41   5/18/09 4:24p erickson
* PR54880: allow StcChannels with different PCR_OFFSET contexts and STC's
*
* 40   5/18/09 2:12p erickson
* PR54880: use non-isr versions of GetStc and GetOffset
*
* 39   5/18/09 12:42p erickson
* PR54880: use NEXUS_TimebaseTrackRange enum
*
* 38   5/11/09 3:21p erickson
* PR54994: NEXUS_StcChannel_GetDefaultSettings(1, &settings) should
*  default to NEXUS_Timebase_e1
*
* 37   4/16/09 8:49a erickson
* PR49940: switch default to
*  NEXUS_StcChannelAutoModeBehavior_eVideoMaster
*
* 36   1/29/09 9:25p bandrews
* PR51691: need to check autoConfigTimebase when not called by ASTM
*
* 35   1/29/09 9:18p bandrews
* PR51691: apply to all checks for source type auto config
*
* 34   1/29/09 5:17a bandrews
* PR51691: pass through source type when not called by astm
*
* 33   1/28/09 6:31a bandrews
* PR51659: astm will only override the sourceType autoConfig
*
* 32   1/20/09 4:59p bandrews
* PR51021: Change warning to message
*
* 31   1/19/09 5:27p bandrews
* PR51021: merge
*
* PR51021/2   1/16/09 4:12p bandrews
* PR51021: Updated to separate permission from enable
*
* PR51021/1   1/16/09 12:18a bandrews
* PR51021: First stab at cleaning up ASTM playback blockage
*
* 30   12/30/08 10:53a erickson
* PR49582: must memset NEXUS_PcrlibData to zero after
*  BPCRlib_Channel_SetConfig to avoid crash race condition
*
* 29   12/15/08 12:26p erickson
* PR50305: added memset for consistent Open/Close functionality
*
* 28   12/8/08 3:00p bandrews
* PR50016: ensure that even if playback is set before start ASTM we
*  default dependencies
*
* 27   12/2/08 9:11p bandrews
* PR49489: regenerate STC request upon entering a PTS master mode
*
* 26   11/19/08 9:47a erickson
* PR49212: merge new status functions
*
* 25   11/18/08 8:53p bandrews
* PR49212: Temporarily block ASTM during playback
*
* 24   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 23   11/18/08 12:57p erickson
* PR48155: extend tsm deadlock algo to check more specific case
*
* 22   11/18/08 11:54a erickson
* PR48155: add decoder fifo watchdog logic to recover from TSM deadlocks
*
* 21   10/24/08 6:48p ssood
* PR47890: StcChannel_SetSettings is not using the user passed setting
*  for disabling jitter adjustment of PCR offset block while updating Pid
*  Channel Settings
*
* 20   10/9/08 11:46a jgarrett
* PR 46648: Default STC channel to STC master, not video master
*
* 19   10/6/08 11:26p erickson
* PR46648: merge
*
* PR46648/1   9/30/08 9:19p bandrews
* PR46648: Added support for specifying STC seed behavior in Auto mode
*
* 18   8/14/08 5:25p katrep
* PR45674: Fix compiiler warning in kernel mode non debug builds
*
* 17   8/1/08 5:38p erickson
* PR45361: c89 fixes
*
* 16   7/23/08 3:01p jgarrett
* PR 39452: Merging to main branch
*
* 15   7/14/08 12:04p erickson
* PR42739: added options for IPTV
*
* PR42739/1   7/10/08 10:26p ssood
* PR42739: Adding support to absorb high jitter for Live IP Channels
*
* 14   6/26/08 1:57p erickson
* PR43541: add NEXUS_StcChannel_GetIndex_priv to get the index of the
*  PCR_OFFSET block
*
* 13   6/9/08 9:34a jgarrett
* PR 43445: Fixing defines for mosaic pcroffset hardware
*
* Nexus_Audio_Descriptors/1   5/15/08 4:40p jgarrett
* PR 42360: Adding third audio decode
*
* 12   4/15/08 2:29p jgarrett
* PR 41837: Fixing audio callback registration, adding debug
*
* 11   4/10/08 11:08a erickson
* PR41597: don't overflow BXPT_PcrOffset_ChangeStcRate
*
* 10   4/7/08 10:42a jgarrett
* PR 41362: Allowing NULL pidChanel
*
* 9   3/31/08 1:41p erickson
* PR41077: added NEXUS_ASSERT_MODULE to _priv function
*
* 8   3/27/08 11:33a erickson
* PR34925: set pcr_offset_settings.OffsetThreshold to 8, not 0.
*
* 7   3/26/08 7:50p katrep
* PR40952: Correct pcrofffset CountMode should applied in case of both
*  playback and live decode cases.
*
* 6   3/26/08 4:44p katrep
* PR40952: Transport type should come from StcChannelSetting not from the
*  handle while doing SetSettings.
*
* 5   3/25/08 5:25p bandrews
* PR40090: Added compile flag for ASTM
*
* 4   3/24/08 9:14p bandrews
* PR40090: stc channel astm private settings
*
* 3   2/20/08 9:40a erickson
* PR39766: check return code
*
* 2   1/24/08 4:23p jgarrett
* PR 38812: Clearing callbacks correctly
*
* 1   1/18/08 2:20p jgarrett
* PR 38808: Merging to main branch
*
***************************************************************************/
#include "nexus_transport_module.h"
#include "priv/nexus_stc_channel_priv.h"
#include "nexus_class_verification.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_stc_channel);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

/*#define NEXUS_STC_CHANNEL_DEBUG_DECODER_QUEUE 1*/

static NEXUS_Error NEXUS_StcChannel_P_ApplySettings(NEXUS_StcChannelHandle stcChannel, bool calledByAstm);

void NEXUS_StcChannel_GetDefaultSettings(unsigned index, NEXUS_StcChannelSettings *pSettings)
{
    BSTD_UNUSED(index);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->stcIndex = -1;
    pSettings->timebase = NEXUS_Timebase_eInvalid;
    pSettings->autoConfigTimebase = true;
    pSettings->mode = NEXUS_StcChannelMode_eAuto;
    pSettings->modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
    /* VideoMaster is default because most people measure PVR start performance by the appearance of video.
    Also, it provides more deterministic behavior than eFirstAvailable.
    The downside is that some audio could be lost, depending on the stream muxing. */
    pSettings->modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;
    pSettings->modeSettings.pcr.maxPcrError = 0xff;
    pSettings->modeSettings.pcr.offsetThreshold = 8;
    pSettings->modeSettings.pcr.disableJitterAdjustment = false;
    pSettings->modeSettings.pcr.disableTimestampCorrection = false;
    pSettings->modeSettings.Auto.offsetThreshold = 100;
    pSettings->modeSettings.host.offsetThreshold = 100;
    return;
}

static void NEXUS_StcChannel_P_Finalizer(NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    stcChannel->timebase = NULL;
    if (stcChannel->pcrlibChannel) {
        BPCRlib_Channel_Destroy(stcChannel->pcrlibChannel);
        stcChannel->pcrlibChannel = NULL;
    }
    if (stcChannel->pcrOffset) {
        (void)BXPT_PcrOffset_Close(stcChannel->pcrOffset);
        stcChannel->pcrOffset = NULL;
    }
    NEXUS_CLIENT_RESOURCES_RELEASE(stcChannel,Count,NEXUS_ANY_ID);
    BDBG_OBJECT_UNSET(stcChannel, NEXUS_StcChannel);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_StcChannel, NEXUS_StcChannel_Close);

NEXUS_StcChannelHandle NEXUS_StcChannel_Open(unsigned index, const NEXUS_StcChannelSettings *pSettings)
{
    BXPT_PcrOffset_Defaults pcrOffsetDefaults;
    BPCRlib_ChannelSettings settings;
    BERR_Code rc;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcChannelSettings;
    NEXUS_TimebaseHandle timebase;
    unsigned int i;

    /* verify nexus does not exceed XPT PI */
    BDBG_CASSERT(NEXUS_NUM_STC_CHANNELS <= BXPT_NUM_PCR_OFFSET_CHANNELS);
    BDBG_CASSERT(NEXUS_NUM_TIMEBASES <= BXPT_NUM_PCRS);
    /* nexus has no #define for BXPT_NUM_STCS */

    if (!pSettings) {
        NEXUS_StcChannel_GetDefaultSettings(index, &stcChannelSettings);
        pSettings = &stcChannelSettings;
    }
    
    if (index == NEXUS_ANY_ID) {
        for (index=0;index<NEXUS_NUM_STC_CHANNELS;index++) {
            if (!pTransport->stcChannel[index].pcrOffset) {
                break;
            }
        }
        if (index == NEXUS_NUM_STC_CHANNELS) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            BDBG_ERR(("no stc channel not available"));
            return NULL;
        }
    }

    if (index >= NEXUS_NUM_STC_CHANNELS) {
        BDBG_ERR(("stcChannel %d not available", index));
        return NULL;
    }

    stcChannel = &pTransport->stcChannel[index];
    if (stcChannel->pcrOffset) {
        BDBG_ERR(("already open"));
        return NULL;
    }

    stcChannel->stcIndex = (pSettings->stcIndex < 0) ? index : (unsigned)pSettings->stcIndex;
    if (stcChannel->stcIndex >= BXPT_NUM_STCS) {
        BDBG_ERR(("invalid stcIndex %d", stcChannel->stcIndex));
        return NULL;
    }
    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(stcChannel,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }

    BXPT_PcrOffset_GetChannelDefSettings(pTransport->xpt, index, &pcrOffsetDefaults);
    pcrOffsetDefaults.UsePcrTimeBase = true;
    /* There could be more StcChannels than Timebases. This gives XPT a meaningful default. */
    timebase = NEXUS_Timebase_Resolve_priv(pSettings->timebase);
    if (timebase)
    {
        pcrOffsetDefaults.WhichPcr = timebase->hwIndex;
    }
    else
    {
        pcrOffsetDefaults.WhichPcr = 0; /* this is just the default, if we are asked to manage the timebase, we will update this later in ApplySettings */
    }
#ifdef BXPT_HAS_MOSAIC_SUPPORT
    pcrOffsetDefaults.WhichStc = stcChannel->stcIndex;
#endif
    rc = BXPT_PcrOffset_Open(pTransport->xpt, index, &pcrOffsetDefaults, &stcChannel->pcrOffset);
    if (rc) {
        NEXUS_CLIENT_RESOURCES_RELEASE(stcChannel,Count,NEXUS_ANY_ID);
        rc=BERR_TRACE(rc); 
        return NULL;
    }

    NEXUS_OBJECT_SET(NEXUS_StcChannel, stcChannel);

#if NEXUS_HAS_ASTM
    BKNI_Memset(&stcChannel->astm.settings, 0, sizeof(NEXUS_StcChannelAstmSettings));
#endif

    BPCRlib_Channel_GetChannelDefSettings(pTransport->pcrlib, &settings);

    rc = BPCRlib_Channel_Create(pTransport->pcrlib, NULL, &stcChannel->pcrlibChannel, &settings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    stcChannel->settings = *pSettings;

    rc = NEXUS_StcChannel_P_ApplySettings(stcChannel, false);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    /* default new stc channel to normal rate */
    rc = NEXUS_StcChannel_SetRate(stcChannel, 1, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    BKNI_Memset(stcChannel->pcrlibData, 0, sizeof(stcChannel->pcrlibData));

    for (i = NEXUS_StcChannelDecoderType_eVideo0; i < NEXUS_StcChannelDecoderType_eMax; i++)
    {
        stcChannel->pcrlibData[i].type = i;
    }

    BLST_Q_INIT(&stcChannel->decoderPriorities);

    stcChannel->swPcrOffsetEnabled = true; /* soft, until connected */
    stcChannel->stcValid = false;
    stcChannel->nonRealTime = false;
    stcChannel->pairedChannel = NULL;
    stcChannel->lowLatencyDecoder = NEXUS_StcChannelDecoderType_eMax; /* off by default */

    return stcChannel;

error:
    NEXUS_StcChannel_Close(stcChannel);
    return NULL;
}

void NEXUS_StcChannel_GetSettings(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    *pSettings = stcChannel->settings;
}

static NEXUS_TransportType NEXUS_StcChannel_P_GetTransportType(const NEXUS_StcChannelSettings *pSettings)
{
    NEXUS_TransportType transportType = NEXUS_TransportType_eTs; /* default */

    switch (pSettings->mode) {
    case NEXUS_StcChannelMode_ePcr:
        if (!pSettings->modeSettings.pcr.pidChannel) {
            break;
        }
        transportType = pSettings->modeSettings.pcr.pidChannel->status.transportType;
        break;
    case NEXUS_StcChannelMode_eAuto:
        transportType = pSettings->modeSettings.Auto.transportType;
        break;
    case NEXUS_StcChannelMode_eHost:
        transportType = pSettings->modeSettings.host.transportType;
        break;
    default:
        break;
    }
    switch (transportType) {
    case NEXUS_TransportType_eAsf:
    case NEXUS_TransportType_eFlv:
    case NEXUS_TransportType_eAvi:
    case NEXUS_TransportType_eMp4:
    case NEXUS_TransportType_eWav:
    case NEXUS_TransportType_eRmff:
    case NEXUS_TransportType_eOgg:
    case NEXUS_TransportType_eFlac:
    case NEXUS_TransportType_eAiff:
    case NEXUS_TransportType_eApe:
        /* Playpump converts these streams to Mpeg2Pes, so Pcrlib should see it that way. */
        transportType = NEXUS_TransportType_eMpeg2Pes;
        break;
    default:
        break;
    }

    return transportType;
}

static NEXUS_Error NEXUS_StcChannel_P_ApplySettings(NEXUS_StcChannelHandle stcChannel, bool calledByAstm)
{
    BERR_Code rc;
    BPCRlib_Config pcrlibConfig;
    BXPT_PcrOffset_Settings pcr_offset_settings;
    NEXUS_TransportType transportType;
    NEXUS_StcChannelMode mode;
    bool autoConfigTimebase;
    bool autoConfigTimebaseSourceType;

    /* TODO: check if timebase = eInvalid and autoConfigTimebase = true, which is ok for unprotected, but not for the others */
    /* timebase code will now resolve eInvalid to the default timebase, but not reserve it against other client usage */
    stcChannel->timebase = NEXUS_Timebase_Resolve_priv(stcChannel->settings.timebase);
    if (!stcChannel->timebase)
    {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    if (stcChannel->settings.stcIndex != -1 && stcChannel->settings.stcIndex != (int)stcChannel->stcIndex) {
        BDBG_ERR(("cannot change stcIndex after Open"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    transportType = NEXUS_StcChannel_P_GetTransportType(&stcChannel->settings);

#if B_HAS_DSS
    rc = BXPT_DirecTv_PcrOffset_SetPcrMode(stcChannel->pcrOffset, NEXUS_IS_DSS_MODE(transportType)?BXPT_PcrMode_eDirecTv:BXPT_PcrMode_eMpeg);
    if (rc) {return BERR_TRACE(rc);}
#endif

    /* We chose to have PCROFFSET always locked to DPCR. DPCR can freerun or is locked to PCR. */
    rc = BXPT_PcrOffset_FreeRun(stcChannel->pcrOffset, false);
    if (rc) {return BERR_TRACE(rc);}

    BPCRlib_Channel_GetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);
    BXPT_PcrOffset_GetSettings(stcChannel->pcrOffset, &pcr_offset_settings);
    /* default pcr offset settings */
    pcr_offset_settings.OffsetThreshold = 8;
    pcr_offset_settings.MaxPcrError = 0xFF;
    pcr_offset_settings.TimestampDisable = false;
    pcr_offset_settings.CountMode = (NEXUS_IS_DSS_MODE(transportType) ||
        (NEXUS_StcChannel_PcrBits_eLegacy!=stcChannel->settings.pcrBits))?BXPT_PcrOffset_StcCountMode_eBinary:BXPT_PcrOffset_StcCountMode_eMod300;

    BDBG_ASSERT(pcr_offset_settings.UsePcrTimeBase == true); /* always use PCR */
    pcr_offset_settings.WhichPcr = stcChannel->timebase->hwIndex;

    mode = stcChannel->settings.mode;

    autoConfigTimebase = stcChannel->settings.autoConfigTimebase;
    autoConfigTimebaseSourceType = autoConfigTimebase;

    if (mode == NEXUS_StcChannelMode_eAuto)
    {
        switch (stcChannel->settings.modeSettings.Auto.behavior)
        {
            case NEXUS_StcChannelAutoModeBehavior_eFirstAvailable:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eSTCMaster;
                break;
            case NEXUS_StcChannelAutoModeBehavior_eVideoMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eVideoMaster;
                break;
            case NEXUS_StcChannelAutoModeBehavior_eAudioMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eAudioMaster;
                break;
            default:
                BDBG_ERR(("invalid mode"));
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

#if NEXUS_HAS_ASTM
    /* if ASTM is enabled, it's mode selection overrides user, but doesn't replace user settings */
    if (stcChannel->astm.settings.enabled)
    {
        BDBG_MSG(("ASTM is setting the mode for stc%u channel %p to %d", stcChannel->stcIndex, stcChannel, stcChannel->astm.settings.mode));
        BDBG_MSG(("ASTM is disabling autoconfig of the timebase source type for stc channel %p", stcChannel));
        BDBG_MSG(("ASTM is setting the PCRlib tsm mode for stc channel %p to %d", stcChannel, stcChannel->astm.settings.tsmMode));
        mode = stcChannel->astm.settings.mode;
        autoConfigTimebaseSourceType = false;

        /* apply tsm mode and sync limit to PCRlib */
        switch (stcChannel->astm.settings.tsmMode)
        {
            case NEXUS_StcChannelTsmMode_eAudioMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eAudioMaster;
                pcrlibConfig.sync_limit = stcChannel->astm.settings.syncLimit;
                break;
            case NEXUS_StcChannelTsmMode_eVideoMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eVideoMaster;
                pcrlibConfig.sync_limit = stcChannel->astm.settings.syncLimit;
                break;
            case NEXUS_StcChannelTsmMode_eStcMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eSTCMaster;
                pcrlibConfig.sync_limit = 0;
                break;
            case NEXUS_StcChannelTsmMode_eOutputMaster:
                pcrlibConfig.tsm_mode = BPCRlib_TsmMode_eOutputMaster;
                pcrlibConfig.sync_limit = 0;
                break;
            default:
                break;
        }
    }
#endif /* NEXUS_HAS_ASTM */

    switch (mode)
    {
    case NEXUS_StcChannelMode_ePcr:
        {
            NEXUS_PidChannelHandle pidChannel = stcChannel->settings.modeSettings.pcr.pidChannel;

            if ( NULL == pidChannel ) {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
            pcr_offset_settings.PidChannelNum = pidChannel->status.pidChannelIndex;
            pcr_offset_settings.UseHostPcrs = false;
            pcr_offset_settings.OffsetThreshold = stcChannel->settings.modeSettings.pcr.offsetThreshold;
            pcr_offset_settings.MaxPcrError = stcChannel->settings.modeSettings.pcr.maxPcrError;
            pcr_offset_settings.TimestampDisable = stcChannel->settings.modeSettings.pcr.disableTimestampCorrection;

            pcrlibConfig.playback = false;

            BDBG_MSG(("%s: STC%u Channel (pid channel %p) in PCR Mode, offset threashold %d, max pcr error %d, timestamp disable %d, jitter disable %d",
                    __FUNCTION__, stcChannel->stcIndex, pidChannel,
                    pcr_offset_settings.OffsetThreshold, pcr_offset_settings.MaxPcrError,
                    pcr_offset_settings.TimestampDisable, stcChannel->settings.modeSettings.pcr.disableJitterAdjustment));

            if (autoConfigTimebase)
            {
                NEXUS_TimebaseSettings timebaseSettings;
                BDBG_MSG(("%s: Auto Configure the Timebase %u", __FUNCTION__, stcChannel->timebase->hwIndex));
                NEXUS_Timebase_P_GetSettings(stcChannel->timebase, &timebaseSettings);
                /* PR:51691 20090129 bandrews - if called by user through setsettings, apply sourceType */
                if (autoConfigTimebaseSourceType || !calledByAstm)
                {
                    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
                }
                timebaseSettings.sourceSettings.pcr.pidChannel = pidChannel;
                timebaseSettings.sourceSettings.pcr.maxPcrError = stcChannel->settings.modeSettings.pcr.maxPcrError;
                timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
                rc = NEXUS_Timebase_P_SetSettings(stcChannel->timebase, &timebaseSettings);
                if (rc) {return BERR_TRACE(rc);}
            }

            rc = NEXUS_P_TransportType_ToMagnum(transportType, &pcrlibConfig.stream);
            if (rc) {return BERR_TRACE(rc);}
        }
        break;

    case NEXUS_StcChannelMode_eAuto:
    case NEXUS_StcChannelMode_eHost:
        BDBG_MSG(("%s: STC%u Channel in Auto/Host mode", __FUNCTION__, stcChannel->stcIndex));
        pcr_offset_settings.PidChannelNum = 0; /* no PCR pid used */
        pcr_offset_settings.UseHostPcrs = true;

        pcrlibConfig.mode = BPCRlib_Mode_eAutoPts;
        pcrlibConfig.playback = true;

        pcrlibConfig.video_pts_offset = (3003*4)/2; /* 120ms */
        pcrlibConfig.audio_pts_offset = (3003*2)/2; /* 60ms */

        /* pcrlib will also reassert OFFSET = 0 when it sets STC. */
        rc = BXPT_PcrOffset_SetOffset(stcChannel->pcrOffset, 0);
        if (rc) {return BERR_TRACE(rc);}

        if (autoConfigTimebaseSourceType || (!calledByAstm && autoConfigTimebase))
        {
            NEXUS_TimebaseSettings timebaseSettings;
            NEXUS_Timebase_P_GetSettings(stcChannel->timebase, &timebaseSettings);
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            rc = NEXUS_Timebase_P_SetSettings(stcChannel->timebase, &timebaseSettings);
            if (rc) {return BERR_TRACE(rc);}
        }
        break;

    default:
        BDBG_ERR(("invalid mode"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
#if BXPT_HAS_TSMUX
    BDBG_CASSERT(NEXUS_StcChannel_PcrBits_eLegacy == (NEXUS_StcChannel_PcrBits)BXPT_PcrOffset_BroadcastMode_eLegacy);
    BDBG_CASSERT(NEXUS_StcChannel_PcrBits_eLsb32 == (NEXUS_StcChannel_PcrBits)BXPT_PcrOffset_BroadcastMode_eLsb32);
    BDBG_CASSERT(NEXUS_StcChannel_PcrBits_eMsb32 == (NEXUS_StcChannel_PcrBits)BXPT_PcrOffset_BroadcastMode_eMsb32);
    BDBG_CASSERT(NEXUS_StcChannel_PcrBits_eFull42 == (NEXUS_StcChannel_PcrBits)BXPT_PcrOffset_BroadcastMode_eFull42);
    pcr_offset_settings.BroadcastMode = stcChannel->settings.pcrBits;
    BDBG_MSG(("STC%u broadcastmode = %u", stcChannel->stcIndex, pcr_offset_settings.BroadcastMode));
#else
    if(stcChannel->settings.pcrBits != NEXUS_StcChannel_PcrBits_eLegacy) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }
#endif
    rc = BXPT_PcrOffset_SetSettings(stcChannel->pcrOffset, &pcr_offset_settings);
    if (rc) {return BERR_TRACE(rc);}

    if ( mode == NEXUS_StcChannelMode_ePcr )
    {
        rc = NEXUS_StcChannel_EnablePidChannel_priv(stcChannel, stcChannel->settings.modeSettings.pcr.pidChannel);
        if (rc) {return BERR_TRACE(rc);}
    }

    rc = BPCRlib_Channel_SetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);
    if (rc) {return BERR_TRACE(rc);}

    return 0;
}

NEXUS_Error NEXUS_StcChannel_SetSettings(NEXUS_StcChannelHandle stcChannel, const NEXUS_StcChannelSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    stcChannel->settings = *pSettings;
    return NEXUS_StcChannel_P_ApplySettings(stcChannel, false);
}

NEXUS_Error NEXUS_StcChannel_GetStatus(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelStatus * status)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    BKNI_Memset(status, 0, sizeof(*status));
#if NEXUS_HAS_ASTM
    if (stcChannel->astm.settings.enabled)
    {
        status->mode = stcChannel->astm.settings.mode;
    }
    else
#endif
    {
        status->mode = stcChannel->settings.mode;
    }
    return 0;
}

NEXUS_Error NEXUS_StcChannel_Invalidate(NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    BDBG_MSG(("%p: stc%u invalid", stcChannel, stcChannel->stcIndex));
    stcChannel->stcValid = false;
    return BPCRlib_Channel_Invalidate(stcChannel->pcrlibChannel);
}

void NEXUS_StcChannel_GetStc(NEXUS_StcChannelHandle stcChannel, uint32_t *pStc)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    if (!stcChannel->swPcrOffsetEnabled) {
        *pStc = BXPT_PcrOffset_GetStc(stcChannel->pcrOffset) + BXPT_PcrOffset_GetOffset(stcChannel->pcrOffset);
    }
    else {
        *pStc = BXPT_PcrOffset_GetStc(stcChannel->pcrOffset) + stcChannel->swPcrOffset;
    }
}

void NEXUS_StcChannel_GetSerialStc_priv( NEXUS_StcChannelHandle stcChannel, uint32_t *pStc )
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    *pStc = BXPT_PcrOffset_GetStc(stcChannel->pcrOffset);
}

void NEXUS_StcChannel_SetPcrOffsetContextAcquireMode_priv(NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    BXPT_PcrOffset_Acquire(stcChannel->pcrOffset);
}

static void NEXUS_StcChannel_P_Notify_isr(NEXUS_StcChannelHandle stcChannel, struct NEXUS_PcrlibData *pcrlibData)
{
    /* this StcChannel is using a SW pcr offset */
    if (pcrlibData->callbacks.setPcrOffset_isr) {
        (*pcrlibData->callbacks.setPcrOffset_isr)(pcrlibData->callbacks.pDevContext, stcChannel->swPcrOffset);
    }

    /* tell the all connected decoders that the STC is now valid */
    if (pcrlibData->callbacks.stcValid_isr) {
        (*pcrlibData->callbacks.stcValid_isr)(pcrlibData->callbacks.pDevContext);
    }
}

static NEXUS_Error NEXUS_StcChannel_P_SetStc_isr(NEXUS_StcChannelHandle stcChannel, struct NEXUS_PcrlibData *pcrlibData, uint32_t stc, bool propagate)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    if (!stcChannel->swPcrOffsetEnabled) {
        /* program the Serial STC directly with this STC value. PCR_OFFSET is zero. */
        uint32_t previousStc = BXPT_PcrOffset_GetStc_isr(stcChannel->pcrOffset);
        int32_t diff;
        unsigned n;
        unsigned offsetThreshold=0;


        switch(stcChannel->settings.mode) {
        case NEXUS_StcChannelMode_ePcr:
            offsetThreshold = stcChannel->settings.modeSettings.pcr.offsetThreshold;
            break;

        case NEXUS_StcChannelMode_eMax:
        case NEXUS_StcChannelMode_eAuto:
            offsetThreshold = stcChannel->settings.modeSettings.Auto.offsetThreshold;
            break;

        case NEXUS_StcChannelMode_eHost:
            offsetThreshold = stcChannel->settings.modeSettings.host.offsetThreshold;
            break;
        /* no default so if there is enum added it would cause compiler time warning */
        }

        /* If the serial STC is already very close, there's no point in resetting it. This
        prevents PTS Errors from raptor which tries to follow a tight TSM threshold. */
        diff = previousStc - stc;
        diff = diff>=0 ? diff:-diff;
        if (diff > (int32_t)offsetThreshold) {
            NEXUS_Error rc;
            rc = BXPT_PcrOffset_SetStc_isr(stcChannel->pcrOffset, stc);
            if (rc) return BERR_TRACE(rc);
            rc = BXPT_PcrOffset_SetOffset_isr(stcChannel->pcrOffset, 0);
            if (rc) return BERR_TRACE(rc);
        }

        /* we have to test every StcChannel in the system because we may have a shared stcIndex. */
        for (n=0;n<NEXUS_NUM_STC_CHANNELS;n++) {
            NEXUS_StcChannelHandle otherStc = &pTransport->stcChannel[n];
            if (otherStc->pcrOffset && otherStc->stcIndex == stcChannel->stcIndex) {
                unsigned i;
                /* allow each one to recalc the swPcrOffset. */
                for (i=0;i<NEXUS_StcChannelDecoderType_eMax;i++) {
                    struct NEXUS_PcrlibData *otherPcrlibData;
                    otherPcrlibData = &otherStc->pcrlibData[i];
                    if (otherStc != stcChannel) {
                        stcChannel->swPcrOffset = previousStc + stcChannel->swPcrOffset - BXPT_PcrOffset_GetStc_isr(stcChannel->pcrOffset);
                    }
                    else {
                        stcChannel->swPcrOffset = 0;
                    }
                    /* if otherStc == stcChannel, should we notify if otherPcrlibData != pcrlibData? I believe this is redundant with PCRlib and harmless. */
                    NEXUS_StcChannel_P_Notify_isr(otherStc, otherPcrlibData);
                }
            }
        }
    }
    else {
        /* NRT mode theoretically should have zero TSM error for master; however, the 1/1001 rounding error
         * might cause TSM jittering; add 2 to the SW stc_offset to avoid TSM jitter */
        stcChannel->swPcrOffset = stc - BXPT_PcrOffset_GetStc_isr(stcChannel->pcrOffset) + 2;
        BDBG_MSG(("%p HW STC%u=0x%x, SW OFFSET = 0x%08x", stcChannel, stcChannel->stcIndex, stc+2-stcChannel->swPcrOffset, stcChannel->swPcrOffset));
        /* do not program the STC. just recalc the swPcrOffset, either for one or all. */
        if (!pcrlibData) {
            unsigned i;
            for (i=0;i<NEXUS_StcChannelDecoderType_eMax;i++) {
                NEXUS_StcChannel_P_Notify_isr(stcChannel, &stcChannel->pcrlibData[i]);
            }
        }
        else {
            NEXUS_StcChannel_P_Notify_isr(stcChannel, pcrlibData);
        }
        if (propagate)
        {
            /* Notify paired channel, if one exists */
            if (stcChannel->pairedChannel)
            {
                NEXUS_StcChannel_P_SetStc_isr(stcChannel->pairedChannel, NULL, stc, false);
            }
        }
    }
    
    BDBG_MSG(("%p: stc%u valid", stcChannel, stcChannel->stcIndex));
    stcChannel->stcValid = true;
    
    return 0;
}

NEXUS_Error NEXUS_StcChannel_SetStc_isr(NEXUS_StcChannelHandle stcChannel, uint32_t stc)
{
    return NEXUS_StcChannel_P_SetStc_isr(stcChannel, NULL, stc, true);
}

NEXUS_Error NEXUS_StcChannel_SetStc(NEXUS_StcChannelHandle stcChannel, uint32_t stc)
{
    NEXUS_Error rc;
    BKNI_EnterCriticalSection();
    rc = NEXUS_StcChannel_P_SetStc_isr(stcChannel, NULL, stc, true);
    BKNI_LeaveCriticalSection();
    return rc;
}

/*********************************************
* private api
******************/

void NEXUS_StcChannel_GetStc_isr(NEXUS_StcChannelHandle stcChannel, uint32_t *pStc)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    if (!stcChannel->swPcrOffsetEnabled)
    {
        *pStc = BXPT_PcrOffset_GetStc_isr(stcChannel->pcrOffset) + BXPT_PcrOffset_GetOffset_isr(stcChannel->pcrOffset);
    }
    else
    {
        *pStc = BXPT_PcrOffset_GetStc_isr(stcChannel->pcrOffset) + stcChannel->swPcrOffset;
    }
}

/* We have to manage the callbacks here in order to not export pcrlib callback peculiarities outside NEXUS_StcChannel. */

static BERR_Code NEXUS_StcChannel_GetPtsCallbackWrapper_isr(void *pDecoderContext, BAVC_PTSInfo *pPTSInfo)
{
    struct NEXUS_PcrlibData *pcrlibData = (struct NEXUS_PcrlibData *)pDecoderContext;
    return (*pcrlibData->callbacks.getPts_isr)(pcrlibData->callbacks.pDevContext, pPTSInfo);
}

static BERR_Code NEXUS_StcChannel_GetCdbLevelCallbackWrapper_isr(void *pDecoderContext, unsigned *level)
{
    struct NEXUS_PcrlibData *pcrlibData = (struct NEXUS_PcrlibData *)pDecoderContext;
    return (*pcrlibData->callbacks.getCdbLevel_isr)(pcrlibData->callbacks.pDevContext, level);
}

static BERR_Code NEXUS_StcChannel_GetStcCallbackWrapper_isr(void *pTransportContext, void *pDecoderContext,  uint32_t *pStc)
{
    NEXUS_StcChannelHandle stcChannel = (NEXUS_StcChannelHandle)pTransportContext;
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    BSTD_UNUSED(pDecoderContext);
    NEXUS_StcChannel_GetStc_isr(stcChannel, pStc);
    BDBG_MSG(("NEXUS_StcChannel_GetStcCallbackWrapper_isr returning stc%u 0x%08x", stcChannel->stcIndex, *pStc));
    return 0;
}

static BERR_Code NEXUS_StcChannel_SetStcCallbackWrapper_isr(void *pTransportContext, void *pDecoderContext,  bool dss, uint32_t stc)
{
    NEXUS_StcChannelHandle stcChannel = (NEXUS_StcChannelHandle)pTransportContext;
    struct NEXUS_PcrlibData *pcrlibData = (struct NEXUS_PcrlibData *)pDecoderContext;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    BSTD_UNUSED(dss);
    BSTD_UNUSED(pcrlibData);

    BDBG_MSG(("NEXUS_StcChannel_SetStcCallbackWrapper_isr stc%u 0x%08x", stcChannel->stcIndex, stc));
    /* 20111117 SW7425-1772 bandrews - notify all decoders during every pcrlib callback by using NULL pcrlibData */
    return NEXUS_StcChannel_P_SetStc_isr(stcChannel, NULL, stc, true); 
}

static BERR_Code NEXUS_StcChannel_UpdateStcCallbackWrapper_isr(void *pTransportContext, bool is_request_stc)
{
    NEXUS_StcChannelHandle stcChannel = (NEXUS_StcChannelHandle)pTransportContext;
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    BDBG_MSG(("NEXUS_StcChannel_UpdateStcCallbackWrapper_isr is_request_stc%u=%d",stcChannel->stcIndex, is_request_stc));
    if (is_request_stc) {
        /* If PCR_OFFSET block has non-zero OFFSET_THRESHOLD, then it needs
        to be forced to regenerate an offset message to RAVE. Otherwise the decoder
        may lose a pcr_offset_valid message. */
        BXPT_PcrOffset_RegenOffset_isr(stcChannel->pcrOffset);
    }
    return 0;
}

void NEXUS_StcChannel_GetDefaultCallbacks_priv(NEXUS_StcChannelCallbacks *callbacks)
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(callbacks, 0, sizeof(*callbacks));
}

#if !BDBG_NO_MSG
static const char *g_stcChannelTypeStr[NEXUS_StcChannelDecoderType_eMax] = {
    "video0", "audio0", "audio1", "audio2", "audio3", "audio4", "audio5"
};
#endif

#define NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(DEC) \
( \
    stcChannel->pcrlibData[(DEC)].callbacks.setPcrOffset_isr \
    || \
    !stcChannel->pcrlibData[(DEC)].connected \
)

static void NEXUS_StcChannel_P_SetSwPcrOffsetEnabled(NEXUS_StcChannelHandle stcChannel)
{
    /* swPcrOffset is false if there's an audio decoder (because RAP does not support a swPcrOffset) or
    if there's a video decoder which doesn't provide the setPcrOffset callback.
    There should be only one audio program (possibly multichannel, but the same PTS domain) active for a set of mosaics. */
    bool enabled =
        stcChannel->settings.mode != NEXUS_StcChannelMode_ePcr;

#if BXPT_HAS_TSMUX
    if (stcChannel->nonRealTime)
    {
        /* must have STC offset setter or not be connected in NRT mode, can't proceed further if not, therefore assert */
        BDBG_ASSERT(NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(NEXUS_StcChannelDecoderType_eVideo0));
        BDBG_ASSERT(NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(NEXUS_StcChannelDecoderType_eAudio0));
        BDBG_ASSERT(NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(NEXUS_StcChannelDecoderType_eAudio1));
        BDBG_ASSERT(NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(NEXUS_StcChannelDecoderType_eAudio2));
    }
    else
#endif
    {
        /* without NRT, condition is whether video has its setter set and audios are not connected */
        enabled = enabled &&
        (
            (
                NEXUS_STC_CHANNEL_P_DEC_HAS_SW_OFFSET_SETTER_OR_NOT_CONNECTED(NEXUS_StcChannelDecoderType_eVideo0)
                &&
                !stcChannel->pcrlibData[NEXUS_StcChannelDecoderType_eAudio0].connected 
                &&
                !stcChannel->pcrlibData[NEXUS_StcChannelDecoderType_eAudio1].connected 
                &&
                !stcChannel->pcrlibData[NEXUS_StcChannelDecoderType_eAudio2].connected
            ) 
        );
    }
    if (stcChannel->swPcrOffsetEnabled != enabled) {
        if (!enabled) {
            uint32_t stc;
            /* switching from SW -> HW */
            /* only print the message if we are in live mode, otherwise it might
            be confusing */
            if (stcChannel->settings.mode == NEXUS_StcChannelMode_ePcr)
            {
                BDBG_MSG(("%p Using HW PCR OFFSET%u", stcChannel, stcChannel->stcIndex));
            }
            NEXUS_StcChannel_GetStc(stcChannel, &stc); /* get HW STC + swPcrOffset */
            stcChannel->swPcrOffset = 0;
            stcChannel->swPcrOffsetEnabled = enabled;
            if (stcChannel->stcValid) /* don't jam STC unless we had it valid before */
            {
                NEXUS_StcChannel_SetStc(stcChannel, stc);  /* set HW STC */
            }
        }
        else {
            BDBG_MSG(("%p Using SW STC%u OFFSET", stcChannel, stcChannel->stcIndex));
            /* switching from HW -> SW */
            stcChannel->swPcrOffset = 0;
            stcChannel->swPcrOffsetEnabled = enabled;
        }
    }
}

static bool NEXUS_StcChannel_P_IsDecoderManagedByPcrlib_isr(NEXUS_StcChannelHandle stcChannel, struct NEXUS_PcrlibData * pcrlibData)
{
    bool result = false;

    if (pcrlibData->type == NEXUS_StcChannelDecoderType_eVideo0)
    {
        result = true;
    }
    else
    {
        struct NEXUS_PcrlibData * tmp;
        unsigned count = 0;
        
        for (tmp = BLST_Q_FIRST(&stcChannel->decoderPriorities); tmp != NULL; tmp = BLST_Q_NEXT(tmp, link))
        {
            if (tmp == pcrlibData)
            {
                result = true;
                break;
            }
            count++;
            if (count == 3) /* pcrlib can only manage 3 audios */
            {
                break;
            }
        }
    }

    return result;
}

static void NEXUS_StcChannel_P_SetPcrlibAudioCallbacks(NEXUS_StcChannelHandle stcChannel, BPCRlib_Config *config)
{
    struct NEXUS_PcrlibData * pcrlibData;

    config->audio = NULL;
    config->audio_iface = NULL;
    config->secondary_audio = NULL;
    config->secondary_audio_iface = NULL;
    config->tertiary_audio = NULL;
    config->tertiary_audio_iface = NULL;

    pcrlibData = BLST_Q_FIRST(&stcChannel->decoderPriorities);

    if (pcrlibData)
    {
        config->audio = pcrlibData;
        config->audio_iface = &pcrlibData->iface;

        pcrlibData = BLST_Q_NEXT(pcrlibData, link);

        if (pcrlibData)
        {
            config->secondary_audio = pcrlibData;
            config->secondary_audio_iface = &pcrlibData->iface;

            pcrlibData = BLST_Q_NEXT(pcrlibData, link);

            if (pcrlibData)
            {
                config->tertiary_audio = pcrlibData;
                config->tertiary_audio_iface = &pcrlibData->iface;

                /* last channels on queue don't get pcrlib */
            }
        }
    }
}

#ifdef NEXUS_STC_CHANNEL_DEBUG_DECODER_QUEUE
static void NEXUS_StcChannel_P_PrintDecoderPriorityQueue(NEXUS_StcChannelHandle stcChannel)
{
    struct NEXUS_PcrlibData * ql = NULL;
    
    BDBG_ERR(("stc channel %p priority queue {", stcChannel));
    for (ql = BLST_Q_FIRST(&stcChannel->decoderPriorities); ql != NULL; ql = BLST_Q_NEXT(ql, link))
    {
        BDBG_ERR(("type %u", ql->type));
    }
    BDBG_ERR(("}"));
}
#endif

static void NEXUS_StcChannel_P_DecoderPriorityQueueInsert(NEXUS_StcChannelHandle stcChannel, struct NEXUS_PcrlibData * pcrlibData)
{
    struct NEXUS_PcrlibData * ql = NULL;
    
    for (ql = BLST_Q_FIRST(&stcChannel->decoderPriorities); ql != NULL; ql = BLST_Q_NEXT(ql, link))
    {
        if (ql->priority > pcrlibData->priority)
        {
            BLST_Q_INSERT_BEFORE(&stcChannel->decoderPriorities, ql, pcrlibData, link);
            break;
        }
    }

    if (!ql)
    {
        BLST_Q_INSERT_TAIL(&stcChannel->decoderPriorities, pcrlibData, link);
    }

#ifdef NEXUS_STC_CHANNEL_DEBUG_DECODER_QUEUE
    NEXUS_StcChannel_P_PrintDecoderPriorityQueue(stcChannel);
#endif
}

static void NEXUS_StcChannel_P_DecoderPriorityQueueRemove(NEXUS_StcChannelHandle stcChannel, struct NEXUS_PcrlibData * pcrlibData)
{
    struct NEXUS_PcrlibData * ql = NULL;
    
    for (ql = BLST_Q_FIRST(&stcChannel->decoderPriorities); ql != NULL; ql = BLST_Q_NEXT(ql, link))
    {
        if (ql == pcrlibData)
        {
            BLST_Q_REMOVE(&stcChannel->decoderPriorities, ql, link);
            break;
        }
    }

    if (!ql)
    {
        BDBG_WRN(("Attempt to remove non-existent decoder from stc priority queue"));
    }

#ifdef NEXUS_STC_CHANNEL_DEBUG_DECODER_QUEUE
    NEXUS_StcChannel_P_PrintDecoderPriorityQueue(stcChannel);
#endif
}

BERR_Code NEXUS_StcChannel_SetCallbacks_priv(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type, unsigned priority, const NEXUS_StcChannelCallbacks *pCallbacks)
{
    BPCRlib_Config pcrlibConfig;
    BERR_Code rc;
    struct NEXUS_PcrlibData *pcrlibData;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();

    pcrlibData = &stcChannel->pcrlibData[type];

    pcrlibData->connected = true;
    pcrlibData->iface.getPts = NEXUS_StcChannel_GetPtsCallbackWrapper_isr;
    pcrlibData->iface.getStc = NEXUS_StcChannel_GetStcCallbackWrapper_isr;
    pcrlibData->iface.getCdbLevel = NEXUS_StcChannel_GetCdbLevelCallbackWrapper_isr;
    pcrlibData->iface.setStc = NEXUS_StcChannel_SetStcCallbackWrapper_isr;
    pcrlibData->iface.updateStc = NEXUS_StcChannel_UpdateStcCallbackWrapper_isr;
    pcrlibData->iface.useAuxTrp = true;
    pcrlibData->callbacks = *pCallbacks;
    pcrlibData->priority = priority;
    pcrlibData->type = type;

    BPCRlib_Channel_GetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);
    switch (type) {
    case NEXUS_StcChannelDecoderType_eVideo0:
        pcrlibConfig.video = pcrlibData;
        pcrlibConfig.video_iface = &pcrlibData->iface;
        break;
    case NEXUS_StcChannelDecoderType_eAudio0:
    case NEXUS_StcChannelDecoderType_eAudio1:
    case NEXUS_StcChannelDecoderType_eAudio2:
    case NEXUS_StcChannelDecoderType_eAudio3:
    case NEXUS_StcChannelDecoderType_eAudio4:
    case NEXUS_StcChannelDecoderType_eAudio5:
        NEXUS_StcChannel_P_DecoderPriorityQueueInsert(stcChannel, pcrlibData);
        NEXUS_StcChannel_P_SetPcrlibAudioCallbacks(stcChannel, &pcrlibConfig);
        break;
    default:
        BDBG_ERR(("invalid decoder type"));
        return NEXUS_INVALID_PARAMETER;
    }

    NEXUS_StcChannel_P_SetSwPcrOffsetEnabled(stcChannel);

    pcrlibConfig.aux_transport = stcChannel;
    pcrlibConfig.playback = (stcChannel->settings.mode != NEXUS_StcChannelMode_ePcr);
#if NEXUS_HAS_ASTM
    if (stcChannel->astm.settings.enabled)
    {
        BDBG_MSG(("ASTM is %s PCRlib playback mode for stc channel %p", (stcChannel->astm.settings.mode != NEXUS_StcChannelMode_ePcr)?"enabling":"disabling", stcChannel));
        pcrlibConfig.playback = (stcChannel->astm.settings.mode != NEXUS_StcChannelMode_ePcr);
    }
#endif
    pcrlibConfig.pcr_offset = 0;

    rc = NEXUS_P_TransportType_ToMagnum(NEXUS_StcChannel_P_GetTransportType(&stcChannel->settings), &pcrlibConfig.stream);
    if (rc) return BERR_TRACE(rc);

    return BPCRlib_Channel_SetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);
}

void NEXUS_StcChannel_ClearCallbacks_priv(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type)
{
    BPCRlib_Config pcrlibConfig;
    struct NEXUS_PcrlibData *pcrlibData;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();

    pcrlibData = &stcChannel->pcrlibData[type];

    BPCRlib_Channel_GetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);
    switch (type) {
    case NEXUS_StcChannelDecoderType_eVideo0:
        pcrlibConfig.video = NULL;
        pcrlibConfig.video_iface = NULL;
        break;
    case NEXUS_StcChannelDecoderType_eAudio0:
    case NEXUS_StcChannelDecoderType_eAudio1:
    case NEXUS_StcChannelDecoderType_eAudio2:
    case NEXUS_StcChannelDecoderType_eAudio3:
    case NEXUS_StcChannelDecoderType_eAudio4:
    case NEXUS_StcChannelDecoderType_eAudio5:
        NEXUS_StcChannel_P_DecoderPriorityQueueRemove(stcChannel, pcrlibData);
        NEXUS_StcChannel_P_SetPcrlibAudioCallbacks(stcChannel, &pcrlibConfig);
        break;
    default:
        BDBG_ERR(("invalid decoder type"));
        return;
    }
    BPCRlib_Channel_SetConfig(stcChannel->pcrlibChannel, &pcrlibConfig);

    /* we can only memset this after it's disconnected from PCRlib */
    /* this memset will clear connected, isHung and requiresFlush */
    BKNI_Memset(pcrlibData, 0, sizeof(struct NEXUS_PcrlibData));

    NEXUS_StcChannel_P_SetSwPcrOffsetEnabled(stcChannel);
}

BERR_Code NEXUS_StcChannel_RequestStc_isr(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type, const struct BAVC_PTSInfo *pPts)
{
    struct NEXUS_PcrlibData *pcrlibData;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    pcrlibData = &stcChannel->pcrlibData[type];

    if (stcChannel->settings.mode == NEXUS_StcChannelMode_eHost) 
    {
        /* in eHost mode, STC request interrupts do not set the STC,
        but we must ensure decoders think the STC is valid when it is
        set before they start */
        if (stcChannel->stcValid)
        {
            NEXUS_StcChannel_P_Notify_isr(stcChannel, pcrlibData);
        }

        return 0;
    }

    /* handle unmanaged decoders first */
    if (!NEXUS_StcChannel_P_IsDecoderManagedByPcrlib_isr(stcChannel, pcrlibData))
    {
        BDBG_MSG(("Unmanaged decoder %u STC request", pcrlibData->type));
        if (stcChannel->stcValid)
        {
            NEXUS_StcChannel_P_Notify_isr(stcChannel, pcrlibData);
        }
        return 0;
    }

    switch (type) {
    case NEXUS_StcChannelDecoderType_eVideo0:
        if (stcChannel->lowLatencyDecoder == NEXUS_StcChannelDecoderType_eVideo0
            || stcChannel->lowLatencyDecoder == NEXUS_StcChannelDecoderType_eMax)
        {
            rc = BPCRlib_Channel_VideoRequestStc_isr(pTransport->pcrlib, pcrlibData, pPts);
        }
        else
        {
            if (stcChannel->stcValid)
            {
                NEXUS_StcChannel_P_Notify_isr(stcChannel, pcrlibData);
            }
        }
        break;
    case NEXUS_StcChannelDecoderType_eAudio0:
    case NEXUS_StcChannelDecoderType_eAudio1:
    case NEXUS_StcChannelDecoderType_eAudio2:
    case NEXUS_StcChannelDecoderType_eAudio3:
    case NEXUS_StcChannelDecoderType_eAudio4:
    case NEXUS_StcChannelDecoderType_eAudio5:
        if (stcChannel->lowLatencyDecoder >= NEXUS_StcChannelDecoderType_eAudio0) /* includes eMax, which means off */
        {
            rc = BPCRlib_Channel_AudioRequestStc_isr(pTransport->pcrlib, pcrlibData, pPts->ui32CurrentPTS);
        }
        else
        {
            if (stcChannel->stcValid)
            {
                NEXUS_StcChannel_P_Notify_isr(stcChannel, pcrlibData);
            }
        }
        break;
    default:
        BDBG_ERR(("invalid decoder type"));
        rc = NEXUS_INVALID_PARAMETER;
        break;
    }
    return rc;
}

BERR_Code NEXUS_StcChannel_PtsError_isr(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type, const struct BAVC_PTSInfo *pPts)
{
    struct NEXUS_PcrlibData *pcrlibData;
    BERR_Code rc = BERR_SUCCESS;
    uint32_t stc;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    pcrlibData = &stcChannel->pcrlibData[type];
    NEXUS_StcChannel_GetStc_isr(stcChannel, &stc);

    if (stcChannel->settings.mode == NEXUS_StcChannelMode_eHost
        || stcChannel->lowLatencyDecoder != NEXUS_StcChannelDecoderType_eMax) 
    {
        /* in eHost mode or low latency mode, PTS interrupts do not set the STC */
        return 0;
    }

    /* handle unmanaged decoders first */
    if (!NEXUS_StcChannel_P_IsDecoderManagedByPcrlib_isr(stcChannel, pcrlibData))
    {
/*        BDBG_MSG(("Unmanaged decoder %u PTS error", pcrlibData->type));*/
        return 0;
    }

    switch (type) {
    case NEXUS_StcChannelDecoderType_eVideo0:
        rc = BPCRlib_Channel_VideoPtsError_isr(pTransport->pcrlib, pcrlibData, pPts, stc);
        break;
    case NEXUS_StcChannelDecoderType_eAudio0:
    case NEXUS_StcChannelDecoderType_eAudio1:
    case NEXUS_StcChannelDecoderType_eAudio2:
    case NEXUS_StcChannelDecoderType_eAudio3:
    case NEXUS_StcChannelDecoderType_eAudio4:
    case NEXUS_StcChannelDecoderType_eAudio5:
        rc = BPCRlib_Channel_AudioPtsError_isr(pTransport->pcrlib, pcrlibData, pPts, stc);
        break;
    default:
        rc = NEXUS_INVALID_PARAMETER;
        break;
    }
    return rc;
}

/*
Enable Output to a specified PID Channel
*/
BERR_Code NEXUS_StcChannel_EnablePidChannel_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_PidChannelHandle pidChannel
    )
{
#ifdef BXPT_HAS_MOSAIC_SUPPORT
    BERR_Code rc;
    bool enableJitterAdjustment;
    
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    /* PI calls it JitterEnable even though RDB calls it JITTER_DISABLE */
    enableJitterAdjustment = !stcChannel->settings.modeSettings.pcr.disableJitterAdjustment;
    rc = BXPT_PcrOffset_EnableOffset(stcChannel->pcrOffset, pidChannel->status.pidChannelIndex, false, enableJitterAdjustment);
    if (rc) {return BERR_TRACE(rc);}
#else
    BSTD_UNUSED(stcChannel);
    BSTD_UNUSED(pidChannel);
#endif
    return BERR_SUCCESS;
}

void NEXUS_StcChannel_GetIndex_priv( NEXUS_StcChannelHandle stcChannel, unsigned *pIndex )
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();
    *pIndex = stcChannel->stcIndex;
}

/*
Disable Output to a specified PID Channel
*/
BERR_Code NEXUS_StcChannel_DisablePidChannel_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_PidChannelHandle pidChannel
    )
{
#ifdef BXPT_HAS_MOSAIC_SUPPORT
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
#if defined(HUMAX_PLATFORM_BASE)
    /* NONE */
#else
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
#endif    
    BXPT_PcrOffset_DisableOffset(stcChannel->pcrOffset, pidChannel->status.pidChannelIndex);
#else
    BSTD_UNUSED(stcChannel);
    BSTD_UNUSED(pidChannel);
#endif
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_StcChannel_Freeze( NEXUS_StcChannelHandle stcChannel, bool frozen )
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    stcChannel->frozen = frozen;
    return BXPT_PcrOffset_FreezeStc( stcChannel->pcrOffset, frozen);
}

bool NEXUS_StcChannel_IsFrozen_priv( NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    return stcChannel->frozen;
}

void NEXUS_StcChannel_GetRate_priv(NEXUS_StcChannelHandle stcChannel, unsigned * increment, unsigned * prescale)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    *increment = stcChannel->increment;
    *prescale = stcChannel->prescale;
}

NEXUS_Error NEXUS_StcChannel_SetRate( NEXUS_StcChannelHandle stcChannel, unsigned Increment, unsigned Prescale )
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    /* STC_CTRL only has 8 bit resolution. So it cannot take values more than 256, so scale down. */
    while (Increment > 255 || Prescale > 255) {
        Increment /= 2;
        Prescale /= 2;
    }
    stcChannel->isStcTrick = (Increment != Prescale + 1);
    stcChannel->increment = Increment;
    stcChannel->prescale = Prescale;

    BDBG_MSG(("BXPT_PcrOffset_ChangeStcRate %d %d", Increment, Prescale));
    return BXPT_PcrOffset_ChangeStcRate( stcChannel->pcrOffset, Increment, Prescale );
}

static bool NEXUS_StcChannel_P_PerformGapDetection(NEXUS_StcChannelHandle thisChannel, bool * wait)
{
    bool gap = false;
#if BXPT_HAS_TSMUX
    bool thisCounterMoved, thatCounterMoved;
    NEXUS_StcChannelHandle thatChannel;
    NEXUS_StcChannelHandle stallingChannel;
    uint64_t thisCurrentCounterValue;
    uint64_t thatCurrentCounterValue;
    uint64_t thisIncrement;
    uint64_t thatIncrement;
    uint64_t diff;
    uint64_t avWindowTicks;
    uint64_t thisConverter;
    uint64_t thatConverter;
    BXPT_PcrOffset_Settings theseSettings;
    BXPT_PcrOffset_Settings thoseSettings;
    BXPT_PcrOffset_NRTConfig thisNrtConfig;
    BXPT_PcrOffset_NRTConfig thatNrtConfig;

    BDBG_MSG(("stcChannel%u %p: Performing gap detection...", thisChannel->stcIndex, thisChannel));

    thisCounterMoved = false;
    thatCounterMoved = false;

    thatChannel = thisChannel->pairedChannel;

    BXPT_PcrOffset_GetSettings(thisChannel->pcrOffset, &theseSettings);
    BXPT_PcrOffset_GetSettings(thatChannel->pcrOffset, &thoseSettings);
    
    BXPT_PcrOffset_GetNRTConfig(thisChannel->pcrOffset, &thisNrtConfig);
    BXPT_PcrOffset_GetNRTConfig(thatChannel->pcrOffset, &thatNrtConfig);

    if (theseSettings.CountMode != BXPT_PcrOffset_StcCountMode_eBinary)
    {
        thisConverter = 600; /* 45 KHz to 27 MHz conversion */
        thisIncrement = (thisNrtConfig.StcIncrement >> 9) * 300 + (thisNrtConfig.StcIncrement & 0x00000000000001FF);
    }
    else
    {
        thisConverter = 1;
        thisIncrement = thisNrtConfig.StcIncrement;
    }

    if (thoseSettings.CountMode != BXPT_PcrOffset_StcCountMode_eBinary)
    {
        thatConverter = 600; /* 45 KHz to 27 MHz conversion */
        thatIncrement = (thatNrtConfig.StcIncrement >> 9) * 300 + (thatNrtConfig.StcIncrement & 0x00000000000001FF);
    }
    else
    {
        thatConverter = 1;
        thatIncrement = thatNrtConfig.StcIncrement;
    }

    BDBG_MSG_TRACE(("stcChannel%u %p: thisConverter = %llu; thatConverter = %llu", thisChannel->stcIndex, thisChannel, thisConverter, thatConverter));

    /* convert everything to 27 MHz */

    thisCurrentCounterValue = BXPT_PcrOffset_GetStc(thisChannel->pcrOffset) * thisConverter;
    thatCurrentCounterValue = BXPT_PcrOffset_GetStc(thatChannel->pcrOffset) * thatConverter;

    BDBG_MSG_TRACE(("stcChannel%u %p: thisIncrement = %llu; thatIncrement = %llu", thisChannel->stcIndex, thisChannel, thisIncrement, thatIncrement));
    BDBG_MSG_TRACE(("stcChannel%u %p: thisCCV = %llu; thatCCV = %llu", thisChannel->stcIndex, thisChannel, thisCurrentCounterValue, thatCurrentCounterValue));
    BDBG_MSG_TRACE(("stcChannel%u %p: this->lCCVal = %s; that->lCCVal = %s",
        thisChannel->stcIndex, thisChannel,
        thisChannel->lastCounterValueValid ? "true" : "false", 
        thatChannel->lastCounterValueValid ? "true" : "false"));

    if (thisChannel->lastCounterValueValid && thatChannel->lastCounterValueValid)
    {
        thisCounterMoved = thisCurrentCounterValue != thisChannel->lastCounterValue;
        thatCounterMoved = thatCurrentCounterValue != thatChannel->lastCounterValue;

        BDBG_MSG_TRACE(("stcChannel%u %p: thisCMov = %s; this->lCV = %llu; thatCMov = %s; that->lCV = %llu", 
            thisChannel->stcIndex, thisChannel,
            thisCounterMoved ? "true" : "false", 
            thisChannel->lastCounterValue,
            thatCounterMoved ? "true" : "false",
            thatChannel->lastCounterValue));
        
        if (!thisCounterMoved && !thatCounterMoved) 
        {
            /* both STC counters are stalled */

            BDBG_MSG_TRACE(("stcChannel%u %p: thisAVW = %u; thatAVW = %u", thisChannel->stcIndex, thisChannel, thisNrtConfig.AvWindow, thatNrtConfig.AvWindow));
            
            if (thisNrtConfig.AvWindow == thatNrtConfig.AvWindow)
            {
                /* AV window milliseconds to 27 MHz clock tick conversion */
                avWindowTicks = (uint64_t)thisNrtConfig.AvWindow * 27000;

                BDBG_MSG_TRACE(("stcChannel%u %p: AVWTicks = %llu", thisChannel->stcIndex, thisChannel, avWindowTicks));

                /* check if (this STC + 1 increment - that STC) >= AV window or vice versa */
                
                if (thisCurrentCounterValue >= thatCurrentCounterValue)
                {
                    diff = thisCurrentCounterValue + thisIncrement - thatCurrentCounterValue;
                    stallingChannel = thatChannel;
                }
                else
                {
                    diff = thatCurrentCounterValue + thatIncrement - thisCurrentCounterValue;
                    stallingChannel = thisChannel;
                }

                BDBG_MSG_TRACE(("stcChannel%u %p: diff = %llu; stallingCh = %p", thisChannel->stcIndex, thisChannel, diff, stallingChannel));

                if (diff >= avWindowTicks)
                {
                    BDBG_MSG_TRACE(("stcChannel%u %p: this->lSSC = %p", thisChannel->stcIndex, thisChannel, thisChannel->lastStallingStcChannel));
                    
                    /* this condition means that both decoders are stalled
                    and the STCs are stalled because of av window reached */
                    if (!thisChannel->lastStallingStcChannel || thisChannel->lastStallingStcChannel == stallingChannel)
                    {
                        /* either this is our first gap, or the last gap was caused by the same channel */
                        thisChannel->lastStallingStcChannel = stallingChannel;
                        thatChannel->lastStallingStcChannel = stallingChannel;
                        gap = true;
                        BDBG_WRN(("stcChannel%u %p: Gap detected.  Stalling STC channel is %p", thisChannel->stcIndex, thisChannel, stallingChannel));
                    }
                    else /* (thisChannel->lastStallingStcChannel != stallingChannel) */
                    {
                        /* we've switched stalling channels, this is likely a deadlock, not a gap */
                        BDBG_WRN(("stcChannel%u %p: Gap stalling STC channel switch detected (%p -> %p), likely deadlock", thisChannel->stcIndex, thisChannel, thisChannel->lastStallingStcChannel, stallingChannel));
                        thisChannel->lastStallingStcChannel = NULL; /* reset for next detection cycle */
                        thatChannel->lastStallingStcChannel = NULL;
                        gap = false;
                    }
                }
                else
                {
                    BDBG_MSG_TRACE(("stcChannel%u %p: diff < AV window; no gap", thisChannel->stcIndex, thisChannel));
                }
            }
            else
            {
                BDBG_ERR(("stcChannel%u %p: Paired STC channels with different AV windows", thisChannel->stcIndex, thisChannel));
                goto end;
            }
        }
        else if (thisCounterMoved && thatCounterMoved)
        {
            BDBG_MSG_TRACE(("stcChannel%u %p: Both paired channel STC counters moved, resetting gap detection", thisChannel->stcIndex, thisChannel));
            /* if both counters move, we are not stalled any more, need to reset */
            /* this is not true if only one counter moves, like from attemped ZF during true deadlock */
            thisChannel->lastStallingStcChannel = NULL;
            thatChannel->lastStallingStcChannel = NULL;
        }
        else
        {
            BDBG_MSG_TRACE(("stcChannel%u %p: %s counter moved; no gap", thisChannel->stcIndex, thisChannel, thisCounterMoved ? "this" : "that"));
        }
    }
    else
    {
        *wait = true;
    }
    
    thisChannel->lastCounterValue = thisCurrentCounterValue;
    thisChannel->lastCounterValueValid = true;
    thatChannel->lastCounterValue = thatCurrentCounterValue;
    thatChannel->lastCounterValueValid = true;

end:    
#else
    BSTD_UNUSED(thisChannel);
    *wait = false;
#endif
    return gap;
}

typedef struct NEXUS_StcChannelDeadlockDetectionInfo
{
    bool allHung;
    bool oneEmpty;
    bool oneFull;
} NEXUS_StcChannelDeadlockDetectionInfo;

static void NEXUS_StcChannel_P_InitDeadlockDetectionInfo(NEXUS_StcChannelDeadlockDetectionInfo * pInfo)
{
    BKNI_Memset(pInfo, 0, sizeof(NEXUS_StcChannelDeadlockDetectionInfo));    
    pInfo->allHung = true;
    pInfo->oneFull = false;
    pInfo->oneEmpty = false;
}

static void NEXUS_StcChannel_P_PerformDeadlockDetection(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDeadlockDetectionInfo * pInfo)
{
    unsigned i = 0;

    NEXUS_StcChannel_P_InitDeadlockDetectionInfo(pInfo);

    BDBG_MSG(("stcChannel%u %p: Performing deadlock detection...", stcChannel->stcIndex, stcChannel));

    for (i=0;i<NEXUS_StcChannelDecoderType_eMax;i++) {
        if (stcChannel->pcrlibData[i].connected) {
            if (stcChannel->pcrlibData[i].percentFull > 95) {
                pInfo->oneFull = true;
            }
            if (stcChannel->pcrlibData[i].percentFull < 5) {
                pInfo->oneEmpty = true;
            }
            if (!stcChannel->pcrlibData[i].isHung) {
                pInfo->allHung = false;
            }
            else {
                BDBG_MSG(("stcChannel%u %p, %s fifo is static (%d%% full)", stcChannel->stcIndex, stcChannel, g_stcChannelTypeStr[i], stcChannel->pcrlibData[i].percentFull));
            }
        }
    }
}

void NEXUS_StcChannel_ReportDecoderHang_priv( NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type, bool hung, unsigned percentFull, bool *pShouldFlush, bool *pShouldZeroFill )
{
    NEXUS_StcChannelDeadlockDetectionInfo overallDdInfo, thisDdInfo;
    bool gap, wait;
    unsigned i;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();

    *pShouldFlush = false;
    *pShouldZeroFill = false;
    gap = false;
    wait = false;

    /* this logic only applies to auto mode */
    if (stcChannel->settings.mode != NEXUS_StcChannelMode_eAuto) {
        return;
    }

    /* if we're in an stc trick mode, ignore any reports of hangs */
    /* TODO: consider only limiting for very slow stc trick. */
    if (stcChannel->isStcTrick) {
        return;
    }

    stcChannel->pcrlibData[type].isHung = hung;
    stcChannel->pcrlibData[type].percentFull = percentFull;

    NEXUS_StcChannel_P_InitDeadlockDetectionInfo(&overallDdInfo);

    /* see if all connected decoders are hung.
       we know there must be at least one connected, otherwise we would not receive this call */
    NEXUS_StcChannel_P_PerformDeadlockDetection(stcChannel, &thisDdInfo);

    if (stcChannel->pairedChannel)
    {
        NEXUS_StcChannelHandle thatChannel = stcChannel->pairedChannel;
        NEXUS_StcChannelDeadlockDetectionInfo thatDdInfo;
        
        NEXUS_StcChannel_P_PerformDeadlockDetection(thatChannel, &thatDdInfo);

        overallDdInfo.allHung = thisDdInfo.allHung && thatDdInfo.allHung;
        overallDdInfo.oneEmpty = thisDdInfo.oneEmpty || thatDdInfo.oneEmpty;
        overallDdInfo.oneFull = thisDdInfo.oneFull || thatDdInfo.oneFull;
        
        /* check for gaps, which only happen with paired STC channels in NRT mode */
        if (stcChannel->nonRealTime && overallDdInfo.allHung && overallDdInfo.oneEmpty)
        {
            BDBG_MSG(("stcChannel%u %p: All decoders are hung, at least one fifo is empty", stcChannel->stcIndex, stcChannel));
            gap = NEXUS_StcChannel_P_PerformGapDetection(stcChannel, &wait);
        }
    }
    else
    {
        overallDdInfo = thisDdInfo;
    }

    if (gap)
    {   
        BDBG_WRN(("stcChannel%u %p: Gap detected", stcChannel->stcIndex, stcChannel));
        /* the only decoder(s) that should ZF are the ones whose STC caused the stall */
        if (stcChannel == stcChannel->lastStallingStcChannel)
        {
            *pShouldZeroFill = true;
        }
    }
    else
    {
        if (!wait)
        {
        if (overallDdInfo.allHung && overallDdInfo.oneFull && overallDdInfo.oneEmpty) {
                BDBG_WRN(("stcChannel%u %p: All decoders are stuck, one fifo is full and one is empty. Flushing fifo's to recover from likely TSM deadlock.", stcChannel->stcIndex, stcChannel));
            for (i=0;i<NEXUS_StcChannelDecoderType_eMax;i++) {
                /* the only way this gets set back to false is for an actual flush to occur, or a disconnect */
                if (stcChannel->pcrlibData[i].connected) {
                    stcChannel->pcrlibData[i].requiresFlush = true;
                }
            }
        }
        else
        {
                BDBG_MSG(("stcChannel%u %p: Neither gap nor deadlock detected", stcChannel->stcIndex, stcChannel));
            }
        }
        else
        {
            BDBG_MSG(("stcChannel%u %p: Waiting one cycle for gap detection", stcChannel->stcIndex, stcChannel));
        }
    }

    /* report if this decoder should flush. even if all are not hung now, if there's an outstanding request to flush,
    it must be acted on. */
    *pShouldFlush = stcChannel->pcrlibData[type].requiresFlush;
}

void NEXUS_StcChannel_ReportFlush_priv( NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType type )
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();
    stcChannel->pcrlibData[type].requiresFlush = false;
}

NEXUS_Error NEXUS_StcChannel_VerifyPidChannel( NEXUS_StcChannelHandle stcChannel , NEXUS_PidChannelHandle pidChannel )
{
    NEXUS_TransportType pidChannelTransportType, stcTransportType = NEXUS_TransportType_eUnknown;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    pidChannelTransportType = pidChannel->status.originalTransportType;

    switch ( stcChannel->settings.mode) {
        case NEXUS_StcChannelMode_eHost:
            stcTransportType = stcChannel->settings.modeSettings.host.transportType;
            break;
        case NEXUS_StcChannelMode_eAuto:
            stcTransportType = stcChannel->settings.modeSettings.Auto.transportType;
            break;
        case NEXUS_StcChannelMode_ePcr:
            if (!stcChannel->settings.modeSettings.pcr.pidChannel) {
                break;
            }
            stcTransportType = stcChannel->settings.modeSettings.pcr.pidChannel->status.transportType;
            break;
        default:  /* not used */
            break;
    }

    if ( NEXUS_IS_DSS_MODE(stcTransportType) !=  NEXUS_IS_DSS_MODE(pidChannelTransportType) ) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return rc;
}

static NEXUS_Error NEXUS_StcChannel_P_RegenStcRequest(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelDecoderType decoder)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BAVC_PTSInfo pts;
#if !BDBG_NO_MSG
    static struct
    {
        char * name;
        NEXUS_StcChannelDecoderType base;
    } const decMap[] = 
    { 
        {"video", NEXUS_StcChannelDecoderType_eVideo0},
        {"audio", NEXUS_StcChannelDecoderType_eAudio0}
    };
    unsigned index = 0;
#endif

    if (decoder != NEXUS_StcChannelDecoderType_eMax)
    {
#if !BDBG_NO_MSG
        if (decoder != NEXUS_StcChannelDecoderType_eVideo0)
        {
            index = 1;
        }
#endif

        if (stcChannel->pcrlibData[decoder].connected)
        {
            BDBG_MSG(("Regenerating an STC request on behalf of %s%u for stc%u channel %p", 
                decMap[index].name, decoder - decMap[index].base, stcChannel->stcIndex, stcChannel));
            BKNI_EnterCriticalSection();
            NEXUS_StcChannel_GetPtsCallbackWrapper_isr(&stcChannel->pcrlibData[decoder], &pts);
            rc = NEXUS_StcChannel_RequestStc_isr(stcChannel, decoder, &pts);
            BKNI_LeaveCriticalSection();
            if (rc)
            {
                rc=BERR_TRACE(rc);
                goto error;
            }
        }
    }

error:
    return rc;
}

#if NEXUS_HAS_ASTM
void NEXUS_StcChannel_GetAstmSettings_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmSettings * pAstmSettings  /* [out] */
)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    NEXUS_ASSERT_MODULE();
    *pAstmSettings = stcChannel->astm.settings;
}

NEXUS_Error NEXUS_StcChannel_SetAstmSettings_priv(
    NEXUS_StcChannelHandle stcChannel,
    const NEXUS_StcChannelAstmSettings * pAstmSettings
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    bool regenVideoStcRequest = false;
    bool regenAudioStcRequest = false;

    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    NEXUS_ASSERT_MODULE();

    if
        (
            (pAstmSettings->mode == NEXUS_StcChannelMode_eAuto)
            &&
            (
                (
                    (stcChannel->astm.settings.tsmMode == NEXUS_StcChannelTsmMode_eOutputMaster)
                    &&
                    (pAstmSettings->tsmMode != NEXUS_StcChannelTsmMode_eOutputMaster)
                )
                ||
                (stcChannel->astm.settings.mode == NEXUS_StcChannelMode_ePcr)
            )
        )
    {
        if (pAstmSettings->tsmMode == NEXUS_StcChannelTsmMode_eVideoMaster)
        {
            regenVideoStcRequest = true;
        }
        else if (pAstmSettings->tsmMode == NEXUS_StcChannelTsmMode_eAudioMaster)
        {
            regenAudioStcRequest = true;
        }
        /* CSP359468: STC master mode must also generate a request, so we do
        both (just like in real STC master mode) */
        else if (pAstmSettings->tsmMode == NEXUS_StcChannelTsmMode_eStcMaster)
        {
            regenVideoStcRequest = true;
            regenAudioStcRequest = true;
        }
    }

    /* always copy settings, so ASTM can get what it sets */
    stcChannel->astm.settings = *pAstmSettings;

    /* if ASTM is internally permitted, apply settings and regen requests if necessary */
    rc = NEXUS_StcChannel_P_ApplySettings(stcChannel, true);
    if (rc) goto error;

    if (regenVideoStcRequest || regenAudioStcRequest)
    {
        BDBG_MSG(("ASTM is forcing an STC request regeneration"));
    }

    /* CSP359468: changed order of processing requests to match many real streams (audio request is
    first for STC master mode) */

    if (regenAudioStcRequest)
    {
        struct NEXUS_PcrlibData * p1audio = BLST_Q_FIRST(&stcChannel->decoderPriorities);
        if (p1audio)
        {
            rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, p1audio->type);
        }
    }
    
    if (regenVideoStcRequest)
    {
        rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, NEXUS_StcChannelDecoderType_eVideo0);
    }

error:
    return rc;
}

NEXUS_Error NEXUS_StcChannel_GetAstmStatus_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmStatus * pAstmStatus
)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);

    pAstmStatus->timebase = stcChannel->timebase;
    return 0;
}

#endif /* NEXUS_HAS_ASTM */

void NEXUS_StcChannel_GetDefaultPairSettings(NEXUS_StcChannelPairSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->window = 100;
    pSettings->connected = false;
    return;
}

NEXUS_Error NEXUS_StcChannel_SetPairSettings( NEXUS_StcChannelHandle stcChannel1, NEXUS_StcChannelHandle stcChannel2, const NEXUS_StcChannelPairSettings *pSettings)
{
#if BXPT_HAS_TSMUX
    BERR_Code rc;
    BXPT_PcrOffset_NRTConfig nrtConfig1, nrtConfig2;
    BXPT_PcrOffset_Settings pcrOffsetSettings1, pcrOffsetSettings2;
    BPCRlib_Config pcrlibConfig;

    BDBG_OBJECT_ASSERT(stcChannel1, NEXUS_StcChannel);
    BDBG_OBJECT_ASSERT(stcChannel2, NEXUS_StcChannel);
    BDBG_ASSERT(pSettings);

    BDBG_MSG(("channel%u %p is paired with channel%u %p", stcChannel1->stcIndex, stcChannel1, stcChannel2->stcIndex, stcChannel2));
    stcChannel1->pairedChannel = pSettings->connected ? stcChannel2 : NULL;
    stcChannel2->pairedChannel = pSettings->connected ? stcChannel1 : NULL;

    BXPT_PcrOffset_GetNRTConfig( stcChannel1->pcrOffset, &nrtConfig1 );
    BXPT_PcrOffset_GetNRTConfig( stcChannel2->pcrOffset, &nrtConfig2 );

    rc = BXPT_PcrOffset_GetSettings( stcChannel1->pcrOffset, &pcrOffsetSettings1 );
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    rc = BXPT_PcrOffset_GetSettings( stcChannel2->pcrOffset, &pcrOffsetSettings2 );
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    nrtConfig1.PairedStc = pcrOffsetSettings2.StcSelect;
    nrtConfig2.PairedStc = pcrOffsetSettings1.StcSelect;

    nrtConfig1.EnableAvWindowComparison = pSettings->connected;
    nrtConfig2.EnableAvWindowComparison = pSettings->connected;

    nrtConfig1.AvWindow = pSettings->window;
    nrtConfig2.AvWindow = pSettings->window;

    rc = BXPT_PcrOffset_SetNRTConfig(stcChannel1->pcrOffset, &nrtConfig1);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    rc = BXPT_PcrOffset_SetNRTConfig(stcChannel2->pcrOffset, &nrtConfig2);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    BPCRlib_Channel_GetConfig(stcChannel1->pcrlibChannel, &pcrlibConfig);
    pcrlibConfig.paired = pSettings->connected;
    rc = BPCRlib_Channel_SetConfig(stcChannel1->pcrlibChannel, &pcrlibConfig);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    BPCRlib_Channel_GetConfig(stcChannel2->pcrlibChannel, &pcrlibConfig);
    pcrlibConfig.paired = pSettings->connected;
    rc = BPCRlib_Channel_SetConfig(stcChannel2->pcrlibChannel, &pcrlibConfig);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    return NEXUS_SUCCESS;
error:
    return rc;
    
#else /* BXPT_HAS_TSMUX */
    BSTD_UNUSED(stcChannel1);
    BSTD_UNUSED(stcChannel2);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

void NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv (NEXUS_StcChannelNonRealtimeSettings *pSettings)
{
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->triggerMode = NEXUS_StcChannelTriggerMode_eTimebase;
    return;
}

NEXUS_Error NEXUS_StcChannel_SetNonRealtimeConfig_priv(NEXUS_StcChannelHandle handle, NEXUS_StcChannelNonRealtimeSettings *pSettings)
{
#if BXPT_HAS_TSMUX
    BERR_Code rc;
    BXPT_PcrOffset_NRTConfig nrtConfig;
    BPCRlib_Config pcrlibConfig;
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pSettings);
    BDBG_OBJECT_ASSERT(handle, NEXUS_StcChannel);
    BDBG_CASSERT(NEXUS_StcChannelTriggerMode_eTimebase  ==  (unsigned)BXPT_PcrOffset_StcTriggerMode_eTimebase);
    BDBG_CASSERT(NEXUS_StcChannelTriggerMode_eExternalTrig ==  (unsigned)BXPT_PcrOffset_StcTriggerMode_eExternalTrig);
    BDBG_CASSERT(NEXUS_StcChannelTriggerMode_eSoftIncrement == (unsigned)BXPT_PcrOffset_StcTriggerMode_eSoftIncrement);
    BXPT_PcrOffset_GetNRTConfig( handle->pcrOffset, &nrtConfig );
    handle->nonRealTime = (pSettings->triggerMode != NEXUS_StcChannelTriggerMode_eTimebase);
    BDBG_MSG(("%p STC%u channel configured in %s mode", handle, handle->stcIndex, handle->nonRealTime ? "non-realtime" : "realtime"));
    nrtConfig.TriggerMode = pSettings->triggerMode;
#if B_HAS_DSS
    if(NEXUS_IS_DSS_MODE(NEXUS_StcChannel_P_GetTransportType(&handle->settings)))
    {
        nrtConfig.StcIncrement = pSettings->stcIncrement; /* binary format for DSS */
    }
    else
#endif
    {/* binary to MOD300 conversion for MPEG2TS */
        nrtConfig.StcIncrement = ((pSettings->stcIncrement/300)<<9) + (pSettings->stcIncrement % 300);
    }
    nrtConfig.ExternalTriggerNum = pSettings->externalTrigger;

    rc = BXPT_PcrOffset_SetNRTConfig(handle->pcrOffset, &nrtConfig);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}

    if (handle->nonRealTime)
    {
        rc = BXPT_PcrOffset_SetStc(handle->pcrOffset, 0);
        if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}
    }

    BPCRlib_Channel_GetConfig(handle->pcrlibChannel, &pcrlibConfig);
    pcrlibConfig.non_real_time = handle->nonRealTime;
    rc = BPCRlib_Channel_SetConfig(handle->pcrlibChannel, &pcrlibConfig);
    if(rc!=BERR_SUCCESS){rc=BERR_TRACE(rc);goto error;}
    

    NEXUS_StcChannel_P_SetSwPcrOffsetEnabled(handle); /* re-set this as it depends on NRT config */
    
    return NEXUS_SUCCESS;
error:
    return rc;
#else /* BXPT_HAS_TSMUX */
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pSettings);
    BSTD_UNUSED(handle);
    if(pSettings->triggerMode==NEXUS_StcChannelTriggerMode_eTimebase) {
        return NEXUS_SUCCESS;
    }
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif

}

void NEXUS_StcChannel_GetSoftIncrementRegOffset_priv(NEXUS_StcChannelHandle handle, BAVC_Xpt_StcSoftIncRegisters *regMap)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(handle, NEXUS_StcChannel);
    BDBG_ASSERT(regMap);
#if BXPT_HAS_TSMUX
    BXPT_PcrOffset_GetSoftIncrementRegisterOffsets(handle->pcrOffset, regMap);
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(regMap);
    (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
    return;
}

bool NEXUS_StcChannel_IsStcValid_isr(NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    return stcChannel->stcValid;
}

uint32_t NEXUS_StcChannel_GetSwPcrOffset_isr(NEXUS_StcChannelHandle stcChannel)
{
    BDBG_OBJECT_ASSERT(stcChannel, NEXUS_StcChannel);
    return stcChannel->swPcrOffset;
}

void NEXUS_StcChannel_GetDefaultLowLatencySettings_priv(NEXUS_StcChannelLowLatencySettings * pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_StcChannelLowLatencySettings));
}

NEXUS_Error NEXUS_StcChannel_SetLowLatencySettings_priv(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelDecoderType decoder, NEXUS_StcChannelLowLatencySettings * pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelDecoderType oldDecoder = stcChannel->lowLatencyDecoder;
    bool oldModeEnabled = oldDecoder != NEXUS_StcChannelDecoderType_eMax;
    bool newModeEnabled = pSettings->enabled;
    struct NEXUS_PcrlibData * p1audio = BLST_Q_FIRST(&stcChannel->decoderPriorities);

    if (!oldModeEnabled && newModeEnabled)
    {
        /* switching into low latency mode does nothing until a request comes in */
        stcChannel->lowLatencyDecoder = decoder;
    }
    else if (oldModeEnabled && !newModeEnabled)
    {
        /* if we are switching out of low latency mode */
        stcChannel->lowLatencyDecoder = NEXUS_StcChannelDecoderType_eMax;
        
        switch (stcChannel->settings.mode)
        {
            case NEXUS_StcChannelMode_ePcr:
                /* regen PCR offset */
                BKNI_EnterCriticalSection();
                NEXUS_StcChannel_UpdateStcCallbackWrapper_isr(stcChannel, true);
                BKNI_LeaveCriticalSection();
                break;
            case NEXUS_StcChannelMode_eAuto:
                switch (stcChannel->settings.modeSettings.Auto.behavior)
                {
                    case NEXUS_StcChannelAutoModeBehavior_eFirstAvailable:
                        /* audio, then video, to mimic normal streams */
                        rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, p1audio->type);
                        if (rc) goto error;
                        rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, NEXUS_StcChannelDecoderType_eVideo0);
                        break;
                    case NEXUS_StcChannelAutoModeBehavior_eVideoMaster:
                        if (oldDecoder != NEXUS_StcChannelDecoderType_eVideo0)
                        {
                            rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, NEXUS_StcChannelDecoderType_eVideo0);
                        }
                        break;
                    case NEXUS_StcChannelAutoModeBehavior_eAudioMaster:
                        if (oldDecoder == NEXUS_StcChannelDecoderType_eVideo0)
                        {
                            rc = NEXUS_StcChannel_P_RegenStcRequest(stcChannel, p1audio->type);
                        }
                        break;
                    default:
                        /* do nothing for output master */
                        break;
                }
                break;
            default: 
                /* do nothing special for host mode */
                break;
        }
    }
    else if (oldModeEnabled && newModeEnabled)
    {
        if (oldDecoder != decoder)
        {
            BDBG_WRN(("UNSUPPORTED: Decoders %u and %u both tried to enable low latency TSM mode on a shared STC channel", oldDecoder, decoder));
        }
    }
    
error:
    return rc;
}

