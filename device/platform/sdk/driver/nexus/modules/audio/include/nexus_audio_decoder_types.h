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
* $brcm_Workfile: nexus_audio_decoder_types.h $
* $brcm_Revision: PROD_HUMAX_URSR/1 $
* $brcm_Date: 12/18/13 3:16p $
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_audio_decoder_types.h $
* 
* PROD_HUMAX_URSR/1   12/18/13 3:16p scho
* SW7425-4944: changes to allow 96k output rate from the decoder
* 
* SW7425-4386-1/1   5/21/13 2:51p scho
* SW7425-4386-1: updates to mono encode APIs
* 
* 11   10/9/12 12:13a jgarrett
* SW7346-914: Adding audio ancillary data support
* 
* 10   8/21/12 5:22p erickson
* SW7231-958: separate structs/enums and functions for perl parser
* 
* 9   7/30/12 5:09p jgarrett
* SW7425-3490: Merge to main branch
* 
* SW7425-3490/1   7/13/12 5:41p jgarrett
* SW7425-3490: Adding initial frame preservation for MS-11 certification
* 
* 8   2/3/12 4:29p jgarrett
* SW7425-2268: Adding delay mode
* 
* 7   11/18/11 2:57p jgarrett
* SW7425-1519: Adding decoder mixing mode settings
* 
* 6   11/9/11 4:39p jgarrett
* SW7125-1040: Adding AC3+ Dependent Frame Map
* 
* 5   10/11/11 8:24a erickson
* SW7420-1992: move types to nexus_audio_decoder_types.h
* 
* 4   7/1/11 10:54a jgarrett
* SW7405-5072: Adding numWatchdogs and raveIndex status
* 
* 3   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 2   5/20/11 11:32a jgarrett
* SW7405-5335: Adding numBytesDecoded to NEXUS_AudioDecoderStatus
* 
* 1   2/15/11 2:22p erickson
* SW7420-1135: refactor into nexus_audio_decoder_types.h
*
***************************************************************************/
#ifndef NEXUS_AUDIO_DECODER_TYPES__
#define NEXUS_AUDIO_DECODER_TYPES__

#include "nexus_pid_channel.h"  /* NEXUS_PidChannelHandle */
#include "nexus_stc_channel.h"   /* NEXUS_StcChannelHandle */

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for audio decoder
**/
typedef struct NEXUS_AudioDecoder *NEXUS_AudioDecoderHandle;

/***************************************************************************
Summary:
Audio decoder mixing modes
***************************************************************************/
typedef enum NEXUS_AudioDecoderMixingMode
{
    NEXUS_AudioDecoderMixingMode_eDescription,  /* In this mixing mode, channels will be mixed using
                                                   volume coefficients contained in the secondary audio
                                                   program.  This is typically used in audio services for
                                                   the visually impaired (as described in DTG D-Book section 4.5),
                                                   where a voice over track can be mixed with the primary 
                                                   audio track. */
    NEXUS_AudioDecoderMixingMode_eSoundEffects, /* In this mixing mode, channels will be mixed using static
                                                   coefficients in the mixer as opposed to stream-based 
                                                   coefficients. */
    NEXUS_AudioDecoderMixingMode_eMax
} NEXUS_AudioDecoderMixingMode;

/***************************************************************************
Summary:
Audio decoder delay modes
***************************************************************************/
typedef enum NEXUS_AudioDecoderDelayMode
{
    NEXUS_AudioDecoderDelayMode_eAuto,          /* Default.  In this mode, when the decoder receives input
                                                   from a PidChannel it will use the fixed path delay mode.
                                                   When decoding from an external input such as HDMI, it will
                                                   use the low delay mode.  This is default for backward compatibility. */
    NEXUS_AudioDecoderDelayMode_eFixed,         /* Fixed delay.  In this mode, the decoder will operate with a
                                                   fixed output delay irrespective of the input format. */
    NEXUS_AudioDecoderDelayMode_eLow,           /* Lowest delay.  In this mode, the decoder will operate with a minimal
                                                   output delay based on the input format.  This can be useful for very 
                                                   low delay applications such as receiving from HDMI input, game mode
                                                   processing, or voice conferencing. */
    NEXUS_AudioDecoderDelayMode_eMax
} NEXUS_AudioDecoderDelayMode;

/***************************************************************************
Summary:
Audio decoder program settings
***************************************************************************/
typedef struct NEXUS_AudioDecoderStartSettings
{
    NEXUS_PidChannelHandle pidChannel;  /* Input from transport */
    NEXUS_AudioCodec codec;             /* Format of data coming from the pid channel */
    NEXUS_StcChannelHandle stcChannel;  /* Connection to clock for TSM and lipsync with VideoDecoder. If NULL, decode in vsync mode. */
    NEXUS_AudioInput input;             /* If you are decoding from a digital source such as HdmiInput or SpdifInput,
                                           pass the connector here.  If this value is set, pidChannel must be NULL.  */
    bool targetSyncEnabled;             /* If true, normal frame sync operation will be used (default).  This flag can be set to false
                                           for certification applications that require the last frame of the input buffer to be consumed
                                           without the audio decoder finding the start of the successive frame.  */
    bool nonRealTime;                   /* Normal operation for decoding is real time, if this is set to 'true' then decoding is used as a
                                           source for non-realtime transcode operations */
    bool secondaryDecoder;              /* If this decoder is a secondary, set true here.  A secondary decoder can be mixed with a
                                           primary decoder on the same StcChannel timebase.  Default=false. */
    bool forceCompleteFirstFrame;       /* If true, the first frame will always be entirely rendered to the output and not partially truncated
                                           for TSM computations.  This should be disabled for normal operation, but may be required for some
                                           bit-exact certification testing that requires all data to be rendered even with TSM enabled. */
    NEXUS_AudioDecoderMixingMode mixingMode;    /* Mixing mode to be used when multiple decoders are mixed. */
    NEXUS_AudioDecoderDelayMode delayMode;      /* Delay mode to be used while decoding.  Default is NEXUS_AudioDecoderDelayMode_eAuto. */
    unsigned maxOutputRate;             /* Max output rate we support from the decoder - 
                                           Valid values are 48000 or 96000. Units are Hz. Default is 48000.
                                           Content above the maxOutputRate will be downsampled within its rate family.
                                           For example - if the content is 88200Hz and maxOutputRate = 48000, decoder
                                           will output 44100Hz for that content.  Similarly, if the content is 
                                           176,400Hz and maxOutputRate = 96000, decoder will output 88200Hz. 
                                           
                                           Note, setting maxOutputRate = 96000 requires NEXUS_AudioModuleSettings->maxPcmSampleRate to 
                                           also be increased to 96000 at Init time. */
} NEXUS_AudioDecoderStartSettings;

/***************************************************************************
Summary:
MPEG Channel Mode values
***************************************************************************/
typedef enum NEXUS_AudioMpegChannelMode
{
    NEXUS_AudioMpegChannelMode_eStereo = 0,         /* Stereo */
    NEXUS_AudioMpegChannelMode_eIntensityStereo,    /* Intensity_stereo for layer I and II */
    NEXUS_AudioMpegChannelMode_eDualChannel,        /* Dual Channel */
    NEXUS_AudioMpegChannelMode_eSingleChannel,      /* Single Channel */
    NEXUS_AudioMpegChannelMode_eMax
} NEXUS_AudioMpegChannelMode;

/***************************************************************************
Summary:
MPEG layer values
***************************************************************************/
typedef enum NEXUS_AudioMpegLayer
{
    NEXUS_AudioMpegLayer_e3=1,   /* MPEG Layer III */
    NEXUS_AudioMpegLayer_e2,     /* MPEG Layer II */
    NEXUS_AudioMpegLayer_e1,     /* MPEG Layer I */
    NEXUS_AudioMpegLayer_eMax
} NEXUS_AudioMpegLayer;

/***************************************************************************
Summary:
MPEG Emphasis values
***************************************************************************/
typedef enum NEXUS_AudioMpegEmphasis
{
    NEXUS_AudioMpegEmphasis_eNone = 0,      /* No deemphasis */
    NEXUS_AudioMpegEmphasis_eEmph50_15ms,   /* Emph_50_15ms */
    NEXUS_AudioMpegEmphasis_eReserved,      /* Reserved */
    NEXUS_AudioMpegEmphasis_eCCITT_J17,     /* CCITT_J17 */
    NEXUS_AudioMpegEmphasis_eMax
} NEXUS_AudioMpegEmphasis;

/***************************************************************************
Summary:
AC3 ACMOD values
***************************************************************************/
typedef enum NEXUS_AudioAc3Acmod
{
    NEXUS_AudioAc3Acmod_eTwoMono_1_ch1_ch2,
    NEXUS_AudioAc3Acmod_eOneCenter_1_0_C,
    NEXUS_AudioAc3Acmod_eTwoChannel_2_0_L_R,
    NEXUS_AudioAc3Acmod_eThreeChannel_3_0_L_C_R,
    NEXUS_AudioAc3Acmod_eThreeChannel_2_1_L_R_S,
    NEXUS_AudioAc3Acmod_eFourChannel_3_1_L_C_R_S,
    NEXUS_AudioAc3Acmod_eFourChannel_2_2_L_R_SL_SR,
    NEXUS_AudioAc3Acmod_eFiveChannel_3_2_L_C_R_SL_SR,
    NEXUS_AudioAc3Acmod_eMax
} NEXUS_AudioAc3Acmod;

/***************************************************************************
Summary:
AC3 Center Mix Level (cmixlev) 
 
Description: 
Expressed as dB attenuation 
***************************************************************************/
typedef enum NEXUS_AudioAc3CenterMixLevel
{
    NEXUS_AudioAc3CenterMixLevel_e3,
    NEXUS_AudioAc3CenterMixLevel_e4_5,
    NEXUS_AudioAc3CenterMixLevel_e6,
    NEXUS_AudioAc3CenterMixLevel_eReserved
} NEXUS_AudioAc3CenterMixLevel;

/***************************************************************************
Summary:
AC3 Surround Mix Level (surmixlev) 
 
Description: 
Expressed as dB attenuation 
***************************************************************************/
typedef enum NEXUS_AudioAc3SurroundMixLevel
{
    NEXUS_AudioAc3SurroundMixLevel_e3,
    NEXUS_AudioAc3SurroundMixLevel_e6,
    NEXUS_AudioAc3SurroundMixLevel_e0,
    NEXUS_AudioAc3SurroundMixLevel_eReserved
} NEXUS_AudioAc3SurroundMixLevel;

/***************************************************************************
Summary:
AC3 Center Mix Level (dsurmod)
***************************************************************************/
typedef enum NEXUS_AudioAc3DolbySurround
{
    NEXUS_AudioAc3DolbySurround_eNotIndicated,
    NEXUS_AudioAc3DolbySurround_eNotEncoded,
    NEXUS_AudioAc3DolbySurround_eEncoded,
    NEXUS_AudioAc3DolbySurround_eReserved
} NEXUS_AudioAc3DolbySurround;

/***************************************************************************
Summary:
AC3 Dependent Frame Channel Map Mode
***************************************************************************/
typedef enum NEXUS_AudioAc3DependentFrameChannelMap
{
    NEXUS_AudioAc3DependentFrameChannelMap_eReserved,
    NEXUS_AudioAc3DependentFrameChannelMap_eC,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_Cvh,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Ts,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Ts,                
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Cvh,            
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_Lc_Rc,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Lw_Rw,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Lvh_Rvh,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Lsd_rsd,            
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Lrs_Rrs,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Lc_Rc,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Lw_Rw,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Lvh_Rvh,            
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Lsg_Rsd,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Lrs_Rrs,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Ts_Cvh,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Cs,            
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Cs,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_R_l_r_Cs_Ts,           
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Cs_Cvh,
    NEXUS_AudioAc3DependentFrameChannelMap_eL_C_R_l_r_Cs_Ts,
    NEXUS_AudioAc3DependentFrameChannelMap_eMax    
}NEXUS_AudioAc3DependentFrameChannelMap;

/***************************************************************************
Summary:
AAC ACMOD values
***************************************************************************/
typedef enum NEXUS_AudioAacAcmod
{
    NEXUS_AudioAacAcmod_eTwoMono_1_ch1_ch2,
    NEXUS_AudioAacAcmod_eOneCenter_1_0_C,
    NEXUS_AudioAacAcmod_eTwoChannel_2_0_L_R,
    NEXUS_AudioAacAcmod_eThreeChannel_3_0_L_C_R,
    NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S,
    NEXUS_AudioAacAcmod_eFourChannel_3_1_L_C_R_S,
    NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR,
    NEXUS_AudioAacAcmod_eFiveChannel_3_2_L_C_R_SL_SR,
    NEXUS_AudioAacAcmod_eMax
} NEXUS_AudioAacAcmod;

/***************************************************************************
Summary:
AAC Profile values
***************************************************************************/
typedef enum NEXUS_AudioAacProfile
{
    NEXUS_AudioAacProfile_eMain,
    NEXUS_AudioAacProfile_eLowComplexity,
    NEXUS_AudioAacProfile_eScalableSamplingRate,
    NEXUS_AudioAacProfile_eMax
} NEXUS_AudioAacProfile;

/***************************************************************************
Summary:
DTS Audio Coding Mode (AMODE) values
***************************************************************************/
typedef enum NEXUS_AudioDtsAmode
{
    NEXUS_AudioDtsAmode_eOneCh_A,
    NEXUS_AudioDtsAmode_eTwoCh_A_B,
    NEXUS_AudioDtsAmode_eTwoCh_L_R,
    NEXUS_AudioDtsAmode_eTwoCh_LpR_LmR,
    NEXUS_AudioDtsAmode_eTwoCh_LT_RT,
    NEXUS_AudioDtsAmode_eThreeCh_C_L_R,
    NEXUS_AudioDtsAmode_eThreeCh_L_R_S,
    NEXUS_AudioDtsAmode_eFourCh_C_L_R_S,
    NEXUS_AudioDtsAmode_eFourCh_L_R_SL_SR,
    NEXUS_AudioDtsAmode_eFiveCh_C_L_R_SL_SR,
    NEXUS_AudioDtsAmode_eSixCh_CL_CR_L_R_SL_SR,
    NEXUS_AudioDtsAmode_eSixCh_C_L_R_LR_RR_OV,
    NEXUS_AudioDtsAmode_eSixCh_CF_CR_LF_RF_Lr_Rr,
    NEXUS_AudioDtsAmode_eSevenCh_CL_C_CR_L_R_SL_SR,
    NEXUS_AudioDtsAmode_eEightCh_CL_CR_L_R_SL1_SL2_SR1_SR2,
    NEXUS_AudioDtsAmode_eEightCh_CL_C_CR_L_R_SL_S_SR,
    NEXUS_AudioDtsAmode_eUserDefined,
    NEXUS_AudioDtsAmode_eMax
}NEXUS_AudioDtsAmode;

/***************************************************************************
Summary:
DTS stream copy history
***************************************************************************/
typedef enum NEXUS_AudioDtsCopyHistory
{
    NEXUS_AudioDtsCopyHistory_eCopyProhibited,
    NEXUS_AudioDtsCopyHistory_eFirstGeneration,
    NEXUS_AudioDtsCopyHistory_eSecondGeneration,
    NEXUS_AudioDtsCopyHistory_eOriginal,
    NEXUS_AudioDtsCopyHistory_eMax
} NEXUS_AudioDtsCopyHistory;

/***************************************************************************
Summary:
DTS extension types
***************************************************************************/
typedef enum NEXUS_AudioDtsExtension
{
    NEXUS_AudioDtsExtension_eXCh,       /* Channel Extension supporting 5.1 plus center surround channel */
    NEXUS_AudioDtsExtension_eXXCh,      /* Channel Extension supporting additional channels beyond 5.1 */
    NEXUS_AudioDtsExtension_eX96k,      /* Frequency Extension */
    NEXUS_AudioDtsExtension_eReserved,
    NEXUS_AudioDtsExtension_eMax
} NEXUS_AudioDtsExtension;

/***************************************************************************
Summary:
WMA Pro Stereo Downmix Modes
***************************************************************************/
typedef enum NEXUS_AudioWmaProStereoMode
{
    NEXUS_AudioWmaProStereoMode_eAuto,
    NEXUS_AudioWmaProStereoMode_eLtRt,
    NEXUS_AudioWmaProStereoMode_eLoRo,
    NEXUS_AudioWmaProStereoMode_eMax
} NEXUS_AudioWmaProStereoMode;

/***************************************************************************
Summary:
WMA Pro Audio Coding Modes (ACMOD)
***************************************************************************/
typedef enum NEXUS_AudioWmaProAcmod
{
    NEXUS_AudioWmaProAcmod_eUndefined,
    NEXUS_AudioWmaProAcmod_e1_0_C,
    NEXUS_AudioWmaProAcmod_e2_0_LR,
    NEXUS_AudioWmaProAcmod_e3_0_LCR,
    NEXUS_AudioWmaProAcmod_e2_1_LRS,
    NEXUS_AudioWmaProAcmod_e3_1_LCRS,
    NEXUS_AudioWmaProAcmod_e2_2_LRLsRs,
    NEXUS_AudioWmaProAcmod_e3_2_LCRLsRs,
    NEXUS_AudioWmaProAcmod_eMax
} NEXUS_AudioWmaProAcmod;

/***************************************************************************
Summary:
AMR Bitrate values
***************************************************************************/
typedef enum NEXUS_AudioAmrBitrate
{
    NEXUS_AudioAmrBitrate_e4_75kbps,
    NEXUS_AudioAmrBitrate_e5_15kbps,
    NEXUS_AudioAmrBitrate_e5_90kbps,
    NEXUS_AudioAmrBitrate_e6_70kbps,
    NEXUS_AudioAmrBitrate_e7_40kbps,
    NEXUS_AudioAmrBitrate_e7_95kbps,
    NEXUS_AudioAmrBitrate_e10_20kbps,
    NEXUS_AudioAmrBitrate_e12_20kbps,
    NEXUS_AudioAmrBitrate_eSilence,
    NEXUS_AudioAmrBitrate_eMax
} NEXUS_AudioAmrBitrate;

/***************************************************************************
Summary:
DRA Audio Coding Mode (ACMOD) values
***************************************************************************/
typedef enum NEXUS_AudioDraAcmod
{
    NEXUS_AudioDraAcmod_e1_0_C,
    NEXUS_AudioDraAcmod_e2_0_LR,
    NEXUS_AudioDraAcmod_e2_1_LRS,
    NEXUS_AudioDraAcmod_e2_2_LRLrRr,
    NEXUS_AudioDraAcmod_e3_2_LRLrRrC,
    NEXUS_AudioDraAcmod_e3_3_LRLrRrCrC,
    NEXUS_AudioDraAcmod_e5_2_LRLrRrLsRsC,
    NEXUS_AudioDraAcmod_e5_3_LRLrRrLsRsCrC,
    NEXUS_AudioDraAcmod_eMax
} NEXUS_AudioDraAcmod;

/***************************************************************************
Summary:
DRA Stereo Modes
***************************************************************************/
typedef enum NEXUS_AudioDraStereoMode
{
    NEXUS_AudioDraStereoMode_eLoRo,
    NEXUS_AudioDraStereoMode_eLtRt,
    NEXUS_AudioDraStereoMode_eMax
} NEXUS_AudioDraStereoMode;

/***************************************************************************
Summary:
Audio decoder status
***************************************************************************/
typedef struct NEXUS_AudioDecoderStatus
{
    bool    started; /* true if decoder was started */

    bool tsm;        /* AudioDecoder is in TSM (time stamp managed) mode. TSM may be not applicable (e.g. ES streams), may be disabled directly
                        by the user (setting stcChannel = NULL) or indirectly by other API's (e.g. trick modes, ASTM, etc.) */

    bool locked;     /* True if the decoder has found a valid frame sync */

    NEXUS_AudioCodec codec;     /* Audio stream format */
    unsigned sampleRate;        /* In Hz */
    unsigned timebase;          /* STC index. This is derived from NEXUS_AudioDecoderStartSettings.stcChannel. The name is unfortunate because this is not a timebase. */

    unsigned framesDecoded;     /* Total number of frames decoded since start was called. */
    unsigned frameErrors;       /* Total number of frame errors since start was called. */
    unsigned dummyFrames;       /* Total number of dummy (zero/null) frames filled by the decoder since start was called. */
    unsigned numFifoOverflows;  /* Total number of FIFO overflows since start was called. */
    unsigned numFifoUnderflows; /* Total number of FIFO underflows since start was called. */
    uint64_t numBytesDecoded;   /* Total number of ES bytes decoded since last start or flush.  Applicable to transport (NEXUS_PidChannel) input only. */
    unsigned numWatchdogs;      /* Total number of watchdog events for the device since NEXUS_AudioModule_Init.  The count is global for the audio module and not per-decoder. */

    union
    {
        struct
        {
            unsigned bitStreamId;
            NEXUS_AudioAc3Acmod acmod;
            unsigned frameSizeCode;
            unsigned bitrate;                                                  /* bitrate in Kbps */
            bool lfe;
            bool copyright;                                                    /* true if the copyright bit is asserted */
            NEXUS_AudioAc3DependentFrameChannelMap dependentFrameChannelMap;   /* Dependent Frame channel map for 7.1 streams */
         } ac3;  /* Applies to both AC3 and AC3+ */
        struct
        {
            NEXUS_AudioMpegChannelMode channelMode;
            NEXUS_AudioMpegLayer layer;
            NEXUS_AudioMpegEmphasis emphasis;
            unsigned bitrate;           /* bitrate in Kbps */
            unsigned numSamples;    /* Number of decoded samples */
            bool original;
            bool copyright;  /* true if the copyright bit is asserted */
        } mpeg; /* Valid for MPEG and MP3 */
        struct
        {
            NEXUS_AudioAacAcmod acmod;
            NEXUS_AudioAacProfile profile;
            unsigned bitrate;           /* bitrate in Kbps */
            unsigned numSamples;        /* Number of decoded samples */
            bool lfe;
            bool pseudoSurround;
            bool drc;
            bool stereoMatrix;          /* True if a stereo downmix matrix is present */
            unsigned matrixIndex;       /* A two-bit value indicating the coefficient to be used in a stereo downmix */
            unsigned numLfeChannels;
            unsigned numBackChannels;
            unsigned numSideChannels;
            unsigned numFrontChannels;
        } aac;  /* Applies to AAC, AAC-HE (SBR), and Dolby Pulse */
        struct
        {
            NEXUS_AudioDtsAmode amode;              /* DTS audio channel arrangement */
            unsigned pcmResolution;                 /* Quantization resolution of source PCM samples (in bits) */
            NEXUS_AudioDtsCopyHistory copyHistory;  /* Copy history of the stream */
            unsigned lfeInterpolation;              /* LFE interpolation factor */
            NEXUS_AudioDtsExtension extensionDescriptor; /* Extension descriptor code from the stream header. Valid if extensionPresent is true */
            unsigned bitRate;                       /* Bitrate in kbps.  0 for Open, Variable, or Lossless coded streams (bitRateCode=29..31) */
            unsigned version;                       /* Version Number */
            uint8_t bitRateCode;                    /* Raw bitrate value coded in the stream header */
            bool esFormat;                          /* If true, the left and right surround channels are mastered in ES format */
            bool lfe;                               /* True if LFE is present */
            bool extensionPresent;                  /* True if extended audio coding data is present in the stream */
            bool crc;                               /* True if a CRC is present */
            bool hdcdFormat;                        /* True if the source content is mastered in HDCD format */
            bool drc;                               /* True if DRC (dynamic range compression) is present */
            bool downmixCoefficients;               /* True if downmix coefficients are present in the stream */
            bool neo;                               /* True if DTS NEO is enabled */
            unsigned frameSize;                     /* Total bytes in the current frame including primary and extension data */
            unsigned numChannels;                   /* Total number of channels (primary+LFE) supported by the decoder depending on user configuration */
            unsigned pcmFrameSize;                  /* Total size of the current PCM frame in bytes (valid if frameSize ranges between 95..16383) */
            unsigned numPcmBlocks;                  /* Number of coded PCM blocks */
        } dts;  /* Applies to DTS, DTS-HD */
        struct
        {
            unsigned bitRate;           /* In kbps */
            bool original;
            bool copyright;
            bool crc;
            bool stereo;                /* True if stereo data is present, false if mono data is present */
            unsigned version;           /* Version - 1 or 2 */
        } wma;
        struct
        {
            unsigned bitRate;           /* In kbps */
            bool original;
            bool copyright;
            bool crc;
            bool lfe;
            unsigned version;           /* Version - 1 or 2 is WMA Standard, 3 is WMA Pro */
            NEXUS_AudioWmaProStereoMode stereoMode;
            NEXUS_AudioWmaProAcmod acmod;
        } wmaPro;
        struct
        {
            unsigned numChannels;
        } pcmWav;
        struct
        {
            NEXUS_AudioAmrBitrate bitRate;
        } amr;
        struct
        {
            unsigned frameSize;             /* In bytes */
            unsigned numBlocks;             /* Number of short window mdct blocks in the frame.  There are 128 samples per block. */
            NEXUS_AudioDraAcmod acmod;      /* Channel coding mode */
            bool lfe;
            NEXUS_AudioDraStereoMode stereoMode;
        } dra;
        struct
        {
            bool stereo;        /* True if stereo, false if mono */
            unsigned frameSize; /* Frame size in bytes */
        } cook;
    } codecStatus;

    unsigned fifoDepth; /* depth in bytes of the compressed buffer */
    unsigned fifoSize; /* size in bytes of the compressed buffer */

    uint32_t pts;              /* current PTS of the audio decoder */
    NEXUS_PtsType ptsType;     /* current PTS type of the codec */
    int32_t ptsStcDifference;  /* current PTS-STC difference including lipsync adjustments */
    unsigned ptsErrorCount;    /* counter for number of PTS errors since start of decode */
    unsigned queuedFrames;     /* Number of compressed frames in the queue */

    uint8_t wordLength;    /* For SPDIF/HDMI inputs, this is the bits per sample of PCM data */
    uint8_t maxWordLength; /* For SPDIF/HDMI inputs, this is the max bits per sample of PCM data */

    uint8_t algorithmProfileId;     /* Profile ID of the current decode algorithm */
    uint8_t algorithmVersionId[4];  /* Version ID of the current decode algorithm */
    uint8_t firmwareVersionId;      /* Firmware Version ID of the current decode algorithm */
} NEXUS_AudioDecoderStatus;

/**
Summary:
Set a collection of video decoder trick mode parameters.

Description:
Trick mode state is separated from other state so that higher-level modules can handle trick modes apart from direct app control.

See Also:
NEXUS_AudioDecoder_GetTrickState
NEXUS_AudioDecoder_SetTrickState
**/
typedef struct NEXUS_AudioDecoderTrickState
{
    bool forceStopped;              /*
                                       If true, decoder will not be permitted to run, otherwise it's allowed.
                                       Settings this to true causes audio decoder to drop all data that is
                                        coming to audio decoder. This is usefull in video only trickmodes (e.g. fast forward or rewind).
                                     */
    unsigned rate;                  /* Speed control based on units of NEXUS_NORMAL_DECODE_RATE:
                                        NEXUS_NORMAL_DECODE_RATE*2 = 2x fast-forward.
                                        NEXUS_NORMAL_DECODE_RATE/2 = 2x slow-motion.
                                        0 = pause.
                                       NEXUS_NORMAL_DECODE_RATE = normal play.
                                    */
    bool muted;                     /* Mute for certain trick operations (e.g. slow motion, advance) */
    bool tsmEnabled;                /* This allows app to turn off TSM as required. */
    bool stcTrickEnabled;           /* Set this true if you are doing STC trick modes. This allows AudioDecoder to properly handle PTS error interrupts. */
} NEXUS_AudioDecoderTrickState;

/***************************************************************************
Summary:
    Dual Mono Mode
Description:
    Selects the output mode for a dual-mono stream.  These settings have no effect for other streams.
***************************************************************************/
typedef enum NEXUS_AudioDecoderDualMonoMode
{
    NEXUS_AudioDecoderDualMonoMode_eStereo,
    NEXUS_AudioDecoderDualMonoMode_eLeft,
    NEXUS_AudioDecoderDualMonoMode_eRight,
    NEXUS_AudioDecoderDualMonoMode_eMix,
    NEXUS_AudioDecoderDualMonoMode_eMax
} NEXUS_AudioDecoderDualMonoMode;

/***************************************************************************
Summary:
    Audio Output Mode
Description:
    Selects the output mode from the audio decoder.
***************************************************************************/
#define NEXUS_AudioDecoderOutputMode        NEXUS_AudioMode
#define NEXUS_AudioDecoderOutputMode_eAuto  NEXUS_AudioMode_eAuto /* Automatic, based on stereo/multichannel output configuration */
#define NEXUS_AudioDecoderOutputMode_e1_0   NEXUS_AudioMode_e1_0
#define NEXUS_AudioDecoderOutputMode_e1_1   NEXUS_AudioMode_e1_1
#define NEXUS_AudioDecoderOutputMode_e2_0   NEXUS_AudioMode_e2_0
#define NEXUS_AudioDecoderOutputMode_e3_0   NEXUS_AudioMode_e3_0
#define NEXUS_AudioDecoderOutputMode_e2_1   NEXUS_AudioMode_e2_1
#define NEXUS_AudioDecoderOutputMode_e3_1   NEXUS_AudioMode_e3_1
#define NEXUS_AudioDecoderOutputMode_e2_2   NEXUS_AudioMode_e2_2
#define NEXUS_AudioDecoderOutputMode_e3_2   NEXUS_AudioMode_e3_2
#define NEXUS_AudioDecoderOutputMode_e3_4   NEXUS_AudioMode_e3_4
#define NEXUS_AudioDecoderOutputMode_eMax   NEXUS_AudioMode_eMax

/***************************************************************************
Summary:
    Audio LFE Output Mode
Description:
    Selects the LFE output mode from the audio decoder.
***************************************************************************/
typedef enum NEXUS_AudioDecoderOutputLfeMode
{
    NEXUS_AudioDecoderOutputLfeMode_eAuto,     /* Automatic, based on stereo/multichannel output configuration */
    NEXUS_AudioDecoderOutputLfeMode_eOff,
    NEXUS_AudioDecoderOutputLfeMode_eOn,
    NEXUS_AudioDecoderOutputLfeMode_eMax
} NEXUS_AudioDecoderOutputLfeMode;

/***************************************************************************
Summary:
Audio decoder settings
***************************************************************************/
typedef struct NEXUS_AudioDecoderSettings
{
    NEXUS_AudioDecoderDualMonoMode dualMonoMode;    /* Dual-Mono output mode.  Only applies if the input stream is dual-mono. */

    NEXUS_AudioDecoderOutputMode outputMode;        /* Decoder output mode.  In general, eAuto is correct, but this may be overridden
                                                       for testing and/or certification purposes. */
    NEXUS_AudioDecoderOutputLfeMode outputLfeMode;  /* LFE output flag.  In general, eAuto is correct, but this may be overridden
                                                       for testing and/or certification purposes. */

    unsigned fifoThreshold; /* If set, this can limit the maximum FIFO depth to a value <= the value specified
                               in NEXUS_AudioDecoderOpenSettings.fifoSize.  If 0, the entire FIFO can fill.  */

    int ptsOffset;  /* Add an offset used in the decoder's TSM equation. Measured in PTS units (45KHz for MPEG2TS, 27MHz for DSS).
                       This is added to any PTS offset also set by SyncChannel. */

    unsigned discardThreshold;  /* If non-zero, specifies the TSM discard threshold in ms.  If zero (default), values will be selected based upon
                                   live vs. playback mode of operation.  If PTS-STC is greater than this threshold, the frame will be dropped.  */
    unsigned gaThreshold;       /* If non-zero, specifies the TSM Gross Adjustment threshold in ms.  If zero (default), values will be selected based upon
                                   live vs. playback mode of operation and codec/container formats.  If PTS-STC is greater than this threshold but less than
                                   the discardThreshold, partial frame fills will be used to achieve lipsync better than a frame boundary.  */
    bool wideGaThreshold;       /* If true, set the GA threshold wider, in order to tolerate up to +/- 1 AAU of PTS error.  Default = false.
                                   Only used if gaThreshold = 0. */

    bool loudnessEquivalenceEnabled;        /* If true, loudness equivalence will be enabled based on the mode specified
                                               in NEXUS_AudioModuleSettings.loudnessMode.  Ignored if NEXUS_AudioModuleSettings.loudnessMode 
                                               is set to NEXUS_AudioLoudnessEquivalenceMode_eNone.  Default is true. */

    bool ancillaryDataEnabled;                      /* If true, ancillary data parsing will be enabled.  Default is false. */

    NEXUS_CallbackDesc sourceChanged;       /* This callback fires whenever specifics such as sample rate, layer, or bitrate change.
                                               It also fires when decoding HDMI/SPDIF input if the input codec changes.  */
    NEXUS_CallbackDesc lockChanged;         /* This callback fires whenever NEXUS_AudioDecoderStatus.locked changes */
    NEXUS_CallbackDesc ptsError;            /* Fires on any PTS discontinuity. This callback is used for application notification. No response is required for TSM. */
    NEXUS_CallbackDesc firstPts;            /* Fires when the decoder receives the first PTS after Start.
                                               This event is also called "request STC" because the decoder needs StcChannel to set the STC in Auto mode.
                                               This callback is used for application notification. No response is required for TSM. */
    NEXUS_CallbackDesc fifoOverflow;        /* This callback fires when the input FIFO overflows */
    NEXUS_CallbackDesc fifoUnderflow;       /* This callback fires when the input FIFO underflows */
    NEXUS_CallbackDesc streamStatusAvailable;   /* This callback fires when the decoder first acquires the detailed codec status information such as channel coding, etc. */
    NEXUS_CallbackDesc ancillaryData;       /* This callack fires when new ancillary data is available. */

    int32_t volumeMatrix[NEXUS_AudioChannel_eMax][NEXUS_AudioChannel_eMax]; /* Volume matrix.  This allows customization of channel volume
                                                                               output.  Default is [Left][Left] = Normal, [Right][Right] = Normal,
                                                                               [Center][Center] = Normal, etc.  All other volumes are zero by default.
                                                                               This will affect stereo and multichannel PCM outputs only,  not compressed. */
    bool muted;     /* This will affect stereo and multichannel PCM outputs only, not compressed. */

    NEXUS_AudioDecoderHandle descriptionDecoder; /* Optional.  If specified, the channel handle provided will be used to decode audio descriptor
                                                    data (as described in DTG D-Book section 4.5).  Outputs attached to the current channel will
                                                    receive only the primary channel audio.  To mix the description program with the primary
                                                    program, both decoders should be connected to an AudioMixer object.  Outputs connected to the
                                                    mixer will receive audio containing the mixed result with the appropriate panning and fading
                                                    applied to the streams. The descriptionDecoder must be started, stopped, and have trickmodes
                                                    controlled by the application in addition to the current channel.  This value can not be
                                                    changed while either decoder is running. */
    unsigned descriptionRampTime;               /* Audio descriptor volume ramp time in ms.  Default is 1000ms.  The value for the decoder handling the
                                                   description will be used. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    bool liveOverride;	  /* If true, force the AudioDecoder into playback mode, even if PidChannel is not playback. But bandHold -> false */
#endif		
} NEXUS_AudioDecoderSettings;

/***************************************************************************
Summary:
Dynamic Range Compression Mode

Description:
This corresponds to the eCompMode field of BRAP_DSPCHN_Ac3*ConfigParams.  The
original is an enum with four possibilities: Custom A, Custom D, Rf, and Line.
All four values can cause changes in the output volume level.  The most
noticeable change is the difference between Line and Rf.  With the compression
mode set to Rf, input PCM samples are scaled up by 12 dB.  In Line mode, input
PCM samples are not scaled up at all.  The two most commonly used modes are line
and Rf, in that order.
***************************************************************************/
typedef enum NEXUS_AudioDecoderDolbyDrcMode
{
    NEXUS_AudioDecoderDolbyDrcMode_eLine,
    NEXUS_AudioDecoderDolbyDrcMode_eRf,
    NEXUS_AudioDecoderDolbyDrcMode_eCustomA,
    NEXUS_AudioDecoderDolbyDrcMode_eCustomD,
    NEXUS_AudioDecoderDolbyDrcMode_eOff,
    NEXUS_AudioDecoderDolbyDrcMode_eMax
} NEXUS_AudioDecoderDolbyDrcMode;

/***************************************************************************
Summary:
Stereo Downmix Mode

Description:
This enum represents the formulas available for doing stereo downmixing.  It
only applies when the downmix mode is stereo.  The enum corresponds to the
eStereoMode field of BRAP_DSPCHN_Ac3*ConfigParams.  It is an enum with three
possibilities: Automatic, LtRt, and LoRo.  LoRo is the standard stereo downmix
formula.  LtRt is a Dolby Surround compatible stereo downmix formula.  Automatic
means that which stereo downmix formula chosen is determined by parameters
in the stream (as specified by Annex D of the AC-3 specification).
***************************************************************************/
typedef enum NEXUS_AudioDecoderDolbyStereoDownmixMode
{
    NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic,
    NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible,
    NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard,
    NEXUS_AudioDecoderDolbyStereoDownmixMode_eMax
} NEXUS_AudioDecoderDolbyStereoDownmixMode;

/***************************************************************************
Summary:
Dolby audio decoder settings

Description:
These settings control the parameters involved in the decode of Dolby-based
streams.  They only apply when the audio type is Dolby Digital (AC3) or Dolby
Digital Plus (AC3Plus). The variable "cut" determines the percentage cut of
louder sounds from the normal value. The variable "boost" determines the
percentage boost of quiter sounds from the normal value.
***************************************************************************/
typedef struct NEXUS_AudioDecoderDolbySettings
{
    NEXUS_AudioDecoderDolbyDrcMode drcMode;
    NEXUS_AudioDecoderDolbyDrcMode drcModeDownmix;  /* DRC mode for stereo downmixed data only.  Applicable to MS11 Licensed decoder only. */
    NEXUS_AudioDecoderDolbyStereoDownmixMode stereoDownmixMode;

    uint16_t cut;                       /* Cut factor */
    uint16_t boost;                     /* Boost factor */
    uint16_t cutDownmix;                /* Cut factor for stereo downmixed data only.  Applicable to MS11 licensed decoder only. */
    uint16_t boostDownmix;              /* Boost factor for stereo downmixed data only.  Applicable to MS11 licensed decoder only. */
    uint16_t scale;                     /* PCM Scale factor */
    uint16_t scaleDownmix;              /* PCM Scale factor for stereo downmixed data only.  Applicable to MS11 licensed decoder only. */

    bool dialogNormalization;           /* Use dialog normalization values from the stream.  This should be true for normal operation. */
    unsigned dialogNormalizationValue; /* User-specified dialog normalization value.  Used only if dialogNormalization = false. Specified in dB, 0..31 */

    unsigned substreamId;               /* Applicable only to MS10/MS11 licensed decoders */
} NEXUS_AudioDecoderDolbySettings;

/***************************************************************************
Summary:
Downmix type for the AAC decoder
***************************************************************************/
typedef enum NEXUS_AudioDecoderAacDownmixMode
{
    NEXUS_AudioDecoderAacDownmixMode_eMatrix,   /* Not supported for Dolby Pulse (MS10) */
    NEXUS_AudioDecoderAacDownmixMode_eArib,     /* Not supported for Dolby Pulse (MS10) */
    NEXUS_AudioDecoderAacDownmixMode_eLtRt,
    NEXUS_AudioDecoderAacDownmixMode_eLoRo,
    NEXUS_AudioDecoderAacDownmixMode_eMax
} NEXUS_AudioDecoderAacDownmixMode;

/***************************************************************************
Summary:
Dolby Pulse audio decoder dynamic range compression mode
***************************************************************************/
typedef enum NEXUS_AudioDecoderDolbyPulseDrcMode
{
    NEXUS_AudioDecoderDolbyPulseDrcMode_eLine,
    NEXUS_AudioDecoderDolbyPulseDrcMode_eRf,
    NEXUS_AudioDecoderDolbyPulseDrcMode_eMax
}NEXUS_AudioDecoderDolbyPulseDrcMode;

/***************************************************************************
Summary:
AAC audio decoder settings

Description:
These settings control the parameters involved in the decode of AAC and
AAC-Plus streams.  They only apply when the audio type is AAC or AAC-Plus
***************************************************************************/
typedef struct NEXUS_AudioDecoderAacSettings
{
    uint16_t cut;   /* Dynamic range compression cut scale factor (in %).
                       Input value range = 0% to 100%. 100% corresponds to
                       a value of unity. Any value above 100% is considered
                       as unity */

    uint16_t boost; /* Dynamic range compression boost scale factor (in %).
                       Input value range = 0% to 100%. 100% corresponds to
                       a value of unity. Any value above 100% is considered
                       as unity */

    uint16_t drcTargetLevel; /* Dynamic range compression target level.
                                Input value range = 0 to 127. */

    uint16_t drcDefaultLevel; /* Default Dynamic range compression target level.
                                 Input value range = 0 to 127. */

    bool  decode96Khz;   /* Enable decoding of 96Kz audio.
                            Not supported for Dolby Pulse (MS10). */

    bool  mpegConformanceMode;  /* Enable MPEG Conformance mode.  This should
                                   only be set for Dolby Pulse (MS10) 
                                   certification testing */

    NEXUS_AudioDecoderAacDownmixMode downmixMode;   /* Downmix mode */
    NEXUS_AudioDecoderDolbyPulseDrcMode drcMode;    /* DRC mode.
                                                       Only supported for Dolby Pulse (MS10). */
} NEXUS_AudioDecoderAacSettings;

/***************************************************************************
Summary:
WMA pro audio decoder settings

Description:
These settings control the parameters involved in the decode of WMA-pro streams
***************************************************************************/
typedef struct NEXUS_AudioDecoderWmaProSettings
{
    bool dynamicRangeControlValid;
    struct {
        unsigned peakReference;
        unsigned peakTarget;
        unsigned averageReference;
        unsigned averageTarget;
    } dynamicRangeControl;

}NEXUS_AudioDecoderWmaProSettings;


/***************************************************************************
Summary:
    This enumeration defines configuration parameter values for
    DTS stereo mode. This eum is effective only when stream is
    Annex-D compliant and
    BRAP_DEC_AudioParams.eOutputMode = BRAP_OutputMode_eStereo.
    This is also used for DTS-HD.
***************************************************************************/
typedef enum NEXUS_AudioDecoderDtsStereoDownmixMode
{
    NEXUS_AudioDecoderDtsDownmixMode_eAuto,
    NEXUS_AudioDecoderDtsDownmixMode_eLtRt,
    NEXUS_AudioDecoderDtsDownmixMode_eLoRo,
    NEXUS_AudioDecoderDtsDownmixMode_eMax
} NEXUS_AudioDecoderDtsStereoDownmixMode;

/***************************************************************************
Summary:
DTS audio decoder settings

Description:
These settings control the parameters involved in the decode of DTS and
DTS-HD streams.  They only apply when the audio type is DTS or DTS-HD
***************************************************************************/
typedef struct NEXUS_AudioDecoderDtsSettings
{
    bool littleEndian;      /* Set to true if the data is little-endian as opposed to a bytestream (e.g. .wav content) */
    bool mixLfeToPrimary;   /* Mix LFE to primary while downmixing, when Lfe output is disabled */
    bool enableDrc;         /* Dynamic Range Compression is enabled if true, disabled if false */
    uint16_t cut;           /* Dynamic range compression cut scale factor (in %).
                               Input value range = 0% to 100%. 100% corresponds to
                               a value of unity. Any value above 100% is considered
                               as unity */

    uint16_t boost;         /* Dynamic range compression boost scale factor (in %).
                               Input value range = 0% to 100%. 100% corresponds to
                               a value of unity. Any value above 100% is considered
                               as unity */
    NEXUS_AudioDecoderDtsStereoDownmixMode stereoDownmixMode;
} NEXUS_AudioDecoderDtsSettings;


/***************************************************************************
Summary:
ADPCM audio decoder settings

Description:
These settings control the parameters involved in the decode of ADPCM.
They only apply when the audio type is ADPCM
***************************************************************************/
typedef struct NEXUS_AudioDecoderAdpcmSettings
{
    bool enableGain;      /* True to apply the gain factor below */
    unsigned gainFactor;  /* Similar to Volume Gain in 8.24 format.
                             E.g. 0x00800000: unity
                                  0x016A09E6: +3dB */
} NEXUS_AudioDecoderAdpcmSettings;


/***************************************************************************
Summary:
    Codec-specific decoder settings
***************************************************************************/
typedef struct NEXUS_AudioDecoderCodecSettings
{
    NEXUS_AudioCodec codec;
    union
    {
        NEXUS_AudioDecoderDolbySettings ac3;
        NEXUS_AudioDecoderDolbySettings ac3Plus;
        NEXUS_AudioDecoderAacSettings aac;
        NEXUS_AudioDecoderAacSettings aacPlus;
        NEXUS_AudioDecoderWmaProSettings wmaPro;
        NEXUS_AudioDecoderDtsSettings dts;
        NEXUS_AudioDecoderAdpcmSettings adpcm;
    } codecSettings;
} NEXUS_AudioDecoderCodecSettings;

/***************************************************************************
Summary:
Ancillary Data Header
***************************************************************************/
typedef struct NEXUS_AudioAncillaryDataHeader
{
    unsigned blockSize;         /* Block size including this header and any padding */
    unsigned payloadSize;       /* Payload size in bytes */
    unsigned payloadSizeBits;   /* Payload size in bits */
    unsigned frameNumber;       /* Frame number */
} NEXUS_AudioAncillaryDataHeader;

#ifdef __cplusplus
}
#endif

#endif
