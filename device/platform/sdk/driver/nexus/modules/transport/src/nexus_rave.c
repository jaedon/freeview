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
 * $brcm_Workfile: nexus_rave.c $
 * $brcm_Revision: 105 $
 * $brcm_Date: 10/12/12 12:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_rave.c $
 * 
 * 105   10/12/12 12:47p vsilyaev
 * SW7425-3955: Fixed typo
 * 
 * 104   10/11/12 12:06p vsilyaev
 * SW7425-3955: Use upper 8 bits as PES sub-id for MPEG-2 TS streams
 * 
 * 103   8/29/12 5:20p bandrews
 * SW7425-3820: fix copy-paste error
 * 
 * 102   8/29/12 5:06p bandrews
 * SW7231-975: add ITB status accessor
 * 
 * 101   7/18/12 9:01a erickson
 * SW7552-306: disable cc check in rave in lieu of new decoder api
 * 
 * 100   7/5/12 1:30p erickson
 * SW7425-3288: set BXPT_Rave_AvSettings.EnableBPPSearch = true
 * 
 * 99   6/19/12 6:04p vsilyaev
 * SW7425-3028: Fixed itb scan
 * 
 * 98   6/18/12 11:45a vsilyaev
 * SW7425-3028: Fixed copy-and-paste error
 * 
 * 97   5/30/12 4:50p vsilyaev
 * SW7425-3028: Added function to look for specific PTS in the rave buffer
 * 
 * 96   5/23/12 10:17a jgarrett
 * SW7425-3065: Adding BD-LPCM private header generation
 * 
 * 95   4/12/12 4:53p jgarrett
 * SW7425-2378: Adding missing break
 * 
 * 94   4/12/12 4:00p jgarrett
 * SW7425-2378: Adding Monkey's Audio and MLP
 * 
 * 93   3/28/12 6:34p mward
 * SW7435-64:  Add NEXUS_BDSP_VP6_SUPPORT to NEXUS_PLATFORM_DEFINES if
 *  BDSP_VP6_SUPPORT=y, to differentiate platforms (such as 7435) which
 *  can decode VP6 in AVD, same as NEXUS_VideoCodec_eVp8.
 * 
 * 92   3/14/12 11:39a erickson
 * SW7401-4517: remove old code
 * 
 * 91   2/22/12 5:13p erickson
 * SW7425-2459: OTFPVR requires heap used by RAVE, verify CPU
 *  accessibility
 * 
 * 90   2/16/12 3:22p jtna
 * SW7425-2357: do H263-like ES indexing for Spark ES
 * 
 * 89   2/14/12 12:50p vsilyaev
 * SW7425-2258: Added Motion Jpeg codec type
 * 
 * 88   2/7/12 7:34p jgarrett
 * SW7425-2136: Adding FLAC support
 * 
 * 87   2/2/12 4:47p jgarrett
 * SW7425-2268: Adding voice conferencing codecs
 * 
 * 86   12/29/11 1:14p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM
 * 
 * 85   11/21/11 2:48p jgarrett
 * SW7425-1406: Adding Vorbis audio support
 * 
 * VORBIS_DEVEL/2   11/16/11 4:21p jgarrett
 * SW7425-1406: Merge latest changes from main branch
 * 
 * 84   10/19/11 6:03p jtna
 * SW7422-452: update RAVE programming for vp6 video
 * 
 * VORBIS_DEVEL/1   10/5/11 6:18p jgarrett
 * SW7425-1406: Adding Vorbis support
 * 
 * 83   9/23/11 11:07a jtna
 * SW7346-493: Coverity defect 35380: DEADCODE
 * 
 * 82   8/26/11 5:57p jgarrett
 * SW7344-160: Adding DTS-Legacy support
 * 
 * 81   8/25/11 3:29p erickson
 * SW7346-454: add NEXUS_RecpumpStatus.rave.index
 * 
 * 80   8/15/11 3:11p vsilyaev
 * SW7425-938: Added mapping for SH263 and VP8 video codecs
 * 
 * 79   7/1/11 11:18a jgarrett
 * SW7405-5071: Adding WMA TS
 * 
 * 78   4/18/11 10:19a erickson
 * SW7335-1168: add NEXUS_RaveStatus.enabled
 *
 * 77   4/12/11 10:28a erickson
 * SW7342-331: switch SW RAVE extension to use
 *  BXPT_RaveSoftMode_ePointersOnly, fail on any codec that requires ITB
 *  manipulation. rename to NEXUS_SW_RAVE_PEEK_EXTENSION.
 *
 * 76   4/6/11 2:56p erickson
 * SW7342-340: add HW rave indices to NEXUS_VideoDecoderExtendedStatus
 *
 * 75   3/11/11 3:33p vsilyaev
 * SW7422-14: Properly signal invalid video codec
 *
 * 74   2/28/11 11:06a erickson
 * SW7342-331: add template SW RAVE extension
 *
 * 73   2/14/11 12:09p dlwin
 * SW7405-5140: Updated with a better fix to the problem.
 *
 * 72   2/14/11 11:38a dlwin
 * SW7405-5140: For ES input streams, Nexus needs to disable XPT
 *  DISABLE_BEFORE_PES filter.
 *
 * 71   1/19/11 3:48p vsilyaev
 * SW7422-216: Added VP6 video codec
 *
 * 70   12/13/10 4:06p erickson
 * SW35230-179: add NEXUS_RaveStatus.numOutputBytes
 *
 * 69   11/29/10 11:33a vsilyaev
 * SW7422-65: Fixed mapping of SVC/MVC video codecs
 *
 * 68   11/18/10 1:49p akam
 * SW35230-91: Added ADPCM audio decoder support.
 *
 * 67   11/5/10 12:28p gmohile
 * SW7422-20 : Use BXPT_HAS_PID_CHANNEL_PES_FILTERING instead of
 *  BXPT_P_MAX_PACKETIZERS_PER_PB
 *
 * 66   11/3/10 7:10p jgarrett
 * SW35230-91: Renamed NEXUS_AudioCodec_eRealAudioLbr to _eCook
 *
 * 65   11/3/10 4:32p erickson
 * SW7422-20: use BXPT_P_MAX_PACKETIZERS_PER_PB instead of
 *  NEXUS_SW_VOB_SUPPORT
 *
 * 64   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 63   10/14/10 11:19a akam
 * SW35230-91: Added Real Audio LBR codec support.
 *
 * 62   9/8/10 12:06p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 61   6/22/10 3:57p erickson
 * SW7405-4492: refactor RAVE BDBG_MSG to print context #, codec and pid
 *  ch for audio and video. allow user to figure out which RAVE context is
 *  being used.
 *
 * 60   5/13/10 6:50p jgarrett
 * SW7405-4298: Adding AD extraction for AAC/AAC-HE/AC3/DDP
 *
 * 59   5/3/10 5:22p jgarrett
 * SW7405-4212: Removing DolbyPulse as an explicit codec type
 *
 * SW7405-4212/1   4/26/10 5:23p jgarrett
 * SW7405-4212: Adding MS10 changes for pulse/transcode
 *
 * 58   3/18/10 10:55a jgarrett
 * SW7405-4030: Adding Dolby Pulse Support
 *
 * 57   3/10/10 6:59p jtna
 * SW3556-1051: enable sw rave for mkv streams
 *
 * 56   2/8/10 5:31p jtna
 * SW3556-1003: added option to disable reordering of video frames
 *
 * 55   1/12/10 6:16p jtna
 * SW3548-2716: fix enabling of swRave for AVI/ASF streams
 *
 * 47   7/15/09 7:24p vsilyaev
 * PR 55653: Added WAV format
 *
 * 46   6/30/09 4:49p katrep
 * PR55809: Add support for PCMWAV used with AVI PCM
 *
 * 45   6/4/09 5:45p katrep
 * PR55074: Add support DTS Broadcast decoder
 *
 * 44   5/8/09 2:42p erickson
 * PR54061: set default values to BXPT_Rave_SetContextConfig. this fixes
 *  MPEG audio in VOB after AC3 audio in VOB.
 *
 * 43   3/30/09 10:35a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 42   3/17/09 5:41p vsilyaev
 * PR 46190: Adding mappings to the PCM audio codec
 *
 * 41   2/4/09 7:09p vsilyaev
 * PR 50887: Added bit to enable OTF PVR mode
 *
 * 40   1/16/09 2:02p erickson
 * PR50905: added NEXUS_Rave_GetMostRecentPts_priv
 *
 * 39   1/16/09 2:00p erickson
 * PR51209: must check if swRave is enabled before using its context map
 *
 * 38   12/8/08 12:22p erickson
 * PR48155: check cdb valid and read to determine if fifo is hung
 *
 * 37   12/3/08 1:18p erickson
 * PR45405: rename rave member to pidChannelIndex to avoid
 *  misunderstanding
 *
 * 36   12/3/08 12:26p erickson
 * PR45405: implicit NEXUS_Rave_RemovePidChannel_priv is needed when
 *  closing rave
 *
 * 35   11/18/08 12:56p erickson
 * PR48155: report swRave CDB_Valid if appropriate
 *
 * 34   10/21/08 3:35p erickson
 * PR48149: start/stop sw rave timer when rave context is enabled/disabled
 *
 * 33   9/11/08 4:37p erickson
 * PR46825: fix sw rave
 *
 * 32   9/9/08 10:04a erickson
 * PR45940: added NEXUS_Rave_GetCdbValidPointer_isr for better detection
 *  if source fifo goes dry
 *
 * 31   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 30   8/4/08 9:15a erickson
 * PR45037: fix 3563 compilation
 *
 * 29   7/31/08 9:22a erickson
 * PR45037: fix HW index detection
 *
 * 28   7/29/08 2:24p erickson
 * PR45238: convert WRN to MSG
 *
 * 27   7/23/08 3:01p jgarrett
 * PR 39452: Merging to main branch
 *
 * 26   7/21/08 4:32p katrep
 * PR44143: Set the LOAS as default configuration for AAC Plus audio
 *  format.
 *
 * 25   7/17/08 5:24p jgarrett
 * PR 42642: Adding Get/Set thresholds
 *
 * 24   7/1/08 11:43a erickson
 * PR44070: fix non-sw rave chips
 *
 * 23   6/30/08 1:45p erickson
 * PR44165: impl NEXUS_CORE_NUM_SECURE_HEAPS
 *
 * 22   6/26/08 4:33p erickson
 * PR44070: NEXUS_Rave_FindVideoStartCode_priv needs to search swRave
 *  context if enabled. NEXUS_VideoCodec_eDivx311 should be handled like
 *  NEXUS_VideoCodec_eDivx311 if SW rave not enabled.
 *
 * 21   5/30/08 12:45p erickson
 * PR43123: fix non-sw rave chips
 *
 * 20   5/30/08 12:40p erickson
 * PR43123: properly flush sw RAVE context if B_USE_SETTOP_API_SW_RAVE is
 *  defined
 *
 * 19   5/30/08 10:54a erickson
 * PR43123: add USE_SETTOP_API_SW_RAVE build option, but default it off.
 *
 * 18   5/22/08 1:48p vsilyaev
 * PR 42365: Only use SW rave of required by original transport format
 *
 * 17   5/20/08 10:08a erickson
 * PR42758: check pidChannelStatus.originalTransportType for VOB test
 *
 * 16   5/19/08 3:48p gmullen
 * PR42758: Added AC3+ case to VOB config code.
 *
 * Nexus_Audio_Descriptors/1   5/15/08 5:35p jgarrett
 * PR 42360: Adding audio descriptor placeholder
 *
 * 15   5/14/08 3:28p jgarrett
 * PR 42221: Adding audio frame depth
 *
 * 14   5/9/08 4:51p erickson
 * PR34925: added cont_count_ignore env override for debug
 *
 * 13   4/30/08 10:11a erickson
 * PR42349: cancel swRave timer in NEXUS_CancelTimer
 *
 * 12   4/10/08 2:04p erickson
 * PR37893: fix warning
 *
 * 11   4/7/08 10:16a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 10   4/4/08 11:18a erickson
 * PR40079: impl NEXUS_PidChannel_GetScramblingStatus
 *
 * 9   3/26/08 12:14p katrep
 * PR40841: For DSS ES streams stream type should be changed to DDS PES
 *  when given to RAVE.
 *
 * 8   3/18/08 4:51p erickson
 * PR40307: perform more robust check of itb pointers before dereferencing
 *
 * 7   3/14/08 7:06p katrep
 * PR40514:Add support for avs
 *
 * 6   3/3/08 11:30a erickson
 * PR39934: fix SW RAVE flush
 *
 * 5   2/27/08 9:19a erickson
 * PR39934: fix non-sw rave platforms
 *
 * 4   2/26/08 1:15p erickson
 * PR39934: don't reschedule a timer when it's already running
 *
 * 3   2/26/08 10:01a erickson
 * PR39934: added SW RAVE support
 *
 * 2   2/20/08 4:59p erickson
 * PR39786: add NEXUS_Rave_FindVideoStartCode_priv
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/29   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/28   12/28/07 5:19p erickson
 * PR38470: move conversion functions to core
 *
 * Nexus_Devel/27   12/3/07 4:46p vsilyaev
 * PR 36570: More generous safety thresholds for rave
 *
 * Nexus_Devel/26   11/9/07 5:04p erickson
 * PR36774: fixing 7401
 *
 * Nexus_Devel/25   11/9/07 4:23p jgarrett
 * PR 36774: Adding conditional compiles for certain platforms
 *
 * Nexus_Devel/24   11/9/07 4:05p jgarrett
 * PR 36774: converting to Ac3+ Itb format for 7405
 *
 * Nexus_Devel/23   11/6/07 3:43p erickson
 * PR34925: added BDBG_OBJECT checking
 *
 * Nexus_Devel/22   11/1/07 1:30p erickson
 * PR36637: use latest transport calls
 *
 * Nexus_Devel/21   11/1/07 9:06a erickson
 * PR36633: update base enums
 *
 * Nexus_Devel/20   10/31/07 9:07a erickson
 * PR36570: added 3563 support
 *
 * Nexus_Devel/19   10/24/07 5:40p vsilyaev
 * PR 36404: Added audio decode trick mode
 *
 * Nexus_Devel/18   10/19/07 11:21a vsilyaev
 * PR 35824: Fixed data corruption on trick modes
 *
 * Nexus_Devel/17   10/4/07 12:54p erickson
 * PR34925: use standard NEXUS_Error values
 *
 * Nexus_Devel/16   10/3/07 2:50p erickson
 * PR34925: call BXPT_Rave_GetBufferInfo_isr
 *
 * Nexus_Devel/15   10/2/07 1:59p erickson
 * PR34925: NEXUS_Rave_GetStatus_priv needs to call
 * BXPT_Rave_GetContextRegisters
 *
 * Nexus_Devel/14   10/1/07 1:45p erickson
 * PR34925: make Rave a private API
 *
 * Nexus_Devel/13   9/28/07 4:24p jgarrett
 * PR 34594: Downgrading prints
 *
 * Nexus_Devel/12   9/27/07 7:52p jgarrett
 * PR 34954: Adding debug
 *
 * Nexus_Devel/11   9/27/07 1:43p jgarrett
 * PR 34954: Adding RAVE Audio config
 *
 * Nexus_Devel/10   9/25/07 4:33p erickson
 * PR34925: fix NEXUS_Rave_Open logic
 *
 * Nexus_Devel/9   9/25/07 3:07p erickson
 * PR34925: update
 *
 * Nexus_Devel/8   9/25/07 1:19p erickson
 * PR34925: update
 *
 * Nexus_Devel/7   9/24/07 11:16a erickson
 * PR34925: update
 *
 * Nexus_Devel/6   9/24/07 10:09a erickson
 * PR34925: update
 *
 * Nexus_Devel/5   9/21/07 11:16a erickson
 * PR34925: update
 *
 * Nexus_Devel/4   9/14/07 4:59p erickson
 * PR34925: update
 *
 * Nexus_Devel/3   9/14/07 4:32p erickson
 * PR34925: added BDBG_MODULE
 *
 * Nexus_Devel/2   9/14/07 3:27p erickson
 * PR34925: initial compilation
 *
 * Nexus_Devel/1   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#if NEXUS_SW_RAVE_PEEK_EXTENSION
/* The NEXUS_SW_RAVE_PEEK_EXTENSION forces SW RAVE in ePointersOnly mode.
This is only supported for codecs that don't require SW RAVE for ITB manipulation (e.g. MPEG, AVC).
By not copying ITB, it requires minimal memory.
It allows the extension to peek at the ITB before it goes to the decoder. */
#include "nexus_sw_rave_extension.h"
#endif

BDBG_MODULE(nexus_rave);

#define ITB_SIZE 16
#define NEXUS_RAVE_CONTEXT_MAP(RAVE) (((RAVE)->swRave.raveHandle && (RAVE)->swRave.enabled) ? &(RAVE)->swRave.xptContextMap : &(RAVE)->xptContextMap)

/* for SW rave, we need 100 msec. for numBytesOutput, 250 msec is enough (a 20Mbps stream and 1.5MB CDB will wrap every 800 msec) */
#define NEXUS_RAVE_TIMER_PERIOD(RAVE) ((RAVE)->swRave.enabled?100:250)

void NEXUS_Rave_GetDefaultOpenSettings_priv(NEXUS_RaveOpenSettings *pSettings)
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* RAVE only supports buffers aligned on 4-byte boundaries. This alignment is expressed as a power of 2. */
    pSettings->config.Cdb.Alignment = 2;
    pSettings->config.Itb.Alignment = 2;
    return;
}

NEXUS_RaveHandle NEXUS_Rave_Open_priv(const NEXUS_RaveOpenSettings *pSettings)
{
    NEXUS_RaveHandle rave = NULL;
    BERR_Code rc;
    unsigned i;

    NEXUS_ASSERT_MODULE();
    for (i=0;i<NEXUS_NUM_RAVE_CONTEXTS;i++) {
        /* rave[0] means we are hardcoded for only one RAVE "channel", but many "contexts" */
        if (!pTransport->rave[0].context[i].raveHandle) {
            rave = &pTransport->rave[0].context[i];
            break;
        }
    }
    if (!rave) {
        BDBG_ERR(("No more RAVE contexts"));
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    BDBG_OBJECT_SET(rave, NEXUS_Rave);
#define INVALID_PID_CHANNEL 0xFFFF
    rave->pidChannelIndex = INVALID_PID_CHANNEL;

    rc = BXPT_Rave_AllocContext(pTransport->rave[0].channel,
        pSettings->record?BXPT_RaveCx_eRecord:(pTransport->settings.secureHeap?BXPT_RaveCx_eAvR:BXPT_RaveCx_eAv),
        (BAVC_CdbItbConfig *)&pSettings->config, &rave->raveHandle);
    if (rc) {BERR_TRACE(rc); return NULL;}

    BDBG_ASSERT(rave->raveHandle);
    rc = BXPT_Rave_GetContextRegisters(rave->raveHandle, &rave->xptContextMap);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    /* because of XPT sw rave, we can't assume the HW index number. so, we derive it from the registers. */
    rave->index = NEXUS_RAVE_INDEX(rave->xptContextMap.CDB_Read);
    rave->swRave.index = -1;

#if NEXUS_SW_RAVE_SUPPORT
    if (pSettings->allocSoftRave) {
        BXPT_RaveSoftMode mode = BXPT_RaveSoftMode_eCopyItb;

#if NEXUS_SW_RAVE_PEEK_EXTENSION
        mode = BXPT_RaveSoftMode_ePointersOnly;
#endif
        rc = BXPT_Rave_AllocSoftContext(rave->raveHandle, mode, &rave->swRave.raveHandle);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
        rc = BXPT_Rave_GetContextRegisters(rave->swRave.raveHandle, &rave->swRave.xptContextMap);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
        rave->swRave.index = NEXUS_RAVE_INDEX(rave->swRave.xptContextMap.CDB_Read);

#if NEXUS_SW_RAVE_PEEK_EXTENSION
        rc = NEXUS_P_SwRaveExtension_Init(rave);
        if (rc) { BERR_TRACE(rc); goto error; }
#endif
    }
#endif

    return rave;

error:
    if (rave) {
        NEXUS_Rave_Close_priv(rave);
    }
    return NULL;
}

void NEXUS_Rave_Close_priv(NEXUS_RaveHandle rave)
{
    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    NEXUS_Rave_Disable_priv(rave);
    NEXUS_Rave_RemovePidChannel_priv(rave);

#if NEXUS_SW_RAVE_SUPPORT
    if (rave->swRave.raveHandle) {
#if NEXUS_SW_RAVE_PEEK_EXTENSION
        NEXUS_P_SwRaveExtension_Uninit(rave);
#endif
        BXPT_Rave_FreeContext(rave->swRave.raveHandle);
        rave->swRave.raveHandle = NULL;
    }
#endif
    if (rave->raveHandle) {
        BXPT_Rave_FreeContext(rave->raveHandle);
        rave->raveHandle = NULL;
    }
    BDBG_OBJECT_UNSET(rave, NEXUS_Rave);
}

void NEXUS_Rave_GetDefaultSettings_priv(NEXUS_RaveSettings *pSettings)
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

static void NEXUS_Rave_P_Timer(void *context)
{
    NEXUS_RaveHandle rave = context;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    rave->timer = NULL;

    if (rave->swRave.raveHandle && rave->swRave.enabled) {
#if NEXUS_SW_RAVE_PEEK_EXTENSION
        /* this extension allows for a custom SW RAVE algorithm */
        NEXUS_P_SwRaveExtension_Advance(rave);
#else
        BXPT_Rave_AdvanceSoftContext(rave->swRave.raveHandle);
#endif
    }
    if (rave->settings.numOutputBytesEnabled) {
        BAVC_XptContextMap *pXptContextMap = NEXUS_RAVE_CONTEXT_MAP(rave);
        uint32_t valid = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Valid);
        if (valid < rave->lastValid && rave->lastValid) {
            /* every time it wraps, we read WRAP-BASE. this is the total number of bytes in the CDB for the past wrap.
            see GetStatus where we add VALID-BASE to improve the result */
            unsigned n = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Wrap) - BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Base);
            rave->numOutputBytes += n;
            BDBG_MSG(("rave read %lld", rave->numOutputBytes));
        }
        rave->lastValid = valid;
    }

    rave->timer = NEXUS_ScheduleTimer(NEXUS_RAVE_TIMER_PERIOD(rave), NEXUS_Rave_P_Timer, rave);
    BDBG_ASSERT(rave->timer);
}

static void NEXUS_Rave_P_ConvertForES(NEXUS_TransportType originalTransportType, BAVC_StreamType streamType, BAVC_StreamType *raveStreamType )
{

    /* SW7405-5140: if audio is ES format. Nexus needs to tell XPT PI to
    * disable DISABLE_BEFORE_PES filter.  If DISABLE_BEFORE_PES filter is enabled,
    * the RAVE discard all ES data as RAVE looks for a valid PES startcode. */
    *raveStreamType = streamType;
    if ( originalTransportType == NEXUS_TransportType_eEs )
    {
        /* limit change to ES input only */
        *raveStreamType = BAVC_StreamType_eEs;
    }
}

NEXUS_Error NEXUS_Rave_ConfigureVideo_priv(NEXUS_RaveHandle rave,
    NEXUS_VideoCodec codec, const NEXUS_RaveSettings *pSettings)
{
    BERR_Code rc;
    BXPT_Rave_AvSettings AvCtxCfg;
    BAVC_StreamType streamType;
#if NEXUS_SW_RAVE_SUPPORT
    bool swRave;
    BXPT_RaveSoftMode sw_rave_mode = BXPT_RaveSoftMode_eCopyItb;
#endif

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    rc = NEXUS_P_TransportType_ToMagnum(pSettings->pidChannel->status.transportType, &streamType);
    if (rc) return BERR_TRACE(rc);

    /* get default state into structure */
    BXPT_Rave_GetAvConfig(rave->raveHandle, &AvCtxCfg);
    NEXUS_Rave_P_ConvertForES( pSettings->pidChannel->status.originalTransportType, streamType, &AvCtxCfg.InputFormat );
    AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;

    /* default all EsRange off, then allow codecs to turn on what's needed */
    AvCtxCfg.EsRanges[0].Enable = false;
    AvCtxCfg.EsRanges[1].Enable = false;
    AvCtxCfg.EsRanges[2].Enable = false;
    AvCtxCfg.EsRanges[3].Enable = false;

#if NEXUS_SW_RAVE_SUPPORT
    swRave = false;
    if (NEXUS_GetEnv("force_sw_rave")) {
        swRave = true;
    }
    switch(pSettings->pidChannel->status.originalTransportType) {
    case NEXUS_TransportType_eAvi: /* DivX */
    case NEXUS_TransportType_eAsf:
    case NEXUS_TransportType_eMkv:
        switch (codec) {
        case NEXUS_VideoCodec_eMpeg4Part2:
        case NEXUS_VideoCodec_eDivx311:
        case NEXUS_VideoCodec_eVc1SimpleMain:
            swRave = true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
#if NEXUS_SW_RAVE_PEEK_EXTENSION
    if (swRave) {
        /* if swRave is required by the codec, then NEXUS_SW_RAVE_PEEK_EXTENSION does not work. */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    /* force SW RAVE on for peeking */
    swRave = true;
    sw_rave_mode = BXPT_RaveSoftMode_ePointersOnly;
#endif
#endif

    switch (codec) {
    case NEXUS_VideoCodec_eH263:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eH263;
        AvCtxCfg.EsRanges[0].RangeHi = 0xFE; /* everything */
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = false;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_VideoCodec_eH264_Svc:
    case NEXUS_VideoCodec_eH264_Mvc:
    case NEXUS_VideoCodec_eH264:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAvcVideo;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        AvCtxCfg.EsRanges[0].RangeHi = 0xFE; /* everything */
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = false;
        AvCtxCfg.EsRanges[0].Enable = true;
        break;
    case NEXUS_VideoCodec_eVc1:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eVc1Video;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        AvCtxCfg.EsRanges[0].RangeHi = 0xFE; /* everything */
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = false;
        AvCtxCfg.EsRanges[0].Enable = true;
        break;
    case NEXUS_VideoCodec_eVc1SimpleMain:
        /* this allows PES entries through, nothing else. */
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eVC1SimpleMain;
        AvCtxCfg.StreamIdHi = 0xFD;
        AvCtxCfg.StreamIdLo = 0xBD;
#if NEXUS_SW_RAVE_SUPPORT
        sw_rave_mode = BXPT_RaveSoftMode_eVc1SimpleMain;
#endif
        break;
    case NEXUS_VideoCodec_eMpeg1:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpeg1Video;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        AvCtxCfg.EsRanges[0].RangeHi = 0xAF; /* MPEG slices */
        AvCtxCfg.EsRanges[0].RangeLo = 0x01;
        AvCtxCfg.EsRanges[0].RangeIsASlice = true;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.EsRanges[1].RangeHi = 0xBF; /* start of frame */
        AvCtxCfg.EsRanges[1].RangeLo = 0xB0;
        AvCtxCfg.EsRanges[1].RangeIsASlice = false;
        AvCtxCfg.EsRanges[1].Enable = true;
        AvCtxCfg.EsRanges[2].RangeHi = 0x0; /* start of picture */
        AvCtxCfg.EsRanges[2].RangeLo = 0x0;
        AvCtxCfg.EsRanges[2].RangeIsASlice = false;
        AvCtxCfg.EsRanges[2].Enable = true;
        break;
    case NEXUS_VideoCodec_eMpeg2:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpeg2Video;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        AvCtxCfg.EsRanges[0].RangeHi = 0xAF; /* MPEG slices */
        AvCtxCfg.EsRanges[0].RangeLo = 0x01;
        AvCtxCfg.EsRanges[0].RangeIsASlice = true;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.EsRanges[1].RangeHi = 0xBF; /* start of frame */
        AvCtxCfg.EsRanges[1].RangeLo = 0xB0;
        AvCtxCfg.EsRanges[1].RangeIsASlice = false;
        AvCtxCfg.EsRanges[1].Enable = true;
        AvCtxCfg.EsRanges[2].RangeHi = 0x0; /* start of picture */
        AvCtxCfg.EsRanges[2].RangeLo = 0x0;
        AvCtxCfg.EsRanges[2].RangeIsASlice = false;
        AvCtxCfg.EsRanges[2].Enable = true;
        break;
    case NEXUS_VideoCodec_eDivx311:
#if NEXUS_SW_RAVE_SUPPORT
        if (swRave) {
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpeg4Part2;
            AvCtxCfg.StreamIdHi = 0xEF;
            AvCtxCfg.StreamIdLo = 0xE0;
            sw_rave_mode = BXPT_RaveSoftMode_eDivX_311;
            break;
        }
#endif
        /* if not SW rave, then fall through as Mpeg4Part2 */

    case NEXUS_VideoCodec_eMpeg4Part2:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpeg4Part2;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        AvCtxCfg.EsRanges[0].RangeHi = 0x2F;
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = true;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.EsRanges[1].RangeHi = 0xB8;
        AvCtxCfg.EsRanges[1].RangeLo = 0xB0;
        AvCtxCfg.EsRanges[1].RangeIsASlice = false;
        AvCtxCfg.EsRanges[1].Enable = true;
#if NEXUS_SW_RAVE_SUPPORT
        sw_rave_mode = BXPT_RaveSoftMode_eDivX;
        if (pSettings->disableReordering) {
            sw_rave_mode = BXPT_RaveSoftMode_eDivX_noReorder;
        }
#endif
        break;
    case NEXUS_VideoCodec_eAvs:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAvsVideo;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        AvCtxCfg.EsRanges[0].RangeHi = 0xAF; /* AVS slices */
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = true;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.EsRanges[1].RangeHi = 0xBF; /* start of frame and pictures */
        AvCtxCfg.EsRanges[1].RangeLo = 0xB0;
        AvCtxCfg.EsRanges[1].RangeIsASlice = false;
        AvCtxCfg.EsRanges[1].Enable = true;
        break;
	case NEXUS_VideoCodec_eVp6:
#if NEXUS_BDSP_VP6_SUPPORT /* If not, AVD can decode VP6 */
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eVp6Video;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        AvCtxCfg.EsRanges[0].RangeHi = 0xFE; /* everything */
        AvCtxCfg.EsRanges[0].RangeLo = 0x00;
        AvCtxCfg.EsRanges[0].RangeIsASlice = false;
        AvCtxCfg.EsRanges[0].Enable = true;
        AvCtxCfg.EsRanges[1] = AvCtxCfg.EsRanges[0];
        AvCtxCfg.EsRanges[2] = AvCtxCfg.EsRanges[0];
        AvCtxCfg.EsRanges[3] = AvCtxCfg.EsRanges[0];
        break;
#endif
    case NEXUS_VideoCodec_eRv40:
    case NEXUS_VideoCodec_eVp8:
    case NEXUS_VideoCodec_eMotionJpeg:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eVC1SimpleMain;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xE0;
        break;
    case NEXUS_VideoCodec_eSpark:
        if (pSettings->pidChannel->status.originalTransportType==NEXUS_TransportType_eEs) {
            /* for spark ES, we must do H263-like ES indexing */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eH263;
            AvCtxCfg.EsRanges[0].RangeHi = 0xFE; /* everything */
            AvCtxCfg.EsRanges[0].RangeLo = 0x00;
            AvCtxCfg.EsRanges[0].RangeIsASlice = false;
            AvCtxCfg.EsRanges[0].Enable = true;
            AvCtxCfg.StreamIdHi = 0xEF;
            AvCtxCfg.StreamIdLo = 0xBD;
        }
        else {
            /* otherwise, we send transport a PES with BCMV headers and no ES indexing is needed */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eVC1SimpleMain;
            AvCtxCfg.StreamIdHi = 0xEF;
            AvCtxCfg.StreamIdLo = 0xE0;
        }
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    BDBG_CASSERT(BXPT_HAS_PID_CHANNEL_PES_FILTERING);

#if NEXUS_SW_RAVE_SUPPORT
    /* coverity[dead_error_condition] */
    if (swRave) {
        if (!rave->swRave.raveHandle) {
            BDBG_ERR(("This video codec requires that you set allocSoftRave for ITB transformation."));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        rc = BXPT_Rave_ResetSoftContext(rave->swRave.raveHandle, sw_rave_mode);
        if (rc) return BERR_TRACE(rc);
        rave->swRave.enabled = true;
    }
    else {
        rave->swRave.enabled = false;
    }
#endif

    AvCtxCfg.BandHoldEn = pSettings->bandHold;
    /* unconditionally disable CC check. we already have CC check in parserband and pidchannel. if someone must have CC check in RAVE, we
    should expose decoder API to make it conditional, like NEXUS_ParserBandSettings.continuityCountEnabled */
    AvCtxCfg.DisableContinuityCheck = true;
#if BXPT_HAS_BPP_SEARCH
    AvCtxCfg.EnableBPPSearch = true; /* allows LAST BPP command to flow to ITB. if feature works, no reason to expose knob to turn off. */
#endif

    BDBG_MSG(("RAVE_CX%d configured for video codec %d, pid channel %d", rave->index, codec, pSettings->pidChannel->status.pidChannelIndex));

    rc = BXPT_Rave_AddPidChannel(rave->raveHandle, pSettings->pidChannel->status.pidChannelIndex, false);
    if (rc) {return BERR_TRACE(rc);}

    rave->pidChannelIndex = pSettings->pidChannel->status.pidChannelIndex; /* remember for the remove */

    if(pSettings->otfPvr) {
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eOTFVideo;
    }

    rc = BXPT_Rave_SetAvConfig(rave->raveHandle, &AvCtxCfg);
    if (rc) {return BERR_TRACE(rc);}

    /* NOTE: do not flush rave here. if playback has already started, then we could lose a small
    amount of data. rave should already be in a flushed state, either from initial conditions
    or from a flush after stopping decode. */
    rave->settings = *pSettings;
    rave->lastValid = 0;

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Rave_ConfigureAudio_priv(
    NEXUS_RaveHandle rave,
    NEXUS_AudioCodec codec,
    const NEXUS_RaveSettings *pSettings
    )
{
    BERR_Code rc;
    BXPT_Rave_AvSettings AvCtxCfg;
    BAVC_StreamType streamType;
    NEXUS_PidChannelStatus pidChannelStatus;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    rc = NEXUS_P_TransportType_ToMagnum(pSettings->pidChannel->status.transportType, &streamType);
    if (rc) return BERR_TRACE(rc);

    /* There are 4 types of DSS A/V streams:
    DSS SD video - this is DSS ES
    DSS HD video - this is DSS PES
    DSS MPEG audio - this actually uses MPEG1 system headers, but it's very similar to PES, therefore DSS PES
    Therefore we convert DSS ES to DSS PES here.
    DSS AC3 audio - uses MPEG2 System PES, therefore DSS PES
    */
    if (streamType == BAVC_StreamType_eDssEs)
    {
        streamType = BAVC_StreamType_eDssPes;
    }

    BXPT_Rave_GetAvConfig(rave->raveHandle, &AvCtxCfg);
    NEXUS_Rave_P_ConvertForES( pSettings->pidChannel->status.originalTransportType, streamType, &AvCtxCfg.InputFormat );
    AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;

#if BCHP_CHIP == 7400 || BCHP_CHIP == 7118 || BCHP_CHIP == 7401 || BCHP_CHIP == 7403
    /* Old FW arch only supports AD on MPEG */
    if ( pSettings->audioDescriptor && (codec != NEXUS_AudioCodec_eMpeg && codec != NEXUS_AudioCodec_eMp3) )
    {
        BDBG_ERR(("Audio descriptors are only supported with MPEG audio currently"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
#endif

    /* TODO: bsettop_p_rap_set_rave_thresholds(rap, mpeg->audio[program].format, cfg->playback, rave->raveHandle, &AvCtxCfg); */
    BKNI_Memset(AvCtxCfg.EsRanges, 0, sizeof(AvCtxCfg.EsRanges)); /* all disabled */
    switch (codec) {
    case NEXUS_AudioCodec_eMpeg:
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
        }
        else
        {
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudio;
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eMp3:
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
        }
        else
        {
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudioLayer3;
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eAacAdts:
    case NEXUS_AudioCodec_eAacLoas:
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAacAudio;
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eAacPlusLoas:
    case NEXUS_AudioCodec_eAacPlusAdts:
        /* baudio_format_aac_plus_loas is also handled here as it has the same value as baudio_format_aac_plus */
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAacHe;
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eDra:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eDra;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_AudioCodec_ePcm: /* PCM audio in PES uses the same BCMA header/encapsulation */
    case NEXUS_AudioCodec_ePcmWav:
    case NEXUS_AudioCodec_eWmaStd:
    case NEXUS_AudioCodec_eWmaStdTs:
    case NEXUS_AudioCodec_eWmaPro:
    case NEXUS_AudioCodec_eAmr:
    case NEXUS_AudioCodec_eAdpcm:
    case NEXUS_AudioCodec_eVorbis:
    case NEXUS_AudioCodec_eG711:
    case NEXUS_AudioCodec_eG726:
    case NEXUS_AudioCodec_eG729:
    case NEXUS_AudioCodec_eG723_1:
    case NEXUS_AudioCodec_eFlac:
    case NEXUS_AudioCodec_eApe:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eWma;
        AvCtxCfg.StreamIdHi = 0xEF; /* PES stream id 0xCD is set by playpump ASF->PES packetizer.
                                    but allow a greater range. */
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eCook:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAmr;
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xC0;
        break;
    case NEXUS_AudioCodec_eAc3:
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAc3Plus; /* Use AC3+ ITB type for AC3 */
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_AudioCodec_eAc3Plus:
        if ( pSettings->audioDescriptor )
        {
            /* Currently there is no other way to specify this in RAVE */
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
        }
        else
        {
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAc3Plus;
        }
        AvCtxCfg.StreamIdHi = 0xEF;
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_AudioCodec_eLpcmDvd:    
    case NEXUS_AudioCodec_eLpcm1394:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eLpcmAudio;
        AvCtxCfg.StreamIdHi = 0xEF; /* TBD */
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_AudioCodec_eLpcmBluRay:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eLpcmAudio;
        AvCtxCfg.StreamIdHi = 0xFF;
        AvCtxCfg.StreamIdLo = 0xB0;
        break;
    case NEXUS_AudioCodec_eDts:
    case NEXUS_AudioCodec_eDtsHd:
    case NEXUS_AudioCodec_eDtsLegacy:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eDtsAudio;
        AvCtxCfg.StreamIdHi = 0xEF; /* TBD */
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    case NEXUS_AudioCodec_eMlp:
        AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMlpAudio;
        AvCtxCfg.StreamIdHi = 0xEF; /* TBD */
        AvCtxCfg.StreamIdLo = 0xBD;
        break;
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    rc = NEXUS_PidChannel_GetStatus(pSettings->pidChannel, &pidChannelStatus);
    if (rc) {return BERR_TRACE(rc);}

    {
        BXPT_Rave_ContextSettings CtxConfig;
        BXPT_Rave_GetContextConfig(rave->raveHandle, &CtxConfig);

        CtxConfig.PesSidExtMode = BXPT_Rave_PesSidExtNormal;

        /* this is the default */
        CtxConfig.EnablePrivateHdrItbEntry = false;
        CtxConfig.AudFrameInfo = 0;

        if ( pidChannelStatus.originalTransportType == NEXUS_TransportType_eVob )
        {
            switch ( AvCtxCfg.ItbFormat )
            {
            case BAVC_ItbEsType_eLpcmAudio:
                CtxConfig.EnablePrivateHdrItbEntry = true;
                CtxConfig.AudFrameInfo = 7;
                break;
            case BAVC_ItbEsType_eAc3gAudio:
            case BAVC_ItbEsType_eAc3Plus:
            case BAVC_ItbEsType_eDtsAudio:
                CtxConfig.EnablePrivateHdrItbEntry = false;
                CtxConfig.AudFrameInfo = 4;
                break;
            default:
                break;
            }
        }
        else if ( pidChannelStatus.originalTransportType == NEXUS_TransportType_eTs )
        {
            switch ( codec )
            {
            case NEXUS_AudioCodec_eLpcmBluRay:
            case NEXUS_AudioCodec_eLpcm1394:
                /* 1394 LPCM and BD-LPCM are very similar to DVD-LPCM over MPEG2-TS.  They have a 4-byte header instead of 7 however. */
                CtxConfig.EnablePrivateHdrItbEntry = true;
                CtxConfig.AudFrameInfo = 4;
                break;
            default:
                break;
            }
            if(pSettings->pidChannel->combinedPid&0xFF0000) {
                CtxConfig.PesSidExtMode = BXPT_Rave_PesSidExtIndependent;
                CtxConfig.PesExtSearchMode = BXPT_Rave_PesExtSearchSpecial;
                CtxConfig.SidExtIndependent = pSettings->pidChannel->combinedPid>>16;

                if(codec == NEXUS_AudioCodec_eAc3Plus) {
                    /* AC3 plus needs both base and extansion data */
                    CtxConfig.PesSidExtMode = BXPT_Rave_PesSidExtKeepAll;
                    CtxConfig.SidExtDependent = CtxConfig.SidExtIndependent;
                    CtxConfig.SidExtIndependent --;
                }
            }
        }
        rc = BXPT_Rave_SetContextConfig(rave->raveHandle, &CtxConfig);
        if (rc) {return BERR_TRACE(rc);}
    }

    AvCtxCfg.BandHoldEn = pSettings->bandHold;
    /* unconditionally disable CC check. we already have CC check in parserband and pidchannel. if someone must have CC check in RAVE, we
    should expose decoder API to make it conditional, like NEXUS_ParserBandSettings.continuityCountEnabled */
    AvCtxCfg.DisableContinuityCheck = true;

    BDBG_MSG(("RAVE_CX%d configured for audio codec %d, pid channel %d", rave->index, codec, pSettings->pidChannel->status.pidChannelIndex));

    rc = BXPT_Rave_AddPidChannel(rave->raveHandle, pSettings->pidChannel->status.pidChannelIndex, pTransport->settings.secureHeap != NULL);
    if (rc) {return BERR_TRACE(rc);}

    rave->pidChannelIndex = pSettings->pidChannel->status.pidChannelIndex; /* remember for the remove */

    rc = BXPT_Rave_SetAvConfig(rave->raveHandle, &AvCtxCfg);
    if (rc) {return BERR_TRACE(rc);}

    /* NOTE: do not flush rave here. if playback has already started, then we could lose a small
    amount of data. rave should already be in a flushed state, either from initial conditions
    or from a flush after stopping decode. */
    rave->settings = *pSettings;
    rave->lastValid = 0;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Rave_ConfigureAll_priv(NEXUS_RaveHandle rave, const NEXUS_RaveSettings *pSettings)
{
    BERR_Code rc;
    BXPT_Rave_AvSettings AvCtxCfg;
    BAVC_StreamType streamType;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    rc = NEXUS_P_TransportType_ToMagnum(pSettings->pidChannel->status.transportType, &streamType);
    if (rc) return BERR_TRACE(rc);

    /* get default state into structure */
    BXPT_Rave_GetAvConfig(rave->raveHandle, &AvCtxCfg);
    NEXUS_Rave_P_ConvertForES( pSettings->pidChannel->status.originalTransportType, streamType, &AvCtxCfg.InputFormat );
    AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;

    /* default all EsRange off, then allow codecs to turn on what's needed */
    AvCtxCfg.EsRanges[0].Enable = false;
    AvCtxCfg.EsRanges[1].Enable = false;
    AvCtxCfg.EsRanges[2].Enable = false;
    AvCtxCfg.EsRanges[3].Enable = false;
    AvCtxCfg.StreamIdHi = 0xEF;
    AvCtxCfg.StreamIdLo = 0xBD; /* TODO: 0x00..0xFF? */
    AvCtxCfg.BandHoldEn = pSettings->bandHold;
    /* unconditionally disable CC check. we already have CC check in parserband and pidchannel. if someone must have CC check in RAVE, we
    should expose decoder API to make it conditional, like NEXUS_ParserBandSettings.continuityCountEnabled */
    AvCtxCfg.DisableContinuityCheck = true;

    BDBG_MSG(("RAVE_CX%d configured for data, pid channel %d", rave->index, pSettings->pidChannel->status.pidChannelIndex));

    rc = BXPT_Rave_AddPidChannel(rave->raveHandle, pSettings->pidChannel->status.pidChannelIndex, false);
    if (rc) {return BERR_TRACE(rc);}

    rave->pidChannelIndex = pSettings->pidChannel->status.pidChannelIndex; /* remember for the remove */

    rc = BXPT_Rave_SetAvConfig(rave->raveHandle, &AvCtxCfg);
    if (rc) {return BERR_TRACE(rc);}
    rave->settings = *pSettings;
    rave->lastValid = 0;

    return BERR_SUCCESS;
}

void NEXUS_Rave_RemovePidChannel_priv(NEXUS_RaveHandle rave)
{
    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    if (rave->pidChannelIndex != INVALID_PID_CHANNEL) {
        BERR_Code rc;
        rc = BXPT_Rave_RemovePidChannel(rave->raveHandle, rave->pidChannelIndex);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
        rave->pidChannelIndex = INVALID_PID_CHANNEL;
    }
    return;
}

void NEXUS_Rave_Enable_priv(NEXUS_RaveHandle rave)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    if (rave->swRave.enabled || rave->settings.numOutputBytesEnabled) {
        if (!rave->timer) {
            rave->timer = NEXUS_ScheduleTimer(NEXUS_RAVE_TIMER_PERIOD(rave), NEXUS_Rave_P_Timer, rave);
            BDBG_ASSERT(rave->timer);
        }
    }

    rc = BXPT_Rave_EnableContext(rave->raveHandle);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    rave->enabled = true;
}

void NEXUS_Rave_Disable_priv(NEXUS_RaveHandle rave)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    if (rave->timer) {
        NEXUS_CancelTimer(rave->timer);
        rave->timer = NULL;
    }

    rc=BXPT_Rave_DisableContext(rave->raveHandle);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    rave->enabled = false;
    return;
}

void NEXUS_Rave_Flush_priv(NEXUS_RaveHandle rave)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    rave->lastValid = 0;
    rc = BXPT_Rave_FlushContext(rave->raveHandle);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}

#if NEXUS_SW_RAVE_SUPPORT
    if (rave->swRave.raveHandle) {
        rc = BXPT_Rave_FlushContext(rave->swRave.raveHandle);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    }
#endif
}

NEXUS_Error NEXUS_Rave_GetStatus_priv(NEXUS_RaveHandle rave, NEXUS_RaveStatus *pStatus)
{
    BAVC_XptContextMap *pXptContextMap;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->index = rave->index;
    pStatus->swRaveIndex = rave->swRave.index;
#if NEXUS_SW_RAVE_SUPPORT
    if (rave->swRave.raveHandle && rave->swRave.enabled) {
        pStatus->xptContextMap = rave->swRave.xptContextMap;
    }
    else
#endif
    {
        pStatus->xptContextMap = rave->xptContextMap;
    }

    pStatus->numOutputBytes = rave->numOutputBytes;
    /* augment with VALID-BASE. this is useful for low bitrate streams that are just starting decode. */
    pXptContextMap = NEXUS_RAVE_CONTEXT_MAP(rave);
    pStatus->numOutputBytes += BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Valid) - BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Base);
    pStatus->enabled = rave->enabled;
    pStatus->heap = g_pCoreHandles->nexusHeap[pTransport->settings.mainHeapIndex];

    return NEXUS_SUCCESS;
}

void NEXUS_Rave_GetCdbBufferInfo_isr(NEXUS_RaveHandle rave, unsigned *depth, unsigned *size)
{
    BERR_Code rc;
    BXPT_Rave_BufferInfo buffer_info;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    BKNI_ASSERT_ISR_CONTEXT();

    rc = BXPT_Rave_GetBufferInfo_isr(rave->raveHandle, &buffer_info);
    if(rc==BERR_SUCCESS) {
        *depth = buffer_info.CdbDepth;
        *size = buffer_info.CdbSize;
    } else {
        rc = BERR_TRACE(rc);
        *depth=0;
        *size=0;
    }
    return;
}

void NEXUS_Rave_GetItbBufferInfo_isr(NEXUS_RaveHandle rave, unsigned *depth, unsigned *size)
{
    BERR_Code rc;
    uint32_t base;
    uint32_t end;
    BXPT_Rave_ContextPtrs CtxPtrs;
    BAVC_XptContextMap * pXptContextMap;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    BKNI_ASSERT_ISR_CONTEXT();

    rc = BXPT_Rave_CheckBuffer(rave->raveHandle, &CtxPtrs );
    if(rc==BERR_SUCCESS) {
        *depth = CtxPtrs.Itb.ByteCount + CtxPtrs.Itb.WrapByteCount;
    } else {
        rc = BERR_TRACE(rc);
        *depth=0;
    }

    pXptContextMap = NEXUS_RAVE_CONTEXT_MAP(rave);
    base = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_Base);
    end = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_End);
    *size = end - base + 1;
    
    return;
}

#define ITB_START_CODE 0x00
typedef struct NEXUS_Rave_P_ItbEntry {
    uint32_t word[ITB_SIZE/sizeof(uint32_t)];
} NEXUS_Rave_P_ItbEntry;

static BERR_Code NEXUS_Rave_P_ScanItb(NEXUS_RaveHandle rave, bool (*one_itb)(void *, const NEXUS_Rave_P_ItbEntry *), void *context)
{
    BXPT_Rave_ContextPtrs CtxPtrs;
    BXPT_RaveCx_Handle raveHandle;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    raveHandle = rave->raveHandle;
#if NEXUS_SW_RAVE_SUPPORT
    if (rave->swRave.enabled) {
        raveHandle = rave->swRave.raveHandle;
    }
#endif
    if(raveHandle==NULL) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    rc = BXPT_Rave_CheckBuffer(rave->raveHandle, &CtxPtrs );
    if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc); }
    /* check for an empty or reset buffer. super large ByteCount can happen because of reset */
    if ( CtxPtrs.Itb.DataPtr && CtxPtrs.Itb.ByteCount ) {
        const NEXUS_Rave_P_ItbEntry *entry;
        unsigned itbCount;
        
        entry = (void *)CtxPtrs.Itb.DataPtr;
        for(itbCount = CtxPtrs.Itb.ByteCount/ITB_SIZE;itbCount;itbCount--,entry++) {
            if(one_itb(context, entry)) {
                return NEXUS_SUCCESS;
            }
        }
        entry = (void *)CtxPtrs.Itb.WrapDataPtr;
        for(itbCount = CtxPtrs.Itb.WrapByteCount/ITB_SIZE;itbCount;itbCount--,entry++) {
            if(one_itb(context, entry)) {
                return NEXUS_SUCCESS;
            }
        }
    }
    return NEXUS_NOT_AVAILABLE; /* no BERR_TRACE */
}

/* 
Summary:
Returns bit 'b' from word 'w', 

Example:
	B_GET_BIT(0xDE,1)==0 
	B_GET_BIT(0xDE,7)!=0 
*/
#define B_GET_BIT(w,b)  ((w)&(1<<(b)))

/* 
Summary:
Returns bits 'e'..'b' from word 'w', 

Example:
	B_GET_BITS(0xDE,7,4)==0x0D
	B_GET_BITS(0xDE,3,0)=0x0E 
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

typedef enum brave_itb_types {
    brave_itb_extracted_data    =    0x00,   
    brave_itb_base_address =         0x20,
    brave_itb_pts_dts =              0x21,
    brave_itb_pcr_offset =           0x22,  
    brave_itb_btp =                  0x23,
    brave_itb_private_hdr =          0x24,
    brave_itb_rts =                  0x25,
    brave_itb_pcr =                  0x26,
    brave_itb_ip_stream_out =        0x30,
    brave_itb_termination =          0x70
} brave_itb_types;



static bool NEXUS_Rave_P_ScanItb_Pts(void *context, const NEXUS_Rave_P_ItbEntry *itb)
{
    const uint32_t *pts = context;
    unsigned type = B_GET_BITS(itb->word[0], 31, 24);

    if(type==brave_itb_pts_dts) {
        if( *pts == itb->word[1]) {
            return true;
        }
    }
    return false;
}

bool NEXUS_Rave_FindPts_priv(NEXUS_RaveHandle rave, uint32_t pts)
{
    NEXUS_Error rc;

    rc = NEXUS_Rave_P_ScanItb(rave, NEXUS_Rave_P_ScanItb_Pts, &pts);
    return rc==NEXUS_SUCCESS;
}

bool NEXUS_Rave_FindVideoStartCode_priv(NEXUS_RaveHandle rave, uint8_t startCode)
{
    uint8_t *ItbByte,i;
    BXPT_Rave_ContextPtrs CtxPtrs;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

#if NEXUS_SW_RAVE_SUPPORT
    if (rave->swRave.raveHandle && rave->swRave.enabled) {
        rc = BXPT_Rave_CheckBuffer(rave->swRave.raveHandle, &CtxPtrs );
        if (rc) {rc = BERR_TRACE(rc); return false;}
    }
    else
#endif
    {
    rc = BXPT_Rave_CheckBuffer(rave->raveHandle, &CtxPtrs );
    if (rc) {rc = BERR_TRACE(rc); return false;}
    }

    /* check for an empty or reset buffer. super large ByteCount can happen because of reset */
    if ( !CtxPtrs.Itb.DataPtr || CtxPtrs.Itb.ByteCount < ITB_SIZE || CtxPtrs.Itb.ByteCount > 0x1000000) {
        return false;
    }

    /*
    ** If the entire pic is in the CDB, there will be a startcode at the end of the ITB.
    ** Each ITB entry is ITB_SIZE bytes, so back up to the start of the last entry.
    */
    ItbByte = CtxPtrs.Itb.DataPtr + ( CtxPtrs.Itb.ByteCount - ITB_SIZE );

#if BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE
    /* detect valid video start code entry */
    if(ItbByte[3] != ITB_START_CODE) {
        return false;
    }
    for(i=0;i<ITB_SIZE;i++)
    {
        /* ignore offset byte */
        if(i%2) {
            if(ItbByte[i]==startCode)
                return true;
        }
    }
#else
    if(ItbByte[0] != ITB_START_CODE)
        return false;
    for(i=0;i<ITB_SIZE;i++)
    {
        /* ignore offset byte */
        if(i%2)
            continue;
        if(ItbByte[i]==startCode)
            return true;
    }
#endif
    return false;
}

void NEXUS_Rave_GetAudioFrameCount_priv(
    NEXUS_RaveHandle rave,
    unsigned *pFrameCount
    )
{
    BXPT_Rave_ContextPtrs ptrs;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(NULL != pFrameCount);

    *pFrameCount = 0;
    rc = BXPT_Rave_CheckBuffer(rave->raveHandle, &ptrs);
    if (rc == BERR_SUCCESS) {
        *pFrameCount = (ptrs.Itb.ByteCount+ptrs.Itb.WrapByteCount) / ITB_SIZE;
    }
}

NEXUS_Error NEXUS_Rave_SetCdbThreshold_priv(
    NEXUS_RaveHandle rave,
    unsigned cdbDepth       /* CDB threshold in bytes (0 is default) */
    )
{
    BERR_Code errCode;
    BXPT_Rave_ContextThresholds thresholds;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    errCode = BXPT_Rave_GetDefaultThresholds(rave->raveHandle, &thresholds);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( cdbDepth > 0 )
    {
        size_t cdbUpper = (cdbDepth + 255)/256; /* Specified in units of 256 bytes */
        /* Only permit values less than default */
        if ( cdbUpper < thresholds.CdbUpper )
        {
            thresholds.CdbUpper = cdbUpper;
        }
    }

    errCode = BXPT_Rave_SetThresholds(rave->raveHandle, &thresholds);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

void NEXUS_Rave_GetCdbPointers_isr( NEXUS_RaveHandle rave, uint32_t *validPointer, uint32_t *readPointer)
{
    BAVC_XptContextMap *pXptContextMap;

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    BKNI_ASSERT_ISR_CONTEXT();

    pXptContextMap = NEXUS_RAVE_CONTEXT_MAP(rave);
    *validPointer = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Valid);
    *readPointer = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->CDB_Read);
}

NEXUS_Error NEXUS_Rave_GetMostRecentPts_priv( NEXUS_RaveHandle rave, uint32_t *pPts )
{
    BAVC_XptContextMap *pXptContextMap;
    uint32_t valid_offset, read_offset, base_offset, end_offset;
    uint32_t *valid, *read, *base, *end, *itb_mem;
    void *temp;
    NEXUS_Error rc;
    unsigned itr = 0; /* debug stats */

    BDBG_OBJECT_ASSERT(rave, NEXUS_Rave);
    NEXUS_ASSERT_MODULE();

    pXptContextMap = NEXUS_RAVE_CONTEXT_MAP(rave);
    valid_offset = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_Valid);
    valid_offset -= valid_offset % ITB_SIZE; /* VALID points to last byte in ITB entry, move to first byte for easy algo */
    read_offset = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_Read);
    base_offset = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_Base);
    end_offset = BREG_Read32(g_pCoreHandles->reg, pXptContextMap->ITB_End);
    end_offset -= end_offset % ITB_SIZE; /* END points to last byte in ITB entry, move to first byte for easy algo */

    /* validate that we're in range */
    if (!base_offset || !end_offset || valid_offset < base_offset || read_offset < base_offset) {
        return NEXUS_UNKNOWN;
    }

    rc = BMEM_ConvertOffsetToAddress( g_pCoreHandles->heap[0], valid_offset, &temp);
    if (rc) return rc;
    valid = (uint32_t *)temp;
    rc = BMEM_ConvertOffsetToAddress( g_pCoreHandles->heap[0], read_offset, &temp);
    if (rc) return rc;
    read = (uint32_t *)temp;
    rc = BMEM_ConvertOffsetToAddress( g_pCoreHandles->heap[0], base_offset, &temp);
    if (rc) return rc;
    base = (uint32_t *)temp;
    rc = BMEM_ConvertOffsetToAddress( g_pCoreHandles->heap[0], end_offset, &temp);
    if (rc) return rc;
    end = (uint32_t *)temp;

    itb_mem = valid;
    do {
        int itb_type = (itb_mem[0]>>24) & 0xFF;
        /* 0x21 for unified ITB, 0x2 for legacy ITB */
        if (itb_type == 2 || itb_type == 0x21) {
            /* PTS found */
            *pPts = itb_mem[1];
            BDBG_MSG(("MostRecentPts pts %#x, %d itrs", *pPts, itr));
            return 0;
        }
        if (itb_mem == read) { /* if valid == read, we have just processed the last element. */
            break;
        }

        itb_mem -= ITB_SIZE/sizeof(itb_mem[0]); /* walk backwards */
        itr++;

        if (itb_mem < base) {
            itb_mem = end; /* end points to the last valid */
        }
    } while (itb_mem != valid); /* this should not occur if read pointer is in range */

    BDBG_MSG(("No MostRecentPts found, %d itrs", itr));
    return NEXUS_UNKNOWN;
}

