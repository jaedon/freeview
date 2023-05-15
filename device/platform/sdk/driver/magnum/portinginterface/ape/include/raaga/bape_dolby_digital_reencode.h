/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: bape_dolby_digital_reencode.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 10/9/12 4:37a $
*
* API Description:
*   API name: DolbyDigitalReencode
*    Specific APIs related to Dolby Digital Reencoding used in Dolby MS11
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/include/raaga/bape_dolby_digital_reencode.h $
* 
* Hydra_Software_Devel/6   10/9/12 4:37a jgarrett
* SW7425-4042: Adding additional external pcm mode settings
* 
* Hydra_Software_Devel/5   12/19/11 4:27p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* Hydra_Software_Devel/4   5/18/11 6:50p jgarrett
* SW7425-408: Adding preliminary DDRE support
* 
* Hydra_Software_Devel/3   5/14/11 12:23p jgarrett
* SW7425-408: Adding DDRE APIs
* 
* Hydra_Software_Devel/2   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
* comments
* 
* Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
* SW7422-146: Adding additional APIs
* 
***************************************************************************/
#ifndef BAPE_DOLBY_DIGITAL_REENCODE_H__
#define BAPE_DOLBY_DIGITAL_REENCODE_H__

#include "bape.h"
#include "bape_encoder.h"
#include "bape_codec_types.h"

/***************************************************************************
Summary:
Dolby Digital Reencode Handle
***************************************************************************/
typedef struct BAPE_DolbyDigitalReencode *BAPE_DolbyDigitalReencodeHandle;

/***************************************************************************
Summary:
Dolby Digital Reencode Profile
***************************************************************************/
typedef enum BAPE_DolbyDigitalReencodeProfile
{
    BAPE_DolbyDigitalReencodeProfile_eNoCompression,
    BAPE_DolbyDigitalReencodeProfile_eFilmStandardCompression,
    BAPE_DolbyDigitalReencodeProfile_eFilmLightCompression,
    BAPE_DolbyDigitalReencodeProfile_eMusicStandardCompression,
    BAPE_DolbyDigitalReencodeProfile_eMusicLightCompression,
    BAPE_DolbyDigitalReencodeProfile_eSpeechCompression,
    BAPE_DolbyDigitalReencodeProfile_eMax
} BAPE_DolbyDigitalReencodeProfile;

/***************************************************************************
Summary:
Dolby Digital Reencode DRC modes
***************************************************************************/
typedef enum BAPE_DolbyDigitalReencodeDrcMode
{
    BAPE_DolbyDigitalReencodeDrcMode_eLine,
    BAPE_DolbyDigitalReencodeDrcMode_eRf,
    BAPE_DolbyDigitalReencodeDrcMode_eMax
} BAPE_DolbyDigitalReencodeDrcMode;

/***************************************************************************
Summary:
Dolby Digital Reencode Stereo Downmix modes
***************************************************************************/
typedef enum BAPE_DolbyDigitalReencodeStereoMode
{
    BAPE_DolbyDigitalReencodeStereoMode_eLtRt,
    BAPE_DolbyDigitalReencodeStereoMode_eLoRo,
    BAPE_DolbyDigitalReencodeStereoMode_eArib,
    BAPE_DolbyDigitalReencodeStereoMode_eMax
} BAPE_DolbyDigitalReencodeStereoMode;

/***************************************************************************
Summary:
Dolby Digital Reencode Settings
***************************************************************************/
typedef struct BAPE_DolbyDigitalReencodeSettings
{
    BAPE_Ac3EncodeSettings encodeSettings;  /* Settings for the AC3/Dolby Digital encoder */

    bool loudnessEquivalenceEnabled;        /* If true, loudness equivalence will be enabled based on the mode specified
                                               in BAPE_Settings.loudnessMode.  Ignored if BAPE_Settings.loudnessMode is
                                               set to BAPE_LoudnessEquivalenceMode_eNone.  Default is true. */

    bool externalPcmMode;                   /* If true, operate with input from non-MS decoders.  
                                               Should be false if any input is from a dolby MS decoder (AC3/AC3+/AAC). */

    /* The following settings are only applied if externalPcmMode = true */
    BAPE_DolbyDigitalReencodeProfile profile;           /* Compression Profile */
    BAPE_Ac3CenterMixLevel          centerMixLevel;     /* cmixlev - Center Mix Level */
    BAPE_Ac3SurroundMixLevel        surroundMixLevel;   /* surmixlev - Surround Mix Level */
    BAPE_Ac3DolbySurround           dolbySurround;      /* dsurmod - AC3 Dolby Surround Mode */
    unsigned                        dialogLevel;        /* Dialog level of incoming PCM in dB.  Ranges from 0..31. */

    BAPE_DolbyDigitalReencodeDrcMode drcMode;           /* DRC (Dynamic Range Compression) Mode */
    BAPE_DolbyDigitalReencodeDrcMode drcModeDownmix;    /* DRC (Dynamic Range Compression) Mode for stereo downmixed data*/
    uint16_t                         drcScaleHi;        /* In %, ranges from 0..100 */
    uint16_t                         drcScaleLow;       /* In %, ranges from 0..100 */
    uint16_t                         drcScaleHiDownmix; /* In %, ranges from 0..100 */
    uint16_t                         drcScaleLowDownmix;/* In %, ranges from 0..100 */

    BAPE_DolbyDigitalReencodeStereoMode stereoMode;     /* Stereo Downmix Mode */

    BAPE_DualMonoMode                   dualMonoMode;   /* Dual Mono Processing Mode */
} BAPE_DolbyDigitalReencodeSettings;

/***************************************************************************
Summary:
    Get default settings for a Dolby Digital Reencoder
***************************************************************************/
void BAPE_DolbyDigitalReencode_GetDefaultSettings(
    BAPE_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a Dolby Digital Reencoder
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_Create(
    BAPE_Handle deviceHandle,
    const BAPE_DolbyDigitalReencodeSettings *pSettings,
    BAPE_DolbyDigitalReencodeHandle *pHandle    /* [out] */
    );

/***************************************************************************
Summary:
    Close a Dolby Digital Reencoder
***************************************************************************/
void BAPE_DolbyDigitalReencode_Destroy(
    BAPE_DolbyDigitalReencodeHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for a Dolby Digital Reencoder
***************************************************************************/
void BAPE_DolbyDigitalReencode_GetSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Set Settings for a Dolby Digital Reencoder
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_SetSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    const BAPE_DolbyDigitalReencodeSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the audio connector for a Dolby Digital Reencoder
***************************************************************************/
void BAPE_DolbyDigitalReencode_GetConnector(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_ConnectorFormat format,
    BAPE_Connector *pConnector /* [out] */
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_AddInput(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_RemoveInput(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_RemoveAllInputs(
    BAPE_DolbyDigitalReencodeHandle handle
    );

#endif /* #ifndef BAPE_DOLBY_DIGITAL_REENCODE_H__ */

