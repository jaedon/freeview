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
 * $brcm_Workfile: nexus_transport_module.c $
 * $brcm_Revision: 119 $
 * $brcm_Date: 10/16/12 1:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_transport_module.c $
 * 
 * 119   10/16/12 1:34p erickson
 * SW7435-375: transport does not depend on security
 * 
 * 118   10/15/12 12:01p erickson
 * SW7435-375: transport module should not call into security module
 * 
 * 117   9/27/12 1:13p erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 116   9/19/12 10:28a erickson
 * SW7435-362: lock module around NEXUS_CancelTimer. reset global state
 *  for various workaround threads/timers.
 * 
 * 115   9/5/12 6:09p jtna
 * SW7425-3782: rework host and demod TSMF impl. both cases now go through
 *  the host API
 * 
 * 114   8/15/12 11:04a vsilyaev
 * SW7420-2085: Can't call NEXUS_OBJECT_UNSET in the Timebase finalizer
 * 
 * 113   8/2/12 10:35a erickson
 * SW7429-227: support NEXUS_NUM_PARSER_BANDS == 0
 * 
 * 112   7/23/12 9:20a gmullen
 * SW7425-3550: Removed dead code
 * 
 * 111   6/26/12 3:34p gmullen
 * SW7429-115: Cleaned up workaround
 * 
 * 110   6/21/12 6:39p gmullen
 * SW7429-115: Revert to using thread to call XPT DPCR workaround
 * 
 * 109   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 108   6/1/12 11:50a vsilyaev
 * SW7420-2311: Acquire lock before calling internal functions
 * 
 * 107   5/15/12 12:02p jtna
 * SW7425-3013: default MTSIF to enabled and don't ignore app's request to
 *  disable
 * 
 * 106   5/8/12 2:22p gmullen
 * SW7429-115: Merged changes to main
 * 
 * 105   3/29/12 3:44p erickson
 * SW7425-2497: stop timer if BXPT_CheckPipelineErrors not supported
 * 
 * 104   3/27/12 11:24a erickson
 * SW7435-77: add #if for minimal resource build
 * 
 * 103   3/27/12 10:37a erickson
 * SW7425-2497: convert thread to timer
 * 
 * SW7429-115/2   4/20/12 10:20a gmullen
 * SW7429-115: Return BERR_Code
 * 
 * SW7429-115/1   4/6/12 2:07p gmullen
 * SW7429-115: Implemented workaround
 * 
 * 102   3/14/12 11:39a erickson
 * SW7401-4517: remove old code
 * 
 * 101   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 100   3/8/12 3:42p gmullen
 * SW7425-2497: Merged to main
 * 
 * 99   3/6/12 10:33a mward
 * SW7425-2549, SW7425-2399: Set maxDataRate only if
 *  BXPT_HAS_PLAYBACK_PARSERS.
 * 
 * 98   3/5/12 2:49p vsilyaev
 * SW7425-2549, SW7425-2399: Improved handling of playback peak rate
 * 
 * SW7425-2497/2   3/8/12 3:19p gmullen
 * SW7425-2497: Added BXPT_HAS_PIPELINE_ERROR_REPORTING around code
 * 
 * SW7425-2497/1   3/5/12 4:31p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * 97   2/16/12 2:41p vsilyaev
 * SW7425-2399: Added peakDataRate for the playback(playpump) channels
 * 
 * 96   2/13/12 12:01p jtna
 * SW7231-653: Coverity Defect ID:36825 DEADCODE
 * 
 * 95   2/9/12 11:56a jtna
 * SW7425-2248: add
 *  NEXUS_TransportModuleSettings.clientEnabled.parserBand[].mpodRs
 * 
 * 94   1/24/12 10:19a gmullen
 * SW7425-1886: Merged to main
 * 
 * SW7425-1886/1   1/19/12 5:44p gmullen
 * SW7425-1886: CC interrupt now goes to app
 * 
 * 93   1/12/12 12:25p jtna
 * SW7425-1863: correctly conditionally-compile standby-related code
 * 
 * 92   1/11/12 5:59p bandrews
 * SW7425-1863: regroup BXPT_HAS_WAKEUP_PKT_SUPPORT-protected stuff so it
 *  builds on 7408/7550
 * 
 * 91   1/10/12 3:52p jtna
 * SW7425-1863: add code that removes app requirement to close/re-open pid
 *  channels
 * 
 * 90   1/10/12 2:58p jtna
 * SW7425-1863: set BXPT_StandbySettings.S3Standby correctly
 * 
 * 89   1/2/12 10:02a erickson
 * SW7358-176: fix #define NEXUS_NUM_REMUX 0 case
 * 
 * 88   12/21/11 1:49p erickson
 * SW7358-176: fix typo, rework some bounds
 * 
 * 87   12/20/11 1:40p gmullen
 * SW7358-176: Added per-client XC and RS allocation control
 * 
 * SW7358-176/1   12/20/11 1:32p gmullen
 * SW7358-176: Added per-client XC and RS allocation control
 * 
 * 86   12/8/11 12:17p jtna
 * SW7425-1923: Coverity Defect ID:36279 STACK_USE
 * 
 * 85   12/2/11 1:35p jtna
 * SW7425-1863: comment out while label hasn't moved
 * 
 * 84   12/2/11 12:39p jtna
 * SW7425-1863: add S3 standby support
 * 
 * 83   12/2/11 11:20a gmohile
 * SW7425-1183 : Add transport wake up
 * 
 * 82   11/8/11 7:32p bandrews
 * SW7231-446: fix for platforms without parser bands
 * 
 * 81   10/31/11 7:49p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/2   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * SW7420-2078/1   10/11/11 8:26p bandrews
 * SW7231-391: add support for parser band and timebase protected client
 *  usage
 * 
 * 80   10/10/11 4:49p katrep
 * SW7425-1427:xpt SW workaround,working on both user and kernel mode,
 *  automatic disable when workaround is not required on B0 chips
 * 
 * 79   10/4/11 2:22p gmohile
 * SW7425-533 : XPT standby function requires settings
 * 
 * 78   9/21/11 10:57a vsilyaev
 * SW7425-1323: Added rave timer function
 * 
 * 77   8/4/11 7:39p mphillip
 * SW7422-404: Update region verification code
 * 
 * 76   8/3/11 11:09a gmohile
 * SW7125-1014 : Add module to active priority scheduler
 * 
 * 75   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * SW7125-1014/1   7/20/11 4:37p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 74   7/12/11 10:24a erickson
 * SW7346-316: fix NEXUS_Transport_P_SetInterrupts logic
 * 
 * 73   7/1/11 12:50p erickson
 * SW7335-1216: allow chips with HW message filter to use
 *  NEXUS_USE_SW_FILTER option
 * 
 * 72   4/15/11 10:35a erickson
 * SW7425-313: add initial MTSIF support
 *
 * 71   4/4/11 10:38a gmullen
 * SW7346-119: Added BandwidthConfig compile-time check to
 *  NEXUS_TransportModule_GetDefaultSettings()
 *
 * 70   4/1/11 4:25p jtna
 * SW7346-119: add 40nm allpass support
 *
 * 69   3/24/11 2:59p ahulse
 * SW7346-124: Optimize Xc,Rs buffers according to platform_features for
 *  non legacy XPT
 *
 * 68   3/16/11 2:51p erickson
 * SW7422-377: add NEXUS_Transport_GetXpt_priv
 *
 * 67   3/8/11 6:08p vsilyaev
 * SW7422-101: Updated logic of handling continuityCountEnabled, so per
 *  band and per pid settings are combined with logical AND
 *
 * 66   3/3/11 12:43p gmohile
 * SW7408-210 : Fix warnings
 *
 * 65   3/1/11 6:23p vishk
 * SW7408-210 : Add standby support. Typo.
 *
 * 64   3/1/11 5:38p gmohile
 * SW7408-210 : Add standby support
 *
 * 63   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 62   1/17/11 9:31a erickson
 * SW7425-68: add NEXUS_TransportModuleSettings.mainHeapIndex
 *
 * 61   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 60   9/8/10 4:41p erickson
 * SW3548-3073: moved definition of struct NEXUS_Message into each
 *  implementation
 *
 * 59   7/14/10 5:37p erickson
 * SW3556-1145: add NEXUS_TimebaseTrackRange to all source type settings
 *
 * 58   7/6/10 10:58a erickson
 * SW7405-4047: implement jail_xvd/vdc/xpt/rap using
 *  BMRC_Monitor_JailByFilename
 *
 * 57   5/13/10 12:09p erickson
 * SW7550-437: check return code on BXPT_PCR_GetTimeBaseFreqRefDefaults
 *
 * 56   3/8/10 1:03p erickson
 * SW7405-4008: add BDBG_OBJECT_ASSERT's for message error interrupts
 *
 * 55   2/17/10 2:18p gmohile
 * SW3548-2697 : Conditional compile for input band support
 *
 * 54   2/17/10 12:09p maivu
 * SW3548-2697: Use parallel input value return from XPT
 *
 * 53   2/17/10 10:13a erickson
 * SW7335-675: added NEXUS_TransportModuleSettings.tpit settings for
 *  idleEventTimeout and recordEventTimeout
 *
 * 52   2/8/10 4:18p gmohile
 * SW7408-1 : Disable MPT for 7408
 *
 * 51   1/20/10 5:13p erickson
 * SW7550-159: allow RAVE to be configured with a non-default
 *  ThresholdGranularityInBytes. switching default to 128, which allows
 *  for an interrupt on one packet.
 *
 * 50   12/29/09 12:51p erickson
 * SW7550-146: allow security module to be initialized after the transport
 *  module if HW requires it
 *
 * 49   12/11/09 3:32p erickson
 * SW7550-112: merge 7550 code. add support for sw message filtering.
 *  allow for no HW message filtering using standard XPT define.
 *
 * 48   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 47   10/19/09 11:40a erickson
 * SW7400-2559: added NEXUS_TransportModuleSettings.dssSaveMptFlag
 *
 * 46   9/22/09 5:29p erickson
 * SWDEPRECATED-3717: move BXPT_Mesg_SetPid2Buff to system init time
 *
 * 45   9/4/09 12:49p erickson
 * SW7325-586: clear error status bits for message filters
 *
 * 44   8/28/09 1:16p erickson
 * SWDEPRECATED-3995: fix last checkin
 *
 * 43   8/26/09 8:07a erickson
 * SWDEPRECATED-3995: fixed last checkin
 *
 * 42   8/26/09 7:57a erickson
 * SWDEPRECATED-3995: fixed for pid2buf
 *
 * 41   8/19/09 2:52p erickson
 * PR57800: fix function declaration
 *
 * 40   7/31/09 2:47p gmohile
 * PR 56512 : Add 7401/03 support
 *
 * 39   6/8/09 11:49a erickson
 * PR53324: add PID2BUF impl, default off
 *
 * 38   5/22/09 9:35a erickson
 * PR54880: fix shutdown of stc channels
 *
 * 37   5/18/09 4:24p erickson
 * PR54880: allow StcChannels with different PCR_OFFSET contexts and STC's
 *
 * 36   4/20/09 10:53a erickson
 * PR53662: rework XPT power management code to call BXPT_P_CanPowerDown
 *  before powering down core
 *
 * 35   3/30/09 10:35a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 34   1/19/09 5:25p bandrews
 * PR51021: merge
 *
 * PR51021/2   1/16/09 4:13p bandrews
 * PR51021: Updated to separate permission from enable
 *
 * PR51021/1   1/16/09 12:18a bandrews
 * PR51021: First stab at cleaning up ASTM playback blockage
 *
 * 33   12/22/08 1:23p yili
 * PR42660:IPTV Integration
 *
 * 32   12/19/08 10:15a jgarrett
 * PR 49587: Fixing loop filter values to appropriate values by default
 *
 * 31   12/15/08 5:40p jtna
 * PR49927: add NEXUS_ParserBandSettings.lengthError
 *
 * 30   12/11/08 2:04p erickson
 * PR50113: fix warnings
 *
 * 29   12/11/08 8:05a vsilyaev
 * PR 48908: Changed algorithm to power up/down blocks
 *
 * 28   11/24/08 10:28a erickson
 * PR48846: added pcrError callback
 *
 * 27   11/11/08 2:55a erickson
 * PR 48847: added NEXUS_Message_SetSettings
 *
 * 26   10/30/08 10:59p erickson
 * PR47232: ensure WhichPcr does not exceed max # of DPCR's
 *
 * 25   10/24/08 4:42p erickson
 * PR47232: added trackRange for freeRun
 *
 * 24   10/7/08 10:40p erickson
 * PR47232: move centerFrequency to freeRun.centerFrequency'
 *
 * 23   10/6/08 2:35a erickson
 * PR47232: set pcr settings to match HW defaults
 *
 * 22   9/26/08 2:37p erickson
 * PR46513: backing out 3255 code. it belongs in
 *  nexus_platform_frontend.c.
 *
 * 21   9/23/08 9:16a jgarrett
 * PR 46513: Merging 97459 support to mainline
 *
 * PR44830/1   9/17/08 5:02p anilmm
 * PR44830: Input band clock polarity needs to flipped for non-fpga
 *  platforms
 *
 * 20   9/18/08 4:18p erickson
 * PR47111: fix warning
 *
 * 19   9/11/08 3:13p erickson
 * PR46646: added crcError, pesLengthError, pesStartCodeError
 *
 * 17   9/2/08 12:10p erickson
 * PR44165: assert that secure heap has been created
 *
 * 16   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 15   7/30/08 11:54a erickson
 * PR44919: reduce XPT memory consumption if user reduces # of available
 *  IB,PB and playbacks
 *
 * 14   6/30/08 1:45p erickson
 * PR44165: impl NEXUS_CORE_NUM_SECURE_HEAPS
 *
 * 13   6/25/08 4:30p katrep
 * PR44085: Program the block XPT RS/XC buffer blockout counters and
 *  according to packet size.
 *
 * 12   5/22/08 3:31p erickson
 * PR34925: add b_get_xpt for debug
 *
 * 11   5/1/08 2:55p erickson
 * PR42391: remove duplicate and unused storage to prevent future bugs
 *
 * 10   4/18/08 4:46p vsilyaev
 * PR 41868: Acquire module lock prior to calling API functions
 *
 * 9   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 8   4/10/08 1:58p erickson
 * PR40079: added PidChannel ScramblingCheck
 *
 * 7   4/2/08 11:51a katrep
 * PR40851: Internal sync detect enable should be on for mpeg TS, this
 *  also the default setting
 *
 * 6   3/24/08 10:12p bandrews
 * PR40090: pcr received moved
 *
 * 5   3/24/08 9:14p bandrews
 * PR40090: stc channel astm private settings
 *
 * 3   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 * 2   2/20/08 9:28a erickson
 * PR39764: resolved unused path static analysis warning
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/23   12/4/07 5:20p erickson
 * PR36068: default playpump parserband transporttype
 *
 * Nexus_Devel/22   11/19/07 4:33p erickson
 * PR34925: added NEXUS_TimebaseStatus
 *
 * Nexus_Devel/21   11/16/07 9:38a erickson
 * PR34925: remove SET to Open
 *
 * Nexus_Devel/20   11/15/07 3:55p erickson
 * PR37137: moved HSM code to Security module
 *
 * Nexus_Devel/19   11/15/07 10:39a erickson
 * PR34925: make NEXUS_InputBand_GetDefaultSettings private
 *
 * Nexus_Devel/18   11/6/07 5:49p erickson
 * PR34925: added BDBG_OBJECT checking
 *
 * Nexus_Devel/17   11/6/07 2:28p rrlee
 * PR36754: Allow init to succeed even if OTP not programmed
 *
 * Nexus_Devel/16   11/1/07 9:06a erickson
 * PR36633: update base enums
 *
 * Nexus_Devel/15   10/31/07 9:07a erickson
 * PR36570: added 3563 support
 *
 * Nexus_Devel/14   10/5/07 1:53p erickson
 * PR34925: added NEXUS_ParserBandSourceType to ParserBandSettings
 *
 * Nexus_Devel/13   9/28/07 11:41a jgarrett
 * PR 35002: Adding input band defaults
 *
 * Nexus_Devel/12   9/28/07 10:00a erickson
 * PR34925: 0.1 prerel review
 *
 * Nexus_Devel/11   9/27/07 7:51p jgarrett
 * PR 34954: Adding HSM for RAVE
 *
 * Nexus_Devel/10   9/25/07 3:07p erickson
 * PR34925: update
 *
 * Nexus_Devel/9   9/25/07 1:46p erickson
 * PR34925: fix warnings
 *
 * Nexus_Devel/8   9/24/07 2:36p erickson
 * PR34925: added Message
 *
 * Nexus_Devel/7   9/21/07 4:19p erickson
 * PR32925: update
 *
 * Nexus_Devel/6   9/17/07 4:12p erickson
 * PR34925: added timebase and stcchannel
 *
 * Nexus_Devel/5   9/14/07 4:32p erickson
 * PR34925: added BDBG_MODULE
 *
 * Nexus_Devel/4   9/14/07 3:27p erickson
 * PR34925: initial compilation
 *
 * Nexus_Devel/3   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#include "nexus_transport_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_tsmf_priv.h"
#if !BXPT_HAS_MESG_BUFFERS && !NEXUS_USE_SW_FILTER
#error chips without message filtering require NEXUS_USE_SW_FILTER
#endif
#if !NEXUS_USE_SW_FILTER
#include "bchp_int_id_xpt_msg.h"
#endif
#ifdef NEXUS_SECURITY_RAVE_VERIFICATION
#include "nexus_transport_security.h"
#endif
#include "nexus_power_management.h"
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
#include "bchp_int_id_xpt_wakeup.h"
#endif

BDBG_MODULE(nexus_transport_module);

/* global instances */
struct NEXUS_Transport_P_State g_NEXUS_Transport_P_State;
BDBG_OBJECT_ID(NEXUS_Rave);

#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
static NEXUS_ThreadHandle dpcr_integrator_workaround_thread;
static bool dpcr_integrator_workaround_thread_exit;

static BERR_Code do_dpcr_integrator_workaround (void)
{
    return BXPT_PCR_P_Integrator( pTransport->xpt );
}

static void dpcr_integrator_workaround_thread_entry(void *arg)
{
    NEXUS_Time prev;
    unsigned count=0;
    long max = 0;

    BSTD_UNUSED(arg);

    BDBG_WRN(("DPCR Thread: started"));
    NEXUS_Time_Get(&prev);
    for(;;) {
        NEXUS_Time now;
        long diff;
        BERR_Code rc = BERR_SUCCESS;

        BKNI_Sleep(2);  /* 2 mS */
        NEXUS_LockModule();
        if(dpcr_integrator_workaround_thread_exit) {
            NEXUS_UnlockModule();
            break;
        }
        rc = do_dpcr_integrator_workaround();
        if(rc!=BERR_SUCCESS) {
            break;
        }
        NEXUS_UnlockModule();
        NEXUS_Time_Get(&now);
        diff = NEXUS_Time_Diff(&now, &prev);
        prev = now;
        max = diff>max ? diff:max;
        count++;
    }
    BDBG_WRN(("DPCR Thread: Called %u times, max delay %ldms", count, max));
    return;
}

static void dpcr_integrator_workaround_start(void)
{
    NEXUS_ThreadSettings settings;
    NEXUS_Thread_GetDefaultSettings(&settings);
    dpcr_integrator_workaround_thread_exit = false;
    dpcr_integrator_workaround_thread = NEXUS_Thread_Create("dpcr_integrator_workaround", 
        dpcr_integrator_workaround_thread_entry, NULL, &settings);
    return;
}

static void dpcr_integrator_workaround_stop(void)
{
    dpcr_integrator_workaround_thread_exit = true;
    if (dpcr_integrator_workaround_thread) {
        NEXUS_UnlockModule();
        NEXUS_Thread_Destroy(dpcr_integrator_workaround_thread);
        NEXUS_LockModule();
        dpcr_integrator_workaround_thread = NULL;
    }
    return;
}
#else /* BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND */
#define dpcr_integrator_workaround_start()
#define dpcr_integrator_workaround_stop()
#endif /* BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND */

#if BXPT_SW7425_1323_WORKAROUND
static NEXUS_ThreadHandle timer_thread;
static bool timer_thread_exit;

static BERR_Code do_flow_control(BXPT_Rave_Handle hRave)
{
    return BXPT_Rave_P_FlowControl( hRave );
}

static void timer_thread_entry(void *arg)
{
    NEXUS_Time prev;
    unsigned count=0;
    long max = 0;

    BSTD_UNUSED(arg);

    BDBG_WRN(("Timer: started"));
    NEXUS_Time_Get(&prev);
    for(;;) {
        NEXUS_Time now;
        long diff;
        unsigned i;
        BERR_Code rc = BERR_SUCCESS;

        BKNI_Sleep(1);
        NEXUS_LockModule();
        if(timer_thread_exit) {
            NEXUS_UnlockModule();
            break;
        }
        for(i=0;i<NEXUS_NUM_RAVE_CHANNELS;i++) {
            rc = do_flow_control(pTransport->rave[i].channel);
            if(rc!=BERR_SUCCESS) {
                break;
            }
        }
        NEXUS_UnlockModule();
        NEXUS_Time_Get(&now);
        diff = NEXUS_Time_Diff(&now, &prev);
        prev = now;
        max = diff>max ? diff:max;
        count ++;
        if(rc!=BERR_SUCCESS) {
            break;
        }
    }
    BDBG_WRN(("Timer: Called %u times, max delay %ldms", count, max));
    return;
}

static void timer_start(void)
{
    NEXUS_ThreadSettings settings;
    NEXUS_Thread_GetDefaultSettings(&settings);
    timer_thread_exit = false;
    timer_thread = NEXUS_Thread_Create("playback_pacing", timer_thread_entry, NULL, &settings);
    return;
}

static void timer_stop(void)
{
    timer_thread_exit = true;
    if (timer_thread) {
        NEXUS_UnlockModule();
        NEXUS_Thread_Destroy(timer_thread);
        NEXUS_LockModule();
        timer_thread = NULL;
    }
    return;
}
#else /* BXPT_SW7425_1323_WORKAROUND */
#define timer_start()
#define timer_stop()
#endif /* BXPT_SW7425_1323_WORKAROUND */

#if BXPT_HAS_PIPELINE_ERROR_REPORTING
static NEXUS_TimerHandle pipeline_error_checker_timer;
static void pipeline_checker_timer(void *arg)
{
    int rc;
    BSTD_UNUSED(arg);
    pipeline_error_checker_timer = NULL;
    BDBG_MSG(("check pipeline"));
    rc = BXPT_CheckPipelineErrors( pTransport->xpt );
    if (rc != BERR_NOT_SUPPORTED) {
        pipeline_error_checker_timer = NEXUS_ScheduleTimer(2000, pipeline_checker_timer, NULL);
    }
}

static void pipeline_checker_start(void)
{
    BDBG_ASSERT(!pipeline_error_checker_timer);
    pipeline_error_checker_timer = NEXUS_ScheduleTimer(2000, pipeline_checker_timer, NULL);
}

static void pipeline_checker_stop(void)
{
    if (pipeline_error_checker_timer) {
        NEXUS_CancelTimer(pipeline_error_checker_timer);
        pipeline_error_checker_timer = NULL;
    }
}
#else /* BXPT_HAS_PIPELINE_ERROR_REPORTING */
#define pipeline_checker_start()
#define pipeline_checker_stop()
#endif /* BXPT_HAS_PIPELINE_ERROR_REPORTING */

#if BXPT_HAS_WAKEUP_PKT_SUPPORT
static void NEXUS_Transport_P_PacketFound_isr(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BXPT_Wakeup_ClearInterruptToPMU(pTransport->xpt);  
    NEXUS_IsrCallback_Fire_isr(pTransport->wakeup.wakeupCallback);
}
#endif

void NEXUS_TransportModule_GetDefaultSettings(NEXUS_TransportModuleSettings *pSettings)
{
    BXPT_DefaultSettings xptsettings;
    unsigned i, j;

    BSTD_UNUSED(i); /* in case not used */
    BSTD_UNUSED(j); /* in case not used */
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    /* we don't have a chp handle here if called from NEXUS_Platform_GetDefaultSettings.
       BXPT_GetDefaultSettings doesn't use the chp handle so pass in a bogus one */
    BXPT_GetDefaultSettings(&xptsettings, (BCHP_Handle)1);

#if (BXPT_HAS_FIXED_RSBUF_CONFIG || BXPT_HAS_FIXED_XCBUF_CONFIG)
    #if NEXUS_NUM_PARSER_BANDS
    for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
        pSettings->maxDataRate.parserBand[i] = xptsettings.BandwidthConfig.MaxInputRate[i];
        pSettings->clientEnabled.parserBand[i].rave = xptsettings.BandwidthConfig.IbParserClients[i].ToRave;
        pSettings->clientEnabled.parserBand[i].message = xptsettings.BandwidthConfig.IbParserClients[i].ToMsg;
        #if NEXUS_NUM_REMUX
        for( j = 0; j < NEXUS_NUM_REMUX; j++ ) 
        {
            pSettings->clientEnabled.parserBand[i].remux[ j ] = xptsettings.BandwidthConfig.IbParserClients[i].ToRmx[ j ];
        }
        #endif
        pSettings->clientEnabled.parserBand[i].mpodRs = xptsettings.BandwidthConfig.IbParserClients[i].ToMpodRs;
    }
    #endif

    #if NEXUS_NUM_PLAYPUMPS && BXPT_HAS_PLAYBACK_PARSERS
    for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) 
    {
        pSettings->clientEnabled.playback[i].rave = xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRave;
        pSettings->clientEnabled.playback[i].message = xptsettings.BandwidthConfig.PlaybackParserClients[i].ToMsg;
        #if NEXUS_NUM_REMUX
        for( j = 0; j < NEXUS_NUM_REMUX; j++ ) 
        {
            pSettings->clientEnabled.playback[i].remux[ j ] = xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRmx[ j ];
        }
        #endif
    }
    #endif
#endif

#if NEXUS_NUM_MTSIF
    BDBG_CASSERT(NEXUS_NUM_MTSIF <= BXPT_NUM_MTSIF);
    for (i=0;i<NEXUS_NUM_MTSIF;i++) {
        pSettings->mtsif[i].enabled = true; /* XPT defaults to disabled, but nexus defaults to enabled if NEXUS_NUM_MTSIF>0*/
        pSettings->mtsif[i].rxInterfaceWidth = xptsettings.MtsifConfig[i].RxInterfaceWidth;
    }
#endif

#if BXPT_HAS_PLAYBACK_PARSERS
    for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) {
        pSettings->maxDataRate.playback[i] = xptsettings.BandwidthConfig.MaxPlaybackRate[i];
    }
#endif

    return;
}

void NEXUS_InputBand_P_GetDefaultSettings(NEXUS_InputBand inputBand, NEXUS_InputBandSettings *pSettings)
{
#if NEXUS_NUM_INPUT_BANDS
    BXPT_InputBandConfig cfg;
    BERR_Code rc;
#else
    BSTD_UNUSED(inputBand);
#endif

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* Defaults taken from RDB defaults for now -- needs a BXPT default function */
    pSettings->clockActiveHigh = true;
    pSettings->dataLsbFirst = false;
    pSettings->dataActiveHigh = true;
    pSettings->validEnabled = true;
    pSettings->validActiveHigh = true;
    pSettings->useInternalSync = true;   /* internal sync detect should be on for MPEG TS by default */
    pSettings->syncActiveHigh = true;
    pSettings->errorEnabled = false;
    pSettings->errorActiveHigh = true;
    pSettings->packetLength = 188;

#if NEXUS_NUM_INPUT_BANDS
    rc = BXPT_GetInputBandConfig(pTransport->xpt, inputBand, &cfg);
    if (!rc) {
        pSettings->parallelInput = cfg.ParallelInputSel;
    }
    else {
        pSettings->parallelInput = false;
    }
#endif
}

#if NEXUS_NUM_PARSER_BANDS
static void NEXUS_ParserBand_P_GetDefaultSettings(unsigned index, NEXUS_ParserBandSettings * pSettings)
{
    /* set default state */
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_ParserBandSettings));
    pSettings->transportType = NEXUS_TransportType_eTs;
    pSettings->continuityCountEnabled = true;

    pSettings->sourceType = NEXUS_ParserBandSourceType_eInputBand;
#if NEXUS_NUM_INPUT_BANDS >= NEXUS_NUM_PARSER_BANDS
    pSettings->sourceTypeSettings.inputBand = index; /* straight mapping */
#else
    pSettings->sourceTypeSettings.inputBand = index<NEXUS_NUM_INPUT_BANDS?index:0; /* straight mapping for as many as possible */
#endif
#if NEXUS_HAS_LEGACY_XPT
    pSettings->maxDataRate = 25000000; /* 25 Mbps */
#else
    pSettings->maxDataRate = pTransport->settings.maxDataRate.parserBand[index];
#endif
}

static NEXUS_Error NEXUS_ParserBand_P_Init(unsigned index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_ParserBandHandle parserBand;
    
    parserBand = &pTransport->parserBand[index];
    BDBG_OBJECT_SET(parserBand, NEXUS_ParserBand);

    NEXUS_ParserBand_P_GetDefaultSettings(index, &parserBandSettings);

    parserBand->hwIndex = index;
    parserBand->enumBand = NEXUS_ParserBand_e0 + index; /* assumes enums are continuous */
    parserBand->pidChannels = 0;
    parserBand->enabled = false;
    parserBand->settings = parserBandSettings;
    parserBand->teiErrorCallback = NEXUS_IsrCallback_Create(parserBand, NULL);
    parserBand->ccErrorCallback = NEXUS_IsrCallback_Create(parserBand, NULL);
    parserBand->lengthErrorCallback = NEXUS_IsrCallback_Create(parserBand, NULL);
    rc = NEXUS_ParserBand_P_SetSettings(parserBand, &parserBandSettings);
    return rc;
}

static void NEXUS_ParserBand_P_Uninit(NEXUS_ParserBandHandle parserBand)
{
    NEXUS_IsrCallback_Destroy(parserBand->teiErrorCallback);
    NEXUS_IsrCallback_Destroy(parserBand->ccErrorCallback);
    NEXUS_IsrCallback_Destroy(parserBand->lengthErrorCallback);
    if (parserBand->refcnt) {
        BDBG_WRN(("parser band %u still in use", parserBand->hwIndex));
    }
    BDBG_OBJECT_UNSET(parserBand, NEXUS_ParserBand);
}
#endif

#if NEXUS_NUM_TIMEBASES
static NEXUS_Error NEXUS_Timebase_P_Init(unsigned index)
{
    BERR_Code  rc = BERR_SUCCESS;
    BXPT_PCR_DefSettings pcr_cfg;
    NEXUS_TimebaseHandle timebase;

    timebase = &pTransport->timebase[index];

    BDBG_OBJECT_SET(timebase, NEXUS_Timebase);

    rc = BXPT_PCR_GetChannelDefSettings(pTransport->xpt, index, &pcr_cfg);
    if (rc) {rc=BERR_TRACE(rc); goto done;}
    rc = BXPT_PCR_Open(pTransport->xpt, index, &pcr_cfg, &timebase->pcr);
    if (rc) {rc=BERR_TRACE(rc); goto done;}
    
    NEXUS_Timebase_GetDefaultSettings_priv(&timebase->settings);

    timebase->hwIndex = index;
    timebase->enumTimebase = NEXUS_Timebase_e0 + index; /* assumes enums are continuous */
    timebase->monitorCallback = NEXUS_IsrCallback_Create(timebase, NULL);
    timebase->pcrErrorCallback = NEXUS_IsrCallback_Create(timebase, NULL);
#if NEXUS_HAS_ASTM
    timebase->astm.permitted = false;
    BKNI_Memset(&timebase->astm.settings, 0, sizeof(NEXUS_TimebaseAstmSettings));
#endif

    NEXUS_Timebase_P_StartMonitor(timebase);

done:
    return rc;
}

static void NEXUS_Timebase_P_Uninit(NEXUS_TimebaseHandle timebase)
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_Timebase_P_StopMonitor(timebase);
    NEXUS_IsrCallback_Destroy(timebase->monitorCallback);
    NEXUS_IsrCallback_Destroy(timebase->pcrErrorCallback);
    rc = BXPT_PCR_Close(timebase->pcr);
    if (rc) rc=BERR_TRACE(rc); /* keep going */
    BDBG_OBJECT_UNSET(timebase, NEXUS_Timebase);
}
#endif /* NEXUS_NUM_TIMEBASES */

NEXUS_ModuleHandle NEXUS_TransportModule_Init(const NEXUS_TransportModuleSettings *pSettings)
{
    BERR_Code rc;
    BXPT_DefaultSettings xptsettings;
    BXPT_Handle xpt; /* shorthand */
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    BXPT_Wakeup_Settings wakeupSettings;
#endif
    NEXUS_ModuleSettings moduleSettings;
    unsigned i, j;

    BSTD_UNUSED(j); /* in case not used */

    if(!pSettings) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    /* XXX Verify pSettings->dma */

    /* init global module data */
    BKNI_Memset(&g_NEXUS_Transport_P_State, 0, sizeof(g_NEXUS_Transport_P_State));
#if NEXUS_HAS_TSMF
    BKNI_Memset(&g_NEXUS_Tsmf_P_State, 0, sizeof(g_NEXUS_Tsmf_P_State));
#endif

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eHighActiveStandby; /* transport interface is fast and is powered up in active standby mode*/
    g_NEXUS_Transport_P_State.transportModule = NEXUS_Module_Create("transport", &moduleSettings);
    if (!g_NEXUS_Transport_P_State.transportModule) {
          return NULL;
    }
    NEXUS_LockModule();
    pTransport->settings = *pSettings;
    
    if ( NEXUS_GetEnv("jail_xpt") ) {
        NEXUS_MemoryStatus memStatus;
        (void)NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[0], &memStatus);
        BMRC_Monitor_JailByFilename(g_pCoreHandles->memc[memStatus.memcIndex].rmm, "bxpt_");
        BMRC_Monitor_JailUpdate(g_pCoreHandles->memc[memStatus.memcIndex].rmm);
    }

    BXPT_GetDefaultSettings(&xptsettings, g_pCoreHandles->chp);

    if (pSettings->secureHeap) {
        xptsettings.hRHeap = NEXUS_Heap_GetMemHandle(pSettings->secureHeap);
    }

    /* verify API macros aren't below nexus_platform_features.h */
    BDBG_CASSERT(NEXUS_MAX_PARSER_BANDS >= NEXUS_NUM_PARSER_BANDS);
    BDBG_CASSERT(NEXUS_MAX_PLAYPUMPS >= NEXUS_NUM_PLAYPUMPS);
#if NEXUS_NUM_MTSIF
    BDBG_CASSERT(NEXUS_MAX_MTSIF >= NEXUS_NUM_MTSIF);
#endif
#if NEXUS_NUM_REMUX
    BDBG_CASSERT(NEXUS_MAX_REMUX >= NEXUS_NUM_REMUX);
#endif
    
    BDBG_CASSERT(NEXUS_NUM_PARSER_BANDS <= BXPT_NUM_PID_PARSERS);
    BDBG_CASSERT(NEXUS_NUM_PLAYPUMPS <= BXPT_NUM_PLAYBACKS);
#if NEXUS_NUM_REMUX
    BDBG_CASSERT(NEXUS_NUM_REMUX <= BXPT_NUM_REMULTIPLEXORS);
#endif

#if (BXPT_HAS_FIXED_RSBUF_CONFIG || BXPT_HAS_FIXED_XCBUF_CONFIG)

    #if NEXUS_NUM_PARSER_BANDS
    for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
        xptsettings.BandwidthConfig.MaxInputRate[i] = pSettings->maxDataRate.parserBand[i];
        xptsettings.BandwidthConfig.IbParserClients[i].ToRave = pSettings->clientEnabled.parserBand[i].rave;
        xptsettings.BandwidthConfig.IbParserClients[i].ToMsg = pSettings->clientEnabled.parserBand[i].message;
        #if BXPT_HAS_REMUX
        #if NEXUS_NUM_REMUX
        for(j=0;j<NEXUS_NUM_REMUX;j++)
        {
            xptsettings.BandwidthConfig.IbParserClients[i].ToRmx[ j ] = pSettings->clientEnabled.parserBand[i].remux[ j ];
        }
        #endif
        #if (NEXUS_NUM_REMUX<BXPT_NUM_REMULTIPLEXORS)
        for (j=NEXUS_NUM_REMUX;j<BXPT_NUM_REMULTIPLEXORS;j++)
        {
            xptsettings.BandwidthConfig.IbParserClients[i].ToRmx[ j ] = false;
        }
        #endif
        #endif
        xptsettings.BandwidthConfig.IbParserClients[i].ToMpodRs = pSettings->clientEnabled.parserBand[i].mpodRs;
    }
    #endif

#if NEXUS_NUM_PARSER_BANDS < BXPT_NUM_PID_PARSERS
    for (i=NEXUS_NUM_PARSER_BANDS;i<BXPT_NUM_PID_PARSERS;i++) {
        xptsettings.BandwidthConfig.MaxInputRate[i] = 0;
        xptsettings.BandwidthConfig.IbParserClients[i].ToRave = false;
        xptsettings.BandwidthConfig.IbParserClients[i].ToMsg = false;
        #if BXPT_HAS_REMUX
        for (j=0;j<BXPT_NUM_REMULTIPLEXORS;j++)
        {
            xptsettings.BandwidthConfig.IbParserClients[i].ToRmx[ j ] = false;
        }
        #endif
        xptsettings.BandwidthConfig.IbParserClients[i].ToMpodRs = false;
    }
#endif

    for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) {
        xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRave = pSettings->clientEnabled.playback[i].rave;
        xptsettings.BandwidthConfig.PlaybackParserClients[i].ToMsg = pSettings->clientEnabled.playback[i].message;
        #if BXPT_HAS_REMUX
        #if NEXUS_NUM_REMUX
        for(j=0;j<NEXUS_NUM_REMUX;j++)
        {
            xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRmx[ j ] = pSettings->clientEnabled.playback[i].remux[ j ];
        }
        #endif
        #if (NEXUS_NUM_REMUX<BXPT_NUM_REMULTIPLEXORS)
        for (j=NEXUS_NUM_REMUX;j<BXPT_NUM_REMULTIPLEXORS;j++)
        {
            xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRmx[ j ] = false;
        }
        #endif
        #endif
    }
    for (i=0;i<NEXUS_NUM_PLAYPUMPS;i++) {
        xptsettings.BandwidthConfig.MaxPlaybackRate[i] = pSettings->maxDataRate.playback[i];
    }

#if NEXUS_NUM_PLAYPUMPS < BXPT_NUM_PLAYBACKS
    for (i=NEXUS_NUM_PLAYPUMPS;i<BXPT_NUM_PLAYBACKS;i++) {
        xptsettings.BandwidthConfig.MaxPlaybackRate[i] = 0;
        xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRave = false;
        xptsettings.BandwidthConfig.PlaybackParserClients[i].ToMsg = false;
        #if BXPT_HAS_REMUX
        for (j=0;j<BXPT_NUM_REMULTIPLEXORS;j++) 
        {
            xptsettings.BandwidthConfig.PlaybackParserClients[i].ToRmx[ j ] = false;
        }
        #endif
    }
#endif

#if NEXUS_NUM_REMUX < BXPT_NUM_REMULTIPLEXORS
    for (i=NEXUS_NUM_REMUX;i<BXPT_NUM_REMULTIPLEXORS;i++) {
        xptsettings.BandwidthConfig.RemuxUsed[i] = false;
    }
#endif

#else /* NEXUS_HAS_LEGACY_XPT */

    /* reduce XPT memory if user has limited capabilities */
#if NEXUS_NUM_PARSER_BANDS < BXPT_NUM_PID_PARSERS
    for (i=NEXUS_NUM_PARSER_BANDS;i<BXPT_NUM_PID_PARSERS;i++) {
        xptsettings.DramBuffers.IbParserRsSize[i] = 0;
        xptsettings.DramBuffers.RaveXcCfg.IbParserXcSize[i] = 0;
        xptsettings.DramBuffers.MesgBufXcCfg.IbParserXcSize[i] = 0;
    }
#endif
#if NEXUS_NUM_PLAYPUMPS < BXPT_NUM_PLAYBACKS
    for (i=NEXUS_NUM_PLAYPUMPS;i<BXPT_NUM_PLAYBACKS;i++) {
        xptsettings.DramBuffers.PbParserRsSize[i] = 0;
        xptsettings.DramBuffers.RaveXcCfg.PbParserXcSize[i] = 0;
        xptsettings.DramBuffers.MesgBufXcCfg.PbParserXcSize[i] = 0;
    }
#endif
#if NEXUS_NUM_REMUX < BXPT_NUM_REMULTIPLEXORS
    for (i=NEXUS_NUM_REMUX;i<BXPT_NUM_REMULTIPLEXORS;i++) {
        xptsettings.DramBuffers.RemuxXcCfg[i].BandAXcSize = 0;
        xptsettings.DramBuffers.RemuxXcCfg[i].BandBXcSize = 0;
    }
#endif
#endif

#if NEXUS_NUM_MTSIF
    BDBG_CASSERT(NEXUS_NUM_MTSIF <= BXPT_NUM_MTSIF);
    for (i=0;i<NEXUS_NUM_MTSIF;i++) {
        xptsettings.MtsifConfig[i].Enable = pSettings->mtsif[i].enabled;
        xptsettings.MtsifConfig[i].RxInterfaceWidth = pSettings->mtsif[i].rxInterfaceWidth;
    }
#endif

    rc = BXPT_Open(&xpt, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[pTransport->settings.mainHeapIndex], g_pCoreHandles->bint, &xptsettings);
    if (rc) {rc=BERR_TRACE(rc); goto done;}

    pTransport->xpt = xpt;

#if B_HAS_DSS && !NEXUS_USE_SW_FILTER
    BXPT_DirecTv_SaveMptFlag(pTransport->xpt, pSettings->dssSaveMptFlag);
#endif

#if NEXUS_NUM_TIMEBASES
    BDBG_CASSERT(NEXUS_Timebase_eMax >= NEXUS_NUM_TIMEBASES); /* make sure no platform has gone beyond the enums */
    for (i=0;i<NEXUS_NUM_TIMEBASES;i++) {
        rc = NEXUS_Timebase_P_Init(i);
        if (rc) {rc=BERR_TRACE(rc); goto done;}
    }
#endif

#if NEXUS_NUM_RAVE_CHANNELS
    for (i=0;i<NEXUS_NUM_RAVE_CHANNELS;i++) {
        BXPT_Rave_ChannelSettings rave_channel_settings;

        rc = BXPT_Rave_GetChannelDefaultSettings(xpt, i, &rave_channel_settings);
        if (rc) {rc=BERR_TRACE(rc); goto done;}
#ifdef NEXUS_SECURITY_RAVE_VERIFICATION
        NEXUS_Rave_P_EnableFwVerification (&rave_channel_settings);
#endif
        rave_channel_settings.ThresholdGranularityInBytes = NEXUS_RAVE_THRESHOLD_UNITS;
        /* for nexus naming, I kept the NEXUS_TransportModuleSettings and NEXUS_RecpumpSettings names in sync. this means the naming
        doesn't match magnum exactly. */
        rave_channel_settings.TpitEventTimeout = pSettings->tpit.idleEventTimeout;
        rave_channel_settings.TpitPacketTimeout = pSettings->tpit.recordEventTimeout;
        rc = BXPT_Rave_OpenChannel(xpt, &pTransport->rave[i].channel, i, &rave_channel_settings);
        if (rc) {rc=BERR_TRACE(rc); goto done;}

        /* contexts are opened with the RAVE interface */
        BKNI_Memset(pTransport->rave[i].context, 0, sizeof(pTransport->rave[i].context));
    }
#endif

#if NEXUS_NUM_INPUT_BANDS
    for (i=0;i<NEXUS_NUM_INPUT_BANDS;i++) {
        NEXUS_InputBandSettings settings;
        /* Set defaults */
        pTransport->inputBand[i].enabled = false;

        NEXUS_InputBand_P_GetDefaultSettings(i, &settings);
        NEXUS_InputBand_SetSettings(i, &settings);
    }
#endif
#if NEXUS_NUM_PARSER_BANDS
    BDBG_CASSERT(NEXUS_ParserBand_eMax >= NEXUS_NUM_PARSER_BANDS); /* make sure no platform has gone beyond the enums */
    for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
        rc = NEXUS_ParserBand_P_Init(i);
        if (rc) { rc = BERR_TRACE(rc); goto done; }
    }
#endif

    rc = BPCRlib_Open(&pTransport->pcrlib, g_pCoreHandles->chp);
    if (rc) {rc=BERR_TRACE(rc); goto done;}

#if NEXUS_USE_MSGLIB
    {
    BMSGlib_Settings msglibSettings;
    BMSGlib_GetDefaultSettings(&msglibSettings);
    msglibSettings.mem = g_pCoreHandles->heap[pTransport->settings.mainHeapIndex];
    msglibSettings.xpt = pTransport->xpt;
    rc = BMSGlib_Open(&pTransport->msglib, &msglibSettings);
    if (rc) {rc=BERR_TRACE(rc); goto done;}
    }
#else
    /* required to be called once before use. */
#if NEXUS_HAS_LEGACY_XPT
#if !NEXUS_USE_SW_FILTER
    BXPT_Mesg_SetPid2Buff(pTransport->xpt, true);
#endif
#endif
#endif

#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    BXPT_Wakeup_GetDefaults(&wakeupSettings ); 
    pTransport->wakeup.settings.inputBand = wakeupSettings.InputBand;
    pTransport->wakeup.settings.packetLength = wakeupSettings.PacketLength;
    pTransport->wakeup.settings.errorIgnore = wakeupSettings.ErrorInputIgnore;
    /* Create a callback for packet wakeup interrupt */
    rc = BINT_CreateCallback(&pTransport->wakeup.intPacketFoundCallback, g_pCoreHandles->bint, BCHP_INT_ID_PKT_DETECT, NEXUS_Transport_P_PacketFound_isr, NULL, 0);
    if (rc) {rc = BERR_TRACE(rc); goto done;}    
    pTransport->wakeup.wakeupCallback = NEXUS_IsrCallback_Create(pTransport, NULL);
    if (rc) {rc = BERR_TRACE(rc); goto done;}     
#endif

    timer_start();
    pipeline_checker_start();
    dpcr_integrator_workaround_start();
    
done:
    NEXUS_UnlockModule();
    if (rc) {
        /* TODO: unwind Init? */
        return NULL;
    }
    return pTransport->transportModule;
}

void NEXUS_TransportModule_Uninit(void)
{
    unsigned i;
    BERR_Code rc;
    NEXUS_PidChannelHandle pidChannel;

    NEXUS_LockModule();
    BDBG_ASSERT(pTransport->transportModule);

    timer_stop();
    pipeline_checker_stop();
    dpcr_integrator_workaround_stop();
    
#if BXPT_HAS_WAKEUP_PKT_SUPPORT    
    if(pTransport->wakeup.wakeupCallback) {
        NEXUS_IsrCallback_Destroy(pTransport->wakeup.wakeupCallback);
    }
    BINT_DestroyCallback(pTransport->wakeup.intPacketFoundCallback);
#endif    

#if NEXUS_USE_MSGLIB
    BMSGlib_Close(pTransport->msglib);
#endif

#if NEXUS_NUM_RAVE_CHANNELS
    for (i=0;i<NEXUS_NUM_RAVE_CHANNELS;i++) {
        rc = BXPT_Rave_CloseChannel(pTransport->rave[i].channel);
        if (rc) rc=BERR_TRACE(rc); /* keep going */
    }
#endif
#if NEXUS_NUM_STC_CHANNELS
    for (i=0;i<NEXUS_NUM_STC_CHANNELS;i++) {
        if (pTransport->stcChannel[i].pcrOffset) {
            NEXUS_StcChannel_Close(&pTransport->stcChannel[i]);
        }
    }
#endif
    BPCRlib_Close(pTransport->pcrlib);
#if NEXUS_NUM_TIMEBASES
    for (i=0;i<NEXUS_NUM_TIMEBASES;i++) {
        NEXUS_Timebase_P_Uninit(&pTransport->timebase[i]);
    }
#endif
    while ((pidChannel = BLST_S_FIRST(&pTransport->pidChannels))) {
        BDBG_WRN(("pid channel %p left open. closing.", pidChannel));
        NEXUS_PidChannel_Close(pidChannel);
    }
    /* HW pid channel refcnt ought to be at zero now */
    for (i=0;i<NEXUS_NUM_PID_CHANNELS;i++) {
        BDBG_ASSERT(!pTransport->hwPidChannelRefCnt[i]);
    }

#if NEXUS_NUM_PARSER_BANDS
    for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
        NEXUS_ParserBand_P_Uninit(&pTransport->parserBand[i]);
    }
#endif

    BXPT_Close(pTransport->xpt);
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(pTransport->transportModule);
    pTransport->transportModule = NULL;
    return;
}

/****
The following code handles XPT interrupts and dispatches based on a looked-up pid channel
****/

#if !NEXUS_USE_SW_FILTER
static void NEXUS_Transport_P_FireInterrupt_isr(unsigned pidChannelIndex, NEXUS_XptDataInterrupt xptDataInterrupt)
{
    unsigned i;
    for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
        NEXUS_MessageHandle msg = pTransport->message.handle[i];
        if (msg) {
            NEXUS_Message_P_FireInterrupt_isr(msg, pidChannelIndex, xptDataInterrupt);
        }
    }
}

static void NEXUS_Transport_P_MessageError_isr(void *context, int param)
{
    unsigned pid_channel_group;

    BSTD_UNUSED(context);
    for (pid_channel_group=0;pid_channel_group<4;pid_channel_group++) {
        unsigned i;
        uint32_t addr = BCHP_XPT_MSG_BUF_ERR_00_31 + sizeof(uint32_t)*pid_channel_group;
        uint32_t status = BREG_Read32(g_pCoreHandles->reg, addr);
        BREG_Write32(g_pCoreHandles->reg, addr, 0);
        for (i=0;i<32;i++) {
            if (status & (1<<i)) {
                unsigned pidChannelIndex = pid_channel_group * 32 + i;
                NEXUS_Transport_P_FireInterrupt_isr(pidChannelIndex, param);
            }
        }
    }
}
#endif /* !NEXUS_USE_SW_FILTER */

void NEXUS_Transport_P_SetInterrupts(void)
{
#if !NEXUS_USE_SW_FILTER
    BERR_Code rc;
    NEXUS_XptDataInterrupt xptDataInterrupt;

    for (xptDataInterrupt=0;xptDataInterrupt<NEXUS_XptDataInterrupt_eMax;xptDataInterrupt++) {
        unsigned i;
        bool hasCallback = false;
        for (i=0;i<NEXUS_TRANSPORT_MAX_MESSAGE_HANDLES;i++) {
            NEXUS_MessageHandle msg = pTransport->message.handle[i];
            if (msg) {
                hasCallback = NEXUS_Message_P_HasCallback(msg, xptDataInterrupt);
                if (hasCallback) 
                    break;
            }
        }

        if (hasCallback) {
            if (!pTransport->message.xptDataInterrupt[xptDataInterrupt]) {
                unsigned intId;
                switch (xptDataInterrupt) {
                case NEXUS_XptDataInterrupt_ePsiLengthError: intId = BCHP_INT_ID_PSI_ERROR; break;
                case NEXUS_XptDataInterrupt_eCrcError: intId = BCHP_INT_ID_CRC_ERROR; break;
                case NEXUS_XptDataInterrupt_ePesLengthError: intId = BCHP_INT_ID_PES_ERROR; break;
                case NEXUS_XptDataInterrupt_ePesStartCodeError: intId = BCHP_INT_ID_PSC_ERROR; break;
                default: continue;
                }
                rc = BINT_CreateCallback(&pTransport->message.xptDataInterrupt[xptDataInterrupt], g_pCoreHandles->bint, intId, NEXUS_Transport_P_MessageError_isr, NULL, xptDataInterrupt);
                if (rc) {rc = BERR_TRACE(rc);}
                rc = BINT_EnableCallback(pTransport->message.xptDataInterrupt[xptDataInterrupt]);
                if (rc) {rc = BERR_TRACE(rc);}
            }
        }
        else if (pTransport->message.xptDataInterrupt[xptDataInterrupt]) {
            BINT_DisableCallback(pTransport->message.xptDataInterrupt[xptDataInterrupt]);
            BINT_DestroyCallback(pTransport->message.xptDataInterrupt[xptDataInterrupt]);
            pTransport->message.xptDataInterrupt[xptDataInterrupt] = NULL;
        }
    }
#endif
}

/* This function is dangerous and not meant for general use. It is needed for
test code that needs the XPT handle. It must be extern'd. */
void b_get_xpt(BXPT_Handle *xpt)
{
    *xpt = pTransport->xpt;
}

void NEXUS_Transport_P_IncPowerDown(bool powered)
{
#if 0
    bool currentState = pTransport->powerCount;

    /* transport module must reference count so we don't call BXPT_P_CanPowerDown excessively */
    if (powered) {
        pTransport->powerCount++;
    }
    else if (pTransport->powerCount) {
        pTransport->powerCount--;
    }
    else {
        BDBG_ERR(("Invalid transport power count"));
    }

    if (currentState != (bool)pTransport->powerCount) {
        if (pTransport->powerCount) {
            /* powering up can be done immediately */
            NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eTransport, true);
        }
        else {
            /* powering down can only be done after checks */
            if (BXPT_P_CanPowerDown(pTransport->xpt)) {
                NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eTransport, false);
            }
            else {
                /* If this fails, the nexus or XPT PI code must be reworked. */
                BDBG_ERR(("Unable to power down transport"));
            }
        }
    }
#else
    BSTD_UNUSED(powered);
#endif
}

NEXUS_Error NEXUS_TransportModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT    
    BERR_Code rc;
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    unsigned i;
#endif
    BSTD_UNUSED(pSettings);

    if (enabled) {
        BXPT_StandbySettings standbySettings;
        BXPT_GetDefaultStandbySettings(&standbySettings);

#if (!NEXUS_HAS_LEGACY_XPT)
        /* disable all parsers before BXPT_Standby and restore them later. 
           this removes the requirement for apps to close/re-open pid channels */
#if NEXUS_NUM_PARSER_BANDS
        for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
            pTransport->standby.parserEnabled[i] = pTransport->parserBand[i].refcnt;
            if (pTransport->standby.parserEnabled[i]) {
                BXPT_SetParserEnable(pTransport->xpt, pTransport->parserBand[i].hwIndex, false);
            }
        }
#endif

        standbySettings.S3Standby = (pSettings->mode == NEXUS_StandbyMode_eDeepSleep); /* S3Standby is only avail on 40nm platforms */
#endif
        
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
        /* need to disable the standard L2 callback */
        if (pTransport->wakeup.settings.enabled) {
            BINT_DisableCallback(pTransport->wakeup.intPacketFoundCallback);
        }
        standbySettings.UseWakeupPacket = pSettings->wakeupSettings.transport;  
#endif
        rc = BXPT_Standby(pTransport->xpt, &standbySettings);
    } 
    else {
        rc = BXPT_Resume(pTransport->xpt);
        if (rc) { rc = BERR_TRACE(rc); goto done; }

#if BXPT_HAS_WAKEUP_PKT_SUPPORT
        if (pTransport->wakeup.settings.enabled) {
            rc = BINT_EnableCallback(pTransport->wakeup.intPacketFoundCallback);        
        }
#endif

#if (!NEXUS_HAS_LEGACY_XPT)
#if NEXUS_NUM_PARSER_BANDS
        for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
            if (pTransport->standby.parserEnabled[i]) {
                BXPT_SetParserEnable(pTransport->xpt, pTransport->parserBand[i].hwIndex, true);
            }
        }
#endif
#endif
    }

done:
    return rc;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
    return NEXUS_SUCCESS;
#endif

}

void NEXUS_Transport_GetXpt_priv( BXPT_Handle *pXpt )
{
    NEXUS_ASSERT_MODULE();
    *pXpt = pTransport->xpt;
}

void NEXUS_TransportWakeup_GetSettings( NEXUS_TransportWakeupSettings * pSettings)
{
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    *pSettings = pTransport->wakeup.settings;
#else
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#endif
}

NEXUS_Error NEXUS_TransportWakeup_SetSettings(const NEXUS_TransportWakeupSettings * pSettings)
{
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    BERR_Code rc;
    BXPT_Wakeup_Settings wakeupSettings; 

    if (pSettings->enabled) {
        unsigned i,j;
        BXPT_Wakeup_PacketFilter *Filter = BKNI_Malloc(sizeof(BXPT_Wakeup_PacketFilter)*NEXUS_WAKEUP_PACKET_SIZE);
        if (!Filter) {return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);}

        for (i=0; i<NEXUS_MAX_WAKEUP_PACKETS; i++) {
            for (j=0; j<pSettings->packetLength; j++) {
                Filter[j].CompareByte = pSettings->filter[i].packet[j].comparePattern;
                Filter[j].Mask = pSettings->filter[i].packet[j].compareMask;
                Filter[j].MaskType = pSettings->filter[i].packet[j].maskType;
            }
            rc = BXPT_Wakeup_SetPacketFilterBytes(pTransport->xpt, i, Filter);
            if (rc) {return BERR_TRACE(rc);}
        }

        BXPT_Wakeup_GetDefaults(&wakeupSettings); 
        wakeupSettings.InputBand = pSettings->inputBand;
        wakeupSettings.PacketLength = pSettings->packetLength;
        wakeupSettings.ErrorInputIgnore = pSettings->errorIgnore;
        rc = BXPT_Wakeup_SetSettings(pTransport->xpt , &wakeupSettings); 
        if (rc) {return BERR_TRACE(rc);}

        rc = BINT_EnableCallback(pTransport->wakeup.intPacketFoundCallback);
        if (rc) {return BERR_TRACE(rc);}
        BXPT_Wakeup_Armed(pTransport->xpt, true);
        BKNI_Free(Filter);
    } else {
        BXPT_Wakeup_Armed(pTransport->xpt, false);
        BINT_DisableCallback(pTransport->wakeup.intPacketFoundCallback);
    }
    
    NEXUS_IsrCallback_Set(pTransport->wakeup.wakeupCallback, &pSettings->wakeupCallback);
    pTransport->wakeup.settings = *pSettings;
#else
    BSTD_UNUSED(pSettings);
#endif

    return 0;
}

bool NEXUS_TransportModule_P_IsMtsifEncrypted(void)
{
#if NEXUS_NUM_MTSIF
    return BXPT_IsMtsifDecryptionEnabled(pTransport->xpt, 0); /* assume that either all MTSIF channels are encrypted or all clear */
#else
    return false;
#endif
}
