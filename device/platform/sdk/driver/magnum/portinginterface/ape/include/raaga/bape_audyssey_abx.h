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
* $brcm_Workfile: bape_audyssey_abx.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 5/14/11 11:22a $
*
* API Description:
*   API name: AudysseyAbx
*    Specific APIs related to Audyssey ABX Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_audyssey_abx.h $
* 
* Hydra_Software_Devel/1   5/14/11 11:22a jgarrett
* SWDTV-6762: Adding Audyssey ADV/ABX
* 
***************************************************************************/

#ifndef BAPE_AUDYSSEY_ABX_H__
#define BAPE_AUDYSSEY_ABX_H__

#include "bape.h"

/***************************************************************************
Summary: 
AudysseyAbx Handle 
***************************************************************************/
typedef struct BAPE_AudysseyAbx *BAPE_AudysseyAbxHandle;

/***************************************************************************
Summary:
AudysseyAbx Settings
***************************************************************************/
typedef struct BAPE_AudysseyAbxSettings
{
    bool enabled;               /* If true, processing is enabled.  Otherwise this stage is bypassed */

    unsigned filterSet;         /* Default : 1, Range [1,2] 
                                    1->is for systems that roll-off lower (42/46 inch TV) 
                                    2->is for systems that roll-off higher(22 inch TV) */
    unsigned harmonicGain;      /* Default is 6(3 dB), Ranges from 0 to 30
                                    Every step increment is equivalent to 0.5 dB step*/
    unsigned dryGain;           /* Default is 0, Ranges from 0 to 30
                                    Typically always 0, Every step increament is equivalent to 0.5 dB step */
} BAPE_AudysseyAbxSettings;

/***************************************************************************
Summary:
    Get default settings for an AudysseyAbx stage
***************************************************************************/
void BAPE_AudysseyAbx_GetDefaultSettings(
    BAPE_AudysseyAbxSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
    Open an AudysseyAbx stage
***************************************************************************/
BERR_Code BAPE_AudysseyAbx_Create(
    BAPE_Handle deviceHandle,
    const BAPE_AudysseyAbxSettings *pSettings,
    BAPE_AudysseyAbxHandle *pHandle
    );

/***************************************************************************
Summary:
    Close an AudysseyAbx stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_AudysseyAbx_Destroy(
    BAPE_AudysseyAbxHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for an AudysseyAbx stage
***************************************************************************/
void BAPE_AudysseyAbx_GetSettings(
    BAPE_AudysseyAbxHandle handle,
    BAPE_AudysseyAbxSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Settings for an AudysseyAbx stage
***************************************************************************/
BERR_Code BAPE_AudysseyAbx_SetSettings(
    BAPE_AudysseyAbxHandle handle,
    const BAPE_AudysseyAbxSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the audio connector for an AudysseyAbx stage
***************************************************************************/
void BAPE_AudysseyAbx_GetConnector(
    BAPE_AudysseyAbxHandle handle,
    BAPE_Connector *pConnector
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAbx_AddInput(
    BAPE_AudysseyAbxHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAbx_RemoveInput(
    BAPE_AudysseyAbxHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_AudysseyAbx_RemoveAllInputs(
    BAPE_AudysseyAbxHandle handle
    );

#endif /* #ifndef BAPE_AUDYSSEY_ABX_H__ */

