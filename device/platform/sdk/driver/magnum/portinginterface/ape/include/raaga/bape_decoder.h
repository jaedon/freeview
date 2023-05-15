/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_decoder.h $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-5116-12.4/2 $
 * $brcm_Date: 12/12/13 4:04p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/include/raaga/bape_decoder.h $
 * 
 * Hydra_Software_Devel/SW7425-5116-12.4/2   12/12/13 4:04p mpeteri
 * SW7425-5116: pick up changes to comments
 * 
 * Hydra_Software_Devel/SW7425-5116/2   12/12/13 3:19p mpeteri
 * SW7425-5116: Update comments
 * 
 * Hydra_Software_Devel/SW7425-5116-12.4/1   12/6/13 11:57a mpeteri
 * SW7425-5116: back-port changes to 12.4
 * 
 * Hydra_Software_Devel/SW7425-5116/1   12/6/13 11:27a mpeteri
 * SW7425-5116: changes to allow 96k output rate from the decoder
 * 
 * Hydra_Software_Devel/22   10/8/12 2:02a jgarrett
 * SW7346-914: Adding enable for ancillary data
 * 
 * Hydra_Software_Devel/21   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-914/1   9/28/12 12:05p jgarrett
 * SW7346-914: Initial ancillary data implementation
 * 
 * Hydra_Software_Devel/20   7/20/12 4:43p jgarrett
 * SW7425-3490: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3490/1   7/13/12 5:41p jgarrett
 * SW7425-3490: Adding initial frame preservation for MS-11 certification
 * 
 * Hydra_Software_Devel/19   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/27/12 6:35p gskerl
 * SW7425-2075: Added halted and running fields to BAPE_DecoderStatus
 * structure.
 * 
 * Hydra_Software_Devel/18   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/17   2/17/12 4:13p jgarrett
 * SW7425-2262: Adding API to enter Non-Realtime Gap Fill mode
 * 
 * Hydra_Software_Devel/16   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/15   12/19/11 4:27p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * Hydra_Software_Devel/14   11/21/11 11:44a jgarrett
 * SW7425-1756: Adding overflow and status ready interrupts
 * 
 * Hydra_Software_Devel/13   11/16/11 5:15p jgarrett
 * SW7425-1519: Adding external PCM mixing option
 * 
 * Hydra_Software_Devel/12   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/11   8/22/11 7:03p jgarrett
 * SWDTV-8271: Adding low delay mode support
 * 
 * Hydra_Software_Devel/10   6/21/11 3:50p jgarrett
 * SW7425-654: Adding NRT APIs to decoder and mux
 * 
 * Hydra_Software_Devel/9   3/11/11 6:02p jgarrett
 * SW7422-146: Decoder supports external inputs
 * 
 * Hydra_Software_Devel/8   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/7   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/6   2/15/11 4:07p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/5   2/7/11 11:30a jgarrett
 * SW7422-146: Implementing DDP -> AC3 conversion and status for
 * MPEG/AAC/AC3
 * 
 * Hydra_Software_Devel/4   1/19/11 6:13p jgarrett
 * SW7422-146: Enabling TSM
 * 
 * Hydra_Software_Devel/3   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 * Hydra_Software_Devel/2   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BAPE_DECODER_H_
#define BAPE_DECODER_H_

#include "bavc.h"
#include "bape_types.h"
#include "bape_input.h"
#include "bape_codec_types.h"

/***************************************************************************
Summary:
Decoder Handle
***************************************************************************/
typedef struct BAPE_Decoder *BAPE_DecoderHandle;

/***************************************************************************
Summary:
Decoder Open settings
***************************************************************************/
typedef struct BAPE_DecoderOpenSettings
{
    unsigned dspIndex;              /* Index of the DSP you would like to use.  Default = 0. */
    size_t ancillaryDataFifoSize;   /* Ancillary data FIFO size in bytes */
} BAPE_DecoderOpenSettings;

/***************************************************************************
Summary:
Get default open settings for an audio decoder
***************************************************************************/
void BAPE_Decoder_GetDefaultOpenSettings(
    BAPE_DecoderOpenSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Open an audio decoder
***************************************************************************/
BERR_Code BAPE_Decoder_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_DecoderOpenSettings *pSettings, 
    BAPE_DecoderHandle *pHandle                 /* [out] */
    );

/***************************************************************************
Summary:
Close an audio decoder
***************************************************************************/
void BAPE_Decoder_Close(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Decoder Mixing Modes
***************************************************************************/
typedef enum BAPE_DecoderMixingMode
{
    BAPE_DecoderMixingMode_eDescription,  /* In this mixing mode, channels will be mixed using
                                             volume coefficients contained in the secondary audio
                                             program.  This is typically used in audio services for
                                             the visually impaired (as described in DTG D-Book section 4.5),
                                             where a voice over track can be mixed with the primary 
                                             audio track. */
    BAPE_DecoderMixingMode_eSoundEffects, /* In this mixing mode, channels will be mixed using static
                                             coefficients in the mixer as opposed to stream-based 
                                             coefficients. */
    BAPE_DecoderMixingMode_eMax
} BAPE_DecoderMixingMode;

/***************************************************************************
Summary:
Start-time settings for an audio decoder
***************************************************************************/
typedef struct BAPE_DecoderStartSettings
{
    BAVC_AudioCompressionStd codec;
    BAVC_StreamType streamType;         /* Required for TSM computation */

    unsigned stcIndex;                  /* Which STC index should be used */

    const BAVC_XptContextMap *pContextMap;  /* What RAVE context should be read while decoding.  
                                               If decoding from an input port, pass NULL for this 
                                               setting */
    BAPE_InputPort inputPort;               /* What input port to read data from.  If decoding from
                                               RAVE, pass NULL for this setting */

    bool targetSyncEnabled;             /* If true, normal frame sync operation will be used (default).  This flag can be set to false
                                           for certification applications that require the last frame of the input buffer to be consumed
                                           without the audio decoder finding the start of the successive frame.  */

    bool ppmCorrection;                 /* If true, PPM correction will be performed for 2ms lipsync 
                                           precision on PCM outputs.  Not available with any compressed
                                           or multichannel data outputs into hardware. */

    bool decodeRateControl;             /* If true, decoder rate control is enabled for trick mode
                                           operations.  Not availble with ppmCorrection or if any
                                           compressed or multichannel data outputs into hardware. */

    bool nonRealTime;                   /* Normal operation for decoding is real time, if this is set to 'true' then decoding is used as a
                                           source for non-realtime transcode operations */
                                           
    bool forceCompleteFirstFrame;       /* If true, the first frame will always be entirely rendered to the output and not partially truncated
                                           for TSM computations.  This should be disabled for normal operation, but may be required for some
                                           bit-exact certification testing that requires all data to be rendered even with TSM enabled. */
                                           
    BAPE_DspDelayMode delayMode;        /* DSP Delay mode for this task.  Default uses a fixed path delay irrespective of source codec.
                                           Low delay mode provides lower delay depending on the input codec, but has restrictions on
                                           running only a single DSP task at a time as well as limits on the post-processing that can
                                           be performed */       
                                        
    BAPE_DecoderMixingMode mixingMode;  /* Mixing mode to be used when multiple decoders are mixed */
    unsigned maxOutputRate;             /* Max output rate we support from the decoder - 
                                           Valid values are 48000 or 96000. Units are Hz. Default is 48000.
                                           Content above the maxOutputRate will be downsampled within its rate family.
                                           For example - if the content is 88200Hz and maxOutputRate = 48000, decoder
                                           will output 44100Hz for that content.  Similarly, if the content is 
                                           176,400Hz and maxOutputRate = 96000, decoder will output 88200Hz. */
} BAPE_DecoderStartSettings;

/***************************************************************************
Summary:
Get default start settings for an audio decoder
***************************************************************************/
void BAPE_Decoder_GetDefaultStartSettings(
    BAPE_DecoderStartSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Start an audio decoder
***************************************************************************/
BERR_Code BAPE_Decoder_Start(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop an audio decoder
***************************************************************************/
void BAPE_Decoder_Stop(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Pause an audio decoder
***************************************************************************/
BERR_Code BAPE_Decoder_Pause(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Resume an audio decoder from a paused state
***************************************************************************/
BERR_Code BAPE_Decoder_Resume(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Advance an audio decoder in ms units.  Must be paused first.
***************************************************************************/
BERR_Code BAPE_Decoder_Advance(
    BAPE_DecoderHandle handle,
    unsigned milliseconds           /* Milliseconds to advance */
    );

/***************************************************************************
Summary:
Prepare the decoder to flush (called prior to flushing RAVE context) 
***************************************************************************/
BERR_Code BAPE_Decoder_DisableForFlush(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Flush the decoder's buffers (called after flushing RAVE context) 
***************************************************************************/
BERR_Code BAPE_Decoder_Flush(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Audio Decoder TSM Settings
***************************************************************************/
typedef struct BAPE_DecoderTsmSettings
{
    bool tsmEnabled;
    bool astmEnabled;
    bool playback;
    uint32_t ptsOffset;     /* PTS offset value in 45 kHz ticks.  Will be internally converted to
                               27MHz if required. */
    uint32_t stcOffset;     /* SW STC Offset in 45kHz ticks.  Will be internaly converted to 
                               27 MHz if required.*/
    /* All thresholds below are programmed in milliseconds. */
    struct 
    {
        unsigned discard;           /* Threshold (ms) beyond which frames will be discarded */
        unsigned grossAdjustment;   /* Threshold (ms) beyond which gross adjustments will be performed */
        unsigned smoothTrack;       /* Threshold (ms) beyond which smooth tracking (ppm) adjustments will be performed */
        unsigned syncLimit;         /* Sets the sync limit for audio master mode (in ms).  Set to 0 to disable audio master mode */
    } thresholds;
} BAPE_DecoderTsmSettings;

/***************************************************************************
Summary:
Get Audio Decoder TSM Settings
***************************************************************************/
void BAPE_Decoder_GetTsmSettings(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmSettings *pSettings  /* [out] */
    );

/***************************************************************************
Summary:
Get Audio Decoder TSM Settings in isr context
***************************************************************************/
void BAPE_Decoder_GetTsmSettings_isr(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmSettings *pSettings  /* [out] */
    );

/***************************************************************************
Summary:
Set Audio Decoder TSM Settings
***************************************************************************/
BERR_Code BAPE_Decoder_SetTsmSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderTsmSettings *pSettings
    );

/***************************************************************************
Summary:
Set Audio Decoder TSM Settings in isr context
***************************************************************************/
BERR_Code BAPE_Decoder_SetTsmSettings_isr(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderTsmSettings *pSettings
    );

/***************************************************************************
Summary:
Indicate a playback STC is valid to the decoder in isr context
***************************************************************************/
BERR_Code BAPE_Decoder_SetStcValid_isr(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Audio Decoder TSM Status
***************************************************************************/
typedef struct BAPE_DecoderTsmStatus
{
    BAVC_PTSInfo ptsInfo;
    int32_t ptsStcDifference;   /* PTS - STC value */
    unsigned lastFrameLength;   /* In ms */
} BAPE_DecoderTsmStatus;

/***************************************************************************
Summary:
Get Audio Decoder TSM Status
***************************************************************************/
BERR_Code BAPE_Decoder_GetTsmStatus(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Get Audio Decoder TSM Status in isr context
***************************************************************************/
BERR_Code BAPE_Decoder_GetTsmStatus_isr(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmStatus *pStatus  /* [out] */
    );

/***************************************************************************
Summary:
Normal Playback Rate
***************************************************************************/
#define BAPE_NORMAL_DECODE_RATE (100)

/***************************************************************************
Summary:
Audio Decoder Settings that are changeable on the fly
***************************************************************************/
typedef struct BAPE_DecoderSettings
{
    BAPE_MultichannelFormat multichannelFormat;     /* Controls whether the decoder outputs 2.0, 5.1, or 7.1 data on the multichannel path.
                                                       This is not changeable on the fly. */
    BAPE_ChannelMode outputMode;                    /* Can not be set to a wider value than multichannelFormat (e.g. 3_4 is not possible 
                                                       when multichannel format is 5.1 */
    bool outputLfe;

    bool loudnessEquivalenceEnabled;                /* If true, loudness equivalence will be enabled based on the mode specified
                                                       in BAPE_Settings.loudnessMode.  Ignored if BAPE_Settings.loudnessMode is
                                                       set to BAPE_LoudnessEquivalenceMode_eNone.  Default is true. */

    bool ancillaryDataEnabled;                      /* If true, ancillary data parsing will be enabled.  Default is false. */

    BAPE_DualMonoMode dualMonoMode;

    unsigned decodeRate;                /* Playback Rate for trick modes.  Use BAPE_NORMAL_DECODE_RATE
                                           for normal operation.  Ranges from BAPE_NORMAL_DECODE_RATE/2
                                           to BAPE_NORMAL_DECODE_RATE*2.  ppmCorrection is not available
                                           if the rate is non-standard.  This is ignored unless
                                           BAPE_DecoderStartSettings.decodeRateControl is true */
} BAPE_DecoderSettings;

/***************************************************************************
Summary:
Get Audio Decoder Settings
***************************************************************************/
void BAPE_Decoder_GetSettings(
    BAPE_DecoderHandle handle,
    BAPE_DecoderSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Set Audio Decoder Settings
***************************************************************************/
BERR_Code BAPE_Decoder_SetSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderSettings *pSettings
    );

/***************************************************************************
Summary:
Audio Decoder Status
***************************************************************************/
typedef struct BAPE_DecoderStatus
{
    BAVC_AudioCompressionStd codec;
    BAPE_DecoderTsmStatus tsmStatus;
    unsigned sampleRate;
    unsigned framesDecoded;
    unsigned frameErrors;
    unsigned dummyFrames;
    bool running;
    bool halted;
    union
    {
        BAPE_MpegStatus   mpeg;
        BAPE_Ac3Status    ac3;
        BAPE_AacStatus    aac;
        BAPE_WmaStatus    wma;
        BAPE_WmaProStatus wmaPro;
        BAPE_DtsStatus    dts;
        BAPE_PcmWavStatus pcmWav;
        BAPE_AmrStatus    amr;
        BAPE_DraStatus    dra;
        BAPE_CookStatus   cook;
    } codecStatus;
} BAPE_DecoderStatus;

/***************************************************************************
Summary:
Get Audio Decoder Status
***************************************************************************/
void BAPE_Decoder_GetStatus(
    BAPE_DecoderHandle handle,
    BAPE_DecoderStatus *pStatus     /* [out] */
    );

/***************************************************************************
Summary:
Audio Decoder Codec-Specific Settings
***************************************************************************/
typedef struct BAPE_DecoderCodecSettings
{
    BAVC_AudioCompressionStd codec;
    union
    {
        BAPE_Ac3Settings    ac3;
        BAPE_Ac3Settings    ac3Plus;
        BAPE_AacSettings    aac;       /* Applies to both ADTS/LOAS */
        BAPE_AacSettings    aacPlus;   /* Applies to both ADTS/LOAS */
        BAPE_WmaProSettings wmaPro;
        BAPE_DtsSettings    dts;       /* Applies to DTS, DTS-HD, DTS-Legacy */
        BAPE_AdpcmSettings  adpcm;
    } codecSettings;
} BAPE_DecoderCodecSettings;

/***************************************************************************
Summary:
Get Audio Decoder Codec-Specific Settings
***************************************************************************/
void BAPE_Decoder_GetCodecSettings(
    BAPE_DecoderHandle handle,
    BAVC_AudioCompressionStd codec,
    BAPE_DecoderCodecSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Set Audio Decoder Codec-Specific Settings
***************************************************************************/
BERR_Code BAPE_Decoder_SetCodecSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderCodecSettings *pSettings
    );

/***************************************************************************
Summary:
Get connector for decoder output data
***************************************************************************/
void BAPE_Decoder_GetConnector(
    BAPE_DecoderHandle handle,
    BAPE_ConnectorFormat format,
    BAPE_Connector *pConnector /* [out] */
    );

/***************************************************************************
Summary:
Decoder Interrupt Handlers
***************************************************************************/
typedef struct BAPE_DecoderInterruptHandlers
{
    /* Interrupt fires when first PTS is received */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;
    } firstPts;
    /* Interrupt fires when TSM Fail (PTS Error) occurs */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;    
    } tsmFail;
    /* Interrupt fires when TSM transitions from fail -> pass in ASTM mode */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus);
        void *pParam1;
        int param2;
    } tsmPass;
    /* Interrupt fires when the decoder receives the first or any new sample rate in the stream */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2, unsigned sampleRate);
        void *pParam1;
        int param2;
    } sampleRateChange;
    /* Interrupt fires when the decoder achieves frame lock */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } lock;
    /* Interrupt fires when the decoder loses frame lock */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } unlock;
    /* Interrupt fires when the decoder status is ready for the host to read.
       Typically, this occurs after the first frame is successfully decoded. */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } statusReady;
    /* Interrupt fires with stream channel mode (acmod) changes.
       Call BAPE_Decoder_GetStatus() from task context to determine
       latest status. */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } modeChange;
    /* Interrupt fires with stream bitrate changes. Call BAPE_Decoder_GetStatus()
       from task context to determine latest status. */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } bitrateChange;
    /* This interrupt fires when the CDB and/or ITB overflow */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } cdbItbOverflow;
    /* This interrupt fires when the active input has changed to an incompatible format and been halted.
       The application must call BAPE_Decoder_Stop() and BAPE_Decoder_Start() to resume processing. */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } inputHalted;
    /* This interrupt fires when ancillary data has been added to the FIFO.  The application
       should call BAPE_Decoder_GetAncillaryDataBuffer() and BAPE_Decoder_ConsumeAncillaryData()
       to retrieve the data */
    struct
    {
        void (*pCallback_isr)(void *pParam1, int param2);
        void *pParam1;
        int param2;
    } ancillaryData;
} BAPE_DecoderInterruptHandlers;

/***************************************************************************
Summary:
Get Currently Registered Interrupt Handlers
***************************************************************************/
void BAPE_Decoder_GetInterruptHandlers(
    BAPE_DecoderHandle handle,
    BAPE_DecoderInterruptHandlers *pInterrupts     /* [out] */
    );

/***************************************************************************
Summary:
Set Interrupt Handlers 
 
Description: 
To disable any unwanted interrupt, pass NULL for its callback routine
***************************************************************************/
BERR_Code BAPE_Decoder_SetInterruptHandlers(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderInterruptHandlers *pInterrupts
    );

/***************************************************************************
Summary:
Get Default CDB/ITB configuration for decoding
***************************************************************************/
void BAPE_Decoder_GetDefaultCdbItbConfig(
    BAPE_DecoderHandle handle,
    BAVC_CdbItbConfig *pConfig  /* [out] */
    );

/***************************************************************************
Summary:
Treat input underflows as gaps in non-realtime mode. 
 
Description: 
In non-realtime mode, underflow conditions are not typically treated as a 
gap in data and the decoder will stall waiting for more data.  This can 
lead to deadlock scenarios if there is an actual gap in the data.  When 
this routine is called, the decoder will enter a gap-fill mode and will 
treat input underflows as missing frames until valid input data is received. 
 
This routine can only be called if the decoder is started and running 
in non-realtime mode. 
***************************************************************************/
BERR_Code BAPE_Decoder_EnterUnderflowMode(
    BAPE_DecoderHandle handle
    );

/***************************************************************************
Summary:
Ancillary Data Header
***************************************************************************/
typedef struct BAPE_AncillaryDataHeader
{
    unsigned blockSize;         /* Block size including this header and any padding */
    unsigned payloadSize;       /* Payload size in bytes */
    unsigned payloadSizeBits;   /* Payload size in bits */
    unsigned frameNumber;       /* Frame number */
} BAPE_AncillaryDataHeader;

/***************************************************************************
Summary:
Get Decoder Ancillary Data
***************************************************************************/
BERR_Code BAPE_Decoder_GetAncillaryDataBuffer(
    BAPE_DecoderHandle hDecoder,
    void **pBuffer, /* [out] pointer to ancillary data buffer */
    size_t *pSize   /* [out] size of data buffer in bytes */
    );

/***************************************************************************
Summary:
Consume Decoder Ancillary Data
***************************************************************************/
BERR_Code BAPE_Decoder_ConsumeAncillaryData(
    BAPE_DecoderHandle hDecoder,
    size_t numBytes
    );

/***************************************************************************
Summary:
Flush Decoder Ancillary Data
***************************************************************************/
void BAPE_Decoder_FlushAncillaryData(
    BAPE_DecoderHandle hDecoder
    );

#endif

