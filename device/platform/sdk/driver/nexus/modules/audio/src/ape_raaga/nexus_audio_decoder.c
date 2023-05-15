/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_decoder.c $
* $brcm_Revision: SW7425-5116-12.4/1 $
* $brcm_Date: 12/6/13 11:57a $
*
* API Description:
*   API name: AudioDecoder
*    API for audio decoder management.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_decoder.c $
* 
* SW7425-5116-12.4/1   12/6/13 11:57a mpeteri
* SW7425-5116: back-port changes to 12.4
* 
* SW7425-5116/1   12/6/13 11:27a mpeteri
* SW7425-5116: changes to allow 96k output rate from the decoder
*
* 88   10/24/12 10:29a jgarrett
* SW7231-1048: Enabling wide ga threshold
* 
* 87   10/9/12 5:36a jgarrett
* SW7425-4045: Adding no_watchdog env variable similar to RAP nexus
* 
* 86   10/9/12 12:13a jgarrett
* SW7346-914: Adding audio ancillary data support
* 
* 85   9/25/12 2:50p jgarrett
* SW7425-3962: Adding overflow interrupt handling
* 
* 84   9/21/12 3:46p jgarrett
* SW7435-362: Changing timer calls to avoid transport lock
* 
* 83   8/22/12 9:46p bandrews
* SW7420-2314: change mute control to start muted, and unmute immediately
* 
* 82   7/30/12 5:09p jgarrett
* SW7425-3490: Merge to main branch
* 
* 81   7/27/12 4:40p bandrews
* SW7125-1309: fix compile error
* 
* 80   7/27/12 3:10p bandrews
* SW7125-1309: don't enable bandHold for live parser band sources
* 
* 79   7/26/12 4:08p jgarrett
* SW7435-235: Adding NEXUS_HAS_ASTM to support a non-ASTM build
* 
* 78   7/26/12 11:08a jgarrett
* SW7425-3603: Correcting DRC default level handling
* 
* SW7425-3490/1   7/13/12 5:41p jgarrett
* SW7425-3490: Adding initial frame preservation for MS-11 certification
* 
* 77   7/5/12 4:09p jgarrett
* SW7420-2312: Adding stc channel reference counting
* 
* 76   7/5/12 4:00p jgarrett
* SW7420-2311: Adding pid channel reference counting
* 
* 75   7/3/12 6:03p mphillip
* SW7125-1296: Add audio decoder status to proc debug output
* 
* 74   6/26/12 2:15p jgarrett
* SWDEPRECATED-3659: Adding NEXUS_AudioDecoder_GetCapabilities
* 
* 73   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 72   6/14/12 2:35p erickson
* SW7425-2741: add AudioDecoderPrimer
* 
* 71   6/11/12 5:26p jgarrett
* SW7425-2786: Disabling TSM for MLP streams
* 
* 70   6/7/12 11:06a mpeteri
* SW7425-2889: Add support for DTS-CD
* 
* SW7425-2889/1   6/5/12 3:45p mpeteri
* changes for DTS-CD
* 
* 69   5/17/12 7:13p bandrews
* SW7425-3061: fully reset deadlock detection when asked to zero fill
* 
* 68   5/4/12 11:05a jgarrett
* SW7425-2075: Merge to main branch
* 
* SW7425-2075/3   5/3/12 4:03p gskerl
* SW7425-2075: Reworked the handling of audio decoder input format
*  changes.
* 
* SW7425-2075/3   5/3/12 4:00p gskerl
* SW7425-2075: Reworked the handling of audio decoder input format
*  changes.
* 
* SW7425-2075/2   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline updates
* 
* 67   5/1/12 6:22p jgarrett
* SW7425-2690: Adding Audio Capabilities routine
* 
* 66   4/20/12 5:23p jgarrett
* SW7231-774: Resetting MuxOutput on a watchdog interrupt
* 
* SW7425-2075/1   4/16/12 3:46p jgarrett
* SW7425-2075: Adding Compressed 4x/16x connectors
* 
* 65   4/9/12 3:50p jgarrett
* SW7231-662: Adding AIFF support
* 
* 64   3/13/12 6:27p jgarrett
* SW7435-24: Adding programmable DSP index
* 
* 63   3/2/12 6:32p bandrews
* SW7425-2276: add a warning when entering underflow mode
* 
* 62   3/2/12 4:56p bandrews
* SW7425-2276: merge to main
* 
* 61   2/24/12 9:58a jgarrett
* SW7425-2277: Handling NRT underflow internally instead of via a public
*  API
* 
* SW7425-2276/1   2/23/12 9:08p bandrews
* SW7425-2276: initial impl of gap detection
* 
* 60   2/22/12 3:51p bandrews
* SW7552-66: port audio fix to APE platforms
* 
* 59   2/22/12 3:31p jgarrett
* SW7405-5003: Removing BKNI_Memcmp
* 
* 58   2/21/12 6:15p bandrews
* SW7346-684: Sarnoff stream offset handling
* 
* 57   2/21/12 11:25a jgarrett
* SW7425-2277: Adding NRT underflow mode support
* 
* 56   2/14/12 2:46p jgarrett
* SW7425-2314: Zeroing startSettings structure on stop and error cases to
*  avoid attempt to get PidChannelStatus from closed pids.
* 
* 55   2/10/12 2:27p jgarrett
* SW7425-2314: Fixing GA threshold programming when decoder is not yet
*  started
* 
* 54   2/3/12 4:28p jgarrett
* SW7425-2268: Adding delay mode
* 
* 53   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 52   2/1/12 2:09p jgarrett
* SW7425-2310: Removing warnings
* 
* 51   2/1/12 10:30a jgarrett
* SW7425-2310: Correcting handling of locked status in interrupt
*  callbacks
* 
* 50   1/25/12 4:44p bandrews
* SW7346-368: Sarnoff streams require 8 ms offset, plus we add 2 ms for
*  mem->FMM 64 sample FIFO time; this matches behavior from previous
*  chips (7405/7325/etc.)
* 
* 49   1/10/12 11:14a jgarrett
* SW7231-524: Default multichannel format is now 5.1 to avoid confusion
* 
* 48   12/29/11 4:19p jgarrett
* SW7425-1671: Adding dependent frame channel map for Ac3Plus
* 
* 47   12/21/11 4:52p jgarrett
* SW7425-2042: Audio decoder will only report running when actually
*  started.  This resolves issues with attaching a mixer to HDMI/SPDIF
*  inputs
* 
* 46   12/21/11 12:45p jgarrett
* SW7425-1756: Bitrate changes should not fire sourceChagned callback
* 
* 45   12/21/11 12:21p jgarrett
* SW7425-2000: Disabling TSM for OGG source material
* 
* 44   12/20/11 4:18p jgarrett
* SW7425-1018: Propagating loudness equivalence setting to APE
* 
* 43   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 42   12/13/11 9:44p bandrews
* SW7425-1772: merge to main
* 
* SW7425-1772/4   12/12/11 10:18p bandrews
* SW7425-1772: merge from main
* 
* 41   12/9/11 5:51p jgarrett
* SW7425-1756: Adding stream status callback and bitrate/mode change
*  handling
* 
* 40   12/7/11 5:36p jgarrett
* SW7425-1893: Adding MPEG Conformance mode
* 
* 39   11/21/11 11:30a jgarrett
* SW7425-1756: Adding overflow callback
* 
* 38   11/18/11 3:04p jgarrett
* SW7425-1756: Adding PTS callbacks and overflow callback
* 
* 37   11/18/11 2:57p jgarrett
* SW7425-1519: Adding decoder mixing mode settings
* 
* SW7425-1772/3   12/12/11 10:02p bandrews
* SW7425-1772: add priority scheme to stc channel
* 
* SW7425-1772/2   11/17/11 9:19p bandrews
* SW7425-1772: make decoders id themselves to stc channel as index,
*  instead of funky way based on compressed / pcm / input
* 
* SW7425-1772/1   11/17/11 6:31p bandrews
* SW7425-1772: 6 channel support to stc channel
* 
* 36   11/8/11 6:06p jgarrett
* SW7422-400: Adding audio debug log support in kernel mode
* 
* 35   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
* 
* 34   11/2/11 6:26p jgarrett
* SW7422-400: Adding routines for audio DSP debug
* 
* 33   10/25/11 2:50p gskerl
* SW7231-129: Removed call to NEXUS_AudioInput_P_RemoveInput() from
*  NEXUS_AudioDecoder_Start()
* 
* 32   10/10/11 1:42p jgarrett
* SW7425-1457: Ensuring PidChannel is only refernced after
*  NEXUS_AudioDecoder_Start() during ApplySettings()
* 
* 31   10/6/11 6:20p jgarrett
* SW7425-1428: Fixing pause handling
* 
* 30   9/30/11 5:31p jgarrett
* SW7425-1297: Changes for 0.5x..2.0x audio trick modes and automatic
*  muting on unsupported rates
* 
* SW7425-1297/1   9/28/11 6:29p jgarrett
* SW7425-1297: Adding automatic mute on unsupported trick rates
* 
* 29   9/20/11 5:10p jgarrett
* SW7425-1045: Mapping additional settings and status between nexus and
*  the audio FW
* 
* 28   9/9/11 3:20p jgarrett
* SW7552-113: Merge to main branch (original branch was inadvertent)
* 
* 27   9/9/11 10:08a ialauder
* SWDTV-8364: Added call to convert the decoder codec to the nexus codec
*  when callling NEXUS_AudioDecoder_GetStatus().
* 
* SW7425-1212/1   9/9/11 3:12p jgarrett
* SW7552-113: Adding ASTM hooks
* 
* 24   8/15/11 5:01p jgarrett
* SW7346-438: Coverity CID 34705,32711
* 
* 23   8/8/11 6:01p jgarrett
* SW7346-412: Propagating errors from BAPE_GetTsmStatus
* 
* 22   7/29/11 5:34p jgarrett
* SW7346-358: coverity cid 33739
* 
* 21   7/14/11 1:02p jgarrett
* SW7425-406: Fixing TSM Playback setting with no STC channel
* 
* 20   7/1/11 10:54a jgarrett
* SW7405-5072: Adding numWatchdogs and raveIndex status
* 
* 19   6/22/11 11:36a jgarrett
* SW7425-654: Adding NRT hooks
* 
* 18   6/21/11 5:03p jgarrett
* SW7425-654: Adding Audio NRT APIs
* 
* 17   6/9/11 2:56p jgarrett
* SW7425-406: Adding secondary decoder support
* 
* 16   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 15   5/20/11 11:32a jgarrett
* SW7405-5335: Adding numBytesDecoded to NEXUS_AudioDecoderStatus
* 
* 14   5/5/11 6:07p jgarrett
* SW7425-524: Muting decoder on invalid trick modes
* 
* 13   5/4/11 4:04p jgarrett
* SW7425-469: Adding no_watchdog environment variable to disable audio
*  watchdog
* 
* 12   4/26/11 4:23p jgarrett
* SW7425-437: Not allowing dead nodes in the filter graph
* 
* 11   4/20/11 6:16p jtna
* SW7425-365: fix build warnings for 2.6.37 kernel
* 
* 10   4/18/11 10:29p jgarrett
* SW7425-361: Not implicitly forcing audio connection memory to be
*  allocated with sync mute
* 
* 9   3/23/11 6:28p jgarrett
* SW7422-352: adding HDMI input support to nexus
* 
* 8   3/23/11 4:29p jgarrett
* SW7422-403: Fixing sync mute and DSOLA
* 
* 7   3/21/11 6:59p jgarrett
* SW7422-146: Fixing crash on stop with external inputs
* 
* 6   3/11/11 6:02p jgarrett
* SW7422-146: Decoder supports external inputs
* 
* 5   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 4   2/7/11 11:30a jgarrett
* SW7422-146: Implementing DDP -> AC3 conversion and status for
*  MPEG/AAC/AC3
* 
* 3   1/13/11 5:25p jgarrett
* SW7422-146: Adding encoder hooks
* 
* 2   1/12/11 4:25p jgarrett
* SW7422-146: Refactoring data path from decoder
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/

#include "nexus_audio_module.h"
#include "priv/nexus_audio_decoder_priv.h"
#include "priv/nexus_pid_channel_priv.h"
#include "priv/nexus_stc_channel_priv.h"

#define RAAGA_DEBUG_LOG_CHANGES 1
#include "bdsp_raaga.h"

BDBG_MODULE(nexus_audio_decoder);

NEXUS_AudioDecoder g_decoders[NEXUS_NUM_AUDIO_DECODERS];

static void NEXUS_AudioDecoder_P_Watchdog(void *pParam);
static void NEXUS_AudioDecoder_P_SampleRate(void *pParam);
static void NEXUS_AudioDecoder_P_ChannelChangeReport(void * context);
static void NEXUS_AudioDecoder_P_FirstPts_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
static void NEXUS_AudioDecoder_P_AudioTsmFail_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
static void NEXUS_AudioDecoder_P_AudioTsmPass_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
static void NEXUS_AudioDecoder_P_SampleRateChange_isr(void *pParam1, int param2, unsigned sampleRate);
static void NEXUS_AudioDecoder_P_Lock_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_StreamStatusAvailable_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_AncillaryData_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_StreamParameterChanged_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_CdbItbOverflow_isr(void *pParam1, int param2);
static BERR_Code NEXUS_AudioDecoder_P_GetPtsCallback_isr(void *pContext, BAVC_PTSInfo *pPTSInfo);
static BERR_Code NEXUS_AudioDecoder_P_GetCdbLevelCallback_isr(void *pContext, unsigned *pCdbLevel);
static BERR_Code NEXUS_AudioDecoder_P_StcValidCallback_isr(void *pContext);
static void NEXUS_AudioDecoder_P_FifoWatchdog(void *context);
static void NEXUS_AudioDecoder_P_Watchdog_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_InputFormatChange_isr(void *pParam1, int param2);
static void NEXUS_AudioDecoder_P_InputFormatChange(void *pContext);
static BERR_Code NEXUS_AudioDecoder_P_SetPcrOffset_isr(void *pContext, uint32_t pcrOffset);
static BERR_Code NEXUS_AudioDecoder_P_GetPcrOffset_isr(void *pContext, uint32_t *pPcrOffset);

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.transport)

static BKNI_EventHandle g_watchdogEvent;
static NEXUS_EventCallbackHandle g_watchdogCallback;
static unsigned g_numWatchdogs;

static void NEXUS_AudioDecoder_P_Watchdog_isr(void *pParam1, int param2)
{
    BSTD_UNUSED(pParam1);
    BSTD_UNUSED(param2);
    BDBG_ERR(("Audio Watchdog Interrupt Received"));
    BKNI_SetEvent_isr(g_watchdogEvent);
}

NEXUS_Error NEXUS_AudioDecoder_P_Init(void)
{
    BERR_Code errCode;
    BAPE_InterruptHandlers interrupts;

    g_numWatchdogs = 0;
    errCode = BKNI_CreateEvent(&g_watchdogEvent);
    if ( errCode ) 
    {
        return BERR_TRACE(errCode);
    }
    g_watchdogCallback = NEXUS_RegisterEvent(g_watchdogEvent, NEXUS_AudioDecoder_P_Watchdog, NULL);
    if ( NULL == g_watchdogCallback ) 
    {
        BKNI_DestroyEvent(g_watchdogEvent);
        g_watchdogEvent = NULL;
        return BERR_TRACE(BERR_OS_ERROR);
    }

    /* Allow watchdog to be disabled for debugging */
    if ( !NEXUS_GetEnv("no_watchdog") && g_NEXUS_audioModuleData.settings.watchdogEnabled )
    {
        BAPE_GetInterruptHandlers(NEXUS_AUDIO_DEVICE_HANDLE, &interrupts);
        interrupts.watchdog.pCallback_isr = NEXUS_AudioDecoder_P_Watchdog_isr;
        errCode = BAPE_SetInterruptHandlers(NEXUS_AUDIO_DEVICE_HANDLE, &interrupts);
        if ( errCode ) 
        {
            NEXUS_UnregisterEvent(g_watchdogCallback);
            g_watchdogCallback = NULL;
            BKNI_DestroyEvent(g_watchdogEvent);
            g_watchdogEvent = NULL;
            return BERR_TRACE(errCode);
        }
    }

    /* initialize global decoder data */
    BKNI_Memset(&g_decoders, 0, sizeof(g_decoders));

    return BERR_SUCCESS;
}

void NEXUS_AudioDecoder_P_Uninit(void)
{
    BAPE_InterruptHandlers interrupts;

    BAPE_GetInterruptHandlers(NEXUS_AUDIO_DEVICE_HANDLE, &interrupts);
    interrupts.watchdog.pCallback_isr = NULL;
    BAPE_SetInterruptHandlers(NEXUS_AUDIO_DEVICE_HANDLE, &interrupts);

    NEXUS_UnregisterEvent(g_watchdogCallback);
    g_watchdogCallback = NULL;
    BKNI_DestroyEvent(g_watchdogEvent);
    g_watchdogEvent = NULL;
}

/***************************************************************************
Summary:
Get default open settings for an audio decoder
***************************************************************************/
void NEXUS_AudioDecoder_GetDefaultOpenSettings(
    NEXUS_AudioDecoderOpenSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_DecoderOpenSettings apeSettings;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->independentDelay = g_NEXUS_audioModuleData.settings.independentDelay;
    pSettings->multichannelFormat = NEXUS_AudioMultichannelFormat_e5_1;
    BAPE_Decoder_GetDefaultOpenSettings(&apeSettings);
    pSettings->ancillaryDataFifoSize = apeSettings.ancillaryDataFifoSize;
}

static void NEXUS_AudioDecoder_P_GetDefaultSettings(
    NEXUS_AudioDecoderSettings *pSettings   /* [out] default settings */
    )
{
    int i;    
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioDecoderSettings));   
    pSettings->loudnessEquivalenceEnabled = true;
    for ( i = 0; i < NEXUS_AudioChannel_eMax; i++ )
    {
        pSettings->volumeMatrix[i][i] = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
    }
}

NEXUS_Error NEXUS_AudioDecoder_P_ConfigureRave(NEXUS_AudioDecoderHandle handle, NEXUS_RaveHandle rave, const NEXUS_AudioDecoderStartSettings *pProgram, const NEXUS_PidChannelStatus * pPidStatus)
{
    NEXUS_RaveSettings raveSettings;
    NEXUS_Error errCode;

    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
    raveSettings.pidChannel = pProgram->pidChannel;
    raveSettings.bandHold = handle->isPlayback && pPidStatus->playback; /* do not enable bandHold for live parser bands */
    raveSettings.continuityCountEnabled = !handle->isPlayback;
    raveSettings.numOutputBytesEnabled = true;
    raveSettings.audioDescriptor = pProgram->secondaryDecoder; /* We need descriptor values for any secondary decoder */
    errCode = NEXUS_Rave_ConfigureAudio_priv(rave, pProgram->codec, &raveSettings);
    if (errCode)
    {
        UNLOCK_TRANSPORT();
        return BERR_TRACE(errCode);
    }
    UNLOCK_TRANSPORT();
    return 0;
}


/***************************************************************************
Summary:
Open an audio decoder of the specified type
***************************************************************************/
NEXUS_AudioDecoderHandle NEXUS_AudioDecoder_Open( /* attr{destructor=NEXUS_AudioDecoder_Close}  */
    unsigned index,
    const NEXUS_AudioDecoderOpenSettings *pSettings   /* settings */
    )
{
    NEXUS_AudioDecoderHandle handle;
    NEXUS_AudioDecoderOpenSettings openSettings;
    NEXUS_RaveOpenSettings raveSettings;
    BAPE_DecoderOpenSettings decoderOpenSettings;
    BAPE_DecoderInterruptHandlers interrupts;
    BAPE_Connector mixerInput;
    BERR_Code errCode;
    unsigned j;

    if ( index >= NEXUS_NUM_AUDIO_DECODERS )
    {
        errCode=BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    handle = &g_decoders[index];

    /* Check if handle is already open */
    if ( handle->opened )
    {
        BDBG_ERR(("Decoder %d already opened", index));
        errCode=BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    if ( NULL == pSettings )
    {
        NEXUS_AudioDecoder_GetDefaultOpenSettings(&openSettings);
        pSettings = &openSettings;
    }

    /* Reset to default state */
    NEXUS_OBJECT_INIT(NEXUS_AudioDecoder, handle);
    handle->index = index;
    handle->stcDecoderType = NEXUS_StcChannelDecoderType_eMax;    /* Invalid */
    handle->trickMute = false;
    handle->trickForceStopped = false;

#if NEXUS_HAS_ASTM
    BKNI_Memset(&handle->astm.settings, 0, sizeof(NEXUS_AudioDecoderAstmSettings));
    BKNI_Memset(&handle->astm.status, 0, sizeof(NEXUS_AudioDecoderAstmStatus));
#endif

    for ( j = 0; j < NEXUS_AudioConnectorType_eMax; j++ )
    {
        /* Setup handle linkage */
        NEXUS_AUDIO_INPUT_INIT(&handle->connectors[j], NEXUS_AudioInputType_eDecoder, handle);

        /* Set format per-connector */
        if ( j == NEXUS_AudioConnectorType_eStereo )
        {
            handle->connectors[j].format = NEXUS_AudioInputFormat_ePcmStereo;
        }
        else if ( j == NEXUS_AudioConnectorType_eMultichannel )
        {
            /* The correct value for this is set later if multichannel is enabled */
            handle->connectors[j].format = NEXUS_AudioInputFormat_eNone;
        }
        else if ( j == NEXUS_AudioConnectorType_eMono )
        {
            handle->connectors[j].format = NEXUS_AudioInputFormat_ePcmMono;
        }
        else
        {
            handle->connectors[j].format = NEXUS_AudioInputFormat_eCompressed;
        }

        /* Invalidate outputs */
        BKNI_Memset(&(handle->outputLists[j]), 0, sizeof(NEXUS_AudioOutputList));
    }

    NEXUS_AudioDecoder_P_TrickReset(handle);

    /* Create Events */
    errCode = BKNI_CreateEvent(&handle->sampleRateEvent);
    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
        goto err_sample_rate_event;
    }

    handle->sampleRateCallback = NEXUS_RegisterEvent(handle->sampleRateEvent, NEXUS_AudioDecoder_P_SampleRate, handle);
    if ( NULL == handle->sampleRateCallback )
    {
        errCode=BERR_TRACE(BERR_OS_ERROR);
        goto err_sample_rate_callback;
    }

    errCode = BKNI_CreateEvent(&handle->channelChangeReportEvent);
    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
        goto err_channel_change_report_event;
    }

     handle->channelChangeReportEventHandler = NEXUS_RegisterEvent(handle->channelChangeReportEvent, NEXUS_AudioDecoder_P_ChannelChangeReport, handle);
     if ( NULL == handle->channelChangeReportEventHandler )
     {
         errCode=BERR_TRACE(BERR_OS_ERROR);
         goto err_channel_change_report_event_handler;
     }

     errCode = BKNI_CreateEvent(&handle->inputFormatChangeEvent);
     if ( errCode )
     {
         errCode=BERR_TRACE(errCode);
         goto err_input_format_change_event;
     }

    handle->inputFormatChangeEventHandler = NEXUS_RegisterEvent(handle->inputFormatChangeEvent, NEXUS_AudioDecoder_P_InputFormatChange, handle);
    if ( NULL == handle->inputFormatChangeEventHandler )
    {
      errCode=BERR_TRACE(BERR_OS_ERROR);
      goto err_input_format_change_event_handler;
    }

    if ( NEXUS_GetEnv("multichannel_disabled") || NEXUS_GetEnv("audio_processing_disabled") )
    {
        handle->connectors[NEXUS_AudioConnectorType_eMultichannel].format = NEXUS_AudioInputFormat_ePcmStereo;
    }
    else
    {
        switch ( pSettings->multichannelFormat )
        {
        case NEXUS_AudioMultichannelFormat_eNone:
            /* There is no benefit to disabling multichannel audio on APE it does not save memory.  Always use 5.1 as default. */
        case NEXUS_AudioMultichannelFormat_e5_1:
            handle->connectors[NEXUS_AudioConnectorType_eMultichannel].format = NEXUS_AudioInputFormat_ePcm5_1;
            #if NEXUS_NUM_HDMI_OUTPUTS && 0
            /* TODO: Notify PI of 5.1 vs. 7.1 */
            pChannelSettings->sChnRBufPool.uiMaxNumOutChPairs[0] += 3;
            #endif
            break;
        #if 0 /* Not supported on any current chipsets */
        case NEXUS_AudioMultichannelFormat_e7_1:
            handle->connectors[NEXUS_AudioConnectorType_eMultichannel].format = NEXUS_AudioInputFormat_ePcm7_1;
            #if NEXUS_NUM_HDMI_OUTPUTS && 0
            /* TODO: Notify PI of 5.1 vs. 7.1 */
            pChannelSettings->sChnRBufPool.uiMaxNumOutChPairs[0] += 4;
            #endif
            break;
        #endif
        default:
            BDBG_ERR(("Unsupported multichannel audio format"));
            errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_channel;
        }
    }
    if ( pSettings->independentDelay && !g_NEXUS_audioModuleData.settings.independentDelay )
    {
        BDBG_ERR(("Independent delay must be enabled at the audio module level.  Please check NEXUS_AudioModuleSettings.independentDelay."));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_channel;
    }

    /* Open APE decoder */
    BAPE_Decoder_GetDefaultOpenSettings(&decoderOpenSettings);
    decoderOpenSettings.dspIndex = pSettings->dspIndex;
    decoderOpenSettings.ancillaryDataFifoSize = pSettings->ancillaryDataFifoSize;
    errCode = BAPE_Decoder_Open(NEXUS_AUDIO_DEVICE_HANDLE, index, &decoderOpenSettings, &handle->channel);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_channel;
    }

    /* Init connectors */
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eStereo, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eStereo].port = (uint32_t)mixerInput;
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eMultichannel, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eMultichannel].port = (uint32_t)mixerInput;
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eCompressed, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eCompressed].port = (uint32_t)mixerInput;
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eCompressed4x, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eCompressed4x].port = (uint32_t)mixerInput;
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eCompressed16x, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eCompressed16x].port = (uint32_t)mixerInput;
    BAPE_Decoder_GetConnector(handle->channel, BAPE_ConnectorFormat_eMono, &mixerInput);
    handle->connectors[NEXUS_AudioConnectorType_eMono].port = (uint32_t)mixerInput;

    /* Hook up decoder interrupts */
    BAPE_Decoder_GetInterruptHandlers(handle->channel, &interrupts);
    interrupts.firstPts.pCallback_isr = NEXUS_AudioDecoder_P_FirstPts_isr;
    interrupts.firstPts.pParam1 = handle;
    interrupts.tsmFail.pCallback_isr = NEXUS_AudioDecoder_P_AudioTsmFail_isr;
    interrupts.tsmFail.pParam1 = handle;
    interrupts.tsmPass.pCallback_isr = NEXUS_AudioDecoder_P_AudioTsmPass_isr;
    interrupts.tsmPass.pParam1 = handle;
    interrupts.sampleRateChange.pCallback_isr = NEXUS_AudioDecoder_P_SampleRateChange_isr;
    interrupts.sampleRateChange.pParam1 = handle;
    interrupts.lock.pCallback_isr = NEXUS_AudioDecoder_P_Lock_isr;
    interrupts.lock.pParam1 = handle;
    interrupts.lock.param2 = true;
    interrupts.unlock.pCallback_isr = NEXUS_AudioDecoder_P_Lock_isr;
    interrupts.unlock.pParam1 = handle;
    interrupts.unlock.param2 = false;
    interrupts.statusReady.pCallback_isr = NEXUS_AudioDecoder_P_StreamStatusAvailable_isr;
    interrupts.statusReady.pParam1 = handle;
    interrupts.modeChange.pCallback_isr = NEXUS_AudioDecoder_P_StreamParameterChanged_isr;
    interrupts.modeChange.pParam1 = handle;
    interrupts.cdbItbOverflow.pCallback_isr = NEXUS_AudioDecoder_P_CdbItbOverflow_isr;
    interrupts.cdbItbOverflow.pParam1 = handle;
    interrupts.ancillaryData.pCallback_isr = NEXUS_AudioDecoder_P_AncillaryData_isr;
    interrupts.ancillaryData.pParam1 = handle;
#if 0   /* JDG: This causes far too many source changed callbacks to fire.  Some codecs will trigger
                this interrupt on every frame.  It was not functional on older RAP platforms likely
                for the same reason. */
    interrupts.bitrateChange.pCallback_isr = NEXUS_AudioDecoder_P_StreamParameterChanged_isr;
    interrupts.bitrateChange.pParam1 = handle;
#endif
    BAPE_Decoder_SetInterruptHandlers(handle->channel, &interrupts);

    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    UNLOCK_TRANSPORT();

    BAPE_Decoder_GetDefaultCdbItbConfig(handle->channel, &raveSettings.config);

    if ( pSettings->fifoSize == 0 )
    {
        /* NOTE: Don't automatically increase CDB/ITB for IP Settop internally. */
    }
    else
    {
        /* Make ITB proportional to CDB */
        raveSettings.config.Itb.Length = 1024*((raveSettings.config.Itb.Length/1024) * (pSettings->fifoSize/1024))/(raveSettings.config.Cdb.Length/1024);
        BDBG_ASSERT(0 != raveSettings.config.Itb.Length);
        raveSettings.config.Cdb.Length = pSettings->fifoSize;
    }

    LOCK_TRANSPORT();
    handle->raveContext = NEXUS_Rave_Open_priv(&raveSettings);
    UNLOCK_TRANSPORT();
    if ( NULL == handle->raveContext )
    {
        BDBG_ERR(("Unable to allocate RAVE context"));
        goto err_rave;
    }

    handle->sourceChangeAppCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->sourceChangeAppCallback )
    {
        goto err_app_callback;
    }

    handle->lockCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->lockCallback )
    {
        goto err_lock_callback;
    }

    handle->ptsErrorCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->ptsErrorCallback )
    {
        goto err_pts_error_callback;
    }

    handle->firstPtsCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->firstPtsCallback )
    {
        goto err_first_pts_callback;
    }

    handle->fifoOverflowCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->fifoOverflowCallback )
    {
        goto err_overflow_callback;
    }

    handle->streamStatusCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->streamStatusCallback )
    {
        goto err_status_callback;
    }

    handle->ancillaryDataCallback = NEXUS_IsrCallback_Create(handle, NULL);
    if ( NULL == handle->ancillaryDataCallback )
    {
        goto err_ancillary_callback;
    }

    /* Success */
    NEXUS_AudioDecoder_P_GetDefaultSettings(&handle->settings);
    (void)NEXUS_AudioDecoder_SetSettings(handle, &handle->settings);
    handle->opened = true;

    return handle;

err_ancillary_callback:
    NEXUS_IsrCallback_Destroy(handle->streamStatusCallback);
err_status_callback:
    NEXUS_IsrCallback_Destroy(handle->fifoOverflowCallback);
err_overflow_callback:
    NEXUS_IsrCallback_Destroy(handle->firstPtsCallback);
err_first_pts_callback:
    NEXUS_IsrCallback_Destroy(handle->ptsErrorCallback);
err_pts_error_callback:
    NEXUS_IsrCallback_Destroy(handle->lockCallback);
err_lock_callback:
    NEXUS_IsrCallback_Destroy(handle->sourceChangeAppCallback);
err_app_callback:
    LOCK_TRANSPORT();
    NEXUS_Rave_Close_priv(handle->raveContext);
    UNLOCK_TRANSPORT();
err_rave:
    BAPE_Decoder_Close(handle->channel);
err_channel:
    NEXUS_UnregisterEvent(handle->inputFormatChangeEventHandler);
err_input_format_change_event_handler:
    BKNI_DestroyEvent(handle->inputFormatChangeEvent);
err_input_format_change_event:
    NEXUS_UnregisterEvent(handle->channelChangeReportEventHandler);
err_channel_change_report_event_handler:
    BKNI_DestroyEvent(handle->channelChangeReportEvent);
err_channel_change_report_event:
    NEXUS_UnregisterEvent(handle->sampleRateCallback);
err_sample_rate_callback:
    BKNI_DestroyEvent(handle->sampleRateEvent);
err_sample_rate_event:
    NEXUS_OBJECT_UNSET(NEXUS_AudioDecoder, handle);
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioDecoder));
    return NULL;
}

/***************************************************************************
Summary:
Close an audio decoder of the specified type
***************************************************************************/
static void NEXUS_AudioDecoder_P_Finalizer(
    NEXUS_AudioDecoderHandle handle
    )
{
    int i;

    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoder, handle);
    BDBG_ASSERT(true == handle->opened);

    if ( handle->started )
    {
        NEXUS_AudioDecoder_Stop(handle);
    }

    for ( i = 0; i < NEXUS_AudioConnectorType_eMax; i++ )
    {
        if ( handle->connectors[i].pMixerData )
        {
            const char *pType;
            switch ( i )
            {
            case NEXUS_AudioConnectorType_eStereo:
                pType = "stereo";
                break;
            case NEXUS_AudioConnectorType_eCompressed:
                pType = "compressed";
                break;
            case NEXUS_AudioConnectorType_eCompressed4x:
                pType = "compressed 4x";
                break;
            case NEXUS_AudioConnectorType_eCompressed16x:
                pType = "compressed 16x";
                break;
            case NEXUS_AudioConnectorType_eMultichannel:
                pType = "multichannel";
                break;
            case NEXUS_AudioConnectorType_eMono:
                pType = "mono";
                break;
            default:
                pType = "unknown";
                break;
            }
            BDBG_MSG(("Decoder Connector %p (type=%s) is still active. Calling NEXUS_AudioInput_Shutdown.", &handle->connectors[i], pType));
            NEXUS_AudioInput_Shutdown(&handle->connectors[i]);
        }
    }

    BAPE_Decoder_Close(handle->channel);

    NEXUS_IsrCallback_Destroy(handle->ptsErrorCallback);
    NEXUS_IsrCallback_Destroy(handle->lockCallback);
    NEXUS_IsrCallback_Destroy(handle->sourceChangeAppCallback);
    NEXUS_IsrCallback_Destroy(handle->firstPtsCallback);
    NEXUS_IsrCallback_Destroy(handle->fifoOverflowCallback);
    NEXUS_IsrCallback_Destroy(handle->streamStatusCallback);
    NEXUS_IsrCallback_Destroy(handle->ancillaryDataCallback);
    LOCK_TRANSPORT();
    NEXUS_Rave_Close_priv(handle->raveContext);
    UNLOCK_TRANSPORT();
    NEXUS_UnregisterEvent(handle->inputFormatChangeEventHandler);
    BKNI_DestroyEvent(handle->inputFormatChangeEvent);
    NEXUS_UnregisterEvent(handle->channelChangeReportEventHandler);
    BKNI_DestroyEvent(handle->channelChangeReportEvent);
    NEXUS_UnregisterEvent(handle->sampleRateCallback);
    BKNI_DestroyEvent(handle->sampleRateEvent);
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioDecoder));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioDecoder, NEXUS_AudioDecoder_Close);

/***************************************************************************
Summary:
Get Settings for an audio decoder
***************************************************************************/
void NEXUS_AudioDecoder_GetSettings(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioDecoderSettings *pSettings   /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
Set Settings for an audio decoder
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_SetSettings(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderSettings *pSettings /* Settings */
    )
{
    if ( NULL == pSettings )
    {
        NEXUS_AudioDecoder_P_GetDefaultSettings(&handle->settings);
    }
    else
    {
        if ( pSettings->fifoThreshold != handle->settings.fifoThreshold )
        {
            BERR_Code rc;

            LOCK_TRANSPORT();

            rc = NEXUS_Rave_SetCdbThreshold_priv(handle->raveContext, pSettings->fifoThreshold);

            UNLOCK_TRANSPORT();

            if (rc) return BERR_TRACE(rc);
        }

        handle->settings = *pSettings;
    }
    NEXUS_IsrCallback_Set(handle->sourceChangeAppCallback, &(handle->settings.sourceChanged));
    NEXUS_IsrCallback_Set(handle->lockCallback, &(handle->settings.lockChanged));
    NEXUS_IsrCallback_Set(handle->ptsErrorCallback, &(handle->settings.ptsError));
    NEXUS_IsrCallback_Set(handle->firstPtsCallback, &(handle->settings.firstPts));
    NEXUS_IsrCallback_Set(handle->fifoOverflowCallback, &(handle->settings.fifoOverflow));
    NEXUS_IsrCallback_Set(handle->streamStatusCallback, &(handle->settings.streamStatusAvailable));
    NEXUS_IsrCallback_Set(handle->ancillaryDataCallback, &(handle->settings.ancillaryData));
    return NEXUS_AudioDecoder_ApplySettings_priv(handle);
}

/***************************************************************************
Summary:
Initialize an audio decoder program structure
***************************************************************************/
void NEXUS_AudioDecoder_GetDefaultStartSettings(
    NEXUS_AudioDecoderStartSettings *pSettings /* [out] Program Defaults */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioDecoderStartSettings));
    pSettings->codec = NEXUS_AudioCodec_eAc3;
    pSettings->targetSyncEnabled = true;
    pSettings->maxOutputRate = 48000;
}

/***************************************************************************
Summary:
Start deocding the specified program
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Start(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderStartSettings *pProgram    /* What to start decoding */
    )
{
    NEXUS_Error errCode;
    bool useTsm, playback=false;
    NEXUS_PidChannelStatus pidChannelStatus;
    BAVC_StreamType streamType=BAVC_StreamType_eTsMpeg;
    BAPE_DecoderStartSettings *pStartSettings;
    BAPE_DecoderTsmSettings tsmSettings;


    BDBG_ENTER(NEXUS_AudioDecoder_Start);

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pProgram);

    if ( handle->started )
    {
        BDBG_ERR(("This decoder is already started.  Please call stop first."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    pStartSettings = &handle->apeStartSettings;
    BAPE_Decoder_GetDefaultStartSettings(pStartSettings);
    pStartSettings->targetSyncEnabled = pProgram->targetSyncEnabled;
    pStartSettings->forceCompleteFirstFrame = pProgram->forceCompleteFirstFrame;
    pStartSettings->nonRealTime = pProgram->nonRealTime;
    switch ( pProgram->mixingMode )
    {
    default:
        BDBG_ERR(("Invalid mixing mode"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    case NEXUS_AudioDecoderMixingMode_eDescription:
        pStartSettings->mixingMode = BAPE_DecoderMixingMode_eDescription;
        break;
    case NEXUS_AudioDecoderMixingMode_eSoundEffects:
        pStartSettings->mixingMode = BAPE_DecoderMixingMode_eSoundEffects;
        break;
    }
    switch ( pProgram->maxOutputRate )
    {
    default:
        BDBG_ERR(("Invalid max output rate"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    case 48000:
    case 96000:
        pStartSettings->maxOutputRate = pProgram->maxOutputRate;
        break;
    }
    NEXUS_AudioDecoder_P_TrickReset(handle); /* reset trick state on start */

    /* Save Program */
    handle->programSettings = *pProgram;

    /* Sanity check program */
    if ( pProgram->pidChannel )
    {
        NEXUS_OBJECT_ACQUIRE(handle, NEXUS_PidChannel, pProgram->pidChannel);

        /* Transport source */
        errCode = NEXUS_PidChannel_GetStatus(pProgram->pidChannel, &pidChannelStatus);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }

        handle->isPlayback = pidChannelStatus.playback;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if (handle->settings.liveOverride) {
			handle->isPlayback = true;
			BDBG_MSG(("%s: Overriding live mode of Audio Decoder, ptsoffset %d", __FUNCTION__, handle->settings.ptsOffset));
		}
#endif		
        playback = handle->isPlayback;

        /* There are 4 types of DSS A/V streams:
        DSS SD video - this is DSS ES
        DSS HD video - this is DSS PES
        DSS MPEG audio - this actually uses MPEG1 system headers, but it's very similar to PES, therefore DSS PES
            Therefore we convert DSS ES to DSS PES here.
        DSS AC3 audio - uses MPEG2 System PES, therefore DSS PES
        */
        if (pidChannelStatus.transportType == NEXUS_TransportType_eDssEs)
        {
            pidChannelStatus.transportType = NEXUS_TransportType_eDssPes;
        }
        /* All DSS is converted to PES above */
        handle->isDss = (pidChannelStatus.transportType == NEXUS_TransportType_eDssPes)?true:false;

        /* Convert to AVC transport type */
        errCode = NEXUS_P_TransportType_ToMagnum(pidChannelStatus.transportType, &streamType);
        if (errCode) 
        {
            errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_stream_type;
        }

        switch ( pProgram->delayMode )
        {
        case NEXUS_AudioDecoderDelayMode_eLow:
            pStartSettings->delayMode = BAPE_DspDelayMode_eLow;
            break;
        default:
        case NEXUS_AudioDecoderDelayMode_eAuto:
        case NEXUS_AudioDecoderDelayMode_eFixed:
            pStartSettings->delayMode = BAPE_DspDelayMode_eDefault;
            break;
        }
    }
    else if ( pProgram->input )
    {
        streamType = BAVC_StreamType_ePes;
        handle->isPlayback = false;
        handle->isDss = false;
        pStartSettings->inputPort = NEXUS_AudioInput_P_GetInputPort(pProgram->input);
        switch ( pProgram->delayMode )
        {
        default:
        case NEXUS_AudioDecoderDelayMode_eAuto:
        case NEXUS_AudioDecoderDelayMode_eLow:
            pStartSettings->delayMode = BAPE_DspDelayMode_eLow;
            break;
        case NEXUS_AudioDecoderDelayMode_eFixed:
            pStartSettings->delayMode = BAPE_DspDelayMode_eDefault;
            break;
        }
        if ( NULL == pStartSettings->inputPort )
        {
            errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_input_port;
        }
        playback = false;
        BKNI_Memset(&pidChannelStatus, 0, sizeof(NEXUS_PidChannelStatus));        
    }
    else
    {
        BDBG_ERR(("No PID Channel or Input provided"));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_start_settings;
    }

    pStartSettings->streamType = streamType;
    pStartSettings->codec = NEXUS_Audio_P_CodecToMagnum(pProgram->codec);

    if (pProgram->stcChannel) 
    {
        NEXUS_StcChannelSettings stcSettings;
        LOCK_TRANSPORT();
        NEXUS_StcChannel_GetIndex_priv(pProgram->stcChannel, &pStartSettings->stcIndex);
        UNLOCK_TRANSPORT();
        NEXUS_StcChannel_GetSettings(pProgram->stcChannel, &stcSettings);
        playback = (stcSettings.mode != NEXUS_StcChannelMode_ePcr && NULL == pProgram->input);
        NEXUS_OBJECT_ACQUIRE(handle, NEXUS_StcChannel, pProgram->stcChannel);
    }

    /* Compute TSM details */
    BAPE_Decoder_GetTsmSettings(handle->channel, &tsmSettings);
    tsmSettings.playback = handle->isPlayback && playback;
    tsmSettings.ptsOffset = handle->settings.ptsOffset; 
#if NEXUS_HAS_SYNC_CHANNEL
    tsmSettings.ptsOffset += handle->sync.settings.delay;
#endif
    tsmSettings.thresholds.discard = (tsmSettings.playback)?30000:3000;   /* ms */
    tsmSettings.thresholds.grossAdjustment = (pidChannelStatus.originalTransportType == NEXUS_TransportType_eAvi) ? 0x30 : 0x8;
    BAPE_Decoder_SetTsmSettings(handle->channel, &tsmSettings);

    /* Determine TSM mode */
    if ( NULL == pProgram->stcChannel ||
         NULL != NEXUS_GetEnv("force_vsync") ||
         (pProgram->pidChannel && (pidChannelStatus.transportType == NEXUS_TransportType_eEs ||
                                   pidChannelStatus.originalTransportType == NEXUS_TransportType_eWav ||
                                   pidChannelStatus.originalTransportType == NEXUS_TransportType_eAiff ||
                                   #if 1 /* SW7425-2000: OGG PTS values are invalid from bmedia currently */
                                   pidChannelStatus.originalTransportType == NEXUS_TransportType_eOgg ||
                                   #endif
                                   pidChannelStatus.originalTransportType == NEXUS_TransportType_eEs)) ||
         (pProgram->codec == NEXUS_AudioCodec_eMlp) )
    {
        useTsm = false;
    }
    else
    {
        useTsm = true;
    }
    handle->tsmPermitted = useTsm;
    NEXUS_AudioDecoder_P_SetTsm(handle);

    if ( pProgram->pidChannel )
    {
        if (!handle->savedRaveContext) {
            errCode = NEXUS_AudioDecoder_P_ConfigureRave(handle, handle->raveContext, pProgram, &pidChannelStatus);
            if (errCode)
            {
                errCode = BERR_TRACE(errCode);
                goto err_configure_audio;
            }
        }
        LOCK_TRANSPORT();
        errCode = NEXUS_Rave_GetStatus_priv(handle->raveContext, &handle->raveStatus);
        UNLOCK_TRANSPORT();
        if (errCode)
        {
            errCode = BERR_TRACE(errCode);
            goto err_rave_status;
        }
        pStartSettings->pContextMap = &handle->raveStatus.xptContextMap;
    }

    if ( pProgram->input )
    {
        if ( NEXUS_AudioInput_P_SupportsFormatChanges(pProgram->input) )
        {
            /* If this input supports dynamic format changes, enable the dynamic format change interrupt and kick off the format state machine. */
            errCode = NEXUS_AudioInput_P_SetFormatChangeInterrupt(pProgram->input, NEXUS_AudioDecoder_P_InputFormatChange_isr, handle, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_start;
            }
            handle->started = true;
            NEXUS_AudioDecoder_P_InputFormatChange(handle);
            return BERR_SUCCESS;
        }
    }

    /* We're ready to start.  Build up lists of outputs to check for configuration changes. */
    errCode = NEXUS_AudioDecoder_P_Start(handle);
    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
        goto err_start;
    }

    handle->started = true;

    BDBG_LEAVE(NEXUS_AudioDecoder_Start);

    /* Success */
    return BERR_SUCCESS;

err_start:
err_rave_status:
    if ( pProgram->pidChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_Rave_RemovePidChannel_priv(handle->raveContext);
        UNLOCK_TRANSPORT();
    }

err_configure_audio:
    if ( handle->programSettings.stcChannel )
    {
        NEXUS_OBJECT_RELEASE(handle, NEXUS_StcChannel, handle->programSettings.stcChannel);
    }
err_start_settings:
err_input_port:
err_stream_type:
    if ( handle->programSettings.pidChannel )
    {
        NEXUS_OBJECT_RELEASE(handle, NEXUS_PidChannel, handle->programSettings.pidChannel);
    }
    BKNI_Memset(&handle->programSettings, 0, sizeof(NEXUS_AudioDecoderStartSettings));

    return errCode;
}

/***************************************************************************
Summary:
Stop deocding the current program
***************************************************************************/
void NEXUS_AudioDecoder_Stop(
    NEXUS_AudioDecoderHandle handle
    )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);

    BDBG_ENTER(NEXUS_AudioDecoder_Stop);

    if ( !handle->started )
    {
        BDBG_ERR(("Decoder not started"));
        return;
    }

    if ( handle->programSettings.input )
    {
        if ( NEXUS_AudioInput_P_SupportsFormatChanges(handle->programSettings.input) )
        {
            /* If this input supports dynamic format changes, disable the dynamic format change interrupt. */
            (void)NEXUS_AudioInput_P_SetFormatChangeInterrupt(handle->programSettings.input, NULL, NULL, 0);
        }
    }

    errCode = NEXUS_AudioDecoder_P_Stop(handle, true);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
    }

    handle->running = false;
    handle->started = false;
    handle->trickForceStopped = false; /* do we need to forcedly unmute on Stop, in a way it helps if in a PIP change mode decoder is moved from trickmode on one channel to normal mode on another channel, however it hurts if one stops decoder just in order to change a PID/ audio program */

    if ( handle->programSettings.stcChannel )
    {
        NEXUS_OBJECT_RELEASE(handle, NEXUS_StcChannel, handle->programSettings.stcChannel);
    }
    if ( handle->programSettings.pidChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_Rave_RemovePidChannel_priv(handle->raveContext);
        UNLOCK_TRANSPORT();
        NEXUS_OBJECT_RELEASE(handle, NEXUS_PidChannel, handle->programSettings.pidChannel);
    }

    BKNI_Memset(&handle->programSettings, 0, sizeof(handle->programSettings));

    BDBG_LEAVE(NEXUS_AudioDecoder_Stop);
}

/***************************************************************************
Summary:
Discards all data accumulated in the decoder buffer
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Flush(
    NEXUS_AudioDecoderHandle handle
    )
{
    BERR_Code rc = BERR_SUCCESS;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);

    if ( !handle->started || !handle->running )
    {
        return BERR_SUCCESS;
    }

    BAPE_Decoder_DisableForFlush(handle->channel);

    BDBG_ASSERT(handle->raveContext);

    if ( handle->programSettings.pidChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_Rave_Disable_priv(handle->raveContext);
        NEXUS_Rave_Flush_priv(handle->raveContext);
        UNLOCK_TRANSPORT();
    }

    rc = BAPE_Decoder_Flush(handle->channel);
    if ( rc )
    {
        (void)BERR_TRACE(rc);
    }

    if ( handle->programSettings.pidChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_Rave_Enable_priv(handle->raveContext);
        UNLOCK_TRANSPORT();
    }

    return rc;
}

/***************************************************************************
Summary:
    Get codec-specific decoder settings
***************************************************************************/
void NEXUS_AudioDecoder_GetCodecSettings(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioCodec codec,
    NEXUS_AudioDecoderCodecSettings *pSettings  /* [out] settings for specified codec */
    )
{
    BAPE_DecoderCodecSettings codecSettings;
    BAVC_AudioCompressionStd avcCodec;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSettings);

    avcCodec = NEXUS_Audio_P_CodecToMagnum(codec);

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->codec = codec;

    BAPE_Decoder_GetCodecSettings(handle->channel, avcCodec, &codecSettings);
    switch ( codec )
    {
    case NEXUS_AudioCodec_eAc3:
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyDrcMode_eLine == (NEXUS_AudioDecoderDolbyDrcMode)BAPE_Ac3DrcMode_eLine);
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyDrcMode_eRf == (NEXUS_AudioDecoderDolbyDrcMode)BAPE_Ac3DrcMode_eRf);
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyDrcMode_eCustomA == (NEXUS_AudioDecoderDolbyDrcMode)BAPE_Ac3DrcMode_eCustomA);
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyDrcMode_eCustomD == (NEXUS_AudioDecoderDolbyDrcMode)BAPE_Ac3DrcMode_eCustomD);
        pSettings->codecSettings.ac3.drcMode = codecSettings.codecSettings.ac3.drcMode;
        pSettings->codecSettings.ac3.drcModeDownmix = codecSettings.codecSettings.ac3.drcModeDownmix;
        pSettings->codecSettings.ac3.cut = codecSettings.codecSettings.ac3.drcScaleHi;
        pSettings->codecSettings.ac3.boost = codecSettings.codecSettings.ac3.drcScaleLow;
        pSettings->codecSettings.ac3.cutDownmix = codecSettings.codecSettings.ac3.drcScaleHiDownmix;
        pSettings->codecSettings.ac3.boostDownmix = codecSettings.codecSettings.ac3.drcScaleLowDownmix;
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic == (NEXUS_AudioDecoderDolbyStereoDownmixMode)BAPE_Ac3StereoMode_eAuto);
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible == (NEXUS_AudioDecoderDolbyStereoDownmixMode)BAPE_Ac3StereoMode_eLtRt);
        BDBG_CASSERT(NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard == (NEXUS_AudioDecoderDolbyStereoDownmixMode)BAPE_Ac3StereoMode_eLoRo);
        pSettings->codecSettings.ac3.stereoDownmixMode = codecSettings.codecSettings.ac3.stereoMode;
        pSettings->codecSettings.ac3.scale = codecSettings.codecSettings.ac3.scale;
        pSettings->codecSettings.ac3.scaleDownmix = codecSettings.codecSettings.ac3.scaleDownmix;
        pSettings->codecSettings.ac3.dialogNormalization = codecSettings.codecSettings.ac3.dialogNormalization;
        pSettings->codecSettings.ac3.dialogNormalizationValue = codecSettings.codecSettings.ac3.dialogNormalizationValue;
        pSettings->codecSettings.ac3.substreamId = codecSettings.codecSettings.ac3.substreamId;
        break;
    case NEXUS_AudioCodec_eAc3Plus:
        pSettings->codecSettings.ac3Plus.drcMode = codecSettings.codecSettings.ac3Plus.drcMode;
        pSettings->codecSettings.ac3Plus.drcModeDownmix = codecSettings.codecSettings.ac3Plus.drcModeDownmix;
        pSettings->codecSettings.ac3Plus.cut = codecSettings.codecSettings.ac3Plus.drcScaleHi;
        pSettings->codecSettings.ac3Plus.boost = codecSettings.codecSettings.ac3Plus.drcScaleLow;
        pSettings->codecSettings.ac3Plus.cutDownmix = codecSettings.codecSettings.ac3Plus.drcScaleHiDownmix;
        pSettings->codecSettings.ac3Plus.boostDownmix = codecSettings.codecSettings.ac3Plus.drcScaleLowDownmix;
        pSettings->codecSettings.ac3Plus.stereoDownmixMode = codecSettings.codecSettings.ac3Plus.stereoMode;
        pSettings->codecSettings.ac3Plus.scale = codecSettings.codecSettings.ac3Plus.scale;
        pSettings->codecSettings.ac3Plus.scaleDownmix = codecSettings.codecSettings.ac3Plus.scaleDownmix;
        pSettings->codecSettings.ac3Plus.dialogNormalization = codecSettings.codecSettings.ac3Plus.dialogNormalization;
        pSettings->codecSettings.ac3Plus.dialogNormalizationValue = codecSettings.codecSettings.ac3Plus.dialogNormalizationValue;
        pSettings->codecSettings.ac3Plus.substreamId = codecSettings.codecSettings.ac3Plus.substreamId;
        break;
    case NEXUS_AudioCodec_eAacAdts:
    case NEXUS_AudioCodec_eAacLoas:
        pSettings->codecSettings.aac.cut = codecSettings.codecSettings.aac.drcScaleHi;
        pSettings->codecSettings.aac.boost = codecSettings.codecSettings.aac.drcScaleLow;
        pSettings->codecSettings.aac.drcTargetLevel = codecSettings.codecSettings.aac.drcTargetLevel;
        BDBG_CASSERT((int)BAPE_AacStereoMode_eMatrix == (int)NEXUS_AudioDecoderAacDownmixMode_eMatrix);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eArib == (int)NEXUS_AudioDecoderAacDownmixMode_eArib);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eLtRt == (int)NEXUS_AudioDecoderAacDownmixMode_eLtRt);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eLoRo == (int)NEXUS_AudioDecoderAacDownmixMode_eLoRo);
        pSettings->codecSettings.aac.downmixMode = codecSettings.codecSettings.aac.downmixMode;
        BDBG_CASSERT((int)NEXUS_AudioDecoderDolbyPulseDrcMode_eLine == (int)BAPE_DolbyPulseDrcMode_eLine);
        BDBG_CASSERT((int)NEXUS_AudioDecoderDolbyPulseDrcMode_eRf == (int)BAPE_DolbyPulseDrcMode_eRf);
        pSettings->codecSettings.aac.drcMode = codecSettings.codecSettings.aac.drcMode;
        pSettings->codecSettings.aac.drcDefaultLevel = codecSettings.codecSettings.aac.drcDefaultLevel;
        pSettings->codecSettings.aac.mpegConformanceMode = codecSettings.codecSettings.aac.mpegConformanceMode;
        break;
    case NEXUS_AudioCodec_eAacPlusAdts:
    case NEXUS_AudioCodec_eAacPlusLoas:
        pSettings->codecSettings.aacPlus.cut = codecSettings.codecSettings.aacPlus.drcScaleHi;
        pSettings->codecSettings.aacPlus.boost = codecSettings.codecSettings.aacPlus.drcScaleLow;
        pSettings->codecSettings.aacPlus.drcTargetLevel = codecSettings.codecSettings.aacPlus.drcTargetLevel;
        BDBG_CASSERT((int)BAPE_AacStereoMode_eMatrix == (int)NEXUS_AudioDecoderAacDownmixMode_eMatrix);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eArib == (int)NEXUS_AudioDecoderAacDownmixMode_eArib);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eLtRt == (int)NEXUS_AudioDecoderAacDownmixMode_eLtRt);
        BDBG_CASSERT((int)BAPE_AacStereoMode_eLoRo == (int)NEXUS_AudioDecoderAacDownmixMode_eLoRo);
        pSettings->codecSettings.aacPlus.downmixMode = codecSettings.codecSettings.aacPlus.downmixMode;
        BDBG_CASSERT((int)NEXUS_AudioDecoderDolbyPulseDrcMode_eLine == (int)BAPE_DolbyPulseDrcMode_eLine);
        BDBG_CASSERT((int)NEXUS_AudioDecoderDolbyPulseDrcMode_eRf == (int)BAPE_DolbyPulseDrcMode_eRf);
        pSettings->codecSettings.aacPlus.drcMode = codecSettings.codecSettings.aacPlus.drcMode;
        pSettings->codecSettings.aacPlus.drcDefaultLevel = codecSettings.codecSettings.aacPlus.drcDefaultLevel;
        pSettings->codecSettings.aacPlus.mpegConformanceMode = codecSettings.codecSettings.aacPlus.mpegConformanceMode;
        break;
    case NEXUS_AudioCodec_eWmaPro:
        /* TODO: Nexus is exposing the older WMA Pro DRC mode.  This should switch from bool .. enum to match APE */
        switch ( codecSettings.codecSettings.wmaPro.drcMode )
        {
        case BAPE_WmaProDrcMode_eDisabled:
            pSettings->codecSettings.wmaPro.dynamicRangeControlValid = false;
            break;
        default:
            pSettings->codecSettings.wmaPro.dynamicRangeControlValid = true;
            break;
        }
        /* TODO: Expose stereo mode */
        pSettings->codecSettings.wmaPro.dynamicRangeControl.peakReference = codecSettings.codecSettings.wmaPro.peakAmplitudeReference;
        pSettings->codecSettings.wmaPro.dynamicRangeControl.peakTarget = codecSettings.codecSettings.wmaPro.desiredPeak;
        pSettings->codecSettings.wmaPro.dynamicRangeControl.averageReference = codecSettings.codecSettings.wmaPro.rmsAmplitudeReference;
        pSettings->codecSettings.wmaPro.dynamicRangeControl.averageTarget = codecSettings.codecSettings.wmaPro.desiredRms;
        break;
    case NEXUS_AudioCodec_eDts:
    case NEXUS_AudioCodec_eDtsHd:
    case NEXUS_AudioCodec_eDtsLegacy: /* For DTS streams with legacy frame-sync.  These streams are something called as 14bits stream */
        pSettings->codecSettings.dts.mixLfeToPrimary = codecSettings.codecSettings.dts.mixLfeToPrimary;
        BDBG_CASSERT((int)NEXUS_AudioDecoderDtsDownmixMode_eAuto == (int)BAPE_DtsStereoMode_eAuto);
        BDBG_CASSERT((int)NEXUS_AudioDecoderDtsDownmixMode_eLtRt == (int)BAPE_DtsStereoMode_eLtRt);
        BDBG_CASSERT((int)NEXUS_AudioDecoderDtsDownmixMode_eLoRo == (int)BAPE_DtsStereoMode_eLoRo);
        pSettings->codecSettings.dts.stereoDownmixMode = (NEXUS_AudioDecoderDtsStereoDownmixMode)codecSettings.codecSettings.dts.stereoMode;
        pSettings->codecSettings.dts.enableDrc = codecSettings.codecSettings.dts.drcMode == BAPE_DtsDrcMode_eEnabled ? true : false;
        pSettings->codecSettings.dts.boost = codecSettings.codecSettings.dts.drcScaleLow;
        pSettings->codecSettings.dts.cut = codecSettings.codecSettings.dts.drcScaleHi;
        pSettings->codecSettings.dts.littleEndian = codecSettings.codecSettings.dts.littleEndian;
        break;
    case NEXUS_AudioCodec_eAdpcm:
        pSettings->codecSettings.adpcm.enableGain = codecSettings.codecSettings.adpcm.gain.enabled;
        pSettings->codecSettings.adpcm.gainFactor = codecSettings.codecSettings.adpcm.gain.factor;
        break;
    default:
        return;
    }
}

/***************************************************************************
Summary:
    Set codec-specific decoder settings
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_SetCodecSettings(
    NEXUS_AudioDecoderHandle handle,
    const NEXUS_AudioDecoderCodecSettings *pSettings
    )
{
    BAPE_DecoderCodecSettings codecSettings;
    BAVC_AudioCompressionStd avcCodec;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSettings);

    avcCodec = NEXUS_Audio_P_CodecToMagnum(pSettings->codec);

    BAPE_Decoder_GetCodecSettings(handle->channel, avcCodec, &codecSettings);
    switch ( pSettings->codec )
    {
    case NEXUS_AudioCodec_eAc3:
        codecSettings.codecSettings.ac3.drcMode = pSettings->codecSettings.ac3.drcMode;
        codecSettings.codecSettings.ac3.drcModeDownmix = pSettings->codecSettings.ac3.drcModeDownmix;
        codecSettings.codecSettings.ac3.drcScaleHi = pSettings->codecSettings.ac3.cut;
        codecSettings.codecSettings.ac3.drcScaleLow = pSettings->codecSettings.ac3.boost;
        codecSettings.codecSettings.ac3.drcScaleHiDownmix = pSettings->codecSettings.ac3.cutDownmix;
        codecSettings.codecSettings.ac3.drcScaleLowDownmix = pSettings->codecSettings.ac3.boostDownmix;
        codecSettings.codecSettings.ac3.stereoMode = pSettings->codecSettings.ac3.stereoDownmixMode;
        codecSettings.codecSettings.ac3.scale = pSettings->codecSettings.ac3.scale;
        codecSettings.codecSettings.ac3.scaleDownmix = pSettings->codecSettings.ac3.scaleDownmix;
        codecSettings.codecSettings.ac3.dialogNormalization = pSettings->codecSettings.ac3.dialogNormalization;
        codecSettings.codecSettings.ac3.dialogNormalizationValue = pSettings->codecSettings.ac3.dialogNormalizationValue;
        codecSettings.codecSettings.ac3.substreamId = pSettings->codecSettings.ac3.substreamId;
        break;
    case NEXUS_AudioCodec_eAc3Plus:
        codecSettings.codecSettings.ac3Plus.drcMode = pSettings->codecSettings.ac3Plus.drcMode;
        codecSettings.codecSettings.ac3Plus.drcModeDownmix = pSettings->codecSettings.ac3Plus.drcModeDownmix;
        codecSettings.codecSettings.ac3Plus.drcScaleHi = pSettings->codecSettings.ac3Plus.cut;
        codecSettings.codecSettings.ac3Plus.drcScaleLow = pSettings->codecSettings.ac3Plus.boost;
        codecSettings.codecSettings.ac3Plus.drcScaleHiDownmix = pSettings->codecSettings.ac3Plus.cutDownmix;
        codecSettings.codecSettings.ac3Plus.drcScaleLowDownmix = pSettings->codecSettings.ac3Plus.boostDownmix;
        codecSettings.codecSettings.ac3Plus.stereoMode = pSettings->codecSettings.ac3Plus.stereoDownmixMode;
        codecSettings.codecSettings.ac3Plus.scale = pSettings->codecSettings.ac3Plus.scale;
        codecSettings.codecSettings.ac3Plus.scaleDownmix = pSettings->codecSettings.ac3Plus.scaleDownmix;
        codecSettings.codecSettings.ac3Plus.dialogNormalization = pSettings->codecSettings.ac3Plus.dialogNormalization;
        codecSettings.codecSettings.ac3Plus.dialogNormalizationValue = pSettings->codecSettings.ac3Plus.dialogNormalizationValue;
        codecSettings.codecSettings.ac3Plus.substreamId = pSettings->codecSettings.ac3Plus.substreamId;
        break;
    case NEXUS_AudioCodec_eAacAdts:
    case NEXUS_AudioCodec_eAacLoas:
        codecSettings.codecSettings.aac.drcScaleHi = pSettings->codecSettings.aac.cut;
        codecSettings.codecSettings.aac.drcScaleLow = pSettings->codecSettings.aac.boost;
        codecSettings.codecSettings.aac.drcTargetLevel = pSettings->codecSettings.aac.drcTargetLevel;
        codecSettings.codecSettings.aac.downmixMode = pSettings->codecSettings.aac.downmixMode;
        codecSettings.codecSettings.aac.drcMode = pSettings->codecSettings.aac.drcMode;
        codecSettings.codecSettings.aac.drcDefaultLevel = pSettings->codecSettings.aac.drcDefaultLevel;
        codecSettings.codecSettings.aac.mpegConformanceMode = pSettings->codecSettings.aac.mpegConformanceMode;
        break;
    case NEXUS_AudioCodec_eAacPlusAdts:
    case NEXUS_AudioCodec_eAacPlusLoas:
        codecSettings.codecSettings.aacPlus.drcScaleHi = pSettings->codecSettings.aacPlus.cut;
        codecSettings.codecSettings.aacPlus.drcScaleLow = pSettings->codecSettings.aacPlus.boost;
        codecSettings.codecSettings.aacPlus.drcTargetLevel = pSettings->codecSettings.aacPlus.drcTargetLevel;
        codecSettings.codecSettings.aacPlus.downmixMode = pSettings->codecSettings.aacPlus.downmixMode;
        codecSettings.codecSettings.aacPlus.drcMode = pSettings->codecSettings.aacPlus.drcMode;
        codecSettings.codecSettings.aacPlus.drcDefaultLevel = pSettings->codecSettings.aacPlus.drcDefaultLevel;
        codecSettings.codecSettings.aacPlus.mpegConformanceMode = pSettings->codecSettings.aacPlus.mpegConformanceMode;
        break;
    case NEXUS_AudioCodec_eWmaPro:
        /* TODO: Nexus is exposing the older WMA Pro DRC mode.  This should switch from bool .. enum to match APE */
        if ( pSettings->codecSettings.wmaPro.dynamicRangeControlValid )
        {
            codecSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eHigh;
        }
        else
        {
            codecSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eDisabled;
        }
        /* TODO: Expose stereo mode */
        codecSettings.codecSettings.wmaPro.peakAmplitudeReference = pSettings->codecSettings.wmaPro.dynamicRangeControl.peakReference;
        codecSettings.codecSettings.wmaPro.desiredPeak = pSettings->codecSettings.wmaPro.dynamicRangeControl.peakTarget;
        codecSettings.codecSettings.wmaPro.rmsAmplitudeReference = pSettings->codecSettings.wmaPro.dynamicRangeControl.averageReference;
        codecSettings.codecSettings.wmaPro.desiredRms = pSettings->codecSettings.wmaPro.dynamicRangeControl.averageTarget;
        break;
    case NEXUS_AudioCodec_eDts:
    case NEXUS_AudioCodec_eDtsHd:
    case NEXUS_AudioCodec_eDtsLegacy: /* For DTS streams with legacy frame-sync.  These streams are something called as 14bits stream */
        codecSettings.codecSettings.dts.littleEndian = pSettings->codecSettings.dts.littleEndian;
        codecSettings.codecSettings.dts.mixLfeToPrimary = pSettings->codecSettings.dts.mixLfeToPrimary;
        codecSettings.codecSettings.dts.stereoMode = (BAPE_DtsStereoMode)pSettings->codecSettings.dts.stereoDownmixMode;
        codecSettings.codecSettings.dts.drcMode = pSettings->codecSettings.dts.enableDrc ? BAPE_DtsDrcMode_eEnabled : BAPE_DtsDrcMode_eDisabled;
        codecSettings.codecSettings.dts.drcScaleLow = pSettings->codecSettings.dts.boost;
        codecSettings.codecSettings.dts.drcScaleHi = pSettings->codecSettings.dts.cut;
        break;
    case NEXUS_AudioCodec_eAdpcm:
        codecSettings.codecSettings.adpcm.gain.enabled = pSettings->codecSettings.adpcm.enableGain;
        codecSettings.codecSettings.adpcm.gain.factor = pSettings->codecSettings.adpcm.gainFactor;
        break;
    default:
        return BERR_SUCCESS;
    }

    errCode = BAPE_Decoder_SetCodecSettings(handle->channel, &codecSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static unsigned NEXUS_AudioDecoder_P_GetAc3Bitrate(unsigned frameSizeCode)
{
    const unsigned bitrateTable[] = {

        32000,  /* '000000' 32 kbps  */
        32000,  /* '000001' 32 kbps  */
        40000,  /* '000010' 40 kbps  */
        40000,  /* '000011' 40 kbps  */
        48000,  /* '000100' 48 kbps  */
        48000,  /* '000101' 48 kbps  */
        56000,  /* '000110' 56 kbps  */
        56000,  /* '000111' 56 kbps  */
        64000,  /* '001000' 64 kbps  */
        64000,  /* '001001' 64 kbps  */
        80000,  /* '001010' 80 kbps  */
        80000,  /* '001011' 80 kbps  */
        96000,  /* '001100' 96 kbps  */
        96000,  /* '001101' 96 kbps  */
        112000, /* '001110' 112 kbps */
        112000, /* '001111' 112 kbps */

        128000, /* '010000' 128 kbps */
        128000, /* '010001' 128 kbps */
        160000, /* '010010' 160 kbps */
        160000, /* '010011' 160 kbps */
        192000, /* '010100' 192 kbps */
        192000, /* '010101' 192 kbps */
        224000, /* '010110' 224 kbps */
        224000, /* '010111' 224 kbps */
        256000, /* '011000' 256 kbps */
        256000, /* '011001' 256 kbps */
        320000, /* '011010' 320 kbps */
        320000, /* '011011' 320 kbps */
        384000, /* '011100' 384 kbps */
        384000, /* '011101' 384 kbps */
        448000, /* '011110' 448 kbps */
        448000, /* '011111' 448 kbps */

        512000, /* '100000' 512 kbps */
        512000, /* '100001' 512 kbps */
        576000, /* '100010' 576 kbps */
        576000, /* '100011' 576 kbps */
        640000, /* '100100' 640 kbps */
        640000  /* '100101' 640 kbps */
    };

    if ( frameSizeCode < 38 )
    {
        return bitrateTable[frameSizeCode];
    }
    else
    {
        return 0;
    }
}

/***************************************************************************
Summary:
    Get the current audio decoder status
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_GetStatus(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioDecoderStatus *pStatus   /* [out] current status */
    )
{
    unsigned depth=0, size=0;
    BAPE_DecoderStatus decoderStatus;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(NEXUS_AudioDecoderStatus));
    pStatus->started = handle->started;
    pStatus->locked = handle->locked;
    pStatus->codec = handle->programSettings.codec;
    pStatus->ptsType = NEXUS_PtsType_eInterpolatedFromInvalidPTS;
    pStatus->numFifoOverflows = handle->numFifoOverflows;
    pStatus->numFifoUnderflows = handle->numFifoUnderflows;

    if(!handle->running)
    {
        return BERR_SUCCESS;
    }

    BKNI_EnterCriticalSection();
    if ( handle->programSettings.pidChannel )
    {
        NEXUS_Rave_GetCdbBufferInfo_isr(handle->raveContext, &depth, &size);
    }
    BKNI_LeaveCriticalSection();

    pStatus->fifoDepth = depth;
    pStatus->fifoSize = size;

    pStatus->tsm = handle->tsmEnabled;
    pStatus->timebase = handle->apeStartSettings.stcIndex;
    pStatus->ptsErrorCount = handle->ptsErrorCount;
    pStatus->codec = handle->programSettings.codec;
    pStatus->numWatchdogs = g_numWatchdogs;

    if ( handle->programSettings.pidChannel )
    {
        NEXUS_RaveStatus raveStatus;
        NEXUS_Error errCode;
        LOCK_TRANSPORT();
        NEXUS_Rave_GetAudioFrameCount_priv(handle->raveContext, &pStatus->queuedFrames);
        errCode = NEXUS_Rave_GetStatus_priv(handle->raveContext, &raveStatus);
        UNLOCK_TRANSPORT();
        if ( NEXUS_SUCCESS == errCode )
        {
            pStatus->numBytesDecoded = raveStatus.numOutputBytes;
        }
    }

    /* Get decoder info */
    BAPE_Decoder_GetStatus(handle->channel, &decoderStatus);
    {
        unsigned frameLength, bitrate;

        pStatus->sampleRate = decoderStatus.sampleRate;
        pStatus->pts = decoderStatus.tsmStatus.ptsInfo.ui32CurrentPTS;
        pStatus->ptsType = decoderStatus.tsmStatus.ptsInfo.ePTSType == BAVC_PTSType_eCoded ? NEXUS_PtsType_eCoded :
            decoderStatus.tsmStatus.ptsInfo.ePTSType == BAVC_PTSType_eInterpolatedFromValidPTS ? NEXUS_PtsType_eInterpolatedFromValidPTS : NEXUS_PtsType_eInterpolatedFromInvalidPTS;
        pStatus->ptsStcDifference = decoderStatus.tsmStatus.ptsStcDifference;

        pStatus->framesDecoded = decoderStatus.framesDecoded;
        pStatus->frameErrors = decoderStatus.frameErrors;
        pStatus->dummyFrames = decoderStatus.dummyFrames;

        /* Convert codec to nexus type */
        pStatus->codec = NEXUS_Audio_P_MagnumToCodec(decoderStatus.codec);
        
        /* Handle specifics per-codec */
        switch ( decoderStatus.codec )
        {
        case BAVC_AudioCompressionStd_eMpegL1:
        case BAVC_AudioCompressionStd_eMpegL2:
        case BAVC_AudioCompressionStd_eMpegL3:
            pStatus->codec = (decoderStatus.codecStatus.mpeg.layer == 3)?NEXUS_AudioCodec_eMp3:NEXUS_AudioCodec_eMpeg;
            pStatus->codecStatus.mpeg.channelMode = decoderStatus.codecStatus.mpeg.mpegChannelMode;
            switch (  decoderStatus.codecStatus.mpeg.layer )
            {
            default:
            case 1:
                pStatus->codecStatus.mpeg.layer = NEXUS_AudioMpegLayer_e1;
                pStatus->codec = NEXUS_AudioCodec_eMpeg;
                break;
            case 2:
                pStatus->codecStatus.mpeg.layer = NEXUS_AudioMpegLayer_e2;
                pStatus->codec = NEXUS_AudioCodec_eMpeg;
                break;
            case 3:
                pStatus->codecStatus.mpeg.layer = NEXUS_AudioMpegLayer_e3;
                pStatus->codec = NEXUS_AudioCodec_eMp3;
                break;
            }
            pStatus->codecStatus.mpeg.emphasis = decoderStatus.codecStatus.mpeg.emphasisMode;
            pStatus->codecStatus.mpeg.original = decoderStatus.codecStatus.mpeg.original;
            pStatus->codecStatus.mpeg.copyright = decoderStatus.codecStatus.mpeg.copyright;
            pStatus->codecStatus.mpeg.bitrate = decoderStatus.codecStatus.mpeg.bitRate;
            if (  pStatus->sampleRate > 0 )
            {
                /* MPEG audio uses a CDB sync, so the frame count is bogus.  Calculate based on frame size and CDB depth */
                bitrate = (decoderStatus.codecStatus.mpeg.bitRate>0)?1000*decoderStatus.codecStatus.mpeg.bitRate:128000;
                if ( decoderStatus.codecStatus.mpeg.layer == 1 )
                {
                    frameLength = (48*bitrate)/pStatus->sampleRate;
                }
                else
                {
                    frameLength = (144*bitrate)/pStatus->sampleRate;
                }
                if ( frameLength == 0 )
                {
                    pStatus->queuedFrames = 0;
                }
                else
                {
                    pStatus->queuedFrames = pStatus->fifoDepth/frameLength;
                }
                BDBG_MSG(("Queued Frames %d bitrate %d framelength %d samplerate %d", pStatus->queuedFrames, bitrate, frameLength, pStatus->sampleRate));
            }
            break;
        case BAVC_AudioCompressionStd_eAacLoas:
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
        case BAVC_AudioCompressionStd_eAacPlusAdts:
            /* Convert channel mode to aac acmod */
            switch ( decoderStatus.codecStatus.aac.channelMode )
            {
            case BAPE_ChannelMode_e1_0: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eOneCenter_1_0_C; break;
            case BAPE_ChannelMode_e1_1: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eTwoMono_1_ch1_ch2; break;
            case BAPE_ChannelMode_e2_0: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eTwoChannel_2_0_L_R; break;
            case BAPE_ChannelMode_e3_0: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eThreeChannel_3_0_L_C_R; break;
            case BAPE_ChannelMode_e2_1: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S; break;
            case BAPE_ChannelMode_e3_1: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eFourChannel_3_1_L_C_R_S; break;
            case BAPE_ChannelMode_e2_2: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR; break;
            case BAPE_ChannelMode_e3_2: pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eFiveChannel_3_2_L_C_R_SL_SR; break;
            default:                    pStatus->codecStatus.aac.acmod = NEXUS_AudioAacAcmod_eMax; break;
            }
            pStatus->codecStatus.aac.profile = decoderStatus.codecStatus.aac.profile;
            pStatus->codecStatus.aac.bitrate = decoderStatus.codecStatus.aac.bitRate;
            pStatus->codecStatus.aac.lfe = decoderStatus.codecStatus.aac.lfe;
            pStatus->codecStatus.aac.pseudoSurround = decoderStatus.codecStatus.aac.pseudoSurround;
            pStatus->codecStatus.aac.stereoMatrix = decoderStatus.codecStatus.aac.stereoMatrix;
            pStatus->codecStatus.aac.matrixIndex = decoderStatus.codecStatus.aac.matrixIndex;
            pStatus->codecStatus.aac.numLfeChannels = decoderStatus.codecStatus.aac.numLfeChannels;
            pStatus->codecStatus.aac.numBackChannels = decoderStatus.codecStatus.aac.numBackChannels;
            pStatus->codecStatus.aac.numSideChannels = decoderStatus.codecStatus.aac.numSideChannels;
            pStatus->codecStatus.aac.numFrontChannels = decoderStatus.codecStatus.aac.numFrontChannels;
            break;
        case BAVC_AudioCompressionStd_eAc3:
        case BAVC_AudioCompressionStd_eAc3Plus:
            pStatus->codec = (decoderStatus.codec == BAVC_AudioCompressionStd_eAc3)?NEXUS_AudioCodec_eAc3:NEXUS_AudioCodec_eAc3Plus;
            pStatus->codecStatus.ac3.bitStreamId = decoderStatus.codecStatus.ac3.bitstreamId;
            pStatus->codecStatus.ac3.acmod = decoderStatus.codecStatus.ac3.acmod;
            pStatus->codecStatus.ac3.frameSizeCode = decoderStatus.codecStatus.ac3.frameSizeCode;
            pStatus->codecStatus.ac3.bitrate = NEXUS_AudioDecoder_P_GetAc3Bitrate(decoderStatus.codecStatus.ac3.frameSizeCode);
            pStatus->codecStatus.ac3.lfe = decoderStatus.codecStatus.ac3.lfe;
            pStatus->codecStatus.ac3.copyright = decoderStatus.codecStatus.ac3.copyright;
            BDBG_CASSERT((int)NEXUS_AudioAc3DependentFrameChannelMap_eMax==(int)BAPE_Ac3DependentFrameChannelMap_eMax);
            pStatus->codecStatus.ac3.dependentFrameChannelMap = (NEXUS_AudioAc3DependentFrameChannelMap)decoderStatus.codecStatus.ac3.dependentFrameChannelMap;
            break;
        case BAVC_AudioCompressionStd_eDts:
        case BAVC_AudioCompressionStd_eDtshd:
        case BAVC_AudioCompressionStd_eDtsLegacy:
            pStatus->codecStatus.dts.amode = decoderStatus.codecStatus.dts.amode;
            pStatus->codecStatus.dts.pcmResolution = decoderStatus.codecStatus.dts.pcmResolution;
            pStatus->codecStatus.dts.copyHistory = decoderStatus.codecStatus.dts.copyHistory;
            pStatus->codecStatus.dts.extensionDescriptor = decoderStatus.codecStatus.dts.extensionDescriptor;
            pStatus->codecStatus.dts.bitRate = decoderStatus.codecStatus.dts.bitRate;
            pStatus->codecStatus.dts.version = decoderStatus.codecStatus.dts.version;
            pStatus->codecStatus.dts.esFormat = decoderStatus.codecStatus.dts.esFormat;
            pStatus->codecStatus.dts.lfe = decoderStatus.codecStatus.dts.lfe;
            pStatus->codecStatus.dts.extensionPresent = decoderStatus.codecStatus.dts.extensionPresent;
            pStatus->codecStatus.dts.crc = decoderStatus.codecStatus.dts.crc;
            pStatus->codecStatus.dts.hdcdFormat = decoderStatus.codecStatus.dts.hdcdFormat;
            pStatus->codecStatus.dts.drc = decoderStatus.codecStatus.dts.drc;
            pStatus->codecStatus.dts.downmixCoefficients = decoderStatus.codecStatus.dts.downmixCoefficients;
            pStatus->codecStatus.dts.neo = decoderStatus.codecStatus.dts.neo;
            pStatus->codecStatus.dts.frameSize = decoderStatus.codecStatus.dts.frameSize;
            pStatus->codecStatus.dts.numChannels = decoderStatus.codecStatus.dts.numChannels;
            pStatus->codecStatus.dts.pcmFrameSize = decoderStatus.codecStatus.dts.pcmFrameSize;
            pStatus->codecStatus.dts.numPcmBlocks = decoderStatus.codecStatus.dts.numPcmBlocks;
            break;
        case BAVC_AudioCompressionStd_eWmaStd:
        case BAVC_AudioCompressionStd_eWmaStdTs:
            pStatus->codecStatus.wma.bitRate = decoderStatus.codecStatus.wma.bitRate;
            pStatus->codecStatus.wma.original = decoderStatus.codecStatus.wma.original;
            pStatus->codecStatus.wma.copyright = decoderStatus.codecStatus.wma.copyright;
            pStatus->codecStatus.wma.crc = decoderStatus.codecStatus.wma.crc;
            pStatus->codecStatus.wma.stereo = decoderStatus.codecStatus.wma.channelMode == (BAPE_ChannelMode_e2_0)?true:false;
            pStatus->codecStatus.wma.version = decoderStatus.codecStatus.wma.version;
            break;
        case BAVC_AudioCompressionStd_eWmaPro:
            pStatus->codecStatus.wmaPro.bitRate = decoderStatus.codecStatus.wmaPro.bitRate;
            pStatus->codecStatus.wmaPro.original = decoderStatus.codecStatus.wmaPro.original;
            pStatus->codecStatus.wmaPro.copyright = decoderStatus.codecStatus.wmaPro.copyright;
            pStatus->codecStatus.wmaPro.crc = decoderStatus.codecStatus.wmaPro.crc;
            pStatus->codecStatus.wmaPro.lfe = decoderStatus.codecStatus.wmaPro.lfe;
            pStatus->codecStatus.wmaPro.version = decoderStatus.codecStatus.wmaPro.version;
            pStatus->codecStatus.wmaPro.stereoMode = decoderStatus.codecStatus.wmaPro.stereoMode;
            switch ( decoderStatus.codecStatus.wmaPro.channelMode )
            {
            case BAPE_ChannelMode_e1_0:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e1_0_C;
                break;
            case BAPE_ChannelMode_e2_0:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e2_0_LR;
                break;
            case BAPE_ChannelMode_e3_0:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e3_0_LCR;
                break;
            case BAPE_ChannelMode_e2_1:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e2_1_LRS;
                break;
            case BAPE_ChannelMode_e3_1:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e3_1_LCRS;
                break;
            case BAPE_ChannelMode_e2_2:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e2_2_LRLsRs;
                break;
            case BAPE_ChannelMode_e3_2:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_e3_2_LCRLsRs;
                break;
            default:
                pStatus->codecStatus.wmaPro.acmod = NEXUS_AudioWmaProAcmod_eUndefined;
                break;
            }
            break;
        case BAVC_AudioCompressionStd_ePcmWav:
            pStatus->codecStatus.pcmWav.numChannels = decoderStatus.codecStatus.pcmWav.numChannels;
            break;
        case BAVC_AudioCompressionStd_eAmr:
            pStatus->codecStatus.amr.bitRate = decoderStatus.codecStatus.amr.bitRate;
            break;
        case BAVC_AudioCompressionStd_eDra:
            pStatus->codecStatus.dra.frameSize = decoderStatus.codecStatus.dra.frameSize;
            pStatus->codecStatus.dra.numBlocks = decoderStatus.codecStatus.dra.numBlocks;
            pStatus->codecStatus.dra.acmod = decoderStatus.codecStatus.dra.acmod;
            pStatus->codecStatus.dra.lfe = decoderStatus.codecStatus.dra.lfe;
            pStatus->codecStatus.dra.stereoMode = decoderStatus.codecStatus.dra.stereoMode;
            break;
        case BAVC_AudioCompressionStd_eCook:
            pStatus->codecStatus.cook.stereo = decoderStatus.codecStatus.cook.stereo;
            pStatus->codecStatus.cook.frameSize = decoderStatus.codecStatus.cook.frameSize;
            break;
        default:
            /* No specifics for this codec */
            break;
        }
    }

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Get raw channel status information from the decoder
    
Description:
    When the decoder is connected to a digital input, this routine can
    return the raw channel status bit information from the input device.
    Currently, this applies to HDMI or SPDIF inputs only.  This routine
    will return an error if not connected to a digital input.
    
See Also:
    NEXUS_SpdifOutput_SetRawChannelStatus
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_GetRawChannelStatus(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioRawChannelStatus *pStatus   /* [out] current status */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
    Get an audio connector for use in the audio mixer
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioDecoder_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioConnectorType type
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(type < NEXUS_AudioConnectorType_eMax);
    return &handle->connectors[type];
}

NEXUS_Error NEXUS_AudioDecoder_ApplySettings_priv(
    NEXUS_AudioDecoderHandle handle
    )
{
    BAPE_DecoderTsmSettings tsmSettings;
    BAPE_DecoderSettings decoderSettings;
    BAPE_MixerInputVolume volume;
    unsigned i,j;
    BERR_Code errCode;
    bool forceMute=false;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);

    BAPE_Decoder_GetTsmSettings(handle->channel, &tsmSettings);
    tsmSettings.ptsOffset = handle->settings.ptsOffset;

#if NEXUS_HAS_SYNC_CHANNEL
    tsmSettings.ptsOffset += handle->sync.settings.delay;
 #endif

#if NEXUS_HAS_ASTM
    if (handle->astm.settings.enableAstm)
    {
        BDBG_MSG(("ASTM is setting the sync limit for audio channel %p to %d", handle, handle->astm.settings.syncLimit));
        tsmSettings.thresholds.syncLimit = handle->astm.settings.syncLimit;
        /* PR49489 20081201 bandrews - added for audio master mode */
        BDBG_MSG(("ASTM is %s playback mode for audio channel %p", handle->astm.settings.enablePlayback ? "enabling" : "disabling", handle));
        tsmSettings.playback = handle->astm.settings.enablePlayback;
        tsmSettings.ptsOffset += handle->astm.settings.ptsOffset;
    }
    else
    {
        tsmSettings.thresholds.syncLimit = 0;
    }
#endif

    if ( 0 == handle->settings.discardThreshold )
    {
        tsmSettings.thresholds.discard = (handle->isPlayback)?30000:3000;   /* ms */
    }
    else
    {
        tsmSettings.thresholds.discard = handle->settings.discardThreshold;
    }

    if ( 0 == handle->settings.gaThreshold )
    {
        NEXUS_PidChannelStatus pidChannelStatus;

        BKNI_Memset(&pidChannelStatus, 0, sizeof(pidChannelStatus));
        if ( NULL != handle->programSettings.pidChannel )
        {
            (void)NEXUS_PidChannel_GetStatus(handle->programSettings.pidChannel, &pidChannelStatus);
        }
        tsmSettings.thresholds.grossAdjustment = (pidChannelStatus.originalTransportType == NEXUS_TransportType_eAvi) ? 0x30 : 0x8;
    }
    else
    {
        tsmSettings.thresholds.grossAdjustment = handle->settings.gaThreshold;
    }
    BAPE_Decoder_SetTsmSettings(handle->channel, &tsmSettings);

    NEXUS_AudioDecoder_P_SetTsm(handle);

    BAPE_Decoder_GetSettings(handle->channel, &decoderSettings);
    switch ( handle->settings.outputLfeMode )
    {
    default:
        decoderSettings.outputLfe = true;
        break;
    case NEXUS_AudioDecoderOutputLfeMode_eOff:
        decoderSettings.outputLfe = false;
        break;
    }
    switch ( handle->settings.outputMode )
    {
    default:
    case NEXUS_AudioDecoderOutputMode_eAuto:
        decoderSettings.outputMode = BAPE_ChannelMode_e3_2;
        break;
    case NEXUS_AudioDecoderOutputMode_e1_0:
        decoderSettings.outputMode = BAPE_ChannelMode_e1_0;
        break;
    case NEXUS_AudioDecoderOutputMode_e1_1:
        decoderSettings.outputMode = BAPE_ChannelMode_e1_1;
        break;
    case NEXUS_AudioDecoderOutputMode_e2_0:
        decoderSettings.outputMode = BAPE_ChannelMode_e2_0;
        break;
    case NEXUS_AudioDecoderOutputMode_e3_0:
        decoderSettings.outputMode = BAPE_ChannelMode_e3_0;
        break;
    case NEXUS_AudioDecoderOutputMode_e2_1:
        decoderSettings.outputMode = BAPE_ChannelMode_e2_1;
        break;
    case NEXUS_AudioDecoderOutputMode_e3_1:
        decoderSettings.outputMode = BAPE_ChannelMode_e3_1;
        break;
    case NEXUS_AudioDecoderOutputMode_e2_2:
        decoderSettings.outputMode = BAPE_ChannelMode_e2_2;
        break;
    case NEXUS_AudioDecoderOutputMode_e3_2:
        decoderSettings.outputMode = BAPE_ChannelMode_e3_2;
        break;
    case NEXUS_AudioDecoderOutputMode_e3_4:
        decoderSettings.outputMode = BAPE_ChannelMode_e3_4;
        break;
    }
    switch ( handle->settings.dualMonoMode )
    {
    default:
    case NEXUS_AudioDecoderDualMonoMode_eStereo:
        decoderSettings.dualMonoMode = BAPE_DualMonoMode_eStereo;
        break;
    case NEXUS_AudioDecoderDualMonoMode_eLeft:
        decoderSettings.dualMonoMode = BAPE_DualMonoMode_eLeft;
        break;
    case NEXUS_AudioDecoderDualMonoMode_eRight:
        decoderSettings.dualMonoMode = BAPE_DualMonoMode_eRight;
        break;
    case NEXUS_AudioDecoderDualMonoMode_eMix:
        decoderSettings.dualMonoMode = BAPE_DualMonoMode_eMix;
        break;
    }

    if ( handle->trickState.rate == 0 )
    {
        decoderSettings.decodeRate = BAPE_NORMAL_DECODE_RATE;
    }
    else if ( handle->trickState.rate > (2*NEXUS_NORMAL_DECODE_RATE) || handle->trickState.rate < (NEXUS_NORMAL_DECODE_RATE/2) )
    {
        BDBG_MSG(("Audio trick modes are only supported between 0.5x and 2x normal rate.  Muting audio decoder."));
        forceMute = true;
        decoderSettings.decodeRate = BAPE_NORMAL_DECODE_RATE;
    }
    else
    {
        decoderSettings.decodeRate = (handle->trickState.rate * BAPE_NORMAL_DECODE_RATE)/ NEXUS_NORMAL_DECODE_RATE;
    }
    decoderSettings.loudnessEquivalenceEnabled = handle->settings.loudnessEquivalenceEnabled;
    decoderSettings.ancillaryDataEnabled = handle->settings.ancillaryDataEnabled;
    errCode = BAPE_Decoder_SetSettings(handle->channel, &decoderSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    /* Apply volume settings - but only if an output has ever been connected by the user.
       Otherwise this forces a Shutdown() call in the application. */
    if ( handle->connectors[NEXUS_AudioConnectorType_eStereo].pMixerData )
    {
        NEXUS_AudioInput_P_GetVolume(&handle->connectors[NEXUS_AudioConnectorType_eStereo], &volume);
    }
    else if ( handle->connectors[NEXUS_AudioConnectorType_eMultichannel].pMixerData )
    {
        NEXUS_AudioInput_P_GetVolume(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], &volume);
    }
    else if ( handle->connectors[NEXUS_AudioConnectorType_eMono].pMixerData )
    {
        NEXUS_AudioInput_P_GetVolume(&handle->connectors[NEXUS_AudioConnectorType_eMono], &volume);
    }
    else
    {
        /* Compressed outputs do not have volume control */
        goto skip_volume;
    }
    for ( i = 0; i < NEXUS_AudioChannel_eMax; i++ )
    {
        for ( j = 0; j < NEXUS_AudioChannel_eMax; j++ )
        {
            volume.coefficients[i][j] = handle->settings.volumeMatrix[j][i];
        }
    }       
    volume.muted = handle->settings.muted || handle->trickState.muted 
#if NEXUS_HAS_SYNC_CHANNEL
        /* TODO: if we are not running, or if we are running already muted, and sync wants to keep it muted, fine;
        but if we are already running, have unmuted, and sync wants to re-mute, don't! */
        || handle->sync.mute 
#endif
        || forceMute;
    if ( handle->connectors[NEXUS_AudioConnectorType_eStereo].pMixerData )
    {
        errCode = NEXUS_AudioInput_P_SetVolume(&handle->connectors[NEXUS_AudioConnectorType_eStereo], &volume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    if ( handle->connectors[NEXUS_AudioConnectorType_eMultichannel].pMixerData )
    {
        errCode = NEXUS_AudioInput_P_SetVolume(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], &volume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    if ( handle->connectors[NEXUS_AudioConnectorType_eMono].pMixerData )
    {
        errCode = NEXUS_AudioInput_P_SetVolume(&handle->connectors[NEXUS_AudioConnectorType_eMono], &volume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
skip_volume:
    return BERR_SUCCESS;
}
        
#if NEXUS_HAS_ASTM
void NEXUS_AudioDecoder_GetAstmSettings_priv(
    NEXUS_AudioDecoderHandle audioDecoder, 
    NEXUS_AudioDecoderAstmSettings * pAstmSettings  /* [out] */
    )
{
    NEXUS_ASSERT_MODULE();

    *pAstmSettings = audioDecoder->astm.settings;
}

NEXUS_Error NEXUS_AudioDecoder_SetAstmSettings_priv( 
    NEXUS_AudioDecoderHandle audioDecoder, 
    const NEXUS_AudioDecoderAstmSettings * pAstmSettings 
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    NEXUS_ASSERT_MODULE();

    /* copy settings as-is, this way ASTM will always get what it set */
    audioDecoder->astm.settings = *pAstmSettings;

    /* if ASTM is internally permitted, apply settings */
    rc = NEXUS_AudioDecoder_ApplySettings_priv(audioDecoder);

    return rc;
}
    
NEXUS_Error NEXUS_AudioDecoder_GetAstmStatus_isr( 
    NEXUS_AudioDecoderHandle audioDecoder, 
    NEXUS_AudioDecoderAstmStatus * pAstmStatus  /* [out] */
    )
{
    BAPE_DecoderTsmStatus tsmStatus;
    BERR_Code rc;

    BKNI_ASSERT_ISR_CONTEXT();

    rc = BAPE_Decoder_GetTsmStatus_isr(audioDecoder->channel, &tsmStatus);
    audioDecoder->astm.status.ptsStcDiff = tsmStatus.ptsStcDifference;

    *pAstmStatus = audioDecoder->astm.status;

    return 0;
}
#endif

#if NEXUS_HAS_SYNC_CHANNEL
/* TODO */
#if 0
static NEXUS_Error NEXUS_AudioDecoder_P_SetCompressedSyncMute(NEXUS_AudioDecoderHandle decoder, bool muted)
{
    NEXUS_AudioOutput output;
    NEXUS_Error rc;
    int i;

    for ( i = 0; i < NEXUS_AUDIO_MAX_OUTPUTS; i++ )
    {
        output = decoder->outputLists[NEXUS_AudioDecoderConnectorType_eCompressed].outputs[i];
        if ( output )
        {
            BDBG_MSG(("setting output mute to %d", muted));
            rc=NEXUS_AudioOutput_P_SetSyncMute(output, muted);
            if ( rc )
            {
                return BERR_TRACE(rc);
            }
        }
    }

    return NEXUS_SUCCESS;
}
#endif

static NEXUS_Error NEXUS_AudioDecoder_P_SetSyncMute(NEXUS_AudioDecoderHandle decoder, bool muted)
{
    bool wasMuted = false;

    /* if sync called this while decoder was not running, it was to set persistent startup behavior */
    if (!decoder->running)
    {
        BDBG_MSG(("SetSyncMute(%u): start muted = %d", decoder->index, muted));
        decoder->sync.startMuted = muted;
    }

    wasMuted = decoder->sync.mute;
    decoder->sync.mute = muted;

    /* if we are running, and unmuting */    
    if (decoder->running && wasMuted && !muted)
    {
        /* the ApplySettings call is in SetSyncSettings_priv now */
        
        BDBG_MSG(("SetSyncMute(%u): unmute", decoder->index));

/* TODO */
#if 0
#if B_HAS_TRC
        if (decoder->index == 0) {
            BTRC_TRACE(ChnChange_SyncUnmuteAudio, STOP);
            if (BTRC_MODULE_HANDLE(ChnChange_DecodeStopAudio)->stats[0].count) {
                BTRC_TRACE(ChnChange_Total_Audio, STOP);
                BKNI_SetEvent(decoder->channelChangeReportEvent);
            }
        }
#endif
#endif
    }

    return NEXUS_SUCCESS;
}

void NEXUS_AudioDecoder_GetSyncSettings_priv(
    NEXUS_AudioDecoderHandle audioDecoder, 
    NEXUS_AudioInputSyncSettings *pSyncSettings  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(audioDecoder, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSyncSettings);
    *pSyncSettings = audioDecoder->sync.settings;
}
    
NEXUS_Error NEXUS_AudioDecoder_SetSyncSettings_priv( 
    NEXUS_AudioDecoderHandle audioDecoder, 
    const NEXUS_AudioInputSyncSettings *pSyncSettings 
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BDBG_OBJECT_ASSERT(audioDecoder, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSyncSettings);
    
    audioDecoder->sync.settings = *pSyncSettings;

    rc = NEXUS_AudioDecoder_P_SetSyncMute(audioDecoder, audioDecoder->sync.settings.mute);
    if (rc) BERR_TRACE(rc);

    /* here we must apply settings, because pts offset and mute are both handled there */
    rc = NEXUS_AudioDecoder_ApplySettings_priv(audioDecoder);
    if (rc) BERR_TRACE(rc);

    return rc;
}
    
NEXUS_Error NEXUS_AudioDecoder_GetSyncStatus_isr( 
    NEXUS_AudioDecoderHandle audioDecoder, 
    NEXUS_AudioInputSyncStatus *pSyncStatus  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(audioDecoder, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pSyncStatus);
    BKNI_Memset(pSyncStatus, 0, sizeof(*pSyncStatus));
    pSyncStatus->started = audioDecoder->running;
    pSyncStatus->digital = true;
    pSyncStatus->dsolaEnabled = false;
    BAPE_GetDecoderPathDelay(NEXUS_AUDIO_DEVICE_HANDLE, &pSyncStatus->delay);
    /* for 64 sample FMM delay -> this gets us within 0 - 1 ms, since the FMM 
    delay really depends on Fs. 32K -> 2 ms, 44.1K -> 1.45 ms, 48K -> 1.33 ms */
    pSyncStatus->delay += 1; 
    return BERR_SUCCESS;
}
#endif

bool NEXUS_AudioDecoder_P_IsRunning(NEXUS_AudioDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    /* This is slightly dangerous with trick modes, because a trick mute may cause this to return false.
       But, it's required for HDMI/SPDIF inputs where the decoder actually "starts stopped".  */
    return handle->running;
}

void NEXUS_AudioDecoder_P_Reset(void)
{
    unsigned i;

    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_AUDIO_DECODERS; i++ )
    {
        if ( g_decoders[i].opened && g_decoders[i].running && NULL == g_decoders[i].programSettings.input )
        {
            NEXUS_Rave_Disable_priv(g_decoders[i].raveContext);
            NEXUS_Rave_Flush_priv(g_decoders[i].raveContext);
        }
    }
    UNLOCK_TRANSPORT();

    /* Process watchdog event */
    BAPE_ProcessWatchdogInterrupt(NEXUS_AUDIO_DEVICE_HANDLE);

    /* Reset AudioMuxOutput objects */
    NEXUS_AudioMuxOutput_P_WatchdogReset();

    /* Restart RAVE contexts */
    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_AUDIO_DECODERS; i++ )
    {
        if ( g_decoders[i].opened && g_decoders[i].running && NULL == g_decoders[i].programSettings.input )
        {
            NEXUS_Rave_Enable_priv(g_decoders[i].raveContext);
        }
    }
    UNLOCK_TRANSPORT();

#if NEXUS_HAS_ASTM
    for ( i = 0; i < NEXUS_NUM_AUDIO_DECODERS; i++ )
    {
        if ( g_decoders[i].opened && g_decoders[i].running && g_decoders[i].astm.settings.enableAstm )
        {
            NEXUS_Callback astm_watchdog_isr = g_decoders[i].astm.settings.watchdog_isr;
            BDBG_MSG(("Audio channel %p is notifying ASTM of its watchdog recovery", &g_decoders[i]));
            if (astm_watchdog_isr)
            {
                BKNI_EnterCriticalSection();
                astm_watchdog_isr(g_decoders[i].astm.settings.callbackContext, 0);
                BKNI_LeaveCriticalSection();
            }
        }
    }
#endif       
}

static void NEXUS_AudioDecoder_P_Watchdog(void *pParam)
{    
    bool corePending=false;

    BSTD_UNUSED(pParam);    

    g_numWatchdogs++;

    /* Check if core dump support is enabled.  If so, spin and wait for it to complete. */
    if ( g_NEXUS_audioModuleData.settings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eCoreDump].enabled )
    {
        /* Poll for core dump to finish and inform application */
        unsigned retries = 100;

        while ( BDSP_Raaga_GetCoreDumpStatus(g_NEXUS_audioModuleData.dspHandle, 0) == BDSP_Raaga_FwStatus_eCoreDumpInProgress )
        {
            BKNI_Sleep(1);
            if ( 0 == --retries )
            {
                break;
            }
        }

        if ( BDSP_Raaga_GetCoreDumpStatus(g_NEXUS_audioModuleData.dspHandle, 0) == BDSP_Raaga_FwStatus_eCoreDumpComplete )
        {
            corePending = true;
        }
    }

    if ( corePending )
    {
        BDBG_WRN(("Audio watchdog reset postponed for core dump retrieval"));
        g_NEXUS_audioModuleData.watchdogDeferred = true;
    }
    else if ( !NEXUS_GetEnv("no_watchdog") )
    {
        /* Just restart now */
        NEXUS_AudioDecoder_P_Reset();
    }
    else
    {
        BDBG_WRN(("Watchdog Processing Disabled"));
    }
}

static void NEXUS_AudioDecoder_P_SampleRate(void *pParam)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam;

    if(pDecoder->settings.wideGaThreshold) {
        BAPE_DecoderTsmStatus tsmStatus;
        BAPE_DecoderTsmSettings tsmSettings;
        uint32_t new_threshold;

        BAPE_Decoder_GetTsmStatus(pDecoder->channel, &tsmStatus);

        new_threshold = (tsmStatus.lastFrameLength*3)/2;

        BAPE_Decoder_GetTsmSettings(pDecoder->channel, &tsmSettings);

        BDBG_WRN(("Adjusting GA threshold to: %d (was %d)", new_threshold, tsmSettings.thresholds.grossAdjustment));

        tsmSettings.thresholds.grossAdjustment = new_threshold;

        (void)BAPE_Decoder_SetTsmSettings(pDecoder->channel, &tsmSettings);
    }

    BDBG_MSG(("Sample Rate decoder[%d]", pDecoder->index));    
}

static void NEXUS_AudioDecoder_P_ChannelChangeReport(void * context)
{
    BSTD_UNUSED(context);
}

static void NEXUS_AudioDecoder_P_FirstPts_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus)
{
    NEXUS_Error errCode;
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;

    BSTD_UNUSED(param2);

    BDBG_MSG(("audio[%d] First PTS %08x", pDecoder->index, pTsmStatus->ptsInfo.ui32CurrentPTS));
    if ( pDecoder->programSettings.stcChannel )
    {
        NEXUS_IsrCallback_Fire_isr(pDecoder->firstPtsCallback);
        errCode = NEXUS_StcChannel_RequestStc_isr(pDecoder->programSettings.stcChannel, pDecoder->stcDecoderType, &pTsmStatus->ptsInfo);
        if ( errCode )
        {
            errCode=BERR_TRACE(errCode);
        }
    }    
#if NEXUS_HAS_ASTM
    if (pDecoder->astm.settings.enableAstm)
    {
        pDecoder->astm.status.pts = pTsmStatus->ptsInfo.ui32CurrentPTS;

        if (pDecoder->astm.settings.firstPts_isr)
        {
            pDecoder->astm.settings.firstPts_isr(pDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif /* NEXUS_HAS_ASTM */
}

static void NEXUS_AudioDecoder_P_AudioTsmFail_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;
#if NEXUS_HAS_ASTM
    BAPE_DecoderTsmStatus tsmStatus;
#endif
    NEXUS_Error errCode;

    uint32_t stc;

    BSTD_UNUSED(param2);
    BDBG_ASSERT(NULL != pTsmStatus);

    BDBG_MSG(("audio[%d] Tsm fail: PTS 0x%08x, type %d",
        pDecoder->index, pTsmStatus->ptsInfo.ui32CurrentPTS, pTsmStatus->ptsInfo.ePTSType));

    NEXUS_StcChannel_GetStc_isr(pDecoder->programSettings.stcChannel, &stc);

    /* If we're in a non-standard STC trick mode */
    if ( pDecoder->trickState.stcTrickEnabled && (pDecoder->trickState.rate < 500 || pDecoder->trickState.rate > 2000) )
    {
        /* in STC trick mode, PTS might lag the STC because of decoder drop algorithm. don't reset STC in this case. */
        if ( stc > pTsmStatus->ptsInfo.ui32CurrentPTS && stc - pTsmStatus->ptsInfo.ui32CurrentPTS < 45000 * 8 )
        {
            return;
        }
    }

    BDBG_MSG(("pts2stcphase: %d", pTsmStatus->ptsStcDifference));

#if NEXUS_HAS_ASTM
    if (pDecoder->astm.settings.enableAstm && pDecoder->astm.settings.syncLimit > 0)
    {
        tsmStatus = *pTsmStatus;       
        tsmStatus.ptsInfo.ui32CurrentPTS = (uint32_t)((int32_t)stc - tsmStatus.ptsStcDifference);
        pTsmStatus = &tsmStatus;
    }
#endif /* NEXUS_HAS_ASTM */

    /* PR:52308 ignore PTS errors for non-XPT inputs - we can't do anything about them from stcchannel/pcrlib anyway */
    if (!pDecoder->programSettings.input)
    {
        NEXUS_IsrCallback_Fire_isr(pDecoder->ptsErrorCallback);
        errCode = NEXUS_StcChannel_PtsError_isr(pDecoder->programSettings.stcChannel, pDecoder->stcDecoderType, &pTsmStatus->ptsInfo);
        if (errCode)
        {
            errCode=BERR_TRACE(errCode);
            /* keep going */
        }
    }

#if NEXUS_HAS_ASTM
    if (pDecoder->astm.settings.enableAstm)
    {
        pDecoder->astm.status.pts = pTsmStatus->ptsInfo.ui32CurrentPTS;

        if (pDecoder->astm.settings.tsmFail_isr)
        {
            pDecoder->astm.settings.tsmFail_isr(pDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif /* NEXUS_HAS_ASTM */

    pDecoder->ptsErrorCount++;    
}

static void NEXUS_AudioDecoder_P_AudioTsmPass_isr(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;

    BSTD_UNUSED(param2);
    BSTD_UNUSED(pTsmStatus);
    BDBG_ASSERT(NULL != pTsmStatus);

    BDBG_MSG(("audio[%d] TSM pass: PTS 0x%08x, type %d",
        pDecoder->index, pTsmStatus->ptsInfo.ui32CurrentPTS, pTsmStatus->ptsInfo.ePTSType));

#if NEXUS_HAS_ASTM
    if (pDecoder->astm.settings.enableAstm)
    {
        pDecoder->astm.status.pts = pTsmStatus->ptsInfo.ui32CurrentPTS;

        if (pDecoder->astm.settings.tsmPass_isr)
        {
            pDecoder->astm.settings.tsmPass_isr(pDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif
}

static void NEXUS_AudioDecoder_P_SampleRateChange_isr(void *pParam1, int param2, BAVC_AudioSamplingRate sampleRate)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;

    BSTD_UNUSED(param2);

    BDBG_MSG(("audio[%d] Sampling Rate Info, samplingRate=%x", pDecoder->index, sampleRate));

    BKNI_SetEvent_isr(pDecoder->sampleRateEvent);
    NEXUS_IsrCallback_Fire_isr(pDecoder->sourceChangeAppCallback);

#if NEXUS_HAS_SYNC_CHANNEL
    if (pDecoder->sync.settings.sampleRateCallback_isr) 
    {
        (*pDecoder->sync.settings.sampleRateCallback_isr)(pDecoder->sync.settings.callbackContext, 0);
    }
#endif
}

static void NEXUS_AudioDecoder_P_Lock_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;

    pDecoder->locked = param2;
    BDBG_MSG(("Decoder %u %s", pDecoder->index, param2?"lock":"unlock"));
    NEXUS_IsrCallback_Fire_isr(pDecoder->lockCallback);
}

static void NEXUS_AudioDecoder_P_StreamStatusAvailable_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;
    BSTD_UNUSED(param2);

    BDBG_MSG(("Decoder %u Stream Status Ready", pDecoder->index));
    NEXUS_IsrCallback_Fire_isr(pDecoder->streamStatusCallback);
    NEXUS_IsrCallback_Fire_isr(pDecoder->sourceChangeAppCallback);
}

static void NEXUS_AudioDecoder_P_AncillaryData_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;
    BSTD_UNUSED(param2);

    BDBG_MSG(("Decoder %u Ancillary Data Ready", pDecoder->index));
    NEXUS_IsrCallback_Fire_isr(pDecoder->ancillaryDataCallback);
}

static void NEXUS_AudioDecoder_P_StreamParameterChanged_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;
    BSTD_UNUSED(param2);

    BDBG_MSG(("Decoder %u Stream Parameter Changed", pDecoder->index));
    NEXUS_IsrCallback_Fire_isr(pDecoder->sourceChangeAppCallback);
}

static void NEXUS_AudioDecoder_P_CdbItbOverflow_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoder *pDecoder = (NEXUS_AudioDecoder *)pParam1;
    BSTD_UNUSED(param2);

    BDBG_MSG(("Decoder %u CDB/ITB overflow", pDecoder->index));
    pDecoder->numFifoOverflows++;
    NEXUS_IsrCallback_Fire_isr(pDecoder->fifoOverflowCallback);
}

void NEXUS_AudioDecoder_P_SetTsm(NEXUS_AudioDecoderHandle handle)
{
    BAPE_DecoderTsmSettings tsmSettings;

    BAPE_Decoder_GetTsmSettings(handle->channel, &tsmSettings);
    tsmSettings.tsmEnabled = false;

    if ( handle->tsmPermitted )
    {
        if ( handle->trickState.tsmEnabled )
        {
#if NEXUS_HAS_ASTM
            if ( handle->astm.settings.enableAstm )
            {
                BDBG_MSG(("ASTM is %s TSM for audio channel %p", handle->astm.settings.enableTsm ? "enabling" : "disabling", handle));
                if ( handle->astm.settings.enableTsm )
                {
                    tsmSettings.tsmEnabled = true;
                }
            }
            else
#endif
            {
                tsmSettings.tsmEnabled = true;
            }
        }
    }

    BDBG_MSG(("%s TSM",tsmSettings.tsmEnabled?"Enabling":"Disabling"));

#if NEXUS_HAS_ASTM
    /* Only allow ASTM if we have TSM enabled for all non-ASTM controls and we have a TS source */
    if ( handle->tsmPermitted && handle->trickState.tsmEnabled &&
         handle->apeStartSettings.streamType == BAVC_StreamType_eTsMpeg )
    {
        BDBG_MSG(("%s ASTM mode for audio channel %p",handle->astm.settings.enableAstm?"Enabling":"Disabling", handle));
        tsmSettings.astmEnabled = handle->astm.settings.enableAstm;
    }
    else
    {
        BDBG_MSG(("Disabling ASTM mode for audio channel %p", handle));
        tsmSettings.astmEnabled = false;
    }
#endif

    BAPE_Decoder_SetTsmSettings(handle->channel, &tsmSettings);

    handle->tsmEnabled = tsmSettings.tsmEnabled;
}


NEXUS_Error NEXUS_AudioDecoder_P_Start(NEXUS_AudioDecoderHandle handle)
{
    NEXUS_Error errCode;
    const NEXUS_AudioDecoderStartSettings *pProgram;

    BDBG_ASSERT(NULL != handle);

    BDBG_ENTER(NEXUS_AudioDecoder_P_Start);

    pProgram = &handle->programSettings;

    if ( handle->programSettings.input || !handle->started || handle->outputLists[NEXUS_AudioConnectorType_eCompressed].outputs[0] != NULL )
    {
        int i;
        NEXUS_AudioOutputList outputLists[NEXUS_AudioConnectorType_eMax];
        bool hasPcmOutputs;
        bool hasCompressedOutputs;

        /* Get output lists */
        for ( i = 0; i < NEXUS_AudioConnectorType_eMax; i++ )
        {
            NEXUS_AudioInput_P_GetOutputs(&handle->connectors[i], &outputLists[i], false);
        }

        /* 20111212 bandrews - the following code is concerned with stc programming priority.
        As such, the variable in use has been renamed to stcPriority.  DecoderType is now simply
        a mapping combining type and index */

        /* type now matches index directly */
        handle->stcDecoderType = NEXUS_StcChannelDecoderType_eAudio0 + handle->index;

        hasPcmOutputs = (outputLists[NEXUS_AudioConnectorType_eStereo].outputs[0] ||
                         outputLists[NEXUS_AudioConnectorType_eMultichannel].outputs[0] ||
                         outputLists[NEXUS_AudioConnectorType_eMono].outputs[0]) ? true : false;
        hasCompressedOutputs = (outputLists[NEXUS_AudioConnectorType_eCompressed].outputs[0] ||
                                outputLists[NEXUS_AudioConnectorType_eCompressed4x].outputs[0] ||
                                outputLists[NEXUS_AudioConnectorType_eCompressed16x].outputs[0]) ? true : false;

        /* Determine mode to add new outputs */
        if ( hasCompressedOutputs && !hasPcmOutputs )
        {
            /* Compressed output, but no stereo or multichannel.  This is a passthrough channel. */
            if ( handle->connectors[NEXUS_AudioConnectorType_eCompressed].format == NEXUS_AudioInputFormat_eCompressed )
            {
                BDBG_MSG(("Decoder %d is a passthrough channel", handle->index));
            }
            else
            {
                BDBG_MSG(("Decoder %d is a decode channel (PCM data for passthrough)", handle->index));
            }

            handle->stcPriority = 1;
        }
        else
        {
            if ( handle->descriptorParent || handle->programSettings.secondaryDecoder )
            {
                /* AD child or secondary decoder (MS10/MS11).  This is decoder type audio2 */
                handle->stcPriority = 2;
            }
            else
            {
                /* Standard decode channel.  Compressed will be simul */
                handle->stcPriority = 0;
            }
            /* Determine decoder mode as decode or simul based on presence of compressed outputs */
            if ( hasCompressedOutputs )
            {
                /* Simul Mode */
                BDBG_MSG(("Decoder %d is a simul channel", handle->index));
            }
            else
            {
                /* Decode Mode */
                BDBG_MSG(("Decoder %d is a decode channel", handle->index));
            }
        }

        if ( !hasCompressedOutputs && !hasPcmOutputs )
        {
            BDBG_ERR(("No outputs have been connected to this decoder."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        for ( i = 0; i < NEXUS_AudioConnectorType_eMax; i++ )
        {
            int j;

            for ( j = 0; j < NEXUS_AUDIO_MAX_OUTPUTS; j++ )
            {
                NEXUS_AudioOutput output = outputLists[i].outputs[j];
                handle->outputLists[i].outputs[j] = output;
            }

            /* Only call this for inputs about to actually start */
            BDBG_MSG(("Preparing to start path %d", i));

            errCode = NEXUS_AudioInput_P_PrepareToStart(&handle->connectors[i]);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    /* Setup StcChannel */
    if ( pProgram->stcChannel )
    {
        NEXUS_StcChannelCallbacks callbacks;

        LOCK_TRANSPORT();

        NEXUS_StcChannel_GetDefaultCallbacks_priv(&callbacks);
        callbacks.getPts_isr = NEXUS_AudioDecoder_P_GetPtsCallback_isr;
        callbacks.getCdbLevel_isr = NEXUS_AudioDecoder_P_GetCdbLevelCallback_isr;
        callbacks.stcValid_isr = NEXUS_AudioDecoder_P_StcValidCallback_isr;
        callbacks.pDevContext = handle;
        if ( pProgram->nonRealTime )
        {
            callbacks.getPcrOffset_isr = NEXUS_AudioDecoder_P_GetPcrOffset_isr;
            callbacks.setPcrOffset_isr = NEXUS_AudioDecoder_P_SetPcrOffset_isr;
        }
        NEXUS_StcChannel_SetCallbacks_priv(pProgram->stcChannel, handle->stcDecoderType, handle->stcPriority, &callbacks);
        if ( pProgram->pidChannel )
        {
            NEXUS_StcChannel_EnablePidChannel_priv(pProgram->stcChannel, pProgram->pidChannel);
        }

        UNLOCK_TRANSPORT();
        
        if ( pProgram->pidChannel && !handle->fifoWatchdogTimer)
        {
            handle->fifoWatchdogTimer = NEXUS_ScheduleTimer(250, NEXUS_AudioDecoder_P_FifoWatchdog, handle);
        }
    }

    /* After all that, we're ready to go.  Start. */
    handle->ptsErrorCount = 0;
    handle->numFifoOverflows = 0;
    BDBG_MSG(("Starting Decoder %d", handle->index));

#if NEXUS_HAS_SYNC_CHANNEL
    if (handle->sync.startMuted)
    {
        handle->sync.mute = true;
        BDBG_MSG(("Sync requested to start %u muted", handle->index));
    }
#endif

    /* Re-apply settings */
    NEXUS_AudioDecoder_ApplySettings_priv(handle);
    BDBG_MSG(("Starting with codec %u", handle->apeStartSettings.codec));

    handle->locked = false;
    handle->numFifoOverflows = handle->numFifoUnderflows = 0;
    errCode = BAPE_Decoder_Start(handle->channel, &handle->apeStartSettings);
    if ( errCode && !handle->started )
    {
        if ( handle->programSettings.stcChannel )
        {
            LOCK_TRANSPORT();
            if ( handle->programSettings.pidChannel )
            {
                NEXUS_StcChannel_DisablePidChannel_priv(handle->programSettings.stcChannel, handle->programSettings.pidChannel);
            }
            NEXUS_StcChannel_ClearCallbacks_priv(handle->programSettings.stcChannel, handle->stcDecoderType);
            UNLOCK_TRANSPORT();
            if ( handle->fifoWatchdogTimer )
            {
                NEXUS_CancelTimer(handle->fifoWatchdogTimer);
                handle->fifoWatchdogTimer = NULL;
            }
        }
        return BERR_TRACE(errCode);
    }

    if ( handle->programSettings.pidChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_Rave_Enable_priv(handle->raveContext);
        UNLOCK_TRANSPORT();
    }
    handle->running = true;

    if ( handle->programSettings.pidChannel )
    {
        (void)NEXUS_PidChannel_ConsumerStarted(handle->programSettings.pidChannel);
    }

#if NEXUS_HAS_SYNC_CHANNEL
    if (handle->sync.settings.startCallback_isr) 
    {
        BKNI_EnterCriticalSection();
        (*handle->sync.settings.startCallback_isr)(handle->sync.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }
#endif

#if NEXUS_HAS_ASTM
    handle->astm.status.started = true;

    if (handle->astm.settings.lifecycle_isr)
    {
        BDBG_MSG(("Audio channel %p is notifying ASTM of its start action", handle));
        BKNI_EnterCriticalSection();
        (*handle->astm.settings.lifecycle_isr)(handle->astm.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }
#endif

    BDBG_LEAVE(NEXUS_AudioDecoder_P_Start);

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioDecoder_P_Stop(NEXUS_AudioDecoderHandle handle, bool flush)
{
    BDBG_ENTER(NEXUS_AudioDecoder_P_Stop);

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);

    if ( handle->running )
    {
        if ( handle->fifoWatchdogTimer )
        {
            NEXUS_CancelTimer(handle->fifoWatchdogTimer);
            handle->fifoWatchdogTimer = NULL;
        }

        handle->locked = false;
        BAPE_Decoder_Stop(handle->channel);

        if ( handle->programSettings.pidChannel && flush )
        {
            LOCK_TRANSPORT();
            NEXUS_Rave_Disable_priv(handle->raveContext);
            NEXUS_Rave_Flush_priv(handle->raveContext);
            UNLOCK_TRANSPORT();
        }

        handle->running = false;

        if ( handle->programSettings.stcChannel && handle->stcDecoderType != NEXUS_StcChannelDecoderType_eMax )
        {
            LOCK_TRANSPORT();
            if ( handle->programSettings.pidChannel && handle->programSettings.stcChannel )
            {
                NEXUS_StcChannel_DisablePidChannel_priv(handle->programSettings.stcChannel, handle->programSettings.pidChannel);
            }
            NEXUS_StcChannel_ClearCallbacks_priv(handle->programSettings.stcChannel, handle->stcDecoderType);
            UNLOCK_TRANSPORT();
        }

#if NEXUS_HAS_SYNC_CHANNEL
        if (handle->sync.settings.startCallback_isr) 
        {
            BKNI_EnterCriticalSection();
            (*handle->sync.settings.startCallback_isr)(handle->sync.settings.callbackContext, 0);
            BKNI_LeaveCriticalSection();
        }
#endif

#if NEXUS_HAS_ASTM
        handle->astm.status.started = false;

        if (handle->astm.settings.lifecycle_isr)
        {
            BDBG_MSG(("Audio channel %p is notifying ASTM of its stop action", handle));
            BKNI_EnterCriticalSection();
            (*handle->astm.settings.lifecycle_isr)(handle->astm.settings.callbackContext, 0);
            BKNI_LeaveCriticalSection();
        }
#endif
    }

    BDBG_LEAVE(NEXUS_AudioDecoder_P_Stop);

    return BERR_SUCCESS;
}

static BERR_Code NEXUS_AudioDecoder_P_GetPtsCallback_isr(void *pContext, BAVC_PTSInfo *pPTSInfo)
{
    BERR_Code errCode;
    NEXUS_AudioDecoderHandle handle = pContext;
    BAPE_DecoderTsmStatus tsmStatus;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pPTSInfo);
    errCode = BAPE_Decoder_GetTsmStatus_isr(handle->channel, &tsmStatus);
    if ( errCode )
    {
        return errCode; /* BERR_TRACE intentionally omitted */   
    }
    *pPTSInfo = tsmStatus.ptsInfo;
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_AudioDecoder_P_GetCdbLevelCallback_isr(void *pContext, unsigned *pCdbLevel)
{
    unsigned depth=0, size=0;
    NEXUS_AudioDecoderHandle audioDecoder = (NEXUS_AudioDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(audioDecoder, NEXUS_AudioDecoder);
    if ( audioDecoder->programSettings.pidChannel )
    {
        NEXUS_Rave_GetCdbBufferInfo_isr(audioDecoder->raveContext, &depth, &size);
    }
    BDBG_MSG(("GetCdbLevel - returned %d", depth));
    *pCdbLevel = depth;
    return 0;
}

static BERR_Code NEXUS_AudioDecoder_P_StcValidCallback_isr(void *pContext)
{
    NEXUS_AudioDecoderHandle audioDecoder = (NEXUS_AudioDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(audioDecoder, NEXUS_AudioDecoder);
    return BAPE_Decoder_SetStcValid_isr(audioDecoder->channel);
}

static void NEXUS_AudioDecoder_P_FifoWatchdog(void *context)
{
    NEXUS_AudioDecoderHandle audioDecoder = context;
    unsigned timeout=150;

    audioDecoder->fifoWatchdogTimer = NULL;

    if (audioDecoder->programSettings.stcChannel) {
        uint32_t cdbValidPointer, cdbReadPointer;
        bool shouldFlush, isLocked, shouldZeroFill;
        unsigned depth, size;

        BKNI_EnterCriticalSection();
        NEXUS_Rave_GetCdbPointers_isr(audioDecoder->raveContext, &cdbValidPointer, &cdbReadPointer);
        NEXUS_Rave_GetCdbBufferInfo_isr(audioDecoder->raveContext, &depth, &size);
        BKNI_LeaveCriticalSection();
        if (audioDecoder->lastCdbValidPointer == cdbValidPointer && audioDecoder->lastCdbReadPointer == cdbReadPointer) {
            if (audioDecoder->staticFifoCount < 20) {
                audioDecoder->staticFifoCount++;
            }
        }
        else {
            audioDecoder->staticFifoCount = 0;
            audioDecoder->lastCdbValidPointer = cdbValidPointer;
            audioDecoder->lastCdbReadPointer = cdbReadPointer;
        }

        isLocked = (audioDecoder->staticFifoCount > 4) && (audioDecoder->trickState.rate >= NEXUS_NORMAL_DECODE_RATE);

        LOCK_TRANSPORT();
        NEXUS_StcChannel_ReportDecoderHang_priv(audioDecoder->programSettings.stcChannel,
            audioDecoder->stcDecoderType, isLocked, size?depth*100/size:0, &shouldFlush, &shouldZeroFill);
        UNLOCK_TRANSPORT();

        /* if both flush and zero-fill are set, flush first */

        if (shouldFlush) {
            NEXUS_AudioDecoder_Flush(audioDecoder);
            LOCK_TRANSPORT();
            NEXUS_StcChannel_ReportFlush_priv(audioDecoder->programSettings.stcChannel,
                audioDecoder->stcDecoderType);
            UNLOCK_TRANSPORT();
        }

        /* this is a one-shot, like a flush */
        if (shouldZeroFill)
        {
            BDBG_WRN(("audio %p: Enabling audio gap zero fill", audioDecoder));
            audioDecoder->staticFifoCount = 0;
            BAPE_Decoder_EnterUnderflowMode(audioDecoder->channel);
        }
    }

    audioDecoder->fifoWatchdogTimer = NEXUS_ScheduleTimer(timeout, NEXUS_AudioDecoder_P_FifoWatchdog, audioDecoder);
}

static void NEXUS_AudioDecoder_P_InputFormatChange_isr(void *pParam1, int param2)
{
    NEXUS_AudioDecoderHandle handle = (NEXUS_AudioDecoderHandle)pParam1;
    BSTD_UNUSED(param2);
    /* convert to task time */
    BKNI_SetEvent_isr(handle->inputFormatChangeEvent);
}

static void NEXUS_AudioDecoder_P_InputFormatChange(void *pParam)
{
    NEXUS_AudioDecoderHandle handle = (NEXUS_AudioDecoderHandle)pParam;
    NEXUS_AudioInputPortStatus inputPortStatus;
    NEXUS_Error errCode;
    BAVC_AudioCompressionStd avcCodec;
    BAPE_DecoderStatus decoderStatus;
    bool stop=false, start=false;

    errCode = NEXUS_AudioInput_P_GetInputPortStatus(handle->programSettings.input, &inputPortStatus);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return;
    }
        
    BAPE_Decoder_GetStatus(handle->channel, &decoderStatus);

    avcCodec = NEXUS_Audio_P_CodecToMagnum(inputPortStatus.codec);

    if ( handle->running )  /* If Nexus thinks that APE decoder is running. */
    {
        BDBG_MSG(("Input Format Change - Decoder is running"));
        if ( decoderStatus.halted       ||      /* due to on-the-fly input format change. */
             ! decoderStatus.running    ||      /* due to format change during APE decoder start. */
             avcCodec != handle->apeStartSettings.codec  )  /* due to on-the-fly codec change.    */
        {
            /* The APE decoder has stopped because of a format change that can't be handled on-the-fly. */
            BDBG_MSG(("APE Decoder has halted due to format change.  Need to stop decoder."));
            stop = true;
            if ( inputPortStatus.signalPresent )
            {
                /* Restart with new codec if signal is present */
                start = true;
                BDBG_MSG(("Valid input signal. Need to restart."));
            }
            else
            {
                BDBG_MSG(("No valid input signal. Don't need to restart."));
            }
        }
        else
        {
            /* Nothing to do, return. */
            BDBG_MSG(("APE Decoder running and not halted. No action needed."));
            return;
        }
    }
    else  /* Nexus thinks that APE decoder is stopped. */
    {
        BDBG_MSG(("Input Format Change - Decoder is not running"));
        if ( inputPortStatus.signalPresent )
        {
            /* Start with new codec if signal is present */
            BDBG_MSG(("Valid input signal, starting decoder."));
            start = true;
        }
        else
        {
            BDBG_MSG(("No valid input signal, not starting decoder."));
        }
    }

    if ( stop )
    {
        BDBG_MSG(("Stop decoder on input format change"));
        NEXUS_AudioDecoder_P_Stop(handle, true);
    }

    handle->apeStartSettings.codec = avcCodec;
    if ( start )
    {
        NEXUS_Error errCode;
        BDBG_MSG(("Start decoder on input format change"));
        errCode = NEXUS_AudioDecoder_P_Start(handle);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
        }
    }
}

static BERR_Code NEXUS_AudioDecoder_P_SetPcrOffset_isr(void *pContext, uint32_t pcrOffset)
{
    NEXUS_AudioDecoderHandle handle = (NEXUS_AudioDecoderHandle)pContext;
    BAPE_DecoderTsmSettings tsmSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);

    BAPE_Decoder_GetTsmSettings_isr(handle->channel, &tsmSettings);
    tsmSettings.stcOffset = pcrOffset;
    errCode = BAPE_Decoder_SetTsmSettings_isr(handle->channel, &tsmSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_AudioDecoder_P_GetPcrOffset_isr(void *pContext, uint32_t *pPcrOffset)
{
    NEXUS_AudioDecoderHandle handle = (NEXUS_AudioDecoderHandle)pContext;
    BAPE_DecoderTsmSettings tsmSettings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pPcrOffset);

    BAPE_Decoder_GetTsmSettings_isr(handle->channel, &tsmSettings);
    *pPcrOffset = tsmSettings.stcOffset;
    return BERR_SUCCESS;    
}

NEXUS_Error NEXUS_AudioDecoder_GetExtendedStatus(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioDecoderExtendedStatus *pStatus /* [out] */
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDecoder);
    BDBG_ASSERT(NULL != pStatus);
    LOCK_TRANSPORT();
    if ( handle->raveContext ) 
    {
        NEXUS_RaveStatus raveStatus;

        errCode = NEXUS_Rave_GetStatus_priv(handle->raveContext, &raveStatus);
        if ( errCode == BERR_SUCCESS ) 
        {
            pStatus->raveIndex = raveStatus.index;
        }
    }
    UNLOCK_TRANSPORT();

    return errCode;
}

void NEXUS_AudioDecoder_IsCodecSupported(
    NEXUS_AudioDecoderHandle handle,
    NEXUS_AudioCodec codec,
    bool *pSupported
    )
{
    NEXUS_AudioCapabilities caps;

    BSTD_UNUSED(handle);
    BDBG_ASSERT(codec < NEXUS_AudioCodec_eMax);
    BDBG_ASSERT(NULL != pSupported);

    NEXUS_GetAudioCapabilities(&caps);
    *pSupported = caps.dsp.codecs[codec].decode;
}

NEXUS_Error NEXUS_AudioDecoder_GetAncillaryDataBuffer(
    NEXUS_AudioDecoderHandle handle,
    void **pBuffer,   /* [out] attr{memory=cached} pointer to ancillary data */
    size_t *pSize     /* [out] number of bytes of userdata */
    )
{
    void *pBuf;
    BERR_Code errCode;

    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoder, handle);

    errCode = BAPE_Decoder_GetAncillaryDataBuffer(handle->channel, &pBuf, pSize);
    if ( errCode )
    {
        handle->lastAncillarySize = 0;
        return BERR_TRACE(errCode);
    }
    errCode = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], pBuf, pBuffer);
    if ( errCode )
    {
        handle->lastAncillarySize = 0;
        return BERR_TRACE(errCode);
    }
    handle->lastAncillarySize = *pSize;
    return BERR_SUCCESS;
}

void NEXUS_AudioDecoder_AncillaryDataReadComplete(
    NEXUS_AudioDecoderHandle handle,
    size_t size   /* number of bytes of userdata consumed by the application */
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoder, handle);

    if ( size > handle->lastAncillarySize )
    {
        BDBG_ERR(("Invalid number of bytes to NEXUS_AudioDecoder_AncillaryDataReadComplete"));
        BDBG_ERR(("Last Size from GetBuffer: %u Request to Read %u", handle->lastAncillarySize, size));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return;
    }
    (void)BAPE_Decoder_ConsumeAncillaryData(handle->channel, size);
    handle->lastAncillarySize = 0;
}

void NEXUS_AudioDecoder_FlushAncillaryData(
    NEXUS_AudioDecoderHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoder, handle);

    BAPE_Decoder_FlushAncillaryData(handle->channel);
}

