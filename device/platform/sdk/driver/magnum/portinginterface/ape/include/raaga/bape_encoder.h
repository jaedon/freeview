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
* $brcm_Workfile: bape_encoder.h $
* $brcm_Revision: Hydra_Software_Devel/SW7425-4386/2 $
* $brcm_Date: 5/16/13 5:52p $
*
* API Description:
*   API name: Encoder
*    Specific APIs related to Audio Encoding
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/include/raaga/bape_encoder.h $
* 
* Hydra_Software_Devel/SW7425-4386/2   5/16/13 5:52p mpeteri
* SW7425-4386: revisions per NEXUS API CR
* 
* Hydra_Software_Devel/SW7425-4386/1   5/14/13 4:53p mpeteri
* SW7425-4386: initial changes for MP3/AAC mono encode
* 
* Hydra_Software_Devel/14   9/10/12 4:43p jgarrett
* SW7425-3795: Adding enumeration for aac encode complexity
* 
* Hydra_Software_Devel/13   8/31/12 4:13p jgarrett
* SW7425-3795: Exposing AAC complexity setting
* 
* Hydra_Software_Devel/12   7/3/12 2:12p jgarrett
* SW7425-3115: Adding output sample rate conversion for aac encoder
* 
* Hydra_Software_Devel/11   1/31/12 6:17p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* Hydra_Software_Devel/10   12/19/11 4:27p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* Hydra_Software_Devel/9   12/8/11 2:21p jgarrett
* SW7425-1478: Adding WMA Std settings interface
* 
* Hydra_Software_Devel/8   12/7/11 5:44p jgarrett
* SW7425-1478: Adding WMA encoder interface
* 
* Hydra_Software_Devel/7   6/16/11 3:27p jgarrett
* SW7425-741: Adding MP3 encoder
* 
* Hydra_Software_Devel/6   6/14/11 3:37p jgarrett
* SW7425-716: Updating AAC bitrate comments
* 
* Hydra_Software_Devel/5   3/21/11 7:07p jgarrett
* SW7422-355: Adding encoder updates
* 
* Hydra_Software_Devel/4   3/1/11 7:26p jgarrett
* SW7422-146: Adding encoders
* 
* Hydra_Software_Devel/3   2/22/11 5:43p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
* comments
* 
* Hydra_Software_Devel/2   1/13/11 5:26p jgarrett
* SW7422-146: Renaming encoder codec settings
* 
* Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
* SW7422-146: Adding additional APIs
* 
***************************************************************************/

#ifndef BAPE_ENCODER_H__
#define BAPE_ENCODER_H__

#include "bape.h"

/***************************************************************************
Summary:
Audio Encoder Settings
***************************************************************************/
typedef struct BAPE_Encoder *BAPE_EncoderHandle;

/***************************************************************************
Summary:
Audio Encoder Settings
***************************************************************************/
typedef struct BAPE_EncoderSettings
{
    BAVC_AudioCompressionStd codec; /* Codec to output.  Cannot be changed while running. */

    bool loudnessEquivalenceEnabled;    /* If true, loudness equivalence will be enabled based on the mode specified
                                           in BAPE_Settings.loudnessMode.  Ignored if BAPE_Settings.loudnessMode is
                                           set to BAPE_LoudnessEquivalenceMode_eNone.  Default is true. */
} BAPE_EncoderSettings;

/***************************************************************************
Summary:
    Get default settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetDefaultSettings(
    BAPE_EncoderSettings *pSettings   /* [out] default settings */
    );

/***************************************************************************
Summary:
    Open an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_Create(
    BAPE_Handle deviceHandle,
    const BAPE_EncoderSettings *pSettings,
    BAPE_EncoderHandle *pHandle             /* [out] */
    );

/***************************************************************************
Summary:
    Close an Audio Encoder stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_Encoder_Destroy(
    BAPE_EncoderHandle handle
    );

/***************************************************************************
Summary:
    Get Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetSettings(
    BAPE_EncoderHandle handle,
    BAPE_EncoderSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_SetSettings(
    BAPE_EncoderHandle handle,
    const BAPE_EncoderSettings *pSettings
    );

/***************************************************************************
Summary:
AC3 Encode Settings
***************************************************************************/
typedef struct BAPE_Ac3EncodeSettings
{
    bool spdifHeaderEnabled;    /* If true, SPDIF header generation is enabled.  */
    bool certificationMode;
} BAPE_Ac3EncodeSettings;

/***************************************************************************
Summary:
DTS Encode Settings
***************************************************************************/
typedef struct BAPE_DtsEncodeSettings
{
    bool spdifHeaderEnabled;    /* If true, SPDIF header generation is enabled.  */
} BAPE_DtsEncodeSettings;

/***************************************************************************
Summary:
AAC Encode Complexity 
 
Description: 
AAC Encode algorithm complexity.  Default is the lowest complexity, 
which uses the fewest DSP cycles.  To achieve a higher quality encode at a 
given bitrate, this can be set to higher values at the cost of increased 
DSP usage.
***************************************************************************/
typedef enum BAPE_AacEncodeComplexity
{
    BAPE_AacEncodeComplexity_eLowest,       /* Lowest complexity. */
    BAPE_AacEncodeComplexity_eMediumLow,
    BAPE_AacEncodeComplexity_eMediumHigh,
    BAPE_AacEncodeComplexity_eHighest,      /* Highest complexity. */
    BAPE_AacEncodeComplexity_eMax   
} BAPE_AacEncodeComplexity;

/***************************************************************************
Summary:
AAC Encode Settings
***************************************************************************/
typedef struct BAPE_AacEncodeSettings
{
    unsigned                    bitRate;        /* Output bitrate of the encoder in bps. 
                                                   For AAC-Plus, bitrates from 16000 to 48000 are valid.  
                                                   For AAC-LC, bitrates from 16000 to 320000 are valid. */
    BAPE_ChannelMode            channelMode;    /* Output channel mode.  Currently only 2_0 and 1_0 are supported. */
    BAPE_MonoChannelMode        monoMode;       /* Channel to be used for mono encode */
    unsigned                    sampleRate;     /* Output sample rate.  If 0 (default), the stream sample rate will be 
                                                   preserved.  Otherwise, if set to 32000 48 kHz streams will be
                                                   converted to 32 kHz while other sample rates will be preserved. */
    BAPE_AacEncodeComplexity    complexity;     /* Encode algorithm complexity.  Default is the lowest
                                                   complexity, which uses the fewest DSP cycles.  */
} BAPE_AacEncodeSettings;

/***************************************************************************
Summary:
MPEG Encode Settings
***************************************************************************/
typedef struct BAPE_MpegEncodeSettings
{
    unsigned                    bitRate;        /* Output bitrate of the encoder in bps. Ranges from 32000 to 320000. */
    bool                        privateBit;     /* If true, the private bit will be asserted in the header */
    bool                        copyrightBit;   /* If true, the copyright bit will be asserted in the header */
    bool                        originalBit;    /* If true, the original bit will be asserted in the header */
    BAPE_MpegEmphasisMode       emphasisMode;   /* De-Emphasis mode */
    BAPE_ChannelMode            channelMode;    /* Output channel mode.  Currently only 2_0 and 1_0 are supported. */
    BAPE_MonoChannelMode        monoMode;       /* Channel to be used for mono encode */
} BAPE_MpegEncodeSettings;

/***************************************************************************
Summary:
WMA Encode Settings
***************************************************************************/
typedef struct BAPE_WmaEncodeSettings
{
    unsigned bitRate;               /* Output bitrate of the encoder in bps. Default is 192000. Lower bitrates may cause very high DSP load. */

    BAPE_ChannelMode channelMode;   /* Output channel mode.  Currently only 2_0 and 1_0 are supported. */
} BAPE_WmaEncodeSettings;

/***************************************************************************
Summary:
G.711/G.726 Compression Modes
***************************************************************************/
typedef enum BAPE_G711G726CompressionMode
{
    BAPE_G711G726CompressionMode_eUlaw,     /* u-Law Compression.  Typically used in North America and Japan. */
    BAPE_G711G726CompressionMode_eAlaw,     /* a-Law Compression.  Typically used in Europe and elsewhere. */
    BAPE_G711G726CompressionMode_eMax
} BAPE_G711G726CompressionMode;

/***************************************************************************
Summary:
G.711 Encode Settings
***************************************************************************/
typedef struct BAPE_G711EncodeSettings
{
    BAPE_G711G726CompressionMode compressionMode;   /* Select compression mode. */
} BAPE_G711EncodeSettings;

/***************************************************************************
Summary:
G.726 Encode Settings
***************************************************************************/
typedef struct BAPE_G726EncodeSettings
{
    BAPE_G711G726CompressionMode compressionMode;   /* Select compression mode. */

    unsigned bitRate;                               /* Output bitrate of the encoder in bps.  Valid values are 16000, 24000, 32000, and 40000 for G.726 */
} BAPE_G726EncodeSettings;

/***************************************************************************
Summary:
G.729 Encode Settings
***************************************************************************/
typedef struct BAPE_G729EncodeSettings
{
    bool dtxEnabled;    /* If true, DTX (Discontinuous Transmission) is enabled to conserve bandwidth during silence. */

    unsigned bitRate;   /* Output bitrate of the encoder in bps.  Valid values are 6400, 8000, and 11800 for G.729 */
} BAPE_G729EncodeSettings;

/***************************************************************************
Summary:
G.723.1 Encode Settings
***************************************************************************/
typedef struct BAPE_G723_1EncodeSettings
{
    bool vadEnabled;    /* If true, VAD (Voice Activity Detection) is enabled to conserve bandwidth during silence. */
    bool hpfEnabled;    /* If true, a high-pass filter will be enabled. */

    unsigned bitRate;   /* Output bitrate of the encoder in bps.  Valid values are 6300 and 5300 for G.723.1 */
} BAPE_G723_1EncodeSettings;

/***************************************************************************
Summary:
Codec-Specific Settings for an audio encoder
***************************************************************************/
typedef struct BAPE_EncoderCodecSettings
{
    BAVC_AudioCompressionStd codec; /* this is used for the codecSettings lookup */
    union
    {
        BAPE_Ac3EncodeSettings    ac3;
        BAPE_DtsEncodeSettings    dts;     
        BAPE_AacEncodeSettings    aac;
        BAPE_AacEncodeSettings    aacPlus;   
        BAPE_MpegEncodeSettings   mp3;
        BAPE_WmaEncodeSettings    wmaStd;
        BAPE_G711EncodeSettings   g711;
        BAPE_G726EncodeSettings   g726;
        BAPE_G729EncodeSettings   g729;
        BAPE_G723_1EncodeSettings g723_1;
    } codecSettings;
} BAPE_EncoderCodecSettings;

/***************************************************************************
Summary:
    Get Codec-Specific Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetCodecSettings(
    BAPE_EncoderHandle handle,
    BAVC_AudioCompressionStd codec,              /* the codec for which you are retrieving settings. */
    BAPE_EncoderCodecSettings *pSettings    /* [out] Settings */
    );

/***************************************************************************
Summary:
    Set Codec-Specific Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_SetCodecSettings(
    BAPE_EncoderHandle handle,
    const BAPE_EncoderCodecSettings *pSettings
    );

/***************************************************************************
Summary:
    Get the audio connector for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetConnector(
    BAPE_EncoderHandle handle,
    BAPE_Connector *pConnector   /* [out] */
    );

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_Encoder_AddInput(
    BAPE_EncoderHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_Encoder_RemoveInput(
    BAPE_EncoderHandle handle,
    BAPE_Connector input
    );

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_Encoder_RemoveAllInputs(
    BAPE_EncoderHandle handle
    );

#endif /* #ifndef BAPE_ENCODER_H__ */


