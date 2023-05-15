/***************************************************************************
 *     (c)2007-2014 Broadcom Corporation
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
 * $brcm_Workfile: nexus_astm.c $
 * $brcm_Revision: PROD_HUMAX_URSR/2 $
 * $brcm_Date: 8/25/14 2:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/astm/src/nexus_astm.c $
 * 
 * PROD_HUMAX_URSR/2   8/25/14 2:52p scho
 * PROD_HUMAX_URSR: check for decoder lifecycle event sans stc channel
 *
 * PROD_HUMAX_URSR/1   8/25/14 2:48p scho
 * PROD_HUMAX_URSR: Check that stc channel is non-NULL before using it in
 *  ResolveAdaptiveModes
 * 
 * 66   9/27/12 2:26p erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 65   9/21/12 3:26a bandrews
 * SW7231-979: ensure we are comparing against the correct PCR; allow up
 *  to 200 ms negative margin
 * 
 * 64   7/5/12 4:01p bandrews
 * SW7231-876: fixed
 * 
 * 63   6/28/12 6:54p bandrews
 * SW7550-798: merge to main
 * 
 * SW7550-798/3   6/20/12 6:35p bandrews
 * SW7550-798: also compute and apply stc channel tsm mode
 * 
 * SW7550-798/2   6/19/12 9:20p bandrews
 * SW7550-798: merge from main
 * 
 * 62   6/19/12 12:41p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7550-798/1   6/15/12 8:35p bandrews
 * SW7550-798: prefer switching to PTS master mode instead of vsync if
 *  PCR/PTS diff is bad but PTS/PTS diff is ok
 * 
 * 61   5/21/12 4:03p bandrews
 * SW7425-3096: whoops, fixed
 * 
 * 60   5/17/12 6:34p bandrews
 * SW7425-3082: fix setting application sequence
 * 
 * 59   5/8/12 6:27p bandrews
 * SW3556-677: disabled ALC as some customers are still attempting to use
 *  it even though it doesn't work
 * 
 * 58   5/7/12 8:06p bandrews
 * SW7231-772: fix warning message for audio case
 * 
 * SW7550-791/1   5/7/12 7:49p bandrews
 * SW7550-791: fix API to only use one struct for PRC; fix mapping in
 *  source
 * 
 * 56   4/20/12 4:04p bandrews
 * SW7358-273: fixed to match other loops using these variables
 * 
 * 55   4/16/12 4:59p bandrews
 * SW7231-772: check return
 * 
 * 54   2/21/12 3:27p bandrews
 * SW7550-691: fix debug message warnings
 * 
 * 53   2/21/12 2:19p erickson
 * SW7425-2130: fix debug warnings
 * 
 * 52   2/20/12 8:17p bandrews
 * SW7550-691: merge to main
 * 
 * 51   12/15/11 2:49p bandrews
 * SW3548-1159: missing a module lock
 * 
 * 50   12/7/11 2:56p bandrews
 * SW7335-1330: must not use memset to init the settings; instead get them
 *  from the decoder, preserving previous settings for callbacks and such
 * 
 * 49   11/3/11 4:45p bandrews
 * SW7231-391: fix warning
 * 
 * 48   10/31/11 7:46p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 47   9/2/11 10:21a dlwin
 * SW7335-1330: Coverity: 35222, UNINIT
 * 
 * 46   8/17/11 5:01p jrubio
 * SW7346-456: fix coverity issues CID35192-CID35195
 * 
 * 45   8/8/11 8:23p bandrews
 * SW3548-1159: merge to main
 * 
 * SW3548-1159/2   3/11/11 8:39p bandrews
 * SW3548-1159: must reset firstPtsTsm when each decoder is restarted
 * 
 * SW3548-1159/1   11/2/10 6:39p bandrews
 * SW3548-1159: added non-TSM mode recovery methods and some options for
 *  controlling ASTM better
 * 
 * 44   8/8/11 6:24p bandrews
 * SWDTV-7819: add default for tsm threshold adjustment
 * 
 * SW7550-691/3   3/3/11 7:54p bandrews
 * SW7550-691: correct compile error in DEBUG=n mode for trace message
 * 
 * SW7550-691/2   3/2/11 11:42p bandrews
 * SW7550-691: ensure that astm handles dubious app calling order of
 *  setsettings after a presenter it manages is running
 * 
 * SW7550-691/1   3/1/11 10:06p bandrews
 * SW7550-691: ensure astm does not apply settings when it is stopped
 * 
 * 43   10/26/10 12:14p bandrews
 * SW7125-642: reorder application of ASTM state
 * 
 * 42   7/14/10 7:45p bandrews
 * SW3548-1161: get default config for presentation and clock coupling
 * 
 * 41   2/9/10 8:26p VISHK
 * SW3548-1161: nexus_astm: expose all tunable params from astmlib
 * 
 * 40   7/17/09 7:10p bandrews
 * PR49215: astm dvr support
 * 
 * 39   6/12/09 5:58p bandrews
 * PR52472: Use phase error instead of computing PCR - STC directly
 * 
 * 38   6/5/09 5:33p bandrews
 * PR52503: Moved singleton counter/idgen to astmlib struct
 * 
 * 37   4/30/09 3:37p bandrews
 * PR51684: Default to false until more robust
 * 
 * 36   2/6/09 2:16p bandrews
 * PR50938: added debug for when ASTM starts
 * 
 * 35   1/26/09 10:55a erickson
 * PR51468: global variable naming convention
 *
 * 34   1/20/09 10:12p bandrews
 * PR50938: merge
 *
 * PR50938/1   1/8/09 11:11p bandrews
 * PR50938: Added capability to disable automatic lifecycle control
 *
 * 33   1/6/09 12:37a bandrews
 * PR50848: Added static keyword to global state
 *
 * 32   12/31/08 1:05p jgarrett
 * PR 50783: Adding runtime option to disable ASTM
 *
 * 32   12/31/08 1:03p jgarrett
 * PR 50783: Adding runtime option to disable ASTM
 *
 * 31   12/17/08 3:28p bandrews
 * PR49720: Added watchdog recovery to astm
 *
 * 30   12/17/08 2:37p bandrews
 * PR50427: Disable ASTM in dependencies on disconnect of said
 *  dependencies
 *
 * 29   12/12/08 6:13p bandrews
 * PR50079: fixed test framework
 *
 * 28   12/8/08 4:36p bandrews
 * PR41322: auto start astm from decoder start signal
 *
 * 27   12/8/08 2:53p bandrews
 * PR49568: staticize the connect/disconnect functions
 *
 * 26   12/4/08 8:17p bandrews
 * PR49489: Clean up
 *
 * 25   12/2/08 9:15p bandrews
 * PR49489: Ensure defaults are applied to dependencies to allow user
 *  overrides
 *
 * 24   12/1/08 3:21p bandrews
 * PR49489: Propagate playback flag to decoder interfaces
 *
 * 23   11/26/08 2:38p bandrews
 * PR49671: Fixed bug in setsettings
 *
 * 22   11/24/08 8:13p bandrews
 * PR49527: Make Start return an error
 *
 * 21   11/24/08 5:58p katrep
 * PR49539: compiler error
 *
 * 20   11/21/08 9:04p bandrews
 * PR49539: Fixed
 *
 * 19   11/21/08 9:02p bandrews
 * PR49535: Fixed
 *
 * 18   11/21/08 8:58p bandrews
 * PR49536: Fixed
 *
 * 17   11/21/08 8:53p bandrews
 * PR49568: Create/Destroy presenters in Create/Destroy (not SetSettings).
 *  Allow disconnection of decoders.  Allow single audio decoder.
 *
 * 16   11/18/08 9:42p bandrews
 * PR49213: Fixed debug message arg order
 *
 * 15   11/18/08 7:45p bandrews
 * PR49212: ASTM overrides user settings, but it should not destroy them
 *  when it does so
 *
 * 14   11/17/08 7:33p bandrews
 * PR49213: Fix order of initialization of contexts and added ready flag
 *  for SetSettings
 *
 * 13   9/18/08 4:18p erickson
 * PR47111: fix warnings
 *
 * 12   9/18/08 3:29p erickson
 * PR46838: merge
 *
 * PR46838/1   9/17/08 3:27p dyzhang
 * PR46838: memory leak if do switch channel stress test(kylin will fault
 *  after switch channel over 700 times). Astm presenter need to be
 *  destoried when destorying the astm.
 *
 * 11   8/28/08 11:14a erickson
 * PR46151: merge
 *
 * PR46151/2   8/27/08 5:17p dyzhang
 * PR46151: fix a warning
 *
 * PR46151/1   8/27/08 4:58p dyzhang
 * PR46151: _isr callbacks installed for astm in videodecoder,
 *  audiodecoder and transport are installed when creating astm but not
 *  uninstalled when destorying astm
 *
 * 10   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 9   4/3/08 2:40p bandrews
 * PR40090: sync needs separate context for syslib callbacks and synclib
 *  callbacks
 *
 * 8   4/2/08 11:31a erickson
 * PR40198: fix DEBUG=n warning
 *
 * 7   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 6   3/27/08 6:50p bandrews
 * PR40090: upgraded tsm mode change messages to warnings
 *
 * 5   3/27/08 5:51p bandrews
 * PR40090: default stc tsm mode for adaptive to stc master; fixed debug
 *  print of video tsm pass
 *
 * 4   3/26/08 9:47p bandrews
 * PR40090: stc channel must be passed to astm before video is started
 *
 * 3   3/26/08 5:11p bandrews
 * PR40090: syslib needs context of at least module handle
 *
 * 2   3/24/08 10:53p bandrews
 * PR40090: astm integration
 *
 * 1   3/24/08 9:30p bandrews
 * PR40090: astm impl
 **************************************************************************/
#include "nexus_astm_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_syslib_framework.h"
#include "priv/nexus_video_decoder_priv.h"
#include "priv/nexus_audio_decoder_priv.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_timebase_priv.h"
#include "bastmlib.h"
#if NEXUS_HAS_ASTM_TEST_SUPPORT
#include "nexus_astm_test_extensions.h"
#endif

BDBG_MODULE(nexus_astm);

#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */
#define NEXUS_ASTM_P_PRINT_SETTINGS 0 /* set to 1 to enable printing settings on start */

static NEXUS_Error NEXUS_Astm_P_Start(NEXUS_AstmHandle astm);
static void NEXUS_Astm_P_Stop(NEXUS_AstmHandle astm);
static void NEXUS_Astm_P_TsmRecoveryAcquisitionTask(void * context);
static void NEXUS_Astm_P_TsmRecoveryTrackingTimeoutHandler(void * context);

NEXUS_ModuleHandle g_NEXUS_astmModule;

static struct
{
    NEXUS_AstmModuleSettings settings;
    NEXUS_SYSlib_ContextHandle syslibContext;
} g_astm;

void NEXUS_AstmModule_GetDefaultSettings(NEXUS_AstmModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_AstmModule_Init(const NEXUS_AstmModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_SYSlib_ContextSettings syslibSettings;

    BDBG_ASSERT(!g_NEXUS_astmModule);

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pSettings->modules.videoDecoder);
    BDBG_ASSERT(pSettings->modules.audio);
    BDBG_ASSERT(pSettings->modules.transport);

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    g_NEXUS_astmModule = NEXUS_Module_Create("astm", &moduleSettings);
    if (!g_NEXUS_astmModule) {
        return NULL;
    }
    NEXUS_LockModule();

    g_astm.settings = *pSettings;

    NEXUS_SYSlib_GetDefaultContextSettings_priv(&syslibSettings);
    syslibSettings.module = NEXUS_MODULE_SELF;
    NEXUS_SYSlib_CreateContext_priv(&g_astm.syslibContext, &syslibSettings);

    NEXUS_UnlockModule();
    return g_NEXUS_astmModule;
}

void NEXUS_AstmModule_Uninit()
{
    NEXUS_LockModule();
    NEXUS_SYSlib_DestroyContext_priv(g_astm.syslibContext);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)	
    g_astm.syslibContext = NULL;
#endif
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_astmModule);
    g_NEXUS_astmModule = NULL;
}

/****************************************
* API functions
***************/

struct NEXUS_AstmDecoderStatus
{
    bool started;
    uint32_t pts;
    int32_t ptsStcDiff;
    unsigned int decodedCount;
    struct
    {
        unsigned int address;
        unsigned int size;
    } tsmLog;
};

struct NEXUS_AstmDecoderSettings
{
    int32_t ptsOffset;
    bool tsm;
};

typedef void (*NEXUS_Astm_DecoderStatusAccessor_isr)(void * decoder, struct NEXUS_AstmDecoderStatus * status);
typedef void (*NEXUS_Astm_DecoderStatusAccessor)(void * decoder, struct NEXUS_AstmDecoderStatus * status);
typedef void (*NEXUS_Astm_DecoderSettingsMutator)(void * decoder, const struct NEXUS_AstmDecoderSettings * settings);
typedef void (*NEXUS_Astm_DecoderSettingsAccessor)(void * decoder, struct NEXUS_AstmDecoderSettings * settings);

/* Used to collect information to process callbacks from around the system */
struct NEXUS_AstmContext
{
    NEXUS_AstmHandle astm;
    NEXUS_StcChannelHandle stc;
    bool started;
    void * decoder;
#ifdef BDBG_DEBUG_BUILD
    const char * decoderName;
#endif
    NEXUS_Astm_DecoderStatusAccessor_isr getStatus_isr;
    NEXUS_Astm_DecoderStatusAccessor getStatus;
    NEXUS_Astm_DecoderSettingsAccessor getSettings;
    NEXUS_Astm_DecoderSettingsMutator setSettings;
    BASTMlib_Presenter_Handle presenter;
    BKNI_EventHandle watchdogEvent;
    NEXUS_EventCallbackHandle watchdogEventHandler;
    BKNI_EventHandle lifecycleEvent;
    NEXUS_EventCallbackHandle lifecycleEventHandler;

    unsigned passEventCount;
    unsigned lastDecodedCount;
    NEXUS_TimerHandle tsmRecoveryAcquisitionTimer;
    NEXUS_TimerHandle tsmRecoveryTrackingTimer;
    unsigned ptsStcDiffAdjustmentThreshold;
    unsigned tsmRecoveryAcquisitionPeriod;
    unsigned tsmRecoveryTrackingTimeout;
    BKNI_EventHandle tsmPassEvent;
    NEXUS_EventCallbackHandle tsmPassEventHandler;
    uint32_t maxAllowableFirstPtsStcDiff;
    BKNI_EventHandle firstPtsEvent;
    NEXUS_EventCallbackHandle firstPtsEventHandler;
    bool firstPtsTsm;
    bool firstPtsReceived;
    uint32_t firstPts;
    bool tsmThresholdAdjustment;
    bool manageRateControl;
    NEXUS_ModuleHandle module;
    uint32_t firstPtsArrivalPcr;
    bool firstPtsArrivalPcrValid;
};

#define NEXUS_ASTM_PRESENTERS 3

struct NEXUS_Astm
{
    NEXUS_OBJECT(NEXUS_Astm);
    NEXUS_AstmSettings settings;
    NEXUS_TransportType transportType;
    NEXUS_TimebaseHandle timebase;
    struct NEXUS_AstmContext presenterContexts[NEXUS_ASTM_PRESENTERS];

    /* syslib info */
    BASTMlib_Handle lib;
    BASTMlib_Status status;

    bool ready;
    bool started;
    uint32_t lastPcr;
    bool pcrReceived;
    bool usePtsMasterMode;
};

#if 0 /* old recovery scheme */
#define NEXUS_ASTM_DEFAULT_PASS_EVENT_COUNT_THRESHOLD pcfg.uiPassEventCountThreshold
#else /* new TSM recovery scheme */
#define NEXUS_ASTM_DEFAULT_PASS_EVENT_COUNT_THRESHOLD 0
#endif
#define NEXUS_ASTM_DEFAULT_PTS_STC_DIFF_ADJ_THRESHOLD 90 /* 45 KHz ticks -> 2 ms */
#define NEXUS_ASTM_DEFAULT_TSM_RECOVERY_ACQ_PERIOD 200 /* ms */
#define NEXUS_ASTM_DEFAULT_TSM_RECOVERY_TRACKING_TIMEOUT 1000 /* ms */
#define NEXUS_ASTM_DEFAULT_MAX_ALLOWABLE_VIDEO_FIRST_PTS_STC_DIFF 454500 /* 10.1 seconds, want this outside the avc spec */
#define NEXUS_ASTM_DEFAULT_MAX_ALLOWABLE_AUDIO_FIRST_PTS_STC_DIFF 112500 /* 2.5 seconds, < 3 second live audio buffer */
#define NEXUS_ASTM_P_SYNC_LIMIT 5000

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_ASTM_ASYNC_TIMER)
static unsigned long ulPresentationChangeCnt = 0;
static unsigned long ulClockCouplingChangeCnt = 0;
#endif

void NEXUS_Astm_GetDefaultSettings(NEXUS_AstmSettings *pSettings)
{
    BASTMlib_Config libcfg;
    BASTMlib_Presenter_Config pcfg;
    unsigned int i = 0;

    BASTMlib_GetDefaultConfig(&libcfg);
    BASTMlib_Presenter_GetDefaultConfig(&pcfg);
    
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->enabled = true;
    pSettings->enableAutomaticLifecycleControl = false; /* PR:50938 */ /* PR:51684 updated to false */
    pSettings->syncLimit = NEXUS_ASTM_P_SYNC_LIMIT;
    pSettings->stcMaster = NULL;
    pSettings->clockCoupling = NEXUS_AstmClockCoupling_eAdaptive;
    pSettings->stcSource = NEXUS_AstmStcSource_eAdaptive;
    pSettings->presentationRateControl = NEXUS_AstmPresentationRateControl_eAdaptive;

    /* clock coupling task default cfg */    
    pSettings->clockCouplingConfig.initialAcquisitionTime = libcfg.sClockCoupling.uiInitialAcquisitionTime;
    pSettings->clockCouplingConfig.processingFrequency = libcfg.sClockCoupling.uiProcessingFrequency;
    pSettings->clockCouplingConfig.idealProcessingFrequency = libcfg.sClockCoupling.uiProcessingFrequency;
    pSettings->clockCouplingConfig.settlingTime = libcfg.sClockCoupling.uiSettlingTime;

    pSettings->clockCouplingConfig.minimumTimeBetweenEvents = libcfg.sClockCoupling.uiMinimumTimeBetweenEvents;
    pSettings->clockCouplingConfig.deviationThreshold = libcfg.sClockCoupling.uiDeviationThreshold;
    pSettings->clockCouplingConfig.deviantCountThreshold = libcfg.sClockCoupling.uiDeviantCountThreshold;
    pSettings->clockCouplingConfig.idealCountThreshold = libcfg.sClockCoupling.uiIdealCountThreshold;

    /* presentation task default cfg */
    pSettings->presentationConfig.initialAcquisitionTime = libcfg.sPresentation.uiInitialAcquisitionTime;
    pSettings->presentationConfig.processingFrequency = libcfg.sPresentation.uiProcessingFrequency;
    pSettings->presentationConfig.tsmDisabledWatchdogTimeout = libcfg.sPresentation.uiTsmDisabledWatchdogTimeout;
    pSettings->presentationConfig.settlingTime = libcfg.sPresentation.uiSettlingTime;

    pSettings->videoPresenterConfig.minimumTimeBetweenEvents = pcfg.uiMinimumTimeBetweenEvents;
    pSettings->videoPresenterConfig.failEventCountThreshold = pcfg.uiFailEventCountThreshold;
    pSettings->videoPresenterConfig.passEventCountThreshold = NEXUS_ASTM_DEFAULT_PASS_EVENT_COUNT_THRESHOLD;
    pSettings->videoPresenterConfig.ptsStcDiffAdjustmentThreshold = NEXUS_ASTM_DEFAULT_PTS_STC_DIFF_ADJ_THRESHOLD;
    pSettings->videoPresenterConfig.tsmRecoveryAcquisitionPeriod = NEXUS_ASTM_DEFAULT_TSM_RECOVERY_ACQ_PERIOD;
    pSettings->videoPresenterConfig.tsmRecoveryTrackingTimeout = NEXUS_ASTM_DEFAULT_TSM_RECOVERY_TRACKING_TIMEOUT;
    pSettings->videoPresenterConfig.maxAllowableFirstPtsStcDiff = NEXUS_ASTM_DEFAULT_MAX_ALLOWABLE_VIDEO_FIRST_PTS_STC_DIFF;
    pSettings->videoPresenterConfig.tsmThresholdAdjustment = false; /* default off for video */
    pSettings->videoPresenterConfig.manageRateControl = true;
    
    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        pSettings->audioPresenterConfig[i].failEventCountThreshold = pcfg.uiFailEventCountThreshold;
        pSettings->audioPresenterConfig[i].minimumTimeBetweenEvents = pcfg.uiMinimumTimeBetweenEvents;
        pSettings->audioPresenterConfig[i].passEventCountThreshold = NEXUS_ASTM_DEFAULT_PASS_EVENT_COUNT_THRESHOLD;
        pSettings->audioPresenterConfig[i].ptsStcDiffAdjustmentThreshold = NEXUS_ASTM_DEFAULT_PTS_STC_DIFF_ADJ_THRESHOLD;
        pSettings->audioPresenterConfig[i].tsmRecoveryAcquisitionPeriod = NEXUS_ASTM_DEFAULT_TSM_RECOVERY_ACQ_PERIOD;
        pSettings->audioPresenterConfig[i].tsmRecoveryTrackingTimeout = NEXUS_ASTM_DEFAULT_TSM_RECOVERY_TRACKING_TIMEOUT;
        pSettings->audioPresenterConfig[i].maxAllowableFirstPtsStcDiff = NEXUS_ASTM_DEFAULT_MAX_ALLOWABLE_AUDIO_FIRST_PTS_STC_DIFF;
        pSettings->audioPresenterConfig[i].tsmThresholdAdjustment = true; /* default on for audio */
        pSettings->audioPresenterConfig[i].manageRateControl = true;
    }
}

#if !BDBG_NO_WRN
static const char * stcSourceStrings[] =
{
    "Adaptive",
    "PCR",
    "PTS",
    NULL
};
#endif


static NEXUS_Error NEXUS_Astm_P_SetDependencyAstmBehavior(NEXUS_AstmHandle astm, bool enabled)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoDecoderAstmSettings videoDecoderAstmSettings;
    NEXUS_AudioDecoderAstmSettings audioDecoderAstmSettings;
    NEXUS_StcChannelAstmSettings stcChannelAstmSettings;
    NEXUS_TimebaseAstmSettings timebaseAstmSettings;
    unsigned int i;

    BDBG_MSG(("Setting dependency ASTM behavior to %s", enabled ? "enabled" : "disabled"));
    if (astm->settings.videoDecoder)
    {
        NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
        NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
        videoDecoderAstmSettings.enableAstm = enabled;
        rc = NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
        NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
        if (rc) goto err;
    }

    NEXUS_Module_Lock(g_astm.settings.modules.audio);
    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        if (astm->settings.audioDecoder[i])
        {
            NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
            audioDecoderAstmSettings.enableAstm = enabled;
            rc = NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
        }
        if (rc) break;
    }
    NEXUS_Module_Unlock(g_astm.settings.modules.audio);
    if (rc) goto err;

    /* lock xpt for stc channel and timebase */
    NEXUS_Module_Lock(g_astm.settings.modules.transport);

    if (astm->settings.stcChannel)
    {
        NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
        stcChannelAstmSettings.enabled = enabled;
        rc = NEXUS_StcChannel_SetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
        if (rc) goto xpt_unlock;
    }

    if (astm->timebase)
    {
        NEXUS_Timebase_GetAstmSettings_priv(astm->timebase, &timebaseAstmSettings);
        timebaseAstmSettings.enabled = enabled;
        rc = NEXUS_Timebase_SetAstmSettings_priv(astm->timebase, &timebaseAstmSettings);
        if (rc) goto xpt_unlock;
    }

xpt_unlock:
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);

err:
    return rc;
}

static void NEXUS_Astm_P_ResolveAdaptiveModes(NEXUS_AstmHandle astm, NEXUS_AstmClockCoupling * clockCoupling, NEXUS_AstmStcSource * stcSource, NEXUS_AstmPresentationRateControl * presentationRateControl)
{
    unsigned int i;
    
    /* if we are adaptive and one of the presenters has already started, we 
    can check the STC channel for whether we should do live or pb */
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        if (astm->presenterContexts[i].started)
        {
            NEXUS_StcChannelSettings stcChannelSettings;
            
            if (astm->settings.stcChannel)
            {
	            NEXUS_StcChannel_GetSettings(astm->settings.stcChannel, &stcChannelSettings);

	            if (*stcSource == NEXUS_AstmStcSource_eAdaptive)
	            {
	                if (stcChannelSettings.mode == NEXUS_StcChannelMode_ePcr)
	                {
                            BDBG_MSG(("%p: STC channel user mode was PCR; setting stc source to PCR.", (void *)astm));
	                    *stcSource = NEXUS_AstmStcSource_eClockReference;
	                }
	                else
	                {
                            BDBG_MSG(("%p: STC channel user mode was Auto/Host; setting stc source to PTS.", (void *)astm));
	                    *stcSource = NEXUS_AstmStcSource_ePts;
	                }
	            }

	            if (*clockCoupling == NEXUS_AstmClockCoupling_eAdaptive)
	            {
	                if (stcChannelSettings.mode == NEXUS_StcChannelMode_ePcr)
	                {
                            BDBG_MSG(("%p: STC channel user mode was PCR; setting clock coupling to input clock.", (void *)astm));
	                    *clockCoupling = NEXUS_AstmClockCoupling_eInputClock;
	                }
	                else
	                {
                            BDBG_MSG(("%p: STC channel user mode was Auto/Host; setting clock coupling to internal clock.", (void *)astm));
	                    *clockCoupling = NEXUS_AstmClockCoupling_eInternalClock;
	                }
	            }

	            break;
        }
            else
            {
                BDBG_MSG(("%p: ResolveAdaptiveModes called while STC channel not set; setting clock coupling to internal clock and stc source to PTS", (void *)astm));
                *stcSource = NEXUS_AstmStcSource_ePts;
                *clockCoupling = NEXUS_AstmClockCoupling_eInternalClock;
            }
        }
    }

    if (*stcSource == NEXUS_AstmStcSource_eAdaptive)
    {
        BDBG_MSG(("Defaulting adaptive stc source to clock reference"));
        *stcSource = NEXUS_AstmStcSource_eClockReference;
    }

    if (*clockCoupling == NEXUS_AstmClockCoupling_eAdaptive)
    {
        BDBG_MSG(("Defaulting adaptive clock coupling to input clock"));
        *clockCoupling = NEXUS_AstmClockCoupling_eInputClock;
    }

    if (*presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive)
    {
        BDBG_MSG(("Defaulting adaptive presentation rate control to time stamp"));
        *presentationRateControl = NEXUS_AstmPresentationRateControl_eTimeStamp;
    }
}

static NEXUS_Error NEXUS_Astm_P_ApplyStcSource(NEXUS_AstmHandle astm, NEXUS_AstmStcSource stcSource, void * stcMaster, unsigned syncLimit)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelAstmSettings stcChannelAstmSettings;
    NEXUS_VideoDecoderAstmSettings videoDecoderAstmSettings;
    NEXUS_AudioDecoderAstmSettings audioDecoderAstmSettings;
    unsigned int i;

    if (stcSource == NEXUS_AstmStcSource_eAdaptive)
    {
        BDBG_ERR(("Cannot apply adaptive STC source"));
        return NEXUS_NOT_SUPPORTED;
    }

    BDBG_WRN(("Stc source: %s", stcSourceStrings[stcSource]));

    if (stcSource == NEXUS_AstmStcSource_eClockReference)
    {
        if (astm->settings.videoDecoder)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
            NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            videoDecoderAstmSettings.enablePlayback = false;
            videoDecoderAstmSettings.syncLimit = 0;
            rc = NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
            if (rc) goto err;
        }
        else
        {
            BDBG_MSG(("%d: video decoder was NULL", __LINE__));
        }

        NEXUS_Module_Lock(g_astm.settings.modules.audio);
        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            if (astm->settings.audioDecoder[i])
            {
                NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                audioDecoderAstmSettings.enablePlayback = false;
                audioDecoderAstmSettings.syncLimit = 0;
                rc = NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
            }
            else
            {
                BDBG_MSG(("%d: audio decoder %d was NULL", __LINE__, i));
            }
            if (rc) break;
        }
        NEXUS_Module_Unlock(g_astm.settings.modules.audio);
        if (rc) goto err;

        if (astm->settings.stcChannel)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.transport);
            NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
            stcChannelAstmSettings.mode = NEXUS_StcChannelMode_ePcr;
            stcChannelAstmSettings.syncLimit = 0;
            rc = NEXUS_StcChannel_SetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.transport);
            if (rc) goto err;
        }
        else
        {
            BDBG_WRN(("Attempted to apply stc source state to a NULL stc channel"));
        }
    }
    else if (stcSource == NEXUS_AstmStcSource_ePts)
    {
        if (astm->settings.videoDecoder)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
            NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            if (stcMaster == astm->settings.videoDecoder)
            {
                BDBG_MSG(("Stc master: video"));
                videoDecoderAstmSettings.syncLimit = syncLimit;
            }
            else
            {
                videoDecoderAstmSettings.syncLimit = 0;
            }

            videoDecoderAstmSettings.enablePlayback = true;
            
            rc = NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
            if (rc) goto err;
        }
        else
        {
            BDBG_MSG(("%d: video decoder was NULL", __LINE__));
        }

        NEXUS_Module_Lock(g_astm.settings.modules.audio);
        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            if (astm->settings.audioDecoder[i])
            {
                NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                if (stcMaster == astm->settings.audioDecoder[i])
                {
                    BDBG_MSG(("Stc master: audio %d", i));
                    audioDecoderAstmSettings.syncLimit = syncLimit;
                }
                else
                {
                    audioDecoderAstmSettings.syncLimit = 0;
                }
                
                audioDecoderAstmSettings.enablePlayback = true;
                
                rc = NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                if (rc) break;
            }
            else
            {
                BDBG_MSG(("%d: audio decoder %d was NULL", __LINE__, i));
            }
        }
        NEXUS_Module_Unlock(g_astm.settings.modules.audio);
        if (rc) goto err;

        if (astm->settings.stcChannel)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.transport);
            NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
            stcChannelAstmSettings.mode = NEXUS_StcChannelMode_eAuto;
            stcChannelAstmSettings.syncLimit = syncLimit;
            rc = NEXUS_StcChannel_SetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.transport);
            if (rc) goto err;
        }
        else
        {
            BDBG_WRN(("Attempted to apply stc source state to a NULL stc channel"));
        }
    }

    goto end;

err:

end:

    return rc;
}


#if !(BDBG_NO_WRN && BDBG_NO_MSG)
static const char * clockCouplingStrings[] =
{
    "Adaptive",
    "Input clock",
    "Internal clock",
    NULL
};
#endif

static NEXUS_Error NEXUS_Astm_P_ApplyClockCoupling(NEXUS_AstmHandle astm, NEXUS_AstmClockCoupling clockCoupling)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_TimebaseAstmSettings timebaseAstmSettings;

    if (astm->timebase)
    {
        if (clockCoupling == NEXUS_AstmClockCoupling_eAdaptive)
        {
            BDBG_ERR(("Cannot apply adaptive clock coupling"));
            return NEXUS_NOT_SUPPORTED;
        }

        BDBG_WRN(("Clock coupling: %s", clockCouplingStrings[clockCoupling]));

        NEXUS_Module_Lock(g_astm.settings.modules.transport);
        NEXUS_Timebase_GetAstmSettings_priv(astm->timebase, &timebaseAstmSettings);

        switch (clockCoupling)
        {
            case NEXUS_AstmClockCoupling_eInputClock:
                timebaseAstmSettings.clockCoupling = NEXUS_TimebaseClockCoupling_eInputClock;
                break;
            case NEXUS_AstmClockCoupling_eInternalClock:
                timebaseAstmSettings.clockCoupling = NEXUS_TimebaseClockCoupling_eInternalClock;
                break;
            default:
                break;
        }

        rc = NEXUS_Timebase_SetAstmSettings_priv(astm->timebase, &timebaseAstmSettings);
        NEXUS_Module_Unlock(g_astm.settings.modules.transport);
        if (rc) goto err;
    }
    else
    {
        BDBG_WRN(("Attempted to apply clock coupling state to an invalid timebase"));
    }

    goto end;

err:

end:

    return rc;
}

#if !BDBG_NO_WRN
static const char * presentationRateControlStrings[] =
{
    "Adaptive",
    "Time stamp",
    "Output clock",
    NULL
};
#endif

static void NEXUS_Astm_P_CancelPendingTimers(NEXUS_AstmHandle astm)
{
    unsigned int i;

    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        if (astm->presenterContexts[i].tsmRecoveryAcquisitionTimer)
        {
            NEXUS_CancelTimer(astm->presenterContexts[i].tsmRecoveryAcquisitionTimer);
            astm->presenterContexts[i].tsmRecoveryAcquisitionTimer = NULL;
        }
        if (astm->presenterContexts[i].tsmRecoveryTrackingTimer)
        {
            NEXUS_CancelTimer(astm->presenterContexts[i].tsmRecoveryTrackingTimer);
            astm->presenterContexts[i].tsmRecoveryTrackingTimer = NULL;
        }
    }
}

static NEXUS_Error NEXUS_Astm_P_ApplyPresentationRateControl(NEXUS_AstmHandle astm, NEXUS_AstmPresentationRateControl presentationRateControl)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoDecoderAstmSettings videoDecoderAstmSettings;
    NEXUS_AudioDecoderAstmSettings audioDecoderAstmSettings;
    unsigned int i;

    if (presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive)
    {
        BDBG_ERR(("Cannot apply adaptive presentation rate control"));
        return NEXUS_NOT_SUPPORTED;
    }

    BDBG_WRN(("Presentation rate control: %s", presentationRateControlStrings[presentationRateControl]));

    /* switching TSM modes means clearing/restarting TSM recovery tasks */
    NEXUS_Astm_P_CancelPendingTimers(astm);

    if (presentationRateControl == NEXUS_AstmPresentationRateControl_eTimeStamp)
    {
        if (astm->settings.videoDecoder && astm->settings.videoPresenterConfig.manageRateControl)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
            NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            videoDecoderAstmSettings.enableTsm = true 
                && astm->presenterContexts[0].firstPtsTsm; /* don't enable TSM if we've disabled it for buffering reasons */
            videoDecoderAstmSettings.ptsOffset = 0;
            rc = NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
            if (rc) goto err;
        }

        NEXUS_Module_Lock(g_astm.settings.modules.audio);
        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            if (astm->settings.audioDecoder[i] && astm->settings.audioPresenterConfig[i].manageRateControl)
            {
                NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                audioDecoderAstmSettings.enableTsm = true
                    && astm->presenterContexts[i + 1].firstPtsTsm; /* don't enable TSM if we've disabled it for buffering reasons */;
                audioDecoderAstmSettings.ptsOffset = 0;
                rc = NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                if (rc) break;
            }
        }
        NEXUS_Module_Unlock(g_astm.settings.modules.audio);
        if (rc) goto err;
    }
    else if (presentationRateControl == NEXUS_AstmPresentationRateControl_eOutputClock)
    {
        if (astm->settings.videoDecoder && astm->settings.videoPresenterConfig.manageRateControl)
        {
            NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
            NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            videoDecoderAstmSettings.enableTsm = false;
            rc = NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &videoDecoderAstmSettings);
            NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
            if (rc) goto err;

            /* start timer tasks for non-TSM mode recovery */
            BDBG_MSG(("Starting TSM recovery task for presenter %s", astm->presenterContexts[0].decoderName));
            astm->presenterContexts[0].tsmRecoveryAcquisitionTimer =
                NEXUS_ScheduleTimer(astm->settings.videoPresenterConfig.tsmRecoveryAcquisitionPeriod, 
                    &NEXUS_Astm_P_TsmRecoveryAcquisitionTask, &astm->presenterContexts[0]);
        }

        NEXUS_Module_Lock(g_astm.settings.modules.audio);
        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            if (astm->settings.audioDecoder[i] && astm->settings.audioPresenterConfig[i].manageRateControl)
            {
                NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                audioDecoderAstmSettings.enableTsm = false;
                rc = NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[i], &audioDecoderAstmSettings);
                if (rc) break;

                /* start timer tasks for non-TSM mode recovery */
                BDBG_MSG(("Starting TSM recovery task for presenter %s", astm->presenterContexts[i + 1].decoderName));
                astm->presenterContexts[i + 1].tsmRecoveryAcquisitionTimer =
                    NEXUS_ScheduleTimer(astm->settings.audioPresenterConfig[i].tsmRecoveryAcquisitionPeriod, 
                        &NEXUS_Astm_P_TsmRecoveryAcquisitionTask, &astm->presenterContexts[i + 1]);
            }
        }
        NEXUS_Module_Unlock(g_astm.settings.modules.audio);
        if (rc) goto err;
    }

    goto end;

err:

end:
    return rc;
}

#if !(BDBG_NO_WRN && BDBG_NO_MSG)
static const char * tsmModeStrings[] =
{
    "STC master",
    "video master",
    "audio master",
    "output master",
    NULL
};
#endif

static NEXUS_StcChannelTsmMode NEXUS_Astm_P_ComputeStcChannelTsmMode(
    NEXUS_AstmHandle astm,
    NEXUS_AstmStcSource stcSource,
    void * stcMaster,
    NEXUS_AstmPresentationRateControl presentationRateControl
)
{
    NEXUS_StcChannelTsmMode tsmMode = NEXUS_StcChannelTsmMode_eMax;

    if ((presentationRateControl == NEXUS_AstmPresentationRateControl_eTimeStamp
        && stcSource == NEXUS_AstmStcSource_eClockReference))
    {
        tsmMode = NEXUS_StcChannelTsmMode_eStcMaster;
    }
    else if (presentationRateControl == NEXUS_AstmPresentationRateControl_eTimeStamp
        && stcSource == NEXUS_AstmStcSource_ePts)
    {
        if (stcMaster == astm->settings.videoDecoder)
        {
            tsmMode = NEXUS_StcChannelTsmMode_eVideoMaster;
        }
        else if (stcMaster == NULL)
        {
            tsmMode = NEXUS_StcChannelTsmMode_eStcMaster; /* FCFS PTS in playback mode */
        }
        else
        {
            unsigned int i;

            for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
            {
                if (stcMaster == astm->settings.audioDecoder[i])
                {
                    tsmMode = NEXUS_StcChannelTsmMode_eAudioMaster;
                    break;
                }
            }
        }
    }
    else if (presentationRateControl == NEXUS_AstmPresentationRateControl_eOutputClock)
    {
        unsigned int i;
        bool manageVideoRateControl = true;
        bool manageAudioRateControl = true;

        manageVideoRateControl = astm->settings.videoPresenterConfig.manageRateControl;
        
        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            /* if at least 1 audio channel does not allow rate control management, this will be false */
            manageAudioRateControl = manageAudioRateControl && 
                astm->settings.audioPresenterConfig[i].manageRateControl;
        }

        if (manageVideoRateControl && manageAudioRateControl)
        {
            /* video and all audio channels allow rate control management, so output master mode */
            tsmMode = NEXUS_StcChannelTsmMode_eOutputMaster;
        }
        else if (manageVideoRateControl && !manageAudioRateControl)
        {
            /* at least one audio does not allow rate control management, but video allows it, so audio master mode */
            tsmMode = NEXUS_StcChannelTsmMode_eAudioMaster;
        }
        else if (!manageVideoRateControl && manageAudioRateControl)
        {
            /* video does not allow rate control management, but audio allows it, so video master mode */
            tsmMode = NEXUS_StcChannelTsmMode_eVideoMaster;
        }
        else
        {
            /* none of the presenters allow rate control management, leave as invalid Max setting */
            BDBG_WRN(("No presenters allow presentation rate control management"));
        }
    }

    return tsmMode;
}

static NEXUS_Error NEXUS_Astm_P_ApplyStcChannelTsmMode(
    NEXUS_AstmHandle astm,
    NEXUS_StcChannelTsmMode tsmMode
)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelAstmSettings stcChannelAstmSettings;

    BDBG_WRN(("STC channel tsm mode: %s", tsmModeStrings[tsmMode]));

    if (astm->settings.stcChannel)
    {
        NEXUS_Module_Lock(g_astm.settings.modules.transport);
        NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
        stcChannelAstmSettings.tsmMode = tsmMode;
        rc = NEXUS_StcChannel_SetAstmSettings_priv(astm->settings.stcChannel, &stcChannelAstmSettings);
        NEXUS_Module_Unlock(g_astm.settings.modules.transport);
    }
    else
    {
        BDBG_WRN(("Attempted to apply tsm mode to a NULL stc channel"));
    }

    return rc;
}

#if defined(BDBG_DEBUG_BUILD) && NEXUS_ASTM_P_PRINT_SETTINGS

static void NEXUS_Astm_P_PrintTimebaseAstmSettings(NEXUS_AstmHandle astm)
{
    NEXUS_TimebaseAstmSettings settings;
    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_Timebase_GetAstmSettings_priv(astm->timebase, &settings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);

    BDBG_MSG(("timebase: astm = %s", settings.enabled ? "enabled" : "disabled"));
    BDBG_MSG(("timebase: clock coupling = %s", clockCouplingStrings[settings.clockCoupling]));
    BDBG_MSG(("timebase: pcrReceived_isr = %p", settings.pcrReceived_isr));
    BDBG_MSG(("timebase: context = %p", settings.callbackContext));
}

#if !BDBG_NO_MSG
static const char * modeStrings[] =
{
    "PCR",
    "Auto",
    "Host",
    NULL
};
#endif

static void NEXUS_Astm_P_PrintStcChannelAstmSettings(NEXUS_AstmHandle astm)
{
    NEXUS_StcChannelAstmSettings settings;
    
    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &settings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);

    BDBG_MSG(("stc: astm = %s", settings.enabled ? "enabled" : "disabled"));
    BDBG_MSG(("stc: mode = %s", modeStrings[settings.mode]));
    BDBG_MSG(("stc: tsm mode = %s", tsmModeStrings[settings.tsmMode]));
    BDBG_MSG(("stc: sync limit = %u", settings.syncLimit));
}

static void NEXUS_Astm_P_PrintVideoDecoderAstmSettings(NEXUS_AstmHandle astm)
{
    NEXUS_VideoDecoderAstmSettings settings;
    
    NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
    NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &settings);
    NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);

    BDBG_MSG(("video: astm = %s", settings.enableAstm ? "enabled" : "disabled"));
    BDBG_MSG(("video: sync limit = %u", settings.syncLimit));
    BDBG_MSG(("video: tsm = %s", settings.enableTsm ? "enabled" : "disabled"));
    BDBG_MSG(("video: playback = %s", settings.enablePlayback ? "enabled" : "disabled"));
    BDBG_MSG(("video: tsmPass_isr = %p", settings.tsmPass_isr));
    BDBG_MSG(("video: tsmFail_isr = %p", settings.tsmFail_isr));
    BDBG_MSG(("video: tsmLog_isr = %p", settings.tsmLog_isr));
    BDBG_MSG(("video: lifecycle_isr = %p", settings.lifecycle_isr));
    BDBG_MSG(("video: watchdog_isr = %p", settings.watchdog_isr));
    BDBG_MSG(("video: context = %p", settings.callbackContext));
}

static void NEXUS_Astm_P_PrintAudioDecoderAstmSettings(NEXUS_AstmHandle astm, unsigned int index)
{
    NEXUS_AudioDecoderAstmSettings settings;
    
    NEXUS_Module_Lock(g_astm.settings.modules.audio);
    NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[index], &settings);
    NEXUS_Module_Unlock(g_astm.settings.modules.audio);

    BDBG_MSG(("audio %u: astm = %s", index, settings.enableAstm ? "enabled" : "disabled"));
    BDBG_MSG(("audio %u: sync limit = %u", index, settings.syncLimit));
    BDBG_MSG(("audio %u: tsm = %s", index, settings.enableTsm ? "enabled" : "disabled"));
    BDBG_MSG(("audio %u: playback = %s", index, settings.enablePlayback ? "enabled" : "disabled"));
    BDBG_MSG(("audio %u: tsmPass_isr = %p", index, settings.tsmPass_isr));
    BDBG_MSG(("audio %u: tsmFail_isr = %p", index, settings.tsmFail_isr));
    BDBG_MSG(("audio %u: tsmLog_isr = %p", index, settings.tsmLog_isr));
    BDBG_MSG(("audio %u: lifecycle_isr = %p", index, settings.lifecycle_isr));
    BDBG_MSG(("audio %u: watchdog_isr = %p", index, settings.watchdog_isr));
    BDBG_MSG(("audio %u: context = %p", index, settings.callbackContext));
}

static void NEXUS_Astm_P_PrintDependencyAstmSettings(NEXUS_AstmHandle astm)
{
    unsigned int i;
    bool audio = false;

    if (astm->timebase)
    {
        NEXUS_Astm_P_PrintTimebaseAstmSettings(astm);
    }
    else
    {
        BDBG_MSG(("No timebase configured for ASTM"));
    }
    if (astm->settings.stcChannel)
    {
        NEXUS_Astm_P_PrintStcChannelAstmSettings(astm);
    }
    else
    {
        BDBG_MSG(("No STC channel configured for ASTM"));
    }

    if (astm->settings.videoDecoder)
    {
        NEXUS_Astm_P_PrintVideoDecoderAstmSettings(astm);
    }
    else
    {
        BDBG_MSG(("No video decoder configured for ASTM"));
    }
    
    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        if (astm->settings.audioDecoder[i])
        {
            NEXUS_Astm_P_PrintAudioDecoderAstmSettings(astm, i);
            audio = true;
        }
    }
    if (!audio)
    {
        BDBG_MSG(("No audio decoders configured for ASTM"));
    }
}

#else

#define NEXUS_Astm_P_PrintTimebaseAstmSettings(astm)
#define NEXUS_Astm_P_PrintStcChannelAstmSettings(astm)
#define NEXUS_Astm_P_PrintVideoDecoderAstmSettings(astm)
#define NEXUS_Astm_P_PrintAudioDecoderAstmSettings(astm, i)
#define NEXUS_Astm_P_PrintDependencyAstmSettings(astm)

#endif

static BERR_Code NEXUS_Astmlib_P_PresentationStateChange(void * pvParm1, int iParm2)
{
    NEXUS_AstmHandle astm = (NEXUS_AstmHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_AstmStcSource stcSource;
    NEXUS_AstmPresentationRateControl presentationRateControl;
    NEXUS_StcChannelTsmMode tsmMode;
    void * stcMaster;

    BSTD_UNUSED(iParm2);
    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    BASTMlib_GetStatus(astm->lib, &astm->status);

    switch (astm->status.eStcSource)
    {
        case BASTMlib_StcSource_ePresenter:
            stcSource = NEXUS_AstmStcSource_ePts;
            break;
        case BASTMlib_StcSource_eClockReference:
        default:
            stcSource = NEXUS_AstmStcSource_eClockReference;
            break;
    }

    switch (astm->status.ePresentationRateControl)
    {
        case BASTMlib_PresentationRateControl_eOutputClock:
            presentationRateControl = NEXUS_AstmPresentationRateControl_eOutputClock;
            break;
        case BASTMlib_PresentationRateControl_eTimeStamp:
        default:
            presentationRateControl = NEXUS_AstmPresentationRateControl_eTimeStamp;
            break;
    }

    stcMaster = NULL;

    if (astm->status.hStcMaster == astm->presenterContexts[0].presenter)
    {
        stcMaster = astm->settings.videoDecoder;
    }
    else
    {
        unsigned int i;

        for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
        {
            if (astm->status.hStcMaster == astm->presenterContexts[i + 1].presenter)
            {
                stcMaster = astm->settings.audioDecoder[i];
            }
        }
    }

    /* this will enable/disable TSM in the decoders. 
    we want to disable it first, but enable it last */
    if (astm->settings.presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive
        && presentationRateControl == NEXUS_AstmPresentationRateControl_eOutputClock)
    {
        nrc = NEXUS_Astm_P_ApplyPresentationRateControl(astm, presentationRateControl);
        if (nrc) goto err;
    }

    /* changing pcrlib tsm mode must happen before we switch between Pcr and Auto, 
    so we handle any interrupts accordingly */
    /* if either stc source or presentation rate control are adaptive, it affects
    the stc channel tsm mode */
    if (astm->settings.stcSource == NEXUS_AstmStcSource_eAdaptive
        || astm->settings.presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive)
    {
        tsmMode = NEXUS_Astm_P_ComputeStcChannelTsmMode(astm, stcSource,
            stcMaster, presentationRateControl);
        if (tsmMode != NEXUS_StcChannelTsmMode_eMax)
        {
            nrc = NEXUS_Astm_P_ApplyStcChannelTsmMode(astm, tsmMode);
            if (nrc) goto err;
        }
    }

    /* this will enable/disable playback in the decoders and switch pcrlib between PCR and Auto
    this must occur after we've picked which pcrlib mode we want, otherwise Auto may be
    handled incorrectly */
    /* only apply the settings for the ones that are adaptive */
    if (astm->settings.stcSource == NEXUS_AstmStcSource_eAdaptive)
    {
        nrc = NEXUS_Astm_P_ApplyStcSource(astm, stcSource, stcMaster, astm->settings.syncLimit);
        if (nrc) goto err;
    }

    /* this will enable/disable TSM in the decoders. 
    we want to disable it first, but enable it last */
    if (astm->settings.presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive
        && presentationRateControl == NEXUS_AstmPresentationRateControl_eTimeStamp)
    {
        nrc = NEXUS_Astm_P_ApplyPresentationRateControl(astm, presentationRateControl);
        if (nrc) goto err;
    }
	
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_ASTM_ASYNC_TIMER)
	/* Diff between new & previous status is aleady comapred by bastmlib */
	if (astm->settings.stcSource == NEXUS_AstmStcSource_eAdaptive || 
		astm->settings.presentationRateControl == NEXUS_AstmPresentationRateControl_eAdaptive)
	{
		ulPresentationChangeCnt++;
		BDBG_WRN(("ulPresentationChangeCnt=%d", ulPresentationChangeCnt));
	}
#endif

    goto end;

err:

end:

    return rc;
}

static BERR_Code NEXUS_Astmlib_P_ClockCouplingStateChange(void * pvParm1, int iParm2)
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_AstmHandle astm = (NEXUS_AstmHandle)pvParm1;
    NEXUS_AstmClockCoupling clockCoupling;

    BSTD_UNUSED(iParm2);
    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    BASTMlib_GetStatus(astm->lib, &astm->status);

    switch (astm->status.eClockCoupling)
    {
        case BASTMlib_ClockCoupling_eInternalClock:
            clockCoupling = NEXUS_AstmClockCoupling_eInternalClock;
            break;
        case BASTMlib_ClockCoupling_eInputClock:
        default:
            clockCoupling = NEXUS_AstmClockCoupling_eInputClock;
            break;
    }

    if (astm->settings.clockCoupling == NEXUS_AstmClockCoupling_eAdaptive)
    {
        nrc = NEXUS_Astm_P_ApplyClockCoupling(astm, clockCoupling);
        if (nrc) goto err;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_ASTM_ASYNC_TIMER)
		/* Diff between new & previous status is aleady comapred by bastmlib */
		ulClockCouplingChangeCnt++;
		BDBG_WRN(("ulClockCouplingChangeCnt=%d", ulClockCouplingChangeCnt));
#endif

    }

    goto end;

err:

end:

    return rc;
}

static void NEXUS_Astm_P_GetAudioSettings(void * decoder, struct NEXUS_AstmDecoderSettings * settings)
{
    NEXUS_AudioDecoderAstmSettings audioSettings;

    if (decoder)
    {
        if (settings)
        {
            NEXUS_AudioDecoder_GetAstmSettings_priv((NEXUS_AudioDecoderHandle)decoder, &audioSettings);
            settings->ptsOffset = audioSettings.ptsOffset;
            settings->tsm = audioSettings.enableTsm;
        }
        else
        {
            BDBG_WRN(("GetAudioSettings NULL settings"));
        }
    }
    else
    {
        BDBG_WRN(("GetAudioSettings NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_SetAudioSettings(void * decoder, const struct NEXUS_AstmDecoderSettings * settings)
{
    NEXUS_AudioDecoderAstmSettings audioSettings;

    if (decoder)
    {
        if (settings)
        {
            /* SW7335-1330: Coverity: 35222, UNINIT */
            /* 20111207 bandrews - can't init with memset, need to preserve callbacks and such from previous settings */
            NEXUS_AudioDecoder_GetAstmSettings_priv((NEXUS_AudioDecoderHandle)decoder, &audioSettings);
            audioSettings.ptsOffset = settings->ptsOffset;
            audioSettings.enableTsm = settings->tsm;
            if (NEXUS_AudioDecoder_SetAstmSettings_priv((NEXUS_AudioDecoderHandle)decoder, &audioSettings))
            {
                BDBG_WRN(("Error in NEXUS_AudioDecoder_SetAstmSettings_priv"));
                return;
            }
        }
        else
        {
            BDBG_WRN(("SetAudioSettings NULL settings"));
        }
    }
    else
    {
        BDBG_WRN(("SetAudioSettings NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_GetAudioStatus_isr(void * decoder, struct NEXUS_AstmDecoderStatus * status)
{
    NEXUS_AudioDecoderAstmStatus audioStatus;

    if (decoder)
    {
        NEXUS_AudioDecoder_GetAstmStatus_isr((NEXUS_AudioDecoderHandle)decoder, &audioStatus);
        if (status)
        {
            status->started = audioStatus.started;
            status->pts = audioStatus.pts;
            status->ptsStcDiff = -audioStatus.ptsStcDiff; /* stc - pts is reported */
            status->tsmLog.address = audioStatus.tsmLog.address;
            status->tsmLog.size = audioStatus.tsmLog.size;
        }
    }
    else
    {
        BDBG_WRN(("GetAudioStatus_isr NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_GetAudioStatus(void * decoder, struct NEXUS_AstmDecoderStatus * status)
{
    NEXUS_AudioDecoderStatus audioStatus;

    if (decoder)
    {
        BKNI_EnterCriticalSection();
        NEXUS_Astm_P_GetAudioStatus_isr(decoder, status);
        BKNI_LeaveCriticalSection();
        if (NEXUS_AudioDecoder_GetStatus((NEXUS_AudioDecoderHandle)decoder, &audioStatus))
        {
            BDBG_WRN(("Error in NEXUS_AudioDecoder_GetStatus "));

        }

        if (status && audioStatus.framesDecoded)
        {
            status->decodedCount = audioStatus.framesDecoded;
        }
    }
    else
    {
        BDBG_WRN(("GetAudioStatus NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_GetVideoSettings(void * decoder, struct NEXUS_AstmDecoderSettings * settings)
{
    NEXUS_VideoDecoderAstmSettings videoSettings;

    if (decoder)
    {
        if (settings)
        {
            NEXUS_VideoDecoder_GetAstmSettings_priv((NEXUS_VideoDecoderHandle)decoder, &videoSettings);
            settings->ptsOffset = videoSettings.ptsOffset;
            settings->tsm = videoSettings.enableTsm;
        }
        else
        {
            BDBG_WRN(("GetVideoSettings NULL settings"));
        }
    }
    else
    {
        BDBG_WRN(("GetVideoSettings NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_SetVideoSettings(void * decoder, const struct NEXUS_AstmDecoderSettings * settings)
{
    NEXUS_VideoDecoderAstmSettings videoSettings;

    if (decoder)
    {
        if (settings)
        {
            NEXUS_VideoDecoder_GetAstmSettings_priv((NEXUS_VideoDecoderHandle)decoder, &videoSettings);
            videoSettings.ptsOffset = settings->ptsOffset;
            videoSettings.enableTsm = settings->tsm;
            if (NEXUS_VideoDecoder_SetAstmSettings_priv((NEXUS_VideoDecoderHandle)decoder, &videoSettings))
            {
                BDBG_WRN(("Error in NEXUS_VideoDecoder_SetAstmSettings_priv "));
                return;
            }
        }
        else
        {
            BDBG_WRN(("SetVideoSettings NULL settings"));
        }
    }
    else
    {
        BDBG_WRN(("SetVideoSettings NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_GetVideoStatus_isr(void * decoder, struct NEXUS_AstmDecoderStatus * status)
{
    NEXUS_VideoDecoderAstmStatus videoStatus;

    if (decoder)
    {
        NEXUS_VideoDecoder_GetAstmStatus_isr((NEXUS_VideoDecoderHandle)decoder, &videoStatus);
        if (status)
        {
            status->started = videoStatus.started;
            status->pts = videoStatus.pts;
            status->tsmLog.address = videoStatus.tsmLog.address;
            status->tsmLog.size = videoStatus.tsmLog.size;
        }
    }
    else
    {
        BDBG_WRN(("GetVideoStatus_isr NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_GetVideoStatus(void * decoder, struct NEXUS_AstmDecoderStatus * status)
{
    NEXUS_VideoDecoderStatus videoStatus;

    if (decoder)
    {
        BKNI_EnterCriticalSection();
        NEXUS_Astm_P_GetVideoStatus_isr(decoder, status);
        BKNI_LeaveCriticalSection();
        if (NEXUS_VideoDecoder_GetStatus((NEXUS_VideoDecoderHandle)decoder, &videoStatus))
        {
            BDBG_WRN(("Error in NEXUS_VideoDecoder_GetStatus "));
        }
        
        if (status)
        {
            status->decodedCount = videoStatus.numDecoded;
            status->ptsStcDiff = videoStatus.ptsStcDifference;
        }
    }
    else
    {
        BDBG_WRN(("GetVideoStatus NULL decoder handle"));
    }
}

static void NEXUS_Astm_P_TsmCallback_isr(struct NEXUS_AstmContext * context, bool pass)
{
    struct NEXUS_AstmDecoderStatus decoderStatus;
    BASTMlib_Presenter_Event event;

    BKNI_Memset(&decoderStatus, 0, sizeof(struct NEXUS_AstmDecoderStatus));
    BKNI_Memset(&event, 0, sizeof(BASTMlib_Presenter_Event));

    if (context)
    {
        if (context->astm)
        {
            if (context->astm->ready && context->astm->started)
            {
                if (context->getStatus_isr)
                {
                    context->getStatus_isr(context->decoder, &decoderStatus);
                    event.uiPts = decoderStatus.pts;
                }
                else
                {
                    BDBG_WRN(("%s TSM callback NULL getStatus_isr function pointer", context->decoderName));
                }

                if (context->stc)
                {
                    NEXUS_StcChannel_GetStc_isr(context->stc, &event.uiStc);
                }
                else
                {
                    BDBG_WRN(("%s TSM callback NULL STC channel handle", context->decoderName));
                }

                event.bPass = pass;

                BDBG_MSG_TRACE(("%s TSM %s callback pts=%#x stc=%#x", context->decoderName, pass ? "PASS" : "FAIL", event.uiPts, event.uiStc));

                if (context->presenter)
                {
                    BASTMlib_Presenter_EventHandler_isr(context->presenter, &event);
                }
                else
                {
                    BDBG_WRN(("%s TSM callback NULL presenter handle", context->decoderName));
                }
            }
            else
            {
                BDBG_MSG_TRACE(("%s TSM callback (ASTM not ready)", context->decoderName));
            }
        }
        else
        {
            BDBG_WRN(("%s TSM callback NULL astm handle", context->decoderName));
        }
    }
    else
    {
        BDBG_WRN(("TSM callback NULL context"));
    }
}

static void NEXUS_Astm_P_DecoderFirstPtsEventHandler(void * context)
{
    NEXUS_AstmHandle astm;
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;
    NEXUS_StcChannelSettings stcChannelSettings;
    struct NEXUS_AstmDecoderSettings settings;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    if (astmContext)
    {
#else
    if (!astmContext)
    {
        BDBG_MSG(("%d: decoder first pts event handler NULL context", __LINE__));
        goto end;
    }
#endif
    /* this event only happens when the first pts/pcr diff is outside the allowable range */
    astm = astmContext->astm;
    
    if (!astm)
    {
        goto spurious;
    }

    if (!astm->settings.stcChannel)
    {
        goto spurious;
    }
    
    NEXUS_StcChannel_GetSettings(astm->settings.stcChannel, &stcChannelSettings);

    if (stcChannelSettings.mode == NEXUS_StcChannelMode_ePcr)
    {
        if (astm->usePtsMasterMode)
        {
            NEXUS_StcChannelTsmMode tsmMode;
            BDBG_MSG(("First PTS event occurred in live mode, switching to PTS master mode"));
            NEXUS_Astm_P_ApplyStcSource(astm, NEXUS_AstmStcSource_ePts, 
                astm->settings.stcMaster, astm->settings.syncLimit);
            tsmMode = NEXUS_Astm_P_ComputeStcChannelTsmMode(astm, 
                NEXUS_AstmStcSource_ePts, astm->settings.stcMaster, 
                NEXUS_AstmPresentationRateControl_eTimeStamp);
            NEXUS_Astm_P_ApplyStcChannelTsmMode(astm, tsmMode);
        }
        else
        {
            /* TODO: should manageRateControl apply to this? */
            BDBG_MSG(("First PTS event occurred in live mode, disabling TSM on offending decoder"));
            if (astmContext->getSettings && astmContext->setSettings)
            {
                astmContext->firstPtsTsm = false; /* save for later */
                NEXUS_Module_Lock(astmContext->module);
                astmContext->getSettings(astmContext->decoder, &settings);
                settings.tsm = false;
                settings.ptsOffset = 0;
                astmContext->setSettings(astmContext->decoder, &settings);
                NEXUS_Module_Unlock(astmContext->module);
            }
        }
    }
    else
    {
        BDBG_MSG(("First PTS event occurred in playback mode, ignored"));
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    }
    else
    {
        BDBG_WRN(("%d: decoder first pts event handler NULL context", __LINE__));
    }
#endif
end:
    return;

spurious:
    BDBG_MSG(("%p spurious decoder first PTS event", (void*)astmContext));
    goto end;
}

static bool NEXUS_Astm_P_TimestampDiffCheck_isr(
    uint32_t firstTimestamp, 
    uint32_t secondTimestamp, 
    uint32_t positiveThreshold,
    uint32_t negativeThreshold)
{
    bool fail = false;
    
    BDBG_MSG(("first timestamp: %#x; second timestamp: %#x; allowable diff range: (-%u, %u)", 
        firstTimestamp, secondTimestamp, negativeThreshold, positiveThreshold));

    if (firstTimestamp > secondTimestamp)
    {
        if (firstTimestamp - secondTimestamp > positiveThreshold)
        {
            fail = true;
        }
    }
    else
    {
        if (secondTimestamp - firstTimestamp > negativeThreshold)
        {
            fail = true;
        }
    }

    return fail;
}

static bool NEXUS_Astm_P_CheckFirstPtsReceived_isr(NEXUS_AstmHandle astm)
{
    unsigned i;
    bool allFirstPtsReceived = true;

    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
        allFirstPtsReceived = allFirstPtsReceived 
            && (astmContext->firstPtsReceived || !astmContext->maxAllowableFirstPtsStcDiff);
    }

    return allFirstPtsReceived;
}

static void NEXUS_Astm_P_PropagatePcr_isr(NEXUS_AstmHandle astm)
{
    unsigned i;

    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
        if (!astmContext->firstPtsArrivalPcrValid && astmContext->firstPtsReceived)
        {
            astmContext->firstPtsArrivalPcr = astm->lastPcr;
            astmContext->firstPtsArrivalPcrValid = true;
        }
    }
}

#define MAX_PCR_SPACING_45K 100 * 45 /* 100 ms per MPEG spec */

static bool NEXUS_Astm_P_CheckCompatibleTimestamps_isr(NEXUS_AstmHandle astm)
{
    unsigned i;
    bool compatiblePts = true;
    
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
        if (astmContext->maxAllowableFirstPtsStcDiff)
        {
            compatiblePts = compatiblePts 
                && !NEXUS_Astm_P_TimestampDiffCheck_isr(
                        astmContext->firstPts, astmContext->firstPtsArrivalPcr, astmContext->maxAllowableFirstPtsStcDiff, 2 * MAX_PCR_SPACING_45K); /* safety factor of 2 used in case callbacks are delayed */
        }
    }

    return compatiblePts;
}

static bool NEXUS_Astm_P_CheckMasterSuitability_isr(NEXUS_AstmHandle astm, uint32_t master)
{
    unsigned i;
    bool compatiblePts = true;
    
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
        if (astmContext->maxAllowableFirstPtsStcDiff)
        {
            compatiblePts = compatiblePts 
                && !NEXUS_Astm_P_TimestampDiffCheck_isr(
                        astmContext->firstPts, master, astmContext->maxAllowableFirstPtsStcDiff, 0);
        }
    }

    return compatiblePts;
}

static uint32_t NEXUS_Astm_P_FindMasterTimestamp_isr(NEXUS_AstmHandle astm)
{
    unsigned i;
    uint32_t master = 0;

    if (astm->settings.stcMaster)
    {
        /* just take the one who is the master */
        for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
        {
            struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
            if (astmContext->decoder == astm->settings.stcMaster)
            {
                master = astmContext->firstPts;
                break;
            }
        }
    }
    else
    {
        /* get the min */
        /* TODO: does not handle wraparound */
        master = 0xFFFFFFFF;
        for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
        {
            struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
            if (master < astmContext->firstPts)
            {
                master = astmContext->firstPts;
            }
        }
    }

    return master;
}

static void NEXUS_Astm_P_SetFirstPtsEvents_isr(NEXUS_AstmHandle astm)
{
    unsigned i;
    /* we set the event for all decoders because some of them may need to vsync themselves
    if we didn't want to do pts master mode, and setting pts master mode N times won't hurt */
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        struct NEXUS_AstmContext * astmContext = &astm->presenterContexts[i];
        BKNI_SetEvent(astmContext->firstPtsEvent);
    }
}

static void NEXUS_Astm_P_HandleTimestampCompatibility_isr(NEXUS_AstmHandle astm)
{
    /* check pcr and all contexts for timestamp receipt */
    if (astm->pcrReceived && NEXUS_Astm_P_CheckFirstPtsReceived_isr(astm))
    {
        BDBG_MSG(("PCR and all first PTSs received; checking compatibility"));
        
        /* if any decoder pts is incompatible with its arrival PCR */
        if (!NEXUS_Astm_P_CheckCompatibleTimestamps_isr(astm))
        {
            uint32_t master = NEXUS_Astm_P_FindMasterTimestamp_isr(astm);
            BDBG_WRN(("PCR/PTS incompatibility detected"));
            astm->usePtsMasterMode = NEXUS_Astm_P_CheckMasterSuitability_isr(astm, master);
            NEXUS_Astm_P_SetFirstPtsEvents_isr(astm);
        }
    }
}

static void NEXUS_Astm_P_DecoderFirstPtsCallback_isr(void * context, int param)
{
    NEXUS_AstmHandle astm;
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;
    struct NEXUS_AstmDecoderStatus status;

    BSTD_UNUSED(param);

    if (astmContext)
    {
        astm = astmContext->astm;
        BDBG_MSG(("%d: %s decoder first PTS callback", __LINE__, astmContext->decoderName));

        if (astmContext->getStatus_isr)
        {
            /* save data for this context */
            astmContext->getStatus_isr(astmContext->decoder, &status);
            astmContext->firstPts = status.pts;
            astmContext->firstPtsReceived = true;
            if (astm->pcrReceived) 
            {
                /* if we already saw a PCR, copy it for this decoder */
                astmContext->firstPtsArrivalPcr = astm->lastPcr; /* this will copy the most recent PCR */
                astmContext->firstPtsArrivalPcrValid = true;
            }
            
            NEXUS_Astm_P_HandleTimestampCompatibility_isr(astm);
        }
        else
        {
            BDBG_WRN(("%d: decoder first PTS callback NULL getStatus_isr", __LINE__));
        }
    }
    else
    {
        BDBG_WRN(("%d: decoder first PTS callback NULL context", __LINE__));
    }
}

static void NEXUS_Astm_P_DecoderTsmPassEventHandler(void * context)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    if (astmContext)
    {
#else
    if (!astmContext)
    {
        BDBG_MSG(("%d: decoder TSM pass event handler NULL context", __LINE__));
        goto end;
    }

    if (!astmContext->astm)
    {
        goto spurious;
    }
#endif

    /* if acq timer exists (it should) cancel it */
    if (astmContext->tsmRecoveryAcquisitionTimer)
    {
        NEXUS_CancelTimer(astmContext->tsmRecoveryAcquisitionTimer);
        astmContext->tsmRecoveryAcquisitionTimer = NULL;
    }

    if (!astmContext->tsmRecoveryTrackingTimer)
    {
        struct NEXUS_AstmDecoderStatus status;
        
        if (astmContext->getStatus)
        {
            astmContext->getStatus(astmContext->decoder, &status);
            astmContext->lastDecodedCount = status.decodedCount;
        }

        /* schedule track timer */      
        astmContext->tsmRecoveryTrackingTimer = 
            NEXUS_ScheduleTimer(astmContext->tsmRecoveryTrackingTimeout, 
                &NEXUS_Astm_P_TsmRecoveryTrackingTimeoutHandler, astmContext);
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    }
    else
    {
        BDBG_WRN(("%d: decoder TSM pass event handler NULL context", __LINE__));
    }
#endif
end:
    return;

spurious:
    BDBG_MSG(("%p spurious decoder TSM pass event", (void*)astmContext));
    goto end;
}

static void NEXUS_Astm_P_DecoderTsmPassCallback_isr(void * context, int param)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    BSTD_UNUSED(param);

#if 0 /* don't do normal TSM pass anymore */
    NEXUS_Astm_P_TsmCallback_isr(astmContext, true);
#else
    if (astmContext)
    {
        BDBG_MSG(("%d: %s decoder TSM pass callback", __LINE__, astmContext->decoderName));
        /* count the pass event */
        astmContext->passEventCount++;

        BKNI_SetEvent(astmContext->tsmPassEvent);
    }
    else
    {
        BDBG_WRN(("%d: decoder TSM pass callback NULL context", __LINE__));
    }
#endif
}

static void NEXUS_Astm_P_DecoderTsmFailCallback_isr(void * context, int param)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    BSTD_UNUSED(param);

    NEXUS_Astm_P_TsmCallback_isr(astmContext, false);
}

static void NEXUS_Astm_P_DecoderTsmLogCallback_isr(void * context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    /* TODO */
}

static void NEXUS_Astm_P_DecoderWatchdogCallback_isr(void * context, int param)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    BSTD_UNUSED(param);

    if (astmContext)
    {
        BDBG_ERR(("%s watchdog callback", astmContext->decoderName));

        BKNI_SetEvent(astmContext->watchdogEvent);
    }
}

static void NEXUS_Astm_P_DecoderWatchdogEventHandler(void * context)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    if (astmContext)
    {
        if (!astmContext->astm)
        {
            goto spurious;
        }

    /* if started, restart */
    if (astmContext->astm->started)
    {
            BDBG_ERR(("%p: Auto-restarting", (void *)astmContext->astm));
        NEXUS_Astm_P_Stop(astmContext->astm);
        NEXUS_Astm_P_Start(astmContext->astm);
    }
    }
    else
    {
        BDBG_MSG(("%d: decoder watchdog event handler NULL context", __LINE__));
    }

end:
    return;

spurious:
    BDBG_MSG(("%p spurious decoder watchdog event", (void*)astmContext));
    goto end;
}

static void NEXUS_Astm_P_DecoderLifecycleCallback_isr(void * context, int param)
{
    struct NEXUS_AstmDecoderStatus decoderStatus;
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    BSTD_UNUSED(param);

    BKNI_Memset(&decoderStatus, 0, sizeof(struct NEXUS_AstmDecoderStatus));

    if (astmContext)
    {
        if (astmContext->getStatus_isr)
        {
            astmContext->getStatus_isr(astmContext->decoder, &decoderStatus);
            astmContext->started = decoderStatus.started;
            astmContext->firstPtsReceived = false;
            astmContext->firstPtsArrivalPcrValid = false;
            astmContext->firstPtsTsm = true;

            BDBG_MSG(("%s lifecycle callback: %s", astmContext->decoderName, astmContext->started ? "started" : "stopped"));

            BKNI_SetEvent(astmContext->lifecycleEvent);
        }
        else
        {
            BDBG_WRN(("%s lifecycle callback NULL getStatus_isr function pointer", astmContext->decoderName));
        }
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
static void NEXUS_Astm_P_SetConfigFromStcChannelMode(NEXUS_AstmHandle astm)
{
    BASTMlib_Config astmlibConfig;
    NEXUS_StcChannelSettings stcChannelSettings;

    NEXUS_StcChannel_GetSettings(astm->settings.stcChannel, &stcChannelSettings);

    if (stcChannelSettings.mode == NEXUS_StcChannelMode_ePcr)
    {
        /* assumes pcr's should be available from xpt hw */
        BDBG_MSG(("%p: STC channel in PCR mode, assuming PCRs available", astm));
        BASTMlib_GetConfig(astm->lib, &astmlibConfig);
        astmlibConfig.sPresentation.ePreferredStcSource = BASTMlib_StcSource_eClockReference;
        astmlibConfig.sClockCoupling.ePreferredClockCoupling = BASTMlib_ClockCoupling_eInputClock;
        BASTMlib_SetConfig(astm->lib, &astmlibConfig);
    }
    else
    {
        /* assumes pcr's should not be available from xpt hw */
        BDBG_MSG(("%p: STC channel in auto/host mode, assuming PCRs unavailable", astm));
        BASTMlib_GetConfig(astm->lib, &astmlibConfig);
        astmlibConfig.sPresentation.ePreferredStcSource = BASTMlib_StcSource_ePresenter;
        astmlibConfig.sClockCoupling.ePreferredClockCoupling = BASTMlib_ClockCoupling_eInternalClock;
        BASTMlib_SetConfig(astm->lib, &astmlibConfig);
    }
    /* TODO: need to reapply current ASTM settings to decoder on restart */

#if NEXUS_ASTM_P_PRINT_SETTINGS
    NEXUS_Astm_P_PrintDependencyAstmSettings(astmContext->astm);
#endif
}
#else
static void NEXUS_Astm_P_SetConfigFromStcChannelMode(NEXUS_AstmHandle astm)
{
}
#endif


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
static void NEXUS_Astm_P_DecoderLifecycleEventHandler(void * context)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    if (astmContext)
    {
        NEXUS_OBJECT_ASSERT(NEXUS_Astm, astmContext->astm);

        /* if a decoder is being started, stc channel state should be valid
        for this session */
        /* PR:49215 playback support */
        if (astmContext->started)
        {
            NEXUS_Astm_P_SetConfigFromStcChannelMode(astmContext->astm);
        }

#ifdef NEXUS_ASTM_ALC_SUPPORT
        /* PR50938 add ability to disable ALC */
        if (astmContext->astm->settings.enableAutomaticLifecycleControl)
        {
            if (astmContext->started)
            {
                /* if one presenter is started, start ASTM */
                if (!astmContext->astm->started)
                {
                    BDBG_MSG(("%p: Auto-starting", astmContext->astm));
                    NEXUS_Astm_P_Start(astmContext->astm);
                }
            }
            else /* this presenter was just stopped, check others */
            {
                unsigned int i = 0;
                for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
                {
                    if (astmContext->astm->presenterContexts[i].started)
                    {
                        break;
                    }
                }

                /* all presenters are stopped */
                if (i == NEXUS_ASTM_PRESENTERS)
                {
                    BDBG_MSG(("%p: Auto-stopping", astmContext->astm));
                    NEXUS_Astm_P_Stop(astmContext->astm);
                }
            }
        }
#endif
    }
    else
    {
        BDBG_WRN(("%d: decoder lifecycle event handler NULL context", __LINE__));
    }
}
#else
static void NEXUS_Astm_P_DecoderLifecycleEventHandler(void * context)
{
    struct NEXUS_AstmContext * astmContext = (struct NEXUS_AstmContext * )context;

    if (astmContext)
    {
        if (astmContext->astm)
        {
            if (!astmContext->astm->settings.stcChannel)
            {
                goto spurious;
            }

#ifdef NEXUS_ASTM_ALC_SUPPORT
            /* PR50938 add ability to disable ALC */
            if (astmContext->astm->settings.enableAutomaticLifecycleControl)
            {
                if (astmContext->started)
                {
                    /* if one presenter is started, start ASTM */
                    if (!astmContext->astm->started)
                    {
                        BDBG_MSG(("Auto-starting"));
                        NEXUS_Astm_P_Start(astmContext->astm);
                    }
                }
                else /* this presenter was just stopped, check others */
                {
                    unsigned int i = 0;
                    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
                    {
                        if (astmContext->astm->presenterContexts[i].started)
                        {
                            break;
                        }
                    }

                    /* all presenters are stopped */
                    if (i == NEXUS_ASTM_PRESENTERS)
                    {
                        BDBG_MSG(("Auto-stopping"));
                        NEXUS_Astm_P_Stop(astmContext->astm);
                    }
                }
            }
#endif

            /* if a decoder is being started, stc channel state should be valid
            for this session */
            /* PR:49215 playback support */
            if (astmContext->started)
            {
                BASTMlib_Config astmlibConfig;
                NEXUS_StcChannelSettings stcChannelSettings;
                
                NEXUS_StcChannel_GetSettings(astmContext->astm->settings.stcChannel, &stcChannelSettings);

                if (stcChannelSettings.mode == NEXUS_StcChannelMode_ePcr)
                {
                    /* assumes pcr's should be available from xpt hw */
                    BDBG_MSG(("Decoder started with stc channel in PCR mode, assuming PCRs available"));
                    BASTMlib_GetConfig(astmContext->astm->lib, &astmlibConfig);
                    astmlibConfig.sPresentation.ePreferredStcSource = BASTMlib_StcSource_eClockReference;
                    astmlibConfig.sClockCoupling.ePreferredClockCoupling = BASTMlib_ClockCoupling_eInputClock;
                    BASTMlib_SetConfig(astmContext->astm->lib, &astmlibConfig);
                }
                else
                {
                    /* assumes pcr's should not be available from xpt hw */
                    BDBG_MSG(("Decoder started with stc channel in auto/host mode, assuming PCRs unavailable"));
                    BASTMlib_GetConfig(astmContext->astm->lib, &astmlibConfig);
                    astmlibConfig.sPresentation.ePreferredStcSource = BASTMlib_StcSource_ePresenter;
                    astmlibConfig.sClockCoupling.ePreferredClockCoupling = BASTMlib_ClockCoupling_eInternalClock;
                    BASTMlib_SetConfig(astmContext->astm->lib, &astmlibConfig);
                }
                /* TODO: need to reapply current ASTM settings to decoder on restart */

#if NEXUS_ASTM_P_PRINT_SETTINGS
                NEXUS_Astm_P_PrintDependencyAstmSettings(astmContext->astm);
#endif
            }
        }
    }

end:
    return;

spurious:
    BDBG_MSG(("%p spurious decoder lifecycle event", (void*)astmContext));
    goto end;
}
#endif

static void NEXUS_Astm_P_TimebasePcrReceivedCallback_isr(void * context, int param)
{
    NEXUS_AstmHandle astm = (NEXUS_AstmHandle)context;
    NEXUS_TimebaseStatus status;
    BASTMlib_ClockReference_Event sEvent;

    BSTD_UNUSED(param);
    BKNI_Memset(&sEvent, 0, sizeof(BASTMlib_ClockReference_Event));

    if (astm && astm->ready && astm->started)
    {
        NEXUS_Timebase_GetStatus_priv_isr(astm->timebase, &status);
        sEvent.uiClockReference = status.lastValue;

        if (astm->status.eClockCoupling == BASTMlib_ClockCoupling_eInputClock)
        {
            /* if input clock, then error is between stc and pcr */
            sEvent.uiStc = status.lastValue - status.lastError; /* error saturates
                at 10 bits (instead of 32), but for our purposes this is okay */
        }
        else
        {
            /* if internal, then error is between stc and xtal, so need to just 
            read STC.  This will be sensitive to high latency environments, but
            the worst case in high latency situation is that we measure bad
            PCR/STC jitter and stay in the state we are already in */
            NEXUS_StcChannel_GetStc_isr(astm->settings.stcChannel, &sEvent.uiStc);
        }

        BDBG_MSG_TRACE(("TimebasePcrReceived pcr=%#x stc=%#x", sEvent.uiClockReference, sEvent.uiStc));

        BASTMlib_ClockReferenceEventHandler_isr(astm->lib, &sEvent);

        astm->lastPcr = status.lastValue;

        if (!astm->pcrReceived) /* just check the first PCR */
        {
            astm->pcrReceived = true;
            
            /* propagate the first PCR to any decoder contexts that had FirstPTS before the PCR arrived */
            NEXUS_Astm_P_PropagatePcr_isr(astm);
        
            NEXUS_Astm_P_HandleTimestampCompatibility_isr(astm);
        }
    }
    else
    {
        BDBG_MSG_TRACE(("TimebasePcrReceived (ASTM not ready)"));
    }
}

static void NEXUS_Astm_P_TsmRecoveryTrackingTimeoutHandler(void * context)
{
    struct NEXUS_AstmContext * astmContext = context;
    struct NEXUS_AstmDecoderStatus status;

    if (astmContext)
    {
        if (!astmContext->astm)
        {
            goto spurious;
        }

        astmContext->tsmRecoveryTrackingTimer = NULL;
        
        if (astmContext->getStatus)
        {
            astmContext->getStatus(astmContext->decoder, &status);

            /* TODO: may need a threshold here */
            BDBG_MSG(("%d: tsm pass count (%u); decoded count (%u)", __LINE__, astmContext->passEventCount, status.decodedCount - astmContext->lastDecodedCount));
            if (status.decodedCount && (status.decodedCount <= astmContext->passEventCount + astmContext->lastDecodedCount))
            {
                BKNI_EnterCriticalSection();
                NEXUS_Astm_P_TsmCallback_isr(astmContext, true);
                BKNI_LeaveCriticalSection();
            }
            else
            {
                BDBG_MSG(("%d: reverting to acquisition stage", __LINE__));

                /* revert to acquisition */
                astmContext->tsmRecoveryAcquisitionTimer = 
                    NEXUS_ScheduleTimer(astmContext->tsmRecoveryAcquisitionPeriod, 
                        &NEXUS_Astm_P_TsmRecoveryAcquisitionTask, astmContext);
            }

            astmContext->lastDecodedCount = status.decodedCount;
            astmContext->passEventCount = 0;
        }
        else
        {
            BDBG_MSG(("%d: decoder status accessor was NULL", __LINE__));
        }
    }
    else
    {
        BDBG_MSG(("%d: tsm recovery tracking timeout handler NULL context", __LINE__));
    }

end:
    return;

spurious:
    BDBG_MSG(("%p spurious TSM recovery tracking timeout", (void*)astmContext));
    goto end;
}

static void NEXUS_Astm_P_TsmRecoveryAcquisitionTask(void * context)
{
    struct NEXUS_AstmContext * astmContext = context;
    struct NEXUS_AstmDecoderSettings settings;
    struct NEXUS_AstmDecoderStatus status;

    if (astmContext)
    {
        if (!astmContext->astm)
        {
            goto spurious;
        }

        astmContext->tsmRecoveryAcquisitionTimer = NULL;

        /* if we've disabled TSM for first PTS reasons, don't bother coming back */
        if (astmContext->firstPtsTsm) 
        {
            if (astmContext->getStatus)
            {
                astmContext->getStatus(astmContext->decoder, &status);
                astmContext->lastDecodedCount = status.decodedCount;

                if (astmContext->getSettings && astmContext->setSettings)
                {
                    BDBG_MSG(("%d: %s pts/stc diff = %ld", __LINE__, astmContext->decoderName, status.ptsStcDiff));

                    if (status.ptsStcDiff > (int32_t)astmContext->ptsStcDiffAdjustmentThreshold
                        || status.ptsStcDiff < -(int32_t)astmContext->ptsStcDiffAdjustmentThreshold)
                    {
                        NEXUS_Module_Lock(astmContext->module);
                        astmContext->getSettings(astmContext->decoder, &settings);
                        settings.ptsOffset += -status.ptsStcDiff; 
                        astmContext->setSettings(astmContext->decoder, &settings);
                        NEXUS_Module_Unlock(astmContext->module);
                        BDBG_MSG(("%d: %s ptsOffset = %ld", __LINE__, astmContext->decoderName, settings.ptsOffset));
                    }
                    else
                    {
                        BDBG_MSG(("%d: %s pts/stc diff within tolerance", __LINE__, astmContext->decoderName));
                    }
                }
                else
                {
                    BDBG_MSG(("%d: %s decoder settings mutator was NULL", __LINE__, astmContext->decoderName));
                }
            }
            else
            {
                BDBG_MSG(("%d: %s decoder status accessor was NULL", __LINE__, astmContext->decoderName));
            }

            /* schedule the next recovery task acq instant */
            astmContext->tsmRecoveryAcquisitionTimer = 
                NEXUS_ScheduleTimer(astmContext->tsmRecoveryAcquisitionPeriod, 
                    &NEXUS_Astm_P_TsmRecoveryAcquisitionTask, astmContext);
        }
    }
    else
    {
        BDBG_MSG(("%d: tsm recovery acquisition timeout handler NULL context", __LINE__));
    }

end:
    return;

spurious:
    BDBG_MSG(("%p spurious TSM recovery acquisition timeout", (void*)astmContext));
    goto end;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
static void NEXUS_Astm_P_PcrDataPresenceDetector(void * context, bool * dataPresent)
{
    NEXUS_AstmHandle astm = context;

    NEXUS_OBJECT_ASSERT(NEXUS_Astm, astm);

    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    *dataPresent = NEXUS_Timebase_IsDataPresent_priv(astm->timebase);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);
}
#else
static void NEXUS_Astm_P_PcrDataPresenceDetector(void * context, bool * dataPresent)
{

}
#endif
/**********************
*
* Main functions
*
***********************/

NEXUS_AstmHandle NEXUS_Astm_Create(const NEXUS_AstmSettings *pSettings)
{
    BASTMlib_Settings settings;
    NEXUS_AstmHandle astm;
    BERR_Code rc;
    unsigned int i = 0;
    BASTMlib_Presenter_Settings presenterSettings;

    astm = BKNI_Malloc(sizeof(*astm));
    if (!astm) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Astm, astm);
    NEXUS_Astm_GetDefaultSettings(&astm->settings);
    astm->timebase = NULL;

    BKNI_EnterCriticalSection();
    astm->ready = false;
    BKNI_LeaveCriticalSection();

    BASTMlib_GetDefaultSettings(&settings);

    /* map syslib/framework functions to base */
    settings.cbTimer.pfCreate = NEXUS_SYSlib_P_CreateTimer;
    settings.cbTimer.pfDestroy = NEXUS_SYSlib_P_DestroyTimer;
    settings.cbTimer.pfStart_isr = NEXUS_SYSlib_P_StartTimer_isr;
    settings.cbTimer.pfCancel_isr = NEXUS_SYSlib_P_CancelTimer_isr;
    settings.cbTimer.pvParm1 = g_astm.syslibContext;
    settings.cbTimer.iParm2 = 0; /* unused */

    /* ASTMlib state change callbacks. They are all task time. You cannot make any change to ASTMlib during these callbacks. */
    settings.sClockCoupling.cbStateChange.pfDo = &NEXUS_Astmlib_P_ClockCouplingStateChange;
    settings.sClockCoupling.cbStateChange.pvParm1 = astm;
    settings.sClockCoupling.cbStateChange.iParm2 = 0; /* unused */
    settings.sPresentation.cbStateChange.pfDo = &NEXUS_Astmlib_P_PresentationStateChange;
    settings.sPresentation.cbStateChange.pvParm1 = astm;
    settings.sPresentation.cbStateChange.iParm2 = 0; /* unused */

    rc = BASTMlib_Create(&astm->lib, &settings);
    if (rc) {BERR_TRACE(rc); goto error;}

    BASTMlib_Presenter_GetDefaultSettings(&presenterSettings);

    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        BASTMlib_Presenter_Create(astm->lib, &astm->presenterContexts[i].presenter, &presenterSettings);
        BKNI_CreateEvent(&astm->presenterContexts[i].watchdogEvent);
        BKNI_CreateEvent(&astm->presenterContexts[i].lifecycleEvent);
        BKNI_CreateEvent(&astm->presenterContexts[i].tsmPassEvent);
        BKNI_CreateEvent(&astm->presenterContexts[i].firstPtsEvent);
    }

    BASTMlib_GetStatus(astm->lib, &astm->status);

    rc = NEXUS_Astm_SetSettings(astm, pSettings);
    if (rc) {BERR_TRACE(rc); goto error;}

    BKNI_EnterCriticalSection();
    astm->ready = true;
    BKNI_LeaveCriticalSection();

    return astm;

error:
    NEXUS_Astm_Destroy(astm);
    return NULL;
}

static void NEXUS_Astm_P_Finalizer(NEXUS_AstmHandle astm)
{
    unsigned int i;
    NEXUS_AstmSettings settings;

    NEXUS_OBJECT_ASSERT(NEXUS_Astm, astm);

    NEXUS_Astm_GetSettings(astm, &settings);
    settings.videoDecoder = NULL;
    settings.stcChannel = NULL;
    settings.stcMaster = NULL;
    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        settings.audioDecoder[i] = NULL;
    }
    NEXUS_Astm_SetSettings(astm, &settings);

    /* destroy presenters */
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        if (astm->presenterContexts[i].firstPtsEvent)
        {
            BKNI_DestroyEvent(astm->presenterContexts[i].firstPtsEvent);
        }
        if (astm->presenterContexts[i].tsmPassEvent)
        {
            BKNI_DestroyEvent(astm->presenterContexts[i].tsmPassEvent);
        }
        if (astm->presenterContexts[i].watchdogEvent)
        {
            BKNI_DestroyEvent(astm->presenterContexts[i].watchdogEvent);
        }
        if (astm->presenterContexts[i].lifecycleEvent)
        {
            BKNI_DestroyEvent(astm->presenterContexts[i].lifecycleEvent);
        }
        if (astm->presenterContexts[i].presenter)
        {
            BASTMlib_Presenter_Destroy(astm->presenterContexts[i].presenter);
        }
    }

    BASTMlib_Destroy(astm->lib);

    NEXUS_OBJECT_DESTROY(NEXUS_Astm, astm);
    BKNI_Free(astm);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Astm, NEXUS_Astm_Destroy);

void NEXUS_Astm_GetSettings(NEXUS_AstmHandle astm, NEXUS_AstmSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);
    *pSettings = astm->settings;
}

#ifdef BDBG_DEBUG_BUILD
static const char * decoderNameStrings[] =
{
    "Video",
    "Audio 0",
    "Audio 1",
    NULL
};
#endif

static void NEXUS_Astm_P_DisconnectVideoDecoder(NEXUS_AstmHandle astm)
{
    NEXUS_VideoDecoderAstmSettings astmSettings;
#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_Settings presenterSettings;
#endif
    struct NEXUS_AstmContext * presenterContext;

    presenterContext = &astm->presenterContexts[0];

    NEXUS_UnregisterEvent(presenterContext->lifecycleEventHandler);
    NEXUS_UnregisterEvent(presenterContext->watchdogEventHandler);
    NEXUS_UnregisterEvent(presenterContext->tsmPassEventHandler);
    NEXUS_UnregisterEvent(presenterContext->firstPtsEventHandler);

    NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
    NEXUS_VideoDecoder_GetAstmSettings_priv(astm->settings.videoDecoder, &astmSettings);
    astmSettings.firstPts_isr = NULL;
    astmSettings.tsmPass_isr = NULL;
    astmSettings.tsmFail_isr = NULL;
    astmSettings.tsmLog_isr  = NULL;
    astmSettings.watchdog_isr  = NULL;
    astmSettings.lifecycle_isr  = NULL;
    astmSettings.callbackContext = NULL;
    astmSettings.enableAstm = false;
    astmSettings.ptsOffset = 0;
    (void)NEXUS_VideoDecoder_SetAstmSettings_priv(astm->settings.videoDecoder, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);

    presenterContext->decoder = NULL;
#ifdef BDBG_DEBUG_BUILD
    presenterContext->decoderName = NULL;
#endif
    presenterContext->astm = NULL;
    presenterContext->getStatus_isr = NULL;
    presenterContext->getStatus = NULL;
    presenterContext->setSettings = NULL;
    presenterContext->getSettings = NULL;

    BASTMlib_RemovePresenter(astm->lib, presenterContext->presenter);

#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_GetSettings(presenterContext->presenter, &presenterSettings);
    presenterSettings.pcName = NULL;
    BASTMlib_Presenter_SetSettings(presenterContext->presenter, &presenterSettings);
#endif

    astm->settings.videoDecoder = NULL;
}

static NEXUS_Error NEXUS_Astm_P_ConnectVideoDecoder(NEXUS_AstmHandle astm, const NEXUS_AstmSettings * pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;
    NEXUS_VideoDecoderAstmSettings astmSettings;
#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_Settings presenterSettings;
#endif
    struct NEXUS_AstmContext * presenterContext;

    presenterContext = &astm->presenterContexts[0];

    presenterContext->lifecycleEventHandler = NEXUS_RegisterEvent(presenterContext->lifecycleEvent, &NEXUS_Astm_P_DecoderLifecycleEventHandler, presenterContext);
    presenterContext->watchdogEventHandler = NEXUS_RegisterEvent(presenterContext->watchdogEvent, &NEXUS_Astm_P_DecoderWatchdogEventHandler, presenterContext);
    presenterContext->tsmPassEventHandler = NEXUS_RegisterEvent(presenterContext->tsmPassEvent, &NEXUS_Astm_P_DecoderTsmPassEventHandler, presenterContext);
    presenterContext->firstPtsEventHandler = NEXUS_RegisterEvent(presenterContext->firstPtsEvent, &NEXUS_Astm_P_DecoderFirstPtsEventHandler, presenterContext);

#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_GetSettings(presenterContext->presenter, &presenterSettings);
    presenterContext->decoderName = presenterSettings.pcName = decoderNameStrings[0];
    BASTMlib_Presenter_SetSettings(presenterContext->presenter, &presenterSettings);
#endif

    mrc = BASTMlib_AddPresenter(astm->lib, presenterContext->presenter);
    if (mrc) {rc = NEXUS_UNKNOWN; goto err;}

    presenterContext->decoder = pSettings->videoDecoder;
    presenterContext->astm = astm;
    presenterContext->getStatus_isr = &NEXUS_Astm_P_GetVideoStatus_isr;
    presenterContext->getStatus = &NEXUS_Astm_P_GetVideoStatus;
    presenterContext->getSettings = &NEXUS_Astm_P_GetVideoSettings;
    presenterContext->setSettings = &NEXUS_Astm_P_SetVideoSettings;
    presenterContext->passEventCount = 0;
    presenterContext->ptsStcDiffAdjustmentThreshold = pSettings->videoPresenterConfig.ptsStcDiffAdjustmentThreshold;
    presenterContext->tsmRecoveryAcquisitionTimer = NULL;
    presenterContext->tsmRecoveryAcquisitionPeriod = pSettings->videoPresenterConfig.tsmRecoveryAcquisitionPeriod;
    presenterContext->tsmRecoveryTrackingTimer = NULL;
    presenterContext->tsmRecoveryTrackingTimeout = pSettings->videoPresenterConfig.tsmRecoveryTrackingTimeout;
    presenterContext->maxAllowableFirstPtsStcDiff = pSettings->videoPresenterConfig.maxAllowableFirstPtsStcDiff;
    presenterContext->tsmThresholdAdjustment = pSettings->videoPresenterConfig.tsmThresholdAdjustment;
    presenterContext->manageRateControl = pSettings->videoPresenterConfig.manageRateControl;
    presenterContext->firstPtsTsm = true;
    presenterContext->firstPtsReceived = false;
    presenterContext->module = g_astm.settings.modules.videoDecoder;

    NEXUS_Module_Lock(g_astm.settings.modules.videoDecoder);
    NEXUS_VideoDecoder_GetAstmSettings_priv(pSettings->videoDecoder, &astmSettings);
    astmSettings.enableAstm = false;
    astmSettings.enableTsm = true;
    astmSettings.enablePlayback = false;
    astmSettings.syncLimit = 0;
    astmSettings.ptsOffset = 0;
    astmSettings.firstPts_isr = &NEXUS_Astm_P_DecoderFirstPtsCallback_isr;
    astmSettings.tsmPass_isr = &NEXUS_Astm_P_DecoderTsmPassCallback_isr;
    astmSettings.tsmFail_isr = &NEXUS_Astm_P_DecoderTsmFailCallback_isr;
    astmSettings.tsmLog_isr = &NEXUS_Astm_P_DecoderTsmLogCallback_isr;
    astmSettings.watchdog_isr = &NEXUS_Astm_P_DecoderWatchdogCallback_isr;
    astmSettings.lifecycle_isr = &NEXUS_Astm_P_DecoderLifecycleCallback_isr;
    astmSettings.callbackContext = presenterContext;
    rc = NEXUS_VideoDecoder_SetAstmSettings_priv(pSettings->videoDecoder, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.videoDecoder);
    if (rc) goto err;

    astm->settings.videoDecoder = pSettings->videoDecoder;

err:
    return rc;
}

static NEXUS_Error NEXUS_Astm_P_DisconnectAudioDecoder(NEXUS_AstmHandle astm, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_AudioDecoderAstmSettings astmSettings;
#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_Settings presenterSettings;
#endif
    unsigned int presenterIndex = index + 1;
    struct NEXUS_AstmContext * presenterContext;

    if (index > 1)
    {
        rc = NEXUS_INVALID_PARAMETER;
        goto err;
    }

    presenterContext = &astm->presenterContexts[presenterIndex];

    NEXUS_UnregisterEvent(presenterContext->lifecycleEventHandler);
    NEXUS_UnregisterEvent(presenterContext->watchdogEventHandler);
    NEXUS_UnregisterEvent(presenterContext->tsmPassEventHandler);
    NEXUS_UnregisterEvent(presenterContext->firstPtsEventHandler);

    NEXUS_Module_Lock(g_astm.settings.modules.audio);
    NEXUS_AudioDecoder_GetAstmSettings_priv(astm->settings.audioDecoder[index], &astmSettings);
    astmSettings.firstPts_isr = NULL;
    astmSettings.tsmPass_isr = NULL;
    astmSettings.tsmFail_isr = NULL;
    astmSettings.tsmLog_isr  = NULL;
    astmSettings.watchdog_isr  = NULL;
    astmSettings.lifecycle_isr  = NULL;
    astmSettings.callbackContext = NULL;
    astmSettings.enableAstm = false;
    astmSettings.ptsOffset = 0;
    (void)NEXUS_AudioDecoder_SetAstmSettings_priv(astm->settings.audioDecoder[index], &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.audio);

    presenterContext->decoder = NULL;
    presenterContext->astm = NULL;
    presenterContext->getStatus_isr = NULL;
    presenterContext->getStatus = NULL;
    presenterContext->getSettings = NULL;
    presenterContext->setSettings = NULL;

    BASTMlib_RemovePresenter(astm->lib, presenterContext->presenter);

#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_GetSettings(presenterContext->presenter, &presenterSettings);
    presenterSettings.pcName = NULL;
    BASTMlib_Presenter_SetSettings(presenterContext->presenter, &presenterSettings);
#endif

    astm->settings.audioDecoder[index] = NULL;

err:
    return rc;
}

static NEXUS_Error NEXUS_Astm_P_ConnectAudioDecoder(NEXUS_AstmHandle astm, unsigned int index, const NEXUS_AstmSettings * pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;
    NEXUS_AudioDecoderAstmSettings astmSettings;
#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_Settings presenterSettings;
#endif
    unsigned int presenterIndex = index + 1;
    struct NEXUS_AstmContext * presenterContext;

    if (index > 1)
    {
        rc = NEXUS_INVALID_PARAMETER;
        goto err;
    }

    presenterContext = &astm->presenterContexts[presenterIndex];

    presenterContext->lifecycleEventHandler = NEXUS_RegisterEvent(presenterContext->lifecycleEvent, &NEXUS_Astm_P_DecoderLifecycleEventHandler, presenterContext);
    presenterContext->watchdogEventHandler = NEXUS_RegisterEvent(presenterContext->watchdogEvent, &NEXUS_Astm_P_DecoderWatchdogEventHandler, presenterContext);
    presenterContext->tsmPassEventHandler = NEXUS_RegisterEvent(presenterContext->tsmPassEvent, &NEXUS_Astm_P_DecoderTsmPassEventHandler, presenterContext);
    presenterContext->firstPtsEventHandler = NEXUS_RegisterEvent(presenterContext->firstPtsEvent, &NEXUS_Astm_P_DecoderFirstPtsEventHandler, presenterContext);

#ifdef BDBG_DEBUG_BUILD
    BASTMlib_Presenter_GetSettings(presenterContext->presenter, &presenterSettings);
    presenterContext->decoderName = presenterSettings.pcName = decoderNameStrings[presenterIndex];
    BASTMlib_Presenter_SetSettings(presenterContext->presenter, &presenterSettings);
#endif

    mrc = BASTMlib_AddPresenter(astm->lib, presenterContext->presenter);
    if (mrc) {rc = NEXUS_UNKNOWN; goto err;}

    presenterContext->decoder = pSettings->audioDecoder[index];
    presenterContext->astm = astm;
    presenterContext->getStatus_isr = &NEXUS_Astm_P_GetAudioStatus_isr;
    presenterContext->getStatus = &NEXUS_Astm_P_GetAudioStatus;
    presenterContext->getSettings = &NEXUS_Astm_P_GetAudioSettings;
    presenterContext->setSettings = &NEXUS_Astm_P_SetAudioSettings;
    presenterContext->passEventCount = 0;
    presenterContext->ptsStcDiffAdjustmentThreshold = pSettings->audioPresenterConfig[index].ptsStcDiffAdjustmentThreshold;
    presenterContext->tsmRecoveryAcquisitionTimer = NULL;
    presenterContext->tsmRecoveryAcquisitionPeriod = pSettings->audioPresenterConfig[index].tsmRecoveryAcquisitionPeriod;
    presenterContext->tsmRecoveryTrackingTimer = NULL;
    presenterContext->tsmRecoveryTrackingTimeout = pSettings->audioPresenterConfig[index].tsmRecoveryTrackingTimeout;
    presenterContext->maxAllowableFirstPtsStcDiff = pSettings->audioPresenterConfig[index].maxAllowableFirstPtsStcDiff;
    presenterContext->firstPtsTsm = true;
    presenterContext->firstPtsReceived = false;
    presenterContext->tsmThresholdAdjustment = pSettings->audioPresenterConfig[index].tsmThresholdAdjustment;
    presenterContext->manageRateControl = pSettings->audioPresenterConfig[index].manageRateControl;
    presenterContext->module = g_astm.settings.modules.audio;

    NEXUS_Module_Lock(g_astm.settings.modules.audio);
    NEXUS_AudioDecoder_GetAstmSettings_priv(pSettings->audioDecoder[index], &astmSettings);
    astmSettings.enableAstm = false;
    astmSettings.enableTsm = true;
    astmSettings.enablePlayback = false;
    astmSettings.syncLimit = 0;
    astmSettings.ptsOffset = 0;
    astmSettings.firstPts_isr = &NEXUS_Astm_P_DecoderFirstPtsCallback_isr;
    astmSettings.tsmPass_isr = &NEXUS_Astm_P_DecoderTsmPassCallback_isr;
    astmSettings.tsmFail_isr = &NEXUS_Astm_P_DecoderTsmFailCallback_isr;
    astmSettings.tsmLog_isr = &NEXUS_Astm_P_DecoderTsmLogCallback_isr;
    astmSettings.watchdog_isr = &NEXUS_Astm_P_DecoderWatchdogCallback_isr;
    astmSettings.lifecycle_isr = &NEXUS_Astm_P_DecoderLifecycleCallback_isr;
    astmSettings.callbackContext = presenterContext;
    rc = NEXUS_AudioDecoder_SetAstmSettings_priv(pSettings->audioDecoder[index], &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.audio);
    if (rc) goto err;

    astm->settings.audioDecoder[index] = pSettings->audioDecoder[index];

err:
    return rc;
}

static void NEXUS_Astm_P_DisconnectTimebase(NEXUS_AstmHandle astm)
{
    NEXUS_TimebaseAstmSettings astmSettings;

    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_Timebase_GetAstmSettings_priv(astm->timebase, &astmSettings);
    astmSettings.enabled = false;
    astmSettings.pcrReceived_isr = NULL;
    (void)NEXUS_Timebase_SetAstmSettings_priv(astm->timebase, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);

    astm->timebase = NULL;
}

static NEXUS_Error NEXUS_Astm_P_ConnectTimebase(NEXUS_AstmHandle astm, const NEXUS_AstmSettings * pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelSettings stcChannelSettings;
    NEXUS_TimebaseAstmSettings astmSettings;
    NEXUS_StcChannelAstmStatus stcChannelAstmStatus;

    NEXUS_StcChannel_GetSettings(pSettings->stcChannel, &stcChannelSettings);
    
    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_StcChannel_GetAstmStatus_priv(pSettings->stcChannel, &stcChannelAstmStatus);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);
    
    astm->timebase = stcChannelAstmStatus.timebase;

    switch (stcChannelSettings.mode)
    {
        case NEXUS_StcChannelMode_ePcr:
            if (stcChannelSettings.modeSettings.pcr.pidChannel)
            {
                NEXUS_PidChannelStatus pidChannelStatus;
                NEXUS_PidChannel_GetStatus(stcChannelSettings.modeSettings.pcr.pidChannel, &pidChannelStatus);
                astm->transportType = pidChannelStatus.transportType;
            }
            break;
        case NEXUS_StcChannelMode_eAuto:
            astm->transportType = stcChannelSettings.modeSettings.Auto.transportType;
            break;
        case NEXUS_StcChannelMode_eHost:
            astm->transportType = stcChannelSettings.modeSettings.host.transportType;
            break;
        default:
            BDBG_WRN(("ASTM transport type not set"));
            break;
    }

    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_Timebase_GetAstmSettings_priv(astm->timebase, &astmSettings);
    astmSettings.enabled = false;
    astmSettings.clockCoupling = NEXUS_TimebaseClockCoupling_eInputClock;
    astmSettings.pcrReceived_isr = NEXUS_Astm_P_TimebasePcrReceivedCallback_isr;
    astmSettings.callbackContext = astm;
    rc = NEXUS_Timebase_SetAstmSettings_priv(astm->timebase, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);
    if (rc) goto err;

err:
    return rc;
}

static void NEXUS_Astm_P_DisconnectStcChannel(NEXUS_AstmHandle astm)
{
    unsigned int i = 0;
    NEXUS_StcChannelAstmSettings astmSettings;

    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_StcChannel_GetAstmSettings_priv(astm->settings.stcChannel, &astmSettings);
    astmSettings.enabled = false;
    (void)NEXUS_StcChannel_SetAstmSettings_priv(astm->settings.stcChannel, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);

    /* 20081204 bandrews - apply STC channel to all video and audio decoder
    presenter contexts, in case they were set at a different time from the STC channel */
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        astm->presenterContexts[i].stc = NULL;
    }

    astm->settings.stcChannel = NULL;
}

static NEXUS_Error NEXUS_Astm_P_ConnectStcChannel(NEXUS_AstmHandle astm, const NEXUS_AstmSettings * pSettings)
{
    unsigned int i = 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelAstmSettings astmSettings;

    NEXUS_Module_Lock(g_astm.settings.modules.transport);
    NEXUS_StcChannel_GetAstmSettings_priv(pSettings->stcChannel, &astmSettings);
    astmSettings.enabled = false;
    astmSettings.mode = NEXUS_StcChannelMode_ePcr;
    astmSettings.tsmMode = NEXUS_StcChannelTsmMode_eStcMaster;
    astmSettings.syncLimit = 0;
    rc = NEXUS_StcChannel_SetAstmSettings_priv(pSettings->stcChannel, &astmSettings);
    NEXUS_Module_Unlock(g_astm.settings.modules.transport);
    if (rc) goto err;

    astm->settings.stcChannel = pSettings->stcChannel;

    /* 20081204 bandrews - apply STC channel to all video and audio decoder
    presenter contexts, in case they were set at a different time from the STC channel */
    for (i = 0; i < NEXUS_ASTM_PRESENTERS; i++)
    {
        astm->presenterContexts[i].stc = pSettings->stcChannel;
    }

    BDBG_MSG(("Stc channel %p connected", pSettings->stcChannel));

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    NEXUS_Astm_P_SetConfigFromStcChannelMode(astm);
#endif

err:
    return rc;
}

NEXUS_Error NEXUS_Astm_SetSettings(NEXUS_AstmHandle astm, const NEXUS_AstmSettings *pSettings)
{
    BASTMlib_Config astmlibConfig;
    BASTMlib_Presenter_Config astmPresenterConfig;
    unsigned int i;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_StcChannelTsmMode newTsmMode;
    NEXUS_AstmClockCoupling newClockCoupling;
    NEXUS_AstmStcSource newStcSource;
    NEXUS_AstmPresentationRateControl newPresentationRateControl;
    bool videoChanged = false;
    bool audioChanged = false;
    bool stcChanged = false;
    bool applyClockCoupling = false;
    bool applyStcSource = false;
    bool applyPresentationRateControl = false;
    bool applyTsmMode = false;

    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    if (!pSettings)
    {
        BDBG_ERR(("NULL settings passed to SetSettings"));
        return NEXUS_INVALID_PARAMETER;
    }

    newClockCoupling = pSettings->clockCoupling;
    newStcSource = pSettings->stcSource;
    newPresentationRateControl = pSettings->presentationRateControl;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)

#else    
    if (astm->started)
    {
        BDBG_ERR(("Cannot change astm settings while it is running.  Please stop ASTM first."));
        return NEXUS_NOT_SUPPORTED;
    }
#endif
    BKNI_EnterCriticalSection();
    astm->ready = false;
    BKNI_LeaveCriticalSection();

    if (pSettings->stcChannel != astm->settings.stcChannel)
    {
        stcChanged = true;
        
        /* disco timebase first, relies on STC channel, may change in future with mutable timebases */
        if (astm->timebase)
        {
            NEXUS_Astm_P_DisconnectTimebase(astm);
        }

        if (astm->settings.stcChannel)
        {
            NEXUS_Astm_P_DisconnectStcChannel(astm);
        }

        if (pSettings->stcChannel)
        {
            NEXUS_Astm_P_ConnectStcChannel(astm, pSettings);

            /* need STC channel to connect timebase, this may change in future with mutable timebases */
            NEXUS_Astm_P_ConnectTimebase(astm, pSettings);
        }
    }

    if (pSettings->videoDecoder != astm->settings.videoDecoder)
    {
        videoChanged = true;
        
        if (astm->settings.videoDecoder)
        {
            NEXUS_Astm_P_DisconnectVideoDecoder(astm);
        }

        if (pSettings->videoDecoder)
        {
            NEXUS_Astm_P_ConnectVideoDecoder(astm, pSettings);
        }
    }

    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        if (pSettings->audioDecoder[i] != astm->settings.audioDecoder[i])
        {
            audioChanged = true;
            
            if (astm->settings.audioDecoder[i])
            {
                NEXUS_Astm_P_DisconnectAudioDecoder(astm, i);
            }

            if (pSettings->audioDecoder[i])
            {
                NEXUS_Astm_P_ConnectAudioDecoder(astm, i, pSettings);
            }
        }
    }

    /* ASTMlib config */
    {
        BASTMlib_GetConfig(astm->lib, &astmlibConfig);      

        if (astm->transportType == NEXUS_TransportType_eDssEs
            || astm->transportType == NEXUS_TransportType_eDssPes)
        {
            astmlibConfig.eStcRate = BASTMlib_ClockRate_e27Mhz;
            astmlibConfig.sClockCoupling.eClockReferenceDomain = BASTMlib_ClockRate_e27Mhz;
            
        }
        else
        {
            astmlibConfig.eStcRate = BASTMlib_ClockRate_e45Khz;
            astmlibConfig.sClockCoupling.eClockReferenceDomain = BASTMlib_ClockRate_e45Khz;
        }

        astmlibConfig.sPresentation.hPreferredPresenter = NULL;

        if (pSettings->stcMaster == pSettings->videoDecoder)
        {
            astmlibConfig.sPresentation.hPreferredPresenter = astm->presenterContexts[0].presenter;
        }
        else
        {
            for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
            {
                if (pSettings->stcMaster == pSettings->audioDecoder[i])
                {
                    astmlibConfig.sPresentation.hPreferredPresenter = astm->presenterContexts[i + 1].presenter;
                    break;
                }
            }
        }

        astmlibConfig.bEnabled = pSettings->enabled;
        astmlibConfig.sPresentation.uiInitialAcquisitionTime = pSettings->presentationConfig.initialAcquisitionTime;
        astmlibConfig.sPresentation.uiProcessingFrequency = pSettings->presentationConfig.processingFrequency;
        astmlibConfig.sPresentation.uiTsmDisabledWatchdogTimeout = pSettings->presentationConfig.tsmDisabledWatchdogTimeout;
        astmlibConfig.sPresentation.uiSettlingTime = pSettings->presentationConfig.settlingTime;
        switch (pSettings->presentationConfig.preferredPresentationRateControl)
        {
            case NEXUS_AstmPresentationRateControl_eOutputClock:
                astmlibConfig.sPresentation.ePreferredPresentationRateControl = BASTMlib_PresentationRateControl_eOutputClock;
                break;
            default:
                astmlibConfig.sPresentation.ePreferredPresentationRateControl = BASTMlib_PresentationRateControl_eTimeStamp;
                break;
        }
        astmlibConfig.sClockCoupling.uiMinimumTimeBetweenEvents = pSettings->clockCouplingConfig.minimumTimeBetweenEvents;
        astmlibConfig.sClockCoupling.uiDeviationThreshold = pSettings->clockCouplingConfig.deviationThreshold;
        astmlibConfig.sClockCoupling.uiDeviantCountThreshold = pSettings->clockCouplingConfig.deviantCountThreshold;
        astmlibConfig.sClockCoupling.uiIdealCountThreshold = pSettings->clockCouplingConfig.idealCountThreshold;
        astmlibConfig.sClockCoupling.uiInitialAcquisitionTime = pSettings->clockCouplingConfig.initialAcquisitionTime;
        astmlibConfig.sClockCoupling.uiProcessingFrequency = pSettings->clockCouplingConfig.processingFrequency;
        astmlibConfig.sClockCoupling.uiIdealProcessingFrequency = pSettings->clockCouplingConfig.idealProcessingFrequency;
        astmlibConfig.sClockCoupling.uiSettlingTime = pSettings->clockCouplingConfig.settlingTime;
        
        BASTMlib_SetConfig(astm->lib, &astmlibConfig);
    }

    if (astm->settings.videoDecoder)
    {
        BASTMlib_Presenter_GetConfig(astm->presenterContexts[0].presenter, &astmPresenterConfig );
        astmPresenterConfig.uiMinimumTimeBetweenEvents = pSettings->videoPresenterConfig.minimumTimeBetweenEvents;   
        astmPresenterConfig.uiPassEventCountThreshold = pSettings->videoPresenterConfig.passEventCountThreshold;
        astmPresenterConfig.uiFailEventCountThreshold = pSettings->videoPresenterConfig.failEventCountThreshold;
        if (astm->transportType == NEXUS_TransportType_eDssEs || astm->transportType == NEXUS_TransportType_eDssPes)
        {
            astmPresenterConfig.ePtsDomain = BASTMlib_ClockRate_e27Mhz;
        }
        else
        {
            astmPresenterConfig.ePtsDomain = BASTMlib_ClockRate_e45Khz;
        }   
        BASTMlib_Presenter_SetConfig(astm->presenterContexts[0].presenter, &astmPresenterConfig );
    }
    
    for (i = 0; i < NEXUS_ASTM_AUDIO_DECODERS; i++)
    {
        if (astm->settings.audioDecoder[i])
        {
            BASTMlib_Presenter_GetConfig(astm->presenterContexts[i + 1].presenter, &astmPresenterConfig);
            astmPresenterConfig.uiMinimumTimeBetweenEvents = pSettings->audioPresenterConfig[i].minimumTimeBetweenEvents;    
            astmPresenterConfig.uiPassEventCountThreshold = pSettings->audioPresenterConfig[i].passEventCountThreshold;
            astmPresenterConfig.uiFailEventCountThreshold = pSettings->audioPresenterConfig[i].failEventCountThreshold;
            if (astm->transportType == NEXUS_TransportType_eDssEs || astm->transportType == NEXUS_TransportType_eDssPes)
            {
                astmPresenterConfig.ePtsDomain = BASTMlib_ClockRate_e27Mhz;
            }
            else
            {
                astmPresenterConfig.ePtsDomain = BASTMlib_ClockRate_e45Khz;
            }
            BASTMlib_Presenter_SetConfig(astm->presenterContexts[i + 1].presenter, &astmPresenterConfig);
        }
    }

    BASTMlib_GetStatus(astm->lib, &astm->status);

    NEXUS_Astm_P_ResolveAdaptiveModes(astm, &newClockCoupling, &newStcSource, &newPresentationRateControl);

    /* determine changes */
    if (astm->timebase && (pSettings->clockCoupling != astm->settings.clockCoupling))
    {
        applyClockCoupling = true;
    }

    if 
    (
        pSettings->stcChannel 
        && 
        (
            stcChanged 
            || 
            videoChanged
            || 
            audioChanged
            || 
            pSettings->stcSource != astm->settings.stcSource 
            || 
            pSettings->stcMaster != astm->settings.stcMaster 
            || 
            pSettings->syncLimit != astm->settings.syncLimit
        )
    )
    {
        applyStcSource = true;
    }

    if 
    (
        videoChanged 
        ||
        audioChanged
        ||
        pSettings->presentationRateControl != astm->settings.presentationRateControl
    )
    {
        applyPresentationRateControl = true;
    }

    {
        NEXUS_StcChannelTsmMode oldTsmMode;
        NEXUS_AstmClockCoupling oldClockCoupling = astm->settings.clockCoupling;
        NEXUS_AstmStcSource oldStcSource = astm->settings.stcSource;
        NEXUS_AstmPresentationRateControl oldPresentationRateControl = astm->settings.presentationRateControl;

        NEXUS_Astm_P_ResolveAdaptiveModes(astm, &oldClockCoupling, &oldStcSource, &oldPresentationRateControl);

        oldTsmMode = NEXUS_Astm_P_ComputeStcChannelTsmMode(astm, oldStcSource,
            astm->settings.stcMaster, oldPresentationRateControl);
        newTsmMode = NEXUS_Astm_P_ComputeStcChannelTsmMode(astm, newStcSource,
            pSettings->stcMaster, newPresentationRateControl);
        if 
        (
            pSettings->stcChannel 
            && 
            (
                stcChanged
                || 
                newTsmMode != oldTsmMode
            )
        )
        {
            applyTsmMode = true;
        }
    }

    /* copy settings */
    astm->settings = *pSettings;

    /* apply settings */
    {
        if (applyClockCoupling)
        {
            rc = NEXUS_Astm_P_ApplyClockCoupling(astm, newClockCoupling);
            if (rc) goto err;
        }

        if (applyStcSource)
        {
            rc = NEXUS_Astm_P_ApplyStcSource(astm, newStcSource, pSettings->stcMaster, pSettings->syncLimit);
            if (rc) goto err;
        }

        if (applyPresentationRateControl)
        {
            rc = NEXUS_Astm_P_ApplyPresentationRateControl(astm, newPresentationRateControl);
            if (rc) goto err;
        }

        if (applyTsmMode)
        {
            rc = NEXUS_Astm_P_ApplyStcChannelTsmMode(astm, newTsmMode);
            if (rc) goto err;
        }
    }

#ifdef NEXUS_ASTM_ALC_SUPPORT
    BDBG_WRN(("ALC: %s", astm->settings.enableAutomaticLifecycleControl ? "enabled" : "disabled"));
    BDBG_WRN(("Automatic lifecycle control is deprecated and does not work correctly for all cases.  Use at own risk."));
#else
    if (astm->settings.enableAutomaticLifecycleControl)
    {
        BDBG_WRN(("Automatic lifecycle control was selected as enabled, but is not compiled in (it is deprecated and nonfunctional for some cases). Ignored."));
    }
#endif

    goto end;

err:

end:

    BKNI_EnterCriticalSection();
    astm->ready = true;
    BKNI_LeaveCriticalSection();

    return 0;
}

static NEXUS_Error NEXUS_Astm_P_Start(NEXUS_AstmHandle astm)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    if (!NEXUS_GetEnv("astm_disabled"))
    {
        BDBG_MSG(("ASTM starting"));
        rc = (NEXUS_Error)BASTMlib_Start(astm->lib);
        if (rc) goto err;


        rc = NEXUS_Astm_P_SetDependencyAstmBehavior(astm, astm->settings.enabled);
    }

    astm->started = true;

err:

    return rc;
}

static void NEXUS_Astm_P_Stop(NEXUS_AstmHandle astm)
{
    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    if (!NEXUS_GetEnv("astm_disabled"))
    {
        BDBG_MSG(("ASTM stopping"));
        BASTMlib_Stop(astm->lib);

        NEXUS_Astm_P_SetDependencyAstmBehavior(astm, false);
    }

    /* cancel any pending nexus timers */
    NEXUS_Astm_P_CancelPendingTimers(astm);

    astm->pcrReceived = false;
    astm->started = false;
}

NEXUS_Error NEXUS_Astm_Start(NEXUS_AstmHandle astm)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    if (!astm->settings.enableAutomaticLifecycleControl)
    {
        if (!astm->started)
        {
            rc = NEXUS_Astm_P_Start(astm);
        }
        else
        {
            BDBG_WRN(("ASTM already started"));
            rc = NEXUS_INVALID_PARAMETER;
        }
    }
    else
    {
        BDBG_WRN(("You must disable automatic lifecycle control before manually starting ASTM."));
        rc = NEXUS_INVALID_PARAMETER;
    }

    return rc;
}

void NEXUS_Astm_Stop(NEXUS_AstmHandle astm)
{
    BDBG_OBJECT_ASSERT(astm, NEXUS_Astm);

    if (!astm->settings.enableAutomaticLifecycleControl)
    {
        if (astm->started)
        {
            NEXUS_Astm_P_Stop(astm);
        }
        else
        {
            BDBG_WRN(("ASTM not started"));
        }
    }
    else
    {
        BDBG_WRN(("You must disable automatic lifecycle control before manually stopping ASTM."));
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_ASTM_ASYNC_TIMER)
void NEXUS_Astm_GetStausChangeCnt(unsigned long *ulClockCnt, unsigned long *ulPresentationCnt)
{
	*ulClockCnt = ulClockCouplingChangeCnt;
 	*ulPresentationCnt = ulPresentationChangeCnt; 
}
void NEXUS_Astm_ClearStausChangeCnt(void)
{
	ulClockCouplingChangeCnt = 0;
	ulPresentationChangeCnt = 0;
}
#else
void NEXUS_Astm_GetStausChangeCnt(unsigned long *ulClockCnt, unsigned long *ulPresentationCnt)
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
    BSTD_UNUSED(ulClockCnt);
    BSTD_UNUSED(ulPresentationCnt);
#endif	
}
void NEXUS_Astm_ClearStausChangeCnt(void)
{
}
#endif

