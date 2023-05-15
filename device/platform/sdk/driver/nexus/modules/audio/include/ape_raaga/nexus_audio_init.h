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
 * $brcm_Workfile: nexus_audio_init.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *      Audio Initialization Routines
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/include/ape_raaga/nexus_audio_init.h $
 * 
 * 11   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 10   5/4/12 11:05a jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * SW7425-2075/1   4/6/12 12:43p jgarrett
 * SW7425-2075: Adding support for 4x/16x compressed audio
 * 
 * 9   2/17/12 6:04p jgarrett
 * SW7358-214: Changing heap handle to index
 * 
 * 8   12/20/11 4:13p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * 7   11/3/11 6:53p jgarrett
 * SW7422-400: Enabling audio debug logs on 40nm APE chips
 * 
 * 6   11/2/11 6:26p jgarrett
 * SW7422-400: Adding routines for audio DSP debug
 * 
 * 5   8/18/11 5:51p jgarrett
 * SWDTV-6306: Merge DTV APE changes to main branch
 * 
 * Nexus_APE_Integration/1   8/10/11 5:16p jgarrett
 * SWDTV-6306: Adding implicit capture to DSP instead of FMM for DTV
 * 
 * 4   7/1/11 4:03p jgarrett
 * SW7231-274: Adding optional audio firmware heap
 * 
 * 3   5/23/11 6:49p jgarrett
 * SW7425-402: Adding multichannel capture support
 * 
 * 2   4/19/11 6:08p jgarrett
 * SW7425-386: Enabling control of audio memory allocation
 * 
 * 1   4/11/11 5:52p jgarrett
 * SWDTV-6306: Adding preliminary Nexus APE support for DTV
 * 
 **************************************************************************/
#ifndef NEXUS_AUDIO_INIT_H__
#define NEXUS_AUDIO_INIT_H__

#include "nexus_types.h"
#include "nexus_audio_output.h"
#include "nexus_audio_dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_AUDIO_MODULE_FAMILY (7422)

/***************************************************************************
Summary:
Loudness Equivalence Modes
**************************************************************************/
typedef enum NEXUS_AudioLoudnessEquivalenceMode
{
    NEXUS_AudioLoudnessEquivalenceMode_eNone,       /* Default, no loudness equivalence */
    NEXUS_AudioLoudnessEquivalenceMode_eAtscA85,    /* ATSC A/85.  This standardizes all decoders to output 
                                                       Stereo and Multichannel PCM at -20dB.  Compressed data is output at -31dB. 
                                                       The application must set the volume for outputs accordingly so
                                                       that PCM stereo is output to "active" outputs such as DAC at -23 dB 
                                                       and PCM is sent to "passive" outputs such as SPDIF at -31dB.  This
                                                       can be done by calling NEXUS_AudioOutput_SetSettings() to program
                                                       the volume for active outputs with the linear value 0x5A9DF7 (-3dB) 
                                                       and passive outputs with the value 0x241346 (-11dB). */
    NEXUS_AudioLoudnessEquivalenceMode_eEbuR128,    /* EBU-R128.  This standardizes Dolby decoders to output 
                                                       Stereo and Multichannel PCM at -20dB.  Non-Dolby decoders will
                                                       output Stereo and Multichannel PCM at -23dB.  All encoders and 
                                                       passthrough configurations will output compressed at -31dB. 
                                                       The application must set the volume for outputs accordingly so
                                                       that PCM stereo is output to "active" outputs such as DAC at -23 dB 
                                                       and PCM is sent to "passive" outputs such as SPDIF at -31dB.  This
                                                       can be done by calling NEXUS_AudioOutput_SetSettings() to program 
                                                       the volume for active active outputs with the value 0x5A9DF7 (-3dB) 
                                                       and passive outputs with the value 0x241346 (-11dB) for Dolby codecs.  
                                                       For non-Dolby codecs, use the value 0x800000 (-0dB) for active outputs 
                                                       and 0x32F52C (-8dB) for passive outputs.  */
    NEXUS_AudioLoudnessEquivalenceMode_eMax
} NEXUS_AudioLoudnessEquivalenceMode;

/***************************************************************************
Summary:
Audio Module Settings
**************************************************************************/
typedef struct NEXUS_AudioModuleSettings
{
    struct 
    {
        NEXUS_ModuleHandle transport;
        NEXUS_ModuleHandle hdmiInput;   /* Only required for platforms that support HDMI input */
        NEXUS_ModuleHandle hdmiOutput;  /* Only required for platforms that support HDMI output */
        NEXUS_ModuleHandle rfm;         /* Only required for platforms that support RFM */
        NEXUS_ModuleHandle frontend;    /* Only required for platforms that support RfAudioDecoder */
        NEXUS_ModuleHandle surface; 
    } modules;

    unsigned maxAudioDspTasks;          /* Maximum number of audio tasks that will run on the DSP concurrently */
    unsigned maxIndependentDelay;       /* Maximum output delay value in ms.  */
    unsigned maxPcmSampleRate;          /* Maximum PCM output sample rate in Hz. */

    bool watchdogEnabled;               /* If true, watchdog recovery is enabled */
    bool independentDelay;              /* If true, independent delay is enabled */
    bool routeInputsToDsp;              /* If true, external inputs such as I2S, AnalogAudioDecoder, or RfAudioDecoder will
                                           route to the DSP if the input's Start() routine is called.  Otherwise, the input 
                                           will route to the audio hardware and bypass the DSP.  This defaults to true
                                           for DTV platforms and false for STB platforms.  If false, you can still route inputs
                                           to the DSP by passing the connector to NEXUS_AudioDecoder_Start(). */
    unsigned numPcmBuffers;             /* Number of PCM audio buffers required. */
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

    NEXUS_AudioOutputPll defaultPll;    /* Default PLL for outputs.  Default=0.  */

    unsigned firmwareHeapIndex;         /* Optional.  If set, the DSP Firmware images will be loaded in this heap. */

    NEXUS_AudioDspDebugSettings dspDebugSettings;   /* DSP Debug settings */

    NEXUS_AudioLoudnessEquivalenceMode loudnessMode;    /* Loudness equivalence mode.  Default is 
                                                           NEXUS_AudioLoudnessEquivalenceMode_eNone */
} NEXUS_AudioModuleSettings;

void NEXUS_AudioModule_GetDefaultSettings(
    NEXUS_AudioModuleSettings *pSettings    /* [out] */
    );

/**
Summary:
Initialize the audio module

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.audioModuleSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_AudioModule_Init(
    const NEXUS_AudioModuleSettings *pSettings  /* NULL will use default settings */
    );

void NEXUS_AudioModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INIT_H__ */

