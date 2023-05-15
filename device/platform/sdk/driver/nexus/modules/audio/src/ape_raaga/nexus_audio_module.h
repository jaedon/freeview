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
* $brcm_Workfile: nexus_audio_module.h $
* $brcm_Revision: SW7346-1228-1/1 $
* $brcm_Date: 3/29/13 12:39p $
*
* API Description:
*   API name: Audio Module
*    Module includes
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_module.h $
* 
* SW7346-1228-1/1   3/29/13 12:39p scho
* SW7346-1228-1: update NEXUS Audio Input to check if the mixer is
*  already started
* 
* 47   10/9/12 12:13a jgarrett
* SW7346-914: Adding audio ancillary data support
* 
* 46   8/22/12 9:46p bandrews
* SW7420-2314: change mute control to start muted, and unmute immediately
* 
* 45   7/27/12 4:40p bandrews
* SW7125-1309: fix compile error
* 
* 44   7/27/12 2:56p jgarrett
* SW7425-3443: Removing DTV-only APIs and directories
* 
* 43   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 42   6/14/12 2:35p erickson
* SW7425-2741: add AudioDecoderPrimer
* 
* 41   4/20/12 5:23p jgarrett
* SW7231-774: Resetting MuxOutput on a watchdog interrupt
* 
* 40   4/12/12 2:52p jgarrett
* SW7425-2075: Adding HBR support
* 
* 39   2/22/12 3:31p jgarrett
* SW7405-5003: Removing BKNI_Memcmp
* 
* 38   2/22/12 2:06p jgarrett
* SW7425-2455: Removing DTV APIs
* 
* 37   2/9/12 5:39p jgarrett
* SW7425-2074: Fixing issues when NEXUS_NUM_AUDIO_INPUT_CAPTURES is not
*  defined
* 
* 36   2/9/12 2:24p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/4   2/7/12 7:15p jgarrett
* SW7425-2074: Enabling InputCapture
* 
* SW7425-2074/3   2/3/12 5:19p jgarrett
* SW7425-2074: Rebasing to latest code
* 
* SW7425-2074/2   1/18/12 6:06p jgarrett
* SW7425-2074: Initial version of InputCapture API
* 
* SW7425-2074/1   1/18/12 10:21a jgarrett
* SW7425-2156: Adding power management support for DSP video decoder
* 
* 35   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 34   1/16/12 6:02p gmohile
* SW7425-1473 : Merge raaga encoder support
* 
* 33   12/13/11 9:44p bandrews
* SW7425-1772: merge to main
* 
* SW7425-1772/2   12/12/11 10:29p bandrews
* SW7425-1772: merge from main
* 
* 32   12/12/11 8:14a jgarrett
* SW7425-1756: Adding stream status callback and bitrate/mode change
*  handling
* 
* 31   11/18/11 3:04p jgarrett
* SW7425-1756: Adding PTS callbacks and overflow callback
* 
* 30   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
* 
* 29   11/2/11 6:26p jgarrett
* SW7422-400: Adding routines for audio DSP debug
* 
* 28   10/11/11 4:46p jgarrett
* SW7425-1349: Adding AudioDummyOutput
* 
* 27   9/27/11 4:36p jgarrett
* SW7408-304: Destroying IMG context on shutdown
* 
* 26   9/8/11 8:45a jgarrett
* SWDTV-6627: Adding nexus EQ support
* 
* 25   8/26/11 12:03p jgarrett
* SW7425-742: Merge to main branch
* 
* SW7425-724/1   8/24/11 11:55a jgarrett
* SW7425-724: Adding RF Audio Encoder
* 
* 24   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/6   8/11/11 6:26p jgarrett
* SWDTV-6306: Adding SPDIF and RF Input
* 
* Nexus_APE_Integration/5   8/10/11 5:16p jgarrett
* SWDTV-6306: Adding implicit capture to DSP instead of FMM for DTV
* 
* Nexus_APE_Integration/4   8/8/11 5:38p jgarrett
* SWDTV-6761: Adding StudioSound
* 
* Nexus_APE_Integration/3   8/4/11 1:57p gskerl
* SW7125-1014 : Merged changes to Nexus_APE_Integration branch
* 
* Nexus_APE_Integration/2   7/8/11 6:40p jgarrett
* SWDTV-6760: Adding I2sMultiOutput
* 
* Nexus_APE_Integration/1   7/1/11 5:50p jgarrett
* SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
*  directory structure
* 
* 23   7/21/11 4:13p gmohile
* SW7125-1014 : Rework power management
* 
* 22   6/28/11 10:33a jgarrett
* SW7422-406: Adding input volume settings for DSP mixers
* 
* 21   6/20/11 5:42p jgarrett
* SW7425-409: Merging VP6 support to main branch
* 
* SW7425-409/1   6/6/11 7:16p jgarrett
* SW7425-409: Adding DSP video decoder extensions for VDE
* 
* 20   5/16/11 5:19p jgarrett
* SWDTV-6762: Adding Audyssey ADV/ABX
* 
* 19   5/16/11 4:57p jgarrett
* SWDTV-6763: Adding 3D Surround
* 
* 18   5/13/11 6:38p jgarrett
* SW7425-410: Adding AVL
* 
* 17   5/13/11 2:04p gskerl
* SW7422-354: Added prototype for NEXUS_AudioReturnChannel_P_SetMaster()
* 
* 18   5/13/11 6:32p jgarrett
* SW7425-410: Adding AVL
* 
* 16   5/11/11 7:07p jgarrett
* SW7422-410: Adding CustomVoice for APE
* 
* 15   4/27/11 6:52p jgarrett
* SW7420-1826: Adding AudioOutputGroup
* 
* 14   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 13   4/11/11 5:52p jgarrett
* SWDTV-6306: Adding preliminary Nexus APE support for DTV
* 
* 12   3/24/11 10:58a jgarrett
* SW7422-146: Fixing kernel mode build
* 
* 11   3/23/11 6:28p jgarrett
* SW7422-352: adding HDMI input support to nexus
* 
* 10   3/23/11 4:29p jgarrett
* SW7422-403: Fixing sync mute and DSOLA
* 
* 9   3/21/11 7:05p jgarrett
* SW7422-356: Adding MuxOutput support in APE/Nexus
* 
* 8   3/1/11 7:25p jgarrett
* SW7422-146: Adding DTS/AC3 encoders
* 
* 7   2/28/11 2:58p jgarrett
* SW7422-146: Added input capture to I2sInput
* 
* 6   2/22/11 5:30p jgarrett
* SW7422-146: Fixing build errors
* 
* 5   1/13/11 5:25p jgarrett
* SW7422-146: Adding encoder hooks
* 
* 4   1/10/11 4:20p jgarrett
* SW7422-146: Adding missing module-level routines
* 
* 3   1/10/11 3:26p jgarrett
* SW7422-146: Adding full nexus API set
* 
* 2   1/4/11 2:39p jgarrett
* SW7422-146: Adding 7425 APE support
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/

#ifndef NEXUS_AUDIO_MODULE_H_
#define NEXUS_AUDIO_MODULE_H_

#include "bape.h"

#include "nexus_base.h"
#include "nexus_audio_thunks.h"
#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_init.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_input.h"
#include "nexus_audio_input_capture.h"
#include "nexus_audio_output.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_mux_output.h"
#include "priv/nexus_audio_input_priv.h"
#include "priv/nexus_audio_output_priv.h"
#include "priv/nexus_core_img.h"
#include "priv/nexus_core_img_id.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_playback.h"
#include "priv/nexus_core.h"
#include "priv/nexus_core_audio.h"
#include "nexus_spdif_output.h"
#include "nexus_i2s_input.h"
#include "nexus_i2s_output.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_extra.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_decoder_primer.h"
#include "bape.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_audio_decoder_priv.h"
#include "nexus_ac3_encode.h"
#include "nexus_audio_encoder.h"
#include "nexus_dolby_digital_reencode.h"
#include "nexus_dolby_volume.h"
#include "nexus_dts_encode.h"
#include "nexus_echo_canceller.h"
#include "nexus_tru_volume.h"
#include "nexus_audio.h"
#include "nexus_audio_module.h"
#include "priv/nexus_audio_standby_priv.h"
#include "nexus_audio_output_group.h"
#include "nexus_audio_dummy_output.h"
#include "nexus_auto_volume_level.h"
#include "nexus_3d_surround.h"
#if NEXUS_AUDIO_BUFFER_CAPTURE_EXT
#include "nexus_audio_buffer_capture.h"
#include "nexus_audio_buffer_capture_priv.h"
#endif
#include "nexus_audio_equalizer.h"
#include "nexus_audio_return_channel.h"
#include "nexus_rf_audio_encoder.h"
#include "nexus_spdif_input.h"
#include "nexus_audio_dsp.h"
#if NEXUS_CVOICE
#include "nexus_custom_voice.h"
#endif
#if NEXUS_NUM_DSP_VIDEO_DECODERS
#include "nexus_audio_dsp_video_decoder_module.h"
#endif
#if NEXUS_NUM_DSP_VIDEO_ENCODERS
#include "nexus_audio_dsp_video_encoder_module.h"
#endif

/***************************************************************************
Summary:
    BAPE Handle and helper macro
***************************************************************************/
#define NEXUS_AUDIO_DEVICE_HANDLE g_NEXUS_audioModuleData.apeHandle

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME audio
#define NEXUS_MODULE_SELF g_NEXUS_audioModule

extern NEXUS_ModuleHandle g_NEXUS_audioModule;

typedef struct NEXUS_AudioModuleData
{
    BDSP_Handle dspHandle;
    BAPE_Handle apeHandle;
    NEXUS_AudioModuleSettings settings;
    void *pImageContext;
    bool watchdogDeferred;
} NEXUS_AudioModuleData;

extern NEXUS_AudioModuleData g_NEXUS_audioModuleData;

/***************************************************************************
Summary:
    Audio encoder object
 ***************************************************************************/
typedef struct NEXUS_AudioEncoder
{
    NEXUS_OBJECT(NEXUS_AudioEncoder);
    BAPE_EncoderHandle encoder;
    NEXUS_AudioInputObject connector;
    NEXUS_AudioInput input;
} NEXUS_AudioEncoder;

/***************************************************************************
Summary:
    Audio decoder object
 ***************************************************************************/
typedef struct NEXUS_AudioDecoder
{
    NEXUS_OBJECT(NEXUS_AudioDecoder);
    unsigned index;
    NEXUS_AudioDecoderSettings settings;
    NEXUS_AudioDecoderStartSettings programSettings;
    bool opened;
    bool started;                   /* Has the channel been started by the app? */
    bool running;                   /* Is the channel actually active? (enabled && started) */
    bool trickMute;                 /* decoder muted (while running) by request from the trick modes operations (Keep this unconditional for sake of simplicity) */
    bool trickForceStopped;         /* decoder muted (stopped) by request from the trick modes operations (Keep this unconditional for sake of simplicity) */
    bool isDss;
    bool isPlayback;
    bool tsmPermitted;
    bool tsmEnabled;
    bool isDescriptorDecoder;
    bool locked;
    BAPE_DecoderHandle channel;
    BAPE_DecoderStartSettings apeStartSettings;
    NEXUS_RaveStatus raveStatus;
    NEXUS_RaveHandle raveContext;
    NEXUS_RaveHandle savedRaveContext;
    BKNI_EventHandle sampleRateEvent;
    BKNI_EventHandle channelChangeReportEvent;
    BKNI_EventHandle inputFormatChangeEvent;
    NEXUS_EventCallbackHandle sampleRateCallback;
    NEXUS_EventCallbackHandle channelChangeReportEventHandler;
    NEXUS_EventCallbackHandle inputFormatChangeEventHandler;
    NEXUS_IsrCallbackHandle lockCallback;
    NEXUS_IsrCallbackHandle firstPtsCallback;
    NEXUS_IsrCallbackHandle ptsErrorCallback;
    NEXUS_IsrCallbackHandle fifoOverflowCallback;
    NEXUS_IsrCallbackHandle streamStatusCallback;
    NEXUS_IsrCallbackHandle ancillaryDataCallback;
    NEXUS_StcChannelDecoderType stcDecoderType;
    unsigned stcPriority;
    unsigned ptsErrorCount;
    size_t lastAncillarySize;
    NEXUS_AudioInputFormat compressedFormat;
    NEXUS_AudioOutputList outputLists[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioInputObject connectors[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioDecoderTrickState trickState;
    struct
    {
        NEXUS_AudioInputSyncSettings settings;
        bool mute;
        bool startMuted;
    } sync;
#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
        NEXUS_AudioDecoderAstmSettings settings;
        NEXUS_AudioDecoderAstmStatus status;
    } astm;
#endif
    NEXUS_IsrCallbackHandle sourceChangeAppCallback;
    NEXUS_AudioDecoderHandle descriptorParent;
    uint32_t lastCdbValidPointer;
    uint32_t lastCdbReadPointer;
    unsigned staticFifoCount, numFifoOverflows, numFifoUnderflows;
    NEXUS_TimerHandle fifoWatchdogTimer;
} NEXUS_AudioDecoder;

/***************************************************************************
Summary: 
    Initialize the audio input module  
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_Init(void);

/***************************************************************************
Summary: 
    Un-Initialize the audio input module  
 ***************************************************************************/
void NEXUS_AudioInput_P_Uninit(void);

/***************************************************************************
Summary:
    Determine if an input chain is running.  All nodes upstream from the
    specified node will be searched.
 ***************************************************************************/
bool NEXUS_AudioInput_P_IsRunning(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
    Remove all mixer outputs attached to this object and below.  Used when an
    an input/mixer will become invalid.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_RemoveMixerOutputs(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
    Determine format of incoming data.  All nodes upstream from the specified
    node will be searched until a format is found.
 ***************************************************************************/
NEXUS_AudioInputFormat NEXUS_AudioInput_P_GetFormat(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
    Add an input to the specified connector.  Sanity checks in terms of how
    many inputs are supported, etc. should be done by the caller.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_AddInput(
    NEXUS_AudioInput destination,
    NEXUS_AudioInput source);

/***************************************************************************
Summary:
    Remove all inputs to the specified connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_RemoveAllInputs(
    NEXUS_AudioInput destination
    );

/***************************************************************************
Summary:
    Remove a single input from the specified connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_RemoveInput(NEXUS_AudioInput destination, NEXUS_AudioInput source);

/***************************************************************************
Summary:
    Determine if the specified source is an input to the destination
    connector.
 ***************************************************************************/
bool NEXUS_AudioInput_P_IsConnected(
    NEXUS_AudioInput destination,
    NEXUS_AudioInput source
    );

/***************************************************************************
Summary:
    Retrieve all outputs downstream from this connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_GetOutputs(
    NEXUS_AudioInput input,
    NEXUS_AudioOutputList *pOutputList,
    bool directOnly                         /* If true, only outputs connected to this channel will be returned.
                                               If false, all outputs downstream will be returned, including those
                                               attached to downstream mixers.
                                            */
    );

/***************************************************************************
Summary:
    Set connection-specific data for the binding between the specified
    source and destination connectors.  The data will be copied and stored
    inside the connector object.  It will be lost when the connection is
    broken.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_SetConnectionData(
    NEXUS_AudioInput destination,
    NEXUS_AudioInput source,
    const void *pData,
    size_t dataSize
    );

/***************************************************************************
Summary:
    Retrieve a pointer to the stored connection data between this source
    and destination.  May be NULL if not previously set.
 ***************************************************************************/
const void *NEXUS_AudioInput_P_GetConnectionData(
    NEXUS_AudioInput destination,
    NEXUS_AudioInput source
    );

/***************************************************************************
Summary:
    Connect an audio output to the specified connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_ConnectOutput(
    NEXUS_AudioInput input,
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
    Disconnect an audio output from the specified connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_DisconnectOutput(
    NEXUS_AudioInput input,
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
    Prepare the input chain to start, building downstream connections.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_PrepareToStart(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
    Notify the input chain of a data format change (e.g. sample rate)
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_FormatChanged(
    NEXUS_AudioInput input,
    NEXUS_AudioInputFormatData *pData
    );

/***************************************************************************
Summary:
    Returns the first object downstream from the current object that matches
    the specified type.  This is a depth-first search, not breadth-first.
 ***************************************************************************/
NEXUS_AudioInput NEXUS_AudioInput_P_FindByType(
    NEXUS_AudioInput input,
    NEXUS_AudioInputType type
    );

/***************************************************************************
Summary: 
    Called when AudioOutputSettings are changed so that mixers can be updated
    accordingly.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_OutputSettingsChanged(
    NEXUS_AudioInput input, 
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary: 
    Get the mixer input volume for a particular input
 ***************************************************************************/
void NEXUS_AudioInput_P_GetVolume(
    NEXUS_AudioInput input, 
    BAPE_MixerInputVolume *pInputVolume    /* [out] */
    );

/***************************************************************************
Summary: 
    Set the mixer input volume for a particular input
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_SetVolume(
    NEXUS_AudioInput input, 
    const BAPE_MixerInputVolume *pInputVolume
    );

/***************************************************************************
Summary: 
    Propagate mixer input volume into a mixer object in nexus 
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_P_SetInputVolume(
    NEXUS_AudioMixerHandle handle, 
    NEXUS_AudioInput input,
    const BAPE_MixerInputVolume *pInputVolume
    );

/***************************************************************************
Summary: 
    Check mixer state - mixer can be started even if inputs are not started 
 ***************************************************************************/
bool NEXUS_AudioMixer_P_IsStarted(
    NEXUS_AudioMixerHandle handle
    );

/***************************************************************************
Summary: 
    Get an external input port handle
 ***************************************************************************/
BAPE_InputPort NEXUS_AudioInput_P_GetInputPort(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Input Port Status Information
 ***************************************************************************/
typedef struct NEXUS_AudioInputPortStatus
{
    bool signalPresent;         /* If true, a signal is present */
    bool compressed;            /* If true, stream is compressed.  If false, stream is linear PCM */
    bool hbr;                   /* If true, stream is HBR compressed. */
    NEXUS_AudioCodec codec;     /* Stream Codec */
    unsigned numPcmChannels;    /* Number of PCM channels if codec == NEXUS_AudioCodec_ePcm */
    unsigned sampleRate;        /* Sample Rate in Hz */
} NEXUS_AudioInputPortStatus;

/***************************************************************************
Summary:
Determine if this input supports dynamic format changes
 ***************************************************************************/
bool NEXUS_AudioInput_P_SupportsFormatChanges(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Enable/Disable interrupt for dynamic format changes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_SetFormatChangeInterrupt(
    NEXUS_AudioInput input,
    void (*pCallback_isr)(void *, int),
    void *pParam1,
    int param2
    );

/***************************************************************************
Summary:
Get Input Status Information 
 
Description: 
Retrieves input status information.  This is currently only supported for 
HDMI or SPDIF inputs. 
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_GetInputPortStatus(
    NEXUS_AudioInput input,
    NEXUS_AudioInputPortStatus *pStatus     /* [out] */
    );

/***************************************************************************
Summary:
Force a particular input to stop
 
Description: 
This function should only be used when the system is shutting down to 
avoid clobbering application state. 
 ***************************************************************************/
void NEXUS_AudioInput_P_ForceStop(NEXUS_AudioInput input);

/***************************************************************************
Summary: 
    NEXUS audio output private data. 
 ***************************************************************************/
BDBG_OBJECT_ID_DECLARE(NEXUS_AudioOutputData);
typedef struct NEXUS_AudioOutputData
{
    BDBG_OBJECT(NEXUS_AudioOutputData)
    NEXUS_AudioOutputSettings settings;
    NEXUS_AudioInput input;
    NEXUS_AudioOutputSyncSettings syncSettings;
    NEXUS_AudioOutputStatus status;
    NEXUS_AudioEqualizerHandle equalizer;
    BAVC_AudioSamplingRate sampleRate;
    bool settingsChanged;
    bool trickMute;
} NEXUS_AudioOutputData;

/***************************************************************************
Summary: 
    Is an output a DAC slave?
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsDacSlave(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary: 
    Is an output a SPDIF slave (e.g. ARC)?
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsSpdifSlave(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary: 
    Set a DAC slave source.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetSlaveSource(
    NEXUS_AudioOutput slaveHandle,
    NEXUS_AudioOutput sourceHandle
    );

/***************************************************************************
Summary: 
    Get mixer settings required for an output
 ***************************************************************************/
void NEXUS_AudioOutput_P_GetMixerSettings(
    NEXUS_AudioOutput output, 
    BAPE_MixerSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary: 
    Set output format
 ***************************************************************************/
void NEXUS_AudioOutput_P_SetOutputFormat(
    NEXUS_AudioOutput output, 
    NEXUS_AudioInputFormat format
    );

/***************************************************************************
Summary: 
    Determine if an audio output is active
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsRunning(
    NEXUS_AudioOutput output
    );
    
/***************************************************************************
Summary: 
    Create output-private data structure
 ***************************************************************************/
NEXUS_AudioOutputData *NEXUS_AudioOutput_P_CreateData(
    NEXUS_AudioOutput output
    );
    
/***************************************************************************
Summary: 
    Get the equalizer handle for an output
 ***************************************************************************/
NEXUS_AudioEqualizerHandle NEXUS_AudioOutput_P_GetEqualizer(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary: 
    Get the equalizer handle for an output
 ***************************************************************************/
void NEXUS_AudioEqualizer_P_GetStages(
    NEXUS_AudioEqualizerHandle handle,
    BAPE_EqualizerStageHandle **pStages,
    unsigned *pNumStages
    );

#if NEXUS_HAS_AUDIO_MUX_OUTPUT
/***************************************************************************
Summary: 
    Link audio mux output to a particular node
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMuxOutput_P_AddInput(
    NEXUS_AudioMuxOutputHandle handle, 
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary: 
    Unlink audio mux output from a particular node
 ***************************************************************************/
void NEXUS_AudioMuxOutput_P_RemoveInput(
    NEXUS_AudioMuxOutputHandle handle, 
    NEXUS_AudioInput input
    );
#endif

/***************************************************************************
Summary: 
    Reset MuxOutput nodes on a watchdog event
 ***************************************************************************/
void NEXUS_AudioMuxOutput_P_WatchdogReset(void);

/***************************************************************************
Summary: 
    Set the Spdif master for a slaved Audio Return Channel output.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioReturnChannel_P_SetMaster(
    NEXUS_AudioOutput slaveHandle,      /* handle of ARC (slave) output device */
    NEXUS_AudioOutput sourceHandle      /* handle of SPDIF master */
    );

/***************************************************************************
Summary: 
    Initialize the decoder module
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Init(void);

/***************************************************************************
Summary: 
    Un-Initialize the decoder module
 ***************************************************************************/
void NEXUS_AudioDecoder_P_Uninit(void);

/***************************************************************************
Summary: 
    Is a decoder running?
 ***************************************************************************/
bool NEXUS_AudioDecoder_P_IsRunning(NEXUS_AudioDecoderHandle handle);

/***************************************************************************
Summary: 
    Set TSM state for the decoder
 ***************************************************************************/
void NEXUS_AudioDecoder_P_SetTsm(NEXUS_AudioDecoderHandle handle);

/***************************************************************************
Summary: 
    Set Decode rate for the decoder
 ***************************************************************************/
void NEXUS_AudioDecoder_P_SetDecodeRate(NEXUS_AudioDecoderHandle handle);

/***************************************************************************
Summary: 
    Start the decoder
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Start(NEXUS_AudioDecoderHandle handle);

/***************************************************************************
Summary: 
    Stop the decoder
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Stop(NEXUS_AudioDecoderHandle handle, bool flush);

/***************************************************************************
Summary: 
    Set trick mode mute
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_SetTrickMute(NEXUS_AudioDecoderHandle decoder, bool muted);

/***************************************************************************
Summary: 
    Reset decoder trick state
 ***************************************************************************/
void NEXUS_AudioDecoder_P_TrickReset( NEXUS_AudioDecoderHandle decoder);

/***************************************************************************
Summary: 
    Reset audio DSP(s)
 ***************************************************************************/
void NEXUS_AudioDecoder_P_Reset(void);

/***************************************************************************
Summary: 
    Is a playback running?
 ***************************************************************************/
bool NEXUS_AudioPlayback_P_IsRunning(NEXUS_AudioPlaybackHandle handle);

/***************************************************************************
Summary: 
    Convert TruVolume settings to magnum
 ***************************************************************************/
void NEXUS_TruVolume_P_ConvertSettings(const NEXUS_TruVolumeSettings *pNexus, BAPE_TruVolumeSettings *pMagnum);

/***************************************************************************
Summary: 
    Is an I2sInput running?
 ***************************************************************************/
bool NEXUS_I2sInput_P_IsRunning(NEXUS_I2sInputHandle handle);

/***************************************************************************
Summary: 
    Is an InputCapture running?
 ***************************************************************************/
#if NEXUS_NUM_AUDIO_INPUT_CAPTURES > 0
bool NEXUS_AudioInputCapture_P_IsRunning(NEXUS_AudioInputCaptureHandle handle);
#endif

#if NEXUS_NUM_RF_AUDIO_DECODERS > 0
BDBG_OBJECT_ID_DECLARE(NEXUS_RfAudioDecoder);
typedef struct NEXUS_RfAudioDecoder
{
    BDBG_OBJECT(NEXUS_RfAudioDecoder)
    bool started;
    bool running;
    bool opened;
    unsigned outputScaling;
    NEXUS_RfAudioDecoderSettings settings;
	BAPE_DecoderHandle decoder;
	BAPE_InputCaptureHandle inputCapture;
	BAPE_RfInputHandle input;
    NEXUS_AudioInputObject connector;
    NEXUS_RfAudioDecoderMode currentMode;
    void *pStatusData;
    BKNI_EventHandle statusEvent, stopEvent, stopAckEvent;
    NEXUS_TaskCallbackHandle statusChangedCallback;
    NEXUS_TaskCallbackHandle standardDetectedCallback;
    BANA_Handle hAna;
    bool runMagShiftThread;
    NEXUS_ThreadHandle magShiftThread;
    BKNI_EventHandle magShiftChangeEvent;
    bool runAudioClipThread;
    NEXUS_ThreadHandle audioClipThread;
    BKNI_EventHandle audioClipEvent;
} NEXUS_RfAudioDecoder;

/***************************************************************************
Summary:
    Init the RF audio decoder sub-module
 ***************************************************************************/
NEXUS_Error NEXUS_RfAudioDecoder_P_Init(void);

/***************************************************************************
Summary:
    Un-Init the RF audio decoder sub-module
 ***************************************************************************/
void NEXUS_RfAudioDecoder_P_Uninit(void);

/***************************************************************************
Summary:
    Determine if a capture channel is running.
 ***************************************************************************/
bool NEXUS_RfAudioDecoder_P_IsRunning(
    NEXUS_RfAudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Initialize RF Audio decoder status
 ***************************************************************************/
NEXUS_Error NEXUS_RfAudioDecoder_P_InitStatus(
    NEXUS_RfAudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Uninitialize RF Audio decoder status
 ***************************************************************************/
void NEXUS_RfAudioDecoder_P_UninitStatus(
    NEXUS_RfAudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Reset A2 Primary Carrier Status on channel change
 ***************************************************************************/
void NEXUS_RfAudioDecoder_P_ResetStatus(
    NEXUS_RfAudioDecoderHandle handle,
    bool retune
    );
#endif

#if NEXUS_NUM_ANALOG_AUDIO_INPUTS
BDBG_OBJECT_ID_DECLARE(NEXUS_AnalogAudioInput);
/***************************************************************************
Summary: 
    Analog Audio Input
 ***************************************************************************/
typedef struct NEXUS_AnalogAudioInput
{
    BDBG_OBJECT(NEXUS_AnalogAudioInput)
    NEXUS_AnalogAudioInputSettings settings;
    NEXUS_AudioInputObject connector;
} NEXUS_AnalogAudioInput;
#endif

#if NEXUS_NUM_ANALOG_AUDIO_DECODERS
/***************************************************************************
Summary:
    Determine if a capture channel is running.
 ***************************************************************************/
bool NEXUS_AnalogAudioDecoder_P_IsRunning(
    NEXUS_AnalogAudioDecoderHandle handle
    );
#endif

#if NEXUS_NUM_SPDIF_INPUTS
NEXUS_Error NEXUS_SpdifInput_P_GetInputPortStatus(
    NEXUS_SpdifInputHandle input,
    NEXUS_AudioInputPortStatus *pStatus     /* [out] */
    );
    
NEXUS_Error NEXUS_SpdifInput_P_SetFormatChangeInterrupt(
    NEXUS_SpdifInputHandle handle,
    void (*pCallback_isr)(void *, int),
    void *pParam1,
    int param2
    );
#endif

/***************************************************************************
Summary: 
    Input Capture Indexes
 ***************************************************************************/
#define NEXUS_AUDIO_CAPTURE_INDEX_BASE (0)
#ifdef NEXUS_NUM_AUDIO_INPUT_CAPTURES
#define NEXUS_I2S_CAPTURE_INDEX_BASE (NEXUS_NUM_AUDIO_INPUT_CAPTURES)
#else
#define NEXUS_I2S_CAPTURE_INDEX_BASE (0)
#endif
#define NEXUS_ADC_CAPTURE_INDEX_BASE (0)
#define NEXUS_RF_CAPTURE_INDEX_BASE (0)
#define NEXUS_AUDIO_CAPTURE_INDEX(typ, idx) (((typ)==NEXUS_AudioInputType_eInputCapture)?NEXUS_AUDIO_CAPTURE_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eI2s)?NEXUS_I2S_CAPTURE_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eAnalogDecoder)?NEXUS_ADC_CAPTURE_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eRfDecoder)?NEXUS_RF_CAPTURE_INDEX_BASE+(idx):\
                                             (BDBG_ASSERT(0),0))
 
/***************************************************************************
Summary: 
    Audio Decoder Indexes
 ***************************************************************************/
#define NEXUS_AUDIO_DECODER_INDEX_BASE (0)
#define NEXUS_I2S_DECODER_INDEX_BASE (NEXUS_NUM_AUDIO_DECODERS)
#define NEXUS_ADC_DECODER_INDEX_BASE (0)
#define NEXUS_RF_DECODER_INDEX_BASE (0)
#define NEXUS_AUDIO_DECODER_INDEX(typ, idx) (((typ)==NEXUS_AudioInputType_eDecoder)?NEXUS_AUDIO_DECODER_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eI2s)?NEXUS_I2S_DECODER_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eAnalogDecoder)?NEXUS_ADC_DECODER_INDEX_BASE+(idx):\
                                             ((typ)==NEXUS_AudioInputType_eRfDecoder)?NEXUS_RF_DECODER_INDEX_BASE+(idx):\
                                             (BDBG_ASSERT(0),0))

/***************************************************************************
Summary: 
    Convert between nexus/avc codecs
 ***************************************************************************/
NEXUS_AudioCodec NEXUS_Audio_P_MagnumToCodec(BAVC_AudioCompressionStd codec);
BAVC_AudioCompressionStd NEXUS_Audio_P_CodecToMagnum(NEXUS_AudioCodec codec);
NEXUS_Error NEXUS_AudioDecoder_P_ConfigureRave(NEXUS_AudioDecoderHandle handle, NEXUS_RaveHandle rave, const NEXUS_AudioDecoderStartSettings *pProgram, const NEXUS_PidChannelStatus * pPidStatus);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_3dSurround);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Ac3Encode);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioCapture);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDac);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoderPrimer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDummyOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEncoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEqualizer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMixer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMuxOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioPlayback);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AutoVolumeLevel);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DolbyVolume);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DolbyVolume258);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_DtsEncode);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sMultiOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_I2sOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_RfAudioEncoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SpdifInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SpdifOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_TruVolume);


#endif /* #ifndef NEXUS_AUDIO_MODULE_H_ */
