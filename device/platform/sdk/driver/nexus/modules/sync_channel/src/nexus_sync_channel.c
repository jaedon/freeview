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
 * $brcm_Workfile: nexus_sync_channel.c $
 * $brcm_Revision: 65 $
 * $brcm_Date: 10/16/12 4:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/sync_channel/src/nexus_sync_channel.c $
 * 
 * 65   10/16/12 4:12p bandrews
 * SW7425-4022: add delay received to sink status
 * 
 * 64   10/12/12 3:12p erickson
 * SW7435-419: change proc to BDBG_LOG to that they are still available
 *  with B_REFSW_DEBUG_LEVEL=err
 * 
 * 63   9/28/12 5:52p bandrews
 * SW7435-378: fix warning
 * 
 * 62   9/27/12 8:34p bandrews
 * SW7435-378: prevent VDC sub-field phase delay changes from witholding
 *  unmute
 * 
 * 61   9/17/12 7:31p bandrews
 * SW7420-2356: move messages to inside guards
 * 
 * 60   9/5/12 8:17p bandrews
 * SW7420-2314: move update of connectionChanged to before state update
 * 
 * 59   8/22/12 9:42p bandrews
 * SW7420-2314: check if video input is set first
 * 
 * 58   8/22/12 8:17p bandrews
 * SW7420-2314: mute control should translate directly to startup mute
 *  flag
 * 
 * 57   8/22/12 8:15p bandrews
 * SW7420-2314: mute control will now be used to control mute on startup
 *  flag in av inputs
 * 
 * 56   8/22/12 8:08p bandrews
 * SW7429-244: discovered a hole in processing window swap; may not be the
 *  cause of this crash, but might be related
 * 
 * 55   8/10/12 7:47p bandrews
 * SW7420-2314: fix debug message
 * 
 * 54   8/7/12 3:42p bandrews
 * SW7420-2314: clean up video input changed event
 * 
 * 53   8/3/12 7:03p bandrews
 * SW7420-2314: merge to main
 * 
 * SW7420-2314/1   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * 52   7/5/12 5:11p bandrews
 * SW7231-876: fix warnings
 * 
 * 51   7/3/12 5:40p bandrews
 * SW7425-3422: added status to sync channel
 * 
 * 50   7/3/12 5:11p bandrews
 * SW7425-717: merge to main
 * 
 * 49   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7425-717/6   6/11/12 9:00p bandrews
 * SW7425-717: update use of conversion functions
 * 
 * SW7425-717/5   6/11/12 8:34p bandrews
 * SW7425-717: remove hysteresis
 * 
 * SW7425-717/4   6/11/12 7:40p bandrews
 * SW7425-717: fix bugs, add status to sync channel
 * 
 * SW7425-717/3   6/4/12 6:51p bandrews
 * SW7425-717: Added refresh rate for later; add prototype for
 *  UpdatePhaseDelay_isr
 * 
 * SW7425-717/2   6/4/12 5:59p bandrews
 * SW7425-717: update to fix some bugs and get correct status
 * 
 * SW7425-717/1   5/15/12 7:24p bandrews
 * SW7425-717: initial impl
 * 
 * 48   2/21/12 6:15p bandrews
 * SW7346-684: Sarnoff stream offset handling
 * 
 * 47   11/15/11 7:07p bandrews
 * SW7425-1235: merge to main
 * 
 * SW7425-1235/1   10/27/11 5:48p bandrews
 * SW7425-1235: 6 channel support added
 * 
 * 46   8/12/11 4:19p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 * 
 * 45   6/29/11 8:51p bandrews
 * SW7405-3774: change in mute control or simul unmute settings should
 *  also cause a reconfig of synclib
 * 
 * 44   6/28/11 4:37p bandrews
 * SW7425-705: merge to main
 * 
 * SW7425-705/1   6/20/11 9:42p bandrews
 * SW7425-705: Disable application of nonzero window delay when source is
 *  NRT
 * 
 * 43   10/12/10 8:16p bandrews
 * SW7405-3884: get proper defaults from synclib for module settings
 * 
 * 42   7/27/10 6:58p bandrews
 * SW7420-801: temporary fix for off-by-1-frame lipsync error until sync
 *  channel can keep track of state from synclib
 * 
 * 41   6/14/10 6:37p vishk
 * SW7405-3884: sync: expose synclib parameters through nexus_sync_channel
 * 
 * 40   6/9/10 9:43p bandrews
 * SW7420-801: sync should reset the mute and delay on disconnect of
 *  inputs,outputs
 * 
 * 39   6/9/10 8:20p bandrews
 * SW7405-4436: print channel pointer for debug
 * 
 * 38   3/8/10 8:55p bandrews
 * SW7342-155: print error if audio pi returns delay larger than max
 * 
 * 37   3/8/10 4:15p bandrews
 * SW7342-155: Updated sync channel to match RAP delay value return domain
 * 
 * 36   2/2/10 7:27p bandrews
 * SW7405-3774: add validity flag for video source delay
 * 
 * 35   2/1/10 8:08p bandrews
 * SW7405-3774: add mute control support to sync channel
 * 
 * 34   7/24/09 3:53p bandrews
 * PR52812: Added delay kickstart
 * 
 * 33   6/24/09 7:28p mphillip
 * PR54926: Add /proc/sync_channel
 * 
 * 32   4/28/09 10:43p bandrews
 * PR54526: Fix format change call into sync; fix static rate mismatch
 *  detection
 * 
 * 31   2/9/09 3:35p bandrews
 * PR51273: make this part conditional too
 * 
 * 30   1/26/09 12:05p erickson
 * PR51468: global variable naming convention
 *
 * 29   1/26/09 11:07a erickson
 * PR51468: global variable naming convention
 *
 * 28   12/16/08 10:52p bandrews
 * PR46789: add preferred units to audio
 *
 * 27   11/26/08 6:00p bandrews
 * PR49294: Added precision lipsync flag; ignore video input callback when
 *  audio says it's in dsola mode
 *
 * PR49294/1   11/26/08 5:11p bandrews
 * PR49294: Added precision lipsync flag; ignore video input callback when
 *  audio says it's in dsola mode
 *
 * 26   10/30/08 10:31p bandrews
 * PR48453: DSS support added
 *
 * 25   10/20/08 7:39p bandrews
 * PR46915: Cleaned up warnings
 *
 * 24   10/20/08 7:09p bandrews
 * PR46915: Clean up window callbacks when input is nullified
 *
 * 23   10/6/08 11:38p erickson
 * PR44801: merge
 *
 * PR44801/1   9/29/08 9:00p bandrews
 * PR44801: Added support for display VEC alignment
 *
 * 22   9/3/08 1:38p erickson
 * PR46258: merge
 *
 * PR46258/1   9/1/08 6:04p dyzhang
 * PR46258: Start and stop a current record few times cause "segmentation
 *  fault" occurred. Callbacks need to be cleared when destory the sync
 *  channel.
 *
 * 21   6/16/08 5:52p bandrews
 * PR43010: Set avg video source delay in 45 KHz units, otherwise sync
 *  channel complains when it gets its delay back
 *
 * 20   6/16/08 4:52p bandrews
 * PR43010: Disable precision lipsync until this is debugged
 *
 * 19   6/12/08 8:18p bandrews
 * PR42265: Fixed warning
 *
 * 18   6/10/08 7:20p bandrews
 * PR42265: Added 120 ms delay for MPEG-2 video streams
 *
 * 17   4/30/08 6:21p bandrews
 * PR42118: Fixed warnings on return code
 *
 * 16   4/29/08 4:28p bandrews
 * PR42118: Check for NULL input handles before calling out to other
 *  modules.  Raise call-in messages with NULL input handles to warnings,
 *  as this reflects an error in upper layer programming
 *
 * 15   4/11/08 1:14p bandrews
 * PR36808: Downgraded warning to message
 *
 * 14   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 13   4/9/08 1:33p bandrews
 * PR41192: Fixed ordering of calls to sync channel and add support for
 *  NULL inputs to sync channel
 *
 * 12   4/8/08 6:43p bandrews
 * PR41520: Allow NULL input handles to be passed in
 *
 * 11   4/3/08 2:40p bandrews
 * PR40090: sync needs separate context for syslib callbacks and synclib
 *  callbacks
 *
 * 10   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 9   3/26/08 5:11p bandrews
 * PR40090: syslib needs context of at least module handle
 *
 * 8   3/11/08 3:09p erickson
 * PR40222: enable audio output delay
 *
 * 7   2/28/08 9:34p bandrews
 * PR37951: Give audio decoder delay to sync in decoder units so that it
 *  will come out of sync in decoder units
 *
 * 6   2/28/08 10:17a erickson
 * PR36808: added debug, no warnings
 *
 * 5   2/27/08 2:46p bandrews
 * PR37951: For synclib testing
 *
 * 4   2/22/08 2:30p erickson
 * PR36808: update
 *
 * 3   2/21/08 9:12a erickson
 * PR36808: SYNClib changes
 *
 * 2   1/31/08 10:03a erickson
 * PR36808: update
 *
 * Nexus_Devel/1   11/8/07 3:15p erickson
 * PR36808: added sync_channel
 *
 **************************************************************************/
#include "nexus_sync_channel_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_syslib_framework.h"
#include "priv/nexus_core_video.h"
#include "priv/nexus_video_input_priv.h"
#include "priv/nexus_video_window_priv.h"
#include "priv/nexus_audio_input_priv.h"
#include "priv/nexus_audio_output_priv.h"
#include "priv/nexus_sync_channel_priv.h"
#include "bsynclib.h"

BDBG_MODULE(nexus_sync_channel);

NEXUS_ModuleHandle g_NEXUS_syncChannelModule;
struct {
    NEXUS_SyncChannelModuleSettings settings;
    NEXUS_SYSlib_ContextHandle syslibContext;
    BSYNClib_Handle synclib;
} g_NEXUS_syncChannel;

/****************************************
* Module functions
***************/

NEXUS_Error NEXUS_SyncChannel_P_ConnectVideoInput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings);
NEXUS_Error NEXUS_SyncChannel_P_DisconnectVideoInput(NEXUS_SyncChannelHandle syncChannel);
NEXUS_Error NEXUS_SyncChannel_P_ConnectVideoOutput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index);
NEXUS_Error NEXUS_SyncChannel_P_DisconnectVideoOutput(NEXUS_SyncChannelHandle syncChannel, unsigned int index);
NEXUS_Error NEXUS_SyncChannel_P_ConnectAudioInput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index);
NEXUS_Error NEXUS_SyncChannel_P_DisconnectAudioInput(NEXUS_SyncChannelHandle syncChannel, unsigned int index);
NEXUS_Error NEXUS_SyncChannel_P_ConnectAudioOutput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index);
NEXUS_Error NEXUS_SyncChannel_P_DisconnectAudioOutput(NEXUS_SyncChannelHandle syncChannel, unsigned int index);
static void NEXUS_Synclib_P_VideoInputDelayCallback_isr(void *context, int delay);
NEXUS_Error NEXUS_Synclib_P_SetVideoSink(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index);

void NEXUS_SyncChannelModule_GetDefaultSettings(NEXUS_SyncChannelModuleSettings *pSettings)
{
    BSYNClib_Settings synclibSettings;
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BSYNClib_GetDefaultSettings(&synclibSettings);
    pSettings->video.requireFullScreen = synclibSettings.sVideo.bRequireFullScreen;
    pSettings->video.tsmLockTimeout = synclibSettings.sVideo.uiTsmLockTimeout;
    pSettings->video.unmuteTimeout = synclibSettings.sVideo.uiUnmuteTimeout;
    pSettings->video.unconditionalUnmuteTimeout = synclibSettings.sVideo.uiUnconditionalUnmuteTimeout;
    pSettings->video.rateMismatchDetection.timeout = synclibSettings.sVideo.sRateMismatchDetection.uiTimeout;
    pSettings->video.rateMismatchDetection.acceptableMtbcLower = synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcLower;
    pSettings->video.rateMismatchDetection.acceptableMtbcUpper = synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcUpper;
    pSettings->video.rateMismatchDetection.acceptableTtlc = synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableTtlc;
    pSettings->video.rateRematchDetection.timeout = synclibSettings.sVideo.sRateRematchDetection.uiTimeout;
    pSettings->video.rateRematchDetection.acceptableTtlc = synclibSettings.sVideo.sRateRematchDetection.uiAcceptableTtlc;   
    pSettings->audio.unmuteTimeout = synclibSettings.sAudio.uiUnmuteTimeout;
    pSettings->audio.receiverDelayCompensation = synclibSettings.sAudio.uiReceiverDelayCompensation;
    pSettings->audio.unconditionalUnmuteTimeout = synclibSettings.sAudio.uiUnconditionalUnmuteTimeout;
}

void NEXUS_SyncChannelModule_Print(void)
{
    BDBG_LOG(("Sync Channel:"));
    BDBG_LOG((" settings: a:%p v:%p",g_NEXUS_syncChannel.settings.modules.audio,g_NEXUS_syncChannel.settings.modules.display));
    BDBG_LOG((" handles: sys:%p sync:%p",g_NEXUS_syncChannel.syslibContext,g_NEXUS_syncChannel.synclib));
}

NEXUS_ModuleHandle NEXUS_SyncChannelModule_Init(const NEXUS_SyncChannelModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_SYSlib_ContextSettings syslibSettings;
    BSYNClib_Settings synclibSettings;
    BERR_Code rc;

    BDBG_ASSERT(!g_NEXUS_syncChannelModule);

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pSettings->modules.display);
    BDBG_ASSERT(pSettings->modules.audio);

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    moduleSettings.dbgPrint = NEXUS_SyncChannelModule_Print;
    moduleSettings.dbgModules = "nexus_sync_channel";
    g_NEXUS_syncChannelModule = NEXUS_Module_Create("sync_channel", &moduleSettings);
    if (!g_NEXUS_syncChannelModule) {
        return NULL;
    }

    g_NEXUS_syncChannel.settings = *pSettings;

    NEXUS_SYSlib_GetDefaultContextSettings_priv(&syslibSettings);
    syslibSettings.module = NEXUS_MODULE_SELF;
    NEXUS_SYSlib_CreateContext_priv(&g_NEXUS_syncChannel.syslibContext, &syslibSettings);

    BSYNClib_GetDefaultSettings(&synclibSettings);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)	
	/*	NOTICE!!
	*	This line is added to get rid of a bug that the sync channel doesn't work when the screen is cut.
	*/
    synclibSettings.sVideo.bRequireFullScreen = false;
#else
    synclibSettings.sVideo.bRequireFullScreen = pSettings->video.requireFullScreen;
#endif
    synclibSettings.sVideo.uiTsmLockTimeout = pSettings->video.tsmLockTimeout;
    synclibSettings.sVideo.uiUnmuteTimeout = pSettings->video.unmuteTimeout;
    synclibSettings.sVideo.uiUnconditionalUnmuteTimeout = pSettings->video.unconditionalUnmuteTimeout;
    synclibSettings.sVideo.sRateMismatchDetection.uiTimeout = pSettings->video.rateMismatchDetection.timeout;
    synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcLower = pSettings->video.rateMismatchDetection.acceptableMtbcLower;
    synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcUpper = pSettings->video.rateMismatchDetection.acceptableMtbcUpper;
    synclibSettings.sVideo.sRateMismatchDetection.uiAcceptableTtlc = pSettings->video.rateMismatchDetection.acceptableTtlc;
    synclibSettings.sVideo.sRateRematchDetection.uiTimeout = pSettings->video.rateRematchDetection.timeout;
    synclibSettings.sVideo.sRateRematchDetection.uiAcceptableTtlc = pSettings->video.rateRematchDetection.acceptableTtlc;   
    synclibSettings.sAudio.uiUnmuteTimeout = pSettings->audio.unmuteTimeout;
    synclibSettings.sAudio.uiReceiverDelayCompensation = pSettings->audio.receiverDelayCompensation;
    synclibSettings.sAudio.uiUnconditionalUnmuteTimeout = pSettings->audio.unconditionalUnmuteTimeout;
    rc = BSYNClib_Open(&synclibSettings, &g_NEXUS_syncChannel.synclib);
    if (rc) {BERR_TRACE(rc); goto error;}

    return g_NEXUS_syncChannelModule;

error:
    NEXUS_SyncChannelModule_Uninit();
    return NULL;
}

void NEXUS_SyncChannelModule_Uninit()
{
    NEXUS_SYSlib_DestroyContext_priv(g_NEXUS_syncChannel.syslibContext);
    BSYNClib_Close(g_NEXUS_syncChannel.synclib);
    NEXUS_Module_Destroy(g_NEXUS_syncChannelModule);
    g_NEXUS_syncChannelModule = NULL;
}

/****************************************
* API functions
***************/

/* Used to collect information to process callbacks from around the system */
struct NEXUS_SyncChannelContext {
    NEXUS_SyncChannelHandle syncChannel;
    unsigned index; /* SYNClib index */
    bool nonRealTime;
    union
    {
        NEXUS_VideoInputSyncSettings videoInput;
        NEXUS_VideoWindowSyncSettings window;
        NEXUS_AudioInputSyncSettings audioInput;
    } settings;
    bool connectionChanged;
};

#define NEXUS_SYNC_CHANNEL_VIDEO_INPUTS  1
#define NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS NEXUS_SYNC_CHANNEL_NUM_VIDEO_OUTPUTS
#define NEXUS_SYNC_CHANNEL_AUDIO_INPUTS  NEXUS_SYNC_CHANNEL_NUM_AUDIO_INPUTS
#define NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS 4

struct NEXUS_SyncChannel
{
    NEXUS_OBJECT(NEXUS_SyncChannel);
    NEXUS_SyncChannelSettings settings;
    NEXUS_SyncChannelStatus status;
    bool avTrickModeEnabled;
    bool synclibVsourceDelayCallbackEnabled;
    struct NEXUS_SyncChannelContext vsourceContext;
    int vsourcePhaseOffset;
    BKNI_EventHandle phaseOffsetAdjustmentEvent;
    NEXUS_EventCallbackHandle phaseOffsetAdjustmentEventCallback;
    BKNI_EventHandle videoInputConnectionChangedEvent;
    NEXUS_EventCallbackHandle videoInputConnectionChangedCallback;
    struct NEXUS_SyncChannelContext vsinkContext[NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS];
    struct NEXUS_SyncChannelContext asourceContext[NEXUS_SYNC_CHANNEL_AUDIO_INPUTS];
    struct NEXUS_SyncChannelContext asinkContext[NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS];

    struct
    {
        NEXUS_VideoWindowHandle window[NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS];
        NEXUS_AudioOutput audioOutput[NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS];
    } outputs;

    NEXUS_VideoWindowHandle syncLockedWindow;
    bool displaysAligned;

    /* syslib info */
    BSYNClib_Channel_Handle synclibChannel;
};

void NEXUS_SyncChannel_GetDefaultSettings(NEXUS_SyncChannelSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->enablePrecisionLipsync = true;
    pSettings->enableMuteControl = true;
}

/**********************
*
* SYNClib VideoSource = NEXUS VideoInput, which can be VideoDecoder, AnalogVideoDecoder, etc.
*
***********************/

static BERR_Code
NEXUS_Synclib_P_SetVideoSourceMute(void * pvParm1, int iParm2, unsigned index, bool bMute)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoInputSyncSettings syncSettings;

    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(index);
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    if (syncChannel->settings.videoInput)
    {
        BDBG_MSG(("[%p] SetVideoSourceMute(%u): %s", syncChannel, index, bMute ? "muted" : "unmuted"));
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoInput_GetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        syncSettings.mute = bMute;
        rc = NEXUS_VideoInput_SetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* update status */
        syncChannel->status.video.source.muted = syncSettings.mute;
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set video source mute with NULL input", syncChannel));
    }

    return rc;
}

extern const char * BSYNClib_P_UnitsStrings[];

static BERR_Code
NEXUS_Synclib_P_SetVideoSourceDelay(void * pvParm1, int iParm2, unsigned int index, BSYNClib_UnsignedValue * pDelay)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoInputSyncSettings syncSettings;

    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(index);
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    if (syncChannel->settings.videoInput)
    {
        if (syncChannel->displaysAligned)
        {
            BDBG_MSG(("[%p] SetVideoSourceDelay(%u): %u %s", syncChannel, index, pDelay->uiValue + syncChannel->vsourcePhaseOffset, BSYNClib_P_UnitsStrings[pDelay->eUnits]));
        }
        else
        {
            BDBG_MSG(("[%p] SetVideoSourceDelay(%u): %u %s", syncChannel, index, pDelay->uiValue, BSYNClib_P_UnitsStrings[pDelay->eUnits]));
        }

        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display); /* TODO: bja - why display? */
        NEXUS_VideoInput_GetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        BDBG_ASSERT(pDelay->eUnits == BSYNClib_Units_e45KhzTicks);
        if (syncChannel->displaysAligned)
        {
            syncSettings.delay = (unsigned)((int)pDelay->uiValue + syncChannel->vsourcePhaseOffset);
        }
        else
        {
            syncSettings.delay = pDelay->uiValue;
        }
        rc = NEXUS_VideoInput_SetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* update status */
        BSYNClib_ConvertSigned(syncSettings.delay, pDelay->eUnits, 
            BSYNClib_Units_eMilliseconds, &syncChannel->status.video.source.delay.applied);
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set video source delay with NULL input", syncChannel));
    }

    return rc;
}

static BERR_Code
NEXUS_Synclib_P_SetVideoSourceDelayNotification(void * pvParm1, int iParm2, unsigned index, bool bEnable, BSYNClib_UnsignedValue *pThreshold)
{
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoInputSyncSettings syncSettings;
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;

    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(index);
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    /* save this for filtering here instead of in video source code */
    syncChannel->synclibVsourceDelayCallbackEnabled = bEnable;
    
    if (syncChannel->settings.videoInput)
    {
        NEXUS_VideoInputSyncStatus syncStatus;
        BDBG_MSG(("[%p] SetVideoSourceDelayNotification(%u): %s %u %s", syncChannel, index, bEnable? "enabled" : "disabled", pThreshold->uiValue, BSYNClib_P_UnitsStrings[pThreshold->eUnits]));
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoInput_GetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        syncSettings.delayCallbackThreshold = pThreshold->uiValue;
        rc = NEXUS_VideoInput_SetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* update status */
        BSYNClib_ConvertSigned(syncSettings.delayCallbackThreshold, pThreshold->eUnits, 
            BSYNClib_Units_eMilliseconds, &syncChannel->status.video.source.delay.notificationThreshold);

        /* if sync has enabled the callback, and the delay is valid, then grab the value */
        if (syncChannel->synclibVsourceDelayCallbackEnabled)
        {
            BKNI_EnterCriticalSection();
            NEXUS_VideoInput_GetSyncStatus_isr(syncChannel->settings.videoInput, &syncStatus);
            if (syncStatus.delayValid)
            {
                NEXUS_Synclib_P_VideoInputDelayCallback_isr(&syncChannel->vsourceContext, syncStatus.delay);
            }
            BKNI_LeaveCriticalSection();
        }
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set video source delay notification with NULL input", syncChannel));
    }

    return rc;
}

static bool NEXUS_SyncChannel_P_AreDisplaysAligned_isr(NEXUS_SyncChannelHandle syncChannel)
{
    bool aligned;
    unsigned i;
    NEXUS_VideoWindowSyncStatus status;

    /* check all other windows for aligned displays */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        if (syncChannel->outputs.window[i])
        {
            NEXUS_VideoWindow_GetSyncStatus_isr(syncChannel->outputs.window[i], &status);
            if (status.aligned)
            {
                break;
            }
        }
    }
    
    if (i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS)
    {
        aligned = true;
    }
    else
    {
        aligned = false;
    }

    return aligned;
}

static unsigned
NEXUS_Synclib_P_ComputeVideoSourceCustomDelay_isr(const NEXUS_SyncChannelHandle syncChannel, const NEXUS_VideoInputSyncStatus * pStatus)
{
    unsigned customDelay = 0;

    /* all delays are in units of ms */
    customDelay = syncChannel->settings.customVideoDelay;
    customDelay += pStatus->customPtsOffset;

    return customDelay;
}

static void
NEXUS_Synclib_P_VideoInputStartCallback_isr(void *context, int param)
{
    BSYNClib_VideoSource_Config vsourceConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_VideoInputSyncStatus status;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);

    if (syncContext->syncChannel->settings.videoInput)
    {
        NEXUS_VideoInput_GetSyncStatus_isr(syncContext->syncChannel->settings.videoInput, &status);
        BDBG_MSG(("[%p] VideoInputStart(%u) started=%d digital=%d", syncContext->syncChannel, syncContext->index, status.started, status.digital));
        BSYNClib_Channel_GetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);
        vsourceConfig.bStarted = status.started;
        vsourceConfig.bDigital = status.digital;
        vsourceConfig.bLastPictureHeld = status.lastPictureHeld;

        vsourceConfig.sDelay.sCustom.eUnits = BSYNClib_Units_eMilliseconds;
        vsourceConfig.sDelay.sCustom.uiValue = NEXUS_Synclib_P_ComputeVideoSourceCustomDelay_isr(syncContext->syncChannel, &status);

        syncContext->nonRealTime = status.nonRealTime;

        BSYNClib_Channel_SetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);

        /* status updates */
        syncContext->syncChannel->status.nonRealTime = syncContext->nonRealTime;
        syncContext->syncChannel->status.video.source.started = vsourceConfig.bStarted;
        syncContext->syncChannel->status.video.source.digital = vsourceConfig.bDigital;
        syncContext->syncChannel->status.video.source.lastPictureHeld = vsourceConfig.bLastPictureHeld;
        syncContext->syncChannel->status.video.source.delay.custom = vsourceConfig.sDelay.sCustom.uiValue;
        syncContext->syncChannel->status.video.source.delay.applied = -1;
        syncContext->syncChannel->status.video.source.delay.measured = -1;
        syncContext->syncChannel->status.video.source.delay.phase = -1;
        syncContext->syncChannel->status.video.source.delay.notificationThreshold = -1;
        syncContext->syncChannel->status.video.source.format.height = 0;
        syncContext->syncChannel->status.video.source.format.frameRate = 0;
    }
    else
    {
        BDBG_WRN(("[%p] VideoInputStartStop event received for NULL input", syncContext->syncChannel));
    }
}

static void
NEXUS_Synclib_P_VideoInputFormatCallback_isr(void *context, int param)
{
    BSYNClib_VideoSource_Config vsourceConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_VideoInputSyncStatus status;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);

    if (syncContext->syncChannel->settings.videoInput)
    {
        NEXUS_VideoInput_GetSyncStatus_isr(syncContext->syncChannel->settings.videoInput, &status);
        BDBG_MSG(("[%p] VideoInputFormat(%u) height=%d, interlaced=%d, framerate=%d", syncContext->syncChannel, syncContext->index, status.height, status.interlaced, status.frameRate));
        BSYNClib_Channel_GetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);
        vsourceConfig.sFormat.bReceived = true;
        vsourceConfig.sFormat.uiHeight = status.height;
        vsourceConfig.sFormat.bInterlaced = status.interlaced;
        vsourceConfig.sFormat.eFrameRate = status.frameRate;

#if 0
        /* 20080616 bandrews - Temporarily disable this callback, as it
        is producing artifacts -- need to have average delay, tho */
        vsourceConfig.sDelay.bReceived = true;
        /* assume average is 1/2 pass window (pass window = 1 frame int, or 2 frame's prog --> 1 vsync either way) */
        vsourceConfig.sDelay.sMeasured.eUnits = BSYNClib_Units_e45KhzTicks;
        switch (status.frameRate)
        {
            case BAVC_FrameRateCode_e25:
            case BAVC_FrameRateCode_e50:
                vsourceConfig.sDelay.sMeasured.uiValue = 900;
                break;
            case BAVC_FrameRateCode_e23_976:
            case BAVC_FrameRateCode_e24:
                /* TODO */
            case BAVC_FrameRateCode_e29_97:
            case BAVC_FrameRateCode_e30:
            case BAVC_FrameRateCode_e59_94:
            case BAVC_FrameRateCode_e60:
                vsourceConfig.sDelay.sMeasured.uiValue = 750;
                break;
            case BAVC_FrameRateCode_eUnknown:
            default:
                vsourceConfig.sDelay.sMeasured.uiValue = 0;
                break;
        }
#endif

        BSYNClib_Channel_SetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);

        /* status updates */
        syncContext->syncChannel->status.video.source.format.height = vsourceConfig.sFormat.uiHeight;
        syncContext->syncChannel->status.video.source.format.interlaced = vsourceConfig.sFormat.bInterlaced;
        syncContext->syncChannel->status.video.source.format.frameRate = 
            NEXUS_P_RefreshRate_FromFrameRate(NEXUS_P_FrameRate_FromMagnum(vsourceConfig.sFormat.eFrameRate));
    }
    else
    {
        BDBG_WRN(("[%p] VideoInputFormat event received for NULL input", syncContext->syncChannel));
    }
}

static void
NEXUS_Synclib_P_VideoInputDelayCallback_isr(void *context, int delay)
{
    BSYNClib_VideoSource_Config vsourceConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;

    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);
    /* PR49294 added check for callback ignore flag */
    if (syncContext->syncChannel->settings.videoInput)
    {
        /* if callback is enabled by synclib or displays are aligned and we are not in STC trick mode */
        if (syncContext->syncChannel->settings.enablePrecisionLipsync && !syncContext->syncChannel->avTrickModeEnabled 
            && (syncContext->syncChannel->synclibVsourceDelayCallbackEnabled || syncContext->syncChannel->displaysAligned))
        {
            BDBG_MSG(("[%p] VideoInputDelay(%u) %d", syncContext->syncChannel, syncContext->index, delay));
            BSYNClib_Channel_GetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);
            vsourceConfig.sDelay.bReceived = true;
            vsourceConfig.sDelay.sMeasured.uiValue = delay;
            vsourceConfig.sDelay.sMeasured.eUnits = BSYNClib_Units_e45KhzTicks;
            BSYNClib_Channel_SetVideoSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsourceConfig);

            /* status updates */
            BSYNClib_ConvertSigned_isr(vsourceConfig.sDelay.sMeasured.uiValue, vsourceConfig.sDelay.sMeasured.eUnits, 
                BSYNClib_Units_eMilliseconds, &syncContext->syncChannel->status.video.source.delay.measured);
        }
        else
        {
            BDBG_MSG(("[%p] VideoInputDelay %d received while disabled, ignored", syncContext->syncChannel, delay));
        }
    }
    else
    {
        BDBG_WRN(("[%p] VideoInputDelay event received for NULL input", syncContext->syncChannel));
    }
}

static void
NEXUS_Synclib_P_VideoInputConnectionChangedCallback_isr(void *context, int unused)
{
    unsigned int i;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;

    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);
    BSTD_UNUSED(unused);
    BKNI_SetEvent(syncContext->syncChannel->videoInputConnectionChangedEvent);
    
    /* mark all sinks attached to this sync channel as changed */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        syncContext->syncChannel->vsinkContext[i].connectionChanged = true;
    }
}

/**********************
*
* SYNClib VideoSink = NEXUS VideoWindow
*
***********************/

static BERR_Code
NEXUS_Synclib_P_SetVideoSinkDelay(void * pvParm1, int iParm2, unsigned int index, BSYNClib_UnsignedValue * pDelay)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_VideoWindowSyncSettings syncSettings;

    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    BSTD_UNUSED(iParm2);

    BDBG_MSG(("[%p] SetVideoSinkDelay(%u): %u %s", syncChannel, index, pDelay->uiValue, BSYNClib_P_UnitsStrings[pDelay->eUnits]));

    if (syncChannel->outputs.window[index]) /* nexus window handle will persist, even when vdc window handle is gone */
    {
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoWindow_GetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        /* SW7425-705: only apply multi-window delay eq if source not NRT */
        if (!syncChannel->vsourceContext.nonRealTime)
        {
            syncSettings.delay = pDelay->uiValue;
        }
        else
        {
            BDBG_MSG(("[%p] NRT -> multi-window delay eq set to zero on window %d", syncChannel, index));
            syncSettings.delay = 0;
        }
        rc = NEXUS_VideoWindow_SetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* update status */
        syncChannel->vsourceContext.settings.window.delay = syncSettings.delay;
        if (syncChannel->status.video.sinks[index].format.refreshRate)
        {
            syncChannel->status.video.sinks[index].delay.applied = 
                1000 * 1000 * syncSettings.delay / syncChannel->status.video.sinks[index].format.refreshRate;
        }
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set video sink delay with NULL window %d", syncChannel, index));
    }

    return rc;
}

static void 
NEXUS_SyncChannel_P_SortWindowsBySyncLock(NEXUS_SyncChannelHandle syncChannel)
{
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);
    /* TODO */
}

static void
NEXUS_Synclib_P_VideoOutputStateChangeCallback_isr(void *context, int param)
{
    BSYNClib_VideoSink_Config vsinkConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_VideoWindowSyncStatus status;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);

    /* this output is in the middle of a connection change */
    if (syncContext->connectionChanged)
    {
        /* we'll handle this later when we finish changing the connection */
        BDBG_MSG(("Connection change imminent: output %u skipped state change callback", syncContext->index));
        return;
    }

    /* check if displays are aligned */
    syncContext->syncChannel->displaysAligned = NEXUS_SyncChannel_P_AreDisplaysAligned_isr(syncContext->syncChannel);

    /* sort windows by sync-lock can be done only after the VDC windows are created */
    /* TODO: add sync-lock recognition to synclib, this won't be necessary anymore */
    NEXUS_SyncChannel_P_SortWindowsBySyncLock(syncContext->syncChannel);
    
    if (syncContext->syncChannel->outputs.window[syncContext->index])
    {
        NEXUS_VideoWindow_GetSyncStatus_isr(syncContext->syncChannel->outputs.window[syncContext->index], &status);
        BDBG_MSG(("[%p] VideoOutputStateChange(%u) visible=%d", syncContext->syncChannel, syncContext->index, status.visible));
        BSYNClib_Channel_GetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);
        vsinkConfig.bForcedCaptureEnabled = status.forcedCaptureEnabled;
        vsinkConfig.bMasterFrameRateEnabled = status.masterFrameRateEnabled;
        vsinkConfig.bFullScreen = status.fullScreen;
        vsinkConfig.bVisible = status.visible;
        BSYNClib_Channel_SetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);
        if (status.syncLocked)
        {
            syncContext->syncChannel->syncLockedWindow = syncContext->syncChannel->outputs.window[syncContext->index];
            BDBG_MSG(("sync-locked display: %u", syncContext->index));
            /* update status */
            syncContext->syncChannel->status.video.syncLockedWindow = syncContext->index;
        }

        /* update status */
        syncContext->syncChannel->status.video.sinks[syncContext->index].forcedCapture = vsinkConfig.bForcedCaptureEnabled;
        syncContext->syncChannel->status.video.sinks[syncContext->index].masterFrameRate = vsinkConfig.bMasterFrameRateEnabled;
        syncContext->syncChannel->status.video.sinks[syncContext->index].fullScreen = vsinkConfig.bFullScreen;
        syncContext->syncChannel->status.video.sinks[syncContext->index].visible = vsinkConfig.bVisible;
    }
    else
    {
        BDBG_WRN(("[%p] VideoOutputStartStop event received for NULL output %d", syncContext->syncChannel, syncContext->index));
    }
}

static void
NEXUS_Synclib_P_VideoOutputFormatCallback_isr(void *context, int param)
{
    BSYNClib_VideoSink_Config vsinkConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_VideoWindowSyncStatus status;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(syncContext->syncChannel, NEXUS_SyncChannel);

    /* this output is in the middle of a connection change */
    if (syncContext->connectionChanged)
    {
        /* we'll handle this later when we finish changing the connection */
        BDBG_MSG(("Connection change imminent: output %u skipped format callback", syncContext->index));
        return;
    }

    if (syncContext->syncChannel->outputs.window[syncContext->index])
    {
        NEXUS_VideoWindow_GetSyncStatus_isr(syncContext->syncChannel->outputs.window[syncContext->index], &status);
        BDBG_MSG(("[%p] VideoOutputFormat(%u) height=%d, interlaced=%d, framerate=%d, refreshRate=%u", syncContext->syncChannel, syncContext->index, status.height, status.interlaced, status.frameRate, status.refreshRate));
        BSYNClib_Channel_GetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);
        vsinkConfig.sFormat.bReceived = true;
        vsinkConfig.sFormat.uiHeight = status.height;
        vsinkConfig.sFormat.bInterlaced = status.interlaced;
        vsinkConfig.sFormat.eFrameRate = status.frameRate;
        BSYNClib_Channel_SetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);
        if (status.syncLocked)
        {
            syncContext->syncChannel->syncLockedWindow = syncContext->syncChannel->outputs.window[syncContext->index];
            /* update status */
            syncContext->syncChannel->status.video.syncLockedWindow = syncContext->index;
        }

        /* update status */
        syncContext->syncChannel->status.video.sinks[syncContext->index].format.height = vsinkConfig.sFormat.uiHeight;
        syncContext->syncChannel->status.video.sinks[syncContext->index].format.interlaced = vsinkConfig.sFormat.bInterlaced;
        syncContext->syncChannel->status.video.sinks[syncContext->index].format.refreshRate = status.refreshRate;
        if (status.refreshRate)
        {
            syncContext->syncChannel->status.video.sinks[syncContext->index].delay.applied = 
                1000 * 1000 * syncContext->settings.window.delay / ((int)status.refreshRate);
            syncContext->syncChannel->status.video.sinks[syncContext->index].delay.measured = 
                1000 * 1000 * status.delay / ((int)status.refreshRate);
        }
    }
    else
    {
        BDBG_WRN(("[%p] VideoOutputFormat event received for NULL output %d", syncContext->syncChannel, syncContext->index));
    }
}

static void NEXUS_SyncChannel_P_PhaseOffsetAdjustmentEventHandler(void * context)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SyncChannelHandle syncChannel = context;
    BSYNClib_Source_Status status;
    NEXUS_VideoInputSyncSettings syncSettings;

    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    BSYNClib_Channel_GetVideoSourceStatus(syncChannel->synclibChannel, 0, &status);

    if (syncChannel->settings.videoInput)
    {
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display); /* TODO: bja - why display? */
        NEXUS_VideoInput_GetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        BDBG_ASSERT(status.sAppliedDelay.eUnits == BSYNClib_Units_e45KhzTicks);
        if (syncChannel->displaysAligned)
        {
            syncSettings.delay = (unsigned)((int)status.sAppliedDelay.uiValue + syncChannel->vsourcePhaseOffset);
        }
        else
        {
            syncSettings.delay = status.sAppliedDelay.uiValue;
        }
        rc = NEXUS_VideoInput_SetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);
        if (rc) {BERR_TRACE(rc);}

        /* update status */
        BSYNClib_ConvertSigned(syncSettings.delay, BSYNClib_Units_e45KhzTicks, 
            BSYNClib_Units_eMilliseconds, &syncChannel->status.video.source.delay.applied);
    }
    else
    {
        BDBG_MSG(("[%p] Phase offset adjustment event handler called with NULL video input", syncChannel));
    }
}

static const int framePeriods[] = 
{
    0,/* BAVC_FrameRateCode_eUnknown */
    1877,/* BAVC_FrameRateCode_e23_976 */
    1875,/* BAVC_FrameRateCode_e24 */
    1800,/* BAVC_FrameRateCode_e25 */
    1502,/* BAVC_FrameRateCode_e29_97 */
    1500,/* BAVC_FrameRateCode_e30 */
    900,/* BAVC_FrameRateCode_e50 */
    751,/* BAVC_FrameRateCode_e59_94 */
    750,/* BAVC_FrameRateCode_e60 */
    3003,/* BAVC_FrameRateCode_e14_985 */
    6006,/* BAVC_FrameRateCode_e7_493 */
    0 /* BAVC_FrameRateCode_eMax */
};

static void
NEXUS_Synclib_P_VideoOutputDelayCallback_isr(void *context, int delay)
{
    BSYNClib_VideoSink_Config vsinkConfig;
    BSYNClib_Sink_Status vsinkStatus;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_SyncChannelHandle syncChannel = syncContext->syncChannel;
    BSYNClib_Units eDelayUnits;

    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);
    BSTD_UNUSED(delay);

    /* this output is in the middle of a connection change */
    if (syncContext->connectionChanged)
    {
        /* we'll handle this later when we finish changing the connection */
        BDBG_MSG(("Connection change imminent: output %u skipped delay callback: %d", syncContext->index, delay));
        return;
    }

    if (syncChannel->outputs.window[syncContext->index])
    {
        NEXUS_VideoWindowSyncStatus status;
        NEXUS_VideoWindowSyncStatus syncLockedStatus;

        BKNI_Memset(&syncLockedStatus, 0, sizeof(NEXUS_VideoWindowSyncStatus));

        NEXUS_VideoWindow_GetSyncStatus_isr(syncChannel->outputs.window[syncContext->index], &status);
        if (syncChannel->syncLockedWindow)
        {
            NEXUS_VideoWindow_GetSyncStatus_isr(syncChannel->syncLockedWindow, &syncLockedStatus);
        }

        BDBG_MSG(("[%p] VideoOutputDelay(%u,%p) %d", syncChannel, syncContext->index, syncChannel->outputs.window[syncContext->index], status.delay));
        BDBG_MSG(("[%p] VideoOutputPhaseDelay(%u,%p) %d", syncChannel, syncContext->index, syncChannel->outputs.window[syncContext->index], status.phaseDelay));

        /* this window is not sync-locked and is on a display whose alignment target is another display */
        if (!status.syncLocked && status.aligned && syncChannel->settings.videoInput) 
        {
            NEXUS_VideoInputSyncStatus vinStatus;
            int ssPhase = (status.phaseDelay * 45) / 1000; /* phase delay already stores just the sub-VSYNC delay, within +/- 1 VSYNC */

            NEXUS_VideoInput_GetSyncStatus_isr(syncChannel->settings.videoInput, &vinStatus);

            BDBG_MSG(("[%p] source %s (%s); sync-locked display: %s", syncChannel, 
                vinStatus.interlaced ? "interlaced" : "progressive", 
                vinStatus.delayValid ? "valid" : "invalid",
                syncChannel->syncLockedWindow ? (syncLockedStatus.interlaced ? "interlaced" : " progressive") : "none"));
            /* if either source or sync-locked-display is progressive ... */

            if ((!vinStatus.interlaced || (syncChannel->syncLockedWindow && !syncLockedStatus.interlaced)) && vinStatus.delayValid)
            {
                int slPhase = vinStatus.delay;
                int sourceFramePeriod = framePeriods[vinStatus.frameRate];
                int slVsyncPeriod = 45 * 1000 * 1000 / syncLockedStatus.refreshRate;
                int ssVsyncPeriod = 45 * 1000 * 1000 / status.refreshRate;
                int passWindow;
                int sourceVsyncPeriod = sourceFramePeriod;
#ifdef SYNC_CHANNEL_P_USE_HYSTERESIS
                int hysteresis = 45; /* TODO: pull from adjustment threshold once it is correctly defaulted to JTI threshold */
#else
                int hysteresis = 0;
#endif
                int offset = syncChannel->vsourcePhaseOffset;
                
                if (vinStatus.interlaced) 
                {
                    sourceVsyncPeriod /= 2;
                    passWindow = sourceFramePeriod;
                }
                else
                {
                    if (syncLockedStatus.interlaced)
                    {
                        passWindow = slVsyncPeriod * 2;
                    }
                    else
                    {
                        passWindow = ssVsyncPeriod * 2;
                    }
                }

                BDBG_MSG(("[%p] slPhase: %u; offset: %d; ssPhase: %d; pass-window: %d; hysteresis: %d", syncChannel, slPhase, offset, ssPhase, passWindow, hysteresis));

                /* case 1) sync-slipped buffer in (-1, 0) Vsync slip range of the target Vsync delay; 
                   in other words, sync-locked HD window trails sync-slipped SD window; */
                if (ssPhase < 0)
                {
                    /* sync-slipped window moved left to the lower boundary of TSM pass window; */
                    if (slPhase + offset + ssPhase < -hysteresis
                        || ((slPhase + offset < -hysteresis) && (offset < 0)))
                    {
                        /* this will force TSM to repeat a field, i.e. the sync-locked window (TSM) slips from
                        the FIRST_VYSNC_SLOT to the SECOND_VSYNC_SLOT, while the sync-slipped
                        window not only follows along from the left boundary of the TSM PASS WINDOW,
                        but also its multi-buffer also repeats 1-field because the source slips 1-field so that the
                        sync-slipped window delay becomes to the left of the rightmost boundary of the
                        SECOND_VSYNC_SLOT; now sync-locked window leads the sync-slipped window; */
                        offset += sourceVsyncPeriod;
                    }
                }
                /* case 2) sync-slipped buffer in (0, 1) Vsync slip range of the target Vsync delay; 
                   in other words, sync-locked HD window leads sync-slipped SD window; */
                else /* ssPhase >= 0 */
                {
                    /* sync-slipped window moved right to the upper boundary of TSM pass window; */
                    if (slPhase + offset + ssPhase > passWindow + hysteresis
                        || ((slPhase + offset > passWindow + hysteresis) && (offset > 0)))
                    {
                        /* this will force TSM to skip a field, i.e. the sync-locked window (TSM) slips from
                        the SECOND_VYSNC_SLOT to the FIRST_VSYNC_SLOT, while the sync-slipped
                        window not only follows along from the right boundary of the TSM PASS WINDOW,
                        but also its multi-buffer will also skip 1 more field because the source slips 1-field 
                        so that the sync-slipped window delay becomes to the right of the leftmost 
                        boundary of the FIRST_VSYNC_SLOT; 
                        from now upon, sync-locked window lags behind the sync-slipped window; */
                        offset -= sourceVsyncPeriod;
                    }
                }

                syncChannel->vsourcePhaseOffset = offset;
            }
            else
            {
                syncChannel->vsourcePhaseOffset = 0;
            }
            
            BDBG_MSG(("[%p] offset: %d", syncChannel, syncChannel->vsourcePhaseOffset));
        }

        switch (status.frameRate)
        {
            case BAVC_FrameRateCode_e23_976:  /* 23.976 */
            case BAVC_FrameRateCode_e24:          /* 24 */
                eDelayUnits = BSYNClib_Units_e24HzVsyncs;
                break;

            case BAVC_FrameRateCode_e25:          /* 25 */
            case BAVC_FrameRateCode_e50:          /* 50 */
                eDelayUnits = BSYNClib_Units_e50HzVsyncs;
                break;

            case BAVC_FrameRateCode_e29_97:       /* 29.97 */
            case BAVC_FrameRateCode_e30:          /* 30 */
            case BAVC_FrameRateCode_e59_94:       /* 59.94 */
            case BAVC_FrameRateCode_e60:          /* 60 */
            case BAVC_FrameRateCode_eUnknown: /* Unknown */
            default:
                eDelayUnits = BSYNClib_Units_e60HzVsyncs;
                break;
        }

        BSYNClib_Channel_GetVideoSinkStatus_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkStatus);
        BSYNClib_Channel_GetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);

        /* only call into synclib if value or units changes, as this will cause a readjustment and reschedule unmute timers */
        if ((vsinkConfig.sDelay.sMeasured.uiValue != (unsigned)status.delay) 
           || (vsinkConfig.sDelay.sMeasured.eUnits != eDelayUnits)
           || !vsinkStatus.bDelayReceived)
        {
            vsinkConfig.sDelay.bReceived = true;
            vsinkConfig.sDelay.sMeasured.uiValue = (unsigned)status.delay;
            vsinkConfig.sDelay.sMeasured.eUnits = eDelayUnits;
            BSYNClib_Channel_SetVideoSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &vsinkConfig);
        }

        /* force a task-time offset re-apply */
        BKNI_SetEvent(syncChannel->phaseOffsetAdjustmentEvent);

        /* update status */
        BSYNClib_ConvertSigned_isr(syncChannel->vsourcePhaseOffset, BSYNClib_Units_e45KhzTicks, 
            BSYNClib_Units_eMilliseconds, &syncChannel->status.video.source.delay.phase);
        syncChannel->status.video.sinks[syncContext->index].delay.phase = status.phaseDelay / 1000;
        if (status.refreshRate)
        {
            syncChannel->status.video.sinks[syncContext->index].delay.measured = 
                1000 * 1000 * status.delay / ((int)status.refreshRate);
        }
    }
    else
    {
        BDBG_WRN(("[%p] VideoOutputDelay event received for NULL output %d", syncContext->syncChannel, syncContext->index));
    }
}

static void NEXUS_SyncChannel_P_VideoInputConnectionChangedEventHandler(void * context)
{
    NEXUS_SyncChannelHandle syncChannel = context;
    BSYNClib_Channel_Config synclibConfig;
    NEXUS_VideoWindowSyncStatus windowStatus;
    unsigned i = 0;
    unsigned num;

    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    BDBG_MSG(("[%p] Video input connection changed", syncChannel));

    BDBG_MSG(("[%p] Disabling synclib", syncChannel));
    BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
    synclibConfig.bEnabled = false;
    BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);
    BDBG_MSG(("[%p] Synclib disabled", syncChannel));

    /* disconnect all old outputs */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        if (syncChannel->outputs.window[i])
        {
            NEXUS_SyncChannel_P_DisconnectVideoOutput(syncChannel, i);
            NEXUS_Synclib_P_SetVideoSink(syncChannel, &syncChannel->settings, i);
        }
    }

    /* clear outputs on each input connection change */
    BKNI_Memset(syncChannel->outputs.window, 0, sizeof(syncChannel->outputs.window));

    /* re-discover outputs */
    NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
    NEXUS_Display_P_GetWindows_priv(syncChannel->settings.videoInput, syncChannel->outputs.window, NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS, &num);
    NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

    /* reconnect and repopulate config for all new outputs */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        if (syncChannel->outputs.window[i])
        {
            NEXUS_SyncChannel_P_ConnectVideoOutput(syncChannel, &syncChannel->settings, i);
            NEXUS_Synclib_P_SetVideoSink(syncChannel, &syncChannel->settings, i);
            
            BKNI_EnterCriticalSection();

            /* mark sink whose new status has been gathered as finished with the change */
            /* this must be done prior to retrieving updated state info, otherwise it won't update */
            syncChannel->vsinkContext[i].connectionChanged = false;

            /* format and current state are always valid */
            NEXUS_Synclib_P_VideoOutputStateChangeCallback_isr(&syncChannel->vsinkContext[i], 0);
            NEXUS_Synclib_P_VideoOutputFormatCallback_isr(&syncChannel->vsinkContext[i], 0);

            /* delay, we have to check */
            NEXUS_VideoWindow_GetSyncStatus_isr(syncChannel->outputs.window[i], &windowStatus);
            if (windowStatus.delayValid)
            {
                NEXUS_Synclib_P_VideoOutputDelayCallback_isr(&syncChannel->vsinkContext[i], 0);
            }
            BKNI_LeaveCriticalSection();
        }
    }

    BDBG_MSG(("[%p] Enabling synclib", syncChannel));
    BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
    synclibConfig.bEnabled = true;
    BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);
    BDBG_MSG(("[%p] Synclib enabled", syncChannel));
}

/**********************
*
* SYNClib AudioSource = NEXUS AudioInput, which can be AudioDecoder, analog, etc.
*
***********************/

static BERR_Code
NEXUS_Synclib_P_SetAudioSourceMute(void * pvParm1, int iParm2, unsigned index, bool bMute)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_AudioInputSyncSettings syncSettings;

    BSTD_UNUSED(iParm2);

    if (syncChannel->settings.audioInput[index])
    {
        BDBG_MSG(("[%p] SetAudioSourceMute(%u): %s", syncChannel, index, bMute ? "muted" : "unmuted"));
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioInput_GetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        syncSettings.mute = bMute;
        rc = NEXUS_AudioInput_SetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);

        /* update status */
        syncChannel->status.audio.sources[index].muted = syncSettings.mute;
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set audio source mute with NULL input %d", syncChannel, index));
    }

    return rc;
}

static BERR_Code
NEXUS_Synclib_P_SetAudioSourceDelay(void * pvParm1, int iParm2, unsigned int index, BSYNClib_UnsignedValue * pDelay)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_AudioInputSyncSettings syncSettings;

    BSTD_UNUSED(iParm2);

    if (syncChannel->settings.audioInput[index])
    {
        BDBG_MSG(("[%p] SetAudioSourceDelay(%u): %u %s", syncChannel, index, pDelay->uiValue, BSYNClib_P_UnitsStrings[pDelay->eUnits]));
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioInput_GetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        BDBG_ASSERT(pDelay->eUnits == BSYNClib_Units_e45KhzTicks);
        syncSettings.delay = pDelay->uiValue;
        rc = NEXUS_AudioInput_SetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);

        /* update status */
        BSYNClib_ConvertSigned(syncSettings.delay, pDelay->eUnits, 
            BSYNClib_Units_eMilliseconds, &syncChannel->status.audio.sources[index].delay.applied);
    }
    else
    {
        BDBG_MSG(("[%p] Synclib called set audio source delay with NULL input %d", syncChannel, index));
    }

    return rc;
}

#define NEXUS_SYNC_CHANNEL_MAX_SUPPORTED_AUDIO_DELAY 144

static void
NEXUS_Synclib_P_AudioInputStartCallback_isr(void *context, int param)
{
    BSYNClib_AudioSource_Config asourceConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_AudioInputSyncStatus status;

    BSTD_UNUSED(param);

    if (syncContext->syncChannel->settings.audioInput[syncContext->index])
    {
        NEXUS_AudioInput_GetSyncStatus_isr(syncContext->syncChannel->settings.audioInput[syncContext->index], &status);
        BDBG_MSG(("[%p] AudioInputStart(%u) started=%d, digital=%d delay=%u", syncContext->syncChannel, syncContext->index, status.started, status.digital, status.delay));
        BSYNClib_Channel_GetAudioSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asourceConfig);
        asourceConfig.bStarted = status.started;
        asourceConfig.bDigital = status.digital;
        if (status.started)
        {
            asourceConfig.sDelay.bReceived = true;
            asourceConfig.sDelay.sMeasured.eUnits = BSYNClib_Units_eMilliseconds;
            if (status.delay > NEXUS_SYNC_CHANNEL_MAX_SUPPORTED_AUDIO_DELAY)
            {
                BDBG_ERR(("[%p] Audio software is returning an invalid decoder delay value: %u", syncContext->syncChannel, status.delay ));
                status.delay = NEXUS_SYNC_CHANNEL_MAX_SUPPORTED_AUDIO_DELAY;
            }
            asourceConfig.sDelay.sMeasured.uiValue = status.delay;
#if NEXUS_SYNC_CHANNEL_SARNOFF_LIPSYNC_OFFSET_SUPPORT
            BDBG_MSG(("[%p] Sarnoff 8 ms lipsync offset enabled", syncContext->syncChannel));
            asourceConfig.sDelay.sMeasured.uiValue += 8;
#else
            BDBG_MSG(("[%p] Sarnoff 8 ms lipsync offset disabled", syncContext->syncChannel));
#endif
            asourceConfig.bSamplingRateReceived = false;
        }
        else
        {
            asourceConfig.sDelay.bReceived = false;
        }
        BSYNClib_Channel_SetAudioSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asourceConfig);

        /* PR49294 need to ignore callback in av trick modes */
        syncContext->syncChannel->avTrickModeEnabled = status.dsolaEnabled;

        /* update status */
        syncContext->syncChannel->status.audio.sources[syncContext->index].samplingRateReceived = asourceConfig.bSamplingRateReceived;
        syncContext->syncChannel->status.audio.sources[syncContext->index].started = asourceConfig.bStarted;
        syncContext->syncChannel->status.audio.sources[syncContext->index].digital = asourceConfig.bDigital;
        BSYNClib_ConvertSigned_isr(asourceConfig.sDelay.sMeasured.uiValue, asourceConfig.sDelay.sMeasured.eUnits, 
            BSYNClib_Units_eMilliseconds, &syncContext->syncChannel->status.audio.sources[syncContext->index].delay.measured);
        syncContext->syncChannel->status.audio.sources[syncContext->index].delay.applied = -1;
    }
    else
    {
        BDBG_WRN(("[%p] AudioInputStartStop event received for NULL input %d", syncContext->syncChannel, syncContext->index));
    }
}

static void
NEXUS_Synclib_P_AudioInputSampleRateCallback_isr(void *context, int samplingRate)
{
    BSYNClib_AudioSource_Config asourceConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;

    BSTD_UNUSED(samplingRate); /* SYNClib doesn't need to know the value */
    /* TODO: account for 64 sample DRAM FIFO latency in audio FMM (currently accounted in nexus_audio_decoder delay) */

    if (syncContext->syncChannel->settings.audioInput[syncContext->index])
    {
        BDBG_MSG(("[%p] AudioInputSampleRate(%u)", syncContext->syncChannel, syncContext->index));
        BSYNClib_Channel_GetAudioSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asourceConfig);
        asourceConfig.bSamplingRateReceived = true;
        BSYNClib_Channel_SetAudioSourceConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asourceConfig);

        /* update status */
        syncContext->syncChannel->status.audio.sources[syncContext->index].samplingRateReceived = asourceConfig.bSamplingRateReceived;
    }
    else
    {
        BDBG_WRN(("[%p] AudioInputSampleRate event received for NULL input %d", syncContext->syncChannel, syncContext->index));
    }
}

/**********************
*
* AudioSink = AudioOutput
*
***********************/

static BERR_Code
NEXUS_Synclib_P_SetAudioSinkDelay(void * pvParm1, int iParm2, unsigned int index, BSYNClib_UnsignedValue * pDelay)
{
    NEXUS_SyncChannelHandle syncChannel = (NEXUS_SyncChannelHandle)pvParm1;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_AudioOutputSyncSettings syncSettings;

    BDBG_MSG(("[%p] SetAudioSinkDelay(%u): %u %s", syncChannel, index, pDelay->uiValue, BSYNClib_P_UnitsStrings[pDelay->eUnits]));
    BSTD_UNUSED(iParm2);

    /* TODO: handle analog */

#if 0
    /* TODO: implement independent output delay */
    NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
    NEXUS_AudioOutput_GetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
    syncSettings.delay = pDelay->uiValue;
    rc = NEXUS_AudioOutput_SetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
    NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
#else
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(syncSettings);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pDelay);
#endif
    return rc;
}

#if 0
static void
NEXUS_Synclib_P_AudioOutputStartCallback_isr(void *context, int param)
{
    BSYNClib_AudioSink_Config asinkConfig;
    struct NEXUS_SyncChannelContext *syncContext = (struct NEXUS_SyncChannelContext *)context;
    NEXUS_AudioOutputSyncStatus status;

    BSTD_UNUSED(param);

    if (syncContext->syncChannel->outputs.audioOutput[syncContext->index])
    {
        NEXUS_AudioOutput_GetSyncStatus_isr(syncContext->syncChannel->outputs.audioOutput[syncContext->index], &status);
        BDBG_MSG(("AudioOutputStart(%u) started=%d, compressed=%d", syncContext->index, status.started, status.compressed));
        BSYNClib_Channel_GetAudioSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asinkConfig);
        /* TODO: remove status.started */
        asinkConfig.bCompressed = status.compressed;
        BSYNClib_Channel_SetAudioSinkConfig_isr(syncContext->syncChannel->synclibChannel, syncContext->index, &asinkConfig);
    }
    else
    {
        BDBG_MSG(("AudioOutputStartStop event received for NULL output %d", syncContext->index));
    }
}
#endif

/**********************
*
* Main functions
*
***********************/

static void NEXUS_SyncChannel_P_GetDefaultStatus(NEXUS_SyncChannelStatus * pStatus)
{
    unsigned i;

    BDBG_ASSERT(pStatus);
    
    BKNI_Memset(pStatus, 0, sizeof(NEXUS_SyncChannelStatus));
    pStatus->displaySync = NEXUS_SyncChannelState_eAcquiring;
    pStatus->audioOutputSync = NEXUS_SyncChannelState_eAcquiring;
    pStatus->avSync = NEXUS_SyncChannelState_eAcquiring;
    pStatus->video.source.delay.applied = -1;
    pStatus->video.source.delay.measured = -1;
    pStatus->video.source.delay.custom = -1;
    pStatus->video.source.delay.notificationThreshold = -1;
    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        pStatus->video.sinks[i].delay.applied = -1;
        pStatus->video.sinks[i].delay.measured = -1;
        pStatus->video.sinks[i].delay.custom = -1;
        pStatus->video.sinks[i].delay.notificationThreshold = -1;
    }
    pStatus->video.syncLockedWindow = -1;
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
    {
        pStatus->audio.sources[i].delay.applied = -1;
        pStatus->audio.sources[i].delay.measured = -1;
        pStatus->audio.sources[i].delay.custom = -1;
    }
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS; i++)
    {
        pStatus->audio.sinks[i].delay.applied = -1;
        pStatus->audio.sinks[i].delay.measured = -1;
        pStatus->audio.sinks[i].delay.custom = -1;
    }
}

NEXUS_SyncChannelHandle NEXUS_SyncChannel_Create(const NEXUS_SyncChannelSettings *pSettings)
{
    BSYNClib_Channel_Settings channelSettings;
    BSYNClib_Channel_Config synclibConfig;
    NEXUS_SyncChannelHandle syncChannel;
    NEXUS_SyncChannelSettings settings;
    BERR_Code rc;

    syncChannel = BKNI_Malloc(sizeof(*syncChannel));
    if (!syncChannel) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_SyncChannel, syncChannel);

    BKNI_CreateEvent(&syncChannel->phaseOffsetAdjustmentEvent);
    syncChannel->phaseOffsetAdjustmentEventCallback = 
        NEXUS_RegisterEvent(syncChannel->phaseOffsetAdjustmentEvent, 
            &NEXUS_SyncChannel_P_PhaseOffsetAdjustmentEventHandler, 
            syncChannel);

    BKNI_CreateEvent(&syncChannel->videoInputConnectionChangedEvent);
    syncChannel->videoInputConnectionChangedCallback = 
        NEXUS_RegisterEvent(syncChannel->videoInputConnectionChangedEvent, 
            &NEXUS_SyncChannel_P_VideoInputConnectionChangedEventHandler, 
            syncChannel);

    BSYNClib_GetChannelDefaultSettings(&channelSettings);
    NEXUS_SyncChannel_P_GetDefaultStatus(&syncChannel->status);

    /* map syslib/framework functions to base */
    channelSettings.cbTimer.pfCreate = NEXUS_SYSlib_P_CreateTimer;
    channelSettings.cbTimer.pfDestroy = NEXUS_SYSlib_P_DestroyTimer;
    channelSettings.cbTimer.pfStart_isr = NEXUS_SYSlib_P_StartTimer_isr;
    channelSettings.cbTimer.pfCancel_isr = NEXUS_SYSlib_P_CancelTimer_isr;
    channelSettings.cbTimer.pvParm1 = g_NEXUS_syncChannel.syslibContext;
    channelSettings.cbTimer.iParm2 = 0; /* unused */
    channelSettings.cbTime.pfGetTime_isr = NEXUS_SYSlib_P_GetTime_isr;
    channelSettings.cbTime.pvParm1 = g_NEXUS_syncChannel.syslibContext;
    channelSettings.cbTime.iParm2 = 0; /* unused */

    /* TODO: move these to SetSettings / Synclib_P_Connect fns */
    /* SYNClib a/v callbacks. They are all task time. You cannot make any change to SYNClib during these callbacks. */
    channelSettings.sVideo.sSource.cbMute.pfSetMute = NEXUS_Synclib_P_SetVideoSourceMute;
    channelSettings.sVideo.sSource.cbMute.pvParm1 = syncChannel;
    channelSettings.sVideo.sSource.cbMute.iParm2 = 0; /* unused */
    channelSettings.sVideo.sSource.cbDelay.pfSetDelay = NEXUS_Synclib_P_SetVideoSourceDelay;
    channelSettings.sVideo.sSource.cbDelay.pfSetDelayNotification = NEXUS_Synclib_P_SetVideoSourceDelayNotification;
    channelSettings.sVideo.sSource.cbDelay.pvParm1 = syncChannel;
    channelSettings.sVideo.sSource.cbDelay.iParm2 = 0; /* unused */
    channelSettings.sVideo.sSink.cbDelay.pfSetDelay = NEXUS_Synclib_P_SetVideoSinkDelay;
    channelSettings.sVideo.sSink.cbDelay.pvParm1 = syncChannel;
    channelSettings.sVideo.sSink.cbDelay.iParm2 = 0; /* unused */

    channelSettings.sAudio.sSource.cbMute.pfSetMute = NEXUS_Synclib_P_SetAudioSourceMute;
    channelSettings.sAudio.sSource.cbMute.pvParm1 = syncChannel;
    channelSettings.sAudio.sSource.cbMute.iParm2 = 0; /* unused */
    channelSettings.sAudio.sSource.cbDelay.pfSetDelay = NEXUS_Synclib_P_SetAudioSourceDelay;
    channelSettings.sAudio.sSource.cbDelay.preferredDelayUnits = BSYNClib_Units_e45KhzTicks;
    channelSettings.sAudio.sSource.cbDelay.pvParm1 = syncChannel;
    channelSettings.sAudio.sSource.cbDelay.iParm2 = 0; /* unused */
    channelSettings.sAudio.sSink.cbDelay.pfSetDelay = NEXUS_Synclib_P_SetAudioSinkDelay;
    channelSettings.sAudio.sSink.cbDelay.pvParm1 = syncChannel;
    channelSettings.sAudio.sSink.cbDelay.iParm2 = 0; /* unused */

    rc = BSYNClib_CreateChannel(g_NEXUS_syncChannel.synclib, &channelSettings, &syncChannel->synclibChannel);
    if (rc) {BERR_TRACE(rc); goto error;}

    /* do this once at create time */
    BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
    synclibConfig.uiVideoSourceCount = NEXUS_SYNC_CHANNEL_VIDEO_INPUTS;
    synclibConfig.uiVideoSinkCount = NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS;
    synclibConfig.uiAudioSourceCount = NEXUS_SYNC_CHANNEL_AUDIO_INPUTS;
    synclibConfig.uiAudioSinkCount = NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS;
    BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);

    if (pSettings == NULL)
    {
        NEXUS_SyncChannel_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    rc = NEXUS_SyncChannel_SetSettings(syncChannel, pSettings);
    if (rc) {BERR_TRACE(rc); goto error;}

    return syncChannel;
error:
    NEXUS_SyncChannel_Destroy(syncChannel);
    return NULL;
}

static void NEXUS_SyncChannel_P_Finalizer(NEXUS_SyncChannelHandle syncChannel)
{
    NEXUS_SyncChannelSettings settings;
    unsigned i;

    NEXUS_OBJECT_ASSERT(NEXUS_SyncChannel, syncChannel);

    NEXUS_UnregisterEvent(syncChannel->videoInputConnectionChangedCallback);
    syncChannel->videoInputConnectionChangedCallback = NULL;
    BKNI_DestroyEvent(syncChannel->videoInputConnectionChangedEvent);
    syncChannel->videoInputConnectionChangedEvent = NULL;

    NEXUS_UnregisterEvent(syncChannel->phaseOffsetAdjustmentEventCallback);
    syncChannel->phaseOffsetAdjustmentEventCallback = NULL;
    BKNI_DestroyEvent(syncChannel->phaseOffsetAdjustmentEvent);
    syncChannel->phaseOffsetAdjustmentEvent = NULL;

    NEXUS_SyncChannel_GetSettings(syncChannel, &settings);
    settings.videoInput = NULL;
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
    {
        settings.audioInput[i] = NULL;
    }
    NEXUS_SyncChannel_SetSettings(syncChannel, &settings);

    BSYNClib_DestroyChannel(g_NEXUS_syncChannel.synclib, syncChannel->synclibChannel);

    NEXUS_OBJECT_DESTROY(NEXUS_SyncChannel, syncChannel);
    BKNI_Free(syncChannel);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_SyncChannel, NEXUS_SyncChannel_Destroy);

void NEXUS_SyncChannel_GetSettings(NEXUS_SyncChannelHandle syncChannel, NEXUS_SyncChannelSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);
    *pSettings = syncChannel->settings;
}

NEXUS_Error NEXUS_SyncChannel_GetStatus(NEXUS_SyncChannelHandle syncChannel, NEXUS_SyncChannelStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);
    
    *pStatus = syncChannel->status;

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Synclib_P_SetVideoSource(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    NEXUS_VideoInputSyncStatus status;
    BSYNClib_VideoSource_Config vsourceConfig;

    BKNI_Memset(&status, 0, sizeof(NEXUS_VideoInputSyncStatus));

    BKNI_EnterCriticalSection();
    if (pSettings->videoInput)
    {
        NEXUS_VideoInput_GetSyncStatus_isr(pSettings->videoInput, &status);
    }
    BSYNClib_Channel_GetVideoSourceConfig_isr(syncChannel->synclibChannel, index, &vsourceConfig);
    vsourceConfig.bSynchronize = (pSettings->videoInput != NULL);
    vsourceConfig.sJitterToleranceImprovementThreshold.uiValue = pSettings->adjustmentThreshold;
    /* status will be zero'd by memset above if no input is available */
    vsourceConfig.sDelay.sCustom.uiValue = NEXUS_Synclib_P_ComputeVideoSourceCustomDelay_isr(syncChannel, &status);
    BSYNClib_Channel_SetVideoSourceConfig_isr(syncChannel->synclibChannel, index, &vsourceConfig);
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("[%p] vsrc[%d] = %d", syncChannel, index, vsourceConfig.bSynchronize));

    /* update status */
    syncChannel->status.video.source.synchronized = vsourceConfig.bSynchronize;
    syncChannel->status.video.source.delay.custom = vsourceConfig.sDelay.sCustom.uiValue;

    return 0;
}

NEXUS_Error NEXUS_Synclib_P_ConnectVideoSource(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_DisconnectVideoSource(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_SetVideoSink(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSYNClib_VideoSink_Config vsinkConfig;

    BSTD_UNUSED(pSettings); /* for now */

    BKNI_EnterCriticalSection();
    BSYNClib_Channel_GetVideoSinkConfig_isr(syncChannel->synclibChannel, index, &vsinkConfig);
    vsinkConfig.bSynchronize = (syncChannel->outputs.window[index] != NULL);
    BSYNClib_Channel_SetVideoSinkConfig_isr(syncChannel->synclibChannel, index, &vsinkConfig);
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("[%p] vsink[%d] = %d", syncChannel, index, vsinkConfig.bSynchronize));

    /* update status */
    syncChannel->status.video.sinks[index].synchronized = vsinkConfig.bSynchronize;
    
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_ConnectVideoSink(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_DisconnectVideoSink(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_SetAudioSource(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BSYNClib_AudioSource_Config asourceConfig;

    BKNI_EnterCriticalSection();
    BSYNClib_Channel_GetAudioSourceConfig_isr(syncChannel->synclibChannel, index, &asourceConfig);
    asourceConfig.bSynchronize = (pSettings->audioInput[index] != NULL);
    asourceConfig.sDelay.sCustom.uiValue = pSettings->customAudioDelay[index];
    BSYNClib_Channel_SetAudioSourceConfig_isr(syncChannel->synclibChannel, index, &asourceConfig);
    BKNI_LeaveCriticalSection();

    /* update status */
    syncChannel->status.audio.sources[index].synchronized = asourceConfig.bSynchronize;
    syncChannel->status.audio.sources[index].delay.custom = asourceConfig.sDelay.sCustom.uiValue;

#if 0
    if (!asourceConfig.bSynchronize) continue;
#endif

    return rc;
}

NEXUS_Error NEXUS_Synclib_P_ConnectAudioSource(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_DisconnectAudioSource(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_SetAudioSink(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSYNClib_AudioSink_Config asinkConfig;

    BSTD_UNUSED(pSettings);

    BKNI_EnterCriticalSection();
    BSYNClib_Channel_GetAudioSinkConfig_isr(syncChannel->synclibChannel, index, &asinkConfig);
#if 0
    asinkConfig.bSynchronize = (syncChannel->outputs.audioOutput[i] != NULL);
#else
    asinkConfig.bSynchronize = index == 0;
#endif
    asinkConfig.sDelay.bReceived = true;
    asinkConfig.sDelay.sMeasured.uiValue = 0; /* hardcoded */
    asinkConfig.sDelay.sMeasured.eUnits = BSYNClib_Units_eMilliseconds;
    asinkConfig.bCompressed = false; /* TODO */
    BSYNClib_Channel_SetAudioSinkConfig_isr(syncChannel->synclibChannel, index, &asinkConfig);
    BKNI_LeaveCriticalSection();

    /* update status */
    syncChannel->status.audio.sinks[index].synchronized = asinkConfig.bSynchronize;
    syncChannel->status.audio.sinks[index].compressed = asinkConfig.bCompressed;

#if 0 /* really necessary? */
    if (!asinkConfig.bSynchronize) continue;
#endif
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_ConnectAudioSink(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_Synclib_P_DisconnectAudioSink(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(index);
    return 0;
}

NEXUS_Error NEXUS_SyncChannel_P_ConnectVideoInput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings)
{
    unsigned int i = 0;
    unsigned num;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoInputSyncSettings syncSettings;

    if (pSettings->videoInput) /* check new settings for input */
    {
        BDBG_MSG(("[%p] Connecting video input: %p", syncChannel, pSettings->videoInput));

        /* TODO: move context set up to create */
        /* set up context */
        syncChannel->vsourceContext.syncChannel = syncChannel;
        syncChannel->vsourceContext.index = 0;

        /* set up callbacks */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoInput_GetSyncSettings_priv(pSettings->videoInput, &syncSettings);
        syncSettings.delayCallback_isr = NEXUS_Synclib_P_VideoInputDelayCallback_isr;
        syncSettings.startCallback_isr = NEXUS_Synclib_P_VideoInputStartCallback_isr;
        syncSettings.formatCallback_isr = NEXUS_Synclib_P_VideoInputFormatCallback_isr;
        syncSettings.connectionChangedCallback_isr = NEXUS_Synclib_P_VideoInputConnectionChangedCallback_isr;
        syncSettings.callbackContext = &syncChannel->vsourceContext;
        syncSettings.mute = syncChannel->settings.enableMuteControl;
        rc = NEXUS_VideoInput_SetSyncSettings_priv(pSettings->videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* clear outputs on each input connection */
        BKNI_Memset(syncChannel->outputs.window, 0, sizeof(syncChannel->outputs.window));

        /* discover outputs */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_Display_P_GetWindows_priv(pSettings->videoInput, syncChannel->outputs.window, NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS, &num);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);

        /* connect all discovered video outputs for this input */
        for (i = 0; i < num; i++)
        {
#if 0 /* synclib connections are all done in create right now */
            rc = NEXUS_Synclib_P_ConnectVideoSink(syncChannel, pSettings, i);
            if (rc) goto end;
#endif
            rc = NEXUS_SyncChannel_P_ConnectVideoOutput(syncChannel, pSettings, i);
            if (rc) goto end;
        }
    }

end:

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_DisconnectVideoInput(NEXUS_SyncChannelHandle syncChannel)
{
    unsigned int i = 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoInputSyncSettings syncSettings;

    if (syncChannel->settings.videoInput) /* check current settings for input */
    {
        BDBG_MSG(("[%p] Disconnecting video input: %p", syncChannel, syncChannel->settings.videoInput));

        /* need to disconnect all outputs first */
        for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
        {
            rc = NEXUS_SyncChannel_P_DisconnectVideoOutput(syncChannel, i);
            if (rc) goto end;
#if 0 /* synclib connections are all done in create right now */
            rc = NEXUS_Synclib_P_DisconnectVideoSink(syncChannel, pSettings, i);
            if (rc) goto end;
#endif
        }

        /* tear down callbacks; reset mute and delay */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoInput_GetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        syncSettings.delayCallback_isr  = NULL;
        syncSettings.startCallback_isr  = NULL;
        syncSettings.connectionChangedCallback_isr  = NULL;
        syncSettings.formatCallback_isr = NULL;
        syncSettings.callbackContext = NULL;
        syncSettings.mute = false;
        NEXUS_VideoInput_SetSyncSettings_priv(syncChannel->settings.videoInput, &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);
    }

end:

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_ConnectVideoOutput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoWindowSyncSettings syncSettings;

    BSTD_UNUSED(pSettings); /* for now */

    if (syncChannel->outputs.window[index]) /* check current outputs */
    {
        BDBG_MSG(("[%p] Connecting video output: %p", syncChannel, syncChannel->outputs.window[index]));

        /* TODO: move context set up to create */
        /* set up context */
        syncChannel->vsinkContext[index].syncChannel = syncChannel;
        syncChannel->vsinkContext[index].index = index;

        /* set up callbacks */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoWindow_GetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        syncSettings.delayCallback_isr = NEXUS_Synclib_P_VideoOutputDelayCallback_isr;
        syncSettings.stateChangeCallback_isr = NEXUS_Synclib_P_VideoOutputStateChangeCallback_isr;
        syncSettings.formatCallback_isr = NEXUS_Synclib_P_VideoOutputFormatCallback_isr;
        syncSettings.callbackContext = &syncChannel->vsinkContext[index];
        rc = NEXUS_VideoWindow_SetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);
    }

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_DisconnectVideoOutput(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_VideoWindowSyncSettings syncSettings;

    if (syncChannel->outputs.window[index]) /* check current outputs */
    {
        BDBG_MSG(("[%p] Disconnecting video output: %p", syncChannel, syncChannel->outputs.window[index]));

        /* tear down callbacks, reset delay */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
        NEXUS_VideoWindow_GetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        syncSettings.delayCallback_isr = NULL;
        syncSettings.stateChangeCallback_isr = NULL;
        syncSettings.formatCallback_isr = NULL;
        syncSettings.callbackContext = NULL;
        rc = NEXUS_VideoWindow_SetSyncSettings_priv(syncChannel->outputs.window[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);
    }

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_ConnectAudioInput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    unsigned int i = 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_AudioInputSyncSettings syncSettings;

    if (pSettings->audioInput[index])
    {
        BDBG_MSG(("[%p] Connecting audio input: %p", syncChannel, pSettings->audioInput[index]));

        /* TODO: move context set up to create */
        /* set up context */
        syncChannel->asourceContext[index].syncChannel = syncChannel;
        syncChannel->asourceContext[index].index = index;

        /* set up callbacks */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioInput_GetSyncSettings_priv(pSettings->audioInput[index], &syncSettings);
        syncSettings.startCallback_isr = NEXUS_Synclib_P_AudioInputStartCallback_isr;
        syncSettings.sampleRateCallback_isr = NEXUS_Synclib_P_AudioInputSampleRateCallback_isr;
        syncSettings.callbackContext = &syncChannel->asourceContext[index];
        syncSettings.mute = syncChannel->settings.enableMuteControl;
        rc = NEXUS_AudioInput_SetSyncSettings_priv(pSettings->audioInput[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);

        /* clear outputs */
        BKNI_Memset(syncChannel->outputs.audioOutput, 0, sizeof(syncChannel->outputs.audioOutput));
#if 0
            /* discover outputs */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        num = 0;
        if (pSettings->audioInput[0]) {
            NEXUS_Audio_P_GetOutputs_priv(pSettings->audioInput[0], syncChannel->outputs.audioOutput, NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS, &num);
        }
        if (pSettings->audioInput[1] && num < NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS) {
            NEXUS_Audio_P_GetOutputs_priv(pSettings->audioInput[1], &syncChannel->outputs.audioOutput[num], NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS-num, &num);
            /* TODO: pack list. unlikely, but there could be dups because of mixing. */
        }
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
#else
            /* discover outputs */
        syncChannel->outputs.audioOutput[0] = (NEXUS_AudioOutput)1;
#endif

        /* connect all discovered audio outputs for this input */
        for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS; i++)
        {
#if 0 /* synclib connections are all done in create right now */
            rc = NEXUS_Synclib_P_ConnectAudioSink(syncChannel, pSettings, i);
            if (rc) goto end;
#endif

            rc = NEXUS_SyncChannel_P_ConnectAudioOutput(syncChannel, pSettings, i);
            if (rc) goto end;
        }
    }

end:

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_DisconnectAudioInput(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    unsigned int i = 0;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_AudioInputSyncSettings syncSettings;

    if (syncChannel->settings.audioInput[index])
    {
        BDBG_MSG(("[%p] Disconnecting audio input: %p", syncChannel, syncChannel->settings.audioInput[index]));

        /* need to disconnect all outputs first */
        for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS; i++)
        {
            rc = NEXUS_SyncChannel_P_DisconnectAudioOutput(syncChannel, i);
            if (rc) goto end;
#if 0 /* synclib connections are all done in create right now */
            rc = NEXUS_Synclib_P_DisconnectAudioSink(syncChannel, i);
            if (rc) goto end;
#endif
        }

        /* tear down callbacks, and reset mute and delay */
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioInput_GetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        syncSettings.startCallback_isr      = NULL;
        syncSettings.sampleRateCallback_isr = NULL;
        syncSettings.callbackContext        = NULL;
        syncSettings.mute = false;
/* 20100727 bandrews - resetting the delay here causes us to be off by 1 frame */
/*        syncSettings.delay = 0;*/
        rc = NEXUS_AudioInput_SetSyncSettings_priv(syncChannel->settings.audioInput[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
    }

end:

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_ConnectAudioOutput(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
/* TODO */
#if 0
    NEXUS_AudioOutputSyncSettings syncSettings;

    BDBG_MSG(("Connecting audio output: %p", syncChannel->outputs.audioOutput[index]));

    if (syncChannel->outputs.audioOutput[index])
    {
        /* TODO: move context set up to create */
        syncChannel->asinkContext[index].syncChannel = syncChannel;
        syncChannel->asinkContext[index].index = index;

        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioOutput_GetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
            syncSettings.startCallback_isr = NEXUS_Synclib_P_AudioOutputStartCallback_isr;
        syncSettings.callbackContext = &syncChannel->asinkContext[index];
        rc = NEXUS_AudioOutput_SetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
            NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
        }
#else
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(index);
    if (syncChannel->outputs.audioOutput[index])
    {
        BDBG_MSG(("[%p] Connecting audio output: %p", syncChannel, syncChannel->outputs.audioOutput[index]));
    }
#endif

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_P_DisconnectAudioOutput(NEXUS_SyncChannelHandle syncChannel, unsigned int index)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
/* TODO */
#if 0
    NEXUS_AudioOutputSyncSettings syncSettings;

    BDBG_MSG(("Disconnecting audio output: %p", syncChannel->outputs.audioOutput[index]));

    if (syncChannel->outputs.audioOutput[index])
    {
        NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
        NEXUS_AudioOutput_GetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
        syncSettings.startCallback_isr = NULL;
        syncSettings.callbackContext = NULL;
        rc = NEXUS_AudioOutput_SetSyncSettings_priv(syncChannel->outputs.audioOutput[index], &syncSettings);
        NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
    }
#else
    BSTD_UNUSED(syncChannel);
    BSTD_UNUSED(index);
    if (syncChannel->outputs.audioOutput[index])
    {
        BDBG_MSG(("[%p] Disconnecting audio output: %p", syncChannel, syncChannel->outputs.audioOutput[index]));
    }
#endif

    return rc;
}

NEXUS_Error NEXUS_SyncChannel_SetSettings(NEXUS_SyncChannelHandle syncChannel, const NEXUS_SyncChannelSettings *pSettings)
{
    BSYNClib_Channel_Config synclibConfig;
    unsigned i;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(syncChannel, NEXUS_SyncChannel);

    if (pSettings == NULL)
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    BDBG_MSG(("[%p] Disabling synclib", syncChannel));
    /* disable */
    BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
    synclibConfig.bEnabled = false;
    BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);
    BDBG_MSG(("[%p] Synclib disabled", syncChannel));

    if ((syncChannel->settings.videoInput && !pSettings->videoInput) /* disconnect old */
        || (syncChannel->settings.videoInput && pSettings->videoInput
        && (syncChannel->settings.videoInput != pSettings->videoInput))) /* change */
    {
        /* disconnect video input */
        rc = NEXUS_SyncChannel_P_DisconnectVideoInput(syncChannel);
        if (rc) goto end;
    }

    if ((!syncChannel->settings.videoInput && pSettings->videoInput) /* connect new */
        || (syncChannel->settings.videoInput && pSettings->videoInput
        && (syncChannel->settings.videoInput != pSettings->videoInput))) /* change */
    {
        /* connect video input */
        rc = NEXUS_SyncChannel_P_ConnectVideoInput(syncChannel, pSettings);
        if (rc) goto end;
    }

    /* TODO: more external settings coming */
    rc = NEXUS_Synclib_P_SetVideoSource(syncChannel, pSettings, 0);
    if (rc) goto end;

    for (i = 0; i < NEXUS_SYNC_CHANNEL_VIDEO_OUTPUTS; i++)
    {
        rc = NEXUS_Synclib_P_SetVideoSink(syncChannel, pSettings, i);
        if (rc) goto end;
    }

    /* process all disconnections for all audio channels first
    because some outputs may be disconnected from one input
    and connected to another */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
    {
        if ((syncChannel->settings.audioInput[i] && !pSettings->audioInput[i]) /* disconnect old */
            || (syncChannel->settings.audioInput[i] && pSettings->audioInput[i]
            && (syncChannel->settings.audioInput[i] != pSettings->audioInput[i]))) /* change */
        {
            /* disconnect audio input i */
            rc = NEXUS_SyncChannel_P_DisconnectAudioInput(syncChannel, i);
            if (rc) goto end;
        }
    }

    /* process all connections for all audio channels second */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
    {
        if ((!syncChannel->settings.audioInput[i] && pSettings->audioInput[i]) /* connect new */
            || (syncChannel->settings.audioInput[i] && pSettings->audioInput[i]
            && (syncChannel->settings.audioInput[i] != pSettings->audioInput[i]))) /* change */
        {
            /* connect audio input i */
            rc = NEXUS_SyncChannel_P_ConnectAudioInput(syncChannel, pSettings, i);
            if (rc) goto end;
        }
    }

    /* TODO: more external settings coming */
    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
    {
        rc = NEXUS_Synclib_P_SetAudioSource(syncChannel, pSettings, i);
        if (rc) goto end;
    }

    for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_OUTPUTS; i++)
    {
        rc = NEXUS_Synclib_P_SetAudioSink(syncChannel, pSettings, i);
        if (rc) goto end;
    }

    /* PR49294 20081125 bandrews - added precision lipsync flag */
    if ((pSettings->enablePrecisionLipsync != syncChannel->settings.enablePrecisionLipsync)
        || (pSettings->enableMuteControl != syncChannel->settings.enableMuteControl)
        || (pSettings->simultaneousUnmute != syncChannel->settings.simultaneousUnmute))
    {
        BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
        synclibConfig.bPrecisionLipsyncEnabled = pSettings->enablePrecisionLipsync;
        synclibConfig.sMuteControl.bEnabled = pSettings->enableMuteControl;
        synclibConfig.sMuteControl.bSimultaneousUnmute = pSettings->simultaneousUnmute;
        BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);
    }

    /* if changing mute control, need to tell decoders */
    if (pSettings->enableMuteControl != syncChannel->settings.enableMuteControl)
    {
        NEXUS_VideoInputSyncSettings videoSettings;
        NEXUS_AudioInputSyncSettings audioSettings;

        if (pSettings->videoInput)
        {
            NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.display);
            NEXUS_VideoInput_GetSyncSettings_priv(pSettings->videoInput, &videoSettings);
            videoSettings.mute = pSettings->enableMuteControl;
            rc = NEXUS_VideoInput_SetSyncSettings_priv(pSettings->videoInput, &videoSettings);
            NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.display);
        }

        for (i = 0; i < NEXUS_SYNC_CHANNEL_AUDIO_INPUTS; i++)
        {
            if (pSettings->audioInput[i])
            {
                NEXUS_Module_Lock(g_NEXUS_syncChannel.settings.modules.audio);
                NEXUS_AudioInput_GetSyncSettings_priv(pSettings->audioInput[i], &audioSettings);
                audioSettings.mute = pSettings->enableMuteControl;
                rc = NEXUS_AudioInput_SetSyncSettings_priv(pSettings->audioInput[i], &audioSettings);
                NEXUS_Module_Unlock(g_NEXUS_syncChannel.settings.modules.audio);
            }
        }
    }

    /* copy settings to internal */
    syncChannel->settings = *pSettings;

    BDBG_MSG(("[%p] Enabling synclib", syncChannel));
    /* enable */
    BSYNClib_Channel_GetConfig(syncChannel->synclibChannel, &synclibConfig);
    synclibConfig.bEnabled = true;
    BSYNClib_Channel_SetConfig(syncChannel->synclibChannel, &synclibConfig);
    BDBG_MSG(("[%p] Synclib enabled", syncChannel));

end:

    return 0;
}

