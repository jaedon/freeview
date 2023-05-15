/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_types.h $
* $brcm_Revision: SW7425-4386-1/2 $
* $brcm_Date: 5/21/13 3:18p $
*
* API Description:
*   API name: Base
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/nexus_audio_types.h $
* 
* SW7425-4386-1/2   5/21/13 3:18p scho
* SW7425-4386-1: updates to mono encode APIs
* 
* SW7425-4386-1/1   5/21/13 2:50p scho
* SW7425-4386-1: updates to mono encode APIs
* 
* 30   5/23/12 6:12p jgarrett
* SW7425-3070: Adding NEXUS_AudioOutputPll Settings to control VCXO
*  source
* 
* 29   5/4/12 11:05a jgarrett
* SW7425-2075: Merge to main branch
* 
* SW7425-2075/2   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline updates
* 
* 28   5/1/12 6:22p jgarrett
* SW7425-2690: Adding Audio Capabilities routine
* 
* SW7425-2075/1   4/16/12 3:46p jgarrett
* SW7425-2075: Adding Compressed 4x/16x connectors
* 
* 27   4/12/12 4:00p jgarrett
* SW7425-2378: Adding Monkey's Audio and MLP
* 
* 26   2/7/12 7:13p jgarrett
* SW7425-2136: Adding FLAC audio
* 
* 25   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 24   12/29/11 2:48p jgarrett
* SW7231-500: Adding IEEE-1394 LPCM
* 
* 23   11/21/11 2:48p jgarrett
* SW7425-1406: Adding Vorbis audio support
* 
* VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
* SW7425-1406: Adding Vorbis support
* 
* 22   7/1/11 9:58a jgarrett
* SW7405-5072: Adding WMA TS Support
* 
* 21   6/28/11 3:36p jgarrett
* SW7425-649: Cleaning up AAC codec comments
* 
* 20   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 19   5/23/11 6:49p jgarrett
* SW7425-402: Adding multichannel capture support
* 
* 18   4/27/11 4:19p jgarrett
* SW7420-1826: Adding AudioOutputGroup Interface
* 
* 17   1/20/11 1:46p mphillip
* SW7405-5103: Merge Dts enum additions to main
* 
* SW7405-5103/1   1/20/11 12:40p dlwin
* SW7504-5103: Add Nexus_AudioCodec_eDtsLegacy for 14bit DTS streams with
*  legacy framesync.
* 
* 16   12/29/10 6:29p jgarrett
* SW7425-41: Adding audio encoder and mux outputs
* 
* 15   11/18/10 11:06a jhaberf
* SW35230-91: Added ADPCM audio decoder support
* 
* 14   11/3/10 7:10p jgarrett
* SW35230-91: Renamed NEXUS_AudioCodec_eRealAudioLbr to _eCook
* 
* 13   10/12/10 6:39p jhaberf
* SW35230-91: Added Real Audio LBR codec support
* 
* 12   5/3/10 5:22p jgarrett
* SW7405-4212: Removing DolbyPulse as an explicit codec type
* 
* SW7405-4212/1   4/26/10 5:23p jgarrett
* SW7405-4212: Adding MS10 changes for pulse/transcode
* 
* 11   3/18/10 10:55a jgarrett
* SW7405-4030: Adding Dolby Pulse Support
* 
* 10   10/8/09 6:03p jgarrett
* SW7405-3064: Adding DRA
* 
* 9   9/14/09 10:26a jgarrett
* SW3548-2188: Adding AMR support
* 
* 8   6/30/09 3:14p jgarrett
* PR 55809: Adding PcmWav codec
* 
* 7   10/22/08 7:27p jgarrett
* PR 47360: Adding PCM codec
* 
* 6   8/22/08 4:49p jgarrett
* PR 44766: PR 44767: Reworking downmix and dual-mono logic
* 
* 5   7/21/08 4:28p jgarrett
* PR 44143: Fixing codec numbering mismatch
* 
* 3   3/12/08 2:29p katrep
* PR40514:Add support AVS codecs
* 
* 2   1/28/08 9:41a erickson
* PR35457: comment update
* 
* 1   1/18/08 2:12p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/10   1/9/08 5:56p jgarrett
* PR 38535: Adding channel status
* 
* Nexus_Devel/9   1/8/08 7:01p jgarrett
* PR 35835: Adding volume type
* 
* Nexus_Devel/8   11/28/07 10:37a erickson
* PR35457: doc update
* 
* Nexus_Devel/7   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
* 
* Nexus_Devel/6   11/1/07 9:04a erickson
* PR36633: rework enums to naming convention
* 
* Nexus_Devel/5   10/4/07 2:33p erickson
* PR34594: removed audio descriptors
* 
* Nexus_Devel/4   9/27/07 7:51p jgarrett
* PR 34594: Adding downmix mode
* 
* Nexus_Devel/3   9/21/07 9:36a erickson
* PR34954: fix typos
* 
* Nexus_Devel/2   9/17/07 1:33p jgarrett
* PR 34954: Refactoring input enum
* 
* Nexus_Devel/1   9/12/07 1:55p vsilyaev
* PR 34419: Base audio and video types
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_TYPES_H
#define NEXUS_AUDIO_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
Summary:
Audio codecs
***************************************************************************/
typedef enum NEXUS_AudioCodec 
{
    NEXUS_AudioCodec_eUnknown = 0,    /* unknown/not supported audio format */
    NEXUS_AudioCodec_eMpeg,           /* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
    NEXUS_AudioCodec_eMp3,            /* MPEG1/2, layer 3. */
    NEXUS_AudioCodec_eAac,            /* Advanced audio coding with ADTS (Audio Data Transport Format) sync */
    NEXUS_AudioCodec_eAacAdts=NEXUS_AudioCodec_eAac,    /* Advanced audio coding with ADTS (Audio Data Transport Format) sync */
    NEXUS_AudioCodec_eAacLoas,        /* Advanced audio coding with LOAS (Low Overhead Audio Stream) sync and LATM mux */
    NEXUS_AudioCodec_eAacPlus,        /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
    NEXUS_AudioCodec_eAacPlusLoas =NEXUS_AudioCodec_eAacPlus,    /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
    NEXUS_AudioCodec_eAacPlusAdts,    /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync */
    NEXUS_AudioCodec_eAc3,            /* Dolby Digital AC3 audio */
    NEXUS_AudioCodec_eAc3Plus,        /* Dolby Digital Plus (AC3+ or DDP) audio */
    NEXUS_AudioCodec_eDts,            /* Digital Digital Surround sound, uses non-legacy frame-sync */
    NEXUS_AudioCodec_eLpcmDvd,        /* LPCM, DVD mode */
    NEXUS_AudioCodec_eLpcmHdDvd,      /* LPCM, HD-DVD mode */
    NEXUS_AudioCodec_eLpcmBluRay,     /* LPCM, Blu-Ray mode */
    NEXUS_AudioCodec_eDtsHd,          /* Digital Digital Surround sound, HD, uses non-legacy frame-sync, decodes only DTS part of DTS-HD stream */
    NEXUS_AudioCodec_eWmaStd,         /* WMA Standard */
    NEXUS_AudioCodec_eWmaStdTs,       /* WMA Standard with a 24-byte extended header */
    NEXUS_AudioCodec_eWmaPro,         /* WMA Professional */
    NEXUS_AudioCodec_eAvs,            /* AVS */ 
    NEXUS_AudioCodec_ePcm,            /* PCM audio - Generally used only with inputs such as SPDIF or HDMI. */ 
    NEXUS_AudioCodec_ePcmWav,         /* PCM audio with Wave header - Used with streams containing PCM audio */
    NEXUS_AudioCodec_eAmr,            /* Adaptive Multi-Rate compression (typically used w/3GPP) */
    NEXUS_AudioCodec_eDra,            /* Dynamic Resolution Adaptation.  Used in Blu-Ray and China Broadcasts. */
    NEXUS_AudioCodec_eCook,           /* Real Audio 8 LBR */
    NEXUS_AudioCodec_eAdpcm,          /* MS ADPCM audio format */
    NEXUS_AudioCodec_eSbc,            /* Sub Band Codec used in Bluetooth A2DP audio */
    NEXUS_AudioCodec_eDtsLegacy,      /* Digital Digital Surround sound, legacy mode (14 bit), uses legacy frame-sync */
    NEXUS_AudioCodec_eVorbis,         /* Vorbis audio codec.  Typically used with OGG or WebM container formats. */
    NEXUS_AudioCodec_eLpcm1394,       /* IEEE-1394 LPCM audio  */
    NEXUS_AudioCodec_eG711,           /* G.711 a-law and u-law companding.  Typically used for voice transmission. */
    NEXUS_AudioCodec_eG723_1,         /* G.723.1 Dual Rate Speech Coder for Multimedia Communications.  Used in H.324 and 3GPP 3G-324M.  This is different from G.723, which was superceded by G.726. */
    NEXUS_AudioCodec_eG726,           /* G.726 ADPCM speech codec.  Supercedes G.723 and G.721. */
    NEXUS_AudioCodec_eG729,           /* G.729 CS-ACELP speech codec.  Often used in VOIP applications. */
    NEXUS_AudioCodec_eFlac,           /* Free Lossless Audio Codec (see http://flac.sourceforge.net) */ 
    NEXUS_AudioCodec_eMlp,            /* Meridian Lossless Packing (Used in Dolby TrueHD) */
    NEXUS_AudioCodec_eApe,            /* Monkey's Audio (see http://www.monkeysaudio.com/) */
    NEXUS_AudioCodec_eMax
} NEXUS_AudioCodec;

/***************************************************************************
Summary:
Audio Connector Format/Type
***************************************************************************/
typedef enum NEXUS_AudioConnectorType
{
    NEXUS_AudioConnectorType_eStereo,           /* Stereo (2.0) PCM output */
    NEXUS_AudioConnectorType_eMultichannel,     /* Multichannel (5.1/7.1) PCM output */
    NEXUS_AudioConnectorType_eCompressed,       /* Compressed IEC-61937 audio up to 48 kHz for SPDIF/HDMI applications.  */
    NEXUS_AudioConnectorType_eCompressed4x,     /* Compressed IEC-61937 audio for HDMI up to 192kHz.  Typically used for AC3+ and DTS-HD HRA audio formats. */
    NEXUS_AudioConnectorType_eCompressed16x,    /* Compressed IEC-61937 audio for HDMI HBR packets up to 768kHz.  Typically used for MAT/MLP (Dolby TrueHD) 
                                                   and DTS-HD MA audio formats.  Available in HDMI 1.3 and later. */
    NEXUS_AudioConnectorType_eMono,             /* Mono (1.0) PCM output.  This data type is not commonly used, and is generally only for voice conferencing applications. */
    NEXUS_AudioConnectorType_eMax
} NEXUS_AudioConnectorType;

/***************************************************************************
Summary:
Audio Multichannel Format
***************************************************************************/
typedef enum NEXUS_AudioMultichannelFormat
{
    NEXUS_AudioMultichannelFormat_eNone=0,
    NEXUS_AudioMultichannelFormat_eStereo=0,
    NEXUS_AudioMultichannelFormat_e5_1,
    NEXUS_AudioMultichannelFormat_e7_1,
    NEXUS_AudioMultichannelFormat_eMax
} NEXUS_AudioMultichannelFormat;

/***************************************************************************
Summary:
Audio Channels
***************************************************************************/
typedef enum NEXUS_AudioChannel
{
    NEXUS_AudioChannel_eLeft,           /* Left Front */
    NEXUS_AudioChannel_eRight,          /* Right Front */
    NEXUS_AudioChannel_eLeftSurround,   /* Left Surround */
    NEXUS_AudioChannel_eRightSurround,  /* Right Surround */
    NEXUS_AudioChannel_eCenter,         /* Center Channel */
    NEXUS_AudioChannel_eLfe,            /* Low Frequency Effects */
    NEXUS_AudioChannel_eLeftRear,       /* Left Rear (7.1 only) */
    NEXUS_AudioChannel_eRightRear,      /* Right Rear (7.1 only) */
    NEXUS_AudioChannel_eMax
} NEXUS_AudioChannel;

/***************************************************************************
Summary:
Audio Channel Pairs
***************************************************************************/
typedef enum NEXUS_AudioChannelPair
{
    NEXUS_AudioChannelPair_eLeftRight,          /* Front Left and Right */
    NEXUS_AudioChannelPair_eLeftRightSurround,  /* Surround Left and Right */
    NEXUS_AudioChannelPair_eCenterLfe,          /* Center Channel and LFE */
    NEXUS_AudioChannelPair_eLeftRightRear,      /* Rear Left and Right (7.1 only) */
    NEXUS_AudioChannelPair_eMax
} NEXUS_AudioChannelPair;

/***************************************************************************
Summary:
Audio Channel Mode
***************************************************************************/
typedef enum NEXUS_AudioVolumeType
{
    NEXUS_AudioVolumeType_eDecibel, /* Decibel scaling - Values are specified in 1/100 dB.  Currently supports attenuation only (negative values). */
    NEXUS_AudioVolumeType_eLinear,  /* Linear scaling - values are in a 4.23 format, 0x800000 = unity. */
    NEXUS_AudioVolumeType_eMax
} NEXUS_AudioVolumeType;

/***************************************************************************
Summary:
Audio Channel Mode
***************************************************************************/
typedef enum NEXUS_AudioChannelMode
{
    NEXUS_AudioChannelMode_eStereo,     /* Normal ordering, left-> left, right->right */
    NEXUS_AudioChannelMode_eLeft,       /* Duplicate left channel on left & right */
    NEXUS_AudioChannelMode_eRight,      /* Duplicate right channel on left & right */
    NEXUS_AudioChannelMode_eSwapped,    /* Reverse ordering, left->right, right->left */
    NEXUS_AudioChannelMode_eMax
} NEXUS_AudioChannelMode;

/***************************************************************************
Summary:
Audio Mono Channel Mode
***************************************************************************/
typedef enum NEXUS_AudioMonoChannelMode
{
    NEXUS_AudioMonoChannelMode_eLeft,
    NEXUS_AudioMonoChannelMode_eRight,
    NEXUS_AudioMonoChannelMode_eMix,
    NEXUS_AudioMonoChannelMode_eMax
} NEXUS_AudioMonoChannelMode;

/***************************************************************************
Summary:
    Audio  Mode
Description:
    Selects the audio channel mode 
***************************************************************************/
typedef enum NEXUS_AudioMode
{
    NEXUS_AudioMode_eAuto,     
    NEXUS_AudioMode_e1_0,
    NEXUS_AudioMode_e1_1,
    NEXUS_AudioMode_e2_0,
    NEXUS_AudioMode_e3_0,
    NEXUS_AudioMode_e2_1,
    NEXUS_AudioMode_e3_1,
    NEXUS_AudioMode_e2_2,
    NEXUS_AudioMode_e3_2,
    NEXUS_AudioMode_e3_4,
    NEXUS_AudioMode_eMax
} NEXUS_AudioMode;

/***************************************************************************
Summary:
Audio Volume Constants
***************************************************************************/
#define NEXUS_AUDIO_VOLUME_LINEAR_MAX 0x0fffffff
#define NEXUS_AUDIO_VOLUME_LINEAR_NORMAL 0x00800000
#define NEXUS_AUDIO_VOLUME_LINEAR_MIN 0

#define NEXUS_AUDIO_VOLUME_DB_MAX 0
#define NEXUS_AUDIO_VOLUME_DB_NORMAL 0
#define NEXUS_AUDIO_VOLUME_DB_MIN -9000

/***************************************************************************
Summary:
Audio Raw Channel Status
***************************************************************************/
typedef struct NEXUS_AudioRawChannelStatus
{
    uint32_t leftChannelHigh;   /* Bits 32..63 */
    uint32_t leftChannelLow;    /* Bits  0..31 */
    uint32_t rightChannelHigh;  /* Bits 32..63 */
    uint32_t rightChannelLow;   /* Bits  0..31 */
} NEXUS_AudioRawChannelStatus;

/**
Summary:
NEXUS_AudioInput is an abstract connector token for routing audio from a source. 

Description:
See Also:
NEXUS_AudioDecoder_GetConnector
NEXUS_HdmiInput_GetAudioConnector
NEXUS_SpdifInput_GetConnector
**/ 
typedef struct NEXUS_AudioInputObject *NEXUS_AudioInput;

/**
Summary:
NEXUS_AudioOutput is an abstract connector token for routing audio to a source. 

Description:
See Also:
NEXUS_AudioDac_GetConnector
NEXUS_SpdifOutput_GetConnector
NEXUS_HdmiOutput_GetAudioConnector
NEXUS_RfOutput_GetConnector
**/ 
typedef struct NEXUS_AudioOutputObject *NEXUS_AudioOutput;

/***************************************************************************
Summary:
RF Audio Encodings
***************************************************************************/
typedef enum NEXUS_RfAudioEncoding
{
    NEXUS_RfAudioEncoding_eBtsc,
    NEXUS_RfAudioEncoding_eMax
} NEXUS_RfAudioEncoding;

/***************************************************************************
Summary:
Echo Cancellation Algorithms
***************************************************************************/
typedef enum NEXUS_EchoCancellerAlgorithm
{
    NEXUS_EchoCancellerAlgorithm_eSpeex,     /* Speex Algorithm (http://www.speex.org) */
    NEXUS_EchoCancellerAlgorithm_eMax
} NEXUS_EchoCancellerAlgorithm;

/***************************************************************************
Summary:
Audio PLL Selection 

Description: 
Many audio outputs require a PLL to generate their clock.  By default, 
all outputs will use the PLL0, unless 1) a different "defaultPll" has been 
specified in the NEXUS_AudioModuleSettings.defaultPll during the call to 
NEXUS_AudioModule_Init(), or 2) the PLL used by an output has been 
specifed in the NEXUS_AudioOutputSettings for that output.  You 
will need to use multiple PLLs if you have outputs that require different 
sampling rates from one another.  For example, if you have a decoder 
outputting 48kHz on I2S and another decoder outputting 44.1kHz on SPDIF, 
you must use separate PLLs for the two outputs.  If both use the same 
sampling rate, they can share the PLL. There are also cases where sampling 
rates that are multiples of one another can share a PLL (e.g. Dolby Digital 
Plus passthrough with simultaneous AC3 conversion). 
 
All outputs connected to the same source must share the same PLL.  You can 
not use multiple PLLs with a single source. 
 
Currently, DACs and RFM do not require a PLL.  I2S and SPDIF do 
require one. HDMI, DummyOutput, and OutputCapture outputs may use either 
a PLL or an NCO clock source.
***************************************************************************/
typedef enum
{
    NEXUS_AudioOutputPll_e0,
    NEXUS_AudioOutputPll_e1,
    NEXUS_AudioOutputPll_e2,
    NEXUS_AudioOutputPll_e3,
    NEXUS_AudioOutputPll_eMax
} NEXUS_AudioOutputPll;

/***************************************************************************
Summary:
Audio NCO Selection 
 
Description: 
Some audio outputs (that do not require a PLL) can use an NCO (Mclkgen)
oscillator to generate their clock.  By default, audio outputs will never 
use an NCO, but an NCO can be explicitly assigned to one or more outputs. 
As with PLLs, you will need to use multiple NCOs if you have outputs that 
require different sampling rates from one another.
 
If any output uses an NCO, then all other outputs connected to the same 
source must share the same NCO.  You can not use multiple NCOs (or mix NCOs 
and PLLs) with a single source. 
 
Currently, NCOs should only be used for HDMI, DummyOutput, and 
OutputCapture outputs.  NCO clocks are not adequate for I2S, and SPDIF 
outputs, which require a PLL clock source. DACs and RFM outputs will 
always use the DAC's internal rate manager as their clock source. 

Note: Not all chips have an NCO (Mclkgen) clock source available.
***************************************************************************/
typedef enum
{
    NEXUS_AudioOutputNco_e0,
    NEXUS_AudioOutputNco_e1,
    NEXUS_AudioOutputNco_e2,
    NEXUS_AudioOutputNco_e3,
    NEXUS_AudioOutputNco_eMax
} NEXUS_AudioOutputNco;

/***************************************************************************
Summary:
Burst Settings for SPDIF/HDMI

Description:
Options to configure Pause, Null or no bursts when HDMI/SPDIF interfaces 
underflow.  Bursts are designed to keep a downstream receiver engaged 
so that when audio resumes no content is lost.
***************************************************************************/
typedef enum NEXUS_SpdifOutputBurstType
{
    NEXUS_SpdifOutputBurstType_eNull,       /* Insert NULL burst during underflow */
    NEXUS_SpdifOutputBurstType_ePause,      /* Insert PAUSE burst during underflow */
    NEXUS_SpdifOutputBurstType_eNone,       /* Don't insert any burst during underflow */
    NEXUS_SpdifOutputBurstType_eMax
}NEXUS_SpdifOutputBurstType;

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_AUDIO_TYPES_H */

