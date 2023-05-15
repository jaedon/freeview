/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape.h $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 5/3/12 5:05p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape.h $
 * 
 * Hydra_Software_Devel/23   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/22   4/27/12 2:18p jgarrett
 * SW7425-2690: Adding numVcxos to capabilities
 * 
 * Hydra_Software_Devel/21   4/27/12 1:37p jgarrett
 * SW7425-2802: Adding runtime capabilities routine
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:46p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/20   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/19   12/19/11 4:27p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * Hydra_Software_Devel/18   8/24/11 11:53a jgarrett
 * SW7425-724: Adding RF Audio Encoder
 * 
 * Hydra_Software_Devel/17   8/12/11 6:53p jgarrett
 * SWDTV-6627: Refactoring initial EQ code
 * 
 * Hydra_Software_Devel/16   7/15/11 5:51p jgarrett
 * SWDTV-6761: Adding StudioSound API
 * 
 * Hydra_Software_Devel/15   6/1/11 6:30p jgarrett
 * SW7425-406: Updating buffer comments
 * 
 * Hydra_Software_Devel/14   5/23/11 12:46p gskerl
 * SW7231-128: Changed tabs to spaces
 * 
 * Hydra_Software_Devel/13   5/18/11 2:30p gskerl
 * SW7231-128: Added BCHP_PWR support to APE
 * 
 * Hydra_Software_Devel/12   5/14/11 11:33a jgarrett
 * SWDTV-6763: Adding 3D Surround
 * 
 * Hydra_Software_Devel/11   5/14/11 11:22a jgarrett
 * SWDTV-6762: Adding Audyssey ADV/ABX
 * 
 * Hydra_Software_Devel/10   5/13/11 6:39p jgarrett
 * SW7425-410: Adding AVL
 * 
 * Hydra_Software_Devel/9   5/11/11 7:04p jgarrett
 * SW7422-410: Adding CustomProcessing
 * 
 * Hydra_Software_Devel/8   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   4/11/11 5:54p jgarrett
 * SWDTV-6305: Adding ADC/RF Inputs for DTV
 * 
 * Hydra_Software_Devel/6   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/5   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/4   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 * Hydra_Software_Devel/3   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/2   12/14/10 2:39p jgarrett
 * SW7408-146: Renaming MixerOutput to OutputPort
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_H_
#define BAPE_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bdsp.h"
#include "bape_types.h"
#include "bape_mixer.h"
#include "bape_decoder.h"
#include "bape_playback.h"
#include "bape_input.h"
#include "bape_output.h"
#include "bape_mux_output.h"
#include "bape_pll.h"
#include "bape_input_capture.h"
#include "bape_output_capture.h"
#include "bape_encoder.h"
#include "bape_dolby_digital_reencode.h"
#include "bape_dolby_volume.h"
#include "bape_tru_surround.h"
#include "bape_tru_volume.h"
#include "bape_custom_processing.h"
#include "bape_auto_volume_level.h"
#include "bape_audyssey_abx.h"
#include "bape_audyssey_adv.h"
#include "bape_3d_surround.h"
#include "bape_studio_sound.h"
#include "bape_equalizer.h"
#include "bape_rf_encoder.h"
#include "bape_echo_canceller.h"

/*=************************ Module Overview ******************************** 
APE (Audio Processing Engine) is a porting interface (PI) module that 
controls the audio subsystem.  This includes the AIO (mixer, inputs, outputs) 
as well as the decode processor (DSP or CPU). 
 
Device: 
-------
An APE Device is identified by a device handle (BAPE_Handle).  This is the 
top-level interface to the audio subsystem. 
 
Decoder: 
-------- 
A decoder is capable of decoding digital audio into PCM samples and also 
providing compressed passthrough of input data.  This is identified by 
a BAPE_DecoderHandle.  Decoding is done in a decode processor, and the 
output can be fed into a mixer.  Optionally, post-processing (BAPE_Processor) 
may be inserted between the decoder and the mixer.   Decoders can read 
from a RAVE context (BAVC_XptContextMap) or on some systems can read 
from an input port (BAPE_InputPort).  The latter is useful for external 
inputs that provide compressed data (e.g. SPDIF) or inputs that require 
post processing.   

Playback: 
--------- 
Playback is used for playing host-generated PCM data.  It is also an 
input to a mixer. 
 
InputCapture: 
------------- 
InputCapture is used to capture PCM input from an input port (BAPE_InputPort) 
and feeds data into a mixer. 
 
InputGroup: 
----------- 
Input groups allow for multiple stereo inputs to be grouped together 
and treated as multichannel data.  This is useful if you want to combine 
three stereo playback channels to generate 5.1 audio for example.  Grouped 
inputs will always start synchronously in the mixer. 
 
Mixer: 
------ 
Mixers are used to mix one or more inputs to one or more outputs.  A 
mixer is always required to bind an output to its input(s).   Mixers 
provide input volume scaling as well as output volume scaling.  All 
inputs to a mixer must run at the same sample rate, and correspondingly 
they will be sample rate converted to either a fixed sample rate value 
or they can slave to a master input's sampling rate.  It is possible 
for an input to be routed to more than one mixer.  For example, the 
application may want different input volume levels applied to one 
output vs. another one.  For example, you could use two mixers to 
send a decode input mixed with a playback input to one output setting 
both inputs to half volume.  You could simultaneously route the same 
decode input to another output at full volume with no playback 
by using two mixers and applying different volume levels to each. 
Inputs to a mixer are connected using a generic connector type 
(BAPE_Connector), and similarly outputs are connected using 
a generic connector type (BAPE_OutputPort). 
 
Outputs:
-------- 
Several different output types are supported by this module.  They include 
audio DACs (BAPE_DacHandle), SPDIF transmitters (BAPE_SpdifOutputHandle), 
I2S transmitters (BAPE_I2sOutputHandle) and the MAI interface to HDMI 
(BAPE_MaiOutputHandle).  Outputs must always be connected to a single 
mixer to receive data, even if they are receiving compressed output. 
Output data can also be captured by the host by connecting an 
BAPE_OutputCaptureHandle. 
 
Inputs: 
------- 
External audio inputs are supported by this module.  BAPE_I2sInput 
represents an I2S receiver.  In the future, other inputs may be 
added as well. 
 
Example Connection Diagrams: 
---------------------------- 
1) Single decoder routed to DAC + SPDIF 
BAPE_DecoderHandle (stereo data) -> BAPE_MixerHandle -> BAPE_DacHandle + BAPE_SpdifOutputHandle 
 
2) Decoder + host data mixed to DAC, Decoder data routed directly to SPDIF (PCM) 
BAPE_PlaybackHandle --------------\  
BAPE_DecoderHandle (stereo data) --> BAPE_MixerHandle1 -> BAPE_DacHandle 
                                 \-> BAPE_MixerHandle2 -> BAPE_SpdifOutputHandle
 
3) Decode to DAC with compressed passthrough to SPDIF 
 
BAPE_DecoderHandle1 (stereo data)     -> BAPE_MixerHandle1 -> BAPE_DacHandle 
BAPE_DecoderHandle2 (compressed data) -> BAPE_MixerHandle2 -> BAPE_SpdifOutputHandle 
 
4) Decode to DAC and I2S with DAC delayed to compensate for an external 
audio processor on I2S. 
BAPE_DecoderHandle (stereo data) --> BAPE_MixerHandle1 (delay=0) -> BAPE_DacHandle 
                                 \-> BAPE_MixerHandle2 (delay=n) -> BAPE_I2sOutputHandle
 
5) I2S stereo input routed to DAC 
BAPE_I2sInput -> BAPE_InputCapture -> BAPE_MixerHandle -> BAPE_DacHandle 
 
6) Decode of primary audio mixed with decode of secondary audio (e.g. MP3 sound effect) 
BAPE_DecoderHandle1 (stereo data) --> BAPE_MixerHandle -> BAPE_DacHandle 
BAPE_DecoderHandle2 (stereo data) -/
 
***************************************************************************/

/***************************************************************************
Summary:
Device Settings
***************************************************************************/
typedef struct BAPE_Settings
{
    unsigned maxDspTasks;               /* Maximum DSP tasks.  One task is required for each decoder and FW Mixer that will run concurrently. */
    unsigned maxIndependentDelay;       /* Max independent delay value in ms */
    unsigned maxPcmSampleRate;          /* Max PCM sample rate in Hz */
    unsigned numPcmBuffers;             /* Number of discrete PCM decoder output buffers required.  This is the
                                           number of outputs that will receive discrete stereo content plus the
                                           number of outputs that will receive discrete multichannel output * the number
                                           of channel pairs involved in multichannel (3 for 5.1, 4 for 7.1).  
                                           Independent delay does not affect this number, only different content sources. 
                                           These buffers are also used when non-DSP data sources (e.g. Playback or Equalizer) connect 
                                           as input to DSP mixers. */ 
    unsigned numCompressedBuffers;      /* Number of discrete compressed decoder output buffers required.  This is the
                                           number of outputs that will receive discrete compressed content running at up to 48kHz.
                                           Independent delay does not affect this number, only different content sources. */ 
    unsigned numCompressed4xBuffers;    /* Number of discrete compressed decoder output buffers required.  This is the
                                           number of outputs that will receive discrete compressed content running at up to 192kHz.
                                           Independent delay does not affect this number, only different content sources. */ 
    unsigned numCompressed16xBuffers;   /* Number of discrete compressed decoder output buffers required.  This is the
                                           number of outputs that will receive discrete compressed content running at up to 768kHz (HDMI HBR).
                                           Independent delay does not affect this number, only different content sources. */ 
    unsigned numRfEncodedPcmBuffers;    /* Number of discrete RF encoded PCM buffers required.  This is the
                                           number of outputs that will receive discrete RF encoded PCM content running at up to 192kHz (BTSC).
                                           Independent delay does not affect this number, only different content sources. */     
    bool rampPcmSamples;                /* If true (default), PCM samples will be ramped up/down on startup, shutdown, and underflow conditions.  
                                          Set to false if you want to disable this feature for testing or verification purposes. */
    BAPE_LoudnessEquivalenceMode loudnessMode;  /* Loudness Equivalence Mode.  Default is BAPE_LoudnessEquivalenceMode_eNone. */
} BAPE_Settings;

/***************************************************************************
Summary:
Get default settings for an audio processor
***************************************************************************/
void BAPE_GetDefaultSettings(
    BAPE_Settings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Open an audio processor
***************************************************************************/
BERR_Code BAPE_Open(
    BAPE_Handle *pHandle,   /* [out] returned handle */
    BCHP_Handle chpHandle,
    BREG_Handle regHandle,
    BMEM_Handle memHandle,
    BINT_Handle intHandle,
    BTMR_Handle tmrHandle,
    BDSP_Handle dspHandle,
    const BAPE_Settings *pSettings  /* NULL will use default settings */
    );

/***************************************************************************
Summary:
Close an audio processor
***************************************************************************/
void BAPE_Close(
    BAPE_Handle handle
    );

/***************************************************************************
Summary:
Device Level Interrupts
***************************************************************************/
typedef struct BAPE_InterruptHandlers
{
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;        
    } watchdog;
} BAPE_InterruptHandlers;

/***************************************************************************
Summary:
Get Currently Registered Interrupt Handlers
***************************************************************************/
void BAPE_GetInterruptHandlers(
    BAPE_Handle handle,
    BAPE_InterruptHandlers *pInterrupts     /* [out] */
    );

/***************************************************************************
Summary:
Set Interrupt Handlers 
 
Description: 
To disable any unwanted interrupt, pass NULL for its callback routine
***************************************************************************/
BERR_Code BAPE_SetInterruptHandlers(
    BAPE_Handle handle,
    const BAPE_InterruptHandlers *pInterrupts
    );

/***************************************************************************
Summary:
Process a watchdog interrupt
***************************************************************************/
BERR_Code BAPE_ProcessWatchdogInterrupt(
    BAPE_Handle handle
    );

/***************************************************************************
Summary:
Retrieve the audio path delay value in ms.
***************************************************************************/
void BAPE_GetDecoderPathDelay(
    BAPE_Handle handle,
    unsigned *pDelay    /* [out] in ms */
    );

/***************************************************************************
Summary:
Returns single null terminated line from the debug log
***************************************************************************/
BERR_Code BAPE_GetDebugLog(
    BAPE_Handle handle,
    char *buf,
    size_t buf_len
    );

/***************************************************************************
Summary:
Standby settings. Currently unused.
****************************************************************************/
typedef struct BAPE_StandbySettings
{
    bool dummy; /* placeholder to avoid compiler warning */
} BAPE_StandbySettings;

/***************************************************************************
Summary:
Enter Standby mode 
***************************************************************************/
BERR_Code BAPE_Standby(
    BAPE_Handle handle,             /* [in]APE device handle */
    BAPE_StandbySettings *pSettings /* [in] standby settings */
    );

/***************************************************************************
Summary:
Resume from standby mode
***************************************************************************/
BERR_Code BAPE_Resume(
    BAPE_Handle handle      /* [in] APE device handle */
    );

/***************************************************************************
Summary:
Audio HW/FW Capabilities
***************************************************************************/
typedef struct BAPE_Capabilities
{
    struct
    {
        unsigned i2s;                   /* Number of I2S inputs */
        unsigned mai;                   /* Number of MAI inputs */
        unsigned spdif;                 /* Number of SPDIF inputs */
    } numInputs;

    struct
    {
        unsigned audioReturnChannel;    /* Number of ARC outputs */
        unsigned capture;               /* Maximum number of OutputCapture objects */
        unsigned dac;                   /* Number of DAC outputs */
        unsigned dummy;                 /* Maximum number of DummyOutput objects */
        unsigned i2s;                   /* Number of I2S outputs */
        unsigned loopback;              /* Maximum number of Loopback objects */
        unsigned mai;                   /* Number of MAI outputs */
        unsigned rfmod;                 /* Number of RF Mod outputs */
        unsigned spdif;                 /* Number of SPDIF outputs */
    } numOutputs;

    unsigned numDecoders;               /* Maximum number of Decoders */
    unsigned numPlaybacks;              /* Maximum number of Playbacks */
    unsigned numInputCaptures;          /* Maximum number of InputCaptures */
    unsigned numVcxos;                  /* Number of VCXO PLLs that feed PLLs */
    unsigned numPlls;                   /* Number of output PLL clocks */
    unsigned numNcos;                   /* Number of output NCO clocks */
   
    unsigned numDsps;                   /* Number of audio DSPs */
    struct
    {
        struct
        {
            bool decode;                /* True if this codec can be decoded */
            bool encode;                /* True if this codec can be encoded */
        } codecs[BAVC_AudioCompressionStd_eMax];

        bool audysseyAbx;               /* True if Audyssey ABX is supported */
        bool audysseyAdv;               /* True if Audyssey ADV is supported */
        bool autoVolumeLevel;           /* True if AutoVolumeLevel is supported */
        bool _3dSurround;               /* True if 3dSurround is supported */
        bool decodeRateControl;         /* True if DSOLA audio rate control is supported */
        bool dolbyDigitalReencode;      /* True if Dolby MS11 is supported */
        bool dolbyVolume;               /* True if DolbyVolume processing is supported */

        struct
        {
            bool supported;             /* True if echo cancellation is supported */
            bool algorithms[BAPE_EchoCancellerAlgorithm_eMax];  /* Each supported algorithm will be set to true */
        } echoCanceller;

        bool encoder;                   /* True if encoder is supported */
        bool mixer;                     /* True if DSP mixers are supported */
        bool muxOutput;                 /* True if MuxOutput is supported for transcoding */

        struct
        {
            bool supported;             /* True if RF encoder is supported */
            bool encodings[BAPE_RfAudioEncoding_eMax];  /* Each supported encoding will be set to true */
        } rfEncoder;

        bool studioSound;               /* True if SRS StudioSound processing is supported */
        bool truSurroundHd;             /* True if SRS TruSurround HD processing is supported */
        bool truVolume;                 /* True if SRS TruVolume processing is supported */
    } dsp;

    struct
    {
        bool supported;                 /* True if Equalizer support is available */
        bool types[BAPE_EqualizerStageType_eMax];   /* Each supported equalizer type will be set to true */
    } equalizer;

} BAPE_Capabilities;

/***************************************************************************
Summary:
Get Audio HW/FW Capabilities
***************************************************************************/
void BAPE_GetCapabilities(
    BAPE_Handle hApe,
    BAPE_Capabilities *pCaps        /* [out] */
    );

#endif

