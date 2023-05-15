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
 * $brcm_Workfile: nexus_audio.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 5/23/12 6:13p $
 *
 * Module Description:
 *      Audio Module Global Routines
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio.h $
 * 
 * 5   5/23/12 6:13p jgarrett
 * SW7425-3070: Adding NEXUS_AudioOutputPll Settings to control VCXO
 *  source
 * 
 * 4   5/1/12 6:22p jgarrett
 * SW7425-2690: Adding Audio Capabilities routine
 * 
 * 3   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 *
 * 2   2/10/10 5:55p jgarrett
 * SW3556-1033: Adding function to enable external mclk
 *
 * 1   6/16/09 12:13p jgarrett
 * PR 55459: Adding audio module functions for ramp step
 *
 **************************************************************************/
#ifndef NEXUS_AUDIO_H__
#define NEXUS_AUDIO_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary: 
Audio hardware ramp step values. 
 
Description: 
When a hardware block is ramping from one volume value to another, it 
will change the linear volume value by the ramp step size on each sample. 
***************************************************************************/
typedef struct NEXUS_AudioRampStepSettings
{
    unsigned int mixerRampStep;
    unsigned int srcRampStep;
} NEXUS_AudioRampStepSettings;

/***************************************************************************
Summary: 
Get the audio hardware ramp step values 
***************************************************************************/
void NEXUS_AudioModule_GetRampStepSettings(
    NEXUS_AudioRampStepSettings *pRampStepSettings      /* [out] ramping step size for scale change for all output ports */
    );

/***************************************************************************
Summary: 
Set the audio hardware ramp step values 
***************************************************************************/
NEXUS_Error NEXUS_AudioModule_SetRampStepSettings(
    const NEXUS_AudioRampStepSettings *pRampStepSettings  /* ramping step size for scale change for all output ports */
    );

/***************************************************************************
Summary:
External MCLK rates.  Specified as a multiple of the sampling frequency 
the PLL is programmed for. 
***************************************************************************/
typedef enum NEXUS_ExternalMclkRate
{
    NEXUS_ExternalMclkRate_e128Fs,
    NEXUS_ExternalMclkRate_e256Fs,
    NEXUS_ExternalMclkRate_e384Fs,
    NEXUS_ExternalMclkRate_e512Fs,
    NEXUS_ExternalMclkRate_eMax
} NEXUS_ExternalMclkRate;

/***************************************************************************
Summary:
Enable an external MCLK at the rate specified.  
***************************************************************************/
NEXUS_Error NEXUS_AudioModule_EnableExternalMclk(
    unsigned mclkIndex,                 /* Which external MCLK to enable.  For chips with 1 external 
                                           MCLK line, this should be specified as 0.  For chips with
                                           more than one MCLK signal/pin, this ranges from 0 to the
                                           number of MCLK signals/pins - 1 */
    NEXUS_AudioOutputPll pll,           /* The PLL that will drive this MCLK */
    NEXUS_ExternalMclkRate mclkRate     /* The multiplier of the sample rate to be transmitted.  For
                                           example, if 128Fs is specified, the output clock rate will be 
                                           128 * sample_rate_in_hz */
    );

/***************************************************************************
Summary:
Audio PLL Modes
***************************************************************************/
typedef enum NEXUS_AudioOutputPllMode    
{
    NEXUS_AudioOutputPllMode_eVcxo,           /* The input source is a VCXO Rate Manager */
    NEXUS_AudioOutputPllMode_eFreeRun,        /* The input source is a free running clock.  */
    NEXUS_AudioOutputPllMode_eMax
} NEXUS_AudioOutputPllMode;
    
/***************************************************************************
Summary:
Audio PLL Settings
***************************************************************************/
typedef struct NEXUS_AudioOutputPllSettings
{
    NEXUS_AudioOutputPllMode mode;
    union
    {
        struct
        {
            NEXUS_Vcxo vcxo;    /* Specifies the VCXO-PLL driving this Audio PLL. */
        } vcxo;
    } modeSettings;
} NEXUS_AudioOutputPllSettings;   
    
/***************************************************************************
Summary:
Get Audio PLL Settings
***************************************************************************/
void NEXUS_AudioOutputPll_GetSettings(
    NEXUS_AudioOutputPll pll,
    NEXUS_AudioOutputPllSettings *pSettings       /* [out] Current Settings */
    );
    
/***************************************************************************
Summary:
Set Audio PLL Settings
***************************************************************************/
NEXUS_Error NEXUS_AudioOutputPll_SetSettings(
    NEXUS_AudioOutputPll pll,
    const NEXUS_AudioOutputPllSettings *pSettings
    );
    
/***************************************************************************
Summary:
Audio HW/FW Capabilities
***************************************************************************/
typedef struct NEXUS_AudioCapabilities
{
    struct
    {
        unsigned hdmi;                  /* Number of HDMI audio inputs */
        unsigned i2s;                   /* Number of I2S inputs */
        unsigned spdif;                 /* Number of SPDIF inputs */
    } numInputs;

    struct
    {
        unsigned audioReturnChannel;    /* Number of ARC outputs */
        unsigned capture;               /* Maximum number of OutputCapture objects */
        unsigned dac;                   /* Number of DAC outputs */
        unsigned dummy;                 /* Maximum number of DummyOutput objects */
        unsigned hdmi;                  /* Number of HDMI audio outputs */
        unsigned i2s;                   /* Number of I2S outputs */
        unsigned loopback;              /* Maximum number of Loopback objects */
        unsigned rfmod;                 /* Number of RF Mod outputs */
        unsigned spdif;                 /* Number of SPDIF outputs */
    } numOutputs;

    unsigned numDecoders;               /* Maximum number of Decoders */
    unsigned numPlaybacks;              /* Maximum number of Playbacks */
    unsigned numInputCaptures;          /* Maximum number of InputCaptures */
    unsigned numVcxos;                  /* Number of VCXO PLLs that feed audio PLLs */
    unsigned numPlls;                   /* Number of output PLL clocks */
    unsigned numNcos;                   /* Number of output NCO clocks */
   
    unsigned numDsps;                   /* Number of audio DSPs */
    struct
    {
        struct
        {
            bool decode;                /* True if this codec can be decoded */
            bool encode;                /* True if this codec can be encoded */
        } codecs[NEXUS_AudioCodec_eMax];

        bool audysseyAbx;               /* True if Audyssey ABX is supported */
        bool audysseyAdv;               /* True if Audyssey ADV is supported */
        bool autoVolumeLevel;           /* True if AutoVolumeLevel is supported */
        bool _3dSurround;               /* True if 3dSurround is supported */
        bool decodeRateControl;         /* True if DSOLA audio rate control is supported */
        bool dolbyDigitalReencode;      /* True if Dolby MS11 is supported */
        bool dolbyVolume;               /* True if DolbyVolume processing is supported */
        bool dolbyVolume258;            /* True if DolbyVolume258 processing is supported */

        struct
        {
            bool supported;             /* True if echo cancellation is supported */
            bool algorithms[NEXUS_EchoCancellerAlgorithm_eMax];  /* Each supported algorithm will be set to true */
        } echoCanceller;

        bool encoder;                   /* True if encoder is supported */
        bool mixer;                     /* True if DSP mixers are supported */
        bool muxOutput;                 /* True if MuxOutput is supported for transcoding */

        struct
        {
            bool supported;             /* True if RF encoder is supported */
            bool encodings[NEXUS_RfAudioEncoding_eMax];  /* Each supported encoding will be set to true */
        } rfEncoder;

        bool studioSound;               /* True if SRS StudioSound processing is supported */
        bool truSurroundHd;             /* True if SRS TruSurround HD processing is supported */
        bool truVolume;                 /* True if SRS TruVolume processing is supported */
    } dsp;

    struct
    {
        bool supported;                 /* True if Equalizer support is available */
    } equalizer;

} NEXUS_AudioCapabilities;

/***************************************************************************
Summary:
Get Audio HW/FW Capabilities
***************************************************************************/
void NEXUS_GetAudioCapabilities(
    NEXUS_AudioCapabilities *pCaps        /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_H__ */

