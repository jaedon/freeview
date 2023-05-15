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
* $brcm_Workfile: nexus_auto_volume_level.h $
* $brcm_Revision: 8 $
* $brcm_Date: 7/30/12 4:40p $
*
* API Description:
*   API name: AutoVolumeLevel
*    Specific APIs related to automatic volume leveling
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_auto_volume_level.h $
* 
* 8   7/30/12 4:40p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
* 4   4/20/12 2:15p jgarrett
* SW7425-2905: Adding AVL 2.0
* 
* 3   8/24/11 5:31p jgarrett
* SW7344-176: Adding null_allowed attribute for kernelmode
* 
* 2   5/13/11 6:38p jgarrett
* SW7425-410: Removing shutdown attribute
* 
* 1   5/13/11 6:32p jgarrett
* SW7425-410: Adding AVL
* 
***************************************************************************/

#ifndef NEXUS_AUTO_VOLUME_LEVEL_H__
#define NEXUS_AUTO_VOLUME_LEVEL_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AutoVolumeLevel

Header file: nexus_audio_volume_level.h

Module: Audio

Description:

**************************************/

/**
Summary:
Handle for auto volume leveling stage
**/
typedef struct NEXUS_AutoVolumeLevel *NEXUS_AutoVolumeLevelHandle;

/***************************************************************************
Summary:
Auto Volume Level Settings
***************************************************************************/
typedef struct NEXUS_AutoVolumeLevelSettings
{
    bool enabled;                   /* If true, processing is enabled.  Otherwise this stage is bypassed */
    bool loudnessEqualization;      /* If true, loudness level equivalence processing is enabled. */

    /* The settings below are typically used at defaults, but may be modified if desired.
       Careful study and analysis is recommended before adjusting these values.  */

    int target;                     /* Target level of output signal in terms of dBFs */
    int lowerBound;                 /* Lower bound for primary range of volume control */
    unsigned fixedBoost;            /* Amount of fixed boost (in dB) if level < LOWERBOUND */
    unsigned ref;                   /* Reference logarithmic gain value of 0 dBFs */
    unsigned alpha;                 /* Attenuation factor for Level Maximum */
    unsigned beta;					/* Attenuation factor for Level Mean */
    unsigned threshold;				/* Threshold for detecting active portion of signal . Input value should be multiplied by factor of 10 */
    unsigned dtfPercent;            /* Decay Time for DTF Percent delay (in seconds) */
    unsigned alpha2;                /* Attenuation factor for Level Minimum */
    unsigned fastGainRamp;          /* Fast Gain Ramp in milliseconds. Input value should be multiplied by factor of 10 */
    unsigned dtf;                   /* Decay Time Fraction. Input value should be multiplied by factor of 10 */
} NEXUS_AutoVolumeLevelSettings;

/***************************************************************************
Summary:
Get default settings for an auto volume leveling stage
***************************************************************************/
void NEXUS_AutoVolumeLevel_GetDefaultSettings(
    NEXUS_AutoVolumeLevelSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an auto volume leveling stage
***************************************************************************/
NEXUS_AutoVolumeLevelHandle NEXUS_AutoVolumeLevel_Open( /* attr{destructor=NEXUS_AutoVolumeLevel_Close}  */
    const NEXUS_AutoVolumeLevelSettings *pSettings     /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an auto volume leveling stage

Description:
Input to the stage must be removed prior to closing.
***************************************************************************/
void NEXUS_AutoVolumeLevel_Close(
    NEXUS_AutoVolumeLevelHandle handle
    );

/***************************************************************************
Summary:
Get Settings for an auto volume leveling stage
***************************************************************************/
void NEXUS_AutoVolumeLevel_GetSettings(
    NEXUS_AutoVolumeLevelHandle handle,
    NEXUS_AutoVolumeLevelSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
Set Settings for an auto volume leveling stage
***************************************************************************/
NEXUS_Error NEXUS_AutoVolumeLevel_SetSettings(
    NEXUS_AutoVolumeLevelHandle handle,
    const NEXUS_AutoVolumeLevelSettings *pSettings
    );

/***************************************************************************
Summary:
Get the audio connector for an auto volume leveling stage
***************************************************************************/
NEXUS_AudioInput NEXUS_AutoVolumeLevel_GetConnector( 
    NEXUS_AutoVolumeLevelHandle handle
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AutoVolumeLevel_AddInput(
    NEXUS_AutoVolumeLevelHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AutoVolumeLevel_RemoveInput(
    NEXUS_AutoVolumeLevelHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AutoVolumeLevel_RemoveAllInputs(
    NEXUS_AutoVolumeLevelHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUTO_VOLUME_LEVEL_H__ */

