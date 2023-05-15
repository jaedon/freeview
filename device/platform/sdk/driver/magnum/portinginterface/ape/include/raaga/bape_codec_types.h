/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_codec_types.h $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 6/6/12 3:28p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_codec_types.h $
 * 
 * Hydra_Software_Devel/14   6/6/12 3:28p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/14   6/6/12 2:46p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/14   6/6/12 2:43p mpeteri
 * SW7425-2889: Changes for DTS-CD support
 * 
 * Hydra_Software_Devel/SW7425-2889/1   6/5/12 4:30p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/13   12/21/11 12:40p jgarrett
 * SW7425-1018: Documenting units for AAC DRC Target
 * 
 * Hydra_Software_Devel/12   12/20/11 5:35p jgarrett
 * SW7425-1671: Adding Dependent Frame Channel Map for DDP
 * 
 * Hydra_Software_Devel/11   12/2/11 5:20p jgarrett
 * SW7425-1893: Adding MPEG conformance mode for Dolby Pulse
 * 
 * Hydra_Software_Devel/10   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/9   5/25/11 5:15p jgarrett
 * SW7425-408: Removing unused dsurmod member
 * 
 * Hydra_Software_Devel/8   5/14/11 12:23p jgarrett
 * SW7425-408: Adding DDRE APIs
 * 
 * Hydra_Software_Devel/7   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/6   3/8/11 4:23p jgarrett
 * SW7422-146: Adding pulse downmix modes
 * 
 * Hydra_Software_Devel/5   2/15/11 4:07p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/4   2/7/11 11:30a jgarrett
 * SW7422-146: Implementing DDP -> AC3 conversion and status for
 * MPEG/AAC/AC3
 * 
 * Hydra_Software_Devel/3   2/1/11 5:01p jgarrett
 * SW7422-146: Adding AAC and Generic Passthrough
 * 
 * Hydra_Software_Devel/2   1/27/11 5:47p jgarrett
 * SW7422-146: Adding support for multichannel and ac3 user parameters
 * 
 * Hydra_Software_Devel/1   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#ifndef BAPE_CODEC_TYPES_H_
#define BAPE_CODEC_TYPES_H_

/***************************************************************************
Summary:
AC3 DRC (Dynamic Range Compression) Modes
***************************************************************************/
typedef enum BAPE_Ac3DrcMode
{
    BAPE_Ac3DrcMode_eLine,
    BAPE_Ac3DrcMode_eRf,
    BAPE_Ac3DrcMode_eCustomA,
    BAPE_Ac3DrcMode_eCustomD,
    BAPE_Ac3DrcMode_eDisabled,
    BAPE_Ac3DrcMode_eMax
} BAPE_Ac3DrcMode;

/***************************************************************************
Summary:
AC3 Stereo Downmix Modes
***************************************************************************/
typedef enum BAPE_Ac3StereoMode
{
    BAPE_Ac3StereoMode_eAuto,   /* Automatically select based on bitstream */
    BAPE_Ac3StereoMode_eLtRt,   /* Dolby ProLogic-II Compatible */
    BAPE_Ac3StereoMode_eLoRo,   /* Stereo Only */
    BAPE_Ac3StereoMode_eMax
} BAPE_Ac3StereoMode;

/***************************************************************************
Summary:
AC3 Codec Settings
***************************************************************************/
typedef struct BAPE_Ac3Settings
{
    BAPE_Ac3DrcMode drcMode;        /* DRC (Dynamic Range Compression) Mode */
    BAPE_Ac3DrcMode drcModeDownmix; /* DRC (Dynamic Range Compression) Mode for stereo downmixed data*/

    BAPE_Ac3StereoMode stereoMode;  /* Stereo Downmix Mode */

    uint16_t drcScaleHi;            /* In %, ranges from 0..100 */
    uint16_t drcScaleLow;           /* In %, ranges from 0..100 */
    uint16_t drcScaleHiDownmix;     /* In %, ranges from 0..100 */
    uint16_t drcScaleLowDownmix;    /* In %, ranges from 0..100 */

    uint16_t scale;                 /* PCM Scale factor */
    uint16_t scaleDownmix;          /* PCM Scale factor for stereo downmixed data only.  Applicable to MS11 licensed decoder only. */

    bool dialogNormalization;           /* Use dialog normalization values from the stream.  This should be true for normal operation. */
    unsigned dialogNormalizationValue;  /* User-specified dialog normalization value.  Used only if dialogNormalization = false. Specified in dB, 0..31 */

    unsigned substreamId;               /* Applicable only to MS10/MS11 licensed decoders */
} BAPE_Ac3Settings;

/***************************************************************************
Summary:
AAC Stereo Downmix Modes
***************************************************************************/
typedef enum BAPE_AacStereoMode
{
    BAPE_AacStereoMode_eMatrix,     /* Default (Not applicable with Dolby Pulse) */
    BAPE_AacStereoMode_eArib,       /* ARIB Compatible (Not applicable with Dolby Pulse) */
    BAPE_AacStereoMode_eLtRt,       /* LtRt (Dolby Pulse Only) */
    BAPE_AacStereoMode_eLoRo,       /* LoRo (Dolby Pulse Only) */
    BAPE_AacStereoMode_eMax
} BAPE_AacStereoMode;

/***************************************************************************
Summary:
Dolby Pulse Dynamic Range Compression Mode
***************************************************************************/
typedef enum BAPE_DolbyPulseDrcMode
{
    BAPE_DolbyPulseDrcMode_eLine,
    BAPE_DolbyPulseDrcMode_eRf,
    BAPE_DolbyPulseDrcMode_eMax
} BAPE_DolbyPulseDrcMode;

/***************************************************************************
Summary:
AAC Codec Settings
***************************************************************************/
typedef struct BAPE_AacSettings
{
    uint16_t drcScaleHi;            /* In %, ranges from 0..100 */
    uint16_t drcScaleLow;           /* In %, ranges from 0..100 */
    uint16_t drcTargetLevel;        /* DRC (Dynamic Range Compression) Target Level.  Ranges from 0..127  in -0.25dB units (e.g. program 80 for -20dB) */
    
    BAPE_AacStereoMode downmixMode; /* AAC Downmix Mode */

    bool enableGainFactor;      /* If true, the gain factor below is applied (Used in Dolby Pulse only) */
    uint32_t gainFactor;        /* Gain factor in 8.24 format - 0x00800000=unity, 0x00B504F3=-3dB (Used in Dolby Pulse only) */

    uint16_t drcDefaultLevel;   /* Dynamic range compression default level.  Ranges from 0..127 (Used in Dolby Pulse only) */

    BAPE_DolbyPulseDrcMode drcMode; /* Dynamic Range Compression Mode (Used in Dolby Pulse only) */

    bool enableDownmixCoefficients; /* If true, downmix coefficients below are applied (Not applicable with Dolby Pulse) */

    bool mpegConformanceMode;       /* If true, enable MPEG conformance testing for Dolby Pulse certification. */

    uint16_t downmixCoefficients[6][6]; /* User defined downmix (or karaoke) coefficients. 
                                           Valid only when enableDownmixCoefficients is true (Not Applicable with Dolby Pulse) */
} BAPE_AacSettings;

/***************************************************************************
Summary:
WMA Pro Dynamic Range Control modes
***************************************************************************/
typedef enum BAPE_WmaProDrcMode
{
    BAPE_WmaProDrcMode_eDisabled,   /* DRC is disabled */
    BAPE_WmaProDrcMode_eHigh,       /* No scaling is applied.  Content is played in it's original form. */
    BAPE_WmaProDrcMode_eMedium,     /* drc_frame_scale_factor is applied along with a scale factor to make 
                                       the output rms of the content rms_amplitude_target_dB, and the peak 
                                       of the content be peak_amplitude_target_med_dB. */
    BAPE_WmaProDrcMode_eLow,        /* drc_frame_scale_factor is applied along with a scale factor to make 
                                       the output rms of the content rms_amplitude_target_dB, and the peak 
                                       of the content be peak_amplitude_target_low_dB.*/
    BAPE_WmaProDrcMode_eMax

} BAPE_WmaProDrcMode;

/***************************************************************************
Summary:
WMA Pro Stereo Downmix Modes
***************************************************************************/
typedef enum BAPE_WmaProStereoMode
{
    BAPE_WmaProStereoMode_eAuto,
    BAPE_WmaProStereoMode_eLtRt,
    BAPE_WmaProStereoMode_eLoRo,
    BAPE_WmaProStereoMode_eMax
} BAPE_WmaProStereoMode;

/***************************************************************************
Summary:
WMA Pro Codec Settings
***************************************************************************/
typedef struct BAPE_WmaProSettings
{
    BAPE_WmaProStereoMode stereoMode;
    BAPE_WmaProDrcMode drcMode;
    unsigned rmsAmplitudeReference;
    unsigned peakAmplitudeReference;    
    unsigned desiredRms;        /* desired rmsDb for normalization */
    unsigned desiredPeak;       /* desired peakDb for normalization */ 
} BAPE_WmaProSettings;

/***************************************************************************
Summary:
DTS Dynamic Range Compression Mode
***************************************************************************/
typedef enum BAPE_DtsDrcMode
{
    BAPE_DtsDrcMode_eDisabled,
    BAPE_DtsDrcMode_eEnabled,
    BAPE_DtsDrcMode_eMax
} BAPE_DtsDrcMode;

/***************************************************************************
Summary:
DTS Stereo Downmix Modes
***************************************************************************/
typedef enum BAPE_DtsStereoMode
{
    BAPE_DtsStereoMode_eAuto,   /* Automatically select based on bitstream */
    BAPE_DtsStereoMode_eLtRt,
    BAPE_DtsStereoMode_eLoRo,
    BAPE_DtsStereoMode_eMax
} BAPE_DtsStereoMode;

/***************************************************************************
Summary:
DTS Codec Settings
***************************************************************************/
typedef struct BAPE_DtsSettings
{
    bool littleEndian;              /* Set to true if the data is little-endian as opposed to a bytestream (e.g. .wav content) */
    BAPE_DtsDrcMode drcMode;        /* Dynamic Range Compression Mode */
    uint16_t drcScaleHi;            /* In %, ranges from 0..100 */
    uint16_t drcScaleLow;           /* In %, ranges from 0..100 */

    BAPE_DtsStereoMode stereoMode;  /* Stereo Downmix Mode */
    bool mixLfeToPrimary;           /* Mix LFE to primary while downmixing, when Lfe output is disabled */
} BAPE_DtsSettings;

/***************************************************************************
Summary:
ADPCM Codec Settings
***************************************************************************/
typedef struct BAPE_AdpcmSettings
{
    struct
    {
        bool enabled;       /* If true, gain processing is enabled */
        uint32_t factor;    /* Ranges from BAPE_VOLUME_MAX to BAPE_VOLUME_MIN.  Default is BAPE_VOLUME_NORMAL. */
    } gain;
} BAPE_AdpcmSettings;

/***************************************************************************
Summary:
MPEG Channel Modes
***************************************************************************/
typedef enum BAPE_MpegChannelMode
{
    BAPE_MpegChannelMode_eStereo,
    BAPE_MpegChannelMode_eIntensityStereo,
    BAPE_MpegChannelMode_eDualChannel,
    BAPE_MpegChannelMode_eSingleChannel,
    BAPE_MpegChannelMode_eMax
} BAPE_MpegChannelMode;

/***************************************************************************
Summary:
MPEG Emphasis Modes
***************************************************************************/
typedef enum BAPE_MpegEmphasisMode
{
    BAPE_MpegEmphasisMode_eNone,        /* No emphasis */
    BAPE_MpegEmphasisMode_e50_15ms,     /* 50/15 ms */
    BAPE_MpegEmphasisMode_eReserved,    /* Reserved */
    BAPE_MpegEmphasisMode_eCcit_J17,    /* CCIT J.17 */
    BAPE_MpegEmphasisMode_eMax
} BAPE_MpegEmphasisMode;

/***************************************************************************
Summary:
MPEG Codec Status
***************************************************************************/
typedef struct BAPE_MpegStatus
{
    bool original;                                  /* true if the stream is marked as original */
    bool copyright;                                 /* true if copyright is indicated in the stream */
    bool crcPresent;                                /* true if CRC's are present */
    uint32_t                sampleRateCode;         /* Sample Rate Code extracted from stream */
    unsigned                bitRate;                /* Bitrate in kbps.  0 if not specified. */
    unsigned                layer;                  /* 1 for Layer 1, 2 for Layer 2, etc. */
    BAPE_ChannelMode        channelMode;            /* Codec-independent channel mode */
    BAPE_MpegChannelMode    mpegChannelMode;        /* Stream Channel Mode */
    BAPE_MpegEmphasisMode   emphasisMode;           /* Emphasis mode extracted from the stream. */
} BAPE_MpegStatus;

/***************************************************************************
Summary:
AC3 Bitstream Mode
***************************************************************************/
typedef enum BAPE_Ac3Bsmod
{
    BAPE_Ac3Bsmod_eMainAudioCM,         /* Main Audio Service - Complete Main */
    BAPE_Ac3Bsmod_eMainAudioME,         /* Main Audio Service - Music and Effects */
    BAPE_Ac3Bsmod_eAssociatedVI,        /* Associated Service - Visually Impaired */
    BAPE_Ac3Bsmod_eAssociatedHI,        /* Associated Service - Hearing Impaired */
    BAPE_Ac3Bsmod_eAssociatedD,         /* Associated Service - Dialog */
    BAPE_Ac3Bsmod_eAssociatedC,         /* Associated Service - Commentary */
    BAPE_Ac3Bsmod_eAssociatedE,         /* Associated Service - Emergency */
    BAPE_Ac3Bsmod_eAcmodDepenedent,     /* Stream Identified by ACMOD.  If ACMOD = 001 (BAPE_ChannelMode_e1_0) is associated voice-over.  Anything else is Karaoke. */
    BAPE_Ac3Bsmod_eMax
} BAPE_Ac3Bsmod;

/***************************************************************************
Summary:
AC3 Channel Coding Mode
***************************************************************************/
typedef enum BAPE_Ac3Acmod
{
    BAPE_Ac3Acmod_e1_1,     /* 1+1 Dual Mono */
    BAPE_Ac3Acmod_e1_0,     /* Center Mono */
    BAPE_Ac3Acmod_e2_0,     /* Left Right Stereo */
    BAPE_Ac3Acmod_e3_0,     /* Left, Center, Right */
    BAPE_Ac3Acmod_e2_1,     /* Left, Right, Surround */
    BAPE_Ac3Acmod_e3_1,     /* Left, Center, Right, Surround */
    BAPE_Ac3Acmod_e2_2,     /* Left, Right, LeftSurround, RightSurround */
    BAPE_Ac3Acmod_e3_2,     /* Left, Center, Right, LeftSurround, RightSurround */
    BAPE_Ac3Acmod_eMax
} BAPE_Ac3Acmod;

/***************************************************************************
Summary:
AC3 Center Mix Level (cmixlev) 
 
Description: 
Expressed as dB attenuation 
***************************************************************************/
typedef enum BAPE_Ac3CenterMixLevel
{
    BAPE_Ac3CenterMixLevel_e3,
    BAPE_Ac3CenterMixLevel_e4_5,
    BAPE_Ac3CenterMixLevel_e6,
    BAPE_Ac3CenterMixLevel_eReserved
} BAPE_Ac3CenterMixLevel;

/***************************************************************************
Summary:
AC3 Surround Mix Level (surmixlev) 
 
Description: 
Expressed as dB attenuation 
***************************************************************************/
typedef enum BAPE_Ac3SurroundMixLevel
{
    BAPE_Ac3SurroundMixLevel_e3,
    BAPE_Ac3SurroundMixLevel_e6,
    BAPE_Ac3SurroundMixLevel_e0,
    BAPE_Ac3SurroundMixLevel_eReserved
} BAPE_Ac3SurroundMixLevel;

/***************************************************************************
Summary:
AC3 Dolby Surround Mode (dsurmod)
***************************************************************************/
typedef enum BAPE_Ac3DolbySurround
{
    BAPE_Ac3DolbySurround_eNotIndicated,
    BAPE_Ac3DolbySurround_eNotEncoded,
    BAPE_Ac3DolbySurround_eEncoded,
    BAPE_Ac3DolbySurround_eReserved
} BAPE_Ac3DolbySurround;

/***************************************************************************
Summary:
AC3 Dependent Frame Channel Map Mode
***************************************************************************/
typedef enum BAPE_Ac3DependentFrameChannelMap
{
    BAPE_Ac3DependentFrameChannelMap_eReserved,
    BAPE_Ac3DependentFrameChannelMap_eC,
    BAPE_Ac3DependentFrameChannelMap_eL_R,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_Cvh,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Ts,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Ts,                
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Cvh,            
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_Lc_Rc,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Lw_Rw,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Lvh_Rvh,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Lsd_rsd,            
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Lrs_Rrs,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Lc_Rc,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Lw_Rw,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Lvh_Rvh,            
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Lsg_Rsd,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Lrs_Rrs,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Ts_Cvh,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Cs,            
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Cs,
    BAPE_Ac3DependentFrameChannelMap_eL_R_l_r_Cs_Ts,           
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Cs_Cvh,
    BAPE_Ac3DependentFrameChannelMap_eL_C_R_l_r_Cs_Ts,
    BAPE_Ac3DependentFrameChannelMap_eMax    
} BAPE_Ac3DependentFrameChannelMap;

/***************************************************************************
Summary:
AC3 Codec Status
***************************************************************************/
typedef struct BAPE_Ac3Status
{
    uint8_t                     sampleRateCode;         /* fscod - Sample Rate Code */
    uint8_t                     frameSizeCode;          /* frmsizcod - Frame Size Code.  Can be used in conjunction with fscod/sampleRateCode 
                                                           to determine frame size */
    uint8_t                     bitstreamId;            /* bsid - Version of the standard the stream is compliant with */
    BAPE_Ac3Bsmod               bsmod;                  /* bsmod - indicates type of service conveyed according to the standard */
    BAPE_Ac3Acmod               acmod;                  /* acmod - Indicates channel layout */
    BAPE_ChannelMode            channelMode;            /* Codec-independent ACMOD value */
    BAPE_Ac3CenterMixLevel      centerMixLevel;         /* cmixlev - Center Mix Level */
    BAPE_Ac3SurroundMixLevel    surroundMixLevel;       /* surmixlev - Surround Mix Level */
    BAPE_Ac3DolbySurround       dolbySurround;          /* dsurmod - AC3 Dolby Surround Mode */
    bool                        lfe;                    /* lfeon - If true, the LFE channel exists in the stream. */
    bool                        copyright;              /* copyrightb - If true, the stream is marked as copyrighted */
    bool                        original;               /* origbs - If true, the stream is marked as original */
    unsigned                    bitrate;                /* Bitrate in kbps */
    BAPE_Ac3DependentFrameChannelMap dependentFrameChannelMap;  /* Dependent Frame channel map for 7.1 streams */
} BAPE_Ac3Status;

/***************************************************************************
Summary:
AAC Profiles
***************************************************************************/
typedef enum BAPE_AacProfile
{
    BAPE_AacProfile_eMain,
    BAPE_AacProfile_eLowComplexity,
    BAPE_AacProfile_eScalableSamplingRate,
    BAPE_AacProfile_eMax
} BAPE_AacProfile;

/***************************************************************************
Summary:
AAC Codec Status
***************************************************************************/
typedef struct BAPE_AacStatus
{
    unsigned originalSampleRate;                    /* Stream sample rate */
    BAPE_AacProfile profile;                        /* AAC Profile */
    BAPE_ChannelMode channelMode;                   /* Codec-independent ACMOD value */
    unsigned bitRate;                               /* Bitrate in kbps */
    bool lfe;                                       /* True if the LFE channel exists in the steram */
    bool pseudoSurround;                            /* True if pseudo-surround decoding is enabled */
    bool drc;                                       /* True if DRC information is present in the stream */
    bool stereoMatrix;                              /* True if a stereo downmix matrix is present */
    unsigned matrixIndex;                           /* A two-bit value indicating the coefficient to be used in a stereo downmix */
    unsigned numLfeChannels;
    unsigned numBackChannels;
    unsigned numSideChannels;
    unsigned numFrontChannels;
} BAPE_AacStatus;

/***************************************************************************
Summary:
WMA Codec Status
***************************************************************************/
typedef struct BAPE_WmaStatus
{
    unsigned bitRate;               /* Bitrate in kbps */
    bool     copyright;             /* If true, the stream is marked as copyrighted */
    bool     original;              /* If true, the stream is marked as original */
    bool     crc;                   /* True if CRC is present in the stream */
    BAPE_ChannelMode channelMode;   /* Codec-independent ACMOD value */
    unsigned          version;      /* Version of the stream (Currently 1 or 2) */
} BAPE_WmaStatus;

/***************************************************************************
Summary:
WMA Pro Codec Status
***************************************************************************/
typedef struct BAPE_WmaProStatus
{
    unsigned bitRate;           /* In kbps */
    bool original;
    bool copyright;
    bool crc;
    bool lfe;
    unsigned version;           /* Version - 1 or 2 is WMA Standard, 3 is WMA Pro */
    BAPE_WmaProStereoMode stereoMode;
    BAPE_ChannelMode channelMode;           /* Codec-independent ACMOD value */
} BAPE_WmaProStatus;

/***************************************************************************
Summary:
DTS Audio Coding Mode (AMODE) values
***************************************************************************/
typedef enum BAPE_DtsAmode
{
    BAPE_DtsAmode_eOneCh_A,
    BAPE_DtsAmode_eTwoCh_A_B,
    BAPE_DtsAmode_eTwoCh_L_R,
    BAPE_DtsAmode_eTwoCh_LpR_LmR,
    BAPE_DtsAmode_eTwoCh_LT_RT,
    BAPE_DtsAmode_eThreeCh_C_L_R,
    BAPE_DtsAmode_eThreeCh_L_R_S,
    BAPE_DtsAmode_eFourCh_C_L_R_S,
    BAPE_DtsAmode_eFourCh_L_R_SL_SR,
    BAPE_DtsAmode_eFiveCh_C_L_R_SL_SR,
    BAPE_DtsAmode_eSixCh_CL_CR_L_R_SL_SR,
    BAPE_DtsAmode_eSixCh_C_L_R_LR_RR_OV,
    BAPE_DtsAmode_eSixCh_CF_CR_LF_RF_Lr_Rr,
    BAPE_DtsAmode_eSevenCh_CL_C_CR_L_R_SL_SR,
    BAPE_DtsAmode_eEightCh_CL_CR_L_R_SL1_SL2_SR1_SR2,
    BAPE_DtsAmode_eEightCh_CL_C_CR_L_R_SL_S_SR,   
    BAPE_DtsAmode_eUserDefined,
    BAPE_DtsAmode_eMax
}BAPE_DtsAmode;

/***************************************************************************
Summary:
DTS stream copy history
***************************************************************************/
typedef enum BAPE_DtsCopyHistory
{
    BAPE_DtsCopyHistory_eCopyProhibited,
    BAPE_DtsCopyHistory_eFirstGeneration,
    BAPE_DtsCopyHistory_eSecondGeneration,
    BAPE_DtsCopyHistory_eOriginal,
    BAPE_DtsCopyHistory_eMax
} BAPE_DtsCopyHistory;

/***************************************************************************
Summary:
DTS extension types
***************************************************************************/
typedef enum BAPE_DtsExtension
{
    BAPE_DtsExtension_eXCh,       /* Channel Extension supporting 5.1 plus center surround channel */
    BAPE_DtsExtension_eXXCh,      /* Channel Extension supporting additional channels beyond 5.1 */
    BAPE_DtsExtension_eX96k,      /* Frequency Extension */
    BAPE_DtsExtension_eReserved,
    BAPE_DtsExtension_eMax
} BAPE_DtsExtension;

/***************************************************************************
Summary:
DTS Codec Status
***************************************************************************/
typedef struct BAPE_DtsStatus
{
    BAPE_DtsAmode amode;                    /* DTS audio channel arrangement */
    unsigned pcmResolution;                 /* Quantization resolution of source PCM samples (in bits) */
    BAPE_DtsCopyHistory copyHistory;        /* Copy history of the stream */
    BAPE_DtsExtension extensionDescriptor;  /* Extension descriptor code from the stream header. Valid if extensionPresent is true */
    unsigned bitRate;                       /* Bitrate in kbps.  0 for Open, Variable, or Lossless coded streams (bitRateCode=29..31) */
    unsigned version;                       /* Version Number */
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
    BAPE_ChannelMode channelMode;           /* Codec-independent ACMOD value */
} BAPE_DtsStatus;

/***************************************************************************
Summary:
PCMWAV Codec Status
***************************************************************************/
typedef struct BAPE_PcmWavStatus
{
    unsigned numChannels;
} BAPE_PcmWavStatus;

/***************************************************************************
Summary:
AMR Bitrate values
***************************************************************************/
typedef enum BAPE_AmrBitrate
{
    BAPE_AmrBitrate_e4_75kbps,
    BAPE_AmrBitrate_e5_15kbps,
    BAPE_AmrBitrate_e5_90kbps,
    BAPE_AmrBitrate_e6_70kbps,
    BAPE_AmrBitrate_e7_40kbps,
    BAPE_AmrBitrate_e7_95kbps,
    BAPE_AmrBitrate_e10_20kbps,
    BAPE_AmrBitrate_e12_20kbps,
    BAPE_AmrBitrate_eSilence,
    BAPE_AmrBitrate_eMax
} BAPE_AmrBitrate;

/***************************************************************************
Summary:
AMR Codec Status
***************************************************************************/
typedef struct BAPE_AmrStatus
{
    BAPE_AmrBitrate bitRate;
} BAPE_AmrStatus;

/***************************************************************************
Summary:
DRA Audio Coding Mode (ACMOD) values
***************************************************************************/
typedef enum BAPE_DraAcmod
{
    BAPE_DraAcmod_e1_0_C,
    BAPE_DraAcmod_e2_0_LR,
    BAPE_DraAcmod_e2_1_LRS,
    BAPE_DraAcmod_e2_2_LRLrRr,
    BAPE_DraAcmod_e3_2_LRLrRrC,
    BAPE_DraAcmod_e3_3_LRLrRrCrC,
    BAPE_DraAcmod_e5_2_LRLrRrLsRsC,
    BAPE_DraAcmod_e5_3_LRLrRrLsRsCrC,
    BAPE_DraAcmod_eMax
} BAPE_DraAcmod;

/***************************************************************************
Summary:
DRA Stereo Modes
***************************************************************************/
typedef enum BAPE_DraStereoMode
{
    BAPE_DraStereoMode_eLoRo,
    BAPE_DraStereoMode_eLtRt,
    BAPE_DraStereoMode_eMax
} BAPE_DraStereoMode; 

/***************************************************************************
Summary:
DRA Codec Status
***************************************************************************/
typedef struct BAPE_DraStatus
{
    unsigned frameSize;             /* In bytes */
    unsigned numBlocks;             /* Number of short window mdct blocks in the frame.  There are 128 samples per block. */
    BAPE_DraAcmod acmod;            /* Channel coding mode */
    bool lfe;
    BAPE_ChannelMode channelMode;   /* Codec-independent ACMOD value */
    BAPE_DraStereoMode stereoMode;            
} BAPE_DraStatus;

/***************************************************************************
Summary:
Cook Codec Status
***************************************************************************/
typedef struct BAPE_CookStatus
{
    bool stereo;        /* True if stereo, false if mono */
    unsigned frameSize; /* Frame size in bytes */
} BAPE_CookStatus;

#endif /* #ifndef BAPE_CODEC_TYPES_H_ */

