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
 * $brcm_Workfile: nexus_demux.c $
 * $brcm_Revision: 83 $
 * $brcm_Date: 10/16/12 9:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_demux.c $
 * 
 * 83   10/16/12 9:50a erickson
 * SW7435-411: open allpass capable pid channels last
 * 
 * 82   10/11/12 12:06p vsilyaev
 * SW7425-3955: Use upper 8 bits as PES sub-id for MPEG-2 TS streams
 * 
 * 81   10/4/12 5:47p erickson
 * SW7435-391: redo NEXUS_PidChannelStatus.parserBand to return HW parser
 *  band index
 * 
 * 80   8/3/12 4:44p vsilyaev
 * SW7420-2330: NEXUS_OBJECT_CALSS_MAKE should be used _after_
 *  protype/implementaion for the finalizer function
 * 
 * 79   8/2/12 10:39a erickson
 * SW7429-227: reduce use of #if NEXUS_NUM_PARSER_BANDS
 * 
 * 78   8/2/12 10:37a erickson
 * SW7429-227: pidchannel must inc parserband refcnt
 * 
 * 77   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 76   7/6/12 11:40a erickson
 * SW7552-292: apply cont_count_ignore debug env variable to per-pid cc
 *  check
 * 
 * 75   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 74   6/5/12 11:48a erickson
 * SW7340-296: call nexus_unregister only if
 *  NEXUS_Playpump_ClosePidChannel is called
 * 
 * 73   5/31/12 4:29p vsilyaev
 * SW7420-2311: Added reference counting for pid channel
 * 
 * 72   2/21/12 6:34p bandrews
 * SW7231-391: fix derefernce of handle instead of band
 * 
 * 71   2/21/12 5:31p bandrews
 * SW7231-391: added valid status.parserBand back
 * 
 * 70   2/10/12 2:40p gmullen
 * SW7346-679: Merged fix to main
 * 
 * SW7346-679/1   2/10/12 2:32p gmullen
 * SW7346-679: Disable DSS HD filtering before setting PID channel config
 * 
 * 69   2/1/12 5:13p hongtaoz
 * SW7425-2320: fixed PID remapping with message pid2buf and with
 *  duplicated PID channel;
 * 
 * 68   12/9/11 11:13a gmullen
 * SW7425-1871: Add build checks for parts that don't have message
 *  filtering hw
 * 
 * 67   12/5/11 10:21a erickson
 * SW7425-1871: use BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS
 * 
 * 66   11/23/11 3:54p nickh
 * SW7420-2143: Fix audio issue when re-inserting cable card
 * 
 * 65   11/17/11 12:19p nickh
 * SW7420-2143: Fix cable card crash when all pass filtering
 *
 * 64   11/4/11 11:48a erickson
 * SW7231-391: add ERR
 *
 * 63   10/31/11 7:54p bandrews
 * SW7231-391: cleanup from merge to main
 *
 * 62   10/31/11 7:47p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 *
 * 61   10/17/11 11:38a gmullen
 * SW7425-1383:  Merged to main
 *
 * 60   10/13/11 3:32p erickson
 * SW7420-1120: improve code for platforms without parser bands
 *
 * SW7425-1383/2   10/14/11 4:09p gmullen
 * SW7425-1383: Updated per David's suggestions
 *
 * SW7425-1383/1   10/11/11 3:45p gmullen
 * SW7425-1383: Added API to return the chip-specific allPass PID channel
 *  number
 *
 * 59   9/29/11 9:52a erickson
 * SW7358-150: refine comparison NEXUS_PidChannelSettings when opening
 *  duplicate pids to only those settings which must match
 *
 * 58   9/29/11 9:45a erickson
 * SW7358-141: "and" the enable flags for all SW pid channel handles that
 *  share the same HW pid channel.
 *
 * 57   9/6/11 3:53p dlwin
 * SW7344-192: Change for Coverity: 35340
 *
 * 56   9/6/11 10:59a jtna
 * SW7425-1215: add const to NEXUS_PidChannel_SetRemapSettings()
 *
 * 55   9/2/11 2:54p jtna
 * SW7425-1215: add NEXUS_PidChannel_SetRemapSettings() and related
 *  changes
 *
 * 54   9/2/11 12:40p erickson
 * SW7420-1995: unregister handles from objdb when doing automatic close
 *
 * 53   7/1/11 10:16a erickson
 * SW7425-725: disable SPID when opening a pid channel
 *
 * 52   6/16/11 4:17p erickson
 * SW7425-725: clear pid remap when closing pid channel
 *
 * 51   3/10/11 8:59a gmohile
 * SW7422-101 : Fix build error for platform without parser band
 *
 * 50   3/8/11 6:08p vsilyaev
 * SW7422-101: Updated logic of handling continuityCountEnabled, so per
 *  band and per pid settings are combined with logical AND
 *
 * 49   2/11/11 10:23a gmohile
 * SW7420-1120 : Fix platform with no parser band
 *
 * 48   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 47   1/28/11 8:55a erickson
 * SW7420-1440: add internal hooks so that NEXUS_PidChannel_Close can
 *  close playpump pidchannels (but not playback pidchannels)
 *
 * 46   1/3/11 1:56p erickson
 * SW7422-101: add NEXUS_PidChannelSettings.continuityCountEnabled
 *
 * 45   1/27/10 4:34p erickson
 * SW7400-2651: add NEXUS_PidChannelSettings.remap for SPID pid remapping
 *
 * 44   1/6/10 4:32p erickson
 * SW3548-2696: enable IB5P for 3548
 *
 * 43   12/14/09 11:10a erickson
 * SW7335-638: fix pid channel refcnt if
 *  NEXUS_PidChannelSettings.pidChannelIndex is specified
 *
 * 42   12/11/09 3:32p erickson
 * SW7550-112: merge 7550 code. add support for sw message filtering.
 *  allow for no HW message filtering using standard XPT define.
 *
 * Refsw_7550/1   11/19/09 12:36p nitinb
 * SW7550-63: change  NEXUS_NUM_MESSAGE_CAPABLE_PID_CHANNELS to 48 for
 *  7550
 *
 * 41   12/9/09 3:20p gmohile
 * SW7408-1 : Fix conditional compile for parser band support
 *
 * 40   12/1/09 6:24p randyjew
 * SW7468-6: Add 7468 support
 *
 * 39   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 38   11/11/09 2:40p erickson
 * SW7400-2601: add NEXUS_PidChannelSettings.enabled, default to true
 *
 * 37   10/19/09 11:26a erickson
 * SW7400-2559: add NEXUS_PidChannelSettings.dssHdFilter
 *
 * 36   10/19/09 10:42a erickson
 * SW7400-2559: add NEXUS_PID_CHANNEL_OPEN macros for more
 *  NEXUS_PidChannel_Open options
 *
 * 35   10/1/09 5:03p erickson
 * SW7405-3087: add playpump cc check, add cc error counting per pid
 *
 * 34   8/14/09 7:16p jrubio
 * PR55232: Input Band Max 7 for 7342
 *
 * 33   6/11/09 4:30p jtna
 * PR55817: allow refcnt on pidChannelIndex opened more than once
 *
 * 32   6/8/09 7:17a erickson
 * PR55717: support platforms with no input bands
 *
 * 31   2/24/09 5:30p anilmm
 * PR52416:  Check for parser band allpass should include check for
 *  cablecard enable as well
 *
 * 30   12/3/08 1:18p erickson
 * PR45405: rename rave member to pidChannelIndex to avoid
 *  misunderstanding
 *
 * 29   12/1/08 12:49p erickson
 * PR49676: set unused status params to invalid values to flush out app
 *  bugs
 *
 * 28   11/24/08 10:28a erickson
 * PR48846: fix refcnt logic
 *
 * 27   11/12/08 1:26p jgarrett
 * PR 48830: Adding NEXUS_PidChannel_SetEnabled
 *
 * 26   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 25   8/7/08 11:11a piyushg
 * PR45008: Fix compile error due to change in XPT PI function
 * definition.
 *
 * 24   7/23/08 2:22p erickson
 * PR45008: NEXUS_PidChannel_AddSplicePidChannel must support a pid
 *  channel attached to multiple rave contexts
 *
 * 23   7/21/08 1:27p erickson
 * PR45008: add pid splicing API
 *
 * 22   6/18/08 1:19p erickson
 * PR43730: fix warning
 *
 * 21   6/16/08 11:53a erickson
 * PR42973: fix all pass changes. only use parserBand if playpumpIndex ==
 *  -1.
 *
 * 20   6/13/08 7:19p erickson
 * PR42973: merge all pass changes
 *
 * 19   5/28/08 7:45p jrubio
 * PR43085: fix PLAYPUMP=0 compile
 *
 * 18   5/15/08 1:01p erickson
 * PR42706: coverity fix
 *
 * 17   5/2/08 9:18a erickson
 * PR42339: refcnt check needed a little more
 *
 * 16   5/1/08 2:56p erickson
 * PR42391: refcnt pid channels after checking both live and playback
 *  criteria. also, removed duplicate storage to prevent future bugs.
 *
 * 15   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 14   4/21/08 3:10p erickson
 * PR41987: add optional NEXUS_PidChannelSettings.pidChannelIndex for
 *  user-specified pid channel index
 *
 * 13   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 12   4/10/08 1:58p erickson
 * PR40079: added PidChannel ScramblingCheck
 *
 * 11   4/4/08 11:52a erickson
 * PR40079: prevent infinite loop. this algo must be reworked.
 *
 * 10   4/4/08 11:26a erickson
 * PR40079: fix for non-3563
 *
 * 9   4/4/08 11:18a erickson
 * PR40079: impl NEXUS_PidChannel_GetScramblingStatus
 *
 * 8   3/27/08 11:27a erickson
 * PR40978: remove chip-specific parallelInput check. allow XPT PI to fail
 *  or WRN.
 *
 * 7   3/27/08 10:37a erickson
 * PR40851: if parser band is DSS, it should override some input band
 *  settings. if not DSS, don't override.
 *
 * 6   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 5   2/22/08 5:03p erickson
 * PR39868: clean up static analysis warnings
 *
 * 4   2/22/08 10:51a erickson
 * PR39868: added more parser band asserts
 *
 * 3   2/20/08 10:33a erickson
 * PR39743: fix buffer overrun fix
 *
 * 2   2/19/08 3:47p erickson
 * PR39743: fix possible buffer overrun
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#include "bxpt_spid.h"
#include "nexus_playpump_impl.h"
#include "nexus_class_verification.h"

BDBG_MODULE(nexus_demux);

static NEXUS_Error NEXUS_PidChannel_P_SetEnabled( NEXUS_PidChannelHandle pidChannel, bool enabled );

void NEXUS_InputBand_GetSettings(NEXUS_InputBand inputBand, NEXUS_InputBandSettings *pSettings)
{
#if NEXUS_NUM_INPUT_BANDS
    BERR_Code rc=0;
    /* this code assumes Nexus holds the default state, not the PI or HW */
    if (inputBand >= NEXUS_NUM_INPUT_BANDS) {
        rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    *pSettings = pTransport->inputBand[inputBand].settings;
#else
    BSTD_UNUSED(inputBand);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#endif
}

NEXUS_Error NEXUS_InputBand_SetSettings(NEXUS_InputBand inputBand, const NEXUS_InputBandSettings *pSettings)
{
#if NEXUS_NUM_INPUT_BANDS
    BERR_Code rc;
    BXPT_InputBandConfig  config;

    NEXUS_ASSERT_MODULE(); /* make sure init was called */
    if (inputBand >= NEXUS_NUM_INPUT_BANDS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    BXPT_GetInputBandConfig(pTransport->xpt, inputBand, &config);
    config.ClockPolSel = pSettings->clockActiveHigh?BXPT_Polarity_eActiveHigh:BXPT_Polarity_eActiveLow;
    config.LsbFirst = pSettings->dataLsbFirst;
    config.DataPolSel = pSettings->dataActiveHigh?BXPT_Polarity_eActiveHigh:BXPT_Polarity_eActiveLow;
    config.ForceValid = !pSettings->validEnabled;
    config.ValidPolSel = pSettings->validActiveHigh?BXPT_Polarity_eActiveHigh:BXPT_Polarity_eActiveLow;
    config.UseSyncAsValid = pSettings->useSyncAsValid;
    config.SyncPolSel = pSettings->syncActiveHigh?BXPT_Polarity_eActiveHigh:BXPT_Polarity_eActiveLow;
    config.EnableErrorInput = pSettings->errorEnabled;
    config.ErrorPolSel = pSettings->errorActiveHigh?BXPT_Polarity_eActiveHigh:BXPT_Polarity_eActiveLow;
    config.ParallelInputSel = pSettings->parallelInput;
    if (NEXUS_IS_DSS_MODE(pTransport->inputBand[inputBand].transportType)) {
        /* If parser band was configured for DSS, we need to override the input band settings */
        config.IbPktLength = 130;
        config.SyncDetectEn = false;
    }
    else {
        config.IbPktLength = pSettings->packetLength;
        config.SyncDetectEn = pSettings->useInternalSync;
    }
    rc = BXPT_SetInputBandConfig(pTransport->xpt, inputBand, &config);
    if (rc) {return BERR_TRACE(rc);}

    pTransport->inputBand[inputBand].settings = *pSettings;

    return BERR_SUCCESS;
#else
    BSTD_UNUSED(inputBand);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_InputBand_P_SetTransportType(NEXUS_InputBand inputBand, NEXUS_TransportType transportType)
{
#if NEXUS_NUM_INPUT_BANDS
    if (inputBand >= NEXUS_NUM_INPUT_BANDS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    pTransport->inputBand[inputBand].transportType = transportType;
    return NEXUS_InputBand_SetSettings(inputBand, &pTransport->inputBand[inputBand].settings);
#else
    BSTD_UNUSED(inputBand);
    BSTD_UNUSED(transportType);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

#include "bchp_xpt_fe.h"
NEXUS_Error NEXUS_InputBand_GetStatus(NEXUS_InputBand inputBand, NEXUS_InputBandStatus *pStatus)
{
#if NEXUS_NUM_INPUT_BANDS
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    switch (inputBand-NEXUS_InputBand_e0) {
    case 0: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB0_SYNC_COUNT); break;
    case 1: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB1_SYNC_COUNT); break;
#if NEXUS_NUM_INPUT_BANDS > 2
    case 2: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB2_SYNC_COUNT); break;
    case 3: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB3_SYNC_COUNT); break;
#endif
#if NEXUS_NUM_INPUT_BANDS > 4
    case 4: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB4_SYNC_COUNT); break;
#endif
#if NEXUS_NUM_INPUT_BANDS > 5
#ifndef BCHP_XPT_FE_IB5_SYNC_COUNT
#define BCHP_XPT_FE_IB5_SYNC_COUNT BCHP_XPT_FE_IB5P_SYNC_COUNT
#endif
    case 5: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB5_SYNC_COUNT); break;
#endif
#if NEXUS_NUM_INPUT_BANDS > 6
    case 6: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB6_SYNC_COUNT); break;
#endif
#if NEXUS_NUM_INPUT_BANDS > 7
    case 7: pStatus->syncCount = BREG_Read32(g_pCoreHandles->reg, BCHP_XPT_FE_IB7_SYNC_COUNT); break;
#endif
    }
    return 0;
#else
    BSTD_UNUSED(inputBand);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

void NEXUS_PidChannel_GetDefaultSettings(NEXUS_PidChannelSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_ANY;
    pSettings->enabled = true;
    pSettings->continuityCountEnabled = true;
    pSettings->remap.continuityCountEnabled = true;
}

NEXUS_PidChannelHandle NEXUS_PidChannel_Open(NEXUS_ParserBand parserBand, uint16_t pid,
    const NEXUS_PidChannelSettings *pSettings)
{
    NEXUS_PidChannelHandle handle = NULL;
    NEXUS_ParserBandHandle band = NEXUS_ParserBand_Resolve_priv(parserBand);
    if (band)
    {
        handle = NEXUS_PidChannel_P_Open(band, NULL, pid, pSettings,
                       band->settings.continuityCountEnabled
                       );
        if (handle)
        {
            handle->status.parserBand = parserBand; /* need whatever was passed in as status, handle or enum */
        }
    }
    else {
        BDBG_ERR(("unable to resolve %#x", parserBand));
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return handle;
}


NEXUS_PidChannelHandle NEXUS_PidChannel_P_Open(NEXUS_ParserBandHandle parserBand, NEXUS_PlaypumpHandle playpump, unsigned combinedPid,
    const NEXUS_PidChannelSettings *pSettings, bool bandContinuityCountEnabled)
{
    NEXUS_PidChannelHandle duplicatePidChannel = NULL;
    NEXUS_PidChannelHandle pidChannel;
    unsigned index; /* hw index */
    BERR_Code rc;
    NEXUS_PidChannelSettings settings;
    uint16_t pid = combinedPid&0xFFFF; /* virtual_pid could also include the sub-stream id */

    NEXUS_ASSERT_MODULE();

    if (!pSettings) {
        NEXUS_PidChannel_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    if (playpump) {
        parserBand = NULL; /* parserBand is invalid if playpump is set. */
    }
    else {
        BDBG_OBJECT_ASSERT(parserBand, NEXUS_ParserBand);
    }

    /* user specifies the HW pid channel */
    if (pSettings->pidChannelIndex >= 0) {
        index = (unsigned)pSettings->pidChannelIndex;
        if (index >= NEXUS_NUM_PID_CHANNELS) {
            BDBG_ERR(("Invalid pid channel index %d", index));
            return NULL;
        }

        /* if already opened */
        for (pidChannel = BLST_S_FIRST(&pTransport->pidChannels); pidChannel; pidChannel = BLST_S_NEXT(pidChannel, link)) {
            if (pidChannel->status.pidChannelIndex == (unsigned)pSettings->pidChannelIndex) {
                /* must be the same pid */
                if (pidChannel->status.pid != pid) {
                    BDBG_ERR(("Cannot open the same pidChannelIndex (%d) with different pids (0x%x and 0x%x)", pSettings->pidChannelIndex, pidChannel->status.pid, pid));
                    return NULL;
                }
                if (!duplicatePidChannel) {
                    duplicatePidChannel = pidChannel;
                }
            }
        }
    }
    else {
        /* nexus assigns the HW pid channel */
        bool found = false;
        
        /* first, see if the same HW pid channel is already opened */
        for (pidChannel = BLST_S_FIRST(&pTransport->pidChannels); pidChannel; pidChannel = BLST_S_NEXT(pidChannel, link)) {
#if B_HAS_DSS
            /* a different ENABLE_HD_FILTER value allows for a duplicate pid channel */
            if ((pidChannel->settings.dssHdFilter == NEXUS_PidChannelDssHdFilter_eDisabled) != (pSettings->dssHdFilter == NEXUS_PidChannelDssHdFilter_eDisabled)) {
                continue;
            }
#endif

            /* playback */
            if (playpump &&
                pidChannel->status.playback &&
                pidChannel->status.playbackIndex == playpump->index &&
                pidChannel->status.pid == pid)
            {
                break;
            }
            /* live */
            if (!playpump &&
                !pidChannel->status.playback &&
                pidChannel->parserBand == parserBand &&
                (pidChannel->status.pid == pid
                || (parserBand->settings.allPass
                    && parserBand->settings.cableCard == NEXUS_CableCardType_eNone ) ))
            {
                break;
            }
        }
        if (pidChannel) {
            /* we can/must reuse the HW pid channel */
            duplicatePidChannel = pidChannel;
            index = duplicatePidChannel->status.pidChannelIndex;
            found = true;
        }

        /* we're not already using it, so find an available HW pid channel. */

        /* first, handle allpass special case */
        if (!found && 
            !playpump && 
            parserBand->settings.allPass && 
            parserBand->settings.cableCard == NEXUS_CableCardType_eNone)
        {
            /* allPass is a special mode where HW pid channel must == parser band. */
            /* ignore duplicatePidChannel. this is a test scenario anyway. */
            NEXUS_ParserBand_GetAllPassPidChannelIndex((NEXUS_ParserBand)parserBand, &index);
            found = true;
        }

        /* otherwise, we must search. pid channels are arranged in this pattern of capabilities:
        group 1: live parser band all pass capable, msg filter capable
        group 2: msg filter capable
        group 3: playback parser band all pass capable, msg filter capable
        group 4: msg filter capable
        group 5: not msg filter capable
        if we are searching for ANY, we search in this order: group 4, group 5, group 2, group 1, group 3.
        MESSAGE_CAPABLE is like ANY, but excluding group 5
        NOT_MESSAGE_CAPABLE is group 5 only
        */
        /* Some parts, such as the 7550, don't have message filtering hardware. BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS
           won't be defined for those chips. */
#ifdef BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS
        BDBG_CASSERT(BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS <= NEXUS_NUM_PID_CHANNELS);
#endif
        if (!found) {
            unsigned firstPidChannel;
            unsigned totalPidChannels;
            unsigned firstNonAllPass;
#if NEXUS_NUM_PLAYPUMPS
            firstNonAllPass = BXPT_GET_PLAYBACK_ALLPASS_CHNL(NEXUS_NUM_PLAYPUMPS-1) + 1;
#else
            firstNonAllPass = BXPT_GET_IB_PARSER_ALLPASS_CHNL(NEXUS_NUM_PARSER_BANDS-1) + 1;
#endif
            switch (pSettings->pidChannelIndex) {
            case NEXUS_PID_CHANNEL_OPEN_ANY:
                firstPidChannel = firstNonAllPass;
                totalPidChannels = NEXUS_NUM_PID_CHANNELS;
                break;

#ifdef BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS
            case NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE:
                firstPidChannel = firstNonAllPass;
                totalPidChannels = BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS;
                break;
            case NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE:
                firstPidChannel = BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS;
                totalPidChannels = NEXUS_NUM_PID_CHANNELS;
                break;
#else
            case NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE:
                BDBG_ERR(("Message filtering not support in hw."));
                BERR_TRACE(NEXUS_NOT_SUPPORTED);
                return NULL;
            case NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE:
                firstPidChannel = firstNonAllPass;
                totalPidChannels = NEXUS_NUM_PID_CHANNELS;
                break;
#endif

            default:
                BDBG_ERR(("invalid pidChannelIndex %d", pSettings->pidChannelIndex));
                return NULL;
            }
    
            for (index=firstPidChannel;index<totalPidChannels;index++) {
                if (pTransport->hwPidChannelRefCnt[index] == 0) {
                    found = true;
                    break;
                }
            }
            if (!found && pSettings->pidChannelIndex != NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE) {
#if NEXUS_NUM_PLAYPUMPS && NEXUS_NUM_PARSER_BANDS
                /* search group 2 */
                {
                    unsigned first = BXPT_GET_IB_PARSER_ALLPASS_CHNL(NEXUS_NUM_PARSER_BANDS-1) + 1;
                    unsigned bound = BXPT_GET_PLAYBACK_ALLPASS_CHNL(0);
                    for (index=first;index<bound;index++) {
                        if (pTransport->hwPidChannelRefCnt[index] == 0) {
                            found = true;
                            break;
                        }
                    }
                }
#endif            
                if (!found) {
                    /* search group 1 through 3 */
                    for (index=0;index<firstNonAllPass;index++) {
                        if (pTransport->hwPidChannelRefCnt[index] == 0) {
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!found) {
            BDBG_ERR(("No more pid channels available: pid %#x, pidChannelIndex %d", pid, pSettings->pidChannelIndex));
            return NULL;
        }
    }

    /* Don't call BXPT_AllocPidChannel because we need the option of selecting the index.
    BXPT_ConfigurePidChannel will reserve it just the same. */
    BDBG_MSG(("Allocated PID channel %d for PID 0x%04x", index, pid));

    /* If the parser we want is in allPass mode, make sure the correct PID channel is used. Similar code in
       NEXUS_ParserBand_SetSettings() will cover the possiblity that the parser is configured after the PID channel. */
    if (!playpump && parserBand->settings.allPass && parserBand->settings.cableCard == NEXUS_CableCardType_eNone) {
        unsigned AllPassPidChannel;
        NEXUS_ParserBand_GetAllPassPidChannelIndex((NEXUS_ParserBand)parserBand, &AllPassPidChannel);
        if (index != AllPassPidChannel )
        {
            BDBG_ERR(("Incorrect PID channel used for allPass on parser band %u. See NEXUS_ParserBand_GetAllPassPidChannelIndex()", parserBand));
            return NULL;
        }
    }

    if (duplicatePidChannel) {
        /* test if new dup pid channel has conflicting settings. can't just memcmp settings because some don't apply.
        ignore requireMessageBuffer and pidChannelIndex. we already know the HW pid channel index matches.
        enabled doesn't have to match. it will be and-ed with others.
        others must match.
        */
        if (pSettings->dssHdFilter != duplicatePidChannel->settings.dssHdFilter ||
            pSettings->continuityCountEnabled != duplicatePidChannel->settings.continuityCountEnabled ||
            pSettings->generateContinuityCount != duplicatePidChannel->settings.generateContinuityCount ||
            BKNI_Memcmp(&pSettings->remap, &duplicatePidChannel->settings.remap, sizeof(pSettings->remap)))
        {
            BDBG_ERR(("Cannot open duplicate pid channel with different settings"));
            return NULL;
        }
    }

    pidChannel = BKNI_Malloc(sizeof(*pidChannel));
    if (!pidChannel) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_PidChannel, pidChannel);
    pidChannel->playpump = playpump;
    pidChannel->settings = *pSettings;
    pidChannel->status.pid = pid;
    pidChannel->status.pidChannelIndex = index;
    pidChannel->combinedPid = combinedPid;
    pTransport->hwPidChannelRefCnt[index]++;
    BKNI_EnterCriticalSection();
    BLST_S_INSERT_HEAD(&pTransport->pidChannels, pidChannel, link);
    BKNI_LeaveCriticalSection();

    pidChannel->status.playback = playpump != NULL;

    if (pidChannel->status.playback)
    {
        pidChannel->parserBand = NULL;
        pidChannel->status.playbackIndex = playpump->index;
        pidChannel->status.parserBand = 0xffff; /* invalid value */
    }
    else
    {
        NEXUS_OBJECT_ACQUIRE(pidChannel, NEXUS_ParserBand, parserBand);
        pidChannel->parserBand = parserBand;
        pidChannel->status.playbackIndex = 0xffff; /* invalid value */
        pidChannel->status.parserBand = parserBand->hwIndex;
        /* must inc parserBand refcnt here. if it fails later, NEXUS_PidChannel_P_Close will decrement */
        parserBand->refcnt++;
        /* status.transportType gets overwritten by Playpump if it packetizes */
        pidChannel->status.transportType = pidChannel->status.originalTransportType = parserBand->settings.transportType;
    }

    pidChannel->status.remappedPid = pid;
    pidChannel->status.continuityCountErrors = 0;

    if (pTransport->hwPidChannelRefCnt[index] == 1) {
#if B_HAS_DSS
        /* If we're not in DSS mode, HD filtering needs to be disabled BEFORE configuring the PID table */
        rc = BXPT_DirecTv_ConfigHdFiltering(pTransport->xpt, pidChannel->status.pidChannelIndex,
            pSettings->dssHdFilter != NEXUS_PidChannelDssHdFilter_eDisabled,
            ((pSettings->dssHdFilter == NEXUS_PidChannelDssHdFilter_eAux) ? BXPT_HdFilterMode_eAuxOnly : BXPT_HdFilterMode_eNonAuxOnly));
        if (rc) {rc=BERR_TRACE(rc); goto fail1;}
#endif 

        /* Configure it */
        if (pSettings->remap.enabled) {
            /* the old pid goes in */
            rc = BXPT_Spid_ConfigureChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, pid, BXPT_Spid_eChannelMode_Remap);
            if (rc) {rc=BERR_TRACE(rc); goto fail1;}

            /* the new pid comes out */
            rc = BXPT_ConfigurePidChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, pSettings->remap.pid, playpump?BXPT_PB_PARSER(playpump->index):parserBand->hwIndex);
            if (rc) {rc=BERR_TRACE(rc); goto fail1;}

            pidChannel->status.remappedPid = pSettings->remap.pid; /* this should be the new pid */
        }
        else {
            (void)BXPT_Spid_ConfigureChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, 0, BXPT_Spid_eChannelMode_Disable);

            rc = BXPT_ConfigurePidChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, pid, playpump?BXPT_PB_PARSER(playpump->index):parserBand->hwIndex);
            if (rc) {rc=BERR_TRACE(rc); goto fail1;}
        }

        if (NEXUS_GetEnv("cont_count_ignore")) {
            bandContinuityCountEnabled = false;
        }

#if !NEXUS_PARSER_BAND_CC_CHECK
        {
        BXPT_PidChannel_CC_Config cfg;
        BXPT_GetPidChannel_CC_Config(pTransport->xpt, pidChannel->status.pidChannelIndex, &cfg);
        cfg.Primary_CC_CheckEnable = pSettings->continuityCountEnabled && bandContinuityCountEnabled;
        cfg.Secondary_CC_CheckEnable = pSettings->remap.continuityCountEnabled && bandContinuityCountEnabled;
        cfg.Generate_CC_Enable = pSettings->generateContinuityCount;
        rc = BXPT_SetPidChannel_CC_Config(pTransport->xpt, pidChannel->status.pidChannelIndex, &cfg);
        if (rc) {rc=BERR_TRACE(rc); goto fail1;}
        }
#endif
    }
    else {
        if (duplicatePidChannel) {
            pidChannel->status.remappedPid = duplicatePidChannel->status.remappedPid;
        }
        else {
            BDBG_ERR(("refcnt error pidch[%d] = %d", index, pTransport->hwPidChannelRefCnt[index]));
        }
    }

    /* must call NEXUS_PidChannel_SetEnabled because it may disable dups */
    rc = NEXUS_PidChannel_P_SetEnabled(pidChannel, pSettings->enabled);
    if (rc) {rc=BERR_TRACE(rc); goto fail1;}

    pidChannel->status.enabled = pSettings->enabled;

    if (!playpump) {
        NEXUS_ParserBand_P_SetEnable(parserBand);
        parserBand->pidChannels++;
    }
    return pidChannel;

fail1:
    if (pidChannel) {
        NEXUS_PidChannel_P_Close(pidChannel);
    }
    return NULL;
}

void NEXUS_PidChannel_Close(NEXUS_PidChannelHandle pidChannel)
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    if (pidChannel->playpump) {
        NEXUS_Playpump_P_ClosePidChannel(pidChannel->playpump, pidChannel);
    }
    else {
        NEXUS_PidChannel_P_Close(pidChannel);
    }
}

static void NEXUS_PidChannel_P_Finalizer(NEXUS_PidChannelHandle pidChannel)
{
    unsigned index; /* hw index */

    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);

    if (!pidChannel->status.playback) {
        if (pidChannel->parserBand)
        {
            pidChannel->parserBand->pidChannels--;
            BDBG_ASSERT(pidChannel->parserBand->refcnt);
            pidChannel->parserBand->refcnt--;
            NEXUS_ParserBand_P_SetEnable(pidChannel->parserBand);
            NEXUS_OBJECT_RELEASE(pidChannel, NEXUS_ParserBand, pidChannel->parserBand);
        }
    }

    NEXUS_PidChannel_StopScramblingCheck(pidChannel);

    if (pidChannel->settings.remap.enabled) {
        (void)BXPT_Spid_ConfigureChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, 0, BXPT_Spid_eChannelMode_Disable);
    }

    index = pidChannel->status.pidChannelIndex;
    BDBG_ASSERT(pTransport->hwPidChannelRefCnt[index]);
    pTransport->hwPidChannelRefCnt[index]--;
    BDBG_MSG(("dec pidchannel refcnt(%d) on pidchannel %d", pTransport->hwPidChannelRefCnt[index], index));
    if (pTransport->hwPidChannelRefCnt[index] == 0) {
        BXPT_DisablePidChannel(pTransport->xpt, index);
        BXPT_FreePidChannel(pTransport->xpt, index);
    }

    BKNI_EnterCriticalSection();
    BLST_S_REMOVE(&pTransport->pidChannels, pidChannel, NEXUS_PidChannel, link);
    BKNI_LeaveCriticalSection();

    /* recalc "enabled" for remaining dups. find first dup and reapply its state. it will recalc for all. */
    if (pTransport->hwPidChannelRefCnt[index] > 0) {
        NEXUS_PidChannelHandle other;
        for (other = BLST_S_FIRST(&pTransport->pidChannels); other; other = BLST_S_NEXT(other, link)) {
            if (other->status.pidChannelIndex == pidChannel->status.pidChannelIndex) {
                NEXUS_PidChannel_P_SetEnabled(other, other->settings.enabled);
                break;
            }
        }
    }
    NEXUS_OBJECT_DESTROY(NEXUS_PidChannel, pidChannel);
    BKNI_Free(pidChannel);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_PidChannel, NEXUS_PidChannel_P_Close);

void NEXUS_PidChannel_CloseAll(NEXUS_ParserBand parserBand)
{
    NEXUS_ParserBandHandle handle;
    NEXUS_PidChannelHandle pidChannel;

    handle = NEXUS_ParserBand_Resolve_priv(parserBand);

    for (pidChannel = BLST_S_FIRST(&pTransport->pidChannels); pidChannel;) {
        NEXUS_PidChannelHandle next = BLST_S_NEXT(pidChannel, link);

         if (!pidChannel->status.playback && pidChannel->parserBand == handle) {
            NEXUS_OBJECT_UNREGISTER(NEXUS_PidChannel, pidChannel, Close);
            NEXUS_PidChannel_Close(pidChannel);
        }
        pidChannel = next;
    }
}

NEXUS_Error NEXUS_PidChannel_GetStatus(NEXUS_PidChannelHandle pidChannel, NEXUS_PidChannelStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    /* no need to check pidChannel->refcnt. if zero, we should have already asserted */
    *pStatus = pidChannel->status;
    return 0;
}

void NEXUS_PidChannel_ConsumerStarted(NEXUS_PidChannelHandle pidChannel)
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    if (pidChannel->status.playback) {
        NEXUS_PlaypumpHandle playpump;

        BDBG_MSG(("NEXUS_PidChannel_ConsumerStarted on playback %d", pidChannel->status.playbackIndex));
#if NEXUS_NUM_PLAYPUMPS
        if (pidChannel->status.playbackIndex < NEXUS_NUM_PLAYPUMPS) {
            playpump = pTransport->playpump[pidChannel->status.playbackIndex].playpump;
        }
        else
#endif
        {
            playpump = NULL;
        }
        if (playpump) {
            NEXUS_Playpump_P_ConsumerStarted(playpump);
        } else {
           BDBG_ERR(("NEXUS_PidChannel_ConsumerStarted: %#lx invalid state", (unsigned long)pidChannel));
        }
    }
}

/**
TODO: consider adding pTransport->rave[].splicePidChannel list to keep track. for now, this is a seldomly used API, so we'll let XPT keep track.
 also consider adding pidChannel->ravelist to avoid the look up. for now, it's acceptable.
**/
NEXUS_Error NEXUS_PidChannel_AddSplicePidChannel( NEXUS_PidChannelHandle pidChannel, NEXUS_PidChannelHandle splicePidChannel )
{
    NEXUS_Error rc;
    NEXUS_RaveHandle rave;
    unsigned i,j;
    bool found = false;

    for (j=0;j<NEXUS_NUM_RAVE_CHANNELS;j++) {
        for (i=0;i<NEXUS_NUM_RAVE_CONTEXTS;i++) {
            rave = &pTransport->rave[j].context[i];
            if (rave->pidChannelIndex == pidChannel->status.pidChannelIndex && rave->raveHandle) {
                rc = BXPT_Rave_PushPidChannel(rave->raveHandle, pidChannel->status.pidChannelIndex, splicePidChannel->status.pidChannelIndex);
                if (rc) return BERR_TRACE(rc);
                found = true;
            }
        }
    }

    if (!found) {
        BDBG_ERR(("Unable to look up RAVE context. The NEXUS_PidChannel must be currently decoding to allow pid splicing."));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return 0;
}

NEXUS_Error NEXUS_PidChannel_RemoveSplicePidChannel( NEXUS_PidChannelHandle pidChannel, NEXUS_PidChannelHandle splicePidChannel )
{
    NEXUS_Error rc;
    NEXUS_RaveHandle rave;
    unsigned i,j;
    bool found = false;

    for (j=0;j<NEXUS_NUM_RAVE_CHANNELS;j++) {
        for (i=0;i<NEXUS_NUM_RAVE_CONTEXTS;i++) {
            rave = &pTransport->rave[j].context[i];
            if (rave->pidChannelIndex == pidChannel->status.pidChannelIndex && rave->raveHandle) {
                rc = BXPT_Rave_RemovePidChannel(rave->raveHandle, splicePidChannel->status.pidChannelIndex);
                if (rc) return BERR_TRACE(rc);
                found = true;
            }
        }
    }

    if (!found) {
        BDBG_ERR(("Unable to look up RAVE context. The NEXUS_PidChannel must be currently decoding to allow pid splicing."));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return 0;
}

void NEXUS_PidChannel_RemoveAllSplicePidChannels( NEXUS_PidChannelHandle pidChannel )
{
    NEXUS_Error rc;
    NEXUS_RaveHandle rave;
    unsigned i,j;
    bool found = false;

    for (j=0;j<NEXUS_NUM_RAVE_CHANNELS;j++) {
        for (i=0;i<NEXUS_NUM_RAVE_CONTEXTS;i++) {
            rave = &pTransport->rave[j].context[i];
            if (rave->pidChannelIndex == pidChannel->status.pidChannelIndex && rave->raveHandle) {
                rc = BXPT_Rave_ClearQueue(rave->raveHandle);
                if (rc) rc = BERR_TRACE(rc);
                found = true;
            }
        }
    }

    if (!found) {
        BDBG_ERR(("Unable to look up RAVE context. The NEXUS_PidChannel must be currently decoding to allow pid splicing."));
        rc = BERR_TRACE(NEXUS_UNKNOWN);
    }
}

/* public function can filter out change */
NEXUS_Error NEXUS_PidChannel_SetEnabled( NEXUS_PidChannelHandle pidChannel, bool enabled )
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    if (pidChannel->settings.enabled == enabled) {
        return 0;
    }
    pidChannel->settings.enabled = enabled;
    return NEXUS_PidChannel_P_SetEnabled(pidChannel, enabled);
}

/* private function may be called with no settings.enabled change, but status may change */
static NEXUS_Error NEXUS_PidChannel_P_SetEnabled( NEXUS_PidChannelHandle pidChannel, bool enabled )
{
    NEXUS_PidChannelHandle other;
    unsigned index = pidChannel->status.pidChannelIndex;

    /* must "and" with all other SW pid channel handles for this HW pid channel (short circuit if !enabled).
    pidChannel->settings.enabled stores SW state for each handle.
    pidChannel->status.enabled stores HW state (must be the same for all dups). */
    if (pTransport->hwPidChannelRefCnt[index] > 1) {
        for (other = BLST_S_FIRST(&pTransport->pidChannels); other && enabled; other = BLST_S_NEXT(other, link)) {
            BDBG_OBJECT_ASSERT(other, NEXUS_PidChannel);
            if (other->status.pidChannelIndex == index) {
                enabled &= other->settings.enabled;
            }
        }
    }

    if ( pidChannel->status.enabled != enabled )
    {
        BERR_Code errCode;

        if ( enabled )
        {
            BDBG_MSG(("Enabling pid channel %u", index));
            errCode = BXPT_EnablePidChannel(pTransport->xpt, index);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
        else
        {
            BDBG_MSG(("Disabling pid channel %u", index));
            errCode = BXPT_DisablePidChannel(pTransport->xpt, index);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }

        /* update all status */
        if (pTransport->hwPidChannelRefCnt[index] > 1) {
            for (other = BLST_S_FIRST(&pTransport->pidChannels); other; other = BLST_S_NEXT(other, link)) {
                if (other->status.pidChannelIndex == index) {
                    other->status.enabled = enabled;
                }
            }
        }
        else {
            /* avoid search */
            pidChannel->status.enabled = enabled;
        }
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_PidChannel_SetRemapSettings(
    NEXUS_PidChannelHandle pidChannel,
    const NEXUS_PidChannelRemapSettings *pSettings)
{
    /* SW7344-192, Coverity: 35340 */
    BERR_Code rc = NEXUS_SUCCESS;
    bool bandContinuityCountEnabled = pidChannel->settings.continuityCountEnabled;
    bool wasEnabled = false;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    BDBG_ASSERT(pSettings);

    if (pidChannel->status.enabled) {
        rc = NEXUS_PidChannel_P_SetEnabled(pidChannel, false);
        if (rc) {rc=BERR_TRACE(rc); goto fail;}
        wasEnabled = true;
    }

    if (pSettings->enabled) {
        /* the old pid goes in */
        rc = BXPT_Spid_ConfigureChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, pidChannel->status.pid, BXPT_Spid_eChannelMode_Remap);
        if (rc) {rc=BERR_TRACE(rc); goto fail;}

        if (pidChannel->parserBand)
        {
            /* the new pid comes out */
            rc = BXPT_ConfigurePidChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, pSettings->pid, pidChannel->parserBand->hwIndex);
            if (rc) {rc=BERR_TRACE(rc); goto fail;}
        }

        pidChannel->status.remappedPid = pSettings->pid; /* this should be the new pid */

        if (NEXUS_GetEnv("cont_count_ignore")) {
            bandContinuityCountEnabled = false;
        }
#if (!NEXUS_PARSER_BAND_CC_CHECK)
        {
            BXPT_PidChannel_CC_Config cfg;
            BXPT_GetPidChannel_CC_Config(pTransport->xpt,pidChannel->status.pidChannelIndex, &cfg);
            cfg.Secondary_CC_CheckEnable = pSettings->continuityCountEnabled && bandContinuityCountEnabled;
            rc = BXPT_SetPidChannel_CC_Config(pTransport->xpt, pidChannel->status.pidChannelIndex, &cfg);
            if (rc) {rc=BERR_TRACE(rc); goto fail;}
        }
#endif
        pidChannel->settings.remap = *pSettings;
    }
    else {
        if (pidChannel->settings.remap.pid) {
            (void)BXPT_Spid_ConfigureChannel(pTransport->xpt, pidChannel->status.pidChannelIndex, 0, BXPT_Spid_eChannelMode_Disable);
        }
    }

fail:
    if (wasEnabled) {
        /* even on failure condition, we want to reenable */
        (void)NEXUS_PidChannel_P_SetEnabled(pidChannel, true);
    }
    return rc;
}

NEXUS_Error NEXUS_PidChannel_ResetStatus( NEXUS_PidChannelHandle pidChannel )
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    pidChannel->status.continuityCountErrors = 0;
    return 0;
}

#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error NEXUS_CI_Access(unsigned long RegMem, unsigned long RdWr, unsigned long Offset, unsigned long Value)
{
	extern BREG_Handle g_hReg;
	extern BREG_Handle g_CI_MMAP_Handle;
	unsigned long testData;
	if(RdWr == 0 || RdWr == 1) /*Read=0, Write=1*/
	{
		if(RegMem == 0) /*Access register in 32bit mode*/
		{
			if(RdWr == 0) /*Read*/
				*(unsigned long*)Value = BREG_Read32(g_hReg, Offset);
			else if(RdWr == 1) /*Write*/
				BREG_Write32(g_hReg, Offset, Value);
		}
		else if(RegMem == 1) /*Access cam mem/io in 8bit mode*/
		{
			if(RdWr == 0) /*Read*/
				*(unsigned long*)Value = (BREG_Read8(g_CI_MMAP_Handle, Offset) & 0x000000FF);
			else if(RdWr == 1) /*Write*/
				BREG_Write8(g_CI_MMAP_Handle, Offset, (Value & 0x000000FF));
		}	
	}
	else
	{
		BDBG_ERR(("Unknown access type: %u", RegMem));
		return NEXUS_UNKNOWN;
	}

	return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_CI_Read(unsigned long RegMem, unsigned long Offset, unsigned long *Value)
{
	extern BREG_Handle g_hReg;
	extern BREG_Handle g_CI_MMAP_Handle;
	if(RegMem == 0) /*Access register in 32bit mode*/
	{
		*(unsigned long *)Value = BREG_Read32(g_hReg, Offset);
	}
	else if(RegMem == 1) /*Access cam mem/io in 8bit mode*/
	{
		*(unsigned long *)Value = (BREG_Read8(g_CI_MMAP_Handle, Offset) & 0x000000FF);
	}	

	return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_CI_Write(unsigned long RegMem, unsigned long Offset, unsigned long Value)
{
	extern BREG_Handle g_hReg;
	extern BREG_Handle g_CI_MMAP_Handle;
	if(RegMem == 0) /*Access register in 32bit mode*/
	{
		BREG_Write32(g_hReg, Offset, Value);
	}
	else if(RegMem == 1) /*Access cam mem/io in 8bit mode*/
	{
		BREG_Write8(g_CI_MMAP_Handle, Offset, (Value & 0x000000FF));
	}	

	return NEXUS_SUCCESS;
}

unsigned int NEXUS_Get_PidChannel_Used_HUMAX(NEXUS_PidChannelHandle pidChannel)
{
	return pidChannel->status.pidChannelIndex;
}
#else
NEXUS_Error NEXUS_CI_Access(unsigned long RegMem, unsigned long RdWr, unsigned long Offset, unsigned long Value)
{
	return NEXUS_SUCCESS;
}
NEXUS_Error NEXUS_CI_Read(unsigned long RegMem, unsigned long Offset, unsigned long *Value)
{
	return NEXUS_SUCCESS;
}
NEXUS_Error NEXUS_CI_Write(unsigned long RegMem, unsigned long Offset, unsigned long Value)
{
	return NEXUS_SUCCESS;
}
unsigned int NEXUS_Get_PidChannel_Used_HUMAX(NEXUS_PidChannelHandle pidChannel)
{
	return 0;
}
#endif

