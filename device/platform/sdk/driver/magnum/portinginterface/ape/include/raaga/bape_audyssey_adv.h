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
* $brcm_Workfile: bape_audyssey_adv.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 5/14/11 11:22a $
*
* API Description:
*   API name: AudysseyAdv
*    Specific APIs related to Audyssey ADV Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_audyssey_adv.h $
* 
* Hydra_Software_Devel/1   5/14/11 11:22a jgarrett
* SWDTV-6762: Adding Audyssey ADV/ABX
* 
***************************************************************************/

#ifndef BAPE_AUDYSSEY_ADV_H__
#define BAPE_AUDYSSEY_ADV_H__

#include "bape.h"

/***************************************************************************
Summary: 
    AudysseyAdv Handle 
***************************************************************************/
typedef struct BAPE_AudysseyAdv *BAPE_AudysseyAdvHandle;

/***************************************************************************
Summary:
    Audyssey ADV Dynamic EQ Mode
***************************************************************************/
typedef enum BAPE_AudysseyAdvDynamicEqMode
{
    BAPE_AudysseyAdvDynamicEqMode_eOff,
    BAPE_AudysseyAdvDynamicEqMode_eOn,
    BAPE_AudysseyAdvDynamicEqMode_eRampOff,
    BAPE_AudysseyAdvDynamicEqMode_eMax
} BAPE_AudysseyAdvDynamicEqMode;

/***************************************************************************
Summary:
    Audyssey ADV Settings
***************************************************************************/
typedef struct BAPE_AudysseyAdvSettings
{
    bool enabled;                       /* If true, processing is enabled.  Otherwise this stage is bypassed */

    uint8_t channelMask;                /* Specifies the channels that will have gain applied.  
                                           Default: 0xFF	All channels
                                           Meaning of bits
                                            0 -> L, 1 -> R, 2 -> C, 3 -> LFE
                                            4 -> Ls, 5-> Rs, 6 -> Lb, 7 -> Rb */

    bool reInitialize;                  /* If true, processing will be reset/re-initialized. */

    bool volumeScalingEnabled;          /* If true, dynamic volume scaling is enabled */

    bool dynamicSurroundGainEnabled;    /* If true (default), surround channel gain is enabled.  */

    bool hiLoCompress;                  /* If true, there will be boost in calc cntr gain. */

    BAPE_AudysseyAdvDynamicEqMode dynamicEqMode; /* Dynamic Equalizer Mode */

    int volume;		                    /* Volume setting for Gain function and Dynamic EQ.  Default : 0. Ranges from -10..10 */
    int gainCompression;                /* Gain Compression Factor.  Default: 180, Range [1, 180, 240] */
    int headroomOffset;		            /* Headroom offset.  Default: 0, Ranges from -100..100 */

    int chCalbGain[8];		            /* Calibrated Channel Gain. Default: 0, Ranges from -100..100 */
} BAPE_AudysseyAdvSettings;

/***************************************************************************
Summary:
    Get default settings for an AudysseyAdv stage
***************************************************************************/
void BAPE_AudysseyAdv_GetDefaultSettings(
    BAPE_AudysseyAdvSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
    Open an AudysseyAdv stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_Create(
    BAPE_Handle deviceHandle,
    const BAPE_AudysseyAdvSettings *pSettings,
    BAPE_AudysseyAdvHandle *pHandle
    );

/***************************************************************************
Summary:
    Close an AudysseyAdv stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_AudysseyAdv_Destroy(
    BAPE_AudysseyAdvHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for an AudysseyAdv stage
***************************************************************************/
void BAPE_AudysseyAdv_GetSettings(
    BAPE_AudysseyAdvHandle handle,
    BAPE_AudysseyAdvSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Settings for an AudysseyAdv stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_SetSettings(
    BAPE_AudysseyAdvHandle handle,
    const BAPE_AudysseyAdvSettings *pSettings
    );

/***************************************************************************
Summary:
    Audyssey ADV processing status
***************************************************************************/
typedef struct BAPE_AudysseyAdvStatus
{
    int volumeApplied;      /* Dynamic EQ Volume applied in dB*/
    int chCalcLevel[8];     /* chCalcLevel in dB in Q10.22 floating format. */
    unsigned numChannels;   /* Number of channels */
    BAPE_AudysseyAdvDynamicEqMode dynamicEqMode; /* Dynamic Equalizer Mode */
} BAPE_AudysseyAdvStatus;

/***************************************************************************
Summary:
    Get Status for an AudysseyAdv stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_GetStatus(
    BAPE_AudysseyAdvHandle handle,
    BAPE_AudysseyAdvStatus *pStatus    /* [out] Status */
    );

/***************************************************************************
Summary:
    Get the audio connector for an AudysseyAdv stage
***************************************************************************/
void BAPE_AudysseyAdv_GetConnector(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector *pConnector
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_AddInput(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_RemoveInput(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_RemoveAllInputs(
    BAPE_AudysseyAdvHandle handle
    );

#endif /* #ifndef BAPE_AUDYSSEY_Adv_H__ */

