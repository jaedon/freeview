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
* $brcm_Workfile: nexus_audio_dac.h $
* $brcm_Revision: 13 $
* $brcm_Date: 4/22/11 10:43a $
*
* API Description:
*   API name: AudioDac
*    Specific APIs related to audio DAC outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_dac.h $
* 
* 13   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 12   3/25/11 11:55a jgarrett
* SW7422-375: Refactoring peak gain api
* 
* 11   3/23/11 7:12p jgarrett
* SW7422-375: Adding peakingFilter settings
* 
* 10   12/17/10 3:55p jgarrett
* SW7422-146: Adding new dac mute settings for 40nm
* 
* 9   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 8   3/18/10 11:16a jgarrett
* SW3556-940: Changing DAC volume to be cross-platform and dB instead of
*  linear.
* 
* 7   1/11/10 6:15p jgarrett
* SW3556-940: Adding DAC scale and volume for DTV systems
* 
* 6   6/22/09 11:45a jgarrett
* PR 51633: Adding test tone options and new mute options
* 
* PR51633/1   5/14/09 3:08p jgarrett
* PR 51633: Adding test tone options and new mute options
* 
* 5   2/7/08 12:11p vsilyaev
* PR 38692: Added attributes for shutdown functions
* 
* 4   2/6/08 4:28p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
* 
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
* 
* 2   1/23/08 2:39p erickson
* PR35457: update docs
* 
* 1   1/18/08 2:15p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/9   1/8/08 7:00p jgarrett
* PR 38535: Moving channel order to audio output
* 
* Nexus_Devel/8   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
* Nexus_Devel/7   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/6   10/4/07 2:25p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/5   9/27/07 7:51p jgarrett
* PR 34954: Successful build
*
* Nexus_Devel/4   9/17/07 1:32p jgarrett
* PR 34954: Implementing naming convention changes
*
* Nexus_Devel/3   9/10/07 11:30a jgarrett
* PR 34254: Adding multiple inclusion protection
*
* Nexus_Devel/2   9/6/07 5:12p jgarrett
* PR 34254: Fixing naming conventions
*
* Nexus_Devel/1   8/24/07 11:57a jgarrett
* PR 34254: Implementing Source -> Input name changes
*
***************************************************************************/
#ifndef NEXUS_AUDIO_DAC_H__
#define NEXUS_AUDIO_DAC_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioDac

Header file: nexus_audio_dac.h

Module: Audio

Description: Route PCM audio data to a set of L/R DAC outputs

**************************************/

/**
Summary:
Handle for audio DAC
**/
typedef struct NEXUS_AudioDac *NEXUS_AudioDacHandle;

/***************************************************************************
Summary:
Audio DAC Mute Modes
***************************************************************************/
typedef enum NEXUS_AudioDacMuteType
{
    /* These options are supported only on the 7400/7401 chipsets. */
    NEXUS_AudioDacMuteType_eConstantLow,
    NEXUS_AudioDacMuteType_eConstantHigh,
    NEXUS_AudioDacMuteType_eSquareWaveOpp,
    NEXUS_AudioDacMuteType_eSquareWaveSame,    
    NEXUS_AudioDacMuteType_eAaaa,               /* Supported on 7405, 3548, and 7420 classes of 65nm chips */
    NEXUS_AudioDacMuteType_e5555,               /* Supported on 7405, 3548, and 7420 classes of 65nm chips */
    NEXUS_AudioDacMuteType_eCustomValue,        /* Supported on 7405, 3548, and 7420 classes of 65nm chips as well as 40nm chips (7422, 35230, etc.) */
    NEXUS_AudioDacMuteType_eDrive0,             /* Supported on 40nm chips (7422, 35230, etc.) */
    NEXUS_AudioDacMuteType_eDriveNegative1,     /* Supported on 40nm chips (7422, 35230, etc.) */    
    NEXUS_AudioDacMuteType_eMax
} NEXUS_AudioDacMuteType;

/***************************************************************************
Summary:
Audio DAC Settings
***************************************************************************/
typedef struct NEXUS_AudioDacSettings
{
    NEXUS_AudioDacMuteType muteType;
    uint16_t muteValueLeft;     /* 16-bit sample, used only if mute type == NEXUS_AudioDacMuteType_eCustomValue */
    uint16_t muteValueRight;    /* 16-bit sample, used only if mute type == NEXUS_AudioDacMuteType_eCustomValue */

    struct
    {
        bool enabled;           /* If true, test tone output is enabled with the settings below.  If false, the tone output is disabled. */
        int32_t samples[64];    /* Test tone samples.  The samples are 20-bit signed data. */
        bool zeroOnLeft;        /* If true, the left channel will output zeroes */
        bool zeroOnRight;       /* If true, the right channel will output zeroes */
        bool sharedSamples;     /* If true (default), samples 0..63 will be output on both Left and Right.
                                   If false, samples 0..31 will output on Left and 32..63 on Right. */
        unsigned numSamplesLeft;    /* Number of samples to play on the left channel before repeating (1..64) */
        unsigned numSamplesRight;   /* Number of samples to play on the right channel before repeating (1..64) */
        unsigned sampleRate;        /* Sampling frequency of the samples */
    } testTone;

	uint32_t peakGain;      	/* PEAK_GAIN - Peaking filter gain */
    uint32_t scale;             /* Affects the setting of the HIFIDAC_CTRLn_SCALE register directly.  Applicable to DTV systems only. */
    int32_t volume;             /* Attenuation in 1/100 dB.  Ranges from NEXUS_AUDIO_VOLUME_DB_NORMAL to NEXUS_AUDIO_VOLUME_DB_MIN.  Default=0. */
} NEXUS_AudioDacSettings;

/***************************************************************************
Summary:
Get default settings for an audio DAC
***************************************************************************/
void NEXUS_AudioDac_GetDefaultSettings(
    NEXUS_AudioDacSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an audio DAC
***************************************************************************/
NEXUS_AudioDacHandle NEXUS_AudioDac_Open( /* attr{destructor=NEXUS_AudioDac_Close}  */
    unsigned index,
    const NEXUS_AudioDacSettings *pSettings     /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an audio DAC
    
Description:
Input to the DAC must be removed prior to closing.
***************************************************************************/
void NEXUS_AudioDac_Close(
    NEXUS_AudioDacHandle handle
    );

/***************************************************************************
Summary:
Get Settings for an audio DAC
***************************************************************************/
void NEXUS_AudioDac_GetSettings(
    NEXUS_AudioDacHandle handle,
    NEXUS_AudioDacSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
Set Settings for an audio DAC
***************************************************************************/
NEXUS_Error NEXUS_AudioDac_SetSettings(
    NEXUS_AudioDacHandle handle,
    const NEXUS_AudioDacSettings *pSettings    /* [in] Settings */
    );

/***************************************************************************
Summary:
Get the audio connector for an audio DAC
***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioDac_GetConnector( 
    NEXUS_AudioDacHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_DAC_H__ */

