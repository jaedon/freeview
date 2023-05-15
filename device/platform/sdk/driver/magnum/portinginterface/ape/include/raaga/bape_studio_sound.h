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
* $brcm_Workfile: bape_studio_sound.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 8/8/11 5:38p $
*
* API Description:
*   API name: StudioSound
*    Specific APIs related to SRS StudioSound Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_studio_sound.h $
* 
* Hydra_Software_Devel/2   8/8/11 5:38p jgarrett
* SWDTV-6761: Adding StudioSound
* 
* Hydra_Software_Devel/1   7/15/11 5:51p jgarrett
* SWDTV-6761: Adding StudioSound API
* 
***************************************************************************/

#ifndef BAPE_STUDIO_SOUND_H__
#define BAPE_STUDIO_SOUND_H__

#include "bape.h"
#include "bape_tru_surround.h"
#include "bape_tru_volume.h"

/***************************************************************************
Summary: 
StudioSound Handle 
***************************************************************************/
typedef struct BAPE_StudioSound *BAPE_StudioSoundHandle;

/***************************************************************************
Summary:
StudioSound Mode - Select the opearating mode of StudioSound. Default is Mode1.
                   Based on mode, a few of the modules will be disabled by the driver internally. 
    Mode1: TruVolume(on), HPF(on), CircleSurround(on), TruSurroundHD(on), GEQ(off), HardLimiter(on), TruEQ(on)
    Mode2: TruVolume(off), HPF(on), CircleSurround(off), TruSurroundHD(off), GEQ(on), HardLimiter(on), TruEq(on) 

Note: When TruVolume/CircleSurround/TruSurroundHD is off, a bypass gain of 1.0 will be used.
***************************************************************************/
typedef enum BAPE_StudioSoundMode
{
    BAPE_StudioSoundMode_eMode1,
    BAPE_StudioSoundMode_eMode2,
    BAPE_StudioSoundMode_eMax
} BAPE_StudioSoundMode;

/***************************************************************************
Summary:
SRS StudioSound Input Mode
***************************************************************************/
typedef enum BAPE_StudioSoundInputMode
{
    BAPE_StudioSoundInputMode_eMono,
    BAPE_StudioSoundInputMode_eStereo,
    BAPE_StudioSoundInputMode_eMultichannel,
    BAPE_StudioSoundInputMode_eLtRt,
    BAPE_StudioSoundInputMode_eMax
} BAPE_StudioSoundInputMode;

/***************************************************************************
Summary:
SRS CircleSurround Processing Mode
***************************************************************************/
typedef enum BAPE_CircleSurroundMode
{
    BAPE_CircleSurroundMode_eCinema,
    BAPE_CircleSurroundMode_eMusic,
    BAPE_CircleSurroundMode_eMax
}BAPE_CircleSurroundMode;

/***************************************************************************
Summary:
SRS CircleSurround Settings
***************************************************************************/
typedef struct BAPE_CircleSurroundSettings
{
    bool enabled;                       /* If true, CircleSurround processing is enabled. Default: true */

    unsigned inputGain;                 /* Adjusts the signal level immediately prior to CircleSurround processing.  Range: 125 to 1000. Default: 1000 */
    BAPE_CircleSurroundMode mode;       /* The CircleSurround processing mode. Default:  BAPE_CircleSurroundMode_eCinema */
    unsigned outputMode;                /* 0: stereo ; 1: multichannel. Default: 0 (only 0 is supported) */
    unsigned outputGainFront;           /* Output gain for the front (L/R). Range: 89 to 4000. Default: 1000 */
    unsigned outputGainRear;            /* Output gain for the rear (Ls/Rs). Range: 89 to 4000. Default: 1000 */
    unsigned outputGainCenter;          /* Output gain for the center. Range: 89 to 4000. Default: 1000 */
    unsigned outputGainSubwoofer;       /* Output gain for the subwoofer. Range: 89 to 4000. Default: 1000 */
} BAPE_CircleSurroundSettings;

/***************************************************************************
Summary:
SRS TruDialog Settings
***************************************************************************/
typedef struct BAPE_TruDialogSettings
{
    bool enabled;               /* If true, TruDialog processing is enabled. Default: true */

    unsigned inputGain;         /* Adjusts the signal level prior immediately prior to TruDialog processing.  Range: 0 to 100. Default: 100 */
    unsigned outputGain;        /* Adjusts the signal level prior after TruDialog processing.  Range: 0 to 100. Default: 100 */
    unsigned processGain;       /* Controls how much of the calculated final output gain is applied to the output signal, effectively
                                   controlling how much the process will raise the dialog signal.  Range: 50 to 300. Default: 150 */
    unsigned bypassGain;        /* Adjusts the signal level when TruDialog processing is disabled.  Range: 0 to 100. Default: 100 */
    unsigned enhancementGain;  /*  The amount of vocal enhancement that is applied to the audio signal.  Range: 0 to 100. Default: 100 */
} BAPE_TruDialogSettings;

/***************************************************************************
Summary:
SRS Graphic EQ Band Modes
***************************************************************************/
typedef enum BAPE_SrsGraphicEqBandMode
{
    BAPE_SrsGraphicEqBandMode_eFiveBand,
    BAPE_SrsGraphicEqBandMode_eTenBand,
    BAPE_SrsGraphicEqBandMode_eSpeakerCompensation,
    BAPE_SrsGraphicEqBandMode_eMax
} BAPE_SrsGraphicEqBandMode;

#define BAPE_STUDIO_SOUND_MAX_GEQ_BANDS    10
/***************************************************************************
Summary:
SRS Graphic Equalizer Settings
***************************************************************************/
typedef struct BAPE_SrsGraphicEqSettings
{
    bool leftEnabled;           /* If true, left channel processing is enabled. Default: true */
    bool rightEnabled;          /* If true, right channel processing is enabled. Default: true */
    BAPE_SrsGraphicEqBandMode bandMode;  /* Graphic EQ band mode: 5-band or 10-band. Default BAPE_SrsGraphicEqBandMode_eFiveBand */
    unsigned leftBandGain[BAPE_STUDIO_SOUND_MAX_GEQ_BANDS];   /* Left channel band gain.  Range for 5-band: 316 to 3160. Range for 10-band: 251 to 4000. Default: 1000 */
    unsigned rightBandGain[BAPE_STUDIO_SOUND_MAX_GEQ_BANDS];  /* Right channel band gain.  Range for 5-band: 316 to 3160. Range for 10-band: 251 to 4000. Default: 1000 */
    unsigned inputGain;         /* Adjusts the signal level prior immediately prior to Graphic Equalizer processing.  Range: 316 to 3160 Default: 316 */
    unsigned outputGain;        /* Adjusts the signal level prior after Graphic Equalizer processing.  Range: 0 to 100. Default: 100 */
    unsigned bypassGain;        /* Adjusts the signal level when Graphic Equalizer processing is disabled.  Range: 0 to 100. Default: 100 */
} BAPE_SrsGraphicEqSettings;

/***************************************************************************
Summary:
SRS TruEQ Filter parameters for coefficients specified by user
***************************************************************************/
typedef struct BAPE_TruEqFilterCoefficients
{
    BAPE_SrsFilterOrder filterOrder;    /* EQ filter order.  Default: BAPE_SrsFilterOrder_e2 */
    unsigned scale;                     /* Specified q-format of the coefficients. (1+scale).(31-scale) format 
                                           Default: 1. scale=1 implies a q-format of 2.30 */
    int coefficientB0;
    int coefficientA1;
    int coefficientB1;
    int coefficientA2;
    int coefficientB2;
}BAPE_TruEqFilterCoefficients;

/***************************************************************************
Summary:
SRS TruEQ Filter parameters for coefficients generated by specification
***************************************************************************/
typedef struct BAPE_TruEqFilterCoefficientSpecification
{
    int bandGain;    /*  Range: (-120 to 120). Default:0 Format: q24
                         Range in dB: -12 to 12 db. Default: 0dB
                         Range in Fixed point: 0x00404DE6 to 0x03FB2783 */

    unsigned bandFrequency;     /*  Range: (20 - 20000)Hz. Default: 1000 Hz */
    int qFactor;                /*  Range: (25 - 1600).  Default:200 Format: q24
                                    Range in floating point: 0.25 to 16.0
                                    Range in fixed point: 0x00400000 to 0x10000000 */
}BAPE_TruEqFilterCoefficientSpecification;

#define BAPE_STUDIO_SOUND_MAX_PEQ_BANDS    8
/***************************************************************************
Summary:
SRS TruEQ Settings
***************************************************************************/
typedef struct BAPE_TruEqSettings
{
    bool leftEnabled;           /* If true, left channel processing is enabled. Default: true */
    bool rightEnabled;          /* If true, right channel processing is enabled. Default: true */

    bool leftBandEnabled[BAPE_STUDIO_SOUND_MAX_PEQ_BANDS];   /* If true, left channel processing is enabled for this band. Default: true */
    bool rightBandEnabled[BAPE_STUDIO_SOUND_MAX_PEQ_BANDS];  /* If true, right channel processing is enabled for this band. Default: true */

    unsigned inputGain;         /* Adjusts the signal level prior immediately prior to TruEQ processing.  Range: 0 to 100. Default: 100 */
    unsigned outputGain;        /* Adjusts the signal level prior after TruEQ processing.  Range: 0 to 100. Default: 100 */
    unsigned bypassGain;        /* Adjusts the signal level when TruEQ processing is disabled.  Range: 0 to 100. Default: 100 */

    BAPE_SrsFilterCoefficientMode coefficientMode;  /* Coefficients generation mode. Default: BAPE_SrsFilterCoefGenMode_eFilterUser */
    struct
    {
        BAPE_TruEqFilterCoefficients user[3][BAPE_STUDIO_SOUND_MAX_PEQ_BANDS];
                                                                  /* The first array dimension indicates 3 sampling frequencies 32, 44.1 and 48kHz respectively
                                                                     will be used if coefficientMode is BAPE_SrsFilterCoefficientMode_eUser */
        BAPE_TruEqFilterCoefficientSpecification specification[BAPE_STUDIO_SOUND_MAX_PEQ_BANDS];
                                                                  /* Filter specification for generating filter coeffcients,
                                                                     will be used if eCoefGenMode is BAPE_SrsFilterCoefficientMode_eSpecification */
    } coefficientSettings;
} BAPE_TruEqSettings;

/***************************************************************************
Summary:
SRS Hard Limiter Block Size
***************************************************************************/
typedef enum BAPE_SrsHardLimiterBlockSize
{
    BAPE_SrsHardLimiterBlockSize_e256,
    BAPE_SrsHardLimiterBlockSize_e512,
    BAPE_SrsHardLimiterBlockSize_e768,
    BAPE_SrsHardLimiterBlockSize_e1024,
    BAPE_SrsHardLimiterBlockSize_eMax
} BAPE_SrsHardLimiterBlockSize;

/***************************************************************************
Summary:
SRS Hard Limiter Settings
***************************************************************************/
typedef struct BAPE_SrsHardLimiterSettings
{
    bool enabled;                               /* If true, Hard limiter processing is enabled. Default: true */
    BAPE_SrsHardLimiterBlockSize blockSize;     /* Processing block size. Default:  BAPE_SrsHardLimiterBlockSize_e256 */
    unsigned inputGain;                         /* Adjusts the signal level prior immediately prior to hard limiter processing.  Range: 0 to 400. Default: 100 */
    unsigned outputGain;                        /* Adjusts the signal level prior after hard limiter processing.  Range: 0 to 400. Default: 100 */
    unsigned bypassGain;                        /* Adjusts the signal level when hard limiter processing is disabled.  Range: 0 to 100. Default: 100 */    
    unsigned boost;                             /* Modifies the signal level within the context of the hard limiter, so that the signal
                                                   will not exceed the hard limit level parameter.  Range: 1 to 256000. Default: 2820 */
    unsigned level;                             /* Specifies the level over which no signal shall bel allowed to exceed.  Range: 3 to 100000. Default: 100000 */
    unsigned delay;                             /* Specifies the length of the look-ahead delay line utilized in the hard limiter.  Range: 6 to 22.  Default: 22 */
} BAPE_SrsHardLimiterSettings;

/***************************************************************************
Summary:
SRS StudioSound Settings
***************************************************************************/
typedef struct BAPE_StudioSoundSettings
{
    bool enabled;               /* If true, processing is enabled.  Otherwise this stage is bypassed. Default: true */
    BAPE_StudioSoundMode mode;  /* Pre-defined operation mode of StudioSound */
    unsigned inputGain;         /* Adjusts the signal level prior to StudioSound processing.  Ranges from 0 to 100 in %. */
    unsigned outputGain;        /* Adjusts the signal level after StudioSound processing.  Ranges from 0 to 100 in %. */
    unsigned bypassGain;        /* Adjusts the signal level when StudioStound is disabled.  Ranges from 0 to 100 in %. */
    unsigned headroomGain;      /* Provides headroom for intermediate calculations to avoid clipping.  Ranges from 0 to 100 in %. */
    BAPE_StudioSoundInputMode inputMode; /* Channel configuration present in input. Default:  BAPE_StudioSoundInputMode_eLtRt */

    BAPE_CircleSurroundSettings circleSurround;    /* CircleSurround settings */
    BAPE_TruDialogSettings truDialog;              /* TruDialog settings */
    BAPE_TruSurroundHdSettings truSurroundHd;      /* TruSurroundHd settings */
    BAPE_TruVolumeSettings truVolume;              /* TruVolume settings */
    BAPE_SrsGraphicEqSettings graphicEq;           /* Graphic Equalizer Settings */
    BAPE_TruEqSettings truEq;                      /* TruEq Settings */
    BAPE_SrsHighPassFilterSettings highPassFilter; /* High Pass Filter Settings */
    BAPE_SrsHardLimiterSettings hardLimiter;       /* Hard Limiter Settings */
} BAPE_StudioSoundSettings;

/***************************************************************************
Summary:
    Get default settings for an SRS StudioSound stage
***************************************************************************/
void BAPE_StudioSound_GetDefaultSettings(
    BAPE_StudioSoundSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
    Open an SRS StudioSound stage
***************************************************************************/
BERR_Code BAPE_StudioSound_Create(
    BAPE_Handle deviceHandle,
    const BAPE_StudioSoundSettings *pSettings,
    BAPE_StudioSoundHandle *pHandle
    );

/***************************************************************************
Summary:
    Close an SRS StudioSound stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_StudioSound_Destroy(
    BAPE_StudioSoundHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for an SRS StudioSound stage
***************************************************************************/
void BAPE_StudioSound_GetSettings(
    BAPE_StudioSoundHandle handle,
    BAPE_StudioSoundSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Settings for an SRS StudioSound stage
***************************************************************************/
BERR_Code BAPE_StudioSound_SetSettings(
    BAPE_StudioSoundHandle handle,
    const BAPE_StudioSoundSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the audio connector for an SRS StudioSound stage
***************************************************************************/
void BAPE_StudioSound_GetConnector(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector *pConnector
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_StudioSound_AddInput(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_StudioSound_RemoveInput(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_StudioSound_RemoveAllInputs(
    BAPE_StudioSoundHandle handle
    );

#endif /* #ifndef BAPE_STUDIO_SOUND_H__ */


