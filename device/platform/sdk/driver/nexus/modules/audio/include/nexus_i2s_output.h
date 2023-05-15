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
* $brcm_Workfile: nexus_i2s_output.h $
* $brcm_Revision: 10 $
* $brcm_Date: 8/18/11 5:51p $
*
* API Description:
*   API name: I2sOutput
*    Specific APIs related to I2S audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_i2s_output.h $
* 
* 10   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/8/11 6:40p jgarrett
* SWDTV-6760: Adding I2sMultiOutput
* 
* 9   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 8   1/18/11 4:17p jgarrett
* SW7422-146: Adding auto mclk mode
* 
* 7   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 6   2/7/08 12:11p vsilyaev
* PR 38692: Added attributes for shutdown functions
* 
* 5   2/6/08 4:28p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
* 
* 4   1/28/08 9:41a erickson
* PR35457: comment update
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
* Nexus_Devel/2   12/6/07 2:17p jgarrett
* PR 37971: Refactoring to connector model
* 
* Nexus_Devel/1   9/17/07 1:32p jgarrett
* PR 34954: Splitting I2S, SPDIF, and PCM inputs and outputs
* 
***************************************************************************/
#ifndef NEXUS_I2S_OUTPUT_H__
#define NEXUS_I2S_OUTPUT_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: I2sOutput

Header file: nexus_i2s_output.h

Module: Audio

Description: Route PCM audio data to an I2S output

**************************************/

/**
Summary:
Handle for I2S output
**/
typedef struct NEXUS_I2sOutput *NEXUS_I2sOutputHandle;

/***************************************************************************
Summary:
Bit Clock (SCLK) Rate
***************************************************************************/
typedef enum NEXUS_I2sOutputSclkRate
{
    NEXUS_I2sOutputSclkRate_e64Fs,
    NEXUS_I2sOutputSclkRate_e128Fs,
    NEXUS_I2sOutputSclkRate_eMax
} NEXUS_I2sOutputSclkRate;

/***************************************************************************
Summary:
Master Clock (MCLK) Rate
***************************************************************************/
typedef enum NEXUS_I2sOutputMclkRate
{
    NEXUS_I2sOutputMclkRate_e128Fs,
    NEXUS_I2sOutputMclkRate_e256Fs,
    NEXUS_I2sOutputMclkRate_e384Fs,
    NEXUS_I2sOutputMclkRate_e512Fs,
    NEXUS_I2sOutputMclkRate_eAuto,	/* Automatically select based on 
                                           timing source.  Not applicable on 
                                           all platforms */
    NEXUS_I2sOutputMclkRate_eMax
} NEXUS_I2sOutputMclkRate;

/***************************************************************************
Summary:
I2S Output Settings
***************************************************************************/
typedef struct NEXUS_I2sOutputSettings
{
    bool                     lsbAtLRClock;       /* Data Justification.
                                                    Controls whether LSB or 
                                                    MSB is at LRCK transition
                                                    TRUE: LSB, FALSE: MSB */
    bool                     alignedToLRClock;   /* Controls whether data is 
                                                    aligned with LRCK or delayed
                                                    by one SCLK period
                                                    FALSE: Delayed. 
                                                    TRUE: Aligned */
    bool                     lrClkPolarity;      /* Sets the polarity of the 
                                                    left/right clock
                                                    TRUE: High for Left
                                                    FALSE: Low for Left */
    NEXUS_I2sOutputSclkRate  sclkRate;           /* SClk (bit Clock, multiple of Fs) 
                                                    rate. */
    NEXUS_I2sOutputMclkRate  mclkRate;           /* MClk rate. (multiple of Fs) */
} NEXUS_I2sOutputSettings;

/***************************************************************************
Summary:
Get default settings for an I2S output
***************************************************************************/
void NEXUS_I2sOutput_GetDefaultSettings(
    NEXUS_I2sOutputSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an I2S Output device
***************************************************************************/
NEXUS_I2sOutputHandle NEXUS_I2sOutput_Open( /* attr{destructor=NEXUS_I2sOutput_Close}  */
    unsigned index,
    const NEXUS_I2sOutputSettings *pSettings /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an I2S Output device
***************************************************************************/
void NEXUS_I2sOutput_Close(
    NEXUS_I2sOutputHandle handle
    );

/***************************************************************************
Summary:
Get settings for an I2S output
***************************************************************************/
void NEXUS_I2sOutput_GetSettings(
    NEXUS_I2sOutputHandle handle,
    NEXUS_I2sOutputSettings *pSettings  /* [out] Settings */
    );

/***************************************************************************
Summary:
Set settings for an I2S output
***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_SetSettings(
    NEXUS_I2sOutputHandle handle,
    const NEXUS_I2sOutputSettings *pSettings
    );

/***************************************************************************
Summary:
Get the audio connector for an I2S output
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sOutput_GetConnector(
    NEXUS_I2sOutputHandle handle
    );

/**
Summary:
Handle for I2S Multichannel output
**/
typedef struct NEXUS_I2sMultiOutput *NEXUS_I2sMultiOutputHandle;

/***************************************************************************
Summary:
Multichannel I2S Modes
***************************************************************************/
typedef enum NEXUS_I2sMultiMode 
{
    NEXUS_I2sMultiMode_eMultichannel,    /* Default.  I2S multi will be used 
                                           as a single output capable of stereo
                                           or multichannel. */
    NEXUS_I2sMultiMode_eStereo,          /* I2S multi will be used as a set of
                                           discrete stereo outputs that share
                                           a common sample clock and word select. */
    NEXUS_I2sMultiMode_eMax
} NEXUS_I2sMultiMode;

/***************************************************************************
Summary:
Multichannel I2S Output Settings
***************************************************************************/
typedef struct NEXUS_I2sMultiOutputSettings
{
    NEXUS_I2sMultiMode mode; /* Mode of operation, multichannel or stereo.
                                Must be set prior to NEXUS_I2sMultiOutput_Open() 
                                and cannot be changed on the fly. */

    NEXUS_I2sOutputSettings i2sSettings;
} NEXUS_I2sMultiOutputSettings;

/***************************************************************************
Summary:
Get default settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetDefaultSettings(
    NEXUS_I2sMultiOutputSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
Open a Multichannel I2S Output device
***************************************************************************/
NEXUS_I2sMultiOutputHandle NEXUS_I2sMultiOutput_Open( /* attr{destructor=NEXUS_I2sMultiOutput_Close}  */
    unsigned index,
    const NEXUS_I2sMultiOutputSettings *pSettings   /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close a Multichannel I2S Output device
***************************************************************************/
void NEXUS_I2sMultiOutput_Close(
    NEXUS_I2sMultiOutputHandle handle
    );

/***************************************************************************
Summary:
Get settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_I2sMultiOutputSettings *pSettings  /* [out] Settings */
    );

/***************************************************************************
Summary:
Set settings for a Multichannel I2S output
***************************************************************************/
NEXUS_Error NEXUS_I2sMultiOutput_SetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    const NEXUS_I2sMultiOutputSettings *pSettings
    );

/***************************************************************************
Summary:
Get the audio connector for a multichannel I2S or the first stereo pair 
if set to stereo mode. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetConnector(
    NEXUS_I2sMultiOutputHandle handle
    );

/***************************************************************************
Summary:
Get the audio connector for an individual stereo pair on Multichannel I2S. 
Only supported if NEXUS_I2sMultiOutputSettings.mode is set to 
NEXUS_I2sMultiMode_eStereo. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetStereoConnector(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_AudioChannelPair channelPair
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_I2S_OUTPUT_H__ */

