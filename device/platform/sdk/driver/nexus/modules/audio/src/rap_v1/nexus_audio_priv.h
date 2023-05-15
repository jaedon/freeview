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
* $brcm_Workfile: nexus_audio_priv.h $
* $brcm_Revision: 36 $
* $brcm_Date: 6/21/12 1:20p $
*
* API Description:
*   Private routines internal to the audio module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/src/nexus_audio_priv.h $
* 
* 36   6/21/12 1:20p mward
* SW7425-2741: add AudioDecoderPrimer
* 
* 35   6/20/12 1:15p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 34   12/13/11 9:44p bandrews
* SW7425-1772: merge to main
* 
* SW7425-1772/1   12/13/11 9:36p bandrews
* SW7425-1772: port to 7400
* 
* 33   9/16/11 5:24p jgarrett
* SW7125-1092: Adding RfAudioEncoder interface
* 
* 32   7/11/11 5:59p mward
* SW7335-1280: Stubbing 3D Surround for 7400.
* 
* 31   4/27/11 4:19p jgarrett
* SW7420-1826: Adding AudioOutputGroup Interface
* 
* 30   1/6/11 11:55a mward
* SW7425-41: Moving encoder API to all chips
* 
* 29   8/20/10 1:04p mward
* SW7420-790: Merging MS11 updates to main branch
* 
* 28   5/19/10 1:24p mward
* SW7400-2768:  Clean up volume processing stages at module uninit in
*  case Nexus user fails to.
* 
* 27   2/12/10 5:53p mward
* SW7405-3774: add mute control support to sync channel
* 
* 26   11/24/09 5:46p mward
* SW7400-2534:  Remove no longer used (un)setting of #defines to prevent
*  confusion.
* 
* 25   9/9/09 2:03p mward
* SW7400-2509: Add the BTRC ChnChange features.
* 
* 24   9/8/09 6:28p mward
* SW7400-2385: Independent delay fixes from PR56621 branch.
* 
* PR56621/1   8/5/09 5:20p mward
* PR56621: Independent delay must be set before output port is added.
* 
* 23   6/15/09 5:47p mward
* PR 53152: Added NEXUS_AudioDecoder_P_SetOutputMode() to prevent downmix
*  settings from interfering with multichannel mode settings.
* 
* 22   6/12/09 4:15p mward
* PR 49702: Switch to 3.2 or stereo for all supported algorithms/types,
*  as done in Settop API implementation.  Set PLLs for decode / passthru.
* 
* 21   6/8/09 6:13p mward
* PR55830:  Initial implementation of PCM audio capture for 7400.
* 
* 20   5/28/09 7:18p mward
* PR 49702: Add NEXUS_AudioOutput_P_SetHDMI().
* 
* 19   3/20/09 6:56p jgarrett
* PR 53293: Adding DolbyVolume
* 
* 18   3/2/09 5:06p mward
* PR52340: Adding TruVolume for STB platforms
* 
* 17   2/26/09 9:03p mward
* PR47739:  Trick mode updates merged from 7405.
* 
* 16   2/17/09 5:22p mward
* PR52200: PCM playback for 7400.
* 
* 15   11/20/08 2:06p mward
* PR47739: Also enable audio on RFM when setting DAC slave.
* 
* 14   11/17/08 11:40a mward
* PR47739: Add/remove port wrappers not needed.
* 
* 13   11/14/08 5:51p mward
* PR47739:  Functions to add/remove BRAP output ports only if not already
*  done.
* 
* 12   11/10/08 11:29a mward
* PR47739: Defer application of settings until the output is started
*  (opened).  Other updates from 7405 implementation.
* 
* 11   10/31/08 10:21a mward
* PR47739: Updates for HDMI audio support.
* 
* 10   10/27/08 7:19p mward
* PR47739: Add BDBG_OBJECT to NEXUS_AudioDecoder
* 
* 9   10/4/08 6:18a jgarrett
* PR 45709: PR 47226: Adding AC3 encode and making stereo downmix
*  optional
* 
* 8   9/10/08 5:59p jgarrett
* PR 46163: Adding DtsEncode
* 
* 7   8/21/08 5:52p jgarrett
* PR 45959: Removing trick state conditionals
* 
* 6   7/11/08 10:30a jgarrett
* PR 42924: PR 44645: Adding capture and decoder_extra
* 
* 5   3/25/08 5:25p bandrews
* PR40090: Added compile flag for ASTM
* 
* 4   3/24/08 4:31p bandrews
* PR40090: API changes
* 
* 3   2/28/08 8:42p bandrews
* PR37951: Added synclib support
* 
* 2   2/5/08 4:43p jgarrett
* PR 39017: Adding HdmiOutput
* 
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/13   1/8/08 7:37p jgarrett
* PR 38535: Adding volume conversion
* 
* Nexus_Devel/12   12/6/07 2:17p jgarrett
* PR 37971: Adding I2sOutput
* 
* Nexus_Devel/11   12/4/07 2:21p vsilyaev
* PR 36404: Fixed code to reset trick state
* 
* Nexus_Devel/10   12/4/07 2:12p vsilyaev
* PR 36404: Use structure to conrol audio trickmodes
* 
* Nexus_Devel/9   12/3/07 9:51a erickson
* PR34925: rename stc_channel.h
* 
* Nexus_Devel/8   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
* 
* Nexus_Devel/7   10/24/07 5:40p vsilyaev
* PR 36404: Added audio decode trick mode
* 
* Nexus_Devel/6   10/24/07 3:00p vsilyaev
* PR 36404: Trick mode extension
* 
* Nexus_Devel/5   10/4/07 2:26p erickson
* PR34594: removed audio descriptors
* 
* Nexus_Devel/4   9/28/07 5:16p jgarrett
* PR 34594: Adding sync thunks
* 
* Nexus_Devel/3   9/27/07 9:19p jgarrett
* PR 34954: Enabling SPDIF
* 
* Nexus_Devel/2   9/27/07 7:52p jgarrett
* PR 34594: Adding nexus_platform_features.h
* 
* Nexus_Devel/1   9/19/07 6:34p jgarrett
* PR 34543: Adding baseline implementation
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_PRIV_H__
#define NEXUS_AUDIO_PRIV_H__

#include "nexus_base.h"
#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_extra.h"
#include "brap.h"
#include "nexus_platform_features.h"
#include "nexus_audio_init.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_core_audio.h"
#include "priv/nexus_audio_input_priv.h"
#include "priv/nexus_audio_decoder_priv.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_dts_encode.h"
#include "nexus_ac3_encode.h"
#include "nexus_dolby_volume.h"
#include "nexus_audio_output_group.h"

#include "nexus_audio_dac.h"
#if NEXUS_NUM_I2S_OUTPUTS
#include "nexus_i2s_output.h"
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
#include "nexus_spdif_output.h"
#endif
#if 0 && NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_NUM_RF_MODS
#include "nexus_rf_mod.h"
#endif
#if NEXUS_NUM_AUDIO_PLAYBACKS
#include "nexus_audio_playback.h"
#endif

#include "nexus_tru_volume.h"
#include "nexus_audio_encoder.h"
#include "nexus_rf_audio_encoder.h"
#include "nexus_auto_volume_level.h"
#include "nexus_3d_surround.h"
#include "nexus_dolby_digital_reencode.h"
#include "nexus_audio_capture.h"

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_AudioDecoder);

typedef struct NEXUS_AudioDecoder
{
    NEXUS_OBJECT(NEXUS_AudioDecoder);
    unsigned index;
    NEXUS_AudioDecoderSettings settings;
    NEXUS_AudioDecoderStartSettings programSettings;
    bool opened;
    bool started;                   /* Has the channel been started by the app? */
    bool running;                   /* Is the channel actually active? (enabled && started) */
    bool trickMute;                 /* decoder muted (stopped) by request from the trick modes operations (Keep this unconditional for sake of simplicity) */
    bool trickForceStopped;         /* decoder muted (stopped) by request from the trick modes operations (Keep this unconditional for sake of simplicity) */
    bool channelStatusValid;        /* Is the channel status valid yet? */
    bool isDss;
    bool isPlayback;
    bool tsmPermitted;
    bool tsmEnabled;
    BRAP_DEC_AudioParams audioParams;
    BRAP_ChannelHandle rapChannel;
    NEXUS_RaveHandle raveContext;
    NEXUS_RaveHandle savedRaveContext;
    BKNI_EventHandle sampleRateEvent;
    BKNI_EventHandle sourceChangedEvent;
    BKNI_EventHandle layerChangeEvent;
    BKNI_EventHandle channelChangeReportEvent;
    NEXUS_EventCallbackHandle layerChangeCallback;
    NEXUS_EventCallbackHandle channelChangeReportEventHandler;
    NEXUS_StcChannelDecoderType stcDecoderType;
    unsigned stcPriority;
    unsigned ptsErrorCount;
    BRAP_DSPCHN_SampleRateChangeInfo sampleRateInfo;
    NEXUS_AudioOutputList outputLists[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioInputObject connectors[NEXUS_AudioDecoderConnectorType_eMax];
    NEXUS_AudioDecoderTrickState trickState;
    NEXUS_AudioInputSyncSettings syncSettings;
    bool syncMute;                 /* decoder muted (while running) by request from the sync module (Keep this unconditional for sake of simplicity) */
#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
	    NEXUS_AudioDecoderAstmSettings settings;
	    NEXUS_AudioDecoderAstmStatus status;
    } astm;
#endif
    NEXUS_AudioOutput hdmiOutput;
    bool mono_to_all;
} NEXUS_AudioDecoder;

/***************************************************************************
Summary:
    Start the audio decoder using the current program settings.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Start(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Start the audio decoder using the current program settings.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Stop(
    NEXUS_AudioDecoderHandle handle,
    bool flush
    );

/***************************************************************************
Summary:
    Determine if a decoder is running.
 ***************************************************************************/
bool NEXUS_AudioDecoder_P_IsRunning(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Retrieve the raptor channel for a decoder object.
 ***************************************************************************/
BRAP_ChannelHandle NEXUS_AudioDecoder_P_GetChannel(
    NEXUS_AudioDecoderHandle handle
    );

/***************************************************************************
Summary:
    Set the decoder's mute state for trick modes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_SetTrickMute(
    NEXUS_AudioDecoderHandle decoder,
    bool mute
    );

/***************************************************************************
Summary:
    Set the decoder's mute state for sync channel
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_SetSyncMute(
    NEXUS_AudioDecoderHandle decoder,
    bool mute
    );

#if NEXUS_NUM_AUDIO_PLAYBACKS
/***************************************************************************
Summary:
    Determine if a playback channel is running.
 ***************************************************************************/
bool NEXUS_AudioPlayback_P_IsRunning(
    NEXUS_AudioPlaybackHandle handle
    );

/***************************************************************************
Summary:
    Retrieve the raptor channel for a playback channel.
 ***************************************************************************/
BRAP_ChannelHandle NEXUS_AudioPlayback_P_GetChannel(
    NEXUS_AudioPlaybackHandle handle
    );
#endif

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
    Remove all destinations attached to this object and below.  Used when an
    association will become invalid.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_RemoveDestinations(
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
    Find the raptor channel handle for the specified connector.  If a mixer
    is present in the stream or there is no actual input connected, the 
    result will be NULL.  
 ***************************************************************************/
BRAP_ChannelHandle NEXUS_AudioInput_P_GetChannel(
    NEXUS_AudioInput input,
    NEXUS_AudioInput *pChannelInput  /* Actual input node with the channel.  Optional, pass NULL if not interested */
    );

/***************************************************************************
Summary:
    Retrieve all outputs downstream from this connector.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_GetOutputs(
    NEXUS_AudioInput input,
    NEXUS_AudioOutputList *pOutputList,
    bool directOnly                          /* If true, only outputs connected to this channel will be returned.
                                               If false, all outputs downstream will be returned, including those
                                               attached to downstream mixers. */
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
    Prepare the input chain to start.  Destinations will be attached
    and downstream connections will be made.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_PrepareToStart(
    NEXUS_AudioInput input
    );
/***************************************************************************
Summary:
    Prepare the input chain to start with some processing added beforehand.  
    Destinations will be attached to the provided association, and 
    downstream connections will be made.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioInput_P_PrepareToStartWithProcessing(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
    Invalidate all downstream destinations
    related to this object.  The channel is being closed or being repurposed.
 ***************************************************************************/
void NEXUS_AudioInput_P_InvalidateConnections(
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
    Validate if an output has a valid destination at the specified input.
 ***************************************************************************/
bool NEXUS_AudioInput_P_HasOutputDestination(
    NEXUS_AudioInput input,
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
    Return a downstream mixer handle if present
 ***************************************************************************/
NEXUS_AudioMixerHandle NEXUS_AudioInput_P_GetMixer(
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
	Determine if an output must be slaved to a DAC
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.  AudioInput will find the
    proper DAC to bind the output to and provide it in a call to SetSlaveSource
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsDacSlave(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
    For slave outputs, set the DAC source
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.  Mixer will find the proper
    DAC to bind the output to and provide it here.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetSlaveSource(
    NEXUS_AudioOutput slaveHandle,
    NEXUS_AudioOutput sourceHandle);

/***************************************************************************
Summary:
	Apply output-specific settings to the raptor structure
Description:
    Mixer will setup the raptor output port settings initially and then
    hand this structure off to the individual outputs to make their
    adjustments.  It requires knowledge of raptor structures, but no
    actual raptor function calls.  This is required for things like LR swap
    because some part of the logic is in a downmix mode and some is in the
    DAC settings.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_AdjustRapSettings(
    NEXUS_AudioOutput handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    );

/***************************************************************************
Summary:
	Inform an audio output of a sample rate change
Description:
    Certain outputs such as HDMI require notification of sample rate changes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SampleRateChange_isr(
    NEXUS_AudioOutput output,
    BAVC_AudioSamplingRate sampleRate
    );

/***************************************************************************
Summary:
	Apply some settings once an output port is connected.
Description:
    Some settings such as volume/mute can not be applied until an output is
    connected.  This routine will be called once
    AudioInput creates the raptor destination for this output in order
    to delay application of the settings until the correct time.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_ApplySettings(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
	Apply delay settings and other output port configuration 
	before an output port is added.
Description:
    Independent delay settings can not be applied after an output port is
    connected.  This routine will be called before NEXUS_AudioDecoder_P_Start()
	adds the output port in order apply the settings at the correct time.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_ApplyConfig(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
	Null out the independent delay settings.
Description:
    Independent delay settings should be nulled out when a port is removed,
	or they may interfere with setting them on the same or another port.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_ZeroDelay(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
	Mute or un-mute the decoder for trick operations
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetTrickMute(
    NEXUS_AudioOutput output,
    bool mute
    );
/***************************************************************************
Summary:
    Mute or un-mute an output for sync channel
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetSyncMute(
    NEXUS_AudioOutput output,
    bool mute
    );


/***************************************************************************
Summary:
	Set the output PLL to be used for this output.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetPll(
    NEXUS_AudioOutput output,
    unsigned pll
    );

/***************************************************************************
Summary:
    Enable the HBR mode on the output port
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetHbrMode(
    NEXUS_AudioOutput output,
    bool bEnableHbr
    );

/***************************************************************************
Summary:
    Determine if an output is connected to any inputs
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsConnected(
    NEXUS_AudioOutput output
    );

/***************************************************************************
Summary:
    Unlink from a parent channel
 ***************************************************************************/
void NEXUS_AudioCapture_P_Disconnect(
    NEXUS_AudioCaptureHandle handle
    );


/***************************************************************************
Summary:
    Set the connection to enable capture start/stop.
 ***************************************************************************/
void NEXUS_AudioCapture_P_SetConnection(
    NEXUS_AudioCaptureHandle handle,
    bool connect
    );

/***************************************************************************
Summary:
	Apply output-specific settings to the raptor structure
Description:
    Mixer will setup the raptor output port settings initially and then
    hand this structure off to the individual outputs to make their
    adjustments.  It requires knowledge of raptor structures, but no
    actual raptor function calls.  This is required for things like LR swap
    because some part of the logic is in a downmix mode and some is in the
    DAC settings.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDac_P_AdjustRapSettings(
    NEXUS_AudioDacHandle handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    );

#if NEXUS_NUM_SPDIF_OUTPUTS
/***************************************************************************
Summary:
	Apply output-specific settings to the raptor structure
Description:
    Mixer will setup the raptor output port settings initially and then
    hand this structure off to the individual outputs to make their
    adjustments.  It requires knowledge of raptor structures, but no
    actual raptor function calls.  This is required for things like LR swap
    because some part of the logic is in a downmix mode and some is in the
    DAC settings.
 ***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_P_AdjustRapSettings(
    NEXUS_SpdifOutputHandle handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    );
#endif

#if NEXUS_NUM_I2S_OUTPUTS
/***************************************************************************
Summary:
	Apply output-specific settings to the raptor structure
Description:
    Mixer will setup the raptor output port settings initially and then
    hand this structure off to the individual outputs to make their
    adjustments.  It requires knowledge of raptor structures, but no
    actual raptor function calls.  This is required for things like LR swap
    because some part of the logic is in a downmix mode and some is in the
    DAC settings.
 ***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_P_AdjustRapSettings(
    NEXUS_I2sOutputHandle handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    );
#endif

#if NEXUS_NUM_I2S_INPUTS
/***************************************************************************
Summary:
    Determine if a capture channel is running.
 ***************************************************************************/
bool NEXUS_I2sInput_P_IsRunning(
    NEXUS_I2sInputHandle handle
    );

/***************************************************************************
Summary:
    Retrieve the raptor channel for an i2s input object.
 ***************************************************************************/
BRAP_ChannelHandle NEXUS_I2sInput_P_GetChannel(
    NEXUS_I2sInputHandle handle
    );

#endif

#if NEXUS_NUM_ANALOG_AUDIO_INPUTS
BDBG_OBJECT_ID_DECLARE(NEXUS_AnalogAudioInput);

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

/***************************************************************************
Summary:
    Retrieve the raptor channel for an analog decoder object.
 ***************************************************************************/
BRAP_ChannelHandle NEXUS_AnalogAudioDecoder_P_GetChannel(
    NEXUS_AnalogAudioDecoderHandle handle
    );
#endif


/***************************************************************************
Summary:
	Init the DAC module -- No uninit is required.
***************************************************************************/
NEXUS_Error NEXUS_AudioDac_P_Init(void);

/***************************************************************************
Summary:
	Init the SPDIF module -- No uninit is required.
***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_P_Init(void);

/***************************************************************************
Summary:
	Init the Decoder module
***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_P_Init(bool *pbSupportAlgos);

/***************************************************************************
Summary:
	Un-Init the Decoder module
***************************************************************************/
void NEXUS_AudioDecoder_P_Uninit(void);

/***************************************************************************
Summary:
	Inform an audio output of a sample rate change
Description:
    Certain outputs such as HDMI require notification of sample rate changes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SampleRateChange_isr(
    NEXUS_AudioOutput output,
    BAVC_AudioSamplingRate sampleRate
    );

void NEXUS_AudioDecoder_P_TrickReset( NEXUS_AudioDecoderHandle decoder);
void NEXUS_AudioDecoder_P_SetTsm(NEXUS_AudioDecoderHandle decoder);
int32_t NEXUS_AudioModule_P_Vol2Magnum(NEXUS_AudioVolumeType type, int32_t volume);

/***************************************************************************
Summary:
    Set audio parameters for an HDMI output
Description:
    Set number of audio channels and compressed / PCM.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetHDMI(
    NEXUS_AudioOutput output,
    NEXUS_AudioInputFormat format
    );

/***************************************************************************
Summary:
    Initialize SRS TruVolume processing.
Description:
    Initialize tracking of SRS TruVolume stages for clean-up at uninit.
 ***************************************************************************/
NEXUS_Error NEXUS_TruVolume_P_Init(void);

/***************************************************************************
Summary:
    Uninitialize SRS TruVolume processing.
Description:
    Clean-up SRS TruVolume stages.
 ***************************************************************************/
void NEXUS_TruVolume_P_Uninit(void);

/***************************************************************************
Summary:
    Initialize Dolby Volume processing.
Description:
    Initialize tracking of Dolby stages for clean-up at uninit.
 ***************************************************************************/
NEXUS_Error NEXUS_DolbyVolume_P_Init(void);

/***************************************************************************
Summary:
    Uninitialize Dolby Volume processing.
Description:
    Clean-up Dolby Volume stages.
 ***************************************************************************/
void NEXUS_DolbyVolume_P_Uninit(void);

NEXUS_Error NEXUS_AudioDecoder_P_ConfigureRave(NEXUS_AudioDecoderHandle handle, NEXUS_RaveHandle rave, const NEXUS_AudioDecoderStartSettings *pProgram);
#endif /* #ifndef NEXUS_AUDIO_PRIV_H__ */

