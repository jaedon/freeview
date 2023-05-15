/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_equalizer.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/15/11 3:25p $
*
* API Description:
*   API name: AudioEqualizer
*    Generic APIs for audio equalizer control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/include/nexus_audio_equalizer.h $
* 
* 3   9/15/11 3:25p jgarrett
* SWDTV-8673: Adding shutdown attribute for kernelmode cleanup
* 
* 2   8/24/11 5:31p jgarrett
* SW7344-176: Adding null_allowed attribute for kernelmode
* 
* 1   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/1/11 5:51p jgarrett
* SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
*  directory structure
* 
* 8   11/10/10 7:00p jhaberf
* SW35230-1934: Extended audio equalizer support for more combination of
*  filters on the same output.
* 
* 7   5/3/10 5:23p jgarrett
* SW3548-2910: Adding custom EQ support
* 
* 6   3/26/10 10:35a jgarrett
* SW3548-2859: Adding subwoofer EQ support
* 
* 5   2/8/10 3:46p jgarrett
* SW3548-2739: Enabling secondary equalizer mode
* 
* 4   7/13/09 10:44a jgarrett
* PR 56061: Adding subsonic filter
* 
* 3   11/12/08 11:27a jgarrett
* PR 42028: Adding Parametric EQ mode
* 
* 2   10/6/08 5:15a jgarrett
* PR 42027: Adding two-band eq mode
* 
* 1   9/25/08 5:07p jgarrett
* PR 46055: Revising equalizer API
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_EQUALIZER_H__
#define NEXUS_AUDIO_EQUALIZER_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_output.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioOutput

Header file: nexus_audio_equalizer.h

Module: Audio

Description: Interface for audio output equalizer settings

**************************************/

/***************************************************************************
Summary:
Audio Equalizer Handle
***************************************************************************/
typedef struct NEXUS_AudioEqualizer *NEXUS_AudioEqualizerHandle;

/***************************************************************************
Summary:
Audio Equalizer Mode
***************************************************************************/
typedef enum NEXUS_AudioEqualizerMode
{
    NEXUS_AudioEqualizerMode_eDisabled,   /* Equalizer disabled */
    NEXUS_AudioEqualizerMode_eTone,       /* Enables tone control (bass/treble) */
    NEXUS_AudioEqualizerMode_eFiveBand,   /* Enables 5-band equalizer control */
    NEXUS_AudioEqualizerMode_eParametric, /* Enables 7-band PEQ control */
    NEXUS_AudioEqualizerMode_eCustom,     /* Enables custom equalizer filter coefficients */
    NEXUS_AudioEqualizerMode_eCascadedParametric,/* Cascaded PEQ to the same output */
    NEXUS_AudioEqualizerMode_eMax
} NEXUS_AudioEqualizerMode;

/***************************************************************************
Summary:
Parametric EQ window step - time for phase in/out for filter change
***************************************************************************/
typedef enum NEXUS_AudioEqualizerWindowStep
{
    NEXUS_AudioEqualizerWindowStep_eNone,   /* No window */
    NEXUS_AudioEqualizerWindowStep_e170_6,  /* 170.6ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e85_3,   /* 85.3ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e42_6,   /* 42.6ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e21_3,   /* 21.3ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e10_6,   /* 10.6ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e5_3,    /* 5.3ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_e2_6,    /* 2.6ms at 48kHz */
    NEXUS_AudioEqualizerWindowStep_eMax
} NEXUS_AudioEqualizerWindowStep;

/***************************************************************************
Summary:
Bass Management Settings
***************************************************************************/
typedef struct NEXUS_AudioEqualizerBassManagementSettings
{
    unsigned cutOffFreqLP;      /* Low Pass Cut off Frequency in Hz */     
    unsigned cutOffFreqHP;      /* High Pass Cut off Frequency in Hz */     
    unsigned leftInToLeftOut;   /* Mixing: Left In Contribution to Left Out (0-0x800000) */  
    unsigned rightInToRightOut; /* Mixing: Right In Contribution to Right Out (0-0x800000) */
    unsigned subToLeftOut;      /* Mixing: LFE Contribution to Left Out (0-0x800000) */
    unsigned subToRightOut;     /* Mixing: LFE Contribution to Right Out (0-0x800000) */
} NEXUS_AudioEqualizerBassManagementSettings;


#define NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS (8)
#define NEXUS_MAX_CASCADED_PARAMETRIC_EQ              (3)
#define NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE     (2)

/***************************************************************************
Summary:
Audio Equalizer Settings
***************************************************************************/
typedef struct NEXUS_AudioEqualizerSettings
{
    NEXUS_AudioEqualizerMode mode;              /* Primary equalizer mode */
    NEXUS_AudioEqualizerMode secondaryMode;     /* Secondary equalizer mode if combining two modes.  If not used, set to NEXUS_AudioEqualizerMode_eDisabled. */
    NEXUS_AudioEqualizerMode additionalModes[NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE];  /* Additional equalizer modes that can be combined. */
    struct
    {
        struct
        {
            int bass;       /* Gain Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            int treble;     /* Gain Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            unsigned bassFrequency;            /* Bass center frequency in Hz */
            unsigned bassBandWidthFrequency;   /* Bass bandwidth frequency in Hz */
            unsigned trebleFrequency;          /* Treble center frequency in Hz */
            unsigned trebleBandWidthFrequency; /* Treble bandwidth frequency in Hz */
        } tone;
        struct
        {
            int gain100Hz;  /* Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            int gain300Hz;  /* Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            int gain1000Hz; /* Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            int gain3000Hz; /* Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
            int gain10000Hz;/* Specified in 1/100 dB.  Values range from -10dB (-1000) to +10dB (+1000) */
        } fiveBand;
        struct
        {
            struct
            {
                unsigned peak;  /* Peak frequency in Hz */
                int gain;       /* Specified in 1/100 dB.  Values range from -12dB (-1200) to +12dB (+1200) */
                unsigned q;     /* Peak Frequency Q gain specified in 1/100 (e.g. .33 = 33) */
            } bandSettings[7];

            NEXUS_AudioEqualizerWindowStep  windowStep; /* time for phase in/out for filter change */
        } parametric;
        struct
        {
            /* Please see the description of BRAP_EqualizerCustomSettings in magnum/portinginterface/rap/xxxx/brap.h for
               a description of how to program these values properly.  */
            unsigned numFilters;        /* Number of biquad IIR filters to be programmed.  Ranges from 0..8. */
            int32_t coefficient0[NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS];    
            int32_t coefficient1[NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS];
            int32_t coefficient2[NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS];
            int32_t coefficient3[NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS];
            int32_t coefficient4[NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS];
        } custom;
        struct
        {
            /* This is a cascaded mode of Parametric EQ that can be applied to the same output port */
            bool enabled;
            struct
            {
                unsigned peak;  /* Peak frequency in Hz */
                int gain;       /* Specified in 1/100 dB.  Values range from -12dB (-1200) to +12dB (+1200) */
                unsigned q;     /* Peak Frequency Q gain specified in 1/100 (e.g. .33 = 33) */
            } bandSettings[7];

            NEXUS_AudioEqualizerWindowStep  windowStep; /* time for phase in/out for filter change */
        } cascadedParametric[NEXUS_MAX_CASCADED_PARAMETRIC_EQ];
    } modeSettings;

    bool subsonicFilterEnabled;  /* If true, the subsonic (high-pass) filter is enabled. */
    bool subwooferFilterEnabled; /* If true, the subwoofer (low-pass) filter is enabled. */
    unsigned subsonicFrequency;  /* Subsonic filter frequency (in Hz)  Ranges from 40..315Hz */
    unsigned subwooferFrequency; /* Subwoofer filter frequency (in Hz) Ranges from 40..315Hz */

    bool bassManagementEnabled;  /* True to enable bass management (define cut off frequencies and mixing parameters) */
    NEXUS_AudioEqualizerBassManagementSettings bassManagementSettings;

} NEXUS_AudioEqualizerSettings;

/***************************************************************************
Summary:
Get default settings of an audio equalizer
***************************************************************************/
void NEXUS_AudioEqualizer_GetDefaultSettings(
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Open an equalizer
***************************************************************************/
NEXUS_AudioEqualizerHandle NEXUS_AudioEqualizer_Open(  /* attr{destructor=NEXUS_AudioEqualizer_Close}  */
    unsigned index,
    const NEXUS_AudioEqualizerSettings *pSettings   /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an equalizer
***************************************************************************/
void NEXUS_AudioEqualizer_Close(
    NEXUS_AudioEqualizerHandle equalizer
    );

/***************************************************************************
Summary:
Get settings of an audio equalizer
***************************************************************************/
void NEXUS_AudioEqualizer_GetSettings(
    NEXUS_AudioEqualizerHandle equalizer,
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Current Settings */
    );

/***************************************************************************
Summary:
Set settings of an audio equalizer
***************************************************************************/
NEXUS_Error NEXUS_AudioEqualizer_SetSettings(
    NEXUS_AudioEqualizerHandle equalizer,
    const NEXUS_AudioEqualizerSettings *pSettings
    );

/***************************************************************************
Summary:
Connect an equalizer to an output

Description:
This can only be called when all inputs to the specified output are 
stopped.
***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_SetEqualizer(
    NEXUS_AudioOutput output,
    NEXUS_AudioEqualizerHandle equalizer    /* Pass NULL to remove any equalizer connected to this output */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_EQUALIZER_H__ */

