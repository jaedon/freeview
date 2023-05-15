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
* $brcm_Workfile: nexus_audio_module.h $
* $brcm_Revision: 16 $
* $brcm_Date: 10/5/12 9:46a $
*
* API Description:
*   API name: Audio Module
*    Module includes
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_mips/nexus_audio_module.h $
* 
* 16   10/5/12 9:46a gmohile
* SW7408-1 : Add save rave context
* 
* 15   7/30/12 4:35p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
* 14   7/27/12 4:38p jgarrett
* SW7420-2085: Converting from BDBG_OBJECT to NEXUS_OBJECT
* 
* 13   7/27/12 3:50p jgarrett
* SW7435-235: Fixing restructuring symlink issues
* 
* 12   5/2/12 5:04p jgarrett
* SW7425-2802: Adding 7408 stubs for missing 7400/7405 routines
* 
* 11   5/2/12 1:56p jgarrett
* SW7425-2802: Adding runtime capabilities
* 
* 10   12/15/11 8:20p bandrews
* SW7425-1772: port to 7408
* 
* 9   10/3/11 10:23a erickson
* SW7408-304: call Nexus_Core_P_Img_Destroy
* 
* 8   7/25/11 3:34p gmohile
* SW7125-1014 : Update audio standby function
* 
* 7   4/12/11 6:47p bandrews
* SW7408-37: move UART logging to separate timer task; allow logging when
*  TSM disabled
* 
* 6   2/22/11 11:34a gmohile
* SW7408-210 : Fix build error
* 
* 5   11/17/10 3:35p jgarrett
* SW7408-137: Adding buffer capture extension
* 
* 4   10/22/10 4:43p jgarrett
* SW7408-133: Implementing mixer input volume control in nexus
* 
* 3   8/20/10 4:08p jgarrett
* SW7408-52: Adding multichannel support
* 
* 2   12/9/09 12:05p jgarrett
* SW7408-17: Adding stubs
* 
* 1   11/20/09 5:22p jgarrett
* SW7408-17: Adding soft_audio
*
***************************************************************************/

#ifndef NEXUS_AUDIO_MODULE_H_
#define NEXUS_AUDIO_MODULE_H_

#include "bape.h"

#include "nexus_base.h"
#include "nexus_audio_thunks.h"
#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio.h"
#include "nexus_audio_init.h"
#include "priv/nexus_audio_standby_priv.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_extra.h"
#include "nexus_audio_decoder_primer.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_audio_output_group.h"
#include "nexus_audio_capture.h"
#include "nexus_3d_surround.h"
#include "nexus_ac3_encode.h"
#include "nexus_audio_encoder.h"
#include "nexus_auto_volume_level.h"
#include "nexus_dolby_digital_reencode.h"
#include "nexus_dolby_volume.h"
#include "nexus_dts_encode.h"
#include "nexus_tru_volume.h"  
#include "nexus_rf_audio_encoder.h"       
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
#include "bape.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_audio_decoder_priv.h"
#if NEXUS_AUDIO_BUFFER_CAPTURE_EXT
#include "nexus_audio_buffer_capture.h"
#include "nexus_audio_buffer_capture_priv.h"
#endif
#include "nexus_echo_canceller.h"
#include "nexus_audio_equalizer.h"
#include "nexus_audio_return_channel.h"
#include "nexus_audio_dsp.h"
#include "nexus_spdif_input.h"
#include "nexus_audio_input_capture.h"
#include "nexus_audio_dummy_output.h"
#include "nexus_audio_mux_output.h"

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
    BAPE_Handle apeHandle;
    NEXUS_AudioModuleSettings settings;
    void *pImageContext;
} NEXUS_AudioModuleData;

extern NEXUS_AudioModuleData g_NEXUS_audioModuleData;

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
    NEXUS_EventCallbackHandle sampleRateCallback;
    NEXUS_EventCallbackHandle channelChangeReportEventHandler;
    NEXUS_IsrCallbackHandle lockCallback;
    NEXUS_IsrCallbackHandle firstPtsCallback;
    NEXUS_IsrCallbackHandle ptsErrorCallback;
    NEXUS_StcChannelDecoderType stcDecoderType;
    unsigned stcPriority;
    unsigned ptsErrorCount;
    NEXUS_AudioInputFormat compressedFormat;
    NEXUS_AudioOutputList outputLists[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioInputObject connectors[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioDecoderTrickState trickState;
    NEXUS_AudioInputSyncSettings syncSettings;
#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
        NEXUS_AudioDecoderAstmSettings settings;
        NEXUS_AudioDecoderAstmStatus status;
    } astm;
#endif
    NEXUS_AudioOutput hdmiOutput;
    NEXUS_IsrCallbackHandle sourceChangeAppCallback;
    NEXUS_AudioDecoderHandle descriptorParent;
    uint32_t lastCdbValidPointer;
    uint32_t lastCdbReadPointer;
    unsigned staticFifoCount, numFifoOverflows, numFifoUnderflows;
    NEXUS_TimerHandle fifoWatchdogTimer;
    NEXUS_TimerHandle debugLogTimer;
} NEXUS_AudioDecoder;

/* Utility functions */
BAVC_AudioSamplingRate NEXUS_AudioModule_P_SampleRate2Avc(unsigned sampleRate);
unsigned NEXUS_AudioModule_P_Avc2SampleRate(BAVC_AudioSamplingRate sampleRate);

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
    Get the mixer input volume for a particular input
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_SetVolume(
    NEXUS_AudioInput input, 
    const BAPE_MixerInputVolume *pInputVolume
    );

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
    Is a playback running?
 ***************************************************************************/
bool NEXUS_AudioPlayback_P_IsRunning(NEXUS_AudioPlaybackHandle handle);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_3dSurround);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Ac3Encode);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioCapture);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDac);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoderPrimer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDummyOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEncoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioEqualizer);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMuxOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioMixer);
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
