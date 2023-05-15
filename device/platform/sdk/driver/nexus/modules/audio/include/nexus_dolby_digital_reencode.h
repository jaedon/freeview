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
* $brcm_Workfile: nexus_dolby_digital_reencode.h $
* $brcm_Revision: 7 $
* $brcm_Date: 2/2/12 11:37a $
*
* API Description:
*   API name: DolbyDigitalReencode
*    Specific APIs related to Dolby Digital Reencoding used in Dolby MS11
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_dolby_digital_reencode.h $
* 
* 7   2/2/12 11:37a jgarrett
* SW7420-2230: Adding additional certification params
* 
* 6   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 5   11/29/11 2:21p jgarrett
* SW7420-2131: Adding certification parameters
* 
* 4   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 3   5/13/11 12:18p erickson
* SW7420-1873: remove shutdown attribute
* 
* 2   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 1   8/18/10 10:06a jgarrett
* SW7420-790: Merging MS11 updates to main branch
* 
* SW7420-790/1   6/11/10 5:00p jgarrett
* SW7420-790: Adding MS11 Support
* 
***************************************************************************/
#ifndef NEXUS_DOLBY_DIGITAL_REENCODE_H__
#define NEXUS_DOLBY_DIGITAL_REENCODE_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_ac3_encode.h"
#include "nexus_audio_decoder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: DolbyDigitalReencode

Header file: nexus_dolby_digital_reencode.h

Module: Audio

Description: Dolby Digital Reencode for MS11

**************************************/

/**
Summary:
Handle for Dolby Digital Reencoder stage
**/
typedef struct NEXUS_DolbyDigitalReencode *NEXUS_DolbyDigitalReencodeHandle;

/***************************************************************************
Summary:
Dolby Digital Reencode Downmix Modes
***************************************************************************/
typedef enum NEXUS_DolbyDigitalReencodeDownmixMode
{
    NEXUS_DolbyDigitalReencodeDownmixMode_eLtRt,
    NEXUS_DolbyDigitalReencodeDownmixMode_eLoRo,
    NEXUS_DolbyDigitalReencodeDownmixMode_eArib,
    NEXUS_DolbyDigitalReencodeDownmixMode_eMax
} NEXUS_DolbyDigitalReencodeDownmixMode;

/***************************************************************************
Summary:
Dolby Digital Reencode Profile
***************************************************************************/
typedef enum NEXUS_DolbyDigitalReencodeProfile
{
    NEXUS_DolbyDigitalReencodeProfile_eNoCompression,
    NEXUS_DolbyDigitalReencodeProfile_eFilmStandardCompression,
    NEXUS_DolbyDigitalReencodeProfile_eFilmLightCompression,
    NEXUS_DolbyDigitalReencodeProfile_eMusicStandardCompression,
    NEXUS_DolbyDigitalReencodeProfile_eMusicLightCompression,
    NEXUS_DolbyDigitalReencodeProfile_eSpeechCompression,
    NEXUS_DolbyDigitalReencodeProfile_eMax
} NEXUS_DolbyDigitalReencodeProfile;

/***************************************************************************
Summary:
Dolby Digital Reencode Settings
***************************************************************************/
typedef struct NEXUS_DolbyDigitalReencodeSettings
{
    NEXUS_Ac3EncodeSettings encodeSettings; /* Settings for the AC3/Dolby Digital encoder */

    NEXUS_AudioDecoderDualMonoMode          dualMonoMode;   /* Mode for streams coded as dual-mono */
    NEXUS_AudioDecoderOutputMode            outputMode;     /* DDRE output mode.  In general, eAuto is correct, but this may be overridden
                                                               for testing and/or certification purposes. */
    NEXUS_AudioDecoderOutputLfeMode         outputLfeMode;  /* LFE output flag.  In general, eAuto is correct, but this may be overridden
                                                               for testing and/or certification purposes. */

    NEXUS_AudioDecoderDolbyDrcMode          drcMode;
    NEXUS_DolbyDigitalReencodeDownmixMode   stereoDownmixMode;
    uint16_t                                cut;                           /* DRC Cut factor */
    uint16_t                                boost;                         /* DRC Boost factor */

    bool loudnessEquivalenceEnabled;    /* If true, loudness equivalence will be enabled based on the mode specified
                                           in NEXUS_AudioModuleSettings.loudnessMode.  Ignored if NEXUS_AudioModuleSettings.loudnessMode 
                                           is set to NEXUS_AudioLoudnessEquivalenceMode_eNone.  Default is true. */

    bool externalPcmMode;               /* If true, operate with input from non-MS decoders.  
                                           Should be false if any input is from a dolby MS decoder (AC3/AC3+/AAC). */
    bool certificationMode;             /* If true, operate in certification mode.  This also implies externalPcmMode=true. */

    /* The following settings are only applied if externalPcmMode = true or certificationMode = true. */
    NEXUS_DolbyDigitalReencodeProfile     profile;              /* Compression Profile */
    NEXUS_AudioAc3CenterMixLevel          centerMixLevel;       /* cmixlev - Center Mix Level */
    NEXUS_AudioAc3SurroundMixLevel        surroundMixLevel;     /* surmixlev - Surround Mix Level */
    NEXUS_AudioAc3DolbySurround           dolbySurround;        /* dsurmod - AC3 Dolby Surround Mode */
    unsigned                              dialogLevel;          /* Dialog level of incoming PCM in dB.  Ranges from 0..31. */

    /* The following settings are only applied if certificationMode = true. */
    NEXUS_AudioAc3Acmod                   inputAcmod;           /* ACMOD of the incoming stream */
    bool                                  inputHasLfe;          /* True if the incoming stream has LFE data */
    bool                                  inputHasDv258Applied; /* True if the input has DV258 processing applied. */

} NEXUS_DolbyDigitalReencodeSettings;

/***************************************************************************
Summary:
    Get default settings for a Dolby Digital Reencoder
***************************************************************************/
void NEXUS_DolbyDigitalReencode_GetDefaultSettings(
    NEXUS_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a Dolby Digital Reencoder
***************************************************************************/
NEXUS_DolbyDigitalReencodeHandle NEXUS_DolbyDigitalReencode_Open(
    const NEXUS_DolbyDigitalReencodeSettings *pSettings /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
    Close a Dolby Digital Reencoder
***************************************************************************/
void NEXUS_DolbyDigitalReencode_Close(
    NEXUS_DolbyDigitalReencodeHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for a Dolby Digital Reencoder
***************************************************************************/
void NEXUS_DolbyDigitalReencode_GetSettings(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Set Settings for a Dolby Digital Reencoder
***************************************************************************/
NEXUS_Error NEXUS_DolbyDigitalReencode_SetSettings(
    NEXUS_DolbyDigitalReencodeHandle handle,
    const NEXUS_DolbyDigitalReencodeSettings *pSettings
    );

/***************************************************************************
Summary:
    Connector Types for a Dolby Digital Reencoder
***************************************************************************/
#define NEXUS_DolbyDigitalReencodeConnectorType                 NEXUS_AudioConnectorType
#define NEXUS_DolbyDigitalReencodeConnectorType_eStereo         NEXUS_AudioConnectorType_eStereo
#define NEXUS_DolbyDigitalReencodeConnectorType_eMultichannel   NEXUS_AudioConnectorType_eMultichannel
#define NEXUS_DolbyDigitalReencodeConnectorType_eCompressed     NEXUS_AudioConnectorType_eCompressed
#define NEXUS_DolbyDigitalReencodeConnectorType_eMax            NEXUS_AudioConnectorType_eMax

/***************************************************************************
Summary:
    Get the audio connector for a Dolby Digital Reencoder
***************************************************************************/
NEXUS_AudioInput NEXUS_DolbyDigitalReencode_GetConnector(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioConnectorType connectorType
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
NEXUS_Error NEXUS_DolbyDigitalReencode_AddInput(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_DolbyDigitalReencode_RemoveInput(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_DolbyDigitalReencode_RemoveAllInputs(
    NEXUS_DolbyDigitalReencodeHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_DOLBY_DIGITAL_REENCODER_H__ */
