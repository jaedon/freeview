/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core.c $
* $brcm_Revision: SW7435-433/1 $
* $brcm_Date: 12/4/12 3:56p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/src/nexus_core.c $
* 
* SW7435-433/1   12/4/12 3:56p mward
* SW7425-4254: fix resume from standby
* 
* 146   11/8/12 11:13a jtna
* SW7552-346: Add TMR standby
* 
* 145   10/25/12 1:58p erickson
* SW7435-453: add #if NEXUS_POWER_MANAGEMENT
* 
* 144   10/25/12 11:54a erickson
* SW7435-453: add core standby
* 
* 143   10/16/12 10:15a erickson
* SW7425-1792: standardize NEXUS_CoreModule_Init to return module handle
* 
* 142   10/12/12 3:59p erickson
* SW7435-419: change proc to BDBG_LOG to that they are still available
*  with B_REFSW_DEBUG_LEVEL=err
* 
* 141   10/11/12 12:25p vsilyaev
* SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
*  added object verification
* 
* 140   9/27/12 1:12p erickson
* SW7435-362: lock module during init and uninit
* 
* 139   9/19/12 10:21a erickson
* SW7435-362: lock/unlock module around NEXUS_CancelTimer
* 
* 138   9/14/12 4:57p erickson
* SWDEPRECATED-2425: print WRN when debug_mem=y does heap validation so
*  there's confirmation
* 
* 137   7/31/12 6:44p vsilyaev
* SW7435-287: Added framework to measure run-time stack usage
* 
* 136   7/27/12 7:06p vsilyaev
* SW7435-281: Added function BKNI_P_Stats_Print that would print
*  instances where critical section were hold the longest time
* 
* 135   7/20/12 5:32p rjlewis
* SW7425-3461: Added support for AVS enabled and track status.
* 
* 134   7/20/12 3:25p vsilyaev
* SW7435-267: Added code to provide/print callback timing statistics
* 
* 133   7/9/12 3:48p vsilyaev
* SW7435-256: Use BLST_AAT implementation of AA Tree
* 
* 132   6/26/12 12:03p vsilyaev
* SW7420-2316: Acquire the module lock before accessing the NEXUS_KeySlot
*  object
* 
* 131   6/22/12 1:43p vsilyaev
* SW7420-2316: Moved NEXUS_OBJECT_CLASS(NEXUS_KeySlot) into the core
*  module
* 
* 130   6/21/12 2:55p vsilyaev
* SW7420-2315: Use AA tree for address->handle lookup
* 
* 129   6/21/12 1:59p vsilyaev
* SW7420-2085: Fixed typo
* 
* 128   6/21/12 12:12p vsilyaev
* SW7420-2085: In absence of the security module define NEXUS_KeySlot
*  class in the core module
* 
* 127   6/19/12 1:38p erickson
* SW7435-238: remove unused code for deprecated MRC config
* 
* 126   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 125   3/19/12 2:28p erickson
* SW7425-2662: bounds check NEXUS_MemoryStatus.memcIndex with both
*  NEXUS_MAX_HEAPS and NEXUS_MAX_MEMC
* 
* 124   3/14/12 3:11p erickson
* SW7425-2607: use NEXUS_MAX_MEMC
* 
* 123   3/14/12 11:04a erickson
* SW7425-2607: use NEXUS_MAX_MEMC and allow NEXUS_NUM_MEMC to be
*  customized
* 
* 122   2/1/12 5:32p erickson
* SW7425-2231: report highWatermark as peak usage %
* 
* 121   2/1/12 10:39a erickson
* SW7425-2231: add core procfs for heap status
* 
* 120   1/4/12 3:20p erickson
* SW7420-1318: save pointer to BINT_P_IntMap for later lookup
* 
* 119   12/22/11 3:58p vsilyaev
* SW7425-2039: Added system wide counter that just collects data every
*  500ms
* 
* 118   11/16/11 1:21p erickson
* SW7125-1121: add NEXUS_MEMORY_TYPE_RESERVED for placeholder heaps that
*  prevent allocations from certain regions
* 
* 117   11/16/11 12:54p erickson
* SW7422-445: remove heap aliasing code. platform must adapt heaps to
*  ensure heap[0] exists.
* 
* 116   8/8/11 5:20p erickson
* SW7346-117: merge
* 
* SW7346-117/3   8/4/11 5:56p rjlewis
* SW7346-117: instead of erroring on invalid part, just ignore unused
*  items.
* 
* SW7346-117/2   8/3/11 3:35p rjlewis
* SW7346-117: fix the warning.
* 
* SW7346-117/1   8/3/11 1:14p rjlewis
* SW7346-117: Added support for GetAvsData.
*
* 115   8/2/11 2:40p jtna
* SW7425-692: move g_pCoreHandles = NULL; to end of function
* 
* 114   7/18/11 5:16p jtna
* SW7425-692: wrap BINT_Close() with BCHP_PWR_Acquire/Release
* 
* 113   6/2/11 4:19p erickson
* SW7405-5345: backing out change in favor of a BCHP_Open7405 reset
* 
* 112   6/1/11 2:26p erickson
* SW7405-5345: reset CFE splash screen before configuring ARC's
* 
* 111   5/10/11 12:13p erickson
* SW7420-1734: fix WRN
* 
* 110   4/5/11 1:28p erickson
* SW7420-1734: fix unalias
*
* 109   4/1/11 4:24p erickson
* SW7420-1734: rework heap calc algorithm so that whole memc regions can
*  be skipped if not made available by the OS, and that heap[0] is
*  aliased if not available
*
* 108   3/4/11 11:26a erickson
* SW7400-3006: add locked setting. rework so that NEXUS_Heap_Create
*  populates g_NexusCore.publicHandles.
*
* 107   2/17/11 5:00p jtna
* SW7408-210: merge for 7408
*
* 106   1/27/11 2:20p jtna
* SW7420-1401: use only one flavor of BPWR or BCHP_PWR
*
* 105   1/20/11 1:58p erickson
* SW7408-212: merge
*
* SW7408-212/1   1/20/11 1:43p gmohile
* SW7408-212 : Add support for avs settings
*
* 104   1/19/11 5:08p erickson
* SW7422-220: allow heap[] array to be sparse
*
* 103   1/19/11 11:15a erickson
* SW7420-1123: expose NEXUS_MODULE_SELF as normal
*
* 102   1/13/11 9:35a erickson
* SW7422-168: adapt to BMRC_Monitor_Open api change
*
* 101   1/12/11 9:19a erickson
* SW7550-618: merge
*
* SW7550-618/1   1/12/11 7:00p nitinb
* SW7550-618: Implemented AVS functions
*
* 100   1/11/11 9:35a erickson
* SW7550-618: add AVS functions
*
* 99   12/22/10 3:09p erickson
* SW7422-2: change continue to break because heap[] must be packed
*
* 98   12/15/10 6:38p jtna
* SW7408-190: fix 7408
*
* 97   12/15/10 6:36p jtna
* SW7408-190: #include "bpwr.h" still needed for 7420
*
* 96   12/3/10 3:49p erickson
* SW7408-190: 7408 merge
*
* 95   12/1/10 10:23a erickson
* SW7405-5025: add NEXUS_TMR_EXCLUSION_MASK and clarify comment
*
* 94   11/22/10 1:13p vsilyaev
* SW7405-4924: BMRC_Monitor_Open takes offsetLow and offsetHigh (not
*  offset and size)
*
* 93   10/27/10 4:16p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
*
* 92   10/14/10 4:55p erickson
* SW7405-4924: downgrade ERR to MSG
*
* 91   10/7/10 11:19a vsilyaev
* SW7405-4924: Communicate size of each DDRAM controller
*
* 90   10/1/10 3:02p jtna
* SW7420-972: NEXUS_POWER_MANAGEMENT option sets BCHP_PWR_SUPPORT option
*
* 89   10/1/10 12:44p jtna
* SW7420-972: BINT_OPEN pwr resource must be acquired even when compiling
*  with NEXUS_POWER_MANAGEMENT=n
*
* 88   9/1/10 5:27p erickson
* SW7420-972: merge
*
* SW7420-972/2   9/1/10 3:34p jtna
* SW7420-972: add bchp_pwr resource init scheme
*
* SW7420-972/1   8/31/10 2:44p jtna
* SW7420-972: rename bchp_pwr node BINT_INIT to BINT_OPEN
*
* 86   8/26/10 3:14p jtna
* SW7420-972: wrap #include "bchp_pwr_resources.h" with
*  NEXUS_POWER_MANAGEMENT==2
*
* 85   8/26/10 2:59p jtna
* SW7420-972: added RFM power management
*
* 84   8/10/10 3:06p randyjew
* SW7208-105:Fix Compile Warning for BCHP_Monitor_PVT
*
* 83   8/6/10 4:39p mphillip
* SW7405-4325: Destroy keyslot debug object in core, not security
*
* 82   8/4/10 3:04p mphillip
* SW7405-4325: Merge keyslot refactoring to main
*
* SW7405-4325/1   8/4/10 2:31p mphillip
* SW7405-4325: Move keyslot memory management into core
*
* 81   6/29/10 10:35a gmohile
* SW7408-69 : Add runtime support to disable avs
*
* 80   3/1/10 11:35a erickson
* SW7405-3892: restore code so that ARC0 protects OS memory at the base
*  of MEMC0
*
* 79   2/26/10 2:15p erickson
* SW7325-655: use BMEM_Heap_FreeCached to remove side-effect based
*  NEXUS_Core_P_AddressToHeap
*
* 78   1/29/10 4:03p gmohile
* SW7408-69 : Add AVS support for 7408
*
* 77   12/30/09 10:02a erickson
* SW7405-3691: add BMEM_Heap_Validate per heap to NEXUS_Core_P_Timer
*
* 76   12/23/09 10:35a jgarrett
* SW7408-17: Removing timer mask setting
*
* 75   12/17/09 4:20p erickson
* SW3556-933: added new format. moved all type lookup and conversion code
*  into separate .c so we don't have to rev nexus_core.c so often.
*
* 74   12/14/09 3:36p erickson
* SW7420-70: report failures from NEXUS_Heap_Create with non-zero error
*  code
*
* 73   12/14/09 11:44a mphillip
* SW7405-3471: Merge additional pixel format support to main
*
* DirectFB_1_4_Port/2   12/14/09 3:02p robertwm
* SW7405-3471: Nexus needs to support additional pixel formats for
*  DirectFB.
*
* DirectFB_1_4_Port/1   11/24/09 7:12p robertwm
* SW7405-3471: Nexus needs to support additional pixel formats for
*  DirectFB.
*
* 71   10/15/09 12:50p erickson
* SW7405-3221: add BINT_Stats_Enable support. default off.
*
* 70   9/22/09 1:25p erickson
* SW3548-2489: update NEXUS_P_VideoFormat_FromInfo so that there's no
*  50/60Hz or interlaced/progressive mixup.
*
* 69   8/6/09 2:22p erickson
* PR57387: add 720p 25/30 to NEXUS_VideoFormat list
*
* 68   7/23/09 10:37a katrep
* PR57010: Fixed issue with memc arc .
*
* 67   6/25/09 10:21a erickson
* PR55968: Nexus should rely on BPWR_Open to set inital power state
*
* 66   6/8/09 7:06a erickson
* PR55617: add NEXUS_P_AspectRatio_ToMagnum
*
* 65   5/15/09 1:40p erickson
* PR54843: fix 3548
*
* 64   5/15/09 12:33p erickson
* PR54843: use BPWR_Open to guarantee initial power state
*
* 63   5/4/09 3:40p erickson
* PR53373: merge HDMI 1.3a support
*
* PR53373_DeepColor/1   4/16/09 3:58p vle
* PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
*
* 62   4/17/09 9:30a erickson
* PR54304: add NEXUS_PixelFormat_eR8_G8_B8_A8
*
* 61   4/13/09 12:53p erickson
* PR53866: fix NEXUS_CoreModule_Init when called for second time
*
* 60   4/8/09 5:21p erickson
* PR53866: support ARC's on all memc's and for all heaps
*
* 59   4/8/09 4:01p erickson
* PR53866: add custom_arc=y option for user control, correct spelling of
*  mrc struct member
*
* 58   3/19/09 11:37a erickson
* PR53234: add two DVI VESA formats
*
* 57   3/9/09 2:32p erickson
* PR50748: remove nexus multi-heap management
*
* 56   3/6/09 3:35p jgarrett
* PR 52899: Converting 1080p@23.976 properly
*
* 55   2/26/09 1:57p erickson
* PR50998: default Nexus' use of MEM to BMEM_SafetyConfig_eNormal. allow
*  eSafe if debug_mem=y is defined.
*
* 54   2/25/09 1:05p erickson
* PR47129: allow drop-frame rates for NEXUS_VideoFormat_e240p60hz in
*  NEXUS_P_VideoFormat_FromInfo
*
* 53   2/23/09 12:13p erickson
* PR52370: reduce compilation dependencies by removing bvdc.h from
*  nexus_core.h
*
* 52   2/11/09 10:08a erickson
* PR52007: added NEXUS_LookupFrameRate
*
* 51   2/3/09 2:50p jgarrett
* PR 51743: Allowing 29.97 1080p to be returned as 1080p30.
*
* 50   1/26/09 12:05p erickson
* PR51468: global variable naming convention
*
* 49   1/8/09 9:37p erickson
* PR50757: added NEXUS_P_FrameRate_ToMagnum
*
* 48   12/10/08 3:35p vsilyaev
* PR 48908: Moved nexus_power_management.h to the public API
*
* 47   12/3/08 3:12p erickson
* PR48963: added 7420 heap options
*
* 46   12/3/08 1:07p erickson
* PR47914: improve NEXUS_P_VideoFormat_FromInfo logic, no eUnknown, added
*  more specific types
*
* 45   11/19/08 10:06a erickson
* PR48764: merge in macrovision status
*
* PR48764/1   11/10/08 4:39p chengli
* PR48764 : Add API NEXUS_P_MacrovisionType_FromMagnum
*
* 44   11/5/08 5:25p erickson
* PR48698: switch id's
*
* 43   11/4/08 1:29p erickson
* PR40799: added ADS
*
* 42   10/31/08 1:32p erickson
* PR47030: power up XVD before BCHP_Open
*
* 41   10/30/08 8:49p erickson
* PR47030: added NEXUS_HeapHandle and related functions
*
* 40   10/30/08 1:22p erickson
* PR40799: add BPWR_Open to ensure power up for BCHP_Open. switch to
*  NEXUS_POWER_MANAGEMENT. fix Uninit.
*
* 39   10/17/08 12:23p erickson
* PR47030: fix misspelling
*
* 38   10/7/08 2:57a erickson
* PR40799: fix settop chips
*
* 37   10/7/08 1:29a erickson
* PR40799: unset CLKGEN PWRDN bits before starting magnum
*
* 36   9/26/08 11:10a erickson
* PR40799: add internal power management API
*
* 35   9/12/08 9:08a erickson
* PR46828: fix PixelFormatInfo for some AYCrCb formats
*
* 34   8/21/08 3:22p katrep
* PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
*
* 33   8/20/08 2:12p erickson
* PR45677: fix NEXUS_P_VideoFormat_FromInfo to use > logic instead of >=
*  logic. the previous code didn't handle a 1248x702p 30Hz stream.
*
* 32   8/11/08 11:40a erickson
* PR45219: merge
*
* 31   8/8/08 6:59p jgarrett
* PR 45171: Adding new PAL modes
*
* 30   7/30/08 2:00p erickson
* PR45266: converted NEXUS_P_VideoFormat_FromInfo switch statement to
*  cascading if/else
*
* 29   6/30/08 1:45p erickson
* PR44165: impl NEXUS_CORE_NUM_SECURE_HEAPS
*
* 28   6/20/08 2:10p erickson
* PR43859: merge
*
* PR43859/1   6/20/08 1:10p dyzhang
* PR43859: add NEXUS_P_VertFreq_FromMagnum to convert bfmt format infor
*  vertical frequency to nexus framerate enum
*
* 27   6/17/08 5:21p vsilyaev
* PR 40870: Added YUV 4:4:4 pixel formats
*
* 26   5/16/08 1:55p erickson
* PR42758: add NEXUS_TransportType_eVob support
*
* 25   5/8/08 2:48p erickson
* PR42438: derive NEXUS_VideoInputStatus.format for digital decode using
*  common function
*
* 24   5/2/08 10:59a erickson
* PR41314: fix naming convention on ColorSpace
*
* 23   5/2/08 10:53a erickson
* PR41314: add SECAM variants
*
* 22   4/21/08 2:14p erickson
* PR39009: fix 7401
*
* 21   4/16/08 12:45p erickson
* PR39009: merge
*
* PR39009/1   4/17/08 12:39a honglang
* PR39009: add two format conversion: NTSC_443, PAL_60
*
* 20   4/10/08 5:45p vsilyaev
* PR 41083: Added BTRC support
*
* 19   4/10/08 11:24a erickson
* PR41084: decrease freq of int stats timer
*
* 18   4/8/08 3:18p erickson
* PR39453: add 3548 & 3556
*
* 17   4/8/08 10:43a vsilyaev
* PR 41083: Added bprofile support
*
* 16   4/4/08 4:36p erickson
* PR41383: replace assert with conditional
*
* 15   4/1/08 4:18p vsilyaev
* PR 41084: Added printing of L1 interrupt statistics
*
* 14   3/27/08 2:43p vsilyaev
* PR 40818: Added settings to allocate shared heap
*
* 13   3/26/08 11:44p vsilyaev
* PR 40410: Added more video settings for HDMI Input interface
*
* 12   3/26/08 12:51p erickson
* PR40567: added pixel format functions
*
* PR40567/1   3/26/08 6:22p dyzhang
* PR40567: add NEXUS_P_PixelFormat_FromMagnum and
*  NEXUS_P_PixelFormat_ToMagnum
*
* 11   3/25/08 2:53p vsilyaev
* PR 39818: Fixed return type
*
* 10   3/25/08 2:20p vsilyaev
* PR 39818: Added MKV stream type
*
* 9   3/25/08 1:22p vsilyaev
* PR 40744: Fixed compile on 7401
*
* 8   3/25/08 12:36p erickson
* PR40744: standardized capitalization on enums
*
* 7   3/21/08 11:18a erickson
* PR40744: merge
*
* PR40744/1   3/19/08 4:17p dyzhang
* PR40744: NEXUS_VideoInput_GetStatus returns unknown format for all VESA
*  formats
*
* 6   3/21/08 11:11a erickson
* PR40716: merge
*
* PR40716/1   3/21/08 3:39p dyzhang
* PR40716: timebase settings for analog inputs are not correct. add a
*  function NEXUS_P_VideoFormat_FromMagnum to translate magnum framerate
*  code to nexus one.
*
* 5   3/20/08 6:15p rjlewis
* PR40352: type mismatch compile error under VxWorks.
*
* 3   2/22/08 10:55a erickson
* PR39863: add #if avoid unused code warning
*
* 2   2/7/08 3:06p jgarrett
* PR 39359: Disabling bookeeping for all non-UMA heaps
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
*
***************************************************************************/
#include "nexus_core_module.h"
#if NEXUS_POWER_MANAGEMENT
#include "bchp_pwr.h"
#include "bchp_pwr_resources.h"
#endif
#include "bint_stats.h"
#include "nexus_base_statistics.h"
#include "bkni_metrics.h"

#if NEXUS_AVS_MONITOR
/* These aren't needed anymore since the AVS info moved to bchp.h */
#if BCHP_CHIP == 7468
#include "bchp_7468.h"
#elif BCHP_CHIP == 7408
#include "bchp_7408.h"
#elif BCHP_CHIP == 7550
#include "bchp_7550.h"
#endif
#endif

BDBG_MODULE(nexus_core);
BTRC_MODULE(500ms_tick, ENABLE);

NEXUS_Core_P_State g_NexusCore;

const NEXUS_Core_MagnumHandles *g_pCoreHandles = NULL;

void
NEXUS_CoreModule_GetDefaultSettings(NEXUS_Core_Settings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

#if B_HAS_TRC
/* reduce period to make more frequent snapshots */
#define NEXUS_CORE_TIMER_FREQ 500
#else
#define NEXUS_CORE_TIMER_FREQ 5000
#endif


static void
NEXUS_Core_P_PrintStatistics(void)
{
    /* you can see these stats with msg_modules=int */
    BINT_DumpInfo(g_NexusCore.publicHandles.bint);
    /* you can see these stats with msg_modules=nexus_statistics\* */
    NEXUS_P_Base_Stats_Report();
#if defined(BKNI_P_Stats_Print)
    /* you can see these stats with msg_modules=bkni_statistics\* */
    BKNI_P_Stats_Print();
#endif

    if (NEXUS_GetEnv("debug_mem")) {
        unsigned i;
        BDBG_WRN(("calling BMEM_Heap_Validate for all heaps"));
        for(i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (g_pCoreHandles->heap[i]) {
                BMEM_Heap_Validate(g_pCoreHandles->heap[i]);
            }
        }
    }
    return;
}
static void
NEXUS_Core_P_Timer(void *cntx)
{
    BSTD_UNUSED(cntx);

    BTRC_TRACE(500ms_tick, STOP); BTRC_TRACE(500ms_tick, START); /* snapshot counters to prevent underflow */

    NEXUS_Core_P_PrintStatistics();

    g_NexusCore.timer = NEXUS_ScheduleTimer(NEXUS_CORE_TIMER_FREQ, NEXUS_Core_P_Timer, NULL);
    if(!g_NexusCore.timer) {
        BDBG_WRN(("NEXUS_Core_P_Timer: can't schedule timer"));
    }
    return;
}

static NEXUS_AvsSettings g_avsSettings;

void NEXUS_GetAvsSettings( NEXUS_AvsSettings *pSettings )
{
    *pSettings = g_avsSettings;
}

NEXUS_Error NEXUS_SetAvsSettings( const NEXUS_AvsSettings *pSettings )
{
    BDBG_ASSERT(pSettings);

    if (pSettings->hardStopOffset > 15)
        return NEXUS_INVALID_PARAMETER;
    if (pSettings->maxVoltageStopOffset > 15)
        return NEXUS_INVALID_PARAMETER;

    /* Note: if this hardware does not support these settings they will not be used */
    g_avsSettings = *pSettings;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_GetAvsStatus( NEXUS_AvsStatus *pStatus )
{
    BCHP_AvsData data;
    BERR_Code rc;

    BDBG_ASSERT(pStatus);

    /* Note: if the AVS hardware is not supported this call will return an error */
    rc = BCHP_GetAvsData(g_NexusCore.publicHandles.chp, &data);
    if(rc!=BERR_SUCCESS) {
        return BERR_TRACE(rc);
    }

    pStatus->voltage     = data.voltage;
    pStatus->temperature = data.temperature;
    pStatus->enabled     = data.enabled;
    pStatus->tracking    = data.tracking;

    return rc;
}

#if NEXUS_AVS_MONITOR
static void
NEXUS_Core_P_MonitorPvt(void *context)
{
    BCHP_AvsSettings avsSettings;

    /* Note: if this hardware does not support these settings they will not be used */
    avsSettings.hardStopOffset = g_avsSettings.hardStopOffset;
    avsSettings.maxVoltageStopOffset = g_avsSettings.maxVoltageStopOffset;

    BSTD_UNUSED(context);

    BCHP_MonitorPvt(g_NexusCore.publicHandles.chp, &avsSettings);

    g_NexusCore.pvtTimer = NEXUS_ScheduleTimer(1000, NEXUS_Core_P_MonitorPvt, NULL);

    if(!g_NexusCore.pvtTimer) {
        BDBG_WRN(("NEXUS_Core_P_Timer: can't schedule PVT timer"));
    }
    return;
}
#endif /*NEXUS_AVS_MONITOR*/

static void NEXUS_CoreModule_P_Print(void)
{
    unsigned i;
    NEXUS_Error rc;
    BDBG_LOG(("Core:"));
    BDBG_LOG(("heap offset     size       mapping used peak largestavail"));
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_pCoreHandles->nexusHeap[i]) {
            NEXUS_MemoryStatus status;
            char buf[16];
            unsigned shifted_size, shifted_free, shifted_highwater;
            
            rc = NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[i], &status);
            if (rc) continue;

            if (status.memoryType == NEXUS_MemoryType_eDriver) {
                BKNI_Snprintf(buf, 16, "eDriver");
            }
            else if (status.memoryType == NEXUS_MemoryType_eFull) {
                BKNI_Snprintf(buf, 16, "eFull  ");
            }
            else if (status.memoryType == NEXUS_MemoryType_eApplication) {
                BKNI_Snprintf(buf, 16, "eApp   ");
            }
            else if (status.memoryType == NEXUS_MemoryType_eDeviceOnly) {
                BKNI_Snprintf(buf, 16, "eDevice");
            }
            else {
                BKNI_Snprintf(buf, 16, "0x%-5x", status.memoryType);
            }
            
            shifted_size = status.size/1024;
            shifted_free = status.free/1024;
            shifted_highwater = status.highWatermark/1024;
            BDBG_LOG(("%-4d 0x%08x 0x%08x %s %3d%% %3d%% 0x%08x", i, status.offset, status.size, buf, 
                shifted_size?(shifted_size-shifted_free)*100/shifted_size:0, 
                shifted_size?shifted_highwater*100/shifted_size:0, 
                status.largestFreeBlock));
        }
    }
}

NEXUS_ModuleHandle
NEXUS_CoreModule_Init(const NEXUS_Core_Settings *pSettings)
{
    BERR_Code rc;
    BMEM_Settings mem_module_settings;
    const BINT_Settings *intr_cfg;
    BTMR_DefaultSettings tmr_settings;
    unsigned i;
    NEXUS_ModuleSettings moduleSettings;

#if NEXUS_NUM_MEMC
    /* verify API macros aren't below nexus_platform_features.h. NEXUS_NUM_MEMC is no longer used inside nexus. */
    BDBG_CASSERT(NEXUS_MAX_MEMC >= NEXUS_NUM_MEMC);
#endif
    
    if (!pSettings) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    BKNI_Memset(&g_NexusCore, 0, sizeof(g_NexusCore));
    BLST_Q_INIT(&g_NexusCore.allocatedBlocks);
    BLST_AA_TREE_INIT(NEXUS_P_MemoryBlockTree, &g_NexusCore.allocatedBlocksTree);

    if(!pSettings->interruptInterface.pDisconnectInterrupt || !pSettings->interruptInterface.pConnectInterrupt || !pSettings->interruptInterface.pEnableInterrupt_isr || !pSettings->interruptInterface.pDisableInterrupt_isr) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_params;
    }

    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.dbgPrint = NEXUS_CoreModule_P_Print;
    moduleSettings.dbgModules = "nexus_core";
    g_NexusCore.module = NEXUS_Module_Create("core", &moduleSettings);
    if(!g_NexusCore.module) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_module;
    }
    NEXUS_LockModule();

    g_NexusCore.cfg = *pSettings;
    BDBG_ASSERT(NULL != pSettings->regHandle);
    g_NexusCore.publicHandles.reg = pSettings->regHandle;

    rc = BCHP_OPEN(&g_NexusCore.publicHandles.chp, g_NexusCore.publicHandles.reg);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_chp;
    }
#if NEXUS_POWER_MANAGEMENT
    /* BCHP_Open will initialize (i.e. power down) all MAGNUM_CONTROLLED nodes.
       This will initialize the rest */
    BCHP_PWR_InitAllHwResources(g_NexusCore.publicHandles.chp);
#endif

    rc = BMEM_GetDefaultSettings(&mem_module_settings);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_mem_cfg;
    }
    rc = BMEM_Open(&g_NexusCore.publicHandles.mem, &mem_module_settings);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_mem;
    }
    for(i=0;i<NEXUS_MAX_HEAPS;i++) {
        /* platform may have runtime option for memory configuration. length == 0 means no heap. */
        if (pSettings->heapRegion[i].length == 0) {
            continue;
        }
        if (!NEXUS_Heap_Create(i, g_NexusCore.publicHandles.mem, &pSettings->heapRegion[i])) {
            rc = BERR_TRACE(BERR_UNKNOWN);
            goto err_heap;
        }
        /* successful heap is stored in g_pCoreHandles->heap[i] */
    }

    rc = NEXUS_Core_P_Profile_Init();
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_profile;
    }

    intr_cfg = BINT_GETSETTINGS();
    BDBG_ASSERT(intr_cfg);
    g_NexusCore.publicHandles.bint_map = intr_cfg->pIntMap;
#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_AcquireResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif
    /* BINT_Open needs to write to L2 interrupts, and should ideally guarantee its own power by
       acquiring/releasing within BINT_Open. However, there's no BCHP handle available inside BINT_Open,
       and we can't make it available without breaking the BINT_Open prototype.
       Hence, upper-level software must guarantee power to BINT_Open. */
    rc = BINT_Open(&g_NexusCore.publicHandles.bint, g_NexusCore.publicHandles.reg, intr_cfg);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_int;
    }
#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_ReleaseResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif

    /* User can export custom_arc=y to have direct control of the MEMC ARC (address range checker) HW */
    if (!NEXUS_GetEnv("custom_arc")) {
        for(i=0;i<NEXUS_MAX_HEAPS;i++) {
            NEXUS_MemoryStatus memStatus;

            if (!g_NexusCore.publicHandles.nexusHeap[i]) continue;

            if (!g_NexusCore.publicHandles.heap[i]) continue; /* NEXUS_MEMORY_TYPE_RESERVED may have no BMEM heap */
            
            (void)NEXUS_Heap_GetStatus(g_NexusCore.publicHandles.nexusHeap[i], &memStatus);

            /* must have at least one heap for MEMC. also check memc[] bounds. */
            if (memStatus.memcIndex >= NEXUS_MAX_HEAPS || memStatus.memcIndex >= NEXUS_MAX_MEMC) {
                BDBG_ERR(("Invalid memcIndex %d", memStatus.memcIndex));
                continue;
            }

            if (!g_NexusCore.publicHandles.memc[memStatus.memcIndex].mrc) {
                BMRC_Settings mrcSettings;
                BMRC_Monitor_Settings mrcMonitorSettings;
                unsigned memcOffset, memcSize;

                (void)BMRC_GetDefaultSettings(&mrcSettings);
                mrcSettings.usMemcId = memStatus.memcIndex;
                rc = BMRC_Open(&g_NexusCore.publicHandles.memc[memStatus.memcIndex].mrc, g_NexusCore.publicHandles.reg, g_NexusCore.publicHandles.bint, &mrcSettings);
                if (rc!=BERR_SUCCESS) {
                    rc = BERR_TRACE(rc);
                    goto err_mrc;
                }

                BMRC_Monitor_GetDefaultSettings(&mrcMonitorSettings);

                memcOffset = g_NexusCore.cfg.memcRegion[memStatus.memcIndex].offset;
                memcSize = g_NexusCore.cfg.memcRegion[memStatus.memcIndex].length;
                BDBG_MSG(("MEMC%u %u:%u", memStatus.memcIndex, memcOffset, memcSize));

                BDBG_ASSERT(memcOffset+memcSize >= memcOffset); /* check that addresses wouldn't wrap */
                rc = BMRC_Monitor_Open(&g_NexusCore.publicHandles.memc[memStatus.memcIndex].rmm, g_NexusCore.publicHandles.reg, g_NexusCore.publicHandles.bint, g_NexusCore.publicHandles.chp,
                        g_NexusCore.publicHandles.memc[memStatus.memcIndex].mrc, memcOffset, memcOffset+memcSize, &mrcMonitorSettings);
                if (rc!=BERR_SUCCESS) {
                    rc = BERR_TRACE(rc);
                    goto err_mrc;
                }
            }

            rc = BMRC_Monitor_GetMemoryInterface(g_NexusCore.publicHandles.memc[memStatus.memcIndex].rmm, &g_NexusCore.publicHandles.memc[memStatus.memcIndex].mem_monitor);
            if (rc!=BERR_SUCCESS) {
                rc = BERR_TRACE(rc);
                goto err_mrc;
            }

            rc = BMEM_InstallMonitor(g_NexusCore.publicHandles.heap[i], &g_NexusCore.publicHandles.memc[memStatus.memcIndex].mem_monitor);
            if (rc!=BERR_SUCCESS) {
                rc = BERR_TRACE(rc);
                goto err_mrc;
            }
        }
    }

    rc = BTMR_GetDefaultSettings(&tmr_settings);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_tmr_cfg;
    }

    #if NEXUS_TMR_EXCLUSION_MASK
    /* Use default timer settings unless user sets NEXUS_TMR_EXCLUSION_MASK in nexus_platform_features.h. Linux sometimes uses the last timer on certain chips.
    The timerMask (below) indicates which timers should be EXCLUDED from the list of available timers for nexus/magnum allocation and use.
    If 0 (default), no timers are using used externally. If 2, timer 1 is being used; if 6, timers 1&2 are being used, etc. */
    tmr_settings.timerMask = NEXUS_TMR_EXCLUSION_MASK;
    #endif

    rc = BTMR_Open(&g_NexusCore.publicHandles.tmr, g_NexusCore.publicHandles.chp, g_NexusCore.publicHandles.reg, g_NexusCore.publicHandles.bint, &tmr_settings);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_tmr;
    }
    g_NexusCore.timer = NEXUS_ScheduleTimer(NEXUS_CORE_TIMER_FREQ, NEXUS_Core_P_Timer, NULL);
    if(!g_NexusCore.timer) {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err_timer;
    }
#if NEXUS_AVS_MONITOR
    if (!NEXUS_GetEnv("disable_avs")) {
        g_NexusCore.pvtTimer = NEXUS_ScheduleTimer(2000, NEXUS_Core_P_MonitorPvt, NULL);
        if(!g_NexusCore.pvtTimer) {
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err_timer;
        }
    }
#endif

#if 0
    /* adds isr time to NEXUS_Core_P_Timer stats. you also need to enable code in bint.c. */
    rc = BINT_Stats_Enable(g_NexusCore.publicHandles.bint, g_NexusCore.publicHandles.tmr);
    if(rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto err_int;
    }
#endif

    g_pCoreHandles = &g_NexusCore.publicHandles;

    NEXUS_PowerManagement_Init();
    BTRC_TRACE(500ms_tick, START);

    NEXUS_UnlockModule();
    return g_NexusCore.module;

err_timer:
    BTMR_Close(g_NexusCore.publicHandles.tmr);
err_tmr:
err_tmr_cfg:
err_mrc:
    for(i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_NexusCore.publicHandles.memc[i].rmm) {
            BMRC_Monitor_Close(g_NexusCore.publicHandles.memc[i].rmm);
        }
        if (g_NexusCore.publicHandles.memc[i].mrc) {
            BMRC_Close(g_NexusCore.publicHandles.memc[i].mrc);
        }
    }
    BINT_Close(g_NexusCore.publicHandles.bint);
err_int:
    NEXUS_Core_P_Profile_Uninit();
err_profile:
err_heap:
    for(i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_NexusCore.publicHandles.nexusHeap[i]) {
            NEXUS_Heap_Destroy(g_NexusCore.publicHandles.nexusHeap[i]);
        }
    }
    BMEM_Close(g_NexusCore.publicHandles.mem);
err_mem:
err_mem_cfg:
    BCHP_Close(g_NexusCore.publicHandles.chp);
err_chp:
    g_NexusCore.publicHandles.reg = NULL; /* reg handle is passed in */
err_module:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NexusCore.module);
err_params:
    return NULL;
}

void
NEXUS_CoreModule_Uninit(void)
{
    unsigned i;

    NEXUS_LockModule();
    NEXUS_Core_P_PrintStatistics();
    BTRC_Module_Report(BTRC_MODULE_HANDLE(500ms_tick));

    NEXUS_PowerManagement_Uninit();

    BDBG_ASSERT(g_pCoreHandles);
    
    if(g_NexusCore.timer) {
        NEXUS_CancelTimer(g_NexusCore.timer);
    }
#if NEXUS_AVS_MONITOR
    if(g_NexusCore.pvtTimer) {
      NEXUS_CancelTimer(g_NexusCore.pvtTimer);
    }
#endif
    
    BTMR_Close(g_NexusCore.publicHandles.tmr);
    NEXUS_Core_P_Profile_Uninit();
    for(i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_NexusCore.publicHandles.memc[i].rmm) {
            BMRC_Monitor_Close(g_NexusCore.publicHandles.memc[i].rmm);
        }
        if (g_NexusCore.publicHandles.memc[i].mrc) {
            BMRC_Close(g_NexusCore.publicHandles.memc[i].mrc);
        }
        if(g_NexusCore.publicHandles.nexusHeap[i]) {
            NEXUS_Heap_Destroy(g_NexusCore.publicHandles.nexusHeap[i]);
        }
    }
#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_AcquireResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif
    BINT_Close(g_NexusCore.publicHandles.bint);
#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_ReleaseResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif
    BMEM_Close(g_NexusCore.publicHandles.mem);
    BCHP_Close(g_NexusCore.publicHandles.chp);
    g_NexusCore.publicHandles.reg = NULL; /* reg handle is passed in */
    g_pCoreHandles = NULL;
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NexusCore.module);
    BTRC_TRACE(500ms_tick, STOP);
    return;
}

NEXUS_Error NEXUS_CoreModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    unsigned i;

    if(enabled) {
        if(pSettings->mode == NEXUS_StandbyMode_eDeepSleep) {
        BTMR_Standby(g_NexusCore.publicHandles.tmr);
        for(i=0;i<NEXUS_MAX_MEMC;i++) {
                if(g_NexusCore.publicHandles.memc[i].mrc) {
                    BMRC_Standby(g_NexusCore.publicHandles.memc[i].mrc);
                }
            }
            BCHP_Standby(g_NexusCore.publicHandles.chp);
            BCHP_PWR_Standby(g_NexusCore.publicHandles.chp, NULL); /* currently a no-op, because BCHP_PWR_Standby() doesn't have to do anything special */           
            /* DMA clock needs to be powered ON for secure memory hash */ 
#ifdef BCHP_PWR_RESOURCE_DMA
            BCHP_PWR_AcquireResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_DMA);
#endif    
            g_NexusCore.standby = true;
        }
    } else {
        if(g_NexusCore.standby) {
            /* DMA clock was acquired for secure memory hash. So release it firs after resume */
#ifdef BCHP_PWR_RESOURCE_DMA
            BCHP_PWR_ReleaseResource(g_NexusCore.publicHandles.chp, BCHP_PWR_RESOURCE_DMA);
#endif
            BCHP_PWR_Resume(g_NexusCore.publicHandles.chp); /* on a resume from S3, the HW is running at full power. BCHP_PWR_Resume() powers down so the SW and HW states match */
            BCHP_Resume(g_NexusCore.publicHandles.chp);
            for(i=0;i<NEXUS_MAX_MEMC;i++) {
                if(g_NexusCore.publicHandles.memc[i].mrc){          
                    BMRC_Resume(g_NexusCore.publicHandles.memc[i].mrc);
                }
            }
            BTMR_Resume(g_NexusCore.publicHandles.tmr);    
            g_NexusCore.standby = false;
        }
    }
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif
    
    return NEXUS_SUCCESS;
}

BERR_Code
NEXUS_Core_EnableInterrupt(unsigned irqNum)
{
    BERR_Code rc;
    BKNI_EnterCriticalSection();
    rc = g_NexusCore.cfg.interruptInterface.pEnableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
    return rc;
}

BERR_Code
NEXUS_Core_EnableInterrupt_isr(unsigned irqNum)
{
    BERR_Code rc;
    rc = g_NexusCore.cfg.interruptInterface.pEnableInterrupt_isr(irqNum);
    return rc;
}

void
NEXUS_Core_DisableInterrupt(unsigned irqNum)
{
    BKNI_EnterCriticalSection();
    g_NexusCore.cfg.interruptInterface.pDisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
    return;
}

void
NEXUS_Core_DisableInterrupt_isr(unsigned irqNum)
{
    g_NexusCore.cfg.interruptInterface.pDisableInterrupt_isr(irqNum);
    return;
}

BERR_Code
NEXUS_Core_ConnectInterrupt(unsigned irqNum, NEXUS_Core_InterruptFunction pIsrFunc, void *pFuncParam, int iFuncParam)
{
    return g_NexusCore.cfg.interruptInterface.pConnectInterrupt(irqNum, pIsrFunc, pFuncParam, iFuncParam);
}

void
NEXUS_Core_DisconnectInterrupt(unsigned irqNum)
{
    g_NexusCore.cfg.interruptInterface.pDisconnectInterrupt(irqNum);
    return;
}

static bool NEXUS_Core_P_AddressInRegion(NEXUS_Core_MemoryRegion *pRegion, void *pAddress, void **ppUncachedAddress)
{
    if ( pAddress >= pRegion->pvAddr && pAddress < (void *)((uint32_t)pRegion->pvAddr + pRegion->length) )
    {
        if (ppUncachedAddress) {
            *ppUncachedAddress = pAddress;
        }
        return true;
    }
    else if ( pAddress >= pRegion->pvAddrCached && pAddress < (void *)((uint32_t)pRegion->pvAddrCached + pRegion->length) )
    {
        if (ppUncachedAddress) {
            *ppUncachedAddress = (void *)((uint32_t)pAddress - (uint32_t)pRegion->pvAddrCached +
                                      (uint32_t)pRegion->pvAddr);
        }
        return true;
    }

    return false;
}

BMEM_Heap_Handle NEXUS_Core_P_AddressToHeap(void *pAddress, void **ppUncachedAddress)
{
    int i;
    for ( i = 0; i < NEXUS_MAX_HEAPS; i++ )
    {
        NEXUS_Core_MemoryRegion *pRegion = &g_NexusCore.cfg.heapRegion[i];
        if ( NEXUS_Core_P_AddressInRegion(pRegion, pAddress, ppUncachedAddress) )
        {
            return g_pCoreHandles->heap[i];
        }
    }
    return NULL;
}

BDBG_OBJECT_ID(NEXUS_VideoOutput);
BDBG_OBJECT_ID(NEXUS_AudioInput);
BDBG_OBJECT_ID(NEXUS_AudioOutput);

#include "nexus_security_types.h"
#include "priv/nexus_core_security.h"

NEXUS_KeySlotHandle
NEXUS_P_Core_AllocateKeySlotHandle(void)
{
    NEXUS_KeySlotHandle pHandle;
    /* XXX this function called without acquiring the nexus core module lock XXX */
    pHandle = (NEXUS_KeySlotHandle)BKNI_Malloc(sizeof(*pHandle));
    if ( !pHandle) { BERR_Code rc; rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); return NULL; }
    NEXUS_LockModule();
    NEXUS_OBJECT_INIT(NEXUS_KeySlot, pHandle);
    NEXUS_UnlockModule();
    return pHandle;
}


void NEXUS_P_Core_GetKeySlotInfo(
    NEXUS_KeySlotHandle keyHandle,
    NEXUS_SecurityKeySlotInfo *pKeyslotInfo /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(keyHandle, NEXUS_KeySlot);
    BDBG_ASSERT(pKeyslotInfo);


    BKNI_Memset(pKeyslotInfo, 0, sizeof(*pKeyslotInfo));
    pKeyslotInfo->keySlotNumber = keyHandle->keySlotNumber;
    pKeyslotInfo->keySlotEngine = keyHandle->cryptoEngine;
    pKeyslotInfo->keySlotType = keyHandle->keyslotType;
    return;
}

static void
NEXUS_KeySlot_P_Finalizer(NEXUS_KeySlotHandle keyHandle)
{
    NEXUS_OBJECT_DESTROY(NEXUS_KeySlot, keyHandle);
    BKNI_Free(keyHandle);
    return;
}

static NEXUS_OBJECT_CLASS_MAKE(NEXUS_KeySlot, NEXUS_P_Core_FreeKeySlotHandle_locked);

void
NEXUS_P_Core_FreeKeySlotHandle(NEXUS_KeySlotHandle keyHandle)
{
    NEXUS_LockModule();
    NEXUS_P_Core_FreeKeySlotHandle_locked(keyHandle);
    NEXUS_UnlockModule();
    return;
}

static void NEXUS_VideoInput_P_Finalizer(NEXUS_VideoInputHandle videoInput)
{
    BSTD_UNUSED(videoInput);
    return;
}

NEXUS_VideoInputHandle NEXUS_VideoInput_Create(void)
{
    (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_VideoInput, NEXUS_VideoInput_Destroy);


