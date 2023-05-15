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
* $brcm_Workfile: nexus_tru_volume.h $
* $brcm_Revision: 12 $
* $brcm_Date: 7/31/12 3:17p $
*
* API Description:
*   API name: TruVolume
*    Specific APIs related to SRS TruVolume (formerly Volume IQ) Audio Processing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_tru_volume.h $
* 
* 12   7/31/12 3:17p jgarrett
* SW7425-3504: Standardizing to 1/100 units for gain parameters
* 
* 11   2/2/12 10:15a jgarrett
* SW7420-2216: Merge to main branch
* 
* SW7420-2216/1   1/26/12 4:31p jgarrett
* SW7420-2216: Reverting to TruVolume 1.2.3.2
* 
* 10   4/22/11 10:44a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 9   3/16/11 3:04p jhaberf
* SWDTV-5784: Updated SRS StudioSound interface with new HPF/PEQ
*  parameters
* 
* 8   2/7/11 3:33p jhaberf
* SW35230-2902: Provided SRS High Pass Filter settings in TruVolume
* 
* 7   1/7/11 4:42p jhaberf
* SW35230-2530: Modified the TruVol mode values requested by SRS lab
* 
* 6   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 5   12/8/10 4:07p jhaberf
* SW35230-2530: Update Nexus TruVolume parameters to support
*  certification test
* 
* 4   10/4/10 5:31p jgarrett
* SW7420-1133: Adding certification parameters for tru volume
* 
* 3   11/9/09 2:46p jgarrett
* SW3548-2566: Adding DC notch filter setting
* 
* 2   8/31/09 11:52a jgarrett
* SW3548-2440: Updating comments regarding noise manager threshold
* 
* 1   2/12/09 5:36p jgarrett
* PR 46135: Adding TruVolume
* 
***************************************************************************/

#ifndef NEXUS_TRU_VOLUME_H__
#define NEXUS_TRU_VOLUME_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: TruVolume

Header file: nexus_tru_volume.h

Module: Audio

Description: SRS TruVolume stage

**************************************/

/**
Summary:
Handle for SRS Volume IQ stage
**/
typedef struct NEXUS_TruVolume *NEXUS_TruVolumeHandle;

/***************************************************************************
Summary:
TruVolume Block Size
***************************************************************************/
typedef enum NEXUS_TruVolumeBlockSize
{
    NEXUS_TruVolumeBlockSize_e256,
    NEXUS_TruVolumeBlockSize_e512,
    NEXUS_TruVolumeBlockSize_e768,
    NEXUS_TruVolumeBlockSize_e1024,
    NEXUS_TruVolumeBlockSize_eMax
} NEXUS_TruVolumeBlockSize;

/***************************************************************************
Summary:
TruVolume Mode
***************************************************************************/
typedef enum NEXUS_TruVolumeMode
{
    NEXUS_TruVolumeMode_eLight,
    NEXUS_TruVolumeMode_eNormal,
    NEXUS_TruVolumeMode_eHeavy,
    NEXUS_TruVolumeMode_eMax
} NEXUS_TruVolumeMode;

/***************************************************************************
Summary:
TruVolume Speaker Resolution
***************************************************************************/
typedef enum NEXUS_TruVolumeSpeakerResolution
{
    NEXUS_TruVolumeSpeakerResolution_e20Hz,
    NEXUS_TruVolumeSpeakerResolution_e40Hz,
    NEXUS_TruVolumeSpeakerResolution_e110Hz,
    NEXUS_TruVolumeSpeakerResolution_e200Hz,
    NEXUS_TruVolumeSpeakerResolution_e315Hz,
    NEXUS_TruVolumeSpeakerResolution_e410Hz,
    NEXUS_TruVolumeSpeakerResolution_eMax
} NEXUS_TruVolumeSpeakerResolution;

/***************************************************************************
Summary:
SRS Filter Coefficient Generation Modes. Also required by StudioSound.
***************************************************************************/
typedef enum NEXUS_SrsFilterCoefGenMode
{
    NEXUS_SrsFilterCoefGenMode_eFilterUser,  /* Use user specified coefficients */
    NEXUS_SrsFilterCoefGenMode_eFilterSpec,  /* Use Filter Specification to generate filter coeffcients */
    NEXUS_SrsFilterCoefGenMode_eMax
} NEXUS_SrsFilterCoefGenMode;

/***************************************************************************
Summary:
SRS High Pass Filter Order. Also required by StudioSound.
***************************************************************************/
typedef enum NEXUS_SrsFilterOrder
{
    NEXUS_SrsFilterOrder_eOrder0 = 0,
    NEXUS_SrsFilterOrder_eOrder2 = 2,
    NEXUS_SrsFilterOrder_eOrder4 = 4,  
    NEXUS_SrsFilterOrder_eOrder6 = 6,
    NEXUS_SrsFilterOrder_eMax
} NEXUS_SrsFilterOrder;

/***************************************************************************
Summary:
SRS High Pass Filter Coefficients. Also required by StudioSound.
***************************************************************************/
typedef struct NEXUS_SrsHighPassFilterCoef
{
    unsigned scale;    /* Specified q-format of the coefficients. (1+scale).(31-scale) format 
                                             Default: 1.  scale=1 implies a q-format of 2.30 */
    int coefficientB0;
    int coefficientB1;
    int coefficientB2;
    int coefficientA1;
    int coefficientA2;
} NEXUS_SrsHighPassFilterCoef;

/***************************************************************************
Summary:
SRS High Pass Filter parameter for coefficients specified by user. Also required by StudioSound.
***************************************************************************/
typedef struct NEXUS_SrsHighPassFilterCoefUserParam
{
    NEXUS_SrsFilterOrder filterOrder;            /* HighPass Filter order. Default NEXUS_SrsFilterOrder_eOrder4 */
    NEXUS_SrsHighPassFilterCoef coefficients[3];  /* Number of active biquads filter decided by filterOrder,  N = filterOrder/2 */
} NEXUS_SrsHighPassFilterCoefUserParam;


/***************************************************************************
Summary:
SRS High Pass Filter parameter for coefficients generated by specification. Also required by StudioSound.
***************************************************************************/
typedef struct NEXUS_SrsHighPassFilterCoefSpecParam
{
    unsigned cutOffFrequency;           /* Range: 20 to 1000 hz, Default: 180 Hz */
    NEXUS_SrsFilterOrder filterOrder;   /* HighPass Filter order. Default NEXUS_SrsFilterOrder_eOrder4 */
} NEXUS_SrsHighPassFilterCoefSpecParam;


/***************************************************************************
Summary:
SRS High Pass Filter Settings. Also required by StudioSound.
***************************************************************************/
typedef struct NEXUS_SrsHighPassFilterSettings
{
    bool enabled;                       /* If true, High Pass Filter processing is enabled. Default: true */
    NEXUS_SrsFilterCoefGenMode coefGenMode;         /* Coefficients generation mode. Default: NEXUS_SrsHighPassFilterCoefGenMode_eFilterUser */

    union
    {
        NEXUS_SrsHighPassFilterCoefUserParam highPassFilterCoefUser[3];  /* Array size 3, representing supported sampling rates 32, 44.1 and 48kHz, 
                                                                                                                                     will be used if coefMode is NEXUS_SrsFilterCoefGenMode_eFilterUser */
        NEXUS_SrsHighPassFilterCoefSpecParam highPassFilterCoefSpec;      /* Filter specification for generating filter coeffcients, 
                                                                                                                                      will be used if coefMode is NEXUS_SrsFilterCoefGenMode_eFilterSpec */ 
    } coefParam;
} NEXUS_SrsHighPassFilterSettings;

/***************************************************************************
Summary:
TruVolume Settings
***************************************************************************/
typedef struct NEXUS_TruVolumeSettings
{
    bool enabled;               /* If true, processing is enabled.  Otherwise this stage is bypassed */

    NEXUS_TruVolumeBlockSize blockSize;    /* Block Size for processing */

    bool enableNormalGain;      /* If true, normal processing will be performed.  If false, only bypassGain is applied. */

    unsigned inputGain;         /* Input gain ranges from 0 to 3200 in %. Default: 100 (1.0) */
    unsigned outputGain;        /* Output gain ranges from 0 to 400 in %. Default: 100 (1.0) */
    unsigned bypassGain;        /* Bypass gain (In %).  Ranges from 0 to 100.  Default: 100 (1.0) */

    unsigned referenceLevel;    /* Reference level.  Ranges from 0 to 100 in %.  Default: 50 (0.5) */

    NEXUS_TruVolumeMode mode;    /* Processing Mode. */

    NEXUS_TruVolumeSpeakerResolution speakerResolution;	

    unsigned maxGain;           /* Max gain control (In %).  Ranges from 25 to 25600.  Default: 1600 (16.0) */

    bool enableDcNotchFilter;   /* If true, the DC notch filter will be enabled */

    bool enableNoiseManager;
    int noiseManagerThreshold;  /* Noise manager threshold.  Ranges from 0 to 100 in %.  Default: 10 (0.10) */

    bool enableNormalizer;      /* If true, the normalizer is enabled.  */

    unsigned calibrate;         /* Ranges from 0..25600 in %.  Default=100 (1.0) */

    NEXUS_SrsHighPassFilterSettings highPassFilter; /* High Pass Filter Settings applied prior TruVolume */
} NEXUS_TruVolumeSettings;

/***************************************************************************
Summary:
    Get default settings for an SRS TruVolume stage
***************************************************************************/
void NEXUS_TruVolume_GetDefaultSettings(
    NEXUS_TruVolumeSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
    Open an SRS TruVolume stage
***************************************************************************/
NEXUS_TruVolumeHandle NEXUS_TruVolume_Open( /* attr{destructor=NEXUS_TruVolume_Close}  */
    const NEXUS_TruVolumeSettings *pSettings     /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
    Close an SRS TruVolume stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void NEXUS_TruVolume_Close(
    NEXUS_TruVolumeHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for an SRS TruVolume stage
***************************************************************************/
void NEXUS_TruVolume_GetSettings(
    NEXUS_TruVolumeHandle handle,
    NEXUS_TruVolumeSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Settings for an SRS TruVolume stage
***************************************************************************/
NEXUS_Error NEXUS_TruVolume_SetSettings(
    NEXUS_TruVolumeHandle handle,
    const NEXUS_TruVolumeSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the audio connector for an SRS TruVolume stage
***************************************************************************/
NEXUS_AudioInput NEXUS_TruVolume_GetConnector(
    NEXUS_TruVolumeHandle handle
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
NEXUS_Error NEXUS_TruVolume_AddInput(
    NEXUS_TruVolumeHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_TruVolume_RemoveInput(
    NEXUS_TruVolumeHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_TruVolume_RemoveAllInputs(
    NEXUS_TruVolumeHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_TRU_VOLUME_H__ */

