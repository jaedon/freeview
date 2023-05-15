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
* $brcm_Workfile: nexus_i2s_input.h $
* $brcm_Revision: 11 $
* $brcm_Date: 4/22/11 10:43a $
*
* API Description:
*   API name: I2sInput
*    Specific APIs related to I2S audio inputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_i2s_input.h $
* 
* 11   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 10   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 9   7/17/08 11:52a jgarrett
* PR 44502: Adding bits per sample option, renaming settings to match
*  output
* 
* 8   7/17/08 11:29a jgarrett
* PR 44502: Adding additional I2S configuration parameters
* 
* 7   6/19/08 3:17p jgarrett
* PR 42243: Moving input volume control to specific inputs and out of
*  mixer
* 
* 6   4/22/08 10:56a jgarrett
* PR 41880: Changing return type for stop routines
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
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/3   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
* Nexus_Devel/2   11/27/07 5:25p jgarrett
* PR 37471: Adding I2sInput and SpdifInput
*
* Nexus_Devel/1   9/17/07 1:32p jgarrett
* PR 34954: Splitting I2S, SPDIF, and PCM inputs and outputs
*
***************************************************************************/
#ifndef NEXUS_I2S_INPUT_H__
#define NEXUS_I2S_INPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_timebase.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: I2sInput

Header file: nexus_i2s_input.h

Module: Audio

Description: Route incoming I2S audio data

**************************************/

/**
Summary:
Handle for I2S input
**/
typedef struct NEXUS_I2sInput *NEXUS_I2sInputHandle;

/***************************************************************************
Summary:
I2S Input Settings
***************************************************************************/
typedef struct NEXUS_I2sInputSettings
{
    unsigned sampleRate;        /* In Hz - can only be changed while stopped */
    unsigned bitsPerSample;     /* Supports 16, 18, 20, and 24 - can only be changed while stopped. */
    NEXUS_Timebase timebase;    /* Timebase to use while capturing - can only be changed while stopped */
    bool lsbAtLRClock;          /* Data Justification.  If true, LSB is at LR clock transition else MSB. - can only be changed while stopped */
    bool alignedToLRClock;      /* If true, data is aligned with LR clock.  If false, data is delayed by one SCLK period. - can only be changed while stopped */
    bool lrClkPolarity;         /* If true, a high edge of LR clock indicates left channel data.  Else, a low edge does. - can only be changed while stopped */

    int32_t leftVolume;         /* Linear volume level of left channel output */
    int32_t rightVolume;        /* Linear volume level of right channel output */
    bool muted;
} NEXUS_I2sInputSettings;

/***************************************************************************
Summary:
Get default settings for an I2S Input
***************************************************************************/
void NEXUS_I2sInput_GetDefaultSettings(
    NEXUS_I2sInputSettings *pSettings      /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an I2S input.
***************************************************************************/
NEXUS_I2sInputHandle NEXUS_I2sInput_Open( /* attr{destructor=NEXUS_I2sInput_Close}  */
    unsigned index,
    const NEXUS_I2sInputSettings *pSettings /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an I2S Input
***************************************************************************/
void NEXUS_I2sInput_Close(
    NEXUS_I2sInputHandle handle
    );

/***************************************************************************
Summary:
Get settings for an I2S input
***************************************************************************/
void NEXUS_I2sInput_GetSettings(
    NEXUS_I2sInputHandle handle,
    NEXUS_I2sInputSettings *pSettings  /* [out] Settings */
    );

/***************************************************************************
Summary:
Set settings for an I2S input
***************************************************************************/
NEXUS_Error NEXUS_I2sInput_SetSettings(
    NEXUS_I2sInputHandle handle,
    const NEXUS_I2sInputSettings *pSettings    /* [in] Settings */
    );

/***************************************************************************
Summary:
Start capturing data from an I2S Input
***************************************************************************/
NEXUS_Error NEXUS_I2sInput_Start(
    NEXUS_I2sInputHandle handle
    );

/***************************************************************************
Summary:
Stop capturing data from an I2S Input
***************************************************************************/
void NEXUS_I2sInput_Stop(
    NEXUS_I2sInputHandle handle
    );

/***************************************************************************
Summary:
Get an audio connector for use with downstream components
**************************************************************************/
NEXUS_AudioInput NEXUS_I2sInput_GetConnector(
    NEXUS_I2sInputHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_I2S_INPUT_H__ */

