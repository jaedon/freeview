/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_types.h $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-4386/1 $
 * $brcm_Date: 5/16/13 5:52p $
 *
 * Module Description: APE Types
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/include/raaga/bape_types.h $
 * 
 * Hydra_Software_Devel/SW7425-4386/1   5/16/13 5:52p mpeteri
 * SW7425-4386: revisions per NEXUS API CR
 * 
 * Hydra_Software_Devel/18   9/27/12 10:17a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:48p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/17   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:47p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/16   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/15   12/20/11 5:24p jgarrett
 * SW7425-1018: Updating comments describing loudness equivalence modes
 * 
 * Hydra_Software_Devel/14   12/19/11 4:27p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * Hydra_Software_Devel/13   8/24/11 11:53a jgarrett
 * SW7425-724: Adding RF Audio Encoder
 * 
 * Hydra_Software_Devel/12   8/23/11 10:17a jgarrett
 * SWDTV-8271: Removing warning
 * 
 * Hydra_Software_Devel/11   8/22/11 7:03p jgarrett
 * SWDTV-8271: Adding low delay mode support
 * 
 * Hydra_Software_Devel/10   7/8/11 6:38p jgarrett
 * SWDTV-6760: Adding I2sMultiOutput
 * 
 * Hydra_Software_Devel/9   7/8/11 4:23p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/8   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/7   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/6   1/13/11 5:14p gskerl
 * SW7422-146:Added BAPE_MclkRate enum for specifying master clock (MCLK)
 * rate.
 * 
 * Hydra_Software_Devel/5   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 * Hydra_Software_Devel/4   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/3   12/14/10 5:21p jgarrett
 * SW7422-146: Adding multichannel format
 * 
 * Hydra_Software_Devel/2   12/14/10 2:39p jgarrett
 * SW7422-146: Renaming MixerOutput to OutputPort
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_TYPES_H_
#define BAPE_TYPES_H_

/***************************************************************************
Summary:
Device Handle
***************************************************************************/
typedef struct BAPE_Device *BAPE_Handle;

/***************************************************************************
Summary:
Mixer Input Representation
***************************************************************************/
typedef struct BAPE_PathConnector *BAPE_Connector;

/***************************************************************************
Summary:
Output Port Representation
***************************************************************************/
typedef struct BAPE_OutputPortObject *BAPE_OutputPort;

/***************************************************************************
Summary:
Input Port Representation
***************************************************************************/
typedef struct BAPE_InputPortObject *BAPE_InputPort;

/***************************************************************************
Summary:
Channel Modes
***************************************************************************/
typedef enum BAPE_ChannelMode
{
    BAPE_ChannelMode_e1_0,
    BAPE_ChannelMode_e1_1,
    BAPE_ChannelMode_e2_0,
    BAPE_ChannelMode_e3_0,
    BAPE_ChannelMode_e2_1,
    BAPE_ChannelMode_e3_1,
    BAPE_ChannelMode_e2_2,
    BAPE_ChannelMode_e3_2,
    BAPE_ChannelMode_e3_4,
    BAPE_ChannelMode_eMax
} BAPE_ChannelMode;

/***************************************************************************
Summary:
Channels
***************************************************************************/
typedef enum BAPE_Channel
{
    BAPE_Channel_eLeft,
    BAPE_Channel_eRight,
    BAPE_Channel_eLeftSurround,
    BAPE_Channel_eRightSurround,
    BAPE_Channel_eCenter,
    BAPE_Channel_eLfe,
    BAPE_Channel_eLeftRear,
    BAPE_Channel_eRightRear,
    BAPE_Channel_eMax
} BAPE_Channel;

/***************************************************************************
Summary:
Channel Pairs
***************************************************************************/
typedef enum BAPE_ChannelPair
{
    BAPE_ChannelPair_eLeftRight,
    BAPE_ChannelPair_eLeftRightSurround,
    BAPE_ChannelPair_eCenterLfe,
    BAPE_ChannelPair_eLeftRightRear,
    BAPE_ChannelPair_eMax
} BAPE_ChannelPair;

/***************************************************************************
Summary:
Stereo Mode
***************************************************************************/
typedef enum BAPE_StereoMode
{
    BAPE_StereoMode_eLeftRight,
    BAPE_StereoMode_eLeftLeft,
    BAPE_StereoMode_eRightRight,
    BAPE_StereoMode_eRightLeft
} BAPE_StereoMode;

/***************************************************************************
Summary:
Dual Mono Modes
***************************************************************************/
typedef enum BAPE_DualMonoMode
{
    BAPE_DualMonoMode_eStereo,
    BAPE_DualMonoMode_eLeft,
    BAPE_DualMonoMode_eRight,
    BAPE_DualMonoMode_eMix,
    BAPE_DualMonoMode_eMax
} BAPE_DualMonoMode;

/***************************************************************************
Summary:
Mono Channel Select
***************************************************************************/
typedef enum BAPE_MonoChannelMode
{
    BAPE_MonoChannelMode_eLeft,
    BAPE_MonoChannelMode_eRight,
    BAPE_MonoChannelMode_eMix,
    BAPE_MonoChannelMode_eMax
} BAPE_MonoChannelMode;

/***************************************************************************
Summary:
SPDIF Burst Types
***************************************************************************/
typedef enum BAPE_SpdifBurstType
{
    BAPE_SpdifBurstType_eNull,
    BAPE_SpdifBurstType_ePause,
    BAPE_SpdifBurstType_eNone,
    BAPE_SpdifBurstType_eMax
} BAPE_SpdifBurstType;

/***************************************************************************
Summary:
SPDIF Output Formatter Settings
***************************************************************************/
typedef struct BAPE_SpdifChannelStatus
{
    bool separateLeftRight;     /* Use different channel number bits for left and right channels (Bits 20..21) */
    bool professional;          /* Bit 0 */
    bool copyright;             /* Bit 2 */
    uint8_t formatInformation;  /* Bits 3..5 */
    uint8_t categoryCode;       /* Bits 8..15 */
    uint8_t sourceNumber;       /* Bits 16..19 */
    uint8_t clockAccuracy;      /* bits 28..29 */
    uint8_t cgmsA;              /* bits 40..41 */
} BAPE_SpdifChannelStatus;

/***************************************************************************
Summary:
SCLK (Bit Clock) Rate
***************************************************************************/
typedef enum BAPE_SclkRate
{
    BAPE_SclkRate_e64Fs,
    BAPE_SclkRate_e128Fs,
    BAPE_SclkRate_eMax
} BAPE_SclkRate;

/***************************************************************************
Summary:
MCLK (Master Clock) Rate
***************************************************************************/
typedef enum BAPE_MclkRate
{
    BAPE_MclkRate_eAuto,
    BAPE_MclkRate_e128Fs,
    BAPE_MclkRate_e256Fs,
    BAPE_MclkRate_e384Fs,
    BAPE_MclkRate_e512Fs,
    BAPE_MclkRate_eMax
} BAPE_MclkRate;

/***************************************************************************
Summary:
PLL Indexes
***************************************************************************/
typedef enum BAPE_Pll
{
    BAPE_Pll_e0,
    BAPE_Pll_e1,
    BAPE_Pll_e2,
    BAPE_Pll_eMax
} BAPE_Pll;

/***************************************************************************
Summary:
PLL Indexes
***************************************************************************/
typedef enum BAPE_Nco
{
    BAPE_Nco_e0,
    BAPE_Nco_e1,
    BAPE_Nco_e2,
    BAPE_Nco_e3,
    BAPE_Nco_e4,
    BAPE_Nco_eMax
} BAPE_Nco;

/***************************************************************************
Summary:
I2S Data Justification
***************************************************************************/
typedef enum BAPE_I2sJustification
{
    BAPE_I2sJustification_eLsbFirst,    /* LSB is at the LRCK transition */
    BAPE_I2sJustification_eMsbFirst,    /* MSB is at the LRCK transition */
    BAPE_I2sJustification_eMax
} BAPE_I2sJustification;

/***************************************************************************
Summary:
I2S Data Alignment
***************************************************************************/
typedef enum BAPE_I2sDataAlignment
{
    BAPE_I2sDataAlignment_eAligned,     /* Data is aligned to LRCK */
    BAPE_I2sDataAlignment_eDelayed,     /* Data is delayed one SCLK from LRCK */
    BAPE_I2sDataAlignment_eMax
} BAPE_I2sDataAlignment;

/***************************************************************************
Summary:
I2S LRCK Polarity
***************************************************************************/
typedef enum BAPE_I2sLRClockPolarity
{
    BAPE_I2sLRClockPolarity_eLeftHigh,  /* Left is high on LRCK */
    BAPE_I2sLRClockPolarity_eLeftLow,   /* Left is low on LRCK */
    BAPE_I2sLRClockPolarity_eMax
} BAPE_I2sLRClockPolarity;

/***************************************************************************
Summary:
I2S SCLK Polarity
***************************************************************************/
typedef enum BAPE_I2sSclkPolarity
{
    BAPE_I2sSclkPolarity_eRising,   /* Rising edge aligned with SDATA */
    BAPE_I2sSclkPolarity_eFalling,   /* Falling edge aligned with SDATA */
    BAPE_I2sSclkPolarity_eMax
} BAPE_I2sSclkPolarity;

/***************************************************************************
Summary:
Multichannel I2S Mode
***************************************************************************/
typedef enum BAPE_I2sMultiMode
{
    BAPE_I2sMultiMode_eMultichannel,    /* Default.  I2S multi will be used 
                                           as a single output capable of stereo
                                           or multichannel. */
    BAPE_I2sMultiMode_eStereo,          /* I2S multi will be used as a set of
                                           discrete stereo outputs that share
                                           a common sample clock and word select. */
    BAPE_I2sMultiMode_eMax
} BAPE_I2sMultiMode;

/***************************************************************************
Summary:
Normal volume (0dB)
***************************************************************************/
#define BAPE_VOLUME_NORMAL (0x800000)

/***************************************************************************
Summary:
Minimum volume (mute)
***************************************************************************/
#define BAPE_VOLUME_MIN    (0)

/***************************************************************************
Summary:
Buffer Descriptor
***************************************************************************/
typedef struct BAPE_BufferDescriptor
{
    bool interleaved;               /* If true, every other channel will have valid pointers,
                                       e.g. L for L/R, Ls for Ls/Rs, etc.  */
    unsigned numBuffers;            /* Number of buffers.  For mono/interleaved stereo this is 1.  For
                                       non-interleaved stereo, it's 2.  For non-interleaved 7.1 it's 8. */
    struct
    {
        void *pBuffer;              /* Buffer base address prior to wraparound */
        void *pWrapBuffer;          /* Buffer address after wraparound (NULL if no wrap has occurred) */
    } buffers[BAPE_Channel_eMax];

    unsigned bufferSize;            /* Buffer size before wraparound in bytes */
    unsigned wrapBufferSize;        /* Buffer size after wraparound in bytes */        
} BAPE_BufferDescriptor;

/***************************************************************************
Summary:
Multichannel Formats
***************************************************************************/
typedef enum BAPE_MultichannelFormat
{
    BAPE_MultichannelFormat_e2_0,
    BAPE_MultichannelFormat_e5_1,
    BAPE_MultichannelFormat_e7_1,
    BAPE_MultichannelFormat_eMax
} BAPE_MultichannelFormat;

/***************************************************************************
Summary:
Data Path Types 
 
Description: 
Some object types expose multiple data path connectors.  This will specify 
the data type of the connector. 
***************************************************************************/
typedef enum BAPE_ConnectorFormat
{
    BAPE_ConnectorFormat_eStereo,           /* Stereo PCM */
    BAPE_ConnectorFormat_eMultichannel,     /* Multichannel PCM */
    BAPE_ConnectorFormat_eCompressed,       /* Compressed IEC-61937 audio up to 48 kHz for SPDIF/HDMI applications.  */
    BAPE_ConnectorFormat_eCompressed4x,     /* Compressed IEC-61937 audio for HDMI up to 192kHz.  Typically used for AC3+ and DTS-HD HRA audio formats. */
    BAPE_ConnectorFormat_eCompressed16x,    /* Compressed IEC-61937 audio for HDMI HBR packets up to 768kHz.  Typically used for MAT/MLP (Dolby TrueHD) 
                                               and DTS-HD MA audio formats.  Available in HDMI 1.3 and later. */
    BAPE_ConnectorFormat_eMono,             /* Mono PCM, typically only used for voice conferencing applications. */
    BAPE_ConnectorFormat_eMax
} BAPE_ConnectorFormat;

/***************************************************************************
Summary:
DSP Delay Mode
 
Description: 
Determines the DSP's delay mode for an operation.  Default uses a
fixed path delay, but low delay mode provides lower delay depending on the
codec in use.  There are also usage restrictions for low delay mode in
terms of what post-processing can be performed as well as the number of
concurrent DSP tasks in use.
***************************************************************************/
typedef enum BAPE_DspDelayMode
{
    BAPE_DspDelayMode_eDefault, /* Fixed path delay */
    BAPE_DspDelayMode_eLow,     /* Variable low path delay depending on codec */
    BAPE_DspDelayMode_eMax
} BAPE_DspDelayMode;

/***************************************************************************
Summary:
RF Audio Encodings
**************************************************************************/
typedef enum BAPE_RfAudioEncoding
{
    BAPE_RfAudioEncoding_eBtsc,
    BAPE_RfAudioEncoding_eMax
} BAPE_RfAudioEncoding;

/***************************************************************************
Summary:
Loudness Equivalence Modes
**************************************************************************/
typedef enum BAPE_LoudnessEquivalenceMode
{
    BAPE_LoudnessEquivalenceMode_eNone,         /* Default, no loudness equivalence */
    BAPE_LoudnessEquivalenceMode_eAtscA85,      /* ATSC A/85.  This standardizes all decoders to output 
                                                   Stereo and Multichannel PCM at -20dB.  Compressed data is output at -31dB. 
                                                   The application must set the volume for outputs accordingly so
                                                   that PCM stereo is output to "active" outputs such as DAC at -23 dB 
                                                   and PCM is sent to "passive" outputs such as SPDIF at -31dB.  This
                                                   can be done by calling BAPE_SetOutputVolume() for active outputs
                                                   with the value 0x5A9DF7 (-3dB) and passive outputs with the
                                                   value 0x241346 (-11dB). */
    BAPE_LoudnessEquivalenceMode_eEbuR128,      /* EBU-R128.  This standardizes Dolby decoders to output 
                                                   Stereo and Multichannel PCM at -20dB.  Non-Dolby decoders will
                                                   output Stereo and Multichannel PCM at -23dB.  All encoders and 
                                                   passthrough configurations will output compressed at -31dB. 
                                                   The application must set the volume for outputs accordingly so
                                                   that PCM stereo is output to "active" outputs such as DAC at -23 dB 
                                                   and PCM is sent to "passive" outputs such as SPDIF at -31dB.  This
                                                   can be done by calling BAPE_SetOutputVolume() for active outputs
                                                   with the value 0x5A9DF7 (-3dB) and passive outputs with the
                                                   value 0x241346 (-11dB) for Dolby codecs.  For non-Dolby codecs, use 
                                                   the value 0x800000 (-0dB) for active outputs and 0x32F52C (-8dB) for
                                                   passive outputs.  */
    BAPE_LoudnessEquivalenceMode_eMax
} BAPE_LoudnessEquivalenceMode;

#endif /* #ifndef BAPE_TYPES_H_ */

